//***************************************************************************
//  NARS2000 -- Primitive Function -- RightCaretUnderbar
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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
PRIMSPEC PrimSpecRightCaretUnderbar = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecRightCaretUnderbarStorageTypeMon, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarAPA_EM, -- Can't happen w/RightCaretUnderbar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonRightCaretUnderbarBisB, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarBisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarBisF, -- Can't happen w/RightCaretUnderbar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonRightCaretUnderbarIisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarIisF, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisI, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisF, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarRisR, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisR, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisV, -- Can't happen w/RightCaretUnderbar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonRightCaretUnderbarHC2IisHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2IisHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2FisHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2FisHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2RisHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2VisHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC2VisHC2V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnMonRightCaretUnderbarHC4IisHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4IisHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4FisHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4FisHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4RisHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4VisHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC4VisHC4V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnMonRightCaretUnderbarHC8IisHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8IisHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8FisHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8FisHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8RisHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8VisHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarHC8VisHC8V, -- Can't happen w/RightCaretUnderbar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC2V, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC4V, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarFisHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarVisHC8V, -- Can't happen w/RightCaretUnderbar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRightCaretUnderbarStorageTypeDyd,
    NULL,   // &PrimFnDydRightCaretUnderbarAPA_EM, -- Can't happen w/RightCaretUnderbar

    // Dyadic Boolean result functions
    &PrimFnDydRightCaretUnderbarBisBvB,
    &PrimFnDydRightCaretUnderbarBisIvI,
    &PrimFnDydRightCaretUnderbarBisFvF,
    NULL,   // &PrimFnDydRightCaretUnderbarBisCvC, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHvH, -- Can't happen w/RightCaretUnderbar
    &PrimFnDydRightCaretUnderbarBisRvR,
    &PrimFnDydRightCaretUnderbarBisVvV,

    NULL,   // &PrimFnDydRightCaretUnderbarBisHC2IvHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC2FvHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC2RvHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC2VvHC2V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnDydRightCaretUnderbarBisHC4IvHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC4FvHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC4RvHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC4VvHC4V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnDydRightCaretUnderbarBisHC8IvHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC8FvHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC8RvHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarBisHC8VvHC8V, -- Can't happen w/RightCaretUnderbar

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydRightCaretUnderbarIisIvI, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarIisFvF, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarFisIvI, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarFisFvF, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarRisRvR, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarVisRvR, -- Result Boolean, can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarVisVvV, -- Result Boolean, can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnDydRightCaretUnderbarHC2IisHC2IvHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2IisHC2FvHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2FisHC2IvHC2I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2FisHC2FvHC2F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2RisHC2RvHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2VisHC2RvHC2R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC2VisHC2VvHC2V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnDydRightCaretUnderbarHC4IisHC4IvHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4IisHC4FvHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4FisHC4IvHC4I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4FisHC4FvHC4F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4RisHC4RvHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4VisHC4RvHC4R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC4VisHC4VvHC4V, -- Can't happen w/RightCaretUnderbar

    NULL,   // &PrimFnDydRightCaretUnderbarHC8IisHC8IvHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8IisHC8FvHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8FisHC8IvHC8I, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8FisHC8FvHC8F, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8RisHC8RvHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8VisHC8RvHC8R, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnDydRightCaretUnderbarHC8VisHC8VvHC8V, -- Can't happen w/RightCaretUnderbar

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonRightCaretUnderbarB64isB64, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarB32isB32, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarB16isB16, -- Can't happen w/RightCaretUnderbar
    NULL,   // &PrimFnMonRightCaretUnderbarB08isB08, -- Can't happen w/RightCaretUnderbar

    // Dyadic Boolean chunk functions
    &PrimFnDydRightCaretUnderbarB64isB64vB64,
    &PrimFnDydRightCaretUnderbarB32isB32vB32,
    &PrimFnDydRightCaretUnderbarB16isB16vB16,
    &PrimFnDydRightCaretUnderbarB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
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
    Assert (IsAPLCharNotLess (lptkFunc->tkData.tkChar));

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
    if (IsCharEmpty (*lpaplTypeLft, aplNELMLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Is either arg HC?
    if (IsHCAny (*lpaplTypeLft)
     || IsHCAny (*lpaplTypeRht))
        // Tell the caller to demote the data to HC dimension 1
        return ARRAY_REALONLY;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // All numeric results are Boolean
    if (IsNumeric (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecRightCaretUnderbarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisBvB
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} B fn B
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean >= lpatRht->aplBoolean)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisBvB


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB64isB64vB64
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydRightCaretUnderbarB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB64isB64vB64


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB32isB32vB32
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydRightCaretUnderbarB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB32isB32vB32


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB16isB16vB16
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydRightCaretUnderbarB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB16isB16vB16


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarB08isB08vB08
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydRightCaretUnderbarB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydRightCaretUnderbarB08isB08vB08


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisIvI
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} I fn I
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisIvI
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplInteger >= lpatRht->aplInteger)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisIvI


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisFvF
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} F fn F
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisFvF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_F (lpatLft->aplFloat, lpatRht->aplFloat, GetQuadCT (), >=)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisFvF


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisRvR
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} R fn R
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisRvR
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_R (lpatLft->aplRat, lpatRht->aplRat, GetQuadCT (), >=)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisRvR


//***************************************************************************
//  $PrimFnDydRightCaretUnderbarBisVvV
//
//  Primitive scalar function dyadic RightCaretUnderbar:  B {is} V fn V
//***************************************************************************

void PrimFnDydRightCaretUnderbarBisVvV
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_V (lpatLft->aplVfp, lpatRht->aplVfp, GetQuadCT (), >=)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretUnderbarBisVvV


//***************************************************************************
//  End of File: pf_rcaretund.c
//***************************************************************************
