//***************************************************************************
//  NARS2000 -- Primitive Function -- CircleStar
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

////#define OWN_EXPLOG

#define STRICT
#include <windows.h>
#include <math.h>
#include "headers.h"

#ifndef PROTO
PRIMSPEC PrimSpecCircleStar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecCircleStarStorageTypeMon,
    NULL,   // &PrimFnMonCircleStarAPA_EM, -- Can't happen w/CircleStar

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonCircleStarBisB, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarBisI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarBisF, -- Can't happen w/CircleStar

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonCircleStarIisI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarIisF, -- Can't happen w/CircleStar
    &PrimFnMonCircleStarFisI,
    &PrimFnMonCircleStarFisF,
    NULL,   // &PrimFnMonCircleStarRisR, -- Can't happen w/CircleStar
    &PrimFnMonCircleStarVisR,
    &PrimFnMonCircleStarVisV,

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonCircleStarHC2IisHC2I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarHC2IisHC2F, -- Can't happen w/CircleStar
    &PrimFnMonCircleStarHC2FisHC2I,
    &PrimFnMonCircleStarHC2FisHC2F,
    NULL,   // &PrimFnMonCircleStarHC2RisHC2R, -- Can't happen w/CircleStar
    &PrimFnMonCircleStarHC2VisHC2R,
    &PrimFnMonCircleStarHC2VisHC2V,

    NULL,   // &PrimFnMonCircleStarHC4IisHC4I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarHC4IisHC4F, -- Can't happen w/CircleStar
    &PrimFnMonCircleStarHC4FisHC4I,
    &PrimFnMonCircleStarHC4FisHC4F,
    NULL,   // &PrimFnMonCircleStarHC4RisHC4R, -- Can't happen w/CircleStar
    &PrimFnMonCircleStarHC4VisHC4R,
    &PrimFnMonCircleStarHC4VisHC4V,

    NULL,   // &PrimFnMonCircleStarHC8IisHC8I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarHC8IisHC8F, -- Can't happen w/CircleStar
    &PrimFnMonCircleStarHC8FisHC8I,
    &PrimFnMonCircleStarHC8FisHC8F,
    NULL,   // &PrimFnMonCircleStarHC8RisHC8R, -- Can't happen w/CircleStar
    &PrimFnMonCircleStarHC8VisHC8R,
    &PrimFnMonCircleStarHC8VisHC8V,

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonCircleStarFisHC2I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarFisHC2F, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarVisHC2R, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarVisHC2V, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarFisHC4I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarFisHC4F, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarVisHC4R, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarVisHC4V, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarFisHC8I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarFisHC8F, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarVisHC8R, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarVisHC8V, -- Can't happen w/CircleStar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecCircleStarStorageTypeDyd,
    NULL,   // &PrimFnDydCircleStarAPA_EM, -- Can't happen w/CircleStar

    // Dyadic Boolean result functions
    &PrimFnDydRightCaretUnderbarBisBvB,
    NULL,   // &PrimFnDydCircleStarBisIvI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisFvF, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisCvC, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisHvH, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisRvR, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisVvV, -- Can't happen w/CircleStar

    NULL,   // &PrimFnDydCircleStarBisHC2IvHC2I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisHC2FvHC2F, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisHC2RvHC2R, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisHC2VvHC2V, -- Can't happen w/CircleStar

    NULL,   // &PrimFnDydCircleStarBisHC4IvHC4I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisHC4FvHC4F, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisHC4RvHC4R, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisHC4VvHC4V, -- Can't happen w/CircleStar

    NULL,   // &PrimFnDydCircleStarBisHC8IvHC8I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisHC8FvHC8F, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisHC8RvHC8R, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisHC8VvHC8V, -- Can't happen w/CircleStar

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydCircleStarIisIvI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarIisFvF, -- Can't happen w/CircleStar
    &PrimFnDydCircleStarFisIvI,
    &PrimFnDydCircleStarFisFvF,
    NULL,   // &PrimFnDydCircleStarRisRvR, -- Can't happen w/CircleStar
    &PrimFnDydCircleStarVisRvR,
    &PrimFnDydCircleStarVisVvV,

    NULL,   // &PrimFnDydCircleStarHC2IisHC2IvHC2I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarHC2IisHC2FvHC2F, -- Can't happen w/CircleStar
    &PrimFnDydCircleStarHC2FisHC2IvHC2I,
    &PrimFnDydCircleStarHC2FisHC2FvHC2F,
    NULL,   // &PrimFnDydCircleStarHC2RisHC2RvHC2R, -- Can't happen w/CircleStar
    &PrimFnDydCircleStarHC2VisHC2RvHC2R,
    &PrimFnDydCircleStarHC2VisHC2VvHC2V,

    NULL,   // &PrimFnDydCircleStarHC4IisHC4IvHC4I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarHC4IisHC4FvHC4F, -- Can't happen w/CircleStar
    &PrimFnDydCircleStarHC4FisHC4IvHC4I,
    &PrimFnDydCircleStarHC4FisHC4FvHC4F,
    NULL,   // &PrimFnDydCircleStarHC4RisHC4RvHC4R, -- Can't happen w/CircleStar
    &PrimFnDydCircleStarHC4VisHC4RvHC4R,
    &PrimFnDydCircleStarHC4VisHC4VvHC4V,

    NULL,   // &PrimFnDydCircleStarHC8IisHC8IvHC8I, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarHC8IisHC8FvHC8F, -- Can't happen w/CircleStar
    &PrimFnDydCircleStarHC8FisHC8IvHC8I,
    &PrimFnDydCircleStarHC8FisHC8FvHC8F,
    NULL,   // &PrimFnDydCircleStarHC8RisHC8RvHC8R, -- Can't happen w/CircleStar
    &PrimFnDydCircleStarHC8VisHC8RvHC8R,
    &PrimFnDydCircleStarHC8VisHC8VvHC8V,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonCircleStarB64isB64, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarB32isB32, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarB16isB16, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarB08isB08, -- Can't happen w/CircleStar

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonCircleStarB64isB64vB64, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarB32isB32vB32, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarB16isB16vB16, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarB08isB08vB08, -- Can't happen w/CircleStar

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecCircleStar};
#endif

IC_INDICES icndxLog[2][2] =
{{ICNDX_0LOG0, ICNDX_0LOG1},
 {ICNDX_1LOG0, ICNDX_1LOG1}};


//***************************************************************************
//  $PrimFnCircleStar_EM_YY
//
//  Primitive function for monadic and dyadic CircleStar
//     ("natural logarithm" and "logarithm")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnCircleStar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnCircleStar_EM_YY
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis)        // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnCircleStar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecCircleStarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStarStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE aplTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // The storage type of the result is
    //   the same as that of the right arg
    aplTypeRes = *lpaplTypeRht;

    // Split cases based upon the storage type
    switch (aplTypeRes)
    {
        // Except that BOOL, INT and APA become FLOAT
        case ARRAY_BOOL:
        case ARRAY_APA:
            aplTypeRes = ARRAY_FLOAT;

            break;

        // Except that INT -> FLT, RAT -> VFP, etc.
        case ARRAY_INT:
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            aplTypeRes++;               // Assuming order as in <ARRAY_TYPES>

            break;

        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
        case ARRAY_NESTED:
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
            aplTypeRes = ARRAY_ERROR;

            break;

        defstop
            break;
    } // End SWITCH

    return aplTypeRes;
} // End PrimSpecCircleStarStorageTypeMon


//***************************************************************************
//  $PrimFnMonCircleStarFisI
//
//  Primitive scalar function monadic CircleStar:  F {is} fn I
//***************************************************************************

void PrimFnMonCircleStarFisI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for Complex result
    if (SIGN_APLFLOAT (lpatRht->aplInteger))
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
    // Check for indeterminates:  {log} 0
    if (lpatRht->aplInteger EQ 0)
        lpMemRes[uRes] = TranslateQuadICIndex (0,
                                     ICNDX_LOG0,
                          (APLFLOAT) lpatRht->aplInteger,
                                     FALSE);
    else
        lpMemRes[uRes] = log ((APLFLOAT) lpatRht->aplInteger);
} // End PrimFnMonCircleStarFisI


//***************************************************************************
//  $PrimFnMonCircleStarFisF
//
//  Primitive scalar function monadic CircleStar:  F {is} fn F
//***************************************************************************

void PrimFnMonCircleStarFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for Complex result
    if (lpatRht->aplFloat < 0)        // Not SIGN_APLFLOAT as that'll catch -0 whose log is indeterminate
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
    // Check for indeterminates:  {log} 0
    if (lpatRht->aplFloat EQ 0)
        lpMemRes[uRes] = TranslateQuadICIndex (0,
                                     ICNDX_LOG0,
                                     lpatRht->aplFloat,
                                     FALSE);
    else
    // Check for special cases:  {log} _
    if (IsFltPosInfinity (lpatRht->aplFloat))
        lpMemRes[uRes] = fltPosInfinity;
    else
    // Check for special cases:  {log} -_
    if (IsFltNegInfinity (lpatRht->aplFloat))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    else
        lpMemRes[uRes] = log (lpatRht->aplFloat);
} // End PrimFnMonCircleStarFisF


//***************************************************************************
//  $PrimFnMonCircleStarVisR
//
//  Primitive scalar function monadic CircleStar:  V {is} fn R
//***************************************************************************

void PrimFnMonCircleStarVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    // Pass it on
    PrimFnMonCircleStarVisV (lpMemRes, uRes, &atRht, lpPrimSpec);

    // We no longer need this storage
    Myhc1v_clear (&atRht.aplHC1V);
} // End PrimFnMonCircleStarVisR


//***************************************************************************
//  $PrimFnMonCircleStarVisV
//
//  Primitive scalar function monadic CircleStar:  V {is} fn V
//
//  See "http://www.jsoftware.com/jwiki/Essays/Extended%20Precision%20Functions"
//***************************************************************************

void PrimFnMonCircleStarVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
#if OWN_EXPLOG
    APLMPI mpzTmp = {0};
    APLVFP lpMemRes[uRes] = {0},
           mpfTmp = {0},
           mpfLn2;
    signed long log2x;
    int    log2xSign;
#endif
    // Check for Complex result
    if (mpfr_sgn (&lpatRht->aplVfp) < 0)  // Not SIGN_APLVFP as that'll catch -0 whose log is indeterminate
        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    else
    // Check for indeterminates:  {log} 0
    if (IsMpf0 (&lpatRht->aplVfp))
        lpMemRes[uRes] =
          *mpfr_QuadICValue (&lpatRht->aplVfp,       // No left arg
                              ICNDX_LOG0,
                             &lpatRht->aplVfp,
                             &lpMemRes[uRes],
                              FALSE);
    else
    // Check for special cases:  {log} _
    if (IsMpfPosInfinity (&lpatRht->aplVfp))
        lpMemRes[uRes] = mpfPosInfinity;
    else
    // Check for special cases:  {log} -_
    if (IsMpfNegInfinity (&lpatRht->aplVfp))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    else
    {
#if OWN_EXPLOG
        // Initialize the result
        mpfr_init_copy (&lpMemRes[uRes], &lpatRht->aplVfp);

        // First, represent V as r * 2 ^ m where m is an integer
        //   and r is between 1/sqrt (2) and sqrt (2).
        // This means that log (V) = (log (r)) + m * log (2)
////////log2x = (int) (floor (0.5 + (log (mpfr_get_d (&lpatRht->aplVfp)) / log (2.0))));

        // Initialize temps to 0/1 and 0
        mpz_init (&mpzTmp);
        mpfr_init0 (&mpfTmp);

        if (mpfr_cmp (&lpMemRes[uRes], &mpfSqrt2) > 0)
        {
            mpfr_div (&mpfTmp, &lpMemRes[uRes], &mpfSqrt2, MPFR_RNDN);
            mpfr_floor (&mpfTmp, &mpfTmp);
            mpz_set_fr (&mpzTmp, &mpfTmp);
            log2x = (int) mpz_sizeinbase (&mpzTmp, 2);
            mpfr_div_2exp (&lpMemRes[uRes], &lpMemRes[uRes], log2x, MPFR_RNDN);
            log2xSign =  1;
        } else
        if (mpfr_cmp (&mpfInvSqrt2, &lpMemRes[uRes]) > 0)
        {
            mpfr_div (&mpfTmp, &mpfInvSqrt2, &lpMemRes[uRes], MPFR_RNDN);
            mpfr_floor (&mpfTmp, &mpfTmp);
            mpz_set_fr (&mpzTmp, &mpfTmp);
            log2x = (int) mpz_sizeinbase (&mpzTmp, 2);
            mpfr_mul_2exp (&lpMemRes[uRes], &lpMemRes[uRes], log2x, MPFR_RNDN);
            log2xSign = -1;
        } else
            log2x = log2xSign = 0;

        Myf_clear (&mpfTmp);
        Myz_clear (&mpzTmp);

        // Check the scale as the above calculation of log2x can be off by one
        while (mpfr_cmp (&lpMemRes[uRes], &mpfSqrt2) > 0)
        {
            mpfr_div_ui (&lpMemRes[uRes], &lpMemRes[uRes], 2);
            log2x++;
        } // End WHILE

        while (mpfr_cmp (&mpfInvSqrt2, &lpMemRes[uRes]) > 0)
        {
            mpfr_mul_ui (&lpMemRes[uRes], &lpMemRes[uRes], 2, MPFR_RNDN);
            log2x++;
        } // End WHILE

        // Calculate the log of lpMemRes[uRes] via the power series 4.1.27 in Abramowitz & Stegun
        mpfTmp = LogVfp (lpMemRes[uRes]);

        // Copy to the result
        mpfr_copy (&lpMemRes[uRes], &mpfTmp);

        // Calculate the log of 2 ...
        mpfr_set_ui (&mpfTmp, 2);
        mpfLn2 = LogVfp (mpfTmp);

        // Finally, convert the result back to normal
        mpfr_mul_ui (&mpfTmp, &mpfLn2, log2x);
        if (log2xSign < 0)
            mpfr_neg (&mpfTmp, &mpfTmp);
        mpfr_add    (&lpMemRes[uRes], &lpMemRes[uRes], &mpfTmp, MPFR_RNDN);

        // We no longer need this storage
        Myf_clear (&mpfTmp);
        Myf_clear (&mpfLn2);
#else
        // Initialize the result to 0
        mpfr_init0 (&lpMemRes[uRes]);

        // Calculate the function
        mpfr_log   (&lpMemRes[uRes], &lpatRht->aplVfp, MPFR_RNDN);
#endif
    } // End IF/ELSE/...
} // End PrimFnMonCircleStarVisV


#ifdef OWN_EXPLOG
//***************************************************************************
//  $LogVfp
//
//  Calculate the natural log of a VFP where
//       V is between 1/sqrt (2) and sqrt (2)
//
//  See "http://www.jsoftware.com/jwiki/Essays/Extended%20Precision%20Functions"
//***************************************************************************

APLVFP LogVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes   = {0},      // MPFR result
           mpfTmp1  = {0},      // ...  temp
           mpfTmp2  = {0},      // ...
           mpfBase  = {0};      // ...  base
    UINT   uRes;                // Loop counter

    // Initialize the temps to 0
    mpfr_init0 (&mpfRes);
    mpfr_init0 (&mpfTmp1);
    mpfr_init0 (&mpfTmp2);
    mpfr_init0 (&mpfBase);

    // ln z = 2 * [(1/1)*(z-1)/(z+1) + (1/3)*((z-1)/z+1))^3 + (1/5)*...]

    // Calculate Base:  (z-1)/(z+1)
    mpfr_sub_ui (&mpfTmp1, &aplVfpRht, 1);
    mpfr_add_ui (&mpfTmp2, &aplVfpRht, 1);
    mpfr_div    (&mpfBase, &mpfTmp1, &mpfTmp2);

    // Calculate the multiplier:  Base^2
    mpfr_mul    (&mpfTmp2, &mpfBase, &mpfBase);

    // Loop through the # terms
    for (uRes = 0 ; uRes < nDigitsFPC; uRes++)
    {
        // Divide the base by (2 * uRes) + 1
        mpfr_div_ui (&mpfTmp1, &mpfBase, 2 * uRes + 1);

        // Accumulate into the result
        mpfr_add (&mpfRes, &mpfRes, &mpfTmp1);

        // Multiply the base by the multiplier
        mpfr_mul (&mpfBase, &mpfBase, &mpfTmp2);
    } // End FOR

    // Multiply by the final 2
    mpfr_mul_ui (&mpfRes, &mpfRes, 2);

    // We no longer need this storage
    Myf_clear (&mpfBase);
    Myf_clear (&mpfTmp2);
    Myf_clear (&mpfTmp1);

    return mpfRes;
} // End LogVfp
#endif


//***************************************************************************
//  $PrimFnMonCircleStarHC2FisHC2I
//
//  Primitive scalar function monadic CircleStar:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonCircleStarHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonCircleStarHC2FisHC2F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonCircleStarHC2FisHC2I


//***************************************************************************
//  $ArcHCxF_RE
//***************************************************************************

APLHC8F ArcHCxF_RE
    (APLHC8F aplRht,                // Right arg
     int     iHCDimRes)             // HC Dimension (1, 2, 4, 8)

{
    APLHC8F  aplRes = {0};          // Result
    APLFLOAT aplIMag,               // Magnitude of the imaginary parts
             aplMul;                // Multiplier:  arctan2 (...) / ...
    int      i;                     // Loop counter

    /*
        From http://tamivox.org/redbear/qtrn_calc/index.html
        Ln of a Hypercomplex number:
        The real part is
          ln (|R)
        The imaginary parts are
          With g = magnitude of the imaginary parts
               h = real part
          (arctan2 (g, h) / g) times each imaginary part

        h = pF (stack[0].h);
        i = pF (stack[0].i);
        j = pF (stack[0].j);
        k = pF (stack[0].k);

        g = r_sqrt (i * i + j * j + k * k);
        m = r_sqrt (h * h + g * g);
        u = r_log (m);
        v = (g == 0) ? 0 : r_atan2 (g, h) / g;

        stack[0].h = u;
        stack[0].i = v * i;
        stack[0].j = v * j;
        stack[0].k = v * k;

        For Arc, we need to calculate v * i j k only.
    */

    // Calculate the sum of the squares of the imaginary parts
    aplIMag = 0;

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimRes; i++)
        // Calculate the sum of the squares
        aplIMag += aplRht.parts[i] * aplRht.parts[i];

    // Calculate the magnitude of the imaginary parts
    aplIMag = sqrt (aplIMag);                                       // g

    // If the number has imaginary parts, ...
    if (aplIMag NE 0)                                               // g != 0
    {
        // Calculate arctan2 (g, h) / g
        aplMul = atan2 (aplIMag, aplRht.parts[0]) / aplIMag;        // v

        // Loop through the imaginary parts
        for (i = 1; i < iHCDimRes; i++)
        // Split cases based upon whether or not the Real and/or Imaginary parts are infinite
        switch (2 * _isinf (aplRht.parts[0]) + 1 * _isinf (aplRht.parts[i]))
        {
            case 2 * 0 + 1 * 0:         // Finite, finite
                // Multiply each of the imaginary parts by the arctan2
                aplRes.parts[i] = aplRht.parts[i] * aplMul;

                break;

            case 2 * 0 + 1 * 1:         // Finite, infinite
                // Set the imaginary part to 켗i/2 depending upon the sign of the imaginary part
                aplRes.parts[i] = (aplRht.parts[i] < 0) ? -FloatPi2 : FloatPi2;

                break;

            case 2 * 1 + 1 * 0:         // Infinite, finite
                // Set the imaginary part to 0 or Pi depending upon the sign of the real part
                aplRes.parts[i] = (aplRht.parts[0] < 0) ? FloatPi : 0;

                break;

            case 2 * 1 + 1 * 1:         // Infinite, infinite
                // Split cases based upon the signs of the real and imaginary parts
                switch ((2 * (aplRht.parts[0] < 0) + 1 * (aplRht.parts[i] < 0)))
                {
                    // Set the imaginary part to Pi/4 � N * Pi/2 depending upon the signs of the real and imaginary parts
                    case 2 * 0 + 1 * 0:         // Pos, Pos
                        aplRes.parts[i] =  1 * FloatPi2 / 2;

                        break;

                    case 2 * 1 + 1 * 0:         // Neg, Pos
                        aplRes.parts[i] =  3 * FloatPi2 / 2;

                        break;

                    case 2 * 1 + 1 * 1:         // Neg, Neg
                        aplRes.parts[i] = -3 * FloatPi2 / 2;

                        break;

                    case 2 * 0 + 1 * 1:         // Pos, Neg
                        aplRes.parts[i] = -1 * FloatPi2 / 2;

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            defstop
                break;
        } // End SWITCH
    } else                                                          // g == 0
    {
        // Set the real part of the result to 0
        aplRes.parts[0] = 0.0;

        // If the real part is < 0, ...
        if (aplRht.parts[0] < 0)
            // Point the first imaginary part to +180 degrees
            aplRes.parts[1] = FloatPi;                              // v
        else
            // Point the first imaginary part to 0 degrees
            aplRes.parts[1] = 0.0;

        // Loop through the rest of the imaginary parts
        for (i = 2; i < iHCDimRes; i++)
            // Initialize to 0
            aplRes.parts[i] = 0.0;
    } // End IF/ELSE

    return aplRes;
} // End ArcHCxF_RE


//***************************************************************************
//  $LogHCxF_RE
//***************************************************************************

APLHC8F LogHCxF_RE
    (APLHC8F aplRht,                // Right arg
     int     iHCDimRes)             // HC Dimension (1, 2, 4, 8)

{
    APLHC8F  aplRes = {0};          // Result
    APLFLOAT aplRMag,               // Magnitude of the entire arg
             aplIMag,               // ...              imaginary parts
             aplMul;                // Multiplier:  arctan2 (...) / ...
    int      i;                     // Loop counter
    UBOOL    bAllowNeg0 = (UBOOL) gAllowNeg0;

    /*
        From http://tamivox.org/redbear/qtrn_calc/index.html
        Ln of a Hypercomplex number:
        The real part is
          ln (|R)
        The imaginary parts are
          With g = magnitude of the imaginary parts
               h = real part
          (arctan2 (g, h) / g) times each imaginary part

        h = pF (stack[0].h);
        i = pF (stack[0].i);
        j = pF (stack[0].j);
        k = pF (stack[0].k);

        g = r_sqrt (i * i + j * j + k * k);
        m = r_sqrt (h * h + g * g);
        u = r_log (m);
        v = (g == 0) ? 0 : r_atan2 (g, h) / g;

        stack[0].h = u;
        stack[0].i = v * i;
        stack[0].j = v * j;
        stack[0].k = v * k;
    */

    // Calculate the sum of the squares of both the entire arg and
    //   just the imaginary parts
    aplIMag = 0;

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimRes; i++)
        // Calculate the sum of the squares
        aplIMag += aplRht.parts[i] * aplRht.parts[i];

    // Calculate the magnitude of the entire arg and
    //   the magnitude of just the imaginary parts
    aplRMag = sqrt (aplIMag + aplRht.parts[0] * aplRht.parts[0]);   // m
    aplIMag = sqrt (aplIMag);                                       // g

    // Calculate the real part
    aplRes.parts[0] = log (aplRMag);                                // u

    // If the number has imaginary parts, ...
    if (aplIMag NE 0)
    {
        // Calculate arctan2 (aplIMag, aplRht.parts[0]) / aplIMag
        aplMul = atan2 (aplIMag, aplRht.parts[0]) / aplIMag;        // v

        // If the multiplier is 0, ...
        if (aplMul EQ 0.0)
        {
            // Loop through the imaginary parts
            for (i = 1; i < iHCDimRes; i++)
            {
                // If the right arg item is {Inf}, ...
                if (IsFltInfinity (aplRht.parts[i]))
                {
                    // Set the corresponding item in the result to 켗i/2
                    aplRes.parts[i] = FloatPi2;

                    if (SIGN_APLFLOAT_RAW (aplRht.parts[i]) NE SIGN_APLFLOAT_RAW (aplMul))
                        // Negate the value
                        aplRes.parts[i] = -aplRes.parts[i];
                } else
                    aplRes.parts[i] =  bAllowNeg0 ? ((SIGN_APLFLOAT (aplRht.parts[i])
                                                   EQ SIGN_APLFLOAT (aplMul)) ? 0.0
                                                                              : -0.0)
                                                  : 0.0;
            } // End FOR
        } else
////////// If the multiplier is {Inf}, ...
////////if (IsFltInfinity (aplMul))
////////{
////////    // Loop through the imaginary parts
////////    for (i = 1; i < iHCDimRes; i++)
////////        DbgBrk ();
////////
////////
////////
////////
////////} else
        // Loop through the imaginary parts
        for (i = 1; i < iHCDimRes; i++)
            // Multiply each of the imaginary parts by the arctan2
            aplRes.parts[i] = aplRht.parts[i] * aplMul;
    } else
    {
        // If the real part is < 0, ...
        if (aplRht.parts[0] < 0)
            // Point the first imaginary part to +180 degrees
            aplRes.parts[1] = FloatPi;
        else
            // Point the first imaginary part to 0 degrees
            aplRes.parts[1] = 0.0;

        // Loop through the rest of the imaginary parts
        for (i = 2; i < iHCDimRes; i++)
            // Initialize to 0
            aplRes.parts[i] = 0.0;
    } // End IF/ELSE

    return aplRes;
} // End LogHCxF_RE


//***************************************************************************
//  $PrimFnMonCircleStarHC2FisHC2F
//
//  Primitive scalar function monadic CircleStar:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonCircleStarHC2FisHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = LogHCxF_RE (lpatRht->aplHC8F, 2).partsLo[0].partsLo[0];
} // End PrimFnMonCircleStarHC2FisHC2F


//***************************************************************************
//  $PrimFnMonCircleStarHC4FisHC4I
//
//  Primitive scalar function monadic CircleStar:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonCircleStarHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonCircleStarHC4FisHC4F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonCircleStarHC4FisHC4I


//***************************************************************************
//  $PrimFnMonCircleStarHC4FisHC4F
//
//  Primitive scalar function monadic CircleStar:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonCircleStarHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = LogHCxF_RE (lpatRht->aplHC8F, 4).partsLo[0];
} // End PrimFnMonCircleStarHC4FisHC4F


//***************************************************************************
//  $PrimFnMonCircleStarHC8FisHC8I
//
//  Primitive scalar function monadic CircleStar:  HC8F {is} fn HC8I
//***************************************************************************

void PrimFnMonCircleStarHC8FisHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonCircleStarHC8FisHC8F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonCircleStarHC8FisHC8I


//***************************************************************************
//  $PrimFnMonCircleStarHC8FisHC8F
//
//  Primitive scalar function monadic CircleStar:  HC8F {is} fn HC8F
//***************************************************************************

void PrimFnMonCircleStarHC8FisHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = LogHCxF_RE (lpatRht->aplHC8F, 8);
} // End PrimFnMonCircleStarHC8FisHC8F


//***************************************************************************
//  $PrimFnMonCircleStarHC2VisHC2R
//
//  Primitive scalar function monadic CircleStar:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonCircleStarHC2VisHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplRat, 0, &atRht);

    // Pass it on
    PrimFnMonCircleStarHC2VisHC2V (lpMemRes, uRes, &atRht, lpPrimSpec);

    // We no longer need this storage
    Myhc2v_clear (&atRht.aplHC2V);
} // End PrimFnMonCircleStarHC2VisHC2R


//***************************************************************************
//  $ArcHCxV_RE
//***************************************************************************

APLHC8V ArcHCxV_RE
    (APLHC8V aplRht,                // Right arg
     int     iHCDimRes)             // HC Dimension (1, 2, 4, 8)

{
    APLHC8V  aplRes = {0};          // Result
    APLVFP   aplIMag = {0},         // Magnitude of the imaginary parts
             aplTmp;                // Temp var
    int      i;                     // Loop counter

    /*
        From http://tamivox.org/redbear/qtrn_calc/index.html
        Ln of a Hypercomplex number:
        The real part is
          ln (|R)
        The imaginary parts are
          With g = magnitude of the imaginary parts
               h = real part
          (arctan2 (g, h) / g) times each imaginary part

        h = pF (stack[0].h);
        i = pF (stack[0].i);
        j = pF (stack[0].j);
        k = pF (stack[0].k);

        g = r_sqrt (i * i + j * j + k * k);
        m = r_sqrt (h * h + g * g);
        u = r_log (m);
        v = (g == 0) ? 0 : r_atan2 (g, h) / g;

        stack[0].h = u;
        stack[0].i = v * i;
        stack[0].j = v * j;
        stack[0].k = v * k;

        For Arc, we need to calculate v * i j k only.
    */

    // Calculate the sum of the squares of the imaginary parts
    mpfr_init0 (&aplIMag);

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimRes; i++)
    {
        // Calculate the square of the imaginary part
        aplTmp = MulHC1V_RE (aplRht.parts[i], aplRht.parts[i]);

        // Calculate the sum of the squares
        mpfr_add (&aplIMag, &aplIMag, &aplTmp, MPFR_RNDN);

        // We no longer need this storage
        Myf_clear (&aplTmp);
    } // End IF

    // Calculate the magnitude of the imaginary parts
    mpfr_sqrt (&aplIMag, &aplIMag, MPFR_RNDN);                      // g

    // Initialize to 0
    mpfr_init0 (&aplRes.parts[0]);

    // If the number has imaginary parts, ...
    if (!IsMpf0 (&aplIMag))
    {
        APLVFP aplMul;              // Multiplier:  arctan2 (...) / ...

        // Initialize to 0
        mpfr_init0 (&aplMul);

        // Calculate arctan2 (aplIMag, aplRht.parts[0])
        mpfr_atan2 (&aplMul, &aplIMag, &aplRht.parts[0], MPFR_RNDN);

        // Divide by the magnitude of the imaginary parts
        // Calculate arctan2 (aplIMag, aplRht.parts[0]) / aplIMag
        mpfr_div   (&aplMul, &aplMul, &aplIMag, MPFR_RNDN);         // v

        // Loop through the imaginary parts
        for (i = 1; i < iHCDimRes; i++)
        // Split cases based upon whether or not the Real and/or Imaginary parts are infinite
        switch (2 * mpfr_inf_p (&aplRht.parts[0]) + 1 * mpfr_inf_p (&aplRht.parts[i]))
        {
            case 2 * 0 + 1 * 0:         // Finite, finite
                // Multiply each of the imaginary parts by the arctan2
                aplRes.parts[i] = MulHC1V_RE (aplRht.parts[i], aplMul);

                break;

            case 2 * 0 + 1 * 1:         // Finite, infinite
                // Set the imaginary part to 켗i/2
                mpfr_init_set (&aplRes.parts[i], &aplPi2HC8V.parts[0], MPFR_RNDN);

                if (signumvfp (&aplRht.parts[i]) < 0)
                    mpfr_neg (&aplRes.parts[i], &aplRes.parts[i], MPFR_RNDN);

                break;

            case 2 * 1 + 1 * 0:         // Infinite, finite
                // Set the imaginary part to 0 or Pi depending upon the sign of the real part
                mpfr_init_set (&aplRes.parts[i], (signumvfp (&aplRht.parts[0]) < 0) ? &aplPiHC8V.parts[0] : &mpfZero, MPFR_RNDN);

                break;

            case 2 * 1 + 1 * 1:         // Infinite, infinite
                // Copy Pi/2
                mpfr_set (&aplMul, &aplPi2HC8V.parts[0], MPFR_RNDN);

                // Calculate Pi/4
                mpfr_div_ui (&aplMul, &aplMul, 2, MPFR_RNDN);

                // Split cases based upon the signs of the real and imaginary parts
                switch ((2 * (signumvfp (&aplRht.parts[0]) < 0) + 1 * (signumvfp (&aplRht.parts[i]) < 0)))
                {
                    // Set the imaginary part to Pi/4 � N * Pi/2 depending upon the signs of the real and imaginary parts
                    case 2 * 0 + 1 * 0:         // Pos, Pos
////////////////////////aplRes.parts[i] =  1 * FloatPi2 / 2;
////////////////////////mpfr_mul_d (&aplMul, &aplMul,  1.0, MPFR_RNDN);
                        mpfr_init_set (&aplRes.parts[i], &aplMul, MPFR_RNDN);

                        break;

                    case 2 * 1 + 1 * 0:         // Neg, Pos
////////////////////////aplRes.parts[i] =  3 * FloatPi2 / 2;
                        mpfr_mul_d (&aplMul, &aplMul,  3.0, MPFR_RNDN);
                        mpfr_init_set (&aplRes.parts[i], &aplMul, MPFR_RNDN);

                        break;

                    case 2 * 1 + 1 * 1:         // Neg, Neg
////////////////////////aplRes.parts[i] = -3 * FloatPi2 / 2;
                        mpfr_mul_d (&aplMul, &aplMul, -3.0, MPFR_RNDN);
                        mpfr_init_set (&aplRes.parts[i], &aplMul, MPFR_RNDN);

                        break;

                    case 2 * 0 + 1 * 1:         // Pos, Neg
////////////////////////aplRes.parts[i] = -1 * FloatPi2 / 2;
                        mpfr_mul_d (&aplMul, &aplMul, -1.0, MPFR_RNDN);
                        mpfr_init_set (&aplRes.parts[i], &aplMul, MPFR_RNDN);

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this storage
        Myf_clear (&aplMul);
    } else
    {
        // If the real part is < 0, ...
        if (mpfr_cmp_ui (&aplRht.parts[0], 0) < 0)
            // Point the first imaginary part to +180 degrees
            mpfr_init_set (&aplRes.parts[1], &aplPiHC8V.parts[0], MPFR_RNDN);
        else
            // Point the first imaginary part to 0 degrees
            mpfr_init0    (&aplRes.parts[1]);

        // Loop through the rest of the imaginary parts
        for (i = 2; i < iHCDimRes; i++)
            // Initialize to 0
            mpfr_init0 (&aplRes.parts[i]);
    } // End IF/ELSE

    // We no longer need this storage
    Myf_clear (&aplIMag);

    return aplRes;
} // End ArcHCxV_RE


//***************************************************************************
//  $LogHCxV_RE
//***************************************************************************

APLHC8V LogHCxV_RE
    (APLHC8V aplRht,                // Right arg
     int     iHCDimRes)             // HC Dimension (1, 2, 4, 8)

{
    APLHC8V  aplRes = {0};          // Result
    APLVFP   aplRMag,               // Magnitude of the entire arg
             aplIMag = {0},         // ...              imaginary parts
             aplTmp,                // Temp var
             aplMul;                // Multiplier:  arctan2 (...) / ...
    int      i;                     // Loop counter
    UBOOL    bAllowNeg0 = (UBOOL) gAllowNeg0;

    /*
        From http://tamivox.org/redbear/qtrn_calc/index.html
        Ln of a Hypercomplex number:
        The real part is
          ln (|R)
        The imaginary parts are
          With g = magnitude of the imaginary parts
               h = real part
          (arctan2 (g, h) / g) times each imaginary part

        h = pF (stack[0].h);
        i = pF (stack[0].i);
        j = pF (stack[0].j);
        k = pF (stack[0].k);

        g = r_sqrt (i * i + j * j + k * k);
        m = r_sqrt (h * h + g * g);
        u = r_log (m);
        v = (g == 0) ? 0 : r_atan2 (g, h) / g;

        stack[0].h = u;
        stack[0].i = v * i;
        stack[0].j = v * j;
        stack[0].k = v * k;
    */

    // Calculate the sum of the squares of both the entire arg and
    //   just the imaginary parts
    mpfr_init0 (&aplIMag);

    // Calculate the square of the real part
    aplRMag = MulHC1V_RE (aplRht.parts[0], aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimRes; i++)
    {
        // Calculate the square of the imaginary part
        aplTmp = MulHC1V_RE (aplRht.parts[i], aplRht.parts[i]);

        // Calculate the sum of the squares
        mpfr_add (&aplIMag, &aplIMag, &aplTmp, MPFR_RNDN);

        // We no longer need this storage
        Myf_clear (&aplTmp);
    } // End IF

    // Add the square of the real part to the sum of the squares of the imaginary parts
    //   to get the sum of the squares of the entire arg
    mpfr_add  (&aplRMag, &aplRMag, &aplIMag, MPFR_RNDN);

    // Calculate the magnitude of the entire arg and
    //   the magnitude of just the imaginary parts
    mpfr_sqrt (&aplRMag, &aplRMag, MPFR_RNDN);                      // m
    mpfr_sqrt (&aplIMag, &aplIMag, MPFR_RNDN);                      // g

    // Initialize to 0
    mpfr_init0 (&aplRes.parts[0]);
    mpfr_init0 (&aplMul);

    // Calculate the real part
    mpfr_log (&aplRes.parts[0], &aplRMag, MPFR_RNDN);               // u

    // If the number has imaginary parts, ...
    if (!IsMpf0 (&aplIMag))
    {
        // Calculate arctan2 (aplIMag, aplRht.parts[0]) / aplIMag
        mpfr_atan2 (&aplMul, &aplIMag, &aplRht.parts[0], MPFR_RNDN); // v

        // Divide by the magnitude of the imaginary parts
        mpfr_div   (&aplMul, &aplMul, &aplIMag, MPFR_RNDN);

        // If the multiplier is 0, ...
        if (IsMpf0 (&aplMul))
        {
            // Loop through the imaginary parts
            for (i = 1; i < iHCDimRes; i++)
            {
                // If the right arg item is {Inf}, ...
                if (IsMpfInfinity (&aplRht.parts[i]))
                {
                    // Set the corresponding item in the result to 켗i/2
                    mpfr_init_set   (&aplRes.parts[i], &aplPi2HC8V.parts[0], MPFR_RNDN);

                    // If the signs differ, ...
                    if (SIGN_APLVFP_RAW (&aplRht.parts[i]) NE SIGN_APLVFP_RAW (&aplMul))
                        // Negate the value
                        mpfr_neg (&aplRes.parts[i], &aplRes.parts[i], MPFR_RNDN);
                } else
                    mpfr_init_set_d (&aplRes.parts[i],
                                      bAllowNeg0 ? ((SIGN_APLVFP_RAW (&aplRht.parts[i])
                                                  EQ SIGN_APLVFP_RAW (&aplMul)) ? 0.0
                                                                                : -0.0)
                                                 : 0.0,
                                      MPFR_RNDN);
            } // End FOR
        } else
////////// If the multiplier is {Inf}, ...
////////if (IsMpfInfinity (&aplMul))
////////{
////////    // Loop through the imaginary parts
////////    for (i = 1; i < iHCDimRes; i++)
////////        DbgBrk ();
////////
////////
////////
////////
////////} else
        // Loop through the imaginary parts
        for (i = 1; i < iHCDimRes; i++)
            // Multiply each of the imaginary parts by the arctan2
            aplRes.parts[i] = MulHC1V_RE (aplRht.parts[i], aplMul);
    } else
    {
        // If the real part is < 0, ...
        if (mpfr_cmp_ui (&aplRht.parts[0], 0) < 0)
            // Point the first imaginary part to +180 degrees
            mpfr_init_set (&aplRes.parts[1], &aplPiHC8V.parts[0], MPFR_RNDN);
        else
            // Point the first imaginary part to 0 degrees
            mpfr_init0    (&aplRes.parts[1]);

        // Loop through the rest of the imaginary parts
        for (i = 2; i < iHCDimRes; i++)
            // Initialize to 0
            mpfr_init0 (&aplRes.parts[i]);
    } // End IF/ELSE

    // We no longer need this storage
    Myf_clear (&aplMul);
    Myf_clear (&aplIMag);
    Myf_clear (&aplRMag);

    return aplRes;
} // End LogHCxV_RE


//***************************************************************************
//  $PrimFnMonCircleStarHC2VisHC2V
//
//  Primitive scalar function monadic CircleStar:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonCircleStarHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = LogHCxV_RE (lpatRht->aplHC8V, 2).partsLo.partsLo;
} // End PrimFnMonCircleStarHC2VisHC2V


//***************************************************************************
//  $PrimFnMonCircleStarHC4VisHC4R
//
//  Primitive scalar function monadic CircleStar:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonCircleStarHC4VisHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplRat, 0, &atRht);

    // Pass it on
    PrimFnMonCircleStarHC4VisHC4V (lpMemRes, uRes, &atRht, lpPrimSpec);

    // We no longer need this storage
    Myhc4v_clear (&atRht.aplHC4V);
} // End PrimFnMonCircleStarHC4VisHC4R


//***************************************************************************
//  $PrimFnMonCircleStarHC4VisHC4V
//
//  Primitive scalar function monadic CircleStar:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonCircleStarHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call the subfunction
    lpMemRes[uRes] = LogHCxV_RE (lpatRht->aplHC8V, 4).partsLo;
} // End PrimFnMonCircleStarHC4VisHC4V


//***************************************************************************
//  $PrimFnMonCircleStarHC8VisHC8R
//
//  Primitive scalar function monadic CircleStar:  HC8V {is} fn HC8R
//***************************************************************************

void PrimFnMonCircleStarHC8VisHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatRht->aplRat, 0, &atRht);

    // Pass it on
    PrimFnMonCircleStarHC8VisHC8V (lpMemRes, uRes, &atRht, lpPrimSpec);

    // We no longer need this storage
    Myhc8v_clear (&atRht.aplHC8V);
} // End PrimFnMonCircleStarHC8VisHC8R


//***************************************************************************
//  $PrimFnMonCircleStarHC8VisHC8V
//
//  Primitive scalar function monadic CircleStar:  HC8V {is} fn HC8V
//***************************************************************************

void PrimFnMonCircleStarHC8VisHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call the subfunction
    lpMemRes[uRes] = LogHCxV_RE (lpatRht->aplHC8V, 8);
} // End PrimFnMonCircleStarHC8VisHC8V


//***************************************************************************
//  $PrimSpecCircleStarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStarStorageTypeDyd
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

    // If both arguments are simple numeric,
    //   the result is FLOAT
    if (IsSimpleNum (*lpaplTypeLft)
     && IsSimpleNum (*lpaplTypeRht))
        aplTypeRes = ARRAY_FLOAT;
    else
        // Calculate the storage type of the result
        aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // Split cases based upon the storage type
    switch (aplTypeRes)
    {
        // Except all simple numerics become FLOAT
        case ARRAY_BOOL:
        case ARRAY_APA:
            aplTypeRes = ARRAY_FLOAT;

            break;

        // Except that INT -> FLT, RAT -> VFP, etc.
        case ARRAY_INT:
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            aplTypeRes++;               // Assuming order as in <ARRAY_TYPES>

            break;

        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
        case ARRAY_NESTED:
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
            aplTypeRes = ARRAY_ERROR;

            break;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the storage type
    switch (aplTypeRes)
    {
        // Except all simple numerics become FLOAT
        case ARRAY_BOOL:
        case ARRAY_APA:
            aplTypeRes = ARRAY_FLOAT;

            break;

        // Except that INT -> FLT, RAT -> VFP, etc.
        case ARRAY_INT:
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            aplTypeRes++;               // Assuming order as in <ARRAY_TYPES>

            break;

        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
        case ARRAY_NESTED:
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
            aplTypeRes = ARRAY_ERROR;

            break;

        defstop
            break;
    } // End SWITCH

    return aplTypeRes;
} // End PrimSpecCircleStarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydCircleStarFisIvI
//
//  Primitive scalar function dyadic CircleStar:  F {is} I fn I
//***************************************************************************

void PrimFnDydCircleStarFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  B {log} B
    if (IsBooleanValue (lpatLft->aplInteger)
     && IsBooleanValue (lpatRht->aplInteger))
    {
        lpMemRes[uRes] =
          TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                           icndxLog[lpatLft->aplInteger][lpatRht->aplInteger],
                                (APLFLOAT) lpatRht->aplInteger,
                                           FALSE);
    } else
    // Check for indeterminates:  0 {log} N (N != 0 or 1)
    if (lpatLft->aplInteger EQ 0)
    {
        lpMemRes[uRes] =
          TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                           ICNDX_0LOGN,
                                (APLFLOAT) lpatRht->aplInteger,
                                           FALSE);
    } else
    // Check for Complex result
    if (lpatLft->aplInteger < 0
     || lpatRht->aplInteger < 0)
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
    // The EAS says "If A and B are equal, lpMemRes[uRes] = one."
    if (lpatLft->aplInteger EQ lpatRht->aplInteger)
    {
        lpMemRes[uRes] = 1.0;

        return;
    } else
        // Calculate the log
        // Note that the native IEEE code correctly handles infinite results
        //   such as 5{log}0
        lpMemRes[uRes] =
          log ((APLFLOAT) lpatRht->aplInteger)
        / log ((APLFLOAT) lpatLft->aplInteger);
} // End PrimFnDydCircleStarFisIvI


//***************************************************************************
//  $PrimFnDydCircleStarFisFvF
//
//  Primitive scalar function dyadic CircleStar:  F {is} F fn F
//***************************************************************************

void PrimFnDydCircleStarFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for special cases:  0 {log} PoM_
    // Check for special cases:  PoM_ {log} 0
    // Check for special cases:  PoM_ {log} PoM_
    if ((lpatLft->aplFloat EQ 0 && IsFltInfinity (lpatRht->aplFloat))
     || (IsFltInfinity (lpatLft->aplFloat) && lpatRht->aplFloat EQ 0)
     || (IsFltInfinity (lpatLft->aplFloat) && IsFltInfinity (lpatRht->aplFloat)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Check for indeterminates:  B {log} B
    if (IsBooleanValue (lpatLft->aplFloat)
     && IsBooleanValue (lpatRht->aplFloat))
    {
        lpMemRes[uRes] =
          TranslateQuadICIndex (lpatLft->aplFloat,
                                icndxLog[(UINT) lpatLft->aplFloat][(UINT) lpatRht->aplFloat],
                                lpatRht->aplFloat,
                                FALSE);
    } else
    // Check for indeterminates:  0 {log} N  (N != 0 or 1)
    if (lpatLft->aplFloat EQ 0.0)
    {
        lpMemRes[uRes] =
          TranslateQuadICIndex (lpatLft->aplFloat,
                                ICNDX_0LOGN,
                                lpatRht->aplFloat,
                                FALSE);
    } else
    // Check for Complex result
    if (SIGN_APLFLOAT (lpatLft->aplFloat)
     || SIGN_APLFLOAT (lpatRht->aplFloat))
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
    // The EAS says "If A and B are equal, lpMemRes[uRes] = one."
    if (lpatLft->aplFloat EQ lpatRht->aplFloat)
        lpMemRes[uRes] = 1.0;
    else
        // Calculate the log
        // Note that the native IEEE code correctly handles infinite results
        //   such as 5{log}0  and  0.5{log}0
        lpMemRes[uRes] =
          log (lpatRht->aplFloat)
        / log (lpatLft->aplFloat);
} // End PrimFnDydCircleStarFisFvF


//***************************************************************************
//  $PrimFnDydCircleStarVisRvR
//
//  Primitive scalar function dyadic CircleStar:  V {is} R fn R
//***************************************************************************

void PrimFnDydCircleStarVisRvR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...     ight ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the left & right args to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatLft->aplRat, 0, &atLft);
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    // Pass it on
    PrimFnDydCircleStarVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    // We no longer need this storage
    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnMonCircleStarVisRvR


//***************************************************************************
//  $PrimFnDydCircleStarVisVvV
//
//  Primitive scalar function dyadic CircleStar:  V {is} V fn V
//***************************************************************************

void PrimFnDydCircleStarVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for special cases:  0 {log} PoM_
    // Check for special cases:  PoM_ {log} 0
    // Check for special cases:  PoM_ {log} PoM_
    if ((IsMpf0 (&lpatLft->aplVfp) && mpfr_inf_p (&lpatRht->aplVfp))
     || (mpfr_inf_p (&lpatLft->aplVfp) && IsMpf0 (&lpatRht->aplVfp))
     || (mpfr_inf_p (&lpatLft->aplVfp) && mpfr_inf_p (&lpatRht->aplVfp)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    else
    // Check for indeterminates:  B {log} B
    if (IsBooleanVfp (&lpatLft->aplVfp)
     && IsBooleanVfp (&lpatRht->aplVfp))
    {
        lpMemRes[uRes] =
          *mpfr_QuadICValue (&lpatLft->aplVfp,
                              icndxLog[IsMpf1 (&lpatLft->aplVfp)][IsMpf1(&lpatRht->aplVfp)],
                             &lpatRht->aplVfp,
                             &lpMemRes[uRes],
                              FALSE);
    } else
    // Check for indeterminates:  0 {log} N  (N != 0 or 1)
    if (mpfr_zero_p (&lpatLft->aplVfp))
    {
        lpMemRes[uRes] =
          *mpfr_QuadICValue (&lpatLft->aplVfp,
                              ICNDX_0LOGN,
                             &lpatRht->aplVfp,
                             &lpMemRes[uRes],
                              FALSE);
    } else
    // Check for Complex result
    if (SIGN_APLVFP (&lpatLft->aplVfp)
     || SIGN_APLVFP (&lpatRht->aplVfp))
        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    else
    {
        // Initialize the result to 0
        mpfr_init0 (&lpMemRes[uRes]);

        // The EAS says "If A and B are equal, lpMemRes[uRes] = one."
        if (mpfr_cmp (&lpatLft->aplVfp,  &lpatRht->aplVfp) EQ 0)
            mpfr_set_ui (&lpMemRes[uRes], 1, MPFR_RNDN);
        else
        {
            APLVFP mpfLft,
                   mpfRht;
#ifdef DEBUG
////        WCHAR  wszTemp[512];
#endif
            // Calculate log (lpatRht->aplVfp) / log (lpatLft->aplVfp)
            // Note that the MPFR code correctly handles infinite results
            //   such as 5{log}0  and  0.5{log}0
            PrimFnMonCircleStarVisV (&mpfLft, 0, lpatLft, lpPrimSpec);
#ifdef DEBUG
////        strcpyW (wszTemp, L"Lft: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfLft, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
            PrimFnMonCircleStarVisV (&mpfRht, 0, lpatRht, lpPrimSpec);
#ifdef DEBUG
////        strcpyW (wszTemp, L"Rht: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfRht, 0) = WC_EOS; DbgMsgW (wszTemp);
#endif
            mpfr_div (&lpMemRes[uRes], &mpfRht, &mpfLft, MPFR_RNDN);
#ifdef DEBUG
////        strcpyW (wszTemp, L"Res: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], lpMemRes[uRes], 0) = WC_EOS; DbgMsgW (wszTemp);
#endif

            // We no longer need this storage
            Myf_clear (&mpfRht);
            Myf_clear (&mpfLft);
        } // End IF/ELSE
    } // End IF/ELSE/...
} // End PrimFnDydCircleStarVisVvV


//***************************************************************************
//  $PrimFnDydCircleStarHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic CircleStar:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydCircleStarHC2FisHC2IvHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert HC2I to HC2F
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnDydCircleStarHC2FisHC2FvHC2F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydCircleStarHC2FisHC2IvHC2I


//***************************************************************************
//  $BaseLogHC2F_RE
//***************************************************************************

APLHC2F BaseLogHC2F_RE
    (APLHC2F aplLft,                // Left arg
     APLHC2F aplRht)                // Right ...

{
    // log R to the base L is (ln R) / ln L
    return DivHC2F_RE (LogHCxF_RE (*(LPAPLHC8F) &aplRht, 2).partsLo[0].partsLo[0],
                       LogHCxF_RE (*(LPAPLHC8F) &aplLft, 2).partsLo[0].partsLo[0]);
} // End BaseLogHC2F_RE


//***************************************************************************
//  $PrimFnDydCircleStarHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic CircleStar:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydCircleStarHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = BaseLogHC2F_RE (lpatLft->aplHC2F, lpatRht->aplHC2F);
} // End PrimFnDydCircleStarHC2FisHC2FvHC2F


//***************************************************************************
//  $PrimFnDydCircleStarHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic CircleStar:  HC2V {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydCircleStarHC2VisHC2RvHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert HC2R to HC2V
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnDydCircleStarHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    // We no longer need this storage
    Myhc2v_clear (&atRht.aplHC2V);
    Myhc2v_clear (&atLft.aplHC2V);
} // End PrimFnDydCircleStarHC2VisHC2RvHC2R


//***************************************************************************
//  $BaseLogHC2V_RE
//***************************************************************************

APLHC2V BaseLogHC2V_RE
    (APLHC2V aplLft,                // Left arg
     APLHC2V aplRht)                // Right ...

{
    APLHC2V aplRes,                 // The result
            aplLogLft,              // Log left
            aplLogRht;              // ... right

    // Calculate the respective logs
    aplLogLft = LogHCxV_RE (*(LPAPLHC8V) &aplLft, 2).partsLo.partsLo;
    aplLogRht = LogHCxV_RE (*(LPAPLHC8V) &aplRht, 2).partsLo.partsLo;

    // log R to the base L is (ln R) / ln L
    aplRes = DivHC2V_RE (aplLogRht, aplLogLft);

    // We no longer need this storage
    Myhc2v_clear (&aplLogRht);
    Myhc2v_clear (&aplLogLft);

    return aplRes;
} // End BaseLogHC2V_RE


//***************************************************************************
//  $PrimFnDydCircleStarHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic CircleStar:  HC2V {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydCircleStarHC2VisHC2VvHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = BaseLogHC2V_RE (lpatLft->aplHC2V, lpatRht->aplHC2V);
} // End PrimFnDydCircleStarHC2VisHC2VvHC2V


//***************************************************************************
//  $PrimFnDydCircleStarHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic CircleStar:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydCircleStarHC4FisHC4IvHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert HC4I to HC4F
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnDydCircleStarHC4FisHC4FvHC4F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydCircleStarHC4FisHC4IvHC4I


//***************************************************************************
//  $BaseLogHC4F_RE
//***************************************************************************

APLHC4F BaseLogHC4F_RE
    (APLHC4F aplLft,                // Left arg
     APLHC4F aplRht)                // Right ...

{
    // log R to the base L is (ln R) / ln L
    return DivHC4F_RE (LogHCxF_RE (*(LPAPLHC8F) &aplRht, 4).partsLo[0],
                       LogHCxF_RE (*(LPAPLHC8F) &aplLft, 4).partsLo[0]);
} // End BaseLogHC4F_RE


//***************************************************************************
//  $PrimFnDydCircleStarHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic CircleStar:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydCircleStarHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = BaseLogHC4F_RE (lpatLft->aplHC4F, lpatRht->aplHC4F);
} // End PrimFnDydCircleStarHC4FisHC4FvHC4F


//***************************************************************************
//  $PrimFnDydCircleStarHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic CircleStar:  HC4V {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydCircleStarHC4VisHC4RvHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert HC4R to HC4V
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnDydCircleStarHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    // We no longer need this storage
    Myhc4v_clear (&atRht.aplHC4V);
    Myhc4v_clear (&atLft.aplHC4V);
} // End PrimFnDydCircleStarHC4VisHC4RvHC4R


//***************************************************************************
//  $BaseLogHC4V_RE
//***************************************************************************

APLHC4V BaseLogHC4V_RE
    (APLHC4V aplLft,                // Left arg
     APLHC4V aplRht)                // Right ...

{
    APLHC4V aplRes,                 // The result
            aplLogLft,              // Log left
            aplLogRht;              // ... right

    // Calculate the respective logs
    aplLogLft = LogHCxV_RE (*(LPAPLHC8V) &aplLft, 4).partsLo;
    aplLogRht = LogHCxV_RE (*(LPAPLHC8V) &aplRht, 4).partsLo;

    // log R to the base L is (ln R) / ln L
    aplRes = DivHC4V_RE (aplLogRht, aplLogLft);

    // We no longer need this storage
    Myhc4v_clear (&aplLogRht);
    Myhc4v_clear (&aplLogLft);

    return aplRes;
} // End BaseLogHC4V_RE


//***************************************************************************
//  $PrimFnDydCircleStarHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic CircleStar:  HC4V {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydCircleStarHC4VisHC4VvHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = BaseLogHC4V_RE (lpatLft->aplHC4V, lpatRht->aplHC4V);
} // End PrimFnDydCircleStarHC4VisHC4VvHC4V


//***************************************************************************
//  $PrimFnDydCircleStarHC8FisHC8IvHC8I
//
//  Primitive scalar function dyadic CircleStar:  HC8F {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydCircleStarHC8FisHC8IvHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert HC8I to HC8F
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnDydCircleStarHC8FisHC8FvHC8F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydCircleStarHC8FisHC8IvHC8I


//***************************************************************************
//  $BaseLogHC8F_RE
//***************************************************************************

APLHC8F BaseLogHC8F_RE
    (APLHC8F aplLft,                // Left arg
     APLHC8F aplRht)                // Right ...

{
    // log R to the base L is (ln R) / ln L
    return DivHC8F_RE (LogHCxF_RE (*(LPAPLHC8F) &aplRht, 8),
                       LogHCxF_RE (*(LPAPLHC8F) &aplLft, 8));
} // End BaseLogHC8F_RE


//***************************************************************************
//  $PrimFnDydCircleStarHC8FisHC8FvHC8F
//
//  Primitive scalar function dyadic CircleStar:  HC8F {is} HC8F fn HC8F
//***************************************************************************

void PrimFnDydCircleStarHC8FisHC8FvHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = BaseLogHC8F_RE (lpatLft->aplHC8F, lpatRht->aplHC8F);
} // End PrimFnDydCircleStarHC8FisHC8FvHC8F


//***************************************************************************
//  $PrimFnDydCircleStarHC8VisHC8RvHC8R
//
//  Primitive scalar function dyadic CircleStar:  HC8V {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydCircleStarHC8VisHC8RvHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert HC8R to HC8V
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnDydCircleStarHC8VisHC8VvHC8V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    // We no longer need this storage
    Myhc8v_clear (&atRht.aplHC8V);
    Myhc8v_clear (&atLft.aplHC8V);
} // End PrimFnDydCircleStarHC8VisHC8RvHC8R


//***************************************************************************
//  $BaseLogHC8V_RE
//***************************************************************************

APLHC8V BaseLogHC8V_RE
    (APLHC8V aplLft,                // Left arg
     APLHC8V aplRht)                // Right ...

{
    APLHC8V aplRes,                 // The result
            aplLogLft,              // Log left
            aplLogRht;              // ... right

    // Calculate the respective logs
    aplLogLft = LogHCxV_RE (*(LPAPLHC8V) &aplLft, 8);
    aplLogRht = LogHCxV_RE (*(LPAPLHC8V) &aplRht, 8);

    // log R to the base L is (ln R) / ln L
    aplRes = DivHC8V_RE (aplLogRht, aplLogLft);

    // We no longer need this storage
    Myhc8v_clear (&aplLogRht);
    Myhc8v_clear (&aplLogLft);

    return aplRes;
} // End BaseLogHC8V_RE


//***************************************************************************
//  $PrimFnDydCircleStarHC8VisHC8VvHC8V
//
//  Primitive scalar function dyadic CircleStar:  HC8V {is} HC8V fn HC8V
//***************************************************************************

void PrimFnDydCircleStarHC8VisHC8VvHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = BaseLogHC8V_RE (lpatLft->aplHC8V, lpatRht->aplHC8V);
} // End PrimFnDydCircleStarHC8VisHC8VvHC8V


//***************************************************************************
//  End of File: pf_cstar.c
//***************************************************************************
