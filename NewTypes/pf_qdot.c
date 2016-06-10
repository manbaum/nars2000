//***************************************************************************
//  NARS2000 -- Primitive Function -- QuoteDot
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
#include <complex.h>

#define real(z)     (z)._Val[0]
#define imag(z)     (z)._Val[1]

#define __GSL_MATRIX_COMPLEX_LONG_DOUBLE_H__

#include "headers.h"
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_result.h>
#include <gsl/gsl_errno.h>


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
    &PrimFnMonQuoteDotHC8FisHC8I,
    &PrimFnMonQuoteDotHC8FisHC8F,
    NULL,   // &PrimFnMonQuoteDotHC8RisHC8R, -- Can't happen w/QuoteDot
    &PrimFnMonQuoteDotHC8VisHC8R,
    &PrimFnMonQuoteDotHC8VisHC8V,

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

    &PrimFnDydQuoteDotHC8IisHC8IvHC8I,
    NULL,   // &PrimFnDydQuoteDotHC8IisHC8FvHC8F, -- Can't happen w/QuoteDot
    &PrimFnDydQuoteDotHC8FisHC8IvHC8I,
    &PrimFnDydQuoteDotHC8FisHC8FvHC8F,
    &PrimFnDydQuoteDotHC8RisHC8RvHC8R,
    &PrimFnDydQuoteDotHC8VisHC8RvHC8R,
    &PrimFnDydQuoteDotHC8VisHC8VvHC8V,

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

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnQuoteDot_EM_YY"
#else
#define APPEND_NAME
#endif

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
#undef  APPEND_NAME


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

    // Split cases based upon the storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
        case ARRAY_RAT:
        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
        case ARRAY_NESTED:
            break;

        // Except that APAs are converted to INTs
        case ARRAY_APA:
            aplTypeRes = ARRAY_INT;

            break;

        // Except that HCINT -> HCFLT, HCRAT -> HCVFP, etc.
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            aplTypeRes++;               // Assuming order as in <ARRAY_TYPES>

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
            aplTypeRes = ARRAY_ERROR;

            break;

        defstop
            break;
    } // End SWITCH

    return aplTypeRes;
} // End PrimSpecQuoteDotStorageTypeMon


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
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
        else
            lpMemRes[uRes] = (APLINT) aplFloatRes;
    } else
    {
        // Check for results too large to express as integers
        if (lpatRht->aplInteger >= countof (factorial))
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
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
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
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
        lpMemRes[uRes] = *mpq_QuadICValue (&lpatRht->aplRat,        // No left arg
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
        return *mpfr_QuadICValue (&aplVfpRht,       // No left arg
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
////        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
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
//  $FactHC2V
//
//  Primitive scalar function monadic QuoteDot:  HC2V {is} fn HC2V
//***************************************************************************

APLHC2V FactHC2V
    (APLHC2V aplRht)

{
    APLHC2V aplRes = {0};           // The result

    NONCE_RE                // ***FINISHME***

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
//  $FactHC4F
//
//  Primitive scalar function monadic QuoteDot:  HC4F {is} fn HC4F
//***************************************************************************

APLHC4F FactHC4F
    (APLHC4F aplRht)

{
    APLHC4F aplRes = {0};           // The result

    NONCE_RE                // ***FINISHME***

    return aplRes;
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

    NONCE_RE                // ***FINISHME***

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
//  $FactHC8F
//
//  Primitive scalar function monadic QuoteDot:  HC8F {is} fn HC8F
//***************************************************************************

APLHC8F FactHC8F
    (APLHC8F aplRht)

{
    APLHC8F aplRes = {0};           // The result

    NONCE_RE                // ***FINISHME***

    return aplRes;
} // End FactHC8F


//***************************************************************************
//  $PrimFnMonQuoteDotHC8FisHC8I
//
//  Primitive scalar function monadic QuoteDot:  HC8F {is} fn HC8I
//***************************************************************************

void PrimFnMonQuoteDotHC8FisHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonQuoteDotHC8FisHC8F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonQuoteDotHC8FisHC8I


//***************************************************************************
//  $PrimFnMonQuoteDotHC8FisHC8F
//
//  Primitive scalar function monadic QuoteDot:  HC8F {is} fn HC8F
//***************************************************************************

void PrimFnMonQuoteDotHC8FisHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactHC8F (lpatRht->aplHC8F);
} // End PrimFnMonQuoteDotHC8FisHC8F


//***************************************************************************
//  $FactHC8V
//
//  Primitive scalar function monadic QuoteDot:  HC8V {is} fn HC8V
//***************************************************************************

APLHC8V FactHC8V
    (APLHC8V aplRht)

{
    APLHC8V aplRes = {0};           // The result

    NONCE_RE                // ***FINISHME***

    return aplRes;
} // End FactHC8V


//***************************************************************************
//  $PrimFnMonQuoteDotHC8VisHC8R
//
//  Primitive scalar function monadic QuoteDot:  HC8V {is} fn HC8R
//***************************************************************************

void PrimFnMonQuoteDotHC8VisHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonQuoteDotHC8VisHC8V (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonQuoteDotHC8VisHC8R


//***************************************************************************
//  $PrimFnMonQuoteDotHC8VisHC8V
//
//  Primitive scalar function monadic QuoteDot:  HC8V {is} fn HC8V
//***************************************************************************

void PrimFnMonQuoteDotHC8VisHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = FactHC8V (lpatRht->aplHC8V);
} // End PrimFnMonQuoteDotHC8VisHC8V


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
    APLINT aplIntegerTmp,
           aplIntegerRes,
           aplIntegerInd;

#define L   lpatLft->aplInteger
#define R   lpatRht->aplInteger
#define T   aplIntegerTmp
#define Z   aplIntegerRes
#define I   aplIntegerInd

    // Determine whether or not L, R, or L - R is a negative integer
    // From ISO-IEC 13751 Extended APL Standard, p. 90
    switch (4*(L < 0) + 2*(R < 0) + 1*(R < L))
    {
        //    L     R   R-L
        case 4*0 + 2*0 + 0:     // (!R)/((!L)*!R-L)
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

            PrimFnDydQuoteDotIisIvI (lpMemRes, uRes, lpatLft, lpatRht, lpPrimSpec);

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

            PrimFnDydQuoteDotIisIvI (lpMemRes, uRes, lpatLft, lpatRht, lpPrimSpec);

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
    APLFLOAT aplFloatTmp,
             aplFloatRes,
             aplFloatInd;
    APLINT   aplIntegerTmp,
             aplIntegerRes,
             aplIntegerInd,
             aplIntegerTmp2;
    UBOOL    bRet = TRUE;       // Assume the result is valid

    // FYI:  The only way this function can be called is
    //   from an exception raised from IisIvI above.

#define LI  lpatLft->aplInteger
#define RI  lpatRht->aplInteger
#define TI  aplIntegerTmp
#define TI2 aplIntegerTmp2
#define ZI  aplIntegerRes
#define II  aplIntegerInd

#define LF  lpatLft->aplFloat
#define RF  lpatRht->aplFloat
#define TF  aplFloatTmp
#define ZF  aplFloatRes
#define IF  aplFloatInd

    // Determine whether or not LI, RI, or LI - RI is a negative integer
    // From ISO-IEC 13751 Extended APL Standard, p. 90
    switch (4*(LI < 0) + 2*(RI < 0) + 1*(RI < LI))
    {
        //    LI   RI  RI-LI
        case 4*0 + 2*0 + 0:     // (!RI)/((!LI)*!RI-LI)
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
                TI2 = imul64 (ZI, II + TI, &bRet, EXCEPTION_RESULT_FLOAT);
                if (bRet)
                    ZI = TI2 / II;
                else
                {
                    for (ZF = (APLFLOAT) ZI, TF = (APLFLOAT) TI, IF = (APLFLOAT) II;
                         II <= LI;
                         II++, IF++)
                    {
                        ZF = (ZF * (IF + TF)) / IF;
                        if (IsFltInfinity (ZF))
                            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
                    } // End FOR

                    lpMemRes[uRes] = ZF;

                    break;
                } // End IF/ELSE
            } // End FOR

            lpMemRes[uRes] = (APLFLOAT) ZI;

            break;

        case 4*0 + 2*0 + 1*1:   // 0
            lpMemRes[uRes] = 0;

            break;

        case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        case 4*0 + 2*1 + 1*1:   // ((-1)*LI)*LI!(LI-(RI+1))
            // Recurse with L replaced by LI
            // Recurse with R replaced by LI-(RI+1)
////////////LI = LI;
            RI = LI-(RI+1);

            PrimFnDydQuoteDotFisIvI (lpMemRes, uRes, lpatLft, lpatRht, lpPrimSpec);

            // Handle sign
            lpMemRes[uRes] = ((LI%2) ? -1: 1) * lpMemRes[uRes];

            break;

        case 4*1 + 2*0 + 1*0:   // 0
            lpMemRes[uRes] = 0;

            break;

        case 4*1 + 2*0 + 1*1:   // (Can't happen)
            lpMemRes[uRes] = -1;

            break;

        case 4*1 + 2*1 + 1*0:   // ((-1)*RI-LI)*(-RI+1)!|LI+1
            // Recurse with L replaced by -(RI+1)
            // Recurse with R replaced by -(LI+1)
            TI = LI;
            LI = -(RI+1);
            RI = -(TI+1);

            PrimFnDydQuoteDotFisIvI (lpMemRes, uRes, lpatLft, lpatRht, lpPrimSpec);

            // Handle sign
            lpMemRes[uRes] = (((RI-LI)%2) ? -1: 1) * lpMemRes[uRes];

            break;

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
    APLFLOAT aplFloatTmp,
             aplFloatRes,
             aplFloatInd;

#define LF  lpatLft->aplFloat
#define RF  lpatRht->aplFloat
#define TF  aplFloatTmp
#define ZF  aplFloatRes
#define IF  aplFloatInd

    // If LF and RF are exact integers
    if ((LF EQ floor (LF)
      || LF EQ ceil  (LF))
     && (RF EQ floor (RF)
      || RF EQ ceil  (RF)))
    {
        // Determine whether or not LF, RF, or LF - RF is a negative integer
        // From ISO-IEC 13751 Extended APL Standard, p. 90
        switch (4*(LF < 0) + 2*(RF < 0) + 1*(RF < LF))
        {
            //    LF   RF  RF-LF
            case 4*0 + 2*0 + 0:     // (!RF)/((!LF)*!RF-LF)
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
                    ZF = (ZF * (IF + TF)) / IF;

                lpMemRes[uRes] = ZF;

            case 4*0 + 2*0 + 1*1:   // 0
                lpMemRes[uRes] = 0;

            case 4*0 + 2*1 + 1*0:   // DOMAIN ERROR
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            case 4*0 + 2*1 + 1*1:   // ((-1)*LF)*LF!(LF-(RF+1))
                // Recurse with L replaced by LF
                // Recurse with R replaced by LF-(RF+1)
////////////////LF = LF;
                RF = LF-(RF+1);

                PrimFnDydQuoteDotFisFvF (lpMemRes, uRes, lpatLft, lpatRht, lpPrimSpec);

                // Handle sign
                lpMemRes[uRes] = (fmod (LF, 2) ? -1: 1) * lpMemRes[uRes];

            case 4*1 + 2*0 + 1*0:   // 0
                lpMemRes[uRes] = 0;

            case 4*1 + 2*0 + 1*1:   // (Can't happen)
                lpMemRes[uRes] = -1;

            case 4*1 + 2*1 + 1*0:   // ((-1)*RI-LI)*(-RI+1)!|LI+1
                // Recurse with L replaced by -(RF+1)
                // Recurse with R replaced by -(LF+1)
                TF = LF;
                LF = -(RF+1);
                RF = -(TF+1);

                PrimFnDydQuoteDotFisFvF (lpMemRes, uRes, lpatLft, lpatRht, lpPrimSpec);

                // Handle sign
                lpMemRes[uRes] = (fmod (RF-LF, 2) ? -1: 1) * lpMemRes[uRes];

            case 4*1 + 2*1 + 1*1:   // 0
                lpMemRes[uRes] = 0;

            defstop
                lpMemRes[uRes] = -1;
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
    UINT   uLft;

    // Initialize the result to 0/1
    mpq_init (&lpMemRes[uRes]);

    // Using the identity A!B <=> (B-A)!B and because the function
    //   mpz_bin_ui requires that the left argument must fit in a
    //   UINT, we calculate the alternate left argument in case it
    //   fits in a UINT.
    mpq_sub (&lpMemRes[uRes], &lpatRht->aplRat, &lpatLft->aplRat);

    // Use the smaller of the two for the left arg
    if (mpq_cmp (&lpMemRes[uRes], &lpatLft->aplRat) > 0)
        mpq_set (&lpMemRes[uRes], &lpatLft->aplRat);

    // If both denominators are 1,
    //   and the left numerator fits in a UINT, ...
    if (mpq_integer_p (&lpatLft->aplRat)
     && mpq_integer_p (&lpatRht->aplRat)
     && mpz_fits_slong_p (mpq_numref (&lpMemRes[uRes])) NE 0)
    {
        // Extract the numerator of the left argument
        uLft = (UINT) mpz_get_si (mpq_numref (&lpMemRes[uRes]));

        // Compute the binomial coefficient
        mpz_bin_ui (mpq_numref (&lpMemRes[uRes]), mpq_numref (&lpatRht->aplRat), uLft);
    } else
    {
        Myq_clear (&lpMemRes[uRes]);

        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);
    } // End IF/ELSE
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
    APLVFP aplLft,
           aplRht,
           aplTmp = {0};

    // Z = (!R) / (!L) * !R-L
    aplLft = FactVFP (lpatLft->aplVfp);                                 // !L
    aplRht = FactVFP (lpatRht->aplVfp);                                 // !R

    mpfr_init_copy (&aplTmp, &lpatRht->aplVfp);                         // R
    mpfr_sub       (&aplTmp, &aplTmp, &lpatLft->aplVfp, MPFR_RNDN);     // R-L
    lpMemRes[uRes] = FactVFP (aplTmp);                                  // !R-L

    mpfr_mul (&lpMemRes[uRes], &aplLft, &lpMemRes[uRes], MPFR_RNDN);    // (!L) * !R-L
    mpfr_div (&lpMemRes[uRes], &aplRht, &lpMemRes[uRes], MPFR_RNDN);    // (!R) / ((!L) * !R-L)

    // We no longer need this storage
    Myf_clear (&aplRht);
    Myf_clear (&aplLft);
    Myf_clear (&aplTmp);
} // End PrimFnDydQuoteDotVisVvV


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
//  $PrimFnDydQuoteDotHC8IisHC8IvHC8I
//
//  Primitive scalar function dyadic QuoteDot:  HC8I {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydQuoteDotHC8IisHC8IvHC8I
    (LPAPLHC8I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    RaiseException (EXCEPTION_RESULT_HC8F, 0, 0, NULL);
} // End PrimFnDydQuoteDotHC8IisHC8IvHC8I


//***************************************************************************
//  $PrimFnDydQuoteDotHC8FisHC8IvHC8I
//
//  Primitive scalar function dyadic QuoteDot:  HC8F {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydQuoteDotHC8FisHC8IvHC8I
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
    PrimFnDydQuoteDotHC8FisHC8FvHC8F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydQuoteDotHC8FisHC8IvHC8I


//***************************************************************************
//  $PrimFnDydQuoteDotHC8FisHC8FvHC8F
//
//  Primitive scalar function dyadic QuoteDot:  HC8F {is} HC8F fn HC8F
//***************************************************************************

void PrimFnDydQuoteDotHC8FisHC8FvHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLHC8F aplLft,
            aplRht,
            aplTmp1 = {0},
            aplTmp8 = {0};

    // Z = (!R) / (!L) * !R-L
    aplLft = FactHC8F (lpatLft->aplHC8F);                               // !L
    aplRht = FactHC8F (lpatRht->aplHC8F);                               // !R

    aplTmp1 = SubHC8F_RE (lpatRht->aplHC8F, lpatLft->aplHC8F);          // R-L
    aplTmp8 = FactHC8F (aplTmp1);                                       // !R-L

    aplTmp1 = MulHC8F_RE (aplLft, aplTmp8);                             // (!L) * !R-L
    lpMemRes[uRes] = DivHC8F_RE (aplRht, aplTmp1);                      // (!R) / ((!L) * !R-L)
} // End PrimFnDydQuoteDotHC8FisHC8FvHC8F


//***************************************************************************
//  $PrimFnDydQuoteDotHC8RisHC8RvHC8R
//
//  Primitive scalar function dyadic QuoteDot:  HC8R {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydQuoteDotHC8RisHC8RvHC8R
    (LPAPLHC8R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    RaiseException (EXCEPTION_RESULT_HC8V, 0, 0, NULL);
} // End PrimFnDydQuoteDotHC8RisHC8RvHC8R


//***************************************************************************
//  $PrimFnDydQuoteDotHC8VisHC8RvHC8R
//
//  Primitive scalar function dyadic QuoteDot:  HC8V {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydQuoteDotHC8VisHC8RvHC8R
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
    PrimFnDydQuoteDotHC8VisHC8VvHC8V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydQuoteDotHC8VisHC8RvHC8R


//***************************************************************************
//  $PrimFnDydQuoteDotHC8VisHC8VvHC8V
//
//  Primitive scalar function dyadic QuoteDot:  HC8V {is} HC8V fn HC8V
//***************************************************************************

void PrimFnDydQuoteDotHC8VisHC8VvHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLHC8V aplLft,
            aplRht,
            aplTmp1 = {0},
            aplTmp8 = {0};

    // Z = (!R) / (!L) * !R-L
    aplLft = FactHC8V (lpatLft->aplHC8V);                               // !L
    aplRht = FactHC8V (lpatRht->aplHC8V);                               // !R

    aplTmp1 = SubHC8V_RE (lpatRht->aplHC8V, lpatLft->aplHC8V);          // R-L
    aplTmp8 = FactHC8V (aplTmp1);                                       // !R-L

    Myhc8v_clear (&aplTmp1);

    aplTmp1 = MulHC8V_RE (aplLft, aplTmp8);                             // (!L) * !R-L
    lpMemRes[uRes] = DivHC8V_RE (aplRht, aplTmp1);                      // (!R) / ((!L) * !R-L)

    Myhc8v_clear (&aplTmp8);
    Myhc8v_clear (&aplTmp1);
} // End PrimFnDydQuoteDotHC8VisHC8VvHC8V


//***************************************************************************
//  End of File: pf_qdot.c
//***************************************************************************
