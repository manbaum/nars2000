/*
 * Copyright (C) the Wine project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __WINE_WINBASE_H
#define __WINE_WINBASE_H

#include <winerror.h>

#ifdef __cplusplus
extern "C" {
#endif

  /* Windows Exit Procedure flag values */
#define	WEP_FREE_DLL        0
#define	WEP_SYSTEM_EXIT     1

typedef DWORD (CALLBACK *LPTHREAD_START_ROUTINE)(LPVOID);

typedef VOID (WINAPI *PFIBER_START_ROUTINE)( LPVOID lpFiberParameter );
typedef PFIBER_START_ROUTINE LPFIBER_START_ROUTINE;

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;

typedef RTL_CRITICAL_SECTION_DEBUG CRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG PCRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG LPCRITICAL_SECTION_DEBUG;

typedef WAITORTIMERCALLBACKFUNC WAITORTIMERCALLBACK;

#define EXCEPTION_DEBUG_EVENT       1
#define CREATE_THREAD_DEBUG_EVENT   2
#define CREATE_PROCESS_DEBUG_EVENT  3
#define EXIT_THREAD_DEBUG_EVENT     4
#define EXIT_PROCESS_DEBUG_EVENT    5
#define LOAD_DLL_DEBUG_EVENT        6
#define UNLOAD_DLL_DEBUG_EVENT      7
#define OUTPUT_DEBUG_STRING_EVENT   8
#define RIP_EVENT                   9

typedef struct _EXCEPTION_DEBUG_INFO {
    EXCEPTION_RECORD ExceptionRecord;
    DWORD dwFirstChance;
} EXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO {
    HANDLE hThread;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
} CREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO {
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    LPVOID lpBaseOfImage;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
    LPVOID lpImageName;
    WORD fUnicode;
} CREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO {
    HANDLE hFile;
    LPVOID   lpBaseOfDll;
    DWORD    dwDebugInfoFileOffset;
    DWORD    nDebugInfoSize;
    LPVOID   lpImageName;
    WORD     fUnicode;
} LOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO {
    LPVOID lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO {
    LPSTR lpDebugStringData;
    WORD  fUnicode;
    WORD  nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO;

typedef struct _RIP_INFO {
    DWORD dwError;
    DWORD dwType;
} RIP_INFO;

typedef struct _DEBUG_EVENT {
    DWORD dwDebugEventCode;
    DWORD dwProcessId;
    DWORD dwThreadId;
    union {
        EXCEPTION_DEBUG_INFO      Exception;
        CREATE_THREAD_DEBUG_INFO  CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO    ExitThread;
        EXIT_PROCESS_DEBUG_INFO   ExitProcess;
        LOAD_DLL_DEBUG_INFO       LoadDll;
        UNLOAD_DLL_DEBUG_INFO     UnloadDll;
        OUTPUT_DEBUG_STRING_INFO  DebugString;
        RIP_INFO                  RipInfo;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;

typedef PCONTEXT LPCONTEXT;
typedef PEXCEPTION_RECORD LPEXCEPTION_RECORD;
typedef PEXCEPTION_POINTERS LPEXCEPTION_POINTERS;

typedef LONG (CALLBACK *PTOP_LEVEL_EXCEPTION_FILTER)(PEXCEPTION_POINTERS);
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;

#define OFS_MAXPATHNAME 128
typedef struct _OFSTRUCT
{
    BYTE cBytes;
    BYTE fFixedDisk;
    WORD nErrCode;
    WORD Reserved1;
    WORD Reserved2;
    CHAR szPathName[OFS_MAXPATHNAME];
} OFSTRUCT, *POFSTRUCT, *LPOFSTRUCT;

#define OF_READ               0x0000
#define OF_WRITE              0x0001
#define OF_READWRITE          0x0002
#define OF_SHARE_COMPAT       0x0000
#define OF_SHARE_EXCLUSIVE    0x0010
#define OF_SHARE_DENY_WRITE   0x0020
#define OF_SHARE_DENY_READ    0x0030
#define OF_SHARE_DENY_NONE    0x0040
#define OF_PARSE              0x0100
#define OF_DELETE             0x0200
#define OF_VERIFY             0x0400   /* Used with OF_REOPEN */
#define OF_SEARCH             0x0400   /* Used without OF_REOPEN */
#define OF_CANCEL             0x0800
#define OF_CREATE             0x1000
#define OF_PROMPT             0x2000
#define OF_EXIST              0x4000
#define OF_REOPEN             0x8000

/* SetErrorMode values */
#define SEM_FAILCRITICALERRORS      0x0001
#define SEM_NOGPFAULTERRORBOX       0x0002
#define SEM_NOALIGNMENTFAULTEXCEPT  0x0004
#define SEM_NOOPENFILEERRORBOX      0x8000

/* CopyFileEx flags */
#define COPY_FILE_FAIL_IF_EXISTS        0x00000001
#define COPY_FILE_RESTARTABLE           0x00000002
#define COPY_FILE_OPEN_SOURCE_FOR_WRITE 0x00000004

/* return values for CopyProgressRoutine */
#define PROGRESS_CONTINUE   0
#define PROGRESS_CANCEL     1
#define PROGRESS_STOP       2
#define PROGRESS_QUIET      3

/* reason codes for CopyProgressRoutine */
#define CALLBACK_CHUNK_FINISHED 0
#define CALLBACK_STREAM_SWITCH  1

/* GetTempFileName() Flags */
#define TF_FORCEDRIVE	        0x80

#define DRIVE_UNKNOWN              0
#define DRIVE_NO_ROOT_DIR          1
#define DRIVE_REMOVABLE            2
#define DRIVE_FIXED                3
#define DRIVE_REMOTE               4
/* Win32 additions */
#define DRIVE_CDROM                5
#define DRIVE_RAMDISK              6

#define MAX_COMPUTERNAME_LENGTH    15

/* The security attributes structure */
typedef struct _SECURITY_ATTRIBUTES
{
    DWORD   nLength;
    LPVOID  lpSecurityDescriptor;
    BOOL  bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

#ifndef _FILETIME_
#define _FILETIME_
/* 64 bit number of 100 nanoseconds intervals since January 1, 1601 */
typedef struct _FILETIME
{
#ifdef WORDS_BIGENDIAN
  DWORD  dwHighDateTime;
  DWORD  dwLowDateTime;
#else
  DWORD  dwLowDateTime;
  DWORD  dwHighDateTime;
#endif
} FILETIME, *PFILETIME, *LPFILETIME;
#endif /* _FILETIME_ */

/* Find* structures */
typedef struct _WIN32_FIND_DATAA
{
    DWORD     dwFileAttributes;
    FILETIME  ftCreationTime;
    FILETIME  ftLastAccessTime;
    FILETIME  ftLastWriteTime;
    DWORD     nFileSizeHigh;
    DWORD     nFileSizeLow;
    DWORD     dwReserved0;
    DWORD     dwReserved1;
    CHAR      cFileName[260];
    CHAR      cAlternateFileName[14];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

typedef struct _WIN32_FIND_DATAW
{
    DWORD     dwFileAttributes;
    FILETIME  ftCreationTime;
    FILETIME  ftLastAccessTime;
    FILETIME  ftLastWriteTime;
    DWORD     nFileSizeHigh;
    DWORD     nFileSizeLow;
    DWORD     dwReserved0;
    DWORD     dwReserved1;
    WCHAR     cFileName[260];
    WCHAR     cAlternateFileName[14];
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

DECL_WINELIB_TYPE_AW(WIN32_FIND_DATA)
DECL_WINELIB_TYPE_AW(PWIN32_FIND_DATA)
DECL_WINELIB_TYPE_AW(LPWIN32_FIND_DATA)

typedef enum _FINDEX_INFO_LEVELS
{
	FindExInfoStandard,
	FindExInfoMaxInfoLevel
} FINDEX_INFO_LEVELS;

typedef enum _FINDEX_SEARCH_OPS
{
	FindExSearchNameMatch,
	FindExSearchLimitToDirectories,
	FindExSearchLimitToDevices,
	FindExSearchMaxSearchOp
} FINDEX_SEARCH_OPS;

typedef struct _PROCESS_HEAP_ENTRY
{
    LPVOID lpData;
    DWORD cbData;
    BYTE cbOverhead;
    BYTE iRegionIndex;
    WORD wFlags;
    union {
        struct {
            HANDLE hMem;
            DWORD dwReserved[3];
        } Block;
        struct {
            DWORD dwCommittedSize;
            DWORD dwUnCommittedSize;
            LPVOID lpFirstBlock;
            LPVOID lpLastBlock;
        } Region;
    } DUMMYUNIONNAME;
} PROCESS_HEAP_ENTRY, *PPROCESS_HEAP_ENTRY, *LPPROCESS_HEAP_ENTRY;

#define PROCESS_HEAP_REGION                   0x0001
#define PROCESS_HEAP_UNCOMMITTED_RANGE        0x0002
#define PROCESS_HEAP_ENTRY_BUSY               0x0004
#define PROCESS_HEAP_ENTRY_MOVEABLE           0x0010
#define PROCESS_HEAP_ENTRY_DDESHARE           0x0020

#define INVALID_HANDLE_VALUE     ((HANDLE)~0UL)
#define INVALID_FILE_SIZE        ((DWORD)~0UL)
#define INVALID_SET_FILE_POINTER ((DWORD)~0UL)
#define INVALID_FILE_ATTRIBUTES  ((DWORD)~0UL)

#define LOCKFILE_FAIL_IMMEDIATELY   1
#define LOCKFILE_EXCLUSIVE_LOCK     2

#define TLS_OUT_OF_INDEXES ((DWORD)~0UL)

#define SHUTDOWN_NORETRY 1

/* comm */

#define CBR_110	0xFF10
#define CBR_300	0xFF11
#define CBR_600	0xFF12
#define CBR_1200	0xFF13
#define CBR_2400	0xFF14
#define CBR_4800	0xFF15
#define CBR_9600	0xFF16
#define CBR_14400	0xFF17
#define CBR_19200	0xFF18
#define CBR_38400	0xFF1B
#define CBR_56000	0xFF1F
#define CBR_57600       0xFF20
#define CBR_115200      0xFF21
#define CBR_128000	0xFF23
#define CBR_256000	0xFF27

#define NOPARITY	0
#define ODDPARITY	1
#define EVENPARITY	2
#define MARKPARITY	3
#define SPACEPARITY	4
#define ONESTOPBIT	0
#define ONE5STOPBITS	1
#define TWOSTOPBITS	2

#define IGNORE		0
#define INFINITE      0xFFFFFFFF

#define CE_RXOVER	0x0001
#define CE_OVERRUN	0x0002
#define CE_RXPARITY	0x0004
#define CE_FRAME	0x0008
#define CE_BREAK	0x0010
#define CE_CTSTO	0x0020
#define CE_DSRTO	0x0040
#define CE_RLSDTO	0x0080
#define CE_TXFULL	0x0100
#define CE_PTO		0x0200
#define CE_IOE		0x0400
#define CE_DNS		0x0800
#define CE_OOP		0x1000
#define CE_MODE	0x8000

#define IE_BADID	-1
#define IE_OPEN	-2
#define IE_NOPEN	-3
#define IE_MEMORY	-4
#define IE_DEFAULT	-5
#define IE_HARDWARE	-10
#define IE_BYTESIZE	-11
#define IE_BAUDRATE	-12

#define EV_RXCHAR    0x0001
#define EV_RXFLAG    0x0002
#define EV_TXEMPTY   0x0004
#define EV_CTS       0x0008
#define EV_DSR       0x0010
#define EV_RLSD      0x0020
#define EV_BREAK     0x0040
#define EV_ERR       0x0080
#define EV_RING      0x0100
#define EV_PERR      0x0200
#define EV_RX80FULL  0x0400
#define EV_EVENT1    0x0800
#define EV_EVENT2    0x1000

#define SETXOFF	1
#define SETXON		2
#define SETRTS		3
#define CLRRTS		4
#define SETDTR		5
#define CLRDTR		6
#define RESETDEV	7
#define SETBREAK	8
#define CLRBREAK	9

/* Purge functions for Comm Port */
#define PURGE_TXABORT       0x0001  /* Kill the pending/current writes to the
				       comm port */
#define PURGE_RXABORT       0x0002  /*Kill the pending/current reads to
				     the comm port */
#define PURGE_TXCLEAR       0x0004  /* Kill the transmit queue if there*/
#define PURGE_RXCLEAR       0x0008  /* Kill the typeahead buffer if there*/


/* Modem Status Flags */
#define MS_CTS_ON           ((DWORD)0x0010)
#define MS_DSR_ON           ((DWORD)0x0020)
#define MS_RING_ON          ((DWORD)0x0040)
#define MS_RLSD_ON          ((DWORD)0x0080)

#define	RTS_CONTROL_DISABLE	0
#define	RTS_CONTROL_ENABLE	1
#define	RTS_CONTROL_HANDSHAKE	2
#define	RTS_CONTROL_TOGGLE	3

#define	DTR_CONTROL_DISABLE	0
#define	DTR_CONTROL_ENABLE	1
#define	DTR_CONTROL_HANDSHAKE	2


#define LMEM_FIXED          0
#define LMEM_MOVEABLE       0x0002
#define LMEM_NOCOMPACT      0x0010
#define LMEM_NODISCARD      0x0020
#define LMEM_ZEROINIT       0x0040
#define LMEM_MODIFY         0x0080
#define LMEM_DISCARDABLE    0x0F00
#define LMEM_DISCARDED      0x4000
#define LMEM_INVALID_HANDLE 0x8000
#define LMEM_LOCKCOUNT      0x00FF

#define LPTR (LMEM_FIXED | LMEM_ZEROINIT)
#define LHND (LMEM_MOVEABLE | LMEM_ZEROINIT)

#define NONZEROLHND         (LMEM_MOVEABLE)
#define NONZEROLPTR         (LMEM_FIXED)

#define LocalDiscard(h)     (LocalReAlloc((h),0,LMEM_MOVEABLE))

#define GMEM_FIXED          0x0000
#define GMEM_MOVEABLE       0x0002
#define GMEM_NOCOMPACT      0x0010
#define GMEM_NODISCARD      0x0020
#define GMEM_ZEROINIT       0x0040
#define GMEM_MODIFY         0x0080
#define GMEM_DISCARDABLE    0x0100
#define GMEM_NOT_BANKED     0x1000
#define GMEM_SHARE          0x2000
#define GMEM_DDESHARE       0x2000
#define GMEM_NOTIFY         0x4000
#define GMEM_LOWER          GMEM_NOT_BANKED
#define GMEM_DISCARDED      0x4000
#define GMEM_LOCKCOUNT      0x00ff
#define GMEM_INVALID_HANDLE 0x8000

#define GHND                (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR                (GMEM_FIXED | GMEM_ZEROINIT)

#define GlobalLRUNewest(h)  ((HANDLE)(h))
#define GlobalLRUOldest(h)  ((HANDLE)(h))
#define GlobalDiscard(h)    (GlobalReAlloc((h),0,GMEM_MOVEABLE))

#define INVALID_ATOM        ((ATOM)0)
#define MAXINTATOM          0xc000
#ifdef __WINESRC__
/* force using a cast when inside Wine */
#define MAKEINTATOM(atom)   ((ULONG_PTR)((WORD)(atom)))
#else
#define MAKEINTATOM(atom)   ((LPTSTR)((ULONG_PTR)((WORD)(atom))))
#endif

typedef struct tagMEMORYSTATUS
{
    DWORD    dwLength;
    DWORD    dwMemoryLoad;
    SIZE_T   dwTotalPhys;
    SIZE_T   dwAvailPhys;
    SIZE_T   dwTotalPageFile;
    SIZE_T   dwAvailPageFile;
    SIZE_T   dwTotalVirtual;
    SIZE_T   dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

#include <pshpack8.h>
typedef struct tagMEMORYSTATUSEX {
  DWORD dwLength;
  DWORD dwMemoryLoad;
  DWORDLONG DECLSPEC_ALIGN(8) ullTotalPhys;
  DWORDLONG DECLSPEC_ALIGN(8) ullAvailPhys;
  DWORDLONG DECLSPEC_ALIGN(8) ullTotalPageFile;
  DWORDLONG DECLSPEC_ALIGN(8) ullAvailPageFile;
  DWORDLONG DECLSPEC_ALIGN(8) ullTotalVirtual;
  DWORDLONG DECLSPEC_ALIGN(8) ullAvailVirtual;
  DWORDLONG DECLSPEC_ALIGN(8) ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
#include <poppack.h>


typedef struct _SYSTEMTIME{
        WORD wYear;
        WORD wMonth;
        WORD wDayOfWeek;
        WORD wDay;
        WORD wHour;
        WORD wMinute;
        WORD wSecond;
        WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

/* The 'overlapped' data structure used by async I/O functions.
 */
typedef struct _OVERLAPPED {
#ifdef WORDS_BIGENDIAN
        ULONG_PTR InternalHigh;
        ULONG_PTR Internal;
#else
        ULONG_PTR Internal;
        ULONG_PTR InternalHigh;
#endif
        union {
            struct {
#ifdef WORDS_BIGENDIAN
                DWORD OffsetHigh;
                DWORD Offset;
#else
                DWORD Offset;
                DWORD OffsetHigh;
#endif
            } DUMMYSTRUCTNAME;
            PVOID Pointer;
        } DUMMYUNIONNAME;
        HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef VOID (CALLBACK *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD,DWORD,LPOVERLAPPED);

/* Process startup information.
 */

/* STARTUPINFO.dwFlags */
#define	STARTF_USESHOWWINDOW	0x00000001
#define	STARTF_USESIZE		0x00000002
#define	STARTF_USEPOSITION	0x00000004
#define	STARTF_USECOUNTCHARS	0x00000008
#define	STARTF_USEFILLATTRIBUTE	0x00000010
#define	STARTF_RUNFULLSCREEN	0x00000020
#define	STARTF_FORCEONFEEDBACK	0x00000040
#define	STARTF_FORCEOFFFEEDBACK	0x00000080
#define	STARTF_USESTDHANDLES	0x00000100
#define	STARTF_USEHOTKEY	0x00000200

typedef struct _STARTUPINFOA{
        DWORD cb;		/* 00: size of struct */
        LPSTR lpReserved;	/* 04: */
        LPSTR lpDesktop;	/* 08: */
        LPSTR lpTitle;		/* 0c: */
        DWORD dwX;		/* 10: */
        DWORD dwY;		/* 14: */
        DWORD dwXSize;		/* 18: */
        DWORD dwYSize;		/* 1c: */
        DWORD dwXCountChars;	/* 20: */
        DWORD dwYCountChars;	/* 24: */
        DWORD dwFillAttribute;	/* 28: */
        DWORD dwFlags;		/* 2c: */
        WORD wShowWindow;	/* 30: */
        WORD cbReserved2;	/* 32: */
        BYTE *lpReserved2;	/* 34: */
        HANDLE hStdInput;	/* 38: */
        HANDLE hStdOutput;	/* 3c: */
        HANDLE hStdError;	/* 40: */
} STARTUPINFOA, *LPSTARTUPINFOA;

typedef struct _STARTUPINFOW{
        DWORD cb;
        LPWSTR lpReserved;
        LPWSTR lpDesktop;
        LPWSTR lpTitle;
        DWORD dwX;
        DWORD dwY;
        DWORD dwXSize;
        DWORD dwYSize;
        DWORD dwXCountChars;
        DWORD dwYCountChars;
        DWORD dwFillAttribute;
        DWORD dwFlags;
        WORD wShowWindow;
        WORD cbReserved2;
        BYTE *lpReserved2;
        HANDLE hStdInput;
        HANDLE hStdOutput;
        HANDLE hStdError;
} STARTUPINFOW, *LPSTARTUPINFOW;

DECL_WINELIB_TYPE_AW(STARTUPINFO)
DECL_WINELIB_TYPE_AW(LPSTARTUPINFO)

typedef struct _PROCESS_INFORMATION{
	HANDLE	hProcess;
	HANDLE	hThread;
	DWORD		dwProcessId;
	DWORD		dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

typedef struct _TIME_ZONE_INFORMATION{
        LONG Bias;
        WCHAR StandardName[32];
        SYSTEMTIME StandardDate;
        LONG StandardBias;
        WCHAR DaylightName[32];
        SYSTEMTIME DaylightDate;
        LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

#define TIME_ZONE_ID_INVALID	((DWORD)0xFFFFFFFF)
#define TIME_ZONE_ID_UNKNOWN    0
#define TIME_ZONE_ID_STANDARD   1
#define TIME_ZONE_ID_DAYLIGHT   2

/* CreateProcess: dwCreationFlag values
 */
#define DEBUG_PROCESS               0x00000001
#define DEBUG_ONLY_THIS_PROCESS     0x00000002
#define CREATE_SUSPENDED            0x00000004
#define DETACHED_PROCESS            0x00000008
#define CREATE_NEW_CONSOLE          0x00000010
#define NORMAL_PRIORITY_CLASS       0x00000020
#define IDLE_PRIORITY_CLASS         0x00000040
#define HIGH_PRIORITY_CLASS         0x00000080
#define REALTIME_PRIORITY_CLASS     0x00000100
#define BELOW_NORMAL_PRIORITY_CLASS 0x00004000
#define ABOVE_NORMAL_PRIORITY_CLASS 0x00008000
#define CREATE_NEW_PROCESS_GROUP    0x00000200
#define CREATE_UNICODE_ENVIRONMENT  0x00000400
#define CREATE_SEPARATE_WOW_VDM     0x00000800
#define CREATE_SHARED_WOW_VDM       0x00001000
#define STACK_SIZE_PARAM_IS_A_RESERVATION 0x00010000
#define CREATE_DEFAULT_ERROR_MODE   0x04000000
#define CREATE_NO_WINDOW            0x08000000
#define PROFILE_USER                0x10000000
#define PROFILE_KERNEL              0x20000000
#define PROFILE_SERVER              0x40000000


/* File object type definitions
 */
#define FILE_TYPE_UNKNOWN       0
#define FILE_TYPE_DISK          1
#define FILE_TYPE_CHAR          2
#define FILE_TYPE_PIPE          3
#define FILE_TYPE_REMOTE        32768

/* File encryption status
 */
#define FILE_ENCRYPTABLE         0
#define FILE_IS_ENCRYPTED        1
#define FILE_SYSTEM_ATTR         2
#define FILE_ROOT_DIR            3
#define FILE_SYSTEM_DIR          4
#define FILE_UNKNOWN             5
#define FILE_SYSTEM_NOT_SUPPORT  6
#define FILE_USER_DISALLOWED     7
#define FILE_READ_ONLY           8
#define FILE_DIR_DISALOWED       9

/* File creation flags
 */
#define FILE_FLAG_WRITE_THROUGH    0x80000000UL
#define FILE_FLAG_OVERLAPPED 	   0x40000000L
#define FILE_FLAG_NO_BUFFERING     0x20000000L
#define FILE_FLAG_RANDOM_ACCESS    0x10000000L
#define FILE_FLAG_SEQUENTIAL_SCAN  0x08000000L
#define FILE_FLAG_DELETE_ON_CLOSE  0x04000000L
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000L
#define FILE_FLAG_POSIX_SEMANTICS  0x01000000L
#define CREATE_NEW              1
#define CREATE_ALWAYS           2
#define OPEN_EXISTING           3
#define OPEN_ALWAYS             4
#define TRUNCATE_EXISTING       5

/* Standard handle identifiers
 */
#define STD_INPUT_HANDLE        ((DWORD) -10)
#define STD_OUTPUT_HANDLE       ((DWORD) -11)
#define STD_ERROR_HANDLE        ((DWORD) -12)

typedef struct _BY_HANDLE_FILE_INFORMATION
{
  DWORD dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD dwVolumeSerialNumber;
  DWORD nFileSizeHigh;
  DWORD nFileSizeLow;
  DWORD nNumberOfLinks;
  DWORD nFileIndexHigh;
  DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION ;

#define PIPE_ACCESS_INBOUND  1
#define PIPE_ACCESS_OUTBOUND 2
#define PIPE_ACCESS_DUPLEX   3

#define PIPE_CLIENT_END       0
#define PIPE_SERVER_END       1
#define PIPE_READMODE_BYTE    0
#define PIPE_READMODE_MESSAGE 2
#define PIPE_TYPE_BYTE        0
#define PIPE_TYPE_MESSAGE     4

#define PIPE_WAIT   0
#define PIPE_NOWAIT 1

#define PIPE_UNLIMITED_INSTANCES 255

#define NMPWAIT_WAIT_FOREVER		0xffffffff
#define NMPWAIT_NOWAIT			0x00000001
#define NMPWAIT_USE_DEFAULT_WAIT	0x00000000

/* Security flags for dwFlagsAndAttributes of CreateFile */
#define SECURITY_ANONYMOUS          (SecurityAnonymous << 16)
#define SECURITY_IDENTIFICATION     (SecurityIdentification << 16)
#define SECURITY_IMPERSONATION      (SecurityImpersonation << 16)
#define SECURITY_DELEGATION         (SecurityDelegation << 16)

#define SECURITY_CONTEXT_TRACKING   0x00040000
#define SECURITY_EFFECTIVE_ONLY     0x00080000

#define SECURITY_SQOS_PRESENT       0x00100000
#define SECURITY_VALID_SQOS_FLAGS   0x001f0000

typedef struct _SYSTEM_POWER_STATUS
{
  BYTE    ACLineStatus;
  BYTE    BatteryFlag;
  BYTE    BatteryLifePercent;
  BYTE    Reserved1;
  DWORD   BatteryLifeTime;
  DWORD   BatteryFullLifeTime;
} SYSTEM_POWER_STATUS, *LPSYSTEM_POWER_STATUS;


typedef struct _SYSTEM_INFO
{
    union {
	DWORD	dwOemId; /* Obsolete field - do not use */
	struct {
		WORD wProcessorArchitecture;
		WORD wReserved;
	} DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    DWORD	dwPageSize;
    LPVOID	lpMinimumApplicationAddress;
    LPVOID	lpMaximumApplicationAddress;
    DWORD	dwActiveProcessorMask;
    DWORD	dwNumberOfProcessors;
    DWORD	dwProcessorType;
    DWORD	dwAllocationGranularity;
    WORD	wProcessorLevel;
    WORD	wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

typedef BOOL (CALLBACK *ENUMRESTYPEPROCA)(HMODULE,LPSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESTYPEPROCW)(HMODULE,LPWSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESNAMEPROCA)(HMODULE,LPCSTR,LPSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESNAMEPROCW)(HMODULE,LPCWSTR,LPWSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESLANGPROCA)(HMODULE,LPCSTR,LPCSTR,WORD,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESLANGPROCW)(HMODULE,LPCWSTR,LPCWSTR,WORD,LONG_PTR);

DECL_WINELIB_TYPE_AW(ENUMRESTYPEPROC)
DECL_WINELIB_TYPE_AW(ENUMRESNAMEPROC)
DECL_WINELIB_TYPE_AW(ENUMRESLANGPROC)

/* flags that can be passed to LoadLibraryEx */
#define	DONT_RESOLVE_DLL_REFERENCES	0x00000001
#define	LOAD_LIBRARY_AS_DATAFILE	0x00000002
#define	LOAD_WITH_ALTERED_SEARCH_PATH	0x00000008

#define GET_MODULE_HANDLE_EX_FLAG_PIN                 1
#define GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT  2
#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS        4

typedef PLDT_ENTRY LPLDT_ENTRY;

typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard
} GET_FILEEX_INFO_LEVELS;

typedef struct _WIN32_FILE_ATTRIBUTES_DATA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

/*
 * This one seems to be a Win32 only definition. It also is defined with
 * WINAPI instead of CALLBACK in the windows headers.
 */
typedef DWORD (CALLBACK *LPPROGRESS_ROUTINE)(LARGE_INTEGER, LARGE_INTEGER, LARGE_INTEGER,
                                           LARGE_INTEGER, DWORD, DWORD, HANDLE,
                                           HANDLE, LPVOID);


#define WAIT_FAILED		0xffffffff
#define WAIT_OBJECT_0		0
#define WAIT_ABANDONED		STATUS_ABANDONED_WAIT_0
#define WAIT_ABANDONED_0	STATUS_ABANDONED_WAIT_0
#define WAIT_IO_COMPLETION	STATUS_USER_APC
#define WAIT_TIMEOUT		STATUS_TIMEOUT
#define STILL_ACTIVE            STATUS_PENDING

#define FILE_BEGIN              0
#define FILE_CURRENT            1
#define FILE_END                2

#define FILE_MAP_COPY                   0x00000001
#define FILE_MAP_WRITE                  0x00000002
#define FILE_MAP_READ                   0x00000004
#define FILE_MAP_ALL_ACCESS             0x000f001f
#define FILE_MAP_EXECUTE                0x00000020

#define MOVEFILE_REPLACE_EXISTING       0x00000001
#define MOVEFILE_COPY_ALLOWED           0x00000002
#define MOVEFILE_DELAY_UNTIL_REBOOT     0x00000004

#define REPLACEFILE_WRITE_THROUGH       0x00000001
#define REPLACEFILE_IGNORE_MERGE_ERRORS 0x00000002

#define FS_CASE_SENSITIVE               FILE_CASE_SENSITIVE_SEARCH
#define FS_CASE_IS_PRESERVED            FILE_CASE_PRESERVED_NAMES
#define FS_UNICODE_STORED_ON_DISK       FILE_UNICODE_ON_DISK
#define FS_PERSISTENT_ACLS              FILE_PERSISTENT_ACLS
#define FS_VOL_IS_COMPRESSED            FILE_VOLUME_IS_COMPRESSED
#define FS_FILE_COMPRESSION             FILE_FILE_COMPRESSION

#define EXCEPTION_ACCESS_VIOLATION          STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT     STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT                STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP               STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND      STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT        STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION     STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW              STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK           STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW             STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO        STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW              STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION          STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR             STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION       STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW            STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION       STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE                STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE            STATUS_INVALID_HANDLE
#define CONTROL_C_EXIT                      STATUS_CONTROL_C_EXIT

#define HANDLE_FLAG_INHERIT             0x00000001
#define HANDLE_FLAG_PROTECT_FROM_CLOSE  0x00000002

#define HINSTANCE_ERROR 32

#define THREAD_PRIORITY_LOWEST          THREAD_BASE_PRIORITY_MIN
#define THREAD_PRIORITY_BELOW_NORMAL    (THREAD_PRIORITY_LOWEST+1)
#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_HIGHEST         THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL    (THREAD_PRIORITY_HIGHEST-1)
#define THREAD_PRIORITY_ERROR_RETURN    (0x7fffffff)
#define THREAD_PRIORITY_TIME_CRITICAL   THREAD_BASE_PRIORITY_LOWRT
#define THREAD_PRIORITY_IDLE            THREAD_BASE_PRIORITY_IDLE

/* flags to FormatMessage */
#define	FORMAT_MESSAGE_ALLOCATE_BUFFER	0x00000100
#define	FORMAT_MESSAGE_IGNORE_INSERTS	0x00000200
#define	FORMAT_MESSAGE_FROM_STRING	0x00000400
#define	FORMAT_MESSAGE_FROM_HMODULE	0x00000800
#define	FORMAT_MESSAGE_FROM_SYSTEM	0x00001000
#define	FORMAT_MESSAGE_ARGUMENT_ARRAY	0x00002000
#define	FORMAT_MESSAGE_MAX_WIDTH_MASK	0x000000FF

/* flags to ACTCTX[AW] */
#define ACTCTX_FLAG_PROCESSOR_ARCHITECTURE_VALID  (0x00000001)
#define ACTCTX_FLAG_LANGID_VALID                  (0x00000002)
#define ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID      (0x00000004)
#define ACTCTX_FLAG_RESOURCE_NAME_VALID           (0x00000008)
#define ACTCTX_FLAG_SET_PROCESS_DEFAULT           (0x00000010)
#define ACTCTX_FLAG_APPLICATION_NAME_VALID        (0x00000020)
#define ACTCTX_FLAG_SOURCE_IS_ASSEMBLYREF         (0x00000040)
#define ACTCTX_FLAG_HMODULE_VALID                 (0x00000080)

/* flags to DeactiveActCtx */
#define DEACTIVATE_ACTCTX_FLAG_FORCE_EARLY_DEACTIVATION  (0x00000001)

/* flags to FindActCtxSection{Guid,String[AW]} */
#define FIND_ACTCTX_SECTION_KEY_RETURN_HACTCTX            (0x00000001)
#define FIND_ACTCTX_SECTION_KEY_RETURN_FLAGS              (0x00000002)
#define FIND_ACTCTX_SECTION_KEY_RETURN_ASSEMBLY_METADATA  (0x00000004)

/* flags to QueryActCtxW */
#define QUERY_ACTCTX_FLAG_USE_ACTIVE_ACTCTX  (0x00000004)
#define QUERY_ACTCTX_FLAG_ACTCTX_IS_HMODULE  (0x00000008)
#define QUERY_ACTCTX_FLAG_ACTCTX_IS_ADDRESS  (0x00000010)
#define QUERY_ACTCTX_FLAG_NO_ADDREF          (0x80000000)

typedef struct tagACTCTXA {
    ULONG   cbSize;
    DWORD   dwFlags;
    LPCSTR  lpSource;
    USHORT  wProcessorArchitecture;
    LANGID  wLangId;
    LPCSTR  lpAssemblyDirectory;
    LPCSTR  lpResourceName;
    LPCSTR  lpApplicationName;
    HMODULE hModule;
} ACTCTXA, *PACTCTXA;

typedef struct tagACTCTXW {
    ULONG   cbSize;
    DWORD   dwFlags;
    LPCWSTR lpSource;
    USHORT  wProcessorArchitecture;
    LANGID  wLangId;
    LPCWSTR lpAssemblyDirectory;
    LPCWSTR lpResourceName;
    LPCWSTR lpApplicationName;
    HMODULE hModule;
} ACTCTXW, *PACTCTXW;

DECL_WINELIB_TYPE_AW(ACTCTX)
DECL_WINELIB_TYPE_AW(PACTCTX)

typedef const ACTCTXA *PCACTCTXA;
typedef const ACTCTXW *PCACTCTXW;
DECL_WINELIB_TYPE_AW(PCACTCTX)

typedef struct tagACTCTX_SECTION_KEYED_DATA_2600 {
    ULONG  cbSize;
    ULONG  ulDataFormatVersion;
    PVOID  lpData;
    ULONG  ulLength;
    PVOID  lpSectionGlobalData;
    ULONG  ulSectionGlobalDataLength;
    PVOID  lpSectionBase;
    ULONG  ulSectionTotalLength;
    HANDLE hActCtx;
    ULONG  ulAssemblyRosterIndex;
} ACTCTX_SECTION_KEYED_DATA_2600, *PACTCTX_SECTION_KEYED_DATA_2600;
typedef const ACTCTX_SECTION_KEYED_DATA_2600 *PCACTCTX_SECTION_KEYED_DATA_2600;

typedef struct tagACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA {
    PVOID lpInformation;
    PVOID lpSectionBase;
    ULONG ulSectionLength;
    PVOID lpSectionGlobalDataBase;
    ULONG ulSectionGlobalDataLength;
} ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA, *PACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA;
typedef const ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA *PCACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA;

typedef struct tagACTCTX_SECTION_KEYED_DATA {
    ULONG  cbSize;
    ULONG  ulDataFormatVersion;
    PVOID  lpData;
    ULONG  ulLength;
    PVOID  lpSectionGlobalData;
    ULONG  ulSectionGlobalDataLength;
    PVOID  lpSectionBase;
    ULONG  ulSectionTotalLength;
    HANDLE hActCtx;
    ULONG  ulAssemblyRosterIndex;

    /* Non 2600 extra fields */
    ULONG ulFlags;
    ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA AssemblyMetadata;
} ACTCTX_SECTION_KEYED_DATA, *PACTCTX_SECTION_KEYED_DATA;
typedef const ACTCTX_SECTION_KEYED_DATA *PCACTCTX_SECTION_KEYED_DATA;

typedef struct _ACTIVATION_CONTEXT_BASIC_INFORMATION {
    HANDLE hActCtx;
    DWORD  dwFlags;
} ACTIVATION_CONTEXT_BASIC_INFORMATION, *PACTIVATION_CONTEXT_BASIC_INFORMATION;

typedef BOOL (WINAPI *PQUERYACTCTXW_FUNC)(DWORD,HANDLE,PVOID,ULONG,PVOID,SIZE_T,SIZE_T *);

typedef struct tagCOMSTAT
{
    DWORD fCtsHold : 1;
    DWORD fDsrHold : 1;
    DWORD fRlsdHold : 1;
    DWORD fXoffHold : 1;
    DWORD fXoffSent : 1;
    DWORD fEof : 1;
    DWORD fTxim : 1;
    DWORD fReserved : 25;
    DWORD cbInQue;
    DWORD cbOutQue;
} COMSTAT, *LPCOMSTAT;

typedef struct tagDCB
{
    DWORD DCBlength;
    DWORD BaudRate;
    unsigned fBinary               :1;
    unsigned fParity               :1;
    unsigned fOutxCtsFlow          :1;
    unsigned fOutxDsrFlow          :1;
    unsigned fDtrControl           :2;
    unsigned fDsrSensitivity       :1;
    unsigned fTXContinueOnXoff     :1;
    unsigned fOutX                 :1;
    unsigned fInX                  :1;
    unsigned fErrorChar            :1;
    unsigned fNull                 :1;
    unsigned fRtsControl           :2;
    unsigned fAbortOnError         :1;
    unsigned fDummy2               :17;
    WORD wReserved;
    WORD XonLim;
    WORD XoffLim;
    BYTE ByteSize;
    BYTE Parity;
    BYTE StopBits;
    char XonChar;
    char XoffChar;
    char ErrorChar;
    char EofChar;
    char EvtChar;
    WORD wReserved1;
} DCB, *LPDCB;

typedef struct tagCOMMCONFIG {
	DWORD dwSize;
	WORD  wVersion;
	WORD  wReserved;
	DCB   dcb;
	DWORD dwProviderSubType;
	DWORD dwProviderOffset;
	DWORD dwProviderSize;
	DWORD wcProviderData[1];
} COMMCONFIG, *LPCOMMCONFIG;

typedef struct tagCOMMPROP {
	WORD  wPacketLength;
	WORD  wPacketVersion;
	DWORD dwServiceMask;
	DWORD dwReserved1;
	DWORD dwMaxTxQueue;
	DWORD dwMaxRxQueue;
	DWORD dwMaxBaud;
	DWORD dwProvSubType;
	DWORD dwProvCapabilities;
	DWORD dwSettableParams;
	DWORD dwSettableBaud;
	WORD  wSettableData;
	WORD  wSettableStopParity;
	DWORD dwCurrentTxQueue;
	DWORD dwCurrentRxQueue;
	DWORD dwProvSpec1;
	DWORD dwProvSpec2;
	WCHAR wcProvChar[1];
} COMMPROP, *LPCOMMPROP;

#define SP_SERIALCOMM ((DWORD)1)

#define BAUD_075     ((DWORD)0x01)
#define BAUD_110     ((DWORD)0x02)
#define BAUD_134_5   ((DWORD)0x04)
#define BAUD_150     ((DWORD)0x08)
#define BAUD_300     ((DWORD)0x10)
#define BAUD_600     ((DWORD)0x20)
#define BAUD_1200    ((DWORD)0x40)
#define BAUD_1800    ((DWORD)0x80)
#define BAUD_2400    ((DWORD)0x100)
#define BAUD_4800    ((DWORD)0x200)
#define BAUD_7200    ((DWORD)0x400)
#define BAUD_9600    ((DWORD)0x800)
#define BAUD_14400   ((DWORD)0x1000)
#define BAUD_19200   ((DWORD)0x2000)
#define BAUD_38400   ((DWORD)0x4000)
#define BAUD_56K     ((DWORD)0x8000)
#define BAUD_57600   ((DWORD)0x40000)
#define BAUD_115200  ((DWORD)0x20000)
#define BAUD_128K    ((DWORD)0x10000)
#define BAUD_USER    ((DWORD)0x10000000)

#define PST_FAX            ((DWORD)0x21)
#define PST_LAT            ((DWORD)0x101)
#define PST_MODEM          ((DWORD)0x06)
#define PST_NETWORK_BRIDGE ((DWORD)0x100)
#define PST_PARALLEL_PORT  ((DWORD)0x02)
#define PST_RS232          ((DWORD)0x01)
#define PST_RS442          ((DWORD)0x03)
#define PST_RS423          ((DWORD)0x04)
#define PST_RS449          ((DWORD)0x06)
#define PST_SCANNER        ((DWORD)0x22)
#define PST_TCPIP_TELNET   ((DWORD)0x102)
#define PST_UNSPECIFIED    ((DWORD)0x00)
#define PST_X25            ((DWORD)0x103)

#define PCF_16BITMODE     ((DWORD)0x200)
#define PCF_DTRDSR        ((DWORD)0x01)
#define PCF_INTTIMEOUTS   ((DWORD)0x80)
#define PCF_PARITY_CHECK  ((DWORD)0x08)
#define PCF_RLSD          ((DWORD)0x04)
#define PCF_RTSCTS        ((DWORD)0x02)
#define PCF_SETXCHAR      ((DWORD)0x20)
#define PCF_SPECIALCHARS  ((DWORD)0x100)
#define PCF_TOTALTIMEOUTS ((DWORD)0x40)
#define PCF_XONXOFF       ((DWORD)0x10)

#define SP_BAUD         ((DWORD)0x02)
#define SP_DATABITS     ((DWORD)0x04)
#define SP_HANDSHAKING  ((DWORD)0x10)
#define SP_PARITY       ((DWORD)0x01)
#define SP_PARITY_CHECK ((DWORD)0x20)
#define SP_RLSD         ((DWORD)0x40)
#define SP_STOPBITS     ((DWORD)0x08)

#define DATABITS_5   ((DWORD)0x01)
#define DATABITS_6   ((DWORD)0x02)
#define DATABITS_7   ((DWORD)0x04)
#define DATABITS_8   ((DWORD)0x08)
#define DATABITS_16  ((DWORD)0x10)
#define DATABITS_16X ((DWORD)0x20)

#define STOPBITS_10 ((DWORD)1)
#define STOPBITS_15 ((DWORD)2)
#define STOPBITS_20 ((DWORD)4)

#define PARITY_NONE  ((DWORD)0x100)
#define PARITY_ODD   ((DWORD)0x200)
#define PARITY_EVEN  ((DWORD)0x400)
#define PARITY_MARK  ((DWORD)0x800)
#define PARITY_SPACE ((DWORD)0x1000)

typedef struct tagCOMMTIMEOUTS {
	DWORD	ReadIntervalTimeout;
	DWORD	ReadTotalTimeoutMultiplier;
	DWORD	ReadTotalTimeoutConstant;
	DWORD	WriteTotalTimeoutMultiplier;
	DWORD	WriteTotalTimeoutConstant;
} COMMTIMEOUTS,*LPCOMMTIMEOUTS;

#define GET_TAPE_MEDIA_INFORMATION 0
#define GET_TAPE_DRIVE_INFORMATION 1
#define SET_TAPE_MEDIA_INFORMATION 0
#define SET_TAPE_DRIVE_INFORMATION 1

typedef void (CALLBACK *PAPCFUNC)(ULONG_PTR);
typedef void (CALLBACK *PTIMERAPCROUTINE)(LPVOID,DWORD,DWORD);

typedef enum _COMPUTER_NAME_FORMAT
{
	ComputerNameNetBIOS,
	ComputerNameDnsHostname,
	ComputerNameDnsDomain,
	ComputerNameDnsFullyQualified,
	ComputerNamePhysicalNetBIOS,
	ComputerNamePhysicalDnsHostname,
	ComputerNamePhysicalDnsDomain,
	ComputerNamePhysicalDnsFullyQualified,
	ComputerNameMax
} COMPUTER_NAME_FORMAT;

#define HW_PROFILE_GUIDLEN	39
#define MAX_PROFILE_LEN		80

#define DOCKINFO_UNDOCKED	0x1
#define DOCKINFO_DOCKED		0x2
#define DOCKINFO_USER_SUPPLIED	0x4
#define DOCKINFO_USER_UNDOCKED	(DOCKINFO_USER_SUPPLIED | DOCKINFO_UNDOCKED)
#define DOCKINFO_USER_DOCKED	(DOCKINFO_USER_SUPPLIED | DOCKINFO_DOCKED)

typedef struct tagHW_PROFILE_INFOA {
    DWORD dwDockInfo;
    CHAR  szHwProfileGuid[HW_PROFILE_GUIDLEN];
    CHAR  szHwProfileName[MAX_PROFILE_LEN];
} HW_PROFILE_INFOA, *LPHW_PROFILE_INFOA;

typedef struct tagHW_PROFILE_INFOW {
    DWORD dwDockInfo;
    WCHAR szHwProfileGuid[HW_PROFILE_GUIDLEN];
    WCHAR szHwProfileName[MAX_PROFILE_LEN];
} HW_PROFILE_INFOW, *LPHW_PROFILE_INFOW;

DECL_WINELIB_TYPE_AW(HW_PROFILE_INFO)
DECL_WINELIB_TYPE_AW(LPHW_PROFILE_INFO)

/* Stream data structures and defines */
/*the types of backup data -- WIN32_STREAM_ID.dwStreamId below*/
#define BACKUP_INVALID        0
#define BACKUP_DATA           1
#define BACKUP_EA_DATA        2
#define BACKUP_SECURITY_DATA  3
#define BACKUP_ALTERNATE_DATA 4
#define BACKUP_LINK           5
#define BACKUP_PROPERTY_DATA  6
#define BACKUP_OBJECT_ID      7
#define BACKUP_REPARSE_DATA   8
#define BACKUP_SPARSE_BLOCK   9

/*flags for WIN32_STREAM_ID.dwStreamAttributes below*/
#define STREAM_NORMAL_ATTRIBUTE    0
#define STREAM_MODIFIED_WHEN_READ  1
#define STREAM_CONTAINS_SECURITY   2
#define STREAM_CONTAINS_PROPERTIES 4
#define STREAM_SPARSE_ATTRIBUTE    8

#include <pshpack8.h>
typedef struct _WIN32_STREAM_ID {
	DWORD   dwStreamId;
	DWORD   dwStreamAttributes;
	LARGE_INTEGER DECLSPEC_ALIGN(8) Size;
	DWORD   dwStreamNameSize;
	WCHAR   cStreamName[ANYSIZE_ARRAY];
} WIN32_STREAM_ID, *LPWIN32_STREAM_ID;
#include <poppack.h>


/* GetBinaryType return values.
 */

#define SCS_32BIT_BINARY    0
#define SCS_DOS_BINARY      1
#define SCS_WOW_BINARY      2
#define SCS_PIF_BINARY      3
#define SCS_POSIX_BINARY    4
#define SCS_OS216_BINARY    5
#define SCS_64BIT_BINARY    6

/* flags for DefineDosDevice */
#define DDD_RAW_TARGET_PATH         0x00000001
#define DDD_REMOVE_DEFINITION       0x00000002
#define DDD_EXACT_MATCH_ON_REMOVE   0x00000004
#define DDD_NO_BROADCAST_SYSTEM     0x00000008
#define DDD_LUID_BROADCAST_DRIVE    0x00000010

BOOL        WINAPI ActivateActCtx(HANDLE,ULONG_PTR *);
BOOL        WINAPI AddAccessAllowedAce(PACL,DWORD,DWORD,PSID);
BOOL        WINAPI AddAccessAllowedAceEx(PACL,DWORD,DWORD,DWORD,PSID);
BOOL        WINAPI AddAccessDeniedAce(PACL,DWORD,DWORD,PSID);
BOOL        WINAPI AddAccessDeniedAceEx(PACL,DWORD,DWORD,DWORD,PSID);
BOOL        WINAPI AddAce(PACL,DWORD,DWORD,LPVOID,DWORD);
ATOM        WINAPI AddAtomA(LPCSTR);
ATOM        WINAPI AddAtomW(LPCWSTR);
#define     AddAtom WINELIB_NAME_AW(AddAtom)
BOOL        WINAPI AddAuditAccessAce(PACL,DWORD,DWORD,PSID,BOOL,BOOL);
VOID        WINAPI AddRefActCtx(HANDLE);
PVOID       WINAPI AddVectoredExceptionHandler(ULONG,PVECTORED_EXCEPTION_HANDLER);
BOOL        WINAPI AdjustTokenGroups(HANDLE,BOOL,PTOKEN_GROUPS,DWORD,PTOKEN_GROUPS,PDWORD);
BOOL        WINAPI AttachThreadInput(DWORD,DWORD,BOOL);
BOOL        WINAPI AccessCheck(PSECURITY_DESCRIPTOR,HANDLE,DWORD,PGENERIC_MAPPING,PPRIVILEGE_SET,LPDWORD,LPDWORD,LPBOOL);
BOOL        WINAPI AccessCheckAndAuditAlarmA(LPCSTR,LPVOID,LPSTR,LPSTR,PSECURITY_DESCRIPTOR,DWORD,PGENERIC_MAPPING,BOOL,LPDWORD,LPBOOL,LPBOOL);
BOOL        WINAPI AccessCheckAndAuditAlarmW(LPCWSTR,LPVOID,LPWSTR,LPWSTR,PSECURITY_DESCRIPTOR,DWORD,PGENERIC_MAPPING,BOOL,LPDWORD,LPBOOL,LPBOOL);
#define     AccessCheckAndAuditAlarm WINELIB_NAME_AW(AccessCheckAndAuditAlarm)
BOOL        WINAPI AccessCheckByType(PSECURITY_DESCRIPTOR,PSID,HANDLE,DWORD,POBJECT_TYPE_LIST,DWORD,PGENERIC_MAPPING,PPRIVILEGE_SET,LPDWORD,LPDWORD,LPBOOL);
BOOL        WINAPI AdjustTokenPrivileges(HANDLE,BOOL,LPVOID,DWORD,LPVOID,LPDWORD);
BOOL        WINAPI AllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY,BYTE,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,PSID *);
BOOL        WINAPI AllocateLocallyUniqueId(PLUID);
BOOL        WINAPI AreAllAccessesGranted(DWORD,DWORD);
BOOL        WINAPI AreAnyAccessesGranted(DWORD,DWORD);
BOOL        WINAPI AreFileApisANSI(void);
BOOL        WINAPI AssignProcessToJobObject(HANDLE,HANDLE);
BOOL        WINAPI BackupEventLogA(HANDLE,LPCSTR);
BOOL        WINAPI BackupEventLogW(HANDLE,LPCWSTR);
#define     BackupEventLog WINELIB_NAME_AW(BackupEventLog)
BOOL        WINAPI BackupRead(HANDLE,LPBYTE,DWORD,LPDWORD,BOOL,BOOL,LPVOID*);
BOOL        WINAPI BackupSeek(HANDLE,DWORD,DWORD,LPDWORD,LPDWORD,LPVOID*);
BOOL        WINAPI BackupWrite(HANDLE,LPBYTE,DWORD,LPDWORD,BOOL,BOOL,LPVOID*);
BOOL        WINAPI Beep(DWORD,DWORD);
HANDLE      WINAPI BeginUpdateResourceA(LPCSTR,BOOL);
HANDLE      WINAPI BeginUpdateResourceW(LPCWSTR,BOOL);
#define     BeginUpdateResource WINELIB_NAME_AW(BeginUpdateResource)
BOOL        WINAPI BindIoCompletionCallback(HANDLE,LPOVERLAPPED_COMPLETION_ROUTINE,ULONG);
BOOL        WINAPI BuildCommDCBA(LPCSTR,LPDCB);
BOOL        WINAPI BuildCommDCBW(LPCWSTR,LPDCB);
#define     BuildCommDCB WINELIB_NAME_AW(BuildCommDCB)
BOOL        WINAPI BuildCommDCBAndTimeoutsA(LPCSTR,LPDCB,LPCOMMTIMEOUTS);
BOOL        WINAPI BuildCommDCBAndTimeoutsW(LPCWSTR,LPDCB,LPCOMMTIMEOUTS);
#define     BuildCommDCBAndTimeouts WINELIB_NAME_AW(BuildCommDCBAndTimeouts)
BOOL        WINAPI CallNamedPipeA(LPCSTR,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,DWORD);
BOOL        WINAPI CallNamedPipeW(LPCWSTR,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,DWORD);
#define     CallNamedPipe WINELIB_NAME_AW(CallNamedPipe)
BOOL        WINAPI CancelIo(HANDLE);
BOOL        WINAPI CancelWaitableTimer(HANDLE);
BOOL        WINAPI CheckTokenMembership(HANDLE,PSID,PBOOL);
BOOL        WINAPI ClearCommBreak(HANDLE);
BOOL        WINAPI ClearCommError(HANDLE,LPDWORD,LPCOMSTAT);
BOOL        WINAPI ClearEventLogA(HANDLE,LPCSTR);
BOOL        WINAPI ClearEventLogW(HANDLE,LPCWSTR);
#define     ClearEventLog WINELIB_NAME_AW(ClearEventLog)
BOOL        WINAPI CloseEventLog(HANDLE);
BOOL        WINAPI CloseHandle(HANDLE);
BOOL        WINAPI CommConfigDialogA(LPCSTR,HWND,LPCOMMCONFIG);
BOOL        WINAPI CommConfigDialogW(LPCWSTR,HWND,LPCOMMCONFIG);
#define     CommConfigDialog WINELIB_NAME_AW(CommConfigDialog)
BOOL        WINAPI ConnectNamedPipe(HANDLE,LPOVERLAPPED);
BOOL        WINAPI ContinueDebugEvent(DWORD,DWORD,DWORD);
HANDLE      WINAPI ConvertToGlobalHandle(HANDLE hSrc);
BOOL        WINAPI CopyFileA(LPCSTR,LPCSTR,BOOL);
BOOL        WINAPI CopyFileW(LPCWSTR,LPCWSTR,BOOL);
#define     CopyFile WINELIB_NAME_AW(CopyFile)
BOOL        WINAPI CopyFileExA(LPCSTR, LPCSTR, LPPROGRESS_ROUTINE, LPVOID, LPBOOL, DWORD);
BOOL        WINAPI CopyFileExW(LPCWSTR, LPCWSTR, LPPROGRESS_ROUTINE, LPVOID, LPBOOL, DWORD);
#define     CopyFileEx WINELIB_NAME_AW(CopyFileEx)
BOOL        WINAPI CopySid(DWORD,PSID,PSID);
INT         WINAPI CompareFileTime(const FILETIME*,const FILETIME*);
BOOL        WINAPI ConvertFiberToThread(void);
LPVOID      WINAPI ConvertThreadToFiber(LPVOID);
LPVOID      WINAPI ConvertThreadToFiberEx(LPVOID,DWORD);
HANDLE      WINAPI CreateActCtxA(PCACTCTXA);
HANDLE      WINAPI CreateActCtxW(PCACTCTXW);
#define     CreateActCtx WINELIB_NAME_AW(CreateActCtx)
BOOL        WINAPI CreateDirectoryA(LPCSTR,LPSECURITY_ATTRIBUTES);
BOOL        WINAPI CreateDirectoryW(LPCWSTR,LPSECURITY_ATTRIBUTES);
#define     CreateDirectory WINELIB_NAME_AW(CreateDirectory)
BOOL        WINAPI CreateDirectoryExA(LPCSTR,LPCSTR,LPSECURITY_ATTRIBUTES);
BOOL        WINAPI CreateDirectoryExW(LPCWSTR,LPCWSTR,LPSECURITY_ATTRIBUTES);
#define     CreateDirectoryEx WINELIB_NAME_AW(CreateDirectoryEx)
HANDLE      WINAPI CreateEventA(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCSTR);
HANDLE      WINAPI CreateEventW(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCWSTR);
#define     CreateEvent WINELIB_NAME_AW(CreateEvent)
LPVOID      WINAPI CreateFiber(SIZE_T,LPFIBER_START_ROUTINE,LPVOID);
LPVOID      WINAPI CreateFiberEx(SIZE_T,SIZE_T,DWORD,LPFIBER_START_ROUTINE,LPVOID);
HANDLE      WINAPI CreateFileA(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
HANDLE      WINAPI CreateFileW(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
#define     CreateFile WINELIB_NAME_AW(CreateFile)
HANDLE      WINAPI CreateFileMappingA(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,DWORD,DWORD,LPCSTR);
HANDLE      WINAPI CreateFileMappingW(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,DWORD,DWORD,LPCWSTR);
#define     CreateFileMapping WINELIB_NAME_AW(CreateFileMapping)
HANDLE      WINAPI CreateIoCompletionPort(HANDLE,HANDLE,ULONG_PTR,DWORD);
HANDLE      WINAPI CreateJobObjectA(LPSECURITY_ATTRIBUTES,LPCSTR);
HANDLE      WINAPI CreateJobObjectW(LPSECURITY_ATTRIBUTES,LPCWSTR);
#define     CreateJobObject WINELIB_NAME_AW(CreateJobObject)
HANDLE      WINAPI CreateMailslotA(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
HANDLE      WINAPI CreateMailslotW(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
#define     CreateMailslot WINELIB_NAME_AW(CreateMailslot)
HANDLE      WINAPI CreateMutexA(LPSECURITY_ATTRIBUTES,BOOL,LPCSTR);
HANDLE      WINAPI CreateMutexW(LPSECURITY_ATTRIBUTES,BOOL,LPCWSTR);
#define     CreateMutex WINELIB_NAME_AW(CreateMutex)
HANDLE      WINAPI CreateNamedPipeA(LPCSTR,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
HANDLE      WINAPI CreateNamedPipeW(LPCWSTR,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
#define     CreateNamedPipe WINELIB_NAME_AW(CreateNamedPipe)
BOOL        WINAPI CreatePipe(PHANDLE,PHANDLE,LPSECURITY_ATTRIBUTES,DWORD);
BOOL        WINAPI CreatePrivateObjectSecurity(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,
                                               PSECURITY_DESCRIPTOR*,BOOL,HANDLE,PGENERIC_MAPPING);
BOOL        WINAPI CreateProcessA(LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,
                                  LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,
                                  LPSTARTUPINFOA,LPPROCESS_INFORMATION);
BOOL        WINAPI CreateProcessW(LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,
                                  LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCWSTR,
                                  LPSTARTUPINFOW,LPPROCESS_INFORMATION);
#define     CreateProcess WINELIB_NAME_AW(CreateProcess)
BOOL        WINAPI CreateProcessAsUserA(HANDLE,LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,
                                        BOOL,DWORD,LPVOID,LPCSTR,LPSTARTUPINFOA,LPPROCESS_INFORMATION);
BOOL        WINAPI CreateProcessAsUserW(HANDLE,LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,
                                        BOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION);
#define     CreateProcessAsUser WINELIB_NAME_AW(CreateProcessAsUser)
HANDLE      WINAPI CreateRemoteThread(HANDLE,LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
HANDLE      WINAPI CreateSemaphoreA(LPSECURITY_ATTRIBUTES,LONG,LONG,LPCSTR);
HANDLE      WINAPI CreateSemaphoreW(LPSECURITY_ATTRIBUTES,LONG,LONG,LPCWSTR);
#define     CreateSemaphore WINELIB_NAME_AW(CreateSemaphore)
DWORD       WINAPI CreateTapePartition(HANDLE,DWORD,DWORD,DWORD);
HANDLE      WINAPI CreateThread(LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
HANDLE      WINAPI CreateTimerQueue(void);
BOOL        WINAPI CreateTimerQueueTimer(PHANDLE,HANDLE,WAITORTIMERCALLBACK,PVOID,DWORD,DWORD,ULONG);
HANDLE      WINAPI CreateWaitableTimerA(LPSECURITY_ATTRIBUTES,BOOL,LPCSTR);
HANDLE      WINAPI CreateWaitableTimerW(LPSECURITY_ATTRIBUTES,BOOL,LPCWSTR);
#define     CreateWaitableTimer WINELIB_NAME_AW(CreateWaitableTimer)
BOOL        WINAPI CreateWellKnownSid(WELL_KNOWN_SID_TYPE,PSID,PSID,DWORD*);
BOOL        WINAPI DeactivateActCtx(DWORD,ULONG_PTR);
BOOL        WINAPI DebugActiveProcess(DWORD);
BOOL        WINAPI DebugActiveProcessStop(DWORD);
void        WINAPI DebugBreak(void);
BOOL        WINAPI DebugBreakProcess(HANDLE);
BOOL        WINAPI DebugSetProcessKillOnExit(BOOL);
BOOL        WINAPI DecryptFileA(LPCSTR,DWORD);
BOOL        WINAPI DecryptFileW(LPCWSTR,DWORD);
#define     DecryptFile WINELIB_NAME_AW(DecryptFile)
BOOL        WINAPI DefineDosDeviceA(DWORD,LPCSTR,LPCSTR);
BOOL        WINAPI DefineDosDeviceW(DWORD,LPCWSTR,LPCWSTR);
#define     DefineDosDevice WINELIB_NAME_AW(DefineDosDevice)
#define     DefineHandleTable(w) ((w),TRUE)
BOOL        WINAPI DeleteAce(PACL,DWORD);
ATOM        WINAPI DeleteAtom(ATOM);
void        WINAPI DeleteCriticalSection(CRITICAL_SECTION *lpCrit);
void        WINAPI DeleteFiber(LPVOID);
BOOL        WINAPI DeleteFileA(LPCSTR);
BOOL        WINAPI DeleteFileW(LPCWSTR);
#define     DeleteFile WINELIB_NAME_AW(DeleteFile)
BOOL        WINAPI DeleteTimerQueueEx(HANDLE,HANDLE);
BOOL        WINAPI DeleteTimerQueueTimer(HANDLE,HANDLE,HANDLE);
BOOL        WINAPI DeleteVolumeMountPointA(LPCSTR);
BOOL        WINAPI DeleteVolumeMountPointW(LPCWSTR);
#define     DeleteVolumeMountPoint WINELIB_NAME_AW(DeleteVolumeMountPoint)
BOOL        WINAPI DeregisterEventSource(HANDLE);
BOOL        WINAPI DestroyPrivateObjectSecurity(PSECURITY_DESCRIPTOR*);
BOOL        WINAPI DeviceIoControl(HANDLE,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI DisableThreadLibraryCalls(HMODULE);
BOOL        WINAPI DisconnectNamedPipe(HANDLE);
BOOL        WINAPI DnsHostnameToComputerNameA(LPCSTR,LPSTR,LPDWORD);
BOOL        WINAPI DnsHostnameToComputerNameW(LPCWSTR,LPWSTR,LPDWORD);
#define     DnsHostnameToComputerName WINELIB_NAME_AW(DnsHostnameToComputerName)
BOOL        WINAPI DosDateTimeToFileTime(WORD,WORD,LPFILETIME);
BOOL        WINAPI DuplicateHandle(HANDLE,HANDLE,HANDLE,HANDLE*,DWORD,BOOL,DWORD);
BOOL        WINAPI DuplicateToken(HANDLE,SECURITY_IMPERSONATION_LEVEL,PHANDLE);
BOOL        WINAPI DuplicateTokenEx(HANDLE,DWORD,LPSECURITY_ATTRIBUTES,SECURITY_IMPERSONATION_LEVEL,
                                    TOKEN_TYPE,PHANDLE);
BOOL        WINAPI EscapeCommFunction(HANDLE,UINT);
BOOL        WINAPI EncryptFileA(LPCSTR);
BOOL        WINAPI EncryptFileW(LPCWSTR);
#define     EncryptFile WINELIB_NAME_AW(EncryptFile)
BOOL        WINAPI EndUpdateResourceA(HANDLE,BOOL);
BOOL        WINAPI EndUpdateResourceW(HANDLE,BOOL);
#define     EndUpdateResource WINELIB_NAME_AW(EndUpdateResource)
void        WINAPI EnterCriticalSection(CRITICAL_SECTION *lpCrit);
BOOL        WINAPI EnumResourceLanguagesA(HMODULE,LPCSTR,LPCSTR,ENUMRESLANGPROCA,LONG_PTR);
BOOL        WINAPI EnumResourceLanguagesW(HMODULE,LPCWSTR,LPCWSTR,ENUMRESLANGPROCW,LONG_PTR);
#define     EnumResourceLanguages WINELIB_NAME_AW(EnumResourceLanguages)
BOOL        WINAPI EnumResourceNamesA(HMODULE,LPCSTR,ENUMRESNAMEPROCA,LONG_PTR);
BOOL        WINAPI EnumResourceNamesW(HMODULE,LPCWSTR,ENUMRESNAMEPROCW,LONG_PTR);
#define     EnumResourceNames WINELIB_NAME_AW(EnumResourceNames)
BOOL        WINAPI EnumResourceTypesA(HMODULE,ENUMRESTYPEPROCA,LONG_PTR);
BOOL        WINAPI EnumResourceTypesW(HMODULE,ENUMRESTYPEPROCW,LONG_PTR);
#define     EnumResourceTypes WINELIB_NAME_AW(EnumResourceTypes)
BOOL        WINAPI EqualSid(PSID, PSID);
BOOL        WINAPI EqualPrefixSid(PSID,PSID);
DWORD       WINAPI EraseTape(HANDLE,DWORD,BOOL);
VOID DECLSPEC_NORETURN WINAPI ExitProcess(DWORD);
VOID DECLSPEC_NORETURN WINAPI ExitThread(DWORD);
DWORD       WINAPI ExpandEnvironmentStringsA(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI ExpandEnvironmentStringsW(LPCWSTR,LPWSTR,DWORD);
#define     ExpandEnvironmentStrings WINELIB_NAME_AW(ExpandEnvironmentStrings)
void        WINAPI FatalAppExitA(UINT,LPCSTR);
void        WINAPI FatalAppExitW(UINT,LPCWSTR);
void        WINAPI FatalExit(int);
#define     FatalAppExit WINELIB_NAME_AW(FatalAppExit)
BOOL        WINAPI FileEncryptionStatusA(LPCSTR,LPDWORD);
BOOL        WINAPI FileEncryptionStatusW(LPCWSTR,LPDWORD);
#define     FileEncryptionStatus WINELIB_NAME_AW(FileEncryptionStatus)
BOOL        WINAPI FileTimeToDosDateTime(const FILETIME*,LPWORD,LPWORD);
BOOL        WINAPI FileTimeToLocalFileTime(const FILETIME*,LPFILETIME);
BOOL        WINAPI FileTimeToSystemTime(const FILETIME*,LPSYSTEMTIME);
BOOL        WINAPI FindActCtxSectionStringA(DWORD,const GUID *,ULONG,LPCSTR,PACTCTX_SECTION_KEYED_DATA);
BOOL        WINAPI FindActCtxSectionStringW(DWORD,const GUID *,ULONG,LPCWSTR,PACTCTX_SECTION_KEYED_DATA);
#define     FindActCtxSectionString WINELIB_NAME_AW(FindActCtxSectionString)
BOOL        WINAPI FindActCtxSectionGuid(DWORD,const GUID *,ULONG,const GUID *,PACTCTX_SECTION_KEYED_DATA);
ATOM        WINAPI FindAtomA(LPCSTR);
ATOM        WINAPI FindAtomW(LPCWSTR);
#define     FindAtom WINELIB_NAME_AW(FindAtom)
BOOL        WINAPI FindClose(HANDLE);
HANDLE      WINAPI FindFirstChangeNotificationA(LPCSTR,BOOL,DWORD);
HANDLE      WINAPI FindFirstChangeNotificationW(LPCWSTR,BOOL,DWORD);
#define     FindFirstChangeNotification WINELIB_NAME_AW(FindFirstChangeNotification)
HANDLE      WINAPI FindFirstFileA(LPCSTR,LPWIN32_FIND_DATAA);
HANDLE      WINAPI FindFirstFileW(LPCWSTR,LPWIN32_FIND_DATAW);
#define     FindFirstFile WINELIB_NAME_AW(FindFirstFile)
HANDLE      WINAPI FindFirstFileExA(LPCSTR,FINDEX_INFO_LEVELS,LPVOID,FINDEX_SEARCH_OPS,LPVOID,DWORD);
HANDLE      WINAPI FindFirstFileExW(LPCWSTR,FINDEX_INFO_LEVELS,LPVOID,FINDEX_SEARCH_OPS,LPVOID,DWORD);
#define     FindFirstFileEx WINELIB_NAME_AW(FindFirstFileEx)
BOOL        WINAPI FindFirstFreeAce(PACL,LPVOID*);
BOOL        WINAPI FindNextChangeNotification(HANDLE);
BOOL        WINAPI FindNextFileA(HANDLE,LPWIN32_FIND_DATAA);
BOOL        WINAPI FindNextFileW(HANDLE,LPWIN32_FIND_DATAW);
#define     FindNextFile WINELIB_NAME_AW(FindNextFile)
BOOL        WINAPI FindCloseChangeNotification(HANDLE);
HRSRC       WINAPI FindResourceA(HMODULE,LPCSTR,LPCSTR);
HRSRC       WINAPI FindResourceW(HMODULE,LPCWSTR,LPCWSTR);
#define     FindResource WINELIB_NAME_AW(FindResource)
HRSRC       WINAPI FindResourceExA(HMODULE,LPCSTR,LPCSTR,WORD);
HRSRC       WINAPI FindResourceExW(HMODULE,LPCWSTR,LPCWSTR,WORD);
#define     FindResourceEx WINELIB_NAME_AW(FindResourceEx)
HANDLE      WINAPI FindFirstVolumeA(LPSTR,DWORD);
HANDLE      WINAPI FindFirstVolumeW(LPWSTR,DWORD);
#define     FindFirstVolume WINELIB_NAME_AW(FindFirstVolume)
HANDLE      WINAPI FindFirstVolumeMountPointA(LPCSTR,LPSTR,DWORD);
HANDLE      WINAPI FindFirstVolumeMountPointW(LPCWSTR,LPWSTR,DWORD);
#define     FindFirstVolumeMountPoint WINELIB_NAME_AW(FindFirstVolumeMountPoint)
BOOL        WINAPI FindNextVolumeA(HANDLE,LPSTR,DWORD);
BOOL        WINAPI FindNextVolumeW(HANDLE,LPWSTR,DWORD);
#define     FindNextVolume WINELIB_NAME_AW(FindNextVolume)
BOOL        WINAPI FindNextVolumeMountPointA(HANDLE,LPSTR,DWORD);
BOOL        WINAPI FindNextVolumeMountPointW(HANDLE,LPWSTR,DWORD);
#define     FindNextVolumeMountPoint WINELIB_NAME_AW(FindNextVolumeMountPoint)
BOOL        WINAPI FindVolumeClose(HANDLE);
BOOL        WINAPI FindVolumeMountPointClose(HANDLE);
BOOL        WINAPI FlushFileBuffers(HANDLE);
BOOL        WINAPI FlushInstructionCache(HANDLE,LPCVOID,SIZE_T);
BOOL        WINAPI FlushViewOfFile(LPCVOID,SIZE_T);
DWORD       WINAPI FormatMessageA(DWORD,LPCVOID,DWORD,DWORD,LPSTR,DWORD,va_list*);
DWORD       WINAPI FormatMessageW(DWORD,LPCVOID,DWORD,DWORD,LPWSTR,DWORD,va_list*);
#define     FormatMessage WINELIB_NAME_AW(FormatMessage)
BOOL        WINAPI FreeEnvironmentStringsA(LPSTR);
BOOL        WINAPI FreeEnvironmentStringsW(LPWSTR);
#define     FreeEnvironmentStrings WINELIB_NAME_AW(FreeEnvironmentStrings)
BOOL        WINAPI FreeLibrary(HMODULE);
VOID        WINAPI FreeLibraryAndExitThread(HINSTANCE,DWORD);
#define     FreeModule(handle) FreeLibrary(handle)
#define     FreeProcInstance(proc) /*nothing*/
BOOL        WINAPI FreeResource(HGLOBAL);
PVOID       WINAPI FreeSid(PSID);
BOOL        WINAPI GetAce(PACL,DWORD,LPVOID*);
BOOL        WINAPI GetAclInformation(PACL,LPVOID,DWORD,ACL_INFORMATION_CLASS);
UINT        WINAPI GetAtomNameA(ATOM,LPSTR,INT);
UINT        WINAPI GetAtomNameW(ATOM,LPWSTR,INT);
#define     GetAtomName WINELIB_NAME_AW(GetAtomName)
BOOL        WINAPI GetBinaryTypeA( LPCSTR lpApplicationName, LPDWORD lpBinaryType );
BOOL        WINAPI GetBinaryTypeW( LPCWSTR lpApplicationName, LPDWORD lpBinaryType );
#define     GetBinaryType WINELIB_NAME_AW(GetBinaryType)
BOOL        WINAPI GetCurrentActCtx(HANDLE *);
BOOL        WINAPI GetCommConfig(HANDLE,LPCOMMCONFIG,LPDWORD);
BOOL        WINAPI GetCommMask(HANDLE,LPDWORD);
BOOL        WINAPI GetCommModemStatus(HANDLE,LPDWORD);
BOOL        WINAPI GetCommProperties(HANDLE,LPCOMMPROP);
BOOL        WINAPI GetCommState(HANDLE,LPDCB);
BOOL        WINAPI GetCommTimeouts(HANDLE,LPCOMMTIMEOUTS);
LPSTR       WINAPI GetCommandLineA(void);
LPWSTR      WINAPI GetCommandLineW(void);
#define     GetCommandLine WINELIB_NAME_AW(GetCommandLine)
DWORD       WINAPI GetCompressedFileSizeA(LPCSTR,LPDWORD);
DWORD       WINAPI GetCompressedFileSizeW(LPCWSTR,LPDWORD);
#define     GetCompressedFileSize WINELIB_NAME_AW(GetCompressedFileSize)
BOOL        WINAPI GetComputerNameA(LPSTR,LPDWORD);
BOOL        WINAPI GetComputerNameW(LPWSTR,LPDWORD);
#define     GetComputerName WINELIB_NAME_AW(GetComputerName)
BOOL        WINAPI GetComputerNameExA(COMPUTER_NAME_FORMAT,LPSTR,LPDWORD);
BOOL        WINAPI GetComputerNameExW(COMPUTER_NAME_FORMAT,LPWSTR,LPDWORD);
#define     GetComputerNameEx WINELIB_NAME_AW(GetComputerNameEx)
UINT        WINAPI GetCurrentDirectoryA(UINT,LPSTR);
UINT        WINAPI GetCurrentDirectoryW(UINT,LPWSTR);
#define     GetCurrentDirectory WINELIB_NAME_AW(GetCurrentDirectory)
BOOL        WINAPI GetCurrentHwProfileA(LPHW_PROFILE_INFOA);
BOOL        WINAPI GetCurrentHwProfileW(LPHW_PROFILE_INFOW);
#define     GetCurrentHwProfile WINELIB_NAME_AW(GetCurrentHwProfile)
HANDLE      WINAPI GetCurrentProcess(void);
HANDLE      WINAPI GetCurrentThread(void);
#define     GetCurrentTime() GetTickCount()
BOOL        WINAPI GetDefaultCommConfigA(LPCSTR,LPCOMMCONFIG,LPDWORD);
BOOL        WINAPI GetDefaultCommConfigW(LPCWSTR,LPCOMMCONFIG,LPDWORD);
#define     GetDefaultCommConfig WINELIB_NAME_AW(GetDefaultCommConfig)
BOOL        WINAPI GetDevicePowerState(HANDLE,BOOL*);
BOOL        WINAPI GetDiskFreeSpaceA(LPCSTR,LPDWORD,LPDWORD,LPDWORD,LPDWORD);
BOOL        WINAPI GetDiskFreeSpaceW(LPCWSTR,LPDWORD,LPDWORD,LPDWORD,LPDWORD);
#define     GetDiskFreeSpace WINELIB_NAME_AW(GetDiskFreeSpace)
BOOL        WINAPI GetDiskFreeSpaceExA(LPCSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);
BOOL        WINAPI GetDiskFreeSpaceExW(LPCWSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);
#define     GetDiskFreeSpaceEx WINELIB_NAME_AW(GetDiskFreeSpaceEx)
DWORD       WINAPI GetDllDirectoryA(DWORD,LPSTR);
DWORD       WINAPI GetDllDirectoryW(DWORD,LPWSTR);
#define     GetDllDirectory WINELIB_NAME_AW(GetDllDirectory)
UINT        WINAPI GetDriveTypeA(LPCSTR);
UINT        WINAPI GetDriveTypeW(LPCWSTR);
#define     GetDriveType WINELIB_NAME_AW(GetDriveType)
LPSTR       WINAPI GetEnvironmentStringsA(void);
LPWSTR      WINAPI GetEnvironmentStringsW(void);
#define     GetEnvironmentStrings WINELIB_NAME_AW(GetEnvironmentStrings)
DWORD       WINAPI GetEnvironmentVariableA(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI GetEnvironmentVariableW(LPCWSTR,LPWSTR,DWORD);
#define     GetEnvironmentVariable WINELIB_NAME_AW(GetEnvironmentVariable)
BOOL        WINAPI GetExitCodeProcess(HANDLE,LPDWORD);
BOOL        WINAPI GetExitCodeThread(HANDLE,LPDWORD);
DWORD       WINAPI GetFileAttributesA(LPCSTR);
DWORD       WINAPI GetFileAttributesW(LPCWSTR);
#define     GetFileAttributes WINELIB_NAME_AW(GetFileAttributes)
BOOL        WINAPI GetFileAttributesExA(LPCSTR,GET_FILEEX_INFO_LEVELS,LPVOID);
BOOL        WINAPI GetFileAttributesExW(LPCWSTR,GET_FILEEX_INFO_LEVELS,LPVOID);
#define     GetFileAttributesEx WINELIB_NAME_AW(GetFileAttributesEx)
BOOL        WINAPI GetFileInformationByHandle(HANDLE,BY_HANDLE_FILE_INFORMATION*);
BOOL        WINAPI GetFileSecurityA(LPCSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,LPDWORD);
BOOL        WINAPI GetFileSecurityW(LPCWSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,LPDWORD);
#define     GetFileSecurity WINELIB_NAME_AW(GetFileSecurity)
DWORD       WINAPI GetFileSize(HANDLE,LPDWORD);
BOOL        WINAPI GetFileSizeEx(HANDLE,PLARGE_INTEGER);
BOOL        WINAPI GetFileTime(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME);
DWORD       WINAPI GetFileType(HANDLE);
#define     GetFreeSpace(w) (0x100000L)
DWORD       WINAPI GetFullPathNameA(LPCSTR,DWORD,LPSTR,LPSTR*);
DWORD       WINAPI GetFullPathNameW(LPCWSTR,DWORD,LPWSTR,LPWSTR*);
#define     GetFullPathName WINELIB_NAME_AW(GetFullPathName)
BOOL        WINAPI GetHandleInformation(HANDLE,LPDWORD);
BOOL        WINAPI GetKernelObjectSecurity(HANDLE,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,LPDWORD);
DWORD       WINAPI GetLengthSid(PSID);
VOID        WINAPI GetLocalTime(LPSYSTEMTIME);
DWORD       WINAPI GetLogicalDrives(void);
UINT        WINAPI GetLogicalDriveStringsA(UINT,LPSTR);
UINT        WINAPI GetLogicalDriveStringsW(UINT,LPWSTR);
#define     GetLogicalDriveStrings WINELIB_NAME_AW(GetLogicalDriveStrings)
DWORD       WINAPI GetLongPathNameA(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI GetLongPathNameW(LPCWSTR,LPWSTR,DWORD);
#define     GetLongPathName WINELIB_NAME_AW(GetLongPathName)
BOOL        WINAPI GetMailslotInfo(HANDLE,LPDWORD,LPDWORD,LPDWORD,LPDWORD);
DWORD       WINAPI GetModuleFileNameA(HMODULE,LPSTR,DWORD);
DWORD       WINAPI GetModuleFileNameW(HMODULE,LPWSTR,DWORD);
#define     GetModuleFileName WINELIB_NAME_AW(GetModuleFileName)
HMODULE     WINAPI GetModuleHandleA(LPCSTR);
HMODULE     WINAPI GetModuleHandleW(LPCWSTR);
#define     GetModuleHandle WINELIB_NAME_AW(GetModuleHandle)
BOOL        WINAPI GetModuleHandleExA(DWORD,LPCSTR,HMODULE*);
BOOL        WINAPI GetModuleHandleExW(DWORD,LPCWSTR,HMODULE*);
#define     GetModuleHandleEx WINELIB_NAME_AW(GetModuleHandleEx)
BOOL        WINAPI GetNamedPipeHandleStateA(HANDLE,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPSTR,DWORD);
BOOL        WINAPI GetNamedPipeHandleStateW(HANDLE,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPWSTR,DWORD);
#define     GetNamedPipeHandleState WINELIB_NAME_AW(GetNamedPipeHandleState)
BOOL        WINAPI GetNamedPipeInfo(HANDLE,LPDWORD,LPDWORD,LPDWORD,LPDWORD);
VOID        WINAPI GetNativeSystemInfo(LPSYSTEM_INFO);
BOOL        WINAPI GetNumberOfEventLogRecords(HANDLE,PDWORD);
BOOL        WINAPI GetOldestEventLogRecord(HANDLE,PDWORD);
BOOL        WINAPI GetOverlappedResult(HANDLE,LPOVERLAPPED,LPDWORD,BOOL);
DWORD       WINAPI GetPriorityClass(HANDLE);
BOOL        WINAPI GetPrivateObjectSecurity(PSECURITY_DESCRIPTOR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,PDWORD);
UINT        WINAPI GetPrivateProfileIntA(LPCSTR,LPCSTR,INT,LPCSTR);
UINT        WINAPI GetPrivateProfileIntW(LPCWSTR,LPCWSTR,INT,LPCWSTR);
#define     GetPrivateProfileInt WINELIB_NAME_AW(GetPrivateProfileInt)
INT         WINAPI GetPrivateProfileSectionA(LPCSTR,LPSTR,DWORD,LPCSTR);
INT         WINAPI GetPrivateProfileSectionW(LPCWSTR,LPWSTR,DWORD,LPCWSTR);
#define     GetPrivateProfileSection WINELIB_NAME_AW(GetPrivateProfileSection)
DWORD       WINAPI GetPrivateProfileSectionNamesA(LPSTR,DWORD,LPCSTR);
DWORD       WINAPI GetPrivateProfileSectionNamesW(LPWSTR,DWORD,LPCWSTR);
#define     GetPrivateProfileSectionNames WINELIB_NAME_AW(GetPrivateProfileSectionNames)
INT         WINAPI GetPrivateProfileStringA(LPCSTR,LPCSTR,LPCSTR,LPSTR,UINT,LPCSTR);
INT         WINAPI GetPrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,UINT,LPCWSTR);
#define     GetPrivateProfileString WINELIB_NAME_AW(GetPrivateProfileString)
BOOL        WINAPI GetPrivateProfileStructA(LPCSTR,LPCSTR,LPVOID,UINT,LPCSTR);
BOOL        WINAPI GetPrivateProfileStructW(LPCWSTR,LPCWSTR,LPVOID,UINT,LPCWSTR);
#define     GetPrivateProfileStruct WINELIB_NAME_AW(GetPrivateProfileStruct)
FARPROC     WINAPI GetProcAddress(HMODULE,LPCSTR);
BOOL        WINAPI GetProcessAffinityMask(HANDLE,PDWORD_PTR,PDWORD_PTR);
DWORD       WINAPI GetProcessHeaps(DWORD,PHANDLE);
DWORD       WINAPI GetProcessId(HANDLE);
BOOL        WINAPI GetProcessIoCounters(HANDLE,PIO_COUNTERS);
BOOL        WINAPI GetProcessPriorityBoost(HANDLE,PBOOL);
BOOL        WINAPI GetProcessShutdownParameters(LPDWORD,LPDWORD);
BOOL        WINAPI GetProcessTimes(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME);
DWORD       WINAPI GetProcessVersion(DWORD);
BOOL        WINAPI GetProcessWorkingSetSize(HANDLE,PSIZE_T,PSIZE_T);
UINT        WINAPI GetProfileIntA(LPCSTR,LPCSTR,INT);
UINT        WINAPI GetProfileIntW(LPCWSTR,LPCWSTR,INT);
#define     GetProfileInt WINELIB_NAME_AW(GetProfileInt)
INT         WINAPI GetProfileSectionA(LPCSTR,LPSTR,DWORD);
INT         WINAPI GetProfileSectionW(LPCWSTR,LPWSTR,DWORD);
#define     GetProfileSection WINELIB_NAME_AW(GetProfileSection)
INT         WINAPI GetProfileStringA(LPCSTR,LPCSTR,LPCSTR,LPSTR,UINT);
INT         WINAPI GetProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,UINT);
#define     GetProfileString WINELIB_NAME_AW(GetProfileString)
BOOL        WINAPI GetQueuedCompletionStatus(HANDLE,LPDWORD,PULONG_PTR,LPOVERLAPPED*,DWORD);
BOOL        WINAPI GetSecurityDescriptorControl(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR_CONTROL,LPDWORD);
BOOL        WINAPI GetSecurityDescriptorDacl(PSECURITY_DESCRIPTOR,LPBOOL,PACL *,LPBOOL);
BOOL        WINAPI GetSecurityDescriptorGroup(PSECURITY_DESCRIPTOR,PSID *,LPBOOL);
DWORD       WINAPI GetSecurityDescriptorLength(PSECURITY_DESCRIPTOR);
BOOL        WINAPI GetSecurityDescriptorOwner(PSECURITY_DESCRIPTOR,PSID *,LPBOOL);
BOOL        WINAPI GetSecurityDescriptorSacl(PSECURITY_DESCRIPTOR,LPBOOL,PACL *,LPBOOL);
PSID_IDENTIFIER_AUTHORITY WINAPI GetSidIdentifierAuthority(PSID);
DWORD       WINAPI GetSidLengthRequired(BYTE);
PDWORD      WINAPI GetSidSubAuthority(PSID,DWORD);
PUCHAR      WINAPI GetSidSubAuthorityCount(PSID);
DWORD       WINAPI GetShortPathNameA(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI GetShortPathNameW(LPCWSTR,LPWSTR,DWORD);
#define     GetShortPathName WINELIB_NAME_AW(GetShortPathName)
VOID        WINAPI GetStartupInfoA(LPSTARTUPINFOA);
VOID        WINAPI GetStartupInfoW(LPSTARTUPINFOW);
#define     GetStartupInfo WINELIB_NAME_AW(GetStartupInfo)
HANDLE      WINAPI GetStdHandle(DWORD);
UINT        WINAPI GetSystemDirectoryA(LPSTR,UINT);
UINT        WINAPI GetSystemDirectoryW(LPWSTR,UINT);
#define     GetSystemDirectory WINELIB_NAME_AW(GetSystemDirectory)
VOID        WINAPI GetSystemInfo(LPSYSTEM_INFO);
BOOL        WINAPI GetSystemPowerStatus(LPSYSTEM_POWER_STATUS);
VOID        WINAPI GetSystemTime(LPSYSTEMTIME);
BOOL        WINAPI GetSystemTimeAdjustment(PDWORD,PDWORD,PBOOL);
VOID        WINAPI GetSystemTimeAsFileTime(LPFILETIME);
UINT        WINAPI GetSystemWindowsDirectoryA(LPSTR,UINT);
UINT        WINAPI GetSystemWindowsDirectoryW(LPWSTR,UINT);
#define     GetSystemWindowsDirectory WINELIB_NAME_AW(GetSystemWindowsDirectory)
DWORD       WINAPI GetTapeParameters(HANDLE,DWORD,LPDWORD,LPVOID);
DWORD       WINAPI GetTapePosition(HANDLE,DWORD,LPDWORD,LPDWORD,LPDWORD);
DWORD       WINAPI GetTapeStatus(HANDLE);
UINT        WINAPI GetTempFileNameA(LPCSTR,LPCSTR,UINT,LPSTR);
UINT        WINAPI GetTempFileNameW(LPCWSTR,LPCWSTR,UINT,LPWSTR);
#define     GetTempFileName WINELIB_NAME_AW(GetTempFileName)
DWORD       WINAPI GetTempPathA(DWORD,LPSTR);
DWORD       WINAPI GetTempPathW(DWORD,LPWSTR);
#define     GetTempPath WINELIB_NAME_AW(GetTempPath)
DWORD       WINAPI GetTickCount(void);
DWORD       WINAPI GetTimeZoneInformation(LPTIME_ZONE_INFORMATION);
BOOL        WINAPI GetThreadContext(HANDLE,CONTEXT *);
INT         WINAPI GetThreadPriority(HANDLE);
BOOL        WINAPI GetThreadPriorityBoost(HANDLE,PBOOL);
BOOL        WINAPI GetThreadSelectorEntry(HANDLE,DWORD,LPLDT_ENTRY);
BOOL        WINAPI GetThreadTimes(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME);
BOOL        WINAPI GetTokenInformation(HANDLE,TOKEN_INFORMATION_CLASS,LPVOID,DWORD,LPDWORD);
BOOL        WINAPI GetUserNameA(LPSTR,LPDWORD);
BOOL        WINAPI GetUserNameW(LPWSTR,LPDWORD);
#define     GetUserName WINELIB_NAME_AW(GetUserName)
DWORD       WINAPI GetVersion(void);
BOOL        WINAPI GetVersionExA(OSVERSIONINFOA*);
BOOL        WINAPI GetVersionExW(OSVERSIONINFOW*);
#define     GetVersionEx WINELIB_NAME_AW(GetVersionEx)
BOOL        WINAPI GetVolumeInformationA(LPCSTR,LPSTR,DWORD,LPDWORD,LPDWORD,LPDWORD,LPSTR,DWORD);
BOOL        WINAPI GetVolumeInformationW(LPCWSTR,LPWSTR,DWORD,LPDWORD,LPDWORD,LPDWORD,LPWSTR,DWORD);
#define     GetVolumeInformation WINELIB_NAME_AW(GetVolumeInformation)
BOOL        WINAPI GetVolumeNameForVolumeMountPointA(LPCSTR,LPSTR,DWORD);
BOOL        WINAPI GetVolumeNameForVolumeMountPointW(LPCWSTR,LPWSTR,DWORD);
#define     GetVolumeNameForVolumeMountPoint WINELIB_NAME_AW(GetVolumeNameForVolumeMountPoint)
BOOL        WINAPI GetVolumePathNameA(LPCSTR,LPSTR,DWORD);
BOOL        WINAPI GetVolumePathNameW(LPCWSTR,LPWSTR,DWORD);
#define     GetVolumePathName WINELIB_NAME_AW(GetVolumePathName)
BOOL        WINAPI GetVolumePathNamesForVolumeNameA(LPCSTR,LPSTR,DWORD,PDWORD);
BOOL        WINAPI GetVolumePathNamesForVolumeNameW(LPCWSTR,LPWSTR,DWORD,PDWORD);
#define     GetVolumePathNamesForVolumeName WINELIB_NAME_AW(GetVolumePathNamesForVolumeName)
UINT        WINAPI GetWindowsDirectoryA(LPSTR,UINT);
UINT        WINAPI GetWindowsDirectoryW(LPWSTR,UINT);
#define     GetWindowsDirectory WINELIB_NAME_AW(GetWindowsDirectory)
ATOM        WINAPI GlobalAddAtomA(LPCSTR);
ATOM        WINAPI GlobalAddAtomW(LPCWSTR);
#define     GlobalAddAtom WINELIB_NAME_AW(GlobalAddAtom)
HGLOBAL     WINAPI GlobalAlloc(UINT,SIZE_T);
SIZE_T      WINAPI GlobalCompact(DWORD);
ATOM        WINAPI GlobalDeleteAtom(ATOM);
ATOM        WINAPI GlobalFindAtomA(LPCSTR);
ATOM        WINAPI GlobalFindAtomW(LPCWSTR);
#define     GlobalFindAtom WINELIB_NAME_AW(GlobalFindAtom)
VOID        WINAPI GlobalFix(HGLOBAL);
UINT        WINAPI GlobalFlags(HGLOBAL);
HGLOBAL     WINAPI GlobalFree(HGLOBAL);
UINT        WINAPI GlobalGetAtomNameA(ATOM,LPSTR,INT);
UINT        WINAPI GlobalGetAtomNameW(ATOM,LPWSTR,INT);
#define     GlobalGetAtomName WINELIB_NAME_AW(GlobalGetAtomName)
HGLOBAL     WINAPI GlobalHandle(LPCVOID);
LPVOID      WINAPI GlobalLock(HGLOBAL);
VOID        WINAPI GlobalMemoryStatus(LPMEMORYSTATUS);
BOOL        WINAPI GlobalMemoryStatusEx(LPMEMORYSTATUSEX);
HGLOBAL     WINAPI GlobalReAlloc(HGLOBAL,SIZE_T,UINT);
SIZE_T      WINAPI GlobalSize(HGLOBAL);
VOID        WINAPI GlobalUnfix(HGLOBAL);
BOOL        WINAPI GlobalUnlock(HGLOBAL);
BOOL        WINAPI GlobalUnWire(HGLOBAL);
LPVOID      WINAPI GlobalWire(HGLOBAL);
#define     HasOverlappedCompleted(lpOverlapped) ((lpOverlapped)->Internal != STATUS_PENDING)
LPVOID      WINAPI HeapAlloc(HANDLE,DWORD,SIZE_T);
SIZE_T      WINAPI HeapCompact(HANDLE,DWORD);
HANDLE      WINAPI HeapCreate(DWORD,SIZE_T,SIZE_T);
BOOL        WINAPI HeapDestroy(HANDLE);
BOOL        WINAPI HeapFree(HANDLE,DWORD,LPVOID);
BOOL        WINAPI HeapLock(HANDLE);
LPVOID      WINAPI HeapReAlloc(HANDLE,DWORD,LPVOID,SIZE_T);
BOOL        WINAPI HeapQueryInformation(HANDLE,HEAP_INFORMATION_CLASS,PVOID,SIZE_T,PSIZE_T);
BOOL        WINAPI HeapSetInformation(HANDLE,HEAP_INFORMATION_CLASS,PVOID,SIZE_T);
SIZE_T      WINAPI HeapSize(HANDLE,DWORD,LPVOID);
BOOL        WINAPI HeapUnlock(HANDLE);
BOOL        WINAPI HeapValidate(HANDLE,DWORD,LPCVOID);
BOOL        WINAPI HeapWalk(HANDLE,LPPROCESS_HEAP_ENTRY);
BOOL        WINAPI InitAtomTable(DWORD);
BOOL        WINAPI InitializeAcl(PACL,DWORD,DWORD);
void        WINAPI InitializeCriticalSection(CRITICAL_SECTION *lpCrit);
BOOL        WINAPI InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION *,DWORD);
BOOL        WINAPI InitializeSecurityDescriptor(PSECURITY_DESCRIPTOR,DWORD);
BOOL        WINAPI InitializeSid(PSID,PSID_IDENTIFIER_AUTHORITY,BYTE);
BOOL        WINAPI IsBadCodePtr(FARPROC);
BOOL        WINAPI IsBadHugeReadPtr(LPCVOID,UINT);
BOOL        WINAPI IsBadHugeWritePtr(LPVOID,UINT);
BOOL        WINAPI IsBadReadPtr(LPCVOID,UINT);
BOOL        WINAPI IsBadStringPtrA(LPCSTR,UINT);
BOOL        WINAPI IsBadStringPtrW(LPCWSTR,UINT);
#define     IsBadStringPtr WINELIB_NAME_AW(IsBadStringPtr)
BOOL        WINAPI IsBadWritePtr(LPVOID,UINT);
BOOL        WINAPI IsDebuggerPresent(void);
BOOL        WINAPI IsSystemResumeAutomatic(void);
BOOL        WINAPI IsTextUnicode(LPCVOID,INT,LPINT);
BOOL        WINAPI IsTokenRestricted(HANDLE);
BOOL        WINAPI IsValidAcl(PACL);
BOOL        WINAPI IsValidSecurityDescriptor(PSECURITY_DESCRIPTOR);
BOOL        WINAPI IsValidSid(PSID);
BOOL        WINAPI IsWellKnownSid(PSID,WELL_KNOWN_SID_TYPE);
BOOL        WINAPI IsWow64Process(HANDLE,PBOOL);
BOOL        WINAPI ImpersonateLoggedOnUser(HANDLE);
BOOL        WINAPI ImpersonateNamedPipeClient(HANDLE);
BOOL        WINAPI ImpersonateSelf(SECURITY_IMPERSONATION_LEVEL);
BOOL        WINAPI IsProcessorFeaturePresent(DWORD);
void        WINAPI LeaveCriticalSection(CRITICAL_SECTION *lpCrit);
HMODULE     WINAPI LoadLibraryA(LPCSTR);
HMODULE     WINAPI LoadLibraryW(LPCWSTR);
#define     LoadLibrary WINELIB_NAME_AW(LoadLibrary)
HMODULE     WINAPI LoadLibraryExA(LPCSTR,HANDLE,DWORD);
HMODULE     WINAPI LoadLibraryExW(LPCWSTR,HANDLE,DWORD);
#define     LoadLibraryEx WINELIB_NAME_AW(LoadLibraryEx)
HINSTANCE   WINAPI LoadModule(LPCSTR,LPVOID);
HGLOBAL     WINAPI LoadResource(HMODULE,HRSRC);
HLOCAL      WINAPI LocalAlloc(UINT,SIZE_T);
SIZE_T      WINAPI LocalCompact(UINT);
UINT        WINAPI LocalFlags(HLOCAL);
HLOCAL      WINAPI LocalFree(HLOCAL);
HLOCAL      WINAPI LocalHandle(LPCVOID);
LPVOID      WINAPI LocalLock(HLOCAL);
HLOCAL      WINAPI LocalReAlloc(HLOCAL,SIZE_T,UINT);
SIZE_T      WINAPI LocalShrink(HGLOBAL,UINT);
SIZE_T      WINAPI LocalSize(HLOCAL);
BOOL        WINAPI LocalUnlock(HLOCAL);
LPVOID      WINAPI LockResource(HGLOBAL);
#define     LockSegment(handle) GlobalFix((HANDLE)(handle))
BOOL        WINAPI LookupAccountNameA(LPCSTR,LPCSTR,PSID,LPDWORD,LPSTR,LPDWORD,PSID_NAME_USE);
BOOL        WINAPI LookupAccountNameW(LPCWSTR,LPCWSTR,PSID,LPDWORD,LPWSTR,LPDWORD,PSID_NAME_USE);
#define     LookupAccountName WINELIB_NAME_AW(LookupAccountName)
BOOL        WINAPI LookupAccountSidA(LPCSTR,PSID,LPSTR,LPDWORD,LPSTR,LPDWORD,PSID_NAME_USE);
BOOL        WINAPI LookupAccountSidW(LPCWSTR,PSID,LPWSTR,LPDWORD,LPWSTR,LPDWORD,PSID_NAME_USE);
#define     LookupAccountSid WINELIB_NAME_AW(LookupAccountSid)
BOOL        WINAPI LocalFileTimeToFileTime(const FILETIME*,LPFILETIME);
BOOL        WINAPI LockFile(HANDLE,DWORD,DWORD,DWORD,DWORD);
BOOL        WINAPI LockFileEx(HANDLE, DWORD, DWORD, DWORD, DWORD, LPOVERLAPPED);
BOOL        WINAPI LogonUserA(LPCSTR,LPCSTR,LPCSTR,DWORD,DWORD,PHANDLE);
BOOL        WINAPI LogonUserW(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,DWORD,PHANDLE);
#define     LogonUser WINELIB_NAME_AW(LogonUser)
BOOL        WINAPI LookupPrivilegeDisplayNameA(LPCSTR,LPCSTR,LPSTR,LPDWORD,LPDWORD);
BOOL        WINAPI LookupPrivilegeDisplayNameW(LPCWSTR,LPCWSTR,LPWSTR,LPDWORD,LPDWORD);
#define     LookupPrivilegeDisplayName WINELIB_NAME_AW(LookupPrivilegeDisplayName)
BOOL        WINAPI LookupPrivilegeNameA(LPCSTR,PLUID,LPSTR,LPDWORD);
BOOL        WINAPI LookupPrivilegeNameW(LPCWSTR,PLUID,LPWSTR,LPDWORD);
#define     LookupPrivilegeName WINELIB_NAME_AW(LookupPrivilegeName)
BOOL        WINAPI LookupPrivilegeValueA(LPCSTR,LPCSTR,PLUID);
BOOL        WINAPI LookupPrivilegeValueW(LPCWSTR,LPCWSTR,PLUID);
#define     LookupPrivilegeValue WINELIB_NAME_AW(LookupPrivilegeValue)
BOOL        WINAPI MakeAbsoluteSD(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,LPDWORD,
                                  PACL,LPDWORD,PACL,LPDWORD,PSID,LPDWORD,PSID,LPDWORD);
void        WINAPI MakeCriticalSectionGlobal(CRITICAL_SECTION *lpCrit);
#define     MakeProcInstance(proc,inst) (proc)
BOOL        WINAPI MakeSelfRelativeSD(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,LPDWORD);
VOID        WINAPI MapGenericMask(PDWORD,PGENERIC_MAPPING);
HMODULE     WINAPI MapHModuleSL(WORD);
WORD        WINAPI MapHModuleLS(HMODULE);
LPVOID      WINAPI MapViewOfFile(HANDLE,DWORD,DWORD,DWORD,SIZE_T);
LPVOID      WINAPI MapViewOfFileEx(HANDLE,DWORD,DWORD,DWORD,SIZE_T,LPVOID);
BOOL        WINAPI MoveFileA(LPCSTR,LPCSTR);
BOOL        WINAPI MoveFileW(LPCWSTR,LPCWSTR);
#define     MoveFile WINELIB_NAME_AW(MoveFile)
BOOL        WINAPI MoveFileExA(LPCSTR,LPCSTR,DWORD);
BOOL        WINAPI MoveFileExW(LPCWSTR,LPCWSTR,DWORD);
#define     MoveFileEx WINELIB_NAME_AW(MoveFileEx)
BOOL        WINAPI MoveFileWithProgressA(LPCSTR,LPCSTR,LPPROGRESS_ROUTINE,LPVOID,DWORD);
BOOL        WINAPI MoveFileWithProgressW(LPCWSTR,LPCWSTR,LPPROGRESS_ROUTINE,LPVOID,DWORD);
#define     MoveFileWithProgress WINELIB_NAME_AW(MoveFileWithProgress)
INT         WINAPI MulDiv(INT,INT,INT);
BOOL        WINAPI NotifyChangeEventLog(HANDLE,HANDLE);
BOOL        WINAPI ObjectCloseAuditAlarmA(LPCSTR,LPVOID,BOOL);
BOOL        WINAPI ObjectCloseAuditAlarmW(LPCWSTR,LPVOID,BOOL);
#define     ObjectCloseAuditAlarm WINELIB_NAME_AW(ObjectCloseAuditAlarm)
BOOL        WINAPI ObjectDeleteAuditAlarmA(LPCSTR,LPVOID,BOOL);
BOOL        WINAPI ObjectDeleteAuditAlarmW(LPCWSTR,LPVOID,BOOL);
#define     ObjectDeleteAuditAlarm WINELIB_NAME_AW(ObjectDeleteAuditAlarm)
BOOL        WINAPI ObjectOpenAuditAlarmA(LPCSTR,LPVOID,LPSTR,LPSTR,PSECURITY_DESCRIPTOR,HANDLE,
                                         DWORD,DWORD,PPRIVILEGE_SET,BOOL,BOOL,LPBOOL);
BOOL        WINAPI ObjectOpenAuditAlarmW(LPCWSTR,LPVOID,LPWSTR,LPWSTR,PSECURITY_DESCRIPTOR,HANDLE,
                                         DWORD,DWORD,PPRIVILEGE_SET,BOOL,BOOL,LPBOOL);
#define     ObjectOpenAuditAlarm WINELIB_NAME_AW(ObjectOpenAuditAlarm)
BOOL        WINAPI ObjectPrivilegeAuditAlarmA(LPCSTR,LPVOID,HANDLE,DWORD,PPRIVILEGE_SET,BOOL);
BOOL        WINAPI ObjectPrivilegeAuditAlarmW(LPCWSTR,LPVOID,HANDLE,DWORD,PPRIVILEGE_SET,BOOL);
#define     ObjectPrivilegeAuditAlarm WINELIB_NAME_AW(ObjectPrivilegeAuditAlarm)
HANDLE      WINAPI OpenBackupEventLogA(LPCSTR,LPCSTR);
HANDLE      WINAPI OpenBackupEventLogW(LPCWSTR,LPCWSTR);
#define     OpenBackupEventLog WINELIB_NAME_AW(OpenBackupEventLog)
HANDLE      WINAPI OpenEventA(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenEventW(DWORD,BOOL,LPCWSTR);
#define     OpenEvent WINELIB_NAME_AW(OpenEvent)
HANDLE      WINAPI OpenEventLogA(LPCSTR,LPCSTR);
HANDLE      WINAPI OpenEventLogW(LPCWSTR,LPCWSTR);
#define     OpenEventLog WINELIB_NAME_AW(OpenEventLog)
HFILE       WINAPI OpenFile(LPCSTR,OFSTRUCT*,UINT);
HANDLE      WINAPI OpenFileMappingA(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenFileMappingW(DWORD,BOOL,LPCWSTR);
#define     OpenFileMapping WINELIB_NAME_AW(OpenFileMapping)
HANDLE      WINAPI OpenMutexA(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenMutexW(DWORD,BOOL,LPCWSTR);
#define     OpenMutex WINELIB_NAME_AW(OpenMutex)
HANDLE      WINAPI OpenProcess(DWORD,BOOL,DWORD);
BOOL        WINAPI OpenProcessToken(HANDLE,DWORD,PHANDLE);
HANDLE      WINAPI OpenSemaphoreA(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenSemaphoreW(DWORD,BOOL,LPCWSTR);
#define     OpenSemaphore WINELIB_NAME_AW(OpenSemaphore)
HANDLE      WINAPI OpenThread(DWORD,BOOL,DWORD);
BOOL        WINAPI OpenThreadToken(HANDLE,DWORD,BOOL,PHANDLE);
HANDLE      WINAPI OpenWaitableTimerA(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenWaitableTimerW(DWORD,BOOL,LPCWSTR);
#define     OpenWaitableTimer WINELIB_NAME_AW(OpenWaitableTimer)
VOID        WINAPI OutputDebugStringA(LPCSTR);
VOID        WINAPI OutputDebugStringW(LPCWSTR);
#define     OutputDebugString WINELIB_NAME_AW(OutputDebugString)
BOOL        WINAPI PeekNamedPipe(HANDLE,PVOID,DWORD,PDWORD,PDWORD,PDWORD);
BOOL        WINAPI PostQueuedCompletionStatus(HANDLE,DWORD,ULONG_PTR,LPOVERLAPPED);
DWORD       WINAPI PrepareTape(HANDLE,DWORD,BOOL);
BOOL        WINAPI ProcessIdToSessionId(DWORD,DWORD*);
BOOL        WINAPI PrivilegeCheck(HANDLE,PPRIVILEGE_SET,LPBOOL);
BOOL        WINAPI PrivilegedServiceAuditAlarmA(LPCSTR,LPCSTR,HANDLE,PPRIVILEGE_SET,BOOL);
BOOL        WINAPI PrivilegedServiceAuditAlarmW(LPCWSTR,LPCWSTR,HANDLE,PPRIVILEGE_SET,BOOL);
#define     PrivilegedServiceAuditAlarm WINELIB_NAME_AW(PrivilegedServiceAuditAlarm)
BOOL        WINAPI PulseEvent(HANDLE);
BOOL        WINAPI PurgeComm(HANDLE,DWORD);
BOOL        WINAPI QueryActCtxW(DWORD,HANDLE,PVOID,ULONG,PVOID,SIZE_T,SIZE_T *);
DWORD       WINAPI QueryDosDeviceA(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI QueryDosDeviceW(LPCWSTR,LPWSTR,DWORD);
#define     QueryDosDevice WINELIB_NAME_AW(QueryDosDevice)
BOOL        WINAPI QueryPerformanceCounter(LARGE_INTEGER*);
BOOL        WINAPI QueryPerformanceFrequency(LARGE_INTEGER*);
DWORD       WINAPI QueueUserAPC(PAPCFUNC,HANDLE,ULONG_PTR);
BOOL        WINAPI QueueUserWorkItem(LPTHREAD_START_ROUTINE,PVOID,ULONG);
void        WINAPI RaiseException(DWORD,DWORD,DWORD,const ULONG_PTR *);
BOOL        WINAPI ReadEventLogA(HANDLE,DWORD,DWORD,LPVOID,DWORD,DWORD *,DWORD *);
BOOL        WINAPI ReadEventLogW(HANDLE,DWORD,DWORD,LPVOID,DWORD,DWORD *,DWORD *);
#define     ReadEventLog WINELIB_NAME_AW(ReadEventLog)
BOOL        WINAPI ReadDirectoryChangesW(HANDLE,LPVOID,DWORD,BOOL,DWORD,LPDWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE);
BOOL        WINAPI ReadFile(HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI ReadFileEx(HANDLE,LPVOID,DWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE);
BOOL        WINAPI ReadProcessMemory(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
HANDLE      WINAPI RegisterEventSourceA(LPCSTR,LPCSTR);
HANDLE      WINAPI RegisterEventSourceW(LPCWSTR,LPCWSTR);
#define     RegisterEventSource WINELIB_NAME_AW(RegisterEventSource)
BOOL        WINAPI RegisterWaitForSingleObject(PHANDLE,HANDLE,WAITORTIMERCALLBACK,PVOID,ULONG,ULONG);
HANDLE      WINAPI RegisterWaitForSingleObjectEx(HANDLE,WAITORTIMERCALLBACK,PVOID,ULONG,ULONG);
VOID        WINAPI ReleaseActCtx(HANDLE);
BOOL        WINAPI ReleaseMutex(HANDLE);
BOOL        WINAPI ReleaseSemaphore(HANDLE,LONG,LPLONG);
ULONG       WINAPI RemoveVectoredExceptionHandler(PVOID);
BOOL        WINAPI ReplaceFileA(LPCSTR,LPCSTR,LPCSTR,DWORD,LPVOID,LPVOID);
BOOL        WINAPI ReplaceFileW(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,LPVOID,LPVOID);
#define     ReplaceFile WINELIB_NAME_AW(ReplaceFile)
BOOL        WINAPI RemoveDirectoryA(LPCSTR);
BOOL        WINAPI RemoveDirectoryW(LPCWSTR);
#define     RemoveDirectory WINELIB_NAME_AW(RemoveDirectory)
BOOL        WINAPI ReportEventA(HANDLE,WORD,WORD,DWORD,PSID,WORD,DWORD,LPCSTR *,LPVOID);
BOOL        WINAPI ReportEventW(HANDLE,WORD,WORD,DWORD,PSID,WORD,DWORD,LPCWSTR *,LPVOID);
BOOL        WINAPI RequestWakeupLatency(LATENCY_TIME latency);
#define     ReportEvent WINELIB_NAME_AW(ReportEvent)
BOOL        WINAPI ResetEvent(HANDLE);
DWORD       WINAPI ResumeThread(HANDLE);
BOOL        WINAPI RevertToSelf(void);
DWORD       WINAPI SearchPathA(LPCSTR,LPCSTR,LPCSTR,DWORD,LPSTR,LPSTR*);
DWORD       WINAPI SearchPathW(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,LPWSTR,LPWSTR*);
#define     SearchPath WINELIB_NAME_AW(SearchPath)
BOOL        WINAPI SetAclInformation(PACL,LPVOID,DWORD,ACL_INFORMATION_CLASS);
BOOL        WINAPI SetCommConfig(HANDLE,LPCOMMCONFIG,DWORD);
BOOL        WINAPI SetCommBreak(HANDLE);
BOOL        WINAPI SetCommMask(HANDLE,DWORD);
BOOL        WINAPI SetCommState(HANDLE,LPDCB);
BOOL        WINAPI SetCommTimeouts(HANDLE,LPCOMMTIMEOUTS);
BOOL        WINAPI SetComputerNameA(LPCSTR);
BOOL        WINAPI SetComputerNameW(LPCWSTR);
#define     SetComputerName WINELIB_NAME_AW(SetComputerName)
BOOL        WINAPI SetComputerNameExA(COMPUTER_NAME_FORMAT,LPCSTR);
BOOL        WINAPI SetComputerNameExW(COMPUTER_NAME_FORMAT,LPCWSTR);
#define     SetComputerNameEx WINELIB_NAME_AW(SetComputerNameEx)
DWORD       WINAPI SetCriticalSectionSpinCount(LPCRITICAL_SECTION,DWORD);
BOOL        WINAPI SetCurrentDirectoryA(LPCSTR);
BOOL        WINAPI SetCurrentDirectoryW(LPCWSTR);
#define     SetCurrentDirectory WINELIB_NAME_AW(SetCurrentDirectory)
BOOL        WINAPI SetDefaultCommConfigA(LPCSTR,LPCOMMCONFIG,DWORD);
BOOL        WINAPI SetDefaultCommConfigW(LPCWSTR,LPCOMMCONFIG,DWORD);
#define     SetDefaultCommConfig WINELIB_NAME_AW(SetDefaultCommConfig)
BOOL        WINAPI SetDllDirectoryA(LPCSTR);
BOOL        WINAPI SetDllDirectoryW(LPCWSTR);
#define     SetDllDirectory WINELIB_NAME_AW(SetDllDirectory)
BOOL        WINAPI SetEndOfFile(HANDLE);
BOOL        WINAPI SetEnvironmentVariableA(LPCSTR,LPCSTR);
BOOL        WINAPI SetEnvironmentVariableW(LPCWSTR,LPCWSTR);
#define     SetEnvironmentVariable WINELIB_NAME_AW(SetEnvironmentVariable)
UINT        WINAPI SetErrorMode(UINT);
BOOL        WINAPI SetEvent(HANDLE);
VOID        WINAPI SetFileApisToANSI(void);
VOID        WINAPI SetFileApisToOEM(void);
BOOL        WINAPI SetFileAttributesA(LPCSTR,DWORD);
BOOL        WINAPI SetFileAttributesW(LPCWSTR,DWORD);
#define     SetFileAttributes WINELIB_NAME_AW(SetFileAttributes)
DWORD       WINAPI SetFilePointer(HANDLE,LONG,LPLONG,DWORD);
BOOL        WINAPI SetFilePointerEx(HANDLE,LARGE_INTEGER,LARGE_INTEGER*,DWORD);
BOOL        WINAPI SetFileSecurityA(LPCSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR);
BOOL        WINAPI SetFileSecurityW(LPCWSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR);
#define     SetFileSecurity WINELIB_NAME_AW(SetFileSecurity)
BOOL        WINAPI SetFileTime(HANDLE,const FILETIME*,const FILETIME*,const FILETIME*);
UINT        WINAPI SetHandleCount(UINT);
BOOL        WINAPI SetHandleInformation(HANDLE,DWORD,DWORD);
BOOL        WINAPI SetKernelObjectSecurity(HANDLE,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR);
BOOL        WINAPI SetLocalTime(const SYSTEMTIME*);
BOOL        WINAPI SetMailslotInfo(HANDLE,DWORD);
BOOL        WINAPI SetNamedPipeHandleState(HANDLE,LPDWORD,LPDWORD,LPDWORD);
BOOL        WINAPI SetPriorityClass(HANDLE,DWORD);
BOOL        WINAPI SetPrivateObjectSecurity(SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,
                                            PSECURITY_DESCRIPTOR*,PGENERIC_MAPPING,HANDLE);
BOOL        WINAPI SetProcessAffinityMask(HANDLE,DWORD_PTR);
BOOL        WINAPI SetProcessPriorityBoost(HANDLE,BOOL);
BOOL        WINAPI SetProcessShutdownParameters(DWORD,DWORD);
BOOL        WINAPI SetProcessWorkingSetSize(HANDLE,SIZE_T,SIZE_T);
BOOL        WINAPI SetSecurityDescriptorControl(PSECURITY_DESCRIPTOR,SECURITY_DESCRIPTOR_CONTROL,
                                                SECURITY_DESCRIPTOR_CONTROL);
BOOL        WINAPI SetSecurityDescriptorDacl(PSECURITY_DESCRIPTOR,BOOL,PACL,BOOL);
BOOL        WINAPI SetSecurityDescriptorGroup(PSECURITY_DESCRIPTOR,PSID,BOOL);
BOOL        WINAPI SetSecurityDescriptorOwner(PSECURITY_DESCRIPTOR,PSID,BOOL);
BOOL        WINAPI SetSecurityDescriptorSacl(PSECURITY_DESCRIPTOR,BOOL,PACL,BOOL);
BOOL        WINAPI SetStdHandle(DWORD,HANDLE);
#define     SetSwapAreaSize(w) (w)
BOOL        WINAPI SetSystemPowerState(BOOL,BOOL);
BOOL        WINAPI SetSystemTime(const SYSTEMTIME*);
BOOL        WINAPI SetSystemTimeAdjustment(DWORD,BOOL);
DWORD       WINAPI SetTapeParameters(HANDLE,DWORD,LPVOID);
DWORD       WINAPI SetTapePosition(HANDLE,DWORD,DWORD,DWORD,DWORD,BOOL);
DWORD       WINAPI SetThreadAffinityMask(HANDLE,DWORD);
BOOL        WINAPI SetThreadContext(HANDLE,const CONTEXT *);
DWORD       WINAPI SetThreadExecutionState(EXECUTION_STATE);
DWORD       WINAPI SetThreadIdealProcessor(HANDLE,DWORD);
BOOL        WINAPI SetThreadPriority(HANDLE,INT);
BOOL        WINAPI SetThreadPriorityBoost(HANDLE,BOOL);
BOOL        WINAPI SetThreadToken(PHANDLE,HANDLE);
BOOL        WINAPI SetTimeZoneInformation(const TIME_ZONE_INFORMATION *);
BOOL        WINAPI SetTokenInformation(HANDLE,TOKEN_INFORMATION_CLASS,LPVOID,DWORD);
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER);
BOOL        WINAPI SetVolumeLabelA(LPCSTR,LPCSTR);
BOOL        WINAPI SetVolumeLabelW(LPCWSTR,LPCWSTR);
#define     SetVolumeLabel WINELIB_NAME_AW(SetVolumeLabel)
BOOL        WINAPI SetVolumeMountPointA(LPCSTR,LPCSTR);
BOOL        WINAPI SetVolumeMountPointW(LPCSTR,LPCSTR);
#define     SetVolumeMountPoint WINELIB_NAME_AW(SetVolumeMountPoint)
BOOL        WINAPI SetWaitableTimer(HANDLE,const LARGE_INTEGER*,LONG,PTIMERAPCROUTINE,LPVOID,BOOL);
BOOL        WINAPI SetupComm(HANDLE,DWORD,DWORD);
DWORD       WINAPI SignalObjectAndWait(HANDLE,HANDLE,DWORD,BOOL);
DWORD       WINAPI SizeofResource(HMODULE,HRSRC);
VOID        WINAPI Sleep(DWORD);
DWORD       WINAPI SleepEx(DWORD,BOOL);
DWORD       WINAPI SuspendThread(HANDLE);
void        WINAPI SwitchToFiber(LPVOID);
BOOL        WINAPI SwitchToThread(void);
BOOL        WINAPI SystemTimeToFileTime(const SYSTEMTIME*,LPFILETIME);
BOOL        WINAPI SystemTimeToTzSpecificLocalTime(LPTIME_ZONE_INFORMATION,LPSYSTEMTIME,LPSYSTEMTIME);
BOOL        WINAPI TerminateProcess(HANDLE,DWORD);
BOOL        WINAPI TerminateThread(HANDLE,DWORD);
DWORD       WINAPI TlsAlloc(void);
BOOL        WINAPI TlsFree(DWORD);
LPVOID      WINAPI TlsGetValue(DWORD);
BOOL        WINAPI TlsSetValue(DWORD,LPVOID);
BOOL        WINAPI TransactNamedPipe(HANDLE,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI TransmitCommChar(HANDLE,CHAR);
BOOL        WINAPI TryEnterCriticalSection(CRITICAL_SECTION *lpCrit);
BOOL        WINAPI TzSpecificLocalTimeToSystemTime(LPTIME_ZONE_INFORMATION,LPSYSTEMTIME,LPSYSTEMTIME);
LONG        WINAPI UnhandledExceptionFilter(PEXCEPTION_POINTERS);
BOOL        WINAPI UnlockFile(HANDLE,DWORD,DWORD,DWORD,DWORD);
BOOL        WINAPI UnlockFileEx(HANDLE,DWORD,DWORD,DWORD,LPOVERLAPPED);
#define     UnlockSegment(handle) GlobalUnfix((HANDLE)(handle))
BOOL        WINAPI UnmapViewOfFile(LPCVOID);
BOOL        WINAPI UnregisterWait(HANDLE);
BOOL        WINAPI UnregisterWaitEx(HANDLE,HANDLE);
BOOL        WINAPI UpdateResourceA(HANDLE,LPCSTR,LPCSTR,WORD,LPVOID,DWORD);
BOOL        WINAPI UpdateResourceW(HANDLE,LPCWSTR,LPCWSTR,WORD,LPVOID,DWORD);
#define     UpdateResource WINELIB_NAME_AW(UpdateResource)
BOOL        WINAPI VerifyVersionInfoA(LPOSVERSIONINFOEXA,DWORD,DWORDLONG);
BOOL        WINAPI VerifyVersionInfoW(LPOSVERSIONINFOEXW,DWORD,DWORDLONG);
#define     VerifyVersionInfo WINELIB_NAME_AW(VerifyVersionInfo)
LPVOID      WINAPI VirtualAlloc(LPVOID,SIZE_T,DWORD,DWORD);
LPVOID      WINAPI VirtualAllocEx(HANDLE,LPVOID,SIZE_T,DWORD,DWORD);
BOOL        WINAPI VirtualFree(LPVOID,SIZE_T,DWORD);
BOOL        WINAPI VirtualFreeEx(HANDLE,LPVOID,SIZE_T,DWORD);
BOOL        WINAPI VirtualLock(LPVOID,SIZE_T);
BOOL        WINAPI VirtualProtect(LPVOID,SIZE_T,DWORD,LPDWORD);
BOOL        WINAPI VirtualProtectEx(HANDLE,LPVOID,SIZE_T,DWORD,LPDWORD);
SIZE_T      WINAPI VirtualQuery(LPCVOID,PMEMORY_BASIC_INFORMATION,SIZE_T);
SIZE_T      WINAPI VirtualQueryEx(HANDLE,LPCVOID,PMEMORY_BASIC_INFORMATION,SIZE_T);
BOOL        WINAPI VirtualUnlock(LPVOID,SIZE_T);
BOOL        WINAPI WaitCommEvent(HANDLE,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI WaitForDebugEvent(LPDEBUG_EVENT,DWORD);
DWORD       WINAPI WaitForMultipleObjects(DWORD,const HANDLE*,BOOL,DWORD);
DWORD       WINAPI WaitForMultipleObjectsEx(DWORD,const HANDLE*,BOOL,DWORD,BOOL);
DWORD       WINAPI WaitForSingleObject(HANDLE,DWORD);
DWORD       WINAPI WaitForSingleObjectEx(HANDLE,DWORD,BOOL);
BOOL        WINAPI WaitNamedPipeA(LPCSTR,DWORD);
BOOL        WINAPI WaitNamedPipeW(LPCWSTR,DWORD);
#define     WaitNamedPipe WINELIB_NAME_AW(WaitNamedPipe)
UINT        WINAPI WinExec(LPCSTR,UINT);
BOOL        WINAPI WriteFile(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI WriteFileEx(HANDLE,LPCVOID,DWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE);
BOOL        WINAPI WritePrivateProfileSectionA(LPCSTR,LPCSTR,LPCSTR);
BOOL        WINAPI WritePrivateProfileSectionW(LPCWSTR,LPCWSTR,LPCWSTR);
#define     WritePrivateProfileSection WINELIB_NAME_AW(WritePrivateProfileSection)
BOOL        WINAPI WritePrivateProfileStringA(LPCSTR,LPCSTR,LPCSTR,LPCSTR);
BOOL        WINAPI WritePrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR);
#define     WritePrivateProfileString WINELIB_NAME_AW(WritePrivateProfileString)
BOOL        WINAPI WritePrivateProfileStructA(LPCSTR,LPCSTR,LPVOID,UINT,LPCSTR);
BOOL        WINAPI WritePrivateProfileStructW(LPCWSTR,LPCWSTR,LPVOID,UINT,LPCWSTR);
#define     WritePrivateProfileStruct WINELIB_NAME_AW(WritePrivateProfileStruct)
BOOL        WINAPI WriteProcessMemory(HANDLE,LPVOID,LPCVOID,SIZE_T,SIZE_T*);
BOOL        WINAPI WriteProfileSectionA(LPCSTR,LPCSTR);
BOOL        WINAPI WriteProfileSectionW(LPCWSTR,LPCWSTR);
#define     WritePrivateProfileSection WINELIB_NAME_AW(WritePrivateProfileSection)
BOOL        WINAPI WriteProfileStringA(LPCSTR,LPCSTR,LPCSTR);
BOOL        WINAPI WriteProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR);
#define     WriteProfileString WINELIB_NAME_AW(WriteProfileString)
DWORD       WINAPI WriteTapemark(HANDLE,DWORD,DWORD,BOOL);
#define     Yield()
BOOL        WINAPI ZombifyActCtx(HANDLE);

LPSTR       WINAPI lstrcatA(LPSTR,LPCSTR);
LPWSTR      WINAPI lstrcatW(LPWSTR,LPCWSTR);
INT         WINAPI lstrcmpA(LPCSTR,LPCSTR);
INT         WINAPI lstrcmpW(LPCWSTR,LPCWSTR);
INT         WINAPI lstrcmpiA(LPCSTR,LPCSTR);
INT         WINAPI lstrcmpiW(LPCWSTR,LPCWSTR);
LPSTR       WINAPI lstrcpyA(LPSTR,LPCSTR);
LPWSTR      WINAPI lstrcpyW(LPWSTR,LPCWSTR);
LPSTR       WINAPI lstrcpynA(LPSTR,LPCSTR,INT);
LPWSTR      WINAPI lstrcpynW(LPWSTR,LPCWSTR,INT);
INT         WINAPI lstrlenA(LPCSTR);
INT         WINAPI lstrlenW(LPCWSTR);

#if !defined(WINE_NO_INLINE_STRING) && defined(__WINESRC__)

/* string functions without the exception handler */

extern inline LPWSTR WINAPI lstrcpynW( LPWSTR dst, LPCWSTR src, INT n )
{
    LPWSTR d = dst;
    LPCWSTR s = src;
    UINT count = n;

    while ((count > 1) && *s)
    {
        count--;
        *d++ = *s++;
    }
    if (count) *d = 0;
    return dst;
}

extern inline LPSTR WINAPI lstrcpynA( LPSTR dst, LPCSTR src, INT n )
{
    LPSTR d = dst;
    LPCSTR s = src;
    UINT count = n;

    while ((count > 1) && *s)
    {
        count--;
        *d++ = *s++;
    }
    if (count) *d = 0;
    return dst;
}

extern inline INT WINAPI lstrlenW( LPCWSTR str )
{
    const WCHAR *s = str;
    while (*s) s++;
    return s - str;
}

extern inline INT WINAPI lstrlenA( LPCSTR str )
{
    return strlen( str );
}

extern inline LPWSTR WINAPI lstrcpyW( LPWSTR dst, LPCWSTR src )
{
    WCHAR *p = dst;
    while ((*p++ = *src++));
    return dst;
}

extern inline LPSTR WINAPI lstrcpyA( LPSTR dst, LPCSTR src )
{
    return strcpy( dst, src );
}

extern inline LPWSTR WINAPI lstrcatW( LPWSTR dst, LPCWSTR src )
{
    WCHAR *p = dst;
    while (*p) p++;
    while ((*p++ = *src++));
    return dst;
}

extern inline LPSTR WINAPI lstrcatA( LPSTR dst, LPCSTR src )
{
    return strcat( dst, src );
}

/* strncpy doesn't do what you think, don't use it */
#undef strncpy
#define strncpy(d,s,n) error do_not_use_strncpy_use_lstrcpynA_or_memcpy_instead

#endif /* !defined(WINE_NO_INLINE_STRING) && defined(__WINESRC__) */

#define     lstrcat WINELIB_NAME_AW(lstrcat)
#define     lstrcmp WINELIB_NAME_AW(lstrcmp)
#define     lstrcmpi WINELIB_NAME_AW(lstrcmpi)
#define     lstrcpy WINELIB_NAME_AW(lstrcpy)
#define     lstrcpyn WINELIB_NAME_AW(lstrcpyn)
#define     lstrlen WINELIB_NAME_AW(lstrlen)

LONG        WINAPI _hread(HFILE,LPVOID,LONG);
LONG        WINAPI _hwrite(HFILE,LPCSTR,LONG);
HFILE       WINAPI _lcreat(LPCSTR,INT);
HFILE       WINAPI _lclose(HFILE);
LONG        WINAPI _llseek(HFILE,LONG,INT);
HFILE       WINAPI _lopen(LPCSTR,INT);
UINT        WINAPI _lread(HFILE,LPVOID,UINT);
UINT        WINAPI _lwrite(HFILE,LPCSTR,UINT);

/* compatibility macros */
#define     FillMemory RtlFillMemory
#define     MoveMemory RtlMoveMemory
#define     ZeroMemory RtlZeroMemory
#define     CopyMemory RtlCopyMemory

/* undocumented functions */

typedef struct tagSYSLEVEL
{
    CRITICAL_SECTION crst;
    INT              level;
} SYSLEVEL;

/* [GS]etProcessDword offsets */
#define  GPD_APP_COMPAT_FLAGS    (-56)
#define  GPD_LOAD_DONE_EVENT     (-52)
#define  GPD_HINSTANCE16         (-48)
#define  GPD_WINDOWS_VERSION     (-44)
#define  GPD_THDB                (-40)
#define  GPD_PDB                 (-36)
#define  GPD_STARTF_SHELLDATA    (-32)
#define  GPD_STARTF_HOTKEY       (-28)
#define  GPD_STARTF_SHOWWINDOW   (-24)
#define  GPD_STARTF_SIZE         (-20)
#define  GPD_STARTF_POSITION     (-16)
#define  GPD_STARTF_FLAGS        (-12)
#define  GPD_PARENT              (- 8)
#define  GPD_FLAGS               (- 4)
#define  GPD_USERDATA            (  0)

void        WINAPI DisposeLZ32Handle(HANDLE);
HANDLE      WINAPI DosFileHandleToWin32Handle(HFILE);
DWORD       WINAPI GetProcessDword(DWORD,INT);
VOID        WINAPI GetpWin16Lock(SYSLEVEL**);
DWORD       WINAPI MapLS(LPCVOID);
LPVOID      WINAPI MapSL(DWORD);
VOID        WINAPI ReleaseThunkLock(DWORD*);
VOID        WINAPI RestoreThunkLock(DWORD);
VOID        WINAPI UnMapLS(DWORD);
HFILE       WINAPI Win32HandleToDosFileHandle(HANDLE);
VOID        WINAPI _CheckNotSysLevel(SYSLEVEL *lock);
DWORD       WINAPI _ConfirmWin16Lock(void);
DWORD       WINAPI _ConfirmSysLevel(SYSLEVEL*);
VOID        WINAPI _EnterSysLevel(SYSLEVEL*);
VOID        WINAPI _LeaveSysLevel(SYSLEVEL*);


/* Wine internal functions */

extern char *wine_get_unix_file_name( LPCWSTR dos );
extern WCHAR *wine_get_dos_file_name( LPCSTR str );


/* a few optimizations for i386/gcc */

#if defined(__i386__) && defined(__GNUC__) && defined(__WINESRC__) && !defined(_NTSYSTEM_)

extern inline LONG WINAPI InterlockedCompareExchange( LONG volatile *dest, LONG xchg, LONG compare );
extern inline LONG WINAPI InterlockedCompareExchange( LONG volatile *dest, LONG xchg, LONG compare )
{
    LONG ret;
    __asm__ __volatile__( "lock; cmpxchgl %2,(%1)"
                          : "=a" (ret) : "r" (dest), "r" (xchg), "0" (compare) : "memory" );
    return ret;
}

extern inline LONG WINAPI InterlockedExchange( LONG volatile *dest, LONG val );
extern inline LONG WINAPI InterlockedExchange( LONG volatile *dest, LONG val )
{
    LONG ret;
    __asm__ __volatile__( "lock; xchgl %0,(%1)"
                          : "=r" (ret) :"r" (dest), "0" (val) : "memory" );
    return ret;
}

extern inline LONG WINAPI InterlockedExchangeAdd( LONG volatile *dest, LONG incr );
extern inline LONG WINAPI InterlockedExchangeAdd( LONG volatile *dest, LONG incr )
{
    LONG ret;
    __asm__ __volatile__( "lock; xaddl %0,(%1)"
                          : "=r" (ret) : "r" (dest), "0" (incr) : "memory" );
    return ret;
}

extern inline LONG WINAPI InterlockedIncrement( LONG volatile *dest );
extern inline LONG WINAPI InterlockedIncrement( LONG volatile *dest )
{
    return InterlockedExchangeAdd( dest, 1 ) + 1;
}

extern inline LONG WINAPI InterlockedDecrement( LONG volatile *dest );
extern inline LONG WINAPI InterlockedDecrement( LONG volatile *dest )
{
    return InterlockedExchangeAdd( dest, -1 ) - 1;
}

extern inline DWORD WINAPI GetLastError(void);
extern inline DWORD WINAPI GetLastError(void)
{
    DWORD ret;
    __asm__ __volatile__( ".byte 0x64\n\tmovl 0x34,%0" : "=r" (ret) );
    return ret;
}

extern inline DWORD WINAPI GetCurrentProcessId(void);
extern inline DWORD WINAPI GetCurrentProcessId(void)
{
    DWORD ret;
    __asm__ __volatile__( ".byte 0x64\n\tmovl 0x20,%0" : "=r" (ret) );
    return ret;
}

extern inline DWORD WINAPI GetCurrentThreadId(void);
extern inline DWORD WINAPI GetCurrentThreadId(void)
{
    DWORD ret;
    __asm__ __volatile__( ".byte 0x64\n\tmovl 0x24,%0" : "=r" (ret) );
    return ret;
}

extern inline void WINAPI SetLastError( DWORD err );
extern inline void WINAPI SetLastError( DWORD err )
{
    __asm__ __volatile__( ".byte 0x64\n\tmovl %0,0x34" : : "r" (err) : "memory" );
}

extern inline HANDLE WINAPI GetProcessHeap(void);
extern inline HANDLE WINAPI GetProcessHeap(void)
{
    HANDLE *pdb;
    __asm__ __volatile__( ".byte 0x64\n\tmovl 0x30,%0" : "=r" (pdb) );
    return pdb[0x18 / sizeof(HANDLE)];  /* get dword at offset 0x18 in pdb */
}

#else  /* __i386__ && __GNUC__ && __WINESRC__ && !_NTSYSTEM_ */

DWORD       WINAPI GetCurrentProcessId(void);
DWORD       WINAPI GetCurrentThreadId(void);
DWORD       WINAPI GetLastError(void);
HANDLE      WINAPI GetProcessHeap(void);
LONG        WINAPI InterlockedCompareExchange(LONG volatile*,LONG,LONG);
LONG        WINAPI InterlockedDecrement(LONG volatile*);
LONG        WINAPI InterlockedExchange(LONG volatile*,LONG);
LONG        WINAPI InterlockedExchangeAdd(LONG volatile*,LONG);
LONG        WINAPI InterlockedIncrement(LONG volatile*);
VOID        WINAPI SetLastError(DWORD);

#endif  /* __i386__ && __GNUC__ && __WINESRC__ && !_NTSYSTEM_ */

static inline PVOID WINAPI InterlockedCompareExchangePointer( PVOID volatile *dest, PVOID xchg, PVOID compare )
{
#if defined(__x86_64__) && defined(__GNUC__)
    PVOID ret;
    __asm__ __volatile__( "lock; cmpxchgq %2,(%1)"
                          : "=a" (ret) : "r" (dest), "r" (xchg), "0" (compare) : "memory" );
    return ret;
#else
    return (PVOID)InterlockedCompareExchange( (LONG volatile*)dest, (LONG)xchg, (LONG)compare );
#endif
}

static inline PVOID WINAPI InterlockedExchangePointer( PVOID volatile *dest, PVOID val )
{
#if defined(__x86_64__) && defined(__GNUC__)
    PVOID ret;
    __asm__ __volatile__( "lock; xchgq %0,(%1)"
                          : "=r" (ret) :"r" (dest), "0" (val) : "memory" );
    return ret;
#else
    return (PVOID)InterlockedExchange( (LONG volatile*)dest, (LONG)val );
#endif
}

#ifdef __WINESRC__
#define GetCurrentProcess() ((HANDLE)0xffffffff)
#define GetCurrentThread()  ((HANDLE)0xfffffffe)
#endif

/* WinMain(entry point) must be declared in winbase.h. */
/* If this is not declared, we cannot compile many sources written with C++. */
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int);

#ifdef __WINESRC__
/* shouldn't be here, but is nice for type checking */
BOOL WINAPI DllMain( HINSTANCE hinst, DWORD reason, LPVOID reserved ) DECLSPEC_HIDDEN;
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __WINE_WINBASE_H */
