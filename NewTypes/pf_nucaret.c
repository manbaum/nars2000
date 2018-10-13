//***************************************************************************
//  NARS2000 -- Primitive Function -- UpCaretTilde
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
PRIMSPEC PrimSpecUpCaretTilde = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecUpCaretTildeStorageTypeMon, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeAPA_EM, -- Can't happen w/UpCaretTilde

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonUpCaretTildeBisB, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeBisI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeBisF, -- Can't happen w/UpCaretTilde

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonUpCaretTildeIisI, -- Can't happend w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeIisF, -- Can't happend w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeFisI, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeFisF, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeRisR, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeVisR, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeVisV, -- Can't happen w/UpCaretTilde

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonUpCaretTildeHC2IisHC2I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC2IisHC2F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC2FisHC2I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC2FisHC2F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC2RisHC2R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC2VisHC2R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC2VisHC2V, -- Can't happen w/UpCaretTilde

    NULL,   // &PrimFnMonUpCaretTildeHC4IisHC4I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC4IisHC4F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC4FisHC4I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC4FisHC4F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC4RisHC4R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC4VisHC4R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC4VisHC4V, -- Can't happen w/UpCaretTilde

    NULL,   // &PrimFnMonUpCaretTildeHC8IisHC8I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC8IisHC8F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC8FisHC8I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC8FisHC8F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC8RisHC8R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC8VisHC8R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeHC8VisHC8V, -- Can't happen w/UpCaretTilde

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonUpCaretTildeFisHC2I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeFisHC2F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeVisHC2R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeVisHC2V, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeFisHC4I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeFisHC4F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeVisHC4R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeVisHC4V, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeFisHC8I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeFisHC8F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeVisHC8R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeVisHC8V, -- Can't happen w/UpCaretTilde

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpCaretTildeStorageTypeDyd,
    NULL,   // &PrimFnDydUpCaretTildeAPA_EM, -- Can't happen w/UpCaretTilde

    // Dyadic Boolean result functions
    &PrimFnDydUpCaretTildeBisBvB,
    &PrimFnDydUpCaretTildeBisIvI,
    &PrimFnDydUpCaretTildeBisFvF,
    NULL,   // &PrimFnDydUpCaretTildeBisCvC, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeBisHvH, -- Can't happen w/UpCaretTilde
    &PrimFnDydUpCaretTildeBisRvR,
    &PrimFnDydUpCaretTildeBisVvV,

    NULL,   // &PrimFnDydUpCaretTildeBisHC2IvHC2I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeBisHC2FvHC2F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeBisHC2RvHC2R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeBisHC2VvHC2V, -- Can't happen w/UpCaretTilde

    NULL,   // &PrimFnDydUpCaretTildeBisHC4IvHC4I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeBisHC4FvHC4F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeBisHC4RvHC4R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeBisHC4VvHC4V, -- Can't happen w/UpCaretTilde

    NULL,   // &PrimFnDydUpCaretTildeBisHC8IvHC8I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeBisHC8FvHC8F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeBisHC8RvHC8R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeBisHC8VvHC8V, -- Can't happen w/UpCaretTilde

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydUpCaretTildeIisIvI, -- Result Boolean, can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeIisFvF, -- Result Boolean, can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeFisIvI, -- Result Boolean, can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeFisFvF, -- Result Boolean, can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeRisRvR, -- Result Boolean, can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeVisRvR, -- Result Boolean, can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeVisVvV, -- Result Boolean, can't happen w/UpCaretTilde

    NULL,   // &PrimFnDydUpCaretTildeHC2IisHC2IvHC2I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC2IisHC2FvHC2F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC2FisHC2IvHC2I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC2FisHC2FvHC2F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC2RisHC2RvHC2R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC2VisHC2RvHC2R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC2VisHC2VvHC2V, -- Can't happen w/UpCaretTilde

    NULL,   // &PrimFnDydUpCaretTildeHC4IisHC4IvHC4I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC4IisHC4FvHC4F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC4FisHC4IvHC4I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC4FisHC4FvHC4F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC4RisHC4RvHC4R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC4VisHC4RvHC4R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC4VisHC4VvHC4V, -- Can't happen w/UpCaretTilde

    NULL,   // &PrimFnDydUpCaretTildeHC8IisHC8IvHC8I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC8IisHC8FvHC8F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC8FisHC8IvHC8I, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC8FisHC8FvHC8F, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC8RisHC8RvHC8R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC8VisHC8RvHC8R, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnDydUpCaretTildeHC8VisHC8VvHC8V, -- Can't happen w/UpCaretTilde

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonUpCaretTildeB64isB64, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeB32isB32, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeB16isB16, -- Can't happen w/UpCaretTilde
    NULL,   // &PrimFnMonUpCaretTildeB08isB08, -- Can't happen w/UpCaretTilde

    // Dyadic Boolean chunk functions
    &PrimFnDydUpCaretTildeB64isB64vB64,
    &PrimFnDydUpCaretTildeB32isB32vB32,
    &PrimFnDydUpCaretTildeB16isB16vB16,
    &PrimFnDydUpCaretTildeB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpCaretTilde};
#endif


//***************************************************************************
//  $PrimFnUpCaretTilde_EM_YY
//
//  Primitive function for monadic and dyadic UpCaretTilde (ERROR and "nand")
//***************************************************************************

LPPL_YYSTYPE PrimFnUpCaretTilde_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharUpCaretTilde (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpCaretTilde_EM_YY


//***************************************************************************
//  $PrimSpecUpCaretTildeStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpCaretTildeStorageTypeDyd
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
} // End PrimSpecUpCaretTildeStorageTypeDyd


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisBvB
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} B fn B
//***************************************************************************

void PrimFnDydUpCaretTildeBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (!(lpatLft->aplBoolean & lpatRht->aplBoolean))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydUpCaretTildeBisBvB


//***************************************************************************
//  $PrimFnDydUpCaretTildeB64isB64vB64
//
//  Primitive scalar function dyadic UpCaretTilde:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydUpCaretTildeB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeB64isB64vB64


//***************************************************************************
//  $PrimFnDydUpCaretTildeB32isB32vB32
//
//  Primitive scalar function dyadic UpCaretTilde:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydUpCaretTildeB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeB32isB32vB32


//***************************************************************************
//  $PrimFnDydUpCaretTildeB16isB16vB16
//
//  Primitive scalar function dyadic UpCaretTilde:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydUpCaretTildeB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeB16isB16vB16


//***************************************************************************
//  $PrimFnDydUpCaretTildeB08isB08vB08
//
//  Primitive scalar function dyadic UpCaretTilde:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydUpCaretTildeB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return ~(aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretTildeB08isB08vB08


//***************************************************************************
//  $PrimFnDydUpCaretTildeIisIvI
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} I fn I
//***************************************************************************

void PrimFnDydUpCaretTildeBisIvI
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
      (!(lpatLft->aplBoolean & lpatRht->aplBoolean))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydUpCaretTildeBisIvI


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisFvF
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} F fn F
//***************************************************************************

void PrimFnDydUpCaretTildeBisFvF
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
      (!(aplLft & aplRht))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydUpCaretTildeBisFvF


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisRvR
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} R fn R
//***************************************************************************

void PrimFnDydUpCaretTildeBisRvR
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
      (!(aplBooleanLft & aplBooleanRht))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydUpCaretTildeBisRvR


//***************************************************************************
//  $PrimFnDydUpCaretTildeBisVvV
//
//  Primitive scalar function dyadic UpCaretTilde:  B {is} V fn V
//***************************************************************************

void PrimFnDydUpCaretTildeBisVvV
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
      (!(aplBooleanLft & aplBooleanRht))
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydUpCaretTildeBisVvV


//***************************************************************************
//  End of File: pf_nucaret.c
//***************************************************************************
