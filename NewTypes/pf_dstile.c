//***************************************************************************
//  NARS2000 -- Primitive Function -- DownStile
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
#include "pf_fcre.h"
#include "headers.h"


#ifndef PROTO
PRIMSPEC PrimSpecDownStile = {
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecDownStileStorageTypeMon,
    &PrimFnMonDownStileAPA_EM,

    // Monadic Boolean result functions
    &PrimFnMonDownStileBisB,
    NULL,   // &PrimFnMonDownStileBisI, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileBisF, -- Can't happen w/DownStile

    // Monadic non-Boolean result functions (indexable)
    &PrimFnMonDownStileIisI,
    &PrimFnMonDownStileIisF,
    NULL,   // &PrimFnMonDownStileFisI, -- Can't happen w/DownStile
    &PrimFnMonDownStileFisF,
    &PrimFnMonDownStileRisR,
    &PrimFnMonDownStileVisR,
    &PrimFnMonDownStileVisV,

    // Monadic Hypercomplex functions
    &PrimFnMonDownStileHC2IisHC2I,
    &PrimFnMonDownStileHC2IisHC2F,
    &PrimFnMonDownStileHC2FisHC2I,
    &PrimFnMonDownStileHC2FisHC2F,
    &PrimFnMonDownStileHC2RisHC2R,
    &PrimFnMonDownStileHC2VisHC2R,
    &PrimFnMonDownStileHC2VisHC2V,

    &PrimFnMonDownStileHC4IisHC4I,
    &PrimFnMonDownStileHC4IisHC4F,
    &PrimFnMonDownStileHC4FisHC4I,
    &PrimFnMonDownStileHC4FisHC4F,
    &PrimFnMonDownStileHC4RisHC4R,
    &PrimFnMonDownStileHC4VisHC4R,
    &PrimFnMonDownStileHC4VisHC4V,

    NULL,   // &PrimFnMonDownStileHC8IisHC8I, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileHC8IisHC8F, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileHC8FisHC8I, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileHC8FisHC8F, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileHC8RisHC8R, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileHC8VisHC8R, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileHC8VisHC8V, -- Can't happen w/DownStile

    // Monadic FLT result HC arg functions (indexable)
    NULL,   // &PrimFnMonDownStileFisHC2I, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileFisHC2F, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileVisHC2R, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileVisHC2V, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileFisHC4I, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileFisHC4F, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileVisHC4R, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileVisHC4V, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileFisHC8I, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileFisHC8F, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileVisHC8R, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileVisHC8V, -- Can't happen w/DownStile

    // Dyadic functions
    &PrimFnDyd_EM_YY,
    &PrimSpecDownStileStorageTypeDyd,
    NULL,   // &PrimFnDydDownStileAPA_EM, -- Can't happen w/DownStile

    // Dyadic Boolean result functions
    &PrimFnDydUpCaretBisBvB,
    NULL,   // &PrimFnDydDownStileBisIvI, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisFvF, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisCvC, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisHvH, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisRvR, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisVvV, -- Can't happen w/DownStile

    NULL,   // &PrimFnDydDownStileBisHC2IvHC2I, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisHC2FvHC2F, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisHC2RvHC2R, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisHC2VvHC2V, -- Can't happen w/DownStile

    NULL,   // &PrimFnDydDownStileBisHC4IvHC4I, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisHC4FvHC4F, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisHC4RvHC4R, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisHC4VvHC4V, -- Can't happen w/DownStile

    NULL,   // &PrimFnDydDownStileBisHC8IvHC8I, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisHC8FvHC8F, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisHC8RvHC8R, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileBisHC8VvHC8V, -- Can't happen w/DownStile

    // Dyadic non-Boolean result functions (indexable)
    &PrimFnDydDownStileIisIvI,
    NULL,   // &PrimFnDydDownStileIisFvF, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileFisIvI, -- Can't happen w/DownStile
    &PrimFnDydDownStileFisFvF,
    &PrimFnDydDownStileRisRvR,
    &PrimFnDydDownStileVisRvR,
    &PrimFnDydDownStileVisVvV,

    NULL,   // &PrimFnDydDownStileHC2IisHC2IvHC2I, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC2IisHC2FvHC2F, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC2FisHC2IvHC2I, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC2FisHC2FvHC2F, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC2RisHC2RvHC2R, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC2VisHC2RvHC2R, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC2VisHC2VvHC2V, -- Can't happen w/DownStile

    NULL,   // &PrimFnDydDownStileHC4IisHC4IvHC4I, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC4IisHC4FvHC4F, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC4FisHC4IvHC4I, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC4FisHC4FvHC4F, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC4RisHC4RvHC4R, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC4VisHC4RvHC4R, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC4VisHC4VvHC4V, -- Can't happen w/DownStile

    NULL,   // &PrimFnDydDownStileHC8IisHC8IvHC8I, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC8IisHC8FvHC8F, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC8FisHC8IvHC8I, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC8FisHC8FvHC8F, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC8RisHC8RvHC8R, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC8VisHC8RvHC8R, -- Can't happen w/DownStile
    NULL,   // &PrimFnDydDownStileHC8VisHC8VvHC8V, -- Can't happen w/DownStile

    // Monadic Boolean chunk functions
    NULL,   // &PrimFnMonDownStileB64isB64, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileB32isB32, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileB16isB16, -- Can't happen w/DownStile
    NULL,   // &PrimFnMonDownStileB08isB08, -- Can't happen w/DownStile

    &PrimFnDydUpCaretB64isB64vB64,
    &PrimFnDydUpCaretB32isB32vB32,
    &PrimFnDydUpCaretB16isB16vB16,
    &PrimFnDydUpCaretB08isB08vB08,
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecDownStile};
#endif


//***************************************************************************
//  $PrimFnDownStile_EM_YY
//
//  Primitive function for monadic and dyadic DownStile ("floor" and "minimum")
//***************************************************************************

LPPL_YYSTYPE PrimFnDownStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNSTILE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return (*lpPrimSpec->PrimFnDyd_EM_YY) (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
} // End PrimFnDownStile_EM_YY


//***************************************************************************
//  $PrimSpecDownStileStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownStileStorageTypeMon
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
        // Except FLOAT goes to INT
        // IisF promotes to FisF as necessary.
        case ARRAY_FLOAT:
        case ARRAY_HC2F:
            // If we're not using Hurwitz, ...
            if (!gUseHurwitz)
                // IisF
                aplTypeRes--;       // ***ASSUME*** --  order of ARRAY_TYPES allows this
            break;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_RAT:
        case ARRAY_VFP:
        case ARRAY_NESTED:
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_HC8I:
        case ARRAY_HC8F:
        case ARRAY_HC8R:
        case ARRAY_HC8V:
            aplTypeRes = ARRAY_ERROR;

            break;

        case ARRAY_HC2I:
        case ARRAY_HC2R:
        case ARRAY_HC2V:
            break;

        case ARRAY_HC4F:
            if (!HasFractionality (aplTypeRes))
                aplTypeRes = ARRAY_ERROR;
            break;

        case ARRAY_HC4I:
        case ARRAY_HC4R:
        case ARRAY_HC4V:
            if (!HasFractionality (aplTypeRes))
                aplTypeRes = ARRAY_ERROR;
            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    return aplTypeRes;
} // End PrimSpecDownStileStorageTypeMon


//***************************************************************************
//  $PrimFnMonDownStileBisB
//
//  Primitive scalar function monadic DownStile:  B {is} fn B
//***************************************************************************

APLBOOL PrimFnMonDownStileBisB
    (APLBOOL    aplBooleanRht,
     LPPRIMSPEC lpPrimSpec)

{
    return aplBooleanRht;
} // End PrimFnMonDownStileBisB


//***************************************************************************
//  $PrimFnMonDownStileIisI
//
//  Primitive scalar function monadic DownStile:  I {is} fn I
//***************************************************************************

void PrimFnMonDownStileIisI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = lpatRht->aplInteger;
} // End PrimFnMonDownStileIisI


//***************************************************************************
//  $PrimFnMonDownStileIisF
//
//  Primitive scalar function monadic DownStile:  I {is} fn F
//***************************************************************************

void PrimFnMonDownStileIisF
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLFLOAT aplFloatRes;

    // Check for PoM infinity and numbers whose
    //   absolute value is >= 2*53
    if (IsFltInfinity (lpatRht->aplFloat)
     || fabs (lpatRht->aplFloat) >= Float2Pow53)
        RaiseException (EXCEPTION_RESULT_FLOAT, 0, 0, NULL);
    else
    {
        UBOOL bRet;

        PrimFnMonDownStileFisF (&aplFloatRes, 0, lpatRht, lpPrimSpec);

        lpMemRes[uRes] = ConvertToInteger_SCT (ARRAY_FLOAT, &aplFloatRes, 0, &bRet);

        Assert (bRet);
    } // End IF/ELSE
} // End PrimFnMonDownStileIisF


//***************************************************************************
//  $FloorHC1F
//***************************************************************************

APLHC1F FloorHC1F
    (APLHC1F aplRht)                    // Right arg

{
    APLHC1F aplRes,
            aplFloor,
            aplCeil,
            aplNear;

    // Check for PoM infinity
    if (IsFltInfinity (aplRht))
        aplRes = aplRht;
    else
    {
        // Get the exact floor and ceiling
        aplFloor = floor (aplRht);
        aplCeil  = ceil  (aplRht);

        // Calculate the integer nearest the right arg

        // Split cases based upon the signum of the difference between
        //   (the number and its floor) and (the ceiling and the number)
        switch (signumflt ((aplRht  - aplFloor)
                         - (aplCeil - aplRht)))
        {
            case  1:
                aplNear = aplCeil;

                break;

            case  0:
                // They are equal, so use the one with the larger absolute value
                aplNear = ((fabs (aplFloor) > fabs (aplCeil)) ? aplFloor
                                                              : aplCeil);
                break;

            case -1:
                aplNear = aplFloor;

                break;

            defstop
                break;
        } // End SWITCH

        // If Near is < Rht, aplRes = Near
        if (aplNear < aplRht)
            aplRes = aplNear;
        else
        // If Near is non-zero, and
        //   Rht is tolerantly-equal to Near,
        //   aplRes = Near; otherwise, aplRes = Near - 1
        if (aplNear)
        {
            if (CmpCT_F (aplRht,
              (APLFLOAT) aplNear,
                         GetQuadCT (),
                         EQ))
                aplRes = aplNear;
            else
                aplRes = aplNear - 1;
        } else
        // aplNear is zero
        // If Rht is between (-[]CT) and 0 (inclusive),
        //   aplRes = 0; otherwise, aplRes = -1
        if ((-GetQuadCT ()) <= aplRht
         &&                    aplRht <= 0)
            aplRes = 0;
        else
            aplRes = -1;
    } // End IF/ELSE

    return aplRes;
} // End FloorHC1F


//***************************************************************************
//  $PrimFnMonDownStileFisF
//
//  Primitive scalar function monadic DownStile:  F {is} fn F
//***************************************************************************

void PrimFnMonDownStileFisF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = FloorHC1F (lpatRht->aplFloat);
} // End PrimFnMonDownStileFisF


//***************************************************************************
//  $FloorHC1R
//***************************************************************************

APLHC1R FloorHC1R
    (APLHC1R aplRht)                    // Right arg

{
    APLRAT aplRes   = {0},
           mpqFloor = {0},
           mpqCeil  = {0},
           mpqTmp1  = {0},
           mpqTmp2  = {0},
           mpqNear  = {0};

    // Check for PoM infinity
    if (IsMpqInfinity (&aplRht))
        // Copy to the result
        mpq_init_set  (&aplRes, &aplRht);
    else
    {
        // Initialize the temps
        mpq_init (&aplRes);
        mpq_init (&mpqFloor);
        mpq_init (&mpqCeil );
        mpq_init (&mpqTmp1);
        mpq_init (&mpqTmp2);
        mpq_init (&mpqNear);

        // Get the exact floor and ceiling
        mpq_floor (&mpqFloor, &aplRht);
        mpq_ceil  (&mpqCeil , &aplRht);

        // Calculate the integer nearest the right arg

        mpq_sub (&mpqTmp1, &aplRht, &mpqFloor);
        mpq_sub (&mpqTmp2, &mpqCeil  , &aplRht);

        // Split cases based upon the signum of the difference between
        //   (the number and its floor) and (the ceiling and the number)
        switch (signumint (mpq_cmp (&mpqTmp1, &mpqTmp2)))
        {
            case  1:
                mpq_set (&mpqNear, &mpqCeil);

                break;

            case  0:
                mpq_abs (&mpqTmp1, &mpqFloor);
                mpq_abs (&mpqTmp2, &mpqFloor);

                // They are equal, so use the one with the larger absolute value
                mpq_set (&mpqNear, ((mpq_cmp (&mpqTmp1, &mpqTmp2) > 0) ? &mpqFloor
                                                                       : &mpqCeil));
                break;

            case -1:
                mpq_set (&mpqNear, &mpqFloor);

                break;

            defstop
                break;
        } // End SWITCH

        // If Near is < Rht, aplRes = Near
        if (mpq_cmp (&mpqNear, &aplRht) < 0)
            mpq_set (&aplRes, &mpqNear);
        else
        {
            // If Near is non-zero, and
            //   Rht is tolerantly-equal to Near,
            //   aplRes = Near; otherwise, aplRes = Near - 1
            if (mpq_sgn (&mpqNear) NE 0)
            {
                mpq_set (&aplRes, &mpqNear);

                if (CmpCT_R (aplRht,
                             mpqNear,
                             GetQuadCT (),
                             NE))
                    mpq_sub_ui (&aplRes, &aplRes, 1, 1);
            } else
            {
                // mpfNear is zero

                // Get -[]CT as a VFP
                mpq_set_d (&mpqTmp1, -GetQuadCT ());

                // If Rht is between (-[]CT) and 0 (inclusive),
                //   aplRes = 0; otherwise, aplRes = -1
                if (mpq_cmp (&mpqTmp1, &aplRht) <= 0
                 && mpq_sgn (&aplRht)           <= 0)
                    mpq_set_si (&aplRes,  0, 1);
                else
                    mpq_set_si (&aplRes, -1, 1);
            } // End IF/ELSE
        } // End IF/ELSE

        // We no longer need this storage
        Myq_clear (&mpqNear);
        Myq_clear (&mpqTmp2);
        Myq_clear (&mpqTmp1);
        Myq_clear (&mpqCeil);
        Myq_clear (&mpqFloor);
    } // End IF/ELSE

    return aplRes;
} // End FloorHC1R


//***************************************************************************
//  $PrimFnMonDownStileRisR
//
//  Primitive scalar function monadic DownStile:  R {is} fn R
//***************************************************************************

void PrimFnMonDownStileRisR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = FloorHC1R (lpatRht->aplRat);
} // End PrimFnMonDownStileRisR


//***************************************************************************
//  $FloorHC1V
//***************************************************************************

APLHC1V FloorHC1V
    (APLHC1V aplRht)                    // Right arg

{
    APLHC1V aplRes   = {0},
            mpfFloor = {0},
            mpfCeil  = {0},
            mpfTmp1  = {0},
            mpfTmp2  = {0},
            mpfNear  = {0};

    // Check for PoM infinity
    if (IsMpfInfinity (&aplRht))
        // Copy to the result
        mpfr_init_copy (&aplRes, &aplRht);
    else
    {
#ifdef DEBUG
////    WCHAR wszTemp[512];
#endif
        // Initialize the temps
        mpfr_init0 (&aplRes);
        mpfr_init0 (&mpfFloor);
        mpfr_init0 (&mpfCeil );
        mpfr_init0 (&mpfTmp1);
        mpfr_init0 (&mpfTmp2);
        mpfr_init0 (&mpfNear);

        // Get the exact floor and ceiling
        mpfr_floor (&mpfFloor, &aplRht);
        mpfr_ceil  (&mpfCeil , &aplRht);

        // Calculate the integer nearest the right arg

        mpfr_sub (&mpfTmp1, &aplRht, &mpfFloor, MPFR_RNDN);
        mpfr_sub (&mpfTmp2, &mpfCeil  , &aplRht, MPFR_RNDN);

        // Split cases based upon the signum of the difference between
        //   (the number and its floor) and (the ceiling and the number)
        switch (signumint (mpfr_cmp (&mpfTmp1, &mpfTmp2)))
        {
            case  1:
                mpfr_set (&mpfNear, &mpfCeil, MPFR_RNDN);

                break;

            case  0:
                mpfr_abs (&mpfTmp1, &mpfFloor, MPFR_RNDN);
                mpfr_abs (&mpfTmp2, &mpfFloor, MPFR_RNDN);

                // They are equal, so use the one with the larger absolute value
                mpfr_set (&mpfNear,
                           ((mpfr_cmp (&mpfTmp1, &mpfTmp2) > 0) ? &mpfFloor
                                                                : &mpfCeil),
                           MPFR_RNDN);
                break;

            case -1:
                mpfr_set (&mpfNear, &mpfFloor, MPFR_RNDN);

                break;

            defstop
                break;
        } // End SWITCH

#ifdef DEBUG
////    strcpyW (wszTemp, L"Floor: "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfFloor, 0) = WC_EOS; DbgMsgW (wszTemp);
////    strcpyW (wszTemp, L"Near:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfNear , 0) = WC_EOS; DbgMsgW (wszTemp);
////    strcpyW (wszTemp, L"Ceil:  "); *FormatAplVfp (&wszTemp[lstrlenW (wszTemp)], mpfCeil , 0) = WC_EOS; DbgMsgW (wszTemp);
#endif

        // If Near is < Rht, aplRes = Near
        if (mpfr_cmp (&mpfNear, &aplRht) < 0)
            mpfr_set (&aplRes, &mpfNear, MPFR_RNDN);
        else
        {
            // If Near is non-zero, and
            //   Rht is tolerantly-equal to Near,
            //   aplRes = Near; otherwise, aplRes = Near - 1
            if (mpfr_sgn (&mpfNear) NE 0)
            {
                mpfr_set (&aplRes, &mpfNear, MPFR_RNDN);

                if (CmpCT_V (aplRht,
                             mpfNear,
                             GetQuadCT (),
                             NE))
                    mpfr_sub_ui (&aplRes, &aplRes, 1, MPFR_RNDN);
            } else
            {
                // aplNear is zero

                // Get -[]CT as a VFP
                mpfr_set_d (&mpfTmp1, -GetQuadCT (), MPFR_RNDN);

                // If Rht is between (-[]CT) and 0 (inclusive),
                //   aplRes = 0; otherwise, aplRes = -1
                if (mpfr_cmp (&mpfTmp1, &aplRht) <= 0
                 && mpfr_sgn (&aplRht)           <= 0)
                    mpfr_set_si (&aplRes,  0, MPFR_RNDN);
                else
                    mpfr_set_si (&aplRes, -1, MPFR_RNDN);
            } // End IF/ELSE
        } // End IF/ELSE

        // We no longer need this storage
        Myf_clear (&mpfNear);
        Myf_clear (&mpfTmp2);
        Myf_clear (&mpfTmp1);
        Myf_clear (&mpfCeil);
        Myf_clear (&mpfFloor);
    } // End IF/ELSE

    return aplRes;
} // End FloorHC1V


//***************************************************************************
//  $PrimFnMonDownStileVisR
//
//  Primitive scalar function monadic DownStile:  V {is} fn R
//***************************************************************************

void PrimFnMonDownStileVisR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_RAT][ARRAY_VFP]) (&lpatRht->aplRat, 0, &atRht);

    PrimFnMonDownStileVisV (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc1v_clear (&atRht.aplHC1V);
} // End PrimFnMonDownStileVisR


//***************************************************************************
//  $PrimFnMonDownStileVisV
//
//  Primitive scalar function monadic DownStile:  V {is} fn V
//***************************************************************************

void PrimFnMonDownStileVisV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = FloorHC1V (lpatRht->aplVfp);
} // End PrimFnMonDownStileVisV


//***************************************************************************
//  $FloorHC2F
//***************************************************************************

APLHC2F FloorHC2F
    (APLHC2F aplRht)                    // Right arg

{
    int      i;
    APLHC2F  aplFlr = {0},
             aplFrc = {0};
    APLHC1F  aplSum = 0;
    APLFLOAT fQuadCT = GetQuadCT ();

    // No exceptions in this code

    // If we're using Hurwitz's Floor, ...
    if (gUseHurwitz)
    {
        APLHC2F  aplTmp1,
                 aplTmp2;
        APLFLOAT aplDst1,
                 aplDst2;
        APLINT   aplIntFlr;

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
        {
            //***************************************************************************
            // Calculate the all integer coordinate choice
            //***************************************************************************

            // Add 0.5 to each part
            aplTmp1.parts[i] = aplRht.parts[i] + 0.5;

            // Calculate the floor of each part
            aplTmp1.parts[i] = FloorHC1F (aplTmp1.parts[i]);

            //***************************************************************************
            // Calculate the all half integer coordinate choice
            //***************************************************************************

            // Double it
            aplTmp2.parts[i] = 2 * aplRht.parts[i];

            // Add 0.5 to each part
            aplTmp2.parts[i] += 0.5;

            // Calculate the floor of each part
            aplIntFlr = (APLINT) FloorHC1F (aplTmp2.parts[i]);

            // If the number if non-zero and even, ...
            if (aplIntFlr NE 0
             && (aplIntFlr & BIT0) EQ 0)
                // Convert to an odd number with lower absolute value
                aplIntFlr -= signumint (aplIntFlr);

            // Save half the value
            aplTmp2.parts[i] = ((APLFLOAT) aplIntFlr) / 2;
        } // End FOR

        //***************************************************************************
        // Choose the result closer to the original number
        //***************************************************************************

        // Calculate the distances
        aplDst1 = DistHC2F (aplRht, aplTmp1);
        aplDst2 = DistHC2F (aplRht, aplTmp2);

        // Use the one of smaller distance
        //   with ties going to the integer coordinates
        aplFlr = (aplDst1 <= aplDst2) ? aplTmp1 : aplTmp2;
    } else
    {
        // Loop through all of the parts
        for (i = 0; i < 2; i++)
        {
            // Calculate the floor of each part
            aplFlr.parts[i] = FloorHC1F (aplRht.parts[i]);

            // Calculate the fractional value of each part
            aplFrc.parts[i] = ModHC1F (1, aplRht.parts[i]);

            // Calculate the sum of the fractional values
            aplSum += aplFrc.parts[i];
        } // End FOR

        // If the point is NOT in the corner containing zero, ...
        if (CmpCT_F (aplSum, 1, fQuadCT, >=))
        {
            int j;

            // Calculate the index of the largest fractional part
            // Assume it's the real part
            j = 0; aplSum = aplFrc.parts[j];

            // Loop through the imaginary parts
            for (i = 1; i < 2; i++)
            // If this part is larger (within []CT), ...
            if (CmpCT_F (aplSum, aplFrc.parts[i], fQuadCT, <))
            {
                // Save as the largest so far
                j = i; aplSum = aplFrc.parts[j];
            } // End FOR/IF

            // Increment the chosen coordinate
            aplFlr.parts[j]++;
        } // End IF
    } // End IF/ELSE

    return aplFlr;
} // End FloorHC2F


//***************************************************************************
//  $FloorHC4F
//***************************************************************************

APLHC4F FloorHC4F
    (APLHC4F aplRht)                    // Right arg

{
    int      i;
    APLHC4F  aplTmp1,
             aplTmp2,                   // Half-integer
             aplRes = {0};
    APLHC1F  aplDst1,                   // Distance between aplRht and aplTmp1
             aplDst2;                   // ...                         aplTmp2
    APLINT   aplIntFlr;

    // No exceptions in this code

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        //***************************************************************************
        // Calculate the all integer coordinate choice
        //***************************************************************************

        // Add 0.5 to each part
        aplTmp1.parts[i] = aplRht.parts[i] + 0.5;

        // Calculate the floor of each part
        aplTmp1.parts[i] = FloorHC1F (aplTmp1.parts[i]);

        //***************************************************************************
        // Calculate the all half integer coordinate choice
        //***************************************************************************

        // Double it
        aplTmp2.parts[i] = 2 * aplRht.parts[i];

        // Add 0.5 to each part
        aplTmp2.parts[i] += 0.5;

        // Calculate the floor of each part
        aplIntFlr = (APLINT) FloorHC1F (aplTmp2.parts[i]);

        // If the number if non-zero and even, ...
        if (aplIntFlr NE 0
         && (aplIntFlr & BIT0) EQ 0)
            // Convert to an odd number with lower absolute value
            aplIntFlr -= signumint (aplIntFlr);

        // Save half the value
        aplTmp2.parts[i] = ((APLFLOAT) aplIntFlr) / 2;
    } // End FOR

    //***************************************************************************
    // Choose the result closer to the original number
    //***************************************************************************

    // Calculate the distances
    aplDst1 = DistHC4F (aplRht, aplTmp1);
    aplDst2 = DistHC4F (aplRht, aplTmp2);

    // Use the one of smaller distance
    //   with ties going to the integer coordinates
    return (aplDst1 <= aplDst2) ? aplTmp1 : aplTmp2;
} // End FloorHC4F


//***************************************************************************
//  $FloorHC2R
//***************************************************************************

APLHC2R FloorHC2R
    (APLHC2R aplRht)                    // Right arg

{
    int      i;
    UBOOL    bRet;
    APLHC2R  aplFlr = {0},
             aplFrc = {0},
             aplTmp1,
             aplTmp2 = {0};
    APLHC1R  aplSum = {0},
             aplRatFlr = {0};
    APLHC1V  aplDst1 = {0},             // Distance between aplRht and aplTmp1
             aplDst2 = {0};             // ...                         aplTmp2
    APLINT   aplIntFlr;
    APLFLOAT fQuadCT = GetQuadCT ();

    // No exceptions in this code

    // If we're using Hurwitz's Floor, ...
    if (gUseHurwitz)
    {
        // Initialize the half integer coordinate choice to 0/1
        mphc2r_init (&aplTmp2);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
        {
            //***************************************************************************
            // Calculate the all integer coordinate choice
            //***************************************************************************

            // Add 0.5 to each part
            aplSum = AddHC1R_RE (aplRht.parts[i], mpqHalf);

            // Calculate the floor of each part
            aplTmp1.parts[i] = FloorHC1R (aplSum);

            Myq_clear (&aplSum);

            //***************************************************************************
            // Calculate the all half integer coordinate choice
            //***************************************************************************

            // Double it
            mpq_mul_si (&aplTmp2.parts[i], &aplRht.parts[i], 2, 1);

            // Add 0.5 to each part
            aplSum = AddHC1R_RE (aplTmp2.parts[i], mpqHalf);

            // Calculate the floor of each part
            aplRatFlr = FloorHC1R (aplSum);

            Assert (mpq_integer_p (&aplRatFlr));

            Myq_clear (&aplSum);

            // Extract the integer within
            aplIntFlr = ConvertToInteger_SCT (ARRAY_HC1R, &aplRatFlr, 0, &bRet);

            Assert (bRet);

            Myq_clear (&aplRatFlr);

            // If the number if non-zero and even, ...
            if (aplIntFlr NE 0
             && (aplIntFlr & BIT0) EQ 0)
                // Convert to an odd number with lower absolute value
                aplIntFlr -= signumint (aplIntFlr);

            // Save half the value
            mpq_set_sx (&aplTmp2.parts[i], aplIntFlr, 2);
        } // End FOR

        //***************************************************************************
        // Choose the result closer to the original number
        //***************************************************************************

        // Calculate the distances
        aplDst1 = DistHC2R (aplRht, aplTmp1);
        aplDst2 = DistHC2R (aplRht, aplTmp2);

        // Use the one of smaller distance
        //   with ties going to the integer coordinates
        if (mpfr_cmp (&aplDst1, &aplDst2) <= 0)
        {
            mphc2r_set   (&aplFlr, &aplTmp1);
            Myhc2r_clear (&aplTmp2);
        } else
        {
            mphc2r_set   (&aplFlr, &aplTmp2);
            Myhc2r_clear (&aplTmp1);
        } // End IF/ELSE

        Myf_clear (&aplDst1);
        Myf_clear (&aplDst2);
    } else
    {
        // Initialize to 0/1
        mpq_init (&aplSum);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
        {
            // Calculate the floor of each part
            aplFlr.parts[i] = FloorHC1R (aplRht.parts[i]);

            // Calculate the fractional value of each part
            aplFrc.parts[i] = ModHC1R (mpqOne, aplRht.parts[i]);

            // Calculate the sum of the fractional values
            mpq_add (&aplSum, &aplSum, &aplFrc.parts[i]);
        } // End FOR

        // Compare against the hyperplane between the nearest corners
        bRet = CmpCT_R (aplSum, mpqOne, fQuadCT, >=);

        // We no longer need this storage
        Myhc1r_clear (&aplSum);

        // If the point is NOT in the corner containing zero, ...
        if (bRet)
        {
            int j;

            // Calculate the index of the largest fractional part
            // Assume it's the real part
            j = 0; aplSum = aplFrc.parts[j];

            // Loop through the imaginary parts
            for (i = 1; i < 2; i++)
            // If this part is larger (within []CT), ...
            if (CmpCT_R (aplSum, aplFrc.parts[i], fQuadCT, <))
            {
                // Save as the largest so far
                j = i; aplSum = aplFrc.parts[j];
            } // End FOR/IF

            // Increment the chosen coordinate
            mpq_add (&aplFlr.parts[j], &aplFlr.parts[j], &mpqOne);
        } // End IF

        Myhc2r_clear (&aplFrc);
    } // End IF/ELSE

    return aplFlr;
} // End FloorHC2R


//***************************************************************************
//  $FloorHC4R
//***************************************************************************

APLHC4R FloorHC4R
    (APLHC4R aplRht)                    // Right arg

{
    int      i;
    UBOOL    bRet;
    APLHC4R  aplTmp1 = {0},
             aplTmp2 = {0},
             aplRes = {0};
    APLHC1R  aplSum = {0},
             aplRatFlr = {0};
    APLHC1V  aplDst1 = {0},             // Distance between aplRht and aplTmp1
             aplDst2 = {0};             // ...                         aplTmp2
    APLINT   aplIntFlr;

    // No exceptions in this code

    // Initialize the half integer coordinate choice to 0/1
    mphc4r_init (&aplTmp2);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        //***************************************************************************
        // Calculate the all integer coordinate choice
        //***************************************************************************

        // Add 0.5 to each part
        aplSum = AddHC1R_RE (aplRht.parts[i], mpqHalf);

        // Calculate the floor of each part
        aplTmp1.parts[i] = FloorHC1R (aplSum);

        Myq_clear (&aplSum);

        //***************************************************************************
        // Calculate the all half integer coordinate choice
        //***************************************************************************

        // Double it
        mpq_mul_si (&aplTmp2.parts[i], &aplRht.parts[i], 2, 1);

        // Add 0.5 to each part
        aplSum = AddHC1R_RE (aplTmp2.parts[i], mpqHalf);

        // Calculate the floor of each part
        aplRatFlr = FloorHC1R (aplSum);

        Assert (mpq_integer_p (&aplRatFlr));

        Myq_clear (&aplSum);

        // Extract the integer within
        aplIntFlr = ConvertToInteger_SCT (ARRAY_HC1R, &aplRatFlr, 0, &bRet);

        Assert (bRet);

        Myq_clear (&aplRatFlr);

        // If the number if non-zero and even, ...
        if (aplIntFlr NE 0
         && (aplIntFlr & BIT0) EQ 0)
            // Convert to an odd number with lower absolute value
            aplIntFlr -= signumint (aplIntFlr);

        // Save half the value
        mpq_set_sx (&aplTmp2.parts[i], aplIntFlr, 2);
    } // End FOR

    //***************************************************************************
    // Choose the result closer to the original number
    //***************************************************************************

    // Calculate the distances
    aplDst1 = DistHC4R (aplRht, aplTmp1);
    aplDst2 = DistHC4R (aplRht, aplTmp2);

    // Choose the smaller distance
    //   with ties going to the integer coordinates
    if (mpfr_cmp (&aplDst1, &aplDst2) <= 0)
    {
        mphc4r_init_set (&aplRes, &aplTmp1);
        Myhc4r_clear    (&aplTmp2);
    } else
    {
        mphc4r_init_set (&aplRes, &aplTmp2);
        Myhc4r_clear    (&aplTmp1);
    } // End IF/ELSE

    Myf_clear    (&aplDst1);
    Myf_clear    (&aplDst2);

    return aplRes;
} // End FloorHC4R


//***************************************************************************
//  $FloorHC2V
//***************************************************************************

APLHC2V FloorHC2V
    (APLHC2V aplRht)                    // Right arg

{
    int      i;
    UBOOL    bRet;
    APLHC2V  aplFlr = {0},
             aplFrc = {0},
             aplTmp1 = {0},
             aplTmp2 = {0};
    APLHC1V  aplSum = {0},
             aplVfpFlr = {0};
    APLHC1V  aplDst1 = {0},             // Distance between aplRht and aplTmp1
             aplDst2 = {0};             // ...                         aplTmp2
    APLINT   aplIntFlr;
    APLFLOAT fQuadCT = GetQuadCT ();

    // No exceptions in this code

    // If we're using Hurwitz's Floor, ...
    if (gUseHurwitz)
    {
        // Initialize the half integer coordinate choice to 0
        mphc2v_init0 (&aplTmp2);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
        {
            //***************************************************************************
            // Calculate the all integer coordinate choice
            //***************************************************************************

            // Add 0.5 to each part
            aplSum = AddHC1V_RE (aplRht.parts[i], mpfHalf);

            // Calculate the floor of each part
            aplTmp1.parts[i] = FloorHC1V (aplSum);

            Myf_clear (&aplSum);

            //***************************************************************************
            // Calculate the all half integer coordinate choice
            //***************************************************************************

            // Double it
            mpfr_mul_si (&aplTmp2.parts[i], &aplRht.parts[i], 2, MPFR_RNDN);

            // Add 0.5 to each part
            aplSum = AddHC1V_RE (aplTmp2.parts[i], mpfHalf);

            // Calculate the floor of each part
            aplVfpFlr = FloorHC1V (aplSum);

            Assert (mpfr_integer_p (&aplVfpFlr));

            Myf_clear (&aplSum);

            // Extract the integer within
            aplIntFlr = ConvertToInteger_SCT (ARRAY_HC1V, &aplVfpFlr, 0, &bRet);

            Assert (bRet);

            Myf_clear (&aplVfpFlr);

            // If the number if non-zero and even, ...
            if (aplIntFlr NE 0
             && (aplIntFlr & BIT0) EQ 0)
                // Convert to an odd number with lower absolute value
                aplIntFlr -= signumint (aplIntFlr);

            // Save half the value
            mpfr_set_sj_2exp (&aplTmp2.parts[i], aplIntFlr, -1, MPFR_RNDN);
        } // End FOR

        //***************************************************************************
        // Choose the result closer to the original number
        //***************************************************************************

        // Calculate the distances
        aplDst1 = DistHC2V (aplRht, aplTmp1);
        aplDst2 = DistHC2V (aplRht, aplTmp2);

        // Choose the smaller distance
        //   with ties going to the integer coordinates
        if (mpfr_cmp (&aplDst1, &aplDst2) <= 0)
        {
            mphc2v_init_set (&aplFlr, &aplTmp1);
            Myhc2v_clear    (&aplTmp2);
        } else
        {
            mphc2v_init_set (&aplFlr, &aplTmp2);
            Myhc2v_clear    (&aplTmp1);
        } // End IF/ELSE

        Myf_clear    (&aplDst1);
        Myf_clear    (&aplDst2);
    } else
    {
        // Initialize to 0
        mpfr_init0 (&aplSum);

        // Loop through all of the parts
        for (i = 0; i < 2; i++)
        {
            // Calculate the floor of each part
            aplFlr.parts[i] = FloorHC1V (aplRht.parts[i]);

            // Calculate the fractional value of each part
            aplFrc.parts[i] = ModHC1V (mpfOne, aplRht.parts[i]);

            // Calculate the sum of the fractional values
            mpfr_add (&aplSum, &aplSum, &aplFrc.parts[i], MPFR_RNDN);
        } // End FOR

        // Compare against the hyperplane between the nearest corners
        bRet = CmpCT_V (aplSum, mpfOne, fQuadCT, >=);

        // We no longer need this storage
        Myhc1v_clear (&aplSum);

        // If the point is NOT in the corner containing zero, ...
        if (bRet)
        {
            int j;

            // Calculate the index of the largest fractional part
            // Assume it's the real part
            j = 0; aplSum = aplFrc.parts[j];

            // Loop through the imaginary parts
            for (i = 1; i < 2; i++)
            // If this part is larger (within []CT), ...
            if (CmpCT_V (aplSum, aplFrc.parts[i], fQuadCT, <))
            {
                // Save as the largest so far
                j = i; aplSum = aplFrc.parts[j];
            } // End FOR/IF

            // Increment the chosen coordinate
            mpfr_add (&aplFlr.parts[j], &aplFlr.parts[j], &mpfOne, MPFR_RNDN);
        } // End IF

        Myhc2v_clear (&aplFrc);
    } // End IF/ELSE

    return aplFlr;
} // End FloorHC2V


//***************************************************************************
//  $FloorHC4V
//***************************************************************************

APLHC4V FloorHC4V
    (APLHC4V aplRht)                    // Right arg

{
    int      i;
    UBOOL    bRet;
    APLHC4V  aplFlr = {0},
             aplFrc = {0},
             aplTmp1 = {0},
             aplTmp2 = {0},
             aplRes = {0};
    APLHC1V  aplSum = {0},
             aplVfpFlr = {0};
    APLHC1V  aplDst1 = {0},             // Distance between aplRht and aplTmp1
             aplDst2 = {0};             // ...                         aplTmp2
    APLINT   aplIntFlr;

    // No exceptions in this code

    // Initialize the half integer coordinate choice to 0
    mphc4v_init0 (&aplTmp2);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        //***************************************************************************
        // Calculate the all integer coordinate choice
        //***************************************************************************

        // Add 0.5 to each part
        aplSum = AddHC1V_RE (aplRht.parts[i], mpfHalf);

        // Calculate the floor of each part
        aplTmp1.parts[i] = FloorHC1V (aplSum);

        Myf_clear (&aplSum);

        //***************************************************************************
        // Calculate the all half integer coordinate choice
        //***************************************************************************

        // Double it
        mpfr_mul_si (&aplTmp2.parts[i], &aplRht.parts[i], 2, MPFR_RNDN);

        // Add 0.5 to each part
        aplSum = AddHC1V_RE (aplTmp2.parts[i], mpfHalf);

        // Calculate the floor of each part
        aplVfpFlr = FloorHC1V (aplSum);

        Assert (mpfr_integer_p (&aplVfpFlr));

        Myf_clear (&aplSum);

        // Extract the integer within
        aplIntFlr = ConvertToInteger_SCT (ARRAY_HC1V, &aplVfpFlr, 0, &bRet);

        Assert (bRet);

        Myf_clear (&aplVfpFlr);

        // If the number if non-zero and even, ...
        if (aplIntFlr NE 0
         && (aplIntFlr & BIT0) EQ 0)
            // Convert to an odd number with lower absolute value
            aplIntFlr -= signumint (aplIntFlr);

        // Save half the value
        mpfr_set_sj_2exp (&aplTmp2.parts[i], aplIntFlr, -1, MPFR_RNDN);
    } // End FOR

    //***************************************************************************
    // Choose the result closer to the original number
    //***************************************************************************

    // Calculate the distances
    aplDst1 = DistHC4V (aplRht, aplTmp1);
    aplDst2 = DistHC4V (aplRht, aplTmp2);

    // Choose the smaller distance
    //   with ties going to the integer coordinates
    if (mpfr_cmp (&aplDst1, &aplDst2) <= 0)
    {
        mphc4v_init_set (&aplRes, &aplTmp1);
        Myhc4v_clear    (&aplTmp2);
    } else
    {
        mphc4v_init_set (&aplRes, &aplTmp2);
        Myhc4v_clear    (&aplTmp1);
    } // End IF/ELSE

    Myf_clear    (&aplDst1);
    Myf_clear    (&aplDst2);

    return aplRes;
} // End FloorHC4V


//***************************************************************************
//  $PrimFnMonDownStileHC2IisHC2I
//
//  Primitive scalar function monadic DownStile:  HC2I {is} fn HC2I
//***************************************************************************

void PrimFnMonDownStileHC2IisHC2I
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = lpatRht->aplHC2I;
} // End PrimFnMonDownStileHC2IisHC2I


//***************************************************************************
//  $PrimFnMonDownStileHC4IisHC4I
//
//  Primitive scalar function monadic DownStile:  HC4I {is} fn HC4I
//***************************************************************************

void PrimFnMonDownStileHC4IisHC4I
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = lpatRht->aplHC4I;
} // End PrimFnMonDownStileHC4IisHC4I


//***************************************************************************
//  $PrimFnMonDownStileHC2FisHC2I
//
//  Primitive scalar function monadic DownStile:  HC2F {is} fn HC2I
//***************************************************************************

void PrimFnMonDownStileHC2FisHC2I
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // Save in the result
        lpMemRes[uRes].parts[i] = (APLFLOAT) lpatRht->aplHC2I.parts[i];
} // End PrimFnMonDownStileHC2FisHC2I


//***************************************************************************
//  $PrimFnMonDownStileHC4FisHC4I
//
//  Primitive scalar function monadic DownStile:  HC4F {is} fn HC4I
//***************************************************************************

void PrimFnMonDownStileHC4FisHC4I
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // Save in the result
        lpMemRes[uRes].parts[i] = (APLFLOAT) lpatRht->aplHC4I.parts[i];
} // End PrimFnMonDownStileHC4FisHC4I


//***************************************************************************
//  $PrimFnMonDownStileHC2IisHC2F
//
//  Primitive scalar function monadic DownStile:  HC2I {is} fn HC2F
//***************************************************************************

void PrimFnMonDownStileHC2IisHC2F
    (LPAPLHC2I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLHC2F aplRes;
    int     i;
    UBOOL   bRet;

    // Calculate the floor
    aplRes = FloorHC2F (lpatRht->aplHC2F);

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    {
        // Save in the result
        lpMemRes[uRes].parts[i] = ConvertToInteger_SCT (ARRAY_FLOAT, &aplRes.parts[i], 0, &bRet);

        Assert (bRet);
    } // End FOR
} // End PrimFnMonDownStileHC2IisHC2F


//***************************************************************************
//  $PrimFnMonDownStileHC4IisHC4F
//
//  Primitive scalar function monadic DownStile:  HC4I {is} fn HC4F
//***************************************************************************

void PrimFnMonDownStileHC4IisHC4F
    (LPAPLHC4I  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    APLHC4F aplRes;
    int     i;
    UBOOL   bRet;

    // Calculate the floor
    aplRes = FloorHC4F (lpatRht->aplHC4F);

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    {
        // Save in the result
        lpMemRes[uRes].parts[i] = ConvertToInteger_SCT (ARRAY_FLOAT, &aplRes.parts[i], 0, &bRet);

        Assert (bRet);
    } // End FOR
} // End PrimFnMonDownStileHC4IisHC4F


//***************************************************************************
//  $PrimFnMonDownStileHC2FisHC2F
//
//  Primitive scalar function monadic DownStile:  HC2F {is} fn HC2F
//***************************************************************************

void PrimFnMonDownStileHC2FisHC2F
    (LPAPLHC2F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = FloorHC2F (lpatRht->aplHC2F);
} // End PrimFnMonDownStileHC2FisHC2F


//***************************************************************************
//  $PrimFnMonDownStileHC4FisHC4F
//
//  Primitive scalar function monadic DownStile:  HC4F {is} fn HC4F
//***************************************************************************

void PrimFnMonDownStileHC4FisHC4F
    (LPAPLHC4F  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = FloorHC4F (lpatRht->aplHC4F);
} // End PrimFnMonDownStileHC4FisHC4F


//***************************************************************************
//  $PrimFnMonDownStileHC2RisHC2R
//
//  Primitive scalar function monadic DownStile:  HC2R {is} fn HC2R
//***************************************************************************

void PrimFnMonDownStileHC2RisHC2R
    (LPAPLHC2R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = FloorHC2R (lpatRht->aplHC2R);
} // End PrimFnMonDownStileHC2RisHC2R


//***************************************************************************
//  $PrimFnMonDownStileHC4RisHC4R
//
//  Primitive scalar function monadic DownStile:  HC4R {is} fn HC4R
//***************************************************************************

void PrimFnMonDownStileHC4RisHC4R
    (LPAPLHC4R  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = FloorHC4R (lpatRht->aplHC4R);
} // End PrimFnMonDownStileHC4RisHC4R


//***************************************************************************
//  $PrimFnMonDownStileHC2VisHC2R
//
//  Primitive scalar function monadic DownStile:  HC2V {is} fn HC2R
//***************************************************************************

void PrimFnMonDownStileHC2VisHC2R
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC2R][ARRAY_HC2V]) (&lpatRht->aplRat, 0, &atRht);

    // Save in the result
    PrimFnMonDownStileHC2VisHC2V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc2v_clear (&atRht.aplHC2V);
} // End PrimFnMonDownStileHC2VisHC2R


//***************************************************************************
//  $PrimFnMonDownStileHC4VisHC4R
//
//  Primitive scalar function monadic DownStile:  HC4V {is} fn HC4R
//***************************************************************************

void PrimFnMonDownStileHC4VisHC4R
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    ALLTYPES atRht = {0};

    // Promote the right arg to the result type
    (*aTypeActPromote[ARRAY_HC4R][ARRAY_HC4V]) (&lpatRht->aplRat, 0, &atRht);

    // Save in the result
    PrimFnMonDownStileHC4VisHC4V (lpMemRes, uRes, &atRht, lpPrimSpec);

    Myhc4v_clear (&atRht.aplHC4V);
} // End PrimFnMonDownStileHC4VisHC4R


//***************************************************************************
//  $PrimFnMonDownStileHC2VisHC2V
//
//  Primitive scalar function monadic DownStile:  HC2V {is} fn HC2V
//***************************************************************************

void PrimFnMonDownStileHC2VisHC2V
    (LPAPLHC2V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = FloorHC2V (lpatRht->aplHC2V);
} // End PrimFnMonDownStileHC2VisHC2V


//***************************************************************************
//  $PrimFnMonDownStileHC4VisHC4V
//
//  Primitive scalar function monadic DownStile:  HC4V {is} fn HC4V
//***************************************************************************

void PrimFnMonDownStileHC4VisHC4V
    (LPAPLHC4V  lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatRht,            // Ptr to right arg ALLTYPES
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Save in the result
    lpMemRes[uRes] = FloorHC4V (lpatRht->aplHC4V);
} // End PrimFnMonDownStileHC4VisHC4V


//***************************************************************************
//  $PrimFnMonDownStileAPA_EM
//
//  Monadic downstile, result is APA
//***************************************************************************

UBOOL PrimFnMonDownStileAPA_EM
    (LPPL_YYSTYPE lpYYRes,          // Ptr to the result (may be NULL)
     LPTOKEN      lptkFunc,         // Ptr to function token
     HGLOBAL      hGlbRht,          // Right arg handle
     HGLOBAL     *lphGlbRes,        // Ptr to result handle
     APLRANK      aplRankRht,       // Rank fo the right arg
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
} // End PrimFnMonDownStileAPA_EM


//***************************************************************************
//  $PrimSpecDownStileStorageTypeDyd
//
//  Primitive dyadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecDownStileStorageTypeDyd
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

    // Is the result HC?
    if (IsHCAny (aplTypeRes))
        // Tell the caller to demote the data to HC dimension 1
        return ARRAY_REALONLY;
    else
    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
        case ARRAY_RAT:
        case ARRAY_VFP:
        case ARRAY_NESTED:
            return aplTypeRes;

        defstop
        case ARRAY_CHAR:
        case ARRAY_HETERO:
            return ARRAY_ERROR;
    } // End IF/ELSE/SWITCH
} // End PrimSpecDownStileStorageTypeDyd


//***************************************************************************
//  $PrimFnDydDownStileIisIvI
//
//  Primitive scalar function dyadic DownStile:  I {is} I fn I
//***************************************************************************

void PrimFnDydDownStileIisIvI
    (LPAPLINT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = min (lpatLft->aplInteger, lpatRht->aplInteger);
} // End PrimFnDydDownStileIisIvI


//***************************************************************************
//  $PrimFnDydDownStileFisFvF
//
//  Primitive scalar function dyadic DownStile:  F {is} F fn F
//***************************************************************************

void PrimFnDydDownStileFisFvF
    (LPAPLFLOAT lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    lpMemRes[uRes] = min (lpatLft->aplFloat, lpatRht->aplFloat);
} // End PrimFnDydDownStileFisFvF


//***************************************************************************
//  $PrimFnDydDownStileRisRvR
//
//  Primitive scalar function dyadic DownStile:  R {is} R fn R
//***************************************************************************

void PrimFnDydDownStileRisRvR
    (LPAPLRAT   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Compare the two Rationals
    if (mpq_cmp (&lpatLft->aplRat, &lpatRht->aplRat) < 0)
        mpq_init_set (&lpMemRes[uRes], &lpatLft->aplRat);
    else
        mpq_init_set (&lpMemRes[uRes], &lpatRht->aplRat);
} // End PrimFnDydDownStileRisRvR


//***************************************************************************
//  $PrimFnDydDownStileVisRvR
//
//  Primitive scalar function dyadic DownStile:  V {is} R fn R
//***************************************************************************

void PrimFnDydDownStileVisRvR
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Compare the two Variable FPs
    if (mpq_cmp (&lpatLft->aplRat, &lpatRht->aplRat) < 0)
        mpfr_init_set_q (&lpMemRes[uRes], &lpatLft->aplRat, MPFR_RNDN);
    else
        mpfr_init_set_q (&lpMemRes[uRes], &lpatRht->aplRat, MPFR_RNDN);
} // End PrimFnDydDownStileVisRvR


//***************************************************************************
//  $PrimFnDydDownStileVisVvV
//
//  Primitive scalar function dyadic DownStile:  V {is} V fn V
//***************************************************************************

void PrimFnDydDownStileVisVvV
    (LPAPLVFP   lpMemRes,           // Ptr to the result
     APLUINT    uRes,               // Index into the result
     LPALLTYPES lpatLft,            // Ptr to left arg ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     LPPRIMSPEC lpPrimSpec)         // Ptr to local PRIMSPEC

{
    // Compare the two Variable FPs
    if (mpfr_cmp (&lpatLft->aplVfp, &lpatRht->aplVfp) < 0)
        mpfr_init_copy (&lpMemRes[uRes], &lpatLft->aplVfp);
    else
        mpfr_init_copy (&lpMemRes[uRes], &lpatRht->aplVfp);
} // End PrimFnDydDownStileVisVvV


//***************************************************************************
//  End of File: pf_dstile.c
//***************************************************************************
