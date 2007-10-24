//***************************************************************************
//  NARS2000 -- Exception Handling
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
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

    if (hGlbPTD EQ NULL)            // ***DEBUG***
        MB ("hGlbPTD EQ NULL");

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    if (lpMemPTD EQ NULL)           // ***DEBUG***
        MB ("lpMemPTD EQ NULL");

    // Get the ExceptionCode
    ExceptionCode = lpMemPTD->ExceptionCode;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return ExceptionCode;
} // End MyGetExceptionCode


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

    if (hGlbPTD EQ NULL)            // ***DEBUG***
        MB ("hGlbPTD EQ NULL");

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    if (lpMemPTD EQ NULL)           // ***DEBUG***
        MB ("lpMemPTD EQ NULL");

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

    if (hGlbPTD EQ NULL)            // ***DEBUG***
        MB ("hGlbPTD EQ NULL");

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    if (lpMemPTD EQ NULL)           // ***DEBUG***
        MB ("lpMemPTD EQ NULL");

    // Set the ExceptionAddr
    lpMemPTD->ExceptionAddr = (UINT) ExceptionAddr;

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

    if (hGlbPTD EQ NULL)            // ***DEBUG***
        MB ("hGlbPTD EQ NULL");

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    if (lpMemPTD EQ NULL)           // ***DEBUG***
        MB ("lpMemPTD EQ NULL");

    // Set the ExceptionText
    lpMemPTD->ExceptionText = ExceptionText;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End MySetExceptionText


//***************************************************************************
//  $CheckException
//
//  Check on a structured exception
//***************************************************************************

long CheckException
    (LPEXCEPTION_POINTERS lpExcept,
     LPCHAR               lpText)

{
    // Save the exception code & address for later use
    MySetExceptionCode (lpExcept->ExceptionRecord->ExceptionCode);
    MySetExceptionAddr (lpExcept->ExceptionRecord->ExceptionAddress);
    MySetExceptionText (lpText);

    DisplayException ();

    // Split cases based upon the exception code
    switch (MyGetExceptionCode ())
    {
////////case EXCEPTION_RESULT_BOOL:
////////case EXCEPTION_RESULT_INT:
        case EXCEPTION_RESULT_FLOAT:
        case EXCEPTION_DOMAIN_ERROR:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_ACCESS_VIOLATION:
        case EXCEPTION_SINGLE_STEP:
        case EXCEPTION_BREAKPOINT:
            return EXCEPTION_EXECUTE_HANDLER;

        case EXCEPTION_CTRL_BREAK:
            return EXCEPTION_CONTINUE_EXECUTION;

        default:
            return EXCEPTION_CONTINUE_SEARCH;
    } // End SWITCH
} // End CheckException


#ifdef START_ADDRESSES_ENABLE
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


#ifdef START_ADDRESSES_ENABLE
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
                 exceptAddr,
                 nearAddress,
                 nearIndex,
                 i;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPCHAR       exceptText;    // Ptr to exception text

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
     && (exceptAddr - StartAddresses[i].StartAddressAddr) < nearAddress)
    {
        nearAddress = exceptAddr - StartAddresses[i].StartAddressAddr;
        nearIndex   = i;
    } // End FOR/IF

    for (exceptIndex = 0; exceptIndex < EXCEPT_NAMES_LENGTH; exceptIndex++)
    if (exceptCode EQ ExceptNames[exceptIndex].ExceptCode)
        break;

    wsprintf (szTemp,
              "Exception code = %08X (%s) at %08X"
#ifdef START_ADDRESSES_ENABLE
              " (%s + %08X)"
#endif
              " from %s"
            ,
              exceptCode,
              (exceptIndex EQ EXCEPT_NAMES_LENGTH)
                ? "Exception Unknown"
                : ExceptNames[exceptIndex].ExceptName,
              exceptAddr
#ifdef START_ADDRESSES_ENABLE
            , StartAddresses[nearIndex].StartAddressName,
              nearAddress
#endif
            , exceptText
              );
    MB (szTemp);
} // End _DisplayException
#endif


//***************************************************************************
//  End of File: except.c
//***************************************************************************
