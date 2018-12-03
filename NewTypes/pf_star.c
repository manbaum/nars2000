//***************************************************************************
//  NARS2000 -- Primitive Function -- Star
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

////#define OWN_EXPLOG

#define STRICT
#include <windows.h>
#include <math.h>
#include "headers.h"

#ifndef PROTO
PRIMSPEC PrimSpecStar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecStarStorageTypeMon,
    NULL,   // &PrimFnMonStarAPA_EM, -- Can't happen w/Star

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonStarBisB, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarBisI, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarBisF, -- Can't happen w/Star

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonStarIisI, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarIisF, -- Can't happen w/Star
    &PrimFnMonStarFisI,
    &PrimFnMonStarFisF,
    NULL,   // &PrimFnMonStarRisR, -- Can't happen w/Star
    &PrimFnMonStarVisR,
    &PrimFnMonStarVisV,

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonStarHC2IisHC2I, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarHC2IisHC2F, -- Can't happen w/Star
    &PrimFnMonStarHC2FisHC2I,
    &PrimFnMonStarHC2FisHC2F,
    NULL,   // &PrimFnMonStarHC2RisHC2R, -- Can't happen w/Star
    &PrimFnMonStarHC2VisHC2R,
    &PrimFnMonStarHC2VisHC2V,

    NULL,   // &PrimFnMonStarHC4IisHC4I, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarHC4IisHC4F, -- Can't happen w/Star
    &PrimFnMonStarHC4FisHC4I,
    &PrimFnMonStarHC4FisHC4F,
    NULL,   // &PrimFnMonStarHC4RisHC4R, -- Can't happen w/Star
    &PrimFnMonStarHC4VisHC4R,
    &PrimFnMonStarHC4VisHC4V,

    NULL,   // &PrimFnMonStarHC8IisHC8I, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarHC8IisHC8F, -- Can't happen w/Star
    &PrimFnMonStarHC8FisHC8I,
    &PrimFnMonStarHC8FisHC8F,
    NULL,   // &PrimFnMonStarHC8RisHC8R, -- Can't happen w/Star
    &PrimFnMonStarHC8VisHC8R,
    &PrimFnMonStarHC8VisHC8V,

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonStarFisHC2I, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarFisHC2F, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarVisHC2R, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarVisHC2V, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarFisHC4I, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarFisHC4F, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarVisHC4R, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarVisHC4V, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarFisHC8I, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarFisHC8F, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarVisHC8R, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarVisHC8V, -- Can't happen w/Star

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecStarStorageTypeDyd,
    NULL,   // &PrimFnDydStarAPA_EM, -- Can't happen w/Star

    // Dyadic Boolean result functions
    &PrimFnDydStarBisBvB,
    NULL,   // &PrimFnDydStarBisIvI, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisFvF, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisCvC, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisHvH, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisRvR, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisVvV, -- Can't happen w/Star

    NULL,   // &PrimFnDydStarBisHC2IvHC2I, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisHC2FvHC2F, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisHC2RvHC2R, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisHC2VvHC2V, -- Can't happen w/Star

    NULL,   // &PrimFnDydStarBisHC4IvHC4I, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisHC4FvHC4F, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisHC4RvHC4R, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisHC4VvHC4V, -- Can't happen w/Star

    NULL,   // &PrimFnDydStarBisHC8IvHC8I, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisHC8FvHC8F, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisHC8RvHC8R, -- Can't happen w/Star
    NULL,   // &PrimFnDydStarBisHC8VvHC8V, -- Can't happen w/Star

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydStarIisIvI,
    NULL,   // &PrimFnDydStarIisFvF, -- Can't happen w/Star
    &PrimFnDydStarFisIvI,
    &PrimFnDydStarFisFvF,
    &PrimFnDydStarRisRvR,
    &PrimFnDydStarVisRvR,
    &PrimFnDydStarVisVvV,

    &PrimFnDydStarHC2IisHC2IvHC2I,
    NULL,   // &PrimFnDydStarHC2IisHC2FvHC2F, -- Can't happen w/Star
    &PrimFnDydStarHC2FisHC2IvHC2I,
    &PrimFnDydStarHC2FisHC2FvHC2F,
    &PrimFnDydStarHC2RisHC2RvHC2R,
    &PrimFnDydStarHC2VisHC2RvHC2R,
    &PrimFnDydStarHC2VisHC2VvHC2V,

    &PrimFnDydStarHC4IisHC4IvHC4I,
    NULL,   // &PrimFnDydStarHC4IisHC4FvHC4F, -- Can't happen w/Star
    &PrimFnDydStarHC4FisHC4IvHC4I,
    &PrimFnDydStarHC4FisHC4FvHC4F,
    &PrimFnDydStarHC4RisHC4RvHC4R,
    &PrimFnDydStarHC4VisHC4RvHC4R,
    &PrimFnDydStarHC4VisHC4VvHC4V,

    &PrimFnDydStarHC8IisHC8IvHC8I,
    NULL,   // &PrimFnDydStarHC8IisHC8FvHC8F, -- Can't happen w/Star
    &PrimFnDydStarHC8FisHC8IvHC8I,
    &PrimFnDydStarHC8FisHC8FvHC8F,
    &PrimFnDydStarHC8RisHC8RvHC8R,
    &PrimFnDydStarHC8VisHC8RvHC8R,
    &PrimFnDydStarHC8VisHC8VvHC8V,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonStarB64isB64, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarB32isB32, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarB16isB16, -- Can't happen w/Star
    NULL,   // &PrimFnMonStarB08isB08, -- Can't happen w/Star

    // Dyadic Boolean chunk functions
    &PrimFnDydStarB64isB64vB64,
    &PrimFnDydStarB32isB32vB32,
    &PrimFnDydStarB16isB16vB16,
    &PrimFnDydStarB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecStar};
#endif


//***************************************************************************
//  $PrimFnStar_EM_YY
//
//  Primitive function for monadic and dyadic Star ("exponential" and "power")
//***************************************************************************

LPPL_YYSTYPE PrimFnStar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharStar (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnStar_EM_YY


//***************************************************************************
//  $PrimSpecStarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStarStorageTypeMon
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
} // End PrimSpecStarStorageTypeMon


//***************************************************************************
//  $PrimFnMonStarFisI
//
//  Primitive scalar function monadic Star:  F {is} fn I
//***************************************************************************

void PrimFnMonStarFisI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = exp ((APLFLOAT) lpatRht->aplInteger);
} // End PrimFnMonStarFisI


//***************************************************************************
//  $PrimFnMonStarFisF
//
//  Primitive scalar function monadic Star:  F {is} fn F
//***************************************************************************

void PrimFnMonStarFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = exp (lpatRht->aplFloat);
} // End PrimFnMonStarFisF


//***************************************************************************
//  $ExpVfp_RE
//***************************************************************************

APLVFP ExpVfp_RE
    (APLVFP aplVfpRht)

{
#ifdef OWN_EXPLOG
    APLVFP   mpfRes  = {0},
             mpfTmp1 = {0},
             mpfTmp2 = {0},
             mpfLn2;
    APLINT   log2x;
#else
    APLVFP mpfRes = {0};
#endif
    // Check for special case:  ^-_
    if (IsMpfNegInfinity (&aplVfpRht))
        // Initialize the result to 0
        mpfr_init0 (&mpfRes);
    else
    {
#ifdef OWN_EXPLOG
        // Initialize the result
        mpfr_init_copy (&mpfRes, &aplVfpRht);
        mpfr_init0 (&mpfTmp1);
        mpfr_init0 (&mpfTmp2);

        // First, represent exp (V) as (2 ^ m) * exp (V - m * log (2))
        //   and m = floor (0.5 + V/log (2)) which bounds
        //   |V - m * log(2) to be less than 0.5 * log (2)
////////log2x = (APLINT) (floor (0.5 + (mpfr_get_d (&aplVfpRht) / log (2.0))));

        // Get the log of 2
        mpfr_const_log2 (&mpfLn2, MPFR_RNDN);

        // Divide V by log of 2
        mpfr_div (&mpfTmp1, &aplVfpRht, &mpfLn2, MPFR_RNDN);

        // Add 0.5
        mpfr_set_d (&mpfTmp2, 0.5, MPFR_RNDN);
        mpfr_add   (&mpfTmp1, &mpfTmp1, &mpfTmp2, MPFR_RNDN);

        // Floor
        mpfr_floor (&mpfTmp1, &mpfTmp1);

        log2x = mpfr_get_si (&mpfTmp1);

        // Times m
        mpfr_mul_ui (&mpfTmp1, &mpfLn2, abs ((int) log2x), MPFR_RNDN);
        if (log2x < 0)
            mpfr_neg (&mpfTmp1, &mpfTmp1, MPFR_RNDN);
        // Subtract from V to yield V - m * log (2)
        mpfr_sub (&mpfRes, &mpfRes, &mpfTmp1, MPFR_RNDN);

        // Calculate the exp of mpfRes via the power series 4.2.1 in Abramowitz & Stegun
        Myf_clear (&mpfTmp2);
        mpfTmp2 = ExpVfp (mpfRes);

        // Copy to the result
        mpfr_copy (&mpfRes, &mpfTmp2);

        // Finally, convert the result back to normal
        //   by multiplying it by 2 ^ m.
        // Split cases based upon the signum of m
        switch (signumint (log2x))
        {
            case -1:
                mpfr_div_2exp (&mpfRes, &mpfRes, (int) -log2x);

                break;

            case  0:
                break;

            case  1:
                mpfr_mul_2exp (&mpfRes, &mpfRes, (int)  log2x);

                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this storage
        Myf_clear (&mpfTmp2);
        Myf_clear (&mpfLn2 );
        Myf_clear (&mpfTmp1);
#else
#ifdef DEBUG
        mpfr_buildopt_tls_p ();
#endif
        // Initialize to 0
        mpfr_init0 (&mpfRes);

        // Calculate the function
        mpfr_exp  (&mpfRes, &aplVfpRht, MPFR_RNDN);
#endif
    } // End IF/ELSE

    return mpfRes;
} // End ExpVfp_RE


//***************************************************************************
//  $PrimFnMonStarVisR
//
//  Primitive scalar function monadic Star:  V {is} fn R
//***************************************************************************

void PrimFnMonStarVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonStarVisV (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
} // End PrimFnMonStarVisR


//***************************************************************************
//  $PrimFnMonStarVisV
//
//  Primitive scalar function monadic Star:  V {is} fn V
//
//  See "http://www.jsoftware.com/jwiki/Essays/Extended%20Precision%20Functions"
//***************************************************************************

void PrimFnMonStarVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ExpVfp_RE (lpatRht->aplVfp);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnMonStarVisV


#ifdef OWN_EXPLOG
//***************************************************************************
//  $ExpVfp
//
//  Calculate the exp of a VFP where (|V) < 0.5 * log (2)
//
//  See "http://www.jsoftware.com/jwiki/Essays/Extended%20Precision%20Functions"
//***************************************************************************

APLVFP ExpVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes   = {0},      // VFP result
           mpfBase  = {0};      // ... base
    UINT   uRes;                // Loop counter

    // exp (z) = 1 + (z/!1) + ((z^2)/!2) + ((z^3)/!3) + ...

    // Initialize the result to 0
    mpfr_init_set_ui (&mpfRes, 0);

    // Initialize the base to 1
    mpfr_init_set_ui (&mpfBase, 1);

    // Loop through the # terms
    for (uRes = 0 ; uRes < nDigitsFPC; uRes++)
    {
        // Accumulate the base into the result
        mpfr_add (&mpfRes, &mpfRes, &mpfBase, MPFR_RNDN);

        // Multiply the base by z / (uRes + 1)
        mpfr_mul    (&mpfBase, &mpfBase, &aplVfpRht, MPFR_RNDN);
        mpfr_div_ui (&mpfBase, &mpfBase, uRes + 1, MPFR_RNDN);
    } // End FOR

    // We no longer need this storage
    Myf_clear (&mpfBase);

    return mpfRes;
} // End ExpVfp
#endif


//***************************************************************************
//  $PrimFnMonStarHC2FisHC2I
//
//  Primitive scalar function monadic Star:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonStarHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonStarHC2FisHC2F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonStarHC2FisHC2I


//***************************************************************************
//  $ExpHC2F_RE
//***************************************************************************

APLHC2F ExpHC2F_RE
    (APLHC2F aplRht)

{
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLHC2F       aplRes = {0};         // The result
    UBOOL         bCtrlBreak = FALSE;   // Temp var for Ctrl-Break

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars); // Assert (lpplLocalVars NE NULL);

    // If the ptr is NULL (called from within ParsePointNotation), ...
    if (lpplLocalVars EQ NULL)
        // Fake ptr to the Ctrl-Break flag
        lpbCtrlBreak = &bCtrlBreak;
    else
        // Get the ptr to the Ctrl-Break flag
        lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for special case:  ^-_
    if (!IsFltNegInfinity (aplRht.parts[0]))
    {
        // Handle via macro
        aplRes = PrimMonStarHCxF ((LPAPLHC8F) &aplRht, 2).partsLo[0].partsLo[0];
    } // End IF

    return aplRes;
} // End ExpHC2F_RE


//***************************************************************************
//  $PrimMonStarHCxF
//***************************************************************************

APLHC8F PrimMonStarHCxF
    (LPAPLHC8F lpaplRht,            // Ptr to right arg
     int       iHCDim)              /* HC Dimension (1, 2, 4, 8)         */

{
    APLFLOAT aplIMag,               /* Magnitude of the imaginary parts  */
             aplMul;                /* Multiplier:  exp (h) * ...        */
    int      i;                     /* Loop counter                      */
    APLHC8F  aplRes = {0};          // The result

    /*
        From http://tamivox.org/redbear/qtrn_calc/index.html
        Exp of a Hypercomplex number:
          With g = magnitude of the imaginary parts
               h = real part
        The real part is
          exp (h) * cos (g)
        The imaginary parts are
          exp (h) * sin (g) / g   times each imaginary part

        h = pF (stack[0].h);
        i = pF (stack[0].i);
        j = pF (stack[0].j);
        k = pF (stack[0].k);

        g = r_sqrt (i * i + j * j + k * k);
        m = exp (h);
        u = m*cos (g);
        v = (g == 0) ? m : m*sin (g)/g;
        N.B. The above line was changed from "? 0 :" by Bob Smith on 3 Oct 2018

        stack[0].h = u;
        stack[0].i = v * i;
        stack[0].j = v * j;
        stack[0].k = v * k;
    */

    /* Calculate the sum of the squares of the imaginary parts */
    aplIMag = 0;

    /* Loop through the imaginary parts */
    for (i = 1; i < iHCDim; i++)
        /* Calculate the sum of the squares */
        aplIMag += lpaplRht->parts[i] * lpaplRht->parts[i];

    /* If g is {Inf}, ... */
    if (IsFltInfinity (aplIMag))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    /* Calculate the magnitude of the imaginary parts */
    aplIMag = sqrt (aplIMag);

    /* Calculate the multiplier of the real part as exp (h) */
    aplMul = exp (lpaplRht->parts[0]);

    /* Calculate the real part as m * cos (g) */
    aplRes.parts[0] = aplMul * cosCT_Flt (aplIMag);

    /* If the number has imaginary parts, ... */
    if (aplIMag NE 0)
        /* Calculate the multiplier of the imaginary part as m*sin(g)/g */
        aplMul = aplMul * sinCT_Flt (aplIMag) / aplIMag;

    /* Loop through the imaginary parts */
    for (i = 1; i < iHCDim; i++)
    // If the coefficient is zero, ...
    if (lpaplRht->parts[i] EQ 0.0)
        // Initialize to 0 so as to avoid
        //   an indeterminate result of 0 x {Inf}
        aplRes.parts[i] = 0.0;
    else
        aplRes.parts[i] = aplMul * lpaplRht->parts[i];

    return aplRes;
} // End PrimMonStarHCxF


//***************************************************************************
//  $PrimFnMonStarHC2FisHC2F
//
//  Primitive scalar function monadic Star:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonStarHC2FisHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ExpHC2F_RE (lpatRht->aplHC2F);
} // End PrimFnMonStarHC2FisHC2F


//***************************************************************************
//  $PrimFnMonStarHC4FisHC4I
//
//  Primitive scalar function monadic Star:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonStarHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonStarHC4FisHC4F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonStarHC4FisHC4I


//***************************************************************************
//  $ExpHC4F_RE
//***************************************************************************

APLHC4F ExpHC4F_RE
    (APLHC4F aplRht)

{
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLHC4F       aplRes = {0};         // The result
    UBOOL         bCtrlBreak = FALSE;   // Temp var for Ctrl-Break

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars); // Assert (lpplLocalVars NE NULL);

    // If the ptr is NULL (called from within ParsePointNotation), ...
    if (lpplLocalVars EQ NULL)
        // Fake ptr to the Ctrl-Break flag
        lpbCtrlBreak = &bCtrlBreak;
    else
        // Get the ptr to the Ctrl-Break flag
        lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for special case:  ^-_
    if (!IsFltNegInfinity (aplRht.parts[0]))
    {
        // Handle via macro
        aplRes = PrimMonStarHCxF ((LPAPLHC8F) &aplRht, 4).partsLo[0];
    } // End IF

    return aplRes;
} // End ExpHC4F_RE


//***************************************************************************
//  $PrimFnMonStarHC4FisHC4F
//
//  Primitive scalar function monadic Star:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonStarHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ExpHC4F_RE (lpatRht->aplHC4F);
} // End PrimFnMonStarHC4FisHC4F


//***************************************************************************
//  $PrimFnMonStarHC8FisHC8I
//
//  Primitive scalar function monadic Star:  HC8F {is} fn HC8I
//***************************************************************************

void PrimFnMonStarHC8FisHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonStarHC8FisHC8F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonStarHC8FisHC8I


//***************************************************************************
//  $ExpHC8F_RE
//***************************************************************************

APLHC8F ExpHC8F_RE
    (APLHC8F aplRht)

{
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLHC8F       aplRes = {0};         // The result
    UBOOL         bCtrlBreak = FALSE;   // Temp var for Ctrl-Break

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars); // Assert (lpplLocalVars NE NULL);

    // If the ptr is NULL (called from within ParsePointNotation), ...
    if (lpplLocalVars EQ NULL)
        // Fake ptr to the Ctrl-Break flag
        lpbCtrlBreak = &bCtrlBreak;
    else
        // Get the ptr to the Ctrl-Break flag
        lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for special case:  ^-_
    if (!IsFltNegInfinity (aplRht.parts[0]))
    {
        // Handle via macro
        aplRes = PrimMonStarHCxF ((LPAPLHC8F) &aplRht, 8);
    } // End IF

    return aplRes;
} // End ExpHC8F_RE


//***************************************************************************
//  $PrimFnMonStarHC8FisHC8F
//
//  Primitive scalar function monadic Star:  HC8F {is} fn HC8F
//***************************************************************************

void PrimFnMonStarHC8FisHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ExpHC8F_RE (lpatRht->aplHC8F);
} // End PrimFnMonStarHC8FisHC8F


//***************************************************************************
//  $ExpHC2V_RE
//***************************************************************************

APLHC2V ExpHC2V_RE
    (APLHC2V aplRht)

{
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLHC2V       aplRes = {0};         // The result
    UBOOL         bCtrlBreak = FALSE;   // Temp var for Ctrl-Break

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars); // Assert (lpplLocalVars NE NULL);

    // If the ptr is NULL (called from within ParsePointNotation), ...
    if (lpplLocalVars EQ NULL)
        // Fake ptr to the Ctrl-Break flag
        lpbCtrlBreak = &bCtrlBreak;
    else
        // Get the ptr to the Ctrl-Break flag
        lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for special case:  ^-_
    if (IsMpfNegInfinity (&aplRht.parts[0]))
        // Initialize the result to 0
        mphc2v_init0 (&aplRes);
    else
    {
        // Handle via macro
        aplRes = PrimMonStarHCxV ((LPAPLHC8V) &aplRht, 2).partsLo.partsLo;
    } // End IF

    return aplRes;
} // End ExpHC2V_RE


//***************************************************************************
//  $PrimMonStarHCxV
//***************************************************************************

APLHC8V PrimMonStarHCxV
    (LPAPLHC8V lpaplRht,
     int       iHCDim)

{
    APLVFP  aplIMag = {0},          /* Magnitude of the imaginary parts  */
            aplMul,                 /* Multiplier:  exp (h) * ...        */
            aplTmp;                 /* Temp var                          */
    int     i;                      /* Loop counter                      */
    APLHC8V aplRes = {0};           // The result

    /*
        From http://tamivox.org/redbear/qtrn_calc/index.html
        Exp of a Hypercomplex number:
          With g = magnitude of the imaginary parts
               h = real part
        The real part is
          exp (h) * cos (g)
        The imaginary parts are
          exp (h) * sin (g) / g   times each imaginary part

        h = pF (stack[0].h);
        i = pF (stack[0].i);
        j = pF (stack[0].j);
        k = pF (stack[0].k);

        g = r_sqrt (i * i + j * j + k * k);
        m = exp (h);
        u = m*cos (g);
        v = (g == 0) ? m : m*sin (g)/g;
        N.B. The above line was changed from "? 0 :" by Bob Smith on 3 Oct 2018

        stack[0].h = u;
        stack[0].i = v * i;
        stack[0].j = v * j;
        stack[0].k = v * k;
    */

    /* Calculate the sum of the squares of the imaginary parts */
    mpfr_init0 (&aplIMag);

    /* Loop through the imaginary parts */
    for (i = 1; i < iHCDim; i++)
    {
        /* Calculate the square of the imaginary part */
        aplTmp = MulHC1V_RE (lpaplRht->parts[i], lpaplRht->parts[i]);

        /* Calculate the sum of the squares */
        mpfr_add (&aplIMag, &aplIMag, &aplTmp, MPFR_RNDN);

        /* We no longer need this resource */
        Myf_clear (&aplTmp);
    } /* End IF */

    /* If g is {Inf}, ... */
    if (IsMpfInfinity (&aplIMag))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    /* Calculate the magnitude of the imaginary parts */
    mpfr_sqrt (&aplIMag, &aplIMag, MPFR_RNDN);

    /* Calculate the multiplier of the real part as exp (h) */
    aplMul = ExpVfp_RE (lpaplRht->parts[0]);

    /* Calculate the cos (g) */
    aplTmp = cosCT_Vfp (aplIMag);

    /* Calculate the real part as m * cos (g) */
    aplRes.parts[0] = MulHC1V_RE (aplMul, aplTmp);

    /* We no longer need this resource */
    Myf_clear (&aplTmp);

    /* If the number has imaginary parts, ... */
    if (!IsMpf0 (&aplIMag))
    {
        APLVFP aplSin,
               aplDiv;

        /* Calculate the multiplier of the imaginary part as m * sin (g) / g  */

        /* Calculate the sin (g) */
        aplSin = sinCT_Vfp (aplIMag);

        /* Calculate sin (g) / g */
        aplDiv = DivHC1V_RE (aplSin, aplIMag);

        /* Calculate m * sin (g) / g */
        aplTmp = MulHC1V_RE (aplMul, aplDiv);

        /* Loop through the imaginary parts */
        for (i = 1; i < iHCDim; i++)
            /* Multiply each imaginary part by  m * sin (g) /g  */
            aplRes.parts[i] = MulHC1V_RE (aplTmp, lpaplRht->parts[i]);

        /* We no longer need these resources */
        Myf_clear (&aplTmp);
        Myf_clear (&aplDiv);
        Myf_clear (&aplSin);
    } else
    {
        /* Multiply the imaginary parts by m (= aplMul) */

        /* Loop through the imaginary parts */
        for (i = 1; i < iHCDim; i++)
        // If the coefficient is zero, ...
        if (IsMpf0 (&lpaplRht->parts[i]))
            // Initialize to 0 so as to avoid
            //   an indeterminate result of 0 x {Inf}
            mpfr_init0 (&aplRes.parts[i]);
        else
            /* Multiply by aplMul */
            aplRes.parts[i] = MulHC1V_RE (lpaplRht->parts[i], aplMul);
    } /* End IF/ELSE */

    /* We no longer need these resources */
    Myf_clear (&aplMul);
    Myf_clear (&aplIMag);

    return aplRes;
} // End PrimMonStarHCxV


//***************************************************************************
//  $PrimFnMonStarHC2VisHC2R
//
//  Primitive scalar function monadic Star:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonStarHC2VisHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonStarHC2VisHC2V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
} // End PrimFnMonStarHC2VisHC2R


//***************************************************************************
//  $PrimFnMonStarHC2VisHC2V
//
//  Primitive scalar function monadic Star:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonStarHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ExpHC2V_RE (lpatRht->aplHC2V);
} // End PrimFnMonStarHC2VisHC2V


//***************************************************************************
//  $ExpHC4V_RE
//***************************************************************************

APLHC4V ExpHC4V_RE
    (APLHC4V aplRht)

{
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLHC4V       aplRes = {0};         // The result
    UBOOL         bCtrlBreak = FALSE;   // Temp var for Ctrl-Break

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars); // Assert (lpplLocalVars NE NULL);

    // If the ptr is NULL (called from within ParsePointNotation), ...
    if (lpplLocalVars EQ NULL)
        // Fake ptr to the Ctrl-Break flag
        lpbCtrlBreak = &bCtrlBreak;
    else
        // Get the ptr to the Ctrl-Break flag
        lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for special case:  ^-_
    if (IsMpfNegInfinity (&aplRht.parts[0]))
        // Initialize the result to 0
        mphc4v_init0 (&aplRes);
    else
    {
        // Handle via macro
        aplRes = PrimMonStarHCxV ((LPAPLHC8V) &aplRht, 4).partsLo;
    } // End IF/ELSE

    return aplRes;
} // End ExpHC4V_RE


//***************************************************************************
//  $PrimFnMonStarHC4VisHC4R
//
//  Primitive scalar function monadic Star:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonStarHC4VisHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonStarHC4VisHC4V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
} // End PrimFnMonStarHC4VisHC4R


//***************************************************************************
//  $PrimFnMonStarHC4VisHC4V
//
//  Primitive scalar function monadic Star:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonStarHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ExpHC4V_RE (lpatRht->aplHC4V);
} // End PrimFnMonStarHC4VisHC4V


//***************************************************************************
//  $ExpHC8V_RE
//***************************************************************************

APLHC8V ExpHC8V_RE
    (APLHC8V aplRht)

{
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLHC8V       aplRes = {0};         // The result
    UBOOL         bCtrlBreak = FALSE;   // Temp var for Ctrl-Break

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars); // Assert (lpplLocalVars NE NULL);

    // If the ptr is NULL (called from within ParsePointNotation), ...
    if (lpplLocalVars EQ NULL)
        // Fake ptr to the Ctrl-Break flag
        lpbCtrlBreak = &bCtrlBreak;
    else
        // Get the ptr to the Ctrl-Break flag
        lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for special case:  ^-_
    if (IsMpfNegInfinity (&aplRht.parts[0]))
        // Initialize the result to 0
        mphc8v_init0 (&aplRes);
    else
    {
        // Handle via macro
        aplRes = PrimMonStarHCxV ((LPAPLHC8V) &aplRht, 8);
    } // End IF/ELSE

    return aplRes;
} // End ExpHC8V_RE


//***************************************************************************
//  $PrimFnMonStarHC8VisHC8R
//
//  Primitive scalar function monadic Star:  HC8V {is} fn HC8R
//***************************************************************************

void PrimFnMonStarHC8VisHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonStarHC8VisHC8V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc8v_clear (&atRht.aplHC8V);
} // End PrimFnMonStarHC8VisHC8R


//***************************************************************************
//  $PrimFnMonStarHC8VisHC8V
//
//  Primitive scalar function monadic Star:  HC8V {is} fn HC8V
//***************************************************************************

void PrimFnMonStarHC8VisHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ExpHC8V_RE (lpatRht->aplHC8V);
} // End PrimFnMonStarHC8VisHC8V


//***************************************************************************
//  $PrimSpecStarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStarStorageTypeDyd
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

    // Split cases based upon the storage type
    switch (aplTypeRes)
    {
        // Except all simple numerics become FLOAT
        case ARRAY_BOOL:
        case ARRAY_APA:
            aplTypeRes = ARRAY_FLOAT;

            break;

////////// Except that INT -> FLT, RAT -> VFP, etc.
////////case ARRAY_INT:
////////case ARRAY_HC2I:
////////case ARRAY_HC4I:
////////case ARRAY_HC8I:
////////case ARRAY_RAT:
////////case ARRAY_HC2R:
////////case ARRAY_HC4R:
////////case ARRAY_HC8R:
////////    aplTypeRes++;               // Assuming order as in <ARRAY_TYPES>
////////
////////    break;

        case ARRAY_INT:                 // We test for integer exponent and return the same type
        case ARRAY_HC2I:                // ...
        case ARRAY_HC4I:                // ...
        case ARRAY_HC8I:                // ...

        case ARRAY_RAT:                 // ...
        case ARRAY_HC2R:                // ...
        case ARRAY_HC4R:                // ...
        case ARRAY_HC8R:                // ...

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
} // End PrimSpecStarStorageTypeDyd


//***************************************************************************
//  $PrimFnDydStarBisBvB
//
//  Primitive scalar function dyadic Star:  B {is} B fn B
//***************************************************************************

void PrimFnDydStarBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result*?0i1*?0i1
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLFLOAT aplFloatRes;
    UBOOL    bRet;

    // Check for indeterminates:  0 * 0
    if (lpatLft->aplBoolean EQ 0
     && lpatRht->aplBoolean EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) lpatLft->aplBoolean,
                                           ICNDX_0EXP0,
                                (APLFLOAT) lpatRht->aplBoolean,
                                           FALSE);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            bRet = 0;
        else
        if (aplFloatRes EQ 1)
            bRet = 1;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } else
      bRet = (lpatLft->aplBoolean >= lpatRht->aplBoolean);

    lpMemRes[uRes >> LOG2NBIB] |=
      bRet
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydStarBisBvB


//***************************************************************************
//  $PrimFnDydStarB64isB64vB64
//
//  Primitive scalar function dyadic Star:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydStarB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) aplBooleanLft,
                                           ICNDX_0EXP0,
                                (APLFLOAT) aplBooleanRht,
                                           FALSE);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0x0000000000000000;
        else
        if (aplFloatRes EQ 1)
            return 0xFFFFFFFFFFFFFFFF;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydStarB64isB64vB64


//***************************************************************************
//  $PrimFnDydStarB32isB32vB32
//
//  Primitive scalar function dyadic Star:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydStarB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) aplBooleanLft,
                                           ICNDX_0EXP0,
                                (APLFLOAT) aplBooleanRht,
                                           FALSE);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0x00000000;
        else
        if (aplFloatRes EQ 1)
            return 0xFFFFFFFF;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydStarB32isB32vB32


//***************************************************************************
//  $PrimFnDydStarB16isB16vB16
//
//  Primitive scalar function dyadic Star:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydStarB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) aplBooleanLft,
                                           ICNDX_0EXP0,
                                (APLFLOAT) aplBooleanRht,
                                           FALSE);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0x0000;
        else
        if (aplFloatRes EQ 1)
            return 0xFFFF;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydStarB16isB16vB16


//***************************************************************************
//  $PrimFnDydStarB08isB08vB08
//
//  Primitive scalar function dyadic Star:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydStarB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLFLOAT aplFloatRes;

    // Check for indeterminates:  0 * 0
    if (aplBooleanLft EQ 0
     && aplBooleanRht EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) aplBooleanLft,
                                           ICNDX_0EXP0,
                                (APLFLOAT) aplBooleanRht,
                                           FALSE);
        // If the result is Boolean
        if (aplFloatRes EQ 0)
            return 0x00;
        else
        if (aplFloatRes EQ 1)
            return 0xFF;
        else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    return ~((~aplBooleanLft) & aplBooleanRht);
} // End PrimFnDydStarB08isB08vB08


//***************************************************************************
//  $PrimFnDydStarIisIvI
//
//  Primitive scalar function dyadic Star:  I {is} I fn I
//***************************************************************************

void PrimFnDydStarIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  0 * 0
    if (lpatLft->aplInteger EQ 0
     && lpatRht->aplInteger EQ 0)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    else
    // Check for special cases:  (|1) * R
    if (abs64 (lpatLft->aplInteger) EQ 1)
    {
        // If the right arg is odd, lpMemRes[uRes] = the left arg (1 or -1)
        if (abs64 (lpatRht->aplInteger) % 2)
            lpMemRes[uRes] = lpatLft->aplInteger;
        else
            // Otherwise lpMemRes[uRes] = 1.
            lpMemRes[uRes] = 1;
    } else
    // Check for special cases:  L * R for R < 0 or R > 62
    if (lpatRht->aplInteger < 0
     || lpatRht->aplInteger > 62)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    else
    {
        // Initialize with identity element for multiplication
        lpMemRes[uRes] = 1;
        while (lpatRht->aplInteger--)
            lpMemRes[uRes] = imul64_RE (lpMemRes[uRes], lpatLft->aplInteger, FLOAT);
    } // End IF/ELSE/...
} // End PrimFnDydStarIisIvI


//***************************************************************************
//  $PrimFnDydStarFisIvI
//
//  Primitive scalar function dyadic Star:  F {is} I fn I
//***************************************************************************

void PrimFnDydStarFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  0 * 0
    if (lpatLft->aplInteger EQ 0
     && lpatRht->aplInteger EQ 0)
        lpMemRes[uRes] = TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                                          ICNDX_0EXP0,
                                               (APLFLOAT) lpatRht->aplInteger,
                                                          FALSE);
    else
        lpMemRes[uRes] = pow ((APLFLOAT) lpatLft->aplInteger, (APLFLOAT) lpatRht->aplInteger);
} // End PrimFnDydStarFisIvI


//***************************************************************************
//  $PowHC1F_RE
//***************************************************************************

APLHC1F PowHC1F_RE
    (APLHC1F aplLft,                // Left arg
     APLHC1F aplRht)                // Right ...

{
    APLHC1F aplRes = {0};           // The result

    // Check for indeterminates:  0 * 0
    if (aplLft EQ 0
     && aplRht EQ 0)
    {
        aplRes = TranslateQuadICIndex (aplLft,
                                       ICNDX_0EXP0,
                                       aplRht,
                                       FALSE);
    } else
    // Check for indeterminates:  0 * +_
    if (aplLft EQ 0
     && IsFltPosInfinity (aplRht))
    {
        aplRes = TranslateQuadICIndex (aplLft,
                                       ICNDX_0EXPPi,
                                       aplRht,
                                       FALSE);
    } else
    // Check for indeterminates:  0 * -_
    if (aplLft EQ 0
     && IsFltNegInfinity (aplRht))
    {
        aplRes = TranslateQuadICIndex (aplLft,
                                       ICNDX_0EXPNi,
                                       aplRht,
                                       FALSE);
    } else
    // Check for indeterminates:  L * _ for L <= -1
    if (aplLft <= -1
     && IsFltPosInfinity (aplRht))
    {
        aplRes = TranslateQuadICIndex (aplLft,
                                       ICNDX_NEXPPi,
                                       aplRht,
                                       FALSE);
    } else
    // Check for indeterminates:  L * -_ for -1 <= L < 0
    if (IsFltNegInfinity (aplRht)
     && aplLft >= -1
     && aplLft <   0)
    {
        aplRes = TranslateQuadICIndex (aplLft,
                                       ICNDX_N1to0EXPNi,
                                       aplRht,
                                       FALSE);
    } else
////// Check for indeterminates:  L * R for L < 0 and R not an integer
////if (aplLft < 0
//// && aplRht NE floor (aplRht))
////////aplRes = TranslateQuadICIndex (aplLft,
////                                   ICNDX_NegEXPFrc,
////                                   aplRht,
////                                   FALSE);
    // Check for complex result
    if (SIGN_APLFLOAT (aplLft)
     && aplRht NE floor (aplRht))
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
    // Check for special cases:  _ * 0 and -_ * 0
    if (IsFltInfinity (aplLft)
     && aplRht EQ 0)
    {
        aplRes = TranslateQuadICIndex (aplLft,
                                       ICNDX_InfEXP0,
                                       aplRht,
                                       FALSE);
    } else
    // Check for special cases:  1 * _ and 1 * -_
    if (aplLft EQ 1
     && IsFltInfinity (aplRht))
        aplRes = aplLft;
    else
        // Calculate the power
        aplRes = pow (aplLft, aplRht);

    return aplRes;
} // End PowHC1F


//***************************************************************************
//  $PrimFnDydStarFisFvF
//
//  Primitive scalar function dyadic Star:  F {is} F fn F
//***************************************************************************

void PrimFnDydStarFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC1F_RE (lpatLft->aplHC1F, lpatRht->aplHC1F);
} // End PrimFnDydStarFisFvF


//***************************************************************************
//  $PowHC1R_RE
//***************************************************************************

APLHC1R PowHC1R_RE
    (APLHC1R aplLft,                // Left arg
     APLHC1R aplRht)                // Right ...

{
    APLHC1R aplRes = {0};           // The result
    mpir_ui uRht;

    // Check for indeterminates:  0 * 0
    if (IsMpq0 (&aplLft)
     && IsMpq0 (&aplRht))
        mpq_QuadICValue (&aplLft,
                          ICNDX_0EXP0,
                         &aplRht,
                         &aplRes,
                          FALSE);
    else
    // Check for indeterminates:  0 * +_
    if (IsMpq0 (&aplLft)
     && mpq_cmp (&aplRht, &mpqPosInfinity) EQ 0)
        mpq_QuadICValue (&aplLft,
                          ICNDX_0EXPPi,
                         &aplRht,
                         &aplRes,
                          FALSE);
    else
    // Check for indeterminates:  0 * -_
    if (IsMpq0 (&aplLft)
     && mpq_cmp (&aplRht, &mpqNegInfinity) EQ 0)
        mpq_QuadICValue (&aplLft,
                          ICNDX_0EXPNi,
                         &aplRht,
                         &aplRes,
                          FALSE);
    else
    // Check for indeterminates:  L * _ for L <= -1
    if (mpq_cmp_si (&aplLft, -1, 1) <= 0
     && mpq_cmp (&aplRht, &mpqPosInfinity) EQ 0)
        mpq_QuadICValue (&aplLft,
                          ICNDX_NEXPPi,
                         &aplRht,
                         &aplRes,
                          FALSE);
    else
    // Check for indeterminates:  L * -_ for -1 <= L < 0
    if (mpq_cmp    (&aplRht, &mpqNegInfinity) EQ 0
     && mpq_cmp_si (&aplLft, -1, 1) >= 0
     && mpq_cmp_si (&aplLft,  0, 1) <  0)
        mpq_QuadICValue (&aplLft,
                          ICNDX_N1to0EXPNi,
                         &aplRht,
                         &aplRes,
                          FALSE);
    else
////// Check for indeterminates:  L * R for L < 0 and R not an integer
////if (SIGN_APLRAT (&aplLft)
//// && !mpq_integer_p (&aplRht))
////    mpq_QuadICValue (&aplLft,
////                      ICNDX_NegEXPFrc,
////                     &aplRht,
////                     &aplRes,
////                      FALSE);
////else
    // Check for complex result
    if (SIGN_APLRAT (&aplLft)
     && !mpq_integer_p (&aplRht))
        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    else
    // Check for special cases:  _ * 0 and -_ * 0
    if (mpq_inf_p (&aplLft)
     && IsMpq0 (&aplRht))
        mpq_QuadICValue (&aplLft,
                          ICNDX_InfEXP0,
                         &aplRht,
                         &aplRes,
                          FALSE);
    else
    // Check for special cases:  1 * _ and 1 * -_
    if (IsMpq1 (&aplLft)
     && mpq_inf_p (&aplRht))
        mpq_init_set (&aplRes, &aplLft);
    else
    // If the exponent's denominator is 1,
    //   and the exponent's numerator fits in an UINT, ...
    if (IsMpz1 (mpq_denref (&aplRht))
     && mpz_fits_slong_p (mpq_numref (&aplRht)) NE 0)
    {
        // Initialize the base
        mpq_init_set (&aplRes, &aplLft);

        // Extract the exponent
        uRht = mpz_get_ui (mpq_numref (&aplRht));

        // Compute the powers
        mpq_pow_ui (&aplRes, &aplRes, uRht);

        // If the right arg is negative, ...
        if (SIGN_APLRAT (&aplRht))
        {
            // Check for indeterminates:  {div} 0
            if (IsMpq0 (&aplRes))
                mpq_QuadICValue (&aplRes,           // No left arg
                                  ICNDX_DIV0,
                                 &aplRes,
                                 &aplRes,
                                  FALSE);
            else
                // Invert it
                mpq_inv (&aplRes, &aplRes);
        } // End IF
    } else
    // If the base is negative, ...
    if (SIGN_APLRAT (&aplLft))
    {
        // If the denominator is even, ...
        if (mpz_even_p (mpq_denref (&aplRht)))
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
//      else
//      // If the numerator is even (and the denominator is odd), ...
//      if (mpz_even_p (mpq_numref (&aplRht)))
//          // Blowup to VFP positive:  (|L)*R
//          RaiseException (EXCEPTION_RESULT_VFPPOS, 0, 0, NULL);
//      else
//      // The numerator and denominator are both odd
//          // Blowup to VFP negative:  -(|L)*R
//          RaiseException (EXCEPTION_RESULT_VFPNEG, 0, 0, NULL);
    } else
        // Otherwise, blowup to VFP
        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);

    return aplRes;
} // End PowHC1R_RE


//***************************************************************************
//  $PrimFnDydStarRisRvR
//
//  Primitive scalar function dyadic Star:  R {is} R fn R
//***************************************************************************

void PrimFnDydStarRisRvR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC1R_RE (lpatLft->aplHC1R, lpatRht->aplHC1R);
} // End PrimFnDydStarRisRvR


//***************************************************************************
//  $PrimFnDydStarVisRvR
//
//  Primitive scalar function dyadic Star:  V {is} R fn R
//***************************************************************************

void PrimFnDydStarVisRvR
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

    PrimFnDydStarVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydStarVisRvR


//***************************************************************************
//  $PowHC1V_RE
//***************************************************************************

APLHC1V PowHC1V_RE
    (APLHC1V aplLft,                // Left arg
     APLHC1V aplRht)                // Right ...

{
    APLHC1V aplRes = {0};           // The result
    APLVFP mpfTmp = {0};

    // Check for indeterminates:  0 * 0
    if (IsMpf0 (&aplLft)
     && IsMpf0 (&aplRht))
        mpfr_QuadICValue (&aplLft,
                                   ICNDX_0EXP0,
                                  &aplRht,
                                  &aplRes,
                                   FALSE);
    else
    // Check for indeterminates:  0 * +_
    if (IsMpf0 (&aplLft)
     && mpfr_cmp (&aplRht, &mpfPosInfinity) EQ 0)
        mpfr_QuadICValue (&aplLft,
                                   ICNDX_0EXPPi,
                                  &aplRht,
                                  &aplRes,
                                   FALSE);
    else
    // Check for indeterminates:  0 * -_
    if (IsMpf0 (&aplLft)
     && mpfr_cmp (&aplRht, &mpfNegInfinity) EQ 0)
        mpfr_QuadICValue (&aplLft,
                                   ICNDX_0EXPNi,
                                  &aplRht,
                                  &aplRes,
                                   FALSE);
    else
    // Check for indeterminates:  L * _ for L <= -1
    if (mpfr_cmp_si (&aplLft, -1) <= 0
     && mpfr_cmp (&aplRht, &mpfPosInfinity) EQ 0)
        mpfr_QuadICValue (&aplLft,
                                   ICNDX_NEXPPi,
                                  &aplRht,
                                  &aplRes,
                                   FALSE);
    else
    // Check for indeterminates:  L * -_ for -1 <= L < 0
    if (mpfr_cmp    (&aplRht, &mpfNegInfinity) EQ 0
     && mpfr_cmp_si (&aplLft, -1) >= 0
     && mpfr_cmp_si (&aplLft,  0) <  0)
        mpfr_QuadICValue (&aplLft,
                                   ICNDX_N1to0EXPNi,
                                  &aplRht,
                                  &aplRes,
                                   FALSE);
    else
    // Check for indeterminates:  L * R for L < 0 and R not an integer
////if (mpfr_sgn (&aplLft) < 0
//// && !mpfr_integer_p (&aplRht))
////    mpfr_QuadICValue (&aplLft,
////                               ICNDX_NegEXPFrc,
////                              &aplRht,
////                              &aplRes,
////                               FALSE);
////else
    // Check for complex result
    if (mpfr_sgn (&aplLft) < 0
     && !mpfr_integer_p (&aplRht))
        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    else
    // Check for special cases:  _ * 0 and -_ * 0
    if (mpfr_inf_p (&aplLft)
     && IsMpf0 (&aplRht))
        mpfr_QuadICValue (&aplLft,
                                   ICNDX_InfEXP0,
                                  &aplRht,
                                  &aplRes,
                                   FALSE);
    else
    // Check for special cases:  1 * _ and 1 * -_
    if (IsMpf1 (&aplLft)
     && mpfr_inf_p (&aplRht))
        mpfr_init_copy (&aplRes, &aplLft);
    else
    // If the exponent is an integer, ...
    if (mpfr_integer_p (&aplRht))
    {
        mpz_t mpzTmp = {0};

        // Initialize the result and temp to 0
        mpfr_init0  (&aplRes);
        mpz_init   ( mpzTmp);

        // Extract the exponent as MPZ
        mpfr_get_z ( mpzTmp, &aplRht, MPFR_RNDN);

        // Compute the power
        mpfr_pow_z (&aplRes, &aplLft, mpzTmp, MPFR_RNDN);

        // We no longer need this storage
        Myz_clear  ( mpzTmp);
    } else
    {
        // General exp a^z = exp (z * ln (a))

        // Calculate ln (a)
        PrimFnMonCircleStarVisV (&mpfTmp, 0, (LPALLTYPES) &aplLft, lpPrimSpec);

        // Multiply by the exponent
        mpfr_mul (&mpfTmp, &mpfTmp, &aplRht, MPFR_RNDN);

        // Calculate the exp of that
        aplRes = ExpVfp_RE (mpfTmp);

        // We no longer need this storage
        Myf_clear (&mpfTmp);
    } // End IF/ELSE

    return aplRes;
} // End PowHC1V_RE


//***************************************************************************
//  $PrimFnDydStarVisVvV
//
//  Primitive scalar function dyadic Star:  V {is} V fn V
//***************************************************************************

void PrimFnDydStarVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC1V_RE (lpatLft->aplHC1V, lpatRht->aplHC1V);
} // End PrimFnDydStarVisVvV


//***************************************************************************
//  $PowHC2I_RE
//***************************************************************************

APLHC2I PowHC2I_RE
    (APLHC2I aplLft,                // Left arg
     APLHC2I aplRht)                // Right ...

{
    APLHC2I aplRes = {0};           // The result
    int     i;                      // Loop counter

    // Check for indeterminates:  0 * 0
    if (IsZeroHCxI (&aplLft, 2)
     && IsZeroHCxI (&aplRht, 2))
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
    // Check for special cases:  0 * Imag
    if (IsZeroHCxI (&aplLft, 2)
     && IzitImaginary (ARRAY_HC2I, &aplRht))
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
    // Check for special cases:  (|1) * R
    if (abs64 (aplLft.parts[0]) EQ 1
     &&        !IzitImaginary (ARRAY_HC2I, &aplLft))
    {
        // If the right arg is odd, aplRes = the left arg (1 or -1)
        if (abs64 (aplRht.parts[0]) % 2
         &&        !IzitImaginary (ARRAY_HC2I, &aplRht))
            aplRes = aplLft;
        else
        {
            // Otherwise aplRes = 1.
            aplRes.parts[0] = 1;

            // Loop through the imaginary parts
            for (i = 1; i < 2; i++)
                // Initialize to 0
                aplRes.parts[i] = 0;
        } // End IF/ELSE
    } else
    // Check for special cases:  L * R for R < 0 or R > 62
    if (aplRht.parts[0] < 0
     || aplRht.parts[0] > 62
     || IzitImaginary (ARRAY_HC2I, &aplRht))
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
    {
        APLINT aplExp;                  // Temp for Exponent

        // Initialize with identity element for multiplication
        aplRes.parts[0] = 1;

        // Loop through the imaginary parts
        for (i = 1; i < 2; i++)
            // Initialize to 0
            aplRes.parts[i] = 0;

        // Copy the exponent as an integer
        aplExp = aplRht.parts[0];

        __try
        {
            while (aplExp--)
                aplRes = MulHC2I_RE (aplRes, aplLft);
        } __except (CheckException (GetExceptionInformation (), WFCN))
        {
            EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

            dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

            // Check the exception code in a main function
            CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2F);
        } // End __try/__except
    } // End IF/ELSE

    return aplRes;
} // End PowHC2I_RE


//***************************************************************************
//  $PrimFnDydStarHC2IisHC2IvHC2I
//
//  Primitive scalar function dyadic Star:  HC2I {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydStarHC2IisHC2IvHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC2I_RE (lpatLft->aplHC2I, lpatRht->aplHC2I);
} // End PrimFnDydStarHC2IisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydStarHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic Star:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydStarHC2FisHC2IvHC2I
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
    PrimFnDydStarHC2FisHC2FvHC2F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydStarHC2FisHC2IvHC2I


//***************************************************************************
//  $PowHC2F_RE
//***************************************************************************

APLHC2F PowHC2F_RE
    (APLHC2F aplLft,                // Left arg
     APLHC2F aplRht)                // Right ...

{
    APLHC2F aplRes = {0};           // The result
    UBOOL   bImagLft = IzitImaginary (ARRAY_HC2F, &aplLft),
            bImagRht = IzitImaginary (ARRAY_HC2F, &aplRht);

    // If the left & right args are real, ...
    if (!bImagLft
     && !bImagRht)
    {
        // Check for indeterminates:  0 * 0
        if (aplLft.parts[0] EQ 0
         && aplRht.parts[0] EQ 0)
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_0EXP0,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  0 * +_
        if (aplLft.parts[0] EQ 0
         && IsFltPosInfinity (aplRht.parts[0]))
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_0EXPPi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  0 * -_
        if (aplLft.parts[0] EQ 0
         && IsFltNegInfinity (aplRht.parts[0]))
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_0EXPNi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  L * _ for L <= -1
        if (aplLft.parts[0] <= -1
         && IsFltPosInfinity (aplRht.parts[0]))
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_NEXPPi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  L * -_ for -1 <= L < 0
        if (IsFltNegInfinity (aplRht.parts[0])
         && aplLft.parts[0] >= -1
         && aplLft.parts[0] <   0)
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_N1to0EXPNi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
////////// Check for indeterminates:  L * R for L < 0 and R not an integer
////////if (aplLft.parts[0] < 0 && !IzitImaginary (ARRAY_HC2F
//////// && aplRht.parts[0] NE floor (aplRht.parts[0]))
////////{
////////////aplRes.parts[0] =
////////      TranslateQuadICIndex (aplLft.parts[0],
////////                            ICNDX_NegEXPFrc,
////////                            aplRht.parts[0],
////////                            FALSE);
////////} else
        // Check for special cases:  _ * 0 and -_ * 0
        if (IsFltInfinity (aplLft.parts[0])
         && aplRht.parts[0] EQ 0)
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_InfEXP0,
                                    aplRht.parts[0],
                                    FALSE);
        else
        // Check for special cases:  1 * _ and 1 * -_
        if (aplLft.parts[0] EQ 1
         && IsFltInfinity (aplRht.parts[0]))
            aplRes.parts[0] = aplLft.parts[0];
        else
            // Calculate the HC power
            aplRes = PowHC2F_SUB (aplLft, aplRht);
    } else
        // Calculate the HC power
        aplRes = PowHC2F_SUB (aplLft, aplRht);

    return aplRes;
} // End PowHC2F_RE


//***************************************************************************
//  $PowHC2F_SUB
//***************************************************************************

APLHC2F PowHC2F_SUB
    (APLHC2F aplLft,                // Left arg
     APLHC2F aplRht)                // Right ...

{
    APLHC2F aplRes = {0};           // The result
    APLINT  aplExp;                 // Temp for Exponent
    UBOOL   bRet;                   // TRUE iff the result is valid
    int     i;                      // Loop counter

    // Check for special cases:  0 * Imag
    if (IsZeroHCxF (&aplLft, 2)
     && IzitImaginary (ARRAY_HC2F, &aplRht)
     && gAllowNaN)
    {
        // The real part is NaN
        aplRes.parts[0] = fltNaN;

        // Loop through the imaginary parts
        for (i = 1; i < 2; i++)
        // If that coefficient is non-zero, ...
        if (aplRht.parts[i] NE 0.0)
            // Set the corresponding coefficient in the result to NaN
            aplRes.parts[i] = fltNaN;

        return aplRes;
    } // End IF

    // Get the real part of the exponent
    aplExp = ConvertToInteger_CT (ARRAY_HC2F, &aplRht, 0, 0.0, &bRet);

    // If the right arg is an integer, ...
    if (bRet
     && aplExp >= 0)
    {
        // Initialize with identity element for multiplication
        aplRes.parts[0] = 1.0;

        for (i = 1; i < 2; i++)
            aplRes.parts[i] = 0.0;

        while (aplExp--)    // ***OPTIMIZEME*** -- Use binary exponentiation
            aplRes = MulHC2F_RE (aplRes, aplLft);

        return aplRes;
    } else
        // L * R  is  * (R x ln (L))
        return ExpHC2F_RE (MulHC2F_RE (aplRht, LogHCxF_RE (*(LPAPLHC8F) &aplLft, 2).partsLo[0].partsLo[0]));
} // End PowHC2F_SUB


//***************************************************************************
//  $PrimFnDydStarHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic Star:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydStarHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfuntion
    lpMemRes[uRes] = PowHC2F_RE (lpatLft->aplHC2F, lpatRht->aplHC2F);
} // End PrimFnDydStarHC2FisHC2FvHC2F


//***************************************************************************
//  $PowHC2R_RE
//***************************************************************************

APLHC2R PowHC2R_RE
    (APLHC2R aplLft,                // Left arg
     APLHC2R aplRht)                // Right ...

{
    APLHC2R aplRes = {0};
    UBOOL   bImagLft = IzitImaginary (ARRAY_HC2F, &aplLft),
            bImagRht = IzitImaginary (ARRAY_HC2F, &aplRht);

    // If the left & right args are real, ...
    if (!bImagLft
     && !bImagRht)
    {
        // Initialize to 0/1
        mphc2r_init (&aplRes);

        // Check for indeterminates:  0 * 0
        if (IsZeroHCxR (&aplLft, 2)
         && IsZeroHCxR (&aplRht, 2))
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_0EXP0,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  0 * +_
        if (IsZeroHCxR (&aplLft, 2)
         && mpq_cmp (&aplRht.parts[0], &mpqPosInfinity) EQ 0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_0EXPPi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  0 * -_
        if (IsZeroHCxR (&aplLft, 2)
         && mpq_cmp (&aplRht.parts[0], &mpqNegInfinity) EQ 0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_0EXPNi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  L * _ for L <= -1
        if (mpq_cmp_si (&aplLft.parts[0], -1, 1) <= 0
         && mpq_cmp (&aplRht.parts[0], &mpqPosInfinity) EQ 0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_NEXPPi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  L * -_ for -1 <= L < 0
        if (mpq_cmp    (&aplRht.parts[0], &mpqNegInfinity) EQ 0
         && mpq_cmp_si (&aplLft.parts[0], -1, 1) >= 0
         && mpq_cmp_si (&aplLft.parts[0],  0, 1) <  0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_N1to0EXPNi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
////////// Check for indeterminates:  L * R for L < 0 and R not an integer
////////if (SIGN_APLRAT (&aplLft.parts[0])
//////// && !mpq_integer_p (&aplRht.parts[0]))
////////{
////////    mpq_QuadICValue (&aplLft.parts[0],
////////                      ICNDX_NegEXPFrc,
////////                     &aplRht.parts[0],
////////                     &aplRes.parts[0],
////////                      FALSE);
////////} // End IF
////////else
        // Check for special cases:  _ * 0 and -_ * 0
        if (mpq_inf_p (&aplLft.parts[0])
         && IsZeroHCxR (&aplRht, 2))
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_InfEXP0,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for special cases:  1 * _ and 1 * -_
        if (IsMpq1 (&aplLft.parts[0])
         && mpq_inf_p (&aplRht.parts[0]))
            mpq_init_set (&aplRes.parts[0], &aplLft.parts[0]);
        else
        // Check for complex result
        if (SIGN_APLRAT (&aplLft.parts[0])
         && !mpq_integer_p (&aplRht.parts[0]))
        {
            // Avoid memory leak
            Myhc2r_clear (&aplRes);

            RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
        } else
        {
            // Avoid memory leak
            Myhc2r_clear (&aplRes);

            // Calculate the HC power
            aplRes = PowHC2R_SUB (aplLft, aplRht);
        } // End IF/ELSE/...
    } else
        // Calculate the HC power
        aplRes = PowHC2R_SUB (aplLft, aplRht);

    return aplRes;
} // End PowHC2R_RE


//***************************************************************************
//  $PowHC2R_SUB
//***************************************************************************

APLHC2R PowHC2R_SUB
    (APLHC2R aplLft,                    // Left arg
     APLHC2R aplRht)                    // Right ...

{
    APLHC2R aplRes = {0};
    UBOOL   bRet = FALSE;               // TRUE iff the exponent is an intger
    APLINT  aplExp;                     // Temp for Exponent

    // Get the real part of the exponent
    aplExp = ConvertToInteger_CT (ARRAY_HC2R, &aplRht, 0, 0.0, &bRet);

    // If the exponent is an integer (and thus not imaginary), ...
    if (bRet)
    {
        APLHC2R aplTmp;
        APLINT  aplAbs;                     // Temp for abs64 (aplExp)

        // Initialize to 0/1
        mphc2r_init (&aplRes);

        // Initialize with identity element for multiplication
        mpq_set_si (&aplRes.parts[0], 1, 1);

        // Calculate the absolute value of aplExp
        aplAbs = abs64 (aplExp);

        // Compute the powers
        while (aplAbs--)                        // ***OPTIMIZEME*** -- Use binary exponentiation
        {
            // Accumulate the product
            aplTmp = MulHC2R_RE (aplRes, aplLft);

            // Save in the result
            mphc2r_set (&aplRes, &aplTmp);

            // We no longer need this resource
            Myhc2r_clear (&aplTmp);
        } // End WHILE

        // If the exponent is negative, ...
        if (aplExp < 0)
        {
            // Invert the result
            aplTmp = InvHC2R_RE (aplRes);

            // We no longer need this resource
            Myhc2r_clear (&aplRes);

            aplRes = aplTmp;
        } // End IF
//  } else
//  // If the base is negative, ...
//  if (SIGN_APLRAT (&aplLft.parts[0]))
//  {
//      // If the denominator is even, ...
//      if (mpz_even_p (mpq_denref (&aplRht.parts[0])))
//      {
//          // Avoid memory leak
//          Myhc2r_clear (&aplRes);
//
//          RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
//      } // End IF
//      else
//      // If the numerator is even (and the denominator is odd), ...
//      if (mpz_even_p (mpq_numref (&aplRht.parts[0])))
//          // Blowup to VFP positive:  (|L)*R
//          RaiseException (EXCEPTION_RESULT_VFPPOS, 0, 0, NULL);
//      else
//      // The numerator and denominator are both odd
//          // Blowup to VFP negative:  -(|L)*R
//          RaiseException (EXCEPTION_RESULT_VFPNEG, 0, 0, NULL);
    } else
        // Otherwise, blowup to HC2V
        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

    return aplRes;
} // End PowHC2R_SUB


//***************************************************************************
//  $PrimFnDydStarHC2RisHC2RvHC2R
//
//  Primitive scalar function dyadic Star:  HC2R {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydStarHC2RisHC2RvHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC2R_RE (lpatLft->aplHC2R, lpatRht->aplHC2R);
} // End PrimFnDydStarHC2RisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydStarHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic Star:  HC2V {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydStarHC2VisHC2RvHC2R
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
    PrimFnDydStarHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydStarHC2VisHC2RvHC2R


//***************************************************************************
//  $PowHC2V_RE
//***************************************************************************

APLHC2V PowHC2V_RE
    (APLHC2V aplLft,                // Left arg
     APLHC2V aplRht)                // Right ...

{
    APLHC2V aplRes = {0};
    UBOOL   bImagLft = IzitImaginary (ARRAY_HC2V, &aplLft),
            bImagRht = IzitImaginary (ARRAY_HC2V, &aplRht);

    // If the left & right args are real, ...
    if (!bImagLft
     && !bImagRht)
    {
        // Initialize to 0
        mphc2v_init0 (&aplRes);

        // Check for indeterminates:  0 * 0
        if (IsZeroHCxV (&aplLft, 2)
         && IsZeroHCxV (&aplRht, 2))
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_0EXP0,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  0 * +_
        if (IsZeroHCxV (&aplLft, 2)
         && mpfr_cmp (&aplRht.parts[0], &mpfPosInfinity) EQ 0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_0EXPPi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  0 * -_
        if (IsZeroHCxV (&aplLft, 2)
         && mpfr_cmp (&aplRht.parts[0], &mpfNegInfinity) EQ 0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_0EXPNi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  L * _ for L <= -1
        if (mpfr_cmp_si (&aplLft.parts[0], -1) <= 0
         && mpfr_cmp (&aplRht.parts[0], &mpfPosInfinity) EQ 0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_NEXPPi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  L * -_ for -1 <= L < 0
        if (mpfr_cmp    (&aplRht.parts[0], &mpfNegInfinity) EQ 0
         && mpfr_cmp_si (&aplLft.parts[0], -1) >= 0
         && mpfr_cmp_si (&aplLft.parts[0],  0) <  0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_N1to0EXPNi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  L * R for L < 0 and R not an integer
////////if (mpfr_sgn (&aplLft.parts[0]) < 0
//////// && !mpfr_integer_p (&aplRht.parts[0]))
////////    mpfr_QuadICValue (&aplLft.parts[0],
////////                       ICNDX_NegEXPFrc,
////////                      &aplRht.parts[0],
////////                      &aplRes.parts[0],
////////                       FALSE);
////////else
        // Check for special cases:  _ * 0 and -_ * 0
        if (mpfr_inf_p (&aplLft.parts[0])
         && IsZeroHCxV (&aplRht, 2))
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_InfEXP0,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for special cases:  1 * _ and 1 * -_
        if (IsMpf1 (&aplLft.parts[0])
         && mpfr_inf_p (&aplRht.parts[0]))
            mpfr_init_copy (&aplRes.parts[0], &aplLft.parts[0]);
        else
        {
            // Avoid memory leak
            Myhc2v_clear (&aplRes);

            // Calculate the HC power
            aplRes = PowHC2V_SUB (aplLft, aplRht);
        } // End IF/ELSE/...
    } else
        // Calculate the HC power
        aplRes = PowHC2V_SUB (aplLft, aplRht);

    return aplRes;
} // End PowHC2V_RE


//***************************************************************************
//  $PowHC2V_SUB
//***************************************************************************

APLHC2V PowHC2V_SUB
    (APLHC2V aplLft,                // Left arg
     APLHC2V aplRht)                // Right ...

{
    APLHC2V aplRes,                 // The result
            aplLog,                 // Temp var
            aplMul;                 // ...
    UBOOL   bRet = FALSE;           // TRUE iff the exponent is an intger
    APLINT  aplExp;                 // Temp for Exponent
    int     i;                      // Loop counter

    // Check for special cases:  0 * Imag
    if (IsZeroHCxV (&aplLft, 2)
     && IzitImaginary (ARRAY_HC2V, &aplRht)
     && gAllowNaN)
    {
        // Initialize to 0
        mphc2v_init0 (&aplRes);

        // The real part is NaN
        mpfr_set_nan (&aplRes.parts[0]);

        // Loop through the imaginary parts
        for (i = 1; i < 2; i++)
        // If that coefficient is non-zero, ...
        if (!IsZeroHCxV (&aplRht.parts[i], 2))
            // Set the corresponding coefficient in the result to NaN
            mpfr_set_nan (&aplRes.parts[i]);

        return aplRes;
    } // End IF

    // Get the real part of the exponent
    aplExp = ConvertToInteger_CT (ARRAY_HC2V, &aplRht, 0, 0.0, &bRet);

    // If the exponent is a non-negative integer (and thus not imaginary), ...
    if (bRet
     && aplExp >= 0)
    {
        APLHC2V aplTmp;

        // Initialize to 0
        mphc2v_init0 (&aplRes);

        // Initialize with identity element for multiplication
        mpfr_set_si (&aplRes.parts[0], 1, MPFR_RNDN);

        // Compute the powers
        while (aplExp--)    // ***OPTIMIZEME*** -- Use binary exponentiation
        {
            // Accumulate the product
            aplTmp = MulHC2V_RE (aplRes, aplLft);

            // Save in the result
            mphc2v_set (&aplRes, &aplTmp);

            // We no longer need this resource
            Myhc2v_clear (&aplTmp);
        } // End WHILE
    } else
    {
        // L * R  is  * (R x ln (L))
////////ExpHC2V_RE (MulHC2V_RE (aplRht, LogHCxF_RE (*(LPAPLHC8F) &aplLft, 2).partsLo.partsLo));

        // Calculate ln (L)
        aplLog = LogHCxV_RE (*(LPAPLHC8V) &aplLft, 2).partsLo.partsLo;

        // Multiply by R
        aplMul = MulHC2V_RE (aplRht, aplLog);

        // Calculate exp (aplMul)
        aplRes = ExpHC2V_RE (aplMul);

        // We no longer need these resources
        Myhc2v_clear (&aplMul);
        Myhc2v_clear (&aplLog);
    } // End IF/ELSE

    return aplRes;
} // End PowHC2V_SUB


//***************************************************************************
//  $PrimFnDydStarHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic Star:  HC2V {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydStarHC2VisHC2VvHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC2V_RE (lpatLft->aplHC2V, lpatRht->aplHC2V);
} // End PrimFnDydStarHC2VisHC2VvHC2V


//***************************************************************************
//  $PowHC4I_RE
//***************************************************************************

APLHC4I PowHC4I_RE
    (APLHC4I aplLft,
     APLHC4I aplRht)

{
    APLHC4I aplRes = {0};           // The result
    int     i;                      // Loop counter

    // Check for indeterminates:  0 * 0
    if (IsZeroHCxI (&aplLft, 4)
     && IsZeroHCxI (&aplRht, 4))
        RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);
    else
    // Check for special cases:  (|1) * R
    if (abs64 (aplLft.parts[0]) EQ 1
     &&        !IzitImaginary (ARRAY_HC4I, &aplLft))
    {
        // If the right arg is odd, aplRes = the left arg (1 or -1)
        if (abs64 (aplRht.parts[0]) % 2
         &&        !IzitImaginary (ARRAY_HC4I, &aplRht))
            aplRes = aplLft;
        else
        {
            // Otherwise aplRes = 1.
            aplRes.parts[0] = 1;

            // Loop through the imaginary parts
            for (i = 1; i < 4; i++)
                // Initialize to 0
                aplRes.parts[i] = 0;
        } // End IF
    } else
    // Check for special cases:  L * R for R < 0 or R > 64
    if (aplRht.parts[0] < 0
     || aplRht.parts[0] > 64
     || IzitImaginary (ARRAY_HC4I, &aplRht))
        RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);
    else
    {
        APLINT aplExp;                  // Temp for Exponent

        // Initialize with identity element for multiplication
        aplRes.parts[0] = 1;

        // Loop through the imaginary parts
        for (i = 1; i < 4; i++)
            // Initialize to 0
            aplRes.parts[i] = 0;

        // Copy the exponent as an integer
        aplExp = aplRht.parts[0];

        __try
        {
            while (aplExp--)
                aplRes = MulHC4I_RE (aplRes, aplLft);
        } __except (CheckException (GetExceptionInformation (), WFCN))
        {
            EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

            dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

            // Check the exception code in a main function
            CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4F);
        } // End __try/__except
    } // End IF/ELSE

    return aplRes;
} // End PowHC4I_RE


//***************************************************************************
//  $PrimFnDydStarHC4IisHC4IvHC4I
//
//  Primitive scalar function dyadic Star:  HC4I {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydStarHC4IisHC4IvHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC4I_RE (lpatLft->aplHC4I, lpatRht->aplHC4I);
} // End PrimFnDydStarHC4IisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydStarHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic Star:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydStarHC4FisHC4IvHC4I
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
    PrimFnDydStarHC4FisHC4FvHC4F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydStarHC4FisHC4IvHC4I


//***************************************************************************
//  $PowHC4F_RE
//***************************************************************************

APLHC4F PowHC4F_RE
    (APLHC4F aplLft,                // Left arg
     APLHC4F aplRht)                // Right ...

{
    APLHC4F aplRes = {0};           // The result
    UBOOL   bImagLft = IzitImaginary (ARRAY_HC4F, &aplLft),
            bImagRht = IzitImaginary (ARRAY_HC4F, &aplRht);

    // If the left & right args are real, ...
    if (!bImagLft
     && !bImagRht)
    {
        // Check for indeterminates:  0 * 0
        if (aplLft.parts[0] EQ 0
         && aplRht.parts[0] EQ 0)
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_0EXP0,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  0 * +_
        if (aplLft.parts[0] EQ 0
         && IsFltPosInfinity (aplRht.parts[0]))
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_0EXPPi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  0 * -_
        if (aplLft.parts[0] EQ 0
         && IsFltNegInfinity (aplRht.parts[0]))
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_0EXPNi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  L * _ for L <= -1
        if (aplLft.parts[0] <= -1
         && IsFltPosInfinity (aplRht.parts[0]))
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_NEXPPi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  L * -_ for -1 <= L < 0
        if (IsFltNegInfinity (aplRht.parts[0])
         && aplLft.parts[0] >= -1
         && aplLft.parts[0] <   0)
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_N1to0EXPNi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
////////// Check for indeterminates:  L * R for L < 0 and R not an integer
////////if (aplLft.parts[0] < 0 && !IzitImaginary (ARRAY_HC4F
//////// && aplRht.parts[0] NE floor (aplRht.parts[0]))
////////{
////////////aplRes.parts[0] =
////////      TranslateQuadICIndex (aplLft.parts[0],
////////                            ICNDX_NegEXPFrc,
////////                            aplRht.parts[0],
////////                            FALSE);
////////} else
        // Check for special cases:  _ * 0 and -_ * 0
        if (IsFltInfinity (aplLft.parts[0])
         && aplRht.parts[0] EQ 0)
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_InfEXP0,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for special cases:  1 * _ and 1 * -_
        if (aplLft.parts[0] EQ 1
         && IsFltInfinity (aplRht.parts[0]))
            aplRes.parts[0] = aplLft.parts[0];
        else
            // Calculate the power
            aplRes = PowHC4F_SUB (aplLft, aplRht);
    } else
        // Calculate the power
        aplRes = PowHC4F_SUB (aplLft, aplRht);

    return aplRes;
} // End PowHC4F_RE


//***************************************************************************
//  $PowHC4F_SUB
//
//  Power function for non-real arg(s)
//***************************************************************************

APLHC4F PowHC4F_SUB
    (APLHC4F aplLft,                // Left arg
     APLHC4F aplRht)                // Right ...

{
    APLHC4F aplRes = {0};           // The result
    APLINT  aplExp;                 // Temp for Exponent
    UBOOL   bRet;                   // TRUE iff the result is valid
    int     i;                      // Loop counter

    // Check for special cases:  0 * Imag
    if (IsZeroHCxF (&aplLft, 4)
     && IzitImaginary (ARRAY_HC4F, &aplRht)
     && gAllowNaN)
    {
        // The real part is NaN
        aplRes.parts[0] = fltNaN;

        // Loop through the imaginary parts
        for (i = 1; i < 4; i++)
        // If that coefficient is non-zero, ...
        if (aplRht.parts[i] NE 0.0)
            // Set the corresponding coefficient in the result to NaN
            aplRes.parts[i] = fltNaN;

        return aplRes;
    } // End IF

    // Get the real part of the exponent
    aplExp = ConvertToInteger_CT (ARRAY_HC4F, &aplRht, 0, 0.0, &bRet);

    // If the right arg is an integer, ...
    if (bRet
     && aplExp >= 0)
    {
        // Initialize with identity element for multiplication
        aplRes.parts[0] = 1.0;

        for (i = 1; i < 4; i++)
            aplRes.parts[i] = 0.0;

        while (aplExp--)
            aplRes = MulHC4F_RE (aplRes, aplLft);

        return aplRes;
    } else
        // L * R  is  * (R x ln (L))
        return ExpHC4F_RE (MulHC4F_RE (aplRht, LogHCxF_RE (*(LPAPLHC8F) &aplLft, 4).partsLo[0]));
} // End PowHC4F_SUB


//***************************************************************************
//  $PrimFnDydStarHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic Star:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydStarHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC4F_RE (lpatLft->aplHC4F, lpatRht->aplHC4F);
} // End PrimFnDydStarHC4FisHC4FvHC4F


//***************************************************************************
//  $PowHC4R_RE
//***************************************************************************

APLHC4R PowHC4R_RE
    (APLHC4R aplLft,                // Left arg
     APLHC4R aplRht)                // Right ...

{
    APLHC4R aplRes = {0};
    UBOOL   bImagLft = IzitImaginary (ARRAY_HC4F, &aplLft),
            bImagRht = IzitImaginary (ARRAY_HC4F, &aplRht);

    // If the left & right args are real, ...
    if (!bImagLft
     && !bImagRht)
    {
        // Initialize to 0/1
        mphc4r_init (&aplRes);

        // Check for indeterminates:  0 * 0
        if (IsZeroHCxR (&aplLft, 4)
         && IsZeroHCxR (&aplRht, 4))
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_0EXP0,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  0 * +_
        if (IsZeroHCxR (&aplLft, 4)
         && mpq_cmp (&aplRht.parts[0], &mpqPosInfinity) EQ 0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_0EXPPi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  0 * -_
        if (IsZeroHCxR (&aplLft, 4)
         && mpq_cmp (&aplRht.parts[0], &mpqNegInfinity) EQ 0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_0EXPNi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  L * _ for L <= -1
        if (mpq_cmp_si (&aplLft.parts[0], -1, 1) <= 0
         && mpq_cmp (&aplRht.parts[0], &mpqPosInfinity) EQ 0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_NEXPPi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  L * -_ for -1 <= L < 0
        if (mpq_cmp    (&aplRht.parts[0], &mpqNegInfinity) EQ 0
         && mpq_cmp_si (&aplLft.parts[0], -1, 1) >= 0
         && mpq_cmp_si (&aplLft.parts[0],  0, 1) <  0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_N1to0EXPNi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
////////// Check for indeterminates:  L * R for L < 0 and R not an integer
////////if (SIGN_APLRAT (&aplLft.parts[0])
//////// && !mpq_integer_p (&aplRht.parts[0]))
////////{
////////    mpq_QuadICValue (&aplLft.parts[0],
////////                      ICNDX_NegEXPFrc,
////////                     &aplRht.parts[0],
////////                     &aplRes.parts[0],
////////                      FALSE);
////////} // End IF
////////else
        // Check for special cases:  _ * 0 and -_ * 0
        if (mpq_inf_p (&aplLft.parts[0])
         && IsZeroHCxR (&aplRht, 4))
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_InfEXP0,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for special cases:  1 * _ and 1 * -_
        if (IsMpq1 (&aplLft.parts[0])
         && mpq_inf_p (&aplRht.parts[0]))
            mpq_init_set (&aplRes.parts[0], &aplLft.parts[0]);
        else
        // Check for complex result
        if (SIGN_APLRAT (&aplLft.parts[0])
         && !mpq_integer_p (&aplRht.parts[0]))
        {
            // Avoid memory leak
            Myhc4r_clear (&aplRes);

            RaiseException (EXCEPTION_RESULT_HC4V, 0, 0, NULL);
        } else
        {
            // Avoid memory leak
            Myhc4r_clear (&aplRes);

            // Calculate the HC power
            aplRes = PowHC4R_SUB (aplLft, aplRht);
        } // End IF/ELSE/...
    } else
        // Calculate the HC power
        aplRes = PowHC4R_SUB (aplLft, aplRht);

    return aplRes;
} // End PowHC4R_RE


//***************************************************************************
//  $PowHC4R_SUB
//***************************************************************************

APLHC4R PowHC4R_SUB
    (APLHC4R aplLft,                    // Left arg
     APLHC4R aplRht)                    // Right ...

{
    APLHC4R aplRes = {0};
    UBOOL   bRet = FALSE;               // TRUE iff the exponent is an intger
    APLINT  aplExp;                     // Temp for Exponent

    // Get the real part of the exponent
    aplExp = ConvertToInteger_CT (ARRAY_HC4R, &aplRht, 0, 0.0, &bRet);

    // If the exponent is a non-negative integer (and thus not imaginary), ...
    if (bRet)
    {
        APLHC4R aplTmp;
        APLINT  aplAbs;                     // Temp for abs64 (aplExp)

        // Initialize to 0/1
        mphc4r_init (&aplRes);

        // Initialize with identity element for multiplication
        mpq_set_si (&aplRes.parts[0], 1, 1);

        // Calculate the absolute value of aplExp
        aplAbs = abs64 (aplExp);

        // Compute the powers
        while (aplAbs--)                        // ***OPTIMIZEME*** -- Use binary exponentiation
        {
            // Accumulate the product
            aplTmp = MulHC4R_RE (aplRes, aplLft);

            // Save in the result
            mphc4r_set (&aplRes, &aplTmp);

            // We no longer need this resource
            Myhc4r_clear (&aplTmp);
        } // End WHILE

        // If the exponent is negative, ...
        if (aplExp < 0)
        {
            // Invert the result
            aplTmp = InvHC4R_RE (aplRes);

            // We no longer need this resource
            Myhc4r_clear (&aplRes);

            aplRes = aplTmp;
        } // End IF
//  } else
//  // If the base is negative, ...
//  if (SIGN_APLRAT (&aplLft.parts[0]))
//  {
//      // If the denominator is even, ...
//      if (mpz_even_p (mpq_denref (&aplRht.parts[0])))
//      {
//          // Avoid memory leak
//          Myhc4r_clear (&aplRes);
//
//          RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
//      } // End IF
//      else
//      // If the numerator is even (and the denominator is odd), ...
//      if (mpz_even_p (mpq_numref (&aplRht.parts[0])))
//          // Blowup to VFP positive:  (|L)*R
//          RaiseException (EXCEPTION_RESULT_VFPPOS, 0, 0, NULL);
//      else
//      // The numerator and denominator are both odd
//          // Blowup to VFP negative:  -(|L)*R
//          RaiseException (EXCEPTION_RESULT_VFPNEG, 0, 0, NULL);
    } else
        // Otherwise, blowup to HC4V
        RaiseException (EXCEPTION_RESULT_HC4V, 0, 0, NULL);

    return aplRes;
} // End PowHC4R_SUB


//***************************************************************************
//  $PrimFnDydStarHC4RisHC4RvHC4R
//
//  Primitive scalar function dyadic Star:  HC4R {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydStarHC4RisHC4RvHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC4R_RE (lpatLft->aplHC4R, lpatRht->aplHC4R);
} // End PrimFnDydStarHC4RisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydStarHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic Star:  HC4V {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydStarHC4VisHC4RvHC4R
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
    PrimFnDydStarHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydStarHC4VisHC4RvHC4R


//***************************************************************************
//  $PowHC4V_RE
//***************************************************************************

APLHC4V PowHC4V_RE
    (APLHC4V aplLft,                // Left arg
     APLHC4V aplRht)                // Right ...

{
    APLHC4V aplRes = {0};
    UBOOL   bImagLft = IzitImaginary (ARRAY_HC4V, &aplLft),
            bImagRht = IzitImaginary (ARRAY_HC4V, &aplRht);

    // If the left & right args are real, ...
    if (!bImagLft
     && !bImagRht)
    {
        // Initialize to 0
        mphc4v_init0 (&aplRes);

        // Check for indeterminates:  0 * 0
        if (IsZeroHCxV (&aplLft, 4)
         && IsZeroHCxV (&aplRht, 4))
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_0EXP0,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  0 * +_
        if (IsZeroHCxV (&aplLft, 4)
         && mpfr_cmp (&aplRht.parts[0], &mpfPosInfinity) EQ 0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_0EXPPi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  0 * -_
        if (IsZeroHCxV (&aplLft, 4)
         && mpfr_cmp (&aplRht.parts[0], &mpfNegInfinity) EQ 0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_0EXPNi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  L * _ for L <= -1
        if (mpfr_cmp_si (&aplLft.parts[0], -1) <= 0
         && mpfr_cmp (&aplRht.parts[0], &mpfPosInfinity) EQ 0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_NEXPPi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  L * -_ for -1 <= L < 0
        if (mpfr_cmp    (&aplRht.parts[0], &mpfNegInfinity) EQ 0
         && mpfr_cmp_si (&aplLft.parts[0], -1) >= 0
         && mpfr_cmp_si (&aplLft.parts[0],  0) <  0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_N1to0EXPNi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  L * R for L < 0 and R not an integer
////////if (mpfr_sgn (&aplLft.parts[0]) < 0
//////// && !mpfr_integer_p (&aplRht.parts[0]))
////////    mpfr_QuadICValue (&aplLft.parts[0],
////////                       ICNDX_NegEXPFrc,
////////                      &aplRht.parts[0],
////////                      &aplRes.parts[0],
////////                       FALSE);
////////else
        // Check for special cases:  _ * 0 and -_ * 0
        if (mpfr_inf_p (&aplLft.parts[0])
         && IsZeroHCxV (&aplRht, 4))
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_InfEXP0,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for special cases:  1 * _ and 1 * -_
        if (IsMpf1 (&aplLft.parts[0])
         && mpfr_inf_p (&aplRht.parts[0]))
            mpfr_init_copy (&aplRes.parts[0], &aplLft.parts[0]);
        else
        {
            // Avoid memory leak
            Myhc4v_clear (&aplRes);
            // Calculate the HC power
            aplRes = PowHC4V_SUB (aplLft, aplRht);
        } // End IF/ELSE/...
    } else
        // Calculate the HC power
        aplRes = PowHC4V_SUB (aplLft, aplRht);

    return aplRes;
} // End PowHC4V_RE


//***************************************************************************
//  $PowHC4V_SUB
//***************************************************************************

APLHC4V PowHC4V_SUB
    (APLHC4V aplLft,                // Left arg
     APLHC4V aplRht)                // Right ...

{
    APLHC4V aplRes,                 // The result
            aplLog,                 // Temp var
            aplMul;                 // ...
    UBOOL   bRet = FALSE;           // TRUE iff the exponent is an intger
    APLINT  aplExp;                 // Temp for Exponent
    int     i;                      // Loop counter

    // Check for special cases:  0 * Imag
    if (IsZeroHCxV (&aplLft, 4)
     && IzitImaginary (ARRAY_HC4F, &aplRht)
     && gAllowNaN)
    {
        // Initialize to 0
        mphc4v_init0 (&aplRes);

        // The real part is NaN
        mpfr_set_nan (&aplRes.parts[0]);

        // Loop through the imaginary parts
        for (i = 1; i < 4; i++)
        // If that coefficient is non-zero, ...
        if (!IsZeroHCxV (&aplRht.parts[i], 4))
            // Set the corresponding coefficient in the result to NaN
            mpfr_set_nan (&aplRes.parts[i]);

        return aplRes;
    } // End IF/ELSE

    // Get the real part of the exponent
    aplExp = ConvertToInteger_CT (ARRAY_HC4V, &aplRht, 0, 0.0, &bRet);

    // If the exponent is a non-negative integer (and thus not imaginary), ...
    if (bRet
     && aplExp >= 0)
    {
        APLHC4V aplTmp;

        // Initialize to 0
        mphc4v_init0 (&aplRes);

        // Initialize with identity element for multiplication
        mpfr_set_si (&aplRes.parts[0], 1, MPFR_RNDN);

        // Compute the powers
        while (aplExp--)    // ***OPTIMIZEME*** -- Use binary exponentiation
        {
            // Accumulate the product
            aplTmp = MulHC4V_RE (aplRes, aplLft);

            // Save in the result
            mphc4v_set (&aplRes, &aplTmp);

            // We no longer need this resource
            Myhc4v_clear (&aplTmp);
        } // End WHILE
    } else
    {
        // L * R  is  * (R x ln (L))
////////ExpHC4V_RE (MulHC4V_RE (aplRht, LogHCxF_RE (*(LPAPLHC8F) &aplLft, 4).partsLo.partsLo));

        // Calculate ln (L)
        aplLog = LogHCxV_RE (*(LPAPLHC8V) &aplLft, 4).partsLo;

        // Multiply by R
        aplMul = MulHC4V_RE (aplRht, aplLog);

        // Calculate exp (aplMul)
        aplRes = ExpHC4V_RE (aplMul);

        // We no longer need these resources
        Myhc4v_clear (&aplMul);
        Myhc4v_clear (&aplLog);
    } // End IF/ELSE

    return aplRes;
} // End PowHC4V_SUB


//***************************************************************************
//  $PrimFnDydStarHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic Star:  HC4V {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydStarHC4VisHC4VvHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC4V_RE (lpatLft->aplHC4V, lpatRht->aplHC4V);
} // End PrimFnDydStarHC4VisHC4VvHC4V


//***************************************************************************
//  $PowHC8I_RE
//***************************************************************************

APLHC8I PowHC8I_RE
    (APLHC8I aplLft,                // Left arg
     APLHC8I aplRht)                // Right ...

{
    APLHC8I aplRes = {0};           // The result
    int     i;                      // Loop counter

    // Check for indeterminates:  0 * 0
    if (IsZeroHCxI (&aplLft, 8)
     && IsZeroHCxI (&aplRht, 8))
        RaiseException (EXCEPTION_RESULT_HC8F, 0, 0, NULL);

    // Check for special cases:  (|1) * R
    if (abs64 (aplLft.parts[0]) EQ 1
     &&        !IzitImaginary (ARRAY_HC8I, &aplLft))
    {
        // If the right arg is odd, aplRes = the left arg (1 or -1)
        if (abs64 (aplRht.parts[0]) % 2
         &&        !IzitImaginary (ARRAY_HC8I, &aplRht))
            aplRes = aplLft;
        else
        {
            // Otherwise aplRes = 1.
            aplRes.parts[0] = 1;

            // Loop through the imaginary parts
            for (i = 1; i < 8; i++)
                // Initialize to 0
                aplRes.parts[i] = 0;
        } // End IF
    } else
    // Check for special cases:  L * R for R < 0 or R > 64
    if (aplRht.parts[0] < 0
     || aplRht.parts[0] > 64
     || IzitImaginary (ARRAY_HC8I, &aplRht))
        RaiseException (EXCEPTION_RESULT_HC8F, 0, 0, NULL);
    else
    {
        APLINT aplExp;                  // Temp for Exponent

        // Initialize with identity element for multiplication
        aplRes.parts[0] = 1;

        // Loop through the imaginary parts
        for (i = 1; i < 8; i++)
            // Initialize to 0
            aplRes.parts[i] = 0;

        // Copy the exponent as an integer
        aplExp = aplRht.parts[0];

        __try
        {
            while (aplExp--)
                aplRes = MulHC8I_RE (aplRes, aplLft);
        } __except (CheckException (GetExceptionInformation (), WFCN))
        {
            EXCEPTION_CODES exCode = MyGetExceptionCode ();  // The exception code

            dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

            // Check the exception code in a main function
            CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8F);
        } // End __try/__except
    } // End IF/ELSE

    return aplRes;
} // End PowHC8I_RE


//***************************************************************************
//  $PrimFnDydStarHC8IisHC8IvHC8I
//
//  Primitive scalar function dyadic Star:  HC8I {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydStarHC8IisHC8IvHC8I
    (LPAPLHC8I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC8I_RE (lpatLft->aplHC8I, lpatRht->aplHC8I);
} // End PrimFnDydStarHC8IisHC8IvHC8I


//***************************************************************************
//  $PrimFnDydStarHC8FisHC8IvHC8I
//
//  Primitive scalar function dyadic Star:  HC8F {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydStarHC8FisHC8IvHC8I
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
    PrimFnDydStarHC8FisHC8FvHC8F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydStarHC8FisHC8IvHC8I


//***************************************************************************
//  $PowHC8F_RE
//***************************************************************************

APLHC8F PowHC8F_RE
    (APLHC8F aplLft,                // Left arg
     APLHC8F aplRht)                // Right ...

{
    APLHC8F aplRes = {0};           // The result
    UBOOL   bImagLft = IzitImaginary (ARRAY_HC8F, &aplLft),
            bImagRht = IzitImaginary (ARRAY_HC8F, &aplRht);

    // If the left & right args are real, ...
    if (!bImagLft
     && !bImagRht)
    {
        // Check for indeterminates:  0 * 0
        if (aplLft.parts[0] EQ 0
         && aplRht.parts[0] EQ 0)
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_0EXP0,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  0 * +_
        if (aplLft.parts[0] EQ 0
         && IsFltPosInfinity (aplRht.parts[0]))
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_0EXPPi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  0 * -_
        if (aplLft.parts[0] EQ 0
         && IsFltNegInfinity (aplRht.parts[0]))
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_0EXPNi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  L * _ for L <= -1
        if (aplLft.parts[0] <= -1
         && IsFltPosInfinity (aplRht.parts[0]))
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_NEXPPi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for indeterminates:  L * -_ for -1 <= L < 0
        if (IsFltNegInfinity (aplRht.parts[0])
         && aplLft.parts[0] >= -1
         && aplLft.parts[0] <   0)
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_N1to0EXPNi,
                                    aplRht.parts[0],
                                    FALSE);
        } else
////////// Check for indeterminates:  L * R for L < 0 and R not an integer
////////if (aplLft.parts[0] < 0 && !IzitImaginary (ARRAY_HC8F
//////// && aplRht.parts[0] NE floor (aplRht.parts[0]))
////////{
////////////aplRes.parts[0] =
////////      TranslateQuadICIndex (aplLft.parts[0],
////////                            ICNDX_NegEXPFrc,
////////                            aplRht.parts[0],
////////                            FALSE);
////////} else
        // Check for special cases:  _ * 0 and -_ * 0
        if (IsFltInfinity (aplLft.parts[0])
         && aplRht.parts[0] EQ 0)
        {
            aplRes.parts[0] =
              TranslateQuadICIndex (aplLft.parts[0],
                                    ICNDX_InfEXP0,
                                    aplRht.parts[0],
                                    FALSE);
        } else
        // Check for special cases:  1 * _ and 1 * -_
        if (aplLft.parts[0] EQ 1
         && IsFltInfinity (aplRht.parts[0]))
            aplRes.parts[0] = aplLft.parts[0];
        else
            // Calculate the power
            aplRes = PowHC8F_SUB (aplLft, aplRht);
    } else
        // Calculate the power
        aplRes = PowHC8F_SUB (aplLft, aplRht);

    return aplRes;
} // End PowHC8F_RE


//***************************************************************************
//  $PowHC8F_SUB
//
//  Power function for non-real arg(s)
//***************************************************************************

APLHC8F PowHC8F_SUB
    (APLHC8F aplLft,                // Left arg
     APLHC8F aplRht)                // Right ...

{
    APLHC8F aplRes = {0};           // The result
    APLINT  aplExp;                 // Temp for Exponent
    UBOOL   bRet;                   // TRUE iff the result is valid
    int     i;                      // Loop counter

    // Check for special cases:  0 * Imag
    if (IsZeroHCxF (&aplLft, 8)
     && IzitImaginary (ARRAY_HC8F, &aplRht)
     && gAllowNaN)
    {
        // The real part is NaN
        aplRes.parts[0] = fltNaN;

        // Loop through the imaginary parts
        for (i = 1; i < 8; i++)
        // If that coefficient is non-zero, ...
        if (aplRht.parts[i] NE 0.0)
            // Set the corresponding coefficient in the result to NaN
            aplRes.parts[i] = fltNaN;

        return aplRes;
    } // End IF

    // Get the real part of the exponent
    aplExp = ConvertToInteger_CT (ARRAY_HC8F, &aplRht, 0, 0.0, &bRet);

    // If the right arg is an integer, ...
    if (bRet
     && aplExp >= 0)
    {
        // Initialize with identity element for multiplication
        aplRes.parts[0] = 1.0;

        for (i = 1; i < 8; i++)
            aplRes.parts[i] = 0.0;

        while (aplExp--)
            aplRes = MulHC8F_RE (aplRes, aplLft);

        return aplRes;
    } else
        // L * R  is  * (R x ln (L))
        return ExpHC8F_RE (MulHC8F_RE (aplRht, LogHCxF_RE (*(LPAPLHC8F) &aplLft, 8)));
} // End PowHC8F_SUB


//***************************************************************************
//  $PrimFnDydStarHC8FisHC8FvHC8F
//
//  Primitive scalar function dyadic Star:  HC8F {is} HC8F fn HC8F
//***************************************************************************

void PrimFnDydStarHC8FisHC8FvHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC8F_RE (lpatLft->aplHC8F, lpatRht->aplHC8F);
} // End PrimFnDydStarHC8FisHC8FvHC8F


//***************************************************************************
//  $PowHC8R_RE
//***************************************************************************

APLHC8R PowHC8R_RE
    (APLHC8R aplLft,                // Left arg
     APLHC8R aplRht)                // Right ...

{
    APLHC8R aplRes = {0};
    UBOOL   bImagLft = IzitImaginary (ARRAY_HC8F, &aplLft),
            bImagRht = IzitImaginary (ARRAY_HC8F, &aplRht);

    // If the left & right args are real, ...
    if (!bImagLft
     && !bImagRht)
    {
        // Initialize to 0/1
        mphc8r_init (&aplRes);

        // Check for indeterminates:  0 * 0
        if (IsZeroHCxR (&aplLft, 8)
         && IsZeroHCxR (&aplRht, 8))
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_0EXP0,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  0 * +_
        if (IsZeroHCxR (&aplLft, 8)
         && mpq_cmp (&aplRht.parts[0], &mpqPosInfinity) EQ 0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_0EXPPi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  0 * -_
        if (IsZeroHCxR (&aplLft, 8)
         && mpq_cmp (&aplRht.parts[0], &mpqNegInfinity) EQ 0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_0EXPNi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  L * _ for L <= -1
        if (mpq_cmp_si (&aplLft.parts[0], -1, 1) <= 0
         && mpq_cmp (&aplRht.parts[0], &mpqPosInfinity) EQ 0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_NEXPPi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for indeterminates:  L * -_ for -1 <= L < 0
        if (mpq_cmp    (&aplRht.parts[0], &mpqNegInfinity) EQ 0
         && mpq_cmp_si (&aplLft.parts[0], -1, 1) >= 0
         && mpq_cmp_si (&aplLft.parts[0],  0, 1) <  0)
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_N1to0EXPNi,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
////////// Check for indeterminates:  L * R for L < 0 and R not an integer
////////if (SIGN_APLRAT (&aplLft.parts[0])
//////// && !mpq_integer_p (&aplRht.parts[0]))
////////{
////////    mpq_QuadICValue (&aplLft.parts[0],
////////                      ICNDX_NegEXPFrc,
////////                     &aplRht.parts[0],
////////                     &aplRes.parts[0],
////////                      FALSE);
////////} // End IF
////////else
        // Check for special cases:  _ * 0 and -_ * 0
        if (mpq_inf_p (&aplLft.parts[0])
         && IsZeroHCxR (&aplRht, 8))
            mpq_QuadICValue (&aplLft.parts[0],
                              ICNDX_InfEXP0,
                             &aplRht.parts[0],
                             &aplRes.parts[0],
                              FALSE);
        else
        // Check for special cases:  1 * _ and 1 * -_
        if (IsMpq1 (&aplLft.parts[0])
         && mpq_inf_p (&aplRht.parts[0]))
            mpq_init_set (&aplRes.parts[0], &aplLft.parts[0]);
        else
        // Check for complex result
        if (SIGN_APLRAT (&aplLft.parts[0])
         && !mpq_integer_p (&aplRht.parts[0]))
        {
            // Avoid memory leak
            Myhc8r_clear (&aplRes);

            RaiseException (EXCEPTION_RESULT_HC8V, 0, 0, NULL);
        } else
        {
            // Avoid memory leak
            Myhc8r_clear (&aplRes);

            // Calculate the HC power
            aplRes = PowHC8R_SUB (aplLft, aplRht);
        } // End IF/ELSE
    } else
        // Calculate the HC power
        aplRes = PowHC8R_SUB (aplLft, aplRht);

    return aplRes;
} // End PowHC8R_RE


//***************************************************************************
//  $PowHC8R_SUB
//***************************************************************************

APLHC8R PowHC8R_SUB
    (APLHC8R aplLft,                    // Left arg
     APLHC8R aplRht)                    // Right ...

{
    APLHC8R aplRes = {0};
    UBOOL   bRet = FALSE;               // TRUE iff the exponent is an intger
    APLINT  aplExp;                     // Temp for Exponent

    // Get the real part of the exponent
    aplExp = ConvertToInteger_CT (ARRAY_HC8R, &aplRht, 0, 0.0, &bRet);

    // If the exponent is a non-negative integer (and thus not imaginary), ...
    if (bRet)
    {
        APLHC8R aplTmp;
        APLINT  aplAbs;                     // Temp for abs64 (aplExp)

        // Initialize to 0/1
        mphc8r_init (&aplRes);

        // Initialize with identity element for multiplication
        mpq_set_si (&aplRes.parts[0], 1, 1);

        // Calculate the absolute value of aplExp
        aplAbs = abs64 (aplExp);

        // Compute the powers
        while (aplAbs--)                        // ***OPTIMIZEME*** -- Use binary exponentiation
        {
            // Accumulate the product
            aplTmp = MulHC8R_RE (aplRes, aplLft);

            // Save in the result
            mphc8r_set (&aplRes, &aplTmp);

            // We no longer need this resource
            Myhc8r_clear (&aplTmp);
        } // End WHILE

        // If the exponent is negative, ...
        if (aplExp < 0)
        {
            // Invert the result
            aplTmp = InvHC8R_RE (aplRes);

            // We no longer need this resource
            Myhc8r_clear (&aplRes);

            aplRes = aplTmp;
        } // End IF
//  } else
//  // If the base is negative, ...
//  if (SIGN_APLRAT (&aplLft.parts[0]))
//  {
//      // If the denominator is even, ...
//      if (mpz_even_p (mpq_denref (&aplRht.parts[0])))
//      {
//          // Avoid memory leak
//          Myhc8r_clear (&aplRes);
//
//          RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
//      } // End IF
//      else
//      // If the numerator is even (and the denominator is odd), ...
//      if (mpz_even_p (mpq_numref (&aplRht.parts[0])))
//          // Blowup to VFP positive:  (|L)*R
//          RaiseException (EXCEPTION_RESULT_VFPPOS, 0, 0, NULL);
//      else
//      // The numerator and denominator are both odd
//          // Blowup to VFP negative:  -(|L)*R
//          RaiseException (EXCEPTION_RESULT_VFPNEG, 0, 0, NULL);
    } else
        // Otherwise, blowup to HC8V
        RaiseException (EXCEPTION_RESULT_HC8V, 0, 0, NULL);

    return aplRes;
} // End PowHC8R_SUB


//***************************************************************************
//  $PrimFnDydStarHC8RisHC8RvHC8R
//
//  Primitive scalar function dyadic Star:  HC8R {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydStarHC8RisHC8RvHC8R
    (LPAPLHC8R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC8R_RE (lpatLft->aplHC8R, lpatRht->aplHC8R);
} // End PrimFnDydStarHC8RisHC8RvHC8R


//***************************************************************************
//  $PrimFnDydStarHC8VisHC8RvHC8R
//
//  Primitive scalar function dyadic Star:  HC8V {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydStarHC8VisHC8RvHC8R
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
    PrimFnDydStarHC8VisHC8VvHC8V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydStarHC8VisHC8RvHC8R


//***************************************************************************
//  $PowHC8V_RE
//***************************************************************************

APLHC8V PowHC8V_RE
    (APLHC8V aplLft,                // Left arg
     APLHC8V aplRht)                // Right ...

{
    APLHC8V aplRes = {0};
    UBOOL   bImagLft = IzitImaginary (ARRAY_HC8V, &aplLft),
            bImagRht = IzitImaginary (ARRAY_HC8V, &aplRht);

    // If the left & right args are real, ...
    if (!bImagLft
     && !bImagRht)
    {
        // Initialize to 0
        mphc8v_init0 (&aplRes);

        // Check for indeterminates:  0 * 0
        if (IsZeroHCxV (&aplLft, 8)
         && IsZeroHCxV (&aplRht, 8))
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_0EXP0,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  0 * +_
        if (IsZeroHCxV (&aplLft, 8)
         && mpfr_cmp (&aplRht.parts[0], &mpfPosInfinity) EQ 0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_0EXPPi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  0 * -_
        if (IsZeroHCxV (&aplLft, 8)
         && mpfr_cmp (&aplRht.parts[0], &mpfNegInfinity) EQ 0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_0EXPNi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  L * _ for L <= -1
        if (mpfr_cmp_si (&aplLft.parts[0], -1) <= 0
         && mpfr_cmp (&aplRht.parts[0], &mpfPosInfinity) EQ 0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_NEXPPi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  L * -_ for -1 <= L < 0
        if (mpfr_cmp    (&aplRht.parts[0], &mpfNegInfinity) EQ 0
         && mpfr_cmp_si (&aplLft.parts[0], -1) >= 0
         && mpfr_cmp_si (&aplLft.parts[0],  0) <  0)
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_N1to0EXPNi,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for indeterminates:  L * R for L < 0 and R not an integer
////////if (mpfr_sgn (&aplLft.parts[0]) < 0
//////// && !mpfr_integer_p (&aplRht.parts[0]))
////////    mpfr_QuadICValue (&aplLft.parts[0],
////////                       ICNDX_NegEXPFrc,
////////                      &aplRht.parts[0],
////////                      &aplRes.parts[0],
////////                       FALSE);
////////else
        // Check for special cases:  _ * 0 and -_ * 0
        if (mpfr_inf_p (&aplLft.parts[0])
         && IsZeroHCxV (&aplRht, 8))
            mpfr_QuadICValue (&aplLft.parts[0],
                               ICNDX_InfEXP0,
                              &aplRht.parts[0],
                              &aplRes.parts[0],
                               FALSE);
        else
        // Check for special cases:  1 * _ and 1 * -_
        if (IsMpf1 (&aplLft.parts[0])
         && mpfr_inf_p (&aplRht.parts[0]))
            mpfr_init_copy (&aplRes.parts[0], &aplLft.parts[0]);
        else
        {
            // Avoid memory leak
            Myhc8v_clear (&aplRes);

            // Calculate the HC power
            aplRes = PowHC8V_SUB (aplLft, aplRht);
        } // End IF/ELSE/...
    } else
        // Calculate the HC power
        aplRes = PowHC8V_SUB (aplLft, aplRht);

    return aplRes;
} // End PowHC8V_RE


//***************************************************************************
//  $PowHC8V_SUB
//
//  Power function for non-real arg(s)
//***************************************************************************

APLHC8V PowHC8V_SUB
    (APLHC8V aplLft,                // Left arg
     APLHC8V aplRht)                // Right ...

{
    APLHC8V aplRes,                 // The result
            aplLog,                 // Temp var
            aplMul;                 // ...
    UBOOL   bRet = FALSE;           // TRUE iff the exponent is an intger
    APLINT  aplExp;                 // Temp for Exponent
    int     i;                      // Loop counter

    // Check for special cases:  0 * Imag
    if (IsZeroHCxV (&aplLft, 8)
     && IzitImaginary (ARRAY_HC8F, &aplRht)
     && gAllowNaN)
    {
        // Initialize to 0
        mphc8v_init0 (&aplRes);

        // The real part is NaN
        mpfr_set_nan (&aplRes.parts[0]);

        // Loop through the imaginary parts
        for (i = 1; i < 8; i++)
        // If that coefficient is non-zero, ...
        if (!IsZeroHCxV (&aplRht.parts[i], 8))
            // Set the corresponding coefficient in the result to NaN
            mpfr_set_nan (&aplRes.parts[i]);

        return aplRes;
    } // End IF/ELSE

    // Get the real part of the exponent
    aplExp = ConvertToInteger_CT (ARRAY_HC8V, &aplRht, 0, 0.0, &bRet);

    // If the exponent is a non-negative integer (and thus not imaginary), ...
    if (bRet
     && aplExp >= 0)
    {
        APLHC8V aplTmp;

        // Initialize to 0
        mphc8v_init0 (&aplRes);

        // Initialize with identity element for multiplication
        mpfr_set_si (&aplRes.parts[0], 1, MPFR_RNDN);

        // Compute the powers
        while (aplExp--)    // ***OPTIMIZEME*** -- Use binary exponentiation
        {
            // Accumulate the product
            aplTmp = MulHC8V_RE (aplRes, aplLft);

            // Save in the result
            mphc8v_set (&aplRes, &aplTmp);

            // We no longer need this resource
            Myhc8v_clear (&aplTmp);
        } // End WHILE
    } else
    {
        // L * R  is  * (R x ln (L))
////////ExpHC8V_RE (MulHC8V_RE (aplRht, LogHCxF_RE (*(LPAPLHC8F) &aplLft, 8).partsLo.partsLo));

        // Calculate ln (L)
        aplLog = LogHCxV_RE (*(LPAPLHC8V) &aplLft, 8);

        // Multiply by R
        aplMul = MulHC8V_RE (aplRht, aplLog);

        // Calculate exp (aplMul)
        aplRes = ExpHC8V_RE (aplMul);

        // We no longer need these resources
        Myhc8v_clear (&aplMul);
        Myhc8v_clear (&aplLog);
    } // End IF/ELSE

    return aplRes;
} // End PowHC8V_SUB


//***************************************************************************
//  $PrimFnDydStarHC8VisHC8VvHC8V
//
//  Primitive scalar function dyadic Star:  HC8V {is} HC8V fn HC8V
//***************************************************************************

void PrimFnDydStarHC8VisHC8VvHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = PowHC8V_RE (lpatLft->aplHC8V, lpatRht->aplHC8V);
} // End PrimFnDydStarHC8VisHC8VvHC8V


//***************************************************************************
//  $sinCT_Flt
//
//  Floating point sine function with special treatment
//    around an integer multiple of Pi
//***************************************************************************

APLFLOAT sinCT_Flt
    (APLFLOAT aplRht)

{
    APLFLOAT aplRes = {0};

    // Divide so we can compare it with its floor
    aplRes = aplRht / FloatPi;

    // Handle sin (Integer*FloatPi) specially as
    //   sin (Integer*FloatPi) isn't 0, but a very small number.
    if (CmpCT_F (aplRes, floor (aplRes), SYS_CT, EQ))
        aplRes = 0.0;
    else
        aplRes = sin (aplRht);

    return aplRes;
} // End sinCT_Flt


//***************************************************************************
//  $sinCT_Vfp
//
//  Vfp sine function with special treatment
//    around an integer multiple of Pi
//***************************************************************************

APLVFP sinCT_Vfp
    (APLVFP aplRht)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory handle
    APLVFP       aplFlr,
                 aplRes = {0};

    // Initialize the temp to 0
    mpfr_init0 (&aplRes);

    // Get the PerTabData ptr
    lpMemPTD = GetMemPTD ();

    // Initialize the VFP Pi if not already done
    InitPTD_Pi (lpMemPTD);

    // Divide so we can compare it with its floor
    mpfr_div (&aplRes, &aplRht, &lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);

    // Calculate the floor so we can see if it's an integer
    aplFlr = FloorHC1V (aplRes);

    // Handle sin (Integer*FloatPi) specially as
    //   sin (Integer*FloatPi) isn't 0, but a very small number.
    if (CmpCT_V (aplRes, aplFlr, SYS_CT, EQ))
        mpfr_set_ui (&aplRes, 0, MPFR_RNDN);
    else
        mpfr_sin    (&aplRes, &aplRht, MPFR_RNDN);

    Myf_clear (&aplFlr);

    return aplRes;
} // End sinCT_Vfp


//***************************************************************************
//  $cosCT_Flt
//
//  Floating point cos function with special treatment
//    around an odd integer multiple of Pi/2
//***************************************************************************

APLFLOAT cosCT_Flt
    (APLFLOAT aplRht)

{
    APLFLOAT aplFlr,
             aplRes;
    APLINT   aplInt;
    UBOOL    bRet;

    // Divide by Pi/2 so we can compare it with its floor
    aplRes = aplRht / (FloatPi / 2);

    // Calculate the floor so we can see if it's an odd integer
    aplFlr = floor (aplRes);

    // Extract the integer from aplFlr
    aplInt = ConvertToInteger_SCT (ARRAY_FLOAT, &aplFlr, 0, &bRet);

    // Handle cos (OddInteger*Pi/2) specially as
    //   cos (OddInteger*Pi/2) isn't 0, but a very small number.
    if (bRet
     && 1 EQ aplInt % 2
     && CmpCT_F (aplRes, aplFlr, SYS_CT, EQ))
        aplRes = 0.0;
    else
        aplRes = cos (aplRht);

    return aplRes;
} // End cosCT_Flt


//***************************************************************************
//  $cosCT_Vfp
//
//  Vfp cos function with special treatment
//    around an odd integer multiple of Pi/2
//***************************************************************************

APLVFP cosCT_Vfp
    (APLVFP aplRht)

{
    APLVFP aplFlr,
           aplRes = {0};
    APLINT aplInt;
    UBOOL  bRet;
    LPPERTABDATA lpMemPTD = GetMemPTD ();   // Ptr to PerTabData global memory

    // Initialize the VFP Pi if not already done
    InitPTD_Pi (lpMemPTD);

    // Initialize the temp to 0
    mpfr_init0 (&aplRes);

    // Divide by Pi/2 so we can compare it with its floor
    mpfr_div    (&aplRes, &aplRht, &lpMemPTD->mpfrHC8V_Pi.parts[0], MPFR_RNDN);
    mpfr_div_ui (&aplRes, &aplRes,                   2, MPFR_RNDN);

    // Calculate the floor so we can see if it's an odd integer
    aplFlr = FloorHC1V (aplRes);

    // Extract the integer from aplFlr
    aplInt = ConvertToInteger_SCT (ARRAY_VFP, &aplFlr, 0, &bRet);

    // Handle cos (OddInteger*Pi/2) specially as
    //   cos (OddInteger*Pi/2) isn't 0, but a very small number.
    if (bRet
     && 1 EQ aplInt % 2
     && CmpCT_V (aplRes, aplFlr, SYS_CT, EQ))
        mpfr_set_ui (&aplRes, 0, MPFR_RNDN);
    else
        mpfr_cos    (&aplRes, &aplRht, MPFR_RNDN);

    Myf_clear (&aplFlr);

    return aplRes;
} // End cosCT_Vfp


//***************************************************************************
//  End of File: pf_star.c
//***************************************************************************
