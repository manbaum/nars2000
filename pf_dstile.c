//***************************************************************************
//  NARS2000 -- Primitive Function -- DownStile
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <float.h>
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
extern APLFLOAT fQuadCT, Float2Pow53;


#ifndef PROTO
PRIMSPEC PrimSpecDownStile = {
    // Monadic functions
    &PrimFnMon_EM,
    &PrimSpecDownStileStorageTypeMon,
    &PrimFnMonDownStileAPA_EM,

    &PrimFnMonDownStileBisB,
    NULL,   // &PrimFnMonDownStileBisI, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileBisF, -- Can't happen w/DownStile

////                   IisB,   // Handled via type promotion (to IisI)
    &PrimFnMonDownStileIisI,
    &PrimFnMonDownStileIisF,

////                   FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonDownStileFisI, -- Can't happen w/DownStile
    &PrimFnMonDownStileFisF,

    // Dyadic functions
    &PrimFnDyd_EM,
    &PrimSpecDownStileStorageTypeDyd,
    NULL,   // &PrimFnDydDownStileAPA_EM, -- Can't happen w/DownStile

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydDownStileBisIvI, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisFvF, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisCvC, -- Can't happen w/DownStile

////                   IisBvB,  // Handled via type promotion (to IisIvI)
    &PrimFnDydDownStileIisIvI,
    NULL,   // &PrimFnDydDownStileIisFvF, -- Can't happen w/DownStile

////                   FisBvB,  // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydDownStileFisIvI, -- Can't happen w/DownStile
    &PrimFnDydDownStileFisFvF,

    // Miscellaneous
    &ExecCode,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecDownStile};
#endif


//***************************************************************************
//  PrimFnDownStile_EM
//
//  Primitive function for monadic and dyadic DownStile ("floor" and "minimum")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownStile_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDownStile_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_DOWNSTILE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnDownStile_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimSpecDownStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownStileStorageTypeMon
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
    //   except FLOAT goes to INT.
    // IisF promotes to FisF as necessary.
    if (*lpcArrTypeRht EQ ARRAY_FLOAT)
        cArrTypeRes = ARRAY_INT;
    else
        cArrTypeRes = *lpcArrTypeRht;

    return cArrTypeRes;
} // End PrimSpecDownStileStorageTypeMon


//***************************************************************************
//  PrimFnMonDownStileBisB
//
//  Primitive scalar function monadic DownStile:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonDownStileBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonDownStileBisB


//***************************************************************************
//  PrimFnMonDownStileIisI
//
//  Primitive scalar function monadic DownStile:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonDownStileIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplIntegerRht;
} // End PrimFnMonDownStileIisI


//***************************************************************************
//  PrimFnMonDownStileIisF
//
//  Primitive scalar function monadic DownStile:  I {is} fn F
//***************************************************************************

APLINT PrimFnMonDownStileIisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for ± infinity and numbers whose
    //   absolute value is >= 2*53
    if (!_finite (aplFloatRht)
     || fabs (aplFloatRht) >= Float2Pow53)
        RaiseException (EXEC_RESULT_FLOAT, 0, 0, NULL);

    return (APLINT) PrimFnMonDownStileFisF (aplFloatRht, lpPrimSpec);
} // End PrimFnMonDownStileIisF


//***************************************************************************
//  PrimFnMonDownStileFisF
//
//  Primitive scalar function monadic DownStile:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonDownStileFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloor, aplCeil, aplNear;

    // Check for ± infinity
    if (!_finite (aplFloatRht))
        return aplFloatRht;

    // Get the exact floor and ceiling
    aplFloor = floor (aplFloatRht);
    aplCeil  = ceil  (aplFloatRht);

    // Calculate the integer nearest the right arg

    // Split cases based upon the signum of the difference between
    //   (the number and its floor) and (the ceiling and the number)
    switch (PrimFnMonTimesIisF ((aplFloatRht - aplFloor)
                              - (aplCeil - aplFloatRht),
                                lpPrimSpec))
    {
        case  1:
            aplNear = aplCeil;

            break;

        case  0:
            // They are equal, so use the one with the larger absolute value
            aplNear = ((fabs (aplFloor) > fabs (aplCeil)) ? aplFloor
                                                          : aplCeil);
            break;

        case -1:
            aplNear = aplFloor;

            break;

        defstop
            break;
    } // End SWITCH

    // If Near is < Rht, return Near
    if (aplNear < aplFloatRht)
        return aplNear;

    // If Near is non-zero, and
    //   Rht is tolerantly-equal to Near,
    //   return Near; otherwise, return Near - 1
    if (aplNear)
    {
        if (PrimFnDydEqualBisFvF (aplFloatRht,
                                  (APLFLOAT) aplNear,
                                  lpPrimSpec))
            return aplNear;
        else
            return aplNear - 1;
    } // End IF

    // aplNear is zer0

    // If Rht is between (-[]CT) and 0 (inclusive),
    //   return 0; othewise, return -1
    if ((-fQuadCT) <= aplFloatRht
     &&               aplFloatRht <= 0)
        return 0;
    else
        return -1;
} // End PrimFnMonDownStileFisF


//***************************************************************************
//  PrimFnMonDownStileAPA_EM
//
//  Monadic downstile, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDownStileAPA_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonDownStileAPA_EM
    (LPYYSTYPE  lpYYRes,    // The result token (may be NULL)
     LPTOKEN    lptkFunc,
     HGLOBAL    hGlbRht,
     HGLOBAL   *lphGlbRes,
     LPVOID    *lplpMemRes,
     APLRANK    aplRankRht,
     LPPRIMSPEC lpPrimSpec)

{
    DBGENTER;

    // Axis may be anything

    *lphGlbRes = CopyArray_EM (hGlbRht, FALSE, lptkFunc);

    if (!*lphGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return FALSE;
    } // End IF

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////    lpYYRes->tkToken.tkFlags.ImmType   = 0;
////    lpYYRes->tkToken.tkFlags.NoDisplay = 0;
////    lpYYRes->tkToken.tkFlags.Color     =
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (*lphGlbRes);
    } // End IF

    DBGEXIT;

    return TRUE;
} // End PrimFnMonDownStileAPA_EM
#undef APPEND_NAME


//***************************************************************************
//  PrimSpecDownStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownStileStorageTypeDyd
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

    return cArrTypeRes;
} // End PrimSpecDownStileStorageTypeDyd


//***************************************************************************
//  PrimFnDydDownStileIisIvI
//
//  Primitive scalar function dyadic DownStile:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydDownStileIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return min (aplIntegerLft, aplIntegerRht);
} // End PrimFnDydDownStileIisIvI


//***************************************************************************
//  PrimFnDydDownStileFisFvF
//
//  Primitive scalar function dyadic DownStile:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydDownStileFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return min (aplFloatLft, aplFloatRht);
} // End PrimFnDydDownStileFisFvF


//***************************************************************************
//  End of File: pf_dstile.c
//***************************************************************************
