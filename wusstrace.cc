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
#include <stdlib.h>
#include <windows.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "config.hh"
#include "console.hh"
#include "common.hh"

void show_help(char *name)
{
  printf("== WUSStrace v%s ==\n", WUSSTRACE_VERSION);
  printf(" Copyright 2010, Lorenzo Martignoni and Roberto Paleari\n");
  printf(" License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n");
  printf(" This  is  free  software:  you  are free to change and redistribute it.\n");
  printf(" There is NO WARRANTY, to the extent permitted by law.\n");
  printf("\n");
  printf(" Syntax: %s [options] <target application | pid>\n\n", name);
  printf(" General options:\n");
  printf(" -d FILE  dump syscalls and arguments to file FILE_pid.dump (in XML)\n");
  printf(" -f       enable tracing of children processes\n");
  printf(" -g       enable tracing of graphic syscalls\n");
  printf(" -h       show this help screen\n");
  printf(" -i FILE  use FILE instead of %s\n", WUSSTRACE_DLL);
  printf(" -l DIR   create lockfile DIR\\pid.lock\n");
  printf(" -p       log pending syscalls only when completed\n");
  printf(" -r       resolve relative object paths (e.g., files, keys) to absolute ones\n");
  printf(" -s       suspend threads on timeout, instead of terminating\n");
  printf(" -t       enable tracing of children threads in remote processes\n");
  printf(" -T       enable tracing of infected processes\n");
  printf(" -w SEC   interrupt the analysis after SEC seconds\n");
  printf(" -x       force absolute file offsets\n");
  printf(" -z       enable various options for malware analysis\n");
  printf("          (children & infected, pending syscalls, file offsets, ...)\n");
  printf("\n");
}

int main(int argc, char **argv)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  // Options-related variables
  Options options;
  int opt;

  // Parse command line options
  memset(&options, 0, sizeof(options));
  options.bDump = TRUE;
  options.bLock = FALSE;
  options.bGUI = FALSE;
  options.bFollowChildren = FALSE;
  options.bFollowRemoteThreads = FALSE;
  options.bFollowInfectedProcesses = FALSE;
  options.bResolveRelativePaths = FALSE;
  options.bPending = FALSE;
  options.bFileOffset= FALSE;
  options.bTimeoutSuspend = FALSE;
  strncpy(options.sDumpFile, "wusstrace", sizeof(options.sDumpFile));

  while ((opt = getopt(argc, argv, "hftTgd:l:prsi:w:xz")) != -1) {
    switch (opt) {
    case 'd':
      options.bDump = TRUE;
      strncpy(options.sDumpFile, optarg, sizeof(options.sDumpFile));
      break;
    case 'l':
      options.bLock = TRUE;
      strncpy(options.sLockFile, optarg, sizeof(options.sLockFile));
      break;
    case 'f':
      options.bFollowChildren = TRUE;
      break;
    case 'i':
      strncpy(options.sDllFile, optarg, sizeof(options.sDllFile));
      break;
    case 't':
      options.bFollowRemoteThreads = TRUE;
      break;
    case 'T':
      options.bFollowInfectedProcesses = TRUE;
      break;
    case 'g':
      options.bGUI = TRUE;
      break;
    case 'r':
      options.bResolveRelativePaths = TRUE;
      break;
    case 's':
      options.bTimeoutSuspend = TRUE;
      break;
    case 'p':
      options.bPending = TRUE;
      break;
    case 'w': {
      // Set analysis timeout
      time_t t;

      errno = 0;
      options.iTimeout = strtoul(optarg, NULL, 10);
      if (errno != 0) {
	printf("! invalid timeout value '%s'\n", optarg);
	return 1;
      }

      // let options.iTimeout hold the expiry time
      t = time(NULL);
      printf("* current time: %s", ctime(&t));

      options.iTimeout = t + options.iTimeout;
      printf("* timeout at: %s", ctime(&options.iTimeout));
      break;
    }
    case 'x':
      options.bFileOffset = TRUE;
      break;
    case 'z':
      // Enable various options, usefull for malware analysis
      options.bFollowRemoteThreads = TRUE;
      options.bFollowInfectedProcesses = TRUE;
      options.bResolveRelativePaths = TRUE;
      options.bPending = TRUE;
      options.bFileOffset = TRUE;
      break;
    default:
      printf("! unknown option '%s'\n", argv[optind-1]);
    case 'h':
      show_help(argv[0]);
      return 0;
    }
  }

  if(!argv[optind]) {
    // Filename is missing
    printf("! target application name is missing. Use '%s -h' for help", argv[0]);
    return 1;
  }

  // Build absolute WUSSTRACE_DLL filename
  if(strlen(options.sDllFile) == 0) {
    char *p;
    GetModuleFileName(NULL, options.sDllFile, sizeof(options.sDllFile));
    p = strrchr(options.sDllFile, '\\');
    if(!p) p = options.sDllFile;
    else p += 1;
    *p = '\0';
    strncat(options.sDllFile, WUSSTRACE_DLL, sizeof(options.sDllFile));
  }

  // Check that DLL to inject exists
  if(!fileExists(options.sDllFile)) {
    printf("! unable to find %s\n", options.sDllFile);   
    return 1;
  }

  printf("* DLL path: %s\n", options.sDllFile);   

  // Allocate console
  console = Console::instance(true);

  printf("* invoking child with command line '%s'\n", argv[optind]);

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Create target (stopped) process
  if(!CreateProcess(NULL, argv[optind], NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, 
		    NULL, NULL, &si, &pi)) {
    printf("! CreateProcess('%s') has failed", argv[optind]);
    return -1;
  }

  printf("* (suspended) process created: pid %u; tid: %u.\n", 
	 (unsigned int) pi.dwProcessId, (unsigned int) pi.dwThreadId);

  // Mark child process' main thread as to be traced
  options.dTraceThread = pi.dwThreadId;

  // Injection
  injectDll(pi.hProcess, options.sDllFile, &options, sizeof(options));

  ResumeThread(pi.hThread);
  printf("* Process (initial thread) resumed..");


  // Close handles
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return 0;
}
