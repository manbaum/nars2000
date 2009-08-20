//***************************************************************************
//  NARS2000 -- Primitive Function -- ColonBar
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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
#include "headers.h"


#ifndef PROTO
PRIMSPEC PrimSpecColonBar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecColonBarStorageTypeMon,
    NULL,   // &PrimFnMonColonBarAPA_EM, -- Can't happen w/ColonBar

    NULL,   // &PrimFnMonColonBarBisB, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarBisI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarBisF, -- Can't happen w/ColonBar

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonColonBarIisI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarIisF, -- Can't happen w/ColonBar

////               FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonColonBarFisI,
    &PrimFnMonColonBarFisF,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecColonBarStorageTypeDyd,
    NULL,   // &PrimFnDydColonBarAPA_EM, -- Can't happen w/ColonBar

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydColonBarBisIvI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisFvF, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisCvC, -- Can't happen w/ColonBar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydColonBarIisIvI,
    NULL,   // &PrimFnDydColonBarIisFvF, -- Can't happen w/ColonBar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydColonBarFisIvI,
    &PrimFnDydColonBarFisFvF,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecColonBar};
#endif


//***************************************************************************
//  $PrimFnColonBar_EM_YY
//
//  Primitive function for monadic and dyadic ColonBar ("reciprocal" and "division")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnColonBar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnColonBar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_COLONBAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnColonBar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecColonBarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecColonBarStorageTypeMon
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
        return ARRAY_FLOAT;

    return aplTypeRes;
} // End PrimSpecColonBarStorageTypeMon


//***************************************************************************
//  $PrimFnMonColonBarFisI
//
//  Primitive scalar function monadic ColonBar:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonColonBarFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  {div} 0
    if (aplIntegerRht EQ 0)
        return TranslateQuadICIndex (ICNDX_DIV0);

    // The FPU handles overflow and underflow for us
    return (1 / (APLFLOAT) aplIntegerRht);
} // End PrimFnMonColonBarFisI


//***************************************************************************
//  $PrimFnMonColonBarFisF
//
//  Primitive scalar function monadic ColonBar:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonColonBarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  {div} 0
    if (aplFloatRht EQ 0)
        return TranslateQuadICIndex (ICNDX_DIV0);

    // If the arg is ± infinity, just return 0.
    // If we don't, then the reciprocal of {neg}infinity
    //   is {neg}0.
    if (!_finite (aplFloatRht))
        return 0;

    // The FPU handles overflow and underflow for us
    return (1 / aplFloatRht);
} // End PrimFnMonColonBarFisF


//***************************************************************************
//  $PrimSpecColonBarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecColonBarStorageTypeDyd
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

    // Except that BOOL, INT, and APA become INT
    // If a result doesn't fit in an INT, blow up to FLOAT.
    if (IsSimpleInt (aplTypeRes))
        return ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecColonBarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydColonBarIisIvI
//
//  Primitive scalar function dyadic ColonBar:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydColonBarIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplIntegerRes;

    // Check for indeterminates:  0 {div} 0
    if (aplIntegerLft EQ 0
     || aplIntegerRht EQ 0)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    // Try integer division first
    aplIntegerRes = aplIntegerLft / aplIntegerRht;

    // See if the result is integral
    if (aplIntegerLft NE (aplIntegerRes * aplIntegerRht))
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return aplIntegerRes;
} // End PrimFnDydColonBarIisIvI


//***************************************************************************
//  $PrimFnDydColonBarFisIvI
//
//  Primitive scalar function dyadic ColonBar:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydColonBarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  0 {div} 0
    if (aplIntegerLft EQ 0
     && aplIntegerRht EQ 0)
        return TranslateQuadICIndex (ICNDX_0DIV0);

    // The FPU handles overflow and underflow for us
    return (((APLFLOAT) aplIntegerLft) / (APLFLOAT) aplIntegerRht);
} // End PrimFnDydColonBarFisIvI


//***************************************************************************
//  $PrimFnDydColonBarFisFvF
//
//  Primitive scalar function dyadic ColonBar:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydColonBarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  0 {div} 0
    if (aplFloatLft EQ 0
     && aplFloatRht EQ 0)
        return TranslateQuadICIndex (ICNDX_0DIV0);

    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (!_finite (aplFloatLft)
     && !_finite (aplFloatRht))
    {
        if (SIGN_APLFLOAT (aplFloatLft) EQ SIGN_APLFLOAT (aplFloatRht))
            return TranslateQuadICIndex (ICNDX_PiDIVPi);
        else
            return TranslateQuadICIndex (ICNDX_NiDIVPi);
    } // End IF

    // The FPU handles overflow and underflow for us
    return (aplFloatLft / aplFloatRht);
} // End PrimFnDydColonBarFisFvF


//***************************************************************************
//  End of File: pf_colonbar.c
//***************************************************************************
