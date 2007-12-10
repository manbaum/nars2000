//***************************************************************************
//  NARS2000 -- Primitive Function -- Times
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
PRIMSPEC PrimSpecTimes =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecTimesStorageTypeMon,
    NULL,   // &PrimFnMonTimesAPA_EM, -- Can't happen w/Times

    &PrimFnMonTimesBisB,
    NULL,   // &PrimFnMonTimesBisI, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesBisF, -- Can't happen w/Times

////               IisB,     // Handled via type promotion (to IisI)
    &PrimFnMonTimesIisI,
    &PrimFnMonTimesIisF,

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonTimesFisI, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesFisF, -- Can't happen w/Times

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecTimesStorageTypeDyd,
    &PrimFnDydTimesAPA_EM,

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydTimesBisIvI, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisFvF, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisCvC, -- Can't happen w/Times

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydTimesIisIvI,
    NULL,   // &PrimFnDydTimesIisFvF, -- Can't happen w/Times

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydTimesFisIvI,
    &PrimFnDydTimesFisFvF,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecTimes};
#endif


//***************************************************************************
//  $PrimFnTimes_EM_YY
//
//  Primitive function for monadic and dyadic Times ("signum" and "multiplication")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnTimes_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnTimes_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_TIMES);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnTimes_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecTimesStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecTimesStorageTypeMon
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
    aplTypeRes = *lpaplTypeRht;

    // Except that FLOAT and APA become INT
    if (aplTypeRes EQ ARRAY_FLOAT
     || aplTypeRes EQ ARRAY_APA)
        return ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecTimesStorageTypeMon


//***************************************************************************
//  $PrimFnMonTimesBisB
//
//  Primitive scalar function monadic Times:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonTimesBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonTimesBisB


//***************************************************************************
//  $PrimFnMonTimesIisI
//
//  Primitive scalar function monadic Times:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonTimesIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (aplIntegerRht < 0)
        return -1;
    else
    if (aplIntegerRht EQ 0)
        return  0;
    else
        return  1;
} // End PrimFnMonTimesIisI


//***************************************************************************
//  $PrimFnMonTimesIisF
//
//  Primitive scalar function monadic Times:  I {is} fn F
//***************************************************************************

APLINT PrimFnMonTimesIisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (aplFloatRht < 0)
        return -1;
    else
    if (aplFloatRht EQ 0)
        return  0;
    else
        return  1;
} // End PrimFnMonTimesIisF


//***************************************************************************
//  $PrimSpecTimesStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecTimesStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (aplNELMLft EQ 0 && *lpaplTypeLft EQ ARRAY_CHAR)
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (aplNELMRht EQ 0 && *lpaplTypeRht EQ ARRAY_CHAR)
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // Special case multiplication with APA
    if (aplTypeRes EQ ARRAY_INT                            // Res = INT
     && (aplNELMLft NE 1 || aplNELMRht NE 1)                // Not both singletons
     && ((aplNELMLft EQ 1 && *lpaplTypeRht EQ ARRAY_APA)   // Non-singleton is APA
      || (aplNELMRht EQ 1 && *lpaplTypeLft EQ ARRAY_APA))) // ...
        aplTypeRes = ARRAY_APA;

    return aplTypeRes;
} // End PrimSpecTimesStorageTypeDyd


//***************************************************************************
//  $PrimFnDydTimesIisIvI
//
//  Primitive scalar function dyadic Times:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydTimesIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return imul64 (aplIntegerLft, aplIntegerRht, NULL);
} // End PrimFnDydTimesIisIvI


//***************************************************************************
//  $PrimFnDydTimesFisIvI
//
//  Primitive scalar function dyadic Times:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydTimesFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    BOOL   bRet = TRUE;
    APLINT aplRes;

    aplRes = imul64 (aplIntegerLft, aplIntegerRht, &bRet);
    if (bRet)
        return (APLFLOAT) aplRes;

    return ((APLFLOAT) aplIntegerLft) * (APLFLOAT) aplIntegerRht;
} // End PrimFnDydTimesFisIvI


//***************************************************************************
//  $PrimFnDydTimesFisFvF
//
//  Primitive scalar function dyadic Times:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydTimesFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow/underflow

    return (aplFloatLft * aplFloatRht);
} // End PrimFnDydTimesFisFvF


//***************************************************************************
//  $PrimFnDydTimesAPA_EM
//
//  Dyadic Times, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesAPA_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydTimesAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to result token (may be NULL)

     LPTOKEN      lptkFunc,         // Ptr to function token

     HGLOBAL      hGlbLft,          // HGLOBAL of left arg (may be NULL if simple)
     HGLOBAL      hGlbRht,          // ...        right ...
     HGLOBAL     *lphGlbRes,        // Ptr to result handle

     APLRANK      aplRankLft,       // Left arg rank
     APLRANK      aplRankRht,       // Right ...

     APLNELM      aplNELMLft,       // Left arg NELM
     APLNELM      aplNELMRht,       // Right ...

     APLINT       aplInteger,       // The integer from the simple side
     LPPRIMSPEC   lpPrimSpec)       // Ptr to local PRIMSPEC

{
    APLRANK aplRankRes;     // Result rank
    LPVOID  lpMemRes;       // Ptr to result global memory

    DBGENTER;

    //***************************************************************
    // The result is an APA, one of the args is a simple singleton,
    //   the other an APA
    //***************************************************************

    // Axis may be anything

    // Get a ptr to each arg's header
    //   and the value of the singleton
    // Also, in case we're doing (1 1 1{rho}1) fn {iota}4
    //   set the rank of the result to the rank of
    //   the APA.  The rank passed into this function
    //   is the larger of the left and right ranks,
    //   which for the above example, is wrong.

    // Split cases based upon the arg's NELM
    //   (one of the arg's must be a singleton)
    if (aplNELMLft NE 1)
    {
        *lphGlbRes = CopyArray_EM (hGlbLft, lptkFunc);
        aplRankRes = aplRankLft;
    } else
    if (aplNELMRht NE 1)
    {
        *lphGlbRes = CopyArray_EM (hGlbRht, lptkFunc);
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
    lpMemRes = MyGlobalLock (*lphGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)

    // Multiply the singleton's value to the result offset and multiplier
    lpAPA->Off *= aplInteger;
    lpAPA->Mul *= aplInteger;

#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    } // End IF

    DBGLEAVE;

    return TRUE;
} // End PrimFnDydTimesAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_times.c
//***************************************************************************
