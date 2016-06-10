//***************************************************************************
//  NARS2000 -- Primitive Function -- RightCaret
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
PRIMSPEC PrimSpecRightCaret = {
    // Monadic functions
    NULL,   // &PrimFnMon_EM, -- Can't happen with RightCaret
    NULL,   // &PrimSpecRightCaretStorageTypeMon, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretAPA_EM, -- Can't happen w/RightCaret

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonRightCaretBisB, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretBisI, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretBisF, -- Can't happen w/RightCaret

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonRightCaretIisI, -- Can't happend w/RightCaret
    NULL,   // &PrimFnMonRightCaretIisF, -- Can't happend w/RightCaret
    NULL,   // &PrimFnMonRightCaretFisI, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretFisF, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretRisR, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretVisR, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretVisV, -- Can't happen w/RightCaret

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonRightCaretHC2IisHC2I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC2IisHC2F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC2FisHC2I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC2FisHC2F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC2RisHC2R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC2VisHC2R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC2VisHC2V, -- Can't happen w/RightCaret

    NULL,   // &PrimFnMonRightCaretHC4IisHC4I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC4IisHC4F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC4FisHC4I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC4FisHC4F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC4RisHC4R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC4VisHC4R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC4VisHC4V, -- Can't happen w/RightCaret

    NULL,   // &PrimFnMonRightCaretHC8IisHC8I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC8IisHC8F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC8FisHC8I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC8FisHC8F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC8RisHC8R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC8VisHC8R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretHC8VisHC8V, -- Can't happen w/RightCaret

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonRightCaretFisHC2I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretFisHC2F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretVisHC2R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretVisHC2V, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretFisHC4I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretFisHC4F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretVisHC4R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretVisHC4V, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretFisHC8I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretFisHC8F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretVisHC8R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretVisHC8V, -- Can't happen w/RightCaret

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRightCaretStorageTypeDyd,
    NULL,   // &PrimFnDydRightCaretAPA_EM, -- Can't happen w/RightCaret

    // Dyadic Boolean result functions
    &PrimFnDydRightCaretBisBvB,
    &PrimFnDydRightCaretBisIvI,
    &PrimFnDydRightCaretBisFvF,
    NULL,   // &PrimFnDydRightCaretBisCvC, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretBisHvH, -- Can't happen w/RightCaret
    &PrimFnDydRightCaretBisRvR,
    &PrimFnDydRightCaretBisVvV,

    NULL,   // &PrimFnDydRightCaretBisHC2IvHC2I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretBisHC2FvHC2F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretBisHC2RvHC2R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretBisHC2VvHC2V, -- Can't happen w/RightCaret

    NULL,   // &PrimFnDydRightCaretBisHC4IvHC4I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretBisHC4FvHC4F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretBisHC4RvHC4R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretBisHC4VvHC4V, -- Can't happen w/RightCaret

    NULL,   // &PrimFnDydRightCaretBisHC8IvHC8I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretBisHC8FvHC8F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretBisHC8RvHC8R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretBisHC8VvHC8V, -- Can't happen w/RightCaret

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydRightCaretIisIvI, -- Result Boolean, can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretIisFvF, -- Result Boolean, can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretFisIvI, -- Result Boolean, can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretFisFvF, -- Result Boolean, can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretRisRvR, -- Result Boolean, can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretVisRvR, -- Result Boolean, can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretVisVvV, -- Result Boolean, can't happen w/RightCaret

    NULL,   // &PrimFnDydRightCaretHC2IisHC2IvHC2I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC2IisHC2FvHC2F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC2FisHC2IvHC2I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC2FisHC2FvHC2F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC2RisHC2RvHC2R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC2VisHC2RvHC2R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC2VisHC2VvHC2V, -- Can't happen w/RightCaret

    NULL,   // &PrimFnDydRightCaretHC4IisHC4IvHC4I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC4IisHC4FvHC4F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC4FisHC4IvHC4I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC4FisHC4FvHC4F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC4RisHC4RvHC4R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC4VisHC4RvHC4R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC4VisHC4VvHC4V, -- Can't happen w/RightCaret

    NULL,   // &PrimFnDydRightCaretHC8IisHC8IvHC8I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC8IisHC8FvHC8F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC8FisHC8IvHC8I, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC8FisHC8FvHC8F, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC8RisHC8RvHC8R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC8VisHC8RvHC8R, -- Can't happen w/RightCaret
    NULL,   // &PrimFnDydRightCaretHC8VisHC8VvHC8V, -- Can't happen w/RightCaret

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonRightCaretB64isB64, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretB32isB32, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretB16isB16, -- Can't happen w/RightCaret
    NULL,   // &PrimFnMonRightCaretB08isB08, -- Can't happen w/RightCaret

    // Dyadic Boolean chunk functions
    &PrimFnDydRightCaretB64isB64vB64,
    &PrimFnDydRightCaretB32isB32vB32,
    &PrimFnDydRightCaretB16isB16vB16,
    &PrimFnDydRightCaretB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecRightCaret};
#endif


//***************************************************************************
//  $PrimFnRightCaret_EM_YY
//
//  Primitive function for monadic and dyadic RightCaret (ERROR and "less than")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnRightCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnRightCaret_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_RIGHTCARET);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonRightCaret_EM_YY      (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnRightCaret_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonRightCaret_EM_YY
//
//  Primitive function for monadic RightCaret (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonRightCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonRightCaret_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonRightCaret_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecRightCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRightCaretStorageTypeDyd
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
} // End PrimSpecRightCaretStorageTypeDyd


//***************************************************************************
//  $PrimFnDydRightCaretBisBvB
//
//  Primitive scalar function dyadic RightCaret:  B {is} B fn B
//***************************************************************************

void PrimFnDydRightCaretBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean > lpatRht->aplBoolean) << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretBisBvB


//***************************************************************************
//  $PrimFnDydRightCaretB64isB64vB64
//
//  Primitive scalar function dyadic RightCaret:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydRightCaretB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft & ~aplBooleanRht;
} // End PrimFnDydRightCaretB64isB64vB64


//***************************************************************************
//  $PrimFnDydRightCaretB32isB32vB32
//
//  Primitive scalar function dyadic RightCaret:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydRightCaretB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft & ~aplBooleanRht;
} // End PrimFnDydRightCaretB32isB32vB32


//***************************************************************************
//  $PrimFnDydRightCaretB16isB16vB16
//
//  Primitive scalar function dyadic RightCaret:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydRightCaretB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft & ~aplBooleanRht;
} // End PrimFnDydRightCaretB16isB16vB16


//***************************************************************************
//  $PrimFnDydRightCaretB08isB08vB08
//
//  Primitive scalar function dyadic RightCaret:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydRightCaretB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanLft & ~aplBooleanRht;
} // End PrimFnDydRightCaretB08isB08vB08


//***************************************************************************
//  $PrimFnDydRightCaretBisIvI
//
//  Primitive scalar function dyadic RightCaret:  B {is} I fn I
//***************************************************************************

void PrimFnDydRightCaretBisIvI
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplInteger > lpatRht->aplInteger) << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretBisIvI


//***************************************************************************
//  $PrimFnDydRightCaretBisFvF
//
//  Primitive scalar function dyadic RightCaret:  B {is} F fn F
//***************************************************************************

void PrimFnDydRightCaretBisFvF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_F (lpatLft->aplFloat, lpatRht->aplFloat, GetQuadCT (), >)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretBisFvF


//***************************************************************************
//  $PrimFnDydRightCaretBisRvR
//
//  Primitive scalar function dyadic RightCaret:  B {is} R fn R
//***************************************************************************

void PrimFnDydRightCaretBisRvR
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_R (lpatLft->aplRat, lpatRht->aplRat, GetQuadCT (), >)
       << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretBisRvR


//***************************************************************************
//  $PrimFnDydRightCaretBisVvV
//
//  Primitive scalar function dyadic RightCaret:  B {is} V fn V
//***************************************************************************

void PrimFnDydRightCaretBisVvV
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Compare the two VFPs relative to []CT
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_V (lpatLft->aplVfp, lpatRht->aplVfp, GetQuadCT (), >)
       << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRightCaretBisVvV


//***************************************************************************
//  End of File: pf_rcaret.c
//***************************************************************************
