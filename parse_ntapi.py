#!/usr/bin/python

# Copyright notice
# ================
# 
# Copyright (C) 2010
#     Lorenzo Martignoni <martignlo@gmail.com>
#     Roberto Paleari <roberto.paleari@gmail.com>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.  
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

import sys
import pickle

t = set()

ss = {}

# This is the hash list of syscalls
sl = {
    "NtAcceptConnectPort": 0x0,
    "NtAccessCheck": 0x1,
    "NtAccessCheckAndAuditAlarm": 0x2,
    "NtAccessCheckByType": 0x3,
    "NtAccessCheckByTypeAndAuditAlarm": 0x4,
    "NtAccessCheckByTypeResultList": 0x5,
    "NtAccessCheckByTypeResultListAndAuditAlarm": 0x6,
    "NtAccessCheckByTypeResultListAndAuditAlarmByHandle": 0x7,
    "NtAddAtom": 0x8,
    "NtAddBootEntry": 0x9,
    "NtAdjustGroupsToken": 0xa,
    "NtAdjustPrivilegesToken": 0xb,
    "NtAlertResumeThread": 0xc,
    "NtAlertThread": 0xd,
    "NtAllocateLocallyUniqueId": 0xe,
    "NtAllocateUserPhysicalPages": 0xf,
    "NtAllocateUuids": 0x10,
    "NtAllocateVirtualMemory": 0x11,
    "NtAreMappedFilesTheSame": 0x12,
    "NtAssignProcessToJobObject": 0x13,
    "NtCallbackReturn": 0x14,
    "NtCancelDeviceWakeupRequest": 0x15,
    "NtCancelIoFile": 0x16,
    "NtCancelTimer": 0x17,
    "NtClearEvent": 0x18,
    "NtClose": 0x19,
    "NtCloseObjectAuditAlarm": 0x1a,
    "NtCompactKeys": 0x1b,
    "NtCompareTokens": 0x1c,
    "NtCompleteConnectPort": 0x1d,
    "NtCompressKey": 0x1e,
    "NtConnectPort": 0x1f,
    "NtContinue": 0x20,
    "NtCreateDebugObject": 0x21,
    "NtCreateDirectoryObject": 0x22,
    "NtCreateEvent": 0x23,
    "NtCreateEventPair": 0x24,
    "NtCreateFile": 0x25,
    "NtCreateIoCompletion": 0x26,
    "NtCreateJobObject": 0x27,
    "NtCreateJobSet": 0x28,
    "NtCreateKey": 0x29,
    "NtCreateMailslotFile": 0x2a,
    "NtCreateMutant": 0x2b,
    "NtCreateNamedPipeFile": 0x2c,
    "NtCreatePagingFile": 0x2d,
    "NtCreatePort": 0x2e,
    "NtCreateProcess": 0x2f,
    "NtCreateProcessEx": 0x30,
    "NtCreateProfile": 0x31,
    "NtCreateSection": 0x32,
    "NtCreateSemaphore": 0x33,
    "NtCreateSymbolicLinkObject": 0x34,
    "NtCreateThread": 0x35,
    "NtCreateTimer": 0x36,
    "NtCreateToken": 0x37,
    "NtCreateWaitablePort": 0x38,
    "NtDebugActiveProcess": 0x39,
    "NtDebugContinue": 0x3a,
    "NtDelayExecution": 0x3b,
    "NtDeleteAtom": 0x3c,
    "NtDeleteBootEntry": 0x3d,
    "NtDeleteFile": 0x3e,
    "NtDeleteKey": 0x3f,
    "NtDeleteObjectAuditAlarm": 0x40,
    "NtDeleteValueKey": 0x41,
    "NtDeviceIoControlFile": 0x42,
    "NtDisplayString": 0x43,
    "NtDuplicateObject": 0x44,
    "NtDuplicateToken": 0x45,
    "NtEnumerateBootEntries": 0x46,
    "NtEnumerateKey": 0x47,
    "NtEnumerateSystemEnvironmentValuesEx": 0x48,
    "NtEnumerateValueKey": 0x49,
    "NtExtendSection": 0x4a,
    "NtFilterToken": 0x4b,
    "NtFindAtom": 0x4c,
    "NtFlushBuffersFile": 0x4d,
    "NtFlushInstructionCache": 0x4e,
    "NtFlushKey": 0x4f,
    "NtFlushVirtualMemory": 0x50,
    "NtFlushWriteBuffer": 0x51,
    "NtFreeUserPhysicalPages": 0x52,
    "NtFreeVirtualMemory": 0x53,
    "NtFsControlFile": 0x54,
    "NtGetContextThread": 0x55,
    "NtGetDevicePowerState": 0x56,
    "NtGetPlugPlayEvent": 0x57,
    "NtGetWriteWatch": 0x58,
    "NtImpersonateAnonymousToken": 0x59,
    "NtImpersonateClientOfPort": 0x5a,
    "NtImpersonateThread": 0x5b,
    "NtInitializeRegistry": 0x5c,
    "NtInitiatePowerAction": 0x5d,
    "NtIsProcessInJob": 0x5e,
    "NtIsSystemResumeAutomatic": 0x5f,
    "NtListenPort": 0x60,
    "NtLoadDriver": 0x61,
    "NtLoadKey": 0x62,
    "NtLoadKey2": 0x63,
    "NtLockFile": 0x64,
    "NtLockProductActivationKeys": 0x65,
    "NtLockRegistryKey": 0x66,
    "NtLockVirtualMemory": 0x67,
    "NtMakePermanentObject": 0x68,
    "NtMakeTemporaryObject": 0x69,
    "NtMapUserPhysicalPages": 0x6a,
    "NtMapUserPhysicalPagesScatter": 0x6b,
    "NtMapViewOfSection": 0x6c,
    "NtModifyBootEntry": 0x6d,
    "NtNotifyChangeDirectoryFile": 0x6e,
    "NtNotifyChangeKey": 0x6f,
    "NtNotifyChangeMultipleKeys": 0x70,
    "NtOpenDirectoryObject": 0x71,
    "NtOpenEvent": 0x72,
    "NtOpenEventPair": 0x73,
    "NtOpenFile": 0x74,
    "NtOpenIoCompletion": 0x75,
    "NtOpenJobObject": 0x76,
    "NtOpenKey": 0x77,
    "NtOpenMutant": 0x78,
    "NtOpenObjectAuditAlarm": 0x79,
    "NtOpenProcess": 0x7a,
    "NtOpenProcessToken": 0x7b,
    "NtOpenProcessTokenEx": 0x7c,
    "NtOpenSection": 0x7d,
    "NtOpenSemaphore": 0x7e,
    "NtOpenSymbolicLinkObject": 0x7f,
    "NtOpenThread": 0x80,
    "NtOpenThreadToken": 0x81,
    "NtOpenThreadTokenEx": 0x82,
    "NtOpenTimer": 0x83,
    "NtPlugPlayControl": 0x84,
    "NtPowerInformation": 0x85,
    "NtPrivilegeCheck": 0x86,
    "NtPrivilegeObjectAuditAlarm": 0x87,
    "NtPrivilegedServiceAuditAlarm": 0x88,
    "NtProtectVirtualMemory": 0x89,
    "NtPulseEvent": 0x8a,
    "NtQueryAttributesFile": 0x8b,
    "NtQueryBootEntryOrder": 0x8c,
    "NtQueryBootOptions": 0x8d,
    "NtQueryDebugFilterState": 0x8e,
    "NtQueryDefaultLocale": 0x8f,
    "NtQueryDefaultUILanguage": 0x90,
    "NtQueryDirectoryFile": 0x91,
    "NtQueryDirectoryObject": 0x92,
    "NtQueryEaFile": 0x93,
    "NtQueryEvent": 0x94,
    "NtQueryFullAttributesFile": 0x95,
    "NtQueryInformationAtom": 0x96,
    "NtQueryInformationFile": 0x97,
    "NtQueryInformationJobObject": 0x98,
    "NtQueryInformationPort": 0x99,
    "NtQueryInformationProcess": 0x9a,
    "NtQueryInformationThread": 0x9b,
    "NtQueryInformationToken": 0x9c,
    "NtQueryInstallUILanguage": 0x9d,
    "NtQueryIntervalProfile": 0x9e,
    "NtQueryIoCompletion": 0x9f,
    "NtQueryKey": 0xa0,
    "NtQueryMultipleValueKey": 0xa1,
    "NtQueryMutant": 0xa2,
    "NtQueryObject": 0xa3,
    "NtQueryOpenSubKeys": 0xa4,
    "NtQueryPerformanceCounter": 0xa5,
    "NtQueryQuotaInformationFile": 0xa6,
    "NtQuerySection": 0xa7,
    "NtQuerySecurityObject": 0xa8,
    "NtQuerySemaphore": 0xa9,
    "NtQuerySymbolicLinkObject": 0xaa,
    "NtQuerySystemEnvironmentValue": 0xab,
    "NtQuerySystemEnvironmentValueEx": 0xac,
    "NtQuerySystemInformation": 0xad,
    "NtQuerySystemTime": 0xae,
    "NtQueryTimer": 0xaf,
    "NtQueryTimerResolution": 0xb0,
    "NtQueryValueKey": 0xb1,
    "NtQueryVirtualMemory": 0xb2,
    "NtQueryVolumeInformationFile": 0xb3,
    "NtQueueApcThread": 0xb4,
    "NtRaiseException": 0xb5,
    "NtRaiseHardError": 0xb6,
    "NtReadFile": 0xb7,
    "NtReadFileScatter": 0xb8,
    "NtReadRequestData": 0xb9,
    "NtReadVirtualMemory": 0xba,
    "NtRegisterThreadTerminatePort": 0xbb,
    "NtReleaseMutant": 0xbc,
    "NtReleaseSemaphore": 0xbd,
    "NtRemoveIoCompletion": 0xbe,
    "NtRemoveProcessDebug": 0xbf,
    "NtRenameKey": 0xc0,
    "NtReplaceKey": 0xc1,
    "NtReplyPort": 0xc2,
    "NtReplyWaitReceivePort": 0xc3,
    "NtReplyWaitReceivePortEx": 0xc4,
    "NtReplyWaitReplyPort": 0xc5,
    "NtRequestDeviceWakeup": 0xc6,
    "NtRequestPort": 0xc7,
    "NtRequestWaitReplyPort": 0xc8,
    "NtRequestWakeupLatency": 0xc9,
    "NtResetEvent": 0xca,
    "NtResetWriteWatch": 0xcb,
    "NtRestoreKey": 0xcc,
    "NtResumeProcess": 0xcd,
    "NtResumeThread": 0xce,
    "NtSaveKey": 0xcf,
    "NtSaveKeyEx": 0xd0,
    "NtSaveMergedKeys": 0xd1,
    "NtSecureConnectPort": 0xd2,
    "NtSetBootEntryOrder": 0xd3,
    "NtSetBootOptions": 0xd4,
    "NtSetContextThread": 0xd5,
    "NtSetDebugFilterState": 0xd6,
    "NtSetDefaultHardErrorPort": 0xd7,
    "NtSetDefaultLocale": 0xd8,
    "NtSetDefaultUILanguage": 0xd9,
    "NtSetEaFile": 0xda,
    "NtSetEvent": 0xdb,
    "NtSetEventBoostPriority": 0xdc,
    "NtSetHighEventPair": 0xdd,
    "NtSetHighWaitLowEventPair": 0xde,
    "NtSetInformationDebugObject": 0xdf,
    "NtSetInformationFile": 0xe0,
    "NtSetInformationJobObject": 0xe1,
    "NtSetInformationKey": 0xe2,
    "NtSetInformationObject": 0xe3,
    "NtSetInformationProcess": 0xe4,
    "NtSetInformationThread": 0xe5,
    "NtSetInformationToken": 0xe6,
    "NtSetIntervalProfile": 0xe7,
    "NtSetIoCompletion": 0xe8,
    "NtSetLdtEntries": 0xe9,
    "NtSetLowEventPair": 0xea,
    "NtSetLowWaitHighEventPair": 0xeb,
    "NtSetQuotaInformationFile": 0xec,
    "NtSetSecurityObject": 0xed,
    "NtSetSystemEnvironmentValue": 0xee,
    "NtSetSystemEnvironmentValueEx": 0xef,
    "NtSetSystemInformation": 0xf0,
    "NtSetSystemPowerState": 0xf1,
    "NtSetSystemTime": 0xf2,
    "NtSetThreadExecutionState": 0xf3,
    "NtSetTimer": 0xf4,
    "NtSetTimerResolution": 0xf5,
    "NtSetUuidSeed": 0xf6,
    "NtSetValueKey": 0xf7,
    "NtSetVolumeInformationFile": 0xf8,
    "NtShutdownSystem": 0xf9,
    "NtSignalAndWaitForSingleObject": 0xfa,
    "NtStartProfile": 0xfb,
    "NtStopProfile": 0xfc,
    "NtSuspendProcess": 0xfd,
    "NtSuspendThread": 0xfe,
    "NtSystemDebugControl": 0xff,
    "NtTerminateJobObject": 0x100,
    "NtTerminateProcess": 0x101,
    "NtTerminateThread": 0x102,
    "NtTestAlert": 0x103,
    "NtTraceEvent": 0x104,
    "NtTranslateFilePath": 0x105,
    "NtUnloadDriver": 0x106,
    "NtUnloadKey": 0x107,
    "NtUnloadKeyEx": 0x108,
    "NtUnlockFile": 0x109,
    "NtUnlockVirtualMemory": 0x10a,
    "NtUnmapViewOfSection": 0x10b,
    "NtVdmControl": 0x10c,
    "NtWaitForDebugEvent": 0x10d,
    "NtWaitForMultipleObjects": 0x10e,
    "NtWaitForSingleObject": 0x10f,
    "NtWaitHighEventPair": 0x110,
    "NtWaitLowEventPair": 0x111,
    "NtWriteFile": 0x112,
    "NtWriteFileGather": 0x113,
    "NtWriteRequestData": 0x114,
    "NtWriteVirtualMemory": 0x115,
    "NtYieldExecution": 0x116,
    "NtCreateKeyedEvent": 0x117,
    "NtOpenKeyedEvent": 0x118,
    "NtReleaseKeyedEvent": 0x119,
    "NtWaitForKeyedEvent": 0x11a,
    "NtQueryPortInformationProcess": 0x11b,
    "NtGdiAbortDoc": 0x1000,
    "NtGdiAbortPath": 0x1001,
    "NtGdiAddFontResourceW": 0x1002,
    "NtGdiAddRemoteFontToDC": 0x1003,
    "NtGdiAddFontMemResourceEx": 0x1004,
    "NtGdiRemoveMergeFont": 0x1005,
    "NtGdiAddRemoteMMInstanceToDC": 0x1006,
    "NtGdiAlphaBlend": 0x1007,
    "NtGdiAngleArc": 0x1008,
    "NtGdiAnyLinkedFonts": 0x1009,
    "NtGdiFontIsLinked": 0x100A,
    "NtGdiArcInternal": 0x100B,
    "NtGdiBeginPath": 0x100C,
    "NtGdiBitBlt": 0x100D,
    "NtGdiCancelDC": 0x100E,
    "NtGdiCheckBitmapBits": 0x100F,
    "NtGdiCloseFigure": 0x1010,
    "NtGdiClearBitmapAttributes": 0x1011,
    "NtGdiClearBrushAttributes": 0x1012,
    "NtGdiColorCorrectPalette": 0x1013,
    "NtGdiCombineRgn": 0x1014,
    "NtGdiCombineTransform": 0x1015,
    "NtGdiComputeXformCoefficients": 0x1016,
    "NtGdiConsoleTextOut": 0x1017,
    "NtGdiConvertMetafileRect": 0x1018,
    "NtGdiCreateBitmap": 0x1019,
    "NtGdiCreateClientObj": 0x101A,
    "NtGdiCreateColorSpace": 0x101B,
    "NtGdiCreateColorTransform": 0x101C,
    "NtGdiCreateCompatibleBitmap": 0x101D,
    "NtGdiCreateCompatibleDC": 0x101E,
    "NtGdiCreateDIBBrush": 0x101F,
    "NtGdiCreateDIBitmapInternal": 0x1020,
    "NtGdiCreateDIBSection": 0x1021,
    "NtGdiCreateEllipticRgn": 0x1022,
    "NtGdiCreateHalftonePalette": 0x1023,
    "NtGdiCreateHatchBrushInternal": 0x1024,
    "NtGdiCreateMetafileDC": 0x1025,
    "NtGdiCreatePaletteInternal": 0x1026,
    "NtGdiCreatePatternBrushInternal": 0x1027,
    "NtGdiCreatePen": 0x1028,
    "NtGdiCreateRectRgn": 0x1029,
    "NtGdiCreateRoundRectRgn": 0x102A,
    "NtGdiCreateServerMetaFile": 0x102B,
    "NtGdiCreateSolidBrush": 0x102C,
    "NtGdiD3dContextCreate": 0x102D,
    "NtGdiD3dContextDestroy": 0x102E,
    "NtGdiD3dContextDestroyAll": 0x102F,
    "NtGdiD3dValidateTextureStageState": 0x1030,
    "NtGdiD3dDrawPrimitives2": 0x1031,
    "NtGdiDdGetDriverState": 0x1032,
    "NtGdiDdAddAttachedSurface": 0x1033,
    "NtGdiDdAlphaBlt": 0x1034,
    "NtGdiDdAttachSurface": 0x1035,
    "NtGdiDdBeginMoCompFrame": 0x1036,
    "NtGdiDdBlt": 0x1037,
    "NtGdiDdCanCreateSurface": 0x1038,
    "NtGdiDdCanCreateD3DBuffer": 0x1039,
    "NtGdiDdColorControl": 0x103A,
    "NtGdiDdCreateDirectDrawObject": 0x103B,
    "NtGdiDdCreateSurface": 0x103C,
    "NtGdiDdCreateD3DBuffer": 0x103D,
    "NtGdiDdCreateMoComp": 0x103E,
    "NtGdiDdCreateSurfaceObject": 0x103F,
    "NtGdiDdDeleteDirectDrawObject": 0x1040,
    "NtGdiDdDeleteSurfaceObject": 0x1041,
    "NtGdiDdDestroyMoComp": 0x1042,
    "NtGdiDdDestroySurface": 0x1043,
    "NtGdiDdDestroyD3DBuffer": 0x1044,
    "NtGdiDdEndMoCompFrame": 0x1045,
    "NtGdiDdFlip": 0x1046,
    "NtGdiDdFlipToGDISurface": 0x1047,
    "NtGdiDdGetAvailDriverMemory": 0x1048,
    "NtGdiDdGetBltStatus": 0x1049,
    "NtGdiDdGetDC": 0x104A,
    "NtGdiDdGetDriverInfo": 0x104B,
    "NtGdiDdGetDxHandle": 0x104C,
    "NtGdiDdGetFlipStatus": 0x104D,
    "NtGdiDdGetInternalMoCompInfo": 0x104E,
    "NtGdiDdGetMoCompBuffInfo": 0x104F,
    "NtGdiDdGetMoCompGuids": 0x1050,
    "NtGdiDdGetMoCompFormats": 0x1051,
    "NtGdiDdGetScanLine": 0x1052,
    "NtGdiDdLock": 0x1053,
    "NtGdiDdLockD3D": 0x1054,
    "NtGdiDdQueryDirectDrawObject": 0x1055,
    "NtGdiDdQueryMoCompStatus": 0x1056,
    "NtGdiDdReenableDirectDrawObject": 0x1057,
    "NtGdiDdReleaseDC": 0x1058,
    "NtGdiDdRenderMoComp": 0x1059,
    "NtGdiDdResetVisrgn": 0x105A,
    "NtGdiDdSetColorKey": 0x105B,
    "NtGdiDdSetExclusiveMode": 0x105C,
    "NtGdiDdSetGammaRamp": 0x105D,
    "NtGdiDdCreateSurfaceEx": 0x105E,
    "NtGdiDdSetOverlayPosition": 0x105F,
    "NtGdiDdUnattachSurface": 0x1060,
    "NtGdiDdUnlock": 0x1061,
    "NtGdiDdUnlockD3D": 0x1062,
    "NtGdiDdUpdateOverlay": 0x1063,
    "NtGdiDdWaitForVerticalBlank": 0x1064,
    "NtGdiDvpCanCreateVideoPort": 0x1065,
    "NtGdiDvpColorControl": 0x1066,
    "NtGdiDvpCreateVideoPort": 0x1067,
    "NtGdiDvpDestroyVideoPort": 0x1068,
    "NtGdiDvpFlipVideoPort": 0x1069,
    "NtGdiDvpGetVideoPortBandwidth": 0x106A,
    "NtGdiDvpGetVideoPortField": 0x106B,
    "NtGdiDvpGetVideoPortFlipStatus": 0x106C,
    "NtGdiDvpGetVideoPortInputFormats": 0x106D,
    "NtGdiDvpGetVideoPortLine": 0x106E,
    "NtGdiDvpGetVideoPortOutputFormats": 0x106F,
    "NtGdiDvpGetVideoPortConnectInfo": 0x1070,
    "NtGdiDvpGetVideoSignalStatus": 0x1071,
    "NtGdiDvpUpdateVideoPort": 0x1072,
    "NtGdiDvpWaitForVideoPortSync": 0x1073,
    "NtGdiDvpAcquireNotification": 0x1074,
    "NtGdiDvpReleaseNotification": 0x1075,
    "NtGdiDxgGenericThunk": 0x1076,
    "NtGdiDeleteClientObj": 0x1077,
    "NtGdiDeleteColorSpace": 0x1078,
    "NtGdiDeleteColorTransform": 0x1079,
    "NtGdiDeleteObjectApp": 0x107A,
    "NtGdiDescribePixelFormat": 0x107B,
    "NtGdiGetPerBandInfo": 0x107C,
    "NtGdiDoBanding": 0x107D,
    "NtGdiDoPalette": 0x107E,
    "NtGdiDrawEscape": 0x107F,
    "NtGdiEllipse": 0x1080,
    "NtGdiEnableEudc": 0x1081,
    "NtGdiEndDoc": 0x1082,
    "NtGdiEndPage": 0x1083,
    "NtGdiEndPath": 0x1084,
    "NtGdiEnumFontChunk": 0x1085,
    "NtGdiEnumFontClose": 0x1086,
    "NtGdiEnumFontOpen": 0x1087,
    "NtGdiEnumObjects": 0x1088,
    "NtGdiEqualRgn": 0x1089,
    "NtGdiEudcLoadUnloadLink": 0x108A,
    "NtGdiExcludeClipRect": 0x108B,
    "NtGdiExtCreatePen": 0x108C,
    "NtGdiExtCreateRegion": 0x108D,
    "NtGdiExtEscape": 0x108E,
    "NtGdiExtFloodFill": 0x108F,
    "NtGdiExtGetObjectW": 0x1090,
    "NtGdiExtSelectClipRgn": 0x1091,
    "NtGdiExtTextOutW": 0x1092,
    "NtGdiFillPath": 0x1093,
    "NtGdiFillRgn": 0x1094,
    "NtGdiFlattenPath": 0x1095,
    "NtGdiFlushUserBatch": 0x1096,
    "NtGdiFlush": 0x1097,
    "NtGdiForceUFIMapping": 0x1098,
    "NtGdiFrameRgn": 0x1099,
    "NtGdiFullscreenControl": 0x109A,
    "NtGdiGetAndSetDCDword": 0x109B,
    "NtGdiGetAppClipBox": 0x109C,
    "NtGdiGetBitmapBits": 0x109D,
    "NtGdiGetBitmapDimension": 0x109E,
    "NtGdiGetBoundsRect": 0x109F,
    "NtGdiGetCharABCWidthsW": 0x10A0,
    "NtGdiGetCharacterPlacementW": 0x10A1,
    "NtGdiGetCharSet": 0x10A2,
    "NtGdiGetCharWidthW": 0x10A3,
    "NtGdiGetCharWidthInfo": 0x10A4,
    "NtGdiGetColorAdjustment": 0x10A5,
    "NtGdiGetColorSpaceforBitmap": 0x10A6,
    "NtGdiGetDCDword": 0x10A7,
    "NtGdiGetDCforBitmap": 0x10A8,
    "NtGdiGetDCObject": 0x10A9,
    "NtGdiGetDCPoint": 0x10AA,
    "NtGdiGetDeviceCaps": 0x10AB,
    "NtGdiGetDeviceGammaRamp": 0x10AC,
    "NtGdiGetDeviceCapsAll": 0x10AD,
    "NtGdiGetDIBitsInternal": 0x10AE,
    "NtGdiGetETM": 0x10AF,
    "NtGdiGetEudcTimeStampEx": 0x10B0,
    "NtGdiGetFontData": 0x10B1,
    "NtGdiGetFontResourceInfoInternalW": 0x10B2,
    "NtGdiGetGlyphIndicesW": 0x10B3,
    "NtGdiGetGlyphIndicesWInternal": 0x10B4,
    "NtGdiGetGlyphOutline": 0x10B5,
    "NtGdiGetKerningPairs": 0x10B6,
    "NtGdiGetLinkedUFIs": 0x10B7,
    "NtGdiGetMiterLimit": 0x10B8,
    "NtGdiGetMonitorID": 0x10B9,
    "NtGdiGetNearestColor": 0x10BA,
    "NtGdiGetNearestPaletteIndex": 0x10BB,
    "NtGdiGetObjectBitmapHandle": 0x10BC,
    "NtGdiGetOutlineTextMetricsInternalW": 0x10BD,
    "NtGdiGetPath": 0x10BE,
    "NtGdiGetPixel": 0x10BF,
    "NtGdiGetRandomRgn": 0x10C0,
    "NtGdiGetRasterizerCaps": 0x10C1,
    "NtGdiGetRealizationInfo": 0x10C2,
    "NtGdiGetRegionData": 0x10C3,
    "NtGdiGetRgnBox": 0x10C4,
    "NtGdiGetServerMetaFileBits": 0x10C5,
    "NtGdiGetSpoolMessage": 0x10C6,
    "NtGdiGetStats": 0x10C7,
    "NtGdiGetStockObject": 0x10C8,
    "NtGdiGetStringBitmapW": 0x10C9,
    "NtGdiGetSystemPaletteUse": 0x10CA,
    "NtGdiGetTextCharsetInfo": 0x10CB,
    "NtGdiGetTextExtent": 0x10CC,
    "NtGdiGetTextExtentExW": 0x10CD,
    "NtGdiGetTextFaceW": 0x10CE,
    "NtGdiGetTextMetricsW": 0x10CF,
    "NtGdiGetTransform": 0x10D0,
    "NtGdiGetUFI": 0x10D1,
    "NtGdiGetEmbUFI": 0x10D2,
    "NtGdiGetUFIPathname": 0x10D3,
    "NtGdiGetEmbedFonts": 0x10D4,
    "NtGdiChangeGhostFont": 0x10D5,
    "NtGdiAddEmbFontToDC": 0x10D6,
    "NtGdiGetFontUnicodeRanges": 0x10D7,
    "NtGdiGetWidthTable": 0x10D8,
    "NtGdiGradientFill": 0x10D9,
    "NtGdiHfontCreate": 0x10DA,
    "NtGdiIcmBrushInfo": 0x10DB,
    "NtGdiInit": 0x10DC,
    "NtGdiInitSpool": 0x10DD,
    "NtGdiIntersectClipRect": 0x10DE,
    "NtGdiInvertRgn": 0x10DF,
    "NtGdiLineTo": 0x10E0,
    "NtGdiMakeFontDir": 0x10E1,
    "NtGdiMakeInfoDC": 0x10E2,
    "NtGdiMaskBlt": 0x10E3,
    "NtGdiModifyWorldTransform": 0x10E4,
    "NtGdiMonoBitmap": 0x10E5,
    "NtGdiMoveTo": 0x10E6,
    "NtGdiOffsetClipRgn": 0x10E7,
    "NtGdiOffsetRgn": 0x10E8,
    "NtGdiOpenDCW": 0x10E9,
    "NtGdiPatBlt": 0x10EA,
    "NtGdiPolyPatBlt": 0x10EB,
    "NtGdiPathToRegion": 0x10EC,
    "NtGdiPlgBlt": 0x10ED,
    "NtGdiPolyDraw": 0x10EE,
    "NtGdiPolyPolyDraw": 0x10EF,
    "NtGdiPolyTextOutW": 0x10F0,
    "NtGdiPtInRegion": 0x10F1,
    "NtGdiPtVisible": 0x10F2,
    "NtGdiQueryFonts": 0x10F3,
    "NtGdiQueryFontAssocInfo": 0x10F4,
    "NtGdiRectangle": 0x10F5,
    "NtGdiRectInRegion": 0x10F6,
    "NtGdiRectVisible": 0x10F7,
    "NtGdiRemoveFontResourceW": 0x10F8,
    "NtGdiRemoveFontMemResourceEx": 0x10F9,
    "NtGdiResetDC": 0x10FA,
    "NtGdiResizePalette": 0x10FB,
    "NtGdiRestoreDC": 0x10FC,
    "NtGdiRoundRect": 0x10FD,
    "NtGdiSaveDC": 0x10FE,
    "NtGdiScaleViewportExtEx": 0x10FF,
    "NtGdiScaleWindowExtEx": 0x1100,
    "NtGdiSelectBitmap": 0x1101,
    "NtGdiSelectBrush": 0x1102,
    "NtGdiSelectClipPath": 0x1103,
    "NtGdiSelectFont": 0x1104,
    "NtGdiSelectPen": 0x1105,
    "NtGdiSetBitmapAttributes": 0x1106,
    "NtGdiSetBitmapBits": 0x1107,
    "NtGdiSetBitmapDimension": 0x1108,
    "NtGdiSetBoundsRect": 0x1109,
    "NtGdiSetBrushAttributes": 0x110A,
    "NtGdiSetBrushOrg": 0x110B,
    "NtGdiSetColorAdjustment": 0x110C,
    "NtGdiSetColorSpace": 0x110D,
    "NtGdiSetDeviceGammaRamp": 0x110E,
    "NtGdiSetDIBitsToDeviceInternal": 0x110F,
    "NtGdiSetFontEnumeration": 0x1110,
    "NtGdiSetFontXform": 0x1111,
    "NtGdiSetIcmMode": 0x1112,
    "NtGdiSetLinkedUFIs": 0x1113,
    "NtGdiSetMagicColors": 0x1114,
    "NtGdiSetMetaRgn": 0x1115,
    "NtGdiSetMiterLimit": 0x1116,
    "NtGdiGetDeviceWidth": 0x1117,
    "NtGdiMirrorWindowOrg": 0x1118,
    "NtGdiSetLayout": 0x1119,
    "NtGdiSetPixel": 0x111A,
    "NtGdiSetPixelFormat": 0x111B,
    "NtGdiSetRectRgn": 0x111C,
    "NtGdiSetSystemPaletteUse": 0x111D,
    "NtGdiSetTextJustification": 0x111E,
    "NtGdiSetupPublicCFONT": 0x111F,
    "NtGdiSetVirtualResolution": 0x1120,
    "NtGdiSetSizeDevice": 0x1121,
    "NtGdiStartDoc": 0x1122,
    "NtGdiStartPage": 0x1123,
    "NtGdiStretchBlt": 0x1124,
    "NtGdiStretchDIBitsInternal": 0x1125,
    "NtGdiStrokeAndFillPath": 0x1126,
    "NtGdiStrokePath": 0x1127,
    "NtGdiSwapBuffers": 0x1128,
    "NtGdiTransformPoints": 0x1129,
    "NtGdiTransparentBlt": 0x112A,
    "NtGdiUnloadPrinterDriver": 0x112B,
    "NtGdiUMPDEngFreeUserMem": 0x112C,
    "NtGdiUnmapMemFont": 0x112C,
    "NtGdiUnrealizeObject": 0x112D,
    "NtGdiUpdateColors": 0x112E,
    "NtGdiWidenPath": 0x112F,
    "NtUserActivateKeyboardLayout": 0x1130,
    "NtUserAlterWindowStyle": 0x1131,
    "NtUserAssociateInputContext": 0x1132,
    "NtUserAttachThreadInput": 0x1133,
    "NtUserBeginPaint": 0x1134,
    "NtUserBitBltSysBmp": 0x1135,
    "NtUserBlockInput": 0x1136,
    "NtUserBuildHimcList": 0x1137,
    "NtUserBuildHwndList": 0x1138,
    "NtUserBuildNameList": 0x1139,
    "NtUserBuildPropList": 0x113A,
    "NtUserCallHwnd": 0x113B,
    "NtUserCallHwndLock": 0x113C,
    "NtUserCallHwndOpt": 0x113D,
    "NtUserCallHwndParam": 0x113E,
    "NtUserCallHwndParamLock": 0x113F,
    "NtUserCallMsgFilter": 0x1140,
    "NtUserCallNextHookEx": 0x1141,
    "NtUserCallNoParam": 0x1142,
    "NtUserCallOneParam": 0x1143,
    "NtUserCallTwoParam": 0x1144,
    "NtUserChangeClipboardChain": 0x1145,
    "NtUserChangeDisplaySettings": 0x1146,
    "NtUserCheckImeHotKey": 0x1147,
    "NtUserCheckMenuItem": 0x1148,
    "NtUserChildWindowFromPointEx": 0x1149,
    "NtUserClipCursor": 0x114A,
    "NtUserCloseClipboard": 0x114B,
    "NtUserCloseDesktop": 0x114C,
    "NtUserCloseWindowStation": 0x114D,
    "NtUserConsoleControl": 0x114E,
    "NtUserConvertMemHandle": 0x114F,
    "NtUserCopyAcceleratorTable": 0x1150,
    "NtUserCountClipboardFormats": 0x1151,
    "NtUserCreateAcceleratorTable": 0x1152,
    "NtUserCreateCaret": 0x1153,
    "NtUserCreateDesktop": 0x1154,
    "NtUserCreateInputContext": 0x1155,
    "NtUserCreateLocalMemHandle": 0x1156,
    "NtUserCreateWindowEx": 0x1157,
    "NtUserCreateWindowStation": 0x1158,
    "NtUserDdeGetQualityOfService": 0x1159,
    "NtUserDdeInitialize": 0x115A,
    "NtUserDdeSetQualityOfService": 0x115B,
    "NtUserDeferWindowPos": 0x115C,
    "NtUserDefSetText": 0x115D,
    "NtUserDeleteMenu": 0x115E,
    "NtUserDestroyAcceleratorTable": 0x115F,
    "NtUserDestroyCursor": 0x1160,
    "NtUserDestroyInputContext": 0x1161,
    "NtUserDestroyMenu": 0x1162,
    "NtUserDestroyWindow": 0x1163,
    "NtUserDisableThreadIme": 0x1164,
    "NtUserDispatchMessage": 0x1165,
    "NtUserDragDetect": 0x1166,
    "NtUserDragObject": 0x1167,
    "NtUserDrawAnimatedRects": 0x1168,
    "NtUserDrawCaption": 0x1169,
    "NtUserDrawCaptionTemp": 0x116A,
    "NtUserDrawIconEx": 0x116B,
    "NtUserDrawMenuBarTemp": 0x116C,
    "NtUserEmptyClipboard": 0x116D,
    "NtUserEnableMenuItem": 0x116E,
    "NtUserEnableScrollBar": 0x116F,
    "NtUserEndDeferWindowPosEx": 0x1170,
    "NtUserEndMenu": 0x1171,
    "NtUserEndPaint": 0x1172,
    "NtUserEnumDisplayDevices": 0x1173,
    "NtUserEnumDisplayMonitors": 0x1174,
    "NtUserEnumDisplaySettings": 0x1175,
    "NtUserEvent": 0x1176,
    "NtUserExcludeUpdateRgn": 0x1177,
    "NtUserFillWindow": 0x1178,
    "NtUserFindExistingCursorIcon": 0x1179,
    "NtUserFindWindowEx": 0x117A,
    "NtUserFlashWindowEx": 0x117B,
    "NtUserGetAltTabInfo": 0x117C,
    "NtUserGetAncestor": 0x117D,
    "NtUserGetAppImeLevel": 0x117E,
    "NtUserGetAsyncKeyState": 0x117F,
    "NtUserGetAtomName": 0x1180,
    "NtUserGetCaretBlinkTime": 0x1181,
    "NtUserGetCaretPos": 0x1182,
    "NtUserGetClassInfo": 0x1183,
    "NtUserGetClassName": 0x1184,
    "NtUserGetClipboardData": 0x1185,
    "NtUserGetClipboardFormatName": 0x1186,
    "NtUserGetClipboardOwner": 0x1187,
    "NtUserGetClipboardSequenceNumber": 0x1188,
    "NtUserGetClipboardViewer": 0x1189,
    "NtUserGetClipCursor": 0x118A,
    "NtUserGetComboBoxInfo": 0x118B,
    "NtUserGetControlBrush": 0x118C,
    "NtUserGetControlColor": 0x118D,
    "NtUserGetCPD": 0x118E,
    "NtUserGetCursorFrameInfo": 0x118F,
    "NtUserGetCursorInfo": 0x1190,
    "NtUserGetDC": 0x1191,
    "NtUserGetDCEx": 0x1192,
    "NtUserGetDoubleClickTime": 0x1193,
    "NtUserGetForegroundWindow": 0x1194,
    "NtUserGetGuiResources": 0x1195,
    "NtUserGetGUIThreadInfo": 0x1196,
    "NtUserGetIconInfo": 0x1197,
    "NtUserGetIconSize": 0x1198,
    "NtUserGetImeHotKey": 0x1199,
    "NtUserGetImeInfoEx": 0x119A,
    "NtUserGetInternalWindowPos": 0x119B,
    "NtUserGetKeyboardLayoutList": 0x119C,
    "NtUserGetKeyboardLayoutName": 0x119D,
    "NtUserGetKeyboardState": 0x119E,
    "NtUserGetKeyNameText": 0x119F,
    "NtUserGetKeyState": 0x11A0,
    "NtUserGetListBoxInfo": 0x11A1,
    "NtUserGetMenuBarInfo": 0x11A2,
    "NtUserGetMenuIndex": 0x11A3,
    "NtUserGetMenuItemRect": 0x11A4,
    "NtUserGetMessage": 0x11A5,
    "NtUserGetMouseMovePointsEx": 0x11A6,
    "NtUserGetObjectInformation": 0x11A7,
    "NtUserGetOpenClipboardWindow": 0x11A8,
    "NtUserGetPriorityClipboardFormat": 0x11A9,
    "NtUserGetProcessWindowStation": 0x11AA,
    "NtUserGetRawInputBuffer": 0x11AB,
    "NtUserGetRawInputData": 0x11AC,
    "NtUserGetRawInputDeviceInfo": 0x11AD,
    "NtUserGetRawInputDeviceList": 0x11AE,
    "NtUserGetRegisteredRawInputDevices": 0x11AF,
    "NtUserGetScrollBarInfo": 0x11B0,
    "NtUserGetSystemMenu": 0x11B1,
    "NtUserGetThreadDesktop": 0x11B2,
    "NtUserGetThreadState": 0x11B3,
    "NtUserGetTitleBarInfo": 0x11B4,
    "NtUserGetUpdateRect": 0x11B5,
    "NtUserGetUpdateRgn": 0x11B6,
    "NtUserGetWindowDC": 0x11B7,
    "NtUserGetWindowPlacement": 0x11B8,
    "NtUserGetWOWClass": 0x11B9,
    "NtUserHardErrorControl": 0x11BA,
    "NtUserHideCaret": 0x11BB,
    "NtUserHiliteMenuItem": 0x11BC,
    "NtUserImpersonateDdeClientWindow": 0x11BD,
    "NtUserInitialize": 0x11BE,
    "NtUserInitializeClientPfnArrays": 0x11BF,
    "NtUserInitTask": 0x11C0,
    "NtUserInternalGetWindowText": 0x11C1,
    "NtUserInvalidateRect": 0x11C2,
    "NtUserInvalidateRgn": 0x11C3,
    "NtUserIsClipboardFormatAvailable": 0x11C4,
    "NtUserKillTimer": 0x11C5,
    "NtUserLoadKeyboardLayoutEx": 0x11C6,
    "NtUserLockWindowStation": 0x11C7,
    "NtUserLockWindowUpdate": 0x11C8,
    "NtUserLockWorkStation": 0x11C9,
    "NtUserMapVirtualKeyEx": 0x11CA,
    "NtUserMenuItemFromPoint": 0x11CB,
    "NtUserMessageCall": 0x11CC,
    "NtUserMinMaximize": 0x11CD,
    "NtUserMNDragLeave": 0x11CE,
    "NtUserMNDragOver": 0x11CF,
    "NtUserModifyUserStartupInfoFlags": 0x11D0,
    "NtUserMoveWindow": 0x11D1,
    "NtUserNotifyIMEStatus": 0x11D2,
    "NtUserNotifyProcessCreate": 0x11D3,
    "NtUserNotifyWinEvent": 0x11D4,
    "NtUserOpenClipboard": 0x11D5,
    "NtUserOpenDesktop": 0x11D6,
    "NtUserOpenInputDesktop": 0x11D7,
    "NtUserOpenWindowStation": 0x11D8,
    "NtUserPaintDesktop": 0x11D9,
    "NtUserPeekMessage": 0x11DA,
    "NtUserPostMessage": 0x11DB,
    "NtUserPostThreadMessage": 0x11DC,
    "NtUserPrintWindow": 0x11DD,
    "NtUserProcessConnect": 0x11DE,
    "NtUserQueryInformationThread": 0x11DF,
    "NtUserQueryInputContext": 0x11E0,
    "NtUserQuerySendMessage": 0x11E1,
    "NtUserQueryUserCounters": 0x11E2,
    "NtUserQueryWindow": 0x11E3,
    "NtUserRealChildWindowFromPoint": 0x11E4,
    "NtUserRealInternalGetMessage": 0x11E5,
    "NtUserRealWaitMessageEx": 0x11E6,
    "NtUserRedrawWindow": 0x11E7,
    "NtUserRegisterClassExWOW": 0x11E8,
    "NtUserRegisterUserApiHook": 0x11E9,
    "NtUserRegisterHotKey": 0x11EA,
    "NtUserRegisterRawInputDevices": 0x11EB,
    "NtUserRegisterTasklist": 0x11EC,
    "NtUserRegisterWindowMessage": 0x11ED,
    "NtUserRemoveMenu": 0x11EE,
    "NtUserRemoveProp": 0x11EF,
    "NtUserResolveDesktop": 0x11F0,
    "NtUserResolveDesktopForWOW": 0x11F1,
    "NtUserSBGetParms": 0x11F2,
    "NtUserScrollDC": 0x11F3,
    "NtUserScrollWindowEx": 0x11F4,
    "NtUserSelectPalette": 0x11F5,
    "NtUserSendInput": 0x11F6,
    "NtUserSetActiveWindow": 0x11F7,
    "NtUserSetAppImeLevel": 0x11F8,
    "NtUserSetCapture": 0x11F9,
    "NtUserSetClassLong": 0x11FA,
    "NtUserSetClassWord": 0x11FB,
    "NtUserSetClipboardData": 0x11FC,
    "NtUserSetClipboardViewer": 0x11FD,
    "NtUserSetConsoleReserveKeys": 0x11FE,
    "NtUserSetCursor": 0x11FF,
    "NtUserSetCursorContents": 0x1200,
    "NtUserSetCursorIconData": 0x1201,
    "NtUserSetDbgTag": 0x1202,
    "NtUserSetFocus": 0x1203,
    "NtUserSetImeHotKey": 0x1204,
    "NtUserSetImeInfoEx": 0x1205,
    "NtUserSetImeOwnerWindow": 0x1206,
    "NtUserSetInformationProcess": 0x1207,
    "NtUserSetInformationThread": 0x1208,
    "NtUserSetInternalWindowPos": 0x1209,
    "NtUserSetKeyboardState": 0x120A,
    "NtUserSetLogonNotifyWindow": 0x120B,
    "NtUserSetMenu": 0x120C,
    "NtUserSetMenuContextHelpId": 0x120D,
    "NtUserSetMenuDefaultItem": 0x120E,
    "NtUserSetMenuFlagRtoL": 0x120F,
    "NtUserSetObjectInformation": 0x1210,
    "NtUserSetParent": 0x1211,
    "NtUserSetProcessWindowStation": 0x1212,
    "NtUserSetProp": 0x1213,
    "NtUserSetRipFlags": 0x1214,
    "NtUserSetScrollInfo": 0x1215,
    "NtUserSetShellWindowEx": 0x1216,
    "NtUserSetSysColors": 0x1217,
    "NtUserSetSystemCursor": 0x1218,
    "NtUserSetSystemMenu": 0x1219,
    "NtUserSetSystemTimer": 0x121A,
    "NtUserSetThreadDesktop": 0x121B,
    "NtUserSetThreadLayoutHandles": 0x121C,
    "NtUserSetThreadState": 0x121D,
    "NtUserSetTimer": 0x121E,
    "NtUserSetWindowFNID": 0x121F,
    "NtUserSetWindowLong": 0x1220,
    "NtUserSetWindowPlacement": 0x1221,
    "NtUserSetWindowPos": 0x1222,
    "NtUserSetWindowRgn": 0x1223,
    "NtUserSetWindowsHookAW": 0x1224,
    "NtUserSetWindowsHookEx": 0x1225,
    "NtUserSetWindowStationUser": 0x1226,
    "NtUserSetWindowWord": 0x1227,
    "NtUserSetWinEventHook": 0x1228,
    "NtUserShowCaret": 0x1229,
    "NtUserShowScrollBar": 0x122A,
    "NtUserShowWindow": 0x122B,
    "NtUserShowWindowAsync": 0x122C,
    "NtUserSoundSentry": 0x122D,
    "NtUserSwitchDesktop": 0x122E,
    "NtUserSystemParametersInfo": 0x122F,
    "NtUserTestForInteractiveUser": 0x1230,
    "NtUserThunkedMenuInfo": 0x1231,
    "NtUserThunkedMenuItemInfo": 0x1232,
    "NtUserToUnicodeEx": 0x1233,
    "NtUserTrackMouseEvent": 0x1234,
    "NtUserTrackPopupMenuEx": 0x1235,
    "NtUserCalcMenuBar": 0x1236,
    "NtUserPaintMenuBar": 0x1237,
    "NtUserTranslateAccelerator": 0x1238,
    "NtUserTranslateMessage": 0x1239,
    "NtUserUnhookWindowsHookEx": 0x123A,
    "NtUserUnhookWinEvent": 0x123B,
    "NtUserUnloadKeyboardLayout": 0x123C,
    "NtUserUnlockWindowStation": 0x123D,
    "NtUserUnregisterClass": 0x123E,
    "NtUserUnregisterUserApiHook": 0x123F,
    "NtUserUnregisterHotKey": 0x1240,
    "NtUserUpdateInputContext": 0x1241,
    "NtUserUpdateInstance": 0x1242,
    "NtUserUpdateLayeredWindow": 0x1243,
    "NtUserGetLayeredWindowAttributes": 0x1244,
    "NtUserSetLayeredWindowAttributes": 0x1245,
    "NtUserUpdatePerUserSystemParameters": 0x1246,
    "NtUserUserHandleGrantAccess": 0x1247,
    "NtUserValidateHandleSecure": 0x1248,
    "NtUserValidateRect": 0x1249,
    "NtUserValidateTimerCallback": 0x124a,
    "NtUserVkKeyScanEx": 0x124b,
    "NtUserWaitForInputIdle": 0x124c,
    "NtUserWaitForMsgAndEvent": 0x124d,
    "NtUserWaitMessage": 0x124e,
    "NtUserWin32PoolAllocationStats": 0x124f,
    "NtUserWindowFromPoint": 0x1250,
    "NtUserYieldTask": 0x1251,
    "NtUserRemoteConnect": 0x1252,
    "NtUserRemoteRedrawRectangle": 0x1253,
    "NtUserRemoteRedrawScreen": 0x1254,
    "NtUserRemoteStopScreenUpdates": 0x1255,
    "NtUserCtxDisplayIOCtl": 0x1256,
    "NtGdiEngAssociateSurface": 0x1257,
    "NtGdiEngCreateBitmap": 0x1258,
    "NtGdiEngCreateDeviceSurface": 0x1259,
    "NtGdiEngCreateDeviceBitmap": 0x125a,
    "NtGdiEngCreatePalette": 0x125b,
    "NtGdiEngComputeGlyphSet": 0x125c,
    "NtGdiEngCopyBits": 0x125d,
    "NtGdiEngDeletePalette": 0x125e,
    "NtGdiEngDeleteSurface": 0x125f,
    "NtGdiEngEraseSurface": 0x1260,
    "NtGdiEngUnlockSurface": 0x1261,
    "NtGdiEngLockSurface": 0x1262,
    "NtGdiEngBitBlt": 0x1263,
    "NtGdiEngStretchBlt": 0x1264,
    "NtGdiEngPlgBlt": 0x1265,
    "NtGdiEngMarkBandingSurface": 0x1266,
    "NtGdiEngStrokePath": 0x1267,
    "NtGdiEngFillPath": 0x1268,
    "NtGdiEngStrokeAndFillPath": 0x1269,
    "NtGdiEngPaint": 0x126a,
    "NtGdiEngLineTo": 0x126b,
    "NtGdiEngAlphaBlend": 0x126c,
    "NtGdiEngGradientFill": 0x126d,
    "NtGdiEngTransparentBlt": 0x126e,
    "NtGdiEngTextOut": 0x126f,
    "NtGdiEngStretchBltROP": 0x1270,
    "NtGdiXLATEOBJ_cGetPalette": 0x1271,
    "NtGdiXLATEOBJ_iXlate": 0x1272,
    "NtGdiXLATEOBJ_hGetColorTransform": 0x1273,
    "NtGdiCLIPOBJ_bEnum": 0x1274,
    "NtGdiCLIPOBJ_cEnumStart": 0x1275,
    "NtGdiCLIPOBJ_ppoGetPath": 0x1276,
    "NtGdiEngDeletePath": 0x1277,
    "NtGdiEngCreateClip": 0x1278,
    "NtGdiEngDeleteClip": 0x1279,
    "NtGdiBRUSHOBJ_ulGetBrushColor": 0x127a,
    "NtGdiBRUSHOBJ_pvAllocRbrush": 0x127b,
    "NtGdiBRUSHOBJ_pvGetRbrush": 0x127c,
    "NtGdiBRUSHOBJ_hGetColorTransform": 0x127d,
    "NtGdiXFORMOBJ_bApplyXform": 0x127e,
    "NtGdiXFORMOBJ_iGetXform": 0x127f,
    "NtGdiFONTOBJ_vGetInfo": 0x1280,
    "NtGdiFONTOBJ_pxoGetXform": 0x1281,
    "NtGdiFONTOBJ_cGetGlyphs": 0x1282,
    "NtGdiFONTOBJ_pifi": 0x1283,
    "NtGdiFONTOBJ_pfdg": 0x1284,
    "NtGdiFONTOBJ_pQueryGlyphAttrs": 0x1285,
    "NtGdiFONTOBJ_pvTrueTypeFontFile": 0x1286,
    "NtGdiFONTOBJ_cGetAllGlyphHandles": 0x1287,
    "NtGdiSTROBJ_bEnum": 0x1288,
    "NtGdiSTROBJ_bEnumPositionsOnly": 0x1289,
    "NtGdiSTROBJ_bGetAdvanceWidths": 0x128a,
    "NtGdiSTROBJ_vEnumStart": 0x128b,
    "NtGdiSTROBJ_dwGetCodePage": 0x128c,
    "NtGdiPATHOBJ_vGetBounds": 0x128d,
    "NtGdiPATHOBJ_bEnum": 0x128e,
    "NtGdiPATHOBJ_vEnumStart": 0x128f,
    "NtGdiPATHOBJ_vEnumStartClipLines": 0x1290,
    "NtGdiPATHOBJ_bEnumClipLines": 0x1291,
    "NtGdiGetDhpdev": 0x1292,
    "NtGdiEngCheckAbort": 0x1293,
    "NtGdiHT_Get8BPPFormatPalette": 0x1294,
    "NtGdiHT_Get8BPPMaskPalette": 0x1295,
    "NtGdiUpdateTransform": 0x1296,
    "NtGdiSetPUMPDOBJ": 0x1297,
    "NtGdiBRUSHOBJ_DeleteRbrush": 0x1298,
    "NtGdiUMPDEngFreeUserMem": 0x1299,
    "NtGdiDrawStream": 0x129a}

sl2 = {}
for s,d in sl.items():
    sl2[d] = s

l = sys.stdin.readline()

ss = {}
ty = set()

lprev = "NTSTATUS"

while l:
    # ret = "NTSTATUS"
    l = l.strip("\n")
    l = l.strip("(")
    if "NTAPI" in l:
        assert l == "NTAPI", l
        lprev = lprev.replace(" ", "")
        ret = lprev
        if ret.endswith("*"):
            ret = "P" + ret[:-1]
        assert "*" not in ret, l


    lprev = l

    if l.startswith("Zw") or l.startswith("Nt"):
        l = l.replace("Zw", "Nt")
        l = l.replace("(", "")
        #print "\n--%s--" % l
        # print l in sl
        # print l, ret
        if l in sl:
            # sl2.append(l)
            syscall = l
            args = []
            l = sys.stdin.readline()
            while True:
                # print l
                l = l.strip("\n")
                l = l.strip("\t")
                l = l.strip(",")
                l = l.strip("const ")
                l = l.replace("   ", "")
                l = l.replace("  ", "")
                l = l.replace(");", "")
                #print "--%s--" % l

                if not l:
                    break;
                
                arg = l.split(" ")
                # print arg

                if (len(arg) == 1 or len(arg) == 2) and arg[0] == "VOID":
                    l = sys.stdin.readline()
                    continue

                if len(arg) == 2:
                    arg = ("INOUT", arg[0], arg[1], "MANDATORY")

                if len(arg) > 2:
                    if len(arg) == 3:
                        arg.append("MANDATORY")


                    if not (arg[0] == "IN" or arg[0] == "OUT" or arg[0] == "INOUT"):                        
                        if arg[0][0] == "P":
                            arg = ("INOUT", arg[0], arg[1], arg[2])
                        else:
                            arg = ("IN", arg[0], arg[1], arg[2])

                    if arg[1] == "OPTIONAL":
                        arg = (arg[0], arg[2], arg[3], arg[1])

                    if arg[0] == "OPTIONAL":
                        arg = (arg[1], arg[2], arg[3], arg[0])

                    if len(arg) > 4:
                        arg = (arg[0], arg[1], arg[2], arg[3])

                if not (arg[3] == "MANDATORY" or arg[3] == "OPTIONAL"):
                    arg = (arg[0], arg[1], arg[2], "MANDATORY")

                # print arg
                assert len(arg) == 4, arg

                aaaa = arg[1].strip("2")
                args.append((arg[0], aaaa, arg[2], arg[3]))


                l = sys.stdin.readline()

            ss[syscall] = (syscall, args, ret)

            # print (syscall, args)

    l = sys.stdin.readline()

max = 0
for s in ss.values():
    if max < len(s[1]):
        max = len(s[1])

r = "#ifndef MagicSyscall\n"
r += "#define MagicSyscall(s, n, r, a"
for j in range(1, max + 1):
    r += ", d%d, t%d, n%d, o%d" % (j, j, j, j)
r += ")\n"
r += "#endif"
print r

# print ss

#for j in range(0, 0x12a0):
for j in range(0, 0x11c):
    if j in sl2:
        s = ss[sl2[j]]
        r = "MagicSyscall(%s, 0x%x, %s, %d" % (s[0], j, s[2], len(s[1]))
        ty.add(s[2])
        for a in s[1]:
            while "*" in a[1]:
                assert a[1].endswith("*")
                a = (a[0], "P" + a[1][:-1], a[2], a[3])
            while "*" in a[2]:
                assert a[2].startswith("*"), str(a) + s[0]
                a = (a[0], "P" + a[1], a[2][1:], a[3])

            assert len(a) == 4, a
            assert (a[0] == "IN" or a[0] == "OUT" or a[0] == "INOUT"), a
            # assert (a[3] == "MANDATORY" or a[3] == "OPTIONAL"), a
            assert (a[0] != "" and a[1] != "" and a[2] != "" and a[3] != ""), a

            r += ", %s, %s_WST, %s, %s" % (a[0], a[1], a[2], a[3])
            ty.add(a[1])
        for i in range(len(s[1]), max):
            r += ", %s, %s, %s, %s" % ("NONE", "NONE", "NONE", "NONE")

        r += ")"
        if "*" in r:
            print r
            assert False, r
        print r
    else:
        r = "MagicSyscall(NONE_%x, 0x%x, NONE, 0" % (j, j)
        for i in range(0, max):
            r += ", %s, %s, %s, %s" % ("NONE", "NONE", "NONE", "NONE")

        r += ")"
        print r

print "#undef MagicSyscall"

print

print """
#ifndef MagicSyscallArgType
#define MagicSyscallArgType(x)
#endif
"""
for a in ty:
    print "MagicSyscallArgType(%s_WST)" % (a)

print """
#undef MagicSyscallArgType
"""

sys.exit(0)

print """
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
"""

for a in ty:
    if a.startswith("P") or a.startswith("LP") or "_PTR" in a:
        p = "Ptr"
    else:
        p = "Obj"

    print "MagicSyscallArgType%s(%s_WST)" % (p, a)

print """
#undef MagicSyscallArgTypeObj
#undef MagicSyscallArgTypePtr
#undef MagicSyscallArgTypeBlob
"""

print
print
for a in ty:
    print "typedef %s %s_WST;" % (a,a)

print
print

sl = set(sl)
#sl2 = set(sl2)

# print >> sys.stderr, len(sl), len(sl2)

print >> sys.stderr
print >> sys.stderr

sys.exit(0)

for i in sl:
    if i not in sl2:
        print >> sys.stderr, i
