//***************************************************************************
//  NARS2000 -- Primitive Function -- NotMore
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
PRIMSPEC PrimSpecNotMore = {
    // Monadic functions
    &PrimFnMonSyntaxError_EM,
    NULL,   // &PrimSpecNotMoreStorageTypeMon, -- Can't happen w/NotMore
    NULL,   // &PrimFnMonNotMoreAPA_EM, -- Can't happen w/NotMore

    NULL,   // &PrimFnMonNotMoreBisB, -- Can't happen w/NotMore
    NULL,   // &PrimFnMonNotMoreBisI, -- Can't happen w/NotMore
    NULL,   // &PrimFnMonNotMoreBisF, -- Can't happen w/NotMore

////                 IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonNotMoreIisI, -- Can't happen w/NotMore
    NULL,   // &PrimFnMonNotMoreIisF, -- Can't happen w/NotMore

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonNotMoreFisI, -- Can't happen w/NotMore
    NULL,   // &PrimFnMonNotMoreFisF, -- Can't happen w/NotMore

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecNotMoreStorageTypeDyd,
    NULL,   // &PrimFnDydNotMoreAPA_EM, -- Can't happen w/NotMore

    &PrimFnDydNotMoreBisBvB,
    &PrimFnDydNotMoreBisIvI,
    &PrimFnDydNotMoreBisFvF,
    NULL,   // &PrimFnDydNotMoreBisCvC, -- Can't happen w/NotMore

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydNotMoreIisIvI, -- Can't happen w/NotMore
    NULL,   // &PrimFnDydNotMoreIisFvF, -- Can't happen w/NotMore

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydNotMoreFisIvI, -- Can't happen w/NotMore
    NULL,   // &PrimFnDydNotMoreFisFvF, -- Can't happen w/NotMore

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecNotMore};
#endif


//***************************************************************************
//  PrimFnNotMore_EM
//
//  Primitive function for monadic and dyadic NotMore (ERROR and "greater than")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnNotMore_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnNotMore_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_NOTMORE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnNotMore_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecNotMoreStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecNotMoreStorageTypeDyd
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
} // End PrimSpecNotMoreStorageTypeDyd


//***************************************************************************
//  PrimFnDydNotMoreBisBvB
//
//  Primitive scalar function dyadic NotMore:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydNotMoreBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft <= aplBooleanRht);
} // End PrimFnDydNotMoreBisBvB


//***************************************************************************
//  PrimFnDydNotMoreBisIvI
//
//  Primitive scalar function dyadic NotMore:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydNotMoreBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft <= aplIntegerRht);
} // End PrimFnDydNotMoreBisIvI


//***************************************************************************
//  PrimFnDydNotMoreBisFvF
//
//  Primitive scalar function dyadic NotMore:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydNotMoreBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // If the left and right arguments are tolerantly-equal, return 1
    if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
        return 1;

    // Otherwise, return the natural result
    return (aplFloatLft <= aplFloatRht);
} // End PrimFnDydNotMoreBisFvF


//***************************************************************************
//  End of File: pf_lesseq.c
//***************************************************************************
