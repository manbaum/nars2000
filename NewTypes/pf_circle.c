//***************************************************************************
//  NARS2000 -- Primitive Function -- Circle
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
#include <math.h>
#include "headers.h"
#include "pf_circle.mac"
#include <gsl\gsl_sys.h>

////#define USE_HYPOT

#ifndef PROTO
PRIMSPEC PrimSpecCircle = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecCircleStorageTypeMon,
    NULL,   // &PrimFnMonCircleAPA_EM, -- Can't happen w/Circle

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonCircleBisB, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleBisI, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleBisF, -- Can't happen w/Circle

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonCircleIisI, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleIisF, -- Can't happen w/Circle
    &PrimFnMonCircleFisI,
    &PrimFnMonCircleFisF,
    NULL,   // &PrimFnMonCircleRisR, -- Can't happen w/Circle
    &PrimFnMonCircleVisR,
    &PrimFnMonCircleVisV,

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonCircleHC2IisHC2I, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleHC2IisHC2F, -- Can't happen w/Circle
    &PrimFnMonCircleHC2FisHC2I,
    &PrimFnMonCircleHC2FisHC2F,
    NULL,   // &PrimFnMonCircleHC2RisHC2R, -- Can't happen w/Circle
    &PrimFnMonCircleHC2VisHC2R,
    &PrimFnMonCircleHC2VisHC2V,

    NULL,   // &PrimFnMonCircleHC4IisHC4I, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleHC4IisHC4F, -- Can't happen w/Circle
    &PrimFnMonCircleHC4FisHC4I,
    &PrimFnMonCircleHC4FisHC4F,
    NULL,   // &PrimFnMonCircleHC4RisHC4R, -- Can't happen w/Circle
    &PrimFnMonCircleHC4VisHC4R,
    &PrimFnMonCircleHC4VisHC4V,

    NULL,   // &PrimFnMonCircleHC8IisHC8I, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleHC8IisHC8F, -- Can't happen w/Circle
    &PrimFnMonCircleHC8FisHC8I,
    &PrimFnMonCircleHC8FisHC8F,
    NULL,   // &PrimFnMonCircleHC8RisHC8R, -- Can't happen w/Circle
    &PrimFnMonCircleHC8VisHC8R,
    &PrimFnMonCircleHC8VisHC8V,

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonCircleFisHC2I, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleFisHC2F, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleVisHC2R, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleVisHC2V, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleFisHC4I, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleFisHC4F, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleVisHC4R, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleVisHC4V, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleFisHC8I, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleFisHC8F, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleVisHC8R, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleVisHC8V, -- Can't happen w/Circle

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecCircleStorageTypeDyd,
    NULL,   // &PrimFnDydCircleAPA_EM, -- Can't happen w/Circle

    // Dyadic Boolean result functions
    NULL,   // &PrimFnDydCircleBisBvB, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisIvI, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisFvF, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisCvC, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisHvH, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisRvR, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleRisVvV, -- Can't happen w/Circle

    NULL,   // &PrimFnDydCircleBisHC2IvHC2I, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisHC2FvHC2F, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisHC2RvHC2R, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisHC2VvHC2V, -- Can't happen w/Circle

    NULL,   // &PrimFnDydCircleBisHC4IvHC4I, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisHC4FvHC4F, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisHC4RvHC4R, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisHC4VvHC4V, -- Can't happen w/Circle

    NULL,   // &PrimFnDydCircleBisHC8IvHC8I, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisHC8FvHC8F, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisHC8RvHC8R, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleBisHC8VvHC8V, -- Can't happen w/Circle

    // Dyadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnDydCircleIisIvI, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleIisFvF, -- Can't happen w/Circle
    &PrimFnDydCircleFisIvI,
    &PrimFnDydCircleFisFvF,
    NULL,   // &PrimFnDydCircleBisRvR, -- Can't happen w/Circle
    &PrimFnDydCircleVisRvR,
    &PrimFnDydCircleVisVvV,

    NULL,   // &PrimFnDydCircleHC2IisHC2IvHC2I, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleHC2IisHC2FvHC2F, -- Can't happen w/Circle
    &PrimFnDydCircleHC2FisHC2IvHC2I,
    &PrimFnDydCircleHC2FisHC2FvHC2F,
    NULL,   // &PrimFnDydCircleHC2RisHC2RvHC2R, -- Can't happen w/Circle
    &PrimFnDydCircleHC2VisHC2RvHC2R,
    &PrimFnDydCircleHC2VisHC2VvHC2V,

    NULL,   // &PrimFnDydCircleHC4IisHC4IvHC4I, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleHC4IisHC4FvHC4F, -- Can't happen w/Circle
    &PrimFnDydCircleHC4FisHC4IvHC4I,
    &PrimFnDydCircleHC4FisHC4FvHC4F,
    NULL,   // &PrimFnDydCircleHC4RisHC4RvHC4R, -- Can't happen w/Circle
    &PrimFnDydCircleHC4VisHC4RvHC4R,
    &PrimFnDydCircleHC4VisHC4VvHC4V,

    NULL,   // &PrimFnDydCircleHC8IisHC8IvHC8I, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleHC8IisHC8IvHC8I, -- Can't happen w/Circle
    &PrimFnDydCircleHC8FisHC8IvHC8I,
    &PrimFnDydCircleHC8FisHC8FvHC8F,
    NULL,   // &PrimFnDydCircleHC8RisHC8RvHC8R, -- Can't happen w/Circle
    &PrimFnDydCircleHC8VisHC8RvHC8R,
    &PrimFnDydCircleHC8VisHC8VvHC8V,

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonCircleB64isB64, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleB32isB32, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleB16isB16, -- Can't happen w/Circle
    NULL,   // &PrimFnMonCircleB08isB08, -- Can't happen w/Circle

    // Dyadic Boolean chunk functions
    NULL,   // &PrimFnDydCircleB64isB64vB64, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleB32isB32vB32, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleB16isB16vB16, -- Can't happen w/Circle
    NULL,   // &PrimFnDydCircleB08isB08vB08, -- Can't happen w/Circle

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    TRUE,   // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecCircle};
#endif


//***************************************************************************
//  $PrimFnCircle_EM_YY
//
//  Primitive function for monadic and dyadic Circle ("pi times" and "circle functions")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnCircle_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnCircle_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (IsAPLCharCircle (lptkFunc->tkData.tkChar));

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnCircle_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecCircleStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStorageTypeMon
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
} // End PrimSpecCircleStorageTypeMon


//***************************************************************************
//  $PrimFnMonCircleFisI
//
//  Primitive scalar function monadic Circle:  F {is} fn I
//***************************************************************************

void PrimFnMonCircleFisI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call assembler function
    iAsmMulPiInt (&lpMemRes[uRes], &lpatRht->aplInteger);
} // End PrimFnMonCircleFisI


//***************************************************************************
//  $PrimFnMonCircleFisF
//
//  Primitive scalar function monadic Circle:  F {is} fn F
//***************************************************************************

void PrimFnMonCircleFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Call assembler function
    iAsmMulPiFlt (&lpMemRes[uRes], &lpatRht->aplFloat);
} // End PrimFnMonCircleFisF


//***************************************************************************
//  $PrimFnMonCircleVisR
//
//  Primitive scalar function monadic Circle:  V {is} fn R
//***************************************************************************

void PrimFnMonCircleVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    // Initialize the result to 0
    mpfr_init0 (&lpMemRes[uRes]);

    mpfr_mul (&lpMemRes[uRes], &atRht.aplVfp, &GetMemPTD ()->mpfrPi, MPFR_RNDN);

    Myhc1v_clear (&atRht.aplHC1V);
} // End PrimFnMonCircleVisR


//***************************************************************************
//  $PrimFnMonCircleVisV
//
//  Primitive scalar function monadic Circle:  V {is} fn V
//***************************************************************************

void PrimFnMonCircleVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Initialize the result to 0
    mpfr_init0 (&lpMemRes[uRes]);

    mpfr_mul (&lpMemRes[uRes], &lpatRht->aplVfp, &GetMemPTD ()->mpfrPi, MPFR_RNDN);
} // End PrimFnMonCircleVisV


//***************************************************************************
//  $PrimFnMonCircleHC2FisHC2I
//
//  Primitive scalar function monadic Circle:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonCircleHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;                          // Loop counter

    // Loop through all the parts
    for (i = 0; i < 2; i++)
        // Call assembler function
        iAsmMulPiInt (&lpMemRes[uRes].parts[i], &lpatRht->aplHC2I.parts[i]);
} // End PrimFnMonCircleHC2FisHC2I


//***************************************************************************
//  $PrimFnMonCircleHC2FisHC2F
//
//  Primitive scalar function monadic Circle:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonCircleHC2FisHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;                          // Loop counter

    // Loop through all the parts
    for (i = 0; i < 2; i++)
        // Call assembler function
        iAsmMulPiFlt (&lpMemRes[uRes].parts[i], &lpatRht->aplHC2F.parts[i]);
} // End PrimFnMonCircleHC2FisHC2F


//***************************************************************************
//  $PrimFnMonCircleHC2VisHC2R
//
//  Primitive scalar function monadic Circle:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonCircleHC2VisHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int    i;                       // Loop counter
    APLVFP aplVfpPi = GetMemPTD ()->mpfrPi;
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplHC2R, 0, &atRht);

    // Initialize to 0
    mpcf_init0 (&lpMemRes[uRes]);

    // Loop through all the parts
    for (i = 0; i < 2; i++)
        mpfr_mul (&lpMemRes[uRes].parts[i], &atRht.aplHC2V.parts[i], &aplVfpPi, MPFR_RNDN);

    mpcf_clear (&atRht.aplHC2V);
} // End PrimFnMonCircleHC2VisHC2R


//***************************************************************************
//  $PrimFnMonCircleHC2VisHC2V
//
//  Primitive scalar function monadic Circle:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonCircleHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int    i;                       // Loop counter
    APLVFP aplVfpPi = GetMemPTD ()->mpfrPi;

    // Initialize to 0
    mpcf_init0 (&lpMemRes[uRes]);

    // Loop through all the parts
    for (i = 0; i < 2; i++)
        mpfr_mul (&lpMemRes[uRes].parts[i], &lpatRht->aplHC2V.parts[i], &aplVfpPi, MPFR_RNDN);
} // End PrimFnMonCircleHC2VisHC2V


//***************************************************************************
//  $PrimFnMonCircleHC4FisHC4I
//
//  Primitive scalar function monadic Circle:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonCircleHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;                          // Loop counter

    // Loop through all the parts
    for (i = 0; i < 4; i++)
        // Call assembler function
        iAsmMulPiInt (&lpMemRes[uRes].parts[i], &lpatRht->aplHC4I.parts[i]);
} // End PrimFnMonCircleHC4FisHC4I


//***************************************************************************
//  $PrimFnMonCircleHC4FisHC4F
//
//  Primitive scalar function monadic Circle:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonCircleHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;                          // Loop counter

    // Loop through all the parts
    for (i = 0; i < 4; i++)
        // Call assembler function
        iAsmMulPiFlt (&lpMemRes[uRes].parts[i], &lpatRht->aplHC4F.parts[i]);
} // End PrimFnMonCircleHC4FisHC4F


//***************************************************************************
//  $PrimFnMonCircleHC4VisHC4R
//
//  Primitive scalar function monadic Circle:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonCircleHC4VisHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int    i;                       // Loop counter
    APLVFP aplVfpPi = GetMemPTD ()->mpfrPi;
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplHC4R, 0, &atRht);

    // Initialize to 0
    mphf_init0 (&lpMemRes[uRes]);

    // Loop through all the parts
    for (i = 0; i < 4; i++)
        mpfr_mul (&lpMemRes[uRes].parts[i], &atRht.aplHC4V.parts[i], &aplVfpPi, MPFR_RNDN);

    mphf_clear (&atRht.aplHC4V);
} // End PrimFnMonCircleHC4VisHC4R


//***************************************************************************
//  $PrimFnMonCircleHC4VisHC4V
//
//  Primitive scalar function monadic Circle:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonCircleHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int    i;                       // Loop counter
    APLVFP aplVfpPi = GetMemPTD ()->mpfrPi;

    // Initialize to 0
    mphf_init0 (&lpMemRes[uRes]);

    // Loop through all the parts
    for (i = 0; i < 4; i++)
        mpfr_mul (&lpMemRes[uRes].parts[i], &lpatRht->aplHC4V.parts[i], &aplVfpPi, MPFR_RNDN);
} // End PrimFnMonCircleHC4VisHC4V


//***************************************************************************
//  $PrimFnMonCircleHC8FisHC8I
//
//  Primitive scalar function monadic Circle:  HC8F {is} fn HC8I
//***************************************************************************

void PrimFnMonCircleHC8FisHC8I
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;                          // Loop counter

    // Loop through all the parts
    for (i = 0; i < 8; i++)
        // Call assembler function
        iAsmMulPiInt (&lpMemRes[uRes].parts[i], &lpatRht->aplHC8I.parts[i]);
} // End PrimFnMonCircleHC8FisHC8I


//***************************************************************************
//  $PrimFnMonCircleHC8FisHC8F
//
//  Primitive scalar function monadic Circle:  HC8F {is} fn HC8F
//***************************************************************************

void PrimFnMonCircleHC8FisHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;                          // Loop counter

    // Loop through all the parts
    for (i = 0; i < 8; i++)
        // Call assembler function
        iAsmMulPiFlt (&lpMemRes[uRes].parts[i], &lpatRht->aplHC8F.parts[i]);
} // End PrimFnMonCircleHC8FisHC8F


//***************************************************************************
//  $PrimFnMonCircleHC8VisHC8R
//
//  Primitive scalar function monadic Circle:  HC8V {is} fn HC8R
//***************************************************************************

void PrimFnMonCircleHC8VisHC8R
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int    i;                       // Loop counter
    APLVFP aplVfpPi = GetMemPTD ()->mpfrPi;
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC8R][ARRAY_HC8V]) (&lpatRht->aplHC8R, 0, &atRht);

    // Initialize to 0
    mpof_init0 (&lpMemRes[uRes]);

    // Loop through all the parts
    for (i = 0; i < 8; i++)
        mpfr_mul (&lpMemRes[uRes].parts[i], &atRht.aplHC8V.parts[i], &aplVfpPi, MPFR_RNDN);

    mpof_clear (&atRht.aplHC8V);
} // End PrimFnMonCircleHC8VisHC8R


//***************************************************************************
//  $PrimFnMonCircleHC8VisHC8V
//
//  Primitive scalar function monadic Circle:  HC8V {is} fn HC8V
//***************************************************************************

void PrimFnMonCircleHC8VisHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int    i;                       // Loop counter
    APLVFP aplVfpPi = GetMemPTD ()->mpfrPi;

    // Initialize to 0
    mpof_init0 (&lpMemRes[uRes]);

    // Loop through all the parts
    for (i = 0; i < 8; i++)
        mpfr_mul (&lpMemRes[uRes].parts[i], &lpatRht->aplHC8V.parts[i], &aplVfpPi, MPFR_RNDN);
} // End PrimFnMonCircleHC8VisHC8V


//***************************************************************************
//  $PrimSpecCircleStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecCircleStorageTypeDyd
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
} // End PrimSpecCircleStorageTypeDyd


//***************************************************************************
//  $PrimFnDydCircleFisIvI
//
//  Primitive scalar function dyadic Circle:  F {is} I fn I
//***************************************************************************

void PrimFnDydCircleFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Split cases based upon the value of the integer left argument
    switch (lpatLft->aplInteger)
    {
        case   4:       // sqrt (1 + R * 2)
            // Call assembler function
            iAsmCircle4Int (&lpMemRes[uRes], &lpatRht->aplInteger);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                break;

            return;

        case   3:       // tan (R)
            // Call assembler function
            iAsmCircle3Int (&lpMemRes[uRes], &lpatRht->aplInteger);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                break;

            return;

        case   2:       // cos (R)
            // Call assembler function
            iAsmCircle2Int (&lpMemRes[uRes], &lpatRht->aplInteger);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                break;

            return;

        case   1:       // sin (R)
            // Call assembler function
            iAsmCircle1Int (&lpMemRes[uRes], &lpatRht->aplInteger);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                break;

            return;

        case   0:       // sqrt (1 - R * 2)
            // Check for Complex result
            if (abs64 (lpatRht->aplInteger) > 1)
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            // Call assembler function
            iAsmCircle0Int (&lpMemRes[uRes], &lpatRht->aplInteger);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                break;

            return;

        case  -4:       // (R + 1) � sqrt ((R - 1) / (R + 1))
                        // a.k.a. sqrt ((-1) + R * 2)
            // Check for Complex result
            if (abs64 (lpatRht->aplInteger) < 1)
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            // Call assembler function
            iAsmCircleN4Int (&lpMemRes[uRes], &lpatRht->aplInteger);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                break;

            return;

        default:
            // Convert left and right integers to FLOAT
            lpatLft->aplFloat = (APLFLOAT) lpatLft->aplInteger;
            lpatRht->aplFloat = (APLFLOAT) lpatRht->aplInteger;

            // Pass on to FisFvF
            PrimFnDydCircleFisFvF (lpMemRes,
                                   uRes,
                                   lpatLft,
                                   lpatRht,
                                   lpPrimSpec);
            return;
    } // End SWITCH

    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
} // End PrimFnDydCircleFisIvI


//***************************************************************************
//  $PrimFnDydCircleFisFvF
//
//  Primitive scalar function dyadic Circle:  F {is} F fn F
//***************************************************************************

void PrimFnDydCircleFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UINT   bRet;
    APLINT aplLft;

    // Attempt to convert the float to an integer using System []CT
    aplLft = ConvertToInteger_SCT (ARRAY_FLOAT, &lpatLft->aplFloat, 0, &bRet);

    if (bRet)
    // Split cases based upon the value of the (now) integer left argument
    switch (aplLft)
    {
        case  12:       // arc (phase) of R
            if (SIGN_APLFLOAT (lpatRht->aplFloat))
                lpMemRes[uRes] = FloatPi;
            else
                lpMemRes[uRes] = 0.0;
            return;

        case  11:       // Imaginary part of R
            lpMemRes[uRes] = 0;

            return;

        case  10:       // |R
            lpMemRes[uRes] = fabs (lpatRht->aplFloat);

            return;

        case   9:       // Real part of R
            lpMemRes[uRes] = lpatRht->aplFloat;

            return;

        case   8:       // sqrt ((-1) - R * 2)
            RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

        case   7:       // tanh (R)
            // Call subroutine
            lpMemRes[uRes] = tanh (lpatRht->aplFloat);

            return;

        case   6:       // cosh (R)
            // Call subroutine
            lpMemRes[uRes] = cosh (lpatRht->aplFloat);

            return;

        case   5:       // sinh (R)
            // Call subroutine
            lpMemRes[uRes] = sinh (lpatRht->aplFloat);

            return;

        case   4:       // sqrt (1 + R * 2)
            // Get absolute value to avoid duplication in comparison and result
            lpatRht->aplFloat = fabs (lpatRht->aplFloat);

            // Values at or above this limit when squared
            //   lpMemRes[uRes] = infinity, so we lpMemRes[uRes] = the arg unchanged.
            // Also, 2*26 = 67108864 is the smallest positive number such that
            //   a=sqrt (1 + a * 2)
            if (lpatRht->aplFloat >= 1.3407807929942596E154)
                lpMemRes[uRes] = lpatRht->aplFloat;
            else
            {
                // Call assembler function
                iAsmCircle4Flt (&lpMemRes[uRes], &lpatRht->aplFloat);

                // Check for NaN
                if (_isnan (lpMemRes[uRes]))
                    break;
            } // End IF/ELSE

            return;

        case   3:       // tan (R)
            // Call assembler function
            iAsmCircle3Flt (&lpMemRes[uRes], &lpatRht->aplFloat);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                break;

            return;

        case   2:       // cos (R)
            // Call assembler function
            iAsmCircle2Flt (&lpMemRes[uRes], &lpatRht->aplFloat);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                break;

            return;

        case   1:       // sin (R)
            // Call assembler function
            iAsmCircle1Flt (&lpMemRes[uRes], &lpatRht->aplFloat);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                break;

            return;

        case   0:       // sqrt (1 - R * 2)
            // Check for Complex result
            if (fabs (lpatRht->aplFloat) > 1)
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            // Call assembler function
            iAsmCircle0Flt (&lpMemRes[uRes], &lpatRht->aplFloat);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                break;

            return;

        case  -1:       // asin (R)
            // Check for Complex result
            if (fabs (lpatRht->aplFloat) > 1)
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            // Call subroutine
            lpMemRes[uRes] = asin (lpatRht->aplFloat);

            return;

        case  -2:       // acos (R)
            // Check for Complex result
            if (fabs (lpatRht->aplFloat) > 1)
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            // Call subroutine
            lpMemRes[uRes] = acos (lpatRht->aplFloat);

            return;

        case  -3:       // atan (R)
            // Call subroutine
            lpMemRes[uRes] = atan (lpatRht->aplFloat);

            return;

        case  -4:       // (R + 1) � sqrt ((R - 1) / (R + 1))
                        // a.k.a. sqrt ((-1) + R * 2)
            // Check for Complex result
            if (fabs (lpatRht->aplFloat) < 1)
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            // Call assembler function
            iAsmCircleN4Flt (&lpMemRes[uRes], &lpatRht->aplFloat);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            return;

        case  -5:       // asinh (R)
                        // ln (R + sqrt (1 + R * 2))
            lpMemRes[uRes] = gsl_asinh (lpatRht->aplFloat);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            return;

        case  -6:       // acosh (R)
                        // 2 x ln (sqrt ((R + 1) x 0.5) + sqrt ((R - 1) x 0.5))
            // Check for Complex result
            if (lpatRht->aplFloat < 1)
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            lpMemRes[uRes] = gsl_acosh (lpatRht->aplFloat);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            return;

        case  -7:       // atanh (R)
                        // 0.5 x (ln (1 + R) - ln (1 - R))
            // Check for Complex result
            if (fabs (lpatRht->aplFloat) >= 1)
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            lpMemRes[uRes] = gsl_atanh (lpatRht->aplFloat);

            // Check for NaN
            if (_isnan (lpMemRes[uRes]))
                RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            return;

        case  -8:       // -sqrt ((-1) - R * 2)
            RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

        case  -9:       // R
            lpMemRes[uRes] = lpatRht->aplFloat;

            return;

        case -10:       // +R
            lpMemRes[uRes] = lpatRht->aplFloat;

            return;

        case -11:       // 0J1 x R
        case -12:       // *0J1 x R
            RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);

            return;

        default:
            break;
    } // End SWITCH

    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
} // End PrimFnDydCircleFisFvF


//***************************************************************************
//  $PrimFnDydCircleVisRvR
//
//  Primitive scalar function dyadic Circle:  V {is} R fn R
//***************************************************************************

void PrimFnDydCircleVisRvR
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

    PrimFnDydCircleVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydCircleVisRvR


//***************************************************************************
//  $PrimFnDydCircleVisVvV
//
//  Primitive scalar function dyadic Circle:  V {is} V fn V
//***************************************************************************

void PrimFnDydCircleVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLINT aplLft;
    UBOOL  bRet;

    // Get the left arg as an int
    aplLft = ConvertToInteger_SCT (ARRAY_VFP, &lpatLft->aplVfp, 0, &bRet);

    if (bRet)
    // Split cases based upon the value of the (now) integer left argument
    switch (aplLft)
    {
        case  12:       // arc (phase) of R
            if (SIGN_APLVFP (&lpatRht->aplVfp))
                mpfr_init_set (&lpMemRes[uRes], &aplPiHC8V.parts[0], MPFR_RNDN);
            else
                mpfr_init0    (&lpMemRes[uRes]);
            return;

        case  11:       // Imaginary part of R
            // Initialize to 0
            mpfr_init0 (&lpMemRes[uRes]);

            return;

        case  10:       // |R
            // Initialize to 0
            mpfr_init0 (&lpMemRes[uRes]);

            // Save in the result
            mpfr_abs (&lpMemRes[uRes], &lpatRht->aplVfp, MPFR_RNDN);

            return;

        case   9:       // Real part of R
            mpfr_init_set (&lpMemRes[uRes], &lpatRht->aplVfp, MPFR_RNDN);

            return;

        case   8:       // sqrt ((-1) - R * 2)
            RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

        case   7:       // tanh (R)
            // Call subroutine
            lpMemRes[uRes] = tanhVfp (lpatRht->aplVfp);

            return;

        case   6:       // cosh (R)
            // Call subroutine
            lpMemRes[uRes] = coshVfp (lpatRht->aplVfp);

            return;

        case   5:       // sinh (R)
            // Call subroutine
            lpMemRes[uRes] = sinhVfp (lpatRht->aplVfp);

            return;

        case   4:       // sqrt (1 + R * 2)
            // Initialize the result to 0
            mpfr_init0  (&lpMemRes[uRes]);
            mpfr_mul    (&lpMemRes[uRes], &lpatRht->aplVfp, &lpatRht->aplVfp, MPFR_RNDN);  // R * 2
            mpfr_add_si (&lpMemRes[uRes], &lpMemRes[uRes]   , 1         , MPFR_RNDN);  // 1 + R * 2
            mpfr_sqrt   (&lpMemRes[uRes], &lpMemRes[uRes]               , MPFR_RNDN);  // sqrt (1 + R * 2)

            return;                      // sqrt (1 + pow (lpatRht->aplVfp, 2));

        case   3:       // tan (R)
            // Call subroutine
            lpMemRes[uRes] = tanVfp (lpatRht->aplVfp);

            return;

        case   2:       // cos (R)
            // Call subroutine
            lpMemRes[uRes] = cosVfp (lpatRht->aplVfp);

            return;

        case   1:       // sin (R)
            // Call subroutine
            lpMemRes[uRes] = sinVfp (lpatRht->aplVfp);

            return;

        case   0:       // sqrt (1 - R * 2)
            // Check for Complex result
            if (mpfr_cmp_si (&lpatRht->aplVfp,  1) > 0
             || mpfr_cmp_si (&lpatRht->aplVfp, -1) < 0)
                RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

            // Initialize the result to 0
            mpfr_init0  (&lpMemRes[uRes]);
            mpfr_mul    (&lpMemRes[uRes], &lpatRht->aplVfp, &lpatRht->aplVfp, MPFR_RNDN);  // R * 2
            mpfr_ui_sub (&lpMemRes[uRes], 1               , &lpMemRes[uRes] , MPFR_RNDN);  // 1 - R * 2
            mpfr_sqrt   (&lpMemRes[uRes],                   &lpMemRes[uRes] , MPFR_RNDN);  // sqrt (1 - R * 2)

            return;

        case  -1:       // asin (R)
            // Check for Complex result
            if (mpfr_cmp_si (&lpatRht->aplVfp,  1) > 0
             || mpfr_cmp_si (&lpatRht->aplVfp, -1) < 0)
                RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

            // Call subroutine
            lpMemRes[uRes] = asinVfp (lpatRht->aplVfp);

            return;

        case  -2:       // acos (R)
            // Check for Complex result
            if (mpfr_cmp_si (&lpatRht->aplVfp,  1) > 0
             || mpfr_cmp_si (&lpatRht->aplVfp, -1) < 0)
                RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

            // Call subroutine
            lpMemRes[uRes] = acosVfp (lpatRht->aplVfp);

            return;

        case  -3:       // atan (R)
            // Call subroutine
            lpMemRes[uRes] = atanVfp (lpatRht->aplVfp);

            return;

        case  -4:       // (R + 1) � sqrt ((R - 1) / (R + 1))
                        // a.k.a. sqrt ((-1) + R * 2)
        {
            APLVFP aplTmp2,
                   aplTmp3;

            // Check for Complex result
            if (mpfr_cmp_si (&lpatRht->aplVfp,  1) < 0
             && mpfr_cmp_si (&lpatRht->aplVfp, -1) > 0)
                RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

            // Initialize the result to 0
            mpfr_init0  (&lpMemRes[uRes]);

            // If the right arg is -1, return 0
            if (mpfr_cmp_si (&lpatRht->aplVfp, -1) EQ 0)
                return;

            // Initialize the temps to 0
            mpfr_init0  (&aplTmp2);
            mpfr_init0  (&aplTmp3);

            mpfr_add    (&aplTmp2       , &lpatRht->aplVfp, &mpfOne         , MPFR_RNDN);   // R + 1
            mpfr_sub    (&aplTmp3       , &lpatRht->aplVfp, &mpfOne         , MPFR_RNDN);   // R - 1
            mpfr_div    (&aplTmp3       , &aplTmp3        , &aplTmp2        , MPFR_RNDN);   // (R - 1) / (R + 1)
            mpfr_sqrt   (&lpMemRes[uRes], &aplTmp3                          , MPFR_RNDN);   // sqrt ((R - 1) / (R + 1))
////////////mpfr_add    (&aplTmp2       , &lpatRht->aplVfp, &mpfOne         , MPFR_RNDN);   // R + 1  -- Already set
            mpfr_mul    (&lpMemRes[uRes], &aplTmp2        , &lpMemRes[uRes] , MPFR_RNDN);   // (R + 1) x sqrt ((R - 1) / (R + 1))

            // We no longer need this storage
            mpfr_clear (&aplTmp3);
            mpfr_clear (&aplTmp2);

            // Check for NaN
            if (mpfr_nan_p (&lpMemRes[uRes]))
                RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

            return;
        } // End case -4

        case  -5:       // asinh (R)
                        // ln (R + sqrt (1 + R * 2))
            // Call subroutine
            lpMemRes[uRes] = asinhVfp (lpatRht->aplVfp);

            return;

        case  -6:       // acosh (R)
                        // 2 x ln (sqrt ((R + 1) x 0.5) + sqrt ((R - 1) x 0.5))
            // Check for Complex result
            if (mpfr_cmp_si (&lpatRht->aplVfp, 1) < 0)
                RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

            // Call subroutine
            lpMemRes[uRes] = acoshVfp (lpatRht->aplVfp);

            return;

        case  -7:       // atanh (R)
                        // 0.5 x (ln (1 + R) - ln (1 - R))
            // Check for Complex result
            if (mpfr_cmp_si (&lpatRht->aplVfp,  1) >= 0
             || mpfr_cmp_si (&lpatRht->aplVfp, -1) <= 0)
                RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

            // Call subroutine
            lpMemRes[uRes] = atanhVfp (lpatRht->aplVfp);

            return;

        case  -8:       // -sqrt ((-1) - R * 2)
            RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

        case  -9:       // R
        case -10:       // +R
            mpfr_init_set (&lpMemRes[uRes], &lpatRht->aplVfp, MPFR_RNDN);

            return;

        case -11:       // 0J1 x R
        case -12:       // *0J1 x R
            RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);

            return;

        default:
            break;
    } // End SWITCH

    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
} // End PrimFnDydCircleVisVvV


//***************************************************************************
//  $tanhVfp:   7
//***************************************************************************

APLVFP tanhVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_tanh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End tanhVfp


//***************************************************************************
//  $coshVfp:   6
//***************************************************************************

APLVFP coshVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_cosh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End coshVfp


//***************************************************************************
//  $sinhVfp:   5
//***************************************************************************

APLVFP sinhVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_sinh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End sinhVfp


//***************************************************************************
//  $tanVfp:    3
//***************************************************************************

APLVFP tanVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_tan  (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End tanVfp


//***************************************************************************
//  $cosVfp:    2
//***************************************************************************

APLVFP cosVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_cos  (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End cosVfp


//***************************************************************************
//  $sinVfp:    1
//***************************************************************************

APLVFP sinVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_sin  (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End sinVfp


//***************************************************************************
//  $asinVfp:  -1
//***************************************************************************

APLVFP asinVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Check for special cases:  R < -1  or  R > 1
    if (mpfr_cmp_si (&aplVfpRht, -1) < 0
     || mpfr_cmp_si (&aplVfpRht,  1) > 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_asin (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End asinVfp


//***************************************************************************
//  $acosVfp:  -2
//***************************************************************************

APLVFP acosVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Check for special cases:  R < -1  or  R > 1
    if (mpfr_cmp_si (&aplVfpRht, -1) < 0
     || mpfr_cmp_si (&aplVfpRht,  1) > 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_acos (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End acosVfp


//***************************************************************************
//  $atanVfp:  -3
//***************************************************************************

APLVFP atanVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_atan (&mpfRes, &aplVfpRht, MPFR_RNDN);

    return mpfRes;
} // End atanVfp


//***************************************************************************
//  $asinhVfp:  -5
//***************************************************************************

APLVFP asinhVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_asinh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    // Check for a NaN
    if (mpfr_nan_p (&mpfRes))
    {
        mpfr_clear (&mpfRes);

        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    } // End IF

    return mpfRes;
} // End asinhVfp


//***************************************************************************
//  $acoshVfp:  -6
//***************************************************************************

APLVFP acoshVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_acosh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    // Check for a NaN
    if (mpfr_nan_p (&mpfRes))
    {
        mpfr_clear (&mpfRes);

        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    } // End IF

    return mpfRes;
} // End acoshVfp


//***************************************************************************
//  $atanhVfp:  -7
//***************************************************************************

APLVFP atanhVfp
    (APLVFP aplVfpRht)

{
    APLVFP mpfRes = {0};

    // Initialize the result to 0
    mpfr_init0 (&mpfRes);

    // Calculate the function
    mpfr_atanh (&mpfRes, &aplVfpRht, MPFR_RNDN);

    // Check for a NaN
    if (mpfr_nan_p (&mpfRes))
    {
        mpfr_clear (&mpfRes);

        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    } // End IF

    return mpfRes;
} // End atanhVfp


//***************************************************************************
//  $PrimFnDydCircleHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic Circle:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydCircleHC2FisHC2IvHC2I
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
    PrimFnDydCircleHC2FisHC2FvHC2F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydCircleHC2FisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydCircleHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic Circle:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydCircleHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    UINT    bRet;
    APLINT  aplLft;
    APLHC2F aplTmp = {0};

    // Attempt to convert the float to an integer using System []CT
    aplLft = ConvertToInteger_SCT (ARRAY_HC2F, &lpatLft->aplHC2F, 0, &bRet);

    if (bRet)
    // Split cases based upon the value of the (now) integer left argument
    switch (aplLft)
    {
        case  12:       // arc (phase) of R
            aplTmp = ArcHCxF_RE (lpatRht->aplHC8F, 2).partsLo[0].partsLo[0];
            lpMemRes[uRes].parts[0] = aplTmp.parts[1];
            lpMemRes[uRes].parts[1] = 0;

            return;

        case  11:       // Imaginary part of R
            lpMemRes[uRes].parts[0] = lpatRht->aplHC2F.parts[1];
            lpMemRes[uRes].parts[1] = 0;

            return;

        case  10:       // |R
            lpMemRes[uRes].parts[0] = MagHC2F (lpatRht->aplHC2F);
            lpMemRes[uRes].parts[1] = 0;

            return;

        case   9:       // Real part of R
            lpMemRes[uRes].parts[0] = lpatRht->aplHC2F.parts[0];
            lpMemRes[uRes].parts[1] = 0;

            return;

        case   8:       // sqrt ((-1) - R * 2)
#ifdef USE_HYPOT
            lpMemRes[uRes] = fpcf_hypot (-1,
                                         confpof_1.partsLo[0].partsLo[0],
                                         -1,
                                         lpatRht->aplHC2F);
#else
            aplTmp = SubHC2F_RE (confpof_N1.partsLo[0].partsLo[0],
                                 MulHC2F_RE (lpatRht->aplHC2F,
                                             lpatRht->aplHC2F));
            // -sqrt ((-1) - R * 2)
            lpMemRes[uRes] = SqrtHCxF_RE (*(LPAPLHC8F) &aplTmp, 2).partsLo->partsLo[0];
#endif
            return;

        case   7:       // tanh (R)
            fpXf_tanh  (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case   6:       // cosh (R)
            fpXf_cosh  (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case   5:       // sinh (R)
            fpXf_sinh  (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case   4:       // sqrt (1 + R * 2)
#ifdef USE_HYPOT
            lpMemRes[uRes] = fpcf_hypot ( 1,
                                         confpof_1.partsLo[0].partsLo[0],
                                          1,
                                         lpatRht->aplHC2F);
#else
            //  1 + R * 2
            aplTmp = AddHC2F_RE (confpof_1.partsLo[0].partsLo[0],
                                 MulHC2F_RE (lpatRht->aplHC2F,
                                             lpatRht->aplHC2F));
            // sqrt (1 + R * 2)
            lpMemRes[uRes] = SqrtHCxF_RE (*(LPAPLHC8F) &aplTmp, 2).partsLo[0].partsLo[0];
#endif
            return;

        case   3:       // tan (R)
            fpXf_tan   (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            // Check for NaN
            if (_isnan (lpMemRes[uRes].parts[0]))
                break;

            return;

        case   2:       // cos (R)
            fpXf_cos   (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case   1:       // sin (R)
            fpXf_sin   (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case   0:       // sqrt (1 - R * 2)
#ifdef USE_HYPOT
            lpMemRes[uRes] = fpcf_hypot ( 1,
                                         confpof_1.partsLo[0].partsLo[0],
                                         -1,
                                         lpatRht->aplHC2F);
#else
            //  1 - R * 2
            aplTmp = SubHC2F_RE (confpof_1.partsLo[0].partsLo[0],
                                 MulHC2F_RE (lpatRht->aplHC2F,
                                             lpatRht->aplHC2F));
            // sqrt (1 - R * 2)
            lpMemRes[uRes] = SqrtHCxF_RE (*(LPAPLHC8F) &aplTmp, 2).partsLo[0].partsLo[0];
#endif
            return;

        case  -1:       // asin (R)
            fpXf_asin  (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case  -2:       // acos (R)
            fpXf_acos  (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case  -3:       // atan (R)
            fpXf_atan  (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case  -4:       // (R + 1) � sqrt ((R - 1) / (R + 1))
                        // a.k.a. sqrt ((-1) + R * 2)
            // If the right arg is -1, return 0
            if (EqualHCxFvHCxF (lpatRht,
                  (LPALLTYPES) &confpof_N1.partsLo[0].partsLo[0],
                                2,
                                GetQuadCT (),
                                __FUNCTION__))
                return;

#ifdef USE_HYPOT
            lpMemRes[uRes] = fpcf_hypot (-1,
                                         confpof_1.partsLo[0].partsLo[0],
                                          1,
                                         lpatRht->aplHC2F);
#else
            //  (R - 1) / (R + 1)
            lpMemRes[uRes] = DivHC2F_RE (SubHC2F_RE (lpatRht->aplHC2F,
                                                     confpof_1.partsLo[0].partsLo[0]),
                                         AddHC2F_RE (lpatRht->aplHC2F,
                                                     confpof_1.partsLo[0].partsLo[0]));
            // sqrt (((R - 1) / (R + 1))
            lpMemRes[uRes] = SqrtHCxF_RE (*(LPAPLHC8F) &lpMemRes[uRes], 2).partsLo[0].partsLo[0];

            // (R + 1) � sqrt ((R - 1) / (R + 1))
            lpMemRes[uRes] = MulHC2F_RE (AddHC2F_RE (lpatRht->aplHC2F,
                                                     confpof_1.partsLo[0].partsLo[0]),
                                         lpMemRes[uRes]);
#endif
            // If the real part of the arg is zero, and the
            //   absolute value of the imag part of the arg
            //   is <= 1, ...
            if (lpatRht->aplHC2F.parts[0] EQ 0
             && fabs (lpatRht->aplHC2F.parts[1]) <= 1)
            {
                Assert (lpMemRes[uRes].parts[0] < SYS_CT);

                // Set the real part of the result to zero
                lpMemRes[uRes].parts[0] = 0;
            } // End IF

            return;

        case  -5:       // asinh (R)
                        // ln (R + sqrt (1 + R * 2))
            fpXf_asinh (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case  -6:       // acosh (R)
                        // 2 x ln (sqrt ((R + 1) / 2) + sqrt ((R - 1) / 2))
            fpXf_acosh (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case  -7:       // atanh (R)
                        // 0.5 x (ln (1 + R) - ln (1 - R))
            fpXf_atanh (*(fpcho_t *) &lpMemRes[uRes],
                        *(fpcho_t *) &lpatRht->aplHC2F,
                                      2);
            return;

        case  -8:       // -sqrt ((-1) - R*2)
#ifdef USE_HYPOT
            lpMemRes[uRes] = fpcf_hypot (-1,
                                         confpof_1.partsLo[0].partsLo[0],
                                         -1,
                                         lpatRht->aplHC2F);
#else
            aplTmp = SubHC2F_RE (confpof_N1.partsLo[0].partsLo[0],
                                 MulHC2F_RE (lpatRht->aplHC2F,
                                             lpatRht->aplHC2F));
            // -sqrt ((-1) - R * 2)
            lpMemRes[uRes] = NegHC2F_RE (SqrtHCxF_RE (*(LPAPLHC8F) &aplTmp, 2).partsLo[0].partsLo[0]);
#endif
            return;

        case  -9:       // R
            lpMemRes[uRes] = lpatRht->aplHC2F;

            return;

        case -10:       // +R
            lpMemRes[uRes] = ConjHC2F (lpatRht->aplHC2F);

            return;

        case -11:       // 0J1 x R
            lpMemRes[uRes] = MulHC2F_RE (confpof_I.partsLo[0].partsLo[0],
                                         lpatRht->aplHC2F);
            return;

        case -12:       // *0J1 x R
            lpMemRes[uRes] = ExpHC2F_RE (MulHC2F_RE (confpof_I.partsLo[0].partsLo[0],
                                                     lpatRht->aplHC2F));
            return;

        default:
            break;
    } // End SWITCH

    RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
} // End PrimFnDydCircleHC2FisHC2FvHC2F


//***************************************************************************
//  $PrimFnDydCircleHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic Circle:  HC2V {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydCircleHC2VisHC2RvHC2R
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
    PrimFnDydCircleHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydCircleHC2VisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydCircleHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic Circle:  HC2V {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydCircleHC2VisHC2VvHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    PrimFnCircleMpfMac (c,2,.partsLo.partsLo)
} // End PrimFnDydCircleHC2VisHC2VvHC2V


//***************************************************************************
//  $PrimFnDydCircleHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic Circle:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydCircleHC4FisHC4IvHC4I
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
    PrimFnDydCircleHC4FisHC4FvHC4F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydCircleHC4FisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydCircleHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic Circle:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydCircleHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    PrimFnCircleFpfMac (h,4,.partsLo[0]     )
} // End PrimFnDydCircleHC4FisHC4FvHC4F


//***************************************************************************
//  $PrimFnDydCircleHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic Circle:  HC4V {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydCircleHC4VisHC4RvHC4R
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
    PrimFnDydCircleHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydCircleHC4VisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydCircleHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic Circle:  HC4V {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydCircleHC4VisHC4VvHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    PrimFnCircleMpfMac (h,4,.partsLo        )
} // End PrimFnDydCircleHC4VisHC4VvHC4V


//***************************************************************************
//  $PrimFnDydCircleHC8FisHC8IvHC8I
//
//  Primitive scalar function dyadic Circle:  HC8F {is} HC8I fn HC8I
//***************************************************************************

void PrimFnDydCircleHC8FisHC8IvHC8I
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
    PrimFnDydCircleHC8FisHC8FvHC8F (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydCircleHC8FisHC8IvHC8I


//***************************************************************************
//  $PrimFnDydCircleHC8FisHC8FvHC8F
//
//  Primitive scalar function dyadic Circle:  HC8F {is} HC8F fn HC8F
//***************************************************************************

void PrimFnDydCircleHC8FisHC8FvHC8F
    (LPAPLHC8F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    PrimFnCircleFpfMac (o,8,                )
} // End PrimFnDydCircleHC8FisHC8FvHC8F


//***************************************************************************
//  $PrimFnDydCircleHC8VisHC8RvHC8R
//
//  Primitive scalar function dyadic Circle:  HC8V {is} HC8R fn HC8R
//***************************************************************************

void PrimFnDydCircleHC8VisHC8RvHC8R
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
    PrimFnDydCircleHC8VisHC8VvHC8V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);
} // End PrimFnDydCircleHC8VisHC8RvHC8R


//***************************************************************************
//  $PrimFnDydCircleHC8VisHC8VvHC8V
//
//  Primitive scalar function dyadic Circle:  HC8V {is} HC8V fn HC8V
//***************************************************************************

void PrimFnDydCircleHC8VisHC8VvHC8V
    (LPAPLHC8V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    PrimFnCircleMpfMac (o,8,                )
} // End PrimFnDydCircleHC8VisHC8VvHC8V


//***************************************************************************
//  End of File: pf_circle.c
//***************************************************************************
