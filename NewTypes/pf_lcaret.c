//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftCaret
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

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
PRIMSPEC PrimSpecLeftCaret = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecLeftCaretStorageTypeMon, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretAPA_EM, -- Can't happen w/LeftCaret

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonLeftCaretBisB, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretBisI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretBisF, -- Can't happen w/LeftCaret

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonLeftCaretIisI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretIisF, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretFisI, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretFisF, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretRisR, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretVisR, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretVisV, -- Can't happen w/LeftCaret

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonLeftCaretHC2IisHC2I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC2IisHC2F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC2FisHC2I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC2FisHC2F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC2RisHC2R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC2VisHC2R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC2VisHC2V, -- Can't happen w/LeftCaret

    NULL,   // &PrimFnMonLeftCaretHC4IisHC4I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC4IisHC4F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC4FisHC4I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC4FisHC4F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC4RisHC4R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC4VisHC4R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC4VisHC4V, -- Can't happen w/LeftCaret

    NULL,   // &PrimFnMonLeftCaretHC8IisHC8I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC8IisHC8F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC8FisHC8I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC8FisHC8F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC8RisHC8R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC8VisHC8R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretHC8VisHC8V, -- Can't happen w/LeftCaret

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonLeftCaretFisHC2I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretFisHC2F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretVisHC2R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretVisHC2V, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretFisHC4I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretFisHC4F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretVisHC4R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretVisHC4V, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretFisHC8I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretFisHC8F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretVisHC8R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretVisHC8V, -- Can't happen w/LeftCaret

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecLeftCaretStorageTypeDyd,
    NULL,   // &PrimFnDydLeftCaretAPA_EM, -- Can't happen w/LeftCaret

    // Dyadic Boolean result functions
    &PrimFnDydLeftCaretBisBvB,
    &PrimFnDydLeftCaretBisIvI,
    &PrimFnDydLeftCaretBisFvF,
    NULL,   // &PrimFnDydLeftCaretBisCvC, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretBisHvH, -- Can't happen w/LeftCaret
    &PrimFnDydLeftCaretBisRvR,
    &PrimFnDydLeftCaretBisVvV,

    NULL,   // &PrimFnDydLeftCaretBisHC2IvHC2I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretBisHC2FvHC2F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretBisHC2RvHC2R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretBisHC2VvHC2V, -- Can't happen w/LeftCaret

    NULL,   // &PrimFnDydLeftCaretBisHC4IvHC4I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretBisHC4FvHC4F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretBisHC4RvHC4R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretBisHC4VvHC4V, -- Can't happen w/LeftCaret

    NULL,   // &PrimFnDydLeftCaretBisHC8IvHC8I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretBisHC8FvHC8F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretBisHC8RvHC8R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretBisHC8VvHC8V, -- Can't happen w/LeftCaret

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydLeftCaretIisIvI, -- Result Boolean, can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretIisFvF, -- Result Boolean, can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretFisIvI, -- Result Boolean, can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretFisFvF, -- Result Boolean, can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretRisRvR, -- Result Boolean, can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretVisRvR, -- Result Boolean, can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretVisVvV, -- Result Boolean, can't happen w/LeftCaret

    NULL,   // &PrimFnDydLeftCaretHC2IisHC2IvHC2I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC2IisHC2FvHC2F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC2FisHC2IvHC2I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC2FisHC2FvHC2F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC2RisHC2RvHC2R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC2VisHC2RvHC2R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC2VisHC2VvHC2V, -- Can't happen w/LeftCaret

    NULL,   // &PrimFnDydLeftCaretHC4IisHC4IvHC4I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC4IisHC4FvHC4F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC4FisHC4IvHC4I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC4FisHC4FvHC4F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC4RisHC4RvHC4R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC4VisHC4RvHC4R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC4VisHC4VvHC4V, -- Can't happen w/LeftCaret

    NULL,   // &PrimFnDydLeftCaretHC8IisHC8IvHC8I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC8IisHC8FvHC8F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC8FisHC8IvHC8I, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC8FisHC8FvHC8F, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC8RisHC8RvHC8R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC8VisHC8RvHC8R, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnDydLeftCaretHC8VisHC8VvHC8V, -- Can't happen w/LeftCaret

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonLeftCaretB64isB64, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretB32isB32, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretB16isB16, -- Can't happen w/LeftCaret
    NULL,   // &PrimFnMonLeftCaretB08isB08, -- Can't happen w/LeftCaret

    // Dyadic Boolean chunk functions
    &PrimFnDydLeftCaretB64isB64vB64,
    &PrimFnDydLeftCaretB32isB32vB32,
    &PrimFnDydLeftCaretB16isB16vB16,
    &PrimFnDydLeftCaretB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecLeftCaret};
#endif


//***************************************************************************
//  $PrimFnLeftCaret_EM_YY
//
//  Primitive function for monadic and dyadic LeftCaret (ERROR and "greater than")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnLeftCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnLeftCaret_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_LEFTCARET);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnLeftCaret_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecLeftCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecLeftCaretStorageTypeDyd
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
        return ARRAY_ERROR;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // All numeric results are Boolean
    if (IsNumeric (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecLeftCaretStorageTypeDyd


//***************************************************************************
//  $PrimFnDydLeftCaretBisBvB
//
//  Primitive scalar function dyadic LeftCaret:  B {is} B fn B
//***************************************************************************

void PrimFnDydLeftCaretBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean < lpatRht->aplBoolean) << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretBisBvB


//***************************************************************************
//  $PrimFnDydLeftCaretB64isB64vB64
//
//  Primitive scalar function dyadic LeftCaret:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydLeftCaretB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (~aplBooleanLft) & aplBooleanRht;
} // End PrimFnDydLeftCaretB64isB64vB64


//***************************************************************************
//  $PrimFnDydLeftCaretB32isB32vB32
//
//  Primitive scalar function dyadic LeftCaret:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydLeftCaretB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (~aplBooleanLft) & aplBooleanRht;
} // End PrimFnDydLeftCaretB32isB32vB32


//***************************************************************************
//  $PrimFnDydLeftCaretB16isB16vB16
//
//  Primitive scalar function dyadic LeftCaret:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydLeftCaretB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (~aplBooleanLft) & aplBooleanRht;
} // End PrimFnDydLeftCaretB16isB16vB16


//***************************************************************************
//  $PrimFnDydLeftCaretB08isB08vB08
//
//  Primitive scalar function dyadic LeftCaret:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydLeftCaretB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (~aplBooleanLft) & aplBooleanRht;
} // End PrimFnDydLeftCaretB08isB08vB08


//***************************************************************************
//  $PrimFnDydLeftCaretBisIvI
//
//  Primitive scalar function dyadic LeftCaret:  B {is} I fn I
//***************************************************************************

void PrimFnDydLeftCaretBisIvI
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplInteger < lpatRht->aplInteger) << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretBisIvI


//***************************************************************************
//  $PrimFnDydLeftCaretBisFvF
//
//  Primitive scalar function dyadic LeftCaret:  B {is} F fn F
//***************************************************************************

void PrimFnDydLeftCaretBisFvF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_F (lpatLft->aplFloat, lpatRht->aplFloat, GetQuadCT (), <)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretBisFvF


//***************************************************************************
//  $PrimFnDydLeftCaretBisRvR
//
//  Primitive scalar function dyadic LeftCaret:  B {is} R fn R
//***************************************************************************

void PrimFnDydLeftCaretBisRvR
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_R (lpatLft->aplRat, lpatRht->aplRat, GetQuadCT (), <)
       << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretBisRvR


//***************************************************************************
//  $PrimFnDydLeftCaretBisVvV
//
//  Primitive scalar function dyadic LeftCaret:  B {is} V fn V
//***************************************************************************

void PrimFnDydLeftCaretBisVvV
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Compare the two VFPs relative to []CT
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_V (lpatLft->aplVfp, lpatRht->aplVfp, GetQuadCT (), <)
       << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydLeftCaretBisVvV


//***************************************************************************
//  End of File: pf_lcaret.c
//***************************************************************************
