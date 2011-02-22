//***************************************************************************
//  NARS2000 -- Primitive Function -- DownCaret
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-201 Sudley Place Software

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
#include "headers.h"


#ifndef PROTO
PRIMSPEC PrimSpecDownCaret = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecDownCaretStorageTypeMon, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretAPA_EM, -- Can't happen w/DownCaret

    NULL,   // &PrimFnMonDownCaretBisB, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretBisI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretBisF, -- Can't happen w/DownCaret

////                   IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonDownCaretIisI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretIisF, -- Can't happen w/DownCaret

////                   FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonDownCaretFisI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretFisF, -- Can't happen w/DownCaret

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDownCaretStorageTypeDyd,
    NULL,   // &PrimFnDydDownCaretAPA_EM, -- Can't happen w/DownCaret

    &PrimFnDydDownCaretBisBvB,
    NULL,   // &PrimFnDydDownCaretBisIvI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisFvF, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisCvC, -- Can't happen w/DownCaret

////                   IisBvB,  // Handled via type promotion (to IisIvI)
    &PrimFnDydDownCaretIisIvI,
    NULL,   // &PrimFnDydDownCaretIisFvF, -- Can't happen w/DownCaret

////                   FisBvB,  // Handled via type promotion (to FisIvI)
    &PrimFnDydDownCaretFisIvI,
    &PrimFnDydDownCaretFisFvF,

    NULL,   // &PrimFnMonDownCaretB64isB64, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretB32isB32, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretB16isB16, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretB08isB08, -- Can't happen w/DownCaret

    &PrimFnDydDownCaretB64isB64vB64,
    &PrimFnDydDownCaretB32isB32vB32,
    &PrimFnDydDownCaretB16isB16vB16,
    &PrimFnDydDownCaretB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecDownCaret};
#endif


//***************************************************************************
//  $PrimFnDownCaret_EM_YY
//
//  Primitive function for monadic and dyadic downCaret (ERROR and "or/GCD")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownCaret_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNCARET);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnDownCaret_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecDownCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownCaretStorageTypeDyd
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
} // End PrimSpecDownCaretStorageTypeDyd


//***************************************************************************
//  $gcdAplInt
//
//  GCD (Greatest Common Divisor) for aplIntegers
//***************************************************************************

APLINT gcdAplInt
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplTmp, aplLft, aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileIisI (aplIntegerLft, lpPrimSpec);
    aplRht = PrimFnMonStileIisI (aplIntegerRht, lpPrimSpec);

    while (aplLft NE 0)
    {
        aplTmp = aplLft;
        aplLft = aplRht % aplLft;
        aplRht = aplTmp;
    } // End WHILE

    // The sign of the result is the sign of the left argument
    if (aplIntegerLft < 0)
        return -aplRht;
    else
        return  aplRht;
} // End gcdAplInt


//***************************************************************************
//  $gcdAplFloat
//
//  GCD (Greatest Common Divisor) for aplFloats
//***************************************************************************

APLFLOAT gcdAplFloat
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplTmp, aplLft, aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileFisF (aplFloatLft, lpPrimSpec);
    aplRht = PrimFnMonStileFisF (aplFloatRht, lpPrimSpec);

    while (aplLft >= SYS_CT)
    {
        aplTmp = aplLft;
        aplLft = fmod (aplRht, aplLft);
        aplRht = aplTmp;
    } // End WHILE

    // The sign of the result is the sign of the left argument
    if (aplFloatLft > 0)
        return  aplRht;
    else
        return -aplRht;
} // End gcdAplFloat


//***************************************************************************
//  $PrimFnDydDownCaretBisBvB
//
//  Primitive scalar function dyadic DownCaret:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydDownCaretBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretBisBvB


//***************************************************************************
//  $PrimFnDydDownCaretB64isB64vB64
//
//  Primitive scalar function dyadic DownCaret:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydDownCaretB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretB64isB64vB64


//***************************************************************************
//  $PrimFnDydDownCaretB32isB32vB32
//
//  Primitive scalar function dyadic DownCaret:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydDownCaretB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretB32isB32vB32


//***************************************************************************
//  $PrimFnDydDownCaretB16isB16vB16
//
//  Primitive scalar function dyadic DownCaret:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydDownCaretB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretB16isB16vB16


//***************************************************************************
//  $PrimFnDydDownCaretB08isB08vB08
//
//  Primitive scalar function dyadic DownCaret:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydDownCaretB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretB08isB08vB08


//***************************************************************************
//  $PrimFnDydDownCaretIisIvI
//
//  Primitive scalar function dyadic DownCaret:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydDownCaretIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return gcdAplInt (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydDownCaretIisIvI


//***************************************************************************
//  $PrimFnDydDownCaretFisIvI
//
//  Primitive scalar function dyadic DownCaret:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydDownCaretFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (APLFLOAT) gcdAplInt (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydDownCaretFisIvI


//***************************************************************************
//  $PrimFnDydDownCaretFisFvF
//
//  Primitive scalar function dyadic DownCaret:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydDownCaretFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return gcdAplFloat (aplFloatLft, aplFloatRht, lpPrimSpec);
} // End PrimFnDydDownCaretFisFvF


//***************************************************************************
//  End of File: pf_dcaret.c
//***************************************************************************
