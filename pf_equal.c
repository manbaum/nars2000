//***************************************************************************
//  NARS2000 -- Primitive Function -- Equal
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
PRIMSPEC PrimSpecEqual = {
    // Monadic functions
    &PrimFnMonSyntaxError_EM,
    NULL,   // &PrimSpecEqualStorageTypeMon, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualAPA_EM, -- Can't happen w/Equal

    NULL,   // &PrimFnMonEqualBisB, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualBisI, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualBisF, -- Can't happen w/Equal

////               IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonEqualIisI, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualIisF, -- Can't happen w/Equal

////               FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonEqualFisI, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualFisF, -- Can't happen w/Equal

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecEqualStorageTypeDyd,
    NULL,   // &PrimFnDydEqualAPA_EM, -- Can't happen w/Equal

    &PrimFnDydEqualBisBvB,
    &PrimFnDydEqualBisIvI,
    &PrimFnDydEqualBisFvF,
    &PrimFnDydEqualBisCvC,

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydEqualIisIvI, -- Can't happen w/Equal
    NULL,   // &PrimFnDydEqualIisFvF, -- Can't happen w/Equal

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydEqualFisIvI, -- Can't happen w/Equal
    NULL,   // &PrimFnDydEqualFisFvF, -- Can't happen w/Equal

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecEqual};
#endif


//***************************************************************************
//  PrimFnEqual_EM
//
//  Primitive function for monadic and dyadic Equal (ERROR and "equal")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnEqual_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnEqual_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_EQUAL);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnEqual_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecEqualStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecEqualStorageTypeDyd
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

    // All simple numeric results are Boolean
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecEqualStorageTypeDyd


//***************************************************************************
//  PrimFnDydEqualBisBvB
//
//  Primitive scalar function dyadic Equal:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydEqualBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft EQ aplBooleanRht);
} // End PrimFnDydEqualBisBvB


//***************************************************************************
//  PrimFnDydEqualBisIvI
//
//  Primitive scalar function dyadic Equal:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydEqualBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft EQ aplIntegerRht);
} // End PrimFnDydEqualBisIvI


//***************************************************************************
//  PrimFnDydEqualBisFvF
//
//  Primitive scalar function dyadic Equal:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydEqualBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return CompareCT (aplFloatLft, aplFloatRht, fQuadCT, lpPrimSpec);
} // End PrimFnDydEqualBisFvF


//***************************************************************************
//  CompareCT
//
//  Compare two floating point values with a Comparison Tolerance
//***************************************************************************

APLBOOL CompareCT
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     APLFLOAT   fCompTol,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplLftAbs,
             aplRhtAbs,
             aplHoodLo;

    // If Lft EQ Rht (absolutely), return 1
    if (aplFloatLft EQ aplFloatRht)
        return 1;

    // If the comparison tolerance is zero, return 0
    if (fCompTol EQ 0)
        return 0;

    // If the signs differ, return 0
    if (PrimFnMonTimesIisF (aplFloatLft, lpPrimSpec)
     NE PrimFnMonTimesIisF (aplFloatRht, lpPrimSpec))
        return 0;

    // Get the absolute values
    aplLftAbs = PrimFnMonStileFisF (aplFloatLft, lpPrimSpec);
    aplRhtAbs = PrimFnMonStileFisF (aplFloatRht, lpPrimSpec);

    // Calculate the low end of the left neighborhood of (|Rht)
    // ***FIXME*** -- Handle exponent underflow in the
    //   following multiplication
    aplHoodLo = aplRhtAbs - PrimFnMonStileFisF (aplRhtAbs, lpPrimSpec) * fCompTol;

    // If (|Rht) is greater than (|Lft),
    // and (|Lft) is in the
    //    left-neighborhood of (|Rht) with CT, return 1
    if (aplHoodLo <= aplLftAbs
     &&              aplLftAbs < aplRhtAbs)
        return 1;

    // Calculate the low end of the left neighborhood of (|Lft)
    // ***FIXME*** -- Handle exponent underflow in the
    //   following multiplication
    aplHoodLo = aplLftAbs - PrimFnMonStileFisF (aplLftAbs, lpPrimSpec) * fCompTol;

    // If (|Lft) is greater than (|Rht),
    // and (|Rht) is in the
    //    left-neighborhood of (|Lft) with CT, return 1
    if (aplHoodLo <= aplRhtAbs
     &&              aplRhtAbs < aplLftAbs)
        return 1;

    return 0;
} // End CompareCT


//***************************************************************************
//  PrimFnDydEqualBisCvC
//
//  Primitive scalar function dyadic Equal:  B {is} C fn C
//***************************************************************************

APLBOOL PrimFnDydEqualBisCvC
    (APLCHAR    aplCharLft,
     APLCHAR    aplCharRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplCharLft EQ aplCharRht);
} // End PrimFnDydEqualBisCvC


//***************************************************************************
//  End of File: pf_equal.c
//***************************************************************************
