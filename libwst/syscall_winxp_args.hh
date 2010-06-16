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

// This is the subset of argument that we currently handle. In the future we
// will use directly MagicSyscallArgType
#ifndef MagicSyscallArgTypeObj
#define MagicSyscallArgTypeObj(x)
#endif
#ifndef MagicSyscallArgTypePtr
#define MagicSyscallArgTypePtr(x)
#endif
#ifndef MagicSyscallArgTypeBlob
#define MagicSyscallArgTypeBlob(x, y)
#endif

// MagicSyscallArgTypeObj(UINT_WST)
// MagicSyscallArgTypePtr(PUINT_WST)
// MagicSyscallArgTypePtr(PINT_WST)
// MagicSyscallArgTypeObj(INT_WST)

MagicSyscallArgTypeObj(LONG_WST)
MagicSyscallArgTypePtr(PLONG_WST)
MagicSyscallArgTypeObj(ULONG_WST)
MagicSyscallArgTypePtr(PULONG_WST)
// MagicSyscallArgTypeObj(ULONG_PTR_WST)
MagicSyscallArgTypePtr(PULONG_PTR_WST)
// MagicSyscallArgTypeObj(FLONG_WST)
MagicSyscallArgTypePtr(PLCID_WST)

MagicSyscallArgTypeObj(THREADINFOCLASS_WST)
MagicSyscallArgTypeObj(MEMORY_INFORMATION_CLASS_WST)

// MagicSyscallArgTypeObj(DWORD_WST)
// MagicSyscallArgTypePtr(PDWORD_WST)
// MagicSyscallArgTypePtr(LPDWORD_WST)

// MagicSyscallArgTypeObj(BOOL_WST);
// MagicSyscallArgTypeObj(BOOLEAN_WST);
// MagicSyscallArgTypePtr(PBOOL_WST);

MagicSyscallArgTypePtr(PLARGE_INTEGER_WST)
MagicSyscallArgTypePtr(PULARGE_INTEGER_WST)

MagicSyscallArgTypeObj(USHORT_WST);
MagicSyscallArgTypePtr(PUSHORT_WST);

MagicSyscallArgTypeObj(ACCESS_MASK_WST)
MagicSyscallArgTypePtr(PUNICODE_STRING_WST)
// MagicSyscallArgTypeObj(LFTYPE_WST)
MagicSyscallArgTypeObj(SECURITY_INFORMATION_WST)
MagicSyscallArgTypeObj(SYSTEM_INFORMATION_CLASS_WST)
MagicSyscallArgTypePtr(PIO_STATUS_BLOCK_WST)
MagicSyscallArgTypePtr(PACCESS_MASK_WST)
MagicSyscallArgTypePtr(POBJECT_ATTRIBUTES_WST)
MagicSyscallArgTypeObj(FS_INFORMATION_CLASS_WST)
MagicSyscallArgTypeObj(PROCESSINFOCLASS_WST)
MagicSyscallArgTypeObj(EVENT_TYPE_WST)
MagicSyscallArgTypeObj(KEY_VALUE_INFORMATION_CLASS_WST)
MagicSyscallArgTypeObj(FILE_INFORMATION_CLASS_WST)
MagicSyscallArgTypeObj(SECTION_INHERIT_WST)
MagicSyscallArgTypeObj(OBJECT_INFORMATION_CLASS_WST)
MagicSyscallArgTypeObj(KEY_INFORMATION_CLASS_WST)
MagicSyscallArgTypeObj(SECTION_INFORMATION_CLASS_WST)
MagicSyscallArgTypePtr(PPORT_MESSAGE_WST)
MagicSyscallArgTypePtr(PCONTEXT_WST)
MagicSyscallArgTypeObj(TOKEN_INFORMATION_CLASS_WST)
MagicSyscallArgTypePtr(PFILE_BASIC_INFORMATION_WST)
// MagicSyscallArgTypeObj(PLANGID_WST)
MagicSyscallArgTypePtr(PCLIENT_ID_WST)
MagicSyscallArgTypePtr(PSECURITY_QUALITY_OF_SERVICE_WST)
MagicSyscallArgTypePtr(PWSTR_WST)

// Return value
MagicSyscallArgTypeObj(NTSTATUS_WST)

// // Handles
MagicSyscallArgTypeObj(HANDLE_WST)
MagicSyscallArgTypePtr(PHANDLE_WST)

// // MagicSyscallArgTypeBlob(PENUMLOGFONTEXDVW, ENUMLOGFONTEXDVW_WST)

// // These types are handled manually
// // PVOID2
// // PWSTR

// // MagicSyscallArgTypePtr(PIO_APC_ROUTINE_WST)

#undef MagicSyscallArgTypeObj
#undef MagicSyscallArgTypePtr
#undef MagicSyscallArgTypeBlob



// #if 0
// // This is the subset of argument that we currently handle. In the future we
// // will use directly MagicSyscallArgType
// #ifndef MagicSyscallArgTypeObj
// #define MagicSyscallArgTypeObj(x)
// #endif
// #ifndef MagicSyscallArgTypePtr
// #define MagicSyscallArgTypePtr(x)
// #endif
// #ifndef MagicSyscallArgTypeBlob
// #define MagicSyscallArgTypeBlob(x, y)
// #endif

// // MagicSyscallArgTypePtr(PTOKEN_USER_WST)
// // MagicSyscallArgTypePtr(PLCID_WST)
// // MagicSyscallArgTypeObj(FS_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypeObj(EVENT_TYPE_WST)
// // MagicSyscallArgTypeObj(SECTION_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypePtr(PSECURITY_QUALITY_OF_SERVICE_WST)
// // MagicSyscallArgTypeObj(LONG_WST)
// // MagicSyscallArgTypePtr(PFILE_USER_QUOTA_INFORMATION_WST)
// // MagicSyscallArgTypeObj(POWER_INFORMATION_LEVEL_WST)
// // MagicSyscallArgTypePtr(PUSHORT_WST)
// // MagicSyscallArgTypePtr(PUCHAR_WST)
// // MagicSyscallArgTypePtr(PLARGE_INTEGER_WST)
// // MagicSyscallArgTypePtr(PREMOTE_PORT_VIEW_WST)
// // MagicSyscallArgTypeObj(SHUTDOWN_ACTION_WST)
// // MagicSyscallArgTypeObj(PEXECUTION_STATE_WST)
// // MagicSyscallArgTypeObj(ACCESS_MASK_WST)
// // MagicSyscallArgTypeObj(DEBUG_CONTROL_CODE_WST)
// // MagicSyscallArgTypePtr(PBOOT_OPTIONS_WST)
// // MagicSyscallArgTypeObj(SEMAPHORE_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypeObj(POBJECT_TYPE_LIST_WST)
// // MagicSyscallArgTypeObj(WAIT_TYPE_WST)
// // MagicSyscallArgTypePtr(PUNICODE_STRING_WST)
// // MagicSyscallArgTypePtr(PCONTEXT_WST)
// // MagicSyscallArgTypePtr(PFILE_NOTIFY_INFORMATION_WST)
// // MagicSyscallArgTypeObj(POWER_ACTION_WST)
// // MagicSyscallArgTypePtr(PLONG_WST)
// // MagicSyscallArgTypeObj(USHORT_WST)
// // MagicSyscallArgTypePtr(PEXCEPTION_RECORD_WST)
// // MagicSyscallArgTypePtr(PFILE_FULL_EA_INFORMATION_WST)
// // MagicSyscallArgTypeObj(VDMSERVICECLASS_WST)
// // MagicSyscallArgTypePtr(PHARDERROR_RESPONSE_WST)
// // MagicSyscallArgTypePtr(PFILE_NETWORK_OPEN_INFORMATION_WST)
// // MagicSyscallArgTypePtr(PTOKEN_SOURCE_WST)
// // MagicSyscallArgTypePtr(PFILE_SEGMENT_ELEMENT_WST)
// // MagicSyscallArgTypeObj(ULONG_WST)
// // MagicSyscallArgTypePtr(PIO_APC_ROUTINE_WST)
// // MagicSyscallArgTypePtr(PULARGE_INTEGER_WST)
// // MagicSyscallArgTypeObj(PLANGID_WST)
// // MagicSyscallArgTypePtr(PSECURITY_DESCRIPTOR_WST)
// // MagicSyscallArgTypePtr(PLUID_WST)
// // MagicSyscallArgTypePtr(PFILE_BASIC_INFORMATION_WST)
// // MagicSyscallArgTypePtr(PFILE_PATH_WST)
// // MagicSyscallArgTypePtr(PUSER_STACK_WST)
// // MagicSyscallArgTypeObj(LDT_ENTRY_WST)
// // MagicSyscallArgTypePtr(PFILE_QUOTA_LIST_INFORMATION_WST)
// // MagicSyscallArgTypeObj(PSID_WST)
// // MagicSyscallArgTypePtr(PKEY_VALUE_ENTRY_WST)
// // MagicSyscallArgTypePtr(PTOKEN_PRIMARY_GROUP_WST)
// // MagicSyscallArgTypePtr(PDEVICE_POWER_STATE_WST)
// // MagicSyscallArgTypeObj(TOKEN_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypePtr(PBOOLEAN_WST)
// // MagicSyscallArgTypeObj(ATOM_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypeObj(KEY_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypePtr(PULONG_WST)
// // MagicSyscallArgTypeObj(JOBOBJECTINFOCLASS_WST)
// // MagicSyscallArgTypeObj(SYSTEM_POWER_STATE_WST)
// // MagicSyscallArgTypeObj(LATENCY_TIME_WST)
// // MagicSyscallArgTypePtr(PDBGUI_WAIT_STATE_CHANGE_WST)
// // MagicSyscallArgTypePtr(PJOB_SET_ARRAY_WST)
// // MagicSyscallArgTypeObj(NTSTATUS_WST)
// // MagicSyscallArgTypeObj(SECURITY_INFORMATION_WST)
// // MagicSyscallArgTypeObj(DEBUGOBJECTINFOCLASS_WST)
// // MagicSyscallArgTypeObj(OBJECT_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypePtr(PULONG_PTR_WST)
// // MagicSyscallArgTypePtr(PTOKEN_PRIVILEGES_WST)
// // MagicSyscallArgTypeObj(TIMER_TYPE_WST)
// // MagicSyscallArgTypeObj(PROCESSINFOCLASS_WST)
// // MagicSyscallArgTypeObj(SECTION_INHERIT_WST)
// // MagicSyscallArgTypeObj(HANDLE_WST)
// // MagicSyscallArgTypePtr(PCLIENT_ID_WST)
// // MagicSyscallArgTypeObj(SYSTEM_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypePtr(PTIMER_APC_ROUTINE_WST)
// // MagicSyscallArgTypeObj(PORT_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypePtr(PTOKEN_OWNER_WST)
// // MagicSyscallArgTypeObj(KEY_VALUE_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypeObj(MEMORY_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypePtr(PPORT_VIEW_WST)
// // MagicSyscallArgTypeObj(AUDIT_EVENT_TYPE_WST)
// // MagicSyscallArgTypeObj(BOOLEAN_WST)
// // MagicSyscallArgTypeObj(EXECUTION_STATE_WST)
// // MagicSyscallArgTypePtr(PKNORMAL_ROUTINE_WST)
// // MagicSyscallArgTypeObj(FILE_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypePtr(PHANDLE_WST)
// // MagicSyscallArgTypePtr(PTOKEN_GROUPS_WST)
// // MagicSyscallArgTypePtr(PSAFEBOOT_MODE_WST)
// // MagicSyscallArgTypeObj(TIMER_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypePtr(PGENERIC_MAPPING_WST)
// // MagicSyscallArgTypeObj(IO_COMPLETION_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypeObj(LCID_WST)
// // MagicSyscallArgTypeObj(HARDERROR_RESPONSE_OPTION_WST)
// // MagicSyscallArgTypePtr(PTOKEN_DEFAULT_DACL_WST)
// // MagicSyscallArgTypeObj(THREADINFOCLASS_WST)
// // MagicSyscallArgTypeObj(KPROFILE_SOURCE_WST)
// // MagicSyscallArgTypePtr(PPRIVILEGE_SET_WST)
// MagicSyscallArgTypePtr(PIO_STATUS_BLOCK_WST)
// // MagicSyscallArgTypeObj(EVENT_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypePtr(PACCESS_MASK_WST)
// // MagicSyscallArgTypePtr(POBJECT_ATTRIBUTES_WST)
// // MagicSyscallArgTypeObj(KEY_SET_INFORMATION_CLASS_WST)
// MagicSyscallArgTypePtr(PWSTR_WST)
// // MagicSyscallArgTypeObj(LANGID_WST)
// // MagicSyscallArgTypeObj(MUTANT_INFORMATION_CLASS_WST)
// // MagicSyscallArgTypeObj(TOKEN_TYPE_WST)
// // MagicSyscallArgTypePtr(PPORT_MESSAGE_WST)
// // MagicSyscallArgTypePtr(PEVENT_TRACE_HEADER_WST)
// // MagicSyscallArgTypePtr(PFILE_GET_EA_INFORMATION_WST)
// // MagicSyscallArgTypeObj(PVOID_WST)

// #undef MagicSyscallArgTypeObj
// #undef MagicSyscallArgTypePtr
// #undef MagicSyscallArgTypeBlob

// #endif
