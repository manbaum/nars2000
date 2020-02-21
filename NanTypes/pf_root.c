//***************************************************************************
//  NARS2000 -- Primitive Function -- Root
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2020 Sudley Place Software

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
#include "acb.h"


extern PRIMSPEC PrimSpecDoubleTildeUnion;


#ifndef PROTO
PRIMSPEC PrimSpecRoot = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecRootStorageTypeMon,
    NULL,   // &PrimFnMonRootAPA_EM, -- Can't happen w/Root

    // Monadic Boolean result functions
    &PrimFnMonRootBisB,
    NULL,   // &PrimFnMonRootBisI, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootBisF, -- Can't happen w/Root

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonRootIisI, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootIisF, -- Can't happen w/Root
    &PrimFnMonRootFisI,
    &PrimFnMonRootFisF,
    NULL,   // &PrimFnMonRootRisR, -- Can't happen w/Root
    &PrimFnMonRootVisR,
    &PrimFnMonRootVisV,

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonRootHC2IisHC2I, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootHC2IisHC2F, -- Can't happen w/Root
    &PrimFnMonRootHC2FisHC2I,
    &PrimFnMonRootHC2FisHC2F,
    NULL,   // &PrimFnMonRootHC2RisHC2R, -- Can't happen w/Root
    &PrimFnMonRootHC2VisHC2R,
    &PrimFnMonRootHC2VisHC2V,

    NULL,   // &PrimFnMonRootHC4IisHC4I, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootHC4IisHC4F, -- Can't happen w/Root
    &PrimFnMonRootHC4FisHC4I,
    &PrimFnMonRootHC4FisHC4F,
    NULL,   // &PrimFnMonRootHC4RisHC4R, -- Can't happen w/Root
    &PrimFnMonRootHC4VisHC4R,
    &PrimFnMonRootHC4VisHC4V,

    NULL,   // &PrimFnMonRootHC8IisHC8I, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootHC8IisHC8F, -- Can't happen w/Root
    &PrimFnMonRootHC8FisHC8I,
    &PrimFnMonRootHC8FisHC8F,
    NULL,   // &PrimFnMonRootHC8RisHC8R, -- Can't happen w/Root
    &PrimFnMonRootHC8VisHC8R,
    &PrimFnMonRootHC8VisHC8V,

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonRootFisHC2I, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootFisHC2F, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootVisHC2R, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootVisHC2V, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootFisHC4I, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootFisHC4F, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootVisHC4R, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootVisHC4V, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootFisHC8I, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootFisHC8F, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootVisHC8R, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootVisHC8V, -- Can't happen w/Root

    // Monadic BAxF result BAxF arg functions (indexable)
    &PrimFnMonRootBA1FisBA1F,
    &PrimFnMonRootBA2FisBA2F,
    &PrimFnMonRootBA4FisBA4F,
    &PrimFnMonRootBA8FisBA8F,
    &PrimFnMonRootBA1FisBA1F,
    NULL,   // &PrimFnMonRootARBisBA2F, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootARBisBA4F, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootARBisBA8F, -- Can't happen w/Root

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecRootStorageTypeDyd,
    NULL,   // &PrimFnDydRootAPA_EM, -- Can't happen w/Root

    // Dyadic Boolean result functions
    &PrimFnDydRootBisBvB,
    NULL,   // &PrimFnDydRootBisIvI, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisFvF, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisCvC, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisHvH, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisRvR, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisVvV, -- Can't happen w/Root

    NULL,   // &PrimFnDydRootBisHC2IvHC2I, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisHC2FvHC2F, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisHC2RvHC2R, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisHC2VvHC2V, -- Can't happen w/Root

    NULL,   // &PrimFnDydRootBisHC4IvHC4I, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisHC4FvHC4F, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisHC4RvHC4R, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisHC4VvHC4V, -- Can't happen w/Root

    NULL,   // &PrimFnDydRootBisHC8IvHC8I, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisHC8FvHC8F, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisHC8RvHC8R, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisHC8VvHC8V, -- Can't happen w/Root

    // Dyadic Boolean result functions with ARBs (indexable)
    NULL,   // &PrimFnDydRootBisBA1FvBA1F, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisBA2FvBA2F, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisBA4FvBA4F, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootBisBA8FvBA8F, -- Can't happen w/Root

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydRootIisIvI,
    NULL,   // &PrimFnDydRootIisFvF, -- Can't happen w/Root
    &PrimFnDydRootFisIvI,
    &PrimFnDydRootFisFvF,
    NULL,   // &PrimFnDydRootRisRvR, -- Can't happen w/Root
    &PrimFnDydRootVisRvR,
    &PrimFnDydRootVisVvV,

    NULL,   // &PrimFnDydRootHC2IisHC2IvHC2I, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootHC2IisHC2FvHC2F, -- Can't happen w/Root
    &PrimFnDydRootHC2FisHC2IvHC2I,
    &PrimFnDydRootHC2FisHC2FvHC2F,
    NULL,   // &PrimFnDydRootHC2RisHC2RvHC2R, -- Can't happen w/Root
    &PrimFnDydRootHC2VisHC2RvHC2R,
    &PrimFnDydRootHC2VisHC2VvHC2V,

    NULL,   // &PrimFnDydRootHC4IisHC4IvHC4I, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootHC4IisHC4FvHC4F, -- Can't happen w/Root
    &PrimFnDydRootHC4FisHC4IvHC4I,
    &PrimFnDydRootHC4FisHC4FvHC4F,
    NULL,   // &PrimFnDydRootHC4RisHC4RvHC4R, -- Can't happen w/Root
    &PrimFnDydRootHC4VisHC4RvHC4R,
    &PrimFnDydRootHC4VisHC4VvHC4V,

    NULL,   // &PrimFnDydRootHC8IisHC8IvHC8I, -- Can't happen w/Root
    NULL,   // &PrimFnDydRootHC8IisHC8FvHC8F, -- Can't happen w/Root
    &PrimFnDydRootHC8FisHC8IvHC8I,
    &PrimFnDydRootHC8FisHC8FvHC8F,
    NULL,   // &PrimFnDydRootHC8RisHC8RvHC8R, -- Can't happen w/Root
    &PrimFnDydRootHC8VisHC8RvHC8R,
    &PrimFnDydRootHC8VisHC8VvHC8V,

    // Dyadic BAxF result functions (indexable)
    &PrimFnDydRootBA1FisBA1FvBA1F,
    &PrimFnDydRootBA2FisBA2FvBA2F,
    &PrimFnDydRootBA4FisBA4FvBA4F,
    &PrimFnDydRootBA8FisBA8FvBA8F,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonRootB64isB64, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootB32isB32, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootB16isB16, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootB08isB08, -- Can't happen w/Root

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonRootB64isB64vB64, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootB32isB32vB32, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootB16isB16vB16, -- Can't happen w/Root
    NULL,   // &PrimFnMonRootB08isB08vB08, -- Can't happen w/Root

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecRoot};
#endif


//***************************************************************************
//  $PrimFnRoot_EM_YY
//
//  Primitive function for monadic and dyadic Root ("Square Root" and "Nth Root")
//***************************************************************************

LPPL_YYSTYPE PrimFnRoot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_ROOT);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnRoot_EM_YY


//***************************************************************************
//  $PrimSpecRootStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRootStorageTypeMon
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

        case ARRAY_ARB:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:

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
} // End PrimSpecRootStorageTypeMon


//***************************************************************************
//  $PrimFnMonRootBisB
//
//  Primitive scalar function monadic Root:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonRootBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonRootBisB


//***************************************************************************
//  $PrimFnMonRootFisI
//
//  Primitive scalar function monadic Root:  F {is} fn I
//***************************************************************************

void PrimFnMonRootFisI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for Complex result
    if (lpatRht->aplInteger < 0)
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
        lpMemRes[uRes] = sqrt ((APLFLOAT) lpatRht->aplInteger);
} // End PrimFnMonRootFisI


//***************************************************************************
//  $PrimFnMonRootFisF
//
//  Primitive scalar function monadic Root:  F {is} fn F
//***************************************************************************

void PrimFnMonRootFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for Complex result
    if (lpatRht->aplFloat < 0)    // Not SIGN_APLFLOAT as that'll catch -0 whose sqrt is -0
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
        lpMemRes[uRes] = sqrt (lpatRht->aplFloat);
} // End PrimFnMonRootFisF


//***************************************************************************
//  $PrimFnMonRootVisR
//
//  Primitive scalar function monadic Root:  V {is} fn R
//***************************************************************************

void PrimFnMonRootVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonRootVisV (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
} // End PrimFnMonRootVisR


//***************************************************************************
//  $PrimFnMonRootVisV
//
//  Primitive scalar function monadic Root:  V {is} fn V
//***************************************************************************

void PrimFnMonRootVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for Complex result
    if (mpfr_cmp_ui (&lpatRht->aplVfp, 0) < 0)
        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    else
    {
        // Initialize the result
        mpfr_init0 (&lpMemRes[uRes]);

        // Extract the square root of a Variable FP
        mpfr_sqrt (&lpMemRes[uRes], &lpatRht->aplVfp, MPFR_RNDN);
    } // End IF/ELSE
} // End PrimFnMonRootVisV


//***************************************************************************
//  $PrimFnMonRootBA1FisBA1F
//
//  Primitive scalar function monadic Root:  V {is} fn V
//***************************************************************************

void PrimFnMonRootBA1FisBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for Complex result
    if (arb_contains_negative (&lpatRht->aplArb))
        RaiseException (EXCEPTION_RESULT_BA2F, 0, 0, NULL);
    else
    {
        mp_limb_signed_t prec = ARB_PREC_FPC;

        // Initialize the result to 0/0
        Myarb_init (&lpMemRes[uRes]);

        // Weed out negative numbers if they are small
        if (arb_contains_negative (&lpatRht->aplArb))
        {
            arf_t    arfLo = {0},
                     arfHi = {0};
            int      signLo,
                     signHi;
            APLFLOAT aplFltLo,
                     aplFltHi;

            Myarf_init (arfLo);
            Myarf_init (arfHi);

            // Get the lower and upper bounds on the Ball
            arb_get_interval_arf (arfLo, arfHi, &lpatRht->aplArb, prec);

            // Get the sign of the lower and upper bounds
            signLo = arf_sgn (arfLo);
            signHi = arf_sgn (arfHi);

            // Get the Lo and Hi bounds as FLT
            aplFltLo = arf_get_d (arfLo, ARF_RND_NEAR);
            aplFltHi = arf_get_d (arfHi, ARF_RND_NEAR);

            // If they are on opposite sides of the origin, ...
            if (signLo < 0 && signHi > 0)
            {
                // If the Lo bound is small, ...
                if (fabs (aplFltLo) < SYS_CT)
                    // Ignore the small negative numbers
                    arb_sqrtpos (&lpMemRes[uRes], &lpatRht->aplArb, prec);
                else
                // If the Hi bound is small, ...
                if (fabs (aplFltHi) < SYS_CT)
                {
                    // We no longer need this storage
                    arf_clear   (arfHi);
                    arf_clear   (arfLo);
                    Myarb_clear (&lpMemRes[uRes]);

                    // Try it as a Complex result
                    RaiseException (EXCEPTION_RESULT_BA2F, 0, 0, NULL);
                } else
                    // Call it a NaN
                    arb_set_nan (&lpMemRes[uRes]);
            } else
            // If the interval is non-negative, ...
            if (signLo >= 0)
                arb_sqrt (&lpMemRes[uRes], &lpatRht->aplArb, prec);
            else
            // The interval is negative
            {
                // We no longer need this storage
                arf_clear   (arfHi);
                arf_clear   (arfLo);
                Myarb_clear (&lpMemRes[uRes]);

                // Try it as a Complex result
                RaiseException (EXCEPTION_RESULT_BA2F, 0, 0, NULL);
            } // End IF/ELSE/...

            // We no longer need this storage
            arf_clear (arfHi);
            arf_clear (arfLo);
        } else
            // Extract the square root of an ARB
            arb_sqrt    (&lpMemRes[uRes], &lpatRht->aplArb, prec);
    } // End IF/ELSE
} // End PrimFnMonRootBA1FisBA1F


//***************************************************************************
//  $PrimFnMonRootHC2FisHC2I
//
//  Primitive scalar function monadic Root:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonRootHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonRootHC2FisHC2F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonRootHC2FisHC2I


//***************************************************************************
//  $SqrtHCxF_RE
//***************************************************************************

APLHC8F SqrtHCxF_RE
    (APLHC8F aplRht,                // Right arg
     int     iHCDimRes)             // HC Dimension (1, 2, 4, 8)

{
    APLHC8F  aplRes = {0};          // Result
    APLFLOAT aplRMag,               // Magnitude of the entire arg
             aplIMag,               // ...              imaginary parts
             aplU,                  // Temp var
             aplV,                  // ...
             aplMul;                // ...
    int      i;                     // Loop counter

    /*
        From http://home.comcast.net/~tamivox/redbear/qtrn_calc/index.html
        Sqrt of a Hypercomplex number:

        h = pF (stack[0].h);
        i = pF (stack[0].i);
        j = pF (stack[0].j);
        k = pF (stack[0].k);

        g = r_sqrt (i * i + j * j + k * k);
        m = r_sqrt (h * h + g * g);
        u = r_sqrt (m);
        v = (g == 0) ? 0 : r_atan2 (g, h) / 2;
        p = u * r_cos (v);
        q = (g == 0) ? 0 : u * r_sin (v) / g;

        stack[0].h = p;
        stack[0].i = q * i;
        stack[0].j = q * j;
        stack[0].k = q * k;
     */

    // Calculate the sum of the squares of both the entire arg and
    //   just the imaginary parts
    aplIMag = 0;

    // Calculate the square of the real part
    aplRMag = aplRht.parts[0] * aplRht.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimRes; i++)
        // Calculate the sum of the squares
        aplIMag += aplRht.parts[i] * aplRht.parts[i];

    // Add the square of the real part to the sum of the squares of the imaginary parts
    //   to get the sum of the squares of the entire arg
    aplRMag = aplRMag + aplIMag;

    // Calculate the magnitude of the entire arg and
    //   the magnitude of just the imaginary parts
    aplIMag = sqrt (aplIMag);                                       // g
    aplRMag = sqrt (aplRMag);                                       // m
    aplU    = sqrt (aplRMag);                                       // u

    // If the imaginary parts are all 0 or a NaN, ...
    if (aplIMag EQ 0 || _isnan (aplIMag))
    {
        // If the real part of the right arg is negative, ...
        if (aplRht.parts[0] < 0)    // Not SIGN_APLFLOAT as -0 is handled
        {                           //   in the "else" clause
            // If it's not a Complex result as yet, ...
            if (iHCDimRes < 2)
                // Make it so
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
            else
            {
                // Set the real part to 0
////////////////aplRes.parts[0] = 0.0;  // Already done in = {0}

                // Calculate the real Sqrt of the absolute value of the right arg real part
                aplRes.parts[1] = sqrt (fabs (aplRht.parts[0]));
            } // End IF/ELSE
        } else
        {
            // Calculate the real Sqrt of the right arg real & imag parts
            aplRes.parts[0] = sqrt (aplRht.parts[0]);
            aplRes.parts[1] = sqrt (aplIMag        );
        } // End IF/ELSE
    } else
    {
        // Calculate v = r_atan2 (g, h) / 2
        aplV = atan2 (aplIMag, aplRht.parts[0]) / 2;                    // v

        // Calculate the real part
        aplRes.parts[0] = aplU * cos (aplV);                            // p

        // If the imaginary parts are all 0, ...
        if (aplIMag EQ 0)
        {
            // Loop through all but the 1st of the imaginary parts
            for (i = 2; i < iHCDimRes; i++)
                // Initialize to 0
                aplRes.parts[i] = 0.0;

            // If the real part is negative, ...
            if (aplRht.parts[0] < 0)
            {
                // Set the real part to 0
                aplRes.parts[0] = 0;

                // Set the 1st imaginary part to aplU
                aplRes.parts[1] = aplU;
            } else
                // Set the 1st imaginary part to 0
                aplRes.parts[1] = 0;
        } else
        {
            // Calculate sin (v)
            aplV   = sin (aplV);

            // Calculate sin (v) / g
            aplV   = DivHC1F_RE (aplV, aplIMag);

            // Calculate u * sin (v) / g
            aplMul = MulHC1F_RE (aplU, aplV);                           // q

            // If the multiplier is bogus (from {Inf} / {Inf}), ...
            if (_isnan (aplMul))
                RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

            // Loop through the imaginary parts
            for (i = 1; i < iHCDimRes; i++)
                // Multiply each of the imaginary parts by the arctan2
                aplRes.parts[i] = aplRht.parts[i] * aplMul;
        } // End IF/ELSE
    } // End IF/ELSE

    return aplRes;
} // End SqrtHCxF_RE


//***************************************************************************
//  $PrimFnMonRootHC2FisHC2F
//
//  Primitive scalar function monadic Root:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonRootHC2FisHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = SqrtHCxF_RE (lpatRht->aplHC8F, 2).partsLo[0].partsLo[0];
} // End PrimFnMonRootHC2FisHC2F


//***************************************************************************
//  $PrimFnMonRootHC4FisHC4I
//
//  Primitive scalar function monadic Root:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonRootHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonRootHC4FisHC4F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonRootHC4FisHC4I


//***************************************************************************
//  $PrimFnMonRootHC4FisHC4F
//
//  Primitive scalar function monadic Root:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonRootHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = SqrtHCxF_RE (lpatRht->aplHC8F, 4).partsLo[0];
} // End PrimFnMonRootHC4FisHC4F


//***************************************************************************
//  $PrimFnMonRootHC8FisHC8I
//
//  Primitive scalar function monadic Root:  HC8F {is} fn HC8I
//***************************************************************************

void PrimFnMonRootHC8FisHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Convert the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatRht, 0, &atRht);

    // Pass it on
    PrimFnMonRootHC8FisHC8F (lpMemRes, uRes, &atRht, lpPrimSpec);
} // End PrimFnMonRootHC8FisHC8I


//***************************************************************************
//  $PrimFnMonRootHC8FisHC8F
//
//  Primitive scalar function monadic Root:  HC8F {is} fn HC8F
//***************************************************************************

void PrimFnMonRootHC8FisHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = SqrtHCxF_RE (lpatRht->aplHC8F, 8);
} // End PrimFnMonRootHC8FisHC8F


//***************************************************************************
//  $PrimFnMonRootHC2VisHC2R
//
//  Primitive scalar function monadic Root:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonRootHC2VisHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonRootHC2VisHC2V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
} // End PrimFnMonRootHC2VisHC2R


//***************************************************************************
//  $SqrtHCxV_RE
//***************************************************************************

APLHC8V SqrtHCxV_RE
    (APLHC8V aplRht,                // Right arg
     int     iHCDimRes)             // HC Dimension (1, 2, 4, 8)

{
    APLHC8V  aplRes = {0};          // Result
    APLVFP   aplRMag,               // Magnitude of the entire arg
             aplIMag = {0},         // ...              imaginary parts
             aplTmp,                // Temp var
             aplU = {0},            // ...
             aplV = {0},            // ...
             aplMul;                  // ...
    int      i;                     // Loop counter
    UBOOL    bError = FALSE;        // TRUE iff we end in an error

    /*
        From http://home.comcast.net/~tamivox/redbear/qtrn_calc/index.html
        Sqrt of a Hypercomplex number:

        h = pF (stack[0].h);
        i = pF (stack[0].i);
        j = pF (stack[0].j);
        k = pF (stack[0].k);

        g = r_sqrt (i * i + j * j + k * k);
        m = r_sqrt (h * h + g * g);
        u = r_sqrt (m);
        v = (g == 0) ? 0 : r_atan2 (g, h) / 2;
        p = u * r_cos (v);
        q = (g == 0) ? 0 : u * r_sin (v) / g;

        stack[0].h = p;
        stack[0].i = q * i;
        stack[0].j = q * j;
        stack[0].k = q * k;
     */

    // Calculate the sum of the squares of both the entire arg and
    //   just the imaginary parts
    mpfr_init0 (&aplIMag);
    mpfr_init0 (&aplU);
    mpfr_init0 (&aplV);

    // Calculate the square of the real part
    aplRMag = MulHC1V_RE (aplRht.parts[0], aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < iHCDimRes; i++)
    {
        // Calculate square of the imaginary part
        aplTmp = MulHC1V_RE (aplRht.parts[i], aplRht.parts[i]);

        // Calculate the sum of the squares
        mpfr_add (&aplIMag, &aplIMag, &aplTmp, MPFR_RNDN);

        // We no longer need this resource
        Myf_clear (&aplTmp);
    } // End FOR

    // Add the square of the real part to the sum of the squares of the imaginary parts
    //   to get the sum of the squares of the entire arg
    mpfr_add  (&aplRMag, &aplRMag, &aplIMag, MPFR_RNDN);

    // Calculate the magnitude of the entire arg and
    //   the magnitude of just the imaginary parts
    mpfr_sqrt (&aplIMag, &aplIMag, MPFR_RNDN);                      // g
    mpfr_sqrt (&aplRMag, &aplRMag, MPFR_RNDN);                      // m
    mpfr_sqrt (&aplU   , &aplRMag, MPFR_RNDN);                      // u

    // If the imaginary parts are all 0 or a NaN, ...
    if (IsMpf0 (&aplIMag) || mpfr_nan_p (&aplIMag))
    {
        // Initialize to 0
        mphcXv_init0 (&aplRes, iHCDimRes);

        // If the real part of the right arg is negative, ...
        if (mpfr_sgn (&aplRht.parts[0]) < 0)    // Not SIGN_APLVFP as -0 is handled
        {                                       //   in the "else" clause
            // If it's not a Complex result as yet, ...
            if (iHCDimRes < 2)
            {
                // Free the storage
                MyhcXv_clear (&aplRes, iHCDimRes);

                // Mark as in error
                bError = TRUE;

                goto ERROR_EXIT;
            } else
            {
                // Set the real part to 0
////////////////mpfr_set_d (&aplRes.parts[0], 0.0, MPFR_RNDN);  // Already done in mphcXv_init0

                // Calculate the absolute value of the right arg real part
                mpfr_abs (&aplV, &aplRht.parts[0], MPFR_RNDN);

                // Calculate the real Sqrt of the absolute value of the right arg real part
                mpfr_sqrt (&aplRes.parts[1], &aplV, MPFR_RNDN);
            } // End IF/ELSE
        } else
        {
            // Calculate the real Sqrt of the right arg real & imag parts
            mpfr_sqrt (&aplRes.parts[0], &aplRht.parts[0], MPFR_RNDN);
            mpfr_sqrt (&aplRes.parts[1], &aplIMag        , MPFR_RNDN);
        } // End IF/ELSE
    } else
    {
        // Calculate atan2 (g, h) /2
        mpfr_atan2  (&aplV, &aplIMag, &aplRht.parts[0], MPFR_RNDN);
        mpfr_div_si (&aplV, &aplV, 2, MPFR_RNDN);

        // Calculate cos (v)
        aplTmp = cosVfp (aplV);

        // Calculate u * cos (v)
        aplRes.parts[0] = MulHC1V_RE (aplU, aplTmp);                    // p

        // We no longer need this storage
        Myf_clear (&aplTmp);

        // If the imaginary parts are all 0, ...
        if (IsMpf0 (&aplIMag))
        {
            // Loop through all but the 1st of the imaginary parts
            for (i = 2; i < iHCDimRes; i++)
                // Initialize to 0
                mpfr_init0 (&aplRes.parts[i]);

            // If the real part is negative, ...
            if (mpfr_sgn (&aplRht.parts[0]) < 0)
            {
                // Set the real part to 0
                mpfr_set_si (&aplRes.parts[0], 0, MPFR_RNDN);

                // Set the 1st imaginary part to aplU
                mpfr_init_set (&aplRes.parts[1], &aplU, MPFR_RNDN);
            } else
                // Initialize the 1st imaginary part to 0
                mpfr_init0 (&aplRes.parts[1]);
        } else
        {
            EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

            //  Calculate sin (v)
            aplTmp = sinVfp (aplV);

            __try
            {
                // Calculate sin (v) / g
                aplMul = DivHC1V_RE (aplTmp, aplIMag);

                // We no longer need this storage
                Myf_clear (&aplTmp);

                // Calculate u * sin (v) / g
                aplTmp = MulHC1V_RE (aplU, aplMul);                         // q

                // Loop through the imaginary parts
                for (i = 1; i < iHCDimRes; i++)
                    // Multiply each of the imaginary parts by the arctan2
                    aplRes.parts[i] = MulHC1V_RE (aplRht.parts[i], aplTmp);
            } __except (CheckException (GetExceptionInformation (), WFCN))
            {
                exCode = MyGetExceptionCode ();  // The exception code

                dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

                // Check the exception code in a helper function
                CheckExCodeHelper (&exCode);
            } // End __try/__except

            // We no longer need this storage
            Myf_clear (&aplTmp);
            Myf_clear (&aplMul);

            // Check the exception code in a main function
            CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
        } // End IF/ELSE
    } // End IF/ELSE
ERROR_EXIT:
    // We no longer need this storage
    Myf_clear (&aplV);
    Myf_clear (&aplU);
    Myf_clear (&aplIMag);
    Myf_clear (&aplRMag);

    if (bError)
        // Make it so
        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

    return aplRes;
} // End SqrtHCxV_RE


//***************************************************************************
//  $PrimFnMonRootHC2VisHC2V
//
//  Primitive scalar function monadic Root:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonRootHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = SqrtHCxV_RE (lpatRht->aplHC8V, 2).partsLo.partsLo;
} // End PrimFnMonRootHC2VisHC2V


//***************************************************************************
//  $SqrtBAxF_RE
//***************************************************************************

APLBA8F SqrtBAxF_RE
    (APLBA8F aplRht,                // Right arg
     int     iHCDimRes)             // HC Dimension (1, 2, 4, 8)

{
    APLBA8F  aplRes = {0};          // The result
    APLARB   aplRMag = {0},         // Magnitude of the entire arg
             aplIMag = {0},         // ...              imaginary parts
             aplTmp = {0},          // Temp var
             aplU = {0},            // ...
             aplV = {0},            // ...
             aplMul;                  // ...
    int      i;                     // Loop counter
    UBOOL    bError = FALSE;        // TRUE iff we end in an error
    mp_limb_signed_t prec = ARB_PREC_FPC;

    /*
        From http://home.comcast.net/~tamivox/redbear/qtrn_calc/index.html
        Sqrt of a Hypercomplex number:

        h = pF (stack[0].h);
        i = pF (stack[0].i);
        j = pF (stack[0].j);
        k = pF (stack[0].k);

        g = r_sqrt (i * i + j * j + k * k);
        m = r_sqrt (h * h + g * g);
        u = r_sqrt (m);
        v = (g == 0) ? 0 : r_atan2 (g, h) / 2;
        p = u * r_cos (v);
        q = (g == 0) ? 0 : u * r_sin (v) / g;

        stack[0].h = p;
        stack[0].i = q * i;
        stack[0].j = q * j;
        stack[0].k = q * k;
     */
#ifdef DEBUG
    if (iHCDimRes EQ 2 && FALSE)
    {
        acb_t w, z;

        acb_init (w);
        acb_init (z);
        acb_set_arb_arb (w, &aplRht.parts[0], &aplRht.parts[1]);

        acb_sqrt (z, w, prec);

        // Initialize to 0/0
        arbX_init (&aplRes, iHCDimRes);

        acb_get_real (&aplRes.parts[0], z);
        acb_get_imag (&aplRes.parts[1], z);

        acb_clear (z);
        acb_clear (w);

        return aplRes;
    } // End IF
#endif

#ifdef DEBUG
        if (guTest > 0)
        {
            ArbOut (L"Rht.parts[0] = ", &aplRht.parts[0]);
            ArbOut (L"Rht.parts[1] = ", &aplRht.parts[1]);
        } // End IF
#endif

    // If the Real part contains both Positive and Negative values, ...
    if (arb_contains_positive (&aplRht.parts[0]) NE 0
     && arb_contains_negative (&aplRht.parts[0]) NE 0)
    {
        APLBA8F aplNonNeg = {0},    // Holds Non-negative part
                aplNonPos = {0},    // ...       positive ...
                aplResNN,           // Temp
                aplResNP;           // ...
        APLBA2F aplRes2;            // Temp
        APLBA4F aplRes4;            // ...
        APLBA8F aplRes8;            // ...

        // Initialize to 0/0 and copy
        arbX_init_set (&aplNonNeg, &aplRht, iHCDimRes);
        arbX_init_set (&aplNonPos, &aplRht, iHCDimRes);

        // Separate the Positive and Negative Real parts
        arb_nonnegative_part (&aplNonNeg.parts[0], &aplRht.parts[0]);
        arb_nonpositive_part (&aplNonPos.parts[0], &aplRht.parts[0]);

#ifdef DEBUG
        if (guTest > 0)
        {
            ArbOut (L"NN.parts[0] = ", &aplNonNeg.parts[0]);
            ArbOut (L"NP.parts[0] = ", &aplNonPos.parts[0]);
        } // End IF
#endif

        // Calculate the Sqrt on the individual parts
        aplResNN = SqrtBAxF_RE (aplNonNeg, iHCDimRes);
        aplResNP = SqrtBAxF_RE (aplNonPos, iHCDimRes);

        // Split cases based upon the HC Dimension (1, 2, 4, 8)
        switch (iHCDimRes)
        {
            // This case is handled by PrimFnMonRootBA1FisBA1F
////////////case 1:
////////////    // Join the two results
////////////    arb_union (&aplRes, aplResNN.parts[0], aplResNP.parts[0]);
////////////
////////////    break;

            case 2:
                // Join the two results
                aplRes2 =
                  ArbUnionBA2F_RE (aplResNN.partsLo.partsLo, aplResNP.partsLo.partsLo, &PrimSpecDoubleTildeUnion);

                // Save in the result
                arb_set (&aplRes.parts[0], &aplRes2.parts[0]);
                arb_set (&aplRes.parts[1], &aplRes2.parts[1]);

                // We no longer need this storage
                arbX_clear (&aplRes2, 2);

                break;

            case 4:
                // Join the two results
                aplRes4 =
                  ArbUnionBA4F_RE (aplResNN.partsLo        , aplResNP.partsLo       , &PrimSpecDoubleTildeUnion);

                // Save in the result
                arb_set (&aplRes.parts[0], &aplRes4.parts[0]);
                arb_set (&aplRes.parts[1], &aplRes4.parts[1]);
                arb_set (&aplRes.parts[2], &aplRes4.parts[2]);
                arb_set (&aplRes.parts[3], &aplRes4.parts[3]);

                // We no longer need this storage
                arbX_clear (&aplRes4, 4);

                break;

            case 8:
                // Join the two results
                aplRes8 =
                  ArbUnionBA8F_RE (aplResNN                , aplResNP               , &PrimSpecDoubleTildeUnion);

                // Save in the result
                arb_set (&aplRes.parts[0], &aplRes8.parts[0]);
                arb_set (&aplRes.parts[1], &aplRes8.parts[1]);
                arb_set (&aplRes.parts[2], &aplRes8.parts[2]);
                arb_set (&aplRes.parts[3], &aplRes8.parts[3]);
                arb_set (&aplRes.parts[4], &aplRes8.parts[4]);
                arb_set (&aplRes.parts[5], &aplRes8.parts[5]);
                arb_set (&aplRes.parts[6], &aplRes8.parts[6]);
                arb_set (&aplRes.parts[7], &aplRes8.parts[7]);

                // We no longer need this storage
                arbX_clear (&aplRes8, 8);

                break;

            defstop
                break;
        } // End SWITCH
    } else
    {
        // Calculate the sum of the squares of both the entire arg and
        //   just the imaginary parts
        Myarb_init (&aplIMag);

        // Initialize to 0/0
        Myarb_init (&aplRMag);
        Myarb_init (&aplTmp);
        Myarb_init (&aplU);
        Myarb_init (&aplV);

        // Calculate the square of the real part
////////aplRMag = MulBA1F_RE (aplRht.parts[0], aplRht.parts[0]);
        arb_sqr2 (&aplRMag, &aplRht.parts[0], prec);
#ifdef DEBUG
        if (guTest > 0)
            ArbOut (L"aplRMag      = ", &aplRMag);
#endif

        // Loop through the imaginary parts
        for (i = 1; i < iHCDimRes; i++)
        {
            // Calculate square of the imaginary part
////////////aplTmp = MulBA1F_RE (aplRht.parts[i], aplRht.parts[i]);
            arb_sqr2 (&aplTmp, &aplRht.parts[i], prec);

            // Calculate the sum of the squares
            arb_add (&aplIMag, &aplIMag, &aplTmp, prec);
        } // End FOR

        // We no longer need this resource
        Myarb_clear (&aplTmp);

#ifdef DEBUG
        if (guTest > 0)
            ArbOut (L"aplIMag      = ", &aplIMag);
#endif

        // Add the square of the real part to the sum of the squares of the imaginary parts
        //   to get the sum of the squares of the entire arg
        arb_add  (&aplRMag, &aplRMag, &aplIMag, prec);
#ifdef DEBUG
        if (guTest > 0)
            ArbOut (L"aplRMag      = ", &aplRMag);
#endif

        // Calculate the magnitude of the entire arg and
        //   the magnitude of just the imaginary parts
        arb_sqrtpos (&aplIMag, &aplIMag, prec);                   // g
#ifdef DEBUG
        if (guTest > 0)
            ArbOut (L"sqrt aplIMag = ", &aplIMag);
#endif
        arb_sqrtpos (&aplRMag, &aplRMag, prec);                   // m
#ifdef DEBUG
        if (guTest > 0)
            ArbOut (L"sqrt aplRMag = ", &aplRMag);
#endif
        arb_sqrtpos (&aplU   , &aplRMag, prec);                   // u
#ifdef DEBUG
        if (guTest > 0)
            ArbOut (L"sqrt aplU    = ", &aplU);
#endif

        // If the imaginary parts are all 0 or a NaN, ...
        if (IsArb0 (&aplIMag) || arb_nan_p (&aplIMag))
        {
            // Initialize to 0/0
            arbX_init (&aplRes, iHCDimRes);

            // If the real part of the right arg is non-positive, ...
            if (arb_sign (&aplRht.parts[0]) <= 0)
            {
                // If it's not a Complex result as yet, ...
                if (iHCDimRes < 2)
                {
                    // Mark as in error
                    bError = TRUE;

                    goto ERROR_EXIT;
                } else
                {
                    // Set the real part to 0/0
                    arb_zero (&aplRes.parts[0]);

                    // Calculate the absolute value of the right arg real part
                    arb_abs2 (&aplV, &aplRht.parts[0]);

#ifdef DEBUG
                    if (guTest > 0)
                    {
                        ArbOut (L"abs Rht.parts[0] = ", &aplV);
                    } // End IF
#endif
                    // Calculate the real Sqrt of the absolute value of the right arg real part
                    arb_sqrtpos (&aplRes.parts[1], &aplV, prec);
#ifdef DEBUG
                    if (guTest > 0)
                    {
                        ArbOut (L"Res.parts[0] = ", &aplRes.parts[0]);
                        ArbOut (L"Res.parts[1] = ", &aplRes.parts[1]);
                    } // End IF
#endif
                } // End IF/ELSE
            } else
            {
#ifdef DEBUG
                if (guTest > 0)
                {
                    ArbOut (L"Rht.parts[0] = ", &aplRht.parts[0]);
                    ArbOut (L"Rht.parts[1] = ", &aplRht.parts[1]);
                } // End IF
#endif
                // Calculate the real Sqrt of the right arg real & imag parts
                arb_sqrt    (&aplRes.parts[0], &aplRht.parts[0], prec);
                arb_sqrt    (&aplRes.parts[1], &aplIMag        , prec);
#ifdef DEBUG
                if (guTest > 0)
                {
                    ArbOut (L"Res.parts[0] = ", &aplRes.parts[0]);
                    ArbOut (L"Res.parts[1] = ", &aplRes.parts[1]);
                } // End IF
#endif
            } // End IF/ELSE
        } else
        {
            // Calculate atan2 (g, h) /2
            arb_atan2  (&aplV, &aplIMag, &aplRht.parts[0], prec);

#ifdef DEBUG
            if (guTest > 0)
                ArbOut (L"aplV         = ", &aplV);
#endif

            arb_div_si (&aplV, &aplV, 2, prec);

#ifdef DEBUG
            if (guTest > 0)
                ArbOut (L"aplV / 2     = ", &aplV);
#endif

            // Calculate cos (v)
            aplTmp = cosArb (aplV, prec);

#ifdef DEBUG
            if (guTest > 0)
                ArbOut (L"cosArb       = ", &aplTmp);
#endif
            // Calculate u * cos (v)
            aplRes.parts[0] = MulBA1F_RE (aplU, aplTmp);                    // p

#ifdef DEBUG
            if (guTest > 0)
                ArbOut (L"cosArb * u   = ", &aplRes.parts[0]);
#endif

            // We no longer need this storage
            Myarb_clear (&aplTmp);

            // If the imaginary parts are all 0, ...
            if (IsArb0 (&aplIMag))
            {
                // Loop through all but the 1st of the imaginary parts
                for (i = 2; i < iHCDimRes; i++)
                    // Initialize to 0
                    Myarb_init (&aplRes.parts[i]);

                // If the real part is non-positive, ...
                if (arb_sign (&aplRht.parts[0]) <= 0)
                {
                    // Set the real part to 0
                    arb_set_si (&aplRes.parts[0], 0);

                    // Set the 1st imaginary part to aplU
                    arb_init_set (&aplRes.parts[1], &aplU);
                } else
                    // Initialize the 1st imaginary part to 0
                    Myarb_init (&aplRes.parts[1]);
            } else
            {
                EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

                //  Calculate sin (v)
                aplTmp = sinArb (aplV, prec);

#ifdef DEBUG
                if (guTest > 0)
                    ArbOut (L"sinArb       = ", &aplTmp);
#endif
                __try
                {
                    // Handle the case where aplIMag contains 0
                    //   by dividing by the Midpoint, not the entire interval
                    APLARB aplIMagMid = {0};

                    // Initialize
                    Myarb_init (&aplIMagMid);

                    // Copy aplIMag
                    arb_set (&aplIMagMid, &aplIMag);

                    // Zero its Radius
                    mag_zero (arb_radref (&aplIMagMid));

                    // Calculate sin (v) / g
                    aplMul = DivBA1F_RE (aplTmp, aplIMagMid);

#ifdef DEBUG
                    if (guTest > 0)
                        ArbOut (L"sinArb/g     = ", &aplMul);
#endif
                    // We no longer need this storage
                    Myarb_clear (&aplTmp);

                    // Calculate u * sin (v) / g
                    aplTmp = MulBA1F_RE (aplU, aplMul);                         // q

#ifdef DEBUG
                    if (guTest > 0)
                        ArbOut (L"u * sinArb/g = ", &aplMul);
#endif
                    // Loop through the imaginary parts
                    for (i = 1; i < iHCDimRes; i++)
                        // Multiply each of the imaginary parts by the u * sin (arctan2) / g
                        aplRes.parts[i] = MulBA1F_RE (aplRht.parts[i], aplTmp);
                    // We no longer need this storage
                    Myarb_clear (&aplIMagMid);
                } __except (CheckException (GetExceptionInformation (), WFCN))
                {
                    exCode = MyGetExceptionCode ();  // The exception code

                    dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

                    // Check the exception code in a helper function
                    CheckExCodeHelper (&exCode);
                } // End __try/__except

                // We no longer need this storage
                Myarb_clear (&aplTmp);
                Myarb_clear (&aplMul);

                // Check the exception code in a main function
                CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
            } // End IF/ELSE
        } // End IF/ELSE
ERROR_EXIT:
        // We no longer need this storage
        Myarb_clear (&aplV);
        Myarb_clear (&aplU);
        Myarb_clear (&aplIMag);
        Myarb_clear (&aplRMag);

        if (bError)
            // Make it so
            RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    } // End IF/ELSE

    return aplRes;
} // End SqrtBAxF_RE


//***************************************************************************
//  $PrimFnMonRootBA2FisBA2F
//
//  Primitive scalar function monadic Root:  BA2F {is} fn BA2F
//***************************************************************************

void PrimFnMonRootBA2FisBA2F
    (LPAPLBA2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = SqrtBAxF_RE (lpatRht->aplBA8F, 2).partsLo.partsLo;
} // End PrimFnMonRootBA2FisBA2F


//***************************************************************************
//  $PrimFnMonRootHC4VisHC4R
//
//  Primitive scalar function monadic Root:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonRootHC4VisHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonRootHC4VisHC4V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
} // End PrimFnMonRootHC4VisHC4R


//***************************************************************************
//  $PrimFnMonRootHC4VisHC4V
//
//  Primitive scalar function monadic Root:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonRootHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call the subfunction
    lpMemRes[uRes] = SqrtHCxV_RE (lpatRht->aplHC8V, 4).partsLo;
} // End PrimFnMonRootHC4VisHC4V


//***************************************************************************
//  $PrimFnMonRootBA4FisBA4F
//
//  Primitive scalar function monadic Root:  BA4F {is} fn BA4F
//***************************************************************************

void PrimFnMonRootBA4FisBA4F
    (LPAPLBA4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = SqrtBAxF_RE (lpatRht->aplBA8F, 4).partsLo;
} // End PrimFnMonRootBA4FisBA4F


//***************************************************************************
//  $PrimFnMonRootHC8VisHC8R
//
//  Primitive scalar function monadic Root:  HC8V {is} fn HC8R
//***************************************************************************

void PrimFnMonRootHC8VisHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonRootHC8VisHC8V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc8v_clear (&atRht.aplHC8V);
} // End PrimFnMonRootHC8VisHC8R


//***************************************************************************
//  $PrimFnMonRootHC8VisHC8V
//
//  Primitive scalar function monadic Root:  HC8V {is} fn HC8V
//***************************************************************************

void PrimFnMonRootHC8VisHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call the subfunction
    lpMemRes[uRes] = SqrtHCxV_RE (lpatRht->aplHC8V, 8);
} // End PrimFnMonRootHC8VisHC8V


//***************************************************************************
//  $PrimFnMonRootBA8FisBA8F
//
//  Primitive scalar function monadic Root:  BA8F {is} fn BA8F
//***************************************************************************

void PrimFnMonRootBA8FisBA8F
    (LPAPLBA8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = SqrtBAxF_RE (lpatRht->aplBA8F, 8);
} // End PrimFnMonRootBA8FisBA8F


//***************************************************************************
//  $PrimSpecRootStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecRootStorageTypeDyd
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

        case ARRAY_ARB:
        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:

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
} // End PrimSpecRootStorageTypeDyd


//***************************************************************************
//  $PrimFnDydRootBisBvB
//
//  Primitive scalar function dyadic Root:  B {is} B fn B
//***************************************************************************

void PrimFnDydRootBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLFLOAT aplFloatRes;
    UBOOL    bRet;

    // Check for indeterminates:  0 {root} 0 <==> 0 * _
    if (lpatLft->aplBoolean EQ 0
     && lpatRht->aplBoolean EQ 0)
    {
        // See what the []IC oracle has to say
        aplFloatRes =
          TranslateQuadICIndex ((APLFLOAT) lpatLft->aplBoolean,
                                           ICNDX_0EXPPi,
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
    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (lpatLft->aplBoolean EQ 1)
        bRet = lpatRht->aplBoolean;
    else
        // It's 0 {root} 1 <==> 1 * _ <==> 1
        bRet = 1;

    lpMemRes[uRes >> LOG2NBIB] |=
      bRet
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydRootBisBvB


//***************************************************************************
//  $PrimFnDydRootIisIvI
//
//  Primitive scalar function dyadic Root:  I {is} I fn I
//***************************************************************************

void PrimFnDydRootIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLFLOAT aplFloatRes;

    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (lpatLft->aplInteger EQ 1)
    {
        lpMemRes[uRes] = lpatRht->aplInteger;

        return;
    } // End IF

    // Check for indeterminates and special cases:  0 {root} R
    if (lpatLft->aplInteger EQ 0)
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (lpatRht->aplInteger <= -1)
        {
            aplFloatRes = TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                                           ICNDX_NEXPPi,
                                                (APLFLOAT) lpatRht->aplInteger,
                                                           FALSE);
            // If aplFloatRes can't be expressed as an integer, ...
            if (_isinf (aplFloatRes))
                RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
            else
                lpMemRes[uRes] = (APLINT) aplFloatRes;

            return;
        } else
////////// Check for special case:  0 {root} R where -1 < R < 0 <==> 0
////////if (-1 < lpatRht->aplInteger
//////// &&      lpatRht->aplInteger < 0)
////////{
////////    lpMemRes[uRes] = 0;
////////
////////    return;
////////} else
        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (lpatRht->aplInteger EQ 0)
        {
            aplFloatRes = TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                                           ICNDX_0EXPPi,
                                                (APLFLOAT) lpatRht->aplInteger,
                                                           FALSE);
            // If aplFloatRes can't be expressed as an integer, ...
            if (_isinf (aplFloatRes))
                RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
            else
                lpMemRes[uRes] = (APLINT) aplFloatRes;

            return;
        } else
////////// Check for special case:  0 {root} R where 0 < R < 1
////////if (0 < lpatRht->aplInteger
//////// &&     lpatRht->aplInteger < 1)
////////{
////////    lpMemRes[uRes] = 0;
////////
////////    return;
////////} else
////////// Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
////////if (lpatRht->aplInteger > 1)
////////    RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
////////else
            lpMemRes[uRes] = 2;        // Any non-Boolean value

        // If the result is Boolean
        if (lpMemRes[uRes] EQ 0)
        {
            lpMemRes[uRes] = 0;

            return;
        } else
        if (lpMemRes[uRes] EQ 1)
        {
            lpMemRes[uRes] = 1;

            return;
        } else
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    // Otherwise, let the FisFvF code handle it
    RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
} // End PrimFnDydRootIisIvI


//***************************************************************************
//  $PrimFnDydRootFisIvI
//
//  Primitive scalar function dyadic Root:  F {is} I fn I
//***************************************************************************

void PrimFnDydRootFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  0 * _
    if (lpatLft->aplInteger EQ 0
     && lpatRht->aplInteger EQ 0)
        lpMemRes[uRes] = TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                                          ICNDX_0EXPPi,
                                               (APLFLOAT) lpatRht->aplInteger,
                                                          FALSE);
    else
    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (lpatLft->aplInteger EQ 1)
        lpMemRes[uRes] = (APLFLOAT) lpatRht->aplInteger;
    else
    // Check for indeterminates and special cases:  0 {root} R
    if (lpatLft->aplInteger EQ 0)
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (lpatRht->aplInteger <= -1)
            lpMemRes[uRes] = TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                                              ICNDX_NEXPPi,
                                                   (APLFLOAT) lpatRht->aplInteger,
                                                              FALSE);
////////else
////////// Check for special case:  0 {root} R where -1 < R < 0 <==> 0
////////if (-1 < lpatRht->aplInteger
//////// &&      lpatRht->aplInteger < 0)
////////    return 0;
        else
        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (lpatRht->aplInteger EQ 0)
            lpMemRes[uRes] = TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
                                                              ICNDX_0EXPPi,
                                                   (APLFLOAT) lpatRht->aplInteger,
                                                              FALSE);
////////else
////////// Check for special case:  0 {root} R where 0 < R < 1
////////if (0 < lpatRht->aplInteger
//////// &&     lpatRht->aplInteger < 1)
////////    lpMemRes[uRes] = 0;
        else
        // Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
        if (lpatRht->aplInteger > 1)
            lpMemRes[uRes] = fltPosInfinity;
    } // End IF
////else
////// Check for indeterminate:  PoM_ {root} 0 <==> 0 * 0
////if (_isinf (lpatLft->aplInteger)
//// && lpatRht->aplInteger EQ 0)
////    lpMemRes[uRes] = TranslateQuadICIndex ((APLFLOAT) lpatLft->aplInteger,
////                                                      ICNDX_0EXP0,
////                                           (APLFLOAT) lpatRht->aplInteger,
////                                                      FALSE);
    else
    // Check for Complex result
    if (lpatRht->aplInteger < 0)
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
        // Calculate the root
        lpMemRes[uRes] = pow ((APLFLOAT) lpatRht->aplInteger, 1 / (APLFLOAT) lpatLft->aplInteger);
} // End PrimFnDydRootFisIvI


//***************************************************************************
//  $PrimFnDydRootFisFvF
//
//  Primitive scalar function dyadic Root:  F {is} F fn F
//***************************************************************************

void PrimFnDydRootFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  0 * _
    if (lpatLft->aplFloat EQ 0
     && lpatRht->aplFloat EQ 0)
        lpMemRes[uRes] = TranslateQuadICIndex (lpatLft->aplFloat,
                                     ICNDX_0EXPPi,
                                     lpatRht->aplFloat,
                                     FALSE);
    else
    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (lpatLft->aplFloat EQ 1)
        lpMemRes[uRes] = lpatRht->aplFloat;
    else
    // Check for indeterminates and special cases:  0 {root} R
    if (lpatLft->aplFloat EQ 0)
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (lpatRht->aplFloat <= -1)
            lpMemRes[uRes] = TranslateQuadICIndex (lpatLft->aplFloat,
                                                   ICNDX_NEXPPi,
                                                   lpatRht->aplFloat,
                                                   FALSE);
        else
        // Check for special case:  0 {root} R where -1 < R < 0 <==> 0
        if (-1 < lpatRht->aplFloat
         &&      lpatRht->aplFloat < 0)
            lpMemRes[uRes] = 0;
        else
        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (lpatRht->aplFloat EQ 0)
            lpMemRes[uRes] = TranslateQuadICIndex (lpatLft->aplFloat,
                                                   ICNDX_0EXPPi,
                                                   lpatRht->aplFloat,
                                                   FALSE);
        else
        // Check for special case:  0 {root} R where 0 < R < 1
        if (0 < lpatRht->aplFloat
         &&     lpatRht->aplFloat < 1)
            lpMemRes[uRes] = 0;
        else
        // Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
        if (lpatRht->aplFloat > 1)
            lpMemRes[uRes] = fltPosInfinity;
    } // End IF
    else
    // Check for indeterminate:  PoM_ {root} 0 <==> 0 * 0
    if (_isinf (lpatLft->aplFloat)
     && lpatRht->aplFloat EQ 0)
        lpMemRes[uRes] = TranslateQuadICIndex (lpatLft->aplFloat,
                                               ICNDX_0EXP0,
                                               lpatRht->aplFloat,
                                               FALSE);
    else
    // Check for Complex result
    if (lpatRht->aplFloat < 0)    // Not SIGN_APLFLOAT as that'll catch -0 whose root is -0
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    else
    // Handle L {root} -0
    if (lpatRht->aplFloat EQ 0)
    {
        // If L < 0, ...
        if (lpatLft->aplFloat < 0)
            lpMemRes[uRes] = SIGN_APLFLOAT (lpatRht->aplFloat) ? fltNegInfinity
                                                               : fltPosInfinity;
        else
            lpMemRes[uRes] = lpatRht->aplFloat;
    } else
        // Calculate the root
        lpMemRes[uRes] = pow (lpatRht->aplFloat, 1 / lpatLft->aplFloat);
} // End PrimFnDydRootFisFvF


//***************************************************************************
//  $PrimFnDydRootVisRvR
//
//  Primitive scalar function dyadic Root:  V {is} R fn R
//***************************************************************************

void PrimFnDydRootVisRvR
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

    PrimFnDydRootVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydRootVisRvR


//***************************************************************************
//  $PrimFnDydRootVisVvV
//
//  Primitive scalar function dyadic Root:  V {is} V fn V
//***************************************************************************

void PrimFnDydRootVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  0 * _
    if (IsMpf0 (&lpatLft->aplVfp)
     && IsMpf0 (&lpatRht->aplVfp))
        mpfr_QuadICValue (&lpatLft->aplVfp,
                           ICNDX_0EXPPi,
                          &lpatRht->aplVfp,
                          &lpMemRes[uRes],
                           FALSE);
    else
    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (IsMpf1 (&lpatLft->aplVfp))
        mpfr_init_copy (&lpMemRes[uRes], &lpatRht->aplVfp);
    else
    // Check for indeterminates and special cases:  0 {root} R
    if (IsMpf0 (&lpatLft->aplVfp))
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (mpfr_cmp_si (&lpatRht->aplVfp, -1) <= 0)  // R <= -1
            mpfr_QuadICValue (&lpatLft->aplVfp,
                               ICNDX_NEXPPi,
                              &lpatRht->aplVfp,
                              &lpMemRes[uRes],
                               FALSE);
        else
        // Check for special case:  0 {root} R where -1 < R < 0 <==> 0
        if (mpfr_si_cmp (-1, &lpatRht->aplVfp   ) < 0 // -1 < R
         && mpfr_cmp_si (    &lpatRht->aplVfp, 0) < 0)    //      R < 0)
        {
            // Initialize to 0
            mpfr_init0 (&lpMemRes[uRes]);

            lpMemRes[uRes] = lpMemRes[uRes];
        } // End IF
        else
        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (IsMpf0 (&lpatRht->aplVfp))
            mpfr_QuadICValue (&lpatLft->aplVfp,
                               ICNDX_0EXPPi,
                              &lpatRht->aplVfp,
                              &lpMemRes[uRes],
                               FALSE);
        else
        // Check for special case:  0 {root} R where 0 < R < 1
        if (mpfr_ui_cmp (0, &lpatRht->aplVfp   ) < 0  // 0 < R
         && mpfr_cmp_ui (   &lpatRht->aplVfp, 1) < 0) //     R < 1
        {
            mpfr_init0 (&lpMemRes[uRes]);
            lpMemRes[uRes] = lpMemRes[uRes];
        } // End IF
        else
        // Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
        if (mpfr_cmp_ui (&lpatRht->aplVfp, 1) > 0)    // R > 1
            lpMemRes[uRes] = mpfPosInfinity;
    } // End IF
    else
    // Check for indeterminate:  PoM_ {root} 0 <==> 0 * 0
    if (mpfr_inf_p (&lpatLft->aplVfp)
     && IsMpf0 (&lpatRht->aplVfp))
        mpfr_QuadICValue (&lpatLft->aplVfp,
                           ICNDX_0EXP0,
                          &lpatRht->aplVfp,
                          &lpMemRes[uRes],
                           FALSE);
    else
    // Check for Complex result
    if (mpfr_cmp_ui (&lpatRht->aplVfp, 0) < 0)        // R < 0:  Not SIGN_APLVFP as that'll catch -0 whose root is -0
        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    else
    // Handle L {root} -0
    if (IsMpf0 (&lpatRht->aplVfp))
    {
        // If L < 0, ...
        if (mpfr_sgn (&lpatLft->aplVfp) < 0)
        {
            // Initialize the result to NaN
            mpfr_init (&lpMemRes[uRes]);

            // Set it to +/- Infinity
            mpfr_set_inf (&lpMemRes[uRes], SIGN_APLVFP (&lpatRht->aplVfp) ? -1 : 1);
        } else
            mpfr_init_copy (&lpMemRes[uRes], &lpatRht->aplVfp);
    } else
    {
        APLVFP mpfTmp;

        // Nth root (V) = exp (ln (V) / N)      for V > 0

        // Calculate ln (V)
        PrimFnMonCircleStarVisV (&mpfTmp, 0, lpatRht, NULL);

        // Divide by N
        mpfr_div (&mpfTmp, &mpfTmp, &lpatLft->aplVfp, MPFR_RNDN);

        // Exp that
        lpMemRes[uRes] = ExpVfp_RE (mpfTmp);

        // We no longer need this storage
        Myf_clear (&mpfTmp);
    } // End IF/ELSE
} // End PrimFnDydRootVisVvV


//***************************************************************************
//  $PrimFnDydRootBA1FisBA1FvBA1F
//
//  Primitive scalar function dyadic Root:  BA1F {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydRootBA1FisBA1FvBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  0 * _
    if (IsArb0 (&lpatLft->aplArb)
     && IsArb0 (&lpatRht->aplArb))
        arb_QuadICValue (&lpatLft->aplArb,
                          ICNDX_0EXPPi,
                         &lpatRht->aplArb,
                         &lpMemRes[uRes],
                          FALSE);
    else
    // Check for special case:  1 {root} R <==> R * 1 <==> R
    if (IsArb1 (&lpatLft->aplArb))
        arb_init_set (&lpMemRes[uRes], &lpatRht->aplArb);
    else
    // Check for indeterminates and special cases:  0 {root} R
    if (IsArb0 (&lpatLft->aplArb))
    {
        // Check for indeterminate:  0 {root} R where R <= -1
        if (arb_le (&lpatRht->aplArb, &arb1f_N1)) // R <= -1
            arb_QuadICValue (&lpatLft->aplArb,
                              ICNDX_NEXPPi,
                             &lpatRht->aplArb,
                             &lpMemRes[uRes],
                              FALSE);
        else
        // Check for special case:  0 {root} R where -1 < R < 0 <==> 0
        if (arb_lt (&arb1f_N1, &lpatRht->aplArb)            // -1 < R
         && arb_lt (           &lpatRht->aplArb, &arb1f_0)) //      R < 0)
        {
            // Initialize to 0/0
            Myarb_init (&lpMemRes[uRes]);

            lpMemRes[uRes] = lpMemRes[uRes];
        } // End IF
        else
        // Check for indeterminate:  0 {root} 0 <==> 0 * _
        if (IsArb0 (&lpatRht->aplArb))
            arb_QuadICValue (&lpatLft->aplArb,
                              ICNDX_0EXPPi,
                             &lpatRht->aplArb,
                             &lpMemRes[uRes],
                              FALSE);
        else
        // Check for special case:  0 {root} R where 0 < R < 1
        if (arb_lt (&arb1f_0, &lpatRht->aplArb)             // 0 < R
         && arb_lt (          &lpatRht->aplArb, &arb1f_1))  //     R < 1
        {
            // Initialize to 0/0
            Myarb_init (&lpMemRes[uRes]);
            lpMemRes[uRes] = lpMemRes[uRes];
        } // End IF
        else
        // Check for special case:  0 {root} R where R > 1 <==> R * _ <==> _
        if (arb_gt (&lpatRht->aplArb, &arb1f_1))    // R > 1
            lpMemRes[uRes] = arbPosInfinity;
    } else
    // Check for indeterminate:  PoM_ {root} 0 <==> 0 * 0
    if (arb_inf_p (&lpatLft->aplArb)
     && IsArb0 (&lpatRht->aplArb))
        arb_QuadICValue (&lpatLft->aplArb,
                          ICNDX_0EXP0,
                         &lpatRht->aplArb,
                         &lpMemRes[uRes],
                          FALSE);
    else
    // Check for Complex result
    if (arb_contains_negative (&lpatRht->aplArb))           // R < 0:  Not SIGN_APLARB as that'll catch -0 whose root is -0
        RaiseException (EXCEPTION_RESULT_BA2F, 0, 0, NULL);
    else
    // Handle L {root} -0
    if (IsArb0 (&lpatRht->aplArb))
    {
        // If L < 0, ...
        if (arb_sign (&lpatLft->aplArb) < 0)
        {
            // Initialize the result to 0
            Myarb_init (&lpMemRes[uRes]);

            // Set it to +/- Infinity
            arb_set_inf  (&lpMemRes[uRes], SIGN_APLARB (&lpatRht->aplArb) ? -1 : 1);
        } else
            arb_init_set (&lpMemRes[uRes], &lpatRht->aplArb);
    } else
    {
        APLARB mpfTmp = {0};
        mp_limb_signed_t prec = ARB_PREC_FPC;

        // Nth root (V) = exp (ln (V) / N)      for V > 0
#ifdef DEBUG
        if (guTest > 0)
            ArbOut (L"R = ", &lpatRht->aplArb);
#endif

        // Calculate ln (V)
        PrimFnMonCircleStarBA1FisBA1F (&mpfTmp, 0, lpatRht, NULL);
#ifdef DEBUG
        if (guTest > 0)
            ArbOut (L"ln (R) = ", &mpfTmp);
#endif

        // Divide by N
        arb_div (&mpfTmp, &mpfTmp, &lpatLft->aplArb, prec);
#ifdef DEBUG
        if (guTest > 0)
            ArbOut (L"ln (R) / L = ", &mpfTmp);
#endif

        // Exp that
        lpMemRes[uRes] = ExpArb_RE (mpfTmp);

#ifdef DEBUG
        if (guTest > 0)
            ArbOut (L"*ln (R) / L = ", &lpMemRes[uRes]);
#endif
        // We no longer need this storage
        Myarb_clear (&mpfTmp);
    } // End IF/ELSE
} // End PrimFnDydRootBA1FisBA1FvBA1F


//***************************************************************************
//  $PrimFnDydRootHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic Root:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydRootHC2FisHC2IvHC2I
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
    PrimFnDydRootHC2FisHC2FvHC2F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydRootHC2FisHC2IvHC2I


//***************************************************************************
//  $RootHC2F_RE
//***************************************************************************

APLHC2F RootHC2F_RE
    (APLHC2F aplLft,                // Left arg
     APLHC2F aplRht)                // Right ...

{
    // Exp ((Log R) / L)
    return ExpHC2F_RE (DivHC2F_RE (LogHCxF_RE (*(LPAPLHC8F) &aplRht, 2).partsLo[0].partsLo[0], aplLft));
} // End RootHC2F_RE


//***************************************************************************
//  $PrimFnDydRootHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic Root:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydRootHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = RootHC2F_RE (lpatLft->aplHC2F, lpatRht->aplHC2F);
} // End PrimFnDydRootHC2FisHC2FvHC2F


//***************************************************************************
//  $PrimFnDydRootHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic Root:  HC2V {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydRootHC2VisHC2RvHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the left & right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatLft->aplRat, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnDydRootHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
    Myhc2v_clear (&atLft.aplHC2V);
} // End PrimFnDydRootHC2VisHC2RvHC2R


//***************************************************************************
//  $RootHC2V_RE
//***************************************************************************

APLHC2V RootHC2V_RE
    (APLHC2V aplLft,                // Left arg
     APLHC2V aplRht)                // Right ...

{
    APLHC2V aplRes,                 // The result
            aplLog,                 // Log (R)
            aplDiv;                 // (Log (R) / L

    // Exp ((Log R) / L)
////return ExpHC2V_RE (DivHC2V_RE (LogHCxV_RE (*(LPAPLHC8V) &aplRht, 2).partsLo.partsLo, aplLft));

    // Calcultae Log (R)
    aplLog = LogHCxV_RE (*(LPAPLHC8V) &aplRht, 2).partsLo.partsLo;

    // Calculate Log (R) / L
    aplDiv = DivHC2V_RE (aplLog, aplLft);

    // Calculate Exp ((Log (R) / L)
    aplRes = ExpHC2V_RE (aplDiv);

    // We no longer need this storage
    Myhc2v_clear (&aplDiv);
    Myhc2v_clear (&aplLog);

    return aplRes;
} // End RootHC2V_RE


//***************************************************************************
//  $PrimFnDydRootHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic Root:  HC2V {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydRootHC2VisHC2VvHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = RootHC2V_RE (lpatLft->aplHC2V, lpatRht->aplHC2V);
} // End PrimFnDydRootHC2VisHC2VvHC2V


//***************************************************************************
//  $RootBA2F_RE
//***************************************************************************

APLBA2F RootBA2F_RE
    (APLBA2F aplLft,                // Left arg
     APLBA2F aplRht)                // Right ...

{
    APLBA2F aplRes = {0},           // The result
            aplLog,                 // Log (R)
            aplDiv;                 // (Log (R) / L

    // Exp ((Log R) / L)
////return ExpBA2F_RE (DivBA2F_RE (LogHCxV_RE (*(LPAPLHC8V) &aplRht, 2).partsLo.partsLo, aplLft));

    // Calcultae Log (R)
    aplLog = LogBAxF_RE (*(LPAPLBA8F) &aplRht, 2).partsLo.partsLo;

    // Calculate Log (R) / L
    aplDiv = DivBA2F_RE (aplLog, aplLft);

    // Calculate Exp ((Log (R) / L)
    aplRes = ExpBA2F_RE (aplDiv);

    // We no longer need this storage
    arb2f_clear (&aplDiv);
    arb2f_clear (&aplLog);

    return aplRes;
} // End RootBA2F_RE


//***************************************************************************
//  $PrimFnDydRootBA2FisBA2FvBA2F
//
//  Primitive scalar function dyadic Root:  BA2F {is} BA2F fn BA2F
//***************************************************************************

void PrimFnDydRootBA2FisBA2FvBA2F
    (LPAPLBA2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = RootBA2F_RE (lpatLft->aplBA2F, lpatRht->aplBA2F);
} // End PrimFnDydRootBA2FisBA2FvBA2F


//***************************************************************************
//  $PrimFnDydRootHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic Root:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydRootHC4FisHC4IvHC4I
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
    PrimFnDydRootHC4FisHC4FvHC4F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydRootHC4FisHC4IvHC4I


//***************************************************************************
//  $RootHC4F_RE
//***************************************************************************

APLHC4F RootHC4F_RE
    (APLHC4F aplLft,                // Left arg
     APLHC4F aplRht)                // Right ...

{
    // Exp ((Log R) / L)
    return ExpHC4F_RE (DivHC4F_RE (LogHCxF_RE (*(LPAPLHC8F) &aplRht, 4).partsLo[0], aplLft));
} // End RootHC4F_RE


//***************************************************************************
//  $PrimFnDydRootHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic Root:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydRootHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = RootHC4F_RE (lpatLft->aplHC4F, lpatRht->aplHC4F);
} // End PrimFnDydRootHC4FisHC4FvHC4F


//***************************************************************************
//  $PrimFnDydRootHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic Root:  HC4V {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydRootHC4VisHC4RvHC4R
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
    PrimFnDydRootHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydRootHC4VisHC4RvHC4R


//***************************************************************************
//  $RootHC4V_RE
//***************************************************************************

APLHC4V RootHC4V_RE
    (APLHC4V aplLft,                // Left arg
     APLHC4V aplRht)                // Right ...

{
    APLHC4V aplRes,                 // The result
            aplLog,                 // Log (R)
            aplDiv;                 // (Log (R) / L

    // Exp ((Log R) / L)
////return ExpHC4V_RE (DivHC4V_RE (LogHCxV_RE (*(LPAPLHC8V) &aplRht, 4).partsLo.partsLo, aplLft));

    // Calcultae Log (R)
    aplLog = LogHCxV_RE (*(LPAPLHC8V) &aplRht, 4).partsLo;

    // Calculate Log (R) / L
    aplDiv = DivHC4V_RE (aplLog, aplLft);

    // Calculate Exp ((Log (R) / L)
    aplRes = ExpHC4V_RE (aplDiv);

    // We no longer need this storage
    Myhc4v_clear (&aplDiv);
    Myhc4v_clear (&aplLog);

    return aplRes;
} // End RootHC4V_RE


//***************************************************************************
//  $PrimFnDydRootHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic Root:  HC4V {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydRootHC4VisHC4VvHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = RootHC4V_RE (lpatLft->aplHC4V, lpatRht->aplHC4V);
} // End PrimFnDydRootHC4VisHC4VvHC4V


//***************************************************************************
//  $RootBA4F_RE
//***************************************************************************

APLBA4F RootBA4F_RE
    (APLBA4F aplLft,                // Left arg
     APLBA4F aplRht)                // Right ...

{
    APLBA4F aplRes = {0},           // The result
            aplLog,                 // Log (R)
            aplDiv;                 // (Log (R) / L

    // Exp ((Log R) / L)
////return ExpBA4F_RE (DivBA4F_RE (LogHCxV_RE (*(LPAPLHC8V) &aplRht, 4).partsLo.partsLo, aplLft));

    // Calcultae Log (R)
    aplLog = LogBAxF_RE (*(LPAPLBA8F) &aplRht, 4).partsLo;

    // Calculate Log (R) / L
    aplDiv = DivBA4F_RE (aplLog, aplLft);

    // Calculate Exp ((Log (R) / L)
    aplRes = ExpBA4F_RE (aplDiv);

    // We no longer need this storage
    arb4f_clear (&aplDiv);
    arb4f_clear (&aplLog);

    return aplRes;
} // End RootBA4F_RE


//***************************************************************************
//  $PrimFnDydRootBA4FisBA4FvBA4F
//
//  Primitive scalar function dyadic Root:  BA4F {is} BA4F fn BA4F
//***************************************************************************

void PrimFnDydRootBA4FisBA4FvBA4F
    (LPAPLBA4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = RootBA4F_RE (lpatLft->aplBA4F, lpatRht->aplBA4F);
} // End PrimFnDydRootBA4FisBA4FvBA4F


//***************************************************************************
//  $PrimFnDydRootHC8FisHC8IvHC8I
//
//  Primitive scalar function dyadic Root:  HC8F {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydRootHC8FisHC8IvHC8I
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
    PrimFnDydRootHC8FisHC8FvHC8F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydRootHC8FisHC8IvHC8I


//***************************************************************************
//  $RootHC8F_RE
//***************************************************************************

APLHC8F RootHC8F_RE
    (APLHC8F aplLft,                // Left arg
     APLHC8F aplRht)                // Right ...

{
    // Exp ((Log R) / L)
    return ExpHC8F_RE (DivHC8F_RE (LogHCxF_RE (*(LPAPLHC8F) &aplRht, 8), aplLft));
} // End RootHC8F_RE


//***************************************************************************
//  $PrimFnDydRootHC8FisHC8FvHC8F
//
//  Primitive scalar function dyadic Root:  HC8F {is} HC8F fn HC8F
//***************************************************************************

void PrimFnDydRootHC8FisHC8FvHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = RootHC8F_RE (lpatLft->aplHC8F, lpatRht->aplHC8F);
} // End PrimFnDydRootHC8FisHC8FvHC8F


//***************************************************************************
//  $PrimFnDydRootHC8VisHC8RvHC8R
//
//  Primitive scalar function dyadic Root:  HC8V {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydRootHC8VisHC8RvHC8R
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
    PrimFnDydRootHC8VisHC8VvHC8V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydRootHC8VisHC8RvHC8R


//***************************************************************************
//  $RootHC8V_RE
//***************************************************************************

APLHC8V RootHC8V_RE
    (APLHC8V aplLft,                // Left arg
     APLHC8V aplRht)                // Right ...

{
    APLHC8V aplRes,                 // The result
            aplLog,                 // Log (R)
            aplDiv;                 // (Log (R) / L

    // Exp ((Log R) / L)
////return ExpHC8V_RE (DivHC8V_RE (LogHCxV_RE (*(LPAPLHC8V) &aplRht, 8).partsLo.partsLo, aplLft));

    // Calcultae Log (R)
    aplLog = LogHCxV_RE (*(LPAPLHC8V) &aplRht, 8);

    // Calculate Log (R) / L
    aplDiv = DivHC8V_RE (aplLog, aplLft);

    // Calculate Exp ((Log (R) / L)
    aplRes = ExpHC8V_RE (aplDiv);

    // We no longer need this storage
    Myhc8v_clear (&aplDiv);
    Myhc8v_clear (&aplLog);

    return aplRes;
} // End RootHC8V_RE


//***************************************************************************
//  $PrimFnDydRootHC8VisHC8VvHC8V
//
//  Primitive scalar function dyadic Root:  HC8V {is} HC8V fn HC8V
//***************************************************************************

void PrimFnDydRootHC8VisHC8VvHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = RootHC8V_RE (lpatLft->aplHC8V, lpatRht->aplHC8V);
} // End PrimFnDydRootHC8VisHC8VvHC8V


//***************************************************************************
//  $RootBA8F_RE
//***************************************************************************

APLBA8F RootBA8F_RE
    (APLBA8F aplLft,                // Left arg
     APLBA8F aplRht)                // Right ...

{
    APLBA8F aplRes = {0},           // The result
            aplLog,                 // Log (R)
            aplDiv;                 // (Log (R) / L

    // Exp ((Log R) / L)
////return ExpBA8F_RE (DivBA8F_RE (LogHCxV_RE (*(LPAPLHC8V) &aplRht, 8).partsLo.partsLo, aplLft));

    // Calcultae Log (R)
    aplLog = LogBAxF_RE (*(LPAPLBA8F) &aplRht, 8);

    // Calculate Log (R) / L
    aplDiv = DivBA8F_RE (aplLog, aplLft);

    // Calculate Exp ((Log (R) / L)
    aplRes = ExpBA8F_RE (aplDiv);

    // We no longer need this storage
    arb8f_clear (&aplDiv);
    arb8f_clear (&aplLog);

    return aplRes;
} // End RootBA8F_RE


//***************************************************************************
//  $PrimFnDydRootBA8FisBA8FvBA8F
//
//  Primitive scalar function dyadic Root:  BA8F {is} BA8F fn BA8F
//***************************************************************************

void PrimFnDydRootBA8FisBA8FvBA8F
    (LPAPLBA8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = RootBA8F_RE (lpatLft->aplBA8F, lpatRht->aplBA8F);
} // End PrimFnDydRootBA8FisBA8FvBA8F


//***************************************************************************
//  End of File: pf_root.c
//***************************************************************************
