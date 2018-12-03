//***************************************************************************
//  NARS2000 -- Primitive Function -- Stile
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
#include "headers.h"


#ifndef PROTO
PRIMSPEC PrimSpecStile = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecStileStorageTypeMon,
    NULL,   // &PrimFnMonStileAPA_EM, -- Can't happen w/Stile

    // Monadic Boolean result functions
    &PrimFnMonStileBisB,
    NULL,   // &PrimFnMonStileBisI, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileBisF, -- Can't happen w/Stile

    // Monadic non-Boolean result functions (indexable)
    &PrimFnMonStileIisI,
    NULL,   // &PrimFnMonStileIisF, -- Can't happen w/Stile
    &PrimFnMonStileFisI,
    &PrimFnMonStileFisF,
    &PrimFnMonStileRisR,
    &PrimFnMonStileVisR,
    &PrimFnMonStileVisV,

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonStileHC2IisHC2I, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileHC2IisHC2F, -- Can't happen w/Stile
    &PrimFnMonStileHC2FisHC2I,
    &PrimFnMonStileHC2FisHC2F,
    NULL,   // &PrimFnMonStileHC2RisHC2R, -- Can't happen w/Stile
    &PrimFnMonStileHC2VisHC2R,
    &PrimFnMonStileHC2VisHC2V,

    NULL,   // &PrimFnMonStileHC4IisHC4I, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileHC4IisHC4F, -- Can't happen w/Stile
    &PrimFnMonStileHC4FisHC4I,
    &PrimFnMonStileHC4FisHC4F,
    NULL,   // &PrimFnMonStileHC4RisHC4R, -- Can't happen w/Stile
    &PrimFnMonStileHC4VisHC4R,
    &PrimFnMonStileHC4VisHC4V,

    NULL,   // &PrimFnMonStileHC8IisHC8I, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileHC8IisHC8F, -- Can't happen w/Stile
    &PrimFnMonStileHC8FisHC8I,
    &PrimFnMonStileHC8FisHC8F,
    NULL,   // &PrimFnMonStileHC8RisHC8R, -- Can't happen w/Stile
    &PrimFnMonStileHC8VisHC8R,
    &PrimFnMonStileHC8VisHC8V,

    // Monadic FLT/VFP result HC arg functions (indexable)
    &PrimFnMonStileFisHC2I,
    &PrimFnMonStileFisHC2F,
    &PrimFnMonStileVisHC2R,
    &PrimFnMonStileVisHC2V,
    &PrimFnMonStileFisHC4I,
    &PrimFnMonStileFisHC4F,
    &PrimFnMonStileVisHC4R,
    &PrimFnMonStileVisHC4V,
    &PrimFnMonStileFisHC8I,
    &PrimFnMonStileFisHC8F,
    &PrimFnMonStileVisHC8R,
    &PrimFnMonStileVisHC8V,

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecStileStorageTypeDyd,
    NULL,   // &PrimFnDydStileAPA_EM, -- Can't happen w/Stile

    // Dyadic Boolean result functions
    &PrimFnDydLeftCaretBisBvB,
    NULL,   // &PrimFnDydStileBisIvI, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisFvF, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisCvC, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisHvH, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisRvR, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisVvV, -- Can't happen w/Stile

    NULL,   // &PrimFnDydStileBisHC2IvHC2I, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisHC2FvHC2F, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisHC2RvHC2R, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisHC2VvHC2V, -- Can't happen w/Stile

    NULL,   // &PrimFnDydStileBisHC4IvHC4I, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisHC4FvHC4F, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisHC4RvHC4R, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisHC4VvHC4V, -- Can't happen w/Stile

    NULL,   // &PrimFnDydStileBisHC8IvHC8I, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisHC8FvHC8F, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisHC8RvHC8R, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileBisHC8VvHC8V, -- Can't happen w/Stile

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydStileIisIvI,
    NULL,   // &PrimFnDydStileIisFvF, -- Can't happen w/Stile
    &PrimFnDydStileFisIvI,
    &PrimFnDydStileFisFvF,
    &PrimFnDydStileRisRvR,
    &PrimFnDydStileVisRvR,
    &PrimFnDydStileVisVvV,

    &PrimFnDydStileHC2IisHC2IvHC2I,
    NULL,   // &PrimFnDydStileHC2IisHC2FvHC2F, -- Can't happen w/Stile
    &PrimFnDydStileHC2FisHC2IvHC2I,
    &PrimFnDydStileHC2FisHC2FvHC2F,
    &PrimFnDydStileHC2RisHC2RvHC2R,
    &PrimFnDydStileHC2VisHC2RvHC2R,
    &PrimFnDydStileHC2VisHC2VvHC2V,

    &PrimFnDydStileHC4IisHC4IvHC4I,
    NULL,   // &PrimFnDydStileHC4IisHC4FvHC4F, -- Can't happen w/Stile
    &PrimFnDydStileHC4FisHC4IvHC4I,
    &PrimFnDydStileHC4FisHC4FvHC4F,
    &PrimFnDydStileHC4RisHC4RvHC4R,
    &PrimFnDydStileHC4VisHC4RvHC4R,
    &PrimFnDydStileHC4VisHC4VvHC4V,

    NULL,   // &PrimFnDydStileHC8IisHC8IvHC8I, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileHC8IisHC8FvHC8F, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileHC8FisHC8IvHC8I, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileHC8FisHC8FvHC8F, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileHC8RisHC8RvHC8R, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileHC8VisHC8RvHC8R, -- Can't happen w/Stile
    NULL,   // &PrimFnDydStileHC8VisHC8VvHC8V, -- Can't happen w/Stile

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonStileB64isB64, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileB32isB32, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileB16isB16, -- Can't happen w/Stile
    NULL,   // &PrimFnMonStileB08isB08, -- Can't happen w/Stile

    // Dyadic Boolean chunk functions
    &PrimFnDydLeftCaretB64isB64vB64,
    &PrimFnDydLeftCaretB32isB32vB32,
    &PrimFnDydLeftCaretB16isB16vB16,
    &PrimFnDydLeftCaretB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecStile};
#endif


//***************************************************************************
//  $PrimFnStile_EM_YY
//
//  Primitive function for monadic and dyadic Stile ("absolute value" and "modulus")
//***************************************************************************

LPPL_YYSTYPE PrimFnStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharStile (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnStile_EM_YY


//***************************************************************************
//  $PrimSpecStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStileStorageTypeMon
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
        case ARRAY_RAT:
        case ARRAY_VFP:
        case ARRAY_NESTED:
            break;

        case ARRAY_APA:
            aplTypeRes = ARRAY_INT;

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
            aplTypeRes = ARRAY_ERROR;

            break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            aplTypeRes = ARRAY_FLOAT;

            break;

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            aplTypeRes = ARRAY_VFP;

            break;

        defstop
            break;
    } // End SWITCH

    return aplTypeRes;
} // End PrimSpecStileStorageTypeMon


//***************************************************************************
//  $PrimFnMonStileBisB
//
//  Primitive scalar function monadic Stile:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonStileBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonStileBisB


//***************************************************************************
//  $MagHC1I
//***************************************************************************

APLHC1I MagHC1I
    (APLHC1I aplRht)                    // Right arg

{
    // Check for |MININT64
    if (aplRht EQ MININT64)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);

    return abs64 (aplRht);
} // End MagHC1I


//***************************************************************************
//  $PrimFnMonStileIisI
//
//  Primitive scalar function monadic Stile:  I {is} fn I
//***************************************************************************

void PrimFnMonStileIisI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC1I (lpatRht->aplInteger);
} // End PrimFnMonStileIisI


//***************************************************************************
//  $PrimFnMonStileFisI
//
//  Primitive scalar function monadic Stile:  F {is} fn I
//***************************************************************************

void PrimFnMonStileFisI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = fabs ((APLFLOAT) lpatRht->aplInteger);
} // End PrimFnMonStileFisI


//***************************************************************************
//  $PrimFnMonStileFisF
//
//  Primitive scalar function monadic Stile:  F {is} fn F
//***************************************************************************

void PrimFnMonStileFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = fabs (lpatRht->aplFloat);
} // End PrimFnMonStileFisF


//***************************************************************************
//  $PrimFnMonStileRisR
//
//  Primitive scalar function monadic Stile:  R {is} fn R
//***************************************************************************

void PrimFnMonStileRisR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Initialize the result to 0/1
    mpq_init (&lpMemRes[uRes]);

    // Calculate the absolute value of a Rational
    mpq_abs (&lpMemRes[uRes], &lpatRht->aplRat);
} // End PrimFnMonStileRisR


//***************************************************************************
//  $MagVfp
//***************************************************************************

APLVFP MagVfp
    (APLVFP  aplRht)                    // Right arg

{
    APLVFP aplRes = {0};

    // No exceptions in this code

    // Initialize to 0
    mpfr_init0 (&aplRes);

    // Calculate the absolute value of a Variable FP
    mpfr_abs (&aplRes, &aplRht, MPFR_RNDN);

    return aplRes;
} // End MagVfp


//***************************************************************************
//  $PrimFnMonStileVisR
//
//  Primitive scalar function monadic Stile:  V {is} fn R
//***************************************************************************

void PrimFnMonStileVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    // Calculate the absolute value of a Variable FP
    lpMemRes[uRes] = MagVfp (atRht.aplVfp);

    Myhc1v_clear (&atRht.aplHC1V);
} // End PrimFnMonStileVisR


//***************************************************************************
//  $PrimFnMonStileVisV
//
//  Primitive scalar function monadic Stile:  V {is} fn V
//***************************************************************************

void PrimFnMonStileVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Calculate the absolute value of a Variable FP
    lpMemRes[uRes] = MagVfp (lpatRht->aplVfp);
} // End PrimFnMonStileVisV


//***************************************************************************
//  $MagHC2I
//***************************************************************************

APLFLOAT MagHC2I
    (APLHC2I aplRht)                    // Right arg

{
    APLFLOAT aplRes;

    // Sum the squares
    aplRes = (APLFLOAT) SqNrmHC2I (&aplRht);

    // Get the magnitude of the arg
    return sqrt (aplRes);
} // End MagHC2I


//***************************************************************************
//  $PrimFnMonStileHC2FisHC2I
//
//  Primitive scalar function monadic Stile:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonStileHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction, setting real part only
    lpMemRes[uRes].parts[0] = MagHC2I (lpatRht->aplHC2I);
} // End PrimFnMonStileHC2FisHC2I


//***************************************************************************
//  $MagHC2F
//***************************************************************************

APLFLOAT MagHC2F
    (APLHC2F aplRht)                    // Right arg

{
    APLFLOAT aplRes;

    // No exceptions in this code

    // Sum the squares
    aplRes = SqNrmHC2F (&aplRht);

    // Get the magnitude of the arg
    return sqrt (aplRes);
} // End MagHC2F


//***************************************************************************
//  $PrimFnMonStileHC2FisHC2F
//
//  Primitive scalar function monadic Stile:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonStileHC2FisHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction, setting real part only
    lpMemRes[uRes].parts[0] = MagHC2F (lpatRht->aplHC2F);
} // End PrimFnMonStileHC2FisHC2F


//***************************************************************************
//  $MagHC2R
//***************************************************************************

APLVFP MagHC2R
    (APLHC2R aplRht)                    // Right arg

{
    APLRAT aplTmp;
    APLVFP aplRes = {0};

    // Sum the squares
    aplTmp = SqNrmHC2R (&aplRht);

    // Convert the Squared Norm to VFP
    mpfr_init_set_q (&aplRes, &aplTmp, MPFR_RNDN);

    // We no longer need this storage
    mpq_clear (&aplTmp);

    // Get the magnitude of the arg
    mpfr_sqrt (&aplRes, &aplRes, MPFR_RNDN);

    return aplRes;
} // End MagHC2R


//***************************************************************************
//  $MagHC2V
//***************************************************************************

APLVFP MagHC2V
    (APLHC2V aplRht)                    // Right arg

{
    APLVFP aplRes;

    // Sum the squares
    aplRes = SqNrmHC2V (&aplRht);

    // Get the magnitude of the arg
    mpfr_sqrt (&aplRes, &aplRes, MPFR_RNDN);

    return aplRes;
} // End MagHC2V


//***************************************************************************
//  $PrimFnMonStileHC2VisHC2R
//
//  Primitive scalar function monadic Stile:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonStileHC2VisHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};
    int      i;

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplHC2R, 0, &atRht);

    // Calculate the absolute value of a HC2V
    lpMemRes[uRes].parts[0] = MagHC2V (atRht.aplHC2V);

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Initialize to 0
        mpfr_init0 (&lpMemRes[uRes].parts[i]);

    Myhc2v_clear (&atRht.aplHC2V);
} // End PrimFnMonStileHC2VisHC2R


//***************************************************************************
//  $PrimFnMonStileHC2VisHC2V
//
//  Primitive scalar function monadic Stile:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonStileHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLVFP aplMag;

    // No exceptions in this code

    // Initialize to 0
    mphc2v_init0 (&lpMemRes[uRes]);

    // Calculate magnitude
    aplMag = MagHC2V (lpatRht->aplHC2V);

    // Call subfunction, setting real part only
    mpfr_set (&lpMemRes[uRes].parts[0], &aplMag, MPFR_RNDN);
} // End PrimFnMonStileHC2VisHC2V


//***************************************************************************
//  $MagHC4I
//***************************************************************************

APLFLOAT MagHC4I
    (APLHC4I aplRht)                    // Right arg

{
    APLFLOAT aplRes;

    // Sum the squares
    aplRes = (APLFLOAT) SqNrmHC4I (&aplRht);

    // Get the magnitude of the arg
    return sqrt (aplRes);
} // End MagHC4I


//***************************************************************************
//  $PrimFnMonStileHC4FisHC4I
//
//  Primitive scalar function monadic Stile:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonStileHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction, setting real part only
    lpMemRes[uRes].parts[0] = MagHC4I (lpatRht->aplHC4I);
} // End PrimFnMonStileHC4FisHC4I


//***************************************************************************
//  $MagHC4F
//***************************************************************************

APLFLOAT MagHC4F
    (APLHC4F aplRht)                    // Right arg

{
    APLFLOAT aplRes;

    // No exceptions in this code

    // Sum the squares
    aplRes = SqNrmHC4F (&aplRht);

    // Get the magnitude of the arg
    return sqrt (aplRes);
} // End MagHC4F


//***************************************************************************
//  $PrimFnMonStileHC4FisHC4F
//
//  Primitive scalar function monadic Stile:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonStileHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction, setting real part only
    lpMemRes[uRes].parts[0] = MagHC4F (lpatRht->aplHC4F);
} // End PrimFnMonStileHC4FisHC4F


//***************************************************************************
//  $MagHC4R
//***************************************************************************

APLVFP MagHC4R
    (APLHC4R aplRht)                    // Right arg

{
    APLRAT aplTmp;
    APLVFP aplRes = {0};

    // Sum the squares
    aplTmp = SqNrmHC4R (&aplRht);

    // Convert the Squared Norm to VFP
    mpfr_init_set_q (&aplRes, &aplTmp, MPFR_RNDN);

    // We no longer need this storage
    mpq_clear (&aplTmp);

    // Get the magnitude of the arg
    mpfr_sqrt (&aplRes, &aplRes, MPFR_RNDN);

    return aplRes;
} // End MagHC4R


//***************************************************************************
//  $MagHC4V
//***************************************************************************

APLVFP MagHC4V
    (APLHC4V aplRht)                    // Right arg

{
    APLVFP aplRes;

    // Sum the squares
    aplRes = SqNrmHC4V (&aplRht);

    // Get the magnitude of the arg
    mpfr_sqrt (&aplRes, &aplRes, MPFR_RNDN);

    return aplRes;
} // End MagHC4V


//***************************************************************************
//  $PrimFnMonStileHC4VisHC4R
//
//  Primitive scalar function monadic Stile:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonStileHC4VisHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};
    int      i;

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplHC4R, 0, &atRht);

    // Calculate the absolute value of a HC4V
    lpMemRes[uRes].parts[0] = MagHC4V (atRht.aplHC4V);

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Initialize to 0
        mpfr_init0 (&lpMemRes[uRes].parts[i]);

    Myhc4v_clear (&atRht.aplHC4V);
} // End PrimFnMonStileHC4VisHC4R


//***************************************************************************
//  $PrimFnMonStileHC4VisHC4V
//
//  Primitive scalar function monadic Stile:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonStileHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLVFP aplRes;

    // No exceptions in this code

    // Initialize to 0
    mphc4v_init0 (&lpMemRes[uRes]);

    // Calculate magnitude
    aplRes = MagHC4V (lpatRht->aplHC4V);

    // Call subfunction, setting real part only
    mpfr_set (&lpMemRes[uRes].parts[0], &aplRes, MPFR_RNDN);
} // End PrimFnMonStileHC4VisHC4V


//***************************************************************************
//  $MagHC8I
//***************************************************************************

APLFLOAT MagHC8I
    (APLHC8I aplRht)                    // Right arg

{
    APLFLOAT aplRes;

    // Sum the squares
    aplRes = (APLFLOAT) SqNrmHC8I (&aplRht);

    // Get the magnitude of the arg
    return sqrt (aplRes);
} // End MagHC8I


//***************************************************************************
//  $PrimFnMonStileHC8FisHC8I
//
//  Primitive scalar function monadic Stile:  HC8F {is} fn HC8I
//***************************************************************************

void PrimFnMonStileHC8FisHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction, returning real part only
    lpMemRes[uRes].parts[0] = MagHC8I (lpatRht->aplHC8I);
} // End PrimFnMonStileHC8FisHC8I


//***************************************************************************
//  $MagHC8F
//***************************************************************************

APLFLOAT MagHC8F
    (APLHC8F aplRht)                    // Right arg

{
    APLFLOAT aplRes;

    // No exceptions in this code

    // Sum the squares
    aplRes = SqNrmHC8F (&aplRht);

    // Get the magnitude of the arg
    return sqrt (aplRes);
} // End MagHC8F


//***************************************************************************
//  $PrimFnMonStileHC8FisHC8F
//
//  Primitive scalar function monadic Stile:  HC8F {is} fn HC8F
//***************************************************************************

void PrimFnMonStileHC8FisHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // No exceptions in this code

    // Call subfunction, returning real part only
    lpMemRes[uRes].parts[0] = MagHC8F (lpatRht->aplHC8F);
} // End PrimFnMonStileHC8FisHC8F


//***************************************************************************
//  $MagHC8R
//***************************************************************************

APLVFP MagHC8R
    (APLHC8R aplRht)                    // Right arg

{
    APLRAT aplTmp;
    APLVFP aplRes = {0};

    // Sum the squares
    aplTmp = SqNrmHC8R (&aplRht);

    // Convert the Squared Norm to VFP
    mpfr_init_set_q (&aplRes, &aplTmp, MPFR_RNDN);

    // We no longer need this storage
    mpq_clear (&aplTmp);

    // Get the magnitude of the arg
    mpfr_sqrt (&aplRes, &aplRes, MPFR_RNDN);

    return aplRes;
} // End MagHC8R


//***************************************************************************
//  $MagHC8V
//***************************************************************************

APLVFP MagHC8V
    (APLHC8V aplRht)                    // Right arg

{
    APLVFP aplRes;

    // Sum the squares
    aplRes = SqNrmHC8V (&aplRht);

    // Get the magnitude of the arg
    mpfr_sqrt (&aplRes, &aplRes, MPFR_RNDN);

    return aplRes;
} // End MagHC8V


//***************************************************************************
//  $PrimFnMonStileHC8VisHC8R
//
//  Primitive scalar function monadic Stile:  HC8V {is} fn HC8R
//***************************************************************************

void PrimFnMonStileHC8VisHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};
    int      i;

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatRht->aplHC8R, 0, &atRht);

    // Calculate the absolute value of a HC8V
    lpMemRes[uRes].parts[0] = MagHC8V (atRht.aplHC8V);

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Initialize to 0
        mpfr_init0 (&lpMemRes[uRes].parts[i]);

    Myhc8v_clear (&atRht.aplHC8V);
} // End PrimFnMonStileHC8VisHC8R


//***************************************************************************
//  $PrimFnMonStileHC8VisHC8V
//
//  Primitive scalar function monadic Stile:  HC8V {is} fn HC8V
//***************************************************************************

void PrimFnMonStileHC8VisHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLVFP aplRes;

    // No exceptions in this code

    // Initialize to 0
    mphc8v_init0 (&lpMemRes[uRes]);

    // Calculate magnitude
    aplRes = MagHC8V (lpatRht->aplHC8V);

    // Call subfunction, returning real part only
    mpfr_set (&lpMemRes[uRes].parts[0], &aplRes, MPFR_RNDN);
} // End PrimFnMonStileHC8VisHC8V


//***************************************************************************
//  $PrimFnMonStileFisHC2I
//
//  Primitive scalar function monadic Stile:  F {is} fn HC2I
//***************************************************************************

void PrimFnMonStileFisHC2I
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC2I (lpatRht->aplHC2I);
} // End PrimFnMonStileFisHC2I


//***************************************************************************
//  $PrimFnMonStileFisHC4I
//
//  Primitive scalar function monadic Stile:  F {is} fn HC4I
//***************************************************************************

void PrimFnMonStileFisHC4I
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC4I (lpatRht->aplHC4I);
} // End PrimFnMonStileFisHC4I


//***************************************************************************
//  $PrimFnMonStileFisHC8I
//
//  Primitive scalar function monadic Stile:  F {is} fn HC8I
//***************************************************************************

void PrimFnMonStileFisHC8I
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC8I (lpatRht->aplHC8I);
} // End PrimFnMonStileFisHC8I


//***************************************************************************
//  $PrimFnMonStileFisHC2F
//
//  Primitive scalar function monadic Stile:  F {is} fn HC2F
//***************************************************************************

void PrimFnMonStileFisHC2F
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC2F (lpatRht->aplHC2F);
} // End PrimFnMonStileFisHC2F


//***************************************************************************
//  $PrimFnMonStileFisHC4F
//
//  Primitive scalar function monadic Stile:  F {is} fn HC4F
//***************************************************************************

void PrimFnMonStileFisHC4F
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC4F (lpatRht->aplHC4F);
} // End PrimFnMonStileFisHC4F


//***************************************************************************
//  $PrimFnMonStileFisHC8F
//
//  Primitive scalar function monadic Stile:  F {is} fn HC8F
//***************************************************************************

void PrimFnMonStileFisHC8F
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC8F (lpatRht->aplHC8F);
} // End PrimFnMonStileFisHC8F


//***************************************************************************
//  $PrimFnMonStileVisHC2R
//
//  Primitive scalar function monadic Stile:  V {is} fn HC2R
//***************************************************************************

void PrimFnMonStileVisHC2R
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC2R (lpatRht->aplHC2R);
} // End PrimFnMonStileVisHC2R


//***************************************************************************
//  $PrimFnMonStileVisHC4R
//
//  Primitive scalar function monadic Stile:  V {is} fn HC4R
//***************************************************************************

void PrimFnMonStileVisHC4R
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC4R (lpatRht->aplHC4R);
} // End PrimFnMonStileVisHC4R


//***************************************************************************
//  $PrimFnMonStileVisHC8R
//
//  Primitive scalar function monadic Stile:  V {is} fn HC8R
//***************************************************************************

void PrimFnMonStileVisHC8R
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC8R (lpatRht->aplHC8R);
} // End PrimFnMonStileVisHC8R


//***************************************************************************
//  $PrimFnMonStileVisHC2V
//
//  Primitive scalar function monadic Stile:  V {is} fn HC2V
//***************************************************************************

void PrimFnMonStileVisHC2V
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC2V (lpatRht->aplHC2V);
} // End PrimFnMonStileVisHC2V


//***************************************************************************
//  $PrimFnMonStileVisHC4V
//
//  Primitive scalar function monadic Stile:  V {is} fn HC4V
//***************************************************************************

void PrimFnMonStileVisHC4V
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC4V (lpatRht->aplHC4V);
} // End PrimFnMonStileVisHC4V


//***************************************************************************
//  $PrimFnMonStileVisHC8V
//
//  Primitive scalar function monadic Stile:  V {is} fn HC8V
//***************************************************************************

void PrimFnMonStileVisHC8V
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = MagHC8V (lpatRht->aplHC8V);
} // End PrimFnMonStileVisHC8V


//***************************************************************************
//  $PrimSpecStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecStileStorageTypeDyd
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
    else
    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Calculate the storage type of the result
    aplTypeRes = StorageType (*lpaplTypeLft, lptkFunc, *lpaplTypeRht);

    // Weed out numbers whose Floor does not have Fractionality
    //   such as Character, Nested, Hetero, Octonions, and Quaternions w/o Hurwitz
    if (!IsNested (aplTypeRes)
     && !HasFractionality (aplTypeRes))
        // Tell the caller to demote the data to HC dimension 1
        return ARRAY_REALONLY;

    return aplTypeRes;
} // End PrimSpecStileStorageTypeDyd


//***************************************************************************
//  $ModHC1I
//***************************************************************************

APLHC1I ModHC1I
    (APLHC1I aplLft,                // Left arg (modulus)
     APLHC1I aplRht)                // Right arg (argument)

{
    APLINT aplRes,
           aplLft2,
           aplRht2;

    // Handle zero modulus or argument
    if (aplLft EQ 0
     || aplRht EQ 0)
        // AplRes = the right arg with the appropriate sign
        aplRes = aplRht;
    else
    {
        // Ensure both arguments are non-negative
        aplLft2 = MagHC1I (aplLft);
        aplRht2 = MagHC1I (aplRht);

        aplRes = aplRht2 % aplLft2;

        // If the arguments are of opposite sign
        //   and the result so far is non-zero,
        //   replace the result with its complement
        //   in the modulus.
        if ((aplLft > 0) NE (aplRht > 0)
         && aplRes NE 0)
            aplRes = aplLft2 - aplRes;
    } // End IF/ELSE

    // The sign of the result is the sign of the left arg
    if (aplLft < 0)
    {
        // If zero and -0 is allowed, ...
        if (aplRes EQ 0 && gAllowNeg0)
            RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
        else
            aplRes = -aplRes;
    } // End IF

    return aplRes;
} // End ModHC1I


//***************************************************************************
//  $PrimFnDydStileIisIvI
//
//  Primitive scalar function dyadic Stile:  I {is} I fn I
//***************************************************************************

void PrimFnDydStileIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC1I (lpatLft->aplInteger, lpatRht->aplInteger);
} // End PrimFnDydStileIisIvI


//***************************************************************************
//  $PrimFnDydStileFisIvI
//
//  Primitive scalar function dyadic Stile:  F {is} I fn I
//***************************************************************************

void PrimFnDydStileFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLFLOAT aplTmp,
             aplLft,
             aplRht;

    // Handle zero modulus or argument
    if (lpatLft->aplInteger EQ 0
     || lpatRht->aplInteger EQ 0)
        // LpMemRes[uRes] = the right arg with the appropriate sign
        aplTmp = (APLFLOAT) lpatRht->aplInteger;
    else
    {
        // Ensure both arguments are non-negative
        PrimFnMonStileFisI (&aplLft, 0, lpatLft, lpPrimSpec);
        PrimFnMonStileFisI (&aplRht, 0, lpatRht, lpPrimSpec);

        aplTmp = fmod (aplRht, aplLft);

        // If the arguments are of opposite sign
        //   and the result so far is non-zero,
        //   replace the result with its complement
        //   in the modulus.
        if ((lpatLft->aplInteger > 0) NE (lpatRht->aplInteger > 0)
         && aplTmp NE 0)
            aplTmp = aplLft - aplTmp;
    } // End IF/ELSE

    // The sign of the result is the sign of the left arg
    if (lpatLft->aplInteger < 0)
        lpMemRes[uRes] = -aplTmp;
    else
        lpMemRes[uRes] =  aplTmp;
} // End PrimFnDydStileFisIvI


//***************************************************************************
//  $ModHC1F
//***************************************************************************

APLHC1F ModHC1F
    (APLHC1F aplLft,                // Left arg (modulus)
     APLHC1F aplRht)                // Right arg (argument)

{
    APLHC1F aplRes,
            aplTmp,
            aplLft2,
            aplRht2,
            fQuadCT;            // []CT
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht,            // ...          right ...
            bNoSign = FALSE;    // TRUE iff we're to skip the ending sign-setting code

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1F, &aplRht, 0);

    // Handle zero modulus or argument
    if (aplLft EQ 0
     || aplRht EQ 0)
        // aplRes = the right arg with the appropriate sign
        aplTmp = aplRht;
    else
    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            aplRes = aplLft;
        else
            aplRes = aplRht;
        // Skip the ending sign-setting code as
        //   there is only one NaN
        bNoSign = TRUE;
    } else
    // If the right arg is negative infinity, ...
    if (IsFltNegInfinity (aplRht))
    {
        // If the left arg is positive, ...
        if (aplLft > 0)
            aplRes = TranslateQuadICIndex (aplLft,
                                           ICNDX_PosMODNi,
                                           aplRht,
                                           FALSE);
        else
        // If the left arg is negative, ...
        if (aplLft < 0)
            aplRes = TranslateQuadICIndex (aplLft,
                                           ICNDX_NegMODNi,
                                           aplRht,
                                           FALSE);
        return aplRes;
    } else
    // If the right arg is positive infinity, ...
    if (IsFltPosInfinity (aplRht))
    {
        // If the left arg is positive, ...
        if (aplLft > 0)
            aplRes = TranslateQuadICIndex (aplLft,
                                           ICNDX_PosMODPi,
                                           aplRht,
                                           FALSE);
        // If the left arg is negative, ...
        if (aplLft < 0)
            aplRes = TranslateQuadICIndex (aplLft,
                                           ICNDX_NegMODPi,
                                           aplRht,
                                           FALSE);
        return aplRes;
    } else
    // If the left arg is negative infinity, ...
    if (IsFltNegInfinity (aplLft))
    {
        // If the right arg is positive, ...
        if (aplRht > 0)
            aplRes = TranslateQuadICIndex (aplLft,
                                           ICNDX_NiMODPos,
                                           aplRht,
                                           FALSE);
        // AplRes = the right arg
        aplRes = aplRht;
    } else
    // If the left arg is positive infinity, ...
    if (IsFltNegInfinity (aplLft))
    {
        // If the right arg is negative, ...
        if (aplRht < 0)
            aplRes = TranslateQuadICIndex (aplLft,
                                           ICNDX_PiMODNeg,
                                           aplRht,
                                           FALSE);
        else
            // AplRes = the right arg
            aplRes = aplRht;
        return aplRes;
    } else
    {
        // Ensure both arguments are non-negative
        aplLft2 = fabs (aplLft);
        aplRht2 = fabs (aplRht);

        // Get the current value of []CT
        fQuadCT = GetQuadCT ();

        // Calculate right divided-by left
        aplTmp = aplRht2 / aplLft2;

        // If Rht divided-by Lft is near an integer within CT
        //   aplRes = 0.
        if (CmpSCT_F (aplTmp, floor (aplTmp), fQuadCT, EQ))
            return (gAllowNeg0 && SIGN_APLFLOAT (aplLft)) ? -0.0 : 0.0;

        if (CmpSCT_F (aplTmp, ceil  (aplTmp), fQuadCT, EQ))
            return (gAllowNeg0 && SIGN_APLFLOAT (aplLft)) ? -0.0 : 0.0;

        // Calculate the modulus
        aplTmp = fmod (aplRht2, aplLft2);

        // Due to differences in fmod and APL's mod as to how they treat signed args, ...
        // If the arguments are of opposite sign
        //   and the result so far is non-zero,
        //   replace the result with its complement
        //   in the modulus.
        if (SIGN_APLFLOAT (aplLft) NE SIGN_APLFLOAT (aplRht)
         && aplTmp NE 0)
            aplTmp = aplLft2 - aplTmp;
    } // End IF/ELSE/...

    // The sign of the result is the sign of the left arg
    if (SIGN_APLFLOAT (aplLft) && !bNoSign)
        aplRes = -aplTmp;
    else
        aplRes =  aplTmp;

    return aplRes;
} // End ModHC1F


//***************************************************************************
//  $PrimFnDydStileFisFvF
//
//  Primitive scalar function dyadic Stile:  F {is} F fn F
//***************************************************************************

void PrimFnDydStileFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC1F (lpatLft->aplFloat, lpatRht->aplFloat);
} // End PrimFnDydStileFisFvF


//***************************************************************************
//  $ModHC1R
//***************************************************************************

APLHC1R ModHC1R
    (APLHC1R aplLft,                // Left arg (modulus)
     APLHC1R aplRht)                // Right arg (argument)

{
    APLHC1R aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht,            // ...          right ...
            bNoSign = FALSE;    // TRUE iff we're to skip the ending sign-setting code

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1R, &aplRht, 0);

    // Handle zero modulus or argument
    if (IsMpq0 (&aplLft)
     || IsMpq0 (&aplRht))
        // AplRes = the right arg with the appropriate sign
        mpq_init_set (&aplRes, &aplRht);
    else
    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mpq_set (&aplRes, &aplLft);
        else
            mpq_set (&aplRes, &aplRht);
        // Skip the ending sign-setting code as
        //   there is only one NaN
        bNoSign = TRUE;
    } else
    // If the right arg is negative infinity, ...
    if (IsMpqNegInfinity (&aplRht))
    {
        // If the left arg is positive, ...
        if (!SIGN_APLRAT (&aplLft))
            mpq_QuadICValue (&aplLft,
                              ICNDX_PosMODNi,
                             &aplRht,
                             &aplRes,
                              FALSE);
        // If the left arg is negative, ...
        if (SIGN_APLRAT (&aplLft))
            mpq_QuadICValue (&aplLft,
                              ICNDX_NegMODNi,
                             &aplRht,
                             &aplRes,
                              FALSE);
        return aplRes;
    } else
    // If the right arg is positive infinity, ...
    if (IsMpqPosInfinity (&aplRht))
    {
        // If the left arg is positive, ...
        if (!SIGN_APLRAT (&aplLft))
            mpq_QuadICValue (&aplLft,
                              ICNDX_PosMODPi,
                             &aplRht,
                             &aplRes,
                              FALSE);
        // If the left arg is negative, ...
        if (SIGN_APLRAT (&aplLft))
            mpq_QuadICValue (&aplLft,
                              ICNDX_NegMODPi,
                             &aplRht,
                             &aplRes,
                              FALSE);
        return aplRes;
    } else
    // If the left arg is negative infinity, ...
    if (IsMpqNegInfinity (&aplLft))
    {
        // If the right arg is positive, ...
        if (!SIGN_APLRAT (&aplRht))
        {
            mpq_QuadICValue (&aplLft,
                              ICNDX_NiMODPos,
                             &aplRht,
                             &aplRes,
                              FALSE);
            return aplRes;
        } // End IF

        // Initialize the result with the right arg
        mpq_init_set (&aplRes, &aplRht);
    } else
    // If the left arg is positive infinity, ...
    if (IsMpqPosInfinity (&aplLft))
    {
        // If the right arg is negative, ...
        if (SIGN_APLRAT (&aplRht))
        {
            mpq_QuadICValue (&aplLft,
                              ICNDX_PiMODNeg,
                             &aplRht,
                             &aplRes,
                              FALSE);
            return aplRes;
        } // End IF

        // Initialize the result with the right arg
        mpq_init_set (&aplRes, &aplRht);
    } else
    {
        // Initialize the result to 0/1
        mpq_init (&aplRes);

        // Calculate the residue
        mpq_mod (&aplRes, &aplRht, &aplLft);

        // Skip the ending sign-setting code
        //   if the result is non-zero
        bNoSign = !IsMpq0 (&aplRes);
    } // End IF/ELSE/...

    // The sign of the result is the sign of the left arg
    if (SIGN_APLRAT (&aplLft) && !bNoSign)
    {
        // If zero and -0 is allowed, ...
        if (IsMpq0 (&aplRes) && gAllowNeg0)
            RaiseException (EXCEPTION_RESULT_VFP, 0, 0, NULL);
        else
            mpq_neg (&aplRes, &aplRes);
    } // End IF

    return aplRes;
} // End ModHC1R


//***************************************************************************
//  $PrimFnDydStileRisRvR
//
//  Primitive scalar function dyadic Stile:  R {is} R fn R
//***************************************************************************

void PrimFnDydStileRisRvR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC1R (lpatLft->aplHC1R, lpatRht->aplHC1R);
} // End PrimFnDydStileRisRvR


//***************************************************************************
//  $ModHC1V
//***************************************************************************

APLHC1V ModHC1V
    (APLHC1V aplLft,                // Left arg (modulus)
     APLHC1V aplRht)                // Right arg (argument)

{
    APLHC1V aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht,            // ...          right ...
            bNoSign = FALSE;    // TRUE iff we're to skip the ending sign-setting code

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC1V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC1V, &aplRht, 0);

    // Handle zero modulus or argument
    if (IsMpf0 (&aplLft)
     || IsMpf0 (&aplRht))
        // AplRes = the right arg with the appropriate sign
        mpfr_init_copy (&aplRes, &aplRht);
    else
    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mpfr_set (&aplRes, &aplLft, MPFR_RNDN);
        else
            mpfr_set (&aplRes, &aplRht, MPFR_RNDN);
        // Skip the ending sign-setting code as
        //   there is only one NaN
        bNoSign = TRUE;
    } else
    // If the right arg is negative infinity, ...
    if (IsMpfNegInfinity (&aplRht))
    {
        // If the left arg is positive, ...
        if (mpfr_sgn (&aplLft) > 0)
            mpfr_QuadICValue (&aplLft,
                               ICNDX_PosMODNi,
                              &aplRht,
                              &aplRes,
                               FALSE);
        // If the left arg is negative, ...
        if (mpfr_sgn (&aplLft) < 0)
            mpfr_QuadICValue (&aplLft,
                               ICNDX_NegMODNi,
                              &aplRht,
                              &aplRes,
                               FALSE);
        return aplRes;
    } else
    // If the right arg is positive infinity, ...
    if (IsMpfPosInfinity (&aplRht))
    {
        // If the left arg is positive, ...
        if (mpfr_sgn (&aplLft) > 0)
            mpfr_QuadICValue (&aplLft,
                               ICNDX_PosMODPi,
                              &aplRht,
                              &aplRes,
                               FALSE);
        // If the left arg is negative, ...
        if (mpfr_sgn (&aplLft) < 0)
            mpfr_QuadICValue (&aplLft,
                               ICNDX_NegMODPi,
                              &aplRht,
                              &aplRes,
                               FALSE);
        return aplRes;
    } else
    // If the left arg is negative infinity, ...
    if (IsMpfNegInfinity (&aplLft))
    {
        // If the right arg is positive, ...
        if (mpfr_sgn (&aplRht) > 0)
        {
            mpfr_QuadICValue (&aplLft,
                               ICNDX_NiMODPos,
                              &aplRht,
                              &aplRes,
                               FALSE);
            return aplRes;
        } // End IF

        // Initialize the result with the right arg
        mpfr_init_copy (&aplRes, &aplRht);
    } else
    // If the left arg is positive infinity, ...
    if (IsMpfPosInfinity (&aplLft))
    {
        // If the right arg is negative, ...
        if (mpfr_sgn (&aplRht) < 0)
        {
            mpfr_QuadICValue (&aplLft,
                               ICNDX_PiMODNeg,
                              &aplRht,
                              &aplRes,
                               FALSE);
            return aplRes;
        } // End IF

        // Initialize the result with the right arg
        mpfr_init_copy (&aplRes, &aplRht);
    } else
    {
        APLHC1V  aplFlr  = {0},
                 aplCel  = {0},
                 aplTmp  = {0};
        APLFLOAT fQuadCT = GetQuadCT ();

        // Initialize the result to 0
        mpfr_init0 (&aplRes);
        mpfr_init0 (&aplFlr);
        mpfr_init0 (&aplCel);
        mpfr_init0 (&aplTmp);
        mpfr_init0 (&aplFlr);
        mpfr_init0 (&aplCel);
        mpfr_init0 (&aplTmp);

        // Ensure both arguments are non-negative
        mpfr_abs (&aplFlr, &aplLft, MPFR_RNDN);
        mpfr_abs (&aplCel, &aplRht, MPFR_RNDN);

        // Calculate right divided-by left
        mpfr_div (&aplTmp, &aplCel, &aplFlr, MPFR_RNDN);

        // If Rht divided-by Lft is near an integer within CT
        //   return 0.
        mpfr_floor (&aplFlr, &aplTmp);
        mpfr_ceil  (&aplCel, &aplTmp);
        if (CmpCT_V (aplTmp, aplFlr, fQuadCT, NE)
         && CmpCT_V (aplTmp, aplCel, fQuadCT, NE))
            // Calculate the residue
            mpfr_mod (&aplRes, &aplRht, &aplLft);

        Myhc1v_clear (&aplTmp);
        Myhc1v_clear (&aplCel);
        Myhc1v_clear (&aplFlr);

        // Skip the ending sign-setting code
        //   if the result is non-zero
        bNoSign = !IsMpf0 (&aplRes);
    } // End IF/ELSE/...

    // The sign of the result is the sign of the left arg
    if (SIGN_APLVFP (&aplLft) && !bNoSign)
    {
        // If non-zero or -0 is allowed, ...
        if (!IsMpf0 (&aplRes) || gAllowNeg0)
            mpfr_neg (&aplRes, &aplRes, MPFR_RNDN);
    } // End IF

    return aplRes;
} // End ModHC1V


//***************************************************************************
//  $PrimFnDydStileVisRvR
//
//  Primitive scalar function dyadic Stile:  V {is} R fn R
//***************************************************************************

void PrimFnDydStileVisRvR
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

    PrimFnDydStileVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydStileVisRvR


//***************************************************************************
//  $PrimFnDydStileVisVvV
//
//  Primitive scalar function dyadic Stile:  V {is} V fn V
//***************************************************************************

void PrimFnDydStileVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC1V (lpatLft->aplHC1V, lpatRht->aplHC1V);
} // End PrimFnDydStileVisVvV


//***************************************************************************
//  $ModHC2I
//***************************************************************************

APLHC2I ModHC2I
    (APLHC2I aplLft,                // Left arg (modulus)
     APLHC2I aplRht)                // Right arg (argument)

{
    APLHC2I aplRes;

    // Using the algorithm in Doug Forkes paper
    if (IsZeroHCxI (&aplLft, 2))
        aplRes = aplRht;
    else
    if (IsOneHCxI  (&aplLft, 2))
        aplRes = fpciZero;
    else
    {
        int     i;
        APLHC2F aplFltL,
                aplFltR,
                aplHCFlr,
                aplTmp;

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
        {
            // Convert the left and right args to FLT
            aplFltL.parts[i] = (APLFLOAT) aplLft.parts[i];
            aplFltR.parts[i] = (APLFLOAT) aplRht.parts[i];
        } // End FOR

        // Calculate Rht / Lft
        aplTmp = DivHC2F_RE (aplFltR, aplFltL);

        // Calculate Floor (Rht / Lft)
        aplHCFlr = FloorHC2F (aplTmp);

        // Calculate aplTmp EQ Floor (aplTmp)
        if (EqualHCxFvHCxF ((LPALLTYPES) &aplTmp, (LPALLTYPES) &aplHCFlr, 2, GetQuadCT (), WFCN))
            // Set aplTmp to Zero
            ZeroMemory (&aplTmp, sizeof (aplTmp));
        else
            // Calculate 1 | Rht / Lft
            // a.k.a. (Rht / Lft) - floor (Rht / Lft)
            aplTmp = SubHC2F_RE (aplTmp, aplHCFlr);

        // Calculate Lft * 1 | Rht / Lft
        // Note that because Complex numbers are commutative,
        //   we don't need to be sensitive to []LR in the
        //   following multiplication
        aplTmp = MulHC2F_RE (aplFltL, aplTmp);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
        {
            UBOOL bRet;

            // Convert back to HC2I
            aplRes.parts[i] =
              ConvertToInteger_SCT (ARRAY_FLOAT,
                                   &aplTmp.parts[i],
                                    0,
                                   &bRet);
            Assert (bRet);
            if (!bRet)
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
        } // End FOR
    } // End IF/ELSE/...

    return aplRes;
} // End ModHC2I


//***************************************************************************
//  $ModHC2F
//***************************************************************************

APLHC2F ModHC2F
    (APLHC2F aplLft,                // Left arg (modulus)
     APLHC2F aplRht)                // Right arg (argument)

{
    APLHC2F aplRes,
            aplHCFlr,
            aplTmp;
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC2F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC2F, &aplRht, 0);

    // Using the algorithm in Doug Forkes paper
    if (IsZeroHCxF (&aplLft, 2))
        aplRes = aplRht;
    else
    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            aplRes = aplLft;
        else
            aplRes = aplRht;
    } else
    if (IsOneHCxF  (&aplLft, 2))
        aplRes = SubHC2F_RE (aplRht, FloorHC2F (aplRht));
    else
    {
        // Calculate Rht / Lft
        aplTmp = DivHC2F_RE (aplRht, aplLft);

        // Calculate Floor (Rht / Lft)
        aplHCFlr = FloorHC2F (aplTmp);

        // Calculate aplTmp EQ Floor (aplTmp)
        if (EqualHCxFvHCxF ((LPALLTYPES) &aplTmp, (LPALLTYPES) &aplHCFlr, 2, GetQuadCT (), WFCN))
            // Set aplTmp to Zero
            ZeroMemory (&aplTmp, sizeof (aplTmp));
        else
            // Calculate 1 | Rht / Lft
            // a.k.a. (Rht / Lft) - floor (Rht / Lft)
            aplTmp = SubHC2F_RE (aplTmp, aplHCFlr);

        // Calculate Lft * 1 | Rht / Lft
        // Note that because Complex numbers are commutative,
        //   we don't need to be sensitive to []LR in the
        //   following multiplication
        aplRes = MulHC2F_RE (aplLft, aplTmp);
    } // End IF/ELSE/...

    return aplRes;
} // End ModHC2F


//***************************************************************************
//  $PrimFnDydStileHC2IisHC2IvHC2I
//
//  Primitive scalar function dyadic Stile:  HC2I {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydStileHC2IisHC2IvHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC2I (lpatLft->aplHC2I, lpatRht->aplHC2I);
} // End PrimFnDydStileHC2IisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydStileHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic Stile:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydStileHC2FisHC2IvHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int     i;
    APLHC2F aplLft,
            aplRht;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Convert the integer parts to float
        aplLft.parts[i] = (APLFLOAT) lpatLft->aplHC2I.parts[i];
        aplRht.parts[i] = (APLFLOAT) lpatRht->aplHC2I.parts[i];
    } // End FOR

    // Save in the result
    lpMemRes[uRes] = ModHC2F (aplLft, aplRht);
} // End PrimFnDydStileHC2FisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydStileHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic Stile:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydStileHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC2F (lpatLft->aplHC2F, lpatRht->aplHC2F);
} // End PrimFnDydStileHC2FisHC2FvHC2F


//***************************************************************************
//  $ModHC2R
//***************************************************************************

APLHC2R ModHC2R
    (APLHC2R aplLft,                // Left arg (modulus)
     APLHC2R aplRht)                // Right arg (argument)

{
    APLHC2R aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC2R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC2R, &aplRht, 0);

    // Using the algorithm in Doug Forkes paper
    if (IsZeroHCxR (&aplLft, 2))
        mphc2r_init_set (&aplRes, &aplRht);
    else
    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc2r_set (&aplRes, &aplLft);
        else
            mphc2r_set (&aplRes, &aplRht);
    } else
    if (IsOneHCxR  (&aplLft, 2))
        aplRes = SubHC2R_RE (aplRht, FloorHC2R (aplRht));
    else
    {
        APLHC2R aplTmp = {0},
                aplHCFlr = {0},
                aplSub = {0};

        // Calculate Rht / Lft
        aplTmp = DivHC2R_RE (aplRht, aplLft);

        // Calculate Floor (Rht / Lft)
        aplHCFlr = FloorHC2R (aplTmp);

        // Calculate aplTmp EQ Floor (aplTmp)
        if (EqualHCxRvHCxR ((LPALLTYPES) &aplTmp, (LPALLTYPES) &aplHCFlr, 2, GetQuadCT (), WFCN))
            // Set aplTmp to Zero
            mphc2r_init (&aplSub);
        else
            // Calculate 1 | Rht / Lft
            // a.k.a. (Rht / Lft) - floor (Rht / Lft)
            aplSub = SubHC2R_RE (aplTmp, aplHCFlr);

        // Calculate Lft * 1 | Rht / Lft
        // Note that because Complex numbers are commutative,
        //   we don't need to be sensitive to []LR in the
        //   following multiplication
        aplRes = MulHC2R_RE (aplLft, aplSub);

        // Free the temps
        Myhc2r_clear (&aplSub);
        Myhc2r_clear (&aplHCFlr);
        Myhc2r_clear (&aplTmp);
    } // End IF/ELSE/...

    return aplRes;
} // End ModHC2R


//***************************************************************************
//  $PrimFnDydStileHC2RisHC2RvHC2R
//
//  Primitive scalar function dyadic Stile:  HC2R {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydStileHC2RisHC2RvHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC2R (lpatLft->aplHC2R, lpatRht->aplHC2R);
} // End PrimFnDydStileHC2RisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydStileHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic Stile:  HC2V {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydStileHC2VisHC2RvHC2R
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

    PrimFnDydStileHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
    Myhc2v_clear (&atLft.aplHC2V);
} // End PrimFnDydStileHC2VisHC2RvHC2R


//***************************************************************************
//  $ModHC2V
//***************************************************************************

APLHC2V ModHC2V
    (APLHC2V aplLft,                // Left arg (modulus)
     APLHC2V aplRht)                // Right arg (argument)

{
    APLHC2V aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC2V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC2V, &aplRht, 0);

    // Using the algorithm in Doug Forkes paper
    if (IsZeroHCxV (&aplLft, 2))
        mphc2v_init_set (&aplRes, &aplRht);
    else
    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc2v_set (&aplRes, &aplLft);
        else
            mphc2v_set (&aplRes, &aplRht);
    } else
    if (IsOneHCxV  (&aplLft, 2))
        aplRes = SubHC2V_RE (aplRht, FloorHC2V (aplRht));
    else
    {
        APLHC2V aplTmp = {0},
                aplHCFlr = {0},
                aplSub = {0};

        // Calculate Rht / Lft
        aplTmp = DivHC2V_RE (aplRht, aplLft);

        // Calculate Floor (Rht / Lft)
        aplHCFlr = FloorHC2V (aplTmp);

        // Calculate aplTmp EQ Floor (aplTmp)
        if (EqualHCxVvHCxV ((LPALLTYPES) &aplTmp, (LPALLTYPES) &aplHCFlr, 2, GetQuadCT (), WFCN))
            // Set aplTmp to Zero
            mphc2v_init0 (&aplSub);
        else
            // Calculate 1 | Rht / Lft
            // a.k.a. (Rht / Lft) - floor (Rht / Lft)
            aplSub = SubHC2V_RE (aplTmp, aplHCFlr);

        // Calculate Lft * 1 | Rht / Lft
        // Note that because Complex numbers are commutative,
        //   we don't need to be sensitive to []LR in the
        //   following multiplication
        aplRes = MulHC2V_RE (aplLft, aplSub);

        // Free the temps
        Myhc2v_clear (&aplSub);
        Myhc2v_clear (&aplHCFlr);
        Myhc2v_clear (&aplTmp);
    } // End IF/ELSE/...

    return aplRes;
} // End ModHC2V


//***************************************************************************
//  $PrimFnDydStileHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic Stile:  HC2V {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydStileHC2VisHC2VvHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC2V (lpatLft->aplHC2V, lpatRht->aplHC2V);
} // End PrimFnDydStileHC2VisHC2VvHC2V


//***************************************************************************
//  $ModHC4I
//
//  Note that this function is called using Hurwitz's Floor function only
//***************************************************************************

APLHC4I ModHC4I
    (APLHC4I aplLft,                // Left arg (modulus)
     APLHC4I aplRht)                // Right arg (argument)

{
    APLHC4I aplRes;

    // Using the algorithm in Doug Forkes paper
    if (IsZeroHCxI (&aplLft, 4))
        aplRes = aplRht;
    else
    if (IsOneHCxI  (&aplLft, 4))
        aplRes = fphiZero;
    else
    {
        int     i;
        APLHC4F aplFltL,
                aplFltR,
                aplHCFlr,
                aplTmp;

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
        {
            // Convert the left and right args to FLT
            aplFltL.parts[i] = (APLFLOAT) aplLft.parts[i];
            aplFltR.parts[i] = (APLFLOAT) aplRht.parts[i];
        } // End FOR

        // Calculate Rht / Lft
        // This function is sensitive to []LR
        aplTmp = DivHC4F_RE (aplFltR, aplFltL);

        // Calculate Floor (Rht / Lft)
        aplHCFlr = FloorHC4F (aplTmp);

        // Calculate aplTmp EQ Floor (aplTmp)
        if (EqualHCxFvHCxF ((LPALLTYPES) &aplTmp, (LPALLTYPES) &aplHCFlr, 4, GetQuadCT (), WFCN))
            // Set aplTmp to Zero
            ZeroMemory (&aplTmp, sizeof (aplTmp));
        else
            // Calculate 1 | Rht / Lft
            // a.k.a. (Rht / Lft) - floor (Rht / Lft)
            aplTmp = SubHC4F_RE (aplTmp, aplHCFlr);

        if (GetQuadLR () EQ 'l')
            // Calculate Lft * 1 | Rht / Lft
            aplTmp = MulHC4F_RE (        aplFltL, aplTmp);
        else
            // Calculate      (1 | Rht / Lft) * Lft
            aplTmp = MulHC4F_RE (aplTmp, aplFltL);

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
        {
            UBOOL bRet;

            // Convert back to HC4I
            aplRes.parts[i] =
              ConvertToInteger_SCT (ARRAY_FLOAT,
                                   &aplTmp.parts[i],
                                    0,
                                   &bRet);
            Assert (bRet);
            if (!bRet)
                RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);
        } // End FOR
    } // End IF/ELSE/...

    return aplRes;
} // End ModHC4I


//***************************************************************************
//  $ModHC4F
//
//  Note that this function is called using Hurwitz's Floor function only
//***************************************************************************

APLHC4F ModHC4F
    (APLHC4F aplLft,                // Left arg (modulus)
     APLHC4F aplRht)                // Right arg (argument)

{
    APLHC4F aplRes;
    UBOOL   bNaNLft,                // TRUE iff the left arg is a NaN
            bNaNRht;                // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC4F, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC4F, &aplRht, 0);

    // Using the algorithm in Doug Forkes paper
    if (IsZeroHCxF (&aplLft, 4))
        aplRes = aplRht;
    else
    // If either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            aplRes = aplLft;
        else
            aplRes = aplRht;
    } else
    {
        APLHC4F  aplTmp,
                 aplHCFlr,
                 aplSub,
                 aplRes2;
        APLFLOAT aplMagL,
                 aplMagZ;
        APLFLOAT aplAbs,
                 aplFlr,
                 aplCel;
        int      i;                         // Loop counter

        if (IsOneHCxF  (&aplLft, 4))
            aplRes = SubHC4F_RE (aplRht, FloorHC4F (aplRht));
        else
        {
            // Calculate Rht / Lft
            // This function is sensitive to []LR
            aplTmp = DivHC4F_RE (aplRht, aplLft);

            // Calculate Floor (Rht / Lft)
            aplHCFlr = FloorHC4F (aplTmp);

            // Calculate aplTmp EQ Floor (aplTmp)
            if (EqualHCxFvHCxF ((LPALLTYPES) &aplTmp, (LPALLTYPES) &aplHCFlr, 4, GetQuadCT (), WFCN))
                // Set aplSub to Zero
                ZeroMemory (&aplSub, sizeof (aplSub));
            else
                // Calculate 1 | Rht / Lft
                // a.k.a. (Rht / Lft) - floor (Rht / Lft)
                aplSub = SubHC4F_RE (aplTmp, aplHCFlr);

            if (GetQuadLR () EQ 'l')
                // Calculate Lft * 1 | Rht / Lft
                aplRes = MulHC4F_RE (        aplLft, aplSub);
            else
                // Calculate      (1 | Rht / Lft) * Lft
                aplRes = MulHC4F_RE (aplSub, aplLft);
        } // End IF/ELSE

        //***************************************************************************
        // Check for near a Hurwitz Integer
        //***************************************************************************

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
        {
            // Get the absolute value
            aplAbs = fabs (aplRes.parts[i]);

            // Get the Floor and Ceiling
            aplFlr = floor (aplAbs);
            aplCel = ceil  (aplAbs);

            // Add/subtract 0.5 to/from Floor/Ceiling
            aplFlr += 0.5;
            aplCel -= 0.5;

            // If floor (aplAbs) is near a Hurwitz integer within SYS_CT, ...
            if (CmpSCT_F (aplAbs, aplFlr, SYS_CT, EQ))
                // Save the half integer
                aplRes2.parts[i] = aplFlr;
            else
            // If ceil  (aplAbs) is near a Hurwitz integer within SYS_CT, ...
            if (CmpSCT_F (aplAbs, aplCel, SYS_CT, EQ))
                // Save the half integer
                aplRes2.parts[i] = aplCel;
            else
                break;
        } // End IF

        // If we didn't terminate early, ...
        if (i EQ 4)
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
                // Copy aplRes2 to aplRes with the sign of aplRes
                aplRes.parts[i] = signumflt (aplRes.parts[i]) * aplRes2.parts[i];
        else
        // We did terminate early, so it's not a Hurwitz Integer
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            // If it's close to 0
            if (CmpSCT_F (aplRes.parts[i], 0, SYS_CT, EQ))
                // Call it 0
                aplRes.parts[i] = 0;
        // Calculate the magnitudes
        aplMagL = MagHC4F (aplLft);
        aplMagZ = MagHC4F (aplRes);

        // If the magnitudes are equal, ...
        if (CmpCT_F (aplMagL, aplMagZ, SYS_CT, EQ))
            // Return 0
            ZeroMemory (&aplRes, sizeof (aplRes));
        else
            Assert (CmpCT_F (aplMagL, aplMagZ, SYS_CT, >));
    } // End IF/ELSE/...

    return aplRes;
} // End ModHC4F


//***************************************************************************
//  $PrimFnDydStileHC4IisHC4IvHC4I
//
//  Primitive scalar function dyadic Stile:  HC4I {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydStileHC4IisHC4IvHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC4I (lpatLft->aplHC4I, lpatRht->aplHC4I);
} // End PrimFnDydStileHC4IisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydStileHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic Stile:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydStileHC4FisHC4IvHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int     i;
    APLHC4F aplLft,
            aplRht;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Convert the integer parts to float
        aplLft.parts[i] = (APLFLOAT) lpatLft->aplHC4I.parts[i];
        aplRht.parts[i] = (APLFLOAT) lpatRht->aplHC4I.parts[i];
    } // End FOR

    // Save in the result
    lpMemRes[uRes] = ModHC4F (aplLft, aplRht);
} // End PrimFnDydStileHC4FisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydStileHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic Stile:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydStileHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC4F (lpatLft->aplHC4F, lpatRht->aplHC4F);
} // End PrimFnDydStileHC4FisHC4FvHC4F


//***************************************************************************
//  $ModHC4R
//
//  Note that this function is called using Hurwitz's Floor function only
//***************************************************************************

APLHC4R ModHC4R
    (APLHC4R aplLft,                // Left arg (modulus)
     APLHC4R aplRht)                // Right arg (argument)

{
    APLHC4R aplRes = {0};           // The result
    UBOOL   bNaNLft,                // TRUE iff the left arg is a NaN
            bNaNRht;                // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC4R, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC4R, &aplRht, 0);

    // Using the algorithm in Doug Forkes paper
    if (IsZeroHCxR (&aplLft, 4))
        mphc4r_init_set (&aplRes, &aplRht);
    else
    // If either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc4r_set (&aplRes, &aplLft);
        else
            mphc4r_set (&aplRes, &aplRht);
    } else
    {
        APLHC4R aplTmp  = {0},
                aplHCFlr  = {0},
                aplSub  = {0},
                aplRes2 = {0};
        APLVFP  aplMagL,
                aplMagZ;
        APLRAT  aplAbs,
                aplFlr,
                aplCel;
        int     i;                      // Loop counter

        if (IsOneHCxR  (&aplLft, 4))
            aplRes = SubHC4R_RE (aplRht, FloorHC4R (aplRht));
        else
        {
            // Calculate Rht / Lft
            // This function is sensitive to []LR
            aplTmp = DivHC4R_RE (aplRht, aplLft);

            // Calculate Floor (Rht / Lft)
            aplHCFlr = FloorHC4R (aplTmp);

            // Calculate aplTmp EQ Floor (aplTmp)
            if (EqualHCxRvHCxR ((LPALLTYPES) &aplTmp, (LPALLTYPES) &aplHCFlr, 4, GetQuadCT (), WFCN))
                // Set aplSub to Zero
                mphc4r_init (&aplSub);
            else
                // Calculate 1 | Rht / Lft
                // a.k.a. (Rht / Lft) - floor (Rht / Lft)
                aplSub = SubHC4R_RE (aplTmp, aplHCFlr);

            if (GetQuadLR () EQ 'l')
                // Calculate Lft * 1 | Rht / Lft
                aplRes = MulHC4R_RE (        aplLft, aplSub);
            else
                // Calculate      (1 | Rht / Lft) * Lft
                aplRes = MulHC4R_RE (aplSub, aplLft);
        } // End IF/ELSE

        // Initialize to 0/1
        mpq_init    (&aplAbs);
        mpq_init    (&aplCel);
        mpq_init    (&aplFlr);
        mphc4r_init (&aplRes2);

        //***************************************************************************
        // Check for near a Hurwitz Integer
        //***************************************************************************

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
        {
            // Get the absolute value
            mpq_abs (&aplAbs, &aplRes.parts[i]);

            // Get the Floor and Ceiling
            mpq_floor (&aplFlr, &aplAbs);
            mpq_ceil  (&aplCel, &aplAbs);

            // Add/subtract 0.5 to/from Floor/Ceiling
            mpq_add (&aplFlr, &aplFlr, &mpqHalf);
            mpq_sub (&aplCel, &aplCel, &mpqHalf);

            // If floor (aplAbs) is near a Hurwitz integer within SYS_CT, ...
            if (CmpSCT_R (aplAbs, aplFlr, SYS_CT, EQ))
                // Save the half integer
                mpq_set (&aplRes2.parts[i], &aplFlr);
            else
            // If ceil  (aplAbs) is near a Hurwitz integer within SYS_CT, ...
            if (CmpSCT_R (aplAbs, aplCel, SYS_CT, EQ))
                // Save the half integer
                mpq_set (&aplRes2.parts[i], &aplCel);
            else
                break;
        } // End IF

        // If we didn't terminate early, ...
        if (i EQ 4)
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            {
                // Copy the sign of aplRes
                if (mpq_sgn (&aplRes.parts[i]) < 0)
                    // Negate it
                    mpq_neg (&aplRes2.parts[i], &aplRes2.parts[i]);

                // Copy aplRes2 to aplRes with the sign of aplRes
                mpq_set (&aplRes.parts[i], &aplRes2.parts[i]);
            } // End FOR
        else
        // We did terminate early, so it's not a Hurwitz Integer
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            // If it's close to 0
            if (CmpSCT_R (aplRes.parts[i], mpqZero, SYS_CT, EQ))
                // Call it 0/1
                mpq_set_ui (&aplRes.parts[i], 0, 1);
        // Calculate the magnitudes
        aplMagL = MagHC4R (aplLft);
        aplMagZ = MagHC4R (aplRes);

        // If the magnitudes are equal, ...
        if (CmpCT_V (aplMagL, aplMagZ, SYS_CT, EQ))
            // Return 0/1
            mphc4r_set (&aplRes, &mphiZero);
        else
            Assert (CmpCT_V (aplMagL, aplMagZ, SYS_CT, >));

        // Free the temps
        Myhc4r_clear (&aplRes2  );
        mpq_clear    (&aplFlr   );
        mpq_clear    (&aplCel   );
        mpq_clear    (&aplAbs   );
        Myhc4r_clear (&aplSub   );
        Myhc4r_clear (&aplHCFlr );
        Myhc4r_clear (&aplTmp   );
    } // End IF/ELSE/...

    return aplRes;
} // End ModHC4R


//***************************************************************************
//  $PrimFnDydStileHC4RisHC4RvHC4R
//
//  Primitive scalar function dyadic Stile:  HC4R {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydStileHC4RisHC4RvHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC4R (lpatLft->aplHC4R, lpatRht->aplHC4R);
} // End PrimFnDydStileHC4RisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydStileHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic Stile:  HC4V {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydStileHC4VisHC4RvHC4R
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

    PrimFnDydStileHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
    Myhc4v_clear (&atLft.aplHC4V);
} // End PrimFnDydStileHC4VisHC4RvHC4R


//***************************************************************************
//  $ModHC4V
//
//  Note that this function is called using Hurwitz's Floor function only
//***************************************************************************

APLHC4V ModHC4V
    (APLHC4V aplLft,                // Left arg (modulus)
     APLHC4V aplRht)                // Right arg (argument)

{
    APLHC4V aplRes = {0};
    UBOOL   bNaNLft,            // TRUE iff the left arg is a NaN
            bNaNRht;            // ...          right ...

    // Is either arg a NaN?
    bNaNLft = IsArgNaN (ARRAY_HC4V, &aplLft, 0);
    bNaNRht = IsArgNaN (ARRAY_HC4V, &aplRht, 0);

    // Using the algorithm in Doug Forkes paper
    if (IsZeroHCxV (&aplLft, 4))
        mphc4v_init_set (&aplRes, &aplRht);
    else
    // If the either arg is a NaN, ...
    if (bNaNLft || bNaNRht)
    {
        if (bNaNLft)
            mphc4v_set (&aplRes, &aplLft);
        else
            mphc4v_set (&aplRes, &aplRht);
    } else
    {
        APLHC4V aplTmp = {0},
                aplHCFlr = {0},
                aplSub = {0},
                aplRes2 = {0};
        APLVFP  aplMagL,
                aplMagZ;
        APLVFP  aplAbs,
                aplFlr,
                aplCel;
        int     i;                      // Loop counter

        if (IsOneHCxV  (&aplLft, 4))
            aplRes = SubHC4V_RE (aplRht, FloorHC4V (aplRht));
        else
        {
            // Calculate Rht / Lft
            // This function is sensitive to []LR
            aplTmp = DivHC4V_RE (aplRht, aplLft);

            // Calculate Floor (Rht / Lft)
            aplHCFlr = FloorHC4V (aplTmp);

            // Calculate aplTmp EQ Floor (aplTmp)
            if (EqualHCxVvHCxV ((LPALLTYPES) &aplTmp, (LPALLTYPES) &aplHCFlr, 4, GetQuadCT (), WFCN))
                // Set aplTmp to Zero
                mphc4v_init0 (&aplSub);
            else
                // Calculate 1 | Rht / Lft
                // a.k.a. (Rht / Lft) - floor (Rht / Lft)
                aplSub = SubHC4V_RE (aplTmp, aplHCFlr);

            if (GetQuadLR () EQ 'l')
                // Calculate Lft * 1 | Rht / Lft
                aplRes = MulHC4V_RE (        aplLft, aplSub);
            else
                // Calculate      (1 | Rht / Lft) * Lft
                aplRes = MulHC4V_RE (aplSub, aplLft);
        } // End IF/ELSE

        // Initialize to 0
        mpfr_init0   (&aplAbs);
        mpfr_init0   (&aplCel);
        mpfr_init0   (&aplFlr);
        mphc4v_init0 (&aplRes2);

        //***************************************************************************
        // Check for near a Hurwitz Integer
        //***************************************************************************

        // Loop through all of the parts
        for (i = 0; i < 4; i++)
        {
            // Get the absolute value
            mpfr_abs (&aplAbs, &aplRes.parts[i], MPFR_RNDN);

            // Get the floor and ceiling
            mpfr_floor (&aplFlr, &aplAbs);
            mpfr_ceil  (&aplCel, &aplAbs);

            // Add/subtract 0.5 to/from Floor/Ceiling
            mpfr_add (&aplFlr, &aplFlr, &mpfHalf, MPFR_RNDN);
            mpfr_sub (&aplCel, &aplCel, &mpfHalf, MPFR_RNDN);

            // If floor (aplTmp) is near a Hurwitz integer within SYS_CT, ...
            if (CmpSCT_V (aplAbs, aplFlr, SYS_CT, EQ))
                // Save the half integer
                mpfr_set (&aplRes2.parts[i], &aplFlr, MPFR_RNDN);
            else
            // If ceil  (aplTmp) is near a Hurwitz integer within SYS_CT, ...
            if (CmpSCT_V (aplAbs, aplCel, SYS_CT, EQ))
                // Save the half integer
                mpfr_set (&aplRes2.parts[i], &aplCel, MPFR_RNDN);
            else
                break;
        } // End IF

        // If we didn't terminate early, ...
        if (i EQ 4)
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            {
                // Save the sign of aplRes
                if (mpfr_sgn (&aplRes.parts[i]) < 0)
                    // Negate it
                    mpfr_neg (&aplRes2.parts[i], &aplRes2.parts[i], MPFR_RNDN);

                // Copy aplRes2 to aplRes with the sign of aplRes
                mpfr_set (&aplRes.parts[i], &aplRes2.parts[i], MPFR_RNDN);
            } // End FOR
        else
        // We did terminate early, so it's not a Hurwitz Integer
            // Loop through all of the parts
            for (i = 0; i < 4; i++)
            // If it's close to 0
            if (CmpSCT_V (aplRes.parts[i], mpfZero, SYS_CT, EQ))
                // Call it 0/1
                mpfr_set_ui (&aplRes.parts[i], 0, MPFR_RNDN);
        // Calculate the magnitudes
        aplMagL = MagHC4V (aplLft);
        aplMagZ = MagHC4V (aplRes);

        // If the magnitudes are equal, ...
        if (CmpCT_V (aplMagL, aplMagZ, SYS_CT, EQ))
            // Return 0/1
            mphc4v_set (&aplRes, &mphfZero);
        else
            Assert (CmpCT_V (aplMagL, aplMagZ, SYS_CT, >));

        // Free the temps
        Myhc4v_clear (&aplRes2);
        mpfr_clear   (&aplFlr);
        mpfr_clear   (&aplCel);
        mpfr_clear   (&aplAbs);
        Myhc4v_clear (&aplSub);
        Myhc4v_clear (&aplHCFlr);
        Myhc4v_clear (&aplTmp);
    } // End IF/ELSE/...

    return aplRes;
} // End ModHC4V


//***************************************************************************
//  $PrimFnDydStileHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic Stile:  HC4V {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydStileHC4VisHC4VvHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = ModHC4V (lpatLft->aplHC4V, lpatRht->aplHC4V);
} // End PrimFnDydStileHC4VisHC4VvHC4V


//***************************************************************************
//  End of File: pf_stile.c
//***************************************************************************
