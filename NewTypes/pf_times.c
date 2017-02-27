//***************************************************************************
//  NARS2000 -- Primitive Function -- Times
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
#include <math.h>


#ifndef PROTO
PRIMSPEC PrimSpecTimes =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecTimesStorageTypeMon,
    NULL,   // &PrimFnMonTimesAPA_EM, -- Can't happen w/Times

    // Monadic Boolean result functions
    &PrimFnMonTimesBisB,
    NULL,   // &PrimFnMonTimesBisI, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesBisF, -- Can't happen w/Times

    // Monadic non-Boolean result functions (indexable)
    &PrimFnMonTimesIisI,
    &PrimFnMonTimesIisF,
    NULL,   // &PrimFnMonTimesFisI, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesFisF, -- Can't happen w/Times
    &PrimFnMonTimesRisR,
    &PrimFnMonTimesVisR,
    &PrimFnMonTimesVisV,

    // Monadic Hypercomplex functions
    &PrimFnMonTimesHC2IisHC2I,
    &PrimFnMonTimesHC2IisHC2F,
    &PrimFnMonTimesHC2FisHC2I,
    &PrimFnMonTimesHC2FisHC2F,
    &PrimFnMonTimesHC2RisHC2R,
    &PrimFnMonTimesHC2VisHC2R,
    &PrimFnMonTimesHC2VisHC2V,

    &PrimFnMonTimesHC4IisHC4I,
    &PrimFnMonTimesHC4IisHC4F,
    &PrimFnMonTimesHC4FisHC4I,
    &PrimFnMonTimesHC4FisHC4F,
    &PrimFnMonTimesHC4RisHC4R,
    &PrimFnMonTimesHC4VisHC4R,
    &PrimFnMonTimesHC4VisHC4V,

    &PrimFnMonTimesHC8IisHC8I,
    &PrimFnMonTimesHC8IisHC8F,
    &PrimFnMonTimesHC8FisHC8I,
    &PrimFnMonTimesHC8FisHC8F,
    &PrimFnMonTimesHC8RisHC8R,
    &PrimFnMonTimesHC8VisHC8R,
    &PrimFnMonTimesHC8VisHC8V,

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonTimesFisHC2I, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesFisHC2F, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesVisHC2R, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesVisHC2V, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesFisHC4I, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesFisHC4F, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesVisHC4R, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesVisHC4V, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesFisHC8I, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesFisHC8F, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesVisHC8R, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesVisHC8V, -- Can't happen w/Times

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecTimesStorageTypeDyd,
    &PrimFnDydTimesAPA_EM,

    // Dyadic Boolean result functions
    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydTimesBisIvI, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisFvF, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisCvC, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisHvH, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisRvR, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisVvV, -- Can't happen w/Times

    NULL,   // &PrimFnDydTimesBisHC2IvHC2I, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisHC2FvHC2F, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisHC2RvHC2R, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisHC2VvHC2V, -- Can't happen w/Times

    NULL,   // &PrimFnDydTimesBisHC4IvHC4I, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisHC4FvHC4F, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisHC4RvHC4R, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisHC4VvHC4V, -- Can't happen w/Times

    NULL,   // &PrimFnDydTimesBisHC8IvHC8I, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisHC8FvHC8F, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisHC8RvHC8R, -- Can't happen w/Times
    NULL,   // &PrimFnDydTimesBisHC8VvHC8V, -- Can't happen w/Times

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydTimesIisIvI,
    NULL,   // &PrimFnDydTimesIisFvF, -- Can't happen w/Times
    &PrimFnDydTimesFisIvI,
    &PrimFnDydTimesFisFvF,
    &PrimFnDydTimesRisRvR,
    &PrimFnDydTimesVisRvR,
    &PrimFnDydTimesVisVvV,

    &PrimFnDydTimesHC2IisHC2IvHC2I,
    NULL,   // &PrimFnDydTimesHC2IisHC2FvHC2F, -- Can't happen w/Times
    &PrimFnDydTimesHC2FisHC2IvHC2I,
    &PrimFnDydTimesHC2FisHC2FvHC2F,
    &PrimFnDydTimesHC2RisHC2RvHC2R,
    &PrimFnDydTimesHC2VisHC2RvHC2R,
    &PrimFnDydTimesHC2VisHC2VvHC2V,

    &PrimFnDydTimesHC4IisHC4IvHC4I,
    NULL,   // &PrimFnDydTimesHC4IisHC4FvHC4F, -- Can't happen w/Times
    &PrimFnDydTimesHC4FisHC4IvHC4I,
    &PrimFnDydTimesHC4FisHC4FvHC4F,
    &PrimFnDydTimesHC4RisHC4RvHC4R,
    &PrimFnDydTimesHC4VisHC4RvHC4R,
    &PrimFnDydTimesHC4VisHC4VvHC4V,

    &PrimFnDydTimesHC8IisHC8IvHC8I,
    NULL,   // &PrimFnDydTimesHC8IisHC8FvHC8F, -- Can't happen w/Times
    &PrimFnDydTimesHC8FisHC8IvHC8I,
    &PrimFnDydTimesHC8FisHC8FvHC8F,
    &PrimFnDydTimesHC8RisHC8RvHC8R,
    &PrimFnDydTimesHC8VisHC8RvHC8R,
    &PrimFnDydTimesHC8VisHC8VvHC8V,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonTimesB64isB64, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesB32isB32, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesB16isB16, -- Can't happen w/Times
    NULL,   // &PrimFnMonTimesB08isB08, -- Can't happen w/Times

    // Dyadic Boolean chunk functions
    &PrimFnDydUpCaretB64isB64vB64,
    &PrimFnDydUpCaretB32isB32vB32,
    &PrimFnDydUpCaretB16isB16vB16,
    &PrimFnDydUpCaretB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecTimes};
#endif


//***************************************************************************
//  $PrimFnTimes_EM_YY
//
//  Primitive function for monadic and dyadic Times ("signum" and "multiplication")
//***************************************************************************

LPPL_YYSTYPE PrimFnTimes_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    PRIMSPEC primSpec = *lpPrimSpec;

    // Set the Hypercomplex Arithmetic Multiplication choice
    primSpec.eHCMul = GetMemPTD ()->eHCMul;

    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_TIMES);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, &primSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, &primSpec);
} // End PrimFnTimes_EM_YY


//***************************************************************************
//  $PrimSpecTimesStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecTimesStorageTypeMon
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

    // Except that FLOAT and APA become INT
    if (IsSimpleFlt (aplTypeRes)
     || IsSimpleAPA (aplTypeRes))
        return ARRAY_INT;

    return aplTypeRes;
} // End PrimSpecTimesStorageTypeMon


//***************************************************************************
//  $PrimFnMonTimesBisB
//
//  Primitive scalar function monadic Times:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonTimesBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonTimesBisB


//***************************************************************************
//  $PrimFnMonTimesIisI
//
//  Primitive scalar function monadic Times:  I {is} fn I
//***************************************************************************

void PrimFnMonTimesIisI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = signumint (lpatRht->aplInteger);
} // End PrimFnMonTimesIisI


//***************************************************************************
//  $PrimFnMonTimesIisF
//
//  Primitive scalar function monadic Times:  I {is} fn F
//***************************************************************************

void PrimFnMonTimesIisF
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Note that signum of -0 is 0
    lpMemRes[uRes] = (lpatRht->aplFloat < 0) ? -1 : (lpatRht->aplFloat > 0);
} // End PrimFnMonTimesIisF


//***************************************************************************
//  $PrimFnMonTimesRisR
//
//  Primitive scalar function monadic Times:  R {is} fn R
//***************************************************************************

void PrimFnMonTimesRisR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    mpq_init_set_si (&lpMemRes[uRes], signumrat (&lpatRht->aplRat), 1);
} // End PrimFnMonTimesRisR


//***************************************************************************
//  $PrimFnMonTimesVisR
//
//  Primitive scalar function monadic Times:  V {is} fn R
//***************************************************************************

void PrimFnMonTimesVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    mpfr_init_set_si (&lpMemRes[uRes], signumrat (&lpatRht->aplRat), MPFR_RNDN);
} // End PrimFnMonTimesVisR


//***************************************************************************
//  $PrimFnMonTimesVisV
//
//  Primitive scalar function monadic Times:  V {is} fn V
//***************************************************************************

void PrimFnMonTimesVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Note that signum of -0 is 0
    mpfr_init_set_si (&lpMemRes[uRes], mpfr_sgn (&lpatRht->aplVfp), MPFR_RNDN);
} // End PrimFnMonTimesVisV


//***************************************************************************
//  $DirHC2I_RE
//***************************************************************************

APLHC2I DirHC2I_RE
    (APLHC2I aplRht)                    // Right arg

{
    APLHC2I aplRes;

    // If the arg is 0, ...
    if (IsZeroHCxI (&aplRht, 2))
        // Zap the memory
        ZeroMemory (&aplRes, sizeof (aplRes));
    else
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

    return aplRes;
} // End DirHC2I_RE


//***************************************************************************
//  $PrimFnMonTimesHC2IisHC2I
//
//  Primitive scalar function monadic Times:  HC2I {is} fn HC2I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonTimesHC2IisHC2I"
#else
#define APPEND_NAME
#endif

void PrimFnMonTimesHC2IisHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DirHC2I_RE (lpatRht->aplHC2I);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2F);
} // End PrimFnMonTimesHC2IisHC2I
#undef APPEND_NAME


//***************************************************************************
//  $PrimFnMonTimesHC2IisHC2F
//
//  Primitive scalar function monadic Times:  HC2I {is} fn HC2F
//***************************************************************************

void PrimFnMonTimesHC2IisHC2F
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
} // End PrimFnMonTimesHC2IisHC2F


//***************************************************************************
//  $PrimFnMonTimesHC2FisHC2I
//
//  Primitive scalar function monadic Times:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonTimesHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRes = {0};

    // No exceptions in this code

    // Convert from HC2I to HC2F
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatRht, 0, &atRes);

    // Call subfunction
    lpMemRes[uRes] = DirHC2F_RE (atRes.aplHC2F);
} // End PrimFnMonTimesHC2FisHC2I


//***************************************************************************
//  $DirHC2F_RE
//***************************************************************************

APLHC2F DirHC2F_RE
    (APLHC2F aplRht)                    // Right arg

{
    APLHC2F  aplRes;
    APLFLOAT aplMag;

    // No exceptions in this code

    // Get the magnitude of the arg
    aplMag = MagHC2F (aplRht);

    // If the arg is 0, ...
    if (aplMag EQ 0.0)
        // Zap the memory
        ZeroMemory (&aplRes, sizeof (aplRes));
    else
        // Divide by the magnitude
        aplRes = DivHC2F_F (aplRht, aplMag);

    return aplRes;
} // End DirHC2F_RE


//***************************************************************************
//  $PrimFnMonTimesHC2FisHC2F
//
//  Primitive scalar function monadic Times:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonTimesHC2FisHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = DirHC2F_RE (lpatRht->aplHC2F);
} // End PrimFnMonTimesHC2FisHC2F


//***************************************************************************
//  $DirHC2R_RE
//***************************************************************************

APLHC2R DirHC2R_RE
    (APLHC2R aplRht)                    // Right arg

{
    APLHC2R aplRes = {0};

    // If the arg is 0, ...
    if (IsZeroHCxR (&aplRht, 2))
        // Initialize to 0/1
        mphc2r_init (&aplRes);
    else
        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    return aplRes;
} // End DirHC2R_RE


//***************************************************************************
//  $PrimFnMonTimesHC2RisHC2R
//
//  Primitive scalar function monadic Times:  HC2R {is} fn HC2R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonTimesHC2RisHC2R"
#else
#define APPEND_NAME
#endif

void PrimFnMonTimesHC2RisHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DirHC2R_RE (lpatRht->aplHC2R);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2V);
} // End PrimFnMonTimesHC2RisHC2R
#undef  APPEND_NAME


//***************************************************************************
//  $DirHC2V_RE
//***************************************************************************

APLHC2V DirHC2V_RE
    (APLHC2V aplRht)                    // Right arg

{
    APLHC2V aplRes = {0};
    APLVFP  aplMag;

    // Get the magnitude of the arg
    aplMag = MagHC2V (aplRht);

    // No exceptions in this code

    // If the arg is 0, ...
    if (IsMpf0 (&aplMag))
        // Initialize to 0
        mphc2v_init0 (&aplRes);
    else
    {
        // Divide by the magnitude
        aplRes = DivHC2V_V (aplRht, aplMag);

        // We no longer need this resource
        Myf_clear (&aplMag);
    } // End IF/ELSE

    return aplRes;
} // End DirHC2V_RE


//***************************************************************************
//  $PrimFnMonTimesHC2VisHC2R
//
//  Primitive scalar function monadic Times:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonTimesHC2VisHC2R
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
    lpMemRes[uRes] = DirHC2V_RE (atRht.aplHC2V);

    Myhc2v_clear (&atRht.aplHC2V);
} // End PrimFnMonTimesHC2VisHC2R


//***************************************************************************
//  $PrimFnMonTimesHC2VisHC2V
//
//  Primitive scalar function monadic Times:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonTimesHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = DirHC2V_RE (lpatRht->aplHC2V);
} // End PrimFnMonTimesHC2VisHC2V


//***************************************************************************
//  $DirHC4I_RE
//***************************************************************************

APLHC4I DirHC4I_RE
    (APLHC4I aplRht)                    // Right arg

{
    APLHC4I aplRes;

    // If the arg is 0, ...
    if (IsZeroHCxI (&aplRht, 4))
        // Zap the memory
        ZeroMemory (&aplRes, sizeof (aplRes));
    else
        RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);

    return aplRes;
} // End DirHC4I_RE


//***************************************************************************
//  $PrimFnMonTimesHC4IisHC4I
//
//  Primitive scalar function monadic Times:  HC4I {is} fn HC4I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonTimesHC4IisHC4I"
#else
#define APPEND_NAME
#endif

void PrimFnMonTimesHC4IisHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DirHC4I_RE (lpatRht->aplHC4I);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4F);
} // End PrimFnMonTimesHC4IisHC4I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonTimesHC4IisHC4F
//
//  Primitive scalar function monadic Times:  HC4I {is} fn HC4F
//***************************************************************************

void PrimFnMonTimesHC4IisHC4F
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);
} // End PrimFnMonTimesHC4IisHC4F


//***************************************************************************
//  $PrimFnMonTimesHC4FisHC4I
//
//  Primitive scalar function monadic Times:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonTimesHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRes = {0};

    // No exceptions in this code

    // Convert from HC4I to HC4F
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatRht, 0, &atRes);

    // Call subfunction
    lpMemRes[uRes] = DirHC4F_RE (atRes.aplHC4F);
} // End PrimFnMonTimesHC4FisHC4I


//***************************************************************************
//  $DirHC4F_RE
//***************************************************************************

APLHC4F DirHC4F_RE
    (APLHC4F aplRht)                    // Right arg

{
    APLHC4F  aplRes;
    APLFLOAT aplMag;

    // Get the magnitude of the arg
    aplMag = MagHC4F (aplRht);

    // No exceptions in this code

    // If the arg is 0, ...
    if (aplMag EQ 0.0)
        // Zap the memory
        ZeroMemory (&aplRes, sizeof (aplRes));
    else
        // Divide by the magnitude
        aplRes = DivHC4F_F (aplRht, aplMag);

    return aplRes;
} // End DirHC4F_RE


//***************************************************************************
//  $PrimFnMonTimesHC4FisHC4F
//
//  Primitive scalar function monadic Times:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonTimesHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = DirHC4F_RE (lpatRht->aplHC4F);
} // End PrimFnMonTimesHC4FisHC4F


//***************************************************************************
//  $DirHC4R_RE
//***************************************************************************

APLHC4R DirHC4R_RE
    (APLHC4R aplRht)                    // Right arg

{
    APLHC4R aplRes = {0};

    // If the arg is 0, ...
    if (IsZeroHCxR (&aplRht, 4))
        // Initialize to 0/1
        mphc4r_init (&aplRes);
    else
        RaiseException (EXCEPTION_RESULT_HC4V, 0, 0, NULL);

    return aplRes;
} // End DirHC4R_RE


//***************************************************************************
//  $PrimFnMonTimesHC4RisHC4R
//
//  Primitive scalar function monadic Times:  HC4R {is} fn HC4R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonTimesHC4RisHC4R"
#else
#define APPEND_NAME
#endif

void PrimFnMonTimesHC4RisHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DirHC4R_RE (lpatRht->aplHC4R);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4V);
} // End PrimFnMonTimesHC4RisHC4R
#undef  APPEND_NAME


//***************************************************************************
//  $DirHC4V_RE
//***************************************************************************

APLHC4V DirHC4V_RE
    (APLHC4V aplRht)                    // Right arg

{
    APLHC4V aplRes;
    APLVFP  aplMag;

    // No exceptions in this code

    // Get the magnitude of the arg
    aplMag = MagHC4V (aplRht);

    // If the arg is 0, ...
    if (IsMpf0 (&aplMag))
        // Initialize to 0
        mphc4v_init0 (&aplRes);
    else
    {
        // Divide by the magnitude
        aplRes = DivHC4V_V (aplRht, aplMag);

        Myf_clear (&aplMag);
    } // End IF/ELSE

    return aplRes;
} // End DirHC4V_RE


//***************************************************************************
//  $PrimFnMonTimesHC4VisHC4R
//
//  Primitive scalar function monadic Times:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonTimesHC4VisHC4R
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
    lpMemRes[uRes] = DirHC4V_RE (atRht.aplHC4V);

    Myhc4v_clear (&atRht.aplHC4V);
} // End PrimFnMonTimesHC4VisHC4R


//***************************************************************************
//  $PrimFnMonTimesHC4VisHC4V
//
//  Primitive scalar function monadic Times:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonTimesHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = DirHC4V_RE (lpatRht->aplHC4V);
} // End PrimFnMonTimesHC4VisHC4V


//***************************************************************************
//  $DirHC8I_RE
//***************************************************************************

APLHC8I DirHC8I_RE
    (APLHC8I aplRht)                    // Right arg

{
    APLHC8I aplRes;

    // If the arg is 0, ...
    if (IsZeroHCxI (&aplRht, 8))
        // Zap the memory
        ZeroMemory (&aplRes, sizeof (aplRes));
    else
        RaiseException (EXCEPTION_RESULT_HC8F, 0, 0, NULL);

    return aplRes;
} // End TimesHC8I


//***************************************************************************
//  $PrimFnMonTimesHC8IisHC8I
//
//  Primitive scalar function monadic Times:  HC8I {is} fn HC8I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonTimesHC8IisHC8I"
#else
#define APPEND_NAME
#endif

void PrimFnMonTimesHC8IisHC8I
    (LPAPLHC8I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DirHC8I_RE (lpatRht->aplHC8I);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8F);
} // End PrimFnMonTimesHC8IisHC8I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonTimesHC8IisHC8F
//
//  Primitive scalar function monadic Times:  HC8I {is} fn HC8F
//***************************************************************************

void PrimFnMonTimesHC8IisHC8F
    (LPAPLHC8I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    RaiseException (EXCEPTION_RESULT_HC8F, 0, 0, NULL);
} // End PrimFnMonTimesHC8IisHC8F


//***************************************************************************
//  $DirHC8F_RE
//***************************************************************************

APLHC8F DirHC8F_RE
    (APLHC8F aplRht)                    // Right arg

{
    APLHC8F  aplRes;
    APLFLOAT aplMag;

    // Get the magnitude
    aplMag = MagHC8F (aplRht);

    // No exceptions in this code

    // If the arg is 0, ...
    if (aplMag EQ 0.0)
        // Zap the memory
        ZeroMemory (&aplRes, sizeof (aplRes));
    else
        // Divide by the magnitude
        aplRes = DivHC8F_F (aplRht, aplMag);

    return aplRes;
} // End DirHC8F_RE


//***************************************************************************
//  $PrimFnMonTimesHC8FisHC8F
//
//  Primitive scalar function monadic Times:  HC8F {is} fn HC8F
//***************************************************************************

void PrimFnMonTimesHC8FisHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = DirHC8F_RE (lpatRht->aplHC8F);
} // End PrimFnMonTimesHC8FisHC8F


//***************************************************************************
//  $PrimFnMonTimesHC8FisHC8I
//
//  Primitive scalar function monadic Times:  HC8F {is} fn HC8I
//***************************************************************************

void PrimFnMonTimesHC8FisHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRes = {0};

    // No exceptions in this code

    // Convert from HC8I to HC8F
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatRht, 0, &atRes);

    // Call subfunction
    lpMemRes[uRes] = DirHC8F_RE (atRes.aplHC8F);
} // End PrimFnMonTimesHC8FisHC8I


//***************************************************************************
//  $DirHC8R_RE
//***************************************************************************

APLHC8R DirHC8R_RE
    (APLHC8R aplRht)                    // Right arg

{
    APLHC8R aplRes = {0};

    // If the arg is 0, ...
    if (IsZeroHCxR (&aplRht, 8))
        // Initialize to 0/1
        mphc8r_init (&aplRes);
    else
        RaiseException (EXCEPTION_RESULT_HC8V, 0, 0, NULL);
    return aplRes;
} // End DirHC8R_RE


//***************************************************************************
//  $PrimFnMonTimesHC8RisHC8R
//
//  Primitive scalar function monadic Times:  HC8R {is} fn HC8R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonTimesHC8RisHC8R"
#else
#define APPEND_NAME
#endif

void PrimFnMonTimesHC8RisHC8R
    (LPAPLHC8R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = DirHC8R_RE (lpatRht->aplHC8R);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8V);
} // End PrimFnMonTimesHC8RisHC8R
#undef  APPEND_NAME


//***************************************************************************
//  $DirHC8V_RE
//***************************************************************************

APLHC8V DirHC8V_RE
    (APLHC8V aplRht)                    // Right arg

{
    APLHC8V aplRes = {0};
    APLVFP  aplMag;

    // No exceptions in this code

    // Get the magnitude of the arg
    aplMag = MagHC8V (aplRht);

    // If the arg is 0, ...
    if (IsMpf0 (&aplMag))
        // Initialize to 0
        mphc8v_init0 (&aplRes);
    else
    {
        // Divide by the magnitude
        aplRes = DivHC8V_V (aplRht, aplMag);

        Myf_clear (&aplMag);
    } // End IF/ELSE

    return aplRes;
} // End DirHC8V_RE


//***************************************************************************
//  $PrimFnMonTimesHC8VisHC8R
//
//  Primitive scalar function monadic Times:  HC8V {is} fn HC8R
//***************************************************************************

void PrimFnMonTimesHC8VisHC8R
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
    lpMemRes[uRes] = DirHC8V_RE (atRht.aplHC8V);

    Myhc8v_clear (&atRht.aplHC8V);
} // End PrimFnMonTimesHC8VisHC8R


//***************************************************************************
//  $PrimFnMonTimesHC8VisHC8V
//
//  Primitive scalar function monadic Times:  HC8V {is} fn HC8V
//***************************************************************************

void PrimFnMonTimesHC8VisHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction
    lpMemRes[uRes] = DirHC8V_RE (lpatRht->aplHC8V);
} // End PrimFnMonTimesHC8VisHC8V


//***************************************************************************
//  $PrimSpecTimesStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecTimesStorageTypeDyd
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

    // Special case multiplication with APA
    if (aplTypeRes EQ ARRAY_INT                            // Res = INT
     && (!IsSingleton (aplNELMLft) || !IsSingleton (aplNELMRht))    // Not both singletons
     && ((IsSingleton (aplNELMLft) && IsSimpleAPA (*lpaplTypeRht))   // Non-singleton is APA
      || (IsSingleton (aplNELMRht) && IsSimpleAPA (*lpaplTypeLft)))) // ...
        aplTypeRes = ARRAY_APA;

    return aplTypeRes;
} // End PrimSpecTimesStorageTypeDyd


//***************************************************************************
//  $MulHC1I_RE
//***************************************************************************

APLINT MulHC1I_RE
    (APLINT aplLft,                     // Left arg
     APLINT aplRht)                     // Right ...

{
    // Check for -0
    // If either arg is 0
    //   and the signs are different
    if (gAllowNeg0
     && (aplLft EQ 0
      || aplRht EQ 0)
     && (aplLft < 0) NE
        (aplRht < 0))
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return imul64_RE (aplLft, aplRht, FLOAT);
} // End MulHC1I_RE


//***************************************************************************
//  $PrimFnDydTimesIisIvI
//
//  Primitive scalar function dyadic Times:  I {is} I fn I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesIisIvI"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesIisIvI
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
        lpMemRes[uRes] = MulHC1I_RE (lpatLft->aplInteger, lpatRht->aplInteger);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_FLOAT);
} // End PrimFnDydTimesIisIvI
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydTimesFisIvI
//
//  Primitive scalar function dyadic Times:  F {is} I fn I
//***************************************************************************

void PrimFnDydTimesFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Check for -0
    // If either arg is 0
    //   and the signs are different
    if (gAllowNeg0
     && (lpatLft->aplInteger EQ 0
      || lpatRht->aplInteger EQ 0)
     && (lpatLft->aplInteger < 0) NE
        (lpatRht->aplInteger < 0))
        lpMemRes[uRes] = -0.0;
    else
    {
        UBOOL  bRet;
        APLINT aplRes;

        aplRes = imul64 (lpatLft->aplInteger, lpatRht->aplInteger, &bRet, EXCEPTION_RESULT_FLOAT);
        if (bRet)
            lpMemRes[uRes] = (APLFLOAT) aplRes;
        else
            lpMemRes[uRes] = (APLFLOAT) lpatLft->aplInteger * (APLFLOAT) lpatRht->aplInteger;
    } // End IF/ELSE
} // End PrimFnDydTimesFisIvI


//***************************************************************************
//  $MulHC1F_RE
//***************************************************************************

APLFLOAT MulHC1F_RE
    (APLFLOAT aplLft,                   // Left arg
     APLFLOAT aplRht)                   // Right arg

{
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1F, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            return aplLft;
        else
            return aplRht;
    } else
    // Check for indeterminates:  0 {times} _  or  _ {times} 0
    if ((aplLft EQ 0.0
      && IsFltPosInfinity (aplRht))
     || (IsFltPosInfinity (aplLft)
      && aplRht EQ 0.0))
        return TranslateQuadICIndex (aplLft,
                                     ICNDX_0MULPi,
                                     aplRht,
                                     (aplLft EQ 0) ? SIGN_APLFLOAT (aplLft)
                                                   : SIGN_APLFLOAT (aplRht));
    else
    // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
    if ((aplLft EQ 0.0
      && IsFltNegInfinity (aplRht))
     || (IsFltNegInfinity (aplLft)
      && aplRht EQ 0.0))
        return TranslateQuadICIndex (aplLft,
                                     ICNDX_0MULNi,
                                     aplRht,
                                     (aplLft EQ 0) ? SIGN_APLFLOAT (aplLft)
                                                   : SIGN_APLFLOAT (aplRht));
    else
    // Check for -0
    // If either arg is 0
    //   and the signs are different
    if (gAllowNeg0
     && (aplLft EQ 0
      || aplRht EQ 0)
     && (SIGN_APLFLOAT (aplLft) NE
         SIGN_APLFLOAT (aplRht)))
        return -0.0;
    else
        return (aplLft * aplRht);
} // End MulHC1F_RE


//***************************************************************************
//  $PrimFnDydTimesFisFvF
//
//  Primitive scalar function dyadic Times:  F {is} F fn F
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesFisFvF"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesFisFvF
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
        lpMemRes[uRes] = MulHC1F_RE (lpatLft->aplFloat, lpatRht->aplFloat);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesFisFvF
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC1R_RE
//***************************************************************************

APLRAT MulHC1R_RE
    (APLRAT aplLft,                     // Left arg
     APLRAT aplRht)                     // Right ...

{
    APLRAT aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1R, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc1r_init_set (&aplRes, &aplLft);
        else
            mphc1r_init_set (&aplRes, &aplRht);
    } else
    // Check for indeterminates:  0 {times} _  or  _ {times} 0
    if ((IsMpq0 (&aplLft)
      && IsMpqPosInfinity (&aplRht))
     || (IsMpqPosInfinity (&aplLft)
      && IsMpq0 (&aplRht)))
        aplRes = *mpq_QuadICValue (&aplLft,
                                    ICNDX_0MULPi,
                                   &aplRht,
                                   &aplRes,
                                    IsMpq0 (&aplLft) ? (mpq_sgn (&aplLft) EQ -1)
                                                              : (mpq_sgn (&aplRht) EQ -1));
    else
    // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
    if ((IsMpq0 (&aplLft)
      && IsMpqNegInfinity (&aplRht))
     || (IsMpqNegInfinity (&aplLft)
      && IsMpq0 (&aplRht)))
        aplRes = *mpq_QuadICValue (&aplLft,
                                    ICNDX_0MULNi,
                                   &aplRht,
                                   &aplRes,
                                    IsMpq0 (&aplLft) ? (mpq_sgn (&aplLft) EQ -1)
                                                     : (mpq_sgn (&aplRht) EQ -1));
    else
    // Check for -0
    // If either arg is 0
    //   and the signs are different
    if (gAllowNeg0
     && (IsMpq0 (&aplLft)
      || IsMpq0 (&aplRht))
     && (SIGN_APLRAT (&aplLft) NE
         SIGN_APLRAT (&aplRht)))
        RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);
    else
    {
        // Initalize the result to 0/1
        mpq_init (&aplRes);

        // Multiply two Rationals
        mpq_mul (&aplRes, &aplLft, &aplRht);
    } // End IF/ELSE/...

    return aplRes;
} // End MulHC1R_RE


//***************************************************************************
//  $PrimFnDydTimesRisRvR
//
//  Primitive scalar function dyadic Times:  R {is} R fn R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesRisRvR"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesRisRvR
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
        lpMemRes[uRes] = MulHC1R_RE (lpatLft->aplRat, lpatRht->aplRat);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_VFP);
} // End PrimFnDydTimesRisRvR
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC1V_RE
//***************************************************************************

APLVFP MulHC1V_RE
    (APLVFP aplLft,                     // Left arg
     APLVFP aplRht)                     // Right ...

{
    APLVFP aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1V, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc1v_init_set (&aplRes, &aplLft);
        else
            mphc1v_init_set (&aplRes, &aplRht);
    } else
    // Check for indeterminates:  0 {times} _  or  _ {times} 0
    if ((IsMpf0 (&aplLft)
      && IsMpfPosInfinity (&aplRht))
     || (IsMpfPosInfinity (&aplLft)
      && IsMpf0 (&aplRht)))
        aplRes = *mpfr_QuadICValue (&aplLft,
                                     ICNDX_0MULPi,
                                    &aplRht,
                                    &aplRes,
                                     IsMpf0 (&aplLft) ? SIGN_APLVFP (&aplLft)
                                                      : SIGN_APLVFP (&aplRht));
    else
    // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
    if ((IsMpf0 (&aplLft)
      && IsMpfNegInfinity (&aplRht))
     || (IsMpfNegInfinity (&aplLft)
      && IsMpf0 (&aplRht)))
        aplRes = *mpfr_QuadICValue (&aplLft,
                                     ICNDX_0MULNi,
                                    &aplRht,
                                    &aplRes,
                                     IsMpf0 (&aplLft) ? SIGN_APLVFP (&aplLft)
                                                      : SIGN_APLVFP (&aplRht));
    else
    // Check for -0
    // If either arg is 0
    //   and the signs are different
    if (gAllowNeg0
     && (IsMpf0 (&aplLft)
      || IsMpf0 (&aplRht))
     && (mpfr_sgn (&aplLft) < 0) NE
        (mpfr_sgn (&aplRht) < 0))
        // Set the result to -0
        mpfr_init_set_d (&aplRes, -0, MPFR_RNDN);
    else
    {
        // Initalize the result to 0
        mpfr_init0 (&aplRes);

        // Multiply two Variable FPs
        mpfr_mul (&aplRes, &aplLft, &aplRht, MPFR_RNDN);
    } // End IF/ELSE/...

    return aplRes;
} // End MulHC1V_RE


//***************************************************************************
//  $PrimFnDydTimesVisRvR
//
//  Primitive scalar function dyadic Times:  V {is} R fn R
//***************************************************************************

void PrimFnDydTimesVisRvR
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

    PrimFnDydTimesVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydTimesVisRvR


//***************************************************************************
//  $PrimFnDydTimesVisVvV
//
//  Primitive scalar function dyadic Times:  V {is} V fn V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesVisVvV"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesVisVvV
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
        lpMemRes[uRes] = MulHC1V_RE (lpatLft->aplVfp, lpatRht->aplVfp);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesVisVvV
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC2I_RE
//***************************************************************************

APLHC2I MulHC2I_RE
    (APLHC2I aplLft,                    // Left arg
     APLHC2I aplRht)                    // Right ...

{
    APLHC2I aplRes;

#define a   aplLft.partsLo
#define b   aplLft.partsHi
#define c   aplRht.partsLo
#define d   aplRht.partsHi

    // Use Cayley-Dickson construction
    // (a,b) x (c,d) = (ac - db, da + bc)
    aplRes.partsLo = SubHC1I_RE (MulHC1I_RE (a, c),
                                 MulHC1I_RE (d, b));
    aplRes.partsHi = AddHC1I_RE (MulHC1I_RE (d, a),
                                 MulHC1I_RE (b, c));
#undef  d
#undef  c
#undef  b
#undef  a

    return aplRes;
} // End MulHC2I_RE


//***************************************************************************
//  $PrimFnDydTimesHC2IisHC2IvHC2I
//
//  Primitive scalar function dyadic Times:  HC2I {is} HC2I fn HC2I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC2IisHC2IvHC2I"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC2IisHC2IvHC2I
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
        lpMemRes[uRes] = MulHC2I_RE (lpatLft->aplHC2I, lpatRht->aplHC2I);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2F);
} // End PrimFnDydTimesHC2IisHC2IvHC2I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydTimesHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic Times:  HC2F {is} HC2I fn HC2I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC2IisHC2IvHC2I"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC2FisHC2IvHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;
    ALLTYPES        atLft = {0},
                    atRht = {0};

    // Convert the HC2I to HC2F
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatRht, 0, &atRht);

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = MulHC2F_RE (atLft.aplHC2F, atRht.aplHC2F);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesHC2FisHC2IvHC2I
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC2F_RE
//***************************************************************************

APLHC2F MulHC2F_RE
    (APLHC2F aplLft,
     APLHC2F aplRht)

{
    APLHC2F aplRes;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC2F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC2F, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            aplRes = aplLft;
        else
            aplRes = aplRht;
    } else

#define a   aplLft.partsLo
#define b   aplLft.partsHi
#define c   aplRht.partsLo
#define d   aplRht.partsHi

    // Special case b EQ 0 && d EQ 0 so as to avoid an error if a or c is {Inf}
    if (b EQ 0.0 && d EQ 0.0)
    {
        aplRes.partsLo = MulHC1F_RE (a, c);
        aplRes.partsHi = 0.0;
    } else
    // Special case b EQ 0 so as to avoid an error if c or d is {Inf}
    if (b EQ 0.0)
    {
        aplRes.partsLo = MulHC1F_RE (a, c);
        aplRes.partsHi = MulHC1F_RE (d, a);
    } else
    // Special case d EQ 0 so as to avoid an error if a or b is {Inf}
    if (d EQ 0.0)
    {
        aplRes.partsLo = MulHC1F_RE (a, c);
        aplRes.partsHi = MulHC1F_RE (b, c);
    } else
    // Special case a EQ 0 && c EQ 0 so as to avoid an error if b or d is {Inf}
    if (a EQ 0.0 && c EQ 0.0)
    {
        aplRes.partsLo = NegHC1F_RE (MulHC1F_RE (d, b));
        aplRes.partsHi = 0.0;
    } else
    {
        // Use Cayley-Dickson construction
        // (a,b) x (c,d) = (ac - db, da + bc)
        aplRes.partsLo = SubHC1F_RE (MulHC1F_RE (a, c),
                                     MulHC1F_RE (d, b));
        aplRes.partsHi = AddHC1F_RE (MulHC1F_RE (d, a),
                                     MulHC1F_RE (b, c));
    } // End IF/ELSE
#undef  d
#undef  c
#undef  b
#undef  a

    return aplRes;
} // End MulHC2F_RE


//***************************************************************************
//  $PrimFnDydTimesHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic Times:  HC2F {is} HC2F fn HC2F
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC2FisHC2FvHC2F"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Call subfunction
        lpMemRes[uRes] = MulHC2F_RE (lpatLft->aplHC2F, lpatRht->aplHC2F);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesHC2FisHC2FvHC2F
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC2R_RE
//***************************************************************************

APLHC2R MulHC2R_RE
    (APLHC2R aplLft,                    // Left arg
     APLHC2R aplRht)                    // Right ...

{
    int     i;
    APLHC2R aplRes = {0};
    APLRAT  aplTmp1,
            aplTmp2;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC2R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC2R, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc2r_init_set (&aplRes, &aplLft);
        else
            mphc2r_init_set (&aplRes, &aplRht);
    } else
    {
        // Initialize to 0/1
        mphc2r_init (&aplRes);

        // Initialize to 0/1
        mpq_init    (&aplTmp1);
        mpq_init    (&aplTmp2);

////    aplRes.parts[0] = aplLft.parts[0] * aplRht.parts[0],
////                    - aplLft.parts[1] * aplRht.parts[1];
////    aplRes.parts[1] = aplLft.parts[0] * aplRht.parts[1]
////                    + aplLft.parts[1] * aplRht.parts[0];

#define a   aplLft.partsLo
#define b   aplLft.partsHi
#define c   aplRht.partsLo
#define d   aplRht.partsHi

        // Special case b EQ 0 && d EQ 0 so as to avoid an error if a or c is {Inf}
        if (IsMpq0 (&b) && IsMpq0 (&d))
        {
            mpq_mul (&aplRes.parts[0], &a      , &c      );
        } else
        // Special case b EQ 0 so as to avoid an error if c or d is {Inf}
        if (IsMpq0 (&b))
        {
            mpq_mul (&aplRes.parts[0], &a      , &c      );
            mpq_mul (&aplRes.parts[1], &d      , &a      );
        } else
        // Special case d EQ 0 so as to avoid an error if a or b is {Inf}
        if (IsMpq0 (&d))
        {
            mpq_mul (&aplRes.parts[0], &a      , &c      );
            mpq_mul (&aplRes.parts[1], &b      , &c      );
        } else
        // Special case a EQ 0 && c EQ 0 so as to avoid an error if b or d is {Inf}
        if (IsMpq0 (&a) && IsMpq0 (&c))
        {
            mpq_mul  (&aplRes.parts[0], &d      , &b      );
            mpq_neg  (&aplRes.parts[0], &aplRes.parts[0]);
        } else
        {
            // Use Cayley-Dickson construction
            // (a,b) x (c,d) = (ac - db, da + bc)
            mpq_mul (&aplTmp1,         &a      , &c      );
            mpq_mul (&aplTmp2,         &d      , &b      );
            mpq_sub (&aplRes.parts[0], &aplTmp1, &aplTmp2);

            mpq_mul (&aplTmp1,         &d      , &a      );
            mpq_mul (&aplTmp2,         &b      , &c      );
            mpq_add (&aplRes.parts[1], &aplTmp1, &aplTmp2);
        } // End IF/ELSE

#undef  d
#undef  c
#undef  b
#undef  a

        Myq_clear (&aplTmp2);
        Myq_clear (&aplTmp1);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
        // Check for indeterminates:  0 {times} _  or  _ {times} 0
        if ((IsMpq0 (&aplLft.parts[i])
          && IsMpqPosInfinity (&aplRht.parts[i]))
         || (IsMpqPosInfinity (&aplLft.parts[i])
          && IsMpq0 (&aplRht.parts[i])))
            mpq_set (&aplRes.parts[i], mpq_QuadICValue (&aplLft.parts[i],
                                                         ICNDX_0MULPi,
                                                       &aplRht.parts[i],
                                                       &aplRes.parts[i],
                                                        IsMpq0 (&aplLft.parts[i]) ? (mpq_sgn (&aplLft.parts[i]) EQ -1)
                                                                                  : (mpq_sgn (&aplRht.parts[i]) EQ -1)));
        else
        // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
        if ((IsMpq0 (&aplLft.parts[i])
          && IsMpqNegInfinity (&aplRht.parts[i]))
         || (IsMpqNegInfinity (&aplLft.parts[i])
          && IsMpq0 (&aplRht.parts[i])))
            mpq_set (&aplRes.parts[i], mpq_QuadICValue (&aplLft.parts[i],
                                                         ICNDX_0MULNi,
                                                        &aplRht.parts[i],
                                                        &aplRes.parts[i],
                                                         IsMpq0 (&aplLft.parts[i]) ? (mpq_sgn (&aplLft.parts[i]) EQ -1)
                                                                                   : (mpq_sgn (&aplRht.parts[i]) EQ -1)));
    } // End IF/ELSE

    return aplRes;
} // End MulHC2R_RE


//***************************************************************************
//  $PrimFnDydTimesHC2RisHC2RvHC2R
//
//  Primitive scalar function dyadic Times:  HC2R {is} HC2R fn HC2R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC2RisHC2RvHC2R"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC2RisHC2RvHC2R
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
        lpMemRes[uRes] = MulHC2R_RE (lpatLft->aplHC2R, lpatRht->aplHC2R);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC2V);
} // End PrimFnDydTimesHC2RisHC2RvHC2R
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC2V_RE
//***************************************************************************

APLHC2V MulHC2V_RE
    (APLHC2V aplLft,
     APLHC2V aplRht)

{
    int     i;
    APLHC2V aplRes = {0};
    APLVFP  aplTmp1 = {0},
            aplTmp2 = {0};
////WCHAR   wszTemp[1024];
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC2V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC2V, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc2v_init_set (&aplRes, &aplLft);
        else
            mphc2v_init_set (&aplRes, &aplRht);
    } else
    {
////////strcpyW (wszTemp, L"Lft (Mul):  "); *FormatAplHC2V (&wszTemp[lstrlenW (wszTemp)], &aplLft, 0) = WC_EOS; DbgMsgW (wszTemp);
////////strcpyW (wszTemp, L"Rht (Mul):  "); *FormatAplHC2V (&wszTemp[lstrlenW (wszTemp)], &aplRht, 0) = WC_EOS; DbgMsgW (wszTemp);

        // Initialize to 0
        mphc2v_init0 (&aplRes);
        mpfr_init0   (&aplTmp1);
        mpfr_init0   (&aplTmp2);

////////aplRes.parts[0] = aplLft.parts[0] * aplRht.parts[0],
////////                - aplLft.parts[1] * aplRht.parts[1];
////////aplRes.parts[1] = aplLft.parts[0] * aplRht.parts[1]
////////                + aplLft.parts[1] * aplRht.parts[0];

#define a   aplLft.partsLo
#define b   aplLft.partsHi
#define c   aplRht.partsLo
#define d   aplRht.partsHi

        // ***FIXME*** -- Use MulHC1V and friends inside __try/__except
        //                instead of mpfr_XXX

        // Special case b EQ 0 && d EQ 0 so as to avoid an error if a or c is {Inf}
        if (IsMpf0 (&b) && IsMpf0 (&d))
        {
            mpfr_mul (&aplRes.parts[0], &a      , &c      , MPFR_RNDN);
        } else
        // Special case b EQ 0 so as to avoid an error if c or d is {Inf}
        if (IsMpf0 (&b))
        {
            mpfr_mul (&aplRes.parts[0], &a      , &c      , MPFR_RNDN);
            mpfr_mul (&aplRes.parts[1], &d      , &a      , MPFR_RNDN);
        } else
        // Special case d EQ 0 so as to avoid an error if a or b is {Inf}
        if (IsMpf0 (&d))
        {
            mpfr_mul (&aplRes.parts[0], &a      , &c      , MPFR_RNDN);
            mpfr_mul (&aplRes.parts[1], &b      , &c      , MPFR_RNDN);
        } else
        // Special case a EQ 0 && c EQ 0 so as to avoid an error if b or d is {Inf}
        if (IsMpf0 (&a) && IsMpf0 (&c))
        {
            mpfr_mul  (&aplRes.parts[0], &d      , &b     , MPFR_RNDN);
            mpfr_neg  (&aplRes.parts[0], &aplRes.parts[0] , MPFR_RNDN);
        } else
        {
            // Use Cayley-Dickson construction
            // (a,b) x (c,d) = (ac - db, da + bc)
            mpfr_mul (&aplTmp1,         &a      , &c      , MPFR_RNDN);
            mpfr_mul (&aplTmp2,         &d      , &b      , MPFR_RNDN);
            mpfr_sub (&aplRes.parts[0], &aplTmp1, &aplTmp2, MPFR_RNDN);

            mpfr_mul (&aplTmp1,         &d      , &a      , MPFR_RNDN);
            mpfr_mul (&aplTmp2,         &b      , &c      , MPFR_RNDN);
            mpfr_add (&aplRes.parts[1], &aplTmp1, &aplTmp2, MPFR_RNDN);
        } // End IF/ELSE

#undef  d
#undef  c
#undef  b
#undef  a

        Myf_clear (&aplTmp2);
        Myf_clear (&aplTmp1);

////////strcpyW (wszTemp, L"Res (Mul):  "); *FormatAplHC2V (&wszTemp[lstrlenW (wszTemp)], &aplRes, 0) = WC_EOS; DbgMsgW (wszTemp);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
        // Check for indeterminates:  0 {times} _  or  _ {times} 0
        if ((IsMpf0 (&aplLft.parts[i])
          && IsMpfPosInfinity (&aplRht.parts[i]))
         || (IsMpfPosInfinity (&aplLft.parts[i])
          && IsMpf0 (&aplRht.parts[i])))
            mpfr_set (&aplRes.parts[i], mpfr_QuadICValue (&aplLft.parts[i],
                                                           ICNDX_0MULPi,
                                                          &aplRht.parts[i],
                                                          &aplRes.parts[i],
                                                           IsMpf0 (&aplLft.parts[i]) ? SIGN_APLVFP (&aplLft.parts[i])
                                                                                     : SIGN_APLVFP (&aplRht.parts[i])),
                                        MPFR_RNDN);
        else
        // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
        if ((IsMpf0 (&aplLft.parts[i])
          && IsMpfNegInfinity (&aplRht.parts[i]))
         || (IsMpfNegInfinity (&aplLft.parts[i])
          && IsMpf0 (&aplRht.parts[i])))
            mpfr_set (&aplRes.parts[i], mpfr_QuadICValue (&aplLft.parts[i],
                                                           ICNDX_0MULNi,
                                                          &aplRht.parts[i],
                                                          &aplRes.parts[i],
                                                           IsMpf0 (&aplLft.parts[i]) ? SIGN_APLVFP (&aplLft.parts[i])
                                                                                     : SIGN_APLVFP (&aplRht.parts[i])),
                                        MPFR_RNDN);
    } // End IF/ELSE

    return aplRes;
} // End MulHC2V_RE


//***************************************************************************
//  $PrimFnDydTimesHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic Times:  HC2V {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydTimesHC2VisHC2RvHC2R
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

    PrimFnDydTimesHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
    Myhc2v_clear (&atLft.aplHC2V);
} // End PrimFnDydTimesHC2VisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydTimesHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic Times:  HC2V {is} HC2V fn HC2V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC2VisHC2VvHC2V"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC2VisHC2VvHC2V
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
        lpMemRes[uRes] = MulHC2V_RE (lpatLft->aplHC2V, lpatRht->aplHC2V);
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesHC2VisHC2VvHC2V
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC4I_RE_HC
//***************************************************************************

APLHC4I MulHC4I_RE_HC
    (APLHC4I    aplLft,             // Left arg
     APLHC4I    aplRht,             // Right ...
     ENUM_HCMUL eHCMul)             // Multiplication choice (see tagENUM_HCMUL)

{
    APLHC4I aplRes,                 // The result
            aplLxR,                 // Temp var
            aplRxL;                 // ...
    int     i;                      // Loop counter

    // Calculate L x R
    aplLxR = MulHC4I_RE (aplLft, aplRht);

    // Calculate R x L
    aplRxL = MulHC4I_RE (aplRht, aplLft);

    // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
    switch (eHCMul)
    {
        case ENUMHCM_ANTI:
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            {
                // Calculate the difference
                aplRes.parts[i] = aplLxR.parts[i] - aplRxL.parts[i];

                // Divide the difference by 2
                aplRes.parts[i] /= 2;
            } // End FOR

            break;

        case ENUMHCM_COMM:
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            {
                // Calculate the sum
                aplRes.parts[i] = aplLxR.parts[i] + aplRxL.parts[i];

                // Divide the sum by 2
                aplRes.parts[i] /= 2;
            } // End FOR

            break;

        case ENUMHCM_NONE:          // Can't happen
        defstop
            break;
    } // End SWITCH

    return aplRes;
} // End MulHC4I_RE_HC


//***************************************************************************
//  $MulHC4I_RE
//***************************************************************************

APLHC4I MulHC4I_RE
    (APLHC4I aplLft,                // Left arg
     APLHC4I aplRht)                // Right ...

{
    APLHC4I aplRes;
    APLHC2I dP, cP;

    // Use Cayley-Dickson construction
    // (a,b) x (c,d) = (ac - d'b, da + bc')
    //   where d' = conjugate (d)
    //   where c' = conjugate (c)

#define a   aplLft.partsLo[0]
#define b   aplLft.partsHi[0]
#define c   aplRht.partsLo[0]
#define d   aplRht.partsHi[0]

    dP = ConjHC2I (d);
    cP = ConjHC2I (c);

    // ***FIXME*** -- Use __try/__except to catch blowup due to -0

    aplRes.partsLo[0] = SubHC2I_RE (MulHC2I_RE (a  , c),
                                    MulHC2I_RE (dP , b));
    aplRes.partsHi[0] = AddHC2I_RE (MulHC2I_RE (d  , a ),
                                    MulHC2I_RE (b  , cP));
#undef  d
#undef  c
#undef  b
#undef  a

    return aplRes;
} // End MulHC4I_RE


//***************************************************************************
//  $PrimFnDydTimesHC4IisHC4IvHC4I
//
//  Primitive scalar function dyadic Times:  HC4I {is} HC4I fn HC4I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC4IisHC4IvHC4I"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC4IisHC4IvHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
        switch (lpPrimSpec->eHCMul)
        {
            case ENUMHCM_NONE:
                // Call subbfunction
                lpMemRes[uRes] = MulHC4I_RE (lpatLft->aplHC4I, lpatRht->aplHC4I);

                break;

            case ENUMHCM_ANTI:      // ((a x b - b x a) / 2
            case ENUMHCM_COMM:      // ((a x b + b x a) / 2
                // Call subbfunction
                lpMemRes[uRes] = MulHC4I_RE_HC (lpatLft->aplHC4I, lpatRht->aplHC4I, lpPrimSpec->eHCMul);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4F);
} // End PrimFnDydTimesHC4IisHC4IvHC4I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydTimesHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic Times:  HC4F {is} HC4I fn HC4I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC4FisHC4IvHC4I"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC4FisHC4IvHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;
    ALLTYPES        atLft = {0},
                    atRht = {0};

    // Convert the HC4I to HC4F
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatRht, 0, &atRht);

    __try
    {
        // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
        switch (lpPrimSpec->eHCMul)
        {
            case ENUMHCM_NONE:
                // Call subfunction
                lpMemRes[uRes] = MulHC4F_RE (atLft.aplHC4F, atRht.aplHC4F);

                break;

            case ENUMHCM_ANTI:      // ((a x b - b x a) / 2
            case ENUMHCM_COMM:      // ((a x b + b x a) / 2
                // Call subbfunction
                lpMemRes[uRes] = MulHC4F_RE_HC (atLft.aplHC4F, atRht.aplHC4F, lpPrimSpec->eHCMul);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesHC4FisHC4IvHC4I
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC4F_RE_HC
//***************************************************************************

APLHC4F MulHC4F_RE_HC
    (APLHC4F    aplLft,             // Left arg
     APLHC4F    aplRht,             // Right ...
     ENUM_HCMUL eHCMul)             // Multiplication choice (see tagENUM_HCMUL)

{
    APLHC4F aplRes,                 // The result
            aplLxR,                 // Temp var
            aplRxL;                 // ...
    int     i;                      // Loop counter

    // Calculate L x R
    aplLxR = MulHC4F_RE (aplLft, aplRht);

    // Calculate R x L
    aplRxL = MulHC4F_RE (aplRht, aplLft);

    // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
    switch (eHCMul)
    {
        case ENUMHCM_ANTI:
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            {
                // Calculate the difference
                aplRes.parts[i] = aplLxR.parts[i] - aplRxL.parts[i];

                // Divide the difference by 2
                aplRes.parts[i] /= 2;
            } // End FOR

            break;

        case ENUMHCM_COMM:
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            {
                // Calculate the sum
                aplRes.parts[i] = aplLxR.parts[i] + aplRxL.parts[i];

                // Divide the sum by 2
                aplRes.parts[i] /= 2;
            } // End FOR

            break;

        case ENUMHCM_NONE:          // Can't happen
        defstop
            break;
    } // End SWITCH

    return aplRes;
} // End MulHC4F_RE_HC


//***************************************************************************
//  $MulHC4F_RE
//***************************************************************************

APLHC4F MulHC4F_RE
    (APLHC4F aplLft,                // Left arg
     APLHC4F aplRht)                // Right ...

{
    APLHC4F aplRes;
    APLHC2F dP, cP;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC4F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC4F, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            aplRes = aplLft;
        else
            aplRes = aplRht;
    } else
    {
        // Use Cayley-Dickson construction
        // (a,b) x (c,d) = (ac - d'b, da + bc')
        //   where d' = conjugate (d)
        //   where c' = conjugate (c)

#define a   aplLft.partsLo[0]
#define b   aplLft.partsHi[0]
#define c   aplRht.partsLo[0]
#define d   aplRht.partsHi[0]

        dP = ConjHC2F (d);
        cP = ConjHC2F (c);

        // ***FIXME*** -- Use __try/__except to catch blowup due to -0

        // Special case b EQ 0 && d EQ 0 so as to avoid an error if a or c is {Inf}
        if (IsZeroHCxF (&b, 2) && IsZeroHCxF (&d, 2))
        {
            aplRes.partsLo[0] = MulHC2F_RE (a, c);
            aplRes.partsHi[0].partsLo =
            aplRes.partsHi[0].partsHi = 0.0;
        } else
        // Special case b EQ 0 so as to avoid an error if c or d is {Inf}
        if (IsZeroHCxF (&b, 2))
        {
            aplRes.partsLo[0] = MulHC2F_RE (a, c );
            aplRes.partsHi[0] = MulHC2F_RE (d, a );
        } else
        // Special case d EQ 0 so as to avoid an error if a or b is {Inf}
        if (IsZeroHCxF (&d, 2))
        {
            aplRes.partsLo[0] = MulHC2F_RE (a, c );
            aplRes.partsHi[0] = MulHC2F_RE (b, cP);
        } else
        // Special case a EQ 0 && c EQ 0 so as to avoid an error if b or d is {Inf}
        if (IsZeroHCxF (&a, 2) && IsZeroHCxF (&c, 2))
        {
            aplRes.partsLo[0] = NegHC2F_RE (MulHC2F_RE (dP, b));
            aplRes.partsHi[0].partsLo =
            aplRes.partsHi[0].partsHi = 0.0;
        } else
        {
            aplRes.partsLo[0] = SubHC2F_RE (MulHC2F_RE (a  , c ),
                                            MulHC2F_RE (dP , b ));
            aplRes.partsHi[0] = AddHC2F_RE (MulHC2F_RE (d  , a ),
                                            MulHC2F_RE (b  , cP));
        } // End IF/ELSE

#undef  d
#undef  c
#undef  b
#undef  a
    } // End IF/ELSE/...

    return aplRes;
} // End MulHC4F_RE


//***************************************************************************
//  $PrimFnDydTimesHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic Times:  HC4F {is} HC4F fn HC4F
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC4FisHC4FvHC4F"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
        switch (lpPrimSpec->eHCMul)
        {
            case ENUMHCM_NONE:
                // Call subfunction
                lpMemRes[uRes] = MulHC4F_RE (lpatLft->aplHC4F, lpatRht->aplHC4F);

                break;

            case ENUMHCM_ANTI:      // ((a x b - b x a) / 2
            case ENUMHCM_COMM:      // ((a x b + b x a) / 2
                // Call subbfunction
                lpMemRes[uRes] = MulHC4F_RE_HC (lpatLft->aplHC4F, lpatRht->aplHC4F, lpPrimSpec->eHCMul);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesHC4FisHC4FvHC4F
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC4R_RE_HC
//***************************************************************************

APLHC4R MulHC4R_RE_HC
    (APLHC4R    aplLft,             // Left arg
     APLHC4R    aplRht,             // Right ...
     ENUM_HCMUL eHCMul)             // Multiplication choice (see tagENUM_HCMUL)

{
    APLHC4R aplRes,                 // The result
            aplLxR,                 // Temp var
            aplRxL;                 // ...
    int     i;                      // Loop counter

    // Calculate L x R
    aplLxR = MulHC4R_RE (aplLft, aplRht);

    // Calculate R x L
    aplRxL = MulHC4R_RE (aplRht, aplLft);

    // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
    switch (eHCMul)
    {
        case ENUMHCM_ANTI:
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            {
                // Calculate the difference
                aplRes.parts[i] = SubHC1R_RE (aplLxR.parts[i], aplRxL.parts[i]);

                // Divide the difference by 2
                mpq_div_2exp (&aplRes.parts[i], &aplRes.parts[i], 1);
            } // End FOR

            break;

        case ENUMHCM_COMM:
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            {
                // Calculate the sum
                aplRes.parts[i] = AddHC1R_RE (aplLxR.parts[i], aplRxL.parts[i]);

                // Divide the sum by 2
                mpq_div_2exp (&aplRes.parts[i], &aplRes.parts[i], 1);
            } // End FOR

            break;

        case ENUMHCM_NONE:          // Can't happen
        defstop
            break;
    } // End SWITCH

    // We no longer need this storage
    Myhc4r_clear (&aplLxR);
    Myhc4r_clear (&aplRxL);

    return aplRes;
} // End MulHC4R_RE_HC


//***************************************************************************
//  $MulHC4R_RE
//***************************************************************************

APLHC4R MulHC4R_RE
    (APLHC4R aplLft,                    // Left arg
     APLHC4R aplRht)                    // Right ...

{
    int     i;
    APLHC4R aplRes = {0};
    APLHC2R dP, cP;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC4R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC4R, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc4r_init_set (&aplRes, &aplLft);
        else
            mphc4r_init_set (&aplRes, &aplRht);
    } else
    {
        // Use Cayley-Dickson construction
        // (a,b) x (c,d) = (ac - d'b, da + bc')
        //   where d' = conjugate (d)
        //   where c' = conjugate (c)

#define a   aplLft.partsLo
#define b   aplLft.partsHi
#define c   aplRht.partsLo
#define d   aplRht.partsHi

        dP = ConjHC2R (d);
        cP = ConjHC2R (c);

        // ***FIXME*** -- Use __try/__except to catch blowup due to -0

        // Special case b EQ 0 && d EQ 0 so as to avoid an error if a or c is {Inf}
        if (IsZeroHCxR (&b, 2) && IsZeroHCxR (&d, 2))
        {
            aplRes.partsLo = MulHC2R_RE (a, c );

            // Initialize to 0/1
            mphc2r_init (&aplRes.partsHi);
        } else
        // Special case b EQ 0 so as to avoid an error if c or d is {Inf}
        if (IsZeroHCxR (&b, 2))
        {
            aplRes.partsLo = MulHC2R_RE (a, c );
            aplRes.partsHi = MulHC2R_RE (d, a );
        } else
        // Special case d EQ 0 so as to avoid an error if a or b is {Inf}
        if (IsZeroHCxR (&d, 2))
        {
            aplRes.partsLo = MulHC2R_RE (a, c );
            aplRes.partsHi = MulHC2R_RE (b, cP);
        } else
        // Special case a EQ 0 && c EQ 0 so as to avoid an error if b or d is {Inf}
        if (IsZeroHCxR (&a, 2) && IsZeroHCxR (&c, 2))
        {
            APLHC2R aplTmp;

            aplRes.partsLo = MulHC2R_RE (dP, b );
            aplTmp = NegHC2R_RE (aplRes.partsLo);
            mphc2r_set (&aplRes.partsLo, &aplTmp);
            Myhc2r_clear (&aplTmp);

            // Initialize to 0/1
            mphc2r_init (&aplRes.partsHi);
        } else
        {
            aplRes.partsLo = SubHC2R_RE (MulHC2R_RE (a  , c  ),
                                         MulHC2R_RE (dP , b  ));
            aplRes.partsHi = AddHC2R_RE (MulHC2R_RE (d  , a  ),
                                         MulHC2R_RE (b  , cP ));
        } // End IF/ELSE

#undef  d
#undef  c
#undef  b
#undef  a

        Myhc2r_clear (&cP);
        Myhc2r_clear (&dP);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
        // Check for indeterminates:  0 {times} _  or  _ {times} 0
        if ((IsMpq0 (&aplLft.parts[i])
          && IsMpqPosInfinity (&aplRht.parts[i]))
         || (IsMpqPosInfinity (&aplLft.parts[i])
          && IsMpq0 (&aplRht.parts[i])))
            mpq_set (&aplRes.parts[i], mpq_QuadICValue (&aplLft.parts[i],
                                                         ICNDX_0MULPi,
                                                       &aplRht.parts[i],
                                                       &aplRes.parts[i],
                                                        IsMpq0 (&aplLft.parts[i]) ? (mpq_sgn (&aplLft.parts[i]) EQ -1)
                                                                                  : (mpq_sgn (&aplRht.parts[i]) EQ -1)));
        else
        // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
        if ((IsMpq0 (&aplLft.parts[i])
          && IsMpqNegInfinity (&aplRht.parts[i]))
         || (IsMpqNegInfinity (&aplLft.parts[i])
          && IsMpq0 (&aplRht.parts[i])))
            mpq_set (&aplRes.parts[i], mpq_QuadICValue (&aplLft.parts[i],
                                                         ICNDX_0MULNi,
                                                        &aplRht.parts[i],
                                                        &aplRes.parts[i],
                                                         IsMpq0 (&aplLft.parts[i]) ? (mpq_sgn (&aplLft.parts[i]) EQ -1)
                                                                                   : (mpq_sgn (&aplRht.parts[i]) EQ -1)));
    } // End IF/ELSE

    return aplRes;
} // End MulHC4R_RE


//***************************************************************************
//  $PrimFnDydTimesHC4RisHC4RvHC4R
//
//  Primitive scalar function dyadic Times:  HC4R {is} HC4R fn HC4R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC4RisHC4RvHC4R"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC4RisHC4RvHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
        switch (lpPrimSpec->eHCMul)
        {
            case ENUMHCM_NONE:
                // Call subfunction
                lpMemRes[uRes] = MulHC4R_RE (lpatLft->aplHC4R, lpatRht->aplHC4R);

                break;

            case ENUMHCM_ANTI:      // ((a x b - b x a) / 2
            case ENUMHCM_COMM:      // ((a x b + b x a) / 2
                // Call subbfunction
                lpMemRes[uRes] = MulHC4R_RE_HC (lpatLft->aplHC4R, lpatRht->aplHC4R, lpPrimSpec->eHCMul);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC4V);
} // End PrimFnDydTimesHC4RisHC4RvHC4R
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC4V_RE_HC
//***************************************************************************

APLHC4V MulHC4V_RE_HC
    (APLHC4V    aplLft,             // Left arg
     APLHC4V    aplRht,             // Right ...
     ENUM_HCMUL eHCMul)             // Multiplication choice (see tagENUM_HCMUL)

{
    APLHC4V aplRes,                 // The result
            aplLxR,                 // Temp var
            aplRxL;                 // ...
    int     i;                      // Loop counter

    // Calculate L x R
    aplLxR = MulHC4V_RE (aplLft, aplRht);

    // Calculate R x L
    aplRxL = MulHC4V_RE (aplRht, aplLft);

    // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
    switch (eHCMul)
    {
        case ENUMHCM_ANTI:
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            {
                // Calculate the difference
                aplRes.parts[i] = SubHC1V_RE (aplLxR.parts[i], aplRxL.parts[i]);

                // Divide the difference by 2
                mpfr_div_2exp (&aplRes.parts[i], &aplRes.parts[i], 1, MPFR_RNDN);
            } // End FOR

            break;

        case ENUMHCM_COMM:
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            {
                // Calculate the sum
                aplRes.parts[i] = AddHC1V_RE (aplLxR.parts[i], aplRxL.parts[i]);

                // Divide the sum by 2
                mpfr_div_2exp (&aplRes.parts[i], &aplRes.parts[i], 1, MPFR_RNDN);
            } // End FOR

            break;

        case ENUMHCM_NONE:          // Can't happen
        defstop
            break;
    } // End SWITCH

    // We no longer need this storage
    Myhc4v_clear (&aplLxR);
    Myhc4v_clear (&aplRxL);

    return aplRes;
} // End MulHC4V_RE_HC


//***************************************************************************
//  $MulHC4V_RE
//***************************************************************************

APLHC4V MulHC4V_RE
    (APLHC4V aplLft,
     APLHC4V aplRht)

{
    int     i;
    APLHC4V aplRes = {0};
    APLHC2V dP,cP;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC4V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC4V, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc4v_init_set (&aplRes, &aplLft);
        else
            mphc4v_init_set (&aplRes, &aplRht);
    } else
    {
////////aplRes.parts[0] = aplLft.parts[0] * aplRht.parts[0],
////////                - aplLft.parts[1] * aplRht.parts[1];
////////aplRes.parts[1] = aplLft.parts[0] * aplRht.parts[1]
////////                + aplLft.parts[1] * aplRht.parts[0];

        // Use Cayley-Dickson construction
        // (a,b) x (c,d) = (ac - d'b, da + bc')
        //   where d' = conjugate (d)
        //   where c' = conjugate (c)

#define a   aplLft.partsLo
#define b   aplLft.partsHi
#define c   aplRht.partsLo
#define d   aplRht.partsHi

        dP = ConjHC2V (d);
        cP = ConjHC2V (c);

        // ***FIXME*** -- Use __try/__except to catch blowup due to -0

        // Special case b EQ 0 && d EQ 0 so as to avoid an error if a or c is {Inf}
        if (IsZeroHCxV (&b, 2) && IsZeroHCxV (&d, 2))
        {
            aplRes.partsLo = MulHC2V_RE (a, c );

            // Initialize to 0
            mphc2v_init0 (&aplRes.partsHi);
        } else
        // Special case b EQ 0 so as to avoid an error if c or d is {Inf}
        if (IsZeroHCxV (&b, 2))
        {
            aplRes.partsLo = MulHC2V_RE (a, c );
            aplRes.partsHi = MulHC2V_RE (d, a );
        } else
        // Special case d EQ 0 so as to avoid an error if a or b is {Inf}
        if (IsZeroHCxV (&d, 2))
        {
            aplRes.partsLo = MulHC2V_RE (a, c );
            aplRes.partsHi = MulHC2V_RE (b, cP);
        } else
        // Special case a EQ 0 && c EQ 0 so as to avoid an error if b or d is {Inf}
        if (IsZeroHCxV (&a, 2) && IsZeroHCxV (&c, 2))
        {
            APLHC2V aplTmp;

            aplRes.partsLo = MulHC2V_RE (dP, b );
            aplTmp = NegHC2V_RE (aplRes.partsLo);
            mphc2v_set (&aplRes.partsLo, &aplTmp);
            Myhc2v_clear (&aplTmp);

            // Initialize to 0
            mphc2v_init0 (&aplRes.partsHi);
        } else
        {
            aplRes.partsLo = SubHC2V_RE (MulHC2V_RE (a  , c  ),
                                         MulHC2V_RE (dP , b  ));
            aplRes.partsHi = AddHC2V_RE (MulHC2V_RE (d  , a  ),
                                         MulHC2V_RE (b  , cP ));
        } // End IF/ELSE

#undef  d
#undef  c
#undef  b
#undef  a

        Myhc2v_clear (&cP);
        Myhc2v_clear (&dP);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
        // Check for indeterminates:  0 {times} _  or  _ {times} 0
        if ((IsMpf0 (&aplLft.parts[i])
          && IsMpfPosInfinity (&aplRht.parts[i]))
         || (IsMpfPosInfinity (&aplLft.parts[i])
          && IsMpf0 (&aplRht.parts[i])))
            mpfr_set (&aplRes.parts[i], mpfr_QuadICValue (&aplLft.parts[i],
                                                           ICNDX_0MULPi,
                                                          &aplRht.parts[i],
                                                          &aplRes.parts[i],
                                                           IsMpf0 (&aplLft.parts[i]) ? SIGN_APLVFP (&aplLft.parts[i])
                                                                                     : SIGN_APLVFP (&aplRht.parts[i])),
                                        MPFR_RNDN);
        else
        // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
        if ((IsMpf0 (&aplLft.parts[i])
          && IsMpfNegInfinity (&aplRht.parts[i]))
         || (IsMpfNegInfinity (&aplLft.parts[i])
          && IsMpf0 (&aplRht.parts[i])))
            mpfr_set (&aplRes.parts[i], mpfr_QuadICValue (&aplLft.parts[i],
                                                           ICNDX_0MULNi,
                                                          &aplRht.parts[i],
                                                          &aplRes.parts[i],
                                                           IsMpf0 (&aplLft.parts[i]) ? SIGN_APLVFP (&aplLft.parts[i])
                                                                                     : SIGN_APLVFP (&aplRht.parts[i])),
                                        MPFR_RNDN);
    } // End IF/ELSE

    return aplRes;
} // End MulHC4V_RE


//***************************************************************************
//  $PrimFnDydTimesHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic Times:  HC4V {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydTimesHC4VisHC4RvHC4R
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

    PrimFnDydTimesHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
    Myhc4v_clear (&atLft.aplHC4V);
} // End PrimFnDydTimesHC4VisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydTimesHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic Times:  HC4V {is} HC4V fn HC4V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC4VisHC4VvHC4V"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC4VisHC4VvHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
        switch (lpPrimSpec->eHCMul)
        {
            case ENUMHCM_NONE:
                // Call subfunction
                lpMemRes[uRes] = MulHC4V_RE (lpatLft->aplHC4V, lpatRht->aplHC4V);

                break;

            case ENUMHCM_ANTI:      // ((a x b - b x a) / 2
            case ENUMHCM_COMM:      // ((a x b + b x a) / 2
                // Call subbfunction
                lpMemRes[uRes] = MulHC4V_RE_HC (lpatLft->aplHC4V, lpatRht->aplHC4V, lpPrimSpec->eHCMul);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesHC4VisHC4VvHC4V
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC8I_RE_HC
//***************************************************************************

APLHC8I MulHC8I_RE_HC
    (APLHC8I    aplLft,             // Left arg
     APLHC8I    aplRht,             // Right ...
     ENUM_HCMUL eHCMul)             // Multiplication choice (see tagENUM_HCMUL)

{
    APLHC8I aplRes,                 // The result
            aplLxR,                 // Temp var
            aplRxL;                 // ...
    int     i;                      // Loop counter

    // Calculate L x R
    aplLxR = MulHC8I_RE (aplLft, aplRht);

    // Calculate R x L
    aplRxL = MulHC8I_RE (aplRht, aplLft);

    // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
    switch (eHCMul)
    {
        case ENUMHCM_ANTI:
            // Loop through all of the parts
            for (i = 0; i < 8; i++)
            {
                // Calculate the difference
                aplRes.parts[i] = aplLxR.parts[i] - aplRxL.parts[i];

                // Divide the difference by 2
                aplRes.parts[i] /= 2;
            } // End FOR

            break;

        case ENUMHCM_COMM:
            // Loop through all of the parts
            for (i = 0; i < 8; i++)
            {
                // Calculate the sum
                aplRes.parts[i] = aplLxR.parts[i] + aplRxL.parts[i];

                // Divide the sum by 2
                aplRes.parts[i] /= 2;
            } // End FOR

            break;

        case ENUMHCM_NONE:          // Can't happen
        defstop
            break;
    } // End SWITCH

    return aplRes;
} // End MulHC8I_RE_HC


//***************************************************************************
//  $MulHC8I_RE
//***************************************************************************

APLHC8I MulHC8I_RE
    (APLHC8I aplLft,                // Left arg
     APLHC8I aplRht)                // Right ...

{
    APLHC8I aplRes;
    APLHC4I dP, cP;

    // Use Cayley-Dickson construction
    // (a,b) x (c,d) = (ac - d'b, da + bc')
    //   where d' = conjugate (d)
    //   where c' = conjugate (c)

#define a   aplLft.partsLo[0]
#define b   aplLft.partsHi[0]
#define c   aplRht.partsLo[0]
#define d   aplRht.partsHi[0]

    dP = ConjHC4I (d);
    cP = ConjHC4I (c);

    // ***FIXME*** -- Use __try/__except to catch blowup due to -0

    aplRes.partsLo[0] = SubHC4I_RE (MulHC4I_RE (a  , c  ),
                                    MulHC4I_RE (dP , b  ));
    aplRes.partsHi[0] = AddHC4I_RE (MulHC4I_RE (d  , a  ),
                                    MulHC4I_RE (b  , cP ));
#undef  d
#undef  c
#undef  b
#undef  a

    return aplRes;
} // End MulHC8I_RE


//***************************************************************************
//  $PrimFnDydTimesHC8IisHC8IvHC8I
//
//  Primitive scalar function dyadic Times:  HC8I {is} HC8I fn HC8I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC8IisHC8IvHC8I"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC8IisHC8IvHC8I
    (LPAPLHC8I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
        switch (lpPrimSpec->eHCMul)
        {
            case ENUMHCM_NONE:
                // Call subfunction
                lpMemRes[uRes] = MulHC8I_RE (lpatLft->aplHC8I, lpatRht->aplHC8I);

                break;

            case ENUMHCM_ANTI:      // ((a x b - b x a) / 2
            case ENUMHCM_COMM:      // ((a x b + b x a) / 2
                // Call subbfunction
                lpMemRes[uRes] = MulHC8I_RE_HC (lpatLft->aplHC8I, lpatRht->aplHC8I, lpPrimSpec->eHCMul);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8F);
} // End PrimFnDydTimesHC8IisHC8IvHC8I
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydTimesHC8FisHC8IvHC8I
//
//  Primitive scalar function dyadic Times:  HC8F {is} HC8I fn HC8I
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC8FisHC8IvHC8I"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC8FisHC8IvHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;
    ALLTYPES        atLft = {0},
                    atRht = {0};

    // Convert the HC8I to HC8F
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC8I][ARRAY_HC8F]) (lpatRht, 0, &atRht);

    __try
    {
        // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
        switch (lpPrimSpec->eHCMul)
        {
            case ENUMHCM_NONE:
                // Call subfunction
                lpMemRes[uRes] = MulHC8F_RE (atLft.aplHC8F, atRht.aplHC8F);

                break;

            case ENUMHCM_ANTI:      // ((a x b - b x a) / 2
            case ENUMHCM_COMM:      // ((a x b + b x a) / 2
                // Call subbfunction
                lpMemRes[uRes] = MulHC8F_RE_HC (atLft.aplHC8F, atRht.aplHC8F, lpPrimSpec->eHCMul);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesHC8FisHC8IvHC8I
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC8F_RE_HC
//***************************************************************************

APLHC8F MulHC8F_RE_HC
    (APLHC8F    aplLft,             // Left arg
     APLHC8F    aplRht,             // Right ...
     ENUM_HCMUL eHCMul)             // Multiplication choice (see tagENUM_HCMUL)

{
    APLHC8F aplRes,                 // The result
            aplLxR,                 // Temp var
            aplRxL;                 // ...
    int     i;                      // Loop counter

    // Calculate L x R
    aplLxR = MulHC8F_RE (aplLft, aplRht);

    // Calculate R x L
    aplRxL = MulHC8F_RE (aplRht, aplLft);

    // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
    switch (eHCMul)
    {
        case ENUMHCM_ANTI:
            // Loop through all of the parts
            for (i = 0; i < 8; i++)
            {
                // Calculate the difference
                aplRes.parts[i] = aplLxR.parts[i] - aplRxL.parts[i];

                // Divide the difference by 2
                aplRes.parts[i] /= 2;
            } // End FOR

            break;

        case ENUMHCM_COMM:
            // Loop through all of the parts
            for (i = 0; i < 8; i++)
            {
                // Calculate the sum
                aplRes.parts[i] = aplLxR.parts[i] + aplRxL.parts[i];

                // Divide the sum by 2
                aplRes.parts[i] /= 2;
            } // End FOR

            break;

        case ENUMHCM_NONE:          // Can't happen
        defstop
            break;
    } // End SWITCH

    return aplRes;
} // End MulHC8F_RE_HC


//***************************************************************************
//  $MulHC8F_RE
//***************************************************************************

APLHC8F MulHC8F_RE
    (APLHC8F aplLft,
     APLHC8F aplRht)

{
    APLHC8F aplRes;
    APLHC4F dP, cP;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC8F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC8F, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            aplRes = aplLft;
        else
            aplRes = aplRht;
    } else
    {
        // Use Cayley-Dickson construction
        // (a,b) x (c,d) = (ac - d'b, da + bc')
        //   where d' = conjugate (d)
        //   where c' = conjugate (c)

#define a   aplLft.partsLo[0]
#define b   aplLft.partsHi[0]
#define c   aplRht.partsLo[0]
#define d   aplRht.partsHi[0]

        dP = ConjHC4F (d);
        cP = ConjHC4F (c);

        // ***FIXME*** -- Use __try/__except to catch blowup due to -0

        // Special case b EQ 0 && d EQ 0 so as to avoid an error if a or c is {Inf}
        if (IsZeroHCxF (&b, 4) && IsZeroHCxF (&d, 4))
        {
            aplRes.partsLo[0] = MulHC4F_RE (a, c );

            // Initialize to 0.0
            aplRes.partsHi[0].partsLo[0].partsLo =
            aplRes.partsHi[0].partsLo[0].partsHi =
            aplRes.partsHi[0].partsHi[0].partsLo =
            aplRes.partsHi[0].partsHi[0].partsHi = 0.0;
        } else
        // Special case b EQ 0 so as to avoid an error if c or d is {Inf}
        if (IsZeroHCxF (&b, 4))
        {
            aplRes.partsLo[0] = MulHC4F_RE (a, c );
            aplRes.partsHi[0] = MulHC4F_RE (d, a );
        } else
        // Special case d EQ 0 so as to avoid an error if a or b is {Inf}
        if (IsZeroHCxF (&d, 4))
        {
            aplRes.partsLo[0] = MulHC4F_RE (a, c );
            aplRes.partsHi[0] = MulHC4F_RE (b, cP);
        } else
        // Special case a EQ 0 && c EQ 0 so as to avoid an error if b or d is {Inf}
        if (IsZeroHCxF (&a, 4) && IsZeroHCxF (&c, 4))
        {
            aplRes.partsLo[0] = NegHC4F_RE (MulHC4F_RE (dP, b));
            aplRes.partsHi[0].partsLo[0].partsLo =
            aplRes.partsHi[0].partsLo[0].partsHi =
            aplRes.partsHi[0].partsHi[0].partsLo =
            aplRes.partsHi[0].partsHi[0].partsHi = 0.0;
        } else
        {
            aplRes.partsLo[0] = SubHC4F_RE (MulHC4F_RE (a  , c ),
                                            MulHC4F_RE (dP , b ));
            aplRes.partsHi[0] = AddHC4F_RE (MulHC4F_RE (d  , a ),
                                            MulHC4F_RE (b  , cP));
        } // End IF/ELSE

#undef  d
#undef  c
#undef  b
#undef  a
    } // End IF/ELSE/...

    return aplRes;
} // End MulHC8F_RE


//***************************************************************************
//  $PrimFnDydTimesHC8FisHC8FvHC8F
//
//  Primitive scalar function dyadic Times:  HC8F {is} HC8F fn HC8F
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC8FisHC8FvHC8F"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC8FisHC8FvHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
        switch (lpPrimSpec->eHCMul)
        {
            case ENUMHCM_NONE:
                // Call subfunction
                lpMemRes[uRes] = MulHC8F_RE (lpatLft->aplHC8F, lpatRht->aplHC8F);

                break;

            case ENUMHCM_ANTI:      // ((a x b - b x a) / 2
            case ENUMHCM_COMM:      // ((a x b + b x a) / 2
                // Call subbfunction
                lpMemRes[uRes] = MulHC8F_RE_HC (lpatLft->aplHC8F, lpatRht->aplHC8F, lpPrimSpec->eHCMul);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesHC8FisHC8FvHC8F
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC8R_RE_HC
//***************************************************************************

APLHC8R MulHC8R_RE_HC
    (APLHC8R    aplLft,             // Left arg
     APLHC8R    aplRht,             // Right ...
     ENUM_HCMUL eHCMul)             // Multiplication choice (see tagENUM_HCMUL)

{
    APLHC8R aplRes,                 // The result
            aplLxR,                 // Temp var
            aplRxL;                 // ...
    int     i;                      // Loop counter

    // Calculate L x R
    aplLxR = MulHC8R_RE (aplLft, aplRht);

    // Calculate R x L
    aplRxL = MulHC8R_RE (aplRht, aplLft);

    // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
    switch (eHCMul)
    {
        case ENUMHCM_ANTI:
            // Loop through all of the parts
            for (i = 0; i < 8; i++)
            {
                // Calculate the difference
                aplRes.parts[i] = SubHC1R_RE (aplLxR.parts[i], aplRxL.parts[i]);

                // Divide the difference by 2
                mpq_div_2exp (&aplRes.parts[i], &aplRes.parts[i], 1);
            } // End FOR

            break;

        case ENUMHCM_COMM:
            // Loop through all of the parts
            for (i = 0; i < 8; i++)
            {
                // Calculate the sum
                aplRes.parts[i] = AddHC1R_RE (aplLxR.parts[i], aplRxL.parts[i]);

                // Divide the sum by 2
                mpq_div_2exp (&aplRes.parts[i], &aplRes.parts[i], 1);
            } // End FOR

            break;

        case ENUMHCM_NONE:          // Can't happen
        defstop
            break;
    } // End SWITCH

    // We no longer need this storage
    Myhc8r_clear (&aplLxR);
    Myhc8r_clear (&aplRxL);

    return aplRes;
} // End MulHC8R_RE_HC


//***************************************************************************
//  $MulHC8R_RE
//***************************************************************************

APLHC8R MulHC8R_RE
    (APLHC8R aplLft,                    // Left arg
     APLHC8R aplRht)                    // Right ...

{
    int     i;
    APLHC8R aplRes = {0};
    APLHC4R dP, cP;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC8R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC8R, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc8r_init_set (&aplRes, &aplLft);
        else
            mphc8r_init_set (&aplRes, &aplRht);
    } else
    {
        // Use Cayley-Dickson construction
        // (a,b) x (c,d) = (ac - d'b, da + bc')
        //   where d' = conjugate (d)
        //   where c' = conjugate (c)

#define a   aplLft.partsLo
#define b   aplLft.partsHi
#define c   aplRht.partsLo
#define d   aplRht.partsHi

        dP = ConjHC4R (d);
        cP = ConjHC4R (c);

        // ***FIXME*** -- Use __try/__except to catch blowup due to -0

        // Special case b EQ 0 && d EQ 0 so as to avoid an error if a or c is {Inf}
        if (IsZeroHCxR (&b, 4) && IsZeroHCxR (&d, 4))
        {
            aplRes.partsLo = MulHC4R_RE (a, c );

            // Initialize to 0/1
            mphc4r_init (&aplRes.partsHi);
        } else
        // Special case b EQ 0 so as to avoid an error if c or d is {Inf}
        if (IsZeroHCxR (&b, 4))
        {
            aplRes.partsLo = MulHC4R_RE (a, c );
            aplRes.partsHi = MulHC4R_RE (d, a );
        } else
        // Special case d EQ 0 so as to avoid an error if a or b is {Inf}
        if (IsZeroHCxR (&d, 4))
        {
            aplRes.partsLo = MulHC4R_RE (a, c );
            aplRes.partsHi = MulHC4R_RE (b, cP);
        } else
        // Special case a EQ 0 && c EQ 0 so as to avoid an error if b or d is {Inf}
        if (IsZeroHCxR (&a, 4) && IsZeroHCxR (&c, 4))
        {
            APLHC4R aplTmp;

            aplRes.partsLo = MulHC4R_RE (dP, b );
            aplTmp = NegHC4R_RE (aplRes.partsLo);
            mphc4r_set (&aplRes.partsLo, &aplTmp);
            Myhc4r_clear (&aplTmp);

            // Initialize to 0/1
            mphc4r_init (&aplRes.partsHi);
        } else
        {
            aplRes.partsLo = SubHC4R_RE (MulHC4R_RE (a  , c  ),
                                         MulHC4R_RE (dP , b  ));
            aplRes.partsHi = AddHC4R_RE (MulHC4R_RE (d  , a  ),
                                         MulHC4R_RE (b  , cP ));
        } // End IF/ELSE

#undef  d
#undef  c
#undef  b
#undef  a

        Myhc4r_clear (&cP);
        Myhc4r_clear (&dP);

        // Loop through all of the parts
        for (i = 0; i < 8; i++)
        // Check for indeterminates:  0 {times} _  or  _ {times} 0
        if ((IsMpq0 (&aplLft.parts[i])
          && IsMpqPosInfinity (&aplRht.parts[i]))
         || (IsMpqPosInfinity (&aplLft.parts[i])
          && IsMpq0 (&aplRht.parts[i])))
            mpq_set (&aplRes.parts[i], mpq_QuadICValue (&aplLft.parts[i],
                                                         ICNDX_0MULPi,
                                                       &aplRht.parts[i],
                                                       &aplRes.parts[i],
                                                        IsMpq0 (&aplLft.parts[i]) ? (mpq_sgn (&aplLft.parts[i]) EQ -1)
                                                                                  : (mpq_sgn (&aplRht.parts[i]) EQ -1)));
        else
        // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
        if ((IsMpq0 (&aplLft.parts[i])
          && IsMpqNegInfinity (&aplRht.parts[i]))
         || (IsMpqNegInfinity (&aplLft.parts[i])
          && IsMpq0 (&aplRht.parts[i])))
            mpq_set (&aplRes.parts[i], mpq_QuadICValue (&aplLft.parts[i],
                                                         ICNDX_0MULNi,
                                                        &aplRht.parts[i],
                                                        &aplRes.parts[i],
                                                         IsMpq0 (&aplLft.parts[i]) ? (mpq_sgn (&aplLft.parts[i]) EQ -1)
                                                                                   : (mpq_sgn (&aplRht.parts[i]) EQ -1)));
    } // End IF/ELSE

    return aplRes;
} // End MulHC8R_RE


//***************************************************************************
//  $PrimFnDydTimesHC8RisHC8RvHC8R
//
//  Primitive scalar function dyadic Times:  HC8R {is} HC8R fn HC8R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC8RisHC8RvHC8R"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC8RisHC8RvHC8R
    (LPAPLHC8R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
        switch (lpPrimSpec->eHCMul)
        {
            case ENUMHCM_NONE:
                // Call subfunction
                lpMemRes[uRes] = MulHC8R_RE (lpatLft->aplHC8R, lpatRht->aplHC8R);

                break;

            case ENUMHCM_ANTI:      // ((a x b - b x a) / 2
            case ENUMHCM_COMM:      // ((a x b + b x a) / 2
                // Call subbfunction
                lpMemRes[uRes] = MulHC8R_RE_HC (lpatLft->aplHC8R, lpatRht->aplHC8R, lpPrimSpec->eHCMul);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_RESULT_HC8V);
} // End PrimFnDydTimesHC8RisHC8RvHC8R
#undef  APPEND_NAME


//***************************************************************************
//  $MulHC8V_RE_HC
//***************************************************************************

APLHC8V MulHC8V_RE_HC
    (APLHC8V    aplLft,             // Left arg
     APLHC8V    aplRht,             // Right ...
     ENUM_HCMUL eHCMul)             // Multiplication choice (see tagENUM_HCMUL)

{
    APLHC8V aplRes,                 // The result
            aplLxR,                 // Temp var
            aplRxL;                 // ...
    int     i;                      // Loop counter

    // Calculate L x R
    aplLxR = MulHC8V_RE (aplLft, aplRht);

    // Calculate R x L
    aplRxL = MulHC8V_RE (aplRht, aplLft);

    // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
    switch (eHCMul)
    {
        case ENUMHCM_ANTI:
            // Loop through all of the parts
            for (i = 0; i < 8; i++)
            {
                // Calculate the difference
                aplRes.parts[i] = SubHC1V_RE (aplLxR.parts[i], aplRxL.parts[i]);

                // Divide the difference by 2
                mpfr_div_2exp (&aplRes.parts[i], &aplRes.parts[i], 1, MPFR_RNDN);
            } // End FOR

            break;

        case ENUMHCM_COMM:
            // Loop through all of the parts
            for (i = 0; i < 8; i++)
            {
                // Calculate the sum
                aplRes.parts[i] = AddHC1V_RE (aplLxR.parts[i], aplRxL.parts[i]);

                // Divide the sum by 2
                mpfr_div_2exp (&aplRes.parts[i], &aplRes.parts[i], 1, MPFR_RNDN);
            } // End FOR

            break;

        case ENUMHCM_NONE:          // Can't happen
        defstop
            break;
    } // End SWITCH

    // We no longer need this storage
    Myhc8v_clear (&aplLxR);
    Myhc8v_clear (&aplRxL);

    return aplRes;
} // End MulHC8V_RE_HC


//***************************************************************************
//  $MulHC8V_RE
//***************************************************************************

APLHC8V MulHC8V_RE
    (APLHC8V aplLft,
     APLHC8V aplRht)

{
    int     i;
    APLHC8V aplRes;
    APLHC4V dP,cP;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC8V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC8V, &aplRht, 0);

    // Check for indeterminates:  _ {times} _  or  -_ {times} -_

    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc8v_init_set (&aplRes, &aplLft);
        else
            mphc8v_init_set (&aplRes, &aplRht);
    } else
    {
        // Use Cayley-Dickson construction
        // (a,b) x (c,d) = (ac - d'b, da + bc')
        //   where d' = conjugate (d)
        //   where c' = conjugate (c)

#define a   aplLft.partsLo
#define b   aplLft.partsHi
#define c   aplRht.partsLo
#define d   aplRht.partsHi

        dP = ConjHC4V (d);
        cP = ConjHC4V (c);

        // ***FIXME*** -- Use __try/__except to catch blowup due to -0

        // Special case b EQ 0 && d EQ 0 so as to avoid an error if a or c is {Inf}
        if (IsZeroHCxV (&b, 4) && IsZeroHCxV (&d, 4))
        {
            aplRes.partsLo = MulHC4V_RE (a, c );

            // Initialize to 0
            mphc4v_init0 (&aplRes.partsHi);
        } else
        // Special case b EQ 0 so as to avoid an error if c or d is {Inf}
        if (IsZeroHCxV (&b, 4))
        {
            aplRes.partsLo = MulHC4V_RE (a, c );
            aplRes.partsHi = MulHC4V_RE (d, a );
        } else
        // Special case d EQ 0 so as to avoid an error if a or b is {Inf}
        if (IsZeroHCxV (&d, 4))
        {
            aplRes.partsLo = MulHC4V_RE (a, c );
            aplRes.partsHi = MulHC4V_RE (b, cP);
        } else
        // Special case a EQ 0 && c EQ 0 so as to avoid an error if b or d is {Inf}
        if (IsZeroHCxV (&a, 4) && IsZeroHCxV (&c, 4))
        {
            APLHC4V aplTmp;

            aplRes.partsLo = MulHC4V_RE (dP, b );
            aplTmp = NegHC4V_RE (aplRes.partsLo);
            mphc4v_set (&aplRes.partsLo, &aplTmp);
            Myhc4v_clear (&aplTmp);

            // Initialize to 0
            mphc4v_init0 (&aplRes.partsHi);
        } else
        {
            aplRes.partsLo = SubHC4V_RE (MulHC4V_RE (a  , c  ),
                                         MulHC4V_RE (dP , b  ));
            aplRes.partsHi = AddHC4V_RE (MulHC4V_RE (d  , a  ),
                                         MulHC4V_RE (b  , cP ));
        } // End IF/ELSE

#undef  d
#undef  c
#undef  b
#undef  a

        Myhc4v_clear (&cP);
        Myhc4v_clear (&dP);

        // Loop through all of the parts
        for (i = 0; i < 8; i++)
        // Check for indeterminates:  0 {times} _  or  _ {times} 0
        if ((IsMpf0 (&aplLft.parts[i])
          && IsMpfPosInfinity (&aplRht.parts[i]))
         || (IsMpfPosInfinity (&aplLft.parts[i])
          && IsMpf0 (&aplRht.parts[i])))
            mpfr_set (&aplRes.parts[i], mpfr_QuadICValue (&aplLft.parts[i],
                                                           ICNDX_0MULPi,
                                                          &aplRht.parts[i],
                                                          &aplRes.parts[i],
                                                           IsMpf0 (&aplLft.parts[i]) ? SIGN_APLVFP (&aplLft.parts[i])
                                                                                     : SIGN_APLVFP (&aplRht.parts[i])),
                                        MPFR_RNDN);
        else
        // Check for indeterminates:  0 {times} {neg}_  or  {neg}_ {times} 0
        if ((IsMpf0 (&aplLft.parts[i])
          && IsMpfNegInfinity (&aplRht.parts[i]))
         || (IsMpfNegInfinity (&aplLft.parts[i])
          && IsMpf0 (&aplRht.parts[i])))
            mpfr_set (&aplRes.parts[i], mpfr_QuadICValue (&aplLft.parts[i],
                                                           ICNDX_0MULNi,
                                                          &aplRht.parts[i],
                                                          &aplRes.parts[i],
                                                           IsMpf0 (&aplLft.parts[i]) ? SIGN_APLVFP (&aplLft.parts[i])
                                                                                     : SIGN_APLVFP (&aplRht.parts[i])),
                                        MPFR_RNDN);
    } // End IF/ELSE

    return aplRes;
} // End MulHC8V_RE


//***************************************************************************
//  $PrimFnDydTimesHC8VisHC8RvHC8R
//
//  Primitive scalar function dyadic times:  HC8V {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydTimesHC8VisHC8RvHC8R
    (LPAPLHC8V  lpMemRes,           // ptr to the result
     APLUINT    uRes,               // index into the result
     LPALLTYPES lpatLft,            // ptr to left arg alltypes
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // ptr to local primspec

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Promote the left & right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatLft->aplHC8R, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatRht->aplHC8R, 0, &atRht);

    PrimFnDydTimesHC8VisHC8VvHC8V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc8v_clear (&atRht.aplHC8V);
    Myhc8v_clear (&atLft.aplHC8V);
} // End PrimFnDydTimesHC8VisHC8RvHC8R


//***************************************************************************
//  $PrimFnDydTimesHC8VisHC8VvHC8V
//
//  Primitive scalar function dyadic Times:  HC8V {is} HC8V fn HC8V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesHC8VisHC8VvHC8V"
#else
#define APPEND_NAME
#endif

void PrimFnDydTimesHC8VisHC8VvHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    EXCEPTION_CODES exCode = EXCEPTION_SUCCESS;

    __try
    {
        // Split cases based upon the Hypercomplex Arithmetic Multiplication choice
        switch (lpPrimSpec->eHCMul)
        {
            case ENUMHCM_NONE:
                // Call subfunction
                lpMemRes[uRes] = MulHC8V_RE (lpatLft->aplHC8V, lpatRht->aplHC8V);

                break;

            case ENUMHCM_ANTI:      // ((a x b - b x a) / 2
            case ENUMHCM_COMM:      // ((a x b + b x a) / 2
                // Call subbfunction
                lpMemRes[uRes] = MulHC8V_RE_HC (lpatLft->aplHC8V, lpatRht->aplHC8V, lpPrimSpec->eHCMul);

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckExceptionS (GetExceptionInformation (), __FUNCTION__))
    {
        exCode = MyGetExceptionCode ();  // The exception code

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %s (%S#%d)", MyGetExceptionStr (exCode), FNLN);

        // Check the exception code in a helper function
        CheckExCodeHelper (&exCode);
    } // End __try/__except

    // Check the exception code in a main function
    CheckExCodeMain_RE (&exCode, EXCEPTION_DOMAIN_ERROR);
} // End PrimFnDydTimesHC8VisHC8VvHC8V
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydTimesAPA_EM
//
//  Dyadic Times, result is APA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydTimesAPA_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydTimesAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to result token (may be NULL)

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
    APLNELM           aplNELMRes;           // Result NELM
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
    // Also, in case we're doing (1 1 1{rho}1) fn {iota}4
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

#define lpHeader    lpMemHdrRes
    aplNELMRes = lpHeader->NELM;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

#define lpAPA       ((LPAPLAPA) lpMemRes)

    // Check for negative integer {times} an APA that spans 0
    if (gAllowNeg0
     && aplInteger < 0
     && (lpAPA->Off EQ 0
      || (lpAPA->Off + aplNELMRes * lpAPA->Mul) EQ 0
      || signumint (lpAPA->Off) NE signumint (lpAPA->Off + aplNELMRes * lpAPA->Mul)
        )
       )
    {
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbRes); lpMemHdrRes = NULL;

        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    } // End IF

    // Multiply the singleton's value to the result offset and multiplier
    lpAPA->Off *= aplInteger;
    lpAPA->Mul *= aplInteger;

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
} // End PrimFnDydTimesAPA_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_times.c
//***************************************************************************
