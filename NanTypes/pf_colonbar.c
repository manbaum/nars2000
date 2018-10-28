//***************************************************************************
//  NARS2000 -- Primitive Function -- ColonBar
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
PRIMSPEC PrimSpecColonBar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecColonBarStorageTypeMon,
    NULL,   // &PrimFnMonColonBarAPA_EM, -- Can't happen w/ColonBar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonColonBarBisB, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarBisI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarBisF, -- Can't happen w/ColonBar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonColonBarIisI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarIisF, -- Can't happen w/ColonBar
    &PrimFnMonColonBarFisI,
    &PrimFnMonColonBarFisF,
    &PrimFnMonColonBarRisR,
    &PrimFnMonColonBarVisR,
    &PrimFnMonColonBarVisV,

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonColonBarHC2IisHC2I, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarHC2IisHC2F, -- Can't happen w/ColonBar
    &PrimFnMonColonBarHC2FisHC2I,
    &PrimFnMonColonBarHC2FisHC2F,
    &PrimFnMonColonBarHC2RisHC2R,
    &PrimFnMonColonBarHC2VisHC2R,
    &PrimFnMonColonBarHC2VisHC2V,

    NULL,   // &PrimFnMonColonBarHC4IisHC4I, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarHC4IisHC4F, -- Can't happen w/ColonBar
    &PrimFnMonColonBarHC4FisHC4I,
    &PrimFnMonColonBarHC4FisHC4F,
    &PrimFnMonColonBarHC4RisHC4R,
    &PrimFnMonColonBarHC4VisHC4R,
    &PrimFnMonColonBarHC4VisHC4V,

    NULL,   // &PrimFnMonColonBarHC8IisHC8I, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarHC8IisHC8F, -- Can't happen w/ColonBar
    &PrimFnMonColonBarHC8FisHC8I,
    &PrimFnMonColonBarHC8FisHC8F,
    &PrimFnMonColonBarHC8RisHC8R,
    &PrimFnMonColonBarHC8VisHC8R,
    &PrimFnMonColonBarHC8VisHC8V,

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonColonBarFisHC2I, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarFisHC2F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarVisHC2R, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarVisHC2V, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarFisHC4I, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarFisHC4F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarVisHC4R, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarVisHC4V, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarFisHC8I, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarFisHC8F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarVisHC8R, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarVisHC8V, -- Can't happen w/ColonBar

    // Monadic BAxF result BAxF arg functions (indexable)
    &PrimFnMonColonBarBA1FisBA1F,
    &PrimFnMonColonBarBA2FisBA2F,
    &PrimFnMonColonBarBA4FisBA4F,
    &PrimFnMonColonBarBA8FisBA8F,
    &PrimFnMonColonBarBA1FisBA1F,
    NULL,   // &PrimFnMonColonBarARBvBA2F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarARBvBA4F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarARBvBA8F, -- Can't happen w/ColonBar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecColonBarStorageTypeDyd,
    NULL,   // &PrimFnDydColonBarAPA_EM, -- Can't happen w/ColonBar

    // Dyadic Boolean result functions
    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydColonBarBisIvI, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisFvF, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisCvC, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisHvH, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisRvR, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisVvV, -- Can't happen w/ColonBar

    NULL,   // &PrimFnDydColonBarBisHC2IvHC2I, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisHC2FvHC2F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisHC2RvHC2R, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisHC2VvHC2V, -- Can't happen w/ColonBar

    NULL,   // &PrimFnDydColonBarBisHC4IvHC4I, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisHC4FvHC4F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisHC4RvHC4R, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisHC4VvHC4V, -- Can't happen w/ColonBar

    NULL,   // &PrimFnDydColonBarBisHC8IvHC8I, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisHC8FvHC8F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisHC8RvHC8R, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisHC8VvHC8V, -- Can't happen w/ColonBar

    // Dyadic Boolean result functions with ARBs (indexable)
    NULL,   // &PrimFnDydColonBarBisBA1FvBA1F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisBA2FvBA2F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisBA4FvBA4F, -- Can't happen w/ColonBar
    NULL,   // &PrimFnDydColonBarBisBA8FvBA8F, -- Can't happen w/ColonBar

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydColonBarIisIvI,
    NULL,   // &PrimFnDydColonBarIisFvF, -- Can't happen w/ColonBar
    &PrimFnDydColonBarFisIvI,
    &PrimFnDydColonBarFisFvF,
    &PrimFnDydColonBarRisRvR,
    &PrimFnDydColonBarVisRvR,
    &PrimFnDydColonBarVisVvV,

    &PrimFnDydColonBarHC2IisHC2IvHC2I,
    NULL,   // &PrimFnDydColonBarHC2IisHC2FvHC2F, -- Can't happen w/ColonBar
    &PrimFnDydColonBarHC2FisHC2IvHC2I,
    &PrimFnDydColonBarHC2FisHC2FvHC2F,
    &PrimFnDydColonBarHC2RisHC2RvHC2R,
    &PrimFnDydColonBarHC2VisHC2RvHC2R,
    &PrimFnDydColonBarHC2VisHC2VvHC2V,

    &PrimFnDydColonBarHC4IisHC4IvHC4I,
    NULL,   // &PrimFnDydColonBarHC4IisHC4FvHC4F, -- Can't happen w/ColonBar
    &PrimFnDydColonBarHC4FisHC4IvHC4I,
    &PrimFnDydColonBarHC4FisHC4FvHC4F,
    &PrimFnDydColonBarHC4RisHC4RvHC4R,
    &PrimFnDydColonBarHC4VisHC4RvHC4R,
    &PrimFnDydColonBarHC4VisHC4VvHC4V,

    &PrimFnDydColonBarHC8IisHC8IvHC8I,
    NULL,   // &PrimFnDydColonBarHC8IisHC8FvHC8F, -- Can't happen w/ColonBar
    &PrimFnDydColonBarHC8FisHC8IvHC8I,
    &PrimFnDydColonBarHC8FisHC8FvHC8F,
    &PrimFnDydColonBarHC8RisHC8RvHC8R,
    &PrimFnDydColonBarHC8VisHC8RvHC8R,
    &PrimFnDydColonBarHC8VisHC8VvHC8V,

    // Dyadic BAxF result functions (indexable)
    &PrimFnDydColonBarBA1FisBA1FvBA1F,
    &PrimFnDydColonBarBA2FisBA2FvBA2F,
    &PrimFnDydColonBarBA4FisBA4FvBA4F,
    &PrimFnDydColonBarBA8FisBA8FvBA8F,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonColonBarB64isB64, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarB32isB32, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarB16isB16, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarB08isB08, -- Can't happen w/ColonBar

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonColonBarB64isB64vB64, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarB32isB32vB32, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarB16isB16vB16, -- Can't happen w/ColonBar
    NULL,   // &PrimFnMonColonBarB08isB08vB08, -- Can't happen w/ColonBar

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecColonBar};
#endif


//***************************************************************************
//  $PrimFnColonBar_EM_YY
//
//  Primitive function for monadic and dyadic ColonBar ("reciprocal" and "division")
//***************************************************************************

LPPL_YYSTYPE PrimFnColonBar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_COLONBAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnColonBar_EM_YY


//***************************************************************************
//  $PrimSpecColonBarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecColonBarStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE aplTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Weed out chars & heteros
    if (IsSimpleCH (*lpaplTypeRht))
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    aplTypeRes = *lpaplTypeRht;

    // Except that BOOL, INT and APA become FLOAT
    if (IsRealBIA (aplTypeRes))
        return ARRAY_FLOAT;

    // Except that HCxI becomes HCxF
    if (IsAnyInt (aplTypeRes))
        return aplTypeRes + 1;          // Assuming same order as in <ARRAY_TYPES>

    return aplTypeRes;
} // End PrimSpecColonBarStorageTypeMon


//***************************************************************************
//  $PrimFnMonColonBarFisI
//
//  Primitive scalar function monadic ColonBar:  F {is} fn I
//***************************************************************************

void PrimFnMonColonBarFisI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  {div} 0
    if (lpatRht->aplInteger EQ 0)
        lpMemRes[uRes] =
          TranslateQuadICIndex (0,
                                ICNDX_DIV0,
                     (APLFLOAT) lpatRht->aplInteger,
                                FALSE);
    else
        // The FPU handles overflow and underflow for us
        lpMemRes[uRes] = (1 / (APLFLOAT) lpatRht->aplInteger);
} // End PrimFnMonColonBarFisI


//***************************************************************************
//  $PrimFnMonColonBarFisF
//
//  Primitive scalar function monadic ColonBar:  F {is} fn F
//***************************************************************************

void PrimFnMonColonBarFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  {div} 0
    if (lpatRht->aplFloat EQ 0)
        lpMemRes[uRes] =
          TranslateQuadICIndex (0,
                                ICNDX_DIV0,
                                lpatRht->aplFloat,
                                SIGN_APLFLOAT (lpatRht->aplFloat));
    else
        // The FPU handles overflow and underflow for us
        lpMemRes[uRes] = (1 / lpatRht->aplFloat);
} // End PrimFnMonColonBarFisF


//***************************************************************************
//  $PrimFnMonColonBarRisR
//
//  Primitive scalar function monadic ColonBar:  R {is} fn R
//***************************************************************************

void PrimFnMonColonBarRisR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  {div} 0
    if (IsMpq0 (&lpatRht->aplRat))
        lpMemRes[uRes] =
          *mpq_QuadICValue (&lpatRht->aplRat,        // No left arg
                             ICNDX_DIV0,
                            &lpatRht->aplRat,
                            &lpMemRes[uRes],
                             FALSE);
    else
    // Check for special case:  {div}{neg}{inf}x
    if (IsMpqInfinity (&lpatRht->aplRat)
     && SIGN_APLRAT (&lpatRht->aplRat))
        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);
    else
    {
        // Initialize the result to 0/1
        mpq_init (&lpMemRes[uRes]);

        // Invert the Rational
        mpq_inv (&lpMemRes[uRes], &lpatRht->aplRat);
    } // End IF/ELSE
} // End PrimFnMonColonBarRisR


//***************************************************************************
//  $PrimFnMonColonBarVisR
//
//  Primitive scalar function monadic ColonBar:  V {is} fn R
//***************************************************************************

void PrimFnMonColonBarVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonColonBarVisV (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
} // End PrimFnMonColonBarVisR


//***************************************************************************
//  $PrimFnMonColonBarVisV
//
//  Primitive scalar function monadic ColonBar:  V {is} fn V
//***************************************************************************

void PrimFnMonColonBarVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  {div} 0
    if (IsMpf0 (&lpatRht->aplVfp))
        lpMemRes[uRes] =
          *mpfr_QuadICValue (&lpatRht->aplVfp,       // No left arg
                              ICNDX_DIV0,
                             &lpatRht->aplVfp,
                             &lpMemRes[uRes],
                              SIGN_APLVFP (&lpatRht->aplVfp));
    else
    {
        // Initialize the result to 0
        mpfr_init0 (&lpMemRes[uRes]);

        // Invert the Variable FP
        mpfr_inv (&lpMemRes[uRes], &lpatRht->aplVfp, MPFR_RNDN);
    } // End IF/ELSE
} // End PrimFnMonColonBarVisV


//***************************************************************************
//  $PrimFnMonColonBarBA1FisBA1F
//
//  Primitive scalar function monadic ColonBar:  BA1F {is} fn BA1F
//***************************************************************************

void PrimFnMonColonBarBA1FisBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  {div} 0
    if (IsArb0 (&lpatRht->aplArb))
        lpMemRes[uRes] =
          *arb_QuadICValue (&lpatRht->aplArb,       // No left arg
                             ICNDX_DIV0,
                            &lpatRht->aplArb,
                            &lpMemRes[uRes],
                             SIGN_APLARB (&lpatRht->aplArb));
    else
    {
        // Initialize the result to 0/0
        Myarb_init (&lpMemRes[uRes]);

        // Invert the ARB
        arb_inv (&lpMemRes[uRes], &lpatRht->aplArb, prec);
    } // End IF/ELSE
} // End PrimFnMonColonBarBA1FisBA1F


//***************************************************************************
//  $PrimFnMonColonBarHC2FisHC2I
//
//  Primitive scalar function monadic ColonBar:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonColonBarHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert from HC2I to HC2F
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonColonBarHC2FisHC2F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonColonBarHC2FisHC2I


//***************************************************************************
//  $PrimFnMonColonBarHC2FisHC2F
//
//  Primitive scalar function monadic ColonBar:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonColonBarHC2FisHC2F
    (LPAPLHC2F lpMemRes,            // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int      i;                 // Loop counter
    APLFLOAT aplDen;            // Denominator
    APLHC2F  aplNum;            // Numerator

    // Calculate the numerator
    aplNum = ConjHC2F (lpatRht->aplHC2F);

    // Calculate the denominator
    aplDen = SqNrmHC2F (&lpatRht->aplHC2F);

    // If the denominator is 0, ...
    if (aplDen EQ 0.0)
    {
        lpMemRes[uRes].parts[0] =
          TranslateQuadICIndex (0,
                                ICNDX_DIV0,
                                lpatRht->aplHC2F.parts[0],
                                SIGN_APLFLOAT (lpatRht->aplHC2F.parts[0]));
        lpMemRes[uRes].parts[1] = 0;
    } else
    // If the denominator is {Inf}, ...
    if (IsFltInfinity (aplDen))
    {
        UBOOL bAllowNeg0 = (UBOOL) gAllowNeg0;

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            lpMemRes[uRes].parts[i] =
              (bAllowNeg0 ? ((SIGN_APLFLOAT_RAW (aplNum.parts[i])
                           EQ SIGN_APLFLOAT_RAW (aplDen)) ?  0.0
                                                          : -0.0)
                          : 0.0);
    } else
    {
        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            // The FPU handles overflow and underflow for us
            lpMemRes[uRes].parts[i] = aplNum.parts[i] / aplDen;
    } // End IF/ELSE
} // End PrimFnMonColonBarHC2FisHC2F


//***************************************************************************
//  $InvHC2R_RE
//***************************************************************************

APLHC2R InvHC2R_RE
    (APLHC2R aplRht)                    // Right arg

{
    int     i;                  // Loop counter
    APLRAT  aplDen = {0};       // Denominator
    APLHC2R aplNum,             // Numerator
            aplRes = {0};
    UBOOL   bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Calculate the numerator
    aplNum = ConjHC2R (aplRht);

    // Calculate the denominator
    aplDen = SqNrmHC2R (&aplRht);

    // If the denominator is 0, ...
    if (IsMpq0 (&aplDen))
    {
        aplRes.parts[0] =
          *mpq_QuadICValue (&aplRht.parts[0],     // No left arg
                             ICNDX_DIV0,
                            &aplRht.parts[0],
                            &aplRes.parts[0],
                             FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 2; i++)
            // Initialize to 0/1
            mpq_init (&aplRes.parts[i]);
    } else
    // Check for special case:  {div}{neg}{inf}x
    if (bAllowNeg0
     && IsMpqInfinity (&aplDen)
     && SIGN_APLRAT (&aplDen))
    {
        // Free the temps
        Myhc1r_clear (&aplDen);
        Myhc2r_clear (&aplNum);

        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    } else
    // If the denominator is {Inf}, ...
    if (IsMpqInfinity (&aplDen))
    {
        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            mpq_init_set_d (&aplRes.parts[i],
                            bAllowNeg0 ? ((SIGN_APLRAT (&aplNum.parts[i])
                                        EQ SIGN_APLRAT (&aplDen)) ?  0.0
                                                                  : -0.0)
                                       : 0.0);
    } else
    {
        // Initialize to 0/1
        mphc2r_init (&aplRes);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            mpq_div (&aplRes.parts[i],
                     &aplNum.parts[i],
                     &aplDen);
    } // End IF/ELSE

    // Free the temps
    Myhc1r_clear (&aplDen);
    Myhc2r_clear (&aplNum);

    return aplRes;
} // End InvHC2R_RE


//***************************************************************************
//  $PrimFnMonColonBarHC2RisHC2R
//
//  Primitive scalar function monadic ColonBar:  HC2R {is} fn HC2R
//***************************************************************************

void PrimFnMonColonBarHC2RisHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = InvHC2R_RE (lpatRht->aplHC2R);
} // End PrimFnMonColonBarHC2RisHC2R


//***************************************************************************
//  $PrimFnMonColonBarHC2VisHC2R
//
//  Primitive scalar function monadic ColonBar:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonColonBarHC2VisHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonColonBarHC2VisHC2V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
} // End PrimFnMonColonBarHC2VisHC2R


//***************************************************************************
//  $PrimFnMonColonBarHC2VisHC2V
//
//  Primitive scalar function monadic ColonBar:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonColonBarHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int     i;                  // Loop counter
    APLVFP  aplDen = {0};       // Denominator
    APLHC2V aplNum;             // Numerator
    UBOOL   bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Calculate the numerator
    aplNum = ConjHC2V (lpatRht->aplHC2V);

    // Calculate the denominator
    aplDen = SqNrmHC2V (&lpatRht->aplHC2V);

    // If the denominator is 0, ...
    if (IsMpf0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] =
          *mpfr_QuadICValue (&lpatRht->aplHC2V.parts[0],     // No left arg
                              ICNDX_DIV0,
                             &lpatRht->aplHC2V.parts[0],
                             &lpMemRes[uRes].parts[0],
                              FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 2; i++)
            // Initialize to 0
            mpfr_init0 (&lpMemRes[uRes].parts[i]);
    } else
    // If the denominator is {Inf}, ...
    if (IsMpfInfinity (&aplDen))
    {
        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            mpfr_init_set_d (&lpMemRes[uRes].parts[i],
                              bAllowNeg0 ? ((SIGN_APLVFP_RAW (&aplNum.parts[i])
                                          EQ SIGN_APLVFP_RAW (&aplDen)) ?  0.0
                                                                        : -0.0)
                                         : 0.0,
                              MPFR_RNDN);
    } else
    {
        // Initialize to 0
        mphc2v_init0 (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            mpfr_div (&lpMemRes[uRes].parts[i],
                      &aplNum.parts[i],
                      &aplDen,
                       MPFR_RNDN);
    } // End IF/ELSE

    // Free the temps
    Myhc1v_clear (&aplDen);
    Myhc2v_clear (&aplNum);
} // End PrimFnMonColonBarHC2VisHC2V


//***************************************************************************
//  $PrimFnMonColonBarBA2FisBA2F
//
//  Primitive scalar function monadic ColonBar:  BA2F {is} fn BA2F
//***************************************************************************

void PrimFnMonColonBarBA2FisBA2F
    (LPAPLBA2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int     i;                  // Loop counter
    APLARB  aplDen = {0};       // Denominator
    APLBA2F aplNum;             // Numerator
    UBOOL   bAllowNeg0 = (UBOOL) gAllowNeg0;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Calculate the numerator
    aplNum = ConjBA2F (lpatRht->aplBA2F);

    // Calculate the denominator
    aplDen = SqNrmBA2F (&lpatRht->aplBA2F);

    // If the denominator is 0, ...
    if (IsArb0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] =
          *arb_QuadICValue (&lpatRht->aplBA2F.parts[0],     // No left arg
                             ICNDX_DIV0,
                            &lpatRht->aplBA2F.parts[0],
                            &lpMemRes[uRes].parts[0],
                             FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 2; i++)
            // Initialize to 0/0
            Myarb_init (&lpMemRes[uRes].parts[i]);
    } else
    // If the denominator is {Inf}, ...
    if (IsArbInfinity (&aplDen))
    {
        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            arb_init_set_d (&lpMemRes[uRes].parts[i],
                             bAllowNeg0 ? ((SIGN_APLARB_RAW (&aplNum.parts[i])
                                         EQ SIGN_APLARB_RAW (&aplDen)) ?  0.0
                                                                       : -0.0)
                                        : 0.0);
    } else
    {
        mp_limb_signed_t prec = ARB_PREC_FPC;

        // Initialize to 0/0
        arb2f_init (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            arb_div (&lpMemRes[uRes].parts[i],
                     &aplNum.parts[i],
                     &aplDen,
                     prec);
    } // End IF/ELSE

    // Free the temps
    arb1f_clear (&aplDen);
} // End PrimFnMonColonBarBA2FisBA2F


//***************************************************************************
//  $PrimFnMonColonBarHC4FisHC4I
//
//  Primitive scalar function monadic ColonBar:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonColonBarHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert from HC4I to HC4F
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonColonBarHC4FisHC4F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonColonBarHC4FisHC4I


//***************************************************************************
//  $PrimFnMonColonBarHC4FisHC4F
//
//  Primitive scalar function monadic ColonBar:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonColonBarHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int      i;                 // Loop counter
    APLFLOAT aplDen;            // Denominator
    APLHC4F  aplNum;            // Numerator

    // Calculate the numerator
    aplNum = ConjHC4F (lpatRht->aplHC4F);

    // Calculate the denominator
    aplDen = SqNrmHC4F (&lpatRht->aplHC4F);

    // If the denominator is 0, ...
    if (aplDen EQ 0.0)
    {
        lpMemRes[uRes].parts[0] =
          TranslateQuadICIndex (0,
                                ICNDX_DIV0,
                                lpatRht->aplHC4F.parts[0],
                                SIGN_APLFLOAT (lpatRht->aplHC4F.parts[0]));
        lpMemRes[uRes].parts[1] = 0;
    } else
    // If the denominator is {Inf}, ...
    if (IsFltInfinity (aplDen))
    {
        UBOOL bAllowNeg0 = (UBOOL) gAllowNeg0;

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            lpMemRes[uRes].parts[i] =
              (bAllowNeg0 ? ((SIGN_APLFLOAT_RAW (aplNum.parts[i])
                           EQ SIGN_APLFLOAT_RAW (aplDen)) ?  0.0
                                                          : -0.0)
                          : 0.0);
    } else
    {
        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            // The FPU handles overflow and underflow for us
            lpMemRes[uRes].parts[i] = aplNum.parts[i] / aplDen;
    } // End IF/ELSE
} // End PrimFnMonColonBarHC4FisHC4F


//***************************************************************************
//  $InvHC4R_RE
//***************************************************************************

APLHC4R InvHC4R_RE
    (APLHC4R aplRht)                    // Right arg

{
    int     i;                  // Loop counter
    APLRAT  aplDen = {0};       // Denominator
    APLHC4R aplNum,             // Numerator
            aplRes = {0};
    UBOOL   bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Calculate the numerator
    aplNum = ConjHC4R (aplRht);

    // Calculate the denominator
    aplDen = SqNrmHC4R (&aplRht);

    // If the denominator is 0, ...
    if (IsMpq0 (&aplDen))
    {
        aplRes.parts[0] =
          *mpq_QuadICValue (&aplRht.parts[0],     // No left arg
                             ICNDX_DIV0,
                            &aplRht.parts[0],
                            &aplRes.parts[0],
                             FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 4; i++)
            // Initialize to 0/1
            mpq_init (&aplRes.parts[i]);
    } else
    // Check for special case:  {div}{neg}{inf}x
    if (bAllowNeg0
     && IsMpqInfinity (&aplDen)
     && SIGN_APLRAT (&aplDen))
    {
        // Free the temps
        Myhc1r_clear (&aplDen);
        Myhc4r_clear (&aplNum);

        RaiseException (EXCEPTION_RESULT_HC4V, 0, 0, NULL);
    } else
    // If the denominator is {Inf}, ...
    if (IsMpqInfinity (&aplDen))
    {
        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            mpq_init_set_d (&aplRes.parts[i],
                            bAllowNeg0 ? ((SIGN_APLRAT (&aplNum.parts[i])
                                        EQ SIGN_APLRAT (&aplDen)) ?  0.0
                                                                  : -0.0)
                                       : 0.0);
    } else
    {
        // Initialize to 0/1
        mphc4r_init (&aplRes);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            mpq_div (&aplRes.parts[i],
                     &aplNum.parts[i],
                     &aplDen);
    } // End IF/ELSE

    // Free the temps
    Myhc1r_clear (&aplDen);
    Myhc4r_clear (&aplNum);

    return aplRes;
} // End InvHC4R_RE


//***************************************************************************
//  $PrimFnMonColonBarHC4RisHC4R
//
//  Primitive scalar function monadic ColonBar:  HC4R {is} fn HC4R
//***************************************************************************

void PrimFnMonColonBarHC4RisHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = InvHC4R_RE (lpatRht->aplHC4R);
} // End PrimFnMonColonBarHC4RisHC4R


//***************************************************************************
//  $PrimFnMonColonBarHC4VisHC4R
//
//  Primitive scalar function monadic ColonBar:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonColonBarHC4VisHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonColonBarHC4VisHC4V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
} // End PrimFnMonColonBarHC4VisHC4R


//***************************************************************************
//  $PrimFnMonColonBarHC4VisHC4V
//
//  Primitive scalar function monadic ColonBar:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonColonBarHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int     i;                  // Loop counter
    APLVFP  aplDen = {0};       // Denominator
    APLHC4V aplNum;             // Numerator
    UBOOL   bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Calculate the numerator
    aplNum = ConjHC4V (lpatRht->aplHC4V);

    // Calculate the denominator
    aplDen = SqNrmHC4V (&lpatRht->aplHC4V);

    // If the denominator is 0, ...
    if (IsMpf0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] =
          *mpfr_QuadICValue (&lpatRht->aplHC4V.parts[0],     // No left arg
                              ICNDX_DIV0,
                             &lpatRht->aplHC4V.parts[0],
                             &lpMemRes[uRes].parts[0],
                              FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 4; i++)
            // Initialize to 0
            mpfr_init0 (&lpMemRes[uRes].parts[i]);
    } else
    // If the denominator is {Inf}, ...
    if (IsMpfInfinity (&aplDen))
    {
        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            mpfr_init_set_d (&lpMemRes[uRes].parts[i],
                              bAllowNeg0 ? ((SIGN_APLVFP_RAW (&aplNum.parts[i])
                                          EQ SIGN_APLVFP_RAW (&aplDen)) ?  0.0
                                                                        : -0.0)
                                         : 0.0,
                              MPFR_RNDN);
    } else
    {
        // Initialize to 0
        mphc4v_init0 (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            mpfr_div (&lpMemRes[uRes].parts[i],
                      &aplNum.parts[i],
                      &aplDen,
                       MPFR_RNDN);
    } // End IF/ELSE

    // Free the temps
    Myhc1v_clear (&aplDen);
    Myhc4v_clear (&aplNum);
} // End PrimFnMonColonBarHC4VisHC4V


//***************************************************************************
//  $PrimFnMonColonBarBA4FisBA4F
//
//  Primitive scalar function monadic ColonBar:  BA4F {is} fn BA4F
//***************************************************************************

void PrimFnMonColonBarBA4FisBA4F
    (LPAPLBA4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int     i;                  // Loop counter
    APLARB  aplDen = {0};       // Denominator
    APLBA4F aplNum;             // Numerator
    UBOOL   bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Calculate the numerator
    aplNum = ConjBA4F (lpatRht->aplBA4F);

    // Calculate the denominator
    aplDen = SqNrmBA4F (&lpatRht->aplBA4F);

    // If the denominator is 0, ...
    if (IsArb0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] =
          *arb_QuadICValue (&lpatRht->aplBA4F.parts[0],     // No left arg
                             ICNDX_DIV0,
                            &lpatRht->aplBA4F.parts[0],
                            &lpMemRes[uRes].parts[0],
                             FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 4; i++)
            // Initialize to 0/0
            Myarb_init (&lpMemRes[uRes].parts[i]);
    } else
    // If the denominator is {Inf}, ...
    if (IsArbInfinity (&aplDen))
    {
        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            arb_init_set_d (&lpMemRes[uRes].parts[i],
                             bAllowNeg0 ? ((SIGN_APLARB_RAW (&aplNum.parts[i])
                                         EQ SIGN_APLARB_RAW (&aplDen)) ?  0.0
                                                                       : -0.0)
                                        : 0.0);
    } else
    {
        mp_limb_signed_t prec = ARB_PREC_FPC;

        // Initialize to 0/0
        arb4f_init (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            arb_div (&lpMemRes[uRes].parts[i],
                     &aplNum.parts[i],
                     &aplDen,
                     prec);
    } // End IF/ELSE

    // Free the temps
    arb1f_clear (&aplDen);
    arb4f_clear (&aplNum);
} // End PrimFnMonColonBarBA4FisBA4F


//***************************************************************************
//  $PrimFnMonColonBarHC8FisHC8I
//
//  Primitive scalar function monadic ColonBar:  HC8F {is} fn HC8I
//***************************************************************************

void PrimFnMonColonBarHC8FisHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht;

    // Convert from HC8I to HC8F
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonColonBarHC8FisHC8F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonColonBarHC8FisHC8I


//***************************************************************************
//  $PrimFnMonColonBarHC8FisHC8F
//
//  Primitive scalar function monadic ColonBar:  HC8F {is} fn HC8F
//***************************************************************************

void PrimFnMonColonBarHC8FisHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int      i;                 // Loop counter
    APLFLOAT aplDen;            // Denominator
    APLHC8F  aplNum;            // Numerator
    UBOOL    bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Calculate the numerator
    aplNum = ConjHC8F (lpatRht->aplHC8F);

    // Calculate the denominator
    aplDen = SqNrmHC8F (&lpatRht->aplHC8F);

    // If the denominator is 0, ...
    if (aplDen EQ 0.0)
    {
        lpMemRes[uRes].parts[0] =
          TranslateQuadICIndex (0,
                                ICNDX_DIV0,
                                lpatRht->aplHC8F.parts[0],
                                SIGN_APLFLOAT (lpatRht->aplHC8F.parts[0]));
        lpMemRes[uRes].parts[1] = 0;
    } else
    // If the denominator is {Inf}, ...
    if (IsFltInfinity (aplDen))
    {
        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            lpMemRes[uRes].parts[i] =
              (bAllowNeg0 ? ((SIGN_APLFLOAT_RAW (aplNum.parts[i])
                           EQ SIGN_APLFLOAT_RAW (aplDen)) ?  0.0
                                                          : -0.0)
                          : 0.0);
    } else
    {
        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            // The FPU handles overflow and underflow for us
            lpMemRes[uRes].parts[i] = aplNum.parts[i] / aplDen;
    } // End IF/ELSE
} // End PrimFnMonColonBarHC8FisHC8F


//***************************************************************************
//  $InvHC8R_RE
//***************************************************************************

APLHC8R InvHC8R_RE
    (APLHC8R aplRht)                    // Right arg

{
    int     i;                  // Loop counter
    APLRAT  aplDen = {0};       // Denominator
    APLHC8R aplNum,             // Numerator
            aplRes = {0};
    UBOOL   bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Calculate the numerator
    aplNum = ConjHC8R (aplRht);

    // Calculate the denominator
    aplDen = SqNrmHC8R (&aplRht);

    // If the denominator is 0, ...
    if (IsMpq0 (&aplDen))
    {
        aplRes.parts[0] =
          *mpq_QuadICValue (&aplRht.parts[0],     // No left arg
                             ICNDX_DIV0,
                            &aplRht.parts[0],
                            &aplRes.parts[0],
                             FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 8; i++)
            // Initialize to 0/1
            mpq_init (&aplRes.parts[i]);
    } else
    // Check for special case:  {div}{neg}{inf}x
    if (bAllowNeg0
     && IsMpqInfinity (&aplDen)
     && SIGN_APLRAT (&aplDen))
    {
        // Free the temps
        Myhc1r_clear (&aplDen);
        Myhc8r_clear (&aplNum);

        RaiseException (EXCEPTION_RESULT_HC8V, 0, 0, NULL);
    } else
    // If the denominator is {Inf}, ...
    if (IsMpqInfinity (&aplDen))
    {
        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            mpq_init_set_d (&aplRes.parts[i],
                            bAllowNeg0 ? ((SIGN_APLRAT (&aplNum.parts[i])
                                        EQ SIGN_APLRAT (&aplDen)) ?  0.0
                                                                  : -0.0)
                                       : 0.0);
    } else
    {
        // Initialize to 0/1
        mphc8r_init (&aplRes);

        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            mpq_div (&aplRes.parts[i],
                     &aplNum.parts[i],
                     &aplDen);
    } // End IF/ELSE

    // Free the temps
    Myhc1r_clear (&aplDen);
    Myhc8r_clear (&aplNum);

    return aplRes;
} // End InvHC8R_RE


//***************************************************************************
//  $PrimFnMonColonBarHC8RisHC8R
//
//  Primitive scalar function monadic ColonBar:  HC8R {is} fn HC8R
//***************************************************************************

void PrimFnMonColonBarHC8RisHC8R
    (LPAPLHC8R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = InvHC8R_RE (lpatRht->aplHC8R);
} // End PrimFnMonColonBarHC8RisHC8R


//***************************************************************************
//  $PrimFnMonColonBarHC8VisHC8R
//
//  Primitive scalar function monadic ColonBar:  HC8V {is} fn HC8R
//***************************************************************************

void PrimFnMonColonBarHC8VisHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonColonBarHC8VisHC8V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc8v_clear (&atRht.aplHC8V);
} // End PrimFnMonColonBarHC8VisHC8R


//***************************************************************************
//  $PrimFnMonColonBarHC8VisHC8V
//
//  Primitive scalar function monadic ColonBar:  HC8V {is} fn HC8V
//***************************************************************************

void PrimFnMonColonBarHC8VisHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int     i;                  // Loop counter
    APLVFP  aplDen = {0};       // Denominator
    APLHC8V aplNum;             // Numerator
    UBOOL   bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Calculate the numerator
    aplNum = ConjHC8V (lpatRht->aplHC8V);

    // Calculate the denominator
    aplDen = SqNrmHC8V (&lpatRht->aplHC8V);

    // If the denominator is 0, ...
    if (IsMpf0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] =
          *mpfr_QuadICValue (&lpatRht->aplHC8V.parts[0],     // No left arg
                              ICNDX_DIV0,
                             &lpatRht->aplHC8V.parts[0],
                             &lpMemRes[uRes].parts[0],
                              FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 8; i++)
            // Initialize to 0
            mpfr_init0 (&lpMemRes[uRes].parts[i]);
    } else
    // If the denominator is {Inf}, ...
    if (IsMpfInfinity (&aplDen))
    {
        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            mpfr_init_set_d (&lpMemRes[uRes].parts[i],
                              bAllowNeg0 ? ((SIGN_APLVFP_RAW (&aplNum.parts[i])
                                          EQ SIGN_APLVFP_RAW (&aplDen)) ?  0.0
                                                                        : -0.0)
                                         : 0.0,
                              MPFR_RNDN);
    } else
    {
        // Initialize to 0
        mphc8v_init0 (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            mpfr_div (&lpMemRes[uRes].parts[i],
                      &aplNum.parts[i],
                      &aplDen,
                       MPFR_RNDN);
    } // End IF/ELSE

    // Free the temps
    Myhc1v_clear (&aplDen);
    Myhc8v_clear (&aplNum);
} // End PrimFnMonColonBarHC8VisHC8V


//***************************************************************************
//  $PrimFnMonColonBarBA8FisBA8F
//
//  Primitive scalar function monadic ColonBar:  BA8F {is} fn BA8F
//***************************************************************************

void PrimFnMonColonBarBA8FisBA8F
    (LPAPLBA8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int     i;                  // Loop counter
    APLARB  aplDen = {0};       // Denominator
    APLBA8F aplNum;             // Numerator
    UBOOL   bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Calculate the numerator
    aplNum = ConjBA8F (lpatRht->aplBA8F);

    // Calculate the denominator
    aplDen = SqNrmBA8F (&lpatRht->aplBA8F);

    // If the denominator is 0, ...
    if (IsArb0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] =
          *arb_QuadICValue (&lpatRht->aplBA8F.parts[0],     // No left arg
                             ICNDX_DIV0,
                            &lpatRht->aplBA8F.parts[0],
                            &lpMemRes[uRes].parts[0],
                             FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 8; i++)
            // Initialize to 0/0
            Myarb_init (&lpMemRes[uRes].parts[i]);
    } else
    // If the denominator is {Inf}, ...
    if (IsArbInfinity (&aplDen))
    {
        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            arb_init_set_d (&lpMemRes[uRes].parts[i],
                             bAllowNeg0 ? ((SIGN_APLARB_RAW (&aplNum.parts[i])
                                         EQ SIGN_APLARB_RAW (&aplDen)) ?  0.0
                                                                       : -0.0)
                                        : 0.0);
    } else
    {
        mp_limb_signed_t prec = ARB_PREC_FPC;

        // Initialize to 0/0
        arb8f_init (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            arb_div (&lpMemRes[uRes].parts[i],
                     &aplNum.parts[i],
                     &aplDen,
                     prec);
    } // End IF/ELSE

    // Free the temps
    arb1f_clear (&aplDen);
    arb8f_clear (&aplNum);
} // End PrimFnMonColonBarBA8FisBA8F


//***************************************************************************
//  $PrimSpecColonBarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecColonBarStorageTypeDyd
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

    // Except that BOOL, INT, and APA become INT
    // If a result doesn't fit in an INT, blow up to FLOAT.
    if (IsRealBIA (aplTypeRes))
        return ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecColonBarStorageTypeDyd


//***************************************************************************
//  $DivHC1I_RE
//***************************************************************************

APLHC1I DivHC1I_RE
    (APLHC1I aplLft,                    // Left arg
     APLHC1I aplRht)                    // Right ...

{
    APLHC1I aplRes = {0};

    // Check for indeterminates:  N {div} 0
    if (aplRht EQ 0)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    // Try integer division first
    aplRes = aplLft / aplRht;

    // See if the result is integral
    if (aplLft NE (aplRes * aplRht))
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return aplRes;
} // End DivHC1I_RE


//***************************************************************************
//  $PrimFnDydColonBarIisIvI
//
//  Primitive scalar function dyadic ColonBar:  I {is} I fn I
//***************************************************************************

void PrimFnDydColonBarIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = DivHC1I_RE (lpatLft->aplInteger, lpatRht->aplInteger);
} // End PrimFnDydColonBarIisIvI


//***************************************************************************
//  $PrimFnDydColonBarFisIvI
//
//  Primitive scalar function dyadic ColonBar:  F {is} I fn I
//***************************************************************************

void PrimFnDydColonBarFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  0 {div} 0
    if (lpatLft->aplInteger EQ 0
     && lpatRht->aplInteger EQ 0)
    {
        lpMemRes[uRes] =
          TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                ICNDX_0DIV0,
                     (APLFLOAT) lpatRht->aplInteger,
                                FALSE);
    } else
    // Check for indeterminates:  L {div} 0
    if (lpatRht->aplInteger EQ 0)
    {
        lpMemRes[uRes] =
          TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                ICNDX_DIV0,
                     (APLFLOAT) lpatRht->aplInteger,
                                FALSE);
    } else
        // The FPU handles overflow and underflow for us
        lpMemRes[uRes] = (((APLFLOAT) lpatLft->aplInteger) / (APLFLOAT) lpatRht->aplInteger);
} // End PrimFnDydColonBarFisIvI


//***************************************************************************
//  $DivHC1F_RE
//***************************************************************************

APLHC1F DivHC1F_RE
    (APLHC1F aplLft,                    // Left arg
     APLHC1F aplRht)                    // Right ...

{
    APLHC1F aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1F, &aplRht, 0);

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            return aplLft;
        else
            return aplRht;
    } else
    if (aplLft EQ 0
     && aplRht EQ 0)
    {
        aplRes =
          TranslateQuadICIndex (aplLft,
                                ICNDX_0DIV0,
                                aplRht,
                                SIGN_APLFLOAT (aplLft) NE SIGN_APLFLOAT (aplRht));
    } else
    // Check for indeterminates:  L {div} 0
    if (aplRht EQ 0)
    {
        aplRes =
          TranslateQuadICIndex (aplLft,
                                ICNDX_DIV0,
                                aplRht,
                                SIGN_APLFLOAT (aplLft) NE SIGN_APLFLOAT (aplRht));
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (IsFltInfinity (aplLft)
     && IsFltInfinity (aplRht))
    {
        if (SIGN_APLFLOAT (aplLft) EQ SIGN_APLFLOAT (aplRht))
            aplRes =
              TranslateQuadICIndex (aplLft,
                                    ICNDX_PiDIVPi,
                                    aplRht,
                                    FALSE);
        else
            aplRes =
              TranslateQuadICIndex (aplLft,
                                    ICNDX_NiDIVPi,
                                    aplRht,
                                    FALSE);
    } else
        // The FPU handles overflow and underflow for us
        aplRes = (aplLft / aplRht);

    return aplRes;
} // End DivHC1F_RE


//***************************************************************************
//  $PrimFnDydColonBarFisFvF
//
//  Primitive scalar function dyadic ColonBar:  F {is} F fn F
//***************************************************************************

void PrimFnDydColonBarFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = DivHC1F_RE (lpatLft->aplFloat, lpatRht->aplFloat);
} // End PrimFnDydColonBarFisFvF


//***************************************************************************
//  $DivHC1R_RE
//***************************************************************************

APLHC1R DivHC1R_RE
    (APLHC1R aplLft,                    // Left arg
     APLHC1R aplRht)                    // Right ...

{
    APLHC1R aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1R, &aplRht, 0);

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc1r_init_set (&aplRes, &aplLft);
        else
            mphc1r_init_set (&aplRes, &aplRht);
    } else
    // Check for indeterminates:  0 {div} 0
    if (IsMpq0 (&aplLft)
     && IsMpq0 (&aplRht))
    {
        aplRes =
          *mpq_QuadICValue (&aplLft,
                             ICNDX_0DIV0,
                            &aplRht,
                            &aplRes,
                             FALSE);
    } else
    // Check for indeterminates:  L {div} 0
    if (IsMpq0 (&aplRht))
    {
        aplRes =
          *mpq_QuadICValue (&aplLft,
                             ICNDX_DIV0,
                            &aplRht,
                            &aplRes,
                             FALSE);
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (mpq_inf_p (&aplLft)
     && mpq_inf_p (&aplRht))
    {
        if (mpq_sgn (&aplLft) EQ mpq_sgn (&aplRht))
            aplRes =
              *mpq_QuadICValue (&aplLft,
                                 ICNDX_PiDIVPi,
                                &aplRht,
                                &aplRes,
                                 FALSE);
        else
            aplRes =
              *mpq_QuadICValue (&aplLft,
                                 ICNDX_NiDIVPi,
                                &aplRht,
                                &aplRes,
                                 FALSE);
    } else
    {
        // Initialize the result
        mpq_init (&aplRes);

        // Divide the Rationals
        mpq_div (&aplRes, &aplLft, &aplRht);
    } // End IF/ELSE/...

    return aplRes;
} // End DivHC1R_RE


//***************************************************************************
//  $PrimFnDydColonBarRisRvR
//
//  Primitive scalar function dyadic ColonBar:  R {is} R fn R
//***************************************************************************

void PrimFnDydColonBarRisRvR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = DivHC1R_RE (lpatLft->aplHC1R, lpatRht->aplHC1R);
} // End PrimFnDydColonBarRisRvR


//***************************************************************************
//  $PrimFnDydColonBarVisRvR
//
//  Primitive scalar function dyadic ColonBar:  V {is} R fn R
//***************************************************************************

void PrimFnDydColonBarVisRvR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the left & right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatLft->aplRat, 0, &atLft);
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnDydColonBarVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydColonBarVisRvR


//***************************************************************************
//  $DivHC1V_RE
//***************************************************************************

//#define DEBUG_FMT

APLHC1V DivHC1V_RE
    (APLHC1V aplLft,                    // Left arg
     APLHC1V aplRht)                    // Right ...

{
    APLHC1V aplRes = {0};

    // Check for indeterminates:  0 {div} 0
    if (IsMpf0 (&aplLft)
     && IsMpf0 (&aplRht))
    {
        aplRes =
          *mpfr_QuadICValue (&aplLft,
                              ICNDX_0DIV0,
                             &aplRht,
                             &aplRes,
                              SIGN_APLVFP (&aplLft) NE
                              SIGN_APLVFP (&aplRht));
    } else
    // Check for indeterminates:  L {div} 0
    if (IsMpf0 (&aplRht))
    {
        aplRes =
          *mpfr_QuadICValue (&aplLft,
                              ICNDX_DIV0,
                             &aplRht,
                             &aplRes,
                              SIGN_APLVFP (&aplLft) NE
                              SIGN_APLVFP (&aplRht));
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (mpfr_inf_p (&aplLft)
     && mpfr_inf_p (&aplRht))
    {
        if (mpfr_sgn (&aplLft) EQ mpfr_sgn (&aplRht))
            aplRes =
              *mpfr_QuadICValue (&aplLft,
                                  ICNDX_PiDIVPi,
                                 &aplRht,
                                 &aplRes,
                                  FALSE);
        else
            aplRes =
              *mpfr_QuadICValue (&aplLft,
                                  ICNDX_NiDIVPi,
                                 &aplRht,
                                 &aplRes,
                                  FALSE);
    } else
    {
        // Initialize the result to 0
        mpfr_init0 (&aplRes);

        // Divide the VFPs
        mpfr_div (&aplRes, &aplLft, &aplRht, MPFR_RNDN);
    } // End IF/ELSE/...

    return aplRes;
} // End DivHC1V_RE


//***************************************************************************
//  $PrimFnDydColonBarVisVvV
//
//  Primitive scalar function dyadic ColonBar:  V {is} V fn V
//***************************************************************************

void PrimFnDydColonBarVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = DivHC1V_RE (lpatLft->aplHC1V, lpatRht->aplHC1V);
} // End PrimFnDydColonBarVisVvV


//***************************************************************************
//  $DivBA1F_RE
//***************************************************************************

APLBA1F DivBA1F_RE
    (APLBA1F aplLft,                    // Left arg
     APLBA1F aplRht)                    // Right ...

{
    APLBA1F aplRes = {0};

    // Check for indeterminates:  0 {div} 0
    if (IsArb0 (&aplLft)
     && IsArb0 (&aplRht))
    {
        aplRes =
          *arb_QuadICValue (&aplLft,
                             ICNDX_0DIV0,
                            &aplRht,
                            &aplRes,
                             SIGN_APLARB (&aplLft) NE
                             SIGN_APLARB (&aplRht));
    } else
    // Check for indeterminates:  L {div} 0
    if (IsArb0 (&aplRht))
    {
        aplRes =
          *arb_QuadICValue (&aplLft,
                             ICNDX_DIV0,
                            &aplRht,
                            &aplRes,
                             SIGN_APLARB (&aplLft) NE
                             SIGN_APLARB (&aplRht));
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (arf_is_inf (arb_midref (&aplLft))
     && arf_is_inf (arb_midref (&aplRht)))
    {
        if (arb_is_positive (&aplLft) EQ arb_is_positive (&aplRht))
            aplRes =
              *arb_QuadICValue (&aplLft,
                                 ICNDX_PiDIVPi,
                                &aplRht,
                                &aplRes,
                                 FALSE);
        else
            aplRes =
              *arb_QuadICValue (&aplLft,
                                 ICNDX_NiDIVPi,
                                &aplRht,
                                &aplRes,
                                 FALSE);
    } else
    {
        mp_limb_signed_t prec = ARB_PREC_FPC;

        // Initialize the result to 0/0
        Myarb_init (&aplRes);

        // Divide the ARBs
        arb_div (&aplRes, &aplLft, &aplRht, prec);
    } // End IF/ELSE/...

#if (defined DEBUG) && (defined DEBUG_FMT)
    ArbOut (L"\r\nLft:  ", &aplLft);
    ArbOut (L"\r\nRht:  ", &aplRht);
    ArbOut (L"\r\nRes:  ", &aplRes);
#endif

    return aplRes;
} // End DivBA1F_RE


//***************************************************************************
//  $PrimFnDydColonBarBA1FisBA1FvBA1F
//
//  Primitive scalar function dyadic ColonBar:  BA1F {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydColonBarBA1FisBA1FvBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = DivBA1F_RE (lpatLft->aplBA1F, lpatRht->aplBA1F);
} // End PrimFnDydColonBarBA1FisBA1FvBA1F


//***************************************************************************
//  $DivHC2I_RE
//***************************************************************************

APLHC2I DivHC2I_RE
    (APLHC2I aplLft,                    // Left arg
     APLHC2I aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLINT  aplDen = 0;         // Denominator
    APLHC2I aplNum,             // Numerator
            aplRes;             // Result

    // Calculate the denominator
    aplDen = SqNrmHC2I (&aplRht);

    // If the denominator is 0, ...
    if (aplDen EQ 0)
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

    // Calculate the conjugate
    aplNum = ConjHC2I (aplRht);

    // Calculate the numerator
    aplNum = MulHC2I_RE (aplLft, aplNum);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Try integer division first
        aplRes.parts[i] = aplNum.parts[i] / aplDen;

        // See if the result is integral
        if (aplNum.parts[i] NE MulHC1I_RE (aplRes.parts[i], aplDen))
            RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    } // End FOR

    return aplRes;
} // End DivHC2I_RE


//***************************************************************************
//  $PrimFnDydColonBarHC2IisHC2IvHC2I
//
//  Primitive scalar function dyadic ColonBar:  HC2I {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydColonBarHC2IisHC2IvHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC2I_RE (lpatLft->aplHC2I, lpatRht->aplHC2I);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2F);
} // End PrimFnDydColonBarHC2IisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydColonBarHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic ColonBar:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydColonBarHC2FisHC2IvHC2I
    (LPAPLHC2F lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert from HC2I to HC2F
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnDydColonBarHC2FisHC2FvHC2F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydColonBarHC2FisHC2IvHC2I


//***************************************************************************
//  $DivHC2F_RE
//***************************************************************************

APLHC2F DivHC2F_RE
    (APLHC2F aplLft,                    // Left arg
     APLHC2F aplRht)                    // Right ...

{
    int      i;                 // Loop counter
    APLFLOAT aplDen;            // Denominator
    APLHC2F  aplNum,            // Numerator
             aplRes;            // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Calculate the numerator
    aplNum = MulHC2F_RE (aplLft, ConjHC2F (aplRht));

    // Calculate the denominator
    aplDen = SqNrmHC2F (&aplRht);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    // Check for indeterminates:  0 {div} 0
    if (aplNum.parts[i] EQ 0
     && aplDen EQ 0)
    {
        aplRes.parts[0] =
          TranslateQuadICIndex (aplLft.parts[i],
                                ICNDX_0DIV0,
                                aplRht.parts[i],
                                SIGN_APLFLOAT (aplLft.parts[i]) NE SIGN_APLFLOAT (aplRht.parts[i]));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (aplDen EQ 0)
    {
        aplRes.parts[0] =
          TranslateQuadICIndex (aplLft.parts[i],
                                ICNDX_DIV0,
                                aplRht.parts[i],
                                SIGN_APLFLOAT (aplLft.parts[i]) NE SIGN_APLFLOAT (aplRht.parts[i]));
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (IsFltInfinity (aplNum.parts[i])
     && IsFltInfinity (aplDen))
    {
        if (SIGN_APLFLOAT (aplLft.parts[i]) EQ SIGN_APLFLOAT (aplRht.parts[i]))
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[i],
                                    ICNDX_PiDIVPi,
                                    aplRht.parts[i],
                                    FALSE);
        else
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[i],
                                    ICNDX_NiDIVPi,
                                    aplRht.parts[i],
                                    FALSE);
        break;
    } else
        // Divide numerator by denominator to get the result
        // The FPU handles infinities, overflow, and underflow for us
        aplRes.parts[i] = aplNum.parts[i] / aplDen;

    // If we terminated the FOR loop early, ...
    if (i < 2)
    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Set to 0
        aplRes.parts[i] = 0;

    return aplRes;
} // End DivHC2F_RE


//***************************************************************************
//  $PrimFnDydColonBarHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic ColonBar:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydColonBarHC2FisHC2FvHC2F
    (LPAPLHC2F lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC2F_RE (lpatLft->aplHC2F, lpatRht->aplHC2F);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC2FisHC2FvHC2F


//***************************************************************************
//  $DivHC2R_RE
//***************************************************************************

APLHC2R DivHC2R_RE
    (APLHC2R aplLft,                    // Left arg
     APLHC2R aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLRAT  aplDen = {0};       // Denominator
    APLHC2R aplNum,             // Numerator
            aplConj,            // Conjugate
            aplRes = {0};       // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Calculate the conjugate of the denominator
    aplConj = ConjHC2R (aplRht);

    // Calculate the numerator
    aplNum = MulHC2R_RE (aplLft, aplConj);

    // Calculate the denominator
    aplDen = SqNrmHC2R (&aplRht);

    // Initialize to 0/1
    mphc2r_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    // Check for indeterminates:  0 {div} 0
    if (IsMpq0 (&aplNum.parts[i])
     && IsMpq0 (&aplDen))
    {
        mpq_set (&aplRes.parts[0],
                  mpq_QuadICValue (&aplLft.parts[i],
                                    ICNDX_0DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (IsMpq0 (&aplDen))
    {
        mpq_set (&aplRes.parts[0],
                  mpq_QuadICValue (&aplLft.parts[i],
                                    ICNDX_DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (mpq_inf_p (&aplNum.parts[i])
     && mpq_inf_p (&aplDen))
    {
        if (mpq_sgn (&aplLft.parts[i]) EQ mpq_sgn (&aplRht.parts[i]))
            mpq_set (&aplRes.parts[0],
                      mpq_QuadICValue (&aplLft.parts[i],
                                        ICNDX_PiDIVPi,
                                       &aplRht.parts[i],
                                       &aplRes.parts[0],
                                        FALSE));
        else
            mpq_set (&aplRes.parts[0],
                      mpq_QuadICValue (&aplLft.parts[i],
                                        ICNDX_NiDIVPi,
                                       &aplRht.parts[i],
                                       &aplRes.parts[0],
                                        FALSE));
        break;
    } else
        // Divide numerator by denominator to get the result
        mpq_div (&aplRes.parts[i], &aplNum.parts[i], &aplDen);

    // If we terminated the FOR loop early, ...
    if (i < 2)
    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Set to 0/1
        mpq_set_si (&aplRes.parts[i], 0, 1);
    // Free the temps
    Myhc1r_clear (&aplDen );
    Myhc2r_clear (&aplNum );
    Myhc2r_clear (&aplConj);

    return aplRes;
} // End DivHC2R_RE


//***************************************************************************
//  $PrimFnDydColonBarHC2RisHC2RvHC2R
//
//  Primitive scalar function dyadic ColonBar:  HC2R {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydColonBarHC2RisHC2RvHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC2R_RE (lpatLft->aplHC2R, lpatRht->aplHC2R);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2V);
} // End PrimFnDydColonBarHC2RisHC2RvHC2R


//***************************************************************************
//  $DivHC2V_RE
//***************************************************************************

APLHC2V DivHC2V_RE
    (APLHC2V aplLft,                    // Left arg
     APLHC2V aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLVFP  aplDen = {0};       // Denominator
    APLHC2V aplNum,             // Numerator
            aplConj,            // Conjugate
            aplRes = {0};       // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Calculate the conjugate of the denominator
    aplConj = ConjHC2V (aplRht);

    // Calculate the numerator
    aplNum = MulHC2V_RE (aplLft, aplConj);

    // Calculate the denominator
    aplDen = SqNrmHC2V (&aplRht);

    // Initialize to 0
    mphc2v_init0 (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    // Check for indeterminates:  0 {div} 0
    if (IsMpf0 (&aplNum.parts[i])
     && IsMpf0 (&aplDen))
    {
        mpfr_set (&aplRes.parts[0],
                   mpfr_QuadICValue (&aplLft.parts[i],
                                      ICNDX_0DIV0,
                                     &aplRht.parts[i],
                                     &aplRes.parts[0],
                                      FALSE),
                   MPFR_RNDN);
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (IsMpf0 (&aplDen))
    {
        mpfr_set (&aplRes.parts[0],
                   mpfr_QuadICValue (&aplLft.parts[i],
                                      ICNDX_DIV0,
                                     &aplRht.parts[i],
                                     &aplRes.parts[0],
                                      FALSE),
                   MPFR_RNDN);
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (mpfr_inf_p (&aplNum.parts[i])
     && mpfr_inf_p (&aplDen))
    {
        if (mpfr_sgn (&aplLft.parts[i]) EQ mpfr_sgn (&aplRht.parts[i]))
            mpfr_set (&aplRes.parts[0],
                       mpfr_QuadICValue (&aplLft.parts[i],
                                          ICNDX_PiDIVPi,
                                         &aplRht.parts[i],
                                         &aplRes.parts[0],
                                          FALSE),
                       MPFR_RNDN);
        else
            mpfr_set (&aplRes.parts[0],
                       mpfr_QuadICValue (&aplLft.parts[i],
                                          ICNDX_NiDIVPi,
                                         &aplRht.parts[i],
                                         &aplRes.parts[0],
                                          FALSE),
                       MPFR_RNDN);
        break;
    } else
        // Divide numerator by denominator to get the result
        mpfr_div (&aplRes.parts[i], &aplNum.parts[i], &aplDen, MPFR_RNDN);

    // If we terminated the FOR loop early, ...
    if (i < 2)
    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Set to 0
        mpfr_set_si (&aplRes.parts[i], 0, MPFR_RNDN);
    // Free the temps
    Myhc1v_clear (&aplDen );
    Myhc2v_clear (&aplNum );
    Myhc2v_clear (&aplConj);

    return aplRes;
} // End DivHC2V_RE


//***************************************************************************
//  $PrimFnDydColonBarHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic ColonBar:  HC2V {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydColonBarHC2VisHC2RvHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the left & right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatLft->aplHC2R, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplHC2R, 0, &atRht);

    PrimFnDydColonBarHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
    Myhc2v_clear (&atLft.aplHC2V);
} // End PrimFnDydColonBarHC2VisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydColonBarHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic ColonBar:  HC2V {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydColonBarHC2VisHC2VvHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC2V_RE (lpatLft->aplHC2V, lpatRht->aplHC2V);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC2VisHC2VvHC2V


//***************************************************************************
//  $DivBA2F_RE
//***************************************************************************

APLBA2F DivBA2F_RE
    (APLBA2F aplLft,                    // Left arg
     APLBA2F aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLARB  aplDen = {0};       // Denominator
    APLBA2F aplNum,             // Numerator
            aplConj,            // Conjugate
            aplRes = {0};       // Result
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Calculate the conjugate of the denominator
    aplConj = ConjBA2F (aplRht);

    // Calculate the numerator
    aplNum = MulBA2F_RE (aplLft, aplConj);

    // Calculate the denominator
    aplDen = SqNrmBA2F (&aplRht);

    // Initialize to 0/0
    arb2f_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    // Check for indeterminates:  0 {div} 0
    if (IsArb0 (&aplNum.parts[i])
     && IsArb0 (&aplDen))
    {
        arb_set (&aplRes.parts[0],
                  arb_QuadICValue (&aplLft.parts[i],
                                    ICNDX_0DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (IsArb0 (&aplDen))
    {
        arb_set (&aplRes.parts[0],
                  arb_QuadICValue (&aplLft.parts[i],
                                    ICNDX_DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (arf_is_inf (arb_midref (&aplNum.parts[i]))
     && arf_is_inf (arb_midref (&aplDen)))
    {
        if (arb_is_positive (&aplLft.parts[i]) EQ arb_is_positive (&aplRht.parts[i]))
            arb_set (&aplRes.parts[0],
                     arb_QuadICValue (&aplLft.parts[i],
                                       ICNDX_PiDIVPi,
                                      &aplRht.parts[i],
                                      &aplRes.parts[0],
                                       FALSE));
        else
            arb_set (&aplRes.parts[0],
                      arb_QuadICValue (&aplLft.parts[i],
                                        ICNDX_NiDIVPi,
                                       &aplRht.parts[i],
                                       &aplRes.parts[0],
                                        FALSE));
        break;
    } else
    {
        // Divide numerator by denominator to get the result
        arb_div (&aplRes.parts[i], &aplNum.parts[i], &aplDen, prec);
    } // End IF/ELSE/...

    // If we terminated the FOR loop early, ...
    if (i < 2)
    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Set to 0
        arb_set_si (&aplRes.parts[i], 0);
    // Free the temps
    arb1f_clear (&aplDen );
    arb2f_clear (&aplNum );
    arb2f_clear (&aplConj);

    return aplRes;
} // End DivBA2F_RE


//***************************************************************************
//  $PrimFnDydColonBarBA2FisBA2FvBA2F
//
//  Primitive scalar function dyadic ColonBar:  BA2F {is} BA2F fn BA2F
//***************************************************************************

void PrimFnDydColonBarBA2FisBA2FvBA2F
    (LPAPLBA2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivBA2F_RE (lpatLft->aplBA2F, lpatRht->aplBA2F);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarBA2FisBA2FvBA2F


//***************************************************************************
//  $DivHC4I_RE
//***************************************************************************

APLHC4I DivHC4I_RE
    (APLHC4I aplLft,                    // Left arg
     APLHC4I aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLINT  aplDen = 0;         // Denominator
    APLHC4I aplNum,             // Numerator
            aplRes;             // Result

    // Calculate the denominator
    aplDen = SqNrmHC4I (&aplRht);

    // If the denominator is 0, ...
    if (aplDen EQ 0)
        RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);

    // Calculate the conjugate
    aplNum = ConjHC4I (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadLR () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC4I_RE (        aplNum, aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC4I_RE (aplLft, aplNum);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Try integer division first
        aplRes.parts[i] = aplNum.parts[i] / aplDen;

        // See if the result is integral
        if (aplNum.parts[i] NE MulHC1I_RE (aplRes.parts[i], aplDen))
            RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);
    } // End FOR

    return aplRes;
} // End DivHC4I_RE


//***************************************************************************
//  $PrimFnDydColonBarHC4IisHC4IvHC4I
//
//  Primitive scalar function dyadic ColonBar:  HC4I {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydColonBarHC4IisHC4IvHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC4I_RE (lpatLft->aplHC4I, lpatRht->aplHC4I);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4F);
} // End PrimFnDydColonBarHC4IisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydColonBarHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic ColonBar:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydColonBarHC4FisHC4IvHC4I
    (LPAPLHC4F lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert from HC4I to HC4F
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnDydColonBarHC4FisHC4FvHC4F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydColonBarHC4FisHC4IvHC4I


//***************************************************************************
//  $DivHC4F_RE
//***************************************************************************

APLHC4F DivHC4F_RE
    (APLHC4F aplLft,                    // Left arg
     APLHC4F aplRht)                    // Right ...

{
    int      i;                 // Loop counter
    APLFLOAT aplDen;            // Denominator
    APLHC4F  aplNum,            // Numerator
             aplRes;            // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // If the user wants the left quotient, ...
    if (GetQuadLR () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC4F_RE (        ConjHC4F (aplRht), aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC4F_RE (aplLft, ConjHC4F (aplRht));

    // Calculate the denominator
    aplDen = SqNrmHC4F (&aplRht);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    // Check for indeterminates:  0 {div} 0
    if (aplNum.parts[i] EQ 0
     && aplDen EQ 0)
    {
        aplRes.parts[0] =
          TranslateQuadICIndex (aplLft.parts[i],
                                ICNDX_0DIV0,
                                aplRht.parts[i],
                                SIGN_APLFLOAT (aplLft.parts[i]) NE SIGN_APLFLOAT (aplRht.parts[i]));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (aplDen EQ 0)
    {
        aplRes.parts[0] =
          TranslateQuadICIndex (aplLft.parts[i],
                                ICNDX_DIV0,
                                aplRht.parts[i],
                                SIGN_APLFLOAT (aplLft.parts[i]) NE SIGN_APLFLOAT (aplRht.parts[i]));
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (IsFltInfinity (aplNum.parts[i])
     && IsFltInfinity (aplDen))
    {
        if (SIGN_APLFLOAT (aplLft.parts[i]) EQ SIGN_APLFLOAT (aplRht.parts[i]))
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[i],
                                    ICNDX_PiDIVPi,
                                    aplRht.parts[i],
                                    FALSE);
        else
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[i],
                                    ICNDX_NiDIVPi,
                                    aplRht.parts[i],
                                    FALSE);
        break;
    } else
        // Divide numerator by denominator to get the result
        // The FPU handles infinities, overflow, and underflow for us
        aplRes.parts[i] = aplNum.parts[i] / aplDen;

    // If we terminated the FOR loop early, ...
    if (i < 4)
    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Set to 0
        aplRes.parts[i] = 0;

    return aplRes;
} // End DivHC4F_RE


//***************************************************************************
//  $PrimFnDydColonBarHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic ColonBar:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydColonBarHC4FisHC4FvHC4F
    (LPAPLHC4F lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC4F_RE (lpatLft->aplHC4F, lpatRht->aplHC4F);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC4FisHC4FvHC4F


//***************************************************************************
//  $DivHC4R_RE
//***************************************************************************

APLHC4R DivHC4R_RE
    (APLHC4R aplLft,                    // Left arg
     APLHC4R aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLRAT  aplDen = {0};       // Denominator
    APLHC4R aplNum,             // Numerator
            aplConj,            // Conjugate
            aplRes = {0};       // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Calculate the conjugate of the denominator
    aplConj = ConjHC4R (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadLR () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC4R_RE (        aplConj, aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC4R_RE (aplLft, aplConj);

    // Calculate the denominator
    aplDen = SqNrmHC4R (&aplRht);

    // Initialize to 0/1
    mphc4r_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    // Check for indeterminates:  0 {div} 0
    if (IsMpq0 (&aplNum.parts[i])
     && IsMpq0 (&aplDen))
    {
        mpq_set (&aplRes.parts[0],
                  mpq_QuadICValue (&aplLft.parts[i],
                                    ICNDX_0DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (IsMpq0 (&aplDen))
    {
        mpq_set (&aplRes.parts[0],
                  mpq_QuadICValue (&aplLft.parts[i],
                                    ICNDX_DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (mpq_inf_p (&aplNum.parts[i])
     && mpq_inf_p (&aplDen))
    {
        if (mpq_sgn (&aplLft.parts[i]) EQ mpq_sgn (&aplRht.parts[i]))
            mpq_set (&aplRes.parts[0],
                      mpq_QuadICValue (&aplLft.parts[i],
                                        ICNDX_PiDIVPi,
                                       &aplRht.parts[i],
                                       &aplRes.parts[0],
                                        FALSE));
        else
            mpq_set (&aplRes.parts[0],
                      mpq_QuadICValue (&aplLft.parts[i],
                                        ICNDX_NiDIVPi,
                                       &aplRht.parts[i],
                                       &aplRes.parts[0],
                                        FALSE));
        break;
    } else
        // Divide numerator by denominator to get the result
        mpq_div (&aplRes.parts[i], &aplNum.parts[i], &aplDen);

    // If we terminated the FOR loop early, ...
    if (i < 4)
    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Set to 0/1
        mpq_set_si (&aplRes.parts[i], 0, 1);
    // Free the temps
    Myhc1r_clear (&aplDen );
    Myhc4r_clear (&aplNum );
    Myhc4r_clear (&aplConj);

    return aplRes;
} // End DivHC4R_RE


//***************************************************************************
//  $PrimFnDydColonBarHC4RisHC4RvHC4R
//
//  Primitive scalar function dyadic ColonBar:  HC4R {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydColonBarHC4RisHC4RvHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC4R_RE (lpatLft->aplHC4R, lpatRht->aplHC4R);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4V);
} // End PrimFnDydColonBarHC4RisHC4RvHC4R


//***************************************************************************
//  $DivHC4V_RE
//***************************************************************************

APLHC4V DivHC4V_RE
    (APLHC4V aplLft,                    // Left arg
     APLHC4V aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLVFP  aplDen = {0};       // Denominator
    APLHC4V aplNum,             // Numerator
            aplConj,            // Conjugate
            aplRes = {0};       // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Calculate the conjugate of the denominator
    aplConj = ConjHC4V (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadLR () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC4V_RE (        aplConj, aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC4V_RE (aplLft, aplConj);

    // Calculate the denominator
    aplDen = SqNrmHC4V (&aplRht);

    // Initialize to 0
    mphc4v_init0 (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    // Check for indeterminates:  0 {div} 0
    if (IsMpf0 (&aplNum.parts[i])
     && IsMpf0 (&aplDen))
    {
        mpfr_set (&aplRes.parts[0],
                   mpfr_QuadICValue (&aplLft.parts[i],
                                      ICNDX_0DIV0,
                                     &aplRht.parts[i],
                                     &aplRes.parts[0],
                                      FALSE),
                   MPFR_RNDN);
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (IsMpf0 (&aplDen))
    {
        mpfr_set (&aplRes.parts[0],
                   mpfr_QuadICValue (&aplLft.parts[i],
                                      ICNDX_DIV0,
                                     &aplRht.parts[i],
                                     &aplRes.parts[0],
                                      FALSE),
                   MPFR_RNDN);
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (mpfr_inf_p (&aplNum.parts[i])
     && mpfr_inf_p (&aplDen))
    {
        if (mpfr_sgn (&aplLft.parts[i]) EQ mpfr_sgn (&aplRht.parts[i]))
            mpfr_set (&aplRes.parts[0],
                       mpfr_QuadICValue (&aplLft.parts[i],
                                          ICNDX_PiDIVPi,
                                         &aplRht.parts[i],
                                         &aplRes.parts[0],
                                          FALSE),
                       MPFR_RNDN);
        else
            mpfr_set (&aplRes.parts[0],
                       mpfr_QuadICValue (&aplLft.parts[i],
                                          ICNDX_NiDIVPi,
                                         &aplRht.parts[i],
                                         &aplRes.parts[0],
                                          FALSE),
                       MPFR_RNDN);
        break;
    } else
        // Divide numerator by denominator to get the result
        mpfr_div (&aplRes.parts[i], &aplNum.parts[i], &aplDen, MPFR_RNDN);

    // If we terminated the FOR loop early, ...
    if (i < 4)
    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Set to 0
        mpfr_set_si (&aplRes.parts[i], 0, MPFR_RNDN);
    // Free the temps
    Myhc1v_clear (&aplDen );
    Myhc4v_clear (&aplNum );
    Myhc4v_clear (&aplConj);

    return aplRes;
} // End DivHC4V_RE


//***************************************************************************
//  $PrimFnDydColonBarHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic ColonBar:  HC4V {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydColonBarHC4VisHC4RvHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the left & right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatLft->aplHC4R, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplHC4R, 0, &atRht);

    PrimFnDydColonBarHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
    Myhc4v_clear (&atLft.aplHC4V);
} // End PrimFnDydColonBarHC4VisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydColonBarHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic ColonBar:  HC4V {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydColonBarHC4VisHC4VvHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC4V_RE (lpatLft->aplHC4V, lpatRht->aplHC4V);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC4VisHC4VvHC4V


//***************************************************************************
//  $DivBA4F_RE
//***************************************************************************

APLBA4F DivBA4F_RE
    (APLBA4F aplLft,                    // Left arg
     APLBA4F aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLARB  aplDen = {0};       // Denominator
    APLBA4F aplNum,             // Numerator
            aplConj,            // Conjugate
            aplRes = {0};       // Result
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Calculate the conjugate of the denominator
    aplConj = ConjBA4F (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadLR () EQ 'l')
        // Calculate the numerator
        aplNum = MulBA4F_RE (        aplConj, aplLft);
    else
        // Calculate the numerator
        aplNum = MulBA4F_RE (aplLft, aplConj);

    // Calculate the denominator
    aplDen = SqNrmBA4F (&aplRht);

    // Initialize to 0
    arb4f_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    // Check for indeterminates:  0 {div} 0
    if (IsArb0 (&aplNum.parts[i])
     && IsArb0 (&aplDen))
    {
        arb_set (&aplRes.parts[0],
                  arb_QuadICValue (&aplLft.parts[i],
                                    ICNDX_0DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (IsArb0 (&aplDen))
    {
        arb_set (&aplRes.parts[0],
                  arb_QuadICValue (&aplLft.parts[i],
                                    ICNDX_DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (arf_is_inf (arb_midref (&aplNum.parts[i]))
     && arf_is_inf (arb_midref (&aplDen)))
    {
        if (arb_is_positive (&aplLft.parts[i]) EQ arb_is_positive (&aplRht.parts[i]))
            arb_set (&aplRes.parts[0],
                     arb_QuadICValue (&aplLft.parts[i],
                                       ICNDX_PiDIVPi,
                                      &aplRht.parts[i],
                                      &aplRes.parts[0],
                                       FALSE));
        else
            arb_set (&aplRes.parts[0],
                      arb_QuadICValue (&aplLft.parts[i],
                                        ICNDX_NiDIVPi,
                                       &aplRht.parts[i],
                                       &aplRes.parts[0],
                                        FALSE));
        break;
    } else
    {
        // Divide numerator by denominator to get the result
        arb_div (&aplRes.parts[i], &aplNum.parts[i], &aplDen, prec);
    } // End IF/ELSE/...

    // If we terminated the FOR loop early, ...
    if (i < 4)
    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Set to 0
        arb_set_si (&aplRes.parts[i], 0);
    // Free the temps
    arb1f_clear (&aplDen );
    arb4f_clear (&aplNum );
    arb4f_clear (&aplConj);

    return aplRes;
} // End DivBA4F_RE


//***************************************************************************
//  $PrimFnDydColonBarBA4FisBA4FvBA4F
//
//  Primitive scalar function dyadic ColonBar:  BA4F {is} BA4F fn BA4F
//***************************************************************************

void PrimFnDydColonBarBA4FisBA4FvBA4F
    (LPAPLBA4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivBA4F_RE (lpatLft->aplBA4F, lpatRht->aplBA4F);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarBA4FisBA4FvBA4F


//***************************************************************************
//  $DivHC8I_RE
//***************************************************************************

APLHC8I DivHC8I_RE
    (APLHC8I aplLft,                    // Left arg
     APLHC8I aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLINT  aplDen = 0;         // Denominator
    APLHC8I aplNum,             // Numerator
            aplRes;             // Result

    // Calculate the denominator
    aplDen = SqNrmHC8I (&aplRht);

    // If the denominator is 0, ...
    if (aplDen EQ 0)
        RaiseException (EXCEPTION_RESULT_HC8F, 0, 0, NULL);

    // Calculate the conjugate
    aplNum = ConjHC8I (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadLR () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC8I_RE (        aplNum, aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC8I_RE (aplLft, aplNum);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Try integer division first
        aplRes.parts[i] = aplNum.parts[i] / aplDen;

        // See if the result is integral
        if (aplNum.parts[i] NE MulHC1I_RE (aplRes.parts[i], aplDen))
            RaiseException (EXCEPTION_RESULT_HC8F, 0, 0, NULL);
    } // End FOR

    return aplRes;
} // End DivHC8I_RE


//***************************************************************************
//  $PrimFnDydColonBarHC8IisHC8IvHC8I
//
//  Primitive scalar function dyadic ColonBar:  HC8I {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydColonBarHC8IisHC8IvHC8I
    (LPAPLHC8I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC8I_RE (lpatLft->aplHC8I, lpatRht->aplHC8I);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8F);
} // End PrimFnDydColonBarHC8IisHC8IvHC8I


//***************************************************************************
//  $PrimFnDydColonBarHC8FisHC8FvHC8F
//
//  Primitive scalar function dyadic ColonBar:  HC8F {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydColonBarHC8FisHC8IvHC8I
    (LPAPLHC8F lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert from HC8I to HC8F
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnDydColonBarHC8FisHC8FvHC8F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydColonBarHC8FisHC8IvHC8I


//***************************************************************************
//  $DivHC8F_RE
//***************************************************************************

APLHC8F DivHC8F_RE
    (APLHC8F aplLft,                    // Left arg
     APLHC8F aplRht)                    // Right ...

{
    int      i;                 // Loop counter
    APLFLOAT aplDen;            // Denominator
    APLHC8F  aplNum,            // Numerator
             aplRes;            // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // If the user wants the left quotient, ...
    if (GetQuadLR () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC8F_RE (        ConjHC8F (aplRht), aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC8F_RE (aplLft, ConjHC8F (aplRht));

    // Calculate the denominator
    aplDen = SqNrmHC8F (&aplRht);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    // Check for indeterminates:  0 {div} 0
    if (aplNum.parts[i] EQ 0
     && aplDen EQ 0)
    {
        aplRes.parts[0] =
          TranslateQuadICIndex (aplLft.parts[i],
                                ICNDX_0DIV0,
                                aplRht.parts[i],
                                SIGN_APLFLOAT (aplLft.parts[i]) NE SIGN_APLFLOAT (aplRht.parts[i]));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (aplDen EQ 0)
    {
        aplRes.parts[0] =
          TranslateQuadICIndex (aplLft.parts[i],
                                ICNDX_DIV0,
                                aplRht.parts[i],
                                SIGN_APLFLOAT (aplLft.parts[i]) NE SIGN_APLFLOAT (aplRht.parts[i]));
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (IsFltInfinity (aplNum.parts[i])
     && IsFltInfinity (aplDen))
    {
        if (SIGN_APLFLOAT (aplLft.parts[i]) EQ SIGN_APLFLOAT (aplRht.parts[i]))
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[i],
                                    ICNDX_PiDIVPi,
                                    aplRht.parts[i],
                                    FALSE);
        else
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[i],
                                    ICNDX_NiDIVPi,
                                    aplRht.parts[i],
                                    FALSE);
        break;
    } else
        // Divide numerator by denominator to get the result
        // The FPU handles infinities, overflow, and underflow for us
        aplRes.parts[i] = aplNum.parts[i] / aplDen;

    // If we terminated the FOR loop early, ...
    if (i < 8)
    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Set to 0
        aplRes.parts[i] = 0;

    return aplRes;
} // End DivHC8F_RE


//***************************************************************************
//  $PrimFnDydColonBarHC8FisHC8FvHC8F
//
//  Primitive scalar function dyadic ColonBar:  HC8F {is} HC8F fn HC8F
//***************************************************************************

void PrimFnDydColonBarHC8FisHC8FvHC8F
    (LPAPLHC8F lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC8F_RE (lpatLft->aplHC8F, lpatRht->aplHC8F);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC8FisHC8FvHC8F


//***************************************************************************
//  $DivHC8R_RE
//***************************************************************************

APLHC8R DivHC8R_RE
    (APLHC8R aplLft,                    // Left arg
     APLHC8R aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLRAT  aplDen = {0};       // Denominator
    APLHC8R aplNum,             // Numerator
            aplConj,            // Conjugate
            aplRes = {0};       // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Calculate the conjugate of the denominator
    aplConj = ConjHC8R (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadLR () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC8R_RE (        aplConj, aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC8R_RE (aplLft, aplConj);

    // Calculate the denominator
    aplDen = SqNrmHC8R (&aplRht);

    // Initialize to 0/1
    mphc8r_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    // Check for indeterminates:  0 {div} 0
    if (IsMpq0 (&aplNum.parts[i])
     && IsMpq0 (&aplDen))
    {
        mpq_set (&aplRes.parts[0],
                  mpq_QuadICValue (&aplLft.parts[i],
                                    ICNDX_0DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (IsMpq0 (&aplDen))
    {
        mpq_set (&aplRes.parts[0],
                  mpq_QuadICValue (&aplLft.parts[i],
                                    ICNDX_DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (mpq_inf_p (&aplNum.parts[i])
     && mpq_inf_p (&aplDen))
    {
        if (mpq_sgn (&aplLft.parts[i]) EQ mpq_sgn (&aplRht.parts[i]))
            mpq_set (&aplRes.parts[0],
                      mpq_QuadICValue (&aplLft.parts[i],
                                        ICNDX_PiDIVPi,
                                       &aplRht.parts[i],
                                       &aplRes.parts[0],
                                        FALSE));
        else
            mpq_set (&aplRes.parts[0],
                      mpq_QuadICValue (&aplLft.parts[i],
                                        ICNDX_NiDIVPi,
                                       &aplRht.parts[i],
                                       &aplRes.parts[0],
                                        FALSE));
        break;
    } else
        // Divide numerator by denominator to get the result
        mpq_div (&aplRes.parts[i], &aplNum.parts[i], &aplDen);

    // If we terminated the FOR loop early, ...
    if (i < 8)
    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Set to 0/1
        mpq_set_si (&aplRes.parts[i], 0, 1);
    // Free the temps
    Myhc1r_clear (&aplDen );
    Myhc8r_clear (&aplNum );
    Myhc8r_clear (&aplConj);

    return aplRes;
} // End DivHC8R_RE


//***************************************************************************
//  $PrimFnDydColonBarHC8RisHC8RvHC8R
//
//  Primitive scalar function dyadic ColonBar:  HC8R {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydColonBarHC8RisHC8RvHC8R
    (LPAPLHC8R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC8R_RE (lpatLft->aplHC8R, lpatRht->aplHC8R);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8V);
} // End PrimFnDydColonBarHC8RisHC8RvHC8R


//***************************************************************************
//  $DivHC8V_RE
//***************************************************************************

APLHC8V DivHC8V_RE
    (APLHC8V aplLft,                    // Left arg
     APLHC8V aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLVFP  aplDen = {0};       // Denominator
    APLHC8V aplNum,             // Numerator
            aplConj,            // Conjugate
            aplRes = {0};       // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Calculate the conjugate of the denominator
    aplConj = ConjHC8V (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadLR () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC8V_RE (        aplConj, aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC8V_RE (aplLft, aplConj);

    // Calculate the denominator
    aplDen = SqNrmHC8V (&aplRht);

    // Initialize to 0
    mphc8v_init0 (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    // Check for indeterminates:  0 {div} 0
    if (IsMpf0 (&aplNum.parts[i])
     && IsMpf0 (&aplDen))
    {
        mpfr_set (&aplRes.parts[0],
                   mpfr_QuadICValue (&aplLft.parts[i],
                                      ICNDX_0DIV0,
                                     &aplRht.parts[i],
                                     &aplRes.parts[0],
                                      FALSE),
                   MPFR_RNDN);
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (IsMpf0 (&aplDen))
    {
        mpfr_set (&aplRes.parts[0],
                   mpfr_QuadICValue (&aplLft.parts[i],
                                      ICNDX_DIV0,
                                     &aplRht.parts[i],
                                     &aplRes.parts[0],
                                      FALSE),
                   MPFR_RNDN);
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (mpfr_inf_p (&aplNum.parts[i])
     && mpfr_inf_p (&aplDen))
    {
        if (mpfr_sgn (&aplLft.parts[i]) EQ mpfr_sgn (&aplRht.parts[i]))
            mpfr_set (&aplRes.parts[0],
                       mpfr_QuadICValue (&aplLft.parts[i],
                                          ICNDX_PiDIVPi,
                                         &aplRht.parts[i],
                                         &aplRes.parts[0],
                                          FALSE),
                       MPFR_RNDN);
        else
            mpfr_set (&aplRes.parts[0],
                       mpfr_QuadICValue (&aplLft.parts[i],
                                          ICNDX_NiDIVPi,
                                         &aplRht.parts[i],
                                         &aplRes.parts[0],
                                          FALSE),
                       MPFR_RNDN);
        break;
    } else
        // Divide numerator by denominator to get the result
        mpfr_div (&aplRes.parts[i], &aplNum.parts[i], &aplDen, MPFR_RNDN);

    // If we terminated the FOR loop early, ...
    if (i < 8)
    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Set to 0
        mpfr_set_si (&aplRes.parts[i], 0, MPFR_RNDN);
    // Free the temps
    Myhc1v_clear (&aplDen );
    Myhc8v_clear (&aplNum );
    Myhc8v_clear (&aplConj);

    return aplRes;
} // End DivHC8V_RE


//***************************************************************************
//  $PrimFnDydColonBarHC8VisHC8RvHC8R
//
//  Primitive scalar function dyadic ColonBar:  HC8V {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydColonBarHC8VisHC8RvHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the left & right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatLft->aplHC8R, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatRht->aplHC8R, 0, &atRht);

    PrimFnDydColonBarHC8VisHC8VvHC8V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc8v_clear (&atRht.aplHC8V);
    Myhc8v_clear (&atLft.aplHC8V);
} // End PrimFnDydColonBarHC8VisHC8RvHC8R


//***************************************************************************
//  $PrimFnDydColonBarHC8VisHC8VvHC8V
//
//  Primitive scalar function dyadic ColonBar:  HC8V {is} HC8V fn HC8V
//***************************************************************************

void PrimFnDydColonBarHC8VisHC8VvHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivHC8V_RE (lpatLft->aplHC8V, lpatRht->aplHC8V);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC8VisHC8VvHC8V


//***************************************************************************
//  $DivBA8F_RE
//***************************************************************************

APLBA8F DivBA8F_RE
    (APLBA8F aplLft,                    // Left arg
     APLBA8F aplRht)                    // Right ...

{
    int     i;                  // Loop counter
    APLARB  aplDen = {0};       // Denominator
    APLBA8F aplNum,             // Numerator
            aplConj,            // Conjugate
            aplRes = {0};       // Result
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Calculate the conjugate of the denominator
    aplConj = ConjBA8F (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadLR () EQ 'l')
        // Calculate the numerator
        aplNum = MulBA8F_RE (        aplConj, aplLft);
    else
        // Calculate the numerator
        aplNum = MulBA8F_RE (aplLft, aplConj);

    // Calculate the denominator
    aplDen = SqNrmBA8F (&aplRht);

    // Initialize to 0
    arb8f_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    // Check for indeterminates:  0 {div} 0
    if (IsArb0 (&aplNum.parts[i])
     && IsArb0 (&aplDen))
    {
        arb_set (&aplRes.parts[0],
                  arb_QuadICValue (&aplLft.parts[i],
                                    ICNDX_0DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (IsArb0 (&aplDen))
    {
        arb_set (&aplRes.parts[0],
                  arb_QuadICValue (&aplLft.parts[i],
                                    ICNDX_DIV0,
                                   &aplRht.parts[i],
                                   &aplRes.parts[0],
                                    FALSE));
        break;
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (arf_is_inf (arb_midref (&aplNum.parts[i]))
     && arf_is_inf (arb_midref (&aplDen)))
    {
        if (arb_is_positive (&aplLft.parts[i]) EQ arb_is_positive (&aplRht.parts[i]))
            arb_set (&aplRes.parts[0],
                     arb_QuadICValue (&aplLft.parts[i],
                                       ICNDX_PiDIVPi,
                                      &aplRht.parts[i],
                                      &aplRes.parts[0],
                                       FALSE));
        else
            arb_set (&aplRes.parts[0],
                      arb_QuadICValue (&aplLft.parts[i],
                                        ICNDX_NiDIVPi,
                                       &aplRht.parts[i],
                                       &aplRes.parts[0],
                                        FALSE));
        break;
    } else
    {
        // Divide numerator by denominator to get the result
        arb_div (&aplRes.parts[i], &aplNum.parts[i], &aplDen, prec);
    } // End IF/ELSE/...

    // If we terminated the FOR loop early, ...
    if (i < 8)
    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Set to 0
        arb_set_si (&aplRes.parts[i], 0);
    // Free the temps
    arb1f_clear (&aplDen );
    arb8f_clear (&aplNum );
    arb8f_clear (&aplConj);

    return aplRes;
} // End DivBA8F_RE


//***************************************************************************
//  $PrimFnDydColonBarBA8FisBA8FvBA8F
//
//  Primitive scalar function dyadic ColonBar:  BA8F {is} BA8F fn BA8F
//***************************************************************************

void PrimFnDydColonBarBA8FisBA8FvBA8F
    (LPAPLBA8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DivBA8F_RE (lpatLft->aplBA8F, lpatRht->aplBA8F);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarBA8FisBA8FvBA8F


//***************************************************************************
//  $DivHC2F_F
//***************************************************************************

APLHC2F DivHC2F_F
    (APLHC2F  aplLft,                   // Left arg
     APLFLOAT aplRht)                   // Right ...

{
    int     i;                  // Loop counter
    APLHC2F aplRes;             // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        aplRes.parts[i] = aplLft.parts[i] / aplRht;

    return aplRes;
} // End DivHC2F_F


//***************************************************************************
//  $DivHC4F_F
//***************************************************************************

APLHC4F DivHC4F_F
    (APLHC4F  aplLft,                   // Left arg
     APLFLOAT aplRht)                   // Right ...

{
    int     i;                  // Loop counter
    APLHC4F aplRes;             // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        aplRes.parts[i] = aplLft.parts[i] / aplRht;

    return aplRes;
} // End DivHC4F_F


//***************************************************************************
//  $DivHC8F_F
//***************************************************************************

APLHC8F DivHC8F_F
    (APLHC8F  aplLft,                   // Left arg
     APLFLOAT aplRht)                   // Right ...

{
    int     i;                  // Loop counter
    APLHC8F aplRes;             // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        aplRes.parts[i] = aplLft.parts[i] / aplRht;

    return aplRes;
} // End DivHC8F_F


//***************************************************************************
//  $DivHC2V_V
//***************************************************************************

APLHC2V DivHC2V_V
    (APLHC2V  aplLft,                   // Left arg
     APLVFP   aplRht)                   // Right ...

{
    int     i;                  // Loop counter
    APLHC2V aplRes = {0};       // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Initialize to 0
    mphc2v_init0 (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        mpfr_div (&aplRes.parts[i], &aplLft.parts[i], &aplRht, MPFR_RNDN);

    return aplRes;
} // End DivHC2V_V


//***************************************************************************
//  $DivHC4V_V
//***************************************************************************

APLHC4V DivHC4V_V
    (APLHC4V  aplLft,                   // Left arg
     APLVFP   aplRht)                   // Right ...

{
    int     i;                  // Loop counter
    APLHC4V aplRes = {0};       // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Initialize to 0
    mphc4v_init0 (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        mpfr_div (&aplRes.parts[i], &aplLft.parts[i], &aplRht, MPFR_RNDN);

    return aplRes;
} // End DivHC4V_V


//***************************************************************************
//  $DivHC8V_V
//***************************************************************************

APLHC8V DivHC8V_V
    (APLHC8V  aplLft,                   // Left arg
     APLVFP   aplRht)                   // Right ...

{
    int     i;                  // Loop counter
    APLHC8V aplRes = {0};       // Result

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Initialize to 0
    mphc8v_init0 (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        mpfr_div (&aplRes.parts[i], &aplLft.parts[i], &aplRht, MPFR_RNDN);

    return aplRes;
} // End DivHC8V_V


//***************************************************************************
//  $DivBA2F_F
//***************************************************************************

APLBA2F DivBA2F_F
    (APLBA2F  aplLft,                   // Left arg
     APLARB   aplRht)                   // Right ...

{
    int     i;                  // Loop counter
    APLBA2F aplRes = {0};       // Result
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Initialize to 0/0
    arb2f_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        arb_div (&aplRes.parts[i], &aplLft.parts[i], &aplRht, prec);

    return aplRes;
} // End DivBA2F_F


//***************************************************************************
//  $DivBA4F_F
//***************************************************************************

APLBA4F DivBA4F_F
    (APLBA4F  aplLft,                   // Left arg
     APLARB   aplRht)                   // Right ...

{
    int     i;                  // Loop counter
    APLBA4F aplRes = {0};       // Result
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Initialize to 0/0
    arb4f_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        arb_div (&aplRes.parts[i], &aplLft.parts[i], &aplRht, prec);

    return aplRes;
} // End DivBA4F_F


//***************************************************************************
//  $DivBA8F_F
//***************************************************************************

APLBA8F DivBA8F_F
    (APLBA8F  aplLft,                   // Left arg
     APLARB   aplRht)                   // Right ...

{
    int     i;                  // Loop counter
    APLBA8F aplRes = {0};       // Result
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  _ {div} _  or  -_ {div} -_

    // Initialize to 0/0
    arb8f_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        arb_div (&aplRes.parts[i], &aplLft.parts[i], &aplRht, prec);

    return aplRes;
} // End DivBA8F_F


//***************************************************************************
//  End of File: pf_colonbar.c
//***************************************************************************
