//***************************************************************************
//  NARS2000 -- Primitive Function -- DownCaretTilde
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>

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
PRIMSPEC PrimSpecDownCaretTilde = {
    // Monadic functions
    &PrimFnMonSyntaxError_EM,
    NULL,   // &PrimSpecDownCaretTildeStorageTypeMon, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeAPA_EM, -- Can't happen w/DownCaretTilde

    NULL,   // &PrimFnMonDownCaretTildeBisB, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeBisI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeBisF, -- Can't happen w/DownCaretTilde

////                        IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonDownCaretTildeIisI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeIisF, -- Can't happen w/DownCaretTilde

////                        FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonDownCaretTildeFisI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeFisF, -- Can't happen w/DownCaretTilde

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecDownCaretTildeStorageTypeDyd,
    NULL,   // &PrimFnDydDownCaretTildeAPA_EM, -- Can't happen w/DownCaretTilde

    &PrimFnDydDownCaretTildeBisBvB,
    &PrimFnDydDownCaretTildeBisIvI,
    &PrimFnDydDownCaretTildeBisFvF,
    NULL,   // &PrimFnDydDownCaretTildeBisCvC, -- Can't happen w/DownCaretTilde

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydDownCaretTildeIisIvI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeIisFvF, -- Can't happen w/DownCaretTilde

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydDownCaretTildeFisIvI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeFisFvF, -- Can't happen w/DownCaretTilde

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecDownCaretTilde};
#endif


//***************************************************************************
//  PrimFnDownCaretTilde_EM
//
//  Primitive function for monadic and dyadic DownCaretTilde (ERROR and "nor")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownCaretTilde_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDownCaretTilde_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_DOWNCARETTILDE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnDownCaretTilde_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecDownCaretTildeStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownCaretTildeStorageTypeDyd
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

    // All simple numerics return Boolean
    if (IsSimpleNum (cArrTypeRes))
        cArrTypeRes = ARRAY_BOOL;

    return cArrTypeRes;
} // End PrimSpecDownCaretTildeStorageTypeDyd


//***************************************************************************
//  PrimFnDydDownCaretTildeBisBvB
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydDownCaretTildeBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return !(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeBisBvB


//***************************************************************************
//  PrimFnDydDownCaretTildeIisIvI
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} I fn I
//***************************************************************************

APLBOOL PrimFnDydDownCaretTildeBisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (!IsBooleanValue (aplIntegerLft)
     || !IsBooleanValue (aplIntegerRht))
        RaiseException (EXEC_DOMAIN_ERROR, 0, 0, NULL);
    return !(((APLBOOL) aplIntegerLft) | (APLBOOL) aplIntegerRht);
} // End PrimFnDydDownCaretTildeBisIvI


//***************************************************************************
//  PrimFnDydDownCaretTildeBisFvF
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} F fn F
//***************************************************************************

APLBOOL PrimFnDydDownCaretTildeBisFvF
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

    return !(((APLBOOL) aplLft) | (APLBOOL) aplRht);
} // End PrimFnDydDownCaretTildeBisFvF


//***************************************************************************
//  End of File: pf_ndcaret.c
//***************************************************************************
