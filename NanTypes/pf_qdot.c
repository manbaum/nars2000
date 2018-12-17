//***************************************************************************
//  NARS2000 -- Primitive Function -- QuoteDot
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
#include <complex.h>

#define real(z)     (z)._Val[0]
#define imag(z)     (z)._Val[1]

#define __GSL_MATRIX_COMPLEX_LONG_DOUBLE_H__

#include "headers.h"
#include "po_quadjot.h"
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_eigen.h>


#ifndef PROTO
PRIMSPEC PrimSpecQuoteDot =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecQuoteDotStorageTypeMon,
    NULL,   // &PrimFnMonQuoteDotAPA_EM, -- Can't happen w/QuoteDot

    // Monadic Boolean result functions
    &PrimFnMonQuoteDotBisB,
    NULL,   // &PrimFnMonQuoteDotBisI, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotBisF, -- Can't happen w/QuoteDot

    // Monadic non-Boolean result functions (indexable)
    &PrimFnMonQuoteDotIisI,
    NULL,   // &PrimFnMonQuoteDotIisF, -- Can't happen w/QuoteDot
    &PrimFnMonQuoteDotFisI,
    &PrimFnMonQuoteDotFisF,
    &PrimFnMonQuoteDotRisR,
    &PrimFnMonQuoteDotVisR,
    &PrimFnMonQuoteDotVisV,

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonQuoteDotHC2IisHC2I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotHC2IisHC2F, -- Can't happen w/QuoteDot
    &PrimFnMonQuoteDotHC2FisHC2I,
    &PrimFnMonQuoteDotHC2FisHC2F,
    NULL,   // &PrimFnMonQuoteDotHC2RisHC2R, -- Can't happen w/QuoteDot
    &PrimFnMonQuoteDotHC2VisHC2R,
    &PrimFnMonQuoteDotHC2VisHC2V,

    NULL,   // &PrimFnMonQuoteDotHC4IisHC4I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotHC4IisHC4F, -- Can't happen w/QuoteDot
    &PrimFnMonQuoteDotHC4FisHC4I,
    &PrimFnMonQuoteDotHC4FisHC4F,
    NULL,   // &PrimFnMonQuoteDotHC4RisHC4R, -- Can't happen w/QuoteDot
    &PrimFnMonQuoteDotHC4VisHC4R,
    &PrimFnMonQuoteDotHC4VisHC4V,

    NULL,   // &PrimFnMonQuoteDotHC8IisHC8I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotHC8IisHC8F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotHC8FisHC8I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotHC8FisHC8F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotHC8RisHC8R, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotHC8VisHC8R, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotHC8VisHC8V, -- Can't happen w/QuoteDot

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonQuoteDotFisHC2I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotFisHC2F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotVisHC2R, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotVisHC2V, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotFisHC4I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotFisHC4F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotVisHC4R, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotVisHC4V, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotFisHC8I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotFisHC8F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotVisHC8R, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotVisHC8V, -- Can't happen w/QuoteDot

    // Monadic BAxF result BAxF arg functions (indexable)
    &PrimFnMonQuoteDotBA1FisBA1F,
    &PrimFnMonQuoteDotBA2FisBA2F,
    &PrimFnMonQuoteDotBA4FisBA4F,
    &PrimFnMonQuoteDotBA8FisBA8F,
    &PrimFnMonQuoteDotBA1FisBA1F,
    NULL,   // &PrimFnMonQuoteDotARBisBA2F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotARBisBA4F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotARBisBA8F, -- Can't happen w/QuoteDot

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecQuoteDotStorageTypeDyd,
    NULL,   // &PrimFnDydQuoteDotAPA_EM, -- Can't happen w/QuoteDot

    // Dyadic Boolean result functions
    &PrimFnDydQuoteDotBisBvB,
    NULL,   // &PrimFnDydQuoteDotBisIvI, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisFvF, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisCvC, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisHvH, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisRvR, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisVvV, -- Can't happen w/QuoteDot

    NULL,   // &PrimFnDydQuoteDotBisHC2IvHC2I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisHC2FvHC2F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisHC2RvHC2R, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisHC2VvHC2V, -- Can't happen w/QuoteDot

    NULL,   // &PrimFnDydQuoteDotBisHC4IvHC4I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisHC4FvHC4F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisHC4RvHC4R, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisHC4VvHC4V, -- Can't happen w/QuoteDot

    NULL,   // &PrimFnDydQuoteDotBisHC8IvHC8I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisHC8FvHC8F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisHC8RvHC8R, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisHC8VvHC8V, -- Can't happen w/QuoteDot

    // Dyadic Boolean result functions with ARBs (indexable)
    NULL,   // &PrimFnDydQuoteDotBisBA1FvBA1F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisBA2FvBA2F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisBA4FvBA4F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotBisBA8FvBA8F, -- Can't happen w/QuoteDot

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydQuoteDotIisIvI,
    NULL,   // &PrimFnDydQuoteDotIisFvF, -- Can't happen w/QuoteDot
    &PrimFnDydQuoteDotFisIvI,
    &PrimFnDydQuoteDotFisFvF,
    &PrimFnDydQuoteDotRisRvR,
    &PrimFnDydQuoteDotVisRvR,
    &PrimFnDydQuoteDotVisVvV,

    &PrimFnDydQuoteDotHC2IisHC2IvHC2I,
    NULL,   // &PrimFnDydQuoteDotHC2IisHC2FvHC2F, -- Can't happen w/QuoteDot
    &PrimFnDydQuoteDotHC2FisHC2IvHC2I,
    &PrimFnDydQuoteDotHC2FisHC2FvHC2F,
    &PrimFnDydQuoteDotHC2RisHC2RvHC2R,
    &PrimFnDydQuoteDotHC2VisHC2RvHC2R,
    &PrimFnDydQuoteDotHC2VisHC2VvHC2V,

    &PrimFnDydQuoteDotHC4IisHC4IvHC4I,
    NULL,   // &PrimFnDydQuoteDotHC4IisHC4FvHC4F, -- Can't happen w/QuoteDot
    &PrimFnDydQuoteDotHC4FisHC4IvHC4I,
    &PrimFnDydQuoteDotHC4FisHC4FvHC4F,
    &PrimFnDydQuoteDotHC4RisHC4RvHC4R,
    &PrimFnDydQuoteDotHC4VisHC4RvHC4R,
    &PrimFnDydQuoteDotHC4VisHC4VvHC4V,

    NULL,   // &PrimFnDydQuoteDotHC8IisHC8IvHC8I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotHC8IisHC8FvHC8F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotHC8FisHC8IvHC8I, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotHC8FisHC8FvHC8F, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotHC8RisHC8RvHC8R, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotHC8VisHC8RvHC8R, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnDydQuoteDotHC8VisHC8VvHC8V, -- Can't happen w/QuoteDot

    // Dyadic BAxF result functions (indexable)
    &PrimFnDydQuoteDotBA1FisBA1FvBA1F,
    &PrimFnDydQuoteDotBA2FisBA2FvBA2F,
    &PrimFnDydQuoteDotBA4FisBA4FvBA4F,
    NULL,   // &PrimFnDydQuoteDotBA8FisBA8FvBA8F, -- Can't happen w/QuoteDot

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonQuoteDotB64isB64, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotB32isB32, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotB16isB16, -- Can't happen w/QuoteDot
    NULL,   // &PrimFnMonQuoteDotB08isB08, -- Can't happen w/QuoteDot

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

static LPPRIMSPEC lpPrimSpec = {&PrimSpecQuoteDot};
#endif


//***************************************************************************
//  $PrimFnQuoteDot_EM_YY
//
//  Primitive function for monadic and dyadic QuoteDot ("factorial" and "binomial")
//***************************************************************************

LPPL_YYSTYPE PrimFnQuoteDot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_QUOTEDOT);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnQuoteDot_EM_YY


//***************************************************************************
//  $PrimSpecQuoteDotStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecQuoteDotStorageTypeMon
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

    // Call common promotion routine
    aplTypeRes = PrimSpecQuoteDotStorageTypeCom (aplTypeRes);

    return aplTypeRes;
} // End PrimSpecQuoteDotStorageTypeMon


//***************************************************************************
//  $PrimSpecQuoteDotStorageTypeCom
//
//  Common primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecQuoteDotStorageTypeCom
    (APLSTYPE aplTypeRes)

{
    // Split cases based upon the storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_RAT:
        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_ARB:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_NESTED:
            break;

        // Except that APAs are converted to INTs
        case ARRAY_APA:
            aplTypeRes = ARRAY_INT;

            break;

        // Except that HCINT -> HCFLT, HCRAT -> HCVFP, etc.
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
            aplTypeRes++;               // Assuming order as in <ARRAY_TYPES>

            break;

        case ARRAY_HC8I:
        case ARRAY_HC8F:
        case ARRAY_HC8R:
        case ARRAY_HC8V:
        case ARRAY_BA8F:
        case ARRAY_CHAR:
        case ARRAY_HETERO:
            aplTypeRes = ARRAY_ERROR;

            break;

        defstop
            break;
    } // End SWITCH

    return aplTypeRes;
} // End PrimSpecQuoteDotStorageTypeCom


//***************************************************************************
//  $PrimFnMonQuoteDotBisB
//
//  Primitive scalar function monadic QuoteDot:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonQuoteDotBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (APLBOOL) 1;
} // End PrimFnMonQuoteDotBisB


//***************************************************************************
//  $PrimFnMonQuoteDotIisI
//
//  Primitive scalar function monadic QuoteDot:  I {is} fn I
//***************************************************************************

void PrimFnMonQuoteDotIisI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLFLOAT      aplFloatRes;
    static APLINT factorial[] =
    {
                    1,      //  0
                    1,      //  1
                    2,      //  2
                    6,      //  3
                   24,      //  4
                  120,      //  5
                  720,      //  6
                 5040,      //  7
                40320,      //  8
               362880,      //  9
              3628800,      // 10
             39916800,      // 11
            479001600,      // 12
           6227020800,      // 13
          87178291200,      // 14
        1307674368000,      // 15
       20922789888000,      // 16
      355687428096000,      // 17
     6402373705728000,      // 18
   121645100408832000,      // 19
  2432902008176640000,      // 20
    };

    // Check for indeterminates:  !N for integer N < 0
    if (lpatRht->aplInteger < 0)
    {
        // Get the result as float
        aplFloatRes =
          TranslateQuadICIndex (0,
                                ICNDX_QDOTn,
                     (APLFLOAT) lpatRht->aplInteger,
                                FALSE);
        // If it's infinite, ...
        if (IsFltInfinity (aplFloatRes))
            RaiseException (EXCEPTION_RESULT_FLT, 0, 0, NULL);
        else
            lpMemRes[uRes] = (APLINT) aplFloatRes;
    } else
    {
        // Check for results too large to express as integers
        if (lpatRht->aplInteger >= countof (factorial))
            RaiseException (EXCEPTION_RESULT_FLT, 0, 0, NULL);
        else
            // Lookup the result
            lpMemRes[uRes] = factorial[lpatRht->aplInteger];
    } // End IF/ELSE
} // End PrimFnMonQuoteDotIisI


//***************************************************************************
//  $PrimFnMonQuoteDotFisI
//
//  Primitive scalar function monadic QuoteDot:  F {is} fn I
//***************************************************************************

void PrimFnMonQuoteDotFisI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpatRht->aplFloat = (APLFLOAT) lpatRht->aplInteger;

    PrimFnMonQuoteDotFisF (lpMemRes, uRes, lpatRht, lpPrimSpec);
} // End PrimFnMonQuoteDotFisI


//***************************************************************************
//  $PrimFnMonQuoteDotFisF
//
//  Primitive scalar function monadic QuoteDot:  F {is} fn F
//***************************************************************************

void PrimFnMonQuoteDotFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactFLT (lpatRht->aplFloat);
} // End PrimFnMonQuoteDotFisF


//***************************************************************************
//  $FactFLT
//
//  Primitive scalar function monadic QuoteDot:  F {is} fn F
//***************************************************************************

APLFLOAT FactFLT
    (APLFLOAT aplFloatRht)

{
    int iRet;

    // Check for indeterminates:  !N for integer N < 0
    if (aplFloatRht < 0
     && (aplFloatRht EQ floor (aplFloatRht)
      || aplFloatRht EQ ceil  (aplFloatRht)))
        return TranslateQuadICIndex (0,
                                     ICNDX_QDOTn,
                                     aplFloatRht,
                                     FALSE);
    else
    {
        // Check for PosInfinity
        if (IsFltPosInfinity (aplFloatRht))
            return fltPosInfinity;
        else
        // Check for too large for GSL
        if ((aplFloatRht + 1) > GSL_SF_GAMMA_XMAX)
            RaiseException (EXCEPTION_RESULT_RAT, 0, 0, NULL);
        else
        {
            gsl_sf_result gsr = {0};

            // Use the GNU Scientific Library Gamma function
            iRet = gsl_sf_gamma_e (aplFloatRht + 1, &gsr);

            // Check the lpMemRes[uRes] = code
            switch (iRet)
            {
                case GSL_SUCCESS:
                    break;

                case GSL_FAILURE:
                case GSL_EDOM:
                case GSL_ERANGE:
                    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

                    break;

                defstop
                    break;
            } // End SWITCH

            return gsr.val;
        } // End IF/ELSE
    } // End IF/ELSE

    return -1.0;
} // End FactFLT


//***************************************************************************
//  $PrimFnMonQuoteDotRisR
//
//  Primitive scalar function monadic QuoteDot:  R {is} fn R
//***************************************************************************

void PrimFnMonQuoteDotRisR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UINT uRht;

    // Check for indeterminates:  !N for integer N < 0
    if (mpq_integer_p (&lpatRht->aplRat)
     && mpq_cmp_ui (&lpatRht->aplRat, 0, 1) < 0)
        mpq_QuadICValue (&lpatRht->aplRat,          // No left arg
                          ICNDX_QDOTn,
                         &lpatRht->aplRat,
                         &lpMemRes[uRes],
                          FALSE);
    else
    // Check for PosInfinity
    if (IsMpqPosInfinity (&lpatRht->aplRat))
        lpMemRes[uRes] = mpqPosInfinity;
    else
    // If the denominator is 1,
    //   and the numerator fts in a UINT, ...
    if (mpq_integer_p (&lpatRht->aplRat)
     && mpz_fits_slong_p (mpq_numref (&lpatRht->aplRat)) NE 0)
    {
        // Initialize the result to 0/1
        mpq_init (&lpMemRes[uRes]);

        // Extract the numerator
        uRht = (UINT) mpz_get_si (mpq_numref (&lpatRht->aplRat));

        // Compute the factorial
        mpz_fac_ui (mpq_numref (&lpMemRes[uRes]), uRht);
    } else
        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);
} // End PrimFnMonQuoteDotRisR


//***************************************************************************
//  $FactVFP
//
//  Primitive scalar function monadic QuoteDot:  V {is} fn V
//***************************************************************************

APLVFP FactVFP
    (APLVFP aplVfpRht)

{
    APLMPI mpzRes = {0};
    APLVFP mpfRes = {0};

    // Check for indeterminates:  !N for integer N < 0
    if (mpfr_integer_p (&aplVfpRht)
     && mpfr_cmp_ui (&aplVfpRht, 0) < 0)
        mpfr_QuadICValue (&aplVfpRht,           // No left arg
                           ICNDX_QDOTn,
                          &aplVfpRht,
                          &mpfRes,
                           FALSE);
    else
    // Check for PosInfinity
    if (IsMpfPosInfinity (&aplVfpRht))
        return mpfPosInfinity;
    else
    // If the arg is an integer,
    //   and it fits in a ULONG, ...
    if (mpfr_integer_p (&aplVfpRht)
     && mpfr_fits_uint_p (&aplVfpRht, MPFR_RNDN))
    {
        // Initialize to 0
        mpz_init   (&mpzRes);
        mpfr_init0 (&mpfRes);

        mpz_fac_ui (&mpzRes, mpfr_get_ui (&aplVfpRht, MPFR_RNDN));
        mpfr_set_z (&mpfRes, &mpzRes, MPFR_RNDN);

        Myz_clear (&mpzRes);
    } else
    {
        // Initialize the result
        mpfr_init_set (&mpfRes, &aplVfpRht, MPFR_RNDN);
        mpfr_add_ui   (&mpfRes, &mpfRes, 1, MPFR_RNDN);

        // Let MPFR handle it
        mpfr_gamma (&mpfRes, &mpfRes, MPFR_RNDN);
#ifdef DEBUG
        mpfr_free_cache ();
#endif
    } // End IF/ELSE

    return mpfRes;
} // End FactVFP


//***************************************************************************
//  $PrimFnMonQuoteDotVisR
//
//  Primitive scalar function monadic QuoteDot:  V {is} fn R
//***************************************************************************

void PrimFnMonQuoteDotVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    lpMemRes[uRes] = FactVFP (atRht.aplVfp);

    Myhc1v_clear (&atRht.aplHC1V);
} // End PrimFnMonQuoteDotVisR


//***************************************************************************
//  $PrimFnMonQuoteDotVisV
//
//  Primitive scalar function monadic QuoteDot:  V {is} fn V
//***************************************************************************

void PrimFnMonQuoteDotVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactVFP (lpatRht->aplVfp);
} // End PrimFnMonQuoteDotVisV


//***************************************************************************
//  $FactARB
//
//  Primitive scalar function monadic QuoteDot:  ARB {is} fn ARB
//***************************************************************************

APLARB FactARB
    (APLARB aplArbRht)

{
    APLARB           arbRes = {0};
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  !N for integer N < 0
    if (arb_integer_p (&aplArbRht)
     && arb_cmp_ui    (&aplArbRht, 0) < 0)
        arb_QuadICValue (&aplArbRht,        // No left arg
                          ICNDX_QDOTn,
                         &aplArbRht,
                         &arbRes,
                          FALSE);
    else
    // Check for PosInfinity
    if (IsArbPosInfinity (&aplArbRht))
        return arbPosInfinity;
    else
    // If the arg is an integer,
    //   and it fits in a ULONG, ...
    if (arb_integer_p (&aplArbRht)
     && arb_fits_uint_p (&aplArbRht))
    {
        fmpz_t fmpzRes;

        // Initialize to 0
        fmpz_init    (fmpzRes);
        Myarb_init     (&arbRes);

        fmpz_fac_ui  (fmpzRes, arf_get_si (arb_midref (&aplArbRht), ARF_RND_NEAR));
        arb_set_fmpz (&arbRes, fmpzRes);

        fmpz_clear   (fmpzRes);
    } else
    {
        // Initialize the result
        arb_init_set (&arbRes, &aplArbRht);
        arb_add_ui   (&arbRes, &arbRes, 1, prec);

#ifdef DEBUG
        if (guTest > 0)
            // Let us handle it
            arb_gamma2 (&arbRes, &arbRes, prec);
        else
#endif
            // Let ARB handle it
            arb_gamma  (&arbRes, &arbRes, prec);
    } // End IF/ELSE

    return arbRes;
} // End FactARB


//***************************************************************************
//  $PrimFnMonQuoteDotBA1FisBA1F
//
//  Primitive scalar function monadic QuoteDot:  BA1F {is} fn BA1F
//***************************************************************************

void PrimFnMonQuoteDotBA1FisBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactARB(lpatRht->aplArb);
} // End PrimFnMonQuoteDotBA1FisBA1F


//***************************************************************************
//  $FactHC2F
//
//  Primitive scalar function monadic QuoteDot:  HC2F {is} fn HC2F
//***************************************************************************

APLHC2F FactHC2F
    (APLHC2F aplRht)

{
    APLHC2F aplRes = {0};           // The result
    APLINT  aplInteger;             // Right arg as an integer
    UBOOL   bRet;                   // TRUE iff exact integer conversion
    int     iRet;                   // Return value from GSL function

    // Attempt to convert the right arg to an integer
    aplInteger = ConvertToInteger_SCT (ARRAY_HC2F, &aplRht, 0, &bRet);

    // Check for indeterminates:  !N for integer N < 0
    if (bRet
     && aplInteger < 0)
        aplRes.parts[0] =
          TranslateQuadICIndex (0,
                                ICNDX_QDOTn,
                     (APLFLOAT) aplInteger,
                                FALSE);
    else
    {
////    // Check for PosInfinity
////    if (IsFltPosInfinity (aplFloatRht))
////        return fltPosInfinity;
////    else
////    // Check for too large for GSL
////    if ((aplFloatRht + 1) > GSL_SF_GAMMA_XMAX)
////        RaiseException (EXCEPTION_RESULT_RAT, 0, 0, NULL);
////    else
        {
            gsl_sf_result gsrLnr = {0},
                          gsrArg = {0};

            // Use the GNU Scientific Library Complex Gamma function
            iRet = gsl_sf_lngamma_complex_e (aplRht.parts[0] + 1,
                                             aplRht.parts[1],
                                            &gsrLnr,
                                            &gsrArg);
            // Check the lpMemRes[uRes] = code
            switch (iRet)
            {
                case GSL_SUCCESS:
                    break;

                case GSL_FAILURE:
                case GSL_EDOM:
                case GSL_ERANGE:
                    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

                    break;

                defstop
                    break;
            } // End SWITCH

            // Save in the result
            aplRes.parts[0] = gsrLnr.val;
            aplRes.parts[1] = gsrArg.val;

            // Undo the ln of lngamma
            aplRes = ExpHC2F_RE (aplRes);
#if FALSE
            _Dcomplex arg, res;
            arg = *(_Dcomplex *) &aplRht;
            real(arg)++;

            // Use external routine in <cgamma.c>
            res = cgamma (arg, 0);

            aplRes = *(LPAPLHC2F) &res;
#endif
        } // End IF/ELSE
    } // End IF/ELSE

    return aplRes;
} // End FactHC2F


//***************************************************************************
//  $PrimFnMonQuoteDotHC2FisHC2I
//
//  Primitive scalar function monadic QuoteDot:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonQuoteDotHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonQuoteDotHC2FisHC2F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonQuoteDotHC2FisHC2I


//***************************************************************************
//  $PrimFnMonQuoteDotHC2FisHC2F
//
//  Primitive scalar function monadic QuoteDot:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonQuoteDotHC2FisHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactHC2F (lpatRht->aplHC2F);
} // End PrimFnMonQuoteDotHC2FisHC2F


//***************************************************************************
//  $FactHCxF
//
//  Primitive scalar function monadic QuoteDot:  HCxF {is} fn HCxF
//    using matrix form.  This routine may be called with N = 1, 2, 4
//    but is not needed for anything but Quaternions as the other cases
//    are handled by special purpose code elsewhere.
//***************************************************************************

APLHC8F FactHCxF
    (APLHC8F aplRht,                // The input
     int     N)                     // The dimension (1, 2, 4, 8)

{
    APLHC8F                       aplRes = {0};                 // The result
    gsl_matrix                   *lpGslMatrixA = NULL;          // GSL Temp
    gsl_vector_complex           *lpGslCVectorEval = NULL;      // Eigenvalues
    gsl_matrix_complex           *lpGslCMatrixEvec = NULL;      // Eigenvectors
    gsl_eigen_nonsymmv_workspace *lpGslEigenWs = NULL;          // Ptr to the GSL workspace
    APLUINT                       ByteRes;                      // # bytes in the result
    EXCEPTION_CODES               exCode = EXCEPTION_SUCCESS;   // Exception code in case we're to signal an exception
    int                           ErrCode,                      // Error code
                                  i,                            // Loop counter
                                  j,                            // ...
                                  k;                            // ...

    // Allocate GSL arrays
    lpGslMatrixA     = gsl_matrix_alloc  (N, N);        // N x N
    lpGslCVectorEval = gsl_vector_complex_alloc (N);    // N
    lpGslCMatrixEvec = gsl_matrix_complex_alloc (N, N); // N x N
    lpGslEigenWs     = gsl_eigen_nonsymmv_alloc (N);

    // Check the return codes for the above allocations
    if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixA
     || GSL_ENOMEM EQ (HANDLE_PTR) lpGslCVectorEval
     || GSL_ENOMEM EQ (HANDLE_PTR) lpGslCMatrixEvec
     || GSL_ENOMEM EQ (HANDLE_PTR) lpGslEigenWs)
        goto WSFULL_EXIT;

    // Calculate the byte size of the vector data
    ByteRes = N * sizeof (aplRht.parts[0]);

////// Check for overflow (can't happen with N = 1, 2, 4, or 8
////if (ByteRes NE (APLU3264) ByteRes)
////    goto WSFULL_EXIT;
////
    // Split cases based upon the dimension
    switch (N)
    {
        case 1:
            // Represent the Real number as a 1x1 matrix
            lpGslMatrixA->data[0] = aplRht.parts[0];

            break;

        case 2:
            // Represent the Complex number as a 2x2 matrix
            //   using the standard Complex number matrix form
            //   as A = (a, -b,
            //           b,  a)

#define lpMemRht    (aplRht.parts)
#define lpMemRes    (lpGslMatrixA->data)
            MATREP_FLT (2, 2)
#undef  lpMemRes
#undef  lpMemRht

            break;

        case 4:
            // Represent the Quaternion number as a 4x4 matrix
            //   using one of the 48 Quaternion number matrix forms
            //   as A = (a, -b, -c, -d,
            //           b,  a, -d,  c,
            //           c,  d,  a, -b,
            //           d, -c,  b,  a)
#define lpMemRht    (aplRht.parts)
#define lpMemRes    (lpGslMatrixA->data)
            MATREP_FLT (4, 4)
#undef  lpMemRes
#undef  lpMemRht

            break;

        defstop
            break;
    } // End SWITCH

    // Set params
////gsl_eigen_nonsymmv_params (0);  // 0 is the default

    // Calculate the Eigenvalues and Eigenvectors
    ErrCode =
      gsl_eigen_nonsymmv (lpGslMatrixA,
                          lpGslCVectorEval,
                          lpGslCMatrixEvec,
                          lpGslEigenWs);
    // Check the error code
    if (ErrCode NE GSL_SUCCESS)
        goto DOMAIN_EXIT;

    // The result is the N-dimensional number  <9{circle}(Evec+.×(Diag !Eval)+.×{domino}Evec)[;1]

    // Finish off the result with a Magic Function
    {
        HGLOBAL           hGlbMFO;              // Magic function/operator ...
        LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
        LPPL_YYSTYPE      lpYYRes;              // The result of the Magic Function
        TOKEN             tkLftArg = {0},       // Left arg token to the Magic Function
                          tkRhtArg = {0},       // Right ...
                          tkFunc   = {0};       // Function token
        HGLOBAL           hGlbTmp;              // Temp global memory handle
        LPVARARRAY_HEADER lpMemHdrTmp = NULL;   // Ptr to the header
        LPAPLHC2F         lpMemTmp;             // Ptr to the global memory data
        APLDIM            NxN[2] = {N, N};      // The array dimensions
        int               iSizeofRes;           // Sizeof an item in the result

        // Setup the left arg token as Evec

        // Allocate space for Evec as an NxN Complex Floating Point matrix
        hGlbTmp = AllocateGlobalArray (ARRAY_HC2F, N * N, 2, &NxN[0]);
        if (hGlbTmp EQ NULL)
            goto DOMAIN_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrTmp = MyGlobalLockVar (hGlbTmp);

        // Skip over the header and dimensions to the data
        lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

        // Copy Evec to the left arg global memory data
        CopyMemory (lpMemTmp, lpGslCMatrixEvec->data, N * N * sizeof (lpMemTmp[0]));

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

        // Fill in the left arg token
        tkLftArg.tkFlags.TknType   = TKT_VARARRAY;
////////tkLftArg.tkFlags.ImmType   =
////////tkLftArg.tkFlags.NoDisplay = FALSE; // Already zero from = {0}
        tkLftArg.tkData.tkGlbData  = MakePtrTypeGlb (hGlbTmp);
////////tkLftArg.tkCharIndex       = tkFunc.tkCharIndex;

        // Setup the right arg token as Eval

        // Allocate space for Eval as a length N Complex Floating Point vector
        hGlbTmp = AllocateGlobalArray (ARRAY_HC2F, N, 1, &NxN[0]);
        if (hGlbTmp EQ NULL)
            goto DOMAIN_EXIT;

        // Lock the memory to get a ptr to it
        lpMemHdrTmp = MyGlobalLockVar (hGlbTmp);

        // Skip over the header and dimensions to the data
        lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

        // Copy Eval to the right arg global memory data
        CopyMemory (lpMemTmp, lpGslCVectorEval->data, N * sizeof (lpMemTmp[0]));

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

        // Fill in the right arg token
        tkRhtArg.tkFlags.TknType   = TKT_VARARRAY;
////////tkRhtArg.tkFlags.ImmType   =
////////tkRhtArg.tkFlags.NoDisplay = FALSE; // Already zero from = {0}
        tkRhtArg.tkData.tkGlbData  = MakePtrTypeGlb (hGlbTmp);
////////tkRhtArg.tkCharIndex       = tkFunc.tkCharIndex;

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get the magic function/operator global memory handle
        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MonShriek];

        //  Use an internal magic function/operator.
        lpYYRes =
          ExecuteMagicFunction_EM_YY (&tkLftArg,        // Ptr to left arg token
                                      &tkFunc,          // Ptr to function token
                                       NULL,            // Ptr to function strand
                                      &tkRhtArg,        // Ptr to right arg token
                                       NULL,            // Ptr to axis token
                                       hGlbMFO,         // Magic function/operator global memory handle
                                       NULL,            // Ptr to HSHTAB struc (may be NULL)
                                       LINENUM_ONE);    // Starting line # type (see LINE_NUMS)
        // We no longer need this storage
        FreeResultTkn (&tkRhtArg);
        FreeResultTkn (&tkLftArg);

        // Check for error
        if (lpYYRes EQ NULL)
            goto ERRMSG_ALREADY_SET_EXIT;

        // Get the global memory handle
        hGlbTmp = lpYYRes->tkToken.tkData.tkGlbData;

        // Lock the memory to get a ptr to it
        lpMemHdrTmp = MyGlobalLockVar (hGlbTmp);

        Assert (lpMemHdrTmp->NELM EQ 1);
        Assert (lpMemHdrTmp->Rank EQ 0);

        // Skip over the header and dimensions to the data
        lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

        // Calculate the size of an element in lpMemHdrTmp
        iSizeofRes = TranslateArrayTypeToSizeof (lpMemHdrTmp->ArrType);

        // The result is the singleton value in lpMemTmp
        CopyMemory (&aplRes, lpMemTmp, iSizeofRes);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;

        // We no longer need this storage
        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
    } // End Magic Function

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    exCode = EXCEPTION_DOMAIN_ERROR;

    goto ERROR_EXIT;

WSFULL_EXIT:
    exCode = EXCEPTION_WS_FULL;

    goto ERROR_EXIT;

ERRMSG_ALREADY_SET_EXIT:
    exCode = EXCEPTION_ERRMSG_ALREADY_SET;

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpGslEigenWs NE NULL)
    {
        gsl_eigen_nonsymmv_free (lpGslEigenWs); lpGslEigenWs = NULL;
    } // End IF

    if (lpGslCMatrixEvec NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_complex_free (lpGslCMatrixEvec); lpGslCMatrixEvec = NULL;
    } // End IF

    if (lpGslCVectorEval NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_complex_free (lpGslCVectorEval); lpGslCVectorEval = NULL;
    } // End IF

    if (lpGslMatrixA NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixA); lpGslMatrixA = NULL;
    } // End IF

    // If we're exiting on an error, ...
    if (exCode NE EXCEPTION_SUCCESS)
        // Signal the error to the caller
        RaiseException (exCode, 0, 0, NULL);

    return aplRes;
} // End FactHCxF


//***************************************************************************
//  $FactHC2V
//
//  Primitive scalar function monadic QuoteDot:  HC2V {is} fn HC2V
//***************************************************************************

APLHC2V FactHC2V
    (APLHC2V aplRht)

{
    APLHC2V aplRes = {0};           // The result

    NONCE_RE                // ***FINISHME*** -- !HC2V

    return aplRes;
} // End FactHC2V


//***************************************************************************
//  $PrimFnMonQuoteDotHC2VisHC2R
//
//  Primitive scalar function monadic QuoteDot:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonQuoteDotHC2VisHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonQuoteDotHC2VisHC2V (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonQuoteDotHC2VisHC2R


//***************************************************************************
//  $PrimFnMonQuoteDotHC2VisHC2V
//
//  Primitive scalar function monadic QuoteDot:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonQuoteDotHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactHC2V (lpatRht->aplHC2V);
} // End PrimFnMonQuoteDotHC2VisHC2V


//***************************************************************************
//  $FactBA2F
//
//  Primitive scalar function monadic QuoteDot:  BA2F {is} fn BA2F
//***************************************************************************

APLBA2F FactBA2F
    (APLBA2F aplRht)

{
    APLBA2F aplRes = {0};           // The result

    NONCE_RE                // ***FINISHME*** -- !BA2F

    return aplRes;
} // End FactBA2F


//***************************************************************************
//  $PrimFnMonQuoteDotBA2FisBA2F
//
//  Primitive scalar function monadic QuoteDot:  BA2F {is} fn BA2F
//***************************************************************************

void PrimFnMonQuoteDotBA2FisBA2F
    (LPAPLBA2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactBA2F (lpatRht->aplBA2F);
} // End PrimFnMonQuoteDotBA2FisBA2F


//***************************************************************************
//  $FactHC4F
//
//  Primitive scalar function monadic QuoteDot:  HC4F {is} fn HC4F
//***************************************************************************

APLHC4F FactHC4F
    (APLHC4F aplRht)

{
    return FactHCxF (*(LPAPLHC8F) &aplRht, 4).partsLo[0];
} // End FactHC4F


//***************************************************************************
//  $PrimFnMonQuoteDotHC4FisHC4I
//
//  Primitive scalar function monadic QuoteDot:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonQuoteDotHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonQuoteDotHC4FisHC4F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonQuoteDotHC4FisHC4I


//***************************************************************************
//  $PrimFnMonQuoteDotHC4FisHC4F
//
//  Primitive scalar function monadic QuoteDot:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonQuoteDotHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactHC4F (lpatRht->aplHC4F);
} // End PrimFnMonQuoteDotHC4FisHC4F


//***************************************************************************
//  $FactHC4V
//
//  Primitive scalar function monadic QuoteDot:  HC4V {is} fn HC4V
//***************************************************************************

APLHC4V FactHC4V
    (APLHC4V aplRht)

{
    APLHC4V aplRes = {0};           // The result

    NONCE_RE                // ***FINISHME*** -- !HC4V

    return aplRes;
} // End FactHC4V


//***************************************************************************
//  $PrimFnMonQuoteDotHC4VisHC4R
//
//  Primitive scalar function monadic QuoteDot:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonQuoteDotHC4VisHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonQuoteDotHC4VisHC4V (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonQuoteDotHC4VisHC4R


//***************************************************************************
//  $PrimFnMonQuoteDotHC4VisHC4V
//
//  Primitive scalar function monadic QuoteDot:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonQuoteDotHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactHC4V (lpatRht->aplHC4V);
} // End PrimFnMonQuoteDotHC4VisHC4V


//***************************************************************************
//  $FactBA4F
//
//  Primitive scalar function monadic QuoteDot:  BA4F {is} fn BA4F
//***************************************************************************

APLBA4F FactBA4F
    (APLBA4F aplRht)

{
    APLBA4F aplRes = {0};           // The result

    NONCE_RE                // ***FINISHME*** -- !BA4F

    return aplRes;
} // End FactBA4F


//***************************************************************************
//  $PrimFnMonQuoteDotBA4FisBA4F
//
//  Primitive scalar function monadic QuoteDot:  BA4F {is} fn BA4F
//***************************************************************************

void PrimFnMonQuoteDotBA4FisBA4F
    (LPAPLBA4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactBA4F (lpatRht->aplBA4F);
} // End PrimFnMonQuoteDotBA4FisBA4F


//***************************************************************************
//  $FactBA8F
//
//  Primitive scalar function monadic QuoteDot:  BA8F {is} fn BA8F
//***************************************************************************

APLBA8F FactBA8F
    (APLBA8F aplRht)

{
    APLBA8F aplRes = {0};           // The result

    NONCE_RE                // ***FINISHME*** -- !BA8F

    return aplRes;
} // End FactBA8F


//***************************************************************************
//  $PrimFnMonQuoteDotBA8FisBA8F
//
//  Primitive scalar function monadic QuoteDot:  BA8F {is} fn BA8F
//***************************************************************************

void PrimFnMonQuoteDotBA8FisBA8F
    (LPAPLBA8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactBA8F (lpatRht->aplBA8F);
} // End PrimFnMonQuoteDotBA8FisBA8F


//***************************************************************************
//  Magic function/operator used to finish off Monadic Shriek
//***************************************************************************

#include "mf_shriek.h"


//***************************************************************************
//  $PrimSpecQuoteDotStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecQuoteDotStorageTypeDyd
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

    // Call common promotion routine
    aplTypeRes = PrimSpecQuoteDotStorageTypeCom (aplTypeRes);

    return aplTypeRes;
} // End PrimSpecQuoteDotStorageTypeDyd


//***************************************************************************
//  $PrimFnDydQuoteDotBisBvB
//
//  Primitive scalar function dyadic QuoteDot:  B {is} B fn B
//***************************************************************************

void PrimFnDydQuoteDotBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean <= lpatRht->aplBoolean)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydQuoteDotBisBvB


//***************************************************************************
//  $PrimFnDydQuoteDotIisIvI
//
//  Primitive scalar function dyadic QuoteDot:  I {is} I fn I
//***************************************************************************

void PrimFnDydQuoteDotIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLINT   aplIntegerTmp,
             aplIntegerRes,
             aplIntegerInd;
    ALLTYPES atLft = *lpatLft,
             atRht = *lpatRht;

    // Because this algorithm changes L and R, we need to copy those
    //   initial values to temps to avoid changing the caller's values.

#define L   atLft.aplInteger
#define R   atRht.aplInteger
#define T   aplIntegerTmp
#define Z   aplIntegerRes
#define I   aplIntegerInd

    // Determine whether or not L, R, or L - R is a negative integer
    // From ISO-IEC 13751 Extended APL Standard, p. 90
    switch (4*(L < 0) + 2*(R < 0) + 1*(R < L))
    {
        //    L     R   R-L
        case 4*0 + 2*0 + 1*0:   // (!R)/((!L)*!R-L)
            // WLOG, change L so that L <= (R/2)
            //   using the reflection rule for
            //   binomial coefficients.
            if (L > (R / 2))
                L = R - L;

            // Compute the result as:
            //      T = R-L;
            //      Z = Prod (I, 1, L, (I+T) / I)
            for (T = R - L, Z = 1, I = 1; I <= L; I++)
                Z = (imul64_RE (Z, I + T, FLOAT)) / I;
            lpMemRes[uRes] = Z;

            break;

        case 4*0 + 2*0 + 1*1:   // 0
            lpMemRes[uRes] = 0;

            break;

        case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            break;

        case 4*0 + 2*1 + 1*1:   // ((-1)*L)*L!(L-(R+1))
            // Recurse with L replaced by L
            // Recurse with R replaced by L-(R+1)
////////////L = L;
            R = L-(R+1);

            PrimFnDydQuoteDotIisIvI (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

            // Handle sign
            lpMemRes[uRes] = ((L%2) ? -1: 1) * lpMemRes[uRes];

            break;

        case 4*1 + 2*0 + 1*0:   // 0
            lpMemRes[uRes] = 0;

            break;

        case 4*1 + 2*0 + 1*1:   // (Can't happen)
            lpMemRes[uRes] = -1;

            break;

        case 4*1 + 2*1 + 1*0:   // ((-1)*R-L)*(-R+1)!|L+1
            // Recurse with L replaced by -(R+1)
            // Recurse with R replaced by -(L+1)
            T = L;
            L = -(R+1);
            R = -(T+1);

            PrimFnDydQuoteDotIisIvI (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

            // Handle sign
            lpMemRes[uRes] = (((R-L)%2) ? -1: 1) * lpMemRes[uRes];

            break;

        case 4*1 + 2*1 + 1*1:   // 0
            lpMemRes[uRes] = 0;

            break;

        defstop
            lpMemRes[uRes] = -1;

            break;
    } // End SWITCH

#undef  I
#undef  Z
#undef  T
#undef  R
#undef  L
} // End PrimFnDydQuoteDotIisIvI


//***************************************************************************
//  $PrimFnDydQuoteDotFisIvI
//
//  Primitive scalar function dyadic QuoteDot:  F {is} I fn I
//***************************************************************************

void PrimFnDydQuoteDotFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLFLOAT      aplFloatTmp,      // Temp vars
                  aplFloatRes,      // ...
                  aplFloatInd;      // ...
    APLINT        aplIntegerTmp,    // ...
                  aplIntegerRes,    // ...
                  aplIntegerInd,    // ...
                  aplIntegerTmp2;   // ...
    ALLTYPES      atLft = *lpatLft, // ...
                  atRht = *lpatRht; // ...
    UBOOL         bRet = TRUE;      // Assume the result is valid
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // FYI:  The only way this function can be called is
    //   from an exception raised from IisIvI above.

    // Because this algorithm changes LI, RI, LF, and RF we need to copy those
    //   initial values to temps to avoid changing the caller's values.

#define LI  atLft.aplInteger
#define RI  atRht.aplInteger
#define TI  aplIntegerTmp
#define TI2 aplIntegerTmp2
#define ZI  aplIntegerRes
#define II  aplIntegerInd

#define LF  atLft.aplFloat
#define RF  atRht.aplFloat
#define TF  aplFloatTmp
#define ZF  aplFloatRes
#define IF  aplFloatInd

    // Determine whether or not LI, RI, or RI - LI is a negative integer
    // From ISO-IEC 13751 Extended APL Standard, p. 90
    switch (4*(LI < 0) + 2*(RI < 0) + 1*(RI < LI))
    {
        //    LI   RI  RI-LI
        case 4*0 + 2*0 + 1*0:   // (!RI)/((!LI)*!RI-LI)
            // WLOG, change LI so that LI <= (RI/2)
            //   using the reflection rule for
            //   binomial coefficients.
            if (LI > (RI / 2))
                LI = RI - LI;

            // Compute the result as:
            //      TI = RI-LI;
            //      ZI = Prod (II, 1, LI, (II+TI) / II)
            for (TI = RI - LI, ZI = 1, II = 1;
                 II <= LI;
                 II++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    break;

                TI2 = imul64 (ZI, II + TI, &bRet, EXCEPTION_RESULT_FLT);
                if (bRet)
                    ZI = TI2 / II;
                else
                {
                    for (ZF = (APLFLOAT) ZI, TF = (APLFLOAT) TI, IF = (APLFLOAT) II;
                         II <= LI;
                         II++, IF++)
                    {
                        ZF = (ZF * (IF + TF)) / IF;
                        if (_isinf (ZF))
                            break;
                    } // End FOR

                    lpMemRes[uRes] = ZF;

                    break;
                } // End IF/ELSE
            } // End FOR

            lpMemRes[uRes] = (APLFLOAT) ZI;

            break;

        //    LI   RI  RI-LI
        case 4*0 + 2*0 + 1*1:   // 0
            lpMemRes[uRes] = 0;

            break;

        //    LI   RI  RI-LI
        case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        //    LI   RI  RI-LI
        case 4*0 + 2*1 + 1*1:   // ((-1)*LI)*LI!(LI-(RI+1))
            // Recurse with LI replaced by LI
            // Recurse with RI replaced by LI-(RI+1)
////////////LI = LI;
            RI = LI-(RI+1);

            PrimFnDydQuoteDotFisIvI (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

            // Handle sign
            lpMemRes[uRes] = ((LI%2) ? -1: 1) * lpMemRes[uRes];

            break;

        //    LI   RI  RI-LI
        case 4*1 + 2*0 + 1*0:   // 0
            lpMemRes[uRes] = 0;

            break;

        //    LI   RI  RI-LI
        case 4*1 + 2*0 + 1*1:   // (Can't happen)
            lpMemRes[uRes] = -1;

            break;

        //    LI   RI  RI-LI
        case 4*1 + 2*1 + 1*0:   // ((-1)*RI-LI)*(-RI+1)!|LI+1
            // Recurse with L replaced by -(RI+1)
            // Recurse with R replaced by -(LI+1)
            TI = LI;
            LI = -(RI+1);
            RI = -(TI+1);

            PrimFnDydQuoteDotFisIvI (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

            // Handle sign
            lpMemRes[uRes] = (((RI-LI)%2) ? -1: 1) * lpMemRes[uRes];

            break;

        //    LI   RI  RI-LI
        case 4*1 + 2*1 + 1*1:   // 0
            lpMemRes[uRes] = 0;

            break;

        defstop
            lpMemRes[uRes] = -1;

            break;
    } // End SWITCH

#undef  IF
#undef  ZF
#undef  TF
#undef  RF
#undef  LF

#undef  II
#undef  ZI
#undef  TI2
#undef  TI
#undef  RI
#undef  LI
} // End PrimFnDydQuoteDotFisIvI


//***************************************************************************
//  $PrimFnDydQuoteDotFisFvF
//
//  Primitive scalar function dyadic QuoteDot:  F {is} F fn F
//***************************************************************************

void PrimFnDydQuoteDotFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLFLOAT      aplFloatTmp,      // Temp vars
                  aplFloatRes,      // ...
                  aplFloatInd;      // ...
    ALLTYPES      atLft = *lpatLft, // ...
                  atRht = *lpatRht; // ...
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Because this algorithm changes LF and RF, we need to copy those
    //   initial values to temps to avoid changing the caller's values.

#define LF  atLft.aplFloat
#define RF  atRht.aplFloat
#define TF  aplFloatTmp
#define ZF  aplFloatRes
#define IF  aplFloatInd

    // If LF and RF are exact integers
    if ((LF EQ floor (LF)
      || LF EQ ceil  (LF))
     && (RF EQ floor (RF)
      || RF EQ ceil  (RF)))
    {
        // Determine whether or not LF, RF, or RF - LF is a negative integer
        // From ISO-IEC 13751 Extended APL Standard, p. 90
        switch (4*(LF < 0) + 2*(RF < 0) + 1*(RF < LF))
        {
            //    LF   RF  RF-LF
            case 4*0 + 2*0 + 1*0:   // (!RF)/((!LF)*!RF-LF)
                // WLOG, change LF so that LF <= (RF/2)
                //   using the reflection rule for
                //   binomial coefficients.
                if (LF > (RF / 2))
                    LF = RF - LF;

                // Compute the result as:
                //      TF = RF-LF;
                //      ZF = Prod (IF, 1, LF, (IF+TF) / IF)
                for (TF = RF - LF, ZF = 1, IF = 1;
                     IF <= LF;
                     IF++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (lpbCtrlBreak))
                        break;

                    ZF = (ZF * (IF + TF)) / IF;

                    if (_isinf (ZF))
                        break;
                } // End DOR

                lpMemRes[uRes] = ZF;

                break;

            //    LF   RF  RF-LF
            case 4*0 + 2*0 + 1*1:   // 0
                lpMemRes[uRes] = 0;

                break;

            //    LF   RF  RF-LF
            case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

                break;

            //    LF   RF  RF-LF
            case 4*0 + 2*1 + 1*1:   // ((-1)*LF)*LF!(LF-(RF+1))
                // Recurse with L replaced by LF
                // Recurse with R replaced by LF-(RF+1)
////////////////LF = LF;
                RF = LF-(RF+1);

                PrimFnDydQuoteDotFisFvF (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

                // Handle sign
                lpMemRes[uRes] = (fmod (LF, 2) ? -1: 1) * lpMemRes[uRes];

                break;

            //    LF   RF  RF-LF
            case 4*1 + 2*0 + 1*0:   // 0
                lpMemRes[uRes] = 0;

                break;

            //    LF   RF  RF-LF
            case 4*1 + 2*0 + 1*1:   // (Can't happen)
                lpMemRes[uRes] = -1;

                break;

            //    LF   RF  RF-LF
            case 4*1 + 2*1 + 1*0:   // ((-1)*RF-LF)*(-RF+1)!|LF+1
                // Recurse with LF replaced by -(RF+1)
                // Recurse with RF replaced by -(LF+1)
                TF = LF;
                LF = -(RF+1);
                RF = -(TF+1);

                PrimFnDydQuoteDotFisFvF (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

                // Handle sign
                lpMemRes[uRes] = (fmod (RF-LF, 2) ? -1: 1) * lpMemRes[uRes];

                break;

            //    LF   RF  RF-LF
            case 4*1 + 2*1 + 1*1:   // 0
                lpMemRes[uRes] = 0;

                break;

            defstop
                lpMemRes[uRes] = -1;

                break;
        } // End SWITCH
    } else
    {
        // Z = (!R) / (!L) * !R-L
        lpMemRes[uRes] =
                FactFLT (RF)
             / (FactFLT (LF)
              * FactFLT (RF - LF));
    } // End IF/ELSE

#undef  IF
#undef  ZF
#undef  TF
#undef  RF
#undef  LF
} // End PrimFnDydQuoteDotFisFvF


//***************************************************************************
//  $PrimFnDydQuoteDotRisRvR
//
//  Primitive scalar function dyadic QuoteDot:  R {is} R fn R
//***************************************************************************

void PrimFnDydQuoteDotRisRvR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLMPI   aplTmpDif = {0};
    APLRAT   aplTmp1 = {0},
             aplTmp2 = {0};
    LPAPLRAT lpaplLft;
    mpir_si  iLft;
    // Calculate the LR, RR, and RR-LR signs
    int LS = mpz_sgn (mpq_numref (&lpatLft->aplRat)),
        RS = mpz_sgn (mpq_numref (&lpatRht->aplRat)),
        TS;

    // Initialize the result to 0/1
    mpq_init (&lpMemRes[uRes]);

    // Copy the ptr
    lpaplLft = &lpMemRes[uRes];

    // Using the identity A!B <=> (B-A)!B and because the function
    //   mpz_bin_ui requires that the left argument must fit in a
    //   UINT, we calculate the alternate left argument in case it
    //   fits in a UINT.
    mpq_sub (lpaplLft, &lpatRht->aplRat, &lpatLft->aplRat);

    // Use the smaller of the two for the left arg
    if (mpq_cmpabs (lpaplLft, &lpatLft->aplRat) > 0)
        mpq_set    (lpaplLft, &lpatLft->aplRat);

    // Initialize to 0
    mpz_init (&aplTmpDif);

    // Calculate RR-LR
    mpz_sub (&aplTmpDif, mpq_numref (&lpatRht->aplRat), mpq_numref (lpaplLft));

    TS = mpz_sgn (&aplTmpDif);

    // If both denominators are 1,
    //   and the left numerator fits in a signed long, ...
    if (mpq_integer_p (lpaplLft        )
     && mpq_integer_p (&lpatRht->aplRat)
     && mpz_fits_slong_p (mpq_numref (lpaplLft)) NE 0)
    {
        // Extract the numerator of the left argument
        iLft = mpz_get_si (mpq_numref (lpaplLft));

        // Determine whether or not LR, RR, or RR - LR is a negative integer
        // From ISO-IEC 13751 Extended APL Standard, p. 90
        switch (4*(LS < 0) + 2*(RS < 0) + 1*(TS < 0))
        {
            //    LR   RR  RR-LR
            case 4*0 + 2*0 + 1*0:   // (!RR)/((!LR)*!RR-LR)
                // Compute the binomial coefficient:  Z = R L
                mpz_bin_ui (mpq_numref (&lpMemRes[uRes]), mpq_numref (&lpatRht->aplRat), iLft);

                break;

            //    LR   RR  RR-LR
            case 4*0 + 2*0 + 1*1:   // 0
                mpq_set_si (&lpMemRes[uRes], 0, 1);

                break;

            //    LR   RR  RR-LR
            case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
                Myz_clear (&aplTmpDif     );
                Myq_clear (&lpMemRes[uRes]);

                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            //    LR   RR  RR-LR
            case 4*0 + 2*1 + 1*1:   // ((-1)*LR)*LR!(LR-(RR+1))
                // Recurse with LR replaced by LR
                // Recurse with RR replaced by LR-(RR+1)
////////////////LR = LR;
////////////////RR = LR-(RR+1)

                // Calculate    (RR+1)
                mpz_add_ui (&aplTmpDif, mpq_numref (&lpatRht->aplRat), 1);

                // Calculate LR-(RR+1)
                mpz_sub (&aplTmpDif, mpq_numref (lpaplLft), &aplTmpDif);

                // Compute the binomial coefficient:  Z = R L
                mpz_bin_ui (mpq_numref (&lpMemRes[uRes]), &aplTmpDif, iLft);

                // Handle sign
                if ((iLft % 2) EQ 1)
                    mpq_neg (&lpMemRes[uRes], &lpMemRes[uRes]);

                break;

            //    LR   RR  RR-LR
            case 4*1 + 2*0 + 1*0:   // 0
                mpq_set_si (&lpMemRes[uRes], 0, 1);

                break;

            //    LR   RR  RR-LR
            case 4*1 + 2*0 + 1*1:   // (Can't happen)
                mpq_set_si (&lpMemRes[uRes], -1, 1);

                break;

            //    LR   RR  RR-LR
            case 4*1 + 2*1 + 1*0:   // ((-1)*RR-LR)*(-RR+1)!|LR+1
                // Recurse with LR replaced by -(RR+1)
                // Recurse with RR replaced by -(LR+1)
                //   TR = LR;
                //   LR = -(RR+1);
                //   RR = -(TR+1);

                // Initialize to 0/1
                mpq_init (&aplTmp1);
                mpq_init (&aplTmp2);

                mpq_set (&aplTmp1, lpaplLft);                   // TR = LR
                mpq_add (&aplTmp2, &lpatRht->aplRat, &mpqOne);  //  (RR+1)
                mpq_neg (&aplTmp2, &aplTmp2);                   // -(RR+1)
                mpq_add (&aplTmp1, &aplTmp1, &mpqOne);          //  (TR+1)
                mpq_neg (&aplTmp1, &aplTmp1);                   // -(TR+1)

                PrimFnDydQuoteDotRisRvR (lpMemRes, uRes, (LPALLTYPES) &aplTmp2, (LPALLTYPES) &aplTmp1, lpPrimSpec);

                // We no longer need these resources
                Myq_clear (&aplTmp2);
                Myq_clear (&aplTmp1);

                // Handle sign
                if ((iLft % 2) EQ 1)
                    mpq_neg (&lpMemRes[uRes], &lpMemRes[uRes]);

                break;

            //    LR   RR  RR-LR
            case 4*1 + 2*1 + 1*1:   // 0
                mpq_set_si (&lpMemRes[uRes], 0, 1);

                break;

            defstop
                mpq_set_si (&lpMemRes[uRes], -1, 1);

                break;
        } // End SWITCH
    } else
    {
        Myq_clear (&lpMemRes[uRes]);
        Myz_clear (&aplTmpDif     );

        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);
    } // End IF/ELSE

    Myz_clear (&aplTmpDif);
} // End PrimFnDydQuoteDotRisRvR


//***************************************************************************
//  $PrimFnDydQuoteDotVisRvR
//
//  Primitive scalar function dyadic QuoteDot:  V {is} R fn R
//***************************************************************************

void PrimFnDydQuoteDotVisRvR
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

    PrimFnDydQuoteDotVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydQuoteDotVisRvR


//***************************************************************************
//  $PrimFnDydQuoteDotVisVvV
//
//  Primitive scalar function dyadic QuoteDot:  V {is} V fn V
//***************************************************************************

void PrimFnDydQuoteDotVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLVFP   aplLft,
             aplRht,
             aplTmp = {0},
             aplTmp1 = {0},
             aplTmp2 = {0},
             aplTmpDif = {0};
    LPAPLVFP lpaplLft;
    long     iLft;
    // Calculate the LV, RV, and RV-LV signs
    int LS = mpfr_sgn (&lpatLft->aplVfp),
        RS = mpfr_sgn (&lpatRht->aplVfp),
        TS;

    // Initialize the result to 0
    mpfr_init0 (&lpMemRes[uRes]);

    // Copy the ptr
    lpaplLft = &lpMemRes[uRes];

    // Using the identity A!B <=> (B-A)!B and because the function
    //   mpz_bin_ui requires that the left argument must fit in a
    //   UINT, we calculate the alternate left argument in case it
    //   fits in a UINT.
    mpfr_sub (lpaplLft, &lpatRht->aplVfp, &lpatLft->aplVfp, MPFR_RNDN);

    // Use the smaller of the two for the left arg
    if (mpfr_cmpabs (lpaplLft, &lpatLft->aplVfp) > 0)
        mpfr_set    (lpaplLft, &lpatLft->aplVfp, MPFR_RNDN);

    // Initialize to 0
    mpfr_init0 (&aplTmpDif);

    // Calculate RV-LV
    mpfr_sub (&aplTmpDif, &lpatRht->aplVfp, lpaplLft, MPFR_RNDN);

    TS = mpfr_sgn (&aplTmpDif);

    // If both denominators are 1,
    //   and the left numerator fits in a signed long, ...
    if (mpfr_integer_p (lpaplLft        )
     && mpfr_integer_p (&lpatRht->aplVfp)
     && mpfr_fits_slong_p (lpaplLft, MPFR_RNDN) NE 0)
    {
        // Extract the numerator of the left argument
        iLft = mpfr_get_si (lpaplLft, MPFR_RNDN);

        // Determine whether or not LV, RV, or RV - LV is a negative integer
        // From ISO-IEC 13751 Extended APL Standard, p. 90
        switch (4*(LS < 0) + 2*(RS < 0) + 1*(TS < 0))
        {
            //    LV   RV  RV-LV
            case 4*0 + 2*0 + 1*0:   // (!R)/((!L)*!R-L)
                // Compute the binomial coefficient
                mpfr_bin_ui (&lpMemRes[uRes], &lpatRht->aplVfp, iLft);

                break;

            //    LV   RV  RV-LV
            case 4*0 + 2*0 + 1*1:   // 0
                mpfr_set_si (&lpMemRes[uRes], 0, MPFR_RNDN);

                break;

            //    LV   RV  RV-LV
            case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
                Myf_clear (&aplTmpDif     );
                Myf_clear (&lpMemRes[uRes]);

                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            //    LV   RV  RV-LV
            case 4*0 + 2*1 + 1*1:   // ((-1)*L)*L!(L-(R+1))
                // Recurse with LR replaced by LR
                // Recurse with RR replaced by LR-(RR+1)
////////////////LR = LR;
////////////////RR = LR-(RR+1)

                // Calculate    (RV+1)
                mpfr_add_ui (&aplTmpDif, &lpatRht->aplVfp, 1, MPFR_RNDN);

                // Calculate LV-(RV+1)
                mpfr_sub (&aplTmpDif, lpaplLft, &aplTmpDif, MPFR_RNDN);

                // Compute the binomial coefficient
                mpfr_bin_ui (&lpMemRes[uRes], &aplTmpDif, iLft);

                // Handle sign
                if ((iLft % 2) EQ 1)
                    mpfr_neg (&lpMemRes[uRes], &lpMemRes[uRes], MPFR_RNDN);

                break;

            //    LV   RV  RV-LV
            case 4*1 + 2*0 + 1*0:   // 0
                mpfr_set_si (&lpMemRes[uRes], 0, MPFR_RNDN);

                break;

            //    LV   RV  RV-LV
            case 4*1 + 2*0 + 1*1:   // (Can't happen)
                mpfr_set_si (&lpMemRes[uRes], -1, MPFR_RNDN);

                break;

            //    LV   RV  RV-LV
            case 4*1 + 2*1 + 1*0:   // ((-1)*RV-LV)*(-RV+1)!|LV+1
                // Recurse with LV replaced by -(RV+1)
                // Recurse with RV replaced by -(LV+1)
                //   TV = LV;
                //   LV = -(RV+1);
                //   RV = -(TV+1);

                // Initialize to 0
                mpfr_init0 (&aplTmp1);
                mpfr_init0 (&aplTmp2);

                mpfr_set    (&aplTmp1, lpaplLft, MPFR_RNDN);                // TV = LV
                mpfr_add_ui (&aplTmp2, &lpatRht->aplVfp, 1, MPFR_RNDN);     //  (RV+1)
                mpfr_neg    (&aplTmp2, &aplTmp2, MPFR_RNDN);                // -(RV+1)
                mpfr_add_ui (&aplTmp1, &aplTmp1, 1, MPFR_RNDN);             //  (TV+1)
                mpfr_neg    (&aplTmp1, &aplTmp1, MPFR_RNDN);                // -(TV+1)

                PrimFnDydQuoteDotVisVvV (lpMemRes, uRes, (LPALLTYPES) &aplTmp2, (LPALLTYPES) &aplTmp1, lpPrimSpec);

                // We no longer need these resources
                Myf_clear (&aplTmp2);
                Myf_clear (&aplTmp1);

                // Handle sign
                if ((iLft % 2) EQ 1)
                    mpfr_neg (&lpMemRes[uRes], &lpMemRes[uRes], MPFR_RNDN);

                break;

            //    LV   RV  RV-LV
            case 4*1 + 2*1 + 1*1:   // 0
                mpfr_set_si (&lpMemRes[uRes], 0, MPFR_RNDN);

                break;

            defstop
                mpfr_set_si (&lpMemRes[uRes], -1, MPFR_RNDN);

                break;
        } // End SWITCH
    } else
    {
        // Z = (!R) / (!L) * !R-L
        aplLft = FactVFP (lpatLft->aplVfp);                                 // !L
        aplRht = FactVFP (lpatRht->aplVfp);                                 // !R

        mpfr_init_copy (&aplTmp, &lpatRht->aplVfp);                         // R
        mpfr_sub       (&aplTmp, &aplTmp, &lpatLft->aplVfp, MPFR_RNDN);     // R-L
        lpMemRes[uRes] = FactVFP (aplTmp);                                  // !R-L

        mpfr_mul (&lpMemRes[uRes], &aplLft, &lpMemRes[uRes], MPFR_RNDN);    // (!L) * !R-L
        mpfr_div (&lpMemRes[uRes], &aplRht, &lpMemRes[uRes], MPFR_RNDN);    // (!R) / ((!L) * !R-L)

        // We no longer need this storage
        Myf_clear (&aplTmp);
        Myf_clear (&aplRht);
        Myf_clear (&aplLft);
    } // End IF/ELSE

    // We no longer need this storage
    Myf_clear (&aplTmpDif);
} // End PrimFnDydQuoteDotVisVvV


//***************************************************************************
//  $PrimFnDydQuoteDotBA1FisBA1FvBA1F
//
//  Primitive scalar function dyadic QuoteDot:  BA1F {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydQuoteDotBA1FisBA1FvBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLARB   aplLft,
             aplRht,
             aplTmp = {0},
             aplTmp1 = {0},
             aplTmp2 = {0},
             aplTmpDif = {0};
    LPAPLARB lpaplLft;
    long             iLft;
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Calculate the LV, RV, and RV-LV signs
    int LS = arb_sign (&lpatLft->aplArb),
        RS = arb_sign (&lpatRht->aplArb),
        TS;

    // Initialize the result to 0/0
    Myarb_init (&lpMemRes[uRes]);

    // Copy the ptr
    lpaplLft = &lpMemRes[uRes];

    // Using the identity A!B <=> (B-A)!B and because the function
    //   mpz_bin_ui requires that the left argument must fit in a
    //   UINT, we calculate the alternate left argument in case it
    //   fits in a UINT.
    arb_sub (lpaplLft, &lpatRht->aplArb, &lpatLft->aplArb, prec);

    // Use the smaller of the two for the left arg
    if (arf_cmpabs (arb_midref (lpaplLft), arb_midref (&lpatLft->aplArb)) > 0)
        arb_set    (lpaplLft, &lpatLft->aplArb);

    // Initialize to 0/0
    Myarb_init (&aplTmpDif);

    // Calculate RV-LV
    arb_sub (&aplTmpDif, &lpatRht->aplArb, lpaplLft, prec);

    TS = arb_sign (&aplTmpDif);

    // If both denominators are 1,
    //   and the left numerator fits in a signed long, ...
    if (arb_integer_p (lpaplLft        )
     && arb_integer_p (&lpatRht->aplArb)
     && arb_fits_slong_p (lpaplLft) NE 0)
    {
        // Extract the numerator of the left argument
        iLft = (long) arf_get_si (arb_midref (lpaplLft), ARF_RND_NEAR);

        // Determine whether or not LV, RV, or RV - LV is a negative integer
        // From ISO-IEC 13751 Extended APL Standard, p. 90
        switch (4*(LS < 0) + 2*(RS < 0) + 1*(TS < 0))
        {
            //    LV   RV  RV-LV
            case 4*0 + 2*0 + 1*0:   // (!R)/((!L)*!R-L)
                // Compute the binomial coefficient
                arb_bin_ui (&lpMemRes[uRes], &lpatRht->aplArb, iLft, prec);

                break;

            //    LV   RV  RV-LV
            case 4*0 + 2*0 + 1*1:   // 0
                arb_set_si (&lpMemRes[uRes], 0);

                break;

            //    LV   RV  RV-LV
            case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
                Myarb_clear (&aplTmpDif     );
                Myarb_clear (&lpMemRes[uRes]);

                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            //    LV   RV  RV-LV
            case 4*0 + 2*1 + 1*1:   // ((-1)*L)*L!(L-(R+1))
                // Recurse with LR replaced by LR
                // Recurse with RR replaced by LR-(RR+1)
////////////////LR = LR;
////////////////RR = LR-(RR+1)

                // Calculate    (RV+1)
                arb_add_ui (&aplTmpDif, &lpatRht->aplArb, 1, prec);

                // Calculate LV-(RV+1)
                arb_sub (&aplTmpDif, lpaplLft, &aplTmpDif, prec);

                // Compute the binomial coefficient
                arb_bin_ui (&lpMemRes[uRes], &aplTmpDif, iLft, prec);

                // Handle sign
                if ((iLft % 2) EQ 1)
                    arb_neg (&lpMemRes[uRes], &lpMemRes[uRes]);

                break;

            //    LV   RV  RV-LV
            case 4*1 + 2*0 + 1*0:   // 0
                arb_set_si (&lpMemRes[uRes], 0);

                break;

            //    LV   RV  RV-LV
            case 4*1 + 2*0 + 1*1:   // (Can't happen)
                arb_set_si (&lpMemRes[uRes], -1);

                break;

            //    LV   RV  RV-LV
            case 4*1 + 2*1 + 1*0:   // ((-1)*RV-LV)*(-RV+1)!|LV+1
                // Recurse with LV replaced by -(RV+1)
                // Recurse with RV replaced by -(LV+1)
                //   TV = LV;
                //   LV = -(RV+1);
                //   RV = -(TV+1);

                // Initialize to 0/0
                Myarb_init (&aplTmp1);
                Myarb_init (&aplTmp2);

                arb_set    (&aplTmp1, lpaplLft);                           // TV = LV
                arb_add_ui (&aplTmp2, &lpatRht->aplArb, 1, prec);  //  (RV+1)
                arb_neg    (&aplTmp2, &aplTmp2);                           // -(RV+1)
                arb_add_ui (&aplTmp1, &aplTmp1, 1, prec);          //  (TV+1)
                arb_neg    (&aplTmp1, &aplTmp1);                           // -(TV+1)

                PrimFnDydQuoteDotBA1FisBA1FvBA1F (lpMemRes, uRes, (LPALLTYPES) &aplTmp2, (LPALLTYPES) &aplTmp1, lpPrimSpec);

                // We no longer need these resources
                Myarb_clear (&aplTmp2);
                Myarb_clear (&aplTmp1);

                // Handle sign
                if ((iLft % 2) EQ 1)
                    arb_neg (&lpMemRes[uRes], &lpMemRes[uRes]);

                break;

            //    LV   RV  RV-LV
            case 4*1 + 2*1 + 1*1:   // 0
                arb_set_si (&lpMemRes[uRes], 0);

                break;

            defstop
                arb_set_si (&lpMemRes[uRes], -1);

                break;
        } // End SWITCH
    } else
    {
        // Z = (!R) / (!L) * !R-L
        aplLft = FactARB (lpatLft->aplArb);                                 // !L
        aplRht = FactARB (lpatRht->aplArb);                                 // !R

        arb_init_set   (&aplTmp, &lpatRht->aplArb);                         // R
        arb_sub        (&aplTmp, &aplTmp, &lpatLft->aplArb, prec);          // R-L
        lpMemRes[uRes] = FactARB (aplTmp);                                  // !R-L

        arb_mul (&lpMemRes[uRes], &aplLft, &lpMemRes[uRes], prec);          // (!L) * !R-L
        arb_div (&lpMemRes[uRes], &aplRht, &lpMemRes[uRes], prec);          // (!R) / ((!L) * !R-L)

        // We no longer need this storage
        Myarb_clear (&aplTmp);
        Myarb_clear (&aplRht);
        Myarb_clear (&aplLft);
    } // End IF/ELSE

    // We no longer need this storage
    Myarb_clear (&aplTmpDif);
} // End PrimFnDydQuoteDotBA1FisBA1FvBA1F


//***************************************************************************
//  $PrimFnDydQuoteDotHC2IisHC2IvHC2I
//
//  Primitive scalar function dyadic QuoteDot:  HC2I {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydQuoteDotHC2IisHC2IvHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
} // End PrimFnDydQuoteDotHC2IisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydQuoteDotHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic QuoteDot:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydQuoteDotHC2FisHC2IvHC2I
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
    PrimFnDydQuoteDotHC2FisHC2FvHC2F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydQuoteDotHC2FisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydQuoteDotHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic QuoteDot:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydQuoteDotHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLHC2F aplLft,
            aplRht,
            aplTmp1 = {0},
            aplTmp2 = {0};
    APLINT  aplInteger;             // Right arg as an integer
    UBOOL   bRet;                   // TRUE iff exact integer conversion

    // Attempt to convert the left arg to an integer
    aplInteger = ConvertToInteger_SCT (ARRAY_HC2F, &lpatLft->aplHC2F, 0, &bRet);

    // If the left arg is an integer, ...
    if (bRet)
    {
        // If L == 0, ...
        if (aplInteger EQ 0)
        {
            // Res = 1
            lpMemRes[uRes] = fpcfOne;

            return;
        } else
        // If L == 1, ...
        if (aplInteger EQ 1)
        {
            // Res = R
            lpMemRes[uRes] = lpatRht->aplHC2F;

            return;
        } // End IF
    } // End IF

    // Z = (!R) / (!L) * !R-L
    aplLft = FactHC2F (lpatLft->aplHC2F);                               // !L
    aplRht = FactHC2F (lpatRht->aplHC2F);                               // !R

    aplTmp1 = SubHC2F_RE (lpatRht->aplHC2F, lpatLft->aplHC2F);          // R-L
    aplTmp2 = FactHC2F (aplTmp1);                                       // !R-L

    aplTmp1 = MulHC2F_RE (aplLft, aplTmp2);                             // (!L) * !R-L
    lpMemRes[uRes] = DivHC2F_RE (aplRht, aplTmp1);                      // (!R) / ((!L) * !R-L)
} // End PrimFnDydQuoteDotHC2FisHC2FvHC2F


//***************************************************************************
//  $PrimFnDydQuoteDotHC2RisHC2RvHC2R
//
//  Primitive scalar function dyadic QuoteDot:  HC2R {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydQuoteDotHC2RisHC2RvHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
} // End PrimFnDydQuoteDotHC2RisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydQuoteDotHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic QuoteDot:  HC2V {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydQuoteDotHC2VisHC2RvHC2R
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
    PrimFnDydQuoteDotHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydQuoteDotHC2VisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydQuoteDotHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic QuoteDot:  HC2V {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydQuoteDotHC2VisHC2VvHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLHC2V aplLft,
            aplRht,
            aplTmp1 = {0},
            aplTmp2 = {0};
    APLINT  aplInteger;             // Right arg as an integer
    UBOOL   bRet;                   // TRUE iff exact integer conversion

    // Attempt to convert the left arg to an integer
    aplInteger = ConvertToInteger_SCT (ARRAY_HC2V, &lpatLft->aplHC2F, 0, &bRet);

    // If the left arg is an integer, ...
    if (bRet)
    {
        // If L == 0, ...
        if (aplInteger EQ 0)
        {
            // Res = 1
            mphc2v_init_set (&lpMemRes[uRes], &mpcfOne);

            return;
        } else
        // If L == 1, ...
        if (aplInteger EQ 1)
        {
            // Res = R
            mphc2v_init_set (&lpMemRes[uRes], &lpatRht->aplHC2V);

            return;
        } // End IF
    } // End IF

    // Z = (!R) / (!L) * !R-L
    aplLft = FactHC2V (lpatLft->aplHC2V);                               // !L
    aplRht = FactHC2V (lpatRht->aplHC2V);                               // !R

    aplTmp1 = SubHC2V_RE (lpatRht->aplHC2V, lpatLft->aplHC2V);          // R-L
    aplTmp2 = FactHC2V (aplTmp1);                                       // !R-L

    Myhc2v_clear (&aplTmp1);

    aplTmp1 = MulHC2V_RE (aplLft, aplTmp2);                             // (!L) * !R-L
    lpMemRes[uRes] = DivHC2V_RE (aplRht, aplTmp1);                      // (!R) / ((!L) * !R-L)

    Myhc2v_clear (&aplTmp2);
    Myhc2v_clear (&aplTmp1);
} // End PrimFnDydQuoteDotHC2VisHC2VvHC2V


//***************************************************************************
//  $PrimFnDydQuoteDotBA2FisBA2FvBA2F
//
//  Primitive scalar function dyadic QuoteDot:  BA2F {is} BA2F fn BA2F
//***************************************************************************

void PrimFnDydQuoteDotBA2FisBA2FvBA2F
    (LPAPLBA2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLBA2F aplLft,
            aplRht,
            aplTmp1 = {0},
            aplTmp2 = {0};
    APLINT  aplInteger;             // Right arg as an integer
    UBOOL   bRet;                   // TRUE iff exact integer conversion

    // Attempt to convert the left arg to an integer
    aplInteger = ConvertToInteger_SCT (ARRAY_BA2F, &lpatLft->aplBA2F, 0, &bRet);

    // If the left arg is an integer, ...
    if (bRet)
    {
        // If L == 0, ...
        if (aplInteger EQ 0)
        {
            // Res = 1
            arb2f_init_set (&lpMemRes[uRes], &arb2f_1);

            return;
        } else
        // If L == 1, ...
        if (aplInteger EQ 1)
        {
            // Res = R
            arb2f_init_set (&lpMemRes[uRes], &lpatRht->aplBA2F);

            return;
        } // End IF
    } // End IF

    // Z = (!R) / (!L) * !R-L
    aplLft = FactBA2F (lpatLft->aplBA2F);                               // !L
    aplRht = FactBA2F (lpatRht->aplBA2F);                               // !R

    aplTmp1 = SubBA2F_RE (lpatRht->aplBA2F, lpatLft->aplBA2F);          // R-L
    aplTmp2 = FactBA2F (aplTmp1);                                       // !R-L

    arb2f_clear (&aplTmp1);

    aplTmp1 = MulBA2F_RE (aplLft, aplTmp2);                             // (!L) * !R-L
    lpMemRes[uRes] = DivBA2F_RE (aplRht, aplTmp1);                      // (!R) / ((!L) * !R-L)

    arb2f_clear (&aplTmp2);
    arb2f_clear (&aplTmp1);
} // End PrimFnDydQuoteDotBA2FisBA2FvBA2F


//***************************************************************************
//  $PrimFnDydQuoteDotHC4IisHC4IvHC4I
//
//  Primitive scalar function dyadic QuoteDot:  HC4I {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydQuoteDotHC4IisHC4IvHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);
} // End PrimFnDydQuoteDotHC4IisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydQuoteDotHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic QuoteDot:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydQuoteDotHC4FisHC4IvHC4I
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
    PrimFnDydQuoteDotHC4FisHC4FvHC4F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydQuoteDotHC4FisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydQuoteDotHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic QuoteDot:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydQuoteDotHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLHC4F aplLft,
            aplRht,
            aplTmp1 = {0},
            aplTmp4 = {0};
    APLINT  aplInteger;             // Right arg as an integer
    UBOOL   bRet;                   // TRUE iff exact integer conversion

    // Attempt to convert the left arg to an integer
    aplInteger = ConvertToInteger_SCT (ARRAY_HC4F, &lpatLft->aplHC4F, 0, &bRet);

    // If the left arg is an integer, ...
    if (bRet)
    {
        // If L == 0, ...
        if (aplInteger EQ 0)
        {
            // Res = 1
            lpMemRes[uRes] = fphfOne;

            return;
        } else
        // If L == 1, ...
        if (aplInteger EQ 1)
        {
            // Res = R
            lpMemRes[uRes] = lpatRht->aplHC4F;

            return;
        } // End IF
    } // End IF

    // Z = (!R) / (!L) * !R-L
    aplLft = FactHC4F (lpatLft->aplHC4F);                               // !L
    aplRht = FactHC4F (lpatRht->aplHC4F);                               // !R

    aplTmp1 = SubHC4F_RE (lpatRht->aplHC4F, lpatLft->aplHC4F);          // R-L
    aplTmp4 = FactHC4F (aplTmp1);                                       // !R-L

    aplTmp1 = MulHC4F_RE (aplLft, aplTmp4);                             // (!L) * !R-L
    lpMemRes[uRes] = DivHC4F_RE (aplRht, aplTmp1);                      // (!R) / ((!L) * !R-L)
} // End PrimFnDydQuoteDotHC4FisHC4FvHC4F


//***************************************************************************
//  $PrimFnDydQuoteDotHC4RisHC4RvHC4R
//
//  Primitive scalar function dyadic QuoteDot:  HC4R {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydQuoteDotHC4RisHC4RvHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    RaiseException (EXCEPTION_RESULT_HC4V, 0, 0, NULL);
} // End PrimFnDydQuoteDotHC4RisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydQuoteDotHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic QuoteDot:  HC4V {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydQuoteDotHC4VisHC4RvHC4R
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
    PrimFnDydQuoteDotHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydQuoteDotHC4VisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydQuoteDotHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic QuoteDot:  HC4V {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydQuoteDotHC4VisHC4VvHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLHC4V aplLft,
            aplRht,
            aplTmp1 = {0},
            aplTmp4 = {0};
    APLINT  aplInteger;             // Right arg as an integer
    UBOOL   bRet;                   // TRUE iff exact integer conversion

    // Attempt to convert the left arg to an integer
    aplInteger = ConvertToInteger_SCT (ARRAY_HC4V, &lpatLft->aplHC4F, 0, &bRet);

    // If the left arg is an integer, ...
    if (bRet)
    {
        // If L == 0, ...
        if (aplInteger EQ 0)
        {
            // Res = 1
            mphc4v_init_set (&lpMemRes[uRes], &mphfOne);

            return;
        } else
        // If L == 1, ...
        if (aplInteger EQ 1)
        {
            // Res = R
            mphc4v_init_set (&lpMemRes[uRes], &lpatRht->aplHC4V);

            return;
        } // End IF
    } // End IF

    // Z = (!R) / (!L) * !R-L
    aplLft = FactHC4V (lpatLft->aplHC4V);                               // !L
    aplRht = FactHC4V (lpatRht->aplHC4V);                               // !R

    aplTmp1 = SubHC4V_RE (lpatRht->aplHC4V, lpatLft->aplHC4V);          // R-L
    aplTmp4 = FactHC4V (aplTmp1);                                       // !R-L

    Myhc4v_clear (&aplTmp1);

    aplTmp1 = MulHC4V_RE (aplLft, aplTmp4);                             // (!L) * !R-L
    lpMemRes[uRes] = DivHC4V_RE (aplRht, aplTmp1);                      // (!R) / ((!L) * !R-L)

    Myhc4v_clear (&aplTmp4);
    Myhc4v_clear (&aplTmp1);
} // End PrimFnDydQuoteDotHC4VisHC4VvHC4V


//***************************************************************************
//  $PrimFnDydQuoteDotBA4FisBA4FvBA4F
//
//  Primitive scalar function dyadic QuoteDot:  BA4F {is} BA4F fn BA4F
//***************************************************************************

void PrimFnDydQuoteDotBA4FisBA4FvBA4F
    (LPAPLBA4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLBA4F aplLft,
            aplRht,
            aplTmp1 = {0},
            aplTmp4 = {0};
    APLINT  aplInteger;             // Right arg as an integer
    UBOOL   bRet;                   // TRUE iff exact integer conversion

    // Attempt to convert the left arg to an integer
    aplInteger = ConvertToInteger_SCT (ARRAY_BA4F, &lpatLft->aplBA4F, 0, &bRet);

    // If the left arg is an integer, ...
    if (bRet)
    {
        // If L == 0, ...
        if (aplInteger EQ 0)
        {
            // Res = 1
            arb4f_init_set (&lpMemRes[uRes], &arb4f_1);

            return;
        } else
        // If L == 1, ...
        if (aplInteger EQ 1)
        {
            // Res = R
            arb4f_init_set (&lpMemRes[uRes], &lpatRht->aplBA4F);

            return;
        } // End IF
    } // End IF

    // Z = (!R) / (!L) * !R-L
    aplLft = FactBA4F (lpatLft->aplBA4F);                               // !L
    aplRht = FactBA4F (lpatRht->aplBA4F);                               // !R

    aplTmp1 = SubBA4F_RE (lpatRht->aplBA4F, lpatLft->aplBA4F);          // R-L
    aplTmp4 = FactBA4F (aplTmp1);                                       // !R-L

    arb4f_clear (&aplTmp1);

    aplTmp1 = MulBA4F_RE (aplLft, aplTmp4);                             // (!L) * !R-L
    lpMemRes[uRes] = DivBA4F_RE (aplRht, aplTmp1);                      // (!R) / ((!L) * !R-L)

    arb4f_clear (&aplTmp4);
    arb4f_clear (&aplTmp1);
} // End PrimFnDydQuoteDotBA4FisBA4FvBA4F


//***************************************************************************
//  End of File: pf_qdot.c
//***************************************************************************
