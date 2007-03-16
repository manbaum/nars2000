//***************************************************************************
//  NARS2000 -- Primitive Function -- NotLess
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
PRIMSPEC PrimSpecNotLess = {
    // Monadic functions
    &PrimFnMonSyntaxError_EM,
    NULL,   // &PrimSpecNotLessStorageTypeMon, -- Can't happen w/NotLess
    NULL,   // &PrimFnMonNotLessAPA_EM, -- Can't happen w/NotLess

    NULL,   // &PrimFnMonNotLessBisB, -- Can't happen w/NotLess
    NULL,   // &PrimFnMonNotLessBisI, -- Can't happen w/NotLess
    NULL,   // &PrimFnMonNotLessBisF, -- Can't happen w/NotLess

////                 IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonNotLessIisI, -- Can't happen w/NotLess
    NULL,   // &PrimFnMonNotLessIisF, -- Can't happen w/NotLess

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonNotLessFisI, -- Can't happen w/NotLess
    NULL,   // &PrimFnMonNotLessFisF, -- Can't happen w/NotLess

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecNotLessStorageTypeDyd,
    NULL,   // &PrimFnDydNotLessAPA_EM, -- Can't happen w/NotLess

    &PrimFnDydNotLessBisBvB,
    &PrimFnDydNotLessBisIvI,
    &PrimFnDydNotLessBisFvF,
    NULL,   // &PrimFnDydNotLessBisCvC, -- Can't happen w/NotLess

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydNotLessIisIvI, -- Can't happen w/NotLess
    NULL,   // &PrimFnDydNotLessIisFvF, -- Can't happen w/NotLess

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydNotLessFisIvI, -- Can't happen w/NotLess
    NULL,   // &PrimFnDydNotLessFisFvF, -- Can't happen w/NotLess

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecNotLess};
#endif


//***************************************************************************
//  PrimFnNotLess_EM
//
//  Primitive function for monadic and dyadic NotLess (ERROR and "greater than or equal")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnNotLess_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnNotLess_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_NOTLESS);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnNotLess_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecNotLessStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecNotLessStorageTypeDyd
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
} // End PrimSpecNotLessStorageTypeDyd


//***************************************************************************
//  PrimFnDydNotLessBisBvB
//
//  Primitive scalar function dyadic NotLess:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydNotLessBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft >= aplBooleanRht);
} // End PrimFnDydNotLessBisBvB


//***************************************************************************
//  PrimFnDydNotLessBisIvI
//
//  Primitive scalar function dyadic NotLess:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydNotLessBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft >= aplIntegerRht);
} // End PrimFnDydNotLessBisIvI


//***************************************************************************
//  PrimFnDydNotLessBisFvF
//
//  Primitive scalar function dyadic NotLess:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydNotLessBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // If the left and right arguments are tolerantly-equal, return 1
    if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
        return 1;

    // Otherwise, return the natural result
    return (aplFloatLft >= aplFloatRht);
} // End PrimFnDydNotLessBisFvF


//***************************************************************************
//  End of File: pf_greateq.c
//***************************************************************************
