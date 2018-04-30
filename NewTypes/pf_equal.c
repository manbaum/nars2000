//***************************************************************************
//  NARS2000 -- Primitive Function -- Equal
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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


#ifndef PROTO
PRIMSPEC PrimSpecEqual = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecEqualStorageTypeMon, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualAPA_EM, -- No monadic defn, can't happen w/Equal

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonEqualBisB, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualBisI, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualBisF, -- No monadic defn, can't happen w/Equal

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonEqualIisI, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualIisF, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualFisI, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualFisF, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualRisR, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualVisR, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualVisV, -- No monadic defn, can't happen w/Equal

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonEqualHC2IisHC2I, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC2IisHC2F, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC2FisHC2I, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC2FisHC2F, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC2RisHC2R, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC2VisHC2R, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC2VisHC2V, -- No monadic defn, can't happen w/Equal

    NULL,   // &PrimFnMonEqualHC4IisHC4I, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC4IisHC4F, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC4FisHC4I, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC4FisHC4F, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC4RisHC4R, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC4VisHC4R, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC4VisHC4V, -- No monadic defn, can't happen w/Equal

    NULL,   // &PrimFnMonEqualHC8IisHC8I, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC8IisHC8F, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC8FisHC8I, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC8FisHC8F, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC8RisHC8R, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC8VisHC8R, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualHC8VisHC8V, -- No monadic defn, can't happen w/Equal

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonEqualFisHC2I, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualFisHC2F, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualVisHC2R, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualVisHC2V, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualFisHC4I, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualFisHC4F, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualVisHC4R, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualVisHC4V, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualFisHC8I, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualFisHC8F, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualVisHC8R, -- Can't happen w/Equal
    NULL,   // &PrimFnMonEqualVisHC8V, -- Can't happen w/Equal

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecEqualStorageTypeDyd,
    NULL,   // &PrimFnDydEqualAPA_EM, -- Result Boolean, can't happen w/Equal

    // Dyadic Boolean result functions
    &PrimFnDydEqualBisBvB,
    &PrimFnDydEqualBisIvI,
    &PrimFnDydEqualBisFvF,
    &PrimFnDydEqualBisCvC,
    &PrimFnDydEqualBisHvH,
    &PrimFnDydEqualBisRvR,
    &PrimFnDydEqualBisVvV,

    &PrimFnDydEqualBisHC2IvHC2I,
    &PrimFnDydEqualBisHC2FvHC2F,
    &PrimFnDydEqualBisHC2RvHC2R,
    &PrimFnDydEqualBisHC2VvHC2V,

    &PrimFnDydEqualBisHC4IvHC4I,
    &PrimFnDydEqualBisHC4FvHC4F,
    &PrimFnDydEqualBisHC4RvHC4R,
    &PrimFnDydEqualBisHC4VvHC4V,

    &PrimFnDydEqualBisHC8IvHC8I,
    &PrimFnDydEqualBisHC8FvHC8F,
    &PrimFnDydEqualBisHC8RvHC8R,
    &PrimFnDydEqualBisHC8VvHC8V,

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydEqualIisIvI, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualIisFvF, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualFisIvI, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualFisFvF, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualRisRvR, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualVisRvR, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualVisVvV, -- Result Boolean, can't happen w/Equal

    NULL,   // &PrimFnDydEqualHC2IisHC2IvHC2I, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC2IisHC2FvHC2F, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC2FisHC2IvHC2I, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC2FisHC2FvHC2F, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC2RisHC2RvHC2R, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC2VisHC2RvHC2R, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC2VisHC2VvHC2V, -- Result Boolean, can't happen w/Equal

    NULL,   // &PrimFnDydEqualHC4IisHC4IvHC4I, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC4IisHC4FvHC4F, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC4FisHC4IvHC4I, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC4FisHC4FvHC4F, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC4RisHC4RvHC4R, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC4VisHC4RvHC4R, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC4VisHC4VvHC4V, -- Result Boolean, can't happen w/Equal

    NULL,   // &PrimFnDydEqualHC8IisHC8IvHC8I, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC8IisHC8FvHC8F, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC8FisHC8IvHC8I, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC8FisHC8FvHC8F, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC8RisHC8RvHC8R, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC8VisHC8RvHC8R, -- Result Boolean, can't happen w/Equal
    NULL,   // &PrimFnDydEqualHC8VisHC8VvHC8V, -- Result Boolean, can't happen w/Equal

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonEqualB64isB64, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualB32isB32, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualB16isB16, -- No monadic defn, can't happen w/Equal
    NULL,   // &PrimFnMonEqualB08isB08, -- No monadic defn, can't happen w/Equal

    // Dyadic Boolean chunk functions
    &PrimFnDydEqualB64isB64vB64,
    &PrimFnDydEqualB32isB32vB32,
    &PrimFnDydEqualB16isB16vB16,
    &PrimFnDydEqualB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecEqual};
#endif


//***************************************************************************
//  $PrimFnEqual_EM_YY
//
//  Primitive function for monadic and dyadic Equal (ERROR and "equal")
//***************************************************************************

LPPL_YYSTYPE PrimFnEqual_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_EQUAL);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonEqual_EM_YY                       (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnEqual_EM_YY


//***************************************************************************
//  $PrimFnMonEqual_EM_YY
//
//  Primitive function for monadic Equal ("dimension")
//***************************************************************************

LPPL_YYSTYPE PrimFnMonEqual_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    int          iHCDimRht;         // Right arg dimension (1, 2, 4, 8)
    LPPL_YYSTYPE lpYYRes;           // Ptr to result

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, NULL, NULL, NULL);

    // If the right arg is HETERO or NESTED, ...
    if (IsPtrArray (aplTypeRht))
        goto DOMAIN_EXIT;

    // Get the HC dimension (1, 2, 4, 8)
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = iHCDimRht;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnMonEqual_EM_YY


//***************************************************************************
//  $PrimSpecEqualStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecEqualStorageTypeDyd
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

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // All numeric results are Boolean
    if (IsNumeric (aplTypeRes))
        aplTypeRes = ARRAY_BOOL;

    return aplTypeRes;
} // End PrimSpecEqualStorageTypeDyd


//***************************************************************************
//  $PrimFnDydEqualBisBvB
//
//  Primitive scalar function dyadic Equal:  B {is} B fn B
//***************************************************************************

void PrimFnDydEqualBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean EQ lpatRht->aplBoolean)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydEqualBisBvB


//***************************************************************************
//  $PrimFnDydEqualB64isB64vB64
//
//  Primitive scalar function dyadic Equal:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydEqualB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydEqualB64isB64vB64


//***************************************************************************
//  $PrimFnDydEqualB32isB32vB32
//
//  Primitive scalar function dyadic Equal:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydEqualB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydEqualB32isB32vB32


//***************************************************************************
//  $PrimFnDydEqualB16isB16vB16
//
//  Primitive scalar function dyadic Equal:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydEqualB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydEqualB16isB16vB16


//***************************************************************************
//  $PrimFnDydEqualB08isB08vB08
//
//  Primitive scalar function dyadic Equal:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydEqualB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft ^ aplBooleanRht);
} // End PrimFnDydEqualB08isB08vB08


//***************************************************************************
//  $PrimFnDydEqualBisIvI
//
//  Primitive scalar function dyadic Equal:  B {is} I fn I
//***************************************************************************

void PrimFnDydEqualBisIvI
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplInteger EQ lpatRht->aplInteger)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydEqualBisIvI


//***************************************************************************
//  $PrimFnDydEqualBisFvF
//
//  Primitive scalar function dyadic Equal:  B {is} F fn F
//***************************************************************************

void PrimFnDydEqualBisFvF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // N.B.:  Because <CmpCT_F> calls <flt_cmp_ct> which
    //          treats NaNs specially, two NaNs compare equally.

    // Compare the two floats relative to []CT
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_F (lpatLft->aplFloat, lpatRht->aplFloat, GetQuadCT (), EQ)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydEqualBisFvF


//***************************************************************************
//  $PrimFnDydEqualBisCvC
//
//  Primitive scalar function dyadic Equal:  B {is} C fn C
//***************************************************************************

void PrimFnDydEqualBisCvC
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplChar EQ lpatRht->aplChar)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydEqualBisCvC


//***************************************************************************
//  $PrimFnDydEqualBisHvH
//
//  Primitive scalar function dyadic NotEqual:  B {is} H fn H
//***************************************************************************

void PrimFnDydEqualBisHvH
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    UBOOL bRet;                         // The result

    // If the immediate types are both char, ...
    if (IsImmChr (lpatLft->aplHetero->stFlags.ImmType)
     && IsImmChr (lpatRht->aplHetero->stFlags.ImmType))
        bRet = (lpatLft->aplHetero->stData.stChar EQ
                lpatRht->aplHetero->stData.stChar);
    else
        bRet = 0;

    // If the result is TRUE, ...
    if (bRet)
        lpMemRes[uRes >> LOG2NBIB] |=
          BIT0
          << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydEqualBisHvH


//***************************************************************************
//  $PrimFnDydEqualRisRvR
//
//  Primitive scalar function dyadic Equal:  B {is} R fn R
//***************************************************************************

void PrimFnDydEqualBisRvR
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    // Compare the two RATs relative to []CT
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_R (lpatLft->aplRat, lpatRht->aplRat, GetQuadCT (), EQ)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydEqualBisRvR


//***************************************************************************
//  $PrimFnDydEqualBisVvV
//
//  Primitive scalar function dyadic Equal:  B {is} V fn V
//***************************************************************************

void PrimFnDydEqualBisVvV
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    // Compare the two VFPs relative to []CT
    lpMemRes[uRes >> LOG2NBIB] |=
      CmpCT_V (lpatLft->aplVfp, lpatRht->aplVfp, GetQuadCT (), EQ)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydEqualBisVvV


//***************************************************************************
//  $EqualHCxIvHCxI
//***************************************************************************

UBOOL EqualHCxIvHCxI
    (LPALLTYPES lpatLft,                // Left arg as LPALLTYPES
     LPALLTYPES lpatRht,                // Right ...
     int        iHCDim,                 // Dimension (1, 2, 4, 8)
     APLFLOAT   fQuadCT,                // []CT (0 = exact comparison)
     LPWCHAR    lpwszMsg)               // Ptr to CheckException msg
{
////APLFLOAT aplMagLft,                 // Magnitude of left arg
////         aplMagRht,                 // ...          right ...
////         aplDist;                   // Distance between left and right args
    UBOOL bRet = TRUE;                  // The result
    int   i;                            // Loop counter

    // Loop through all of the parts
    for (i = 0; i < iHCDim; i++)
        bRet &= (lpatLft->aplHC8I.parts[i] EQ lpatRht->aplHC8I.parts[i]);
    return bRet;

////__try
////{
////    // Split cases based upon the Dimension
////    switch (iHCDim)
////    {
////        case 2:
////            // Calculate the magnitudes
////            aplMagLft = MagHC2I (lpatLft->aplHC2I);
////            aplMagRht = MagHC2I (lpatRht->aplHC2I);
////
////            // Get the distance between the args
////            aplDist   = DistHC2I  (lpatLft->aplHC2I, lpatRht->aplHC2I);
////
////            break;
////
////        case 4:
////            // Calculate the magnitudes
////            aplMagLft = MagHC4I (lpatLft->aplHC4I);
////            aplMagRht = MagHC4I (lpatLft->aplHC4I);
////
////            // Get the distance between the args
////            aplDist   = DistHC4I  (lpatLft->aplHC4I, lpatRht->aplHC4I);
////
////            break;
////
////        case 8:
////            // Calculate the magnitudes
////            aplMagLft = MagHC8I (lpatLft->aplHC8I);
////            aplMagRht = MagHC8I (lpatLft->aplHC8I);
////
////            // Get the distance between the args
////            aplDist   = DistHC8I  (lpatLft->aplHC8I, lpatRht->aplHC8I);
////
////            break;
////
////        defstop
////            break;
////    } // End SWITCH
////} __except (CheckException (GetExceptionInformation (), lpwszMsg))
////{
////    EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code
////
////    Assert (exCode EQ EXCEPTION_DOMAIN_ERROR);
////
////    aplDist = fltPosInfinity;
////} // End __try/__except
////
////return (aplDist <= (fQuadCT * max (aplMagLft, aplMagRht))));
} // End EqualHCxIvHCxI


//***************************************************************************
//  $EqualBisHCxIvHCxI
//***************************************************************************

void EqualBisHCxIvHCxI
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Left arg as LPALLTYPES
     LPALLTYPES lpatRht,                // Right ...
     int        iHCDim,                 // Dimension (1, 2, 4, 8)
     UBOOL      bIsEqual,               // TRUE iff the calling function is Equal (FALSE if NotEqual)
     LPWCHAR    lpwszMsg)                // Ptr to CheckException msg
{
    // The two numbers are equal iff all parts are equal
    if (bIsEqual EQ EqualHCxIvHCxI (lpatLft, lpatRht, iHCDim, GetQuadCT (), lpwszMsg))
        lpMemRes[uRes >> LOG2NBIB] |=
          BIT0
          << (MASKLOG2NBIB & (UINT) uRes);
} // End EqualBisHCxIvHCxI


//***************************************************************************
//  $PrimFnDydEqualBisHC2IvHC2I
//
//  Primitive scalar function dyadic Equal:  B {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydEqualBisHC2IvHC2I
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxIvHCxI (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       2,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydEqualBisHC4IvHC4I
//
//  Primitive scalar function dyadic Equal:  B {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydEqualBisHC4IvHC4I
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxIvHCxI (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       4,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydEqualBisHC8IvHC8I
//
//  Primitive scalar function dyadic Equal:  B {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydEqualBisHC8IvHC8I
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxIvHCxI (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       8,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC8IvHC8I


//***************************************************************************
//  $EqualHCxFvHCxF
//***************************************************************************

UBOOL EqualHCxFvHCxF
    (LPALLTYPES lpatLft,                // Left arg as LPALLTYPES
     LPALLTYPES lpatRht,                // Right ...
     int        iHCDim,                 // Dimension (1, 2, 4, 8)
     APLFLOAT   fQuadCT,                // []CT (0 = exact comparison)
     LPWCHAR    lpwszMsg)               // Ptr to CheckException msg
{
    APLFLOAT aplMagLft,                 // Magnitude of left arg
             aplMagRht,                 // ...          right ...
             aplDist;                   // Distance between left and right args
    int      i;                         // Loop counter
    __try
    {
        // Split cases based upon the Dimension
        switch (iHCDim)
        {
            case 1:
                return CmpCT_F (lpatLft->aplFloat, lpatRht->aplFloat, fQuadCT, EQ);

            case 2:
                // Calculate the magnitudes
                aplMagLft = MagHC2F (lpatLft->aplHC2F);
                aplMagRht = MagHC2F (lpatRht->aplHC2F);

                // Get the distance between the args
                aplDist   = DistHC2F  (lpatLft->aplHC2F, lpatRht->aplHC2F);

                break;

            case 4:
                // Calculate the magnitudes
                aplMagLft = MagHC4F (lpatLft->aplHC4F);
                aplMagRht = MagHC4F (lpatLft->aplHC4F);

                // Get the distance between the args
                aplDist   = DistHC4F  (lpatLft->aplHC4F, lpatRht->aplHC4F);

                break;

            case 8:
                // Calculate the magnitudes
                aplMagLft = MagHC8F (lpatLft->aplHC8F);
                aplMagRht = MagHC8F (lpatLft->aplHC8F);

                // Get the distance between the args
                aplDist   = DistHC8F  (lpatLft->aplHC8F, lpatRht->aplHC8F);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), lpwszMsg))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        Assert (exCode EQ EXCEPTION_DOMAIN_ERROR);

        aplDist = fltPosInfinity;
    } // End __try/__except

    // If both of the magnitudes are NaN, ...
    if (IsFltNaN      (aplMagLft) && IsFltNaN      (aplMagRht))
    {
        // Compare the individual coordinates
        for (i = 0; i < iHCDim; i++)
        if (CmpCT_F (lpatLft->aplHC8F.parts[i],
                     lpatRht->aplHC8F.parts[i],
                     fQuadCT,
                     NE))
            return FALSE;
        return TRUE;
    } else
    // If one of the magnitudes is NaN and the other isn't, ...
    if (IsFltNaN      (aplMagLft) NE IsFltNaN      (aplMagRht))
        return FALSE;
    else
    // If both of the magnitudes are infinite, ...
    if (_isinf (aplMagLft) && _isinf (aplMagRht))
    {
        // Compare the individual coordinates
        for (i = 0; i < iHCDim; i++)
        if (CmpCT_F (lpatLft->aplHC8F.parts[i],
                     lpatRht->aplHC8F.parts[i],
                     fQuadCT,
                     NE))
            return FALSE;
        return TRUE;
    } else
    // If one of the magnitudes is infinite and the other isn't, ...
    if (_isinf (aplMagLft) NE _isinf (aplMagRht))
        return FALSE;
    else
        // The two numbers are equal iff the Dist is <= []CT * max (aplMagLft, aplMagRht)
        return aplDist <= (fQuadCT * max (aplMagLft, aplMagRht));
} // End EqualHCxFvHCxF


//***************************************************************************
//  $EqualBisHCxFvHCxF
//***************************************************************************

void EqualBisHCxFvHCxF
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Left arg as LPALLTYPES
     LPALLTYPES lpatRht,                // Right ...
     int        iHCDim,                 // Dimension (1, 2, 4, 8)
     UBOOL      bIsEqual,               // TRUE iff the calling function is Equal (FALSE if NotEqual)
     LPWCHAR    lpwszMsg)               // Ptr to CheckException msg
{
    // The two numbers are equal iff the Dist is <= []CT * max (aplMagLft, aplMagRht)
    if (bIsEqual EQ EqualHCxFvHCxF (lpatLft, lpatRht, iHCDim, GetQuadCT (), lpwszMsg))
        lpMemRes[uRes >> LOG2NBIB] |=
          BIT0
          << (MASKLOG2NBIB & (UINT) uRes);
} // End EqualBisHCxFvHCxF


//***************************************************************************
//  $PrimFnDydEqualBisHC2FvHC2F
//
//  Primitive scalar function dyadic Equal:  B {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydEqualBisHC2FvHC2F
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxFvHCxF (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       2,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC2FvHC2F


//***************************************************************************
//  $PrimFnDydEqualBisHC4FvHC4F
//
//  Primitive scalar function dyadic Equal:  B {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydEqualBisHC4FvHC4F
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxFvHCxF (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       4,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC4FvHC4F


//***************************************************************************
//  $PrimFnDydEqualBisHC8FvHC8F
//
//  Primitive scalar function dyadic Equal:  B {is} HC8F fn HC8F
//***************************************************************************

void PrimFnDydEqualBisHC8FvHC8F
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxFvHCxF (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       8,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC8FvHC8F


//***************************************************************************
//  $EqualHCxRvHCxR
//***************************************************************************

UBOOL EqualHCxRvHCxR
    (LPALLTYPES lpatLft,                // Left arg as LPALLTYPES
     LPALLTYPES lpatRht,                // Right ...
     int        iHCDim,                 // Dimension (1, 2, 4, 8)
     APLFLOAT   fQuadCT,                // []CT (0 = exact comparison)
     LPWCHAR    lpwszMsg)               // Ptr to GetExceptionInformation msg
{
    APLVFP aplMagLft,                   // Magnitude of left arg
           aplMagRht,                   // ...          right ...
           aplMagMax,                   // Magnitude of larger
           aplDist;                     // Distance between left and right args
    UBOOL  bRet;                        // The result
    int    i;                           // Loop counter

    __try
    {
        // Split cases based upon the Dimension
        switch (iHCDim)
        {
            case 1:
                return CmpCT_R (lpatLft->aplRat, lpatRht->aplRat, fQuadCT, EQ);

            case 2:
                // Calculate the magnitudes
                aplMagLft = MagHC2R (lpatLft->aplHC2R);
                aplMagRht = MagHC2R (lpatLft->aplHC2R);

                // If neither of the magnitudes are infinite, ...
                if (!IsMpfInfinity (&aplMagLft) && !IsMpfInfinity (&aplMagRht))
                    // Get the distance between the args
                    aplDist   = DistHC2R (lpatLft->aplHC2R, lpatRht->aplHC2R);

                break;

            case 4:
                // Calculate the magnitudes
                aplMagLft = MagHC4R (lpatLft->aplHC4R);
                aplMagRht = MagHC4R (lpatLft->aplHC4R);

                // If neither of the magnitudes are infinite, ...
                if (!IsMpfInfinity (&aplMagLft) && !IsMpfInfinity (&aplMagRht))
                    // Get the distance between the args
                    aplDist   = DistHC4R (lpatLft->aplHC4R, lpatRht->aplHC4R);

                break;

            case 8:
                // Calculate the magnitudes
                aplMagLft = MagHC8R (lpatLft->aplHC8R);
                aplMagRht = MagHC8R (lpatLft->aplHC8R);

                // If neither of the magnitudes are infinite, ...
                if (!IsMpfInfinity (&aplMagLft) && !IsMpfInfinity (&aplMagRht))
                    // Get the distance between the args
                    aplDist   = DistHC8R (lpatLft->aplHC8R, lpatRht->aplHC8R);

                break;

            defstop
                break;
        } // End SWITCH

        // Calculate the larger magnitude
        if (mpfr_cmp (&aplMagLft, &aplMagRht) < 0)
            mpfr_init_set (&aplMagMax, &aplMagRht, MPFR_RNDN);
        else
            mpfr_init_set (&aplMagMax, &aplMagLft, MPFR_RNDN);

        // Multiply it by []CT
        mpfr_mul_d (&aplMagMax, &aplMagMax, fQuadCT, MPFR_RNDN);
    } __except (CheckException (GetExceptionInformation (), lpwszMsg))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        Assert (exCode EQ EXCEPTION_DOMAIN_ERROR);

        aplDist = mpfPosInfinity;
    } // End __try/__except

    // Initialize the return value
    bRet = TRUE;

    // If both of the magnitudes are infinite, ...
    if (IsMpfInfinity (&aplMagLft) && IsMpfInfinity (&aplMagRht))
    {
        // Compare the individual coordinates
        for (i = 0; i < iHCDim; i++)
        if (CmpCT_R (lpatLft->aplHC8R.parts[i],
                     lpatRht->aplHC8R.parts[i],
                     fQuadCT,
                     NE))
            bRet = FALSE;
    } else
    // If one of the magnitudes is infinite and the other isn't, ...
    if (IsMpfInfinity (&aplMagLft) NE IsMpfInfinity (&aplMagRht))
        bRet = FALSE;
    else
        // The two numbers are equal iff the Dist is <= []CT * max (aplMagLft, aplMagRht)
        bRet = mpfr_cmp (&aplDist, &aplMagMax) <= 0;

    // We no longer need this resource
    Myf_clear (&aplMagMax);

    return bRet;
} // End EqualHCxRvHCxR


//***************************************************************************
//  $EqualBisHCxRvHCxR
//***************************************************************************

void EqualBisHCxRvHCxR
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Left arg as LPALLTYPES
     LPALLTYPES lpatRht,                // Right ...
     int        iHCDim,                 // Dimension (1, 2, 4, 8)
     UBOOL      bIsEqual,               // TRUE iff the calling function is Equal (FALSE if NotEqual)
     LPWCHAR    lpwszMsg)               // Ptr to GetExceptionInformation msg
{
    // The two numbers are equal iff the Dist is <= []CT * max (aplMagLft, aplMagRht)
    if (bIsEqual EQ EqualHCxRvHCxR (lpatLft, lpatRht, iHCDim, GetQuadCT (), lpwszMsg))
        lpMemRes[uRes >> LOG2NBIB] |=
          BIT0
          << (MASKLOG2NBIB & (UINT) uRes);
} // End EqualBisHCxRvHCxR


//***************************************************************************
//  $PrimFnDydEqualBisHC2RvHC2R
//
//  Primitive scalar function dyadic Equal:  B {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydEqualBisHC2RvHC2R
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxRvHCxR (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       2,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydEqualBisHC4RvHC4R
//
//  Primitive scalar function dyadic Equal:  B {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydEqualBisHC4RvHC4R
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxRvHCxR (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       4,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydEqualBisHC8RvHC8R
//
//  Primitive scalar function dyadic Equal:  B {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydEqualBisHC8RvHC8R
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxRvHCxR (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       8,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC8RvHC8R


//***************************************************************************
//  $EqualHCxVvHCxV
//***************************************************************************

UBOOL EqualHCxVvHCxV
    (LPALLTYPES lpatLft,                // Left arg as LPALLTYPES
     LPALLTYPES lpatRht,                // Right ...
     int        iHCDim,                 // Dimension (1, 2, 4, 8)
     APLFLOAT   fQuadCT,                // []CT (0 = exact comparison)
     LPWCHAR    lpwszMsg)               // Ptr to GetExceptionInformation msg
{
    APLVFP aplMagLft,                   // Magnitude of left arg
           aplMagRht,                   // ...          right ...
           aplMagMax,                   // Magnitude of larger
           aplDist;                     // Distance between left and right args
    UBOOL  bRet;                        // The result
    int    i;                           // Loop counter

    __try
    {
        // Split cases based upon the Dimension
        switch (iHCDim)
        {
            case 1:
                return CmpCT_V (lpatLft->aplVfp, lpatRht->aplVfp, fQuadCT, EQ);

            case 2:
                // Calculate the magnitudes
                aplMagLft = MagHC2V (lpatLft->aplHC2V);
                aplMagRht = MagHC2V (lpatLft->aplHC2V);

                // If neither of the magnitudes are infinite, ...
                if (!IsMpfInfinity (&aplMagLft) && !IsMpfInfinity (&aplMagRht))
                    // Get the distance between the args
                    aplDist   = DistHC2V (lpatLft->aplHC2V, lpatRht->aplHC2V);

                break;

            case 4:
                // Calculate the magnitudes
                aplMagLft = MagHC4V (lpatLft->aplHC4V);
                aplMagRht = MagHC4V (lpatLft->aplHC4V);

                // If neither of the magnitudes are infinite, ...
                if (!IsMpfInfinity (&aplMagLft) && !IsMpfInfinity (&aplMagRht))
                    // Get the distance between the args
                    aplDist   = DistHC4V (lpatLft->aplHC4V, lpatRht->aplHC4V);

                break;

            case 8:
                // Calculate the magnitudes
                aplMagLft = MagHC8V (lpatLft->aplHC8V);
                aplMagRht = MagHC8V (lpatLft->aplHC8V);

                // If neither of the magnitudes are infinite, ...
                if (!IsMpfInfinity (&aplMagLft) && !IsMpfInfinity (&aplMagRht))
                    // Get the distance between the args
                    aplDist   = DistHC8V (lpatLft->aplHC8V, lpatRht->aplHC8V);

                break;

            defstop
                break;
        } // End SWITCH

        // Calculate the larger magnitude
        if (mpfr_cmp (&aplMagLft, &aplMagRht) < 0)
            mpfr_init_set (&aplMagMax, &aplMagRht, MPFR_RNDN);
        else
            mpfr_init_set (&aplMagMax, &aplMagLft, MPFR_RNDN);

        // Multiply it by []CT
        mpfr_mul_d (&aplMagMax, &aplMagMax, fQuadCT, MPFR_RNDN);
    } __except (CheckException (GetExceptionInformation (), lpwszMsg))
    {
        EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

        Assert (exCode EQ EXCEPTION_DOMAIN_ERROR);

        aplDist = mpfPosInfinity;
    } // End __try/__except

    // Initialize the return value
    bRet = TRUE;

    // If both of the magnitudes are infinite or NaNs, ...
    if ((IsMpfInfinity (&aplMagLft) && IsMpfInfinity (&aplMagRht))
     || (mpfr_nan_p    (&aplMagLft) && mpfr_nan_p    (&aplMagRht)))
    {
        // Compare the individual coordinates
        for (i = 0; i < iHCDim; i++)
        if (CmpCT_V (lpatLft->aplHC8V.parts[i],
                     lpatRht->aplHC8V.parts[i],
                     fQuadCT,
                     NE))
            bRet = FALSE;
    } else
    // If one of the magnitudes is infinite and the other isn't, ...
    if (IsMpfInfinity (&aplMagLft) NE IsMpfInfinity (&aplMagRht))
        bRet = FALSE;
    else
        // The two numbers are equal iff the Dist is <= []CT * max (aplMagLft, aplMagRht)
        bRet = mpfr_cmp (&aplDist, &aplMagMax) <= 0;

    // We no longer need this resource
    Myf_clear (&aplMagMax);

    return bRet;
} // End EqualHCxVvHCxV


//***************************************************************************
//  $EqualBisHCxVvHCxV
//***************************************************************************

void EqualBisHCxVvHCxV
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Left arg as LPALLTYPES
     LPALLTYPES lpatRht,                // Right ...
     int        iHCDim,                   // Dimension (1, 2, 4, 8)
     UBOOL      bIsEqual,               // TRUE iff the calling function is Equal (FALSE if NotEqual)
     LPWCHAR    lpwszMsg)               // Ptr to GetExceptionInformation msg
{
    // The two numbers are equal iff the Dist is <= []CT * max (aplMagLft, aplMagRht)
    if (bIsEqual EQ EqualHCxVvHCxV (lpatLft, lpatRht, iHCDim, GetQuadCT (), lpwszMsg))
        lpMemRes[uRes >> LOG2NBIB] |=
          BIT0
          << (MASKLOG2NBIB & (UINT) uRes);
} // End EqualBisHCxVvHCxV


//***************************************************************************
//  $PrimFnDydEqualBisHC2VvHC2V
//
//  Primitive scalar function dyadic Equal:  B {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydEqualBisHC2VvHC2V
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxVvHCxV (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       2,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC2VvHC2V


//***************************************************************************
//  $PrimFnDydEqualBisHC4VvHC4V
//
//  Primitive scalar function dyadic Equal:  B {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydEqualBisHC4VvHC4V
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxVvHCxV (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       4,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC4VvHC4V


//***************************************************************************
//  $PrimFnDydEqualBisHC8VvHC8V
//
//  Primitive scalar function dyadic Equal:  B {is} HC8V fn HC8V
//***************************************************************************

void PrimFnDydEqualBisHC8VvHC8V
    (LPAPLBOOL  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatLft,                // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,                // ...    right ...
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EqualBisHCxVvHCxV (lpMemRes,        // Ptr to the result
                       uRes,            // Index into the result
                       lpatLft,         // Left arg as LPALLTYPES
                       lpatRht,         // Right ...
                       8,               // Dimension (1, 2, 4, 8)
                       TRUE,            // TRUE iff the calling function is Equal (FALSE if NotEqual)
                       WFCN);   // Ptr to CheckException msg
} // End PrimFnDydEqualBisHC8VvHC8V


//***************************************************************************
//  $EqualHCxy
//
//  Single value comparison where both of the args are NaNs.
//  The args may be of different dimension and base type.
//***************************************************************************

UBOOL EqualHCxy
    (APLSTYPE   aplTypeLft,             // Left arg storage type
     LPALLTYPES lpatLft,                // Ptr to left arg
     APLINT     uLft,                   // Index into the left arg
     APLSTYPE   aplTypeRht,             // Right arg storage type
     LPALLTYPES lpatRht,                // Ptr to right arg
     APLINT     uRht)                   // Index into the right arg

{
    APLSTYPE aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];
    int      iHCDimCom = TranslateArrayTypeToHCDim (aplTypeCom),
             i;
    APLFLOAT fQuadCT = GetQuadCT ();
    UBOOL    bRet = TRUE;
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the args to the common storage type
    (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpatLft, uLft, &atLft);
    (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpatRht, uRht, &atRht);

    // Split cases based upon the base type
    switch (aToSimple[aplTypeCom])
    {
////////case ARRAY_INT:                     // No INT NaNs
////////    // Loop through all of the parts
////////    for (i = 0; bRet && i < iHCDimCom; i++)
////////    if (atLft.aplHC8I.parts[i] NE atRht.aplHC8I.parts[i])
////////        bRet = FALSE;
////////    break;
////////
        case ARRAY_FLOAT:
            // Compare the individual coordinates
            for (i = 0; bRet && i < iHCDimCom; i++)
            if (CmpCT_F (atLft.aplHC8F.parts[i],
                         atRht.aplHC8F.parts[i],
                         fQuadCT,
                         NE))
                bRet = FALSE;
            break;

        case ARRAY_RAT:
            // Compare the individual coordinates
            for (i = 0; bRet && i < iHCDimCom; i++)
            if (CmpCT_R (atLft.aplHC8R.parts[i],
                         atRht.aplHC8R.parts[i],
                         fQuadCT,
                         NE))
                bRet = FALSE;
            break;

        case ARRAY_VFP:
            // Compare the individual coordinates
            for (i = 0; bRet && i < iHCDimCom; i++)
            if (CmpCT_V (atLft.aplHC8V.parts[i],
                         atRht.aplHC8V.parts[i],
                         fQuadCT,
                         NE))
                bRet = FALSE;
            break;

        defstop
            return FALSE;
    } // End FOR

    // Free the temps
    (*aTypeFree[aplTypeCom]) (&atLft, 0);
    (*aTypeFree[aplTypeCom]) (&atRht, 0);

    return bRet;
} // End EqualHCxy


//***************************************************************************
//  End of File: pf_equal.c
//***************************************************************************
