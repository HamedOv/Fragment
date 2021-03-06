// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "phnt_ntdef.h"
#include "detours.h"
#pragma warning(disable:4311)
#pragma warning(disable:4312)  
#pragma comment(lib, "detours.lib")

typedef struct _IO_STATUS_BLOCK
{
	union
	{
		NTSTATUS Status;
		PVOID Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
typedef VOID(NTAPI *PIO_APC_ROUTINE)(
	_In_ PVOID ApcContext,
	_In_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG Reserved
	);
typedef enum _FILE_INFORMATION_CLASS
{
	FileDirectoryInformation = 1, // FILE_DIRECTORY_INFORMATION
	FileFullDirectoryInformation, // FILE_FULL_DIR_INFORMATION
	FileBothDirectoryInformation, // FILE_BOTH_DIR_INFORMATION
	FileBasicInformation, // FILE_BASIC_INFORMATION
	FileStandardInformation, // FILE_STANDARD_INFORMATION
	FileInternalInformation, // FILE_INTERNAL_INFORMATION
	FileEaInformation, // FILE_EA_INFORMATION
	FileAccessInformation, // FILE_ACCESS_INFORMATION
	FileNameInformation, // FILE_NAME_INFORMATION
	FileRenameInformation, // FILE_RENAME_INFORMATION // 10
	FileLinkInformation, // FILE_LINK_INFORMATION
	FileNamesInformation, // FILE_NAMES_INFORMATION
	FileDispositionInformation, // FILE_DISPOSITION_INFORMATION
	FilePositionInformation, // FILE_POSITION_INFORMATION
	FileFullEaInformation, // FILE_FULL_EA_INFORMATION
	FileModeInformation, // FILE_MODE_INFORMATION
	FileAlignmentInformation, // FILE_ALIGNMENT_INFORMATION
	FileAllInformation, // FILE_ALL_INFORMATION
	FileAllocationInformation, // FILE_ALLOCATION_INFORMATION
	FileEndOfFileInformation, // FILE_END_OF_FILE_INFORMATION // 20
	FileAlternateNameInformation, // FILE_NAME_INFORMATION
	FileStreamInformation, // FILE_STREAM_INFORMATION
	FilePipeInformation, // FILE_PIPE_INFORMATION
	FilePipeLocalInformation, // FILE_PIPE_LOCAL_INFORMATION
	FilePipeRemoteInformation, // FILE_PIPE_REMOTE_INFORMATION
	FileMailslotQueryInformation, // FILE_MAILSLOT_QUERY_INFORMATION
	FileMailslotSetInformation, // FILE_MAILSLOT_SET_INFORMATION
	FileCompressionInformation, // FILE_COMPRESSION_INFORMATION
	FileObjectIdInformation, // FILE_OBJECTID_INFORMATION
	FileCompletionInformation, // FILE_COMPLETION_INFORMATION // 30
	FileMoveClusterInformation, // FILE_MOVE_CLUSTER_INFORMATION
	FileQuotaInformation, // FILE_QUOTA_INFORMATION
	FileReparsePointInformation, // FILE_REPARSE_POINT_INFORMATION
	FileNetworkOpenInformation, // FILE_NETWORK_OPEN_INFORMATION
	FileAttributeTagInformation, // FILE_ATTRIBUTE_TAG_INFORMATION
	FileTrackingInformation, // FILE_TRACKING_INFORMATION
	FileIdBothDirectoryInformation, // FILE_ID_BOTH_DIR_INFORMATION
	FileIdFullDirectoryInformation, // FILE_ID_FULL_DIR_INFORMATION
	FileValidDataLengthInformation, // FILE_VALID_DATA_LENGTH_INFORMATION
	FileShortNameInformation, // FILE_NAME_INFORMATION // 40
	FileIoCompletionNotificationInformation, // FILE_IO_COMPLETION_NOTIFICATION_INFORMATION // since VISTA
	FileIoStatusBlockRangeInformation, // FILE_IOSTATUSBLOCK_RANGE_INFORMATION
	FileIoPriorityHintInformation, // FILE_IO_PRIORITY_HINT_INFORMATION
	FileSfioReserveInformation, // FILE_SFIO_RESERVE_INFORMATION
	FileSfioVolumeInformation, // FILE_SFIO_VOLUME_INFORMATION
	FileHardLinkInformation, // FILE_LINKS_INFORMATION
	FileProcessIdsUsingFileInformation, // FILE_PROCESS_IDS_USING_FILE_INFORMATION
	FileNormalizedNameInformation, // FILE_NAME_INFORMATION
	FileNetworkPhysicalNameInformation, // FILE_NETWORK_PHYSICAL_NAME_INFORMATION
	FileIdGlobalTxDirectoryInformation, // FILE_ID_GLOBAL_TX_DIR_INFORMATION // since WIN7 // 50
	FileIsRemoteDeviceInformation, // FILE_IS_REMOTE_DEVICE_INFORMATION
	FileUnusedInformation,
	FileNumaNodeInformation, // FILE_NUMA_NODE_INFORMATION
	FileStandardLinkInformation, // FILE_STANDARD_LINK_INFORMATION
	FileRemoteProtocolInformation, // FILE_REMOTE_PROTOCOL_INFORMATION
	FileRenameInformationBypassAccessCheck, // (kernel-mode only); FILE_RENAME_INFORMATION // since WIN8
	FileLinkInformationBypassAccessCheck, // (kernel-mode only); FILE_LINK_INFORMATION
	FileVolumeNameInformation, // FILE_VOLUME_NAME_INFORMATION
	FileIdInformation, // FILE_ID_INFORMATION
	FileIdExtdDirectoryInformation, // FILE_ID_EXTD_DIR_INFORMATION
	FileReplaceCompletionInformation, // FILE_COMPLETION_INFORMATION // since WINBLUE
	FileHardLinkFullIdInformation, // FILE_LINK_ENTRY_FULL_ID_INFORMATION
	FileIdExtdBothDirectoryInformation, // FILE_ID_EXTD_BOTH_DIR_INFORMATION // since THRESHOLD
	FileDispositionInformationEx, // FILE_DISPOSITION_INFO_EX // since REDSTONE
	FileRenameInformationEx,
	FileRenameInformationExBypassAccessCheck,
	FileDesiredStorageClassInformation, // FILE_DESIRED_STORAGE_CLASS_INFORMATION // since REDSTONE2
	FileStatInformation, // FILE_STAT_INFORMATION
	FileMemoryPartitionInformation, // FILE_MEMORY_PARTITION_INFORMATION // since REDSTONE3
	FileStatLxInformation, // FILE_STAT_LX_INFORMATION // since REDSTONE4
	FileCaseSensitiveInformation, // FILE_CASE_SENSITIVE_INFORMATION
	FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef NTSYSCALLAPI NTSTATUS (NTAPI* fnNtQueryDirectoryFileEx)(
	_In_ HANDLE FileHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_writes_bytes_(Length) PVOID FileInformation,
	_In_ ULONG Length,
	_In_ FILE_INFORMATION_CLASS FileInformationClass,
	_In_ ULONG QueryFlags,
	_In_opt_ PUNICODE_STRING FileName
	);
fnNtQueryDirectoryFileEx pNtQueryDirectoryFileEx;

typedef NTSYSCALLAPI NTSTATUS(NTAPI* fnNtQueryDirectoryFile)(
	_In_ HANDLE FileHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_writes_bytes_(Length) PVOID FileInformation,
	_In_ ULONG Length,
	_In_ FILE_INFORMATION_CLASS FileInformationClass,
	_In_ BOOLEAN ReturnSingleEntry,
	_In_opt_ PUNICODE_STRING FileName,
	_In_ BOOLEAN RestartScan
	);
fnNtQueryDirectoryFile pNtQueryDirectoryFile;

#define SL_RESTART_SCAN                 0x01
#define SL_RETURN_SINGLE_ENTRY          0x02
#define SL_INDEX_SPECIFIED              0x04
#define SL_RETURN_ON_DISK_ENTRIES_ONLY  0x08

#define SL_QUERY_DIRECTORY_MASK         0x0b

NTSTATUS NTAPI newNtQueryDirectoryFileEx(_In_ HANDLE FileHandle,
	_In_opt_ HANDLE Event,
	_In_opt_ PIO_APC_ROUTINE ApcRoutine,
	_In_opt_ PVOID ApcContext,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_writes_bytes_(Length) PVOID FileInformation,
	_In_ ULONG Length,
	_In_ FILE_INFORMATION_CLASS FileInformationClass,
	_In_ ULONG QueryFlags,
	_In_opt_ PUNICODE_STRING FileName)
{
	bool ReturnSingleEntry=false;
	bool RestartScan=false;
	if(QueryFlags&SL_RESTART_SCAN)
	{
		RestartScan = true;
	}
	if (QueryFlags&SL_RETURN_SINGLE_ENTRY)
	{
		ReturnSingleEntry = true;
	}
	return pNtQueryDirectoryFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation, Length, FileInformationClass, ReturnSingleEntry, FileName, RestartScan);
}

void BeginDetour()
{
	pNtQueryDirectoryFileEx = (fnNtQueryDirectoryFileEx)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryDirectoryFileEx");
	pNtQueryDirectoryFile = (fnNtQueryDirectoryFile)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryDirectoryFile");
	DetourTransactionBegin();
	DetourAttach((void**)&pNtQueryDirectoryFileEx, newNtQueryDirectoryFileEx);
	if (DetourTransactionCommit() != NO_ERROR)
	{
		MessageBoxW(NULL, L"Hook目标函数失败", L"严重错误", MB_OK | MB_ICONWARNING);
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		BeginDetour();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) void dummy(void) {
	return;
}