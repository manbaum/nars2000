//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftCaret
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "datatype.h"
#include "resdebug.h"
#include "symtab.h"
#include "tokens.h"
#include "parse.h"
#include "primspec.h"
#include "Unicode.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

extern enum EXEC_CODES ExecCode;

#ifndef PROTO
PRIMSPEC PrimSpecLeftCaret = {
    // Monadic functions
    &PrimFnMonSyntaxError_EM,
    NULL,   // &PrimSpecLeftCaretStorageTypeMon, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretAPA_EM, -- Can't happen w/LeftCaret

    NULL,   // &PrimFnMonLeftCaretBisB, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretBisI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretBisF, -- Can't happen w/LeftCaret

////                   IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonLeftCaretIisI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretIisF, -- Can't happen w/LeftCaret

////                   FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonLeftCaretFisI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretFisF, -- Can't happen w/LeftCaret

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecLeftCaretStorageTypeDyd,
    NULL,   // &PrimFnDydLeftCaretAPA_EM, -- Can't happen w/LeftCaret

    &PrimFnDydLeftCaretBisBvB,
    &PrimFnDydLeftCaretBisIvI,
    &PrimFnDydLeftCaretBisFvF,
    NULL,   // &PrimFnDydLeftCaretBisCvC, -- Can't happen w/LeftCaret

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydLeftCaretIisIvI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretIisFvF, -- Can't happen w/LeftCaret

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydLeftCaretFisIvI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretFisFvF, -- Can't happen w/LeftCaret

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecLeftCaret};
#endif


//***************************************************************************
//  PrimFnLeftCaret_EM
//
//  Primitive function for monadic and dyadic LeftCaret (ERROR and "greater than")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnLeftCaret_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnLeftCaret_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_LEFTCARET);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnLeftCaret_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecLeftCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecLeftCaretStorageTypeDyd
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

    // All simple numeric results are Boolean
    if (IsSimpleNum (cArrTypeRes))
        cArrTypeRes = ARRAY_BOOL;

    return cArrTypeRes;
} // End PrimSpecLeftCaretStorageTypeDyd


//***************************************************************************
//  PrimFnDydLeftCaretBisBvB
//
//  Primitive scalar function dyadic LeftCaret:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft < aplBooleanRht;
} // End PrimFnDydLeftCaretBisBvB


//***************************************************************************
//  PrimFnDydLeftCaretBisIvI
//
//  Primitive scalar function dyadic LeftCaret:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplIntegerLft < aplIntegerRht);
} // End PrimFnDydLeftCaretBisIvI


//***************************************************************************
//  PrimFnDydLeftCaretBisFvF
//
//  Primitive scalar function dyadic LeftCaret:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydLeftCaretBisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // If the left and right arguments are tolerantly-equal, return 0
    if (PrimFnDydEqualBisFvF (aplFloatLft, aplFloatRht, lpPrimSpec))
        return 0;

    // Otherwise, return the natural result
    return (aplFloatLft < aplFloatRht);
} // End PrimFnDydLeftCaretBisFvF


//***************************************************************************
//  End of File: pf_lcaret.c
//***************************************************************************
