//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftCaretUnderbar
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
PRIMSPEC PrimSpecLeftCaretUnderbar = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecLeftCaretUnderbarStorageTypeMon, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarAPA_EM, -- Can't happen w/LeftCaretUnderbar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonLeftCaretUnderbarBisB, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarBisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarBisF, -- Can't happen w/LeftCaretUnderbar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonLeftCaretUnderbarIisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarIisF, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisI, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisF, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarRisR, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisR, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisV, -- Can't happen w/LeftCaretUnderbar

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2IisHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2IisHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2FisHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2FisHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2RisHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2VisHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC2VisHC2V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnMonLeftCaretUnderbarHC4IisHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4IisHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4FisHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4FisHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4RisHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4VisHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC4VisHC4V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnMonLeftCaretUnderbarHC8IisHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8IisHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8FisHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8FisHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8RisHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8VisHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarHC8VisHC8V, -- Can't happen w/LeftCaretUnderbar

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC2V, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC4V, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarFisHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarVisHC8V, -- Can't happen w/LeftCaretUnderbar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecLeftCaretUnderbarStorageTypeDyd,
    NULL,   // &PrimFnDydLeftCaretUnderbarAPA_EM, -- Can't happen w/LeftCaretUnderbar

    // Dyadic Boolean result functions
    &PrimFnDydLeftCaretUnderbarBisBvB,
    &PrimFnDydLeftCaretUnderbarBisIvI,
    &PrimFnDydLeftCaretUnderbarBisFvF,
    NULL,   // &PrimFnDydLeftCaretUnderbarBisCvC, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHvH, -- Can't happen w/LeftCaretUnderbar
    &PrimFnDydLeftCaretUnderbarBisRvR,
    &PrimFnDydLeftCaretUnderbarBisVvV,

    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC2IvHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC2FvHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC2RvHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC2VvHC2V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC4IvHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC4FvHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC4RvHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC4VvHC4V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC8IvHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC8FvHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC8RvHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarBisHC8VvHC8V, -- Can't happen w/LeftCaretUnderbar

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydLeftCaretUnderbarIisIvI, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarIisFvF, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarFisIvI, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarFisFvF, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarRisRvR, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarVisRvR, -- Result Boolean, can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarVisVvV, -- Result Boolean, can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnDydLeftCaretUnderbarHC2IisHC2IvHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2IisHC2FvHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2FisHC2IvHC2I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2FisHC2FvHC2F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2RisHC2RvHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2VisHC2RvHC2R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC2VisHC2VvHC2V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnDydLeftCaretUnderbarHC4IisHC4IvHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4IisHC4FvHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4FisHC4IvHC4I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4FisHC4FvHC4F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4RisHC4RvHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4VisHC4RvHC4R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC4VisHC4VvHC4V, -- Can't happen w/LeftCaretUnderbar

    NULL,   // &PrimFnDydLeftCaretUnderbarHC8IisHC8IvHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8IisHC8FvHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8FisHC8IvHC8I, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8FisHC8FvHC8F, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8RisHC8RvHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8VisHC8RvHC8R, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnDydLeftCaretUnderbarHC8VisHC8VvHC8V, -- Can't happen w/LeftCaretUnderbar

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonLeftCaretUnderbarB64isB64, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarB32isB32, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarB16isB16, -- Can't happen w/LeftCaretUnderbar
    NULL,   // &PrimFnMonLeftCaretUnderbarB08isB08, -- Can't happen w/LeftCaretUnderbar

    // Dyadic Boolean chunk functions
    &PrimFnDydLeftCaretUnderbarB64isB64vB64,
    &PrimFnDydLeftCaretUnderbarB32isB32vB32,
    &PrimFnDydLeftCaretUnderbarB16isB16vB16,
    &PrimFnDydLeftCaretUnderbarB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecLeftCaretUnderbar};
#endif


//***************************************************************************
//  $PrimFnLeftCaretUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic LeftCaretUnderbar (ERROR and "less than or equal")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnLeftCaretUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnLeftCaretUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharNotMore (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnLeftCaretUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecLeftCaretUnderbarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecLeftCaretUnderbarStorageTypeDyd
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
} // End PrimSpecLeftCaretUnderbarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisBvB
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} B fn B
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean <= lpatRht->aplBoolean)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisBvB


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB64isB64vB64
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydLeftCaretUnderbarB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB64isB64vB64


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB32isB32vB32
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydLeftCaretUnderbarB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB32isB32vB32


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB16isB16vB16
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydLeftCaretUnderbarB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB16isB16vB16


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarB08isB08vB08
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydLeftCaretUnderbarB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & ~aplBooleanRht);
} // End PrimFnDydLeftCaretUnderbarB08isB08vB08


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisIvI
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} I fn I
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisIvI
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplInteger <= lpatRht->aplInteger)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisIvI


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisFvF
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} F fn F
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisFvF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_F (lpatLft->aplFloat, lpatRht->aplFloat, GetQuadCT (), <=)
    << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisFvF


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisRvR
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} R fn R
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisRvR
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_R (lpatLft->aplRat, lpatRht->aplRat, GetQuadCT (), <=)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisRvR


//***************************************************************************
//  $PrimFnDydLeftCaretUnderbarBisVvV
//
//  Primitive scalar function dyadic LeftCaretUnderbar:  B {is} V fn V
//***************************************************************************

void PrimFnDydLeftCaretUnderbarBisVvV
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_V (lpatLft->aplVfp, lpatRht->aplVfp, GetQuadCT (), <=)
       << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretUnderbarBisVvV


//***************************************************************************
//  End of File: pf_lcaretund.c
//***************************************************************************
