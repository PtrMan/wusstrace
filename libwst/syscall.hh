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

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <boost/archive/basic_archive.hpp>
#include <sys/time.h>
#include <time.h>
#include <sys/time.h>

#include <stdint.h>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

#include "wstconfig.hh"
#include "type_winxp.hh"

// Syscall numbers
enum SyscallNo {
#define MagicSyscall(s, n, r, a, d1, t1, n1, o1, d2, t2, n2, o2, d3, t3, n3, o3, d4, t4, n4, o4, d5, t5, n5, o5, d6, t6, n6, o6, d7, t7, n7, o7, d8, t8, n8, o8, d9, t9, n9, o9, d10, t10, n10, o10, d11, t11, n11, o11, d12, t12, n12, o12, d13, t13, n13, o13, d14, t14, n14, o14, d15, t15, n15, o15, d16, t16, n16, o16, d17, t17, n17, o17) Syscall_ ## s, 
#include "syscall_winxp.hh"
#undef MagicSyscall
  SYSCALLS_NO,
};

// System call direction
enum SyscallDir {
  SyscallDir_ENTER,         // The system call is being invoked
  SyscallDir_EXIT,          // The system call is returning
};

// Argument types
enum SyscallArgType {
  SyscallArgType_NONE = 0,
  #define MagicSyscallArgType(x) SyscallArgType_ ## x,
  #include "syscall_winxp.hh"
  #undef MagicSyscallArgType
};

// Argument directions
enum SyscallArgDir {
  SyscallArgDir_NONE = 0,
  SyscallArgDir_IN = 1,        // Input argument
  SyscallArgDir_OUT = 2,       // Output argument
  SyscallArgDir_INOUT = 3,     /// Input & output argument
};

// Type of argument
enum SyscallArgOpt {
  SyscallArgOpt_NONE = 0,          
  SyscallArgOpt_OPTIONAL = 1,      // The argument is optional
  SyscallArgOpt_MANDATORY = 2,     // The argument is mandatory
};

class SyscallArg;
class Syscall;

//extern SyscallArg *current_syscallarg;
extern ULARGE_INTEGER uliStart;

const char *syscall_name(SyscallNo s);
const char *syscallarg_name(SyscallArgType s);

typedef vector<SyscallArg*> SyscallArgs;
typedef vector<Syscall*> Syscalls;

class SyscallArg {
private:
  SyscallArgType at;		// Argument type
  SyscallArgDir ad;		// Argument direction
  void *ap;			// Pointer to the argument
  void *aa;			// Address to the argument
  int an;			// Argument number
  int as;			// Argument size
  SyscallArgOpt ao;		// Argument mandatory/optional
  const char *aname;		// Argument name

  // To allow unintrusive serialisation/deserialisation
  template<class Archive> friend void serialize(Archive & ar, SyscallArg & arg, const unsigned int version);

public:
  // Empty constructor, for deserialisation
  SyscallArg(void);
  // Real constructor:
  // * argument number
  // * argument type
  // * argument direction
  // * argument address
  // * argument value
  // * argument size
  // * argument optional/mandatory
  // * argument name
  SyscallArg(int, SyscallArgType, SyscallArgDir, void *, void *, int, SyscallArgOpt, const char *);
  ~SyscallArg();

  SyscallArgType getArgType();
  SyscallArgDir getArgDir();
  void *getArg();
  void *getArgAddr();
  void setArg(void *);
  void setSize(int);
  int getArgNo();
  int getSize();
  SyscallArgOpt getOpt();
  string str();
};

class Syscall {
private:
  SyscallNo sysno;		// system call number
  SyscallArgs args;		// system call arguments
  SyscallArg *retval;		// returned value
  uint32_t id;			// ID of this system call
  uint32_t tid;			// ID of the thread that issued this syscall
  struct timeval timestamp;	// when the system call was executed
  SyscallDir dir;               // direction (enter / exit)

  // These are buffers that have been dynamically allocated before the
  // execution of this system call, and needs to be deallocated when the
  // syscall object is destroyed
  vector <void*> *allocated_buffers;

  // To allow unintrusive serialization/deserialization
  template<class Archive> friend void serialize(Archive & ar, Syscall & s, const unsigned int version);

public:
  Syscall(void);
  Syscall(SyscallNo);
  Syscall(SyscallNo, void*, uint32_t current_id, uint32_t t = 0, SyscallDir d = SyscallDir_ENTER);
  ~Syscall();

  SyscallNo getSysNo();
  int getArgsNo();
  SyscallArg *getArg(unsigned int);
  SyscallArgs getArgs();
  SyscallArg *getReturnValue();
  SyscallDir getDir();
  uint32_t getTid();
  void setDir(SyscallDir);
  string str();
  void setReturnValue(DWORD_WST val);
  void setTimestamp(struct timeval *tv);

  HANDLE_WST isPending();
  void addAllocatedBuffer(void *buf);
};

typedef int8_t (*obj_peek_t)(void *addr, uint32_t size, void *out);
typedef uint32_t (* current_thread_id_t)();

extern obj_peek_t obj_peek;
extern current_thread_id_t current_thread_id;

void register_peek_mem(obj_peek_t p);
void register_get_current_thread_id(current_thread_id_t p);
uint32_t peek_dword(uint32_t);
uint32_t TID();

#endif
