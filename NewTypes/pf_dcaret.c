//***************************************************************************
//  NARS2000 -- Primitive Function -- DownCaret
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


#ifndef PROTO
PRIMSPEC PrimSpecDownCaret = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecDownCaretStorageTypeMon, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretAPA_EM, -- Can't happen w/DownCaret

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonDownCaretBisB, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretBisI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretBisF, -- Can't happen w/DownCaret

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonDownCaretIisI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretIisF, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretFisI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretFisF, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretRisR, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretVisR, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretVisV, -- Can't happen w/DownCaret

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonDownCaretHC2IisHC2I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC2IisHC2F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC2FisHC2I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC2FisHC2F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC2RisHC2R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC2VisHC2R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC2VisHC2V, -- Can't happen w/DownCaret

    NULL,   // &PrimFnMonDownCaretHC4IisHC4I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC4IisHC4F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC4FisHC4I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC4FisHC4F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC4RisHC4R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC4VisHC4R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC4VisHC4V, -- Can't happen w/DownCaret

    NULL,   // &PrimFnMonDownCaretHC8IisHC8I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC8IisHC8F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC8FisHC8I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC8FisHC8F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC8RisHC8R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC8VisHC8R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretHC8VisHC8V, -- Can't happen w/DownCaret

    // Monadic FLT/VFP result HC arg functions (indexable)
    NULL,   // &PrimFnMonDownCaretFisHC2I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretFisHC2F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretVisHC2R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretVisHC2V, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretFisHC4I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretFisHC4F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretVisHC4R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretVisHC4V, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretFisHC8I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretFisHC8F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretVisHC8R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretVisHC8V, -- Can't happen w/DownCaret

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDownCaretStorageTypeDyd,
    NULL,   // &PrimFnDydDownCaretAPA_EM, -- Can't happen w/DownCaret

    // Dyadic Boolean result functions
    &PrimFnDydDownCaretBisBvB,
    NULL,   // &PrimFnDydDownCaretBisIvI, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisFvF, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisCvC, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisHvH, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisRvR, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisVvV, -- Can't happen w/DownCaret

    NULL,   // &PrimFnDydDownCaretBisHC2IvHC2I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisHC2FvHC2F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisHC2RvHC2R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisHC2VvHC2V, -- Can't happen w/DownCaret

    NULL,   // &PrimFnDydDownCaretBisHC4IvHC4I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisHC4FvHC4F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisHC4RvHC4R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisHC4VvHC4V, -- Can't happen w/DownCaret

    NULL,   // &PrimFnDydDownCaretBisHC8IvHC8I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisHC8FvHC8F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisHC8RvHC8R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretBisHC8VvHC8V, -- Can't happen w/DownCaret

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydDownCaretIisIvI,
    NULL,   // &PrimFnDydDownCaretIisFvF, -- Can't happen w/DownCaret
    &PrimFnDydDownCaretFisIvI,
    &PrimFnDydDownCaretFisFvF,
    &PrimFnDydDownCaretRisRvR,
    &PrimFnDydDownCaretVisRvR,
    &PrimFnDydDownCaretVisVvV,

    &PrimFnDydDownCaretHC2IisHC2IvHC2I,
    NULL,   // &PrimFnDydDownCaretHC2IisHC2FvHC2F, -- Can't happen w/DownCaret
    &PrimFnDydDownCaretHC2FisHC2IvHC2I,
    &PrimFnDydDownCaretHC2FisHC2FvHC2F,
    &PrimFnDydDownCaretHC2RisHC2RvHC2R,
    &PrimFnDydDownCaretHC2VisHC2RvHC2R,
    &PrimFnDydDownCaretHC2VisHC2VvHC2V,

    &PrimFnDydDownCaretHC4IisHC4IvHC4I,
    NULL,   // &PrimFnDydDownCaretHC2IisHC2FvHC2F, -- Can't happen w/DownCaret
    &PrimFnDydDownCaretHC4FisHC4IvHC4I,
    &PrimFnDydDownCaretHC4FisHC4FvHC4F,
    &PrimFnDydDownCaretHC4RisHC4RvHC4R,
    &PrimFnDydDownCaretHC4VisHC4RvHC4R,
    &PrimFnDydDownCaretHC4VisHC4VvHC4V,

    NULL,   // &PrimFnDydDownCaretHC8IisHC8IvHC8I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretHC8IisHC8FvHC8F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretHC8FisHC8IvHC8I, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretHC8FisHC8FvHC8F, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretHC8RisHC8RvHC8R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretHC8VisHC8RvHC8R, -- Can't happen w/DownCaret
    NULL,   // &PrimFnDydDownCaretHC8VisHC8VvHC8V, -- Can't happen w/DownCaret

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDownCaretB64isB64, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretB32isB32, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretB16isB16, -- Can't happen w/DownCaret
    NULL,   // &PrimFnMonDownCaretB08isB08, -- Can't happen w/DownCaret

    // Dyadic Boolean chunk functions
    &PrimFnDydDownCaretB64isB64vB64,
    &PrimFnDydDownCaretB32isB32vB32,
    &PrimFnDydDownCaretB16isB16vB16,
    &PrimFnDydDownCaretB08isB08vB08,

    // Miscellaneous
    0,      // []RL for atomicity
    FALSE,  // 00000001:  Allow dimension demotion for monadic scalar function
    FALSE,  // 00000002:  ...                          dyadic  ...
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecDownCaret};
#endif


//***************************************************************************
//  $PrimFnDownCaret_EM_YY
//
//  Primitive function for monadic and dyadic downCaret (ERROR and "or/GCD")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownCaret_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownCaret_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNCARET);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnDownCaret_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecDownCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownCaretStorageTypeDyd
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

    // Weed out numbers whose Floor does not have Fractionality
    //   such as Character, Nested, Hetero, Octonions, and Quaternions w/o Hurwitz
    if (!IsNested (aplTypeRes)
     && !HasFractionality (aplTypeRes))
        return ARRAY_ERROR;

    return aplTypeRes;
} // End PrimSpecDownCaretStorageTypeDyd


//***************************************************************************
//  $GcdHC1I
//
//  GCD (Greatest Common Divisor) for aplIntegers
//***************************************************************************

APLINT GcdHC1I
    (APLINT aplLft,
     APLINT aplRht)

{
    APLINT aplTmp;

    // Ensure both arguments are non-negative
    aplLft = abs64 (aplLft);
    aplRht = abs64 (aplRht);

    while (aplLft NE 0)
    {
        aplTmp = aplLft;
        aplLft = aplRht % aplLft;
        aplRht = aplTmp;
    } // End WHILE

    // The sign of the result is non-negative
    return  abs64 (aplRht);
} // End GcdHC1I


//***************************************************************************
//  $GcdHC2I
//
//  GCD (Greatest Common Divisor) for HC2I
//***************************************************************************

APLHC2I GcdHC2I
    (APLHC2I aplLft,
     APLHC2I aplRht)

{
    APLHC2I  aplTmp;
    ALLTYPES atRht = {0};
    UINT     uIndex,                    // Accumulator for negative signs
             uRht;                      // Index of right unit
    int      i;                         // Loop counter
    APLFLOAT fQuadCTOld,                // Original []CT
             fQuadCTNew = 1E-10;        // Default  []CT for comparisons and L|R

    // Save old and set new []CT
    fQuadCTOld = GetQuadCT (); SetQuadCT (fQuadCTNew);

    __try
    {
        do
        {
            // T = L
            aplTmp = aplLft;

            // L = L | R
            aplLft = ModHC2I (aplLft, aplRht);

            // R = T
            aplRht = aplTmp;
        } while (!IsZeroHCxI (&aplLft, 2));

        // Rotate the result into the first quadrant

        // Loop through the parts of the result looking for negative values
        for (uIndex = 0, i = 2 - 1; i >= 0; i--)
            uIndex = (3 * uIndex) + 1 + signumint (aplRht.parts[i]);
        // Get the table index for the righthand unit
        uRht = gGcdRotateTab2[uIndex];

        // Make the righthand unit
        MakeUnit (ARRAY_HC2I, uRht, &atRht);

        // Multiply on the right by the righthand unit
        aplRht = MulHC2I_RE (aplRht, atRht.aplHC2I);
    } __except (CheckHCDim (GetExceptionInformation (), 2))
    {
        // Restore original []CT
        SetQuadCT (fQuadCTOld);

        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    } // End __try/__except

    // Restore original []CT
    SetQuadCT (fQuadCTOld);

    return aplRht;
} // End GcdHC2I


//***************************************************************************
//  $GcdHC4I
//
//  GCD (Greatest Common Divisor) for HC4I
//***************************************************************************

APLHC4I GcdHC4I
    (APLHC4I aplLft,
     APLHC4I aplRht)

{
    APLHC4I  aplTmp;
    ALLTYPES atLft = {0},
             atRht = {0};
    UINT     uIndex,                    // Accumulator for negative signs
             uLft,                      // Index of left unit
             uRht;                      // ...      right ...
    int      i;                         // Loop counter
    APLFLOAT fQuadCTOld,                // Original []CT
             fQuadCTNew = 1E-10;        // Default  []CT for comparisons and L|R

    // Save old and set new []CT
    fQuadCTOld = GetQuadCT (); SetQuadCT (fQuadCTNew);

    __try
    {
        do
        {
            // T = L
            aplTmp = aplLft;

            // L = L | R
            aplLft = ModHC4I (aplLft, aplRht);

            // R = T
            aplRht = aplTmp;
        } while (!IsZeroHCxI (&aplLft, 4));

        // Rotate the result into the first quadrant

        // Loop through the parts of the result looking for negative values
        for (uIndex = 0, i = 4 - 1; i >= 0; i--)
            uIndex = (3 * uIndex) + 1 + signumint (aplRht.parts[i]);
        // Get the table indices
        uLft = gGcdRotateTab4[uIndex][0];
        uRht = gGcdRotateTab4[uIndex][1];

        // Make the left & right units
        MakeUnit (ARRAY_HC4I, uLft, &atLft);
        MakeUnit (ARRAY_HC4I, uRht, &atRht);

        // Multiply by the left and right units
        aplRht = MulHC4I_RE (atLft.aplHC4I, MulHC4I_RE (aplRht, atRht.aplHC4I));
    } __except (CheckHCDim (GetExceptionInformation (), 4))
    {
        // Restore original []CT
        SetQuadCT (fQuadCTOld);

        RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);
    } // End __try/__except

    // Restore original []CT
    SetQuadCT (fQuadCTOld);

    return aplRht;
} // End GcdHC4I


//***************************************************************************
//  $GcdHC1F
//
//  GCD (Greatest Common Divisor) for aplFloats
//***************************************************************************

APLFLOAT GcdHC1F
    (APLFLOAT aplLft,
     APLFLOAT aplRht)

{
    APLFLOAT aplTmp;

    // Ensure both arguments are non-negative
    aplLft = fabs (aplLft);
    aplRht = fabs (aplRht);

    while (aplLft >= GCD_CT)
    {
        aplTmp = aplLft;
        aplLft = AplModF (aplLft, aplRht);
        aplRht = aplTmp;
    } // End WHILE

    // The sign of the result is non-negative
    return fabs (aplRht);
} // End GcdHC1F


//***************************************************************************
//  $GcdHC2F
//
//  GCD (Greatest Common Divisor) for HC2F
//***************************************************************************

APLHC2F GcdHC2F
    (APLHC2F aplLft,
     APLHC2F aplRht)

{
    APLHC2F  aplTmp;
    ALLTYPES atRht = {0};
    UINT     uIndex,                    // Accumulator for negative signs
             uRht;                      // Index of right unit
    int      i;                         // Loop counter
    APLFLOAT fQuadCTOld,                // Original []CT
             fQuadCTNew = 1E-10,        // Default  []CT for comparisons and L|R
             fQuadCTvL  = 4E-15;        // Default  []CT for comparisons and L|R

    // Save old and set new []CT
    fQuadCTOld = GetQuadCT (); SetQuadCT (fQuadCTNew);

    __try
    {
        do
        {
            // T = L
            aplTmp = aplLft;

            // L = L | R
            aplLft = ModHC2F (aplLft, aplRht);

            // R = T
            aplRht = aplTmp;
        } while (MagHC2F (aplLft) > fQuadCTvL);

        // Rotate the result into the first quadrant

        // Loop through the parts of the result looking for negative values
        for (uIndex = 0, i = 2 - 1; i >= 0; i--)
            uIndex = (3 * uIndex) + 1 + signumflt (aplRht.parts[i]);
        // Get the table index for the righthand unit
        uRht = gGcdRotateTab2[uIndex];

        // Make the righthand unit
        MakeUnit (ARRAY_HC2F, uRht, &atRht);

        // Multiply on the right by the righthand unit
        aplRht = MulHC2F_RE (aplRht, atRht.aplHC2F);
    } __except (CheckHCDim (GetExceptionInformation (), 2))
    {
        // Restore original []CT
        SetQuadCT (fQuadCTOld);

        RaiseException (MyGetExceptionCode (), 0, 0, NULL);
    } // End __try/__except

    // Restore original []CT
    SetQuadCT (fQuadCTOld);

    return aplRht;
} // End GcdHC2F


//***************************************************************************
//  $GcdHC4F
//
//  GCD (Greatest Common Divisor) for HC4F
//***************************************************************************

APLHC4F GcdHC4F
    (APLHC4F aplLft,
     APLHC4F aplRht)

{
    APLHC4F  aplTmp;
    ALLTYPES atLft = {0},
             atRht = {0};
    UINT     uIndex,                    // Accumulator for negative signs
             uLft,                      // Index of left unit
             uRht;                      // ...      right ...
    int      i;                         // Loop counter
    APLFLOAT fQuadCTOld,                // Original []CT
             fQuadCTNew = 1E-10,        // Default  []CT for comparisons and L|R
             fQuadCTvL  = 4E-15;        // Default  []CT for comparisons and L|R

    // Save old and set new []CT
    fQuadCTOld = GetQuadCT (); SetQuadCT (fQuadCTNew);

    __try
    {
        do
        {
            // T = L
            aplTmp = aplLft;

            // L = L | R
            aplLft = ModHC4F (aplLft, aplRht);

            // R = T
            aplRht = aplTmp;
        } while (MagHC4F (aplLft) > fQuadCTvL);

        // Rotate the result into the first quadrant

        // Loop through the parts of the result looking for negative values
        for (uIndex = 0, i = 2 - 1; i >= 0; i--)
            uIndex = (3 * uIndex) + 1 + signumflt (aplRht.parts[i]);
        // Get the table indices
        uLft = gGcdRotateTab4[uIndex][0];
        uRht = gGcdRotateTab4[uIndex][1];

        // Make the left & right units
        MakeUnit (ARRAY_HC4F, uLft, &atLft);
        MakeUnit (ARRAY_HC4F, uRht, &atRht);

        // Multiply by the left and right units
        aplRht = MulHC4F_RE (atLft.aplHC4F, MulHC4F_RE (aplRht, atRht.aplHC4F));
    } __except (CheckHCDim (GetExceptionInformation (), 2))
    {
        // Restore original []CT
        SetQuadCT (fQuadCTOld);

        RaiseException (MyGetExceptionCode (), 0, 0, NULL);
    } // End __try/__except

    // Restore original []CT
    SetQuadCT (fQuadCTOld);

    return aplRht;
} // End GcdHC4F


//***************************************************************************
//  $GcdHC1R
//
//  GCD (Greatest Common Divisor) for aplRats
//***************************************************************************

APLRAT GcdHC1R
    (APLRAT aplRatLft,
     APLRAT aplRatRht)

{
    APLRAT aplTmp = {0},
           aplLft = {0},
           aplRht = {0};

    // Initialize the temp
    mpq_init (&aplTmp);
    mpq_init (&aplLft);
    mpq_init (&aplRht);

    // Ensure both arguments are non-negative
    mpq_abs (&aplLft, &aplRatLft);
    mpq_abs (&aplRht, &aplRatRht);

    while (!IsMpq0 (&aplLft))
    {
////////aplTmp = aplLft;
////////aplLft = aplRht % aplLft;
////////aplRht = aplTmp;

        mpq_set (&aplTmp, &aplLft);
        mpq_mod (&aplLft, &aplRht, &aplLft);
        mpq_set (&aplRht, &aplTmp);
    } // End WHILE

    // The sign of the result is non-negative
    mpq_abs (&aplRht, &aplRht);

    // We no longer need this storage
////Myq_clear (&aplRht);        // Returned as result
    Myq_clear (&aplLft);
    Myq_clear (&aplTmp);

    return aplRht;
} // End GcdHC1R


//***************************************************************************
//  $GcdHC2R
//
//  GCD (Greatest Common Divisor) for HC2R
//***************************************************************************

APLHC2R GcdHC2R
    (APLHC2R aplHC2RLft,
     APLHC2R aplHC2RRht)

{
    APLHC2R  aplTmp = {0},
             aplMod,
             aplMul,
             aplLft = {0},
             aplRht = {0};
    ALLTYPES atRht = {0};
    UINT     uIndex,                    // Accumulator for negative signs
             uRht;                      // Index of right unit
    int      i;                         // Loop counter
    APLFLOAT fQuadCTOld,                // Original []CT
             fQuadCTNew = 1E-10,        // Default  []CT for comparisons and L|R
             fQuadCTvL  = 4E-15;        // Default  []CT for comparisons and L|R
    APLVFP   aplMagLft;                 // Temps for magnitude
    int      iRet;                      // Comparison result

    // Save old and set new []CT
    fQuadCTOld = GetQuadCT (); SetQuadCT (fQuadCTNew);

    // Initialize to 0/1
    mphc2r_init (&aplTmp);

    // Copy to temp vars
    mphc2r_init_set (&aplLft, &aplHC2RLft);
    mphc2r_init_set (&aplRht, &aplHC2RRht);

    __try
    {
        do
        {
            // T = L
            mphc2r_set (&aplTmp, &aplLft);

            // L = L | R
            aplMod = ModHC2R (aplLft, aplRht);
            mphc2r_set (&aplLft, &aplMod);
            Myhc2r_clear (&aplMod);

            // R = T
            mphc2r_set (&aplRht, &aplTmp);

            // Calculate the magnitude
            aplMagLft = MagHC2R (aplLft);

            // Compare the magnitude with a small number
            iRet = mpfr_cmp_d (&aplMagLft, fQuadCTvL);

            // We no longer need this temp
            Myhc1v_clear (&aplMagLft);
        } while (iRet > 0);

        // Rotate the result into the first quadrant

        // Loop through the parts of the result looking for negative values
        for (uIndex = 0, i = 2 - 1; i >= 0; i--)
            uIndex = (3 * uIndex) + 1 + signumrat (&aplRht.parts[i]);
        // Get the table index
        uRht = gGcdRotateTab2[uIndex];

        // Make the righthand unit
        MakeUnit (ARRAY_HC2R, uRht, &atRht);

        // Multiply on the right by the righthand unit
        aplMul = MulHC2R_RE (aplRht, atRht.aplHC2R);

        // Save in the result
        mphc2r_set (&aplRht, &aplMul);

        // We no longer need this resource
        Myhc2r_clear (&atRht.aplHC2R);
        Myhc2r_clear (&aplMul       );
        Myhc2r_clear (&aplMod       );
        Myhc2r_clear (&aplTmp       );
////////Myhc2r_clear (&aplRht       );      // Returned as result
        Myhc2r_clear (&aplLft       );
    } __except (CheckHCDim (GetExceptionInformation (), 2))
    {
        // Restore original []CT
        SetQuadCT (fQuadCTOld);

        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    } // End __try/__except

    // Restore original []CT
    SetQuadCT (fQuadCTOld);

    return aplRht;
} // End GcdHC2R


//***************************************************************************
//  $GcdHC4R
//
//  GCD (Greatest Common Divisor) for HC4R
//***************************************************************************

APLHC4R GcdHC4R
    (APLHC4R aplHC4RLft,
     APLHC4R aplHC4RRht)

{
    APLHC4R  aplTmp = {0},
             aplMod,
             aplMulLft,
             aplMulRht,
             aplLft = {0},
             aplRht = {0};
    ALLTYPES atLft = {0},
             atRht = {0};
    UINT     uIndex,                    // Accumulator for negative signs
             uLft,                      // Index of left unit
             uRht;                      // ...      right ...
    int      i;                         // Loop counter
    APLFLOAT fQuadCTOld,                // Original []CT
             fQuadCTNew = 1E-10,        // Default  []CT for comparisons and L|R
             fQuadCTvL  = 4E-15;        // Default  []CT for comparisons and L|R
    APLVFP   aplMagLft;                 // Temps for magnitude
    int      iRet;                      // Comparison result

    // Save old and set new []CT
    fQuadCTOld = GetQuadCT (); SetQuadCT (fQuadCTNew);

    // Initialize to 0/1
    mphc4r_init (&aplTmp);

    // Copy to temp vars
    mphc4r_init_set (&aplLft, &aplHC4RLft);
    mphc4r_init_set (&aplRht, &aplHC4RRht);

    __try
    {
        do
        {
            // T = L
            mphc4r_set (&aplTmp, &aplLft);

            // L = L | R
            aplMod = ModHC4R (aplLft, aplRht);
            mphc4r_set (&aplLft, &aplMod);
            Myhc4r_clear (&aplMod);

            // R = T
            mphc4r_set (&aplRht, &aplTmp);

            // Calculate the magnitude
            aplMagLft = MagHC4R (aplLft);

            // Compare the magnitude with a small number
            iRet = mpfr_cmp_d (&aplMagLft, fQuadCTvL);

            // We no longer need this temp
            Myhc1v_clear (&aplMagLft);
        } while (iRet > 0);

        // Rotate the result into the first bi-quadrant

        // Loop through the parts of the result looking for negative values
        for (uIndex = 0, i = 4 - 1; i >= 0; i--)
            uIndex = (3 * uIndex) + 1 + signumrat (&aplRht.parts[i]);
        // Get the table indices
        uLft = gGcdRotateTab4[uIndex][0];
        uRht = gGcdRotateTab4[uIndex][1];

        // Make the left & right units
        MakeUnit (ARRAY_HC4R, uLft, &atLft);
        MakeUnit (ARRAY_HC4R, uRht, &atRht);

        // Multiply on the right by the righthand unit
        aplMulRht = MulHC4R_RE (aplRht, atRht.aplHC4R);

        // Multiply on the left by the lefthand unit
        aplMulLft = MulHC4R_RE (atLft.aplHC4R, aplMulRht);

        // Save in the result
        mphc4r_set (&aplRht, &aplMulLft);

        // We no longer need these resources
        Myhc4r_clear (&atRht.aplHC4R);
        Myhc4r_clear (&atLft.aplHC4R);
        Myhc4r_clear (&aplMulLft    );
        Myhc4r_clear (&aplMulRht    );
        Myhc4r_clear (&aplTmp       );
////////Myhc4r_clear (&aplRht       );      // Returned as result
        Myhc4r_clear (&aplLft       );
    } __except (CheckHCDim (GetExceptionInformation (), 4))
    {
        // Restore original []CT
        SetQuadCT (fQuadCTOld);

        RaiseException (EXCEPTION_RESULT_HC4V, 0, 0, NULL);
    } // End __try/__except

    // Restore original []CT
    SetQuadCT (fQuadCTOld);

    return aplRht;
} // End GcdHC4R


//***************************************************************************
//  $GcdHC1V
//
//  GCD (Greatest Common Divisor) for aplVfps
//***************************************************************************

APLVFP GcdHC1V
    (APLVFP     aplVfpLft,
     APLVFP     aplVfpRht)

{
    APLVFP aplTmp = {0},
           aplLft = {0},
           aplRht = {0};

    // Initialize the temp to 0
    mpfr_init0 (&aplTmp);
    mpfr_init0 (&aplLft);
    mpfr_init0 (&aplRht);

    // Ensure both arguments are non-negative
    mpfr_abs (&aplLft, &aplVfpLft, MPFR_RNDN);
    mpfr_abs (&aplRht, &aplVfpRht, MPFR_RNDN);

    while (mpfr_cmp_d (&aplLft, GCD_CT) >= 0)
    {
////////aplTmp = aplLft;
////////aplLft = aplRht % aplLft;
////////aplRht = aplTmp;

        mpfr_copy (&aplTmp, &aplLft);
        mpfr_mod  (&aplLft, &aplRht, &aplLft);
        mpfr_copy (&aplRht, &aplTmp);
    } // End WHILE

    // The sign of the result is non-negative
    mpfr_abs (&aplRht, &aplRht, MPFR_RNDN);

    // We no longer need this storage
////Myf_clear (&aplRht);        // Returned as result
    Myf_clear (&aplLft);
    Myf_clear (&aplTmp);

    return aplRht;
} // End GcdHC1V


//***************************************************************************
//  $GcdHC2V
//
//  GCD (Greatest Common Divisor) for HC2V
//***************************************************************************

APLHC2V GcdHC2V
    (APLHC2V aplHC2VLft,
     APLHC2V aplHC2VRht)

{
    APLHC2V  aplTmp = {0},
             aplMod,
             aplMul,
             aplLft = {0},
             aplRht = {0};
    ALLTYPES atRht = {0};
    UINT     uIndex,                    // Accumulator for negative signs
             uRht;                      // Index of right unit
    int      i;                         // Loop counter
    APLFLOAT fQuadCTOld,                // Original []CT
             fQuadCTNew = 1E-10,        // Default  []CT for comparisons and L|R
             fQuadCTvL  = 4E-15;        // Default  []CT for comparisons and L|R
    APLVFP   aplMagLft;                 // Temps for magnitude
    int      iRet;                      // Comparison result

    // Save old and set new []CT
    fQuadCTOld = GetQuadCT (); SetQuadCT (fQuadCTNew);

    // Initialize to 0
    mphc2v_init0 (&aplTmp);

    // Copy to temp vars
    mphc2v_init_set (&aplLft, &aplHC2VLft);
    mphc2v_init_set (&aplRht, &aplHC2VRht);

    __try
    {
        do
        {
            // T = L
            mphc2v_set (&aplTmp, &aplLft);

            // L = L | R
            aplMod = ModHC2V (aplLft, aplRht);
            mphc2v_set (&aplLft, &aplMod);
            Myhc2v_clear (&aplMod);

            // R = T
            mphc2v_set (&aplRht, &aplTmp);

            // Calculate the magnitude
            aplMagLft = MagHC2V (aplLft);

            // Compare the magnitude with a small number
            iRet = mpfr_cmp_d (&aplMagLft, fQuadCTvL);

            // We no longer need this temp
            Myhc1v_clear (&aplMagLft);
        } while (iRet > 0);

        // Rotate the result into the first quadrant

        // Loop through the parts of the result looking for negative values
        for (uIndex = 0, i = 2 - 1; i >= 0; i--)
            uIndex = (3 * uIndex) + 1 + signumvfp (&aplRht.parts[i]);
        // Get the table index
        uRht = gGcdRotateTab2[uIndex];

        // Make the righthand unit
        MakeUnit (ARRAY_HC2V, uRht, &atRht);

        // Multiply on the right by the righthand unit
        aplMul = MulHC2V_RE (aplRht, atRht.aplHC2V);

        // Save in the result
        mphc2v_set (&aplRht, &aplMul);

        // We no longer need this resource
        Myhc2v_clear (&atRht.aplHC2V);
        Myhc2v_clear (&aplMul       );
        Myhc2v_clear (&aplMod       );
        Myhc2v_clear (&aplTmp       );
////////Myhc2v_clear (&aplRht       );      // Returned as result
        Myhc2v_clear (&aplLft       );
    } __except (CheckHCDim (GetExceptionInformation (), 2))
    {
        // Restore original []CT
        SetQuadCT (fQuadCTOld);

        RaiseException (MyGetExceptionCode (), 0, 0, NULL);
    } // End __try/__except

    // Restore original []CT
    SetQuadCT (fQuadCTOld);

    return aplRht;
} // End GcdHC2V


//***************************************************************************
//  $GcdHC4V
//
//  GCD (Greatest Common Divisor) for HC4V
//***************************************************************************

APLHC4V GcdHC4V
    (APLHC4V aplHC4VLft,
     APLHC4V aplHC4VRht)

{
    APLHC4V  aplTmp = {0},
             aplMod,
             aplMulLft,
             aplMulRht,
             aplLft = {0},
             aplRht = {0};
    ALLTYPES atLft = {0},
             atRht = {0};
    UINT     uIndex,                    // Accumulator for negative signs
             uLft,                      // Index of left unit
             uRht;                      // ...      right ...
    int      i;                         // Loop counter
    APLFLOAT fQuadCTOld,                // Original []CT
             fQuadCTNew = 1E-10,        // Default  []CT for comparisons and L|R
             fQuadCTvL  = 4E-15;        // Default  []CT for comparisons and L|R
    APLVFP   aplMagLft;                 // Temps for magnitude
    int      iRet;                      // Comparison result

    // Save old and set new []CT
    fQuadCTOld = GetQuadCT (); SetQuadCT (fQuadCTNew);

    // Initialize to 0/1
    mphc4v_init0 (&aplTmp);

    // Copy to temp vars
    mphc4v_init_set (&aplLft, &aplHC4VLft);
    mphc4v_init_set (&aplRht, &aplHC4VRht);

    __try
    {
        do
        {
            // T = L
            mphc4v_set (&aplTmp, &aplLft);

            // L = L | R
            aplMod = ModHC4V (aplLft, aplRht);
            mphc4v_set (&aplLft, &aplMod);
            Myhc4v_clear (&aplMod);

            // R = T
            mphc4v_set (&aplRht, &aplTmp);

            // Calculate the magnitude
            aplMagLft = MagHC4V (aplLft);

            // Compare the magnitude with a small number
            iRet = mpfr_cmp_d (&aplMagLft, fQuadCTvL);

            // We no longer need this temp
            Myhc1v_clear (&aplMagLft);
        } while (iRet > 0);

        // Rotate the result into the first bi-quadrant

        // Loop through the parts of the result looking for negative values
        for (uIndex = 0, i = 4 - 1; i >= 0; i--)
            uIndex = (3 * uIndex) + 1 + signumvfp (&aplRht.parts[i]);
        // Get the table indices
        uLft = gGcdRotateTab4[uIndex][0];
        uRht = gGcdRotateTab4[uIndex][1];

        // Make the left & right units
        MakeUnit (ARRAY_HC4V, uLft, &atLft);
        MakeUnit (ARRAY_HC4V, uRht, &atRht);

        // Multiply on the right by the righthand unit
        aplMulRht = MulHC4V_RE (aplRht, atRht.aplHC4V);

        // Multiply on the left by the lefthand unit
        aplMulLft = MulHC4V_RE (atLft.aplHC4V, aplMulRht);

        // Save in the result
        mphc4v_set (&aplRht, &aplMulLft);

        // We no longer need these resources
        Myhc4v_clear (&atRht.aplHC4V);
        Myhc4v_clear (&atLft.aplHC4V);
        Myhc4v_clear (&aplMulLft    );
        Myhc4v_clear (&aplMulRht    );
        Myhc4v_clear (&aplTmp       );
////////Myhc4v_clear (&aplRht       );      // Returned as result
        Myhc4v_clear (&aplLft       );
    } __except (CheckHCDim (GetExceptionInformation (), 4))
    {
        // Restore original []CT
        SetQuadCT (fQuadCTOld);

        RaiseException (MyGetExceptionCode (), 0, 0, NULL);
    } // End __try/__except

    // Restore original []CT
    SetQuadCT (fQuadCTOld);

    return aplRht;
} // End GcdHC4V


//***************************************************************************
//  $PrimFnDydDownCaretBisBvB
//
//  Primitive scalar function dyadic DownCaret:  B {is} B fn B
//***************************************************************************

void PrimFnDydDownCaretBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean | lpatRht->aplBoolean)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydDownCaretBisBvB


//***************************************************************************
//  $PrimFnDydDownCaretB64isB64vB64
//
//  Primitive scalar function dyadic DownCaret:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydDownCaretB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretB64isB64vB64


//***************************************************************************
//  $PrimFnDydDownCaretB32isB32vB32
//
//  Primitive scalar function dyadic DownCaret:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydDownCaretB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretB32isB32vB32


//***************************************************************************
//  $PrimFnDydDownCaretB16isB16vB16
//
//  Primitive scalar function dyadic DownCaret:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydDownCaretB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretB16isB16vB16


//***************************************************************************
//  $PrimFnDydDownCaretB08isB08vB08
//
//  Primitive scalar function dyadic DownCaret:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydDownCaretB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft | aplBooleanRht);
} // End PrimFnDydDownCaretB08isB08vB08


//***************************************************************************
//  $PrimFnDydDownCaretIisIvI
//
//  Primitive scalar function dyadic DownCaret:  I {is} I fn I
//***************************************************************************

void PrimFnDydDownCaretIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = GcdHC1I (lpatLft->aplInteger, lpatRht->aplInteger);
} // End PrimFnDydDownCaretIisIvI


//***************************************************************************
//  $PrimFnDydDownCaretFisIvI
//
//  Primitive scalar function dyadic DownCaret:  F {is} I fn I
//***************************************************************************

void PrimFnDydDownCaretFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = (APLFLOAT) GcdHC1I (lpatLft->aplInteger, lpatRht->aplInteger);
} // End PrimFnDydDownCaretFisIvI


//***************************************************************************
//  $PrimFnDydDownCaretFisFvF
//
//  Primitive scalar function dyadic DownCaret:  F {is} F fn F
//***************************************************************************

void PrimFnDydDownCaretFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  gcd (PoM_, 0)  or  gcd (0, PoM_)
    if ((IsFltInfinity (lpatLft->aplFloat) && (lpatRht->aplFloat EQ 0))
     || (IsFltInfinity (lpatRht->aplFloat) && (lpatLft->aplFloat EQ 0)))
        lpMemRes[uRes] = TranslateQuadICIndex (lpatLft->aplFloat,
                                               ICNDX_0GCDInf,
                                               lpatRht->aplFloat,
                                               SIGN_APLFLOAT (lpatLft->aplFloat));
    else
    {
        // Check for special cases:  gcd (PoM_, N)  or  gcd (N, PoM_)
        if (IsFltInfinity (lpatLft->aplFloat)
         || IsFltInfinity (lpatRht->aplFloat))
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        lpMemRes[uRes] = GcdHC1F (lpatLft->aplFloat, lpatRht->aplFloat);
    } // End IF/ELSE
} // End PrimFnDydDownCaretFisFvF


//***************************************************************************
//  $PrimFnDydDownCaretRisRvR
//
//  Primitive scalar function dyadic DownCaret:  R {is} fn R fn R
//***************************************************************************

void PrimFnDydDownCaretRisRvR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  gcd (PoM_, 0)  or  gcd (0, PoM_)
    if ((mpq_inf_p (&lpatLft->aplRat) && IsMpq0 (&lpatRht->aplRat))
     || (mpq_inf_p (&lpatRht->aplRat) && IsMpq0 (&lpatLft->aplRat)))
    {
        lpMemRes[uRes] = *mpq_QuadICValue (&lpatLft->aplRat,
                                            ICNDX_0GCDInf,
                                           &lpatRht->aplRat,
                                           &lpMemRes[uRes],
                                            mpq_sgn (&lpatLft->aplRat) EQ -1);
        return;
    } // End IF

    // Check for special cases:  gcd (PoM_, N)  or  gcd (N, PoM_)
    if (mpq_inf_p (&lpatLft->aplRat)
     || mpq_inf_p (&lpatRht->aplRat))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // If the denominators are both 1, ...
    if (IsMpz1 (mpq_denref (&lpatLft->aplRat))
     && IsMpz1 (mpq_denref (&lpatRht->aplRat)))
    {
        // Initialize the result to 0/1
        mpq_init (&lpMemRes[uRes]);

        // Calculate the GCD on the numerators
        mpz_gcd (mpq_numref (&lpMemRes[uRes]),
                 mpq_numref (&lpatLft->aplRat),
                 mpq_numref (&lpatRht->aplRat));
        // The sign of the result is the sign of the left arg
        if (mpz_sgn (mpq_numref (&lpatLft->aplRat)) EQ -1)
            mpz_neg (mpq_numref (&lpMemRes[uRes]), mpq_numref (&lpMemRes[uRes]));

        // Set the denominator to 1
        mpz_set_ui (mpq_denref (&lpMemRes[uRes]), 1);
    } else
        lpMemRes[uRes] = GcdHC1R (lpatLft->aplRat, lpatRht->aplRat);
} // End PrimFnDydDownCaretRisRvR


//***************************************************************************
//  $PrimFnDydDownCaretVisRvR
//
//  Primitive scalar function dyadic DownCaret:  V {is} fn R fn R
//***************************************************************************

void PrimFnDydDownCaretVisRvR
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

    PrimFnDydDownCaretVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydDownCaretVisRvR


//***************************************************************************
//  $PrimFnDydDownCaretVisVvV
//
//  Primitive scalar function dyadic DownCaret:  V {is} fn V fn V
//***************************************************************************

void PrimFnDydDownCaretVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  gcd (PoM_, 0)  or  gcd (0, PoM_)
    if ((mpfr_inf_p (&lpatLft->aplVfp) && IsMpf0 (&lpatRht->aplVfp))
     || (mpfr_inf_p (&lpatRht->aplVfp) && IsMpf0 (&lpatLft->aplVfp)))
        lpMemRes[uRes] = *mpfr_QuadICValue (&lpatLft->aplVfp,
                                             ICNDX_0GCDInf,
                                            &lpatRht->aplVfp,
                                            &lpMemRes[uRes],
                                             SIGN_APLVFP (&lpatLft->aplVfp));
    else
    {
        // Check for special cases:  gcd (PoM_, N)  or  gcd (N, PoM_)
        if (mpfr_inf_p (&lpatLft->aplVfp)
         || mpfr_inf_p (&lpatRht->aplVfp))
            RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        lpMemRes[uRes] = GcdHC1V (lpatLft->aplVfp, lpatRht->aplVfp);
    } // End IF/ELSE
} // End PrimFnDydDownCaretVisVvV


//***************************************************************************
//  $PrimFnDydDownCaretHC2IisHC2IvHC2I
//
//  Primitive scalar function dyadic DownCaret:  HC2I {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydDownCaretHC2IisHC2IvHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = GcdHC2I (lpatLft->aplHC2I, lpatRht->aplHC2I);
} // End PrimFnDydDownCaretHC2IisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydDownCaretHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic DownCaret:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydDownCaretHC2FisHC2IvHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert the left & right args to the result type
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (lpatRht, 0, &atRht);

    // Save in the result
    lpMemRes[uRes] = GcdHC2F (atLft.aplHC2F, atRht.aplHC2F);
} // End PrimFnDydDownCaretHC2FisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydDownCaretHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic DownCaret:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydDownCaretHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = GcdHC2F (lpatLft->aplHC2F, lpatRht->aplHC2F);
} // End PrimFnDydDownCaretHC2FisHC2FvHC2F


//***************************************************************************
//  $PrimFnDydDownCaretHC2RisHC2RvHC2R
//
//  Primitive scalar function dyadic DownCaret:  HC2R {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydDownCaretHC2RisHC2RvHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = GcdHC2R (lpatLft->aplHC2R, lpatRht->aplHC2R);
} // End PrimFnDydDownCaretHC2RisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydDownCaretHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic DownCaret:  HC2V {is} fn HC2R fn HC2R
//***************************************************************************

void PrimFnDydDownCaretHC2VisHC2RvHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
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

    PrimFnDydDownCaretHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
    Myhc2v_clear (&atLft.aplHC2V);
} // End PrimFnDydDownCaretHC2VisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydDownCaretHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic DownCaret:  HC2V {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydDownCaretHC2VisHC2VvHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = GcdHC2V (lpatLft->aplHC2V, lpatRht->aplHC2V);
} // End PrimFnDydDownCaretHC2VisHC2VvHC2V


//***************************************************************************
//  $PrimFnDydDownCaretHC4IisHC4IvHC4I
//
//  Primitive scalar function dyadic DownCaret:  HC4I {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydDownCaretHC4IisHC4IvHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = GcdHC4I (lpatLft->aplHC4I, lpatRht->aplHC4I);
} // End PrimFnDydDownCaretHC4IisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydDownCaretHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic DownCaret:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydDownCaretHC4FisHC4IvHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},
             atRht = {0};

    // Convert the left & right args to the result type
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatLft, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (lpatRht, 0, &atRht);

    // Save in the result
    lpMemRes[uRes] = GcdHC4F (atLft.aplHC4F, atRht.aplHC4F);
} // End PrimFnDydDownCaretHC4FisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydDownCaretHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic DownCaret:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydDownCaretHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = GcdHC4F (lpatLft->aplHC4F, lpatRht->aplHC4F);
} // End PrimFnDydDownCaretHC4FisHC4FvHC4F


//***************************************************************************
//  $PrimFnDydDownCaretHC4RisHC4RvHC4R
//
//  Primitive scalar function dyadic DownCaret:  HC4R {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydDownCaretHC4RisHC4RvHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = GcdHC4R (lpatLft->aplHC4R, lpatRht->aplHC4R);
} // End PrimFnDydDownCaretHC4RisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydDownCaretHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic DownCaret:  HC4V {is} fn HC4R fn HC4R
//***************************************************************************

void PrimFnDydDownCaretHC4VisHC4RvHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
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

    PrimFnDydDownCaretHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
    Myhc4v_clear (&atLft.aplHC4V);
} // End PrimFnDydDownCaretHC4VisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydDownCaretHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic DownCaret:  HC4V {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydDownCaretHC4VisHC4VvHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = GcdHC4V (lpatLft->aplHC4V, lpatRht->aplHC4V);
} // End PrimFnDydDownCaretHC4VisHC4VvHC4V


//***************************************************************************
//  End of File: pf_dcaret.c
//***************************************************************************
