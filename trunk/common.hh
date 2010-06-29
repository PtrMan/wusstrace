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


#ifndef _COMMON_HH
#define _COMMON_HH

#include <iostream>
#include <fstream>
#include <time.h>

#ifndef DONT_INCLUDE_TYPES
#include "type_winxp.hh"
#endif

using namespace std;

#define WUSSTRACE_DLL     "wusstrace.dll"
#define WUSSTRACE_VERSION "0.1"

// Check whether a system call was successfull
#ifndef NT_SUCCESS
#define NT_SUCCESS(x) ((x)>=0)
#endif

// Executes statement 'x' atomically
// WARNING: 'x' should NOT issue any system call
#define ATOMIC(x)				\
  {						\
    global_us_mutex_lock();			\
    x;						\
    global_us_mutex_unlock();			\
  }
// As the previous one, but value returned by 'x' is assigned to 't'
#define ATOMIC_WITH_RET(t,x)			\
  {						\
    global_us_mutex_lock();			\
    t = (x);					\
    global_us_mutex_unlock();			\
  }

// This is the structure that holds command line options, that are
// passed from multienv-cli.cc to injdll.cc
typedef struct {
  BOOL bGUI;
  BOOL bDump;
  BOOL bFastSyscall;
  BOOL bFollowChildren;
  BOOL bFollowRemoteThreads;
  BOOL bFollowInfectedProcesses;
  BOOL bLock;
  BOOL bResolveRelativePaths;	// Resolve relative paths to absolute ones
  BOOL bPending;                // Delay dump of pending syscalls
  BOOL bFileOffset;		// Force current file offset into read/write system calls
  BOOL bTimeoutSuspend;		// Suspend threads on timeout
  DWORD dTraceThread;
  char sDumpFile[MAX_PATH];
  char sLockFile[MAX_PATH];
  char sDllFile[MAX_PATH];
  time_t iTimeout;
} Options;

#include <windows.h>
#include <string>

using namespace std;

void initNtFunctions();
BOOL resolveRelativeObject(PUNICODE_STRING pus, HANDLE RootDirectory);
void injectDll(HANDLE ph, char *dll, void *buf, int bufsize);
string wide2ansi(unsigned char *src, uint32_t n);
ULARGE_INTEGER getRelativeTime(ULARGE_INTEGER uliBegin, ULARGE_INTEGER uliEnd);
DWORD myGetThreadId(HANDLE_WST hThread);
bool fileExists(char *filename);

void global_us_mutex_lock() __attribute__((noinline));
void global_us_mutex_unlock();

#endif
// _COMMON_HH
