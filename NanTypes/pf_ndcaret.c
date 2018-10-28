//***************************************************************************
//  NARS2000 -- Primitive Function -- DownCaretTilde
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
PRIMSPEC PrimSpecDownCaretTilde = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecDownCaretTildeStorageTypeMon, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeAPA_EM, -- Can't happen w/DownCaretTilde

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDownCaretTildeBisB, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeBisI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeBisF, -- Can't happen w/DownCaretTilde

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDownCaretTildeIisI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeIisF, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeFisI, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeFisF, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeRisR, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeVisR, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeVisV, -- Can't happen w/DownCaretTilde

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDownCaretTildeHC2IisHC2I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC2IisHC2F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC2FisHC2I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC2FisHC2F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC2RisHC2R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC2VisHC2R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC2VisHC2V, -- Can't happen w/DownCaretTilde

    NULL,   // &PrimFnMonDownCaretTildeHC4IisHC4I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC4IisHC4F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC4FisHC4I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC4FisHC4F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC4RisHC4R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC4VisHC4R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC4VisHC4V, -- Can't happen w/DownCaretTilde

    NULL,   // &PrimFnMonDownCaretTildeHC8IisHC8I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC8IisHC8F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC8FisHC8F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC8FisHC8I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC8RisHC8R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC8VisHC8R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeHC8VisHC8V, -- Can't happen w/DownCaretTilde

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDownCaretTildeFisHC2I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeFisHC2F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeVisHC2R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeVisHC2V, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeFisHC4I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeFisHC4F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeVisHC4R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeVisHC4V, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeFisHC8I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeFisHC8F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeVisHC8R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeVisHC8V, -- Can't happen w/DownCaretTilde

    // Monadic BAxF result BAxF arg functions (indexable)
    NULL,   // &PrimFnMonDownCaretTildeBA1FisBA1F,
    NULL,   // &PrimFnMonDownCaretTildeBA2FisBA2F,
    NULL,   // &PrimFnMonDownCaretTildeBA4FisBA4F,
    NULL,   // &PrimFnMonDownCaretTildeBA8FisBA8F,
    NULL,   // &PrimFnMonDownCaretTildeARBisBA1F,
    NULL,   // &PrimFnMonDownCaretTildeARBisBA2F,
    NULL,   // &PrimFnMonDownCaretTildeARBisBA4F,
    NULL,   // &PrimFnMonDownCaretTildeARBisBA8F,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDownCaretTildeStorageTypeDyd,
    NULL,   // &PrimFnDydDownCaretTildeAPA_EM, -- Can't happen w/DownCaretTilde

    // Dyadic Boolean result functions
    &PrimFnDydDownCaretTildeBisBvB,
    &PrimFnDydDownCaretTildeBisIvI,
    &PrimFnDydDownCaretTildeBisFvF,
    NULL,   // &PrimFnDydDownCaretTildeBisCvC, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisHvH, -- Can't happen w/DownCaretTilde
    &PrimFnDydDownCaretTildeBisRvR,
    &PrimFnDydDownCaretTildeBisVvV,

    NULL,   // &PrimFnDydDownCaretTildeBisHC2IvHC2I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisHC2FvHC2F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisHC2RvHC2R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisHC2VvHC2V, -- Can't happen w/DownCaretTilde

    NULL,   // &PrimFnDydDownCaretTildeBisHC4IvHC4I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisHC4FvHC4F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisHC4RvHC4R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisHC4VvHC4V, -- Can't happen w/DownCaretTilde

    NULL,   // &PrimFnDydDownCaretTildeBisHC8IvHC8I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisHC8FvHC8F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisHC8RvHC8R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisHC8VvHC8V, -- Can't happen w/DownCaretTilde

    // Dyadic Boolean result functions with ARBs (indexable)
    &PrimFnDydDownCaretTildeBisBA1FvBA1F,
    NULL,   // &PrimFnDydDownCaretTildeBisBA2FvBA2F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisBA4FvBA4F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeBisBA8FvBA8F, -- Can't happen w/DownCaretTilde

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydDownCaretTildeIisIvI, -- Result Boolean, can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeIisFvF, -- Result Boolean, can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeFisIvI, -- Result Boolean, can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeFisFvF, -- Result Boolean, can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeRisRvR, -- Result Boolean, can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeVisRvR, -- Result Boolean, can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeVisVvV, -- Result Boolean, can't happen w/DownCaretTilde

    NULL,   // &PrimFnDydDownCaretTildeHC2IisHC2IvHC2I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC2IisHC2FvHC2F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC2FisHC2IvHC2I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC2FisHC2FvHC2F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC2RisHC2RvHC2R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC2VisHC2RvHC2R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC2VisHC2VvHC2V, -- Can't happen w/DownCaretTilde

    NULL,   // &PrimFnDydDownCaretTildeHC4IisHC4IvHC4I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC4IisHC4FvHC4F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC4FisHC4IvHC4I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC4FisHC4FvHC4F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC4RisHC4RvHC4R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC4VisHC4RvHC4R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC4VisHC4VvHC4V, -- Can't happen w/DownCaretTilde

    NULL,   // &PrimFnDydDownCaretTildeHC8IisHC8IvHC8I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC8IisHC8FvHC8F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC8FisHC8IvHC8I, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC8FisHC8FvHC8F, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC8RisHC8RvHC8R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC8VisHC8RvHC8R, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnDydDownCaretTildeHC8VisHC8VvHC8V, -- Can't happen w/DownCaretTilde

    // Dyadic BAxF result functions (indexable)
    NULL,   // &PrimFnDydDownCaretTildeBA1FisBA1FvBA1F,
    NULL,   // &PrimFnDydDownCaretTildeBA2FisBA2FvBA2F,
    NULL,   // &PrimFnDydDownCaretTildeBA4FisBA4FvBA4F,
    NULL,   // &PrimFnDydDownCaretTildeBA8FisBA8FvBA8F,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDownCaretTildeB64isB64, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeB32isB32, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeB16isB16, -- Can't happen w/DownCaretTilde
    NULL,   // &PrimFnMonDownCaretTildeB08isB08, -- Can't happen w/DownCaretTilde

    // Dyadic Boolean chunk functions
    &PrimFnDydDownCaretTildeB64isB64vB64,
    &PrimFnDydDownCaretTildeB32isB32vB32,
    &PrimFnDydDownCaretTildeB16isB16vB16,
    &PrimFnDydDownCaretTildeB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecDownCaretTilde};
#endif


//***************************************************************************
//  $PrimFnDownCaretTilde_EM_YY
//
//  Primitive function for monadic and dyadic DownCaretTilde (ERROR and "nor")
//***************************************************************************

LPPL_YYSTYPE PrimFnDownCaretTilde_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharDownCaretTilde (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnDownCaretTilde_EM_YY


//***************************************************************************
//  $PrimSpecDownCaretTildeStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownCaretTildeStorageTypeDyd
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
} // End PrimSpecDownCaretTildeStorageTypeDyd


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisBvB
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} B fn B
//***************************************************************************

void PrimFnDydDownCaretTildeBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (!(lpatLft->aplBoolean | lpatRht->aplBoolean))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydDownCaretTildeBisBvB


//***************************************************************************
//  $PrimFnDydDownCaretTildeB64isB64vB64
//
//  Primitive scalar function dyadic DownCaretTilde:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydDownCaretTildeB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeB64isB64vB64


//***************************************************************************
//  $PrimFnDydDownCaretTildeB32isB32vB32
//
//  Primitive scalar function dyadic DownCaretTilde:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydDownCaretTildeB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeB32isB32vB32


//***************************************************************************
//  $PrimFnDydDownCaretTildeB16isB16vB16
//
//  Primitive scalar function dyadic DownCaretTilde:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydDownCaretTildeB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeB16isB16vB16


//***************************************************************************
//  $PrimFnDydDownCaretTildeB08isB08vB08
//
//  Primitive scalar function dyadic DownCaretTilde:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydDownCaretTildeB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretTildeB08isB08vB08


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisIvI
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} I fn I
//***************************************************************************

void PrimFnDydDownCaretTildeBisIvI
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    if (!IsBooleanValue (lpatLft->aplInteger)
     || !IsBooleanValue (lpatRht->aplInteger))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    lpMemRes[uRes >> LOG2NBIB] |=
      (!(lpatLft->aplBoolean | lpatRht->aplBoolean))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydDownCaretTildeBisIvI


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisFvF
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} F fn F
//***************************************************************************

void PrimFnDydDownCaretTildeBisFvF
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLINT aplLft,
           aplRht;
    UBOOL  bRet = TRUE;

    // Attempt to convert the float to an integer using System []CT
    aplLft = FloatToAplint_SCT (lpatLft->aplFloat, &bRet);
    if (bRet)
        // Attempt to convert the float to an integer using System []CT
        aplRht = FloatToAplint_SCT (lpatRht->aplFloat, &bRet);
    if (!bRet
     || !IsBooleanValue (aplLft)
     || !IsBooleanValue (aplRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    lpMemRes[uRes >> LOG2NBIB] |=
      (!(aplLft | aplRht))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydDownCaretTildeBisFvF


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisRvR
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} R fn R
//***************************************************************************

void PrimFnDydDownCaretTildeBisRvR
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLBOOL aplBooleanLft,
            aplBooleanRht;

    if ((!IsMpq0 (&lpatLft->aplRat)
      && !IsMpq1 (&lpatLft->aplRat))
     || (!IsMpq0 (&lpatRht->aplRat)
      && !IsMpq1 (&lpatRht->aplRat)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    aplBooleanLft = IsMpq1 (&lpatLft->aplRat);
    aplBooleanRht = IsMpq1 (&lpatRht->aplRat);

    lpMemRes[uRes >> LOG2NBIB] |=
      (!(aplBooleanLft | aplBooleanRht))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydDownCaretTildeBisRvR


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisVvV
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} V fn V
//***************************************************************************

void PrimFnDydDownCaretTildeBisVvV
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLBOOL aplBooleanLft,
            aplBooleanRht;

    if ((!IsMpf0 (&lpatLft->aplVfp)
      && !IsMpf1 (&lpatLft->aplVfp))
     || (!IsMpf0 (&lpatRht->aplVfp)
      && !IsMpf1 (&lpatRht->aplVfp)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    aplBooleanLft = IsMpf1 (&lpatLft->aplVfp);
    aplBooleanRht = IsMpf1 (&lpatRht->aplVfp);

    lpMemRes[uRes >> LOG2NBIB] |=
      (!(aplBooleanLft | aplBooleanRht))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydDownCaretTildeBisVvV


//***************************************************************************
//  $PrimFnDydDownCaretTildeBisBA1FvBA1F
//
//  Primitive scalar function dyadic DownCaretTilde:  B {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydDownCaretTildeBisBA1FvBA1F
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLBOOL aplBooleanLft,
            aplBooleanRht;

    if ((!IsArb0 (&lpatLft->aplArb)
      && !IsArb1 (&lpatLft->aplArb))
     || (!IsArb0 (&lpatRht->aplArb)
      && !IsArb1 (&lpatRht->aplArb)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    aplBooleanLft = IsArb1 (&lpatLft->aplArb);
    aplBooleanRht = IsArb1 (&lpatRht->aplArb);

    lpMemRes[uRes >> LOG2NBIB] |=
      (!(aplBooleanLft | aplBooleanRht))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydDownCaretTildeBisBA1FvBA1F


//***************************************************************************
//  End of File: pf_ndcaret.c
//***************************************************************************
