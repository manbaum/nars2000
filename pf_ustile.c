//***************************************************************************
//  NARS2000 -- Primitive Function -- UpStile
//***************************************************************************

#define STRICT
#include <windows.h>
#include <float.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifndef PROTO
PRIMSPEC PrimSpecUpStile = {
    // Monadic functions
    &PrimFnMon_EM_YY,
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
    &PrimFnDyd_EM_YY,
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
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpStile};
#endif


//***************************************************************************
//  $PrimFnUpStile_EM_YY
//
//  Primitive function for monadic and dyadic UpStile ("ceiling" and "maximum")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnUpStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPSTILE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpStile_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpStileStorageTypeMon
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
    //   except FLOAT goes to INT
    // IisF promotes to FisF as necessary.
    if (*lpaplTypeRht EQ ARRAY_FLOAT)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = *lpaplTypeRht;

    return aplTypeRes;
} // End PrimSpecUpStileStorageTypeMon


//***************************************************************************
//  $PrimFnMonUpStileBisB
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
//  $PrimFnMonUpStileIisI
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
//  $PrimFnMonUpStileIisF
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
//  $PrimFnMonUpStileFisF
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
//  $PrimFnMonUpStileAPA_EM
//
//  Monadic UpStile, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpStileAPA_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonUpStileAPA_EM
    (LPYYSTYPE  lpYYRes,            // Ptr to result token (may be NULL)

     LPTOKEN    lptkFunc,           // Ptr to function token

     HGLOBAL    hGlbRht,            // Right arg handle
     HGLOBAL   *lphGlbRes,          // Ptr to result handle

     LPVOID    *lplpMemRes,         // Ptr to ptr to result memory

     APLRANK    aplRankRht,         // Right arg rank

     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    DBGENTER;

    // Axis may be anything

    // Copy the HGLOBAL to the result
    *lphGlbRes = CopySymGlbDirGlb (hGlbRht);

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
} // End PrimFnMonUpStileAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpStileStorageTypeDyd
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

    return aplTypeRes;
} // End PrimSpecUpStileStorageTypeDyd


//***************************************************************************
//  $PrimFnDydUpStileIisIvI
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
//  $PrimFnDydUpStileFisFvF
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
