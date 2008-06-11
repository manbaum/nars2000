//***************************************************************************
//  NARS2000 -- Exception Handling
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "sis.h"
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

#include "startaddr.h"

typedef struct tagEXCEPT_NAMES
{
    char *ExceptName;
    UINT  ExceptCode;
} EXCEPT_NAMES, *LPEXCEPT_NAMES;

EXCEPT_NAMES ExceptNames[] =
{
 {"FLT_DIVIDE_BY_ZERO", EXCEPTION_FLT_DIVIDE_BY_ZERO},
 {"INT_DIVIDE_BY_ZERO", EXCEPTION_INT_DIVIDE_BY_ZERO},
 {"ACCESS_VIOLATION"  , EXCEPTION_ACCESS_VIOLATION  },
 {"SINGLE_STEP"       , EXCEPTION_SINGLE_STEP       },
 {"BREAKPOINT"        , EXCEPTION_BREAKPOINT        },
 {"LIMIT ERROR"       , EXCEPTION_LIMIT_ERROR       },
};

#define EXCEPT_NAMES_LENGTH         (sizeof (ExceptNames) / sizeof (ExceptNames[0]))

// Save area for exception address if EXCEPTION_BREAKPOINT
DWORD glbExceptAddr;

// Save area for crash information
CONTEXT ContextRecord;
EXCEPTION_RECORD ExceptionRecord;


//***************************************************************************
//  $MyGetExceptionCode
//
//  Return the current ExceptionCode
//***************************************************************************

EXCEPTION_CODES MyGetExceptionCode
    (void)

{
    HGLOBAL         hGlbPTD;        // PerTabData global memory handle
    LPPERTABDATA    lpMemPTD;       // Ptr to PerTabData global memory
    EXCEPTION_CODES ExceptionCode;  // Exception code

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // If hGlbPTD isn't set, just exit
    if (hGlbPTD EQ NULL)
        return EXCEPTION_SUCCESS;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the ExceptionCode
    ExceptionCode = lpMemPTD->ExceptionCode;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return ExceptionCode;
} // End MyGetExceptionCode


//***************************************************************************
//  $MyGetInvalidAddr
//
//  Return the current InvalidAddr
//***************************************************************************

LPUCHAR MyGetInvalidAddr
    (void)

{
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPUCHAR      lpInvalidAddr;     // Ptr to invalid address

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // If hGlbPTD isn't set, just exit
    if (hGlbPTD EQ NULL)
        return NULL;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the ContextRecord
    lpInvalidAddr = lpMemPTD->lpInvalidAddr;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpInvalidAddr;
} // End MyGetInvalidAddr


//***************************************************************************
//  $MySetExceptionCode
//
//  Set the current ExceptionCode
//***************************************************************************

void MySetExceptionCode
    (EXCEPTION_CODES ExceptionCode) // Exception code

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // If hGlbPTD isn't set, just exit
    if (hGlbPTD EQ NULL)
        return;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set the ExceptionCode
    lpMemPTD->ExceptionCode = ExceptionCode;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End MySetExceptionCode


//***************************************************************************
//  $MySetExceptionAddr
//
//  Set the current ExceptionAddr
//***************************************************************************

void MySetExceptionAddr
    (LPVOID ExceptionAddr)      // Exception address

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // If hGlbPTD isn't set, just exit
    if (hGlbPTD EQ NULL)
        return;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set the ExceptionAddr
    lpMemPTD->ExceptionAddr = (LPVOID) ExceptionAddr;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End MySetExceptionAddr


//***************************************************************************
//  $MySetExceptionText
//
//  Set the current ExceptionText
//***************************************************************************

void MySetExceptionText
    (LPWCHAR ExceptionText)     // Exception text

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // If hGlbPTD isn't set, just exit
    if (hGlbPTD EQ NULL)
        return;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set the ExceptionText
    lpMemPTD->ExceptionText = ExceptionText;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End MySetExceptionText


//***************************************************************************
//  $MySetInvalidAddr
//
//  Set the current InvalidAddr
//***************************************************************************

void MySetInvalidAddr
    (LPUCHAR lpInvalidAddr)     // Ptr to invalid address

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // If hGlbPTD isn't set, just exit
    if (hGlbPTD EQ NULL)
        return;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set the InvalidAddr
    lpMemPTD->lpInvalidAddr = lpInvalidAddr;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End MySetInvalidAddr


//***************************************************************************
//  $MySetRegisterEBP
//
//  Set the current EBP
//***************************************************************************

void MySetRegisterEBP
    (UINT RegisterEBP)          // Register EBP contents

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // If hGlbPTD isn't set, just exit
    if (hGlbPTD EQ NULL)
        return;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set the register
    lpMemPTD->RegisterEBP = RegisterEBP;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End MySetRegisterEBP


//***************************************************************************
//  $CheckVirtAlloc
//
//  Check on virtual allocs
//***************************************************************************

long CheckVirtAlloc
    (LPEXCEPTION_POINTERS lpExcept, // Ptr to exception & context records
     LPWCHAR              lpText)   // Ptr to text of exception handler

{
    LPUCHAR      lpInvalidAddr;     // Ptr to invalid address
    int          iRet;              // Return code

    // Handle access violations only
    if (lpExcept->ExceptionRecord->ExceptionCode EQ EXCEPTION_ACCESS_VIOLATION)
    {
        // Get the invalid address
        lpInvalidAddr = (LPUCHAR) lpExcept->ExceptionRecord->ExceptionInformation[1];

        // Check on virtual allocs from <memVirtStr>
        iRet = CheckMemVirtStr (lpInvalidAddr);
        if (iRet)
            return iRet;

        // Check on virtual allocs in the <lpMemPTD->lpLstMVS> chain
        iRet = CheckPTDVirtStr (lpInvalidAddr);
        if (iRet)
            return iRet;
    } // End IF

    return EXCEPTION_CONTINUE_SEARCH;
} // End CheckVirtAlloc


//***************************************************************************
//  $CheckPTDVirtStr
//
//  Check on virtual allocs in the <lpMemPTD->lpLstMVS> chain
//***************************************************************************

int CheckPTDVirtStr
    (LPUCHAR lpInvalidAddr)         // Ptr to invalid address

{
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPMEMVIRTSTR lpLstMVS;          // Ptr to last MEMVIRTSTR (NULL = none)
    LPUCHAR      lpIniAddr;         // Ptr to invalid address

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    if (hGlbPTD EQ NULL)
        return 0;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the ptr to the last MVS
    lpLstMVS = lpMemPTD->lpLstMVS;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Check for global VirtualAlloc memory that needs to be expanded
    while (lpLstMVS)
    {
        // Get the initial address
        lpIniAddr = lpLstMVS->IniAddr;

        // If it's within range for this VirtualAlloc address, ...
        if (lpIniAddr <= lpInvalidAddr
         &&              lpInvalidAddr < (lpIniAddr + lpLstMVS->MaxSize))
        {
            // Allocate more memory
            if (VirtualAlloc (lpInvalidAddr,
                              lpLstMVS->IncrSize,
                              MEM_COMMIT,
                              PAGE_READWRITE) NE NULL)
                return EXCEPTION_CONTINUE_EXECUTION;
            else
            // Can't allocate more memory??
            {
                MessageBoxW (NULL,
                             L"Not enough memory for <VirtualAlloc> in <CheckPTDVirtStr>",
                             lpwszAppName,
                             MB_OK | MB_ICONERROR);
                RaiseException (EXCEPTION_LIMIT_ERROR, 0, 0, NULL);
            } // End IF/ELSE
        } else
        {
            // Skip to the guard page address
            lpIniAddr += lpLstMVS->MaxSize;

            // Check for the guard page
            if (lpIniAddr <= lpInvalidAddr
             &&              lpInvalidAddr <  (lpIniAddr + PAGESIZE))
            {
                MySetExceptionCode (EXCEPTION_LIMIT_ERROR);
#ifdef DEBUG
                MBC ("LIMIT ERROR encountered");
                DbgBrk ();
#endif
                return EXCEPTION_EXECUTE_HANDLER;
            } // End IF
        } // End IF/ELSE

        // Get the previous ptr in the chain
        lpLstMVS = lpLstMVS->lpPrvMVS;
    } // End FOR

    // Mark as no match
    return 0;
} // End CheckPTDVirtStr


//***************************************************************************
//  $CheckMemVirtStr
//
//  Check on virtual allocs from <memVirtStr>
//***************************************************************************

int CheckMemVirtStr
    (LPUCHAR lpInvalidAddr)         // Ptr to invalid address

{
    UINT    uMem;                   // Loop counter
    LPUCHAR lpIniAddr;              // Ptr to initial address

    // Check for global VirtualAlloc memory that needs to be expanded
    for (uMem = 0; uMem < uMemVirtCnt; uMem++)
    {
        // Get the initial address
        lpIniAddr = memVirtStr[uMem].IniAddr;

        // If it's within range for this VirtualAlloc address, ...
        if (lpIniAddr <= lpInvalidAddr
         &&              lpInvalidAddr < (lpIniAddr + memVirtStr[uMem].MaxSize))
        {
            // Allocate more memory
            if (VirtualAlloc (lpInvalidAddr,
                              memVirtStr[uMem].IncrSize,
                              MEM_COMMIT,
                              PAGE_READWRITE) NE NULL)
                return EXCEPTION_CONTINUE_EXECUTION;
            else
            // Can't allocate more memory??
            {
                MessageBoxW (NULL,
                             L"Not enough memory for <VirtualAlloc> in <CheckMemVirtStr>",
                             lpwszAppName,
                             MB_OK | MB_ICONERROR);
                RaiseException (EXCEPTION_LIMIT_ERROR, 0, 0, NULL);
            } // End IF/ELSE
        } // End IF
    } // End FOR

    // Mark as no match
    return 0;
} // End CheckMemVirtStr


//***************************************************************************
//  $CheckException
//
//  Check on a structured exception
//***************************************************************************

long CheckException
    (LPEXCEPTION_POINTERS lpExcept,         // Ptr to exception information
     LPWCHAR              lpText)           // Ptr to text of exception handler

{
    int     iRet;               // Return code
    LPUCHAR lpInvalidAddr;      // Ptr to invalid address

    // Save in globals
    ContextRecord   = *lpExcept->ContextRecord;
    ExceptionRecord = *lpExcept->ExceptionRecord;

    // Get the invalid address
    lpInvalidAddr = (LPUCHAR) lpExcept->ExceptionRecord->ExceptionInformation[1];

    // Save the exception code, address, and text for later use
    MySetExceptionCode (lpExcept->ExceptionRecord->ExceptionCode);
    MySetExceptionAddr (lpExcept->ExceptionRecord->ExceptionAddress);
    MySetExceptionText (lpText);
    MySetRegisterEBP   (lpExcept->ContextRecord->Ebp);
    MySetInvalidAddr   (lpInvalidAddr);

    // Split cases based upon the exception code
    switch (lpExcept->ExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            // Check on virtual allocs from <memVirtStr>
            iRet = CheckMemVirtStr (lpInvalidAddr);
            if (iRet)
                return iRet;

            // Check on virtual allocs in the <lpMemPTD->lpLstMVS> chain
            iRet = CheckPTDVirtStr (lpInvalidAddr);
            if (iRet)
                return iRet;

            // Fall through to common handler execution

////////case EXCEPTION_RESULT_BOOL:
////////case EXCEPTION_RESULT_INT:
        case EXCEPTION_RESULT_FLOAT:
        case EXCEPTION_DOMAIN_ERROR:
        case EXCEPTION_LIMIT_ERROR:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_SINGLE_STEP:
        case EXCEPTION_GUARD_PAGE:
            return EXCEPTION_EXECUTE_HANDLER;

        case EXCEPTION_BREAKPOINT:
            // In this case, we need to know who called us,
            //   so we can report it to the end user

            // Save our return address for later use
            glbExceptAddr = *(LPDWORD) ULongToPtr (lpExcept->ContextRecord->Esp);

            return EXCEPTION_EXECUTE_HANDLER;

        case EXCEPTION_CTRL_BREAK:
            return EXCEPTION_CONTINUE_EXECUTION;

        default:
            return EXCEPTION_CONTINUE_SEARCH;
    } // End SWITCH
} // End CheckException


//***************************************************************************
//  $CompareStartAddresses
//
//  Compare starting addresses so as to sort them
//***************************************************************************

UINT __cdecl CompareStartAddresses
    (const void *elem1,
     const void *elem2)

{
#define lpSALft     ((LPSTART_ADDRESSES) elem1)
#define lpSARht     ((LPSTART_ADDRESSES) elem2)

    return (UINT) (lpSALft->StartAddressAddr
                 - lpSARht->StartAddressAddr);
#undef  lpSARht
#undef  lpSALft
} // End CompareStartAddresses


//***************************************************************************
//  $IsGoodReadPtr
//
//  Return TRUE iff the given ptr is valid for reading a given # bytes
//***************************************************************************

BOOL IsGoodReadPtr
    (LPBYTE lpReadPtr,
     DWORD  dwBytes)

{
    DWORD dwCnt;
    BYTE  dwRead;

    __try
    {
        for (dwCnt = 0; dwCnt < dwBytes; dwCnt++)
            dwRead += *lpReadPtr++;

        return TRUE;
    } __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return FALSE;
    } // End __try/__except
} // End IsGoodReadPtr


//***************************************************************************
//  $DisplayException
//
//  Display an exception code, address, etc.
//***************************************************************************

void DisplayException
    (void)

{
    WCHAR        wszTemp[1024]; // Temp output save area
    int          exceptIndex;   // Exception index
    UINT         exceptCode,    // Exception code
                 nearAddress,   // Offset from closest address
                 nearIndex,     // Index into StartAddresses
                 nearAddress0,  // Offset from closest address
                 nearIndex0,    // Index into StartAddresses
                 nearAddress1,  // Offset from closest address
                 nearIndex1,    // Index into StartAddresses
                 uMem,          // Loop counter
                 SILevel;       // The current SI level
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPWCHAR      exceptText;    // Ptr to exception text
    LPUCHAR      exceptAddr;    // Exception address
    DWORD        regEBP;        // Stack trace ptr
    LPBYTE       caller;        // Ptr to caller in stack trace
    LPSIS_HEADER lpSISCur;      // Ptr to current SIS header
    LPMEMVIRTSTR lpLstMVS;      // Ptr to last MEMVIRTSTR (NULL = none)

    // Sort the StartAddresses in ascending order by address
    qsort (StartAddresses,
           START_ADDRESSES_LENGTH,
           sizeof (StartAddresses[0]),
          &CompareStartAddresses);

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // If hGlbPTD isn't set, just exit
    if (hGlbPTD EQ NULL)
        return;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Save the exception code & address, & text
    exceptCode = lpMemPTD->ExceptionCode;
    exceptAddr = lpMemPTD->ExceptionAddr;
    exceptText = lpMemPTD->ExceptionText;
    regEBP     = lpMemPTD->RegisterEBP;
    lpSISCur   = lpMemPTD->lpSISCur;
    lpLstMVS   = lpMemPTD->lpLstMVS;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // If the exception is EXCEPTION_BREAKPOINT (from DbgStop),
    //   we need to display the return address as that's from
    //   where we were called.  Displaying DbgStop address is
    //   of no help
    if (exceptCode EQ EXCEPTION_BREAKPOINT)
        exceptAddr = *(LPUCHAR *) &glbExceptAddr;

    // Find the address closest to and at or below the given address
    // If the address is not found, it could be that we're
    //   running under a debugger and the debugger has changed the
    //   starting address of the routine to a near JMP instruction,
    //   so try again with that assumption
    FindRoutineAddress (exceptAddr, &nearAddress0, &nearIndex0, FALSE);
    FindRoutineAddress (exceptAddr, &nearAddress1, &nearIndex1, TRUE);
    if (nearAddress0 < nearAddress1)
    {
        nearAddress = nearAddress0;
        nearIndex   = nearIndex0;
    } else
    {
        nearAddress = nearAddress1;
        nearIndex   = nearIndex1;
    } // End IF/ELSE

    for (exceptIndex = 0; exceptIndex < EXCEPT_NAMES_LENGTH; exceptIndex++)
    if (exceptCode EQ ExceptNames[exceptIndex].ExceptCode)
        break;

    ShowWindow (hWndCC, SW_SHOWNORMAL);
    UpdateWindow (hWndCC);

#define NewMsg(a)   SendMessageW (hWndCC, LB_ADDSTRING, 0, (LPARAM) (a)); UpdateWindow (hWndCC)

    NewMsg (L"COPY THIS TEXT TO AN EMAIL MESSAGE");
    NewMsg (L"----------------------------------------------------");
    NewMsg (L"Use Right-click:  Select All, and right-click:  Copy");
    NewMsg (L"   to copy the entire text to the clipboard."        );
    NewMsg (L"----------------------------------------------------");
    NewMsg (L"Send the text to <nars2000-discuss@googlegroups.com>");
    NewMsg (L"   along with a detailed statement of what you were" );
    NewMsg (L"   doing just prior to the crash."                   );
    NewMsg (L"----------------------------------------------------");

    wsprintfW (wszTemp,
               L"Exception code = %08X (%S)",
               exceptCode,
               (exceptIndex EQ EXCEPT_NAMES_LENGTH)
                 ? "Exception Unknown"
                 : ExceptNames[exceptIndex].ExceptName);
    NewMsg (wszTemp);

    wsprintfW (wszTemp,
               L"   at %p (%S + %p)",
               exceptAddr,
               StartAddresses[nearIndex].StartAddressName,
               nearAddress);
    NewMsg (wszTemp);

    wsprintfW (wszTemp,
               L"   from %s",
               exceptText);
    NewMsg (wszTemp);

    // Display the registers
    NewMsg (L"");
    NewMsg (L"== REGISTERS ==");
    wsprintfW (wszTemp,
               L"EAX = %08X EBX = %08X ECX = %08X EDX = %08X EIP = %08X",
               ContextRecord.Eax,
               ContextRecord.Ebx,
               ContextRecord.Ecx,
               ContextRecord.Edx,
               ContextRecord.Eip);
    NewMsg (wszTemp);

    wsprintfW (wszTemp,
               L"ESI = %08X EDI = %08X EBP = %08X ESP = %08X EFL = %08X",
               ContextRecord.Esi,
               ContextRecord.Edi,
               ContextRecord.Ebp,
               ContextRecord.Esp,
               ContextRecord.EFlags);
    NewMsg (wszTemp);

    wsprintfW (wszTemp,
               L"CS = %04X DS = %04X ES = %04X FS = %04X GS = %04X SS = %04X CR2 = %08X",
               ContextRecord.SegCs,
               ContextRecord.SegDs,
               ContextRecord.SegEs,
               ContextRecord.SegFs,
               ContextRecord.SegGs,
               ContextRecord.SegSs,
               ExceptionRecord.ExceptionInformation[1]);
    NewMsg (wszTemp);

    // Display the backtrace
    NewMsg (L"");
    NewMsg (L"== BACKTRACE ==");

    wsprintfW (wszTemp,
               L"%p (%S + %p)",
               exceptAddr,
               StartAddresses[nearIndex].StartAddressName,
               nearAddress);
    NewMsg (wszTemp);

    // Do a stack trace
    while (TRUE)
    {
        // If the register is misaligned or a bad ptr, quit
        if (regEBP & 3
         || !IsGoodReadPtr ((LPVOID) regEBP, 8))
            break;

        // Get the caller's address
        caller = ((LPBYTE *) regEBP)[1];

        // Check for all done
        if (caller EQ 0)
            break;

        FindRoutineAddress (caller, &nearAddress0, &nearIndex0, FALSE);
        FindRoutineAddress (caller, &nearAddress1, &nearIndex1, TRUE);
        if (nearAddress0 < nearAddress1)
        {
            nearAddress = nearAddress0;
            nearIndex   = nearIndex0;
        } else
        {
            nearAddress = nearAddress1;
            nearIndex   = nearIndex1;
        } // End IF/ELSE

        // If the address is out of bounds, just display the address
        if (nearAddress > 0x00100000)
            // Format the addresses
            wsprintfW (wszTemp,
                       L"%p -- EBP = %08X",
                       caller,
                       regEBP);
        else
            // Format the addresses
            wsprintfW (wszTemp,
                       L"%p (%S + %p) -- EBP = %08X",
                       caller,
                       StartAddresses[nearIndex].StartAddressName,
                       nearAddress,
                       regEBP);
        NewMsg (wszTemp);

        // Handle case where we get stuck
        if (regEBP EQ *(DWORD *) regEBP)
            break;

        // Get the next EBP
        regEBP = *(DWORD *) regEBP;
    } // End WHILE

    // Display the virtual memory ranges
    NewMsg (L"");
    NewMsg (L"== MEMVIRTSTR ==");
    NewMsg (L" IniAddr IncrSize  MaxSize");

    // Check for global VirtualAlloc memory that needs to be expanded
    for (uMem = 0; uMem < uMemVirtCnt; uMem++)
    {
        wsprintfW (wszTemp,
#ifdef DEBUG
                   L"%p %08X %08X %S",
#else
                   L"%p %08X %08X",
#endif
                   memVirtStr[uMem].IniAddr,
                   memVirtStr[uMem].IncrSize,
                   memVirtStr[uMem].MaxSize
#ifdef DEBUG
                 , memVirtStr[uMem].lpText
#endif
                   );
        NewMsg (wszTemp);
    } // End FOR

    // Display the local virtual memory ranges
    NewMsg (L"");
    NewMsg (L"== LCLMEMVIRTSTR ==");
    NewMsg (L" IniAddr IncrSize  MaxSize");

    while (lpLstMVS)
    {
        wsprintfW (wszTemp,
#ifdef DEBUG
                   L"%p %08X %08X %S",
#else
                   L"%p %08X %08X",
#endif
                   lpLstMVS->IniAddr,
                   lpLstMVS->IncrSize,
                   lpLstMVS->MaxSize
#ifdef DEBUG
                 , lpLstMVS->lpText
#endif
                   );
        NewMsg (wszTemp);

        // Get the previous ptr in the chain
        lpLstMVS = lpLstMVS->lpPrvMVS;
    } // End WHILE

    // Display the SI stack
    NewMsg (L"");
    NewMsg (L"== SI STACK ==");

    // Loop backwards through the SI levels
    for (SILevel = 0;
         lpSISCur;
         lpSISCur = lpSISCur->lpSISPrv, SILevel++)
    {
        LPAPLCHAR lpMemName;            // Ptr to function name global memory

        // Split cases based upon the caller's function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_IMM:
#ifdef DEBUG
                NewMsg (WS_UTF16_IOTA);
#endif
                break;

            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (lpSISCur->hGlbFcnName);

                // Format the Name, Line #, and Suspension marker
                wsprintfW (wszTemp,
                           L"%s[%d] %c",
                           lpMemName,
                           lpSISCur->CurLineNum,
                           " *"[lpSISCur->Suspended]);
                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

                // Display the function name & line #
                NewMsg (wszTemp);

                break;

            case DFNTYPE_EXEC:
                NewMsg (WS_UTF16_UPTACKJOT);

                break;

            case DFNTYPE_QUAD:
                NewMsg (WS_UTF16_QUAD);

                break;

            case DFNTYPE_UNK:
            default:
                NewMsg (L"***UNKNOWN***");

                break;
        } // End SWITCH
    } // End FOR

    // Tell the Crash Control window to display a MessageBox
    SendMessageW (hWndCC, MYWM_DISPMB, 0, 0);

#undef  NewMsg

#ifdef DEBUG
    DbgBrk ();
#else
    exit (exceptCode);
#endif
} // End DisplayException


//***************************************************************************
//  $FindRoutineAddress
//
//  Find the address of the routine at or below a given address
//***************************************************************************

void FindRoutineAddress
    (LPUCHAR exceptAddr,            // Exception address
     LPUINT  lpNearAddress,         // Ptr to offset from closest address
     LPUINT  lpNearIndex,           // Ptr to index into StartAddresses
     BOOL    bDebugger)             // TRUE iff test for running under debugger

{
    UINT    i;                      // Loop counter
    LPUCHAR startAddr;              // Ptr to starting addr

    // Find the address closest to and at or below the given address
    for (i = 0,
           *lpNearAddress = NEG1U,
           *lpNearIndex   = START_ADDRESSES_LENGTH;
         i < START_ADDRESSES_LENGTH;
         i++)
    {
        // Get the starting address
        startAddr = StartAddresses[i].StartAddressAddr;

        // If we're testing for debugger and it's a near JMP (E9 xx xx xx xx), ...
        if (bDebugger
         && IsGoodReadPtr (startAddr, 5)
         && *startAddr EQ 0xE9)
            // Skip over the instruction ("5 +") and add in the
            //   value of the near offset arg to the JMP
            startAddr += 5 + *(LPUINT) &startAddr[1];

        // Check against the actual address
        if (exceptAddr >= startAddr
         && ((UINT) (exceptAddr - startAddr)) < *lpNearAddress)
        {
            *lpNearAddress = (UINT) (exceptAddr - startAddr);
            *lpNearIndex   = i;
        } // End IF
    } // End FOR
} // End FindRoutineAddress


//***************************************************************************
//  $GuardAlloc
//
//  VirtualAlloc with a guard page
//***************************************************************************

LPVOID GuardAlloc
    (LPVOID lpvAddress,         // Address of region to reserve or commit
     DWORD  cbSize,             // Size of region
     DWORD  fdwAllocationType,  // Type of allocation
     DWORD  fdwProtect)         // Type of access protection

{
    LPVOID lpRetAddr;           // Return address

    // Round up the allocation size to a page boundary
    cbSize = PAGESIZE * ((cbSize + PAGESIZE - 1) / PAGESIZE);

    lpRetAddr =
      MyVirtualAlloc (lpvAddress,
                      cbSize + PAGESIZE,// Size of region with an extra guard page
                      fdwAllocationType,
                      fdwProtect);
    return lpRetAddr;
} // End GuardAlloc


//***************************************************************************
//  $LinkMVS
//
//  Link an entry into the MemVirtStr chain
//***************************************************************************

void LinkMVS
    (LPMEMVIRTSTR lpCurMVS)         // Ptr to current MVS entry

{
    HGLOBAL         hGlbPTD;        // PerTabData global memory handle
    LPPERTABDATA    lpMemPTD;       // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // There is no next entry for us
    lpCurMVS->lpNxtMVS = NULL;

    // Our previous entry is the last entry
    lpCurMVS->lpPrvMVS = lpMemPTD->lpLstMVS;

    // If there's a previous entry, ...
    if (lpCurMVS->lpPrvMVS)
        // Its next entry is us
        lpCurMVS->lpPrvMVS->lpNxtMVS = lpCurMVS;

    // We are the new last entry
    lpMemPTD->lpLstMVS = lpCurMVS;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End LinkMVS


//***************************************************************************
//  $UnlinkMVS
//
//  Unlink an entry from the MemVirtStr chain
//***************************************************************************

void UnlinkMVS
    (LPMEMVIRTSTR lpCurMVS)         // Ptr to current MVS entry

{
    HGLOBAL         hGlbPTD;        // PerTabData global memory handle
    LPPERTABDATA    lpMemPTD;       // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If there's a previous entry, ...
    if (lpCurMVS->lpPrvMVS)
        // Unlink us from it
        lpCurMVS->lpPrvMVS->lpNxtMVS = lpCurMVS->lpNxtMVS;

    // If there's a next entry, ...
    if (lpCurMVS->lpNxtMVS)
        // Unlink us from it
        lpCurMVS->lpNxtMVS->lpPrvMVS = lpCurMVS->lpPrvMVS;

    // If we're the last entry, ...
    if (lpMemPTD->lpLstMVS EQ lpCurMVS)
        // Set the new last entry as our previous entry
        lpMemPTD->lpLstMVS = lpCurMVS->lpPrvMVS;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End UnlinkMVS


//***************************************************************************
//  End of File: except.c
//***************************************************************************
