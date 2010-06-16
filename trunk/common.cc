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


#include <windows.h>
#include <assert.h>

#include "config.hh"
#include "console.hh"
#include "common.hh"

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS) 0)
#endif

// Prototypes for undocumented NT functions 
typedef NTSTATUS (WINAPI *NTQUERYKEY) (HANDLE, KEY_INFORMATION_CLASS, PVOID, ULONG, PULONG);
typedef NTSTATUS (WINAPI *NTQUERYINFORMATIONFILE) (HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS);
typedef NTSTATUS (WINAPI *NTQUERYINFORMATIONTHREAD) (HANDLE, THREADINFOCLASS_WST, PVOID, ULONG, PULONG);

// Pointers to undocumented NT functions
static NTQUERYKEY               fp_NtQueryKey;
static NTQUERYINFORMATIONFILE   fp_NtQueryInformationFile;
static NTQUERYINFORMATIONTHREAD fp_NtQueryInformationThread;

// DLL attach time
ULARGE_INTEGER uliStart;

// The global, user-space mutex
static DWORD dwGlobalLock;

void initNtFunctions() {
  HMODULE hNtdll;

  hNtdll = LoadLibrary("ntdll.dll");

  fp_NtQueryKey = (NTQUERYKEY) GetProcAddress(hNtdll, "NtQueryKey");
  assert(fp_NtQueryKey != 0);

  fp_NtQueryInformationFile = (NTQUERYINFORMATIONFILE) GetProcAddress(hNtdll, "NtQueryInformationFile");
  assert(fp_NtQueryInformationFile != 0);

  fp_NtQueryInformationThread = (NTQUERYINFORMATIONTHREAD) GetProcAddress(hNtdll, "NtQueryInformationThread");
  assert(fp_NtQueryInformationThread != 0);
}

// Resolve the relative object name into an absolute one
BOOL resolveRelativeObject(PUNICODE_STRING pus, HANDLE RootDirectory) {
  char rootname[256];
  DWORD x;
  NTSTATUS status;

  memset(pus, 0, sizeof(UNICODE_STRING));

  // registry key
  status = fp_NtQueryKey(RootDirectory, KeyNameInformation, (PKEY_NAME_INFORMATION) rootname, sizeof(rootname), &x);
  if (status == STATUS_SUCCESS) {
    PKEY_NAME_INFORMATION pkni = (PKEY_NAME_INFORMATION) rootname;
      
    pus->Length = pus->MaximumLength = pkni->NameLength;
    pus->Buffer = (PWSTR) malloc(pus->Length);
    memcpy(pus->Buffer, pkni->Name, pus->Length);

    return TRUE;
  }

  // file/directory object
  IO_STATUS_BLOCK ios;
  status = fp_NtQueryInformationFile(RootDirectory, &ios, rootname, sizeof(rootname), FileNameInformation);
  if (status == STATUS_SUCCESS) {
    PKEY_NAME_INFORMATION pkni = (PKEY_NAME_INFORMATION) rootname;
      
    pus->Length = pus->MaximumLength = pkni->NameLength;
    pus->Buffer = (PWSTR) malloc(pus->Length);
    memcpy(pus->Buffer, pkni->Name, pus->Length);

    return TRUE;
  }

  // no luck
  return FALSE;
}

// Inject a dll into a remote process
void injectDll(HANDLE ph, char *dll, void *buf, int bufsize) {
  LPVOID p, ll;
  BOOL b;
  HANDLE ht, hm;
  PVOID rbuf;

  debug("****** Remote process infection ******");

  // Allocate room to store the DLL name
  p = VirtualAllocEx(ph, NULL, strlen(dll),
                     MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  assert(p);
  debug("* Allocated memory to store DLL name");

  // Write the DLL name
  b = WriteProcessMemory(ph, p, dll, strlen(dll), NULL);
  assert(b);
  debug("* Wrote %s @ %.8x", dll, p);

  // Create a new thread to load the dll
  ll = (LPVOID) GetProcAddress(LoadLibrary("kernel32.dll"), "LoadLibraryA");
  debug("* Got address of \"LoadLibraryA\": %.8x", ll);

  ht = CreateRemoteThread(ph, NULL, 0, (LPTHREAD_START_ROUTINE) ll,
                          p, CREATE_SUSPENDED, NULL);
  assert(ht);
  debug("* Created remote thread");

  hm = NULL;
  rbuf = NULL;
  if (buf) {
    // Copy the buffer into the remote process
    hm = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
                           bufsize, TEXT("Global\\MultiEnvOptions"));
    assert(hm);
    rbuf = (PVOID) MapViewOfFile(hm, FILE_MAP_ALL_ACCESS, 0, 0, bufsize);
    assert(rbuf);
    CopyMemory(rbuf, buf, bufsize);
    debug("* Copied %.8x-%.8x into remote process", buf, 
	  ((DWORD) buf) + bufsize - 1);
  }

  // Resume the injected thread
  ResumeThread(ht);
  debug("* Resumed remote thread");
  // Wait till termination
  WaitForSingleObject(ht, INFINITE);
  debug("* Remote thread terminated");

  if (buf) {
    // Deallocate shared memory
    UnmapViewOfFile(rbuf);
    CloseHandle(hm);
  }

  debug("****** Infection completed ******");
}

// Convert a wchar[] into an ASCII string
string wide2ansi(unsigned char *src, uint32_t n) {
  uint32_t cnt;
  string r = "";

  if (!src)
    return r;
  
  for (cnt = 0; cnt < n; ++cnt) {
    if (src[2*cnt + 1])
      break;
    r.push_back(src[2*cnt]);
  }
  
  return r;
}

// Get time as 100ns intervals
ULARGE_INTEGER getRelativeTime(ULARGE_INTEGER uliBegin, ULARGE_INTEGER uliEnd) {
  ULARGE_INTEGER temp;

  temp.HighPart = uliEnd.HighPart - uliBegin.HighPart;

  if (uliEnd.LowPart >= uliBegin.LowPart)
    temp.LowPart = uliEnd.LowPart - uliBegin.LowPart;
  else {
    temp.HighPart--;
    temp.LowPart = (uliEnd.LowPart - uliBegin.LowPart) % 0xffffffff;
  }

  return temp;
}

// GetThreadId is only supported in Windows Vista and Windows 2003 Server; we
// reimplement it here so that it can also be used in Windows XP
DWORD myGetThreadId(HANDLE_WST hThread) {
  DWORD status;
  THREAD_BASIC_INFORMATION tbi;
  
  status = fp_NtQueryInformationThread((HANDLE) hThread, ThreadBasicInformation, 
				       &tbi, sizeof(THREAD_BASIC_INFORMATION), NULL);

  if (status != STATUS_SUCCESS) {
    return 0;
  }
  
  return (DWORD) tbi.ClientId.UniqueThread;
}

// Returns true if the supplied filename corresponds to an existsing file
bool fileExists(char *filename)
{
  return (GetFileAttributes(filename) != INVALID_FILE_ATTRIBUTES);
}

// Acquire the global mutex, without issuing any system call
void global_us_mutex_lock() {
  asm __volatile__ ("do_lock:"			\
		    "lock bts $0x0, %0;"	\
		    "jc do_lock;"		\
		    : "=m" (dwGlobalLock)
		    );
}

// Release the global mutex, without issuing any system call
void global_us_mutex_unlock() {
  asm __volatile__ ("and $0x0, %0;"		\
		    : "=m" (dwGlobalLock)
		    );
}

