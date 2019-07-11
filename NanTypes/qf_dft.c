//***************************************************************************
//  NARS2000 -- System Function -- Quad DFT
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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
#undef rad2             // Defined in U:\include\um\DLGS.H for a radio button
#include "headers.h"
#include <intrin.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>
#include "acb_dft.h"
#include "mpfft_pthread.h"


//***************************************************************************
//  $SysFnDFT_EM_YY
//
//  System function:  []DFT -- Discrete Fourier Transform
//***************************************************************************

LPPL_YYSTYPE SysFnDFT_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonDFT_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydDFT_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnDFT_EM_YY


//***************************************************************************
//  $SysFnMonDFT_EM_YY
//
//  Monadic []DFT -- Discrete Fourier Transform
//***************************************************************************

LPPL_YYSTYPE SysFnMonDFT_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return
      SysFnDFTCom_EM_YY (lptkFunc,      // Ptr to function token
                         lptkRhtArg,    // Ptr to right arg token
                        -1,             // GSL:  -1 for forward, +1 for inverse
                         lptkAxis);     // Ptr to axis token (may be NULL)
} // End SysFnMonDFT_EM_YY


//***************************************************************************
//  $SysFnDFTCom_EM_YY
//
//  []DFT -- Discrete Fourier Transform
//***************************************************************************

LPPL_YYSTYPE SysFnDFTCom_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     int     iSign,                 // GSL:  -1 for forward, +1 for inverse
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht,           // Right arg storage type
                      aplTypeRes;           // Result    ...
    APLNELM           aplNELMRht,           // Right arg NELM
                      aplNELMPow2,          // Power of two >= aplNELMRht
                      i;                    // Loop counter
    int               n;                    // Temp
    APLRANK           aplRankRht;           // Right arg rank
    HGLOBAL           hGlbRht = NULL,       // Right arg global memory handle
                      hGlbRes = NULL,       // Result    ...
                      hGlbTmp = NULL;       // Temp      ...
    LPVARARRAY_HEADER lpMemHdrRht = NULL,   // Ptr to right arg global memory header
                      lpMemHdrRes = NULL,   // ...    result    ...
                      lpMemHdrTmp = NULL;   // ...    temp      ...
    LPVOID            lpMemRht,             // Ptr to right arg global memory data
                      lpMemRes,             // ...    result    ...
                      lpMemTmp;             // ...    temp      ...
    APLLONGEST        aplLongestRht;        // Right arg if immediate
    UBOOL             bReUse = FALSE;       // TRUE iff the right arg can be re-used
    int               ErrCode,              // Error code
                      iHCDimRht;            // HC Dimension (1, 2, 4, 8)
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

    // Check for RANK ERROR
    if (IsRank2P (aplRankRht))
        goto RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht)
     || iHCDimRht > 2)
        goto DOMAIN_EXIT;

    // Calculate the largest power of two <= aplNELMRht
    n = 31 - __lzcnt ((int) aplNELMRht);
    aplNELMPow2 = (1ull << n);

    // If aplNELMRht is not a power of two, ...
    if (aplNELMPow2 < aplNELMRht)
        // Skip to the next power of two > aplNELMRht
        aplNELMPow2 *= 2;

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
        case ARRAY_HC2I:
        case ARRAY_HC2F:
            // Set the result storage type
            aplTypeRes = ARRAY_HC2F;

            break;

        case ARRAY_RAT:
        case ARRAY_HC2R:

        case ARRAY_VFP:
        case ARRAY_HC2V:
            // Set the result storage type
            aplTypeRes = ARRAY_HC2V;

            break;

        case ARRAY_ARB:
        case ARRAY_BA2F:
            // Set the result storage type
            aplTypeRes = ARRAY_BA2F;

            break;

        defstop
            goto DOMAIN_EXIT;
    } // End SWITCH

    // Get the right arg global memory handle
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Point to the data
    lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // If the right arg can be re-used, ...
    if (iHCDimRht EQ 2
     && aplTypeRht NE ARRAY_HC2R
     && lpMemHdrRht NE NULL
     && lpMemHdrRht->RefCnt EQ 1)
    {
        // If the power of 2 NELM is larger than the original NELM, ...
        // This case is triggered by
        //   -1 []DFT +A where A is a non-PowerOfTwo length HC2F array
        if (aplNELMPow2 > aplNELMRht)
        {
            APLUINT ByteRes;

            // Calculate the new size
            ByteRes = CalcArraySize (aplTypeRht, aplNELMPow2, 1);
            // Check for error
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            // We no longer need this ptr as we're trying to re-allocate it
            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

            // Attempt to re-allocate the right arg,
            //   moving the old data to the new location, and
            //   freeing the old global memory
            hGlbTmp = MyGlobalReAlloc (hGlbRht, (APLU3264) ByteRes, GHND);

            // Check for success
            if (hGlbTmp NE NULL)
            {
                // Lock the memory to get a ptr to it
                lpMemHdrRht = MyGlobalLockVar (hGlbTmp);

                // Point to the data
                lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

                // Reset various vars to their new values
                *VarArrayBaseToDim (lpMemHdrRht) =
                lpMemHdrRht->NELM                =
                aplNELMRht                       = aplNELMPow2;
                lptkRhtArg->tkData.tkGlbData     =
                hGlbRht                          = MakePtrTypeGlb (hGlbTmp);
                hGlbTmp                          = NULL;

                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_HC2I:
                    case ARRAY_HC2F:
                        // Because the argument is re-used as the result, make them the same
                        hGlbRes = hGlbRht;

                        break;

                    case ARRAY_HC2V:
                        // Loop through the new trailing values
                        for (i = aplNELMRht; i < aplNELMPow2; i++)
                        {
                            mpfr_init0 (&((LPAPLHC2V) lpMemRht)[i].parts[0]);
                            mpfr_init0 (&((LPAPLHC2V) lpMemRht)[i].parts[1]);
                        } // End FOR

                        break;

                    case ARRAY_BA2F:
                        // Loop through the new trailing values
                        for (i = aplNELMRht; i < aplNELMPow2; i++)
                        {
                            Myarb_init (&((LPAPLBA2F) lpMemRht)[i].parts[0]);
                            Myarb_init (&((LPAPLBA2F) lpMemRht)[i].parts[1]);
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Mark as re-using
                bReUse = TRUE;
            } // else
////////////////// Mark as NOT re-using
////////////////bReUse = FALSE;     // Already zero from declaration
        } else
        {
            // If we're re-using the right arg for GSL, ...
            if (aplTypeRes EQ ARRAY_HC2F)
                // Make them the same
                hGlbRes = CopySymGlbDir_PTB (hGlbRht);

            // Mark as re-using
            bReUse = TRUE;
        } // End IF/ELSE
    } // End IF

    // If we're not re-using, ...
    if (hGlbRes EQ NULL)
    {
        // Allocate space for that many HC2Fs/HC2Vs/BA2Fs
        hGlbRes =
          AllocateGlobalArray (aplTypeRes, aplNELMPow2, 1, &aplNELMPow2);
        // Check for error
        if (hGlbRes EQ NULL)
            goto WSFULL_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // If the right arg is an HGLOBAL, ...
    if (hGlbRht NE NULL)
        // Point to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        // Point to the data
        lpMemRht = &aplLongestRht;

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
            // Initialize the GSL temp from the right arg
            for (i = 0; i < aplNELMRht; i++)
            {
                ((LPAPLHC2F) lpMemRes)[i].parts[0] = GetNextFloat (lpMemRht, aplTypeRht, i);
////////////////((LPAPLHC2F) lpMemRes)[i].parts[1] = 0.0;   // Already 0.0 from GHND
            } // End FOR

            break;

        case ARRAY_HC2I:
            // Initialize the GSL temp from the right arg
            for (i = 0; i < aplNELMRht; i++)
            {
                ((LPAPLHC2F) lpMemRes)[i].parts[0] = GetNextFloat (lpMemRht, ARRAY_INT  , 2 * i + 0);
                ((LPAPLHC2F) lpMemRes)[i].parts[1] = GetNextFloat (lpMemRht, ARRAY_INT  , 2 * i + 1);
            } // End FOR

            break;

        case ARRAY_HC2F:
            // If the right arg can NOT be re-used, ...
            if (!bReUse)
                // Initialize the GSL temp from the right arg
                CopyMemory (lpMemRes, lpMemRht, (size_t) aplNELMRht * sizeof (APLHC2F));

            break;

        // Note that the HC2F entries in [aplNELMRht, aplNELMPow2) are already zero from GHND

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_VFP:
        case ARRAY_HC2V:
            // If the right arg can NOT be re-used, ...
            if (!bReUse)
            {
                // Allocate space for a temp HC2V copy of the right arg
                hGlbTmp =
                  AllocateGlobalArray (aplTypeRes, aplNELMPow2, 1, &aplNELMPow2);

                // Check for error
                if (hGlbTmp EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrTmp = MyGlobalLockVar (hGlbTmp);

                // Skip over the header and dimensions to the data
                lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_RAT:
                        // Initialize the VFP temp from the right arg
                        for (i = 0; i < aplNELMRht; i++)
                        {
                            mpfr_init_set_q (&((LPAPLHC2V) lpMemTmp)[i].parts[0], &((LPAPLRAT ) lpMemRht)[i]         , MPFR_RNDN);
                            mpfr_init0      (&((LPAPLHC2V) lpMemTmp)[i].parts[1]);
                        } // End FOR

                        break;

                    case ARRAY_HC2R:
                        // Initialize the VFP temp from the right arg
                        for (i = 0; i < aplNELMRht; i++)
                        {
                            mpfr_init_set_q (&((LPAPLHC2V) lpMemTmp)[i].parts[0], &((LPAPLHC2R) lpMemRht)[i].parts[0], MPFR_RNDN);
                            mpfr_init_set_q (&((LPAPLHC2V) lpMemTmp)[i].parts[1], &((LPAPLHC2R) lpMemRht)[i].parts[1], MPFR_RNDN);
                        } // End FOR

                        break;

                    case ARRAY_VFP:
                        // Initialize the VFP temp from the right arg
                        for (i = 0; i < aplNELMRht; i++)
                        {
                            mpfr_init_set   (&((LPAPLHC2V) lpMemTmp)[i].parts[0], &((LPAPLVFP ) lpMemRht)[i]         , MPFR_RNDN);
                            mpfr_init0      (&((LPAPLHC2V) lpMemTmp)[i].parts[1]);
                        } // End FOR

                        break;

                    case ARRAY_HC2V:
                        // Initialize the VFP temp from the right arg
                        for (i = 0; i < aplNELMRht; i++)
                        {
                            mpfr_init_set   (&((LPAPLHC2V) lpMemTmp)[i].parts[0], &((LPAPLHC2V) lpMemRht)[i].parts[0], MPFR_RNDN);
                            mpfr_init_set   (&((LPAPLHC2V) lpMemTmp)[i].parts[1], &((LPAPLHC2V) lpMemRht)[i].parts[1], MPFR_RNDN);
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Initialize the VFP temp to 0
                for (; i < aplNELMPow2; i++)
                {
                    mpfr_init0 (&((LPAPLHC2V) lpMemTmp)[i].parts[0]);
                    mpfr_init0 (&((LPAPLHC2V) lpMemTmp)[i].parts[1]);
                } // End FOR

                if (lpMemHdrRht NE NULL)
                {
                    // We no longer need this ptr as we've re-allocated it
                    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
                } // End IF

                // Save as the new right arg
                hGlbRht     = hGlbTmp;
                lpMemHdrRht = lpMemHdrTmp;
                lpMemRht    = lpMemTmp;

                // We no longer need these ptrs
                lpMemHdrTmp = NULL;
                lpMemTmp    = NULL;
            } // End IF

            // Initialize the VFP result to 0
            for (i = 0; i < aplNELMPow2; i++)
            {
                mpfr_init0 (&((LPAPLHC2V) lpMemRes)[i].parts[0]);
                mpfr_init0 (&((LPAPLHC2V) lpMemRes)[i].parts[1]);
            } // End FOR

            break;

        case ARRAY_ARB:
        case ARRAY_BA2F:
            // If the right arg can NOT be re-used, ...
            if (!bReUse)
            {
                // Allocate space for a temp BA2F copy of the right arg
                hGlbTmp =
                  AllocateGlobalArray (aplTypeRes, aplNELMPow2, 1, &aplNELMPow2);

                // Check for error
                if (hGlbTmp EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrTmp = MyGlobalLockVar (hGlbTmp);

                // Skip over the header and dimensions to the data
                lpMemTmp = VarArrayDataFmBase (lpMemHdrTmp);

                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_ARB:
                        // Initialize the ARB temp from the right arg
                        for (i = 0; i < aplNELMRht; i++)
                        {
                              arb_init_set (&((LPAPLBA2F) lpMemTmp)[i].parts[0], &((LPAPLARB) lpMemRht)[i]);
                            Myarb_init     (&((LPAPLBA2F) lpMemTmp)[i].parts[1]);
                        } // End FOR

                        break;

                    case ARRAY_BA2F:
                        // Initialize the ARB temp from the right arg
                        for (i = 0; i < aplNELMRht; i++)
                        {
                              arb_init_set (&((LPAPLBA2F) lpMemTmp)[i].parts[0], &((LPAPLBA2F) lpMemRht)[i].parts[0]);
                              arb_init_set (&((LPAPLBA2F) lpMemTmp)[i].parts[1], &((LPAPLBA2F) lpMemRht)[i].parts[1]);
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Initialize the ARB temp to 0/0
                for (; i < aplNELMPow2; i++)
                {
                    Myarb_init (&((LPAPLBA2F) lpMemTmp)[i].parts[0]);
                    Myarb_init (&((LPAPLBA2F) lpMemTmp)[i].parts[1]);
                } // End FOR

                if (lpMemHdrRht NE NULL)
                {
                    // We no longer need this ptr as we've re-allocated it
                    MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
                } // End IF

                // Save as the new right arg
                hGlbRht     = hGlbTmp;
                lpMemHdrRht = lpMemHdrTmp;
                lpMemRht    = lpMemTmp;

                // We no longer need these ptrs
                lpMemHdrTmp = NULL;
                lpMemTmp    = NULL;
            } // End IF

            // Initialize the ARB result to 0/0
            for (i = 0; i < aplNELMPow2; i++)
            {
                Myarb_init (&((LPAPLBA2F) lpMemRes)[i].parts[0]);
                Myarb_init (&((LPAPLBA2F) lpMemRes)[i].parts[1]);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_HC2F:
            // Split cases based upon iSign
            switch (iSign)
            {
                case -1:        // Forward
                    // Call the GSL routine
                    ErrCode =
                      gsl_fft_complex_radix2_forward ((LPAPLFLOAT) lpMemRes, 1, (int) aplNELMPow2);
                    break;

                case +1:        // Inverse
                    // Call the GSL routine
                    ErrCode =
                      gsl_fft_complex_radix2_inverse ((LPAPLFLOAT) lpMemRes, 1, (int) aplNELMPow2);
                    break;

                defstop
                    break;
            } // End SWITCH

            // Check the error code
            if (ErrCode NE GSL_SUCCESS)
                goto DOMAIN_EXIT;

            break;

        case ARRAY_HC2V:
        {
            int prec = (int) GetQuadFPC (),
                nbits;
            mpfft_plan plan;

            // Calculate the largest power of two <= prec
            nbits = 31 - __lzcnt ((int) aplNELMPow2);

            mpfft_pthread_init (prec);

            // Note that MPFFT uses the same convention as GSL:  -1 for forward, +1 for inverse
            plan = mpfft_pthread_create_plan_1d (lpMemRes, lpMemRht, 2, nbits, prec, iSign, TRUE);
            mpfft_pthread_execute (plan);
            mpfft_pthread_destroy_plan (&plan);

            break;
        } // End ARRAY_HC2V

        case ARRAY_BA2F:
        {
            mp_limb_signed_t prec = ARB_PREC_FPC;

            // Split cases based upon iSign
            switch (iSign)
            {
                case -1:        // Forward
                    // Call the ACB routine
                    acb_dft         ((acb_ptr) lpMemRes, (acb_ptr) lpMemRht, (slong) aplNELMPow2, prec);

                    break;

                case +1:        // Inverse
                    // Call the ARB routine

                    // Call the ACB routine
                    acb_dft_inverse ((acb_ptr) lpMemRes, (acb_ptr) lpMemRht, (slong) aplNELMPow2, prec);

                    break;

                defstop
                    break;
            } // End SWITCH

            break;
        } // End ARRAY_BA2F

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbTmp NE NULL)
    {
        if (lpMemHdrTmp NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemHdrTmp = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbTmp); hGlbTmp = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDFTCom_EM_YY


//***************************************************************************
//  $SysFnDydDFT_EM_YY
//
//  Dyadic []DFT -- Discrete Fourier Transform
//***************************************************************************

LPPL_YYSTYPE SysFnDydDFT_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft;           // Left arg storage type
    APLNELM           aplNELMLft;           // Left arg NELM
    APLRANK           aplRankLft;           // Left arg rank
    HGLOBAL           hGlbLft = NULL;       // Left arg global memory handle
    LPVARARRAY_HEADER lpMemHdrLft = NULL;   // Ptr to left arg global memory header
    LPVOID            lpMemLft;             // Ptr to left arg global memory data
    APLLONGEST        aplLongestLft;        // Left arg if immediate
    APLINT            iSign;                // The left arg to this function uses the convention
                                            //   APL:  +1 for forward, -1 for inverse;
                                            //   however, GSL uses the opposite convention.
    UBOOL             bRet;                 // TRUE iff the result is valid
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for RANK ERROR
    if (IsRank2P (aplRankLft))
        goto RANK_EXIT;

    // Check for LENGTH ERROR
    if (!IsSingleton (aplNELMLft))
        goto LENGTH_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeLft))
        goto DOMAIN_EXIT;

    // Get the left arg global memory handle
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);

    // If the left arg is an HGLOBAL, ...
    if (hGlbLft NE NULL)
        // Point to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    else
        // Point to the data
        lpMemLft = &aplLongestLft;

    // Calculate the sign
    iSign = ConvertToInteger_SCT (aplTypeLft, lpMemLft, 0, &bRet);

    // Check for DOMAIN ERROR
    if (!bRet || abs64 (iSign) NE 1)
        goto DOMAIN_EXIT;

    // The sign is
    //   APL:  +1 for forward, -1 for inverse;
    //   however, GSL/MPFFT use the opposite convention
    //   so we must negate iSign in the
    //   following call to common code.

    // Call the common code
    lpYYRes =
      SysFnDFTCom_EM_YY (lptkFunc,      // Ptr to function token
                         lptkRhtArg,    // Ptr to right arg token
                   (int) -iSign,        // GSL:  -1 for forward, +1 for inverse
                         lptkAxis);     // Ptr to axis token (may be NULL)
    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydDFT_EM_YY


//***************************************************************************
//  End of File: qf_dft.c
//***************************************************************************
