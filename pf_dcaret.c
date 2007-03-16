//***************************************************************************
//  NARS2000 -- Primitive Function -- DownCaret
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "sysvars.h"
#include "externs.h"
#include "primspec.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifndef PROTO
PRIMSPEC PrimSpecDownCaret = {
    // Monadic functions
    &PrimFnMonSyntaxError_EM,
    NULL,   // &PrimSpecDownCaretStorageTypeMon, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretAPA_EM, -- Can't happen w/DownCaret

    NULL,   // &PrimFnMonDownCaretBisB, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretBisI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretBisF, -- Can't happen w/DownCaret

////                   IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonDownCaretIisI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretIisF, -- Can't happen w/DownCaret

////                   FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonDownCaretFisI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretFisF, -- Can't happen w/DownCaret

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecDownCaretStorageTypeDyd,
    NULL,   // &PrimFnDydDownCaretAPA_EM, -- Can't happen w/DownCaret

    &PrimFnDydDownCaretBisBvB,
    NULL,   // &PrimFnDydDownCaretBisIvI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisFvF, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisCvC, -- Can't happen w/DownCaret

////                   IisBvB,  // Handled via type promotion (to IisIvI)
    &PrimFnDydDownCaretIisIvI,
    NULL,   // &PrimFnDydDownCaretIisFvF, -- Can't happen w/DownCaret

////                   FisBvB,  // Handled via type promotion (to FisIvI)
    &PrimFnDydDownCaretFisIvI,
    &PrimFnDydDownCaretFisFvF,

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecDownCaret};
#endif


//***************************************************************************
//  PrimFnDownCaret_EM
//
//  Primitive function for monadic and dyadic downCaret (ERROR and "or/GCD")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownCaret_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDownCaret_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_DOWNCARET);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnDownCaret_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecDownCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownCaretStorageTypeDyd
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
} // End PrimSpecDownCaretStorageTypeDyd


//***************************************************************************
//  gcdAplInt
//
//  GCD (Greatest Common Divisor) for aplIntegers
//***************************************************************************

APLINT gcdAplInt
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplTmp, aplLft, aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileIisI (aplIntegerLft, lpPrimSpec);
    aplRht = PrimFnMonStileIisI (aplIntegerRht, lpPrimSpec);

    while (aplLft NE 0)
    {
        aplTmp = aplLft;
        aplLft = aplRht % aplLft;
        aplRht = aplTmp;
    } // End WHILE

    // The sign of the result is the sign of the left argument
    if (aplIntegerLft < 0)
        return -aplRht;
    else
        return  aplRht;
} // End gcdAplInt


//***************************************************************************
//  gcdAplFloat
//
//  GCD (Greatest Common Divisor) for aplFloats
//***************************************************************************

APLFLOAT gcdAplFloat
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplTmp, aplLft, aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileFisF (aplFloatLft, lpPrimSpec);
    aplRht = PrimFnMonStileFisF (aplFloatRht, lpPrimSpec);

    while (aplLft >= SYS_CT)
    {
        aplTmp = aplLft;
        aplLft = fmod (aplRht, aplLft);
        aplRht = aplTmp;
    } // End WHILE

    // The sign of the result is the sign of the left argument
    if (aplFloatLft > 0)
        return  aplRht;
    else
        return -aplRht;
} // End gcdAplFloat


//***************************************************************************
//  PrimFnDydDownCaretBisBvB
//
//  Primitive scalar function dyadic DownCaret:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydDownCaretBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretBisBvB


//***************************************************************************
//  PrimFnDydDownCaretIisIvI
//
//  Primitive scalar function dyadic DownCaret:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydDownCaretIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return gcdAplInt (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydDownCaretIisIvI


//***************************************************************************
//  PrimFnDydDownCaretFisIvI
//
//  Primitive scalar function dyadic DownCaret:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydDownCaretFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (APLFLOAT) gcdAplInt (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydDownCaretFisIvI


//***************************************************************************
//  PrimFnDydDownCaretFisFvF
//
//  Primitive scalar function dyadic DownCaret:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydDownCaretFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return gcdAplFloat (aplFloatLft, aplFloatRht, lpPrimSpec);
} // End PrimFnDydDownCaretFisFvF


//***************************************************************************
//  End of File: pf_dcaret.c
//***************************************************************************
