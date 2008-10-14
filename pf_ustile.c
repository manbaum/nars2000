//***************************************************************************
//  NARS2000 -- Primitive Function -- UpStile
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#define STRICT
#include <windows.h>
#include <float.h>
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
PRIMSPEC PrimSpecUpStile = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecUpStileStorageTypeMon,
    &PrimFnMonUpStileAPA_EM,

    &PrimFnMonUpStileBisB,
    NULL,   // &PrimFnMonUpStileBisI, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileBisF, -- Can't happen w/UpStile

////                 IisB,   // Handled via type promotion (to IisI)
    &PrimFnMonUpStileIisI,
    &PrimFnMonUpStileIisF,

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonUpStileFisI, -- Can't happen w/UpStile
    &PrimFnMonUpStileFisF,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpStileStorageTypeDyd,
    NULL,   // &PrimFnDydUpStileAPA_EM, -- Can't happen w/UpStile

    &PrimFnDydDownCaretBisBvB,
    NULL,   // &PrimFnDydUpStileBisIvI, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisFvF, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisCvC, -- Can't happen w/UpStile

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydUpStileIisIvI,
    NULL,   // &PrimFnDydUpStileIisFvF, -- Can't happen w/UpStile

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydUpStileFisIvI, -- Can't happen w/UpStile
    &PrimFnDydUpStileFisFvF,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpStile};
#endif


//***************************************************************************
//  $PrimFnUpStile_EM_YY
//
//  Primitive function for monadic and dyadic UpStile ("ceiling" and "maximum")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPSTILE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpStile_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpStileStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE aplTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsEmpty (aplNELMRht) && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    if (IsSimpleChar (*lpaplTypeRht)
     || *lpaplTypeRht EQ ARRAY_LIST)
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    //   except FLOAT goes to INT
    // IisF promotes to FisF as necessary.
    if (IsSimpleFlt (*lpaplTypeRht))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = *lpaplTypeRht;

    return aplTypeRes;
} // End PrimSpecUpStileStorageTypeMon


//***************************************************************************
//  $PrimFnMonUpStileBisB
//
//  Primitive scalar function monadic UpStile:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonUpStileBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonUpStileBisB


//***************************************************************************
//  $PrimFnMonUpStileIisI
//
//  Primitive scalar function monadic UpStile:  I {is} fn I
//***************************************************************************

APLINT PrimFnMonUpStileIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplIntegerRht;
} // End PrimFnMonUpStileIisI


//***************************************************************************
//  $PrimFnMonUpStileIisF
//
//  Primitive scalar function monadic UpStile:  I {is} fn F
//***************************************************************************

APLINT PrimFnMonUpStileIisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for � infinity and numbers whose
    //   absolute value is >= 2*53
    if (!_finite (aplFloatRht)
     || fabs (aplFloatRht) >= Float2Pow53)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return (APLINT) -PrimFnMonDownStileFisF (-aplFloatRht, lpPrimSpec);
} // End PrimFnMonUpStileIisF


//***************************************************************************
//  $PrimFnMonUpStileFisF
//
//  Primitive scalar function monadic UpStile:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonUpStileFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return -PrimFnMonDownStileFisF (-aplFloatRht, lpPrimSpec);
} // End PrimFnMonUpStileFisF


//***************************************************************************
//  $PrimFnMonUpStileAPA_EM
//
//  Monadic UpStile, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpStileAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnMonUpStileAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to result token (may be NULL)

     LPTOKEN      lptkFunc,         // Ptr to function token

     HGLOBAL      hGlbRht,          // Right arg handle
     HGLOBAL     *lphGlbRes,        // Ptr to result handle

     APLRANK      aplRankRht,       // Right arg rank

     LPPRIMSPEC   lpPrimSpec)       // Ptr to local PRIMSPEC

{
    DBGENTER;

    // Axis may be anything

    // Copy the HGLOBAL to the result
    *lphGlbRes = CopySymGlbDirAsGlb (hGlbRht);

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = hGlbRht;
    } // End IF

    DBGLEAVE;

    return TRUE;
} // End PrimFnMonUpStileAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpStileStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;

    // In case the left arg is an empty char,
    //   change its type to BOOL
    if (IsEmpty (aplNELMLft) && IsSimpleChar (*lpaplTypeLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsEmpty (aplNELMRht) && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    return aplTypeRes;
} // End PrimSpecUpStileStorageTypeDyd


//***************************************************************************
//  $PrimFnDydUpStileIisIvI
//
//  Primitive scalar function dyadic UpStile:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydUpStileIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return max (aplIntegerLft, aplIntegerRht);
} // End PrimFnDydUpStileIisIvI


//***************************************************************************
//  $PrimFnDydUpStileFisFvF
//
//  Primitive scalar function dyadic UpStile:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydUpStileFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return max (aplFloatLft, aplFloatRht);
} // End PrimFnDydUpStileFisFvF


//***************************************************************************
//  End of File: pf_ustile.c
//***************************************************************************
