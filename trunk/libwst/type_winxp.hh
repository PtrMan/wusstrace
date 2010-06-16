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

#ifndef __TYPE_WINXP_HH__
#define __TYPE_WINXP_HH__

#ifndef NOMINMAX
#define NOMINMAX
#endif

typedef uint32_t UNKNOWN_TYPE;

#ifdef MINGW
#include <windows.h>
#include <ntdef.h>
#include <ddk/ntapi.h>
typedef UNKNOWN_TYPE SEMAPHORE_INFORMATION_CLASS;
typedef UNKNOWN_TYPE EVENT_INFORMATION_CLASS;
typedef UNKNOWN_TYPE MUTANT_INFORMATION_CLASS;
typedef UNKNOWN_TYPE TIMER_INFORMATION_CLASS;
typedef UNKNOWN_TYPE TIMER_INFORMATION_CLASS;
typedef UNKNOWN_TYPE IO_COMPLETION_INFORMATION_CLASS;
#else
#include <winternl.h>
#include <windef.h>
typedef UNKNOWN_TYPE DEBUG_CONTROL_CODE;
typedef UNKNOWN_TYPE WAIT_TYPE;
typedef UNKNOWN_TYPE *PUSER_STACK;
typedef UNKNOWN_TYPE AUDIT_EVENT_TYPE;
typedef UNKNOWN_TYPE *PKNORMAL_ROUTINE;
typedef UNKNOWN_TYPE KEY_SET_INFORMATION_CLASS;
typedef UNKNOWN_TYPE PEXECUTION_STATE;
typedef UNKNOWN_TYPE JOBOBJECTINFOCLASS;
#endif // !MINGW

typedef UNKNOWN_TYPE *PFILE_PATH;
typedef UNKNOWN_TYPE *PBOOT_OPTIONS;
typedef UNKNOWN_TYPE *PPORT_VIEW;
typedef UNKNOWN_TYPE *PJOB_SET_ARRAY;
typedef UNKNOWN_TYPE *PFILE_QUOTA_LIST_INFORMATION;
typedef UNKNOWN_TYPE *PDBGUI_WAIT_STATE_CHANGE;
typedef UNKNOWN_TYPE DEBUGOBJECTINFOCLASS;
typedef UNKNOWN_TYPE *PFILE_USER_QUOTA_INFORMATION;
typedef UNKNOWN_TYPE *PREMOTE_PORT_VIEW;
typedef UNKNOWN_TYPE *PSAFEBOOT_MODE;
typedef UNKNOWN_TYPE *PFILE_GET_EA_INFORMATION;
typedef UNKNOWN_TYPE VDMSERVICECLASS;
typedef UNKNOWN_TYPE *PEVENT_TRACE_HEADER;


typedef PTOKEN_USER PTOKEN_USER_WST;
typedef PLCID PLCID_WST;
typedef FS_INFORMATION_CLASS FS_INFORMATION_CLASS_WST;
typedef EVENT_TYPE EVENT_TYPE_WST;
typedef SECTION_INFORMATION_CLASS SECTION_INFORMATION_CLASS_WST;
typedef PSECURITY_QUALITY_OF_SERVICE PSECURITY_QUALITY_OF_SERVICE_WST;
typedef LONG LONG_WST;
typedef PFILE_USER_QUOTA_INFORMATION PFILE_USER_QUOTA_INFORMATION_WST;
typedef POWER_INFORMATION_LEVEL POWER_INFORMATION_LEVEL_WST;
typedef PUSHORT PUSHORT_WST;
typedef PUCHAR PUCHAR_WST;
// typedef PLARGE_INTEGER PLARGE_INTEGER_WST;
typedef PREMOTE_PORT_VIEW PREMOTE_PORT_VIEW_WST;
typedef SHUTDOWN_ACTION SHUTDOWN_ACTION_WST;
typedef PEXECUTION_STATE PEXECUTION_STATE_WST;
typedef ACCESS_MASK ACCESS_MASK_WST;
typedef DEBUG_CONTROL_CODE DEBUG_CONTROL_CODE_WST;
typedef PBOOT_OPTIONS PBOOT_OPTIONS_WST;
typedef SEMAPHORE_INFORMATION_CLASS SEMAPHORE_INFORMATION_CLASS_WST;
typedef POBJECT_TYPE_LIST POBJECT_TYPE_LIST_WST;
typedef WAIT_TYPE WAIT_TYPE_WST;
// typedef PUNICODE_STRING PUNICODE_STRING_WST;
typedef PCONTEXT PCONTEXT_WST;
typedef PFILE_NOTIFY_INFORMATION PFILE_NOTIFY_INFORMATION_WST;
typedef POWER_ACTION POWER_ACTION_WST;
typedef PLONG PLONG_WST;
typedef USHORT USHORT_WST;
typedef PEXCEPTION_RECORD PEXCEPTION_RECORD_WST;
typedef PFILE_FULL_EA_INFORMATION PFILE_FULL_EA_INFORMATION_WST;
typedef VDMSERVICECLASS VDMSERVICECLASS_WST;
typedef PHARDERROR_RESPONSE PHARDERROR_RESPONSE_WST;
typedef PFILE_NETWORK_OPEN_INFORMATION PFILE_NETWORK_OPEN_INFORMATION_WST;
typedef PTOKEN_SOURCE PTOKEN_SOURCE_WST;
typedef PFILE_SEGMENT_ELEMENT PFILE_SEGMENT_ELEMENT_WST;
typedef ULONG ULONG_WST;
typedef PIO_APC_ROUTINE PIO_APC_ROUTINE_WST;
// typedef PULARGE_INTEGER PULARGE_INTEGER_WST;
// typedef PLANGID PLANGID_WST;
// typedef PSECURITY_DESCRIPTOR PSECURITY_DESCRIPTOR_WST;
typedef PLUID PLUID_WST;
// typedef PFILE_BASIC_INFORMATION PFILE_BASIC_INFORMATION_WST;
typedef PFILE_PATH PFILE_PATH_WST;
typedef PUSER_STACK PUSER_STACK_WST;
typedef LDT_ENTRY LDT_ENTRY_WST;
typedef PFILE_QUOTA_LIST_INFORMATION PFILE_QUOTA_LIST_INFORMATION_WST;
typedef PSID PSID_WST;
typedef PKEY_VALUE_ENTRY PKEY_VALUE_ENTRY_WST;
typedef PTOKEN_PRIMARY_GROUP PTOKEN_PRIMARY_GROUP_WST;
typedef PDEVICE_POWER_STATE PDEVICE_POWER_STATE_WST;
typedef TOKEN_INFORMATION_CLASS TOKEN_INFORMATION_CLASS_WST;
typedef PBOOLEAN PBOOLEAN_WST;
typedef ATOM_INFORMATION_CLASS ATOM_INFORMATION_CLASS_WST;
typedef KEY_INFORMATION_CLASS KEY_INFORMATION_CLASS_WST;
typedef PULONG PULONG_WST;
typedef JOBOBJECTINFOCLASS JOBOBJECTINFOCLASS_WST;
typedef SYSTEM_POWER_STATE SYSTEM_POWER_STATE_WST;
typedef LATENCY_TIME LATENCY_TIME_WST;
typedef PDBGUI_WAIT_STATE_CHANGE PDBGUI_WAIT_STATE_CHANGE_WST;
typedef PJOB_SET_ARRAY PJOB_SET_ARRAY_WST;
typedef NTSTATUS NTSTATUS_WST;
typedef SECURITY_INFORMATION SECURITY_INFORMATION_WST;
typedef DEBUGOBJECTINFOCLASS DEBUGOBJECTINFOCLASS_WST;
typedef OBJECT_INFORMATION_CLASS OBJECT_INFORMATION_CLASS_WST;
typedef PULONG_PTR PULONG_PTR_WST;
typedef PTOKEN_PRIVILEGES PTOKEN_PRIVILEGES_WST;
typedef TIMER_TYPE TIMER_TYPE_WST;
typedef PROCESSINFOCLASS PROCESSINFOCLASS_WST;
typedef SECTION_INHERIT SECTION_INHERIT_WST;
// typedef HANDLE HANDLE_WST;
// typedef PCLIENT_ID PCLIENT_ID_WST;
typedef SYSTEM_INFORMATION_CLASS SYSTEM_INFORMATION_CLASS_WST;
typedef PTIMER_APC_ROUTINE PTIMER_APC_ROUTINE_WST;
typedef PORT_INFORMATION_CLASS PORT_INFORMATION_CLASS_WST;
typedef PTOKEN_OWNER PTOKEN_OWNER_WST;
typedef KEY_VALUE_INFORMATION_CLASS KEY_VALUE_INFORMATION_CLASS_WST;
typedef MEMORY_INFORMATION_CLASS MEMORY_INFORMATION_CLASS_WST;
typedef PPORT_VIEW PPORT_VIEW_WST;
typedef AUDIT_EVENT_TYPE AUDIT_EVENT_TYPE_WST;
typedef BOOLEAN BOOLEAN_WST;
typedef EXECUTION_STATE EXECUTION_STATE_WST;
typedef PKNORMAL_ROUTINE PKNORMAL_ROUTINE_WST;
typedef FILE_INFORMATION_CLASS FILE_INFORMATION_CLASS_WST;
typedef PTOKEN_GROUPS PTOKEN_GROUPS_WST;
typedef PSAFEBOOT_MODE PSAFEBOOT_MODE_WST;
typedef TIMER_INFORMATION_CLASS TIMER_INFORMATION_CLASS_WST;
typedef PGENERIC_MAPPING PGENERIC_MAPPING_WST;
typedef IO_COMPLETION_INFORMATION_CLASS IO_COMPLETION_INFORMATION_CLASS_WST;
typedef LCID LCID_WST;
typedef HARDERROR_RESPONSE_OPTION HARDERROR_RESPONSE_OPTION_WST;
typedef PTOKEN_DEFAULT_DACL PTOKEN_DEFAULT_DACL_WST;
typedef THREADINFOCLASS THREADINFOCLASS_WST;
typedef KPROFILE_SOURCE KPROFILE_SOURCE_WST;
typedef PPRIVILEGE_SET PPRIVILEGE_SET_WST;
// typedef PIO_STATUS_BLOCK PIO_STATUS_BLOCK_WST;
typedef EVENT_INFORMATION_CLASS EVENT_INFORMATION_CLASS_WST;
typedef PACCESS_MASK PACCESS_MASK_WST;
// typedef POBJECT_ATTRIBUTES POBJECT_ATTRIBUTES_WST;
typedef KEY_SET_INFORMATION_CLASS KEY_SET_INFORMATION_CLASS_WST;
typedef PWSTR PWSTR_WST;
typedef LANGID LANGID_WST;
typedef MUTANT_INFORMATION_CLASS MUTANT_INFORMATION_CLASS_WST;
typedef TOKEN_TYPE TOKEN_TYPE_WST;
// typedef PPORT_MESSAGE PPORT_MESSAGE_WST;
typedef PEVENT_TRACE_HEADER PEVENT_TRACE_HEADER_WST;
typedef PFILE_GET_EA_INFORMATION PFILE_GET_EA_INFORMATION_WST;
// typedef PVOID PVOID_WST;

typedef FILETIME FILETIME_WST;
typedef DWORD HANDLE_WST, *PHANDLE_WST;
typedef UCHAR UCHAR_WST;
typedef DWORD DWORD_WST;
typedef ULONG_PTR ULONG_PTR_WST;
typedef DWORD *PLANGID_WST;

typedef struct _PVOID_WST {
  void *pvoid;
} PVOID_WST, PVOID_WST_SIZEBEFORE, PVOID_WST_SIZEAFTER, *PPVOID_WST, LPVOID_WST;

typedef struct _UNICODE_STRING_WST {
  USHORT_WST  Length;
  USHORT_WST  MaximumLength;
  PWSTR_WST  Buffer;
} UNICODE_STRING_WST, *PUNICODE_STRING_WST;

typedef PISID PISID_WST;
typedef PACL PACL_WST;
typedef SECURITY_DESCRIPTOR_CONTROL SECURITY_DESCRIPTOR_CONTROL_WST;
typedef struct _SECURITY_DESCRIPTOR_WST {
  UCHAR_WST  Revision;
  UCHAR_WST  Sbz1;
  SECURITY_DESCRIPTOR_CONTROL_WST  Control;
  PISID_WST  Owner;
  PISID_WST  Group;
  PACL_WST  Sacl;
  PACL_WST  Dacl;
} SECURITY_DESCRIPTOR_WST, *PSECURITY_DESCRIPTOR_WST;

typedef struct _OBJECT_ATTRIBUTES_WST {
  ULONG_WST  Length;
  HANDLE_WST  RootDirectory;
  PUNICODE_STRING_WST  ObjectName;
  ULONG_WST  Attributes;
  PSECURITY_DESCRIPTOR_WST SecurityDescriptor;
  PSECURITY_QUALITY_OF_SERVICE_WST SecurityQualityOfService;
} OBJECT_ATTRIBUTES_WST, *POBJECT_ATTRIBUTES_WST;

typedef struct _IO_STATUS_BLOCK_WST {
  union {
    NTSTATUS_WST Status;
    ULONG_WST Pointer;
    // PVOID Pointer;
  };
  ULONG_PTR_WST Information;
} IO_STATUS_BLOCK_WST, *PIO_STATUS_BLOCK_WST;

// Redefinition without union (because we cannot serialize directly a union)
typedef struct _LARGE_INTEGER_WST {
  DWORD_WST LowPart;
  DWORD_WST HighPart;
} LARGE_INTEGER_WST, *PLARGE_INTEGER_WST;

typedef struct _ULARGE_INTEGER_WST {
  DWORD_WST LowPart;
  DWORD_WST HighPart;
} ULARGE_INTEGER_WST, *PULARGE_INTEGER_WST;

typedef struct _CLIENT_ID_WST {
  HANDLE_WST UniqueProcess;
  HANDLE_WST UniqueThread;
} CLIENT_ID_WST, *PCLIENT_ID_WST;

typedef struct _PORT_MESSAGE_HEADER_WST {
  USHORT_WST DataSize;
  USHORT_WST MessageSize;
  USHORT_WST MessageType;
  USHORT_WST VirtualRangesOffset;
  CLIENT_ID_WST ClientId;
  ULONG_WST MessageId;
  ULONG_WST SectionSize;
  PUCHAR_WST Data;
} PORT_MESSAGE_HEADER_WST, *PPORT_MESSAGE_HEADER_WST, PORT_MESSAGE_WST, *PPORT_MESSAGE_WST;

typedef struct _FILE_BASIC_INFORMATION_WST {
  LARGE_INTEGER_WST  CreationTime;
  LARGE_INTEGER_WST  LastAccessTime;
  LARGE_INTEGER_WST  LastWriteTime;
  LARGE_INTEGER_WST  ChangeTime;
  ULONG_WST  FileAttributes;
} FILE_BASIC_INFORMATION_WST, *PFILE_BASIC_INFORMATION_WST;

typedef struct _THREAD_BASIC_INFORMATION_WST {
  NTSTATUS_WST  ExitStatus;
  PVOID_WST     TebBaseAddress;
  CLIENT_ID_WST ClientId;
  ULONG_WST     AffinityMask;
  LONG_WST      Priority;
  LONG_WST      BasePriority;
} THREAD_BASIC_INFORMATION_WST, *PTHREAD_BASIC_INFORMATION_WST;

typedef SECURITY_QUALITY_OF_SERVICE SECURITY_QUALITY_OF_SERVICE_WST;
typedef SID_IDENTIFIER_AUTHORITY SID_IDENTIFIER_AUTHORITY_WST;
typedef CONTEXT CONTEXT_WST;
typedef SID SID_WST;
typedef ACL ACL_WST;

#endif
