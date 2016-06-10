//***************************************************************************
//  NARS2000 -- Conversion Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

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
#include <stdio.h>
#include <math.h>
#include "headers.h"


//***************************************************************************
//  $signumflt
//
//  Return -1, 0, 1 as per the sign of teh argument
//
//  Using a macro instead of a defined function risks duplicate
//    execution of the argument as well as tricky handling
//    of unsigned numners.
//***************************************************************************

int signumint
    (APLINT aplRht)

{
    return (aplRht < 0) ? -1 : (aplRht > 0);
} // End signumint


//***************************************************************************
//  $signumflt
//
//  Return -1, 0, 1 as per the sign of teh argument
//
//  For some reason, using
//
//  #define signumflt(a) (SIGN_APLFLOAT (a) ? -1 : ((a) > 0))
//
//  doesn't work, failing with a message about ptrs.
//***************************************************************************

int signumflt
    (APLFLOAT aplRht)

{
    return (aplRht < 0) ? -1 : (aplRht > 0);
} // End signumflt


//***************************************************************************
//  $ConvertFltToInt
//
// Attempt to convert the FLT to an integer using []CT as called for by lpAllTypes->enumCT
//***************************************************************************

APLINT ConvertFltToInt
    (LPAPLFLOAT lpaplFloat,
     APLINT     uInt,
     LPALLTYPES lpAllTypes,
     LPUBOOL    lpbRet)

{
    // Check on how to use []CT
    switch (lpAllTypes->enumCT)
    {
        case ENUMCT_NONE:
            // Attempt to convert the FLT to an integer using NO CT
            return _FloatToAplint_CT (lpaplFloat[uInt], 0.0                   , lpbRet, FALSE);

        case ENUMCT_USER:
            // Attempt to convert the FLT to an integer using User CT
            return _FloatToAplint_CT (lpaplFloat[uInt], lpAllTypes->fQuadCT   , lpbRet, FALSE);

        case ENUMCT_SYS:
            // Attempt to convert the FLT to an integer using System CT
            return _FloatToAplint_CT (lpaplFloat[uInt], SYS_CT                , lpbRet, TRUE);

        defstop
            return 0;
    } // End SWITCH
} // ConvertFltToInt


//***************************************************************************
//  $ConvertRatToInt
//
// Attempt to convert the RAT to an integer using []CT as called for by lpAllTypes->enumCT
//***************************************************************************

APLINT ConvertRatToInt
    (LPAPLRAT   lpaplRat,
     APLINT     uInt,
     LPALLTYPES lpAllTypes,
     LPUBOOL    lpbRet)

{
    // Check on how to use []CT
    switch (lpAllTypes->enumCT)
    {
        case ENUMCT_NONE:
            // Attempt to convert the RAT to an integer using NO CT
            return _mpq_get_ctsx (&lpaplRat[uInt], 0.0                  , lpbRet, FALSE);

        case ENUMCT_USER:
            // Attempt to convert the RAT to an integer using User CT
            return _mpq_get_ctsx (&lpaplRat[uInt], lpAllTypes->fQuadCT  , lpbRet, FALSE);

        case ENUMCT_SYS:
            // Attempt to convert the RAT to an integer using System CT
            return _mpq_get_ctsx (&lpaplRat[uInt], SYS_CT               , lpbRet, TRUE);

        defstop
            return 0;
    } // End SWITCH
} // ConvertRatToInt


//***************************************************************************
//  $ConvertVfpToInt
//
// Attempt to convert the VFP to an integer using []CT as called for by lpAllTypes->enumCT
//***************************************************************************

APLINT ConvertVfpToInt
    (LPAPLVFP   lpaplVfp,
     APLINT     uInt,
     LPALLTYPES lpAllTypes,
     LPUBOOL    lpbRet)

{
    // Check on how to use []CT
    switch (lpAllTypes->enumCT)
    {
        case ENUMCT_NONE:
            // Attempt to convert the VFP to an integer using NO CT
            return _mpfr_get_ctsx (&lpaplVfp[uInt], 0.0                 , lpbRet, FALSE);

        case ENUMCT_USER:
            // Attempt to convert the VFP to an integer using User CT
            return _mpfr_get_ctsx (&lpaplVfp[uInt], lpAllTypes->fQuadCT , lpbRet, FALSE);

        case ENUMCT_SYS:
            // Attempt to convert the VFP to an integer using System CT
            return _mpfr_get_ctsx (&lpaplVfp[uInt], SYS_CT              , lpbRet, TRUE);

        defstop
            return 0;
    } // End SWITCH
} // ConvertVfpToInt


//***************************************************************************
//  $FloatToAplint_CT
//
//  Attempt to convert a Floating Point number to an APLINT
//    using Comparison Tolerance
//***************************************************************************

APLINT _FloatToAplint_CT
    (APLFLOAT fFloat,       // The number to convert
     APLFLOAT fQuadCT,      // Comparison tolerance to use
     LPUBOOL  lpbRet,       // TRUE iff successful conversion
                            // (may be NULL if the caller isn't interested)
     UBOOL    bIntegerTest) // TRUE iff this is an integer test
{
    APLINT aplInteger;
    UBOOL  bRet;

    if (lpbRet EQ NULL)
        lpbRet = &bRet;

    // Convert to an integer (rounding down)
    aplInteger = (APLINT) floor (fFloat);

    // See how the number and its tolerant floor compare
    if (_CompareCT (fFloat, (APLFLOAT) aplInteger, fQuadCT, bIntegerTest))
    {
        *lpbRet = TRUE;

        return aplInteger;
    } // End IF

    // Convert to an integer (rounding up)
    aplInteger = (APLINT) ceil (fFloat);

    // See how the number and its tolerant ceiling compare
    if (_CompareCT (fFloat, (APLFLOAT) aplInteger, fQuadCT, bIntegerTest))
    {
        *lpbRet = TRUE;

        return aplInteger;
    } // End IF

    // It's not close enough, so we failed
    *lpbRet = FALSE;

    // Return the ceiling of the fractional value
    // The ceiling is important in CheckAxis for laminate
    return aplInteger;
} // End _FloatToAplint_CT


//***************************************************************************
//  $FloatToAplint_SCT
//
//  Attempt to convert a Floating Point number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT FloatToAplint_SCT
    (APLFLOAT fFloat,       // The number to convert
     LPUBOOL  lpbRet)       // TRUE iff successful conversion
                            // (may be NULL if the caller isn't interested)
{
    // Floats at or above 2*53 are by definition non-integral
    if (fabs (fFloat) >= Float2Pow53)
    {
        if (lpbRet)
            // Mark as non-integral
            *lpbRet = FALSE;

        return (APLINT) fFloat;
    } else
        return _FloatToAplint_CT (fFloat, SYS_CT, lpbRet, TRUE);
} // End FloatToAplint_SCT


//***************************************************************************
//  $RATToAplint
//
//  Attempt to convert a RAT number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT RATToAplint
    (LPAPLRAT   lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    // Attempt to convert the RAT to an integer using System []CT
    return mpq_get_sctsx (&lpDataLft[u], lpbRet);
} // RATToAplint


//***************************************************************************
//  $VFPToAplint
//
//  Attempt to convert a VFP number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT VFPToAplint
    (LPAPLVFP   lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    // Attempt to convert the VFP to an integer using System []CT
    return mpfr_get_sctsx (&lpDataLft[u], lpbRet);
} // VFPToAplint


//***************************************************************************
//  $HC2IToAplint_SCT
//
//  Attempt to convert a HC2I number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC2IToAplint_SCT
    (LPAPLHC2I  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Determine if the imaginary parts of HC2I are all near zero
        *lpbRet &= abs64 (lpDataLft[u].parts[i]) < SYS_CT;

    // Get the next dimension
    return lpDataLft[u].parts[0];
} // HC2IToAplint_SCT


//***************************************************************************
//  $HC4IToAplint_SCT
//
//  Attempt to convert a HC4I number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC4IToAplint_SCT
    (LPAPLHC4I  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Determine if the imaginary parts of HC4I are all near zero
        *lpbRet &= abs64 (lpDataLft[u].parts[i]) < SYS_CT;

    // Get the next dimension
    return lpDataLft[u].parts[0];
} // HC4IToAplint_SCT


//***************************************************************************
//  $HC8IToAplint_SCT
//
//  Attempt to convert a HC8I number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC8IToAplint_SCT
    (LPAPLHC8I  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Determine if the imaginary parts of HC8I are all near zero
        *lpbRet &= abs64 (lpDataLft[u].parts[i]) < SYS_CT;

    // Get the next dimension
    return lpDataLft[u].parts[0];
} // HC8IToAplint_SCT


//***************************************************************************
//  $HC2FToAplint_SCT
//
//  Attempt to convert a HC2F number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC2FToAplint_SCT
    (LPAPLHC2F  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Determine if the imaginary parts of HC2F are all near zero
        *lpbRet &= fabs (lpDataLft[u].parts[i]) < SYS_CT;

    // Get the next dimension
    return FloatToAplint_SCT (lpDataLft[u].parts[0], lpbRet);
} // HC2FToAplint_SCT


//***************************************************************************
//  $HC4FToAplint_SCT
//
//  Attempt to convert a HC4F number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC4FToAplint_SCT
    (LPAPLHC4F  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;                      // Loop counter

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Determine if the imaginary parts of HC4F are all near zero
        *lpbRet &= fabs (lpDataLft[u].parts[i]) < SYS_CT;

    // Get the next dimension
    return FloatToAplint_SCT (lpDataLft[u].parts[0], lpbRet);
} // HC4FToAplint_SCT


//***************************************************************************
//  $HC8FToAplint_SCT
//
//  Attempt to convert a HC8F number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC8FToAplint_SCT
    (LPAPLHC8F  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;                      // Loop counter

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Determine if the imaginary parts of HC8F are all near zero
        *lpbRet &= fabs (lpDataLft[u].parts[i]) < SYS_CT;

    // Get the next dimension
    return FloatToAplint_SCT (lpDataLft[u].parts[0], lpbRet);
} // HC8FToAplint_SCT


//***************************************************************************
//  $HC2RToAplint_SCT
//
//  Attempt to convert a HC2R number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC2RToAplint_SCT
    (LPAPLHC2R  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;                      // Loop counter

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Determine if the imaginary parts of HC2R are all near zero
        *lpbRet &= mpq_get_sctsx (&lpDataLft[u].parts[1], lpbRet) < SYS_CT;

    // Get the next dimension
    return mpq_get_sctsx (&lpDataLft[u].parts[0], lpbRet);
} // HC2RToAplint_SCT


//***************************************************************************
//  $HC4RToAplint_SCT
//
//  Attempt to convert a HC4R number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC4RToAplint_SCT
    (LPAPLHC4R  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;                      // Loop counter

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Determine if the imaginary parts of HC4R are all near zero
        *lpbRet &= mpq_get_sctsx (&lpDataLft[u].parts[i], lpbRet) < SYS_CT;

    // Get the next dimension
    return mpq_get_sctsx (&((LPAPLHC4R) lpDataLft)[u].parts[0], lpbRet);
} // HC4RToAplint_SCT


//***************************************************************************
//  $HC8RToAplint_SCT
//
//  Attempt to convert a HC8R number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC8RToAplint_SCT
    (LPAPLHC8R  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;                      // Loop counter

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Determine if the imaginary parts of HC8R are all near zero
        *lpbRet &= mpq_get_sctsx (&lpDataLft[u].parts[i], lpbRet) < SYS_CT;

    // Get the next dimension
    return mpq_get_sctsx (&((LPAPLHC8R) lpDataLft)[u].parts[0], lpbRet);
} // HC8RToAplint_SCT


//***************************************************************************
//  $HC2VToAplint_SCT
//
//  Attempt to convert a HC2V number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC2VToAplint_SCT
    (LPAPLHC2V  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;                      // Loop counter

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 2; i++)
        // Determine if the imaginary parts of HC2V are all near zero
        *lpbRet &= mpfr_get_sctsx (&lpDataLft[u].parts[1], lpbRet) < SYS_CT;

    // Get the next dimension
    return mpfr_get_sctsx (&lpDataLft[u].parts[0], lpbRet);
} // HC2VToAplint_SCT


//***************************************************************************
//  $HC4VToAplint_SCT
//
//  Attempt to convert a HC4V number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC4VToAplint_SCT
    (LPAPLHC4V  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;                      // Loop counter

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 4; i++)
        // Determine if the imaginary parts of HC4V are all near zero
        *lpbRet &= mpfr_get_sctsx (&lpDataLft[u].parts[i], lpbRet) < SYS_CT;

    // Get the next dimension
    return mpfr_get_sctsx (&lpDataLft[u].parts[0], lpbRet);
} // HC4VToAplint_SCT


//***************************************************************************
//  $HC8VToAplint_SCT
//
//  Attempt to convert a HC8V number to an APLINT
//    using System Comparison Tolerance
//***************************************************************************

APLINT HC8VToAplint_SCT
    (LPAPLHC8V  lpDataLft,      // Ptr to left arg data
     APLUINT    u,              // Index into left arg
     LPUBOOL    lpbRet)         // Ptr to TRUE iff it succeeded

{
    int i;                      // Loop counter

    // Initialize the identity element for AND
    *lpbRet = TRUE;

    // Loop through the imaginary parts
    for (i = 1; i < 8; i++)
        // Determine if the imaginary parts of HC8V are all near zero
        *lpbRet &= mpfr_get_sctsx (&lpDataLft[u].parts[i], lpbRet) < SYS_CT;

    // Get the next dimension
    return mpfr_get_sctsx (&lpDataLft[u].parts[0], lpbRet);
} // HC8VToAplint_SCT


//***************************************************************************
//  $hcXY_cmp
//
//  Compare two like items
//    returning -1, 0, 1
//***************************************************************************

int hcXY_cmp
    (APLSTYPE   aplTypeCom,         // Common storage type
     LPALLTYPES lpatLft,            // Ptr to left arg as ALLTYPES
     LPALLTYPES lpatRht,            // ...    right ...
     APLFLOAT   fQuadCT)            // []CT (0 = Exact comparison)

{
    // Split cases based upon the common storage type
    switch (aplTypeCom)
    {
        case ARRAY_BOOL:
            return signumint (lpatLft->aplBoolean - lpatRht->aplBoolean);

        case ARRAY_INT:
            return signumint (lpatLft->aplInteger - lpatRht->aplInteger);

        case ARRAY_FLOAT:
            if (fQuadCT EQ 0.0)
                return signumflt (lpatLft->aplFloat - lpatRht->aplFloat);
            else
                return flt_cmp_ct (lpatLft->aplFloat, lpatRht->aplFloat, fQuadCT, FALSE);

        case ARRAY_CHAR:
            return signumint (lpatLft->aplChar - lpatRht->aplChar);

        case ARRAY_RAT:
            if (fQuadCT EQ 0.0)
                return signumint (mpq_cmp (&lpatLft->aplRat, &lpatRht->aplRat));
            else
                return signumint (mpq_cmp_ct (lpatLft->aplRat, lpatRht->aplRat, fQuadCT));

        case ARRAY_VFP:
            if (fQuadCT EQ 0.0)
                return signumint (mpfr_cmp (&lpatLft->aplVfp, &lpatRht->aplVfp));
            else
                return signumint (mpfr_cmp_ct (lpatLft->aplVfp, lpatRht->aplVfp, fQuadCT));

        case ARRAY_HC2I:
            return hc2i_cmp  (lpatLft->aplHC2I    , lpatRht->aplHC2I   );

        case ARRAY_HC4I:
            return hc4i_cmp  (lpatLft->aplHC4I    , lpatRht->aplHC4I   );

        case ARRAY_HC8I:
            return hc8i_cmp  (lpatLft->aplHC8I    , lpatRht->aplHC8I   );

        case ARRAY_HC2F:
            return hc2f_cmp  (lpatLft->aplHC2F    , lpatRht->aplHC2F   , fQuadCT);

        case ARRAY_HC4F:
            return hc4f_cmp  (lpatLft->aplHC4F    , lpatRht->aplHC4F   , fQuadCT);

        case ARRAY_HC8F:
            return hc8f_cmp  (lpatLft->aplHC8F    , lpatRht->aplHC8F   , fQuadCT);

        case ARRAY_HC2R:
            return hc2r_cmp  (lpatLft->aplHC2R    , lpatRht->aplHC2R   , fQuadCT);

        case ARRAY_HC4R:
            return hc4r_cmp  (lpatLft->aplHC4R    , lpatRht->aplHC4R   , fQuadCT);

        case ARRAY_HC8R:
            return hc8r_cmp  (lpatLft->aplHC8R    , lpatRht->aplHC8R   , fQuadCT);

        case ARRAY_HC2V:
            return hc2v_cmp  (lpatLft->aplHC2V    , lpatRht->aplHC2V   , fQuadCT);

        case ARRAY_HC4V:
            return hc4v_cmp  (lpatLft->aplHC4V    , lpatRht->aplHC4V   , fQuadCT);

        case ARRAY_HC8V:
            return hc8v_cmp  (lpatLft->aplHC8V    , lpatRht->aplHC8V   , fQuadCT);

        defstop
            return 0;
    } // End SWITCH
} // End hcXY_cmp


//***************************************************************************
//  $HeNe_cmp
//
//  Compare HETERO/NESTED items
//    returning -1 (L < R)
//               0 (L == R)
//               1 (L > R)
//***************************************************************************

int HeNe_cmp
    (APLHETERO lpSymGlbLft,             // Left arg
     APLHETERO lpSymGlbRht,             // Right ...
     APLFLOAT  fQuadCT)                 // []CT (0 = Exact comparison)

{
    APLSTYPE          aplTypeLft,
                      aplTypeRht,
                      aplTypeCom;
    APLRANK           aplRankLft,
                      aplRankRht;
    APLNELM           aplNELMLft,
                      aplNELMRht;
    LPAPLDIM          lpMemDimLft,
                      lpMemDimRht;
    LPVARARRAY_HEADER lpMemHdrLft = NULL,
                      lpMemHdrRht = NULL,
                      lpMemHdrSub = NULL;
    LPVOID            lpMemSubLft,
                      lpMemSubRht;
    ALLTYPES          atLft = {0},
                      atRht = {0};
    int               iDiff = 0,
                      iDim;

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpSymGlbLft))
    {
        case PTRTYPE_STCONST:
            // Zap ptr to array header
            lpMemHdrLft = NULL;

            // Get the storage type
            aplTypeLft = TranslateImmTypeToArrayType (lpSymGlbLft->stFlags.ImmType);

            // Get the rank
            aplRankLft = 0;

            // Get the NELM
            aplNELMLft = 1;

            // Get a ptr to the dimensions
            lpMemDimLft = NULL;

            // Get a ptr to the data
            lpMemSubLft = &lpSymGlbLft->stData.stLongest;

            break;

        case PTRTYPE_HGLOBAL:
            // Get a ptr to the array header
            lpMemHdrLft = MyGlobalLock (lpSymGlbLft);

            // Get the storage type
            aplTypeLft = lpMemHdrLft->ArrType;

            // Get the rank
            aplRankLft = lpMemHdrLft->Rank;

            // Get the NELM
            aplNELMLft = lpMemHdrLft->NELM;

            // Get a ptr to the dimensions
            lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);

            // Get a ptr to the data
            lpMemSubLft = VarArrayDataFmBase (lpMemHdrLft);

            break;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (lpSymGlbRht))
    {
        case PTRTYPE_STCONST:
            // Zap ptr to array header
            lpMemHdrRht = NULL;

            // Get the storage type
            aplTypeRht = TranslateImmTypeToArrayType (lpSymGlbRht->stFlags.ImmType);

            // Get the rank
            aplRankRht = 0;

            // Get the NELM
            aplNELMRht = 1;

            // Get a ptr to the dimensions
            lpMemDimRht = NULL;

            // Get a ptr to the data
            lpMemSubRht = &lpSymGlbRht->stData.stLongest;

            break;

        case PTRTYPE_HGLOBAL:
            // Get a ptr to the array header
            lpMemHdrRht = MyGlobalLock (lpSymGlbRht);

            // Get the storage type
            aplTypeRht = lpMemHdrRht->ArrType;

            // Get the rank
            aplRankRht = lpMemHdrRht->Rank;

            // Get the NELM
            aplNELMRht = lpMemHdrRht->NELM;

            // Get a ptr to the dimensions
            lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

            // Get a ptr to the data
            lpMemSubRht = VarArrayDataFmBase (lpMemHdrRht);

            break;

        defstop
            break;
    } // End SWITCH

    // If the ranks are different, ...
    if (aplRankLft NE aplRankRht)
    {
        iDiff = signumint (aplRankLft - aplRankRht);

        goto NORMAL_EXIT;
    } else
    {
        // If the leading dimensions differ, ...
        for (iDim = 0; iDim < aplRankLft; iDim++)
        if (lpMemDimLft[iDim] NE lpMemDimRht[iDim])
        {
            iDiff = signumint (lpMemDimLft[iDim] - lpMemDimRht[iDim]);

            goto NORMAL_EXIT;
        } // End FOR/IF
    } // End IF/ELSE

    // If the storage types differ (Numeric vs. Char), ...
    if (IsNumeric (aplTypeLft) && IsSimpleChar (aplTypeRht))
        // Num < Char
        iDiff = -1;
    else
    // If the storage types differ (Char vs. Numeric), ...
    if (IsSimpleChar (aplTypeLft) && IsNumeric (aplTypeRht))
        // Char > Num
        iDiff =  1;
    else
    if ((IsNumeric    (aplTypeLft) && IsNumeric    (aplTypeRht))
     || (IsSimpleChar (aplTypeLft) && IsSimpleChar (aplTypeRht)))
    {
        Assert (aplNELMLft EQ aplNELMRht);

        // Calculate the common var storage type
        aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

        // Loop through the elements
        for (iDim = 0; iDim < aplNELMLft; iDim++)
        {
            // Convert the left arg item to a common storage type
            (*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpMemSubLft, iDim, &atLft);

            // Convert the right arg item to a common storage type
            (*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpMemSubRht, iDim, &atRht);

            // Compare the two items
            iDiff = hcXY_cmp (aplTypeCom, &atLft, &atRht, fQuadCT);

            // Free the old atLft and atRht
            (*aTypeFree[aplTypeCom]) (&atLft, 0);
            (*aTypeFree[aplTypeCom]) (&atRht, 0);

            if (iDiff NE 0)
                break;
        } // End FOR
    } else
    // One or both items are HETERO/NESTED
    {
        // Sp[lit cases based upon the immediate status
        switch (2 * (lpMemHdrLft EQ NULL)
              + 1 * (lpMemHdrRht EQ NULL))
        {
            case 2 * 0 + 1 * 0:         // Neither is STCONST
                // Loop through the elements
                for (iDim = 0; iDim < aplNELMLft; iDim++)
                {
                    // Call recursively
                    iDiff = HeNe_cmp (((LPAPLNESTED) lpMemSubLft)[iDim],
                                      ((LPAPLNESTED) lpMemSubRht)[iDim], fQuadCT);
                    if (iDiff NE 0)
                        break;
                } // End FOR

                break;

            case 2 * 0 + 1 * 1:         // Right   is STCONST
                // Save stLongest
                atRht.aplLongest = *(LPAPLLONGEST) lpMemSubRht;

                // Call common routine, negate result
                iDiff = -HeNe_cmpsub (&atRht,       // Ptr to STE's ALLTYPES (filled in with value)
                                       aplTypeRht,  // STE's array storage type
                                      &atLft,       // Ptr to GLB's ALLTYPES (empty)
                        *(LPAPLNESTED) lpMemSubLft, // GLB's global memory handle
                                       fQuadCT);    // []CT
                break;

            case 2 * 1 + 1 * 0:         // Left    is STCONST
                // Save stLongest
                atLft.aplLongest = *(LPAPLLONGEST) lpMemSubLft;

                // Call common routine
                iDiff =  HeNe_cmpsub (&atLft,       // Ptr to STE's ALLTYPES (filled in with value)
                                       aplTypeLft,  // STE's array storage type
                                      &atRht,       // Ptr to GLB's ALLTYPES (empty)
                        *(LPAPLNESTED) lpMemSubRht, // GLB's global memory handle
                                       fQuadCT);    // []CT
                break;
            case 2 * 1 + 1 * 1:         // Both   are STCONST
            {
                IMM_TYPES immTypeLft = lpSymGlbLft->stFlags.ImmType,
                          immTypeRht = lpSymGlbRht->stFlags.ImmType;

                // If the storage types differ (Numeric vs. Char), ...
                if (IsImmNum (immTypeLft) && IsImmChr (immTypeRht))
                    // Num < Char
                    iDiff = -1;
                else
                // If the storage types differ (Char vs. Numeric), ...
                if (IsImmChr (immTypeLft) && IsImmNum (immTypeRht))
                    // Char > Num
                    iDiff =  1;
                else
                // If the storage types sre both char, ...
                if (IsImmChr (immTypeLft) && IsImmChr (immTypeRht))
                    iDiff = lpSymGlbLft->stData.stChar -
                            lpSymGlbRht->stData.stChar;
                else
                {
                    // Translate to array storage type
                    aplTypeLft = TranslateImmTypeToArrayType (immTypeLft);
                    aplTypeRht = TranslateImmTypeToArrayType (immTypeRht);

                    // Calculate the common var storage type
                    aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

                    // Promote to common storage type
                    (*aTypeActPromote[aplTypeLft][aplTypeCom]) (&lpSymGlbLft->stData.stLongest, 0, &atLft);
                    (*aTypeActPromote[aplTypeRht][aplTypeCom]) (&lpSymGlbRht->stData.stLongest, 0, &atRht);

                    // Compare the two items
                    iDiff = hcXY_cmp (aplTypeCom, &atLft, &atRht, fQuadCT);

                    // Free the old atLft and atRht
                    (*aTypeFree[aplTypeCom]) (&atLft, 0);
                    (*aTypeFree[aplTypeCom]) (&atRht, 0);
                } // End IF/ELSE/...

                break;
            } // End

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE/...
NORMAL_EXIT:
    // If the left item is locked, ...
    if (lpMemHdrLft NE NULL)
    {
        MyGlobalUnlock (lpSymGlbLft); lpSymGlbLft = NULL;
    } // End IF

    // If the right item is locked, ...
    if (lpMemHdrRht NE NULL)
    {
        MyGlobalUnlock (lpSymGlbRht); lpSymGlbRht = NULL;
    } // End IF

    return iDiff;
} // End HeNe_cmp


//***************************************************************************
//  $HeNe_cmpsub
//
//  Comparison subroutine to HeNe_cmp
//
//  Note that it returns the difference of STE-GLB, so if you need
//    GLB-STE, negate the result.
//***************************************************************************

int HeNe_cmpsub
    (LPALLTYPES lpatSte,                // Ptr to STE's ALLTYPES (filled in with value)
     APLSTYPE   aplTypeSte,             // STE's array storage type
     LPALLTYPES lpatGlb,                // Ptr to GLB's ALLTYPES (empty)
     HGLOBAL    hGlb,                   // GLB's global memory handle
     APLFLOAT   fQuadCT)                // []CT

{
    APLSTYPE          aplTypeCom;       // Common array storage type
    LPVARARRAY_HEADER lpMemHdr = NULL;  // Ptr to item header
    LPVOID            lpMem;            // Ptr to item global memory
    int               iDiff;            // The result

    // Lock the memory to get a ptr to it
    lpMemHdr = MyGlobalLock (hGlb);

    // Skip over the headers and dimensions to the data
    lpMem = VarArrayDataFmBase (lpMemHdr);

    // Calculate the common array storage type
    aplTypeCom = aTypePromote[lpMemHdr->ArrType][aplTypeSte];

    // Promote the STE and GLB to common array storage type
    (*aTypeActPromote[aplTypeSte       ][aplTypeCom]) (lpatSte, 0, lpatSte);
    (*aTypeActPromote[lpMemHdr->ArrType][aplTypeCom]) (lpMem  , 0, lpatGlb);

    // Compare the two items
    iDiff = hcXY_cmp (aplTypeCom, lpatSte, lpatGlb, fQuadCT);

    // Free the old atGlb
    (*aTypeFree[aplTypeCom]) (lpatGlb, 0);

    // We no longer need this ptr
    MyGlobalUnlock (hGlb); lpMemHdr = NULL;

    return iDiff;
} // End HeNe_cmpsub


//***************************************************************************
//  $hc2i_cmp
//
//  Compare two HC2I values
//    returning -1, 0, 1
//***************************************************************************

int hc2i_cmp
    (APLHC2I aplHC2ILft,            // Left arg float
     APLHC2I aplHC2IRht)            // Right ...

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
    // Split cases based upon the signum of the difference
    switch (signumint (aplHC2ILft.parts[i]
                     - aplHC2IRht.parts[i]))
    {
        case 1:
            return  1;

        case 0:
            break;

        case -1:
            return -1;

        defstop
            break;
    } // End FOR/SWITCH

    // The numbers are equal
    return 0;
} // hc2i_cmp


//***************************************************************************
//  $hc4i_cmp
//
//  Compare two HC4I values
//    returning -1, 0, 1
//***************************************************************************

int hc4i_cmp
    (APLHC4I aplHC4ILft,            // Left arg float
     APLHC4I aplHC4IRht)            // Right ...

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
    // Split cases based upon the signum of the difference
    switch (signumint (aplHC4ILft.parts[i]
                     - aplHC4IRht.parts[i]))
    {
        case 1:
            return  1;

        case 0:
            break;

        case -1:
            return -1;

        defstop
            break;
    } // End FOR/SWITCH

    // The numbers are equal
    return 0;
} // hc4i_cmp


//***************************************************************************
//  $hc8i_cmp
//
//  Compare two HC8I values
//    returning -1, 0, 1
//***************************************************************************

int hc8i_cmp
    (APLHC8I aplHC8ILft,            // Left arg float
     APLHC8I aplHC8IRht)            // Right ...

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    // Split cases based upon the signum of the difference
    switch (signumint (aplHC8ILft.parts[i]
                     - aplHC8IRht.parts[i]))
    {
        case 1:
            return  1;

        case 0:
            break;

        case -1:
            return -1;

        defstop
            break;
    } // End FOR/SWITCH

    // The numbers are equal
    return 0;
} // hc8i_cmp


//***************************************************************************
//  $hc2f_cmp
//
//  Compare two HC2F values
//    returning -1, 0, 1
//  Because this is a Complex number, there is no total order, but for the
//    purposes of grading, we impose an arbitrary order on it.
//***************************************************************************

int hc2f_cmp
    (APLHC2F  aplHC2FLft,           // Left arg float
     APLHC2F  aplHC2FRht,           // Right ...
     APLFLOAT fQuadCT)              // []CT (0 = exact comparison)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // If this comparison is inexact, ...
        if (fQuadCT NE 0.0
         && EqualHCxFvHCxF ((LPALLTYPES) &aplHC2FLft, (LPALLTYPES) &aplHC2FRht, 2, fQuadCT, __FUNCTION__))
            continue;
        else
        // Split cases based upon the signum of the difference
        switch (signumflt (aplHC2FLft.parts[i]
                         - aplHC2FRht.parts[i]))
        {
            case 1:
                return  1;

            case 0:
                break;

            case -1:
                return -1;

            defstop
                break;
        } // End FOR/IF/ELSE.SWITCH

    // The numbers are equal
    return 0;
} // hc2f_cmp


//***************************************************************************
//  $hc4f_cmp
//
//  Compare two HC4F values
//    returning -1, 0, 1
//  Because this is a Complex number, there is no total order, but for the
//    purposes of grading, we impose an arbitrary order on it.
//***************************************************************************

int hc4f_cmp
    (APLHC4F  aplHC4FLft,           // Left arg float
     APLHC4F  aplHC4FRht,           // Right ...
     APLFLOAT fQuadCT)              // []CT (0 = exact comparison)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // If this comparison is inexact, ...
        if (fQuadCT NE 0.0
         && EqualHCxFvHCxF ((LPALLTYPES) &aplHC4FLft, (LPALLTYPES) &aplHC4FRht, 2, fQuadCT, __FUNCTION__))
            continue;
        else
        // Split cases based upon the signum of the difference
        switch (signumflt (aplHC4FLft.parts[i]
                         - aplHC4FRht.parts[i]))
        {
            case 1:
                return  1;

            case 0:
                break;

            case -1:
                return -1;

            defstop
                break;
        } // End FOR/IF/ELSE/SWITCH

    // The numbers are equal
    return 0;
} // hc4f_cmp


//***************************************************************************
//  $hc8f_cmp
//
//  Compare two HC8F values
//    returning -1, 0, 1
//  Because this is a Complex number, there is no total order, but for the
//    purposes of grading, we impose an arbitrary order on it.
//***************************************************************************

int hc8f_cmp
    (APLHC8F  aplHC8FLft,           // Left arg float
     APLHC8F  aplHC8FRht,           // Right ...
     APLFLOAT fQuadCT)              // []CT (0 = exact comparison)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // If this comparison is inexact, ...
        if (fQuadCT NE 0.0
         && EqualHCxFvHCxF ((LPALLTYPES) &aplHC8FLft, (LPALLTYPES) &aplHC8FRht, 2, fQuadCT, __FUNCTION__))
            continue;
        else
        // Split cases based upon the signum of the difference
        switch (signumflt (aplHC8FLft.parts[i]
                         - aplHC8FRht.parts[i]))
        {
            case 1:
                return  1;

            case 0:
                break;

            case -1:
                return -1;

            defstop
                break;
        } // End FOR/IF/ELSE/SWITCH

    // The numbers are equal
    return 0;
} // hc8f_cmp


//***************************************************************************
//  $hc2r_cmp
//
//  Compare two HC2R values
//    returning -1, 0, 1
//  Because this is a Complex number, there is no total order, but for the
//    purposes of grading, we impose an arbitrary order on it.
//***************************************************************************

int hc2r_cmp
    (APLHC2R  aplHC2RLft,           // Left arg float
     APLHC2R  aplHC2RRht,           // Right ...
     APLFLOAT fQuadCT)              // []CT (0 = exact comparison)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // If this comparison is inexact, ...
        if (fQuadCT NE 0.0
         && EqualHCxRvHCxR ((LPALLTYPES) &aplHC2RLft, (LPALLTYPES) &aplHC2RRht, 2, fQuadCT, __FUNCTION__))
            continue;
        else
        // Split cases based upon the signum of the difference
        switch (signumint (mpq_cmp (&aplHC2RLft.parts[i],
                                    &aplHC2RRht.parts[i])))
        {
            case 1:
                return  1;

            case 0:
                break;

            case -1:
                return -1;

            defstop
                break;
        } // End FOR/IF/ELSE/SWITCH

    // The numbers are equal
    return 0;
} // hc2r_cmp


//***************************************************************************
//  $hc4r_cmp
//
//  Compare two HC4R values
//    returning -1, 0, 1
//  Because this is a Complex number, there is no total order, but for the
//    purposes of grading, we impose an arbitrary order on it.
//***************************************************************************

int hc4r_cmp
    (APLHC4R  aplHC4RLft,           // Left arg float
     APLHC4R  aplHC4RRht,           // Right ...
     APLFLOAT fQuadCT)              // []CT (0 = exact comparison)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // If this comparison is inexact, ...
        if (fQuadCT NE 0.0
         && EqualHCxRvHCxR ((LPALLTYPES) &aplHC4RLft, (LPALLTYPES) &aplHC4RRht, 4, fQuadCT, __FUNCTION__))
            continue;
        else
        // Split cases based upon the signum of the difference
        switch (signumint (mpq_cmp (&aplHC4RLft.parts[i],
                                    &aplHC4RRht.parts[i])))
        {
            case 1:
                return  1;

            case 0:
                break;

            case -1:
                return -1;

            defstop
                break;
        } // End SWITCH

    // The numbers are equal
    return 0;
} // hc4r_cmp


//***************************************************************************
//  $hc8r_cmp
//
//  Compare two HC8R values
//    returning -1, 0, 1
//  Because this is a Complex number, there is no total order, but for the
//    purposes of grading, we impose an arbitrary order on it.
//***************************************************************************

int hc8r_cmp
    (APLHC8R  aplHC8RLft,           // Left arg float
     APLHC8R  aplHC8RRht,           // Right ...
     APLFLOAT fQuadCT)              // []CT (0 = exact comparison)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // If this comparison is inexact, ...
        if (fQuadCT NE 0.0
         && EqualHCxRvHCxR ((LPALLTYPES) &aplHC8RLft, (LPALLTYPES) &aplHC8RRht, 8, fQuadCT, __FUNCTION__))
            continue;
        else
        // Split cases based upon the signum of the difference
        switch (signumint (mpq_cmp (&aplHC8RLft.parts[i],
                                    &aplHC8RRht.parts[i])))
        {
            case 1:
                return  1;

            case 0:
                break;

            case -1:
                return -1;

            defstop
                break;
        } // End FOR/SWITCH

    // The numbers are equal
    return 0;
} // hc8r_cmp


//***************************************************************************
//  $hc2v_cmp
//
//  Compare two HC2V values
//    returning -1, 0, 1
//  Because this is a Complex number, there is no total order, but for the
//    purposes of grading, we impose an arbitrary order on it.
//***************************************************************************

int hc2v_cmp
    (APLHC2V  aplHC2VLft,           // Left arg float
     APLHC2V  aplHC2VRht,           // Right ...
     APLFLOAT fQuadCT)              // []CT (0 = exact comparison)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 2; i++)
        // If this comparison is inexact, ...
        if (fQuadCT NE 0.0
         && EqualHCxVvHCxV ((LPALLTYPES) &aplHC2VLft, (LPALLTYPES) &aplHC2VRht, 2, fQuadCT, __FUNCTION__))
            continue;
        else
        // Split cases based upon the signum of the difference
        switch (signumint (mpfr_cmp_ct (aplHC2VLft.parts[i],
                                        aplHC2VRht.parts[i],
                                        fQuadCT)))
        {
            case 1:
                return  1;

            case 0:
                break;

            case -1:
                return -1;

            defstop
                break;
        } // End FOR/SWITCH

    // The numbers are equal
    return 0;
} // hc2v_cmp


//***************************************************************************
//  $hc4v_cmp
//
//  Compare two HC4V values
//    returning -1, 0, 1
//  Because this is a Complex number, there is no total order, but for the
//    purposes of grading, we impose an arbitrary order on it.
//***************************************************************************

int hc4v_cmp
    (APLHC4V  aplHC4VLft,           // Left arg float
     APLHC4V  aplHC4VRht,           // Right ...
     APLFLOAT fQuadCT)              // []CT (0 = exact comparison)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 4; i++)
        // If this comparison is inexact, ...
        if (fQuadCT NE 0.0
         && EqualHCxVvHCxV ((LPALLTYPES) &aplHC4VLft, (LPALLTYPES) &aplHC4VRht, 4, fQuadCT, __FUNCTION__))
            continue;
        else
        // Split cases based upon the signum of the difference
        switch (signumint (mpfr_cmp (&aplHC4VLft.parts[i],
                                     &aplHC4VRht.parts[i])))
        {
            case 1:
                return  1;

            case 0:
                break;

            case -1:
                return -1;

            defstop
                break;
        } // End FOR/SWITCH

    // The numbers are equal
    return 0;
} // hc4v_cmp


//***************************************************************************
//  $hc8v_cmp
//
//  Compare two HC8V values
//    returning -1, 0, 1
//  Because this is a Complex number, there is no total order, but for the
//    purposes of grading, we impose an arbitrary order on it.
//***************************************************************************

int hc8v_cmp
    (APLHC8V  aplHC8VLft,           // Left arg float
     APLHC8V  aplHC8VRht,           // Right ...
     APLFLOAT fQuadCT)              // []CT (0 = exact comparison)

{
    int i;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        // If this comparison is inexact, ...
        if (fQuadCT NE 0.0
         && EqualHCxVvHCxV ((LPALLTYPES) &aplHC8VLft, (LPALLTYPES) &aplHC8VRht, 8, fQuadCT, __FUNCTION__))
            continue;
        else
        // Split cases based upon the signum of the difference
        switch (signumint (mpfr_cmp (&aplHC8VLft.parts[i],
                                     &aplHC8VRht.parts[i])))
        {
            case 1:
                return  1;

            case 0:
                break;

            case -1:
                return -1;

            defstop
                break;
        } // End FOR/SWITCH

    // The numbers are equal
    return 0;
} // hc8v_cmp


//***************************************************************************
//  $flt_cmp_ct
//
//  Compare two floating point values with a Comparison Tolerance
//    returning -1, 0, 1
//***************************************************************************

int flt_cmp_ct
    (APLFLOAT   aplFloatLft,        // Left arg float
     APLFLOAT   aplFloatRht,        // Right ...
     APLFLOAT   fCompTol,           // Comparison tolerance
     UBOOL      bIntegerTest)       // TRUE iff this is an integer test

{
    APLFLOAT aplLftAbs,
             aplRhtAbs,
             aplHoodLo;

    // If Lft EQ Rht (absolutely), return 0 (equal)
    if (aplFloatLft EQ aplFloatRht)
        return 0;

    // If the comparison tolerance is zero, return signum of the difference
    if (fCompTol EQ 0)
        return signumflt (aplFloatLft - aplFloatRht);

    // Get the absolute values
    aplLftAbs = fabs (aplFloatLft);
    aplRhtAbs = fabs (aplFloatRht);

    // If this is an integer test, allow comparisons with zero
    if (bIntegerTest)
    {
        if (aplFloatLft EQ 0
         && aplRhtAbs <= fCompTol)
            return 0;

        if (aplFloatRht EQ 0
         && aplLftAbs <= fCompTol)
            return 0;
    } // End IF

    // If the signs differ, return signum of the difference
    if (signumflt (aplFloatLft)
     NE signumflt (aplFloatRht))
        return signumflt (aplFloatLft - aplFloatRht);

    // Calculate the low end of the left neighborhood of (|Rht)
    // ***FIXME*** -- Handle exponent underflow in the
    //   following multiplication
    aplHoodLo = aplRhtAbs - aplRhtAbs * fCompTol;

    // If (|Rht) is greater than (|Lft),
    // and (|Lft) is in the
    //    left-neighborhood of (|Rht) with CT, return 0 (equal)
    if (aplHoodLo <= aplLftAbs
     &&              aplLftAbs < aplRhtAbs)
        return 0;

    // Calculate the low end of the left neighborhood of (|Lft)
    // ***FIXME*** -- Handle exponent underflow in the
    //   following multiplication
    aplHoodLo = aplLftAbs - aplLftAbs * fCompTol;

    // If (|Lft) is greater than (|Rht),
    // and (|Rht) is in the
    //    left-neighborhood of (|Lft) with CT, return 0 (equal)
    if (aplHoodLo <= aplRhtAbs
     &&              aplRhtAbs < aplLftAbs)
        return 0;

    // Otherwise, return the signum of the difference
    return signumflt (aplFloatLft - aplFloatRht);
} // End flt_cmp_ct


//***************************************************************************
//  $IntFloatToAplchar
//
//  Convert an integer/floating point number to APLCHARs
//***************************************************************************

void IntFloatToAplchar
    (LPAPLCHAR    lpMemRes,         // Ptr to output save area
     LPAPLLONGEST lpaplLongest)     // Ptr to value to convert

{
    int i;

#define ifta        ((LPCHAR) lpaplLongest)

    for (i = 7; i >= 0; i--)
    {
        *lpMemRes++ = L"0123456789ABCDEF"[(ifta[i] & 0xF0) >> 4];
        *lpMemRes++ = L"0123456789ABCDEF"[(ifta[i] & 0x0F) >> 0];
    } // End FOR
#undef  ifta
} // End IntFloatToAplchar


//***************************************************************************
//  $ConvertWideToName
//
//  Convert wide chars to ASCII or {name} or {\xXXXX}
//***************************************************************************

APLU3264 ConvertWideToName
    (LPWCHAR lpwszOut,              // Ptr to output save buffer
     LPWCHAR lpwszInp)              // Ptr to incoming chars

{
    return ConvertWideToNameLength (lpwszOut,               // Ptr to output save buffer
                                    lpwszInp,               // Ptr to incoming chars
                                    lstrlenW (lpwszInp));   // # chars to convert
} // End ConvertWideToName


//***************************************************************************
//  $ConvertWideToNameLength
//
//  Convert wide chars of a given length to ASCII or {name} or {\xXXXX}
//***************************************************************************

APLU3264 ConvertWideToNameLength
    (LPWCHAR  lpwszOut,             // Ptr to output save buffer
     LPWCHAR  lpwszInp,             // Ptr to incoming chars
     APLU3264 uLen)                 // # chars to convert

{
    WCHAR   wc;                     // A wide char
    LPWCHAR lpwCharName,            // Ptr to char name
            lpwsz;                  // Ptr to output save area

    // Initialize the ptr to the output save area
    lpwsz = lpwszOut;

    // Loop through the wide chars
    while (uLen--)
    {
        // Get the next char
        wc = *lpwszInp++;

        if (32 < wc && wc <= 0x7E
         && wc NE WC_SQ         // Used to surround 'a'
         && wc NE L'#'
         && wc NE L'{'          // Used to surround {symbols}
         && wc NE L'}'          // ...
         && wc NE WC_SLOPE)     // Used in <iniparser_load> for multiline support
            *lpwsz++ = wc;
        else
        // Check for name in hash table
        if (wc NE WC_EOS
         && (lpwCharName = CharToSymbolName (wc)) NE NULL)
            lpwsz += wsprintfW (lpwsz,
                                L"%s",
                                lpwCharName);
        else
            lpwsz += wsprintfW (lpwsz,
                                L"{\\x%04X}",
                                wc);
    } // End WHILE

    // Ensure properly terminated
    *lpwsz = WC_EOS;

    return (APLU3264) (lpwsz - lpwszOut);
} // End ConvertWideToNameLength


//***************************************************************************
//  $FormatCurDateTime
//
//  Format the current date & time as "dd/mm/yyyy hh:mm:ss"
//***************************************************************************

void FormatCurDateTime
    (LPAPLCHAR wszTemp)

{
    SYSTEMTIME SystemTime;

    // Get the current date & time
    if (OptionFlags.bUseLocalTime)
        GetLocalTime  (&SystemTime);
    else
        GetSystemTime (&SystemTime);

    // Format it
    wsprintfW (wszTemp,
               DATETIME_FMT,
               SystemTime.wMonth,
               SystemTime.wDay,
               SystemTime.wYear,
               SystemTime.wHour,
               SystemTime.wMinute,
               SystemTime.wSecond);
} // End FormatCurDateTime


//***************************************************************************
//  $ConvertNameInPlace
//
//  Convert the {name}s and other chars to UTF16_xxx equivalents
//***************************************************************************

LPWCHAR ConvertNameInPlace
    (LPWCHAR lpwSrc)            // Ptr to string to convert

{
    LPWCHAR lpwDst = lpwSrc;
    WCHAR   wcTmp;

    while (*lpwSrc)
    if (*lpwSrc EQ L'{')
    {
        // Get the next char
        wcTmp = SymbolNameToChar (lpwSrc);

        // If there's a matching UTF16_xxx equivalent, ...
        if (wcTmp)
        {
            // Replace {name} with UTF16_xxx equivalent
            *lpwDst++ = wcTmp;

            // Skip over the {name}
            lpwSrc = SkipPastCharW (lpwSrc, L'}');
        } else
        {
            // Copy source to the destin up to and including the matching '}'
            while (*lpwSrc NE L'}')
                *lpwDst++ = *lpwSrc++;

            // Copy the '}'
            *lpwDst++ = *lpwSrc++;
        } // End IF/ELSE
    } else
        *lpwDst++ = *lpwSrc++;

    // Ensure properly terminated
    *lpwDst = WC_EOS;

    return lpwSrc;
} // End ConvertNameInPlace


//***************************************************************************
//  $ConvertToInteger_SCT
//
//  Convert a value to an integer (if possible) using System []CT
//***************************************************************************

APLINT ConvertToInteger_SCT
    (APLSTYPE   aplTypeArg,             // Argument storage type
     LPVOID     lpSymGlbArg,            // ...      global memory ptr
     APLUINT    uArg,                   // Index into <lpSymGlbArg>
     LPUBOOL    lpbRet)                 // Ptr to TRUE iff the result is valid

{
    ALLTYPES atArg = {0};

    // Mark as using SYS_CT
    atArg.enumCT = ENUMCT_SYS;

    // Attempt to convert the value in <lpSymGlbArg> to an INT using System []CT
    (*aTypeActConvert[aplTypeArg][ARRAY_INT]) (lpSymGlbArg, uArg, &atArg, lpbRet);

    return atArg.aplInteger;
} // End ConvertToInteger_SCT


//***************************************************************************
//  $ConvertToRAT_SCT
//
//  Convert a value to a RAT (if possible) using System []CT
//***************************************************************************

APLRAT ConvertToRAT_SCT
    (APLSTYPE   aplTypeArg,             // Argument storage type
     LPVOID     lpSymGlbArg,            // ...      global memory handle
     APLUINT    uArg,                   // Index into <lpSymGlbArg>
     LPUBOOL    lpbRet)                 // Ptr to TRUE iff the result is valid

{
    ALLTYPES atArg = {0};

    // Mark as using SYS_CT
    atArg.enumCT = ENUMCT_SYS;

    // Attempt to convert the value in <lpSymGlbArg> to a RAT using System []CT
    (*aTypeActConvert[aplTypeArg][ARRAY_RAT]) (lpSymGlbArg, uArg, &atArg, lpbRet);

    return atArg.aplRat;
} // End ConvertToRAT_SCT


//***************************************************************************
//  $ConvertToMPI_SCT
//
//  Convert a value to a multi-precision integer (if possible) using System []CT
//***************************************************************************

APLMPI ConvertToMPI_SCT
    (APLSTYPE   aplTypeArg,             // Argument storage type
     LPVOID     lpSymGlbArg,            // ...      global memory handle
     APLUINT    uArg,                   // Index into <lpSymGlbArg>
     LPUBOOL    lpbRet)                 // Ptr to TRUE iff the result is valid

{
    APLMPI aplMPIArg = {0};
    APLRAT aplRatArg = {0};

    // Attempt to convert the value in <lpSymGlbArg> to a RAT using System []CT
    aplRatArg = ConvertToRAT_SCT (aplTypeArg, lpSymGlbArg, uArg, lpbRet);

    // If successful so far, ...
    if (*lpbRet)
    {
        // Izit an MPI?
        *lpbRet = mpq_integer_p (&aplRatArg);

        // If successful, ...
        if (*lpbRet)
            // Save in local var
            mpz_init_set (&aplMPIArg, mpq_numref (&aplRatArg));
    } // End IF

    return aplMPIArg;
} // End ConvertToMPI_SCT


//***************************************************************************
//  $ConvertToFloat
//
//  Convert a value to a float
//***************************************************************************

APLFLOAT ConvertToFloat
    (APLSTYPE   aplTypeArg,             // Argument storage type
     LPVOID     lpSymGlbArg,            // ...      global memory handle
     LPUBOOL    lpbRet)                 // Ptr to TRUE iff the result is valid

{
    ALLTYPES atArg = {0};

    // Attempt to convert the value in <lpSymGlbArg> to a FLT
    (*aTypeActConvert[aplTypeArg][ARRAY_FLOAT]) (lpSymGlbArg, 0, &atArg, lpbRet);

    return atArg.aplFloat;
} // End ConvertToFloat


//***************************************************************************
//  End of File: convert.c
//***************************************************************************
