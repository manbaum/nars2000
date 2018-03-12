//***************************************************************************
//  NARS2000 -- Primitive Function -- UpCaret
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
PRIMSPEC PrimSpecUpCaret = {
    // Monadic functions
    &PrimFnMonValenceError_EM,
    NULL,   // &PrimSpecUpCaretStorageTypeMon, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretAPA_EM, -- Can't happen w/UpCaret

    // Monadic Boolean result functions
    NULL,   // &PrimFnMonUpCaretBisB, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretBisI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretBisF, -- Can't happen w/UpCaret

    // Monadic non-Boolean result functions (indexable)
    NULL,   // &PrimFnMonUpCaretIisI, -- Can't happend w/UpCaret
    NULL,   // &PrimFnMonUpCaretIisF, -- Can't happend w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisF, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretRisR, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretVisR, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretVisV, -- Can't happen w/UpCaret

    // Monadic Hypercomplex functions
    NULL,   // &PrimFnMonUpCaretHC2IisHC2I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC2IisHC2F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC2FisHC2I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC2FisHC2F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC2RisHC2R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC2VisHC2R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC2VisHC2V, -- Can't happen w/UpCaret

    NULL,   // &PrimFnMonUpCaretHC4IisHC4I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC4IisHC4F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC4FisHC4I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC4FisHC4F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC4RisHC4R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC4VisHC4R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC4VisHC4V, -- Can't happen w/UpCaret

    NULL,   // &PrimFnMonUpCaretHC8IisHC8I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC8IisHC8F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC8FisHC8I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC8FisHC8F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC8RisHC8R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC8VisHC8R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretHC8VisHC8V, -- Can't happen w/UpCaret

    // Monadic FLT/VFP result HC arg functions (indexable)
    NULL,   // &PrimFnMonUpCaretFisHC2I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC2F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC2R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC2V, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC4I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC4F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC4R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC4V, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC8I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC8F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC8R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretFisHC8V, -- Can't happen w/UpCaret

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecUpCaretStorageTypeDyd,
    NULL,   // &PrimFnDydUpCaretAPA_EM, -- Can't happen w/UpCaret

    // Dyadic Boolean result functions
    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydUpCaretBisIvI, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisFvF, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisCvC, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisHvH, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisRvR, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisVvV, -- Can't happen w/UpCaret

    NULL,   // &PrimFnDydUpCaretBisHC2IvHC2I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisHC2FvHC2F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisHC2RvHC2R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisHC2VvHC2V, -- Can't happen w/UpCaret

    NULL,   // &PrimFnDydUpCaretBisHC4IvHC4I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisHC4FvHC4F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisHC4RvHC4R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisHC4VvHC4V, -- Can't happen w/UpCaret

    NULL,   // &PrimFnDydUpCaretBisHC8IvHC8I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisHC8FvHC8F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisHC8RvHC8R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretBisHC8VvHC8V, -- Can't happen w/UpCaret

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydUpCaretIisIvI,
    NULL,   // &PrimFnDydUpCaretIisFvF, -- Can't happen w/UpCaret
    &PrimFnDydUpCaretFisIvI,
    &PrimFnDydUpCaretFisFvF,
    &PrimFnDydUpCaretRisRvR,
    &PrimFnDydUpCaretVisRvR,
    &PrimFnDydUpCaretVisVvV,

    &PrimFnDydUpCaretHC2IisHC2IvHC2I,
    NULL,   // &PrimFnDydUpCaretHC2IisHC2FvHC2F, -- Can't happen w/UpCaret
    &PrimFnDydUpCaretHC2FisHC2IvHC2I,
    &PrimFnDydUpCaretHC2FisHC2FvHC2F,
    &PrimFnDydUpCaretHC2RisHC2RvHC2R,
    &PrimFnDydUpCaretHC2VisHC2RvHC2R,
    &PrimFnDydUpCaretHC2VisHC2VvHC2V,

    &PrimFnDydUpCaretHC4IisHC4IvHC4I,
    NULL,   // &PrimFnDydUpCaretHC4IisHC4FvHC4F, -- Can't happen w/UpCaret
    &PrimFnDydUpCaretHC4FisHC4IvHC4I,
    &PrimFnDydUpCaretHC4FisHC4FvHC4F,
    &PrimFnDydUpCaretHC4RisHC4RvHC4R,
    &PrimFnDydUpCaretHC4VisHC4RvHC4R,
    &PrimFnDydUpCaretHC4VisHC4VvHC4V,

    NULL,   // &PrimFnDydUpCaretHC8IisHC8IvHC8I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretHC8IisHC8FvHC8F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretHC8FisHC8IvHC8I, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretHC8FisHC8FvHC8F, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretHC8RisHC8RvHC8R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretHC8VisHC8RvHC8R, -- Can't happen w/UpCaret
    NULL,   // &PrimFnDydUpCaretHC8VisHC8VvHC8V, -- Can't happen w/UpCaret

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonUpCaretB64isB64, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretB32isB32, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretB16isB16, -- Can't happen w/UpCaret
    NULL,   // &PrimFnMonUpCaretB08isB08, -- Can't happen w/UpCaret

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

static LPPRIMSPEC lpPrimSpec = {&PrimSpecUpCaret};
#endif


//***************************************************************************
//  $PrimFnUpCaret_EM_YY
//
//  Primitive function for monadic and dyadic UpCaret (ERROR and "and/LCM")
//***************************************************************************

LPPL_YYSTYPE PrimFnUpCaret_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPCARET
         || lptkFunc->tkData.tkChar EQ UTF16_CIRCUMFLEX);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnUpCaret_EM


//***************************************************************************
//  $PrimSpecUpCaretStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecUpCaretStorageTypeDyd
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
} // End PrimSpecUpCaretStorageTypeDyd


//***************************************************************************
//  $LcmHC1I
//
//  LCM (Least Common Multiple) for aplIntegers
//***************************************************************************

APLINT LcmHC1I
    (APLINT     aplIntegerLft,
     APLINT     aplIntegerRht)

{
    APLINT aplRes,
           aplGcd,
           aplLft,
           aplRht;

    // Ensure both arguments are non-negative
////aplLft = abs64 (aplIntegerLft);
////aplRht = abs64 (aplIntegerRht);
    aplLft =        aplIntegerLft ;
    aplRht =        aplIntegerRht ;

    // Calculate the GCD
    aplGcd = GcdHC1I (aplLft, aplRht);
    if (aplGcd NE 0)
        aplRes = (aplLft * aplRht) / aplGcd;
    else
        aplRes = aplGcd;

////// The sign of the result is the product of the signs of the arguments
////if ((signumint (aplIntegerLft) * signumint (aplIntegerRht)) < 0)
////    return -aplLcm;
////else
////    return  aplLcm;
    return  aplRes;
} // End LcmHC1I


//***************************************************************************
//  $LcmHC2I
//
//  LCM (Least Common Multiple) for aplHC2I
//***************************************************************************

APLHC2I LcmHC2I
    (APLHC2I aplLft,
     APLHC2I aplRht)

{
    APLHC2I aplGcd,
            aplRes;

    __try
    {
        // Calculate the GCD
        aplGcd = GcdHC2I (aplLft, aplRht);
        if (!IsZeroHCxI (&aplGcd, 2))
            aplRes = DivHC2I_RE (MulHC2I_RE (aplLft, aplRht), aplGcd);
        else
            aplRes = aplGcd;
    } __except (CheckHCDim (GetExceptionInformation (), 2))
    {
        RaiseException (EXCEPTION_RESULT_HC2F, 0, 0, NULL);
    } // End __try/__except

    return aplRes;
} // End LcmHC2I


//***************************************************************************
//  $LcmHC4I
//
//  LCM (Least Common Multiple) for aplHC4I
//***************************************************************************

APLHC4I LcmHC4I
    (APLHC4I aplLft,
     APLHC4I aplRht)

{
    APLHC4I aplGcd,
            aplRes;

    __try
    {
        // Calculate the GCD
        aplGcd = GcdHC4I (aplLft, aplRht);
        if (!IsZeroHCxI (&aplGcd, 4))
            aplRes = DivHC4I_RE (MulHC4I_RE (aplLft, aplRht), aplGcd);
        else
            aplRes = aplGcd;
    } __except (CheckHCDim (GetExceptionInformation (), 4))
    {
        RaiseException (EXCEPTION_RESULT_HC4F, 0, 0, NULL);
    } // End __try/__except

    return aplRes;
} // End LcmHC4I


//***************************************************************************
//  $LcmHC1F
//
//  LCM (Least Common Multiple) for aplFloats
//***************************************************************************

APLFLOAT LcmHC1F
    (APLFLOAT aplLft,
     APLFLOAT aplRht)

{
    APLFLOAT aplGcd;

    // Calculate the GCD
    aplGcd = GcdHC1F (aplLft, aplRht);
    if (aplGcd >= GCD_CT)
        return (aplLft * aplRht) / aplGcd;
    else
        return aplGcd;
} // End LcmHC1F


//***************************************************************************
//  $LcmHC2F
//
//  LCM (Least Common Multiple) for aplHC2F
//***************************************************************************

APLHC2F LcmHC2F
    (APLHC2F aplLft,
     APLHC2F aplRht)

{
    APLHC2F aplGcd,
            aplRes;

    __try
    {
        // Calculate the GCD
        aplGcd = GcdHC2F (aplLft, aplRht);
        if (!IsZeroHCxI (&aplGcd, 2))
            aplRes = DivHC2F_RE (MulHC2F_RE (aplLft, aplRht), aplGcd);
        else
            aplRes = aplGcd;
    } __except (CheckHCDim (GetExceptionInformation (), 2))
    {
        RaiseException (EXCEPTION_LIMIT_ERROR, 0, 0, NULL);
    } // End __try/__except

    return aplRes;
} // End LcmHC2F


//***************************************************************************
//  $LcmHC4F
//
//  LCM (Least Common Multiple) for aplHC4F
//***************************************************************************

APLHC4F LcmHC4F
    (APLHC4F aplLft,
     APLHC4F aplRht)

{
    APLHC4F aplGcd,
            aplRes;

    __try
    {
        // Calculate the GCD
        aplGcd = GcdHC4F (aplLft, aplRht);
        if (!IsZeroHCxI (&aplGcd, 4))
            aplRes = DivHC4F_RE (MulHC4F_RE (aplLft, aplRht), aplGcd);
        else
            aplRes = aplGcd;
    } __except (CheckHCDim (GetExceptionInformation (), 4))
    {
        RaiseException (EXCEPTION_LIMIT_ERROR, 0, 0, NULL);
    } // End __try/__except

    return aplRes;
} // End LcmHC4F


//***************************************************************************
//  $LcmHC1R
//
//  LCM (Least Common Multiple) for aplRats
//***************************************************************************

APLRAT LcmHC1R
    (APLRAT aplLft,
     APLRAT aplRht)

{
    APLRAT aplRes = {0},
           aplGcd;

    // Initialize temp to 0/1
    mpq_init (&aplRes);

    // Calculate the GCD
    aplGcd = GcdHC1R (aplLft, aplRht);
    if (!IsMpq0 (&aplGcd))
    {
////////aplTmp = (aplLft * aplRht) / aplRes;

        mpq_mul (&aplRes, &aplLft, &aplRht);
        mpq_div (&aplRes, &aplRes, &aplGcd);
    } // End IF

    // We no longer need this storage
    Myq_clear (&aplGcd);
////Myq_clear (&aplRes);        // Returned as result

    return aplRes;
} // End LcmHC1R


//***************************************************************************
//  $LcmHC2R
//
//  LCM (Least Common Multiple) for aplHC2R
//***************************************************************************

APLHC2R LcmHC2R
    (APLHC2R aplLft,
     APLHC2R aplRht)

{
    APLHC2R aplRes,
            aplGcd,
            aplMul;
    APLVFP  aplMag;

    __try
    {
        // Calculate the GCD
        aplGcd = GcdHC2R (aplLft, aplRht);

        // Calculate the magnitude
        aplMag = MagHC2R (aplGcd);

        if (mpfr_cmp_d (&aplMag, GCD_CT) >= 0)
        {
            aplMul = MulHC2R_RE (aplLft, aplRht);
            aplRes = DivHC2R_RE (aplMul, aplGcd);

            // We no longer need these resources
            Myhc2r_clear (&aplMul);
            Myhc2r_clear (&aplGcd);
        } else
            aplRes = aplGcd;

        // We no longer need these resources
        Myf_clear    (&aplMag);
////////Myhc2r_clear (&aplRes);     // Returned as result
    } __except (CheckHCDim (GetExceptionInformation (), 2))
    {
        // We no longer need these resources
        Myf_clear    (&aplMag);
////////Myhc2r_clear (&aplRes);     // Returned as result

        RaiseException (EXCEPTION_RESULT_HC2V, 0, 0, NULL);
    } // End __try/__except

    return aplRes;
} // End LcmHC2R


//***************************************************************************
//  $LcmHC4R
//
//  LCM (Least Common Multiple) for aplHC4R
//***************************************************************************

APLHC4R LcmHC4R
    (APLHC4R aplLft,
     APLHC4R aplRht)

{
    APLHC4R aplRes,
            aplGcd,
            aplMul;
    APLVFP  aplMag;

    __try
    {
        // Calculate the GCD
        aplGcd = GcdHC4R (aplLft, aplRht);

        // Calculate the magnitude
        aplMag = MagHC4R (aplGcd);

        if (mpfr_cmp_d (&aplMag, GCD_CT) >= 0)
        {
            aplMul = MulHC4R_RE (aplLft, aplRht);
            aplRes = DivHC4R_RE (aplMul, aplGcd);

            // We no longer need these resources
            Myhc4r_clear (&aplMul);
            Myhc4r_clear (&aplGcd);
        } else
            aplRes = aplGcd;

        // We no longer need these resources
        Myf_clear    (&aplMag);
////////Myhc4r_clear (&aplRes);     // Returned as result
    } __except (CheckHCDim (GetExceptionInformation (), 4))
    {
        // We no longer need these resources
        Myf_clear    (&aplMag);
////////Myhc4r_clear (&aplRes);     // Returned as result

        RaiseException (EXCEPTION_RESULT_HC4V, 0, 0, NULL);
    } // End __try/__except

    return aplRes;
} // End LcmHC4R


//***************************************************************************
//  $LcmHC1V
//
//  LCM (Least Common Multiple) for aplVfps
//***************************************************************************

APLVFP LcmHC1V
    (APLVFP aplLft,
     APLVFP aplRht)

{
    APLVFP aplRes = {0},
           aplGcd;
    APLVFP aplMag;

    // Initialize the temp to 0
    mpfr_init0 (&aplRes);

    // Calculate the GCD
    aplGcd = GcdHC1V (aplLft, aplRht);

    // Calculate the magnitude
    aplMag = MagVfp (aplGcd);

    if (mpfr_cmp_d (&aplMag, GCD_CT) >= 0)
    {
////////aplTmp = (aplLft * aplRht) / aplGcd;

        mpfr_mul (&aplRes, &aplLft, &aplRht, MPFR_RNDN);
        mpfr_div (&aplRes, &aplRes, &aplGcd, MPFR_RNDN);
    } else
        mpfr_set (&aplRes, &aplGcd, MPFR_RNDN);

    // We no longer need this storage
    Myf_clear (&aplMag);
    Myf_clear (&aplGcd);
////Myf_clear (&aplRes);        // Returned as result

    return aplRes;
} // End LcmHC1V


//***************************************************************************
//  $LcmHC2V
//
//  LCM (Least Common Multiple) for aplHC2V
//***************************************************************************

APLHC2V LcmHC2V
    (APLHC2V aplLft,
     APLHC2V aplRht)

{
    APLHC2V aplRes,
            aplGcd,
            aplMul;
    APLVFP  aplMag;

    __try
    {
        // Calculate the GCD
        aplGcd = GcdHC2V (aplLft, aplRht);

        // Calculate the magnitude
        aplMag = MagHC2V (aplGcd);

        if (mpfr_cmp_d (&aplMag, GCD_CT) >= 0)
        {
            aplMul = MulHC2V_RE (aplLft, aplRht);
            aplRes = DivHC2V_RE (aplMul, aplGcd);

            // We no longer need these resources
            Myhc2v_clear (&aplMul);
            Myhc2v_clear (&aplGcd);
        } else
            aplRes = aplGcd;

        // We no longer need this resource
        Myf_clear    (&aplMag);
////////Myhc2v_clear (&aplRes);     // Returned as result
    } __except (CheckHCDim (GetExceptionInformation (), 2))
    {
        // We no longer need these resources
        Myf_clear    (&aplMag);
////////Myhc2v_clear (&aplRes);     // Returned as result

        RaiseException (EXCEPTION_LIMIT_ERROR, 0, 0, NULL);
    } // End __try/__except

    return aplRes;
} // End LcmHC2V


//***************************************************************************
//  $LcmHC4V
//
//  LCM (Least Common Multiple) for aplHC4V
//***************************************************************************

APLHC4V LcmHC4V
    (APLHC4V aplLft,
     APLHC4V aplRht)

{
    APLHC4V aplRes,
            aplGcd,
            aplMul;
    APLVFP  aplMag;

    __try
    {
        // Calculate the GCD
        aplGcd = GcdHC4V (aplLft, aplRht);

        // Calculate the magnitude
        aplMag = MagHC4V (aplGcd);

        if (mpfr_cmp_d (&aplMag, GCD_CT) >= 0)
        {
            aplMul = MulHC4V_RE (aplLft, aplRht);
            aplRes = DivHC4V_RE (aplMul, aplGcd);

            // We no longer need these resources
            Myhc4v_clear (&aplMul);
            Myhc4v_clear (&aplGcd);
        } else
            aplRes = aplGcd;

        // We no longer need this resource
        Myf_clear    (&aplMag);
////////Myhc4v_clear (&aplRes);     // Returned as result
    } __except (CheckHCDim (GetExceptionInformation (), 4))
    {
        // We no longer need these resources
        Myf_clear    (&aplMag);
////////Myhc4v_clear (&aplRes);     // Returned as result

        RaiseException (EXCEPTION_LIMIT_ERROR, 0, 0, NULL);
    } // End __try/__except

    return aplRes;
} // End LcmHC4V


//***************************************************************************
//  $PrimFnDydUpCaretBisBvB
//
//  Primitive scalar function dyadic UpCaret:  B {is} B fn B
//***************************************************************************

void PrimFnDydUpCaretBisBvB
    (LPAPLBOOL  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes >> LOG2NBIB] |=
      (lpatLft->aplBoolean & lpatRht->aplBoolean)
      << (MASKLOG2NBIB & (UINT) uRes);
} // End PrimFnDydUpCaretBisBvB


//***************************************************************************
//  $PrimFnDydUpCaretB64isB64vB64
//
//  Primitive scalar function dyadic UpCaret:  B64 {is} B64 fn B64
//***************************************************************************

APLB64 PrimFnDydUpCaretB64isB64vB64
    (APLB64     aplBooleanLft,
     APLB64     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB64isB64vB64


//***************************************************************************
//  $PrimFnDydUpCaretB32isB32vB32
//
//  Primitive scalar function dyadic UpCaret:  B32 {is} B32 fn B32
//***************************************************************************

APLB32 PrimFnDydUpCaretB32isB32vB32
    (APLB32     aplBooleanLft,
     APLB32     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB32isB32vB32


//***************************************************************************
//  $PrimFnDydUpCaretB16isB16vB16
//
//  Primitive scalar function dyadic UpCaret:  B16 {is} B16 fn B16
//***************************************************************************

APLB16 PrimFnDydUpCaretB16isB16vB16
    (APLB16     aplBooleanLft,
     APLB16     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB16isB16vB16


//***************************************************************************
//  $PrimFnDydUpCaretB08isB08vB08
//
//  Primitive scalar function dyadic UpCaret:  B08 {is} B08 fn B08
//***************************************************************************

APLB08 PrimFnDydUpCaretB08isB08vB08
    (APLB08     aplBooleanLft,
     APLB08     aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return (aplBooleanLft & aplBooleanRht);
} // End PrimFnDydUpCaretB08isB08vB08


//***************************************************************************
//  $PrimFnDydUpCaretIisIvI
//
//  Primitive scalar function dyadic UpCaret:  I {is} I fn I
//***************************************************************************

void PrimFnDydUpCaretIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = LcmHC1I (lpatLft->aplInteger, lpatRht->aplInteger);
} // End PrimFnDydUpCaretIisIvI


//***************************************************************************
//  $PrimFnDydUpCaretFisIvI
//
//  Primitive scalar function dyadic UpCaret:  F {is} I fn I
//***************************************************************************

void PrimFnDydUpCaretFisIvI
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = (APLFLOAT) LcmHC1I (lpatLft->aplInteger, lpatRht->aplInteger);
} // End PrimFnDydUpCaretFisIvI


//***************************************************************************
//  $PrimFnDydUpCaretFisFvF
//
//  Primitive scalar function dyadic UpCaret:  F {is} F fn F
//***************************************************************************

void PrimFnDydUpCaretFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  lcm (PoM_, 0)  or  lcm (0, PoM_)
    if ((IsFltInfinity (lpatLft->aplFloat) && (lpatRht->aplFloat EQ 0))
     || (IsFltInfinity (lpatRht->aplFloat) && (lpatLft->aplFloat EQ 0)))
        lpMemRes[uRes] = TranslateQuadICIndex (lpatLft->aplFloat,
                                               ICNDX_0LCMInf,
                                               lpatRht->aplFloat,
                                               SIGN_APLFLOAT (lpatLft->aplFloat));
    else
    // Check for special cases:  lcm (PoM_, N)  or  lcm (N, PoM_)
    if (IsFltInfinity (lpatLft->aplFloat)
     || IsFltInfinity (lpatRht->aplFloat))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    else
        lpMemRes[uRes] = LcmHC1F (lpatLft->aplFloat, lpatRht->aplFloat);
} // End PrimFnDydUpCaretFisFvF


//***************************************************************************
//  $PrimFnDydUpCaretRisRvR
//
//  Primitive scalar function dyadic UpCaret:  R {is} fn R fn R
//***************************************************************************

void PrimFnDydUpCaretRisRvR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  lcm (PoM_, 0)  or  lcm (0, PoM_)
    if ((mpq_inf_p (&lpatLft->aplRat) && IsMpq0 (&lpatRht->aplRat))
     || (mpq_inf_p (&lpatRht->aplRat) && IsMpq0 (&lpatLft->aplRat)))
        lpMemRes[uRes] = *mpq_QuadICValue (&lpatLft->aplRat,
                                            ICNDX_0LCMInf,
                                           &lpatRht->aplRat,
                                           &lpMemRes[uRes],
                                            mpq_sgn (&lpatLft->aplRat) EQ -1);
    else
    // Check for special cases:  lcm (PoM_, N)  or  lcm (N, PoM_)
    if (mpq_inf_p (&lpatLft->aplRat)
     || mpq_inf_p (&lpatRht->aplRat))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    else
    // If the denominators are both 1, ...
    if (IsMpz1 (mpq_denref (&lpatLft->aplRat))
     && IsMpz1 (mpq_denref (&lpatRht->aplRat)))
    {
        // Initialize the result to 0/1
        mpq_init (&lpMemRes[uRes]);

        // Calculate the LCM on the numerators
        mpz_lcm (mpq_numref (&lpMemRes[uRes]),
                 mpq_numref (&lpatLft->aplRat),
                 mpq_numref (&lpatRht->aplRat));
        // The sign of the result is the product of the signs of the arguments
        if ((signumrat (&lpatLft->aplRat) * signumrat (&lpatRht->aplRat)) < 0)
            mpz_neg (mpq_numref (&lpMemRes[uRes]), mpq_numref (&lpMemRes[uRes]));
        // Set the denominator to 1
        mpz_set_ui (mpq_denref (&lpMemRes[uRes]), 1);
    } else
        lpMemRes[uRes] = LcmHC1R (lpatLft->aplRat, lpatRht->aplRat);
} // End PrimFnDydUpCaretRisRvR


//***************************************************************************
//  $PrimFnDydUpCaretVisRvR
//
//  Primitive scalar function dyadic UpCaret:  V {is} fn R fn R
//***************************************************************************

void PrimFnDydUpCaretVisRvR
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

    PrimFnDydUpCaretVisVvV (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
    Myhc1v_clear (&atLft.aplHC1V);
} // End PrimFnDydUpCaretVisRvR


//***************************************************************************
//  $PrimFnDydUpCaretVisVvV
//
//  Primitive scalar function dyadic UpCaret:  V {is} fn V fn V
//***************************************************************************

void PrimFnDydUpCaretVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Check for indeterminates:  lcm (PoM_, 0)  or  lcm (0, PoM_)
    if ((mpfr_inf_p (&lpatLft->aplVfp) && IsMpf0 (&lpatRht->aplVfp))
     || (mpfr_inf_p (&lpatRht->aplVfp) && IsMpf0 (&lpatLft->aplVfp)))
        lpMemRes[uRes] = *mpfr_QuadICValue (&lpatLft->aplVfp,
                                   ICNDX_0LCMInf,
                                  &lpatRht->aplVfp,
                                  &lpMemRes[uRes],
                                   SIGN_APLVFP (&lpatLft->aplVfp));
    else
    // Check for special cases:  lcm (PoM_, N)  or  lcm (N, PoM_)
    if (mpfr_inf_p (&lpatLft->aplVfp)
     || mpfr_inf_p (&lpatRht->aplVfp))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);
    else
        lpMemRes[uRes] = LcmHC1V (lpatLft->aplVfp, lpatRht->aplVfp);
} // End PrimFnDydUpCaretVisVvV


//***************************************************************************
//  $PrimFnDydUpCaretHC2IisHC2IvHC2I
//
//  Primitive scalar function dyadic UpCaret:  HC2I {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydUpCaretHC2IisHC2IvHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = LcmHC2I (lpatLft->aplHC2I, lpatRht->aplHC2I);
} // End PrimFnDydUpCaretHC2IisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydUpCaretHC2FisHC2IvHC2I
//
//  Primitive scalar function dyadic UpCaret:  HC2F {is} HC2I fn HC2I
//***************************************************************************

void PrimFnDydUpCaretHC2FisHC2IvHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},           // Left arg as ALLTYPES
             atRht = {0};           // Right ...

    // Promote the left & right args to the result type
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (&lpatLft->aplHC2I, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC2I][ARRAY_HC2F]) (&lpatRht->aplHC2I, 0, &atRht);

    lpMemRes[uRes] = LcmHC2F (atLft.aplHC2F, atRht.aplHC2F);
} // End PrimFnDydUpCaretHC2FisHC2IvHC2I


//***************************************************************************
//  $PrimFnDydUpCaretHC2FisHC2FvHC2F
//
//  Primitive scalar function dyadic UpCaret:  HC2F {is} HC2F fn HC2F
//***************************************************************************

void PrimFnDydUpCaretHC2FisHC2FvHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = LcmHC2F (lpatLft->aplHC2F, lpatRht->aplHC2F);
} // End PrimFnDydUpCaretHC2FisHC2FvHC2F


//***************************************************************************
//  $PrimFnDydUpCaretHC2RisHC2RvHC2R
//
//  Primitive scalar function dyadic UpCaret:  HC2R {is} HC2R fn HC2R
//***************************************************************************

void PrimFnDydUpCaretHC2RisHC2RvHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = LcmHC2R (lpatLft->aplHC2R, lpatRht->aplHC2R);
} // End PrimFnDydUpCaretHC2RisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydUpCaretHC2VisHC2RvHC2R
//
//  Primitive scalar function dyadic UpCaret:  HC2V {is} fn HC2R fn HC2R
//***************************************************************************

void PrimFnDydUpCaretHC2VisHC2RvHC2R
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

    PrimFnDydUpCaretHC2VisHC2VvHC2V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
    Myhc2v_clear (&atLft.aplHC2V);
} // End PrimFnDydUpCaretHC2VisHC2RvHC2R


//***************************************************************************
//  $PrimFnDydUpCaretHC2VisHC2VvHC2V
//
//  Primitive scalar function dyadic UpCaret:  HC2V {is} HC2V fn HC2V
//***************************************************************************

void PrimFnDydUpCaretHC2VisHC2VvHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = LcmHC2V (lpatLft->aplHC2V, lpatRht->aplHC2V);
} // End PrimFnDydUpCaretHC2VisHC2VvHC2V


//***************************************************************************
//  $PrimFnDydUpCaretHC4IisHC4IvHC4I
//
//  Primitive scalar function dyadic UpCaret:  HC4I {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydUpCaretHC4IisHC4IvHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = LcmHC4I (lpatLft->aplHC4I, lpatRht->aplHC4I);
} // End PrimFnDydUpCaretHC4IisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydUpCaretHC4FisHC4IvHC4I
//
//  Primitive scalar function dyadic UpCaret:  HC4F {is} HC4I fn HC4I
//***************************************************************************

void PrimFnDydUpCaretHC4FisHC4IvHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atLft = {0},           // Left arg as ALLTYPES
             atRht = {0};           // Right ...

    // Promote the left & right args to the result type
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (&lpatLft->aplHC4I, 0, &atLft);
    (*aTypeActPromote[ARRAY_HC4I][ARRAY_HC4F]) (&lpatRht->aplHC4I, 0, &atRht);

    lpMemRes[uRes] = LcmHC4F (atLft.aplHC4F, atRht.aplHC4F);
} // End PrimFnDydUpCaretHC4FisHC4IvHC4I


//***************************************************************************
//  $PrimFnDydUpCaretHC4FisHC4FvHC4F
//
//  Primitive scalar function dyadic UpCaret:  HC4F {is} HC4F fn HC4F
//***************************************************************************

void PrimFnDydUpCaretHC4FisHC4FvHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = LcmHC4F (lpatLft->aplHC4F, lpatRht->aplHC4F);
} // End PrimFnDydUpCaretHC4FisHC4FvHC4F


//***************************************************************************
//  $PrimFnDydUpCaretHC4RisHC4RvHC4R
//
//  Primitive scalar function dyadic UpCaret:  HC4R {is} HC4R fn HC4R
//***************************************************************************

void PrimFnDydUpCaretHC4RisHC4RvHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = LcmHC4R (lpatLft->aplHC4R, lpatRht->aplHC4R);
} // End PrimFnDydUpCaretHC4RisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydUpCaretHC4VisHC4RvHC4R
//
//  Primitive scalar function dyadic UpCaret:  HC4V {is} fn HC4R fn HC4R
//***************************************************************************

void PrimFnDydUpCaretHC4VisHC4RvHC4R
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

    PrimFnDydUpCaretHC4VisHC4VvHC4V (lpMemRes, uRes, &atLft, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
    Myhc4v_clear (&atLft.aplHC4V);
} // End PrimFnDydUpCaretHC4VisHC4RvHC4R


//***************************************************************************
//  $PrimFnDydUpCaretHC4VisHC4VvHC4V
//
//  Primitive scalar function dyadic UpCaret:  HC4V {is} HC4V fn HC4V
//***************************************************************************

void PrimFnDydUpCaretHC4VisHC4VvHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = LcmHC4V (lpatLft->aplHC4V, lpatRht->aplHC4V);
} // End PrimFnDydUpCaretHC4VisHC4VvHC4V


//***************************************************************************
//  End of File: pf_ucaret.c
//***************************************************************************
