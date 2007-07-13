//***************************************************************************
//  NARS2000 -- Primitive Function -- RightCaretUnderbar
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
PRIMSPEC PrimSpecRightCaretUnderbar = {
    // Monadic functions
    &PrimFnMonSyntaxError_EM,
    NULL,   // &PrimSpecRightCaretUnderbarStorageTypeMon, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarAPA_EM, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnMonRightCaretUnderbarBisB, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarBisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarBisF, -- Can't happen w/RightCaretUnderbar

////                 IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonRightCaretUnderbarIisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarIisF, -- Can't happen w/RightCaretUnderbar

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonRightCaretUnderbarFisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisF, -- Can't happen w/RightCaretUnderbar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRightCaretUnderbarStorageTypeDyd,
    NULL,   // &PrimFnDydRightCaretUnderbarAPA_EM, -- Can't happen w/RightCaretUnderbar

    &PrimFnDydRightCaretUnderbarBisBvB,
    &PrimFnDydRightCaretUnderbarBisIvI,
    &PrimFnDydRightCaretUnderbarBisFvF,
    NULL,   // &PrimFnDydRightCaretUnderbarBisCvC, -- Can't happen w/RightCaretUnderbar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydRightCaretUnderbarIisIvI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarIisFvF, -- Can't happen w/RightCaretUnderbar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydRightCaretUnderbarFisIvI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarFisFvF, -- Can't happen w/RightCaretUnderbar
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecRightCaretUnderbar};
#endif


//***************************************************************************
//  $PrimFnRightCaretUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic RightCaretUnderbar (ERROR and "not less")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnRightCaretUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnRightCaretUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTCARETUNDERBAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnRightCaretUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecRightCaretUnderbarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRightCaretUnderbarStorageTypeDyd
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
} // End PrimSpecRightCaretUnderbarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisBvB
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydRightCaretUnderbarBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft >= aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarBisBvB


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisIvI
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydRightCaretUnderbarBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft >= aplIntegerRht);
} // End PrimFnDydRightCaretUnderbarBisIvI


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisFvF
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydRightCaretUnderbarBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // If the left and right arguments are tolerantly-equal, return 1
    if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
        return 1;

    // Otherwise, return the natural result
    return (aplFloatLft >= aplFloatRht);
} // End PrimFnDydRightCaretUnderbarBisFvF


//***************************************************************************
//  End of File: pf_rcaretund.c
//***************************************************************************
