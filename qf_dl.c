//***************************************************************************
//  NARS2000 -- System Function -- Quad DL
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


//***************************************************************************
//  $SysFnDL_EM
//
//  System function:  []DL -- Delay
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDL_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDL_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonDL_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydDL_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnDL_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonDL_EM
//
//  Monadic []DL -- Delay
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonDL_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonDL_EM
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankRht;        // Right arg rank
    APLFLOAT     aplFloatRht;       // Right arg float value
    DWORD        dwTickCount;       // The current tick count (time in millseconds since W was started)
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Check for RANK ERROR
    if (aplRankRht > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for LENGTH ERROR
    if (aplNELMRht NE 1)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for DOMAIN ERROR
    if (!IsSimpleNum (aplTypeRht))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Get the one (and only) value from the right arg
    FirstValue (lptkRhtArg,             // Ptr to right arg token
                NULL,                   // Ptr to integer result
               &aplFloatRht,            // Ptr to float ...
                NULL,                   // Ptr to WCHAR ...
                NULL,                   // Ptr to longest ...
                NULL,                   // Ptr to lpSym/Glb ...
                NULL,                   // Ptr to ...immediate type ...
                NULL);                  // Ptr to array type ...
    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Create a semaphore so we can be interrupted
    lpMemPTD->hSemaDelay =
    CreateSemaphore (NULL,              // No security attrs
                     0,                 // Initial count (non-signalled)
                     64*1024,           // Maximum count
                     NULL);             // No name
    // Get the current tick count so we can subtract it from the next tick count
    dwTickCount = GetTickCount ();

    // Scale the value from float seconds to DWORD milliseconds
    //   and wait for that long
    WaitForSingleObject (lpMemPTD->hSemaDelay,
                 (DWORD) (aplFloatRht * 1000));
    // Determine how long we waited
    dwTickCount = GetTickCount () - dwTickCount;

    // Close the semaphore handle as it is no longer needed
    CloseHandle (lpMemPTD->hSemaDelay); lpMemPTD->hSemaDelay = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Convert from DWORD milliseconds to float seconds
    aplFloatRht = ((APLFLOAT) dwTickCount) / 1000;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkFloat    = aplFloatRht;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnMonDL_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydDL_EM
//
//  Dyadic []DL -- ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydDL_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydDL_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End SysFnDydDL_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_dl.c
//***************************************************************************
