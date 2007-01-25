//***************************************************************************
//  NARS2000 -- Primitive Function -- Divide
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>

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
PRIMSPEC PrimSpecDivide =
{
    // Monadic functions
    &PrimFnMon_EM,
    &PrimSpecDivideStorageTypeMon,
    NULL,   // &PrimFnMonDivideAPA_EM, -- Can't happen w/Divide

    NULL,   // &PrimFnMonDivideBisB, -- Can't happen w/Divide
    NULL,   // &PrimFnMonDivideBisI, -- Can't happen w/Divide
    NULL,   // &PrimFnMonDivideBisF, -- Can't happen w/Divide

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonDivideIisI, -- Can't happen w/Divide
    NULL,   // &PrimFnMonDivideIisF, -- Can't happen w/Divide

////               FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonDivideFisI,
    &PrimFnMonDivideFisF,

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecDivideStorageTypeDyd,
    NULL,   // &PrimFnDydDivideAPA_EM, -- Can't happen w/Divide

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydDivideBisIvI, -- Can't happen w/Divide
    NULL,   // &PrimFnDydDivideBisFvF, -- Can't happen w/Divide
    NULL,   // &PrimFnDydDivideBisCvC, -- Can't happen w/Divide

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydDivideIisIvI, -- Can't happen w/Divide
    NULL,   // &PrimFnDydDivideIisFvF, -- Can't happen w/Divide

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydDivideFisIvI,
    &PrimFnDydDivideFisFvF,

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecDivide};
#endif


//***************************************************************************
//  PrimFnDivide_EM
//
//  Primitive function for monadic and dyadic Divide ("signum" and "multiplication")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDivide_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDivide_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_DIVIDE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnDivide_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecDivideStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDivideStorageTypeMon
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
    cArrTypeRes = *lpcArrTypeRht;

    // Except that BOOL, INT and APA become FLOAT
    if (IsSimpleInt (cArrTypeRes))
        return ARRAY_FLOAT;

    return cArrTypeRes;
} // End PrimSpecDivideStorageTypeMon


//***************************************************************************
//  PrimFnMonDivideFisI
//
//  Primitive scalar function monadic Divide:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonDivideFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // The FPU handles overflow and underflow for us
    return (1 / (APLFLOAT) aplIntegerRht);
} // End PrimFnMonDivideFisI


//***************************************************************************
//  PrimFnMonDivideFisF
//
//  Primitive scalar function monadic Divide:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonDivideFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // The FPU handles overflow and underflow for us
    return (1 / aplFloatRht);
} // End PrimFnMonDivideFisF


//***************************************************************************
//  PrimSpecDivideStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDivideStorageTypeDyd
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

    // Except that BOOL, INT, and APA become FLOAT
    if (IsSimpleInt (cArrTypeRes))
        return ARRAY_FLOAT;

    return cArrTypeRes;
} // End PrimSpecDivideStorageTypeDyd


//***************************************************************************
//  PrimFnDydDivideFisIvI
//
//  Primitive scalar function dyadic Divide:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydDivideFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // The FPU handles overflow and underflow for us
    return (((APLFLOAT) aplIntegerLft) / (APLFLOAT) aplIntegerRht);
} // End PrimFnDydDivideFisIvI


//***************************************************************************
//  PrimFnDydDivideFisFvF
//
//  Primitive scalar function dyadic Divide:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydDivideFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // The FPU handles overflow and underflow for us
    return (aplFloatLft / aplFloatRht);
} // End PrimFnDydDivideFisFvF


//***************************************************************************
//  End of File: pf_divide.c
//***************************************************************************
