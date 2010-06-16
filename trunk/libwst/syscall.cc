// Copyright notice
// ================
//
// Copyright (C) 2010
//     Lorenzo Martignoni <martignlo@gmail.com>
//     Roberto Paleari <roberto.paleari@gmail.com>
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the License, or (at your option) any later
// version.  
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#define WINVER 0x501

#include <stdio.h>
#include <assert.h>
#include <string>
#include <vector>
#include <map>

#define INVALID_HANDLE_VALUE     ((HANDLE)~0UL)

#include "wstconfig.hh"
#include "syscall.hh"

#define DO_INT_SYSCALL(sysno, ptrs, retval)		\
  asm __volatile__ ("mov %1, %%eax;"			\
		    "mov %2, %%edx;"			\
		    "int $0x2e;"			\
		    "mov %%eax, %0;"			\
		    : "=r" (retval)			\
		    : "m" (sysno), "r" (ptrs)		\
		    : "edx", "eax"			\
		    );

// XXX: check me
#define DO_SYSENTER_SYSCALL(sysno, ptrs, retval)	\
  asm __volatile__ ("mov %1, %%eax;"			\
		    "mov %2, %%edx;"			\
		    "jmp l0;"				\
		    "l1:"				\
		    "pop %%ebx;"			\
		    "mov %%ebx, (%%edx);"		\
		    "sysenter;"				\
		    "l0:"				\
		    "call l1;"				\
		    "mov %%eax, %0;"			\
		    : "=r" (retval)			\
		    : "m" (sysno), "r" (ptrs)		\
		    : "edx", "eax", "ebx"		\
		    );

// Number of arguments for each syscall
static int _syscallargsno[] = {
#define MagicSyscall(s, n, r, a, d1, t1, n1, o1, d2, t2, n2, o2, d3, t3, n3, o3, d4, t4, n4, o4, d5, t5, n5, o5, d6, t6, n6, o6, d7, t7, n7, o7, d8, t8, n8, o8, d9, t9, n9, o9, d10, t10, n10, o10, d11, t11, n11, o11, d12, t12, n12, o12, d13, t13, n13, o13, d14, t14, n14, o14, d15, t15, n15, o15, d16, t16, n16, o16, d17, t17, n17, o17) a,
#include "syscall_winxp.hh"
#undef MagicSyscall
};

// Type of arguments
static SyscallArgType _syscallargstype[][17] = {
#define MagicSyscall(s, n, r, a, d1, t1, n1, o1, d2, t2, n2, o2, d3, t3, n3, o3, d4, t4, n4, o4, d5, t5, n5, o5, d6, t6, n6, o6, d7, t7, n7, o7, d8, t8, n8, o8, d9, t9, n9, o9, d10, t10, n10, o10, d11, t11, n11, o11, d12, t12, n12, o12, d13, t13, n13, o13, d14, t14, n14, o14, d15, t15, n15, o15, d16, t16, n16, o16, d17, t17, n17, o17) {SyscallArgType_ ## t1, SyscallArgType_ ## t2, SyscallArgType_ ## t3, SyscallArgType_ ## t4, SyscallArgType_ ## t5, SyscallArgType_ ## t6, SyscallArgType_ ## t7, SyscallArgType_ ## t8, SyscallArgType_ ## t9, SyscallArgType_ ## t10, SyscallArgType_ ## t11, SyscallArgType_ ## t12, SyscallArgType_ ## t13, SyscallArgType_ ## t14, SyscallArgType_ ## t15, SyscallArgType_ ## t16, SyscallArgType_ ## t17},
#include "syscall_winxp.hh"
#undef MagicSyscall
};

// Name of arguments
static const char * _syscallargsname[][17] = {
#define MagicSyscall(s, n, r, a, d1, t1, n1, o1, d2, t2, n2, o2, d3, t3, n3, o3, d4, t4, n4, o4, d5, t5, n5, o5, d6, t6, n6, o6, d7, t7, n7, o7, d8, t8, n8, o8, d9, t9, n9, o9, d10, t10, n10, o10, d11, t11, n11, o11, d12, t12, n12, o12, d13, t13, n13, o13, d14, t14, n14, o14, d15, t15, n15, o15, d16, t16, n16, o16, d17, t17, n17, o17) {#n1, #n2, #n3, #n4, #n5, #n6, #n7, #n8, #n9, #n10, #n11, #n12, #n13, #n14, #n15, #n16, #n17},
#include "syscall_winxp.hh"
#undef MagicSyscall
};

// Type of arguments
static SyscallArgType _syscallretval[] = {
#define MagicSyscall(s, n, r, a, d1, t1, n1, o1, d2, t2, n2, o2, d3, t3, n3, o3, d4, t4, n4, o4, d5, t5, n5, o5, d6, t6, n6, o6, d7, t7, n7, o7, d8, t8, n8, o8, d9, t9, n9, o9, d10, t10, n10, o10, d11, t11, n11, o11, d12, t12, n12, o12, d13, t13, n13, o13, d14, t14, n14, o14, d15, t15, n15, o15, d16, t16, n16, o16, d17, t17, n17, o17) SyscallArgType_ ## r,
#include "syscall_winxp.hh"
#undef MagicSyscall
};

// Direction of arguments
static SyscallArgDir _syscallargsdir[][17] = {
#define MagicSyscall(s, n, r, a, d1, t1, n1, o1, d2, t2, n2, o2, d3, t3, n3, o3, d4, t4, n4, o4, d5, t5, n5, o5, d6, t6, n6, o6, d7, t7, n7, o7, d8, t8, n8, o8, d9, t9, n9, o9, d10, t10, n10, o10, d11, t11, n11, o11, d12, t12, n12, o12, d13, t13, n13, o13, d14, t14, n14, o14, d15, t15, n15, o15, d16, t16, n16, o16, d17, t17, n17, o17) {SyscallArgDir_ ## d1, SyscallArgDir_ ## d2, SyscallArgDir_ ## d3, SyscallArgDir_ ## d4, SyscallArgDir_ ## d5, SyscallArgDir_ ## d6, SyscallArgDir_ ## d7, SyscallArgDir_ ## d8, SyscallArgDir_ ## d9, SyscallArgDir_ ## d10, SyscallArgDir_ ## d11, SyscallArgDir_ ## d12, SyscallArgDir_ ## d13, SyscallArgDir_ ## d14, SyscallArgDir_ ## d15, SyscallArgDir_ ## d16, SyscallArgDir_ ## d17}, 
#include "syscall_winxp.hh"
#undef MagicSyscall
};

// Optional arguments
static SyscallArgOpt _syscallargsopt[][17] = {
#define MagicSyscall(s, n, r, a, d1, t1, n1, o1, d2, t2, n2, o2, d3, t3, n3, o3, d4, t4, n4, o4, d5, t5, n5, o5, d6, t6, n6, o6, d7, t7, n7, o7, d8, t8, n8, o8, d9, t9, n9, o9, d10, t10, n10, o10, d11, t11, n11, o11, d12, t12, n12, o12, d13, t13, n13, o13, d14, t14, n14, o14, d15, t15, n15, o15, d16, t16, n16, o16, d17, t17, n17, o17) {SyscallArgOpt_ ## o1, SyscallArgOpt_ ## o2, SyscallArgOpt_ ## o3, SyscallArgOpt_ ## o4, SyscallArgOpt_ ## o5, SyscallArgOpt_ ## o6, SyscallArgOpt_ ## o7, SyscallArgOpt_ ## o8, SyscallArgOpt_ ## o9, SyscallArgOpt_ ## o10, SyscallArgOpt_ ## o11, SyscallArgOpt_ ## o12, SyscallArgOpt_ ## o13, SyscallArgOpt_ ## o14, SyscallArgOpt_ ## o15, SyscallArgOpt_ ## o16, SyscallArgOpt_ ## o17}, 
#include "syscall_winxp.hh"
#undef MagicSyscall
};

// Syscall names
static const char *_syscallsname[] = {
#define MagicSyscall(s, n, r, a, d1, t1, n1, o1, d2, t2, n2, o2, d3, t3, n3, o3, d4, t4, n4, o4, d5, t5, n5, o5, d6, t6, n6, o6, d7, t7, n7, o7, d8, t8, n8, o8, d9, t9, n9, o9, d10, t10, n10, o10, d11, t11, n11, o11, d12, t12, n12, o12, d13, t13, n13, o13, d14, t14, n14, o14, d15, t15, n15, o15, d16, t16, n16, o16, d17, t17, n17, o17) #s, 
#include "syscall_winxp.hh"
#undef MagicSyscall
};

// Argument names
static const char *_syscallargstypename[] = {
  "NONE",
#define MagicSyscallArgType(x) #x,
#include "syscall_winxp.hh"
#undef MagicSyscallArgType
};

const char *syscall_name(SyscallNo s) {
  assert(s < SYSCALLS_NO);
  return _syscallsname[s];
}

const char *syscallarg_name(SyscallArgType s) {
  return _syscallargstypename[s];
}

uint32_t peek_dword(uint32_t addr) {
  uint32_t dword;

  if (obj_peek) {
    obj_peek((void *) addr, sizeof(dword), (void *) &dword);
  } else {
    dword = *((uint32_t *) addr);
  }

  return dword;
}

uint32_t TID() {
  if (current_thread_id) {
    return current_thread_id();
  } else {
    return 0;
  }
}

Syscall::Syscall(void) {
  // dummy constructor, for deserialization
  sysno = (SyscallNo) 0;
  allocated_buffers = NULL;
}

Syscall::Syscall(SyscallNo n) {
  sysno = n;
  allocated_buffers = NULL;
}

Syscall::Syscall(SyscallNo n, void *base, uint32_t current_id, uint32_t ti, SyscallDir sd) {
  void *p;
  void *a;
  SyscallArgType t;
  SyscallArgDir d;
  SyscallArgOpt o;
  int s = 0;

  assert(n < SYSCALLS_NO);

  sysno = n;
  id = current_id;
  tid = ti;
  allocated_buffers = NULL;
  args = SyscallArgs(_syscallargsno[n]);
  dir = sd;

  // get syscall time
  struct timeval tv;
  gettimeofday(&tv, NULL);
  setTimestamp(&tv);

  // Generate the arguments backward. This approach easily allows us to process
  // arguments describing the size of buffers before the buffers are processed
  // (the argument describing the size always follows the argument describing
  // the buffer).
  for (int i = _syscallargsno[n] - 1; i >= 0; i--) {
    a = (void *) (((uint32_t *) base) + i);
    p = (void *) peek_dword((uint32_t) (((uint32_t *) base) + i));
    t = _syscallargstype[n][i];
    // This should be useless (we leave it here as a sanity check)
    assert (t != SyscallArgType_NONE);

    d = _syscallargsdir[n][i];
    o = _syscallargsopt[n][i];
    // Size is generally 0 because it can be derived from the type of the
    // argument
    s = 0;

    // This is used to compute the size of buffers. After an analysis of the
    // prototypes of the syscalls I noticed that if 'i' is a PVOID/PPVOID
    // argument, the size of the buffer is always stored in 'i+1'
    if (t == SyscallArgType_PVOID_WST_SIZEAFTER) {
      // Sanity check: a PVOID cannot be the last argument
      assert(i < _syscallargsno[n] - 1);
      if (args[i + 1]->getArgType() == SyscallArgType_ULONG_WST) {
	s = (int32_t) args[i + 1]->getArg();
      } else if (args[i + 1]->getArgType() == SyscallArgType_PULONG_WST) {
	// this happens for system calls such as NtConnectPort
	assert(args[i + 1]->getArg() != 0);
	s = (int32_t) peek_dword((uint32_t) (int32_t *) args[i + 1]->getArg());
      } else {
	// Sanity check: the argument specifying the size must be an integer
	// (pointers are not allowed)
	wst_debug("* Error analyzing syscall '%s', parameter #%d\n", 
		  _syscallsname[sysno], i);
	assert(0);
      }
    }

    args[i] = new SyscallArg(i + 1, t, d, a, p, s, o, _syscallargsname[n][i]);
  }

  retval = new SyscallArg(0, _syscallretval[n], SyscallArgDir_OUT, NULL, NULL, 0, SyscallArgOpt_MANDATORY, "ReturnValue");
}

Syscall::~Syscall() {
  // Deallocate dynamically allocated buffers (if any)
  if (allocated_buffers) {
    for (vector<void*>::iterator it = allocated_buffers->begin(); 
	 it != allocated_buffers->end(); it++) {
      // wst_debug("[gc] Deallocating buffer %p", *it);
      free(*it);
    }
    delete allocated_buffers;
  }

  for (SyscallArgs::iterator it = args.begin(); it != args.end(); it++) {
    delete *it;
  }

  if (retval)
    delete retval;
}

SyscallNo Syscall::getSysNo() {
  return sysno;
}

SyscallArg *Syscall::getArg(unsigned int i) {
  assert(i <= args.size());
  return args[i - 1];
}

SyscallArgs Syscall::getArgs() {
  return args;
}

int Syscall::getArgsNo() {
  return args.size();
}

uint32_t Syscall::getTid() {
  return tid;
}

SyscallDir Syscall::getDir() {
  return dir;
}

void Syscall::setDir(SyscallDir d) {
  dir = d;
}

void Syscall::setReturnValue(DWORD_WST val) {
  retval->setArg((void *) val);
}

SyscallArg* Syscall::getReturnValue() {
  return retval;
}

void Syscall::setTimestamp(struct timeval *tv) {
  memcpy(&timestamp, tv, sizeof(timestamp));
}

string Syscall::str() {
  char tmp[128];

  snprintf(tmp, sizeof(tmp), "%08x %.8x %s", (unsigned int) tid, sysno, syscall_name(sysno));
  return string(tmp);
}

// Add a buffer to the allocated buffers list
void Syscall::addAllocatedBuffer(void *buf) {
  if(!allocated_buffers)
    allocated_buffers = new vector<void*>;

  // wst_debug("[gc] Recording allocated buffer %p", buf);
  allocated_buffers->push_back(buf);
}

// Checks whether this system call is pending. In this case, it returns the
// event handle that will be set to the signaled state after the write
// operation completes. If this system call is not pending,
// Syscall::isPending() returns INVALID_HANDLE_VALUE;
HANDLE_WST Syscall::isPending() {
  DWORD r;

  r = (HANDLE_WST) INVALID_HANDLE_VALUE;

  if ((DWORD) retval->getArg() == STATUS_PENDING) {
    wst_debug("[W] pending system call '%s' (%.8x)\n", syscall_name(sysno), sysno);
    switch (sysno) {
    case Syscall_NtReadFile:
    case Syscall_NtWriteFile:
      if (getArg(2)->getArg() != NULL) {
 	r = (HANDLE_WST) getArg(2)->getArg();
      }
      break;
    default:
      break;
    }
  }

  return r;
}

// ****************************************************************************
// SyscallArg class methods
// ****************************************************************************
SyscallArg::SyscallArg(void) {
    // dummy constructor, for deserialization
    at = SyscallArgType_NONE;
    ad = SyscallArgDir_NONE;
    ap = NULL;
    aa = NULL;
    an = 0;
    as = 0;
    ao = SyscallArgOpt_NONE;
    aname = "Unknown";
}

SyscallArg::SyscallArg(int n, SyscallArgType t, SyscallArgDir d, void *a, 
		       void *p, int s, SyscallArgOpt o, const char *name) {
  at = t;
  ad = d;
  ap = p;
  aa = a;
  an = n;
  as = s;
  ao = o;
  aname = name;
}

SyscallArg::~SyscallArg() {
  ;
}

SyscallArgType SyscallArg::getArgType() {
  return at;
}

SyscallArgDir SyscallArg::getArgDir() {
  return ad;
}

void *SyscallArg::getArg() {
  return ap;
}

void *SyscallArg::getArgAddr() {
  return aa;
}

int SyscallArg::getSize() {
  return as;
}

SyscallArgOpt SyscallArg::getOpt() {
  return ao;
}

int SyscallArg::getArgNo() {
  return an;
}

void SyscallArg::setArg(void *a) {
  ap = a;
}
// ****************************************************************************

void register_peek_mem(obj_peek_t p) {
  obj_peek = p;
}

void register_get_current_thread_id(current_thread_id_t p) {
  current_thread_id = p;
}
