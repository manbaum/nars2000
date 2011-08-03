//***************************************************************************
//  NARS2000 -- Primitive Function -- Root
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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
#include "headers.h"
#include <math.h>
#include <float.h>


#ifndef PROTO
PRIMSPEC PrimSpecRoot = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecRootStorageTypeMon,
    NULL,   // &PrimFnMonRootAPA_EM, -- Can't happen w/Root

    &PrimFnMonRootBisB,
    NULL,   // &PrimFnMonRootBisI, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootBisF, -- Can't happen w/Root

////               IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonRootIisI, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootIisF, -- Can't happen w/Root

////               FisB,   // Handled via type promotion (to FisI)
    &PrimFnMonRootFisI,
    &PrimFnMonRootFisF,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRootStorageTypeDyd,
    NULL,   // &PrimFnDydRootAPA_EM, -- Can't happen w/Root

    &PrimFnDydRootBisBvB,
    NULL,   // &PrimFnDydRootBisIvI, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisFvF, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisCvC, -- Can't happen w/Root

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydRootIisIvI,
    NULL,   // &PrimFnDydRootIisFvF, -- Can't happen w/Root

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydRootFisIvI,
    &PrimFnDydRootFisFvF,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecRoot};
#endif


//***************************************************************************
//  $PrimFnRoot_EM_YY
//
//  Primitive function for monadic and dyadic Root ("Square Root" and "Nth Root")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnRoot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnRoot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_ROOT);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnRoot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecRootStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRootStorageTypeMon
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

    // Except that APA and INT become FLOAT
    if (IsSimpleAPA (aplTypeRes)
     || IsSimpleInt (aplTypeRes))
        return ARRAY_FLOAT;

    return aplTypeRes;
} // End PrimSpecRootStorageTypeMon


//***************************************************************************
//  $PrimFnMonRootBisB
//
//  Primitive scalar function monadic Root:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonRootBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonRootBisB


//***************************************************************************
//  $PrimFnMonRootFisI
//
//  Primitive scalar function monadic Root:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonRootFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (aplIntegerRht < 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return sqrt ((APLFLOAT) aplIntegerRht);
} // End PrimFnMonRootFisI


//***************************************************************************
//  $PrimFnMonRootFisF
//
//  Primitive scalar function monadic Root:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonRootFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    if (aplFloatRht < 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return sqrt (aplFloatRht);
} // End PrimFnMonRootFisF


//***************************************************************************
//  $PrimSpecRootStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRootStorageTypeDyd
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
} // End PrimSpecRootStorageTypeDyd


//***************************************************************************
//  $PrimFnDydRootBisBvB
//
//  Primitive scalar function dyadic Root:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydRootBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 {root} 0 <==> 0 * _
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes = TranslateQuadICIndex (ICNDX_0EXPPi);

        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0;
        else
        if (aplFloatRes EQ 1)
            return 1;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (aplBooleanLft EQ 1)
        return aplBooleanRht;
    else
        // It's 0 {root} 1 <==> 1 * _ <==> 1
        return 1;
} // End PrimFnDydRootBisBvB


//***************************************************************************
//  $PrimFnDydRootIisIvI
//
//  Primitive scalar function dyadic Root:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydRootIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (aplIntegerLft EQ 1)
        return aplIntegerRht;

    // Check for indeterminates and special cases:  0 {root} R
    if (aplIntegerLft EQ 0)
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (aplIntegerRht <= -1)
            aplFloatRes = TranslateQuadICIndex (ICNDX_NEXPPi);
        else
////////// Check for special case:  0 {root} R where -1 < R < 0 <==> 0
////////if (-1 < aplIntegerRht
//////// &&      aplIntegerRht < 0)
////////    return 0;
////////else
        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (aplIntegerRht EQ 0)
            aplFloatRes = TranslateQuadICIndex (ICNDX_0EXPPi);
        else
////////// Check for special case:  0 {root} R where 0 < R < 1
////////if (0 < aplIntegerRht
//////// &&     aplIntegerRht < 1)
////////    return 0;
////////else
////////// Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
////////if (aplIntegerRht > 1)
////////    RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
////////else
            aplFloatRes = 2;        // Any non-Boolean value

        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0;
        else
        if (aplFloatRes EQ 1)
            return 1;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    // Otherwise, let the FisFvF code handle it
    RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return 0;           // To keep the compiler happy
} // End PrimFnDydRootIisIvI


//***************************************************************************
//  $PrimFnDydRootFisIvI
//
//  Primitive scalar function dyadic Root:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydRootFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * _
    if (aplIntegerLft EQ 0
     && aplIntegerRht EQ 0)
        return TranslateQuadICIndex (ICNDX_0EXPPi);

    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (aplIntegerLft EQ 1)
        return (APLFLOAT) aplIntegerRht;

    // Check for indeterminates and special cases:  0 {root} R
    if (aplIntegerLft EQ 0)
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (aplIntegerRht <= -1)
            return TranslateQuadICIndex (ICNDX_NEXPPi);

////////// Check for special case:  0 {root} R where -1 < R < 0 <==> 0
////////if (-1 < aplIntegerRht
//////// &&      aplIntegerRht < 0)
////////    return 0;

        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (aplIntegerRht EQ 0)
            return TranslateQuadICIndex (ICNDX_0EXPPi);

////////// Check for special case:  0 {root} R where 0 < R < 1
////////if (0 < aplIntegerRht
//////// &&     aplIntegerRht < 1)
////////    return 0;

        // Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
        if (aplIntegerRht > 1)
            return PosInfinity;
    } // End IF

////// Check for indeterminate:  ±_ {root} 0 <==> 0 * 0
////if (!_finite (aplIntegerLft)
//// && aplIntegerRht EQ 0)
////    return TranslateQuadICIndex (ICNDX_0EXP0);
    if (aplIntegerRht < 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Calculate the root
    aplFloatRes = pow ((APLFLOAT) aplIntegerRht, 1 / (APLFLOAT) aplIntegerLft);

////// Check for ± infinity result with both args finite
////if (!_finite (aplFloatRes)
//// && !_finite (aplIntegerLft)
//// &&  _finite (aplIntegerRht))
////    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return aplFloatRes;
} // End PrimFnDydRootFisIvI


//***************************************************************************
//  $PrimFnDydRootFisFvF
//
//  Primitive scalar function dyadic Root:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydRootFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * _
    if (aplFloatLft EQ 0
     && aplFloatRht EQ 0)
        return TranslateQuadICIndex (ICNDX_0EXPPi);

    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (aplFloatLft EQ 1)
        return aplFloatRht;

    // Check for indeterminates and special cases:  0 {root} R
    if (aplFloatLft EQ 0)
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (aplFloatRht <= -1)
            return TranslateQuadICIndex (ICNDX_NEXPPi);

        // Check for special case:  0 {root} R where -1 < R < 0 <==> 0
        if (-1 < aplFloatRht
         &&      aplFloatRht < 0)
            return 0;

        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (aplFloatRht EQ 0)
            return TranslateQuadICIndex (ICNDX_0EXPPi);

        // Check for special case:  0 {root} R where 0 < R < 1
        if (0 < aplFloatRht
         &&     aplFloatRht < 1)
            return 0;

        // Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
        if (aplFloatRht > 1)
            return PosInfinity;
    } // End IF

    // Check for indeterminate:  ±_ {root} 0 <==> 0 * 0
    if (!_finite (aplFloatLft)
     && aplFloatRht EQ 0)
        return TranslateQuadICIndex (ICNDX_0EXP0);

    if (aplFloatRht < 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Calculate the root
    aplFloatRes = pow (aplFloatRht, 1 / aplFloatLft);

    // Check for ± infinity result with both pow args finite
    if (!_finite (aplFloatRes)
     && !_finite (aplFloatLft)
     &&  _finite (aplFloatRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return aplFloatRes;
} // End PrimFnDydRootFisFvF


//***************************************************************************
//  End of File: pf_root.c
//***************************************************************************
