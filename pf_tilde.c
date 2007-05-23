//***************************************************************************
//  NARS2000 -- Primitive Function -- Tilde
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifndef PROTO
PRIMSPEC PrimSpecTilde =
{
    // Monadic functions
    &PrimFnMon_EM,
    &PrimSpecTildeStorageTypeMon,
    NULL,   // &PrimFnMonTildeAPA_EM, -- Can't happen w/Tilde

    &PrimFnMonTildeBisB,
    &PrimFnMonTildeBisI,
    &PrimFnMonTildeBisF,

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonTildeIisI, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeIisF, -- Can't happen w/Tilde

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonTildeFisI, -- Can't happen w/Tilde
    NULL,   // &PrimFnMonTildeFisF, -- can't happen w/Tilde

    // Dyadic functions
    NULL,   // &PrimFnDyd_EM, -- Can't happen w/Tilde
    NULL,   // &PrimSpecTildeStorageTypeDyd, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeAPA_EM, -- Can't happen w/Tilde

    NULL,   // &PrimFnDydTildeBisBvB, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisIvI, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisFvF, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeBisCvC, -- Can't happen w/Tilde

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydTildeIisIvI,
    NULL,   // &PrimFnDydTildeIisFvF, -- Can't happen w/Tilde

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydTildeFisIvI, -- Can't happen w/Tilde
    NULL,   // &PrimFnDydTildeFisFvF, -- Can't happen w/Tilde

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecTilde};
#endif


//***************************************************************************
//  PrimFnTilde_EM
//
//  Primitive function for monadic and dyadic Tilde ("not" and "without")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnTilde_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnTilde_EM
    (LPTOKEN lptkLftArg,             // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_TILDE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return PrimFnDydTilde_EM           (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnTilde_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimProtoFnTilde_EM
//
//  Generate a prototype for the primitive functions monadic & dyadic Tilde
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnTilde_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoFnTilde_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM (&PrimFnTilde_EM,    // Ptr to primitive function routine
                                 lptkLftArg,        // Ptr to left arg token
                                 lptkFunc,          // Ptr to function token
                                 lptkRhtArg,        // Ptr to right arg token
                                 lptkAxis);         // Ptr to axis token (may be NULL)
} // End PrimProtoFnTilde_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecTildeStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecTildeStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE aplTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (aplNELMRht EQ 0 && *lpaplTypeRht EQ ARRAY_CHAR)
        *lpaplTypeRht = ARRAY_BOOL;

    if (*lpaplTypeRht EQ ARRAY_CHAR
     || *lpaplTypeRht EQ ARRAY_LIST)
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    //   except SimpleNum goes to BOOL
    if (IsSimpleNum (*lpaplTypeRht))
        aplTypeRes = ARRAY_BOOL;
    else
        aplTypeRes = *lpaplTypeRht;

    return aplTypeRes;
} // End PrimSpecTildeStorageTypeMon


//***************************************************************************
//  PrimFnMonTildeBisB
//
//  Primitive scalar function monadic Tilde:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonTildeBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return !aplBooleanRht;
} // End PrimFnMonTildeBisB


//***************************************************************************
//  PrimFnMonTildeBisI
//
//  Primitive scalar function monadic Tilde:  B {is} fn I
//***************************************************************************

APLBOOL PrimFnMonTildeBisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (!IsBooleanValue (aplIntegerRht))
        RaiseException (EXEC_DOMAIN_ERROR, 0, 0, NULL);
    return !(APLBOOL) aplIntegerRht;
} // End PrimFnMonTildeBisI


//***************************************************************************
//  PrimFnMonTildeBisF
//
//  Primitive scalar function monadic Tilde:  B {is} fn F
//***************************************************************************

APLBOOL PrimFnMonTildeBisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (!IsBooleanValue (aplFloatRht))
        RaiseException (EXEC_DOMAIN_ERROR, 0, 0, NULL);
    return !(APLBOOL) aplFloatRht;
} // End PrimFnMonTildeBisF


//***************************************************************************
//  PrimFnDydTilde_EM
//
//  Primitive function for dyadic Tilde ("without")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTilde_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydTilde_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE aplTypeLft,
             aplTypeRht;
    APLNELM  aplNELMLft,
             aplNELMRht;
    APLRANK  aplRankLft,
             aplRankRht;
    HGLOBAL  hGlbLft,
             hGlbRht;
    LPVOID   lpMemLft,
             lpMemRht;
    BOOL     bRet = TRUE;
    LPYYSTYPE lpYYRes;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        YYFree (lpYYRes); lpYYRes = NULL; return NULL;
    } // End IF

    DbgBrk ();          // ***FINISHME***

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK ERROR
    if (aplRankLft > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkLftArg);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF















ERROR_EXIT:
    if (hGlbLft && lpMemLft)
    {
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (bRet)
        return lpYYRes;
    else
    {
        YYFree (lpYYRes); lpYYRes = NULL; return NULL;
    } // End IF/ELSE
} // End PrimFnDydTilde_EM


//***************************************************************************
//  End of File: pf_tilde.c
//***************************************************************************
