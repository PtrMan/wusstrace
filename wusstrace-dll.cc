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


#define _WIN32_WINNT 0x501

#include <windows.h>
#include <signal.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

#include "bitset.hh"
#include "config.hh"
#include "hook.hh"
#include "common.hh"
#include "console.hh"
#include "syscall.hh"
#include "logger.hh"

inline BOOL hook(void);
inline BOOL unhook(void);

#define SHARED_DATA           0x7ffe0000 // address of _KUSER_SHARED_DATA structure
#define SD_SYSCALL_OFFSET     0x300	 // offset of Syscall field in
					 // _KUSER_SHARED_DATA structure

#ifdef __cplusplus
extern "C" {
#endif
  BOOL WINAPI __declspec(dllexport) DllMain(HINSTANCE, DWORD, LPVOID);
#ifdef __cplusplus
}
#endif

Options options;
SyscallTracer *tracer = NULL;

extern bool syscall_hooking;
extern set<DWORD> traced_threads;
extern bitset syscall_hooked[];
// extern ThreadLocalStorage tls;

BOOL hook(FARPROC funcaddr, FARPROC hookaddr) {
  // unsigned char newcode[] = "\xe9\x00\x00\x00\x00";
  unsigned char newcode[] = "\x32\xD2\x74\x06\x90\x90\x90\x90\x90\xC3\xE9\x00\x00\x00\x00";
  unsigned char oldcode[sizeof(newcode)];
  DWORD dwOldProtect, dwDelta;

  // Setup hooking code

  // edx is killed by both KiFastSystemCall and KiIntSystemCall
  //
  // the forward jump is used in case we have no room between KiFastSystemCall
  // and KiFastSystemCallRet
  //
  //     0:32 d2                xor    %dl,%dl
  //     2:74 06                je     0xa
  //     4:90                   nop    
  //     5:90                   nop    
  //     6:90                   nop    
  //     7:90                   nop    
  //     8:90                   nop    
  //     9:c3                   ret    
  //     a:e9 00 00 00 00       jmp    0xf

  memcpy(oldcode, (char*) hookaddr, sizeof(oldcode));
  dwDelta = (((unsigned long) hookaddr) - (((unsigned long) funcaddr) + 15)) % 0xffffffff;
  *((unsigned long*) (&newcode[sizeof(newcode) - 5])) = dwDelta;
  
  // Unprotect stub page
  if(!VirtualProtect((void *) funcaddr, sizeof(newcode), PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
    debug("! VirtualProtect() has failed (%d)", GetLastError());
    return FALSE;
  }

  debug("* %.8x; syscall_wrapper(): %.8x; delta: %.8x;", funcaddr, hookaddr, dwDelta);

  // fprintf(stderr, " ");		// XXX If we remove this line, regedit.exe crashes
#if 0
  fprintf(stderr, "* old code:");
  for (size_t i = 0; i < sizeof(newcode) - 1; i++)
    fprintf(stderr, " %.2x", oldcode[i]);
  fprintf(stderr, "\n");
  fprintf(stderr, "* new code:");
  for (size_t i = 0; i < sizeof(newcode) - 1; i++)
    fprintf(stderr, " %.2x", newcode[i]);
  fprintf(stderr, "\n");
#endif

  // Hook
  memcpy((char *) funcaddr, newcode, strlen((char *) newcode));

  // Restore stub page protection attributes
  DWORD dwTmp;
  VirtualProtect((void *) funcaddr, sizeof(newcode), dwOldProtect, &dwTmp);
  
  debug("* All done!");

  return TRUE;
}

BOOL WINAPI __declspec(dllexport) DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  char outfname[128];

  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    FARPROC fpSysenter, fpInt, fpCurrent, fpHandler;
    // Read command line options, sent from multienv-cli.cc
    HANDLE hMap;
    Options *pSharedOptions;

    // Allocate console
    console = Console::instance(true);

    // read command-line options
    hMap = OpenFileMapping(FILE_MAP_READ, FALSE, TEXT("Local\\MultiEnvOptions"));
    if (hMap == NULL) {
      debug("! CreateFileMapping() has failed [error 0x%08x (%u)].",
	    (unsigned int) GetLastError(), (unsigned int) GetLastError());
      return -1;
    }

    pSharedOptions = (Options*) MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, sizeof(Options));
    if (pSharedOptions == NULL) {
      debug("! MapViewOfFile() has failed [error 0x%08x (%u)].",
	    (unsigned int) GetLastError(), (unsigned int) GetLastError());
      return -1;
    }

    CopyMemory(&options, pSharedOptions, sizeof(options));
    UnmapViewOfFile(pSharedOptions);
    CloseHandle(hMap);

    // init some undocumented NT function pointers
    initNtFunctions();

    // get candidate system call entry points
    fpSysenter = GetProcAddress(GetModuleHandle("ntdll.dll"), "KiFastSystemCall");
    if(!fpSysenter) {
      debug("! unable to retrieve KiFastSystemCall's address (%d)", GetLastError());
      break;
    }

    fpInt = GetProcAddress(GetModuleHandle("ntdll.dll"), "KiIntSystemCall");
    if(!fpInt) {
      debug("! unable to retrieve KiIntSystemCall's address (%d)", GetLastError());
      break;
    }

    // Get system call entry point used on current system
    // (this is ok on Windows XP SP2 and Windows 2003)
    fpCurrent = (FARPROC) *(DWORD*)(SHARED_DATA + SD_SYSCALL_OFFSET);

    fpHandler = (FARPROC) syscall_hook;
    print("* System call stub is at %08x, hook is at %08x", fpCurrent, fpHandler);
    if(fpCurrent == fpSysenter) {
      // print("* Using fast syscalls (sysenter)");
      print("KiFastSystemCall is not yet supported sorry!\n");
      assert(0);
      options.bFastSyscall = true;
    } else if(fpCurrent == fpInt) {
      print("* Using legacy syscalls (int 0x2e)");
      options.bFastSyscall = false;
    } else {
      print("! unknown system call stub!!");
      break;
    }

    if (options.bDump) {
      snprintf(outfname, sizeof(outfname) - 1, 
	       "%s%.6u.dump", options.sDumpFile,
	       (unsigned int) GetCurrentProcessId());
      tracer = new XMLLogger(GetCurrentProcessId(), outfname);
      debug("* dumping to %s;\n", outfname);
    }

    if (options.bLock) {
      snprintf(outfname, sizeof(outfname) - 1, 
	       "%s%.6u.lock", options.sLockFile, 
	       (unsigned int) GetCurrentProcessId());
      debug("LOCKFILE: %s", outfname);
      FILE *f = fopen(outfname, "w");
      fclose(f);
    }

    // We don't want to hook the syscalls of this thread
    ATOMIC(bitset_set_bit(syscall_hooked, GetCurrentThreadId()));

    if (options.bFollowRemoteThreads) {
      traced_threads.insert(options.dTraceThread);
    }

    // Hook!
    if(!hook(fpCurrent, fpHandler)) {
      // Hooking has failed
#ifdef DEBUG_MODE
      MessageBox(NULL, "Hooking failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
#endif
    }

    break;

  case DLL_PROCESS_DETACH:
    if (options.bDump) {
      syscall_hooking = false;
      if (tracer)
	delete tracer;
    }

    if (options.bLock) {
      snprintf(outfname, sizeof(outfname) - 1, 
	       "%s%.6u.lock", options.sLockFile, 
	       (unsigned int) GetCurrentProcessId());
      DeleteFile(outfname);
    }
    break;
  }

  return TRUE;
}
