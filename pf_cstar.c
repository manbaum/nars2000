//***************************************************************************
//  NARS2000 -- Primitive Function -- CircleStar
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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
#include <float.h>
#include "headers.h"


#ifndef PROTO
PRIMSPEC PrimSpecCircleStar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecCircleStarStorageTypeMon,
    NULL,   // &PrimFnMonCircleStarAPA_EM, -- Can't happen w/CircleStar

    NULL,   // &PrimFnMonCircleStarBisB, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarBisI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarBisF, -- Can't happen w/CircleStar

////               IisB,     // Handled via type promotion (to IisI)
    NULL,   // &PrimFnMonCircleStarIisI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnMonCircleStarIisF, -- Can't happen w/CircleStar

////               FisB,     // Handled via type promotion (to FisI)
    &PrimFnMonCircleStarFisI,
    &PrimFnMonCircleStarFisF,

    NULL,   // &PrimFnMonCircleStarRisR, -- Can't happen w/CircleStar

////               VisR,    // Handled via type promotion (to VisV)
    &PrimFnMonCircleStarVisV,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecCircleStarStorageTypeDyd,
    NULL,   // &PrimFnDydCircleStarAPA_EM, -- Can't happen w/CircleStar

    &PrimFnDydRightCaretUnderbarBisBvB,
    NULL,   // &PrimFnDydCircleStarBisIvI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisFvF, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarBisCvC, -- Can't happen w/CircleStar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydCircleStarIisIvI, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarIisFvF, -- Can't happen w/CircleStar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    &PrimFnDydCircleStarFisIvI,
    &PrimFnDydCircleStarFisFvF,

    NULL,   // &PrimFnDydCircleStarBisRvR, -- Can't happen w/CircleStar
    NULL,   // &PrimFnDydCircleStarRisRvR, -- Can't happen w/CircleStar

    NULL,   // &PrimFnDydCircleStarBisVvV, -- Can't happen w/CircleStar
////                 VisRvR,    // Handled via type promotion (to VisVvV)
    &PrimFnDydCircleStarVisVvV,
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
    if (IsEmpty (aplNELMRht) && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    if (IsSimpleChar (*lpaplTypeRht)
     || *lpaplTypeRht EQ ARRAY_LIST)
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    aplTypeRes = *lpaplTypeRht;

    // Except that BOOL, INT and APA become FLOAT
    if (IsSimpleInt (aplTypeRes))
        aplTypeRes = ARRAY_FLOAT;
    else
    // Except that RAT becomes VFP
    if (IsRat (aplTypeRes))
        aplTypeRes = ARRAY_VFP;

    return aplTypeRes;
} // End PrimSpecCircleStarStorageTypeMon


//***************************************************************************
//  $PrimFnMonCircleStarFisI
//
//  Primitive scalar function monadic CircleStar:  F {is} fn I
//***************************************************************************

APLFLOAT PrimFnMonCircleStarFisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  {log} 0
    if (aplIntegerRht EQ 0)
        return TranslateQuadICIndex (0,
                                     ICNDX_LOG0,
                                     (APLFLOAT) aplIntegerRht);

    // Check for special cases:  {log} N  for N < 0
    if (aplIntegerRht < 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return log ((APLFLOAT) aplIntegerRht);
} // End PrimFnMonCircleStarFisI


//***************************************************************************
//  $PrimFnMonCircleStarFisF
//
//  Primitive scalar function monadic CircleStar:  F {is} fn F
//***************************************************************************

APLFLOAT PrimFnMonCircleStarFisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for indeterminates:  {log} 0
    if (aplFloatRht EQ 0)
        return TranslateQuadICIndex (0,
                                     ICNDX_LOG0,
                                     aplFloatRht);

    // Check for special cases:  {log} _
    if (IsPosInfinity (aplFloatRht))
        return PosInfinity;

    // Check for special cases:  {log} -_
    // Check for special cases:  {log} N  for N < 0
    if (IsNegInfinity (aplFloatRht)
     || aplFloatRht < 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return log (aplFloatRht);
} // End PrimFnMonCircleStarFisF


//***************************************************************************
//  $PrimFnMonCircleStarVisV
//
//  Primitive scalar function monadic CircleStar:  V {is} fn V
//
//  See "http://www.jsoftware.com/jwiki/Essays/Extended%20Precision%20Functions"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCircleStarVisV"
#else
#define APPEND_NAME
#endif

APLVFP PrimFnMonCircleStarVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP  mpfRes  = {0};
    APLMPFR mpfrRes = {0};
#if 0
    APLMPI mpzTmp = {0};
    APLVFP mpfRes = {0},
           mpfTmp = {0},
           mpfLn2;
    signed long log2x;
    int    log2xSign;
#endif
    // Check for indeterminates:  {log} 0
    if (IsMpf0 (&aplVfpRht))
        return mpf_QuadICValue (aplVfpRht,          // No left arg
                                ICNDX_LOG0,
                                aplVfpRht,
                                mpfRes);
    // Check for special cases:  {log} _
    if (IsMpfPosInfinity (&aplVfpRht))
        return mpfPosInfinity;

    // Check for special cases:  {log} -_
    // Check for special cases:  {log} N  for N < 0
    if (IsMpfNegInfinity (&aplVfpRht)
     || mpf_cmp_ui (&aplVfpRht, 0) < 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        // Convert the data to mpfr-format
        mpfr_init  (&mpfrRes);
        mpfr_set_f (&mpfrRes, &aplVfpRht, MPFR_RNDN);

        // Let MPFR handle it
        mpfr_log   (&mpfrRes, &mpfrRes, MPFR_RNDN);

        // Convert the data to mpf-format
        mpf_init   (&mpfRes);
        mpfr_get_f (&mpfRes, &mpfrRes, MPFR_RNDN);

        // We no longer need this storage
        mpfr_clear (&mpfrRes);

        return mpfRes;
#if 0
    // Initialize the result
    mpf_init_copy (&mpfRes, &aplVfpRht);



    // First, represent V as r * 2 ^ m where m is an integer
    //   and r is between 1/sqrt (2) and sqrt (2).
    // This means that log (V) = (log (r)) + m * log (2)
////log2x = (int) (floor (0.5 + (log (mpf_get_d (&aplVfpRht)) / log (2.0))));

    mpz_init (&mpzTmp);
    mpf_init (&mpfTmp);

    if (mpf_cmp (&mpfRes, &mpfSqrt2) > 0)
    {
        mpf_div (&mpfTmp, &mpfRes, &mpfSqrt2);
        mpf_floor (&mpfTmp, &mpfTmp);
        mpz_set_f (&mpzTmp, &mpfTmp);
        log2x = (int) mpz_sizeinbase (&mpzTmp, 2);
        mpf_div_2exp (&mpfRes, &mpfRes, log2x);
        log2xSign =  1;
    } else
    if (mpf_cmp (&mpfInvSqrt2, &mpfRes) > 0)
    {
        mpf_div (&mpfTmp, &mpfInvSqrt2, &mpfRes);
        mpf_floor (&mpfTmp, &mpfTmp);
        mpz_set_f (&mpzTmp, &mpfTmp);
        log2x = (int) mpz_sizeinbase (&mpzTmp, 2);
        mpf_mul_2exp (&mpfRes, &mpfRes, log2x);
        log2xSign = -1;
    } else
        log2x = log2xSign = 0;

    Myf_clear (&mpfTmp);
    Myz_clear (&mpzTmp);

    // Check the scale as the above calculation of log2x can be off by one
    while (mpf_cmp (&mpfRes, &mpfSqrt2) > 0)
    {
        mpf_div_ui (&mpfRes, &mpfRes, 2);
        log2x++;
    } // End WHILE

    while (mpf_cmp (&mpfInvSqrt2, &mpfRes) > 0)
    {
        mpf_mul_ui (&mpfRes, &mpfRes, 2);
        log2x++;
    } // End WHILE

    // Calculate the log of mpfRes via the power series 4.1.27 in Abramowitz & Stegun
    mpfTmp = LogVfp (mpfRes);

    // Copy to the result
    mpf_copy (&mpfRes, &mpfTmp);

    // Calculate the log of 2 ...
    mpf_set_ui (&mpfTmp, 2);
    mpfLn2 = LogVfp (mpfTmp);

    // Finally, convert the result back to normal
    mpf_mul_ui (&mpfTmp, &mpfLn2, log2x);
    if (log2xSign < 0)
        mpf_neg (&mpfTmp, &mpfTmp);
    mpf_add    (&mpfRes, &mpfRes, &mpfTmp);

    // We no longer need this storage
    Myf_clear (&mpfTmp);
    Myf_clear (&mpfLn2);

    return mpfRes;
#endif
} // End PrimFnMonCircleStarVisV
#undef  APPEND_NAME


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
    APLVFP mpfRes   = {0},      // MPF result
           mpfTmp1  = {0},      // ... temp
           mpfTmp2  = {0},      // ...
           mpfBase  = {0};      // ... base
    UINT   uRes;                // Loop counter

    mpf_init (&mpfRes);
    mpf_init (&mpfTmp1);
    mpf_init (&mpfTmp2);
    mpf_init (&mpfBase);

    // ln z = 2 * [(1/1)*(z-1)/(z+1) + (1/3)*((z-1)/z+1))^3 + (1/5)*...]

    // Calculate Base:  (z-1)/(z+1)
    mpf_sub_ui (&mpfTmp1, &aplVfpRht, 1);
    mpf_add_ui (&mpfTmp2, &aplVfpRht, 1);
    mpf_div    (&mpfBase, &mpfTmp1, &mpfTmp2);

    // Calculate the multiplier:  Base^2
    mpf_mul    (&mpfTmp2, &mpfBase, &mpfBase);

    // Loop through the # terms
    for (uRes = 0 ; uRes < nDigitsFPC; uRes++)
    {
        // Divide the base by (2 * uRes) + 1
        mpf_div_ui (&mpfTmp1, &mpfBase, 2 * uRes + 1);

        // Accumulate into the result
        mpf_add (&mpfRes, &mpfRes, &mpfTmp1);

        // Multiply the base by the multiplier
        mpf_mul (&mpfBase, &mpfBase, &mpfTmp2);
    } // End FOR

    // Multiply by the final 2
    mpf_mul_ui (&mpfRes, &mpfRes, 2);

    // We no longer need this storage
    Myf_clear (&mpfBase);
    Myf_clear (&mpfTmp2);
    Myf_clear (&mpfTmp1);

    return mpfRes;
} // End LogfVfp
#endif

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
    if (IsEmpty (aplNELMLft) && IsSimpleChar (*lpaplTypeLft))
        *lpaplTypeLft = ARRAY_BOOL;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsEmpty (aplNELMRht) && IsSimpleChar (*lpaplTypeRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // If both arguments are simple numeric,
    //   the result is FLOAT
    if (IsSimpleNum (*lpaplTypeLft)
     && IsSimpleNum (*lpaplTypeRht))
        aplTypeRes = ARRAY_FLOAT;
    else
        // Calculate the storage type of the result
        aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // Except that RAT becomes VFP
    if (IsRat (aplTypeRes))
        aplTypeRes = ARRAY_VFP;

    return aplTypeRes;
} // End PrimSpecCircleStarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydCircleStarFisIvI
//
//  Primitive scalar function dyadic CircleStar:  F {is} I fn I
//***************************************************************************

APLFLOAT PrimFnDydCircleStarFisIvI
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  B {log} B
    if (IsBooleanValue (aplIntegerLft)
     && IsBooleanValue (aplIntegerRht))
        return TranslateQuadICIndex ((APLFLOAT) aplIntegerLft,
                                     icndxLog[aplIntegerLft][aplIntegerRht],
                                     (APLFLOAT) aplIntegerRht);

    // The EAS says "If A and B are equal, return one."
    if (aplIntegerLft EQ aplIntegerRht)
        return 1;

    // Calculate the log
    aplFloatRes = log ((APLFLOAT) aplIntegerRht) / log ((APLFLOAT) aplIntegerLft);

    // Check for ± infinity result (both args are finite)
    if (IsInfinity (aplFloatRes))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return aplFloatRes;
} // End PrimFnDydCircleStarFisIvI


//***************************************************************************
//  $PrimFnDydCircleStarFisFvF
//
//  Primitive scalar function dyadic CircleStar:  F {is} F fn F
//***************************************************************************

APLFLOAT PrimFnDydCircleStarFisFvF
    (APLFLOAT   aplFloatLft,
     APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT    aplFloatRes;

    // Check for special cases:  0 {log} ±_
    // Check for special cases:  ±_ {log} 0
    // Check for special cases:  ±_ {log} ±_
    if ((aplFloatLft EQ 0 && IsInfinity (aplFloatRht))
     || (IsInfinity (aplFloatLft) && aplFloatRht EQ 0)
     || (IsInfinity (aplFloatLft) && IsInfinity (aplFloatRht)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Check for indeterminates:  B {log} B
    if (IsBooleanValue (aplFloatLft)
     && IsBooleanValue (aplFloatRht))
        return TranslateQuadICIndex (aplFloatLft,
                                     icndxLog[(UINT) aplFloatLft][(UINT) aplFloatRht],
                                     aplFloatRht);

    // The EAS says "If A and B are equal, return one."
    if (aplFloatLft EQ aplFloatRht)
        return 1;

    // Calculate the log
    aplFloatRes = log (aplFloatRht) / log (aplFloatLft);

    // Check for ± infinity result with both args finite
    if ( IsInfinity (aplFloatRes)
     && !IsInfinity (aplFloatLft)
     && !IsInfinity (aplFloatRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return aplFloatRes;
} // End PrimFnDydCircleStarFisFvF


//***************************************************************************
//  $PrimFnDydCircleStarVisVvV
//
//  Primitive scalar function dyadic CircleStar:  V {is} V fn V
//***************************************************************************

APLVFP PrimFnDydCircleStarVisVvV
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLVFP mpfRes = {0},
           mpfLft,
           mpfRht;

    // Check for special cases:  0 {log} ±_
    // Check for special cases:  ±_ {log} 0
    // Check for special cases:  ±_ {log} ±_
    if ((IsMpf0 (&aplVfpLft) && mpf_inf_p (&aplVfpRht))
     || (mpf_inf_p (&aplVfpLft) && IsMpf0 (&aplVfpRht))
     || (mpf_inf_p (&aplVfpLft) && mpf_inf_p (&aplVfpRht)))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Check for indeterminates:  B {log} B
    if (IsBooleanVfp (&aplVfpLft)
     && IsBooleanVfp (&aplVfpRht))
        return mpf_QuadICValue (aplVfpLft,
                                icndxLog[IsMpf1 (&aplVfpLft)][IsMpf1(&aplVfpRht)],
                                aplVfpRht,
                                mpfRes);
    // Initialize the result
    mpf_init (&mpfRes);

    // The EAS says "If A and B are equal, return one."
    if (mpf_cmp (&aplVfpLft,  &aplVfpRht) EQ 0)
        mpf_set_ui (&mpfRes, 1);
    else
    {
        // Calculate log (aplMpfRht) / log (aplVfpLft)
        mpfLft = PrimFnMonCircleStarVisV (aplVfpLft, lpPrimSpec);
        mpfRht = PrimFnMonCircleStarVisV (aplVfpRht, lpPrimSpec);
        mpf_div (&mpfRes, &mpfRht, &mpfLft);

        // We no longer need this storage
        Myf_clear (&mpfRht);
        Myf_clear (&mpfLft);
    } // End IF

    return mpfRes;
} // End PrimFnDydCircleStarVisVvV


//***************************************************************************
//  End of File: pf_cstar.c
//***************************************************************************
