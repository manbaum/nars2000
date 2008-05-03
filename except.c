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
};

#define EXCEPT_NAMES_LENGTH         (sizeof (ExceptNames) / sizeof (ExceptNames[0]))


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
    (LPCHAR ExceptionText)      // Exception text

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
//  $CheckVirtAlloc
//
//  Check on virtual allocs
//***************************************************************************

long CheckVirtAlloc
    (LPEXCEPTION_POINTERS lpExcept,
     LPMEMVIRTSTR         lpMemVirtStr,
     UINT                 uMemVirtCnt,
     LPCHAR               lpText)

{
    LPUCHAR lpInvalidAddr;      // Ptr to invalid address
    UINT    uMem;               // Loop counter
    LPUCHAR lpIniAddr;          // Ptr to invalid address

    // Handle access violations only
    if (lpExcept->ExceptionRecord->ExceptionCode EQ EXCEPTION_ACCESS_VIOLATION)
    {
        // Get the invalid address
        lpInvalidAddr = (LPUCHAR) lpExcept->ExceptionRecord->ExceptionInformation[1];

        // Check for global VirtualAlloc memory that needs to be expanded
        for (uMem = 0; uMem < uMemVirtCnt; uMem++)
        {
            // Get the initial address
            lpIniAddr = lpMemVirtStr[uMem].IniAddr;

            // If it's within range for this VirtualAlloc address, ...
            if (lpIniAddr <= lpInvalidAddr
             &&              lpInvalidAddr < (lpIniAddr + lpMemVirtStr[uMem].MaxSize))
            {
                // Allocate more memory
                if (VirtualAlloc (lpInvalidAddr,
                                  lpMemVirtStr[uMem].IncrSize,
                                  MEM_COMMIT,
                                  PAGE_READWRITE) NE NULL)
                    return EXCEPTION_CONTINUE_EXECUTION;
            } // End IF
        } // End FOR
    } // End IF

    return EXCEPTION_CONTINUE_SEARCH;
} // End CheckVirtAlloc


//***************************************************************************
//  $CheckException
//
//  Check on a structured exception
//***************************************************************************

long CheckException
    (LPEXCEPTION_POINTERS lpExcept,         // Ptr to exception information
     LPCHAR               lpText)           // Ptr to text of exception handler

{
    UINT    uMem;               // Loop counter
    LPUCHAR lpInvalidAddr;      // Ptr to invalid address

    // Save the exception code, address, and text for later use
    MySetExceptionCode (lpExcept->ExceptionRecord->ExceptionCode);
    MySetExceptionAddr (lpExcept->ExceptionRecord->ExceptionAddress);
    MySetExceptionText (lpText);

    // Split cases based upon the exception code
    switch (lpExcept->ExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            // Get the invalid address
            lpInvalidAddr = (LPUCHAR) lpExcept->ExceptionRecord->ExceptionInformation[1];

            // Save for later use
            MySetInvalidAddr (lpInvalidAddr);

            // Check for global VirtualAlloc memory that needs to be expanded
            for (uMem = 0; uMem < uMemVirtCnt; uMem++)
            {
                LPUCHAR lpIniAddr;

                // Get the initial address
                lpIniAddr = memVirtStr[uMem].IniAddr;

                // If it's within range for this VirtualAlloc address, ...
                if (lpIniAddr <= lpInvalidAddr
                 &&              lpInvalidAddr < (lpIniAddr + memVirtStr[uMem].MaxSize))
                {
                    // Allocate more memory
                    if (VirtualAlloc (lpInvalidAddr,
                                      1,
                                      MEM_COMMIT,
                                      PAGE_READWRITE) NE NULL)
                        return EXCEPTION_CONTINUE_EXECUTION;
                } // End IF
            } // End FOR

#if 0
            {
                DWORD   regEBP;             // Stack trace ptr
                LPBYTE  caller;             // Ptr to caller in stack trace

                DbgBrk ();

                // Do a stack trace
                regEBP = lpExcept->ContextRecord->Ebp;

                do
                {
                    if (regEBP & 3
                     || IsBadReadPtr ((LPVOID) regEBP, 8))
                        break;

                    caller = ((LPBYTE *) regEBP)[1];
                    regEBP = *(DWORD *) regEBP;
                } while (TRUE);
            } // End ***DEBUG***
#endif
            // Fall through to common handler execution

////////case EXCEPTION_RESULT_BOOL:
////////case EXCEPTION_RESULT_INT:
        case EXCEPTION_RESULT_FLOAT:
        case EXCEPTION_DOMAIN_ERROR:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_SINGLE_STEP:
        case EXCEPTION_BREAKPOINT:
        case EXCEPTION_GUARD_PAGE:
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

int __cdecl CompareStartAddresses
    (const void *elem1,
     const void *elem2)

{
#define lpSALft     ((LPSTART_ADDRESSES) elem1)
#define lpSARht     ((LPSTART_ADDRESSES) elem2)

    return lpSALft->StartAddressAddr
         - lpSARht->StartAddressAddr;
#undef  lpSARht
#undef  lpSALft
} // End CompareStartAddresses


//***************************************************************************
//  $DisplayException
//
//  Display an exception code, address, etc.
//***************************************************************************

void DisplayException
    (void)

{
    char         szTemp[1024];  // Temp output save area
    int          exceptIndex;   // Exception index
    UINT         exceptCode,    // Exception code
                 nearAddress,   // Offset from closest address
                 nearIndex;     // Index into StartAddresses
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPCHAR       exceptText;    // Ptr to exception text
    LPUCHAR      exceptAddr;    // Exception address

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

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Find the address closest to and at or below the given address
    // If the address is not found, it could be that we're
    //   running under a debugger and the debugger has changed the
    //   starting address of the routine to a near JMP instruction,
    //   so try again with that assumption
    if (START_ADDRESSES_LENGTH EQ FindRoutineAddress (exceptAddr, &nearAddress, &nearIndex, TRUE))
        FindRoutineAddress (exceptAddr, &nearAddress, &nearIndex, TRUE);

    for (exceptIndex = 0; exceptIndex < EXCEPT_NAMES_LENGTH; exceptIndex++)
    if (exceptCode EQ ExceptNames[exceptIndex].ExceptCode)
        break;

    wsprintf (szTemp,
              "WRITE THIS DOWN!! -- "
              "Exception code = %08X (%s) at %08X"
              " (%s + %08X)"
              " from %s",
              exceptCode,
              (exceptIndex EQ EXCEPT_NAMES_LENGTH)
                ? "Exception Unknown"
                : ExceptNames[exceptIndex].ExceptName,
              exceptAddr,
              StartAddresses[nearIndex].StartAddressName,
              nearAddress,
              exceptText);
#ifdef DEBUG
    DbgMsg (szTemp);
    DbgStop ();
#else
    MB (szTemp);
    exit (exceptCode);
#endif
} // End DisplayException


//***************************************************************************
//  $FindRoutineAddress
//
//  Find the address of the routine at or below a given address
//***************************************************************************

UINT FindRoutineAddress
    (LPUCHAR exceptAddr,            // Exception address
     LPUINT lpNearAddress,          // Ptr to offset from closest address
     LPUINT lpNearIndex,            // Ptr to index into StartAddresses
     BOOL   bDebugger)              // TRUE iff test for running under debugger

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
         && *startAddr EQ 0xE9)
            // Skip over the instruction ("5 +") and add in the
            //   value of the near offset arg to the JMP
            startAddr += 5 + *(LPUINT) &startAddr[1];

        // Check against the actual address
        if (exceptAddr >= startAddr
         && ((UINT) (exceptAddr - startAddr)) < *lpNearAddress)
        {
            *lpNearAddress = exceptAddr - startAddr;
            *lpNearIndex   = i;
        } // End IF
    } // End FOR

    return i;
} // End FindRoutineAddress


//***************************************************************************
//  End of File: except.c
//***************************************************************************
