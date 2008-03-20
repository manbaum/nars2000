//***************************************************************************
//  NARS2000 -- Primitive Function -- Plus
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
PRIMSPEC PrimSpecPlus =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecPlusStorageTypeMon,
    &PrimFnMonPlusAPA_EM,

    &PrimFnMonPlusBisB,
    NULL,   // &PrimFnMonPlusBisI, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusBisF, -- Can't happen w/Plus

////              IisB,     // Handled via type promotion (to IisI)
    &PrimFnMonPlusIisI,
    NULL,   // &PrimFnMonPlusIisF, -- Can't happen w/Plus

////              FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonPlusFisI, -- Can't happen w/Plus
    &PrimFnMonPlusFisF,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecPlusStorageTypeDyd,
    &PrimFnDydPlusAPA_EM,

    NULL,   // &PrimFnDydPlusBisBvB, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisIvI, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisFvF, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisCvC, -- Can't happen w/Plus

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydPlusIisIvI,
    NULL,   // &PrimFnDydPlusIisFvF, -- Can't happen w/Plus

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydPlusFisIvI,
    &PrimFnDydPlusFisFvF,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecPlus};
#endif


//***************************************************************************
//  $PrimFnPlus_EM_YY
//
//  Primitive function for monadic and dyadic Plus ("conjugate" and "addition")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnPlus_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnPlus_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_PLUS);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnPlus_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecPlusStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecPlusStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE aplTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (aplNELMRht EQ 0 && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    if (IsSimpleChar (*lpaplTypeRht)
     || *lpaplTypeRht EQ ARRAY_LIST)
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    aplTypeRes = *lpaplTypeRht;

    return aplTypeRes;
} // End PrimSpecPlusStorageTypeMon


//***************************************************************************
//  $PrimFnMonPlusBisB
//
//  Primitive scalar function monadic Plus:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonPlusBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonPlusBisB


//***************************************************************************
//  $PrimFnMonPlusIisI
//
//  Primitive scalar function monadic Plus:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonPlusIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplIntegerRht;
} // End PrimFnMonPlusIisI


//***************************************************************************
//  $PrimFnMonPlusFisF
//
//  Primitive scalar function monadic Plus:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonPlusFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplFloatRht;
} // End PrimFnMonPlusFisF


//***************************************************************************
//  $PrimFnMonPlusAPA_EM
//
//  Monadic plus, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonPlusAPA_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonPlusAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // The result token (may be NULL)
     LPTOKEN      lptkFunc,         // Ptr to function token
     HGLOBAL      hGlbRht,          // Right arg handle
     HGLOBAL     *lphGlbRes,        // Ptr to result handle
     APLRANK      aplRankRht,       // Right arg rank
     LPPRIMSPEC   lpPrimSpec)       // Ptr to local PRIMSPEC

{
    DBGENTER;

    // Axis may be anything

    // Copy the HGLOBAL to the result
    *lphGlbRes = CopySymGlbDirAsGlb (hGlbRht);

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = hGlbRht;
    } // End IF

    DBGLEAVE;

    return TRUE;
} // End PrimFnMonPlusAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecPlusStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecPlusStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (aplNELMLft EQ 0 && IsSimpleChar (*lpaplTypeLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (aplNELMRht EQ 0 && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // No Boolean results for addition
    if (IsSimpleBool (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    // Special case addition with APA
    if (aplTypeRes EQ ARRAY_INT                             // Res = INT
     && (aplNELMLft NE 1 || aplNELMRht NE 1)                // Not both singletons
     && ((aplNELMLft EQ 1 && IsSimpleAPA (*lpaplTypeRht))   // Non-singleton is APA
      || (aplNELMRht EQ 1 && IsSimpleAPA (*lpaplTypeLft)))) // ...
        aplTypeRes = ARRAY_APA;

    return aplTypeRes;
} // End PrimSpecPlusStorageTypeDyd


//***************************************************************************
//  $PrimFnDydPlusIisIvI
//
//  Primitive scalar function dyadic Plus:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydPlusIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow

    return aplIntegerLft + aplIntegerRht;
} // End PrimFnDydPlusIisIvI


//***************************************************************************
//  $PrimFnDydPlusFisIvI
//
//  Primitive scalar function dyadic Plus:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydPlusFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow

    return (APLFLOAT) (aplIntegerLft + aplIntegerRht);
} // End PrimFnDydPlusFisIvI


//***************************************************************************
//  $PrimFnDydPlusFisFvF
//
//  Primitive scalar function dyadic Plus:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydPlusFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow

    return aplFloatLft + aplFloatRht;
} // End PrimFnDydPlusFisFvF


//***************************************************************************
//  $PrimFnDydPlusAPA_EM
//
//  Dyadic plus, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydPlusAPA_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydPlusAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // The result token (may be NULL)

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
    APLRANK aplRankRes;         // Result rank
    LPVOID  lpMemRes;           // Ptr to result global memory
    BOOL    bRet = FALSE;       // TRUE iff the result is valid

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
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)

    // Add in the singleton's value to the result offset
    lpAPA->Off += aplInteger;

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

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
NORMAL_EXIT:
    DBGLEAVE;

    return bRet;
} // End PrimFnDydPlusAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_plus.c
//***************************************************************************
