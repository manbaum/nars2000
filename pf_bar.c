//***************************************************************************
//  NARS2000 -- Primitive Function -- Bar
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
PRIMSPEC PrimSpecBar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecBarStorageTypeMon,
    &PrimFnMonBarAPA_EM,

    NULL,   // &PrimFnMonBarBisB, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarBisI, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarBisF, -- Can't happen w/Bar

////               IisB,     // Handled via type promotion (to IisI)
    &PrimFnMonBarIisI,
    NULL,   // &PrimFnMonBarIisF, -- Can't happen w/Bar

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonBarFisI, -- Can't happen w/Bar
    &PrimFnMonBarFisF,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecBarStorageTypeDyd,
    &PrimFnDydBarAPA_EM,

    NULL,   // &PrimFnDydBarBisBvB, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisIvI, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisFvF, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisCvC, -- Can't happen w/Bar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydBarIisIvI,
    NULL,   // &PrimFnDydBarIisFvF, -- Can't happen w/Bar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydBarFisIvI,
    &PrimFnDydBarFisFvF,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecBar};
#endif


//***************************************************************************
//  $PrimFnBar_EM_YY
//
//  Primitive function for monadic and dyadic Bar ("negation" and "subtraction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnBar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnBar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_BAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnBar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecBarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecBarStorageTypeMon
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

    // No Boolean result for negation
    if (IsSimpleBool (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecBarStorageTypeMon


//***************************************************************************
//  $PrimFnMonBarIisI
//
//  Primitive scalar function monadic Bar:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonBarIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return -aplIntegerRht;
} // End PrimFnMonBarIisI


//***************************************************************************
//  $PrimFnMonBarFisF
//
//  Primitive scalar function monadic Bar:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonBarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return -aplFloatRht;
} // End PrimFnMonBarFisF


//***************************************************************************
//  $PrimFnMonBarAPA_EM
//
//  Monadic Bar, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarAPA_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonBarAPA_EM
    (LPPL_YYSTYPE  lpYYRes,         // Ptr to the result (may be NULL)
     LPTOKEN       lptkFunc,        // Ptr to function token
     HGLOBAL       hGlbRht,         // Handle to right arg
     HGLOBAL      *lphGlbRes,       // Ptr to handle to result
     APLRANK       aplRankRht,      // Rank of the right arg
     LPPRIMSPEC    lpPrimSpec)      // Ptr to local PRIMSPEC

{
    LPVOID  lpMemRes;
    APLRANK aplRankRes;

    DBGENTER;

    // Axis may be anything

    *lphGlbRes = CopyArray_EM (hGlbRht, lptkFunc);
    if (!*lphGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (*lphGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    aplRankRes = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);

    // Negating the offset and multiplier negates the APA
#define lpAPA       ((LPAPLAPA) lpMemRes)
    lpAPA->Off = -lpAPA->Off;
    lpAPA->Mul = -lpAPA->Mul;
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

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    DBGLEAVE;

    return FALSE;
} // End PrimFnMonBarAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecBarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecBarStorageTypeDyd
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

    // No Boolean results for subtraction
    if (IsSimpleBool (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    // Special case subtraction with APA
    if (aplTypeRes EQ ARRAY_INT                            // Res = INT
     && (aplNELMLft NE 1 || aplNELMRht NE 1)                // Not both singletons
     && ((aplNELMLft EQ 1 && IsSimpleAPA (*lpaplTypeRht))   // Non-singleton is APA
      || (aplNELMRht EQ 1 && IsSimpleAPA (*lpaplTypeLft)))) // ...
        aplTypeRes = ARRAY_APA;

    return aplTypeRes;
} // End PrimSpecBarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydBarIisIvI
//
//  Primitive scalar function dyadic Bar:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydBarIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplIntegerLft - aplIntegerRht;
} // End PrimFnDydBarIisIvI


//***************************************************************************
//  $PrimFnDydBarFisIvI
//
//  Primitive scalar function dyadic Bar:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydBarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (APLFLOAT) (aplIntegerLft - aplIntegerRht);
} // End PrimFnDydBarFisIvI


//***************************************************************************
//  $PrimFnDydBarFisFvF
//
//  Primitive scalar function dyadic Bar:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydBarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplFloatLft - aplFloatRht);
} // End PrimFnDydBarFisFvF


//***************************************************************************
//  $PrimFnDydBarAPA_EM
//
//  Dyadic Bar, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydBarAPA_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydBarAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to the result (may be NULL)

     LPTOKEN      lptkFunc,         // Ptr to function token

     HGLOBAL      hGlbLft,          // HGLOBAL of left arg (may be NULL if simple)
     HGLOBAL      hGlbRht,          // ...        right ...
     HGLOBAL     *lphGlbRes,        // Ptr to HGLOBAL of the result

     APLRANK      aplRankLft,       // Rank of the left arg
     APLRANK      aplRankRht,       // ...         right ...

     APLNELM      aplNELMLft,       // Only one of these NELMs is 1
     APLNELM      aplNELMRht,       // ...

     APLINT       aplInteger,       // The integer from the simple side
     LPPRIMSPEC   lpPrimSpec)       // Ptr to PRIMSPEC

{
    APLRANK aplRankRes;         // Result rank
    LPVOID  lpMemRes;           // Ptr to result global memory

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

    if (aplNELMLft NE 1)
        lpAPA->Off -= aplInteger;
    else
    {
        lpAPA->Off = aplInteger - lpAPA->Off;
        lpAPA->Mul = -lpAPA->Mul;
    } // End IF

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

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    DBGLEAVE;

    return FALSE;
} // End PrimFnDydBarAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_bar.c
//***************************************************************************
