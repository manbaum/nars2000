//***************************************************************************
//  NARS2000 -- Primitive Function -- CircleStar
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <math.h>

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
PRIMSPEC PrimSpecCircleStar =
{
    // Monadic functions
    &PrimFnMon_EM,
    &PrimSpecCircleStarStorageTypeMon,
    NULL,   // &PrimFnMonCircleStarAPA_EM, -- Can't happen w/CircleStar

    NULL,   // &PrimFnMonCircleStarBisB, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarBisI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarBisF, -- Can't happen w/CircleStar

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonCircleStarIisI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarIisF, -- Can't happen w/CircleStar

////               FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonCircleStarFisI,
    &PrimFnMonCircleStarFisF,

    // Dyadic functions
    &PrimFnDyd_EM,
    NULL,   // &PrimSpecCircleStarStorageTypeDyd, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarAPA_EM, -- Can't happen w/CircleStar

    &PrimFnDydRightCaretUnderbarBisBvB,
    NULL,   // &PrimFnDydCircleStarBisIvI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisFvF, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisCvC, -- Can't happen w/CircleStar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydCircleStarIisIvI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarIisFvF, -- Can't happen w/CircleStar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydCircleStarFisIvI,
    &PrimFnDydCircleStarFisFvF,

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecCircleStar};
#endif


//***************************************************************************
//  PrimFnCircleStar_EM
//
//  Primitive function for monadic and dyadic CircleStar ("natural logarithm" and "logarithm")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnCircleStar_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnCircleStar_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnCircleStar_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecCircleStarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStarStorageTypeMon
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

    // Except that BOOL, INT and APA become FLOAT
    if (IsSimpleInt (aplTypeRes))
        aplTypeRes = ARRAY_FLOAT;

    return aplTypeRes;
} // End PrimSpecCircleStarStorageTypeMon


//***************************************************************************
//  PrimFnMonCircleStarFisI
//
//  Primitive scalar function monadic CircleStar:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonCircleStarFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow
    // ***FIXME*** -- Possible loss of precision
    // ***FIXME*** -- Handle DOMAIN ERROR

    return log ((APLFLOAT) aplIntegerRht);
} // End PrimFnMonCircleStarFisI


//***************************************************************************
//  PrimFnMonCircleStarFisF
//
//  Primitive scalar function monadic CircleStar:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonCircleStarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow
    // ***FIXME*** -- Handle DOMAIN ERROR

    return log (aplFloatRht);
} // End PrimFnMonCircleStarFisF


//***************************************************************************
//  PrimSpecCircleStarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStarStorageTypeDyd
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

    // If both arguments are simple numeric,
    //   the result is FLOAT
    if (IsSimpleNum (*lpaplTypeLft)
     && IsSimpleNum (*lpaplTypeRht))
        aplTypeRes = ARRAY_FLOAT;
    else
        // Calculate the storage type of the result
        aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    return aplTypeRes;
} // End PrimSpecCircleStarStorageTypeDyd


//***************************************************************************
//  PrimFnDydCircleStarFisIvI
//
//  Primitive scalar function dyadic CircleStar:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydCircleStarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow/underflow
    // ***FIXME*** -- Possible loss of precision
    // ***FIXME*** -- Handle DOMAIN ERROR

    // Handle special cases
    if (aplIntegerLft EQ 1
     && aplIntegerRht EQ 1)
        return (APLFLOAT) 1;

    return log ((APLFLOAT) aplIntegerLft) / log ((APLFLOAT) aplIntegerRht);
} // End PrimFnDydCircleStarFisIvI


//***************************************************************************
//  PrimFnDydCircleStarFisFvF
//
//  Primitive scalar function dyadic CircleStar:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydCircleStarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow/underflow
    // ***FIXME*** -- Handle DOMAIN ERROR

    return log (aplFloatLft) / log (aplFloatRht);
} // End PrimFnDydCircleStarFisFvF


//***************************************************************************
//  End of File: pf_cstar.c
//***************************************************************************
