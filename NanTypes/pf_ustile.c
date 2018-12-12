//***************************************************************************
//  NARS2000 -- Primitive Function -- UpStile
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
#include <math.h>
#include "pf_fcre.h"
#include "headers.h"


#ifndef PROTO
PRIMSPEC PrimSpecUpStile = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecUpStileStorageTypeMon,
    &PrimFnMonUpStileAPA_EM,

    // Monadic Boolean result functions
    &PrimFnMonUpStileBisB,
    NULL,   // &PrimFnMonUpStileBisI, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileBisF, -- Can't happen w/UpStile

    // Monadic non-Boolean result functions (indexable)
    &PrimFnMonUpStileIisI,
    &PrimFnMonUpStileIisF,
    NULL,   // &PrimFnMonUpStileFisI, -- Can't happen w/UpStile
    &PrimFnMonUpStileFisF,
    &PrimFnMonUpStileRisR,
    &PrimFnMonUpStileVisR,
    &PrimFnMonUpStileVisV,

    // Monadic Hypercomplex functions
    &PrimFnMonUpStileHC2IisHC2I,
    &PrimFnMonUpStileHC2IisHC2F,
    &PrimFnMonUpStileHC2FisHC2I,
    &PrimFnMonUpStileHC2FisHC2F,
    &PrimFnMonUpStileHC2RisHC2R,
    &PrimFnMonUpStileHC2VisHC2R,
    &PrimFnMonUpStileHC2VisHC2V,

    &PrimFnMonUpStileHC4IisHC4I,
    &PrimFnMonUpStileHC4IisHC4F,
    &PrimFnMonUpStileHC4FisHC4I,
    &PrimFnMonUpStileHC4FisHC4F,
    &PrimFnMonUpStileHC4RisHC4R,
    &PrimFnMonUpStileHC4VisHC4R,
    &PrimFnMonUpStileHC4VisHC4V,

    NULL,   // &PrimFnMonUpStileHC8IisHC8I, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileHC8IisHC8F, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileHC8FisHC8I, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileHC8FisHC8F, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileHC8RisHC8R, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileHC8VisHC8R, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileHC8VisHC8V, -- Can't happen w/UpStile

    // Monadic INT/FLT/RAT/VFP result HC arg functions (indexable)
    NULL,   // &PrimFnMonUpStileFisHC2I, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileFisHC2F, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileVisHC2R, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileVisHC2V, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileFisHC4I, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileFisHC4F, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileVisHC4R, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileVisHC4V, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileFisHC8I, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileFisHC8F, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileVisHC8R, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileVisHC8V, -- Can't happen w/UpStile

    // Monadic BAxF result BAxF arg functions (indexable)
    &PrimFnMonUpStileBA1FisBA1F,
    &PrimFnMonUpStileBA2FisBA2F,
    &PrimFnMonUpStileBA4FisBA4F,
    NULL,   // &PrimFnMonUpStileBA8FisBA8F, -- Can't happen w/UpStile
    &PrimFnMonUpStileBA1FisBA1F,
    NULL,   // &PrimFnMonUpStileARBisBA2F, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileARBisBA4F, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileARBisBA8F, -- Can't happen w/UpStile

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpStileStorageTypeDyd,
    NULL,   // &PrimFnDydUpStileAPA_EM, -- Can't happen w/UpStile

    // Dyadic Boolean result functions (indexable)
    &PrimFnDydDownCaretBisBvB,
    NULL,   // &PrimFnDydUpStileBisIvI, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisFvF, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisCvC, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisHvH, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisRvR, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisVvV, -- Can't happen w/UpStile

    NULL,   // &PrimFnDydUpStileBisHC2IvHC2I, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisHC2FvHC2F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisHC2RvHC2R, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisHC2VvHC2V, -- Can't happen w/UpStile

    NULL,   // &PrimFnDydUpStileBisHC4IvHC4I, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisHC4FvHC4F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisHC4RvHC4R, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisHC4VvHC4V, -- Can't happen w/UpStile

    NULL,   // &PrimFnDydUpStileBisHC8IvHC8I, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisHC8FvHC8F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisHC8RvHC8R, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisHC8VvHC8V, -- Can't happen w/UpStile

    // Dyadic Boolean result functions with ARBs (indexable)
    NULL,   // &PrimFnDydUpStileBisBA1FvBA1F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisBA2FvBA2F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisBA4FvBA4F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBisBA8FvBA8F, -- Can't happen w/UpStile

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydUpStileIisIvI,
    NULL,   // &PrimFnDydUpStileIisFvF, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileFisIvI, -- Can't happen w/UpStile
    &PrimFnDydUpStileFisFvF,
    &PrimFnDydUpStileRisRvR,
    &PrimFnDydUpStileVisRvR,
    &PrimFnDydUpStileVisVvV,

    NULL,   // &PrimFnDydUpStileHC2IisHC2IvHC2I, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC2IisHC2FvHC2F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC2FisHC2IvHC2I, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC2FisHC2FvHC2F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC2RisHC2RvHC2R, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC2VisHC2RvHC2R, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC2VisHC2VvHC2V, -- Can't happen w/UpStile

    NULL,   // &PrimFnDydUpStileHC4IisHC4IvHC4I, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC4IisHC4FvHC4F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC4FisHC4IvHC4I, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC4FisHC4FvHC4F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC4RisHC4RvHC4R, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC4VisHC4RvHC4R, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC4VisHC4VvHC4V, -- Can't happen w/UpStile

    NULL,   // &PrimFnDydUpStileHC8IisHC8IvHC8I, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC8IisHC8FvHC8F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC8FisHC8IvHC8I, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC8FisHC8FvHC8F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC8RisHC8RvHC8R, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC8VisHC8RvHC8R, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileHC8VisHC8VvHC8V, -- Can't happen w/UpStile

    // Dyadic BAxF result functions (indexable)
    &PrimFnDydUpStileBA1FisBA1FvBA1F,
    NULL,   // &PrimFnDydUpStileBA2FisBA2FvBA2F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBA4FisBA4FvBA4F, -- Can't happen w/UpStile
    NULL,   // &PrimFnDydUpStileBA8FisBA8FvBA8F, -- Can't happen w/UpStile

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonUpStileB64isB64, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileB32isB32, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileB16isB16, -- Can't happen w/UpStile
    NULL,   // &PrimFnMonUpStileB08isB08, -- Can't happen w/UpStile

    // Dyadic Boolean chunk functions
    &PrimFnDydDownCaretB64isB64vB64,
    &PrimFnDydDownCaretB32isB32vB32,
    &PrimFnDydDownCaretB16isB16vB16,
    &PrimFnDydDownCaretB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpStile};
#endif


//***************************************************************************
//  $PrimFnUpStile_EM_YY
//
//  Primitive function for monadic and dyadic UpStile ("ceiling" and "maximum")
//***************************************************************************

LPPL_YYSTYPE PrimFnUpStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPSTILE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpStile_EM


//***************************************************************************
//  $PrimSpecUpStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpStileStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    return
      PrimSpecDownStileStorageTypeMon (aplNELMRht,
                                       lpaplTypeRht,
                                       lptkFunc);
} // End PrimSpecUpStileStorageTypeMon


//***************************************************************************
//  $PrimFnMonUpStileBisB
//
//  Primitive scalar function monadic UpStile:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonUpStileBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonUpStileBisB


//***************************************************************************
//  $PrimFnMonUpStileIisI
//
//  Primitive scalar function monadic UpStile:  I {is} fn I
//***************************************************************************

void PrimFnMonUpStileIisI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = lpatRht->aplInteger;
} // End PrimFnMonUpStileIisI


//***************************************************************************
//  $PrimFnMonUpStileIisF
//
//  Primitive scalar function monadic UpStile:  I {is} fn F
//***************************************************************************

void PrimFnMonUpStileIisF
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for PoM infinity, NaNs, and numbers whose
    //   absolute value is >= 2*53
    if (!_finite (lpatRht->aplFloat)
     || fabs (lpatRht->aplFloat) >= Float2Pow53)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    else
    {
        APLFLOAT aplFloatRes;

        // No need to set enumCT as this is UpStile which always uses []CT
        PrimFnMonUpStileFisF (&aplFloatRes, 0, lpatRht, NULL);

        // Check for range
        if (fabs (aplFloatRes) > MAX_APLINT)
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

        // Save in the result
        lpMemRes[uRes] = (APLINT) aplFloatRes;
    } // End IF/ELSE
} // End PrimFnMonUpStileIisF


//***************************************************************************
//  $PrimFnMonUpStileFisF
//
//  Primitive scalar function monadic UpStile:  F {is} fn F
//***************************************************************************

void PrimFnMonUpStileFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLFLOAT aplFloatRes;
    ALLTYPES atTmp = {0};

    // Save in a temp
    atTmp.aplFloat = -lpatRht->aplFloat;

    // Use the code in DownStile
    // No need to set enumCT as this is DownStile which always uses []CT
    PrimFnMonDownStileFisF (&aplFloatRes, 0, &atTmp, NULL);

    // Save in the result
    lpMemRes[uRes] = -aplFloatRes;
} // End PrimFnMonUpStileFisF


//***************************************************************************
//  $PrimFnMonUpStileRisR
//
//  Primitive scalar function monadic UpStile:  R {is} fn R
//***************************************************************************

void PrimFnMonUpStileRisR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // ***FIXME*** -- Handle -0

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_RAT]) (&lpatRht->aplRat, 0, &atRht);

    // Negate the temp to use with DownStile
    mpq_neg (&atRht.aplRat, &lpatRht->aplRat);

    // Use the code in DownStile
    // No need to set enumCT as this is DownStile which always uses []CT
    PrimFnMonDownStileRisR (lpMemRes, uRes, &atRht, NULL);

    // Negate the result after calling RisR
    mpq_neg (&lpMemRes[uRes], &lpMemRes[uRes]);

    // We no longer need this storage
    Myq_clear (&atRht.aplRat);
} // End PrimFnMonUpStileRisR


//***************************************************************************
//  $PrimFnMonUpStileVisR
//
//  Primitive scalar function monadic UpStile:  V {is} fn R
//***************************************************************************

void PrimFnMonUpStileVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // ***FIXME*** -- Handle -0

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    // Negate the temp to use with DownStile
    mpfr_neg (&atRht.aplVfp, &lpatRht->aplVfp, MPFR_RNDN);

    // Use the code in DownStile
    // No need to set enumCT as this is DownStile which always uses []CT
    PrimFnMonDownStileVisV (lpMemRes, uRes, &atRht, NULL);

    // Negate the result after calling VisV
    mpfr_neg (&lpMemRes[uRes], &lpMemRes[uRes], MPFR_RNDN);

    // We no longer need this storage
    Myf_clear (&atRht.aplVfp);
} // End PrimFnMonUpStileVisR


//***************************************************************************
//  $CeilHC1V
//***************************************************************************

APLHC1V CeilHC1V
    (APLHC1V aplRht)                    // Right arg

{
    APLHC1V aplNeg,
            aplFlr,
            aplRes;

    // ***FIXME*** -- Handle -0

    // Negate the temp to use with DownStile
    aplNeg = NegHC1V_RE (aplRht);

    // Use the code in DownStile
    aplFlr = FloorHC1V (aplNeg);

    // Negate the temp to return as the result
    aplRes = NegHC1V_RE (aplFlr);

    // Free the temps
    Myhc1v_clear (&aplFlr);
    Myhc1v_clear (&aplNeg);

    return aplRes;
} // End CeilHC1V


//***************************************************************************
//  $PrimFnMonUpStileVisV
//
//  Primitive scalar function monadic UpStile:  V {is} fn V
//***************************************************************************

void PrimFnMonUpStileVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = CeilHC1V (lpatRht->aplHC1V);
} // End PrimFnMonUpStileVisV


//***************************************************************************
//  $PrimFnMonUpStileBA1FisBA1F
//
//  Primitive scalar function monadic UpStile:  BA1F {is} fn BA1F
//***************************************************************************

void PrimFnMonUpStileBA1FisBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // ***FIXME*** -- Handle -0

    // Initialize the temp to 0/0
    Myarb_init (&atRht.aplArb);

    // Negate the temp to use with DownStile
    arb_neg (&atRht.aplArb, &lpatRht->aplArb);

    // Use the code in DownStile
    // No need to set enumCT as this is DownStile which always uses []CT
    PrimFnMonDownStileBA1FisBA1F (lpMemRes, uRes, &atRht, NULL);

    // Negate the result after calling BA1FisBA1F
    arb_neg (&lpMemRes[uRes], &lpMemRes[uRes]);

    // We no longer need this storage
    Myarb_clear (&atRht.aplArb);
} // End PrimFnMonUpStileBA1FisBA1F


//***************************************************************************
//  $CeilHC2F
//***************************************************************************

APLHC2F CeilHC2F
    (APLHC2F aplRht)                    // Right arg

{
    // No exceptions in this code

    // Use the code in DownStile
    return NegHC2F_RE (FloorHC2F (NegHC2F_RE (aplRht)));
} // End CeilHC2F


//***************************************************************************
//  $CeilHC4F
//***************************************************************************

APLHC4F CeilHC4F
    (APLHC4F aplRht)                    // Right arg

{
    // No exceptions in this code

    // Use the code in DownStile
    return NegHC4F_RE (FloorHC4F (NegHC4F_RE (aplRht)));
} // End CeilHC4F


//***************************************************************************
//  $CeilHC2R
//***************************************************************************

APLHC2R CeilHC2R
    (APLHC2R aplRht)                    // Right arg

{
    APLHC2R aplNeg,
            aplFlr,
            aplRes;

    // No exceptions in this code

    // Negate the temp to use with DownStile
    aplNeg = NegHC2R_RE (aplRht);

    // Use the code in DownStile
    aplFlr = FloorHC2R (aplNeg);

    // Negate the temp to return as the result
    aplRes = NegHC2R_RE (aplFlr);

    // Free the temps
    Myhc2r_clear (&aplFlr);
    Myhc2r_clear (&aplNeg);

    return aplRes;
} // End CeilHC2R


//***************************************************************************
//  $CeilHC4R
//***************************************************************************

APLHC4R CeilHC4R
    (APLHC4R aplRht)                    // Right arg

{
    APLHC4R aplNeg,
            aplFlr,
            aplRes;

    // No exceptions in this code

    // Negate the temp to use with DownStile
    aplNeg = NegHC4R_RE (aplRht);

    // Use the code in DownStile
    aplFlr = FloorHC4R (aplNeg);

    // Negate the temp to return as the result
    aplRes = NegHC4R_RE (aplFlr);

    // Free the temps
    Myhc4r_clear (&aplFlr);
    Myhc4r_clear (&aplNeg);

    return aplRes;
} // End CeilHC4R


//***************************************************************************
//  $CeilHC2V
//***************************************************************************

APLHC2V CeilHC2V
    (APLHC2V aplRht)                    // Right arg

{
    APLHC2V aplNeg,
            aplFlr,
            aplRes;

    // No exceptions in this code

    // Negate the temp to use with DownStile
    aplNeg = NegHC2V_RE (aplRht);

    // Use the code in DownStile
    aplFlr = FloorHC2V (aplNeg);

    // Negate the temp to return as the result
    aplRes = NegHC2V_RE (aplFlr);

    // Free the temps
    Myhc2v_clear (&aplFlr);
    Myhc2v_clear (&aplNeg);

    return aplRes;
} // End CeilHC2V


//***************************************************************************
//  $CeilHC4V
//***************************************************************************

APLHC4V CeilHC4V
    (APLHC4V aplRht)                    // Right arg

{
    APLHC4V aplNeg,
            aplFlr,
            aplRes;

    // No exceptions in this code

    // Negate the temp to use with DownStile
    aplNeg = NegHC4V_RE (aplRht);

    // Use the code in DownStile
    aplFlr = FloorHC4V (aplNeg);

    // Negate the temp to return as the result
    aplRes = NegHC4V_RE (aplFlr);

    // Free the temps
    Myhc4v_clear (&aplFlr);
    Myhc4v_clear (&aplNeg);

    return aplRes;
} // End CeilHC4V


//***************************************************************************
//  $CeilBA2F
//***************************************************************************

APLBA2F CeilBA2F
    (APLBA2F aplRht)                    // Right arg

{
    APLBA2F aplNeg,
            aplFlr,
            aplRes;

    // No exceptions in this code

    // Negate the temp to use with DownStile
    aplNeg = NegBA2F_RE (aplRht);

    // Use the code in DownStile
    aplFlr = FloorBA2F (aplNeg);

    // Negate the temp to return as the result
    aplRes = NegBA2F_RE (aplFlr);

    // Free the temps
    arb2f_clear (&aplFlr);
    arb2f_clear (&aplNeg);

    return aplRes;
} // End CeilBA2F


//***************************************************************************
//  $CeilBA4F
//***************************************************************************

APLBA4F CeilBA4F
    (APLBA4F aplRht)                    // Right arg

{
    APLBA4F aplNeg,
            aplFlr,
            aplRes;

    // No exceptions in this code

    // Negate the temp to use with DownStile
    aplNeg = NegBA4F_RE (aplRht);

    // Use the code in DownStile
    aplFlr = FloorBA4F (aplNeg);

    // Negate the temp to return as the result
    aplRes = NegBA4F_RE (aplFlr);

    // Free the temps
    arb4f_clear (&aplFlr);
    arb4f_clear (&aplNeg);

    return aplRes;
} // End CeilBA4F


//***************************************************************************
//  $PrimFnMonUpStileHC2IisHC2I
//
//  Primitive scalar function monadic UpStile:  HC2I {is} fn HC2I
//***************************************************************************

void PrimFnMonUpStileHC2IisHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = lpatRht->aplHC2I;
} // End PrimFnMonUpStileHC2IisHC2I


//***************************************************************************
//  $PrimFnMonUpStileHC4IisHC4I
//
//  Primitive scalar function monadic UpStile:  HC4I {is} fn HC4I
//***************************************************************************

void PrimFnMonUpStileHC4IisHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = lpatRht->aplHC4I;
} // End PrimFnMonUpStileHC4IisHC4I


//***************************************************************************
//  $PrimFnMonUpStileHC2FisHC2I
//
//  Primitive scalar function monadic UpStile:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonUpStileHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Save in the result
        lpMemRes[uRes].parts[i] = (APLFLOAT) lpatRht->aplHC2I.parts[i];
} // End PrimFnMonUpStileHC2FisHC2I


//***************************************************************************
//  $PrimFnMonUpStileHC4FisHC4I
//
//  Primitive scalar function monadic UpStile:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonUpStileHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Save in the result
        lpMemRes[uRes].parts[i] = (APLFLOAT) lpatRht->aplHC4I.parts[i];
} // End PrimFnMonUpStileHC4FisHC4I


//***************************************************************************
//  $PrimFnMonUpStileHC2IisHC2F
//
//  Primitive scalar function monadic UpStile:  HC2I {is} fn HC2F
//***************************************************************************

void PrimFnMonUpStileHC2IisHC2F
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLHC2F aplRes;
    int     i;
    UBOOL   bRet;

    // Calculate the floor
    aplRes = CeilHC2F (lpatRht->aplHC2F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Save in the result
        lpMemRes[uRes].parts[i] = ConvertToInteger_SCT (ARRAY_FLOAT, &aplRes.parts[i], 0, &bRet);

        Assert (bRet);
    } // End FOR
} // End PrimFnMonUpStileHC2IisHC2F


//***************************************************************************
//  $PrimFnMonUpStileHC4IisHC4F
//
//  Primitive scalar function monadic UpStile:  HC4I {is} fn HC4F
//***************************************************************************

void PrimFnMonUpStileHC4IisHC4F
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLHC4F aplRes;
    int     i;
    UBOOL   bRet;

    // Calculate the floor
    aplRes = CeilHC4F (lpatRht->aplHC4F);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Save in the result
        lpMemRes[uRes].parts[i] = ConvertToInteger_SCT (ARRAY_FLOAT, &aplRes.parts[i], 0, &bRet);

        Assert (bRet);
    } // End FOR
} // End PrimFnMonUpStileHC4IisHC4F


//***************************************************************************
//  $PrimFnMonUpStileHC2FisHC2F
//
//  Primitive scalar function monadic UpStile:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonUpStileHC2FisHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = CeilHC2F (lpatRht->aplHC2F);
} // End PrimFnMonUpStileHC2FisHC2F


//***************************************************************************
//  $PrimFnMonUpStileHC4FisHC4F
//
//  Primitive scalar function monadic UpStile:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonUpStileHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = CeilHC4F (lpatRht->aplHC4F);
} // End PrimFnMonUpStileHC4FisHC4F


//***************************************************************************
//  $PrimFnMonUpStileHC2RisHC2R
//
//  Primitive scalar function monadic UpStile:  HC2R {is} fn HC2R
//***************************************************************************

void PrimFnMonUpStileHC2RisHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = CeilHC2R (lpatRht->aplHC2R);
} // End PrimFnMonUpStileHC2RisHC2R


//***************************************************************************
//  $PrimFnMonUpStileHC4RisHC4R
//
//  Primitive scalar function monadic UpStile:  HC4R {is} fn HC4R
//***************************************************************************

void PrimFnMonUpStileHC4RisHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = CeilHC4R (lpatRht->aplHC4R);
} // End PrimFnMonUpStileHC4RisHC4R


//***************************************************************************
//  $PrimFnMonUpStileHC2VisHC2R
//
//  Primitive scalar function monadic UpStile:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonUpStileHC2VisHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplRat, 0, &atRht);

    // Save in the result
    PrimFnMonUpStileHC2VisHC2V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
} // End PrimFnMonUpStileHC2VisHC2R


//***************************************************************************
//  $PrimFnMonUpStileHC4VisHC4R
//
//  Primitive scalar function monadic UpStile:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonUpStileHC4VisHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplRat, 0, &atRht);

    // Save in the result
    PrimFnMonUpStileHC4VisHC4V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
} // End PrimFnMonUpStileHC4VisHC4R


//***************************************************************************
//  $PrimFnMonUpStileHC2VisHC2V
//
//  Primitive scalar function monadic UpStile:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonUpStileHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = CeilHC2V (lpatRht->aplHC2V);
} // End PrimFnMonUpStileHC2VisHC2V


//***************************************************************************
//  $PrimFnMonUpStileHC4VisHC4V
//
//  Primitive scalar function monadic UpStile:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonUpStileHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = CeilHC4V (lpatRht->aplHC4V);
} // End PrimFnMonUpStileHC4VisHC4V


//***************************************************************************
//  $PrimFnMonUpStileBA2FisBA2F
//
//  Primitive scalar function monadic UpStile:  BA2F {is} fn BA2F
//***************************************************************************

void PrimFnMonUpStileBA2FisBA2F
    (LPAPLBA2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = CeilBA2F (lpatRht->aplBA2F);
} // End PrimFnMonUpStileBA2FisBA2F


//***************************************************************************
//  $PrimFnMonUpStileBA4FisBA4F
//
//  Primitive scalar function monadic UpStile:  BA4F {is} fn BA4F
//***************************************************************************

void PrimFnMonUpStileBA4FisBA4F
    (LPAPLBA4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = CeilBA4F (lpatRht->aplBA4F);
} // End PrimFnMonUpStileBA4FisBA4F


//***************************************************************************
//  $PrimFnMonUpStileAPA_EM
//
//  Monadic UpStile, result is APA
//***************************************************************************

UBOOL PrimFnMonUpStileAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to result token (may be NULL)

     LPTOKEN      lptkFunc,         // Ptr to function token

     HGLOBAL      hGlbRht,          // Right arg handle
     HGLOBAL     *lphGlbRes,        // Ptr to result handle

     APLRANK      aplRankRht,       // Right arg rank

     LPPRIMSPEC   lpPrimSpec)       // Ptr to local PRIMSPEC

{
    // Axis may be anything

    // Copy the HGLOBAL to the result
    *lphGlbRes = CopySymGlbDirAsGlb (hGlbRht);

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = hGlbRht;
    } // End IF

    return TRUE;
} // End PrimFnMonUpStileAPA_EM


//***************************************************************************
//  $PrimSpecUpStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpStileStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    return
      PrimSpecDownStileStorageTypeDyd (aplNELMLft,
                                       lpaplTypeLft,
                                       lptkFunc,
                                       aplNELMRht,
                                       lpaplTypeRht);
} // End PrimSpecUpStileStorageTypeDyd


//***************************************************************************
//  $PrimFnDydUpStileIisIvI
//
//  Primitive scalar function dyadic UpStile:  I {is} I fn I
//***************************************************************************

void PrimFnDydUpStileIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = max (lpatLft->aplInteger, lpatRht->aplInteger);
} // End PrimFnDydUpStileIisIvI


//***************************************************************************
//  $PrimFnDydUpStileFisFvF
//
//  Primitive scalar function dyadic UpStile:  F {is} F fn F
//***************************************************************************

void PrimFnDydUpStileFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = max (lpatLft->aplFloat, lpatRht->aplFloat);
} // End PrimFnDydUpStileFisFvF


//***************************************************************************
//  $PrimFnDydUpStileRisRvR
//
//  Primitive scalar function dyadic UpStile:  R {is} R fn R
//***************************************************************************

void PrimFnDydUpStileRisRvR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Compare the two Rationals
    if (mpq_cmp (&lpatLft->aplRat, &lpatRht->aplRat) > 0)
        mpq_init_set (&lpMemRes[uRes], &lpatLft->aplRat);
    else
        mpq_init_set (&lpMemRes[uRes], &lpatRht->aplRat);
} // End PrimFnDydUpStileRisRvR


//***************************************************************************
//  $PrimFnDydUpStileVisRvR
//
//  Primitive scalar function dyadic UpStile:  V {is} R fn R
//***************************************************************************

void PrimFnDydUpStileVisRvR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Compare the two Variable FPs
    if (mpq_cmp (&lpatLft->aplRat, &lpatRht->aplRat) > 0)
        mpfr_init_set_q (&lpMemRes[uRes], &lpatLft->aplRat, MPFR_RNDN);
    else
        mpfr_init_set_q (&lpMemRes[uRes], &lpatRht->aplRat, MPFR_RNDN);
} // End PrimFnDydUpStileVisRvR


//***************************************************************************
//  $PrimFnDydUpStileVisVvV
//
//  Primitive scalar function dyadic UpStile:  V {is} V fn V
//***************************************************************************

void PrimFnDydUpStileVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Compare the two Variable FPs
    if (mpfr_cmp (&lpatLft->aplVfp, &lpatRht->aplVfp) > 0)
        mpfr_init_copy (&lpMemRes[uRes], &lpatLft->aplVfp);
    else
        mpfr_init_copy (&lpMemRes[uRes], &lpatRht->aplVfp);
} // End PrimFnDydUpStileVisVvV


//***************************************************************************
//  $PrimFnDydUpStileBA1FisBA1FvBA1F
//
//  Primitive scalar function dyadic UpStile:  BA1F {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydUpStileBA1FisBA1FvBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize to 0/0
    Myarb_init (&lpMemRes[uRes]);

    // Calculate the maximum
    arb_max (&lpMemRes[uRes], &lpatLft->aplArb, &lpatRht->aplArb, prec);
} // End PrimFnDydUpStileBA1FisBA1FvBA1F


//***************************************************************************
//  End of File: pf_ustile.c
//***************************************************************************
