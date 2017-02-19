//***************************************************************************
//  NARS2000 -- Primitive Function -- Bar
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
PRIMSPEC PrimSpecBar =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecBarStorageTypeMon,
    &PrimFnMonBarAPA_EM,

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonBarBisB, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarBisI, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarBisF, -- Can't happen w/Bar

    // Monadic non-Boolean result functions (indexable)
    &PrimFnMonBarIisI,
    NULL,   // &PrimFnMonBarIisF, -- Can't happen w/Bar
    &PrimFnMonBarFisI,
    &PrimFnMonBarFisF,
    &PrimFnMonBarRisR,
    &PrimFnMonBarVisR,
    &PrimFnMonBarVisV,

    // Monadic Hypercomplex functions
    &PrimFnMonBarHC2IisHC2I,
    NULL,   // &PrimFnMonBarHC2IisHC2F, -- Can't happen w/Bar
    &PrimFnMonBarHC2FisHC2I,
    &PrimFnMonBarHC2FisHC2F,
    &PrimFnMonBarHC2RisHC2R,
    &PrimFnMonBarHC2VisHC2R,
    &PrimFnMonBarHC2VisHC2V,

    &PrimFnMonBarHC4IisHC4I,
    NULL,   // &PrimFnMonBarHC4IisHC4F, -- Can't happen w/Bar
    &PrimFnMonBarHC4FisHC4I,
    &PrimFnMonBarHC4FisHC4F,
    &PrimFnMonBarHC4RisHC4R,
    &PrimFnMonBarHC4VisHC4R,
    &PrimFnMonBarHC4VisHC4V,

    &PrimFnMonBarHC8IisHC8I,
    NULL,   // &PrimFnMonBarHC8IisHC8F, -- Can't happen w/Bar
    &PrimFnMonBarHC8FisHC8I,
    &PrimFnMonBarHC8FisHC8F,
    &PrimFnMonBarHC8RisHC8R,
    &PrimFnMonBarHC8VisHC8R,
    &PrimFnMonBarHC8VisHC8V,

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonBarFisHC2I, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarFisHC2F, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarVisHC2R, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarVisHC2V, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarFisHC4I, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarFisHC4F, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarVisHC4R, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarVisHC4V, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarFisHC8I, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarFisHC8F, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarVisHC8R, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarVisHC8V, -- Can't happen w/Bar

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecBarStorageTypeDyd,
    &PrimFnDydBarAPA_EM,

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydBarBisBvB, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisIvI, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisFvF, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisCvC, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisHvH, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisRvR, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisVvV, -- Can't happen w/Bar

    NULL,   // &PrimFnDydBarBisHC2IvHC2I, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisHC2FvHC2F, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisHC2RvHC2R, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisHC2VvHC2V, -- Can't happen w/Bar

    NULL,   // &PrimFnDydBarBisHC4IvHC4I, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisHC4FvHC4F, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisHC4RvHC4R, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisHC4VvHC4V, -- Can't happen w/Bar

    NULL,   // &PrimFnDydBarBisHC8IvHC8I, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisHC8FvHC8F, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisHC8RvHC8R, -- Can't happen w/Bar
    NULL,   // &PrimFnDydBarBisHC8VvHC8V, -- Can't happen w/Bar

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydBarIisIvI,
    NULL,   // &PrimFnDydBarIisFvF, -- Can't happen w/Bar
    &PrimFnDydBarFisIvI,
    &PrimFnDydBarFisFvF,
    &PrimFnDydBarRisRvR,
    &PrimFnDydBarVisRvR,
    &PrimFnDydBarVisVvV,

    &PrimFnDydBarHC2IisHC2IvHC2I,
    NULL,   // &PrimFnDydBarHC2IisHC2FvHC2F, -- Can't happen w/Bar
    &PrimFnDydBarHC2FisHC2IvHC2I,
    &PrimFnDydBarHC2FisHC2FvHC2F,
    &PrimFnDydBarHC2RisHC2RvHC2R,
    &PrimFnDydBarHC2VisHC2RvHC2R,
    &PrimFnDydBarHC2VisHC2VvHC2V,

    &PrimFnDydBarHC4IisHC4IvHC4I,
    NULL,   // &PrimFnDydBarHC4IisHC4FvHC4F, -- Can't happen w/Bar
    &PrimFnDydBarHC4FisHC4IvHC4I,
    &PrimFnDydBarHC4FisHC4FvHC4F,
    &PrimFnDydBarHC4RisHC4RvHC4R,
    &PrimFnDydBarHC4VisHC4RvHC4R,
    &PrimFnDydBarHC4VisHC4VvHC4V,

    &PrimFnDydBarHC8IisHC8IvHC8I,
    NULL,   // &PrimFnDydBarHC8IisHC8FvHC8F, -- Can't happen w/Bar
    &PrimFnDydBarHC8FisHC8IvHC8I,
    &PrimFnDydBarHC8FisHC8FvHC8F,
    &PrimFnDydBarHC8RisHC8RvHC8R,
    &PrimFnDydBarHC8VisHC8RvHC8R,
    &PrimFnDydBarHC8VisHC8VvHC8V,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonBarB64isB64, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarB32isB32, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarB16isB16, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarB08isB08, -- Can't happen w/Bar

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnMonBarB64isB64vB64, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarB32isB32vB32, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarB16isB16vB16, -- Can't happen w/Bar
    NULL,   // &PrimFnMonBarB08isB08vB08, -- Can't happen w/Bar

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecBar};
#endif


//***************************************************************************
//  $PrimFnBar_EM_YY
//
//  Primitive function for monadic and dyadic Bar ("negation" and "subtraction")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnBar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnBar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharBar (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnBar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecBarStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecBarStorageTypeMon
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

    // No Boolean result for negation
    if (IsSimpleBool (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecBarStorageTypeMon


//***************************************************************************
//  $NegHC1I_RE
//***************************************************************************

APLINT NegHC1I_RE
    (APLINT aplRht)                 // Right arg

{
    // Check for overflow or -0
    if (aplRht EQ MIN_APLINT
     || (gAllowNeg0 && aplRht EQ 0))
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
////else
        return -aplRht;
} // End NegHC1I_RE


//***************************************************************************
//  $PrimFnMonBarIisI
//
//  Primitive scalar function monadic Bar:  I {is} fn I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarIisI"
#else
#define APPEND_NAME
#endif

void PrimFnMonBarIisI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    // Check for overflow or -0
    if (lpatRht->aplInteger EQ MIN_APLINT
     || (gAllowNeg0 && lpatRht->aplInteger EQ 0))
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = NegHC1I_RE (lpatRht->aplInteger);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_FLOAT);
} // End PrimFnMonBarIisI
#undef  APPEND_NAME


//***************************************************************************
//  $NegHC1F_RE
//***************************************************************************

APLFLOAT NegHC1F_RE
    (APLFLOAT aplRht)               // Right arg

{
    // No exceptions in this code

    // Check for -0
    if (!gAllowNeg0
     && aplRht EQ 0)
        return 0.0;
    else
        return -aplRht;
} // End NegHC1F_RE


//***************************************************************************
//  $PrimFnMonBarFisI
//
//  Primitive scalar function monadic Bar:  F {is} fn I
//***************************************************************************

void PrimFnMonBarFisI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = NegHC1F_RE ((APLFLOAT) lpatRht->aplInteger);
} // End PrimFnMonBarFisI


//***************************************************************************
//  $PrimFnMonBarFisF
//
//  Primitive scalar function monadic Bar:  F {is} fn F
//***************************************************************************

void PrimFnMonBarFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call subfunction
    lpMemRes[uRes] = NegHC1F_RE (lpatRht->aplFloat);
} // End PrimFnMonBarFisF


//***************************************************************************
//  $NegHC1R_RE
//***************************************************************************

APLRAT NegHC1R_RE
    (APLRAT aplRht)                 // Right arg

{
    APLRAT aplRes;

    // Check for -0
    if (gAllowNeg0
     && IsMpq0 (&aplRht))
        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);

    // Initialize the result to 0/1
    mpq_init (&aplRes);

    // Negate the Rational
    mpq_neg (&aplRes, &aplRht);

    return aplRes;
} // End NegHC1R_RE


//***************************************************************************
//  $PrimFnMonBarRisR
//
//  Primitive scalar function monadic Bar:  R {is} fn R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarRisR"
#else
#define APPEND_NAME
#endif

void PrimFnMonBarRisR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = NegHC1R_RE (lpatRht->aplRat);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_VFP);
} // End PrimFnMonBarRisR
#undef APPEND_NAME


//***************************************************************************
//  $NegHC1V_RE
//***************************************************************************

APLVFP NegHC1V_RE
    (APLVFP aplRht)                 // Right arg

{
    APLVFP aplRes;                  // The result

    // No exceptions in this code

    // Initialize the result to 0
    mpfr_init0 (&aplRes);

    // Check for -0
    if (gAllowNeg0
     || !IsMpf0 (&aplRht))
        // Negate the VFP
        mpfr_neg (&aplRes, &aplRht, MPFR_RNDN);

    return aplRes;
} // End NegHC1V_RE


//***************************************************************************
//  $PrimFnMonBarVisR
//
//  Primitive scalar function monadic Bar:  V {is} fn R
//***************************************************************************

void PrimFnMonBarVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = NegHC1V_RE (atRht.aplVfp);

    Myhc1v_clear (&atRht.aplHC1V);
} // End PrimFnMonBarVisR


//***************************************************************************
//  $PrimFnMonBarVisV
//
//  Primitive scalar function monadic Bar:  V {is} fn V
//***************************************************************************

void PrimFnMonBarVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = NegHC1V_RE (lpatRht->aplVfp);
} // End PrimFnMonBarVisV


//***************************************************************************
//  $NegHC2I_RE
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- NegHC2I_RE"
#else
#define APPEND_NAME
#endif

APLHC2I NegHC2I_RE
    (APLHC2I aplRht)                    // Right arg

{
    int     i;                          // Loop counter
    APLHC2I aplRes;                     // The result

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Change the sign of all parts
        aplRes.parts[i] = NegHC1I_RE (aplRht.parts[i]);

    return aplRes;
} // End NegHC2I_RE
#undef APPEND_NAME


//***************************************************************************
//  $PrimFnMonBarHC2IisHC2I
//
//  Primitive scalar function monadic Bar:  HC2I {is} fn HC2I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC2IisHC2I"
#else
#define APPEND_NAME
#endif

void PrimFnMonBarHC2IisHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = NegHC2I_RE (lpatRht->aplHC2I);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check on exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2F);
} // End PrimFnMonBarHC2IisHC2I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonBarHC2FisHC2I
//
//  Primitive scalar function monadic Bar:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonBarHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;                          // Loop counter

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Change the sign of all parts
        lpMemRes[uRes].parts[i] = -(APLFLOAT) lpatRht->aplHC2I.parts[i];
} // End PrimFnMonBarHC2FisHC2I


//***************************************************************************
//  $NegHC2F_RE
//***************************************************************************

APLHC2F NegHC2F_RE
    (APLHC2F aplRht)                    // Right arg

{
    int     i;
    APLHC2F aplRes;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Change the sign of all parts
        aplRes.parts[i] = -aplRht.parts[i];

    return aplRes;
} // End NegHC2F_RE


//***************************************************************************
//  $PrimFnMonBarHC2FisHC2F
//
//  Primitive scalar function monadic Bar:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonBarHC2FisHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = NegHC2F_RE (lpatRht->aplHC2F);
} // End PrimFnMonBarHC2FisHC2F


//***************************************************************************
//  $NegHC2R_RE
//***************************************************************************

APLHC2R NegHC2R_RE
    (APLHC2R aplRht)                    // Right arg

{
    int     i;
    APLHC2R aplRes = {0};

    // Initialize to 0/1
    mphc2r_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
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
} // End NegHC2R_RE


//***************************************************************************
//  $PrimFnMonBarHC2RisHC2R
//
//  Primitive scalar function monadic Bar:  HC2R {is} fn HC2R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC2RisHC2R"
#else
#define APPEND_NAME
#endif

void PrimFnMonBarHC2RisHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = NegHC2R_RE (lpatRht->aplHC2R);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2V);
} // End PrimFnMonBarHC2RisHC2R
#undef APPEND_NAME


//***************************************************************************
//  $NegHC2V_RE
//***************************************************************************

APLHC2V NegHC2V_RE
    (APLHC2V aplRht)                    // Right arg

{
    int     i;
    APLHC2V aplRes = {0};

    // No exceptions in this code

    // Initialize to 0
    mphc2v_init0 (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Change the sign of all parts
        mpfr_neg (&aplRes.parts[i], &aplRht.parts[i], MPFR_RNDN);

    return aplRes;
} // End NegHC2V_RE


//***************************************************************************
//  $PrimFnMonBarHC2VisHC2R
//
//  Primitive scalar function monadic Bar:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonBarHC2VisHC2R
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
    lpMemRes[uRes] = NegHC2V_RE (atRht.aplHC2V);

    Myhc2v_clear (&atRht.aplHC2V);
} // End PrimFnMonBarHC2VisHC2R


//***************************************************************************
//  $PrimFnMonBarHC2VisHC2V
//
//  Primitive scalar function monadic Bar:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonBarHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = NegHC2V_RE (lpatRht->aplHC2V);
} // End PrimFnMonBarHC2VisHC2V


//***************************************************************************
//  $NegHC4I_RE
//***************************************************************************

APLHC4I NegHC4I_RE
    (APLHC4I aplRht)                    // Right arg

{
    int     i;
    APLHC4I aplRes;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Change the sign of all parts
        aplRes.parts[i] = NegHC1I_RE (aplRht.parts[i]);

    return aplRes;
} // End NegHC4I_RE


//***************************************************************************
//  $PrimFnMonBarHC4IisHC4I
//
//  Primitive scalar function monadic Bar:  HC4I {is} fn HC4I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC4IisHC4I"
#else
#define APPEND_NAME
#endif

void PrimFnMonBarHC4IisHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = NegHC4I_RE (lpatRht->aplHC4I);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check on exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4F);
} // End PrimFnMonBarHC4IisHC4I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonBarHC4FisHC4I
//
//  Primitive scalar function monadic Bar:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonBarHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Change the sign of all parts
        lpMemRes[uRes].parts[i] = -(APLFLOAT) lpatRht->aplHC4I.parts[i];
} // End PrimFnMonBarHC4FisHC4I


//***************************************************************************
//  $NegHC4F_RE
//***************************************************************************

APLHC4F NegHC4F_RE
    (APLHC4F aplRht)                    // Right arg

{
    int     i;
    APLHC4F aplRes;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Change the sign of all of the parts
        aplRes.parts[i] = -aplRht.parts[i];

    return aplRes;
} // End NegHC4F_RE


//***************************************************************************
//  $PrimFnMonBarHC4FisHC4F
//
//  Primitive scalar function monadic Bar:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonBarHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = NegHC4F_RE (lpatRht->aplHC4F);
} // End PrimFnMonBarHC4FisHC4F


//***************************************************************************
//  $NegHC4R_RE
//***************************************************************************

APLHC4R NegHC4R_RE
    (APLHC4R aplRht)                    // Right arg

{
    int     i;
    APLHC4R aplRes = {0};

    // Initialize to 0/1
    mphc4r_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Check for -0
        if (gAllowNeg0
         && IsMpq0 (&aplRht.parts[i]))
            RaiseException (EXCEPTION_RESULT_HC4V, 0, 0, NULL);
        else
            // Change the sign of all of the parts
            mpq_neg (&aplRes.parts[i], &aplRht.parts[i]);
    } // End FOR

    return aplRes;
} // End NegHC4R_RE


//***************************************************************************
//  $PrimFnMonBarHC4RisHC4R
//
//  Primitive scalar function monadic Bar:  HC4R {is} fn HC4R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC4RisHC4R"
#else
#define APPEND_NAME
#endif

void PrimFnMonBarHC4RisHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = NegHC4R_RE (lpatRht->aplHC4R);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check on exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4V);
} // End PrimFnMonBarHC4RisHC4R
#undef  APPEND_NAME


//***************************************************************************
//  $NegHC4V_RE
//***************************************************************************

APLHC4V NegHC4V_RE
    (APLHC4V aplRht)                    // Right arg

{
    int     i;
    APLHC4V aplRes = {0};

    // No exceptions in this code

    // Initialize to 0
    mphc4v_init0 (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Change the sign of all of the parts
        mpfr_neg (&aplRes.parts[i], &aplRht.parts[i], MPFR_RNDN);

    return aplRes;
} // End NegHC4V_RE


//***************************************************************************
//  $PrimFnMonBarHC4VisHC4R
//
//  Primitive scalar function monadic Bar:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonBarHC4VisHC4R
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
    lpMemRes[uRes] = NegHC4V_RE (atRht.aplHC4V);

    Myhc4v_clear (&atRht.aplHC4V);
} // End PrimFnMonBarHC4VisHC4R


//***************************************************************************
//  $PrimFnMonBarHC4VisHC4V
//
//  Primitive scalar function monadic Bar:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonBarHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = NegHC4V_RE (lpatRht->aplHC4V);
} // End PrimFnMonBarHC4VisHC4V


//***************************************************************************
//  $NegHC8I_RE
//***************************************************************************

APLHC8I NegHC8I_RE
    (APLHC8I aplRht)                    // Right arg

{
    int     i;
    APLHC8I aplRes;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Change the sign of the imaginary part
        aplRes.parts[i] = NegHC1I_RE (aplRht.parts[i]);

    return aplRes;
} // End NegHC8I_RE


//***************************************************************************
//  $PrimFnMonBarHC8IisHC8I
//
//  Primitive scalar function monadic Bar:  HC8I {is} fn HC8I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC8IisHC8I"
#else
#define APPEND_NAME
#endif

void PrimFnMonBarHC8IisHC8I
    (LPAPLHC8I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = NegHC8I_RE (lpatRht->aplHC8I);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8F);
} // End PrimFnMonBarHC8IisHC8I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonBarHC8FisHC8I
//
//  Primitive scalar function monadic Bar:  HC8F {is} fn HC8I
//***************************************************************************

void PrimFnMonBarHC8FisHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Change the sign of all parts
        lpMemRes[uRes].parts[i] = -(APLFLOAT) lpatRht->aplHC8I.parts[i];
} // End PrimFnMonBarHC8FisHC8I


//***************************************************************************
//  $NegHC8F_RE
//***************************************************************************

APLHC8F NegHC8F_RE
    (APLHC8F aplRht)                // Right arg

{
    int     i;
    APLHC8F aplRes;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Change the sign of all of the parts
        aplRes.parts[i] = -aplRht.parts[i];

    return aplRes;
} // End NegHC8F_RE


//***************************************************************************
//  $PrimFnMonBarHC8FisHC8F
//
//  Primitive scalar function monadic Bar:  HC8F {is} fn HC8F
//***************************************************************************

void PrimFnMonBarHC8FisHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = NegHC8F_RE (lpatRht->aplHC8F);
} // End PrimFnMonBarHC8FisHC8F


//***************************************************************************
//  $NegHC8R_RE
//***************************************************************************

APLHC8R NegHC8R_RE
    (APLHC8R aplRht)                    // Right arg

{
    int     i;
    APLHC8R aplRes = {0};

    // Initialize to 0/1
    mphc8r_init (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Check for -0
        if (gAllowNeg0
         && IsMpq0 (&aplRht.parts[i]))
            RaiseException (EXCEPTION_RESULT_HC8V, 0, 0, NULL);
        else
            // Change the sign of all of the parts
            mpq_neg (&aplRes.parts[i], &aplRht.parts[i]);
    } // End FOR

    return aplRes;
} // End NegHC8R_RE


//***************************************************************************
//  $PrimFnMonBarHC8RisHC8R
//
//  Primitive scalar function monadic Bar:  HC8R {is} fn HC8R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC8RisHC8R"
#else
#define APPEND_NAME
#endif

void PrimFnMonBarHC8RisHC8R
    (LPAPLHC8R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = NegHC8R_RE (lpatRht->aplHC8R);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check on exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8V);
} // End PrimFnMonBarHC8RisHC8R
#undef  APPEND_NAME


//***************************************************************************
//  $NegHC8V_RE
//***************************************************************************

APLHC8V NegHC8V_RE
    (APLHC8V aplRht)                    // Right arg

{
    int     i;
    APLHC8V aplRes = {0};

    // No exceptions in this code

    // Initialize to 0
    mphc8v_init0 (&aplRes);

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Change the sign of all of the parts
        mpfr_neg (&aplRes.parts[i], &aplRht.parts[i], MPFR_RNDN);

    return aplRes;
} // End NegHC8V_RE


//***************************************************************************
//  $PrimFnMonBarHC8VisHC8R
//
//  Primitive scalar function monadic Bar:  HC8V {is} fn HC8R
//***************************************************************************

void PrimFnMonBarHC8VisHC8R
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
    lpMemRes[uRes] = NegHC8V_RE (atRht.aplHC8V);

    Myhc8v_clear (&atRht.aplHC8V);
} // End PrimFnMonBarHC8VisHC8R


//***************************************************************************
//  $PrimFnMonBarHC8VisHC8V
//
//  Primitive scalar function monadic Bar:  HC8V {is} fn HC8V
//***************************************************************************

void PrimFnMonBarHC8VisHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = NegHC8V_RE (lpatRht->aplHC8V);
} // End PrimFnMonBarHC8VisHC8V


//***************************************************************************
//  $PrimFnMonBarAPA_EM
//
//  Monadic Bar, result is APA
//***************************************************************************

UBOOL PrimFnMonBarAPA_EM
    (LPPL_YYSTYPE  lpYYRes,         // Ptr to the result (may be NULL)
     LPTOKEN       lptkFunc,        // Ptr to function token
     HGLOBAL       hGlbRht,         // Handle to right arg
     HGLOBAL      *lphGlbRes,       // Ptr to handle to result
     APLRANK       aplRankRht,      // Rank of the right arg
     LPPRIMSPEC    lpPrimSpec)      // Ptr to local PRIMSPEC

{
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPVOID            lpMemRes;             // Ptr to result global memory
    APLNELM           aplNELMRes;           // Result NELM
    APLRANK           aplRankRes;           // Result rank
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid

    // Axis may be anything

    *lphGlbRes = CopyArray_EM_PTB (hGlbRht, lptkFunc);
    if (!*lphGlbRes)
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

#define lpHeader    lpMemHdrRes
    aplNELMRes = lpHeader->NELM;
    aplRankRes = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Check for un-negatable integer
#define lpAPA       ((LPAPLAPA) lpMemRes)
    if (lpAPA->Off EQ MIN_APLINT
     || (lpAPA->Off + aplNELMRes * lpAPA->Mul) EQ MIN_APLINT
     || (gAllowNeg0
      && (lpAPA->Off EQ 0
       || (lpAPA->Off + aplNELMRes * lpAPA->Mul) EQ 0
       || signumint (lpAPA->Off) NE signumint (lpAPA->Off + aplNELMRes * lpAPA->Mul)))
       )
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    // Negating the offset and multiplier negates the APA
    lpAPA->Off = -lpAPA->Off;
    lpAPA->Mul = -lpAPA->Mul;
#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

    // Fill in the result token
    if (lpYYRes NE NULL)
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
} // End PrimFnMonBarAPA_EM


//***************************************************************************
//  $PrimSpecBarStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecBarStorageTypeDyd
    (APLNELM    aplNELMLft,
     LPAPLSTYPE lpaplTypeLft,
     LPTOKEN    lptkFunc,
     APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht)

{
    APLSTYPE aplTypeRes;            // Result storage type

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

    // No Boolean results for subtraction
    if (IsSimpleBool (aplTypeRes))
        aplTypeRes = ARRAY_INT;

    // Special case subtraction with APA
    if (aplTypeRes EQ ARRAY_INT                            // Res = INT
     && (!IsSingleton (aplNELMLft) || !IsSingleton (aplNELMRht))    // Not both singletons
     && ((IsSingleton (aplNELMLft) && IsSimpleAPA (*lpaplTypeRht))  // Non-singleton is APA
      || (IsSingleton (aplNELMRht) && IsSimpleAPA (*lpaplTypeLft))))// ...
        aplTypeRes = ARRAY_APA;

    return aplTypeRes;
} // End PrimSpecBarStorageTypeDyd


//***************************************************************************
//  $SubHC1I_RE
//***************************************************************************

APLINT SubHC1I_RE
    (APLINT aplLft,                     // Left arg
     APLINT aplRht)                     // Right arg

{
    return isub64_RE (aplLft, aplRht, FLOAT);
} // End SubHC1I_RE


//***************************************************************************
//  $PrimFnDydBarIisIvI
//
//  Primitive scalar function dyadic Bar:  I {is} I fn I
//***************************************************************************

void PrimFnDydBarIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Subtract the two integers and signal overflow execption in <SubHC1I>
    lpMemRes[uRes] = SubHC1I_RE (lpatLft->aplInteger, lpatRht->aplInteger);
} // End PrimFnDydBarIisIvI


//***************************************************************************
//  $PrimFnDydBarFisIvI
//
//  Primitive scalar function dyadic Bar:  F {is} I fn I
//***************************************************************************

void PrimFnDydBarFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = ((APLFLOAT) (APLINT) lpatLft->aplInteger)
                   - ((APLFLOAT) (APLINT) lpatRht->aplInteger);
} // End PrimFnDydBarFisIvI


//***************************************************************************
//  $SubHC1F_RE
//***************************************************************************

APLFLOAT SubHC1F_RE
    (APLFLOAT aplLft,                   // Left arg
     APLFLOAT aplRht)                   // Right ...

{
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1F, &aplRht, 0);

    // Check for indeterminates:  _ - _  or  -_ - -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            return aplLft;
        else
            return aplRht;
    } else
    // If the args are both infinite and of the same signs, ...
    if (_isinf (aplLft)
     && _isinf (aplRht)
     && SIGN_APLFLOAT (aplLft) EQ SIGN_APLFLOAT (aplRht))
        return TranslateQuadICIndex (aplLft,
                                     ICNDX_InfSUBInf,
                                     aplRht,
                                     FALSE);
    else
        // Subtract the two floats
        return aplLft - aplRht;
} // End SubHC1F_RE


//***************************************************************************
//  $PrimFnDydBarFisFvF
//
//  Primitive scalar function dyadic Bar:  F {is} F fn F
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydBarFisFvF"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarFisFvF
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
        lpMemRes[uRes] = SubHC1F_RE (lpatLft->aplFloat, lpatRht->aplFloat);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydBarFisFvF
#undef  APPEND_NAME


//***************************************************************************
//  $SubHC1R_RE
//***************************************************************************

APLRAT SubHC1R_RE
    (APLRAT aplLft,                     // Left arg
     APLRAT aplRht)                     // Right ...

{
    APLRAT aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1R, &aplRht, 0);

    // Check for indeterminates:  _ - _  or  -_ - -_

    // If either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc1r_init_set (&aplRes, &aplLft);
        else
            mphc1r_init_set (&aplRes, &aplRht);
    } else
    // If the args are both infinite and of the same signs, ...
    if (mpq_inf_p (&aplLft)
     && mpq_inf_p (&aplRht)
     && mpq_sgn (&aplLft) EQ mpq_sgn (&aplRht))
        mpq_init_set (&aplRes,
                       mpq_QuadICValue (&aplRht,   // No left arg
                                         ICNDX_InfSUBInf,
                                        &aplRht,
                                        &aplRes,
                                         FALSE));
    else
    {
        // Initialize the result to 0/1
        mpq_init (&aplRes);

        // Subtract the Rationals
        mpq_sub (&aplRes, &aplLft, &aplRht);
    } // End IF/ELSE/...

    return aplRes;
} // End SubHC1R_RE


//***************************************************************************
//  $PrimFnDydBarRisRvR
//
//  Primitive scalar function dyadic Bar:  R {is} R fn R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydBarRisRvR"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarRisRvR
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
        lpMemRes[uRes] = SubHC1R_RE (lpatLft->aplRat, lpatRht->aplRat);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_VFP);
} // End PrimFnDydBarRisRvR
#undef  APPEND_NAME


//***************************************************************************
//  $SubHC1V_RE
//***************************************************************************

APLVFP SubHC1V_RE
    (APLVFP aplLft,                     // Left arg
     APLVFP aplRht)                     // Right ...

{
    APLVFP aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1V, &aplRht, 0);

    // Check for indeterminates:  _ - _  or  -_ - -_

    // If either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc1v_init_set (&aplRes, &aplLft);
        else
            mphc1v_init_set (&aplRes, &aplRht);
    } else
    // If the args are both infinite and of the same signs, ...
    if (mpfr_inf_p (&aplLft)
     && mpfr_inf_p (&aplRht)
     && mpfr_sgn (&aplLft) EQ mpfr_sgn (&aplRht))
        mpfr_init_set (&aplRes,
                        mpfr_QuadICValue (&aplRht, // No left arg
                                           ICNDX_InfSUBInf,
                                          &aplRht,
                                          &aplRes,
                                           FALSE),
                        MPFR_RNDN);
    else
    {
        // Initialize the result to 0
        mpfr_init0 (&aplRes);

        // Subtract the VFPs
        mpfr_sub (&aplRes, &aplLft, &aplRht, MPFR_RNDN);
    } // End IF/ELSE/...

    return aplRes;
} // End SubHC1V_RE


//***************************************************************************
//  $PrimFnDydBarVisRvR
//
//  Primitive scalar function dyadic Bar:  V {is} R fn R
//***************************************************************************

void PrimFnDydBarVisRvR
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

    PrimFnDydBarVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydBarVisRvR


//***************************************************************************
//  $PrimFnDydBarVisVvV
//
//  Primitive scalar function dyadic Bar:  V {is} V fn V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydBarVisVvV"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarVisVvV
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
        lpMemRes[uRes] = SubHC1V_RE (lpatLft->aplVfp, lpatRht->aplVfp);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check on exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydBarVisVvV
#undef  APPEND_NAME


//***************************************************************************
//  $SubHC2I_RE
//***************************************************************************

APLHC2I SubHC2I_RE
    (APLHC2I aplLft,                // Left arg
     APLHC2I aplRht)                // Right ...

{
    int     i;
    APLHC2I aplRes;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Subtract the two integers
        aplRes.parts[i] = SubHC1I_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End SubHC2I_RE


//***************************************************************************
//  $PrimFnDydBarHC2IisHC2IvHC2I
//
//  Primitive scalar function dyadic Bar:  HC2I {is} HC2I fn HC2I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC2IisHC2IvHC2I"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarHC2IisHC2IvHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Subtract the two integers and signal overflow exception in <isub64_RE>
        lpMemRes[uRes] = SubHC2I_RE (lpatLft->aplHC2I, lpatRht->aplHC2I);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2F);
} // End PrimFnDydBarHC2IisHC2IvHC2I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydBarHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic Bar:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydBarHC2FisHC2IvHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Convert the integers to FLOAT and Subtract
        lpMemRes[uRes].parts[i] = (APLFLOAT) lpatLft->aplHC2I.parts[i] - (APLFLOAT) lpatRht->aplHC2I.parts[i];
} // End PrimFnDydBarHC2FisHC2IvHC2I


//***************************************************************************
//  $SubHC2F_RE
//***************************************************************************

APLHC2F SubHC2F_RE
    (APLHC2F aplLft,                // Left arg
     APLHC2F aplRht)                // Right ...

{
    int     i;
    APLHC2F aplRes;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC2F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC2F, &aplRht, 0);

    // No exceptions in this code

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            aplRes = aplLft;
        else
            aplRes = aplRht;
    } else
    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Subtract the two integers
        aplRes.parts[i] = SubHC1F_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End SubHC2F_RE


//***************************************************************************
//  $PrimFnDydBarHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic Bar:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydBarHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = SubHC2F_RE (lpatLft->aplHC2F, lpatRht->aplHC2F);
} // End PrimFnDydBarHC2FisHC2FvHC2F


//***************************************************************************
//  $SubHC2R_RE
//***************************************************************************

APLHC2R SubHC2R_RE
    (APLHC2R aplLft,                    // Left arg
     APLHC2R aplRht)                    // Right ...

{
    int     i;
    APLHC2R aplRes;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC2R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC2R, &aplRht, 0);

    // If either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc2r_init_set (&aplRes, &aplLft);
        else
            mphc2r_init_set (&aplRes, &aplRht);
    } else
    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of same signs, ...
        if (mpq_inf_p (&aplLft.parts[i])
         && mpq_inf_p (&aplRht.parts[i])
         && mpq_sgn (&aplLft.parts[i]) EQ mpq_sgn (&aplRht.parts[i]))
            mpq_init_set (&aplRes.parts[i],
                           mpq_QuadICValue (&aplRht.parts[i], // No left arg
                                             ICNDX_InfSUBInf,
                                            &aplRht.parts[i],
                                            &aplRes.parts[i],
                                             FALSE));
        else
        {
            // Initialize the result to 0/1
            mpq_init (&aplRes.parts[i]);

            // Subtract the two Rationals
            mpq_sub (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i]);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End SubHC2R_RE


//***************************************************************************
//  $PrimFnDydBarHC2RisHC2RvHC2R
//
//  Primitive scalar function dyadic Bar:  HC2R {is} HC2R fn HC2R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC2RisHC2RvHC2R"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarHC2RisHC2RvHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;
    int i;

    __try
    {
        // Loop through all of the parts
        for (i = 0; i < 2; i++)
            // Call subfunction
            lpMemRes[uRes] = SubHC2R_RE (lpatLft->aplHC2R, lpatRht->aplHC2R);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2V);
} // End PrimFnDydBarHC2RisHC2RvHC2R
#undef  APPEND_NAME


//***************************************************************************
//  $SubHC2V_RE
//***************************************************************************

APLHC2V SubHC2V_RE
    (APLHC2V aplLft,                    // Left arg
     APLHC2V aplRht)                    // Right ...

{
    int     i;
    APLHC2V aplRes;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC2V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC2V, &aplRht, 0);

    // If either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc2v_init_set (&aplRes, &aplLft);
        else
            mphc2v_init_set (&aplRes, &aplRht);
    } else
    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of same signs, ...
        if (mpfr_inf_p (&aplLft.parts[i])
         && mpfr_inf_p (&aplRht.parts[i])
         && mpfr_sgn (&aplLft.parts[i]) EQ mpfr_sgn (&aplRht.parts[i]))
            mpfr_init_set (&aplRes.parts[i],
                            mpfr_QuadICValue (&aplRht.parts[i], // No left arg
                                               ICNDX_InfSUBInf,
                                              &aplRht.parts[i],
                                              &aplRes.parts[i],
                                               FALSE),
                            MPFR_RNDN);
        else
        {
            // Initialize the result to 0
            mpfr_init0 (&aplRes.parts[i]);

            // Subtract the two VFPs
            mpfr_sub (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i], MPFR_RNDN);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End SubHC2V_RE


//***************************************************************************
//  $PrimFnDydBarHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic Bar:  HC2V {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydBarHC2VisHC2RvHC2R
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

    PrimFnDydBarHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
    Myhc2v_clear (&atLft.aplHC2V);
} // End PrimFnDydBarHC2VisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydBarHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic Bar:  HC2V {is} HC2V fn HC2V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC2VisHC2VvHC2V"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarHC2VisHC2VvHC2V
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
        lpMemRes[uRes] = SubHC2V_RE (lpatLft->aplHC2V, lpatRht->aplHC2V);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydBarHC2VisHC2VvHC2V
#undef  APPEND_NAME


//***************************************************************************
//  $SubHC4I_RE
//***************************************************************************

APLHC4I SubHC4I_RE
    (APLHC4I aplLft,                // Left arg
     APLHC4I aplRht)                // Right ...

{
    int     i;
    APLHC4I aplRes;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Subtract the two integers
        aplRes.parts[i] = SubHC1I_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End SubHC4I_RE


//***************************************************************************
//  $PrimFnDydBarHC4IisHC4IvHC4I
//
//  Primitive scalar function dyadic Bar:  HC4I {is} HC4I fn HC4I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC4IisHC4IvHC4I"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarHC4IisHC4IvHC4I
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
        lpMemRes[uRes] = SubHC4I_RE (lpatLft->aplHC4I, lpatRht->aplHC4I);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4F);
} // End PrimFnDydBarHC4IisHC4IvHC4I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydBarHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic Bar:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydBarHC4FisHC4IvHC4I
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
        // Convert the integers to FLOAT and Subtract
        lpMemRes[uRes].parts[i] = (APLFLOAT) lpatLft->aplHC4I.parts[i] - (APLFLOAT) lpatRht->aplHC4I.parts[i];
} // End PrimFnDydBarHC4FisHC4IvHC4I


//***************************************************************************
//  $SubHC4F_RE
//***************************************************************************

APLHC4F SubHC4F_RE
    (APLHC4F aplLft,                // Left arg
     APLHC4F aplRht)                // Right ...

{
    int     i;
    APLHC4F aplRes;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC4F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC4F, &aplRht, 0);

    // No exceptions in this code

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            aplRes = aplLft;
        else
            aplRes = aplRht;
    } else
    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Subtract the two FLOATs
        aplRes.parts[i] = SubHC1F_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End SubHC4F_RE


//***************************************************************************
//  $PrimFnDydBarHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic Bar:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydBarHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Subtract the floats
    lpMemRes[uRes] = SubHC4F_RE (lpatLft->aplHC4F, lpatRht->aplHC4F);
} // End PrimFnDydBarHC4FisHC4FvHC4F


//***************************************************************************
//  $SubHC4R_RE
//***************************************************************************

APLHC4R SubHC4R_RE
    (APLHC4R aplLft,                    // Left arg
     APLHC4R aplRht)                    // Right ...

{
    int     i;
    APLHC4R aplRes;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC4R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC4R, &aplRht, 0);

    // If either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc4r_init_set (&aplRes, &aplLft);
        else
            mphc4r_init_set (&aplRes, &aplRht);
    } else
    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of same signs, ...
        if (mpq_inf_p (&aplLft.parts[i])
         && mpq_inf_p (&aplRht.parts[i])
         && mpq_sgn (&aplLft.parts[i]) EQ mpq_sgn (&aplRht.parts[i]))
            mpq_init_set (&aplRes.parts[i],
                           mpq_QuadICValue (&aplRht.parts[i], // No left arg
                                             ICNDX_InfSUBInf,
                                            &aplRht.parts[i],
                                            &aplRes.parts[i],
                                             FALSE));
        else
        {
            // Initialize the result to 0/1
            mpq_init (&aplRes.parts[i]);

            // Subtract the two Rationals
            mpq_sub (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i]);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End SubHC4R_RE


//***************************************************************************
//  $PrimFnDydBarHC4RisHC4RvHC4R
//
//  Primitive scalar function dyadic Bar:  HC4R {is} HC4R fn HC4R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC4RisHC4RvHC4R"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarHC4RisHC4RvHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;
    int i;

    __try
    {
        // Loop through all of the parts
        for (i = 0; i < 4; i++)
            // Call subfunction
            lpMemRes[uRes] = SubHC4R_RE (lpatLft->aplHC4R, lpatRht->aplHC4R);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4V);
} // End PrimFnDydBarHC4RisHC4RvHC4R
#undef  APPEND_NAME


//***************************************************************************
//  $SubHC4V_RE
//***************************************************************************

APLHC4V SubHC4V_RE
    (APLHC4V aplLft,                    // Left arg
     APLHC4V aplRht)                    // Right ...

{
    int     i;
    APLHC4V aplRes;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC4V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC4V, &aplRht, 0);

    // If either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc4v_init_set (&aplRes, &aplLft);
        else
            mphc4v_init_set (&aplRes, &aplRht);
    } else
    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of same signs, ...
        if (mpfr_inf_p (&aplLft.parts[i])
         && mpfr_inf_p (&aplRht.parts[i])
         && mpfr_sgn (&aplLft.parts[i]) EQ mpfr_sgn (&aplRht.parts[i]))
            mpfr_init_set (&aplRes.parts[i],
                            mpfr_QuadICValue (&aplRht.parts[i], // No left arg
                                               ICNDX_InfSUBInf,
                                              &aplRht.parts[i],
                                              &aplRes.parts[i],
                                               FALSE),
                            MPFR_RNDN);
        else
        {
            // Initialize the result to 0
            mpfr_init0 (&aplRes.parts[i]);

            // Subtract the two VFPs
            mpfr_sub (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i], MPFR_RNDN);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End SubHC4V_RE


//***************************************************************************
//  $PrimFnDydBarHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic Bar:  HC4V {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydBarHC4VisHC4RvHC4R
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

    PrimFnDydBarHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
    Myhc4v_clear (&atLft.aplHC4V);
} // End PrimFnDydBarHC4VisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydBarHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic Bar:  HC4V {is} HC4V fn HC4V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC4VisHC4VvHC4V"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarHC4VisHC4VvHC4V
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
        lpMemRes[uRes] = SubHC4V_RE (lpatLft->aplHC4V, lpatRht->aplHC4V);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydBarHC4VisHC4VvHC4V
#undef  APPEND_NAME


//***************************************************************************
//  $SubHC8I_RE
//***************************************************************************

APLHC8I SubHC8I_RE
    (APLHC8I aplLft,                // Left arg
     APLHC8I aplRht)                // Right ...

{
    int     i;
    APLHC8I aplRes;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Subtract the two integers
        aplRes.parts[i] = SubHC1I_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End SubHC8I_RE


//***************************************************************************
//  $PrimFnDydBarHC8IisHC8IvHC8I
//
//  Primitive scalar function dyadic Bar:  HC8I {is} HC8I fn HC8I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC8IisHC8IvHC8I"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarHC8IisHC8IvHC8I
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
        lpMemRes[uRes] = SubHC8I_RE (lpatLft->aplHC8I, lpatRht->aplHC8I);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8F);
} // End PrimFnDydBarHC8IisHC8IvHC8I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydBarHC8FisHC8IvHC8I
//
//  Primitive scalar function dyadic Bar:  HC8F {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydBarHC8FisHC8IvHC8I
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
        // Convert the integers to FLOAT and Subtract
        lpMemRes[uRes].parts[i] = (APLFLOAT) lpatLft->aplHC8I.parts[i] - (APLFLOAT) lpatRht->aplHC8I.parts[i];
} // End PrimFnDydBarHC8FisHC8IvHC8I


//***************************************************************************
//  $SubHC8F_RE
//***************************************************************************

APLHC8F SubHC8F_RE
    (APLHC8F aplLft,                // Left arg
     APLHC8F aplRht)                // Right ...

{
    int     i;
    APLHC8F aplRes;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC8F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC8F, &aplRht, 0);

    // No exceptions in this code

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            aplRes = aplLft;
        else
            aplRes = aplRht;
    } else
    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // Subtract the two integers
        aplRes.parts[i] = SubHC1F_RE (aplLft.parts[i], aplRht.parts[i]);

    return aplRes;
} // End SubHC8F_RE


//***************************************************************************
//  $PrimFnDydBarHC8FisHC8FvHC8F
//
//  Primitive scalar function dyadic Bar:  HC8F {is} HC8F fn HC8F
//***************************************************************************

void PrimFnDydBarHC8FisHC8FvHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Subtract the floats
    lpMemRes[uRes] = SubHC8F_RE (lpatLft->aplHC8F, lpatRht->aplHC8F);
} // End PrimFnDydBarHC8FisHC8FvHC8F


//***************************************************************************
//  $SubHC8R_RE
//***************************************************************************

APLHC8R SubHC8R_RE
    (APLHC8R aplLft,                    // Left arg
     APLHC8R aplRht)                    // Right ...

{
    int     i;
    APLHC8R aplRes;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC8R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC8R, &aplRht, 0);

    // If either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc8r_init_set (&aplRes, &aplLft);
        else
            mphc8r_init_set (&aplRes, &aplRht);
    } else
    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of same signs, ...
        if (mpq_inf_p (&aplLft.parts[i])
         && mpq_inf_p (&aplRht.parts[i])
         && mpq_sgn (&aplLft.parts[i]) EQ mpq_sgn (&aplRht.parts[i]))
            mpq_init_set (&aplRes.parts[i],
                           mpq_QuadICValue (&aplRht.parts[i], // No left arg
                                             ICNDX_InfSUBInf,
                                            &aplRht.parts[i],
                                            &aplRes.parts[i],
                                             FALSE));
        else
        {
            // Initialize the result to 0/1
            mpq_init (&aplRes.parts[i]);

            // Subtract the two Rationals
            mpq_sub (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i]);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End SubHC8R_RE


//***************************************************************************
//  $PrimFnDydBarHC8RisHC8RvHC8R
//
//  Primitive scalar function dyadic Bar:  HC8R {is} HC8R fn HC8R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonBarHC8RisHC8RvHC8R"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarHC8RisHC8RvHC8R
    (LPAPLHC8R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;
    int i;

    __try
    {
        // Loop through all of the parts
        for (i = 0; i < 8; i++)
            // Call subfunction
            lpMemRes[uRes] = SubHC8R_RE (lpatLft->aplHC8R, lpatRht->aplHC8R);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8V);
} // End PrimFnDydBarHC8RisHC8RvHC8R
#undef  APPEND_NAME


//***************************************************************************
//  $SubHC8V_RE
//***************************************************************************

APLHC8V SubHC8V_RE
    (APLHC8V aplLft,                    // Left arg
     APLHC8V aplRht)                    // Right ...

{
    int     i;
    APLHC8V aplRes;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC8V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC8V, &aplRht, 0);

    // If either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc8v_init_set (&aplRes, &aplLft);
        else
            mphc8v_init_set (&aplRes, &aplRht);
    } else
    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    {
        // Check for indeterminates:  _ + -_  or  -_ + _

        // If the args are both infinite and of same signs, ...
        if (mpfr_inf_p (&aplLft.parts[i])
         && mpfr_inf_p (&aplRht.parts[i])
         && mpfr_sgn (&aplLft.parts[i]) EQ mpfr_sgn (&aplRht.parts[i]))
            mpfr_init_set (&aplRes.parts[i],
                            mpfr_QuadICValue (&aplRht.parts[i], // No left arg
                                               ICNDX_InfSUBInf,
                                              &aplRht.parts[i],
                                              &aplRes.parts[i],
                                               FALSE),
                            MPFR_RNDN);
        else
        {
            // Initialize the result to 0
            mpfr_init0 (&aplRes.parts[i]);

            // Subtract the two VFPs
            mpfr_sub (&aplRes.parts[i], &aplLft.parts[i], &aplRht.parts[i], MPFR_RNDN);
        } // End IF/ELSE
    } // End FOR

    return aplRes;
} // End SubHC8V_RE


//***************************************************************************
//  $PrimFnDydBarHC8VisHC8RvHC8R
//
//  Primitive scalar function dyadic Bar:  HC8V {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydBarHC8VisHC8RvHC8R
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

    PrimFnDydBarHC8VisHC8VvHC8V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc8v_clear (&atRht.aplHC8V);
    Myhc8v_clear (&atLft.aplHC8V);
} // End PrimFnDydBarHC8VisHC8RvHC8R


//***************************************************************************
//  $PrimFnDydBarHC8VisHC8VvHC8V
//
//  Primitive scalar function dyadic Bar:  HC8V {is} HC8V fn HC8V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydHC8VisHC8VvHC8V"
#else
#define APPEND_NAME
#endif

void PrimFnDydBarHC8VisHC8VvHC8V
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
        lpMemRes[uRes] = SubHC8V_RE (lpatLft->aplHC8V, lpatRht->aplHC8V);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydBarHC8VisHC8VvHC8V
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydBarAPA_EM
//
//  Dyadic Bar, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydBarAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydBarAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to the result (may be NULL)

     LPTOKEN      lptkFunc,         // Ptr to function token

     HGLOBAL      hGlbLft,          // HGLOBAL of left arg (may be NULL if simple)
     HGLOBAL      hGlbRht,          // ...        right ...
     HGLOBAL     *lphGlbRes,        // Ptr to HGLOBAL of the result

     APLRANK      aplRankLft,       // Rank of the left arg
     APLRANK      aplRankRht,       // ...         right ...

     APLNELM      aplNELMLft,       // Only one of these NELMs is 1
     APLNELM      aplNELMRht,       // ...

     APLINT       aplInteger,       // The integer from the simple side
     LPPRIMSPEC   lpPrimSpec)       // Ptr to PRIMSPEC

{
    APLRANK           aplRankRes;           // Result rank
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPAPLAPA          lpMemRes;             // Ptr to result global memory
    UBOOL             bRet = FALSE;         // TRUE iff the result is valid


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
    lpMemHdrRes = MyGlobalLockVar (*lphGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

#define lpAPA       lpMemRes

    if (!IsSingleton (aplNELMLft))
        lpAPA->Off -= aplInteger;
    else
    {
        lpAPA->Off = aplInteger - lpAPA->Off;
        lpAPA->Mul = -lpAPA->Mul;
    } // End IF

#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

    // Fill in the result token
    if (lpYYRes NE NULL)
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
} // End PrimFnDydBarAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_bar.c
//***************************************************************************
