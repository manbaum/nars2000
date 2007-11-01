//***************************************************************************
//  NARS2000 -- System Function -- Quad ES
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnES_EM_YY
//
//  System function:  []ES -- Event Simulate
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnES_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnES_EM_YY
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
        return SysFnMonERROR_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydERROR_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnES_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonES_EM_YY
//
//  Monadic []ES -- Event Simulate w/ Message or Type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonES_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonES_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankRht;        // Right arg Rank
    APLLONGEST   aplLongestRht;     // Right arg longest if immediate
    HGLOBAL      hGlbRht = NULL;    // Right arg global memory handle
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to result

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (aplRankRht > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleNH (aplTypeRht))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // If the right arg is empty, return NoValue
    if (aplNELMRht EQ 0)
        lpYYRes = MakeNoValue_YY (lptkFunc);
    else
    {
        // Check for RIGHT LENGTH ERROR
        if (IsSimpleNum (aplTypeRht)
         && aplNELMRht NE 2)
        {
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Get the thread's PerTabData global memory handle
        hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Get right arg's global ptrs
        aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

        // Split cases based upon the numeric vs. char storage type
        if (aplTypeRht EQ ARRAY_CHAR)
        {
            // If the message is a global, ...
            if (hGlbRht)
            {
                // Skip over the header and dimensions to the data
                lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

                // Copy the error message to temporary storage
                CopyMemory (lpMemPTD->lpwszQuadErrorMsg, lpMemRht, (UINT) aplNELMRht * sizeof (APLCHAR));
            } else
                lpMemPTD->lpwszQuadErrorMsg[0] = (APLCHAR) aplLongestRht;

            // Ensure properly terminated
            lpMemPTD->lpwszQuadErrorMsg[aplNELMRht] = L'\0';

            // Save in PTD -- note that the tkCharIndex in the
            //   function token passed here isn't used unless this is
            //   immediate execution mode; normally, the tkCharIndex of the
            //   caller's is used.
            ErrorMessageIndirectToken (lpMemPTD->lpwszQuadErrorMsg, lptkFunc);
            lpMemPTD->tkErrorCharIndex = lptkFunc->tkCharIndex;
        } else
        {
            // The right arg is simple numeric







        } // End IF/ELSE

        // Set the reset flag
        lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_QUADERROR_INIT;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF/ELSE
ERROR_EXIT:
    // We no longer need this ptr
    if (hGlbRht && lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnMonES_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydES_EM_YY
//
//  Dyadic []ES -- Event Simulate w/ Message and Type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydES_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydES_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc);
} // End SysFnDydES_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_es.c
//***************************************************************************
