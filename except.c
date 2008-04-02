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
                                  1,
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
    (LPEXCEPTION_POINTERS lpExcept,
     LPCHAR               lpText)

{
    UINT    uMem;               // Loop counter
    LPUCHAR lpInvalidAddr;      // Ptr to invalid address

    // Save the exception code & address for later use
    MySetExceptionCode (lpExcept->ExceptionRecord->ExceptionCode);
    MySetExceptionAddr (lpExcept->ExceptionRecord->ExceptionAddress);
    MySetExceptionText (lpText);

    // Split cases based upon the exception code
    switch (MyGetExceptionCode ())
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


#ifndef DEBUG
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
#endif


#ifndef DEBUG
//***************************************************************************
//  $_DisplayException
//
//  Display an exception code, address, etc.
//***************************************************************************

void _DisplayException
    (void)

{
    char         szTemp[1024];
    int          exceptIndex;
    UINT         exceptCode,
                 nearAddress,
                 nearIndex,
                 i;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPCHAR       exceptText;    // Ptr to exception text
    LPUCHAR      exceptAddr;    // Ptr to exception addr

    // Sort the StartAddresses
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
    for (i = 0,
           nearAddress = NEG1U,
           nearIndex   = START_ADDRESSES_LENGTH;
         i < START_ADDRESSES_LENGTH;
         i++)
    if (exceptAddr >= StartAddresses[i].StartAddressAddr
     && ((UINT) (exceptAddr - StartAddresses[i].StartAddressAddr)) < nearAddress)
    {
        nearAddress = exceptAddr - StartAddresses[i].StartAddressAddr;
        nearIndex   = i;
    } // End FOR/IF

    for (exceptIndex = 0; exceptIndex < EXCEPT_NAMES_LENGTH; exceptIndex++)
    if (exceptCode EQ ExceptNames[exceptIndex].ExceptCode)
        break;

    wsprintf (szTemp,
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
    MB (szTemp);

    exit (1);
} // End _DisplayException
#endif


//***************************************************************************
//  End of File: except.c
//***************************************************************************
