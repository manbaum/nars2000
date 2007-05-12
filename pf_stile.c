//***************************************************************************
//  NARS2000 -- Primitive Function -- Stile
//***************************************************************************

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
PRIMSPEC PrimSpecStile = {
    // Monadic functions
    &PrimFnMon_EM,
    &PrimSpecStileStorageTypeMon,
    NULL,   // &PrimFnMonStileAPA_EM, -- Can't happen w/Stile

    &PrimFnMonStileBisB,
    NULL,   // &PrimFnMonStileBisI, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileBisF, -- Can't happen w/Stile

////               IisB,   // Handled via type promotion (to IisI)
    &PrimFnMonStileIisI,
    NULL,   // &PrimFnMonStileIisF, -- Can't happen w/Stile

////               FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonStileFisI, -- Can't happen w/Stile
    &PrimFnMonStileFisF,

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecStileStorageTypeDyd,
    NULL,   // &PrimFnDydStileAPA_EM, -- Can't happen w/Stile

    &PrimFnDydLeftCaretBisBvB,
    NULL,   // &PrimFnDydStileBisIvI, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisFvF, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisCvC, -- Can't happen w/Stile

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydStileIisIvI,
    NULL,   // &PrimFnDydStileIisFvF, -- Can't happen w/Stile

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydStileFisIvI, -- Can't happen w/Stile
    &PrimFnDydStileFisFvF,

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecStile};
#endif


//***************************************************************************
//  PrimFnStile_EM
//
//  Primitive function for monadic and dyadic Stile ("absolute value" and "modulus")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnStile_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnStile_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_STILE
         || lptkFunc->tkData.tkChar EQ '|');

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnStile_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStileStorageTypeMon
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

    // Except APA becomes INT
    if (aplTypeRes EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecStileStorageTypeMon


//***************************************************************************
//  PrimFnMonStileBisB
//
//  Primitive scalar function monadic Stile:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonStileBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonStileBisB


//***************************************************************************
//  PrimFnMonStileIisI
//
//  Primitive scalar function monadic Stile:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonStileIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (aplIntegerRht < 0)
        return -aplIntegerRht;
    else
        return  aplIntegerRht;
} // End PrimFnMonStileIisI


//***************************************************************************
//  PrimFnMonStileFisF
//
//  Primitive scalar function monadic Stile:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonStileFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return fabs (aplFloatRht);
} // End PrimFnMonStileFisF


//***************************************************************************
//  PrimSpecStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStileStorageTypeDyd
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
} // End PrimSpecStileStorageTypeDyd


//***************************************************************************
//  PrimFnDydStileIisIvI
//
//  Primitive scalar function dyadic Stile:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydStileIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplTmp, aplLft, aplRht;

    // Handle zero modulus or argument
    if (aplIntegerLft EQ 0
     || aplIntegerRht EQ 0)
        return aplIntegerRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileIisI (aplIntegerLft, lpPrimSpec);
    aplRht = PrimFnMonStileIisI (aplIntegerRht, lpPrimSpec);

    aplTmp = aplRht % aplLft;

    // If the arguments are of opposite sign
    //   and the result so far is non-zero,
    //   replace the result with its complement
    //   in the modulus.
    if ((aplIntegerLft > 0) NE (aplIntegerRht > 0)
     && aplTmp NE 0)
        aplTmp = aplLft - aplTmp;

    // The sign of the result is the sign of the left arg
    if (aplIntegerLft < 0)
        return -aplTmp;
    else
        return  aplTmp;
} // End PrimFnDydStileIisIvI


//***************************************************************************
//  PrimFnDydStileFisFvF
//
//  Primitive scalar function dyadic Stile:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydStileFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplTmp, aplLft, aplRht;
    UINT     bRet = TRUE;

    // Handle zero modulus or argument
    if (aplFloatLft EQ 0
     || aplFloatRht EQ 0)
        return aplFloatRht;

    // ***FIXME*** -- Handle overflow in the following FP division

    // If Rht divided-by Lft is near an integer within CT
    //   return 0.
    FloatToAplint_SCT (aplFloatRht / aplFloatLft, &bRet);
    if (bRet)
        return 0;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileFisF (aplFloatLft, lpPrimSpec);
    aplRht = PrimFnMonStileFisF (aplFloatRht, lpPrimSpec);

    aplTmp = fmod (aplRht, aplLft);

    // If the arguments are of opposite sign
    //   and the result so far is non-zero,
    //   replace the result with its complement
    //   in the modulus.
    if ((aplFloatLft > 0) NE (aplFloatRht > 0)
     && aplTmp NE 0)
        aplTmp = aplLft - aplTmp;

    // The sign of the result is the sign of the left arg
    if (aplFloatLft < 0)
        return -aplTmp;
    else
        return  aplTmp;
} // End PrimFnDydStileFisFvF


//***************************************************************************
//  End of File: pf_stile.c
//***************************************************************************
