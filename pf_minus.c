//***************************************************************************
//  NARS2000 -- Primitive Function -- Minus
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "primspec.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifndef PROTO
PRIMSPEC PrimSpecMinus =
{
    // Monadic functions
    &PrimFnMon_EM,
    &PrimSpecMinusStorageTypeMon,
    &PrimFnMonMinusAPA_EM,

    NULL,   // &PrimFnMonMinusBisB, -- Can't happen w/Minus
    NULL,   // &PrimFnMonMinusBisI, -- Can't happen w/Minus
    NULL,   // &PrimFnMonMinusBisF, -- Can't happen w/Minus

////               IisB,     // Handled via type promotion (to IisI)
    &PrimFnMonMinusIisI,
    NULL,   // &PrimFnMonMinusIisF, -- Can't happen w/Minus

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonMinusFisI, -- Can't happen w/Minus
    &PrimFnMonMinusFisF,

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecMinusStorageTypeDyd,
    &PrimFnDydMinusAPA_EM,

    NULL,   // &PrimFnDydMinusBisBvB, -- Can't happen w/Minus
    NULL,   // &PrimFnDydMinusBisIvI, -- Can't happen w/Minus
    NULL,   // &PrimFnDydMinusBisFvF, -- Can't happen w/Minus
    NULL,   // &PrimFnDydMinusBisCvC, -- Can't happen w/Minus

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydMinusIisIvI,
    NULL,   // &PrimFnDydMinusIisFvF, -- Can't happen w/Minus

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydMinusFisIvI,
    &PrimFnDydMinusFisFvF,

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecMinus};
#endif


//***************************************************************************
//  PrimFnMinus_EM
//
//  Primitive function for monadic and dyadic Minus (negation and subtraction)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMinus_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMinus_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ '-');

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnMinus_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecMinusStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecMinusStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpcArrTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE cArrTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (aplNELMRht EQ 0 && *lpcArrTypeRht EQ ARRAY_CHAR)
        *lpcArrTypeRht = ARRAY_BOOL;

    if (*lpcArrTypeRht EQ ARRAY_CHAR
     || *lpcArrTypeRht EQ ARRAY_LIST)
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    cArrTypeRes = *lpcArrTypeRht;

    // No Boolean result for negation
    if (cArrTypeRes EQ ARRAY_BOOL)
        cArrTypeRes = ARRAY_INT;

    return cArrTypeRes;
} // End PrimSpecMinusStorageTypeMon


//***************************************************************************
//  PrimFnMonMinusIisI
//
//  Primitive scalar function monadic Minus:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonMinusIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return -aplIntegerRht;
} // End PrimFnMonMinusIisI


//***************************************************************************
//  PrimFnMonMinusFisF
//
//  Primitive scalar function monadic Minus:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonMinusFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return -aplFloatRht;
} // End PrimFnMonMinusFisF


//***************************************************************************
//  PrimFnMonMinusAPA_EM
//
//  Monadic Minus, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonMinusAPA_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonMinusAPA_EM
    (LPYYSTYPE  lpYYRes,    // The result token (may be NULL)
     LPTOKEN    lptkFunc,
     HGLOBAL    hGlbRht,
     HGLOBAL   *lphGlbRes,
     LPVOID    *lplpMemRes,
     APLRANK    aplRankRht,
     LPPRIMSPEC lpPrimSpec)

{
    LPVOID  lpMemRes;
    APLRANK aplRankRes;

    DBGENTER;

    // Axis may be anything

    *lphGlbRes = CopyArray_EM (hGlbRht, TRUE, lptkFunc);

    if (!*lphGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return FALSE;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    aplRankRes = lpHeader->Rank;

#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

#define lpHeader    ((LPAPLAPA) lpMemRes)

    // Negating the offset and multiplier negates the APA
    lpHeader->Off = -lpHeader->Off;
    lpHeader->Mul = -lpHeader->Mul;

#undef  lpHeader

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////    lpYYRes->tkToken.tkFlags.ImmType   = 0;
////    lpYYRes->tkToken.tkFlags.NoDisplay = 0;
////    lpYYRes->tkToken.tkFlags.Color     =
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (*lphGlbRes);
    } // End IF

    DBGEXIT;

    return TRUE;
} // End PrimFnMonMinusAPA_EM
#undef APPEND_NAME


//***************************************************************************
//  PrimSpecMinusStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecMinusStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpcArrTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpcArrTypeRht)

{
    APLSTYPE cArrTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (aplNELMLft EQ 0 && *lpcArrTypeLft EQ ARRAY_CHAR)
        *lpcArrTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (aplNELMRht EQ 0 && *lpcArrTypeRht EQ ARRAY_CHAR)
        *lpcArrTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    cArrTypeRes = StorageType (*lpcArrTypeLft, lptkFunc, *lpcArrTypeRht);

    // No Boolean results for subtraction
    if (cArrTypeRes EQ ARRAY_BOOL)
        cArrTypeRes = ARRAY_INT;

    // Special case subtraction with APA
    if (cArrTypeRes EQ ARRAY_INT                            // Res = INT
     && (aplNELMLft NE 1 || aplNELMRht NE 1)                // Not both singletons
     && ((aplNELMLft EQ 1 && *lpcArrTypeRht EQ ARRAY_APA)   // Non-singleton is APA
      || (aplNELMRht EQ 1 && *lpcArrTypeLft EQ ARRAY_APA))) // ...
        cArrTypeRes = ARRAY_APA;

    return cArrTypeRes;
} // End PrimSpecMinusStorageTypeDyd


//***************************************************************************
//  PrimFnDydMinusIisIvI
//
//  Primitive scalar function dyadic Minus:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydMinusIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplIntegerLft - aplIntegerRht;
} // End PrimFnDydMinusIisIvI


//***************************************************************************
//  PrimFnDydMinusFisIvI
//
//  Primitive scalar function dyadic Minus:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydMinusFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (APLFLOAT) (aplIntegerLft - aplIntegerRht);
} // End PrimFnDydMinusFisIvI


//***************************************************************************
//  PrimFnDydMinusFisFvF
//
//  Primitive scalar function dyadic Minus:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydMinusFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplFloatLft - aplFloatRht);
} // End PrimFnDydMinusFisFvF


//***************************************************************************
//  PrimFnDydMinusAPA_EM
//
//  Dyadic Minus, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydMinusAPA_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydMinusAPA_EM
    (LPYYSTYPE  lpYYRes,    // The result token (may be NULL)

     LPTOKEN    lptkFunc,

     HGLOBAL    hGlbLft,    // HGLOBAL of left arg (may be NULL if simple)
     HGLOBAL    hGlbRht,    // ...        right ...
     HGLOBAL   *lphGlbRes,

     LPVOID    *lplpMemRes,

     APLRANK    aplRankLft,
     APLRANK    aplRankRht,

     APLNELM    aplNELMLft, // Only one of these NELMs is 1
     APLNELM    aplNELMRht, // ...

     APLINT     aplInteger, // The integer from the simple side
     LPPRIMSPEC lpPrimSpec)

{
    APLRANK aplRankRes;

    DBGENTER;

    //***************************************************************
    // The result is an APA, one of the args is a simple singleton,
    //   the other an APA
    //***************************************************************

    // Axis may be anything

    // Get a ptr to each arg's header
    //   and the value of the singleton
    // Also, in case we're doing (1 1 1{rho}1)+{iota}4
    //   set the rank of the result to the rank of
    //   the APA.  The rank passed into this function
    //   is the larger of the left and right ranks,
    //   which for the above example, is wrong.

    // Split cases based upon the arg's NELM
    //   (one of the arg's must be a singleton)
    if (aplNELMLft NE 1)
    {
        *lphGlbRes = CopyArray_EM (hGlbLft, TRUE, lptkFunc);
        aplRankRes = aplRankLft;
    } else
    if (aplNELMRht NE 1)
    {
        *lphGlbRes = CopyArray_EM (hGlbRht, TRUE, lptkFunc);
        aplRankRes = aplRankRht;
    } else
        DbgStop ();     // We should never get here

    if (!*lphGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return FALSE;
    } // End IF

    // Lock the memory to get a ptr to it
    *lplpMemRes = MyGlobalLock (*lphGlbRes);

    // Skip over the header and dimensions to the data
    *lplpMemRes = VarArrayBaseToData (*lplpMemRes, aplRankRes);

#define lpAPA       ((LPAPLAPA) *lplpMemRes)

    if (aplNELMLft NE 1)
        lpAPA->Off -= aplInteger;
    else
    {
        lpAPA->Off = aplInteger - lpAPA->Off;
        lpAPA->Mul = -lpAPA->Mul;
    } // End IF

#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); *lplpMemRes = NULL;

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////    lpYYRes->tkToken.tkFlags.ImmType   = 0;
////    lpYYRes->tkToken.tkFlags.NoDisplay = 0;
////    lpYYRes->tkToken.tkFlags.Color     =
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (*lphGlbRes);
    } // End IF

    DBGEXIT;

    return TRUE;
} // End PrimFnDydMinusAPA_EM
#undef APPEND_NAME


//***************************************************************************
//  End of File: pf_minus.c
//***************************************************************************
