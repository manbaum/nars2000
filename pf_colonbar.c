//***************************************************************************
//  NARS2000 -- Primitive Function -- ColonBar
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
PRIMSPEC PrimSpecColonBar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecColonBarStorageTypeMon,
    NULL,   // &PrimFnMonColonBarAPA_EM, -- Can't happen w/ColonBar

    NULL,   // &PrimFnMonColonBarBisB, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarBisI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarBisF, -- Can't happen w/ColonBar

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonColonBarIisI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarIisF, -- Can't happen w/ColonBar

////               FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonColonBarFisI,
    &PrimFnMonColonBarFisF,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecColonBarStorageTypeDyd,
    NULL,   // &PrimFnDydColonBarAPA_EM, -- Can't happen w/ColonBar

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydColonBarBisIvI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisFvF, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisCvC, -- Can't happen w/ColonBar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydColonBarIisIvI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarIisFvF, -- Can't happen w/ColonBar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydColonBarFisIvI,
    &PrimFnDydColonBarFisFvF,

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecColonBar};
#endif


//***************************************************************************
//  $PrimFnColonBar_EM_YY
//
//  Primitive function for monadic and dyadic ColonBar ("reciprocal" and "division")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnColonBar_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnColonBar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_COLONBAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnColonBar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecColonBarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecColonBarStorageTypeMon
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
        return ARRAY_FLOAT;

    return aplTypeRes;
} // End PrimSpecColonBarStorageTypeMon


//***************************************************************************
//  $PrimFnMonColonBarFisI
//
//  Primitive scalar function monadic ColonBar:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonColonBarFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // The FPU handles overflow and underflow for us
    return (1 / (APLFLOAT) aplIntegerRht);
} // End PrimFnMonColonBarFisI


//***************************************************************************
//  $PrimFnMonColonBarFisF
//
//  Primitive scalar function monadic ColonBar:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonColonBarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // The FPU handles overflow and underflow for us
    return (1 / aplFloatRht);
} // End PrimFnMonColonBarFisF


//***************************************************************************
//  $PrimSpecColonBarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecColonBarStorageTypeDyd
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

    // Except that BOOL, INT, and APA become FLOAT
    if (IsSimpleInt (aplTypeRes))
        return ARRAY_FLOAT;

    return aplTypeRes;
} // End PrimSpecColonBarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydColonBarFisIvI
//
//  Primitive scalar function dyadic ColonBar:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydColonBarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // The FPU handles overflow and underflow for us
    return (((APLFLOAT) aplIntegerLft) / (APLFLOAT) aplIntegerRht);
} // End PrimFnDydColonBarFisIvI


//***************************************************************************
//  $PrimFnDydColonBarFisFvF
//
//  Primitive scalar function dyadic ColonBar:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydColonBarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // The FPU handles overflow and underflow for us
    return (aplFloatLft / aplFloatRht);
} // End PrimFnDydColonBarFisFvF


//***************************************************************************
//  End of File: pf_colonbar.c
//***************************************************************************
