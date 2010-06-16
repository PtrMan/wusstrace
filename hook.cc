// Copyright notice
// ================
//
// Copyright (C) 2010
//     Lorenzo Martignoni <martignlo@gmail.com>
//     Roberto Paleari <roberto.paleari@gmail.com>
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#define WINVER 0x501

#include <windows.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>

#include "bitset.hh"
#include "config.hh"
#include "debug.hh"
#include "hook.hh"
#include "common.hh"
#include "syscall_tracer.hh"

using namespace std;

// ****************************************************************************

extern Options options;
extern SyscallTracer *tracer;

bool syscall_hooking = true;

bitset syscall_hooked[BITSET_SIZE/BITSET_BITS] = {0};

static map<HANDLE_WST, HANDLE_WST> child_processes;
static map<HANDLE_WST, HANDLE_WST> remote_threads;
set<uint32_t> traced_threads;

uint32_t prehook(uint32_t tid, SyscallNo sysno, void *args, Syscall **syscall);
uint32_t posthook(uint32_t tid, Syscall *syscall, uint32_t retval);

#define DO_INT_SYSCALL(eax, ret)		\
  asm __volatile__ ("movl %1, %%eax;"		\
		    "lea 0xc(%%ebp), %%edx;"	\
		    "int $0x2e;"		\
		    "movl %%eax, %0;"		\
		    : "=r" (ret)		\
		    : "m" (eax)			\
		    : "edx"			\
		    )

// XXX: check me
// original:
// MOV EDX, ESP
// SYSENTER
#define DO_SYSENTER_SYSCALL(eax, esp, retval)		\
  asm __volatile__ ("jmp l0%2;"				\
		    "l1%2:"				\
		    "popl %%edx;"			\
		    "movl %1, %%eax;"			\
		    "movl %%esp, %%edi;"		\
		    "lea 0x4(%%ebp), %%esp;"		\
		    "movl (%%esp), %%esi;"		\
		    "movl %%edx, (%%esp);"		\
		    "movl %%esp, %%edx;"		\
		    "sysenter;"				\
		    "l0%2:"				\
		    "call l1%2;"			\
		    "movl %%eax, %0;"			\
		    "movl %%esi, -0x4(%%esp);"		\
		    "movl %%edi, %%esp;"		\
		    : "=m" (retval)			\
		    : "m" (eax), "i" (__LINE__)		\
		    : "eax", "ecx", "edx", "edi", "esi"	\
		    )

uint32_t syscall_hook() {
  uint32_t orig_eax, eax;
  uint32_t orig_esp;
  uint32_t tid, retval, retaddr;
  Syscall *syscall;
  bool bNoReturn, bIsHooked;

  // Save system call number, original ESP and current thread ID
  asm __volatile__ ("movl %%eax, %0;"
		    "movl %%ebp, %1;"
		    "push %%ebx;"
		    "movl 0x4(%%ebp), %%ebx;"
		    "movl %%ebx, %3;"
		    "movl %%fs:(0x24), %%ebx;"
		    "movl %%ebx, %2;"
		    "pop %%ebx;"
		    : "=m" (orig_eax), "=m" (orig_esp), "=m" (tid),	\
		      "=m" (retaddr)
		    );

  eax = orig_eax;

  ATOMIC_WITH_RET(bIsHooked, !bitset_test_bit(syscall_hooked, tid));

  if (syscall_hooking && 	 // False if the DLL is detaching
      !isGUISyscall(orig_eax) && // Avoid GUI system calls
      bIsHooked) {		 // Do not trace system calls issued by our 
                                 // own hook
    ATOMIC(bitset_set_bit(syscall_hooked, tid));

    
    // Invoke the pre-syscall hook
    syscall = NULL;
    if (options.bFastSyscall) {
      // XXX: check me
      prehook(tid, (SyscallNo) orig_eax, (void *) (orig_esp + 0x4), &syscall);
    } else {
      prehook(tid, (SyscallNo) orig_eax, (void *) (orig_esp + 0xc), &syscall);
    }

    // Check if this system call could not return
    bNoReturn = (orig_eax == Syscall_NtTerminateProcess || 
		 orig_eax == Syscall_NtTerminateThread || 
		 orig_eax == Syscall_NtContinue);

    if (bNoReturn) {
      debug("[I] no returning syscall %s (0x%x)", 
	    syscall_name((SyscallNo) orig_eax), orig_eax);

      if (syscall) {
	delete syscall;
	syscall = NULL;
      }
    }

    ATOMIC(bitset_clear_bit(syscall_hooked, tid));

    if (options.bFastSyscall) {
      DO_SYSENTER_SYSCALL(orig_eax, orig_esp, retval);
    } else {
      DO_INT_SYSCALL(orig_eax, retval);
    }

    ATOMIC(bitset_set_bit(syscall_hooked, tid));

    // Invoke post-syscall hook
    if (!bNoReturn) {
      posthook(tid, syscall, retval);
    }

    syscall = NULL;

    ATOMIC(bitset_clear_bit(syscall_hooked, tid));

    // check timeout
    if (options.iTimeout > 0) {
      if(time(NULL) > options.iTimeout) {
	debug("TIMEOUT!\n");

	if (!options.bTimeoutSuspend) {
	  // ExitProcess() also detaches DLLs
	  ExitProcess(0);
	} else {
	  // Suspend current thread
	  SuspendThread(GetCurrentThread());
	}
      }
    }
  } else {
    // This system call is synthetic (invoked by our hook), just execute it as
    // is
    if (options.bFastSyscall) {
      DO_SYSENTER_SYSCALL(orig_eax, orig_esp, retval);
    } else {
      DO_INT_SYSCALL(orig_eax, retval);
    }
  }

  return retval;
}

// ****************************************************************************

// Hook before the execution of the syscall. The hook returns 1 if the syscall
// has to be executed locally.
uint32_t prehook(uint32_t tid, SyscallNo sysno, void* args, Syscall **syscall) {
  uint32_t r;

  r = tracer->preSyscall(tid, sysno, args, syscall);

#if 0
  // Follow process creation
  if ((*syscall)->getSysNo() == Syscall_NtResumeThread) {
    HANDLE_WST hThread = (HANDLE_WST) (*syscall)->getArg(1)->getArg();

    debug("[I] resuming thread %.8x", hThread);

    if (options.bFollowChildren) {
      for (map<HANDLE_WST,HANDLE_WST>::iterator it = child_processes.begin();
	   it != child_processes.end(); it++) {
	if ((*it).second == hThread) {
	  // found!
	  HANDLE_WST hProcess = (*it).first;
	  uint32_t dwSavedThreadID = options.dTraceThread;

	  if (options.bFollowRemoteThreads) {
	    options.dTraceThread = myGetThreadId(hThread);
	  }

	  injectDll((HANDLE) hProcess, options.sDllFile, &options, sizeof(options));
	  child_processes.erase(hProcess);
	  options.dTraceThread = dwSavedThreadID;
	  break;
	}
      }
    }

    if (options.bFollowRemoteThreads || options.bFollowInfectedProcesses) {
      if (remote_threads.find(hThread) != remote_threads.end()) {
	uint32_t dwSavedThreadID = options.dTraceThread;

	if (options.bFollowRemoteThreads) {
	  options.dTraceThread = myGetThreadId(hThread);
	}

	injectDll((HANDLE) remote_threads[hThread], options.sDllFile, &options, sizeof(options));
	remote_threads.erase(hThread);
	options.dTraceThread = dwSavedThreadID;
      }
    }
  }
#endif

  return 0;
}

uint32_t posthook(uint32_t tid, Syscall *syscall, uint32_t retval) {
  uint32_t r;

  r = tracer->postSyscall(tid, syscall, retval);
  if (!r) {
    delete syscall;
    return r;
  }

#if 0
  if (!NT_SUCCESS(retval)) {
    // this syscall has failed
    return 0;
  }

  if (syscall->getSysNo() == Syscall_NtCreateThread) {
    HANDLE_WST hThread  = *(PHANDLE_WST) syscall->getArg(1)->getArg();
    PCLIENT_ID_WST cid = (PCLIENT_ID_WST) syscall->getArg(5)->getArg();

    if (options.bFollowRemoteThreads && 
	traced_threads.find(tid) != traced_threads.end())
      traced_threads.insert(cid->UniqueThread);
  }

  if (options.bFollowChildren) {
    if (syscall->getSysNo() == Syscall_NtCreateProcess || 
	syscall->getSysNo() == Syscall_NtCreateProcessEx) {
      HANDLE_WST hProcess = *(PHANDLE_WST) syscall->getArg(1)->getArg();

      // Process hProcess has just been created
      debug("[I] (process creation) ProcessHandle:%.8x", hProcess);

      if (child_processes.find(hProcess) == child_processes.end()) {
	// Add this new process, associating to it an invalid main
	// thread handle
	child_processes[hProcess] = (HANDLE_WST) 0;
      }
    } else if (syscall->getSysNo() == Syscall_NtCreateThread) {
      HANDLE_WST hThread  = *(PHANDLE_WST) syscall->getArg(1)->getArg();
      HANDLE_WST hProcess = (HANDLE_WST) syscall->getArg(4)->getArg();

      debug("[I] (process creation) ProcessHandle:%.8x ThreadHandle:%.8x", hProcess,
	    hThread);

      if (child_processes.find(hProcess) != child_processes.end()) {
	// This is a pending process
	child_processes[hProcess] = hThread;
      }
    }
  }

  if (syscall->getSysNo() == Syscall_NtCreateThread) {
    if (options.bFollowRemoteThreads || options.bFollowInfectedProcesses) {
      HANDLE_WST hThread  = *(PHANDLE_WST) syscall->getArg(1)->getArg();
      HANDLE_WST hProcess = (HANDLE_WST) syscall->getArg(4)->getArg();
      // The thread is created into another process
      if (hProcess != 0xFFFFFFFF && 
	  GetProcessId((HANDLE) hProcess) != GetCurrentProcessId()) {
	// The thread is NOT created into a child process
	if (child_processes.find(hProcess) == child_processes.end()) {
	  remote_threads[hThread] = hProcess;
	}
      }
    }
  }
#endif

  return r;
}
