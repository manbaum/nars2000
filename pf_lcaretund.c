//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftCaretUnderbar
//***************************************************************************

#define STRICT
#include <windows.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


#ifndef PROTO
PRIMSPEC PrimSpecLeftCaretUnderbar = {
    // Monadic functions
    &PrimFnMonSyntaxError_EM,
    NULL,   // &PrimSpecLeftCaretUnderbarStorageTypeMon, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarAPA_EM, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnMonLeftCaretUnderbarBisB, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarBisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarBisF, -- Can't happen w/LeftCaretUnderbar

////                 IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonLeftCaretUnderbarIisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarIisF, -- Can't happen w/LeftCaretUnderbar

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonLeftCaretUnderbarFisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisF, -- Can't happen w/LeftCaretUnderbar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecLeftCaretUnderbarStorageTypeDyd,
    NULL,   // &PrimFnDydLeftCaretUnderbarAPA_EM, -- Can't happen w/LeftCaretUnderbar

    &PrimFnDydLeftCaretUnderbarBisBvB,
    &PrimFnDydLeftCaretUnderbarBisIvI,
    &PrimFnDydLeftCaretUnderbarBisFvF,
    NULL,   // &PrimFnDydLeftCaretUnderbarBisCvC, -- Can't happen w/LeftCaretUnderbar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydLeftCaretUnderbarIisIvI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarIisFvF, -- Can't happen w/LeftCaretUnderbar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydLeftCaretUnderbarFisIvI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarFisFvF, -- Can't happen w/LeftCaretUnderbar
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecLeftCaretUnderbar};
#endif


//***************************************************************************
//  $PrimFnLeftCaretUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic LeftCaretUnderbar (ERROR and "less than or equal")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnLeftCaretUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnLeftCaretUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_LEFTCARETUNDERBAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnLeftCaretUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecLeftCaretUnderbarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecLeftCaretUnderbarStorageTypeDyd
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
} // End PrimSpecLeftCaretUnderbarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisBvB
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydLeftCaretUnderbarBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft <= aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarBisBvB


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisIvI
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydLeftCaretUnderbarBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft <= aplIntegerRht);
} // End PrimFnDydLeftCaretUnderbarBisIvI


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisFvF
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydLeftCaretUnderbarBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // If the left and right arguments are tolerantly-equal, return 1
    if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
        return 1;

    // Otherwise, return the natural result
    return (aplFloatLft <= aplFloatRht);
} // End PrimFnDydLeftCaretUnderbarBisFvF


//***************************************************************************
//  End of File: pf_lcaretund.c
//***************************************************************************
