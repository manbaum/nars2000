//***************************************************************************
//  NARS2000 -- Primitive Function -- Plus
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
PRIMSPEC PrimSpecPlus =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecPlusStorageTypeMon,
    &PrimFnMonPlusAPA_EM,

    // Monadic Boolean result functions
    &PrimFnMonPlusBisB,
    NULL,   // &PrimFnMonPlusBisI, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusBisF, -- Can't happen w/Plus

    // Monadic non-Boolean result functions (indexable)
    &PrimFnMonPlusIisI,
    NULL,   // &PrimFnMonPlusIisF, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusFisI, -- Can't happen w/Plus
    &PrimFnMonPlusFisF,
    &PrimFnMonPlusRisR,
    &PrimFnMonPlusVisR,
    &PrimFnMonPlusVisV,

    // Monadic Hypercomplex functions
    &PrimFnMonPlusHC2IisHC2I,
    NULL,   // &PrimFnMonPlusHC2IisHC2F, -- Can't happen w/Plus
    &PrimFnMonPlusHC2FisHC2I,
    &PrimFnMonPlusHC2FisHC2F,
    &PrimFnMonPlusHC2RisHC2R,
    &PrimFnMonPlusHC2VisHC2R,
    &PrimFnMonPlusHC2VisHC2V,

    &PrimFnMonPlusHC4IisHC4I,
    NULL,   // &PrimFnMonPlusHC4IisHC4F, -- Can't happen w/Plus
    &PrimFnMonPlusHC4FisHC4I,
    &PrimFnMonPlusHC4FisHC4F,
    &PrimFnMonPlusHC4RisHC4R,
    &PrimFnMonPlusHC4VisHC4R,
    &PrimFnMonPlusHC4VisHC4V,

    &PrimFnMonPlusHC8IisHC8I,
    NULL,   // &PrimFnMonPlusHC8IisHC8F, -- Can't happen w/Plus
    &PrimFnMonPlusHC8FisHC8I,
    &PrimFnMonPlusHC8FisHC8F,
    &PrimFnMonPlusHC8RisHC8R,
    &PrimFnMonPlusHC8VisHC8R,
    &PrimFnMonPlusHC8VisHC8V,

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonPlusFisHC2I, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusFisHC2F, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusVisHC2R, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusVisHC2V, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusFisHC4I, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusFisHC4F, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusVisHC4R, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusVisHC4V, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusFisHC8I, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusFisHC8F, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusVisHC8R, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusVisHC8V, -- Can't happen w/Plus

    // Monadic BAxF result BAxF arg functions (indexable)
    &PrimFnMonPlusBA1FisBA1F,
    &PrimFnMonPlusBA2FisBA2F,
    &PrimFnMonPlusBA4FisBA4F,
    &PrimFnMonPlusBA8FisBA8F,
    &PrimFnMonPlusBA1FisBA1F,
    NULL,   // &PrimFnMonPlusARBisBA2F, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusARBisBA4F, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusARBisBA8F, -- Can't happen w/Plus

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecPlusStorageTypeDyd,
    &PrimFnDydPlusAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydPlusBisBvB, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisIvI, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisFvF, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisCvC, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisHvH, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisRvR, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisVvV, -- Can't happen w/Plus

    NULL,   // &PrimFnDydPlusBisHC2IvHC2I, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisHC2FvHC2F, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisHC2RvHC2R, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisHC2VvHC2V, -- Can't happen w/Plus

    NULL,   // &PrimFnDydPlusBisHC4IvHC4I, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisHC4FvHC4F, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisHC4RvHC4R, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisHC4VvHC4V, -- Can't happen w/Plus

    NULL,   // &PrimFnDydPlusBisHC8IvHC8I, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisHC8FvHC8F, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisHC8RvHC8R, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisHC8VvHC8V, -- Can't happen w/Plus

    // Dyadic Boolean result functions with ARBs (indexable)
    NULL,   // &PrimFnDydPlusBisBA1FvBA1F, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisBA2FvBA2F, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisBA4FvBA4F, -- Can't happen w/Plus
    NULL,   // &PrimFnDydPlusBisBA8FvBA8F, -- Can't happen w/Plus

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydPlusIisIvI,
    NULL,   // &PrimFnDydPlusIisFvF, -- Can't happen w/Plus
    &PrimFnDydPlusFisIvI,
    &PrimFnDydPlusFisFvF,
    &PrimFnDydPlusRisRvR,
    &PrimFnDydPlusVisRvR,
    &PrimFnDydPlusVisVvV,

    &PrimFnDydPlusHC2IisHC2IvHC2I,
    NULL,   // &PrimFnDydPlusHC2IisHC2FvHC2F, -- Can't happen w/Plus
    &PrimFnDydPlusHC2FisHC2IvHC2I,
    &PrimFnDydPlusHC2FisHC2FvHC2F,
    &PrimFnDydPlusHC2RisHC2RvHC2R,
    &PrimFnDydPlusHC2VisHC2RvHC2R,
    &PrimFnDydPlusHC2VisHC2VvHC2V,

    &PrimFnDydPlusHC4IisHC4IvHC4I,
    NULL,   // &PrimFnDydPlusHC4IisHC4FvHC4F, -- Can't happen w/Plus
    &PrimFnDydPlusHC4FisHC4IvHC4I,
    &PrimFnDydPlusHC4FisHC4FvHC4F,
    &PrimFnDydPlusHC4RisHC4RvHC4R,
    &PrimFnDydPlusHC4VisHC4RvHC4R,
    &PrimFnDydPlusHC4VisHC4VvHC4V,

    &PrimFnDydPlusHC8IisHC8IvHC8I,
    NULL,   // &PrimFnDydPlusHC8IisHC8FvHC8F, -- Can't happen w/Plus
    &PrimFnDydPlusHC8FisHC8IvHC8I,
    &PrimFnDydPlusHC8FisHC8FvHC8F,
    &PrimFnDydPlusHC8RisHC8RvHC8R,
    &PrimFnDydPlusHC8VisHC8RvHC8R,
    &PrimFnDydPlusHC8VisHC8VvHC8V,

    // Dyadic BAxF result functions (indexable)
    &PrimFnDydPlusBA1FisBA1FvBA1F,
    &PrimFnDydPlusBA2FisBA2FvBA2F,
    &PrimFnDydPlusBA4FisBA4FvBA4F,
    &PrimFnDydPlusBA8FisBA8FvBA8F,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonPlusB64isB64, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusB32isB32, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusB16isB16, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusB08isB08, -- Can't happen w/Plus

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonPlusB64isB64vB64, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusB32isB32vB32, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusB16isB16vB16, -- Can't happen w/Plus
    NULL,   // &PrimFnMonPlusB08isB08vB08, -- Can't happen w/Plus

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecPlus};
#endif


//***************************************************************************
//  $PrimFnPlus_EM_YY
//
//  Primitive function for monadic and dyadic Plus ("conjugate" and "addition")
//***************************************************************************

LPPL_YYSTYPE PrimFnPlus_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_PLUS);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnPlus_EM_YY


//***************************************************************************
//  $PrimSpecPlusStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecPlusStorageTypeMon
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

    return aplTypeRes;
} // End PrimSpecPlusStorageTypeMon


//***************************************************************************
//  $PrimFnMonPlusBisB
//
//  Primitive scalar function monadic Plus:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonPlusBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonPlusBisB


//***************************************************************************
//  $ConjHC1I
//***************************************************************************

APLHC1I ConjHC1I
    (APLHC1I aplRht)                    // Right arg

{
    return aplRht;
} // End ConjHC1I


//***************************************************************************
//  $ConjHC1I_RE
//***************************************************************************

APLHC1I ConjHC1I_RE
    (APLHC1I aplRht)                    // Right arg

{
    return aplRht;
} // End ConjHC1I_RE


//***************************************************************************
//  $PrimFnMonPlusIisI
//
//  Primitive scalar function monadic Plus:  I {is} fn I
//***************************************************************************

void PrimFnMonPlusIisI
    (LPAPLINT   lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatRht,                // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = lpatRht->aplInteger;
} // End PrimFnMonPlusIisI


//***************************************************************************
//  $PrimFnMonPlusFisF
//
//  Primitive scalar function monadic Plus:  F {is} fn F
//***************************************************************************

void PrimFnMonPlusFisF
    (LPAPLFLOAT lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatRht,                // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = lpatRht->aplFloat;
} // End PrimFnMonPlusFisF


//***************************************************************************
//  $PrimFnMonPlusRisR
//
//  Primitive scalar function monadic Plus:  R {is} fn R
//***************************************************************************

void PrimFnMonPlusRisR
    (LPAPLRAT   lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatRht,                // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    // Copy the right arg to the result
    mpq_init_set (&lpMemRes[uRes], &lpatRht->aplRat);
} // End PrimFnMonPlusRisR


//***************************************************************************
//  $PrimFnMonPlusVisR
//
//  Primitive scalar function monadic Plus:  V {is} fn R
//***************************************************************************

void PrimFnMonPlusVisR
    (LPAPLVFP   lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatRht,                // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    // Copy the right arg to the result
    mpfr_init_set_q (&lpMemRes[uRes], &lpatRht->aplRat, MPFR_RNDN);
} // End PrimFnMonPlusVisR


//***************************************************************************
//  $PrimFnMonPlusVisV
//
//  Primitive scalar function monadic Plus:  V {is} fn V
//***************************************************************************

void PrimFnMonPlusVisV
    (LPAPLVFP   lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatRht,                // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    // Copy the right arg to the result
    mpfr_init_copy (&lpMemRes[uRes], &lpatRht->aplVfp);
} // End PrimFnMonPlusVisV


//***************************************************************************
//  $ConjBA1F
//***************************************************************************

APLBA1F ConjBA1F
    (APLBA1F aplRht)                    // Right arg

{
    APLBA1F aplRes = {0};               // The result

    // Copy the right arg to the result
    arb_init_set (&aplRes, &aplRht);

    return aplRes;
} // End ConjBA1F


//***************************************************************************
//  $ConjBA1F_RE
//***************************************************************************

APLBA1F ConjBA1F_RE
    (APLBA1F aplRht)                    // Right arg

{
    APLBA1F aplRes = {0};               // The result

    // Copy the right arg to the result
    arb_init_set (&aplRes, &aplRht);

    return aplRes;
} // End ConjBA1F_RE


//***************************************************************************
//  $PrimFnMonPlusBA1FisBA1F
//
//  Primitive scalar function monadic Plus:  BA1F {is} fn BA1F
//***************************************************************************

void PrimFnMonPlusBA1FisBA1F
    (LPAPLARB   lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatRht,                // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    // Copy the right arg to the result
    arb_init_set (&lpMemRes[uRes], &lpatRht->aplArb);
} // End PrimFnMonPlusBA1FisBA1F


//***************************************************************************
//  $ConjHC2I
//***************************************************************************

APLHC2I ConjHC2I
    (APLHC2I aplRht)                    // Right arg

{
    int     i;
    APLHC2I aplRes;

    // Copy the right arg real part to the result
    aplRes.parts[0] =  aplRht.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Change the sign of the imaginary part
        aplRes.parts[i] = -aplRht.parts[i];

    return aplRes;
} // End ConjHC2I


//***************************************************************************
//  $ConjHC2I_RE
//***************************************************************************

APLHC2I ConjHC2I_RE
    (APLHC2I aplRht)                    // Right arg

{
    int     i;
    APLHC2I aplRes;

    // Copy the right arg real part to the result
    aplRes.parts[0] =  aplRht.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Change the sign of the imaginary part
        aplRes.parts[i] = NegHC1I_RE (aplRht.parts[i]);

    return aplRes;
} // End ConjHC2I_RE


//***************************************************************************
//  $PrimFnMonPlusHC2IisHC2I
//
//  Primitive scalar function monadic Plus:  HC2I {is} fn HC2I
//***************************************************************************

void PrimFnMonPlusHC2IisHC2I
    (LPAPLHC2I  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatRht,                // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ConjHC2I_RE (lpatRht->aplHC2I);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check on exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2F);
} // End PrimFnMonPlusHC2IisHC2I


//***************************************************************************
//  $PrimFnMonPlusHC2FisHC2I
//
//  Primitive scalar function monadic Plus:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonPlusHC2FisHC2I
    (LPAPLHC2F  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatRht,                // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    int i;

    // No exceptions in this code

    // Copy the right arg real part to the result
    lpMemRes[uRes].parts[0] =  (APLFLOAT) lpatRht->aplHC2I.parts[0];

    // Loop through the imaginary parts
        for (i = 1; i < 2; i++)
        // Change the sign of the imaginary part
        lpMemRes[uRes].parts[i] = -(APLFLOAT) lpatRht->aplHC2I.parts[i];
} // End PrimFnMonPlusHC2FisHC2I


//***************************************************************************
//  $ConjHC2F
//***************************************************************************

APLHC2F ConjHC2F
    (APLHC2F aplRht)                    // Right arg

{
    int     i;
    APLHC2F aplRes;

    // No exceptions in this code

    // Copy the right arg real part to the result
    aplRes.parts[0] =  aplRht.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Change the sign of the imaginary parts
        aplRes.parts[i] = -aplRht.parts[i];

    return aplRes;
} // End ConjHC2F


//***************************************************************************
//  $PrimFnMonPlusHC2FisHC2F
//
//  Primitive scalar function monadic Plus:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonPlusHC2FisHC2F
    (LPAPLHC2F  lpMemRes,               // Ptr to the result
     APLUINT    uRes,                   // Index into the result
     LPALLTYPES lpatRht,                // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)             // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjHC2F (lpatRht->aplHC2F);
} // End PrimFnMonPlusHC2FisHC2F


//***************************************************************************
//  $ConjHC2R
//***************************************************************************

APLHC2R ConjHC2R
    (APLHC2R aplRht)                    // Right arg

{
    int     i;
    APLHC2R aplRes = {0};

    // Initialize to 0/1
    mphc2r_init (&aplRes);

    // Copy the right arg real part to the result
    mpq_set (&aplRes.parts[0], &aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
         // Change the sign of the imaginary part
         mpq_neg (&aplRes.parts[i], &aplRht.parts[i]);

    return aplRes;
} // End ConjHC2R


//***************************************************************************
//  $ConjHC2R_RE
//***************************************************************************

APLHC2R ConjHC2R_RE
    (APLHC2R aplRht)                    // Right arg

{
    int     i;
    APLHC2R aplRes = {0};

    // Initialize to 0/1
    mphc2r_init (&aplRes);

    // Copy the right arg real part to the result
    mpq_set (&aplRes.parts[0], &aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
    {
        // Check for -0
        if (gAllowNeg0
         && IsMpq0 (&aplRht.parts[i]))
            RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
        else
            // Change the sign of the imaginary part
            mpq_neg (&aplRes.parts[i], &aplRht.parts[i]);
    } // End FOR

    return aplRes;
} // End ConjHC2R_RE


//***************************************************************************
//  $PrimFnMonPlusHC2RisHC2R
//
//  Primitive scalar function monadic Plus:  HC2R {is} fn HC2R
//***************************************************************************

void PrimFnMonPlusHC2RisHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ConjHC2R_RE (lpatRht->aplHC2R);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2V);
} // End PrimFnMonPlusHC2RisHC2R


//***************************************************************************
//  $ConjHC2V
//***************************************************************************

APLHC2V ConjHC2V
    (APLHC2V aplRht)                    // Right arg

{
    int     i;
    APLHC2V aplRes = {0};

    // No exceptions in this code

    // Initialize to 0
    mphc2v_init0 (&aplRes);

    // Copy the right arg real part to the result
    mpfr_set (&aplRes.parts[0], &aplRht.parts[0], MPFR_RNDN);

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Change the sign of the imaginary part
        mpfr_neg (&aplRes.parts[i], &aplRht.parts[i], MPFR_RNDN);

    return aplRes;
} // End ConjHC2V


//***************************************************************************
//  $PrimFnMonPlusHC2VisHC2R
//
//  Primitive scalar function monadic Plus:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonPlusHC2VisHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplHC2R, 0, &atRht);

    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjHC2V (atRht.aplHC2V);

    Myhc2v_clear (&atRht.aplHC2V);
} // End PrimFnMonPlusHC2VisHC2R


//***************************************************************************
//  $PrimFnMonPlusHC2VisHC2V
//
//  Primitive scalar function monadic Plus:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonPlusHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjHC2V (lpatRht->aplHC2V);
} // End PrimFnMonPlusHC2VisHC2V


//***************************************************************************
//  $ConjBA2F
//***************************************************************************

APLBA2F ConjBA2F
    (APLBA2F aplRht)                    // Right arg

{
    int     i;
    APLBA2F aplRes = {0};               // The result

    // No exceptions in this code

    // Initialize to 0/0
    arb2f_init (&aplRes);

    // Copy the right arg real part to the result
    arb_set (&aplRes.parts[0], &aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Change the sign of the imaginary part
        arb_neg (&aplRes.parts[i], &aplRht.parts[i]);

    return aplRes;
} // End ConjBA2F


//***************************************************************************
//  $PrimFnMonPlusBA2FisBA2F
//
//  Primitive scalar function monadic Plus:  BA2F {is} fn BA2F
//***************************************************************************

void PrimFnMonPlusBA2FisBA2F
    (LPAPLBA2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjBA2F (lpatRht->aplBA2F);
} // End PrimFnMonPlusBA2FisBA2F


//***************************************************************************
//  $ConjHC4I
//***************************************************************************

APLHC4I ConjHC4I
    (APLHC4I aplRht)                    // Right arg

{
    int     i;
    APLHC4I aplRes;

    // Copy the right arg real part to the result
    aplRes.parts[0] =  aplRht.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Change the sign of the imaginary part
        aplRes.parts[i] = -aplRht.parts[i];

    return aplRes;
} // End ConjHC4I


//***************************************************************************
//  $ConjHC4I_RE
//***************************************************************************

APLHC4I ConjHC4I_RE
    (APLHC4I aplRht)                    // Right arg

{
    int     i;
    APLHC4I aplRes;

    // Copy the right arg real part to the result
    aplRes.parts[0] =  aplRht.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Change the sign of the imaginary part
        aplRes.parts[i] = NegHC1I_RE (aplRht.parts[i]);

    return aplRes;
} // End ConjHC4I_RE


//***************************************************************************
//  $PrimFnMonPlusHC4IisHC4I
//
//  Primitive scalar function monadic Plus:  HC4I {is} fn HC4I
//***************************************************************************

void PrimFnMonPlusHC4IisHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ConjHC4I_RE (lpatRht->aplHC4I);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4F);
} // End PrimFnMonPlusHC4IisHC4I


//***************************************************************************
//  $PrimFnMonPlusHC4FisHC4I
//
//  Primitive scalar function monadic Plus:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonPlusHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // No exceptions in this code

    // Copy the right arg real part to the result
    lpMemRes[uRes].parts[0] = (APLFLOAT) lpatRht->aplHC4I.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Change the sign of the imaginary parts
        lpMemRes[uRes].parts[i] = -(APLFLOAT) lpatRht->aplHC4I.parts[i];
} // End PrimFnMonPlusHC4FisHC4I


//***************************************************************************
//  $ConjHC4F
//***************************************************************************

APLHC4F ConjHC4F
    (APLHC4F aplRht)                    // Right arg

{
    int     i;
    APLHC4F aplRes;

    // No exceptions in this code

    // Copy the right arg real part to the result
    aplRes.parts[0] =  aplRht.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Change the sign of the imaginary parts
        aplRes.parts[i] = -aplRht.parts[i];

    return aplRes;
} // End ConjHC4F


//***************************************************************************
//  $PrimFnMonPlusHC4FisHC4F
//
//  Primitive scalar function monadic Plus:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonPlusHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjHC4F (lpatRht->aplHC4F);
} // End PrimFnMonPlusHC4FisHC4F


//***************************************************************************
//  $ConjHC4R
//***************************************************************************

APLHC4R ConjHC4R
    (APLHC4R aplRht)                    // Right arg

{
    int     i;
    APLHC4R aplRes = {0};

    // Initialize to 0/1
    mphc4r_init (&aplRes);

    // Copy the right arg real part to the result
    mpq_set (&aplRes.parts[0], &aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
         // Change the sign of the imaginary part
         mpq_neg (&aplRes.parts[i], &aplRht.parts[i]);

    return aplRes;
} // End ConjHC4R


//***************************************************************************
//  $ConjHC4R_RE
//***************************************************************************

APLHC4R ConjHC4R_RE
    (APLHC4R aplRht)                    // Right arg

{
    int     i;
    APLHC4R aplRes = {0};

    // Initialize to 0/1
    mphc4r_init (&aplRes);

    // Copy the right arg real part to the result
    mpq_set (&aplRes.parts[0], &aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
    {
        // Check for -0
        if (gAllowNeg0
         && IsMpq0 (&aplRht.parts[i]))
            RaiseException (EXCEPTION_RESULT_HC4V, 0, 0, NULL);
        else
            // Change the sign of the imaginary part
            mpq_neg (&aplRes.parts[i], &aplRht.parts[i]);
    } // End FOR

    return aplRes;
} // End ConjHC4R_RE


//***************************************************************************
//  $PrimFnMonPlusHC4RisHC4R
//
//  Primitive scalar function monadic Plus:  HC4R {is} fn HC4R
//***************************************************************************

void PrimFnMonPlusHC4RisHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ConjHC4R_RE (lpatRht->aplHC4R);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4V);
} // End PrimFnMonPlusHC4RisHC4R


//***************************************************************************
//  $ConjHC4V
//***************************************************************************

APLHC4V ConjHC4V
    (APLHC4V aplRht)                    // Right arg

{
    int     i;
    APLHC4V aplRes = {0};

    // No exceptions in this code

    // Initialize to 0
    mphc4v_init0 (&aplRes);

    // Copy the right arg real part to the result
    mpfr_set (&aplRes.parts[0], &aplRht.parts[0], MPFR_RNDN);

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Change the sign of the imaginary part
        mpfr_neg (&aplRes.parts[i], &aplRht.parts[i], MPFR_RNDN);

    return aplRes;
} // End ConjHC4V


//***************************************************************************
//  $PrimFnMonPlusHC4VisHC4R
//
//  Primitive scalar function monadic Plus:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonPlusHC4VisHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplHC4R, 0, &atRht);

    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjHC4V (atRht.aplHC4V);

    Myhc4v_clear (&atRht.aplHC4V);
} // End PrimFnMonPlusHC4VisHC4R


//***************************************************************************
//  $PrimFnMonPlusHC4VisHC4V
//
//  Primitive scalar function monadic Plus:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonPlusHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjHC4V (lpatRht->aplHC4V);
} // End PrimFnMonPlusHC4VisHC4V


//***************************************************************************
//  $ConjBA4F
//***************************************************************************

APLBA4F ConjBA4F
    (APLBA4F aplRht)                    // Right arg

{
    int     i;
    APLBA4F aplRes = {0};               // The result

    // No exceptions in this code

    // Initialize to 0/0
    arb4f_init (&aplRes);

    // Copy the right arg real part to the result
    arb_set (&aplRes.parts[0], &aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Change the sign of the imaginary part
        arb_neg (&aplRes.parts[i], &aplRht.parts[i]);

    return aplRes;
} // End ConjBA4F


//***************************************************************************
//  $PrimFnMonPlusBA4FisBA4F
//
//  Primitive scalar function monadic Plus:  BA4F {is} fn BA4F
//***************************************************************************

void PrimFnMonPlusBA4FisBA4F
    (LPAPLBA4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjBA4F (lpatRht->aplBA4F);
} // End PrimFnMonPlusBA4FisBA4F


//***************************************************************************
//  $ConjHC8I
//***************************************************************************

APLHC8I ConjHC8I
    (APLHC8I aplRht)                    // Right arg

{
    int     i;
    APLHC8I aplRes;

    // Copy the right arg real part to the result
    aplRes.parts[0] =  aplRht.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Change the sign of the imaginary part
        aplRes.parts[i] = -aplRht.parts[i];

    return aplRes;
} // End ConjHC8I


//***************************************************************************
//  $ConjHC8I_RE
//***************************************************************************

APLHC8I ConjHC8I_RE
    (APLHC8I aplRht)                    // Right arg

{
    int     i;
    APLHC8I aplRes;

    // Copy the right arg real part to the result
    aplRes.parts[0] =  aplRht.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Change the sign of the imaginary part
        aplRes.parts[i] = NegHC1I_RE (aplRht.parts[i]);

    return aplRes;
} // End ConjHC8I_RE


//***************************************************************************
//  $PrimFnMonPlusHC8IisHC8I
//
//  Primitive scalar function monadic Plus:  HC8I {is} fn HC8I
//***************************************************************************

void PrimFnMonPlusHC8IisHC8I
    (LPAPLHC8I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ConjHC8I_RE (lpatRht->aplHC8I);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8F);
} // End PrimFnMonPlusHC8IisHC8I


//***************************************************************************
//  $PrimFnMonPlusHC8FisHC8I
//
//  Primitive scalar function monadic Plus:  HC8F {is} fn HC8I
//***************************************************************************

void PrimFnMonPlusHC8FisHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // No exceptions in this code

    // Copy the right arg real part to the result
    lpMemRes[uRes].parts[0] = (APLFLOAT) lpatRht->aplHC8I.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Change the sign of the imaginary parts
        lpMemRes[uRes].parts[i] = -(APLFLOAT) lpatRht->aplHC8I.parts[i];
} // End PrimFnMonPlusHC8FisHC8I


//***************************************************************************
//  $ConjHC8F
//***************************************************************************

APLHC8F ConjHC8F
    (APLHC8F aplRht)                // Right arg

{
    int     i;
    APLHC8F aplRes;

    // No exceptions in this code

    // Copy the right arg real part to the result
    aplRes.parts[0] =  aplRht.parts[0];

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Change the sign of the imaginary parts
        aplRes.parts[i] = -aplRht.parts[i];

    return aplRes;
} // End ConjHC8F


//***************************************************************************
//  $PrimFnMonPlusHC8FisHC8F
//
//  Primitive scalar function monadic Plus:  HC8F {is} fn HC8F
//***************************************************************************

void PrimFnMonPlusHC8FisHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjHC8F (lpatRht->aplHC8F);
} // End PrimFnMonPlusHC8FisHC8F


//***************************************************************************
//  $ConjHC8R
//***************************************************************************

APLHC8R ConjHC8R
    (APLHC8R aplRht)                    // Right arg

{
    int     i;
    APLHC8R aplRes = {0};

    // Initialize to 0/1
    mphc8r_init (&aplRes);

    // Copy the right arg real part to the result
    mpq_set (&aplRes.parts[0], &aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
         // Change the sign of the imaginary part
         mpq_neg (&aplRes.parts[i], &aplRht.parts[i]);

    return aplRes;
} // End ConjHC8R


//***************************************************************************
//  $ConjHC8R_RE
//***************************************************************************

APLHC8R ConjHC8R_RE
    (APLHC8R aplRht)                    // Right arg

{
    int     i;
    APLHC8R aplRes = {0};

    // Initialize to 0/1
    mphc8r_init (&aplRes);

    // Copy the right arg real part to the result
    mpq_set (&aplRes.parts[0], &aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
    {
        // Check for -0
        if (gAllowNeg0
         && IsMpq0 (&aplRht.parts[i]))
            RaiseException (EXCEPTION_RESULT_HC8V, 0, 0, NULL);
        else
            // Change the sign of the imaginary part
            mpq_neg (&aplRes.parts[i], &aplRht.parts[i]);
    } // End FOR

    return aplRes;
} // End ConjHC8R_RE


//***************************************************************************
//  $PrimFnMonPlusHC8RisHC8R
//
//  Primitive scalar function monadic Plus:  HC8R {is} fn HC8R
//***************************************************************************

void PrimFnMonPlusHC8RisHC8R
    (LPAPLHC8R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = ConjHC8R_RE (lpatRht->aplHC8R);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8V);
} // End PrimFnMonPlusHC8RisHC8R


//***************************************************************************
//  $ConjHC8V
//***************************************************************************

APLHC8V ConjHC8V
    (APLHC8V aplRht)                    // Right arg

{
    int     i;
    APLHC8V aplRes = {0};

    // No exceptions in this code

    // Initialize to 0
    mphc8v_init0 (&aplRes);

    // Copy the right arg real part to the result
    mpfr_set (&aplRes.parts[0], &aplRht.parts[0], MPFR_RNDN);

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Change the sign of the imaginary part
        mpfr_neg (&aplRes.parts[i], &aplRht.parts[i], MPFR_RNDN);

    return aplRes;
} // End ConjHC8V


//***************************************************************************
//  $PrimFnMonPlusHC8VisHC8R
//
//  Primitive scalar function monadic Plus:  HC8V {is} fn HC8R
//***************************************************************************

void PrimFnMonPlusHC8VisHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatRht->aplHC8R, 0, &atRht);

    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjHC8V (atRht.aplHC8V);

    Myhc8v_clear (&atRht.aplHC8V);
} // End PrimFnMonPlusHC8VisHC8R


//***************************************************************************
//  $PrimFnMonPlusHC8VisHC8V
//
//  Primitive scalar function monadic Plus:  HC8V {is} fn HC8V
//***************************************************************************

void PrimFnMonPlusHC8VisHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjHC8V (lpatRht->aplHC8V);
} // End PrimFnMonPlusHC8VisHC8V


//***************************************************************************
//  $ConjBA8F
//***************************************************************************

APLBA8F ConjBA8F
    (APLBA8F aplRht)                    // Right arg

{
    int     i;
    APLBA8F aplRes = {0};               // The result

    // No exceptions in this code

    // Initialize to 0/0
    arb8f_init (&aplRes);

    // Copy the right arg real part to the result
    arb_set (&aplRes.parts[0], &aplRht.parts[0]);

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Change the sign of the imaginary part
        arb_neg (&aplRes.parts[i], &aplRht.parts[i]);

    return aplRes;
} // End ConjBA8F


//***************************************************************************
//  $PrimFnMonPlusBA8FisBA8F
//
//  Primitive scalar function monadic Plus:  BA8F {is} fn BA8F
//***************************************************************************

void PrimFnMonPlusBA8FisBA8F
    (LPAPLBA8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = ConjBA8F (lpatRht->aplBA8F);
} // End PrimFnMonPlusBA8FisBA8F


//***************************************************************************
//  $PrimFnMonPlusAPA_EM
//
//  Monadic plus, result is APA
//***************************************************************************

UBOOL PrimFnMonPlusAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // The result token (may be NULL)
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
} // End PrimFnMonPlusAPA_EM


//***************************************************************************
//  $PrimSpecPlusStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecPlusStorageTypeDyd
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

    // No Boolean results for addition
    if (IsSimpleBool (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    // Special case addition with APA
    if (aplTypeRes EQ ARRAY_INT                             // Res = INT
     && (!IsSingleton (aplNELMLft) || !IsSingleton (aplNELMRht))    // Not both singletons
     && ((IsSingleton (aplNELMLft) && IsSimpleAPA (*lpaplTypeRht))  // Non-singleton is APA
      || (IsSingleton (aplNELMRht) && IsSimpleAPA (*lpaplTypeLft))))// ...
        aplTypeRes = ARRAY_APA;

    return aplTypeRes;
} // End PrimSpecPlusStorageTypeDyd


//***************************************************************************
//  $AddHC1I_RE
//***************************************************************************

APLINT AddHC1I_RE
    (APLINT aplLft,                     // Left arg
     APLINT aplRht)                     // Right arg

{
    return iadd64_RE (aplLft, aplRht, FLOAT);
} // End AddHC1I_RE


//***************************************************************************
//  $PrimFnDydPlusIisIvI
//
//  Primitive scalar function dyadic Plus:  I {is} I fn I
//***************************************************************************

void PrimFnDydPlusIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = AddHC1I_RE (lpatLft->aplInteger, lpatRht->aplInteger);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_FLOAT);
} // End PrimFnDydPlusIisIvI


//***************************************************************************
//  $PrimFnDydPlusFisIvI
//
//  Primitive scalar function dyadic Plus:  F {is} I fn I
//***************************************************************************

void PrimFnDydPlusFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = ((APLFLOAT) (APLINT) lpatLft->aplInteger)
                   + ((APLFLOAT) (APLINT) lpatRht->aplInteger);
} // End PrimFnDydPlusFisIvI


//***************************************************************************
//  $AddHC1F_RE
//***************************************************************************

APLFLOAT AddHC1F_RE
    (APLFLOAT          aplLft,          // Left arg
     APLFLOAT          aplRht)          // Right ...

{
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1F, &aplRht, 0);

    // Check for indeterminates:  _ + _  or  -_ + -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            return aplLft;
        else
            return aplRht;
    } else
    // If the args are both infinite and of opposite signs, ...
    if (_isinf (aplLft)
     && _isinf (aplRht)
     && SIGN_APLFLOAT (aplLft) NE SIGN_APLFLOAT (aplRht))
        return TranslateQuadICIndex (aplLft,
                                     ICNDX_InfSUBInf,
                                     aplRht,
                                     FALSE);
    else
        // Add the two floats
        return aplLft + aplRht;
} // End AddHC1F_RE


//***************************************************************************
//  $PrimFnDydPlusFisFvF
//
//  Primitive scalar function dyadic Plus:  F {is} F fn F
//***************************************************************************

void PrimFnDydPlusFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = AddHC1F_RE (lpatLft->aplFloat, lpatRht->aplFloat);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydPlusFisFvF


//***************************************************************************
//  $AddHC1R_RE
//***************************************************************************

APLRAT AddHC1R_RE
    (APLRAT aplLft,                     // Left arg
     APLRAT aplRht)                     // Right ...

{
    APLRAT aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1R, &aplRht, 0);

    // Check for indeterminates:  _ + _  or  -_ + -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc1r_init_set (&aplRes, &aplLft);
        else
            mphc1r_init_set (&aplRes, &aplRht);
    } else
    // If the args are both infinite and of opposite signs, ...
    if (mpq_inf_p (&aplLft)
     && mpq_inf_p (&aplRht)
     && mpq_sgn (&aplLft) NE mpq_sgn (&aplRht))
        mpq_QuadICValue (&aplRht,           // No left arg
                          ICNDX_InfSUBInf,
                         &aplRht,
                         &aplRes,
                          FALSE);
    else
    {
        // Initialize the result to 0/1
        mpq_init (&aplRes);

        // Add the two Rationals
        mpq_add (&aplRes, &aplLft, &aplRht);
    } // End IF/ELSE

    return aplRes;
} // End AddHC1R_RE


//***************************************************************************
//  $PrimFnDydPlusRisRvR
//
//  Primitive scalar function dyadic Plus:  R {is} R fn R
//***************************************************************************

void PrimFnDydPlusRisRvR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = AddHC1R_RE (lpatLft->aplRat, lpatRht->aplRat);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_VFP);
} // End PrimFnDydPlusRisRvR


//***************************************************************************
//  $AddHC1V_RE
//***************************************************************************

APLVFP AddHC1V_RE
    (APLVFP aplLft,                     // Left arg
     APLVFP aplRht)                     // Right ...

{
    APLVFP aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1V, &aplRht, 0);

    // Check for indeterminates:  _ + _  or  -_ + -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc1v_init_set (&aplRes, &aplLft);
        else
            mphc1v_init_set (&aplRes, &aplRht);
    } else
    // If the args are both infinite and of opposite signs, ...
    if (mpfr_inf_p (&aplLft)
     && mpfr_inf_p (&aplRht)
     && mpfr_sgn (&aplLft) NE mpfr_sgn (&aplRht))
        mpfr_QuadICValue (&aplRht,          // No left arg
                          ICNDX_InfSUBInf,
                         &aplRht,
                         &aplRes,
                          FALSE);
    else
    {
        // Initialize the result to 0
        mpfr_init0 (&aplRes);

        // Add the two Rationals
        mpfr_add (&aplRes, &aplLft, &aplRht, MPFR_RNDN);
    } // End IF/ELSE

    return aplRes;
} // End AddHC1V_RE


//***************************************************************************
//  $PrimFnDydPlusVisRvR
//
//  Primitive scalar function dyadic Plus:  V {is} R fn R
//***************************************************************************

void PrimFnDydPlusVisRvR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the left & right arg to the result type
    (*aTypeActPromote[ARRAY_HC1R][ARRAY_HC1V]) (&lpatLft->aplRat, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC1R][ARRAY_HC1V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnDydPlusVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydPlusVisRvR


//***************************************************************************
//  $PrimFnDydPlusVisVvV
//
//  Primitive scalar function dyadic Plus:  V {is} V fn V
//***************************************************************************

void PrimFnDydPlusVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = AddHC1V_RE (lpatLft->aplVfp, lpatRht->aplVfp);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydPlusVisVvV


//***************************************************************************
//  $PrimFnDydPlusBA1FisBA1FvBA1F
//
//  Primitive scalar function dyadic Plus:  BA1F {is} BA1F fn BA1F
//***************************************************************************

void PrimFnDydPlusBA1FisBA1FvBA1F
    (LPAPLARB   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = AddBA1F_RE (lpatLft->aplArb, lpatRht->aplArb);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydPlusBA1FisBA1FvBA1F


//***************************************************************************
//  $AddBA1F_RE
//***************************************************************************

APLBA1F AddBA1F_RE
    (APLBA1F aplLft,                // Left arg
     APLBA1F aplRht)                // Right ...

{
////int     i;
    APLBA1F aplRes = {0};           // The result

    // Check for indeterminates:  _ + _  or  -_ + -_

////// Loop through all of the parts
////for (i = 0; i < 1; i++)
////{
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of opposite signs, ...
        if (arf_is_inf (arb_midref (&aplLft))
         && arf_is_inf (arb_midref (&aplRht))
         && arb_sign (&aplLft) NE arb_sign (&aplRht))
            arb_init_set (&aplRes,
                           arb_QuadICValue (&aplRht,       // No left arg
                                             ICNDX_InfSUBInf,
                                            &aplRht,
                                            &aplRes,
                                             FALSE));
        else
        {
            mp_limb_signed_t prec = ARB_PREC_FPC;

            // Initialize the result to 0/0
            Myarb_init (&aplRes);

            // Add the VFPs
            arb_add (&aplRes, &aplLft, &aplRht, prec);
        } // End IF/ELSE
////} // End FOR

    return aplRes;
} // End AddBA1F_RE


//***************************************************************************
//  $AddHC2I_RE
//***************************************************************************

APLHC2I AddHC2I_RE
    (APLHC2I aplLft,                    // Left arg
     APLHC2I aplRht)                    // Right ...

{
    int     i;
    APLHC2I aplRes;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Add the two integers
         aplRes.parts[i] = AddHC1I_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End AddHC2I_RE


//***************************************************************************
//  $PrimFnDydPlusHC2IisHC2IvHC2I
//
//  Primitive scalar function dyadic Plus:  HC2I {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydPlusHC2IisHC2IvHC2I
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
        lpMemRes[uRes] = AddHC2I_RE (lpatLft->aplHC2I, lpatRht->aplHC2I);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2F);
} // End PrimFnDydPlusHC2IisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydPlusHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic Plus:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydPlusHC2FisHC2IvHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Convert the integers to FLOAT and Add
        lpMemRes[uRes].parts[i] = (APLFLOAT) lpatLft->aplHC2I.parts[i] + (APLFLOAT) lpatRht->aplHC2I.parts[i];
} // End PrimFnDydPlusHC2FisHC2IvHC2I


//***************************************************************************
//  $AddHC2F_RE
//***************************************************************************

APLHC2F AddHC2F_RE
    (APLHC2F aplLft,                // Left arg
     APLHC2F aplRht)                // Right ...

{
    int     i;
    APLHC2F aplRes;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Add the two integers
        aplRes.parts[i] = AddHC1F_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End AddHC2F_RE


//***************************************************************************
//  $PrimFnDydPlusHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic Plus:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydPlusHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = AddHC2F_RE (lpatLft->aplHC2F, lpatRht->aplHC2F);
} // End PrimFnDydPlusHC2FisHC2FvHC2F


//***************************************************************************
//  $AddHC2R_RE
//***************************************************************************

APLHC2R AddHC2R_RE
    (APLHC2R aplLft,                // Left arg
     APLHC2R aplRht)                // Right ...

{
    int    i;
    APLHC2R aplRes;

    // Check for indeterminates:  _ + _  or  -_ + -_

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of opposite signs, ...
        if (mpq_inf_p (&aplLft.parts[i])
         && mpq_inf_p (&aplRht.parts[i])
         && mpq_sgn (&aplLft.parts[i]) NE mpq_sgn (&aplRht.parts[i]))
            mpq_QuadICValue (&aplRht.parts[i],   // No left arg
                              ICNDX_InfSUBInf,
                             &aplRht.parts[i],
                             &aplRes.parts[i],
                              FALSE);
        else
        {
            // Initialize the result to 0/1
            mpq_init (&aplRes.parts[i]);

            // Add the two Rationals
            mpq_add (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i]);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End AddHC2R_RE


//***************************************************************************
//  $PrimFnDydPlusHC2RisHC2RvHC2R
//
//  Primitive scalar function dyadic Plus:  HC2R {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydPlusHC2RisHC2RvHC2R
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
        lpMemRes[uRes] = AddHC2R_RE (lpatLft->aplHC2R, lpatRht->aplHC2R);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2V);
} // End PrimFnDydPlusHC2RisHC2RvHC2R


//***************************************************************************
//  $AddHC2V_RE
//***************************************************************************

APLHC2V AddHC2V_RE
    (APLHC2V aplLft,                // Left arg
     APLHC2V aplRht)                // Right ...

{
    int     i;
    APLHC2V aplRes = {0};

    // Check for indeterminates:  _ + _  or  -_ + -_

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of opposite signs, ...
        if (mpfr_inf_p (&aplLft.parts[i])
         && mpfr_inf_p (&aplRht.parts[i])
         && mpfr_sgn (&aplLft.parts[i]) NE mpfr_sgn (&aplRht.parts[i]))
            mpfr_QuadICValue (&aplRht.parts[i],         // No left arg
                               ICNDX_InfSUBInf,
                              &aplRht.parts[i],
                              &aplRes.parts[i],
                               FALSE);
        else
        {
            // Initialize the result to NaN
            mpfr_init (&aplRes.parts[i]);

            // Add the VFPs
            mpfr_add (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i], MPFR_RNDN);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End AddHC2V_RE


//***************************************************************************
//  $PrimFnDydPlusHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic Plus:  HC2V {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydPlusHC2VisHC2RvHC2R
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

    PrimFnDydPlusHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
    Myhc2v_clear (&atLft.aplHC2V);
} // End PrimFnDydPlusHC2VisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydPlusHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic Plus:  HC2V {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydPlusHC2VisHC2VvHC2V
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
        lpMemRes[uRes] = AddHC2V_RE (lpatLft->aplHC2V, lpatRht->aplHC2V);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydPlusHC2VisHC2VvHC2V


//***************************************************************************
//  $AddBA2F_RE
//***************************************************************************

APLBA2F AddBA2F_RE
    (APLBA2F aplLft,                // Left arg
     APLBA2F aplRht)                // Right ...

{
    int     i;
    APLBA2F aplRes = {0};           // The result
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  _ + _  or  -_ + -_

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of opposite signs, ...
        if (arf_is_inf (arb_midref (&aplLft.parts[i]))
         && arf_is_inf (arb_midref (&aplRht.parts[i]))
         && arb_sign (&aplLft.parts[i]) NE arb_sign (&aplRht.parts[i]))
            arb_init_set (&aplRes.parts[i],
                           arb_QuadICValue (&aplRht.parts[i],       // No left arg
                                             ICNDX_InfSUBInf,
                                            &aplRht.parts[i],
                                            &aplRes.parts[i],
                                             FALSE));
        else
        {
            // Initialize the result to 0/0
            Myarb_init (&aplRes.parts[i]);

            // Add the VFPs
            arb_add (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i], prec);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End AddBA2F_RE


//***************************************************************************
//  $PrimFnDydPlusBA2FisBA2FvBA2F
//
//  Primitive scalar function dyadic Plus:  BA2F {is} BA2F fn BA2F
//***************************************************************************

void PrimFnDydPlusBA2FisBA2FvBA2F
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
        lpMemRes[uRes] = AddBA2F_RE (lpatLft->aplBA2F, lpatRht->aplBA2F);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydPlusBA2FisBA2FvBA2F


//***************************************************************************
//  $AddHC4I_RE
//***************************************************************************

APLHC4I AddHC4I_RE
    (APLHC4I aplLft,                // Left arg
     APLHC4I aplRht)                // Right ...

{
    int     i;
    APLHC4I aplRes;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Add the two integers
        aplRes.parts[i] = AddHC1I_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End AddHC4I_RE


//***************************************************************************
//  $PrimFnDydPlusHC4IisHC4IvHC4I
//
//  Primitive scalar function dyadic Plus:  HC4I {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydPlusHC4IisHC4IvHC4I
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
        lpMemRes[uRes] = AddHC4I_RE (lpatLft->aplHC4I, lpatRht->aplHC4I);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4F);
} // End PrimFnDydPlusHC4IisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydPlusHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic Plus:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydPlusHC4FisHC4IvHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Convert the integers to FLOAT and Add
        lpMemRes[uRes].parts[i] = (APLFLOAT) lpatLft->aplHC4I.parts[i] + (APLFLOAT) lpatRht->aplHC4I.parts[i];
} // End PrimFnDydPlusHC4FisHC4IvHC4I


//***************************************************************************
//  $AddHC4F_RE
//***************************************************************************

APLHC4F AddHC4F_RE
    (APLHC4F aplLft,                // Left arg
     APLHC4F aplRht)                // Right ...

{
    int     i;
    APLHC4F aplRes;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Add the two integers
        aplRes.parts[i] = AddHC1F_RE (aplLft.parts[i],aplRht.parts[i]);

    return aplRes;
} // End AddHC4F_RE


//***************************************************************************
//  $PrimFnDydPlusHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic Plus:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydPlusHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Add the floats
    lpMemRes[uRes] = AddHC4F_RE (lpatLft->aplHC4F, lpatRht->aplHC4F);
} // End PrimFnDydPlusHC4FisHC4FvHC4F


//***************************************************************************
//  $AddHC4R_RE
//***************************************************************************

APLHC4R AddHC4R_RE
    (APLHC4R aplLft,                    // Left arg
     APLHC4R aplRht)                    // Right ...

{
    int     i;
    APLHC4R aplRes;

    // Check for indeterminates:  _ + _  or  -_ + -_

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of opposite signs, ...
        if (mpq_inf_p (&aplLft.parts[i])
         && mpq_inf_p (&aplRht.parts[i])
         && mpq_sgn (&aplLft.parts[i]) NE mpq_sgn (&aplRht.parts[i]))
            mpq_QuadICValue (&aplRht.parts[i],   // No left arg
                              ICNDX_InfSUBInf,
                             &aplRht.parts[i],
                             &aplRes.parts[i],
                              FALSE);
        else
        {
            // Initialize the result to 0/1
            mpq_init (&aplRes.parts[i]);

            // Add the two Rationals
            mpq_add (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i]);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End AddHC4R_RE


//***************************************************************************
//  $PrimFnDydPlusHC4RisHC4RvHC4R
//
//  Primitive scalar function dyadic Plus:  HC4R {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydPlusHC4RisHC4RvHC4R
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
        lpMemRes[uRes] = AddHC4R_RE (lpatLft->aplHC4R, lpatRht->aplHC4R);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4V);
} // End PrimFnDydPlusHC4RisHC4RvHC4R


//***************************************************************************
//  $AddHC4V_RE
//***************************************************************************

APLHC4V AddHC4V_RE
    (APLHC4V aplLft,                // Left arg
     APLHC4V aplRht)                // Right ...

{
    int     i;
    APLHC4V aplRes = {0};

    // Check for indeterminates:  _ + _  or  -_ + -_

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of opposite signs, ...
        if (mpfr_inf_p (&aplLft.parts[i])
         && mpfr_inf_p (&aplRht.parts[i])
         && mpfr_sgn (&aplLft.parts[i]) NE mpfr_sgn (&aplRht.parts[i]))
            mpfr_QuadICValue (&aplRht.parts[i],         // No left arg
                               ICNDX_InfSUBInf,
                              &aplRht.parts[i],
                              &aplRes.parts[i],
                               FALSE);
        else
        {
            // Initialize the result to NaN
            mpfr_init (&aplRes.parts[i]);

            // Add the VFPs
            mpfr_add (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i], MPFR_RNDN);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End AddHC4V_RE


//***************************************************************************
//  $PrimFnDydPlusHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic Plus:  HC4V {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydPlusHC4VisHC4RvHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the left & right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatLft->aplRat, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnDydPlusHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
    Myhc4v_clear (&atLft.aplHC4V);
} // End PrimFnDydPlusHC4VisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydPlusHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic Plus:  HC4V {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydPlusHC4VisHC4VvHC4V
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
        lpMemRes[uRes] = AddHC4V_RE (lpatLft->aplHC4V, lpatRht->aplHC4V);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        exCode = MyGetExceptionCode ();  // The exception code

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydPlusHC4VisHC4VvHC4V


//***************************************************************************
//  $AddBA4F_RE
//***************************************************************************

APLBA4F AddBA4F_RE
    (APLBA4F aplLft,                // Left arg
     APLBA4F aplRht)                // Right ...

{
    int     i;
    APLBA4F aplRes = {0};           // The result
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  _ + _  or  -_ + -_

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of opposite signs, ...
        if (arf_is_inf (arb_midref (&aplLft.parts[i]))
         && arf_is_inf (arb_midref (&aplRht.parts[i]))
         && arb_sign (&aplLft.parts[i]) NE arb_sign (&aplRht.parts[i]))
            arb_init_set (&aplRes.parts[i],
                           arb_QuadICValue (&aplRht.parts[i],       // No left arg
                                             ICNDX_InfSUBInf,
                                            &aplRht.parts[i],
                                            &aplRes.parts[i],
                                             FALSE));
        else
        {
            // Initialize the result to 0/0
            Myarb_init (&aplRes.parts[i]);

            // Add the VFPs
            arb_add (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i], prec);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End AddBA4F_RE


//***************************************************************************
//  $PrimFnDydPlusBA4FisBA4FvBA4F
//
//  Primitive scalar function dyadic Plus:  BA4F {is} BA4F fn BA4F
//***************************************************************************

void PrimFnDydPlusBA4FisBA4FvBA4F
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
        lpMemRes[uRes] = AddBA4F_RE (lpatLft->aplBA4F, lpatRht->aplBA4F);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydPlusBA4FisBA4FvBA4F


//***************************************************************************
//  $AddHC8I_RE
//***************************************************************************

APLHC8I AddHC8I_RE
    (APLHC8I aplLft,                // Left arg
     APLHC8I aplRht)                // Right ...

{
    int     i;
    APLHC8I aplRes;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Add the two integers
        aplRes.parts[i] = AddHC1I_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End AddHC8I_RE


//***************************************************************************
//  $PrimFnDydPlusHC8IisHC8IvHC8I
//
//  Primitive scalar function dyadic Plus:  HC8I {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydPlusHC8IisHC8IvHC8I
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
        lpMemRes[uRes] = AddHC8I_RE (lpatLft->aplHC8I, lpatRht->aplHC8I);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8F);
} // End PrimFnDydPlusHC8IisHC8IvHC8I


//***************************************************************************
//  $PrimFnDydPlusHC8FisHC8IvHC8I
//
//  Primitive scalar function dyadic Plus:  HC8F {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydPlusHC8FisHC8IvHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Convert the integers to FLOAT and Add
        lpMemRes[uRes].parts[i] = (APLFLOAT) lpatLft->aplHC8I.parts[i] + (APLFLOAT) lpatRht->aplHC8I.parts[i];
} // End PrimFnDydPlusHC8FisHC8IvHC8I


//***************************************************************************
//  $AddHC8F_RE
//***************************************************************************

APLHC8F AddHC8F_RE
    (APLHC8F aplLft,                // Left arg
     APLHC8F aplRht)                // Right ...

{
    int     i;
    APLHC8F aplRes;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Add the two FLOATs
        aplRes.parts[i] = AddHC1F_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End AddHC8F_RE


//***************************************************************************
//  $PrimFnDydPlusHC8FisHC8FvHC8F
//
//  Primitive scalar function dyadic Plus:  HC8F {is} HC8F fn HC8F
//***************************************************************************

void PrimFnDydPlusHC8FisHC8FvHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = AddHC8F_RE (lpatLft->aplHC8F, lpatRht->aplHC8F);
} // End PrimFnDydPlusHC8FisHC8FvHC8F


//***************************************************************************
//  $AddHC8R_RE
//***************************************************************************

APLHC8R AddHC8R_RE
    (APLHC8R aplLft,                    // Left arg
     APLHC8R aplRht)                    // Right ...

{
    int     i;
    APLHC8R aplRes;

    // Check for indeterminates:  _ + _  or  -_ + -_

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of opposite signs, ...
        if (mpq_inf_p (&aplLft.parts[i])
         && mpq_inf_p (&aplRht.parts[i])
         && mpq_sgn (&aplLft.parts[i]) NE mpq_sgn (&aplRht.parts[i]))
            mpq_QuadICValue (&aplRht.parts[i],   // No left arg
                              ICNDX_InfSUBInf,
                             &aplRht.parts[i],
                             &aplRes.parts[i],
                              FALSE);
        else
        {
            // Initialize the result to 0/1
            mpq_init (&aplRes.parts[i]);

            // Add the two Rationals
            mpq_add (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i]);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End AddHC8R_RE


//***************************************************************************
//  $PrimFnDydPlusHC8RisHC8RvHC8R
//
//  Primitive scalar function dyadic Plus:  HC8R {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydPlusHC8RisHC8RvHC8R
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
        lpMemRes[uRes] = AddHC8R_RE (lpatLft->aplHC8R, lpatRht->aplHC8R);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8V);
} // End PrimFnDydPlusHC8RisHC8RvHC8R


//***************************************************************************
//  $AddHC8V_RE
//***************************************************************************

APLHC8V AddHC8V_RE
    (APLHC8V aplLft,                // Left arg
     APLHC8V aplRht)                // Right ...

{
    int     i;
    APLHC8V aplRes = {0};

    // Check for indeterminates:  _ + _  or  -_ + -_

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of opposite signs, ...
        if (mpfr_inf_p (&aplLft.parts[i])
         && mpfr_inf_p (&aplRht.parts[i])
         && mpfr_sgn (&aplLft.parts[i]) NE mpfr_sgn (&aplRht.parts[i]))
            mpfr_QuadICValue (&aplRht.parts[i],         // No left arg
                               ICNDX_InfSUBInf,
                              &aplRht.parts[i],
                              &aplRes.parts[i],
                               FALSE);
        else
        {
            // Initialize the result to NaN
            mpfr_init (&aplRes.parts[i]);

            // Add the VFPs
            mpfr_add (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i], MPFR_RNDN);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End AddHC8V_RE


//***************************************************************************
//  $PrimFnDydPlusHC8VisHC8RvHC8R
//
//  Primitive scalar function dyadic Plus:  HC8V {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydPlusHC8VisHC8RvHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the left & right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatLft->aplRat, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnDydPlusHC8VisHC8VvHC8V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc8v_clear (&atRht.aplHC8V);
    Myhc8v_clear (&atLft.aplHC8V);
} // End PrimFnDydPlusHC8VisHC8RvHC8R


//***************************************************************************
//  $PrimFnDydPlusHC8VisHC8VvHC8V
//
//  Primitive scalar function dyadic Plus:  HC8V {is} HC8V fn HC8V
//***************************************************************************

void PrimFnDydPlusHC8VisHC8VvHC8V
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
        lpMemRes[uRes] = AddHC8V_RE (lpatLft->aplHC8V, lpatRht->aplHC8V);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydPlusHC8VisHC8VvHC8V


//***************************************************************************
//  $AddBA8F_RE
//***************************************************************************

APLBA8F AddBA8F_RE
    (APLBA8F aplLft,                // Left arg
     APLBA8F aplRht)                // Right ...

{
    int     i;
    APLBA8F aplRes = {0};           // The result
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Check for indeterminates:  _ + _  or  -_ + -_

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of opposite signs, ...
        if (arf_is_inf (arb_midref (&aplLft.parts[i]))
         && arf_is_inf (arb_midref (&aplRht.parts[i]))
         && arb_sign (&aplLft.parts[i]) NE arb_sign (&aplRht.parts[i]))
            arb_init_set (&aplRes.parts[i],
                           arb_QuadICValue (&aplRht.parts[i],       // No left arg
                                             ICNDX_InfSUBInf,
                                            &aplRht.parts[i],
                                            &aplRes.parts[i],
                                             FALSE));
        else
        {
            // Initialize the result to 0/0
            Myarb_init (&aplRes.parts[i]);

            // Add the VFPs
            arb_add (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i], prec);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End AddBA8F_RE


//***************************************************************************
//  $PrimFnDydPlusBA8FisBA8FvBA8F
//
//  Primitive scalar function dyadic Plus:  BA8F {is} BA8F fn BA8F
//***************************************************************************

void PrimFnDydPlusBA8FisBA8FvBA8F
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
        lpMemRes[uRes] = AddBA8F_RE (lpatLft->aplBA8F, lpatRht->aplBA8F);
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydPlusBA8FisBA8FvBA8F


//***************************************************************************
//  $PrimFnDydPlusAPA_EM
//
//  Dyadic plus, result is APA
//***************************************************************************

UBOOL PrimFnDydPlusAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // The result token (may be NULL)

     LPTOKEN      lptkFunc,         // Ptr to function token

     HGLOBAL      hGlbLft,          // HGLOBAL of left arg (may be NULL if simple)
     HGLOBAL      hGlbRht,          // ...        right ...
     HGLOBAL     *lphGlbRes,        // Ptr to result handle

     APLRANK      aplRankLft,       // Left arg rank
     APLRANK      aplRankRht,       // Right ...

     APLNELM      aplNELMLft,       // Left arg NELM
     APLNELM      aplNELMRht,       // Right ...

     APLINT       aplInteger,       // The integer from the simple side
     LPPRIMSPEC   lpPrimSpec)       // Ptr to local PRIMSPEC

{
    APLRANK aplRankRes;             // Result rank
    LPVOID  lpMemRes;               // Ptr to result global memory
    UBOOL   bRet = FALSE;           // TRUE iff the result is valid

    //***************************************************************
    // The result is an APA, one of the args is a simple singleton,
    //   the other an APA
    //***************************************************************

    // Axis may be anything

    // Get a ptr to each arg's header
    //   and the value of the singleton
    // Also, in case we're doing (1 1 1{rho}1)+{iota}4
    //   set the rank of the result to the rank of
    //   the APA.  The rank passed into this function
    //   is the larger of the left and right ranks,
    //   which for the above example, is wrong.

    // Split cases based upon the arg's NELM
    //   (one of the arg's must be a singleton)
    if (!IsSingleton (aplNELMLft))
    {
        *lphGlbRes = CopyArray_EM_PTB (hGlbLft, lptkFunc);
        aplRankRes = aplRankLft;
    } else
    if (!IsSingleton (aplNELMRht))
    {
        *lphGlbRes = CopyArray_EM_PTB (hGlbRht, lptkFunc);
        aplRankRes = aplRankRht;
    } else
        DbgStop ();     // We should never get here

    if (!*lphGlbRes)
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)

    // Add in the singleton's value to the result offset
    lpAPA->Off += aplInteger;

#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemRes = NULL;

    // Fill in the result token
    if (lpYYRes)
    {
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (*lphGlbRes);
    } // End IF

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    return bRet;
} // End PrimFnDydPlusAPA_EM


//***************************************************************************
//  End of File: pf_plus.c
//***************************************************************************