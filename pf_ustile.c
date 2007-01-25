//***************************************************************************
//  NARS2000 -- Primitive Function -- UpStile
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <float.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "Unicode.h"
#include "externs.h"
#include "primspec.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifndef PROTO
PRIMSPEC PrimSpecUpStile = {
    // Monadic functions
    &PrimFnMon_EM,
    &PrimSpecUpStileStorageTypeMon,
    &PrimFnMonUpStileAPA_EM,

    &PrimFnMonUpStileBisB,
    NULL,   // &PrimFnMonUpStileBisI, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileBisF, -- Can't happen w/UpStile

////                 IisB,   // Handled via type promotion (to IisI)
    &PrimFnMonUpStileIisI,
    &PrimFnMonUpStileIisF,

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonUpStileFisI, -- Can't happen w/UpStile
    &PrimFnMonUpStileFisF,

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecUpStileStorageTypeDyd,
    NULL,   // &PrimFnDydUpStileAPA_EM, -- Can't happen w/UpStile

    &PrimFnDydDownCaretBisBvB,
    NULL,   // &PrimFnDydUpStileBisIvI, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisFvF, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisCvC, -- Can't happen w/UpStile

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydUpStileIisIvI,
    NULL,   // &PrimFnDydUpStileIisFvF, -- Can't happen w/UpStile

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydUpStileFisIvI, -- Can't happen w/UpStile
    &PrimFnDydUpStileFisFvF,

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpStile};
#endif


//***************************************************************************
//  PrimFnUpStile_EM
//
//  Primitive function for monadic and dyadic UpStile ("ceiling" and "maximum")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpStile_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnUpStile_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_UPSTILE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpStile_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecUpStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpStileStorageTypeMon
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
    //   except FLOAT goes to INT
    // IisF promotes to FisF as necessary.
    if (*lpcArrTypeRht EQ ARRAY_FLOAT)
        cArrTypeRes = ARRAY_INT;
    else
        cArrTypeRes = *lpcArrTypeRht;

    return cArrTypeRes;
} // End PrimSpecUpStileStorageTypeMon


//***************************************************************************
//  PrimFnMonUpStileBisB
//
//  Primitive scalar function monadic UpStile:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonUpStileBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonUpStileBisB


//***************************************************************************
//  PrimFnMonUpStileIisI
//
//  Primitive scalar function monadic UpStile:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonUpStileIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplIntegerRht;
} // End PrimFnMonUpStileIisI


//***************************************************************************
//  PrimFnMonUpStileIisF
//
//  Primitive scalar function monadic UpStile:  I {is} fn F
//***************************************************************************

APLINT PrimFnMonUpStileIisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for ± infinity and numbers whose
    //   absolute value is >= 2*53
    if (!_finite (aplFloatRht)
     || fabs (aplFloatRht) >= Float2Pow53)
        RaiseException (EXEC_RESULT_FLOAT, 0, 0, NULL);

    return (APLINT) -PrimFnMonDownStileFisF (-aplFloatRht, lpPrimSpec);
} // End PrimFnMonUpStileIisF


//***************************************************************************
//  PrimFnMonUpStileFisF
//
//  Primitive scalar function monadic UpStile:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonUpStileFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return -PrimFnMonDownStileFisF (-aplFloatRht, lpPrimSpec);
} // End PrimFnMonUpStileFisF


//***************************************************************************
//  PrimFnMonUpStileAPA_EM
//
//  Monadic UpStile, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpStileAPA_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonUpStileAPA_EM
    (LPYYSTYPE  lpYYRes,    // The result token (may be NULL)
     LPTOKEN    lptkFunc,
     HGLOBAL    hGlbRht,
     HGLOBAL   *lphGlbRes,
     LPVOID    *lplpMemRes,
     APLRANK    aplRankRht,
     LPPRIMSPEC lpPrimSpec)

{
    DBGENTER;

    // Axis may be anything

    *lphGlbRes = CopyArray_EM (hGlbRht, FALSE, lptkFunc);

    if (!*lphGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return FALSE;
    } // End IF

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
} // End PrimFnMonUpStileAPA_EM
#undef APPEND_NAME


//***************************************************************************
//  PrimSpecUpStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpStileStorageTypeDyd
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

    return cArrTypeRes;
} // End PrimSpecUpStileStorageTypeDyd


//***************************************************************************
//  PrimFnDydUpStileIisIvI
//
//  Primitive scalar function dyadic UpStile:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydUpStileIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return max (aplIntegerLft, aplIntegerRht);
} // End PrimFnDydUpStileIisIvI


//***************************************************************************
//  PrimFnDydUpStileFisFvF
//
//  Primitive scalar function dyadic UpStile:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydUpStileFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return max (aplFloatLft, aplFloatRht);
} // End PrimFnDydUpStileFisFvF


//***************************************************************************
//  End of File: pf_ustile.c
//***************************************************************************
