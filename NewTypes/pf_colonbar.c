//***************************************************************************
//  NARS2000 -- Primitive Function -- ColonBar
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

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnColonBar_EM_YY"
#else
#define APPEND_NAME
#endif

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
#undef  APPEND_NAME


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
    if (IsSimpleInt (aplTypeRes))
        return ARRAY_FLOAT;

    // Except that HCxI becomes HCxF
    if (IsHCInt (aplTypeRes))
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
        lpMemRes[uRes] = TranslateQuadICIndex (0,
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
        lpMemRes[uRes] = TranslateQuadICIndex (0,
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
        lpMemRes[uRes] = *mpq_QuadICValue (&lpatRht->aplRat,        // No left arg
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
        lpMemRes[uRes] = *mpfr_QuadICValue (&lpatRht->aplVfp,       // No left arg
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
    int      i;
    APLFLOAT aplDen = 0.0;      // Denominator
    APLHC2F  aplNum;            // Numerator

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Calculate the denominator
        // Square and sum the next right arg part
        aplDen = AddHC1F_RE (aplDen,
                             MulHC1F_RE (lpatRht->aplHC2F.parts[i], lpatRht->aplHC2F.parts[i]));
    } // End FOR

    // If the denominator is 0, ...
    if (aplDen EQ 0.0)
    {
        lpMemRes[uRes].parts[0] = TranslateQuadICIndex (0,
                                                        ICNDX_DIV0,
                                                        lpatRht->aplHC2F.parts[0],
                                                        SIGN_APLFLOAT (lpatRht->aplHC2F.parts[0]));
        lpMemRes[uRes].parts[1] = 0;
    } else
    // If the denominator is {Inf}, ...
    if (IsFltInfinity (aplDen))
    {
        UBOOL bAllowNeg0 = (UBOOL) gAllowNeg0;

        // Calculate the numerator
        aplNum = ConjHC2F (lpatRht->aplHC2F);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            lpMemRes[uRes].parts[i] = (bAllowNeg0 ? ((SIGN_APLFLOAT_RAW (aplNum.parts[i])
                                                   EQ SIGN_APLFLOAT_RAW (aplDen)) ?  0.0
                                                                                  : -0.0)
                                                  : 0.0);
    } else
    {
        // Calculate the numerator
        aplNum = ConjHC2F (lpatRht->aplHC2F);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            // The FPU handles overflow and underflow for us
            lpMemRes[uRes].parts[i] = aplNum.parts[i] / aplDen;
    } // End IF/ELSE
} // End PrimFnMonColonBarHC2FisHC2F


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
    int      i;
    APLRAT   aplDen = {0},      // Denominator
             aplTmp;            // Temp
    APLHC2R  aplNum;            // Numerator
    UBOOL    bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Initialize to 0/1
    mpq_init (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Calculate the denominator
        // Square the next right arg part
        aplTmp = MulHC1R_RE (lpatRht->aplHC2R.parts[i], lpatRht->aplHC2R.parts[i]);

        // Add into the accumulator
        mpq_add (&aplDen, &aplDen, &aplTmp);
    } // End FOR

    Myq_clear (&aplTmp);

    // If the denominator is 0, ...
    if (IsMpq0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] = *mpq_QuadICValue (&lpatRht->aplHC2R.parts[0],     // No left arg
                                                     ICNDX_DIV0,
                                                    &lpatRht->aplHC2R.parts[0],
                                                    &lpMemRes[uRes].parts[0],
                                                     FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 2; i++)
            // Initialize to 0/1
            mpq_init (&lpMemRes[uRes].parts[i]);
    } else
    // Check for special case:  {div}{neg}{inf}x
    if (bAllowNeg0
     && IsMpqInfinity (&aplDen)
     && SIGN_APLRAT (&aplDen))
        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    else
    // If the denominator is {Inf}, ...
    if (IsMpqInfinity (&aplDen))
    {
        // Calculate the numerator
        aplNum = ConjHC2R (lpatRht->aplHC2R);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            mpq_init_set_d (&lpMemRes[uRes].parts[i],
                            bAllowNeg0 ? ((SIGN_APLRAT (&aplNum.parts[i])
                                        EQ SIGN_APLRAT (&aplDen)) ?  0.0
                                                                  : -0.0)
                                       : 0.0);

    } else
    {
        // Calculate the numerator
        aplNum = ConjHC2R (lpatRht->aplHC2R);

        // Initialize to 0/1
        mphc2r_init (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            mpq_div (&lpMemRes[uRes].parts[i],
                     &aplNum.parts[i],
                     &aplDen);
    } // End IF/ELSE
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
    int      i;
    APLVFP   aplDen = {0},      // Denominator
             aplTmp;            // Temp
    APLHC2V  aplNum;            // Numerator
    UBOOL    bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Initialize to 0
    mpfr_init0 (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Calculate the denominator
        // Square the next right arg part
        aplTmp = MulHC1V_RE (lpatRht->aplHC2V.parts[i], lpatRht->aplHC2V.parts[i]);

        // Add into the accumulator
        mpfr_add (&aplDen, &aplDen, &aplTmp, MPFR_RNDN);
    } // End FOR

    Myf_clear (&aplTmp);

    // If the denominator is 0, ...
    if (IsMpf0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] = *mpfr_QuadICValue (&lpatRht->aplHC2V.parts[0],     // No left arg
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
        // Calculate the numerator
        aplNum = ConjHC2V (lpatRht->aplHC2V);

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
        // Calculate the numerator
        aplNum = ConjHC2V (lpatRht->aplHC2V);

        // Initialize to 0
        mphc2v_init0 (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            mpfr_div (&lpMemRes[uRes].parts[i],
                      &aplNum.parts[i],
                      &aplDen,
                       MPFR_RNDN);
    } // End IF/ELSE
} // End PrimFnMonColonBarHC2VisHC2V


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
    int      i;
    APLFLOAT aplDen = 0.0;      // Denominator
    APLHC4F  aplNum;            // Numerator

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Calculate the denominator
        // Square and sum the next right arg part
        aplDen = AddHC1F_RE (aplDen,
                             MulHC1F_RE (lpatRht->aplHC4F.parts[i], lpatRht->aplHC4F.parts[i]));
    } // End FOR

    // If the denominator is 0, ...
    if (aplDen EQ 0.0)
    {
        lpMemRes[uRes].parts[0] = TranslateQuadICIndex (0,
                                                        ICNDX_DIV0,
                                                        lpatRht->aplHC4F.parts[0],
                                                        SIGN_APLFLOAT (lpatRht->aplHC4F.parts[0]));
        lpMemRes[uRes].parts[1] = 0;
    } else
    // If the denominator is {Inf}, ...
    if (IsFltInfinity (aplDen))
    {
        UBOOL bAllowNeg0 = (UBOOL) gAllowNeg0;

        // Calculate the numerator
        aplNum = ConjHC4F (lpatRht->aplHC4F);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            lpMemRes[uRes].parts[i] = (bAllowNeg0 ? ((SIGN_APLFLOAT_RAW (aplNum.parts[i])
                                                   EQ SIGN_APLFLOAT_RAW (aplDen)) ?  0.0
                                                                                  : -0.0)
                                                  : 0.0);
    } else
    {
        // Calculate the numerator
        aplNum = ConjHC4F (lpatRht->aplHC4F);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            // The FPU handles overflow and underflow for us
            lpMemRes[uRes].parts[i] = aplNum.parts[i] / aplDen;
    } // End IF/ELSE
} // End PrimFnMonColonBarHC4FisHC4F


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
    int      i;
    APLRAT   aplDen = {0},      // Denominator
             aplTmp;            // Temp
    APLHC4R  aplNum;            // Numerator
    UBOOL    bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Initialize to 0/1
    mpq_init (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Calculate the denominator
        // Square the next right arg part
        aplTmp = MulHC1R_RE (lpatRht->aplHC4R.parts[i], lpatRht->aplHC4R.parts[i]);

        // Add into the accumulator
        mpq_add (&aplDen, &aplDen, &aplTmp);
    } // End FOR

    Myq_clear (&aplTmp);

    // If the denominator is 0, ...
    if (IsMpq0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] = *mpq_QuadICValue (&lpatRht->aplHC4R.parts[0],     // No left arg
                                                     ICNDX_DIV0,
                                                    &lpatRht->aplHC4R.parts[0],
                                                    &lpMemRes[uRes].parts[0],
                                                     FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 4; i++)
            // Initialize to 0/1
            mpq_init (&lpMemRes[uRes].parts[i]);
    } else
    // Check for special case:  {div}{neg}{inf}x
    if (bAllowNeg0
     && IsMpqInfinity (&aplDen)
     && SIGN_APLRAT (&aplDen))
        RaiseException (EXCEPTION_RESULT_HC4V, 0, 0, NULL);
    else
    // If the denominator is {Inf}, ...
    if (IsMpqInfinity (&aplDen))
    {
        // Calculate the numerator
        aplNum = ConjHC4R (lpatRht->aplHC4R);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            mpq_init_set_d (&lpMemRes[uRes].parts[i],
                            bAllowNeg0 ? ((SIGN_APLRAT (&aplNum.parts[i])
                                        EQ SIGN_APLRAT (&aplDen)) ?  0.0
                                                                  : -0.0)
                                        : 0.0);
    } else
    {
        // Calculate the numerator
        aplNum = ConjHC4R (lpatRht->aplHC4R);

        // Initialize to 0/1
        mphc4r_init (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            mpq_div (&lpMemRes[uRes].parts[i],
                     &aplNum.parts[i],
                     &aplDen);
    } // End IF/ELSE
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
    int      i;
    APLVFP   aplDen = {0},      // Denominator
             aplTmp;            // Temp
    APLHC4V  aplNum;            // Numerator
    UBOOL    bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Initialize to 0
    mpfr_init0 (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Calculate the denominator
        // Square the next right arg part
        aplTmp = MulHC1V_RE (lpatRht->aplHC4V.parts[i], lpatRht->aplHC4V.parts[i]);

        // Add into the accumulator
        mpfr_add (&aplDen, &aplDen, &aplTmp, MPFR_RNDN);
    } // End FOR

    Myf_clear (&aplTmp);

    // If the denominator is 0, ...
    if (IsMpf0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] = *mpfr_QuadICValue (&lpatRht->aplHC4V.parts[0],     // No left arg
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
        // Calculate the numerator
        aplNum = ConjHC4V (lpatRht->aplHC4V);

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
        // Calculate the numerator
        aplNum = ConjHC4V (lpatRht->aplHC4V);

        // Initialize to 0
        mphc4v_init0 (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            mpfr_div (&lpMemRes[uRes].parts[i],
                      &aplNum.parts[i],
                      &aplDen,
                       MPFR_RNDN);
    } // End IF/ELSE
} // End PrimFnMonColonBarHC4VisHC4V


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
    int      i;
    APLFLOAT aplDen = 0.0;      // Denominator
    APLHC8F  aplNum;            // Numerator

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Calculate the denominator
        // Square and sum the next right arg part
        aplDen = AddHC1F_RE (aplDen,
                             MulHC1F_RE (lpatRht->aplHC8F.parts[i], lpatRht->aplHC8F.parts[i]));
    } // End FOR

    // If the denominator is 0, ...
    if (aplDen EQ 0.0)
    {
        lpMemRes[uRes].parts[0] = TranslateQuadICIndex (0,
                                                        ICNDX_DIV0,
                                                        lpatRht->aplHC8F.parts[0],
                                                        SIGN_APLFLOAT (lpatRht->aplHC8F.parts[0]));
        lpMemRes[uRes].parts[1] = 0;
    } else
    // If the denominator is {Inf}, ...
    if (IsFltInfinity (aplDen))
    {
        UBOOL bAllowNeg0 = (UBOOL) gAllowNeg0;

        // Calculate the numerator
        aplNum = ConjHC8F (lpatRht->aplHC8F);

        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            lpMemRes[uRes].parts[i] = (bAllowNeg0 ? ((SIGN_APLFLOAT_RAW (aplNum.parts[i])
                                                   EQ SIGN_APLFLOAT_RAW (aplDen)) ?  0.0
                                                                                  : -0.0)
                                                  : 0.0);
    } else
    {
        // Calculate the numerator
        aplNum = ConjHC8F (lpatRht->aplHC8F);

        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            // The FPU handles overflow and underflow for us
            lpMemRes[uRes].parts[i] = aplNum.parts[i] / aplDen;
    } // End IF/ELSE
} // End PrimFnMonColonBarHC8FisHC8F


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
    int      i;
    APLRAT   aplDen = {0},      // Denominator
             aplTmp;            // Temp
    APLHC8R  aplNum;            // Numerator
    UBOOL    bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Initialize to 0/1
    mpq_init (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Calculate the denominator
        // Square the next right arg part
        aplTmp = MulHC1R_RE (lpatRht->aplHC8R.parts[i], lpatRht->aplHC8R.parts[i]);

        // Add into the accumulator
        mpq_add (&aplDen, &aplDen, &aplTmp);
    } // End FOR

    Myq_clear (&aplTmp);

    // If the denominator is 0, ...
    if (IsMpq0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] = *mpq_QuadICValue (&lpatRht->aplHC8R.parts[0],     // No left arg
                                                     ICNDX_DIV0,
                                                    &lpatRht->aplHC8R.parts[0],
                                                    &lpMemRes[uRes].parts[0],
                                                     FALSE);
        // Loop through the imaginary parts
        for (i = 1; i < 8; i++)
            // Initialize to 0/1
            mpq_init (&lpMemRes[uRes].parts[i]);
    } else
    // Check for special case:  {div}{neg}{inf}x
    if (bAllowNeg0
     && IsMpqInfinity (&aplDen)
     && SIGN_APLRAT (&aplDen))
        RaiseException (EXCEPTION_RESULT_HC8V, 0, 0, NULL);
    else
    // If the denominator is {Inf}, ...
    if (IsMpqInfinity (&aplDen))
    {
        // Calculate the numerator
        aplNum = ConjHC8R (lpatRht->aplHC8R);

        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            mpq_init_set_d (&lpMemRes[uRes].parts[i],
                            bAllowNeg0 ? ((SIGN_APLRAT (&aplNum.parts[i])
                                        EQ SIGN_APLRAT (&aplDen)) ?  0.0
                                                                  : -0.0)
                                       : 0.0);

    } else
    {
        // Calculate the numerator
        aplNum = ConjHC8R (lpatRht->aplHC8R);

        // Initialize to 0/1
        mphc8r_init (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            mpq_div (&lpMemRes[uRes].parts[i],
                     &aplNum.parts[i],
                     &aplDen);
    } // End IF/ELSE
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
    int      i;
    APLVFP   aplDen = {0},      // Denominator
             aplTmp;            // Temp
    APLHC8V  aplNum;            // Numerator
    UBOOL    bAllowNeg0 = (UBOOL) gAllowNeg0;

    // Initialize to 0
    mpfr_init0 (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Calculate the denominator
        // Square the next right arg part
        aplTmp = MulHC1V_RE (lpatRht->aplHC8V.parts[i], lpatRht->aplHC8V.parts[i]);

        // Add into the accumulator
        mpfr_add (&aplDen, &aplDen, &aplTmp, MPFR_RNDN);
    } // End FOR

    Myf_clear (&aplTmp);

    // If the denominator is 0, ...
    if (IsMpf0 (&aplDen))
    {
        lpMemRes[uRes].parts[0] = *mpfr_QuadICValue (&lpatRht->aplHC8V.parts[0],     // No left arg
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
        // Calculate the numerator
        aplNum = ConjHC8V (lpatRht->aplHC8V);

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
        // Calculate the numerator
        aplNum = ConjHC8V (lpatRht->aplHC8V);

        // Initialize to 0
        mphc8v_init0 (&lpMemRes[uRes]);

        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            mpfr_div (&lpMemRes[uRes].parts[i],
                      &aplNum.parts[i],
                      &aplDen,
                       MPFR_RNDN);
    } // End IF/ELSE
} // End PrimFnMonColonBarHC8VisHC8V


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
    if (IsSimpleInt (aplTypeRes))
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

    // Check for indeterminates:  0 {div} 0
    if (aplLft EQ 0
     || aplRht EQ 0)
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
        lpMemRes[uRes] = TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                                          ICNDX_0DIV0,
                                               (APLFLOAT) lpatRht->aplInteger,
                                                          FALSE);
    } else
    // Check for indeterminates:  L {div} 0
    if (lpatRht->aplInteger EQ 0)
    {
        lpMemRes[uRes] = TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
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

    // Check for indeterminates:  0 {div} 0
    if (aplLft EQ 0
     && aplRht EQ 0)
    {
        aplRes = TranslateQuadICIndex (aplLft,
                                       ICNDX_0DIV0,
                                       aplRht,
                                       SIGN_APLFLOAT (aplLft) NE SIGN_APLFLOAT (aplRht));
    } else
    // Check for indeterminates:  L {div} 0
    if (aplRht EQ 0)
    {
        aplRes = TranslateQuadICIndex (aplLft,
                                       ICNDX_DIV0,
                                       aplRht,
                                       SIGN_APLFLOAT (aplLft) NE SIGN_APLFLOAT (aplRht));
    } else
    // Check for indeterminates:  _ {div} _ (same or different signs)
    if (IsFltInfinity (aplLft)
     && IsFltInfinity (aplRht))
    {
        if (SIGN_APLFLOAT (aplLft) EQ SIGN_APLFLOAT (aplRht))
            aplRes = TranslateQuadICIndex (aplLft,
                                           ICNDX_PiDIVPi,
                                           aplRht,
                                           FALSE);
        else
            aplRes = TranslateQuadICIndex (aplLft,
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

    // Check for indeterminates:  0 {div} 0
    if (IsMpq0 (&aplLft)
     && IsMpq0 (&aplRht))
    {
        aplRes = *mpq_QuadICValue (&aplLft,
                                    ICNDX_0DIV0,
                                   &aplRht,
                                   &aplRes,
                                    FALSE);
    } else
    // Check for indeterminates:  L {div} 0
    if (IsMpq0 (&aplRht))
    {
        aplRes = *mpq_QuadICValue (&aplLft,
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
            aplRes = *mpq_QuadICValue (&aplLft,
                                        ICNDX_PiDIVPi,
                                       &aplRht,
                                       &aplRes,
                                        FALSE);
        else
            aplRes = *mpq_QuadICValue (&aplLft,
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
#ifdef DEBUG_FMT
    WCHAR wszTemp[512];
#endif

    // Check for indeterminates:  0 {div} 0
    if (IsMpf0 (&aplLft)
     && IsMpf0 (&aplRht))
    {
        aplRes = *mpfr_QuadICValue (&aplLft,
                                     ICNDX_0DIV0,
                                    &aplRht,
                                    &aplRes,
                                     SIGN_APLVFP (&aplLft) NE
                                     SIGN_APLVFP (&aplRht));
    } else
    // Check for indeterminates:  L {div} 0
    if (IsMpf0 (&aplRht))
    {
        aplRes = *mpfr_QuadICValue (&aplLft,
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
            aplRes = *mpfr_QuadICValue (&aplLft,
                                         ICNDX_PiDIVPi,
                                        &aplRht,
                                        &aplRes,
                                         FALSE);
        else
            aplRes = *mpfr_QuadICValue (&aplLft,
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

#ifdef DEBUG_FMT
    strcpyW (wszTemp, L"L:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], aplLft, 0) = WC_EOS; DbgMsgW (wszTemp);
    strcpyW (wszTemp, L"R:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], aplRht, 0) = WC_EOS; DbgMsgW (wszTemp);
    strcpyW (wszTemp, L"Z:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], aplRes, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif

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
//  $DivHC2I_RE
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- DivHC2I_RE"
#else
#define APPEND_NAME
#endif

APLHC2I DivHC2I_RE
    (APLHC2I aplLft,                    // Left arg
     APLHC2I aplRht)                    // Right ...

{
    int     i;                      // Loop counter
    APLINT  aplDen = 0;             // Denominator
    APLHC2I aplNum,                 // Numerator
            aplRes;                 // Result

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Calculate the denominator
        aplDen = AddHC1I_RE (aplDen, MulHC1I_RE (aplRht.parts[i], aplRht.parts[i]));

    // If the denominator is 0, ...
    if (aplDen EQ 0)
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

     // Calculate the conjugate
     aplNum = ConjHC2I (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC2I_RE (        aplNum, aplLft);
    else
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
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydColonBarHC2IisHC2IvHC2I
//
//  Primitive scalar function dyadic ColonBar:  HC2I {is} HC2I fn HC2I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC2IisHC2IvHC2I"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2F);
} // End PrimFnDydColonBarHC2IisHC2IvHC2I
#undef  APPEND_NAME


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
    APLFLOAT aplDen = 0;        // Denominator
    APLHC2F  aplNum,            // Numerator
             aplRes;            // Result

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Calculate the denominator
        aplDen = AddHC1F_RE (aplDen, MulHC1F_RE (aplRht.parts[i], aplRht.parts[i]));

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC2F_RE (        ConjHC2F (aplRht), aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC2F_RE (aplLft, ConjHC2F (aplRht));
    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    // Check for indeterminates:  0 {div} 0
    if (aplNum.parts[i] EQ 0
     && aplDen EQ 0)
    {
        aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
                                                ICNDX_0DIV0,
                                                aplRht.parts[i],
                                                SIGN_APLFLOAT (aplLft.parts[i]) NE SIGN_APLFLOAT (aplRht.parts[i]));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (aplDen EQ 0)
    {
        aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
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
            aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
                                                    ICNDX_PiDIVPi,
                                                    aplRht.parts[i],
                                                    FALSE);
        else
            aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
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

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC2FisHC2FvHC2F"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC2FisHC2FvHC2F
#undef  APPEND_NAME


//***************************************************************************
//  $DivHC2R_RE
//***************************************************************************

APLHC2R DivHC2R_RE
    (APLHC2R aplLft,                    // Left arg
     APLHC2R aplRht)                    // Right ...

{
    int      i;                 // Loop counter
    APLRAT   aplDen = {0},      // Denominator
             aplTmp1,           // Temp
             aplTmp2;           // ...
    APLHC2R  aplNum,            // Numerator
             aplTmp3,           // Temp
             aplRes = {0};      // Result

    // Initialize to 0/1
    mpq_init (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Square the part
        aplTmp1 = MulHC1R_RE (aplRht.parts[i], aplRht.parts[i]);

        // Accumulate
        aplTmp2 = AddHC1R_RE (aplDen, aplTmp1);

        // Save in the denominator
        mpq_set (&aplDen, &aplTmp2);

        // We no longer need these reources
        Myq_clear (&aplTmp2);
        Myq_clear (&aplTmp1);
    } // End FOR

    // Calculate the conjugate
    aplTmp3 = ConjHC2R (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC2R_RE (        aplTmp3, aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC2R_RE (aplLft, aplTmp3);

    Myhc2r_clear (&aplTmp3);

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

    return aplRes;
} // End DivHC2R_RE


//***************************************************************************
//  $PrimFnDydColonBarHC2RisHC2RvHC2R
//
//  Primitive scalar function dyadic ColonBar:  HC2R {is} HC2R fn HC2R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC2RisHC2RvHC2R"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2V);
} // End PrimFnDydColonBarHC2RisHC2RvHC2R
#undef  APPEND_NAME


//***************************************************************************
//  $DivHC2V_RE
//***************************************************************************

APLHC2V DivHC2V_RE
    (APLHC2V aplLft,                    // Left arg
     APLHC2V aplRht)                    // Right ...

{
    int      i;
    APLVFP   aplDen = {0},      // Denominator
             aplTmp1,           // Temp
             aplTmp2;           // ...
    APLHC2V  aplNum,            // Numerator
             aplTmp3,           // Temp
             aplRes = {0};      // Result

    // Initialize to 0
    mpfr_init0 (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Square the part
        aplTmp1 = MulHC1V_RE (aplRht.parts[i], aplRht.parts[i]);

        // Accumulate
        aplTmp2 = AddHC1V_RE (aplDen, aplTmp1);

        // Save in the denominator
        mpfr_set (&aplDen, &aplTmp2, MPFR_RNDN);

        // We no longer need these reources
        Myf_clear (&aplTmp2);
        Myf_clear (&aplTmp1);
    } // End FOR

    // Calculate the conjugate
    aplTmp3 = ConjHC2V (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
        // Calculate the numerator
        aplNum  = MulHC2V_RE (        aplTmp3, aplLft);
    else
        // Calculate the numerator
        aplNum  = MulHC2V_RE (aplLft, aplTmp3);

    Myhc2v_clear (&aplTmp3);

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

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC2VisHC2VvHC2V"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC2VisHC2VvHC2V
#undef  APPEND_NAME


//***************************************************************************
//  $DivHC4I_RE
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- DivHC4I_RE"
#else
#define APPEND_NAME
#endif

APLHC4I DivHC4I_RE
    (APLHC4I aplLft,                    // Left arg
     APLHC4I aplRht)                    // Right ...

{
    int     i;                      // Loop counter
    APLINT  aplDen = 0;             // Denominator
    APLHC4I aplNum,                 // Numerator
            aplRes;                 // Result

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Calculate the denominator
        aplDen = AddHC1I_RE (aplDen, MulHC1I_RE (aplRht.parts[i], aplRht.parts[i]));

    // If the denominator is 0, ...
    if (aplDen EQ 0)
        RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);

    // Calculate the conjugate
    aplNum = ConjHC4I (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
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
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydColonBarHC4IisHC4IvHC4I
//
//  Primitive scalar function dyadic ColonBar:  HC4I {is} HC4I fn HC4I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC4IisHC4IvHC4I"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4F);
} // End PrimFnDydColonBarHC4IisHC4IvHC4I
#undef  APPEND_NAME


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
    APLFLOAT aplDen = 0;        // Denominator
    APLHC4F  aplNum,            // Numerator
             aplRes;            // Result

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Calculate the denominator
        aplDen = AddHC1F_RE (aplDen, MulHC1F_RE (aplRht.parts[i], aplRht.parts[i]));

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC4F_RE (        ConjHC4F (aplRht), aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC4F_RE (aplLft, ConjHC4F (aplRht));

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    // Check for indeterminates:  0 {div} 0
    if (aplNum.parts[i] EQ 0
     && aplDen EQ 0)
    {
        aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
                                                ICNDX_0DIV0,
                                                aplRht.parts[i],
                                                SIGN_APLFLOAT (aplLft.parts[i]) NE SIGN_APLFLOAT (aplRht.parts[i]));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (aplDen EQ 0)
    {
        aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
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
            aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
                                                    ICNDX_PiDIVPi,
                                                    aplRht.parts[i],
                                                    FALSE);
        else
            aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
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

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC4FisHC4FvHC4F"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC4FisHC4FvHC4F
#undef  APPEND_NAME


//***************************************************************************
//  $DivHC4R_RE
//***************************************************************************

APLHC4R DivHC4R_RE
    (APLHC4R aplLft,                    // Left arg
     APLHC4R aplRht)                    // Right ...

{
    int      i;                 // Loop counter
    APLRAT   aplDen = {0},      // Denominator
             aplTmp1,           // Temp
             aplTmp2;           // ...
    APLHC4R  aplNum,            // Numerator
             aplTmp3,           // Temp
             aplRes = {0};      // Result

    // Initialize to 0/1
    mpq_init (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Square the part
        aplTmp1 = MulHC1R_RE (aplRht.parts[i], aplRht.parts[i]);

        // Accumulate
        aplTmp2 = AddHC1R_RE (aplDen, aplTmp1);

        // Save in the denominator
        mpq_set (&aplDen, &aplTmp2);

        // We no longer need these reources
        Myq_clear (&aplTmp2);
        Myq_clear (&aplTmp1);
    } // End FOR

    // Calculate the conjugate
    aplTmp3 = ConjHC4R (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
        // Calculate the numerator
        aplNum  = MulHC4R_RE (        aplTmp3, aplLft);
    else
        // Calculate the numerator
        aplNum  = MulHC4R_RE (aplLft, aplTmp3);

    Myhc4r_clear (&aplTmp3);

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

    return aplRes;
} // End DivHC4R_RE


//***************************************************************************
//  $PrimFnDydColonBarHC4RisHC4RvHC4R
//
//  Primitive scalar function dyadic ColonBar:  HC4R {is} HC4R fn HC4R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC4RisHC4RvHC4R"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4V);
} // End PrimFnDydColonBarHC4RisHC4RvHC4R
#undef  APPEND_NAME


//***************************************************************************
//  $DivHC4V_RE
//***************************************************************************

APLHC4V DivHC4V_RE
    (APLHC4V aplLft,                    // Left arg
     APLHC4V aplRht)                    // Right ...

{
    int      i;
    APLVFP   aplDen = {0},      // Denominator
             aplTmp1,           // Temp
             aplTmp2;           // ...
    APLHC4V  aplNum,            // Numerator
             aplTmp3,           // Temp
             aplRes = {0};      // Result

    // Initialize to 0
    mpfr_init0 (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Square the part
        aplTmp1 = MulHC1V_RE (aplRht.parts[i], aplRht.parts[i]);

        // Accumulate
        aplTmp2 = AddHC1V_RE (aplDen, aplTmp1);

        // Save in the denominator
        mpfr_set (&aplDen, &aplTmp2, MPFR_RNDN);

        // We no longer need these reources
        Myf_clear (&aplTmp2);
        Myf_clear (&aplTmp1);
    } // End FOR

    // Calculate the conjugate
    aplTmp3 = ConjHC4V (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
        // Calculate the numerator
        aplNum  = MulHC4V_RE (        aplTmp3, aplLft);
    else
        // Calculate the numerator
        aplNum  = MulHC4V_RE (aplLft, aplTmp3);

    Myhc4v_clear (&aplTmp3);

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

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC4VisHC4VvHC4V"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC4VisHC4VvHC4V
#undef  APPEND_NAME


//***************************************************************************
//  $DivHC8I_RE
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- DivHC8I_RE"
#else
#define APPEND_NAME
#endif

APLHC8I DivHC8I_RE
    (APLHC8I aplLft,                    // Left arg
     APLHC8I aplRht)                    // Right ...

{
    int     i;                      // Loop counter
    APLINT  aplDen = 0;             // Denominator
    APLHC8I aplNum,                 // Numerator
            aplRes;                 // Result

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Calculate the denominator
        aplDen = AddHC1I_RE (aplDen, MulHC1I_RE (aplRht.parts[i], aplRht.parts[i]));

    // If the denominator is 0, ...
    if (aplDen EQ 0)
        RaiseException (EXCEPTION_RESULT_HC8F, 0, 0, NULL);

     // Calculate the conjugate
     aplNum = ConjHC8I (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
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
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydColonBarHC8IisHC8IvHC8I
//
//  Primitive scalar function dyadic ColonBar:  HC8I {is} HC8I fn HC8I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC8IisHC8IvHC8I"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8F);
} // End PrimFnDydColonBarHC8IisHC8IvHC8I
#undef  APPEND_NAME


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
    APLFLOAT aplDen = 0;        // Denominator
    APLHC8F  aplNum,            // Numerator
             aplRes;            // Result

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Calculate the denominator
        aplDen = AddHC1F_RE (aplDen, MulHC1F_RE (aplRht.parts[i], aplRht.parts[i]));

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
        // Calculate the numerator
        aplNum = MulHC8F_RE (        ConjHC8F (aplRht), aplLft);
    else
        // Calculate the numerator
        aplNum = MulHC8F_RE (aplLft, ConjHC8F (aplRht));

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    // Check for indeterminates:  0 {div} 0
    if (aplNum.parts[i] EQ 0
     && aplDen EQ 0)
    {
        aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
                                                ICNDX_0DIV0,
                                                aplRht.parts[i],
                                                SIGN_APLFLOAT (aplLft.parts[i]) NE SIGN_APLFLOAT (aplRht.parts[i]));
        break;
    } else
    // Check for indeterminates:  L {div} 0
    if (aplDen EQ 0)
    {
        aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
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
            aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
                                                    ICNDX_PiDIVPi,
                                                    aplRht.parts[i],
                                                    FALSE);
        else
            aplRes.parts[0] = TranslateQuadICIndex (aplLft.parts[i],
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

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC8FisHC8FvHC8F"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC8FisHC8FvHC8F
#undef  APPEND_NAME


//***************************************************************************
//  $DivHC8R_RE
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- DivHC8R_RE"
#else
#define APPEND_NAME
#endif

APLHC8R DivHC8R_RE
    (APLHC8R aplLft,                    // Left arg
     APLHC8R aplRht)                    // Right ...

{
    int      i;                 // Loop counter
    APLRAT   aplDen = {0},      // Denominator
             aplTmp1,           // Temp
             aplTmp2;           // ...
    APLHC8R  aplNum,            // Numerator
             aplTmp3 = {0},     // Temp
             aplRes = {0};      // Result

    // Initialize to 0/1
    mpq_init (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Square the part
        aplTmp1 = MulHC1R_RE (aplRht.parts[i], aplRht.parts[i]);

        // Accumulate
        aplTmp2 = AddHC1R_RE (aplDen, aplTmp1);

        // Save in the denominator
        mpq_set (&aplDen, &aplTmp2);

        // We no longer need these reources
        Myq_clear (&aplTmp2);
        Myq_clear (&aplTmp1);
    } // End FOR

    // Calculate the conjugate
    aplTmp3 = ConjHC8R (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
        // Calculate the numerator
        aplNum  = MulHC8R_RE (        aplTmp3, aplLft);
    else
        // Calculate the numerator
        aplNum  = MulHC8R_RE (aplLft, aplTmp3);

    Myhc8r_clear (&aplTmp3);

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

    return aplRes;
} // End DivHC8R_RE
#undef  APPEND_NAME

//***************************************************************************
//  $PrimFnDydColonBarHC8RisHC8RvHC8R
//
//  Primitive scalar function dyadic ColonBar:  HC8R {is} HC8R fn HC8R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC8RisHC8RvHC8R"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8V);
} // End PrimFnDydColonBarHC8RisHC8RvHC8R
#undef  APPEND_NAME


//***************************************************************************
//  $DivHC8V_RE
//***************************************************************************

APLHC8V DivHC8V_RE
    (APLHC8V aplLft,                    // Left arg
     APLHC8V aplRht)                    // Right ...

{
    int      i;
    APLVFP   aplDen = {0},      // Denominator
             aplTmp1,           // Temp
             aplTmp2;           // ...
    APLHC8V  aplNum,            // Numerator
             aplTmp3,           // Temp
             aplRes = {0};      // Result

    // Initialize to 0
    mpfr_init0 (&aplDen);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Square the part
        aplTmp1 = MulHC1V_RE (aplRht.parts[i], aplRht.parts[i]);

        // Accumulate
        aplTmp2 = AddHC1V_RE (aplDen, aplTmp1);

        // Save in the denominator
        mpfr_set (&aplDen, &aplTmp2, MPFR_RNDN);

        // We no longer need these reources
        Myf_clear (&aplTmp2);
        Myf_clear (&aplTmp1);
    } // End FOR

    // Calculate the conjugate
    aplTmp3 = ConjHC8V (aplRht);

    // If the user wants the left quotient, ...
    if (GetQuadDQ () EQ 'l')
        // Calculate the numerator
        aplNum  = MulHC8V_RE (        aplTmp3, aplLft);
    else
        // Calculate the numerator
        aplNum  = MulHC8V_RE (aplLft, aplTmp3);

    Myhc8v_clear (&aplTmp3);

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

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydColonBarHC8VisHC8VvHC8V"
#else
#define APPEND_NAME
#endif

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
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydColonBarHC8VisHC8VvHC8V
#undef  APPEND_NAME


//***************************************************************************
//  $DivHC2F_F
//***************************************************************************

APLHC2F DivHC2F_F
    (APLHC2F  aplLft,                   // Left arg
     APLFLOAT aplRht)                   // Right ...

{
    int     i;                  // Loop counter
    APLHC2F aplRes;             // Result

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
////WCHAR   wszTemp[1024];

    // Initialize to 0
    mphc2v_init0 (&aplRes);

////strcpyW (wszTemp, L"Lft (Div):  "); *FormatAplHC2V (&wszTemp[lstrlenW (wszTemp)], &aplLft, 0) = WC_EOS; DbgMsgW (wszTemp);
////strcpyW (wszTemp, L"Rht (Div):  "); *FormatAplVfp  (&wszTemp[lstrlenW (wszTemp)], &aplRht, 0) = WC_EOS; DbgMsgW (wszTemp);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        mpfr_div (&aplRes.parts[i], &aplLft.parts[i], &aplRht, MPFR_RNDN);

////strcpyW (wszTemp, L"Res (Div):  "); *FormatAplHC2V (&wszTemp[lstrlenW (wszTemp)], &aplRes, 0) = WC_EOS; DbgMsgW (wszTemp);

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

    // Initialize to 0
    mphc8v_init0 (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        mpfr_div (&aplRes.parts[i], &aplLft.parts[i], &aplRht, MPFR_RNDN);

    return aplRes;
} // End DivHC8V_V


//***************************************************************************
//  End of File: pf_colonbar.c
//***************************************************************************