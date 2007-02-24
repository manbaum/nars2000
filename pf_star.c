//***************************************************************************
//  NARS2000 -- Primitive Function -- Star
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
PRIMSPEC PrimSpecStar =
{
    // Monadic functions
    &PrimFnMon_EM,
    &PrimSpecStarStorageTypeMon,
    NULL,   // &PrimFnMonStarAPA_EM, -- Can't happen w/Star

    NULL,   // &PrimFnMonStarBisB, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarBisI, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarBisF, -- Can't happen w/Star

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonStarIisI, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarIisF, -- Can't happen w/Star

////               FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonStarFisI,
    &PrimFnMonStarFisF,

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecStarStorageTypeDyd,
    NULL,   // &PrimFnDydStarAPA_EM, -- Can't happen w/Star

    &PrimFnDydNotLessBisBvB,
    NULL,   // &PrimFnDydStarBisIvI, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisFvF, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisCvC, -- Can't happen w/Star

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydStarIisIvI, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarIisFvF, -- Can't happen w/Star

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydStarFisIvI,
    &PrimFnDydStarFisFvF,

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecStar};
#endif


//***************************************************************************
//  PrimFnStar_EM
//
//  Primitive function for monadic and dyadic Star ("signum" and "multiplication")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnStar_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnStar_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_STAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnStar_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecStarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStarStorageTypeMon
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
} // End PrimSpecStarStorageTypeMon


//***************************************************************************
//  PrimFnMonStarFisI
//
//  Primitive scalar function monadic Star:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonStarFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow
    // ***FIXME*** -- Possible loss of precision

    return exp ((APLFLOAT) aplIntegerRht);
} // End PrimFnMonStarFisI


//***************************************************************************
//  PrimFnMonStarFisF
//
//  Primitive scalar function monadic Star:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonStarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow

    return exp (aplFloatRht);
} // End PrimFnMonStarFisF


//***************************************************************************
//  PrimSpecStarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStarStorageTypeDyd
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
} // End PrimSpecStarStorageTypeDyd


//***************************************************************************
//  PrimFnDydStarFisIvI
//
//  Primitive scalar function dyadic Star:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydStarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow/underflow
    // ***FIXME*** -- Possible loss of precision

    return pow ((APLFLOAT) aplIntegerLft, (APLFLOAT) aplIntegerRht);
} // End PrimFnDydStarFisIvI


//***************************************************************************
//  PrimFnDydStarFisFvF
//
//  Primitive scalar function dyadic Star:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydStarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // ***FIXME*** -- Handle overflow/underflow

    return pow (aplFloatLft, aplFloatRht);
} // End PrimFnDydStarFisFvF


//***************************************************************************
//  End of File: pf_star.c
//***************************************************************************
