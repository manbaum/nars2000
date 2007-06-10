//***************************************************************************
//  NARS2000 -- Primitive Function -- UpCaret
//***************************************************************************

#define STRICT
#include <windows.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "sysvars.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifndef PROTO
PRIMSPEC PrimSpecUpCaret = {
    // Monadic functions
    &PrimFnMonSyntaxError_EM,
    NULL,   // &PrimSpecUpCaretStorageTypeMon, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretAPA_EM, -- Can't happen w/UpCaret

    NULL,   // &PrimFnMonUpCaretBisB, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretBisI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretBisF, -- Can't happen w/UpCaret

////                 IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonUpCaretIisI, -- Can't happend w/UpCaret
    NULL,   // &PrimFnMonUpCaretIisF, -- Can't happend w/UpCaret

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonUpCaretFisI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisF, -- Can't happen w/UpCaret

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpCaretStorageTypeDyd,
    NULL,   // &PrimFnDydUpCaretAPA_EM, -- Can't happen w/UpCaret

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydUpCaretBisIvI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisFvF, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisCvC, -- Can't happen w/UpCaret

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydUpCaretIisIvI,
    NULL,   // &PrimFnDydUpCaretIisFvF, -- Can't happen w/UpCaret

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydUpCaretFisIvI,
    &PrimFnDydUpCaretFisFvF,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpCaret};
#endif


//***************************************************************************
//  $PrimFnUpCaret_EM_YY
//
//  Primitive function for monadic and dyadic UpCaret (ERROR and "and/LCM")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnUpCaret_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPCARET
         || lptkFunc->tkData.tkChar EQ UTF16_CIRCUMFLEX);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpCaret_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpCaretStorageTypeDyd
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
} // End PrimSpecUpCaretStorageTypeDyd


//***************************************************************************
//  $lcmAplInt
//
//  LCM (Least Common Multiple) for aplIntegers
//***************************************************************************

APLINT lcmAplInt
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplTmp, aplLft, aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileIisI (aplIntegerLft, lpPrimSpec);
    aplRht = PrimFnMonStileIisI (aplIntegerRht, lpPrimSpec);

    // Calculate the GCD
    aplTmp = gcdAplInt (aplLft, aplRht, lpPrimSpec);
    if (aplTmp EQ 0)
        return aplTmp;

    aplTmp = aplLft * (aplRht / aplTmp);

    // The sign of the result is the sign of the left argument
    if (aplIntegerLft < 0)
        return -aplTmp;
    else
        return  aplTmp;
} // End lcmAplInt


//***************************************************************************
//  $lcmAplFloat
//
//  LCM (Least Common Multiple) for aplFloats
//***************************************************************************

APLFLOAT lcmAplFloat
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplTmp, aplLft, aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileFisF (aplFloatLft, lpPrimSpec);
    aplRht = PrimFnMonStileFisF (aplFloatRht, lpPrimSpec);

    // Calculate the GCD
    aplTmp = gcdAplFloat (aplLft, aplRht, lpPrimSpec);
    if (fabs (aplTmp) < SYS_CT)
        return aplTmp;

    aplTmp = aplLft * (aplRht / aplTmp);

    // The sign of the result is the sign of the left argument
    if (aplFloatLft < 0)
        return -aplTmp;
    else
        return  aplTmp;
} // End lcmAplFloat


//***************************************************************************
//  $PrimFnDydUpCaretBisBvB
//
//  Primitive scalar function dyadic UpCaret:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydUpCaretBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretBisBvB


//***************************************************************************
//  $PrimFnDydUpCaretIisIvI
//
//  Primitive scalar function dyadic UpCaret:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydUpCaretIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return lcmAplInt (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydUpCaretIisIvI


//***************************************************************************
//  $PrimFnDydUpCaretFisIvI
//
//  Primitive scalar function dyadic UpCaret:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydUpCaretFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (APLFLOAT) lcmAplInt (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydUpCaretFisIvI


//***************************************************************************
//  $PrimFnDydUpCaretFisFvF
//
//  Primitive scalar function dyadic UpCaret:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydUpCaretFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return lcmAplFloat (aplFloatLft, aplFloatRht, lpPrimSpec);
} // End PrimFnDydUpCaretFisFvF


//***************************************************************************
//  End of File: pf_ucaret.c
//***************************************************************************
