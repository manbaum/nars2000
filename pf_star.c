//***************************************************************************
//  NARS2000 -- Primitive Function -- Star
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
#include <math.h>
#include <float.h>
#include "headers.h"


#ifndef PROTO
PRIMSPEC PrimSpecStar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecStarStorageTypeMon,
    NULL,   // &PrimFnMonStarAPA_EM, -- Can't happen w/Star

    NULL,   // &PrimFnMonStarBisB, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarBisI, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarBisF, -- Can't happen w/Star

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonStarIisI, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarIisF, -- Can't happen w/Star

////               FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonStarFisI,
    &PrimFnMonStarFisF,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecStarStorageTypeDyd,
    NULL,   // &PrimFnDydStarAPA_EM, -- Can't happen w/Star

    &PrimFnDydStarBisBvB,
    NULL,   // &PrimFnDydStarBisIvI, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisFvF, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisCvC, -- Can't happen w/Star

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydStarIisIvI,
    NULL,   // &PrimFnDydStarIisFvF, -- Can't happen w/Star

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydStarFisIvI,
    &PrimFnDydStarFisFvF,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecStar};
#endif


//***************************************************************************
//  $PrimFnStar_EM_YY
//
//  Primitive function for monadic and dyadic Star ("exponential" and "power")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnStar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnStar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_STAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnStar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecStarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStarStorageTypeMon
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
    aplTypeRes = *lpaplTypeRht;

    // Except that BOOL, INT and APA become FLOAT
    if (IsSimpleInt (aplTypeRes))
        aplTypeRes = ARRAY_FLOAT;

    return aplTypeRes;
} // End PrimSpecStarStorageTypeMon


//***************************************************************************
//  $PrimFnMonStarFisI
//
//  Primitive scalar function monadic Star:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonStarFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return exp ((APLFLOAT) aplIntegerRht);
} // End PrimFnMonStarFisI


//***************************************************************************
//  $PrimFnMonStarFisF
//
//  Primitive scalar function monadic Star:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonStarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return exp (aplFloatRht);
} // End PrimFnMonStarFisF


//***************************************************************************
//  $PrimSpecStarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStarStorageTypeDyd
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
    return aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);
} // End PrimSpecStarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydStarBisBvB
//
//  Primitive scalar function dyadic Star:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydStarBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes = TranslateQuadICIndex (ICNDX_0EXP0);

        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0;
        else
        if (aplFloatRes EQ 1)
            return 1;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return (aplBooleanLft >= aplBooleanRht);
} // End PrimFnDydStarBisBvB


//***************************************************************************
//  $PrimFnDydStarIisIvI
//
//  Primitive scalar function dyadic Star:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydStarIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplIntegerRes;

    // Check for indeterminates:  0 * 0
    if (aplIntegerLft EQ 0
     && aplIntegerRht EQ 0)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    // Check for special cases:  (|1) * R
    if (abs64 (aplIntegerLft) EQ 1)
    {
        // If the right arg is odd, return the left arg (1 or -1)
        if (abs64 (aplIntegerRht) % 2)
            return aplIntegerLft;
        else
        // Otherwise return 1.
            return 1;
    } // End IF

    // Check for special cases:  L * R for R < 0 or R > 62
    if (aplIntegerRht < 0
     || aplIntegerRht > 62)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    // Initialize with identity element for multiplication
    aplIntegerRes = 1;
    while (aplIntegerRht--)
        aplIntegerRes = imul64 (aplIntegerRes, aplIntegerLft);

    return aplIntegerRes;
} // End PrimFnDydStarIisIvI


//***************************************************************************
//  $PrimFnDydStarFisIvI
//
//  Primitive scalar function dyadic Star:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydStarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  0 * 0
    if (aplIntegerLft EQ 0
     && aplIntegerRht EQ 0)
        return TranslateQuadICIndex (ICNDX_0EXP0);

    return pow ((APLFLOAT) aplIntegerLft, (APLFLOAT) aplIntegerRht);
} // End PrimFnDydStarFisIvI


//***************************************************************************
//  $PrimFnDydStarFisFvF
//
//  Primitive scalar function dyadic Star:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydStarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplFloatLft EQ 0
     && aplFloatRht EQ 0)
        return TranslateQuadICIndex (ICNDX_0EXP0);

    // Check for indeterminates:  L * _ for L <= -1
    if (aplFloatLft <= -1
     && aplFloatRht EQ PosInfinity)
        return TranslateQuadICIndex (ICNDX_NEXPPi);

    // Check for special cases:  1 * _ and 1 * -_
    if (aplFloatLft EQ 1
     &&  !_finite (aplFloatRht))
        return aplFloatLft;

    // Calculate the power
    aplFloatRes = pow (aplFloatLft, aplFloatRht);

    // Check for ± infinity result with both args finite
    if (!_finite (aplFloatRes)
     &&  _finite (aplFloatLft)
     &&  _finite (aplFloatRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return aplFloatRes;
} // End PrimFnDydStarFisFvF


//***************************************************************************
//  End of File: pf_star.c
//***************************************************************************
