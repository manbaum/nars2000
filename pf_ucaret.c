//***************************************************************************
//  NARS2000 -- Primitive Function -- UpCaret
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
#include <math.h>
#include "headers.h"


#ifndef PROTO
PRIMSPEC PrimSpecUpCaret = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecUpCaretStorageTypeMon, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretAPA_EM, -- Can't happen w/UpCaret

    NULL,   // &PrimFnMonUpCaretBisB, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretBisI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretBisF, -- Can't happen w/UpCaret

////                 IisB,   // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonUpCaretIisI, -- Can't happend w/UpCaret
    NULL,   // &PrimFnMonUpCaretIisF, -- Can't happend w/UpCaret

////                 FisB,   // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonUpCaretFisI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisF, -- Can't happen w/UpCaret

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpCaretStorageTypeDyd,
    NULL,   // &PrimFnDydUpCaretAPA_EM, -- Can't happen w/UpCaret

    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydUpCaretBisIvI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisFvF, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisCvC, -- Can't happen w/UpCaret

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    &PrimFnDydUpCaretIisIvI,
    NULL,   // &PrimFnDydUpCaretIisFvF, -- Can't happen w/UpCaret

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydUpCaretFisIvI,
    &PrimFnDydUpCaretFisFvF,

    NULL,   // &PrimFnMonUpCaretB64isB64, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretB32isB32, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretB16isB16, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretB08isB08, -- Can't happen w/UpCaret

    &PrimFnDydUpCaretB64isB64vB64,
    &PrimFnDydUpCaretB32isB32vB32,
    &PrimFnDydUpCaretB16isB16vB16,
    &PrimFnDydUpCaretB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpCaret};
#endif


//***************************************************************************
//  $PrimFnUpCaret_EM_YY
//
//  Primitive function for monadic and dyadic UpCaret (ERROR and "and/LCM")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpCaret_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPCARET
         || lptkFunc->tkData.tkChar EQ UTF16_CIRCUMFLEX);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpCaret_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecUpCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpCaretStorageTypeDyd
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
} // End PrimSpecUpCaretStorageTypeDyd


//***************************************************************************
//  $lcmAplInt
//
//  LCM (Least Common Multiple) for aplIntegers
//***************************************************************************

APLINT lcmAplInt
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLINT aplTmp, aplLft, aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileIisI (aplIntegerLft, lpPrimSpec);
    aplRht = PrimFnMonStileIisI (aplIntegerRht, lpPrimSpec);

    // Calculate the GCD
    aplTmp = gcdAplInt (aplLft, aplRht, lpPrimSpec);
    if (aplTmp EQ 0)
        return aplTmp;

    aplTmp = aplLft * (aplRht / aplTmp);

    // The sign of the result is the sign of the left argument
    if (aplIntegerLft < 0)
        return -aplTmp;
    else
        return  aplTmp;
} // End lcmAplInt


//***************************************************************************
//  $lcmAplFloat
//
//  LCM (Least Common Multiple) for aplFloats
//***************************************************************************

APLFLOAT lcmAplFloat
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplTmp, aplLft, aplRht;

    // Ensure both arguments are non-negative
    aplLft = PrimFnMonStileFisF (aplFloatLft, lpPrimSpec);
    aplRht = PrimFnMonStileFisF (aplFloatRht, lpPrimSpec);

    // Calculate the GCD
    aplTmp = gcdAplFloat (aplLft, aplRht, lpPrimSpec);
    if (fabs (aplTmp) < SYS_CT)
        return aplTmp;

    aplTmp = aplLft * (aplRht / aplTmp);

    // The sign of the result is the sign of the left argument
    if (aplFloatLft < 0)
        return -aplTmp;
    else
        return  aplTmp;
} // End lcmAplFloat


//***************************************************************************
//  $PrimFnDydUpCaretBisBvB
//
//  Primitive scalar function dyadic UpCaret:  B {is} B fn B
//***************************************************************************

APLBOOL PrimFnDydUpCaretBisBvB
    (APLBOOL    aplBooleanLft,
     APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretBisBvB


//***************************************************************************
//  $PrimFnDydUpCaretB64isB64vB64
//
//  Primitive scalar function dyadic UpCaret:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydUpCaretB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB64isB64vB64


//***************************************************************************
//  $PrimFnDydUpCaretB32isB32vB32
//
//  Primitive scalar function dyadic UpCaret:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydUpCaretB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB32isB32vB32


//***************************************************************************
//  $PrimFnDydUpCaretB16isB16vB16
//
//  Primitive scalar function dyadic UpCaret:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydUpCaretB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB16isB16vB16


//***************************************************************************
//  $PrimFnDydUpCaretB08isB08vB08
//
//  Primitive scalar function dyadic UpCaret:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydUpCaretB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB08isB08vB08


//***************************************************************************
//  $PrimFnDydUpCaretIisIvI
//
//  Primitive scalar function dyadic UpCaret:  I {is} I fn I
//***************************************************************************

APLINT PrimFnDydUpCaretIisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return lcmAplInt (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydUpCaretIisIvI


//***************************************************************************
//  $PrimFnDydUpCaretFisIvI
//
//  Primitive scalar function dyadic UpCaret:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydUpCaretFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (APLFLOAT) lcmAplInt (aplIntegerLft, aplIntegerRht, lpPrimSpec);
} // End PrimFnDydUpCaretFisIvI


//***************************************************************************
//  $PrimFnDydUpCaretFisFvF
//
//  Primitive scalar function dyadic UpCaret:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydUpCaretFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    return lcmAplFloat (aplFloatLft, aplFloatRht, lpPrimSpec);
} // End PrimFnDydUpCaretFisFvF


//***************************************************************************
//  End of File: pf_ucaret.c
//***************************************************************************
