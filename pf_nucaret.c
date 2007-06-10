//***************************************************************************
//  NARS2000 -- Primitive Function -- UpCaretTilde
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
PRIMSPEC PrimSpecUpCaretTilde = {
    // Monadic functions
    &PrimFnMonSyntaxError_EM,
    NULL,   // &PrimSpecUpCaretTildeStorageTypeMon, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeAPA_EM, -- Can't happen w/UpCaretTilde

    NULL,   // &PrimFnMonUpCaretTildeBisB, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeBisI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeBisF, -- Can't happen w/UpCaretTilde

////                      IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonUpCaretTildeIisI, -- Can't happend w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeIisF, -- Can't happend w/UpCaretTilde

////                      FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonUpCaretTildeFisI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeFisF, -- Can't happen w/UpCaretTilde

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpCaretTildeStorageTypeDyd,
    NULL,   // &PrimFnDydUpCaretTildeAPA_EM, -- Can't happen w/UpCaretTilde

    &PrimFnDydUpCaretTildeBisBvB,
    &PrimFnDydUpCaretTildeBisIvI,
    &PrimFnDydUpCaretTildeBisFvF,
    NULL,   // &PrimFnDydUpCaretTildeBisCvC, -- Can't happen w/UpCaretTilde

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydUpCaretTildeIisIvI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeIisFvF, -- Can't happen w/UpCaretTilde

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydUpCaretTildeFisIvI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeFisFvF, -- Can't happen w/UpCaretTilde
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpCaretTilde};
#endif


//***************************************************************************
//  $PrimFnUpCaretTilde_EM_YY
//
//  Primitive function for monadic and dyadic UpCaretTilde (ERROR and "nand")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpCaretTilde_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnUpCaretTilde_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPCARETTILDE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpCaretTilde_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpCaretTildeStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpCaretTildeStorageTypeDyd
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

    // All simple numerics return Boolean
    if (IsSimpleNum (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecUpCaretTildeStorageTypeDyd


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisBvB
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return !(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeBisBvB


//***************************************************************************
//  $PrimFnDydUpCaretTildeIisIvI
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (!IsBooleanValue (aplIntegerLft)
     || !IsBooleanValue (aplIntegerRht))
        RaiseException (EXEC_DOMAIN_ERROR, 0, 0, NULL);
    return !(((APLBOOL) aplIntegerLft) & (APLBOOL) aplIntegerRht);
} // End PrimFnDydUpCaretTildeBisIvI


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisFvF
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydUpCaretTildeBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplLft,
           aplRht;
    BOOL   bRet = TRUE;

    // Convert the value to an integer using System CT
    aplLft = FloatToAplint_SCT (aplFloatLft, &bRet);
    if (bRet)
        aplRht = FloatToAplint_SCT (aplFloatRht, &bRet);
    if (!bRet
     || !IsBooleanValue (aplLft)
     || !IsBooleanValue (aplRht))
        RaiseException (EXEC_DOMAIN_ERROR, 0, 0, NULL);

    return !(((APLBOOL) aplLft) & (APLBOOL) aplRht);
} // End PrimFnDydUpCaretTildeBisFvF


//***************************************************************************
//  End of File: pf_nucaret.c
//***************************************************************************
