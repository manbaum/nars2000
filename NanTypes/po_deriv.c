//***************************************************************************
//  NARS2000 -- Primitive Operator -- Deriv
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
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_deriv.h>

#include "headers.h"

typedef int (*GSL_DERIV)
    (const gsl_function *f,
     double x, double h,
     double *result, double *abserr);

typedef int (*MPFR_DERIV)
    (const gsl_function_vfp *f,
     LPAPLVFP                x,
     LPAPLVFP                h,
     LPAPLVFP                result,
     LPAPLVFP                abserr);

typedef int (*MPFR_DERIV_CALC)
    (const gsl_function_vfp *f,
     LPAPLVFP                x,
     LPAPLVFP                h,
     LPAPLVFP                result,
     LPAPLVFP                abserr_round,
     LPAPLVFP                abserr_trunc);

typedef int (*ARB_DERIV)
    (const gsl_function_arb *f,
     LPAPLARB                x,
     LPAPLARB                h,
     LPAPLARB                result,
     LPAPLARB                abserr);

typedef int (*ARB_DERIV_CALC)
    (const gsl_function_arb *f,
     LPAPLARB                x,
     LPAPLARB                h,
     LPAPLARB                result,
     LPAPLARB                abserr_round,
     LPAPLARB                abserr_trunc);

typedef struct tagLCLPARAMS
{
    LPTOKEN         lptkLftArg;
    LPPL_YYSTYPE    lpYYFcnStrLft;
    LPUBOOL         lpbCtrlBreak;
    BOOL            bInitPTD;
    EXCEPTION_CODES exCode;        /* Exception code from lclFuncXXX */
} LCL_PARAMS, *LPLCL_PARAMS;


//***************************************************************************
//  $PrimOpDeriv_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator Deriv ("derivative" and "derivative")
//***************************************************************************

LPPL_YYSTYPE PrimOpDeriv_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DERIV);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return
          PrimOpDerivCommon_EM_YY (NULL,            // Ptr to left arg token (may be NULL if monadic)
                                   lpYYFcnStrOpr,   // Ptr to operator function strand
                                   lptkRhtArg,      // Ptr to right arg (may be NULL if niladic)
                                   FALSE);          // TRUE iff ptototyping
    else
        return
          PrimOpDerivCommon_EM_YY (lptkLftArg,      // Ptr to left arg token
                                   lpYYFcnStrOpr,   // Ptr to operator function strand
                                   lptkRhtArg,      // Ptr to right arg token
                                   FALSE);          // TRUE iff ptototyping
} // End PrimOpDeriv_EM_YY


//***************************************************************************
//  $PrimProtoOpDeriv_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Deriv ("derivative" and "derivative")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpDeriv_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return
          PrimOpDerivCommon_EM_YY (NULL,            // Ptr to left arg token (may be NULL if monadic)
                                   lpYYFcnStrOpr,   // Ptr to operator function strand
                                   lptkRhtArg,      // Ptr to right arg token (may be NULL if niladic)
                                   TRUE);           // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return
          PrimOpDerivCommon_EM_YY (lptkLftArg,      // Ptr to left arg token
                                   lpYYFcnStrOpr,   // Ptr to operator function strand
                                   lptkRhtArg,      // Ptr to right arg token
                                   TRUE);           // TRUE iff prototyping
} // End PrimProtoOpDeriv_EM_YY


//***************************************************************************
//  $PrimOpDerivCommon_EM_YY
//
//  Primitive operator for monadic/dyadic derived function from Deriv ("derivative")
//***************************************************************************

LPPL_YYSTYPE PrimOpDerivCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)         // TRUE if prototyping

{
    return
      PrimOpDerivNewCentral_EM_YY
        (lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
         lpYYFcnStrOpr,         // Ptr to operator function strand
         lptkRhtArg,            // Ptr to right arg token (may be NULL if niladic)
         bPrototyping,          // TRUE if prototyping
         0,                     // Sampling frequency exponent (0 = use default)
         0);                    // Order (0 = use default)
} // End PrimOpDerivCommon_EM_YY


//***************************************************************************
//  $PrimOpDerivNewCentral_EM_YY
//
//  Primitive operator for monadic/dyadic derived function from Deriv ("derivative")
//
//  Numerical Differentiation Central Differencing
//    using the new approximations
//
//  This code was translated from the paper
//
//  "Taylor Series Based Finite Difference Approximations of Higher-degree Derivatives",
//  Ishtiaq Rasool Khan, Ryoji Ohba
//  Journal of Computational and Applied Mathematics 154 (2003) 115-124, Elsevier
//***************************************************************************

LPPL_YYSTYPE PrimOpDerivNewCentral_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping,         // TRUE if prototyping
     int          iOrder,               // Order
     int          iSFE)                 // Sampling frequency exponent

{
    LPPL_YYSTYPE      lpYYFcnStrLft,    // Ptr to function strand
                      lpYYFcnNext;      // Ptr to function after Derivative symbols
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function
    LPTOKEN           lptkAxisOpr;      // Ptr to operator axis token (may be NULL)
    APLSTYPE          aplTypeRht;       // Storage type of the right arg
    APLNELM           aplNELMRht;       // NELM         ...
    UBOOL             bRet,             // TRUE iff the result is valid
                      bException;       // TRUE iff lclfuncXXX found an exception
    LCL_PARAMS        params = {0};     // Local parameters
    int               iDegree;          // The degree of the derivative
    APLARB            aplArbRht = {0};  // The argument to the function
    APLVFP            aplVfpRht = {0};  // The argument to the function
    APLFLOAT          aplFltRht;        // The argument to the function
    double            resultFlt;        // The FP result
    double            abserrFlt;        // Absolute error
    APLSTYPE          aplTypeRes;       // The result storage type
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    EXCEPTION_CODES   exCode;           // Exception code from lclfuncXXX

    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************

    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = GetMonLftOper (lpYYFcnStrOpr, lptkAxisOpr); Assert (lpYYFcnStrLft NE NULL);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, NULL, NULL);

    // Ensure the right arg is a simple singleton
    if (!IsSingleton (aplNELMRht))
        goto RIGHT_LENGTH_EXIT;
////if (!IsSimpleNum (aplTypeRht))
////    goto RIGHT_DOMAIN_EXIT;

    // If the right arg is VFP or RAT, ...
    if (IsAnyVfp (aplTypeRht)
     || IsAnyRat (aplTypeRht))
    {
        // Get the singleton value and check for errors
        aplVfpRht = GetNextVfpToken (lptkRhtArg, 0, aplTypeRht, &bRet);

        // Set the result type
        aplTypeRes = ARRAY_VFP;
    } else
    // If the right arg is ARB, ...
    if (IsAnyArb (aplTypeRht))
    {
        // Get the singleton value and check for errors
        aplArbRht = GetNextArbToken (lptkRhtArg, 0, aplTypeRht, &bRet);

        // Set the result type
        aplTypeRes = ARRAY_ARB;
    } else
    {
        // Get the singleton value and check for errors
        aplFltRht = GetNextFloatToken (lptkRhtArg, 0, aplTypeRht, &bRet);

        // Set the result type
        aplTypeRes = ARRAY_FLOAT;
    } // End IF/ELSE

    // Check for errors
    if (!bRet)
        goto RIGHT_DOMAIN_EXIT;

    // Count the degree of the derivative
    lpYYFcnNext = lpYYFcnStrLft;

    // Initialize
    iDegree = 1;

    while (IsTknFcnOpr (&lpYYFcnNext->tkToken)
        && lpYYFcnNext->tkToken.tkData.tkChar EQ UTF16_DERIV)
    {
        // Count it in
        iDegree++;

        // Check for axis operator
        lptkAxisOpr = CheckAxisOper (lpYYFcnNext);

        //***************************************************************
        // The derived functions from this operator are not sensitive
        //   to the axis operator, so signal a syntax error if present
        //***************************************************************

        if (lptkAxisOpr NE NULL)
            goto AXIS_SYNTAX_EXIT;

        // Skip to the next operand
        lpYYFcnNext = GetMonLftOper (lpYYFcnNext, NULL);
    } // End WHILE

    // Split cases depending on whether or not we're prototyping
    if (bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (lpPrimProtoLft EQ NULL)
            goto LEFT_OPERAND_NONCE_EXIT;

        // Save the function in <params> so it may be referenced by the
        //   local function called by GSL
        // Note that we cast the function strand to LPPL_YYSTYPE
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        // ***FIXME*** -- this prototype function is for the
        //   FIRST DERIVATIVE only.  We need something else for
        //   higher degree derivatives.
        params.lpYYFcnStrLft = (LPPL_YYSTYPE) lpPrimProtoLft;
    } else
        // Save the function in <params> so it may be referenced by the
        //   local function called by GSL
        params.lpYYFcnStrLft = lpYYFcnNext;

    // Check for errors
    if (iDegree < 1
     || iDegree > MAX_DEGREE)
        goto DEPTH_EXIT;

    if ((iOrder  < MIN_ORDER && iOrder NE 0)
     || iOrder  > MAX_ORDER
     || iSFE    > 0)
        goto DOMAIN_EXIT;

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Fill in the common function parameters
    params.lptkLftArg    =  lptkLftArg;
    params.lpbCtrlBreak  = &lpplLocalVars->bCtrlBreak;
    params.lpYYFcnStrLft =  lpYYFcnNext;
    params.bInitPTD      =  FALSE;
    params.exCode        =  EXCEPTION_SUCCESS;

    // Mark as *NOT* signalling an exception
    bException = FALSE;

    __try
    {
        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_VFP:
            {
                mpfr_function V;
                mpfr_t        resultVfp = {0},
////                          abserrVfp = {0},
                              arg;

                // Fill in the MPFR function parameters
                V.function          = &lclFuncVfp2;
                V.params            = &params;

                // Set the ptr to the arg
                mpfr_init_set (arg , &aplVfpRht, MPFR_RNDN);

                // Initialize
                mpfr_init (resultVfp);
////            mpfr_init (abserrVfp);

                // Call library code
                mpfr_deriv_central
                  (resultVfp,       // The result
                  &V,               // Ptr to the function to differentiate
                   arg,             // The argument to the function to diff
                   NULL,            // abserrVfp,     // Absolute Error (may be NULL)
                   iDegree,         // Degree of derivative (1st, 2nd, etc.)
                   iOrder,          // Sample size (0 = use default)
                   iSFE,            // Sampling Frequency Exponent (0 = use default)
                   MPFR_RNDN);      // Rounding mode

                // ***FINISHME*** -- Check the absolute error in <abserrVfp>
                //                   In Ball Arithmetic, compare it against the Radius

                // We no longer need this storage
////            Myf_clear (abserrVfp);

                // Check for error
                if (params.exCode NE EXCEPTION_SUCCESS)
                {
                    // Mark as signalling an exception
                    bException = TRUE;

                    goto EXCEPTION_START;
                } // End IF

                // Save aplVfpRes as a scalar in a global
                return SaveGlbNumeric_EM (aplTypeRes, resultVfp, lpYYFcnStrLft->tkToken.tkCharIndex);
            } // End ARRAY_VFP

            case ARRAY_ARB:
            {
                arb_function A;
                arb_t        resultArb = {0},
////                         abserrArb = {0},
                             arg = {0};

                // Fill in the ARB function parameters
                A.function          = &lclFuncArb2;
                A.params            = &params;

                // Set the ptr to the arg
                arb_init_set (arg , &aplArbRht);

                // Initialize
                arb_init (resultArb);
////            arb_init (abserrArb);

                // Call library code
                arb_deriv_central
                  (resultArb,       // The result
                  &A,               // Ptr to the function to differentiate
                   arg,             // The argument to the function to diff
                   iDegree,         // Degree of derivative (1st, 2nd, etc.)
                   iOrder,          // Sample size (0 = use default)
                   iSFE,            // Sampling Frequency Exponent (0 = use default)
                   ARB_PREC_FPC);   // Rounding mode

                // We no longer need this storage
////            arb_clear (abserrArb);

                // Check for error
                if (params.exCode NE EXCEPTION_SUCCESS)
                {
                    // Mark as signalling an exception
                    bException = TRUE;

                    goto EXCEPTION_START;
                } // End IF

                // Save aplArbRes as a scalar in a global
                return SaveGlbNumeric_EM (aplTypeRes, resultArb, lpYYFcnStrLft->tkToken.tkCharIndex);
            } // End ARRAY_ARB

            case ARRAY_FLOAT:
            {
                gsl_function F;
                GSL_ERRNO    ErrCode;       // GSL Error code
                LPPL_YYSTYPE lpYYRes;       // Ptr to the result

                // Fill in the MPFR function parameters
                F.function          = &lclFuncFlt;
                F.params            = &params;

                // Call library code
                ErrCode =
                  gsl_deriv2_central
                    (&F,                // Ptr to the function to differentiate
                      aplFltRht,        // The argument to the function to diff
                     &resultFlt,        // The result
                     &abserrFlt,        // &abserrFlt,        // Absolute error (may be NULL)
                      iDegree,          // Degree of derivative (1st, 2nd, etc.)
                      iOrder,           // Sample size (0 = use default)
                      iSFE);            // Sampling Frequency Exponent (0 = use default)
                // Check for error
                if (ErrCode NE GSL_SUCCESS
                 || params.exCode NE EXCEPTION_SUCCESS)
                {
                    // If the lclFuncXXX code is success, ...
                    if (params.exCode EQ EXCEPTION_SUCCESS)
                        // Set the exception code
                        exCode = EXCEPTION_DOMAIN_ERROR;
                    else
                        // Copy the exception code
                        exCode = params.exCode;

                    // Mark as signalling an exception
                    bException = TRUE;

                    goto EXCEPTION_START;
                } // End IF
#ifdef DEBUG
                APLARB absErr = {0};

                arb_init (&absErr);
                arf_set_d (arb_midref (&absErr), resultFlt);
                mag_set_d (arb_radref (&absErr), abserrFlt);

                // Save absErr in global memory
                lpYYRes = SaveGlbNumeric_EM (ARRAY_ARB,
                                            &absErr,
                                             lpYYFcnStrLft->tkToken.tkCharIndex);
#else
                // Allocate a new YYRes
                lpYYRes = YYAlloc ();

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkFloat    = resultFlt;
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;
#endif
                return lpYYRes;
            } // End ARRAY_FLOAT

            defstop
                break;
        } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", exCode, FNLN);

        // Mark as signalling an exception
        bException = TRUE;
    } // End __try/__except

EXCEPTION_START:
    // If lclfuncXXX found an exception, ...
    if (bException)
    // Split cases based upon the ExceptionCode
    switch (exCode)
    {
        case EXCEPTION_LENGTH_ERROR:
            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

            goto LENGTH_EXIT;

        case EXCEPTION_DOMAIN_ERROR:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

            goto DOMAIN_EXIT;

        case EXCEPTION_WS_FULL:
            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

            goto WSFULL_EXIT;

        case EXCEPTION_CTRL_BREAK:
            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

            goto ERROR_EXIT;

        case EXCEPTION_NONCE_ERROR:
            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

            goto NONCE_EXIT;

        case EXCEPTION_ERRMSG_ALREADY_SET:
            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

            goto ERRMSG_ALREADY_SET_EXIT;

        default:
#ifdef DEBUG
            DbgBrk ();

            // Display message for unhandled exception
            DisplayException ();
#endif
            goto LIMIT_EXIT;
    } // End IF/SWITCH

    Assert (FALSE);         // We should never get here

    goto RIGHT_DOMAIN_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

DEPTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DEPTH_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LIMIT_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LIMIT_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

ERRMSG_ALREADY_SET_EXIT:
ERROR_EXIT:
    return NULL;
} // End PrimOpDerivNewCentral_EM_YY


//***************************************************************************
//  $PrimOpDerivNewBackward_EM_YY
//
//  Numerical Differentiation Backward Differencing
//    using the new approximations
//***************************************************************************

LPPL_YYSTYPE PrimOpDerivNewBackward_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic derived function)
                                    //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                    //     then this value is the original right arg
     LPPL_YYSTYPE lpYYFcnStrLft,    // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     UBOOL        bPrototyping,     // TRUE iff prototyping
     APLUINT      iOrder,           // Order
     APLINT       iSFE)             // Sampling frequency exponent (0 = default)

{
    return
      PrimFnNonceError_EM (&lpYYFcnStrLft->tkToken APPEND_NAME_ARG);
} // End PrimOpDerivNewBackward_EM_YY


//***************************************************************************
//  $PrimOpDerivNewForward_EM_YY
//
//  Numerical Differentiation Forward Differencing
//    using the new approximations
//***************************************************************************

LPPL_YYSTYPE PrimOpDerivNewForward_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic derived function)
                                    //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                    //     then this value is the original right arg
     LPPL_YYSTYPE lpYYFcnStrLft,    // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     UBOOL        bPrototyping,     // TRUE iff prototyping
     APLUINT      iOrder,           // Order
     APLINT       iSFE)             // Sampling frequency exponent (0 = default)

{
    return
      PrimFnNonceError_EM (&lpYYFcnStrLft->tkToken APPEND_NAME_ARG);
} // End PrimOpDerivNewForward_EM_YY


//***************************************************************************
//  $PrimOpDerivGSLCommon_EM_YY
//
//  Primitive operator for monadic/dyadic derived function from Deriv ("derivative")
//***************************************************************************

LPPL_YYSTYPE PrimOpDerivGSLCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrLft,        // Ptr to function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping,         // TRUE if prototyping
     APLINT       iSFE,                 // Sampling frequency exponent (0 = default)
     APLUINT      iOrder,               // Order ***UNUSED***
     void        *gsl_deriv,            // Ptr to <gsl_deriv_[central|forward|backward]>
     void        *mpfr_deriv,           // ...    <mpfr_deriv_[central|forward|backward}>
     void        *arb_deriv)            // ...    <arb_deriv_[central|forward|backward}>

{
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function
    GSL_ERRNO         ErrCode;              // Error code from GSL
    gsl_function      F;                    // GSL function for call to gsl_deriv_xxx
    gsl_function_vfp  V;                    // VFP ...
    gsl_function_arb  A;                    // ARB ...
    APLFLOAT          resultFlt,            // The result as a FLT
                      abserrFlt;            // Absolute error ...
    APLARB            resultArb = {0},      // The result as a ARB
                      abserrArb = {0};      // Absolute error ...
    APLVFP            resultVfp = {0},      // The result as a VFP
                      abserrVfp = {0};      // Absolute error ...
    APLSTYPE          aplTypeRht,           // Storage type of the right arg
                      aplTypeRes;           // ...                 result
    APLNELM           aplNELMRht,           // NELM         ...
                      aplNELMRes = 1;       // ...
    APLFLOAT          aplFltRht,            // Immediate value ...
                      dSFE;                 // FLT version of 10^iSFE
    APLARB            aplArbRht = {0},      // ARB       ...
                      arbSFE = {0};         // ARB version of 10^iSFE
    APLVFP            aplVfpRht = {0},      // VFP       ...
                      vfpSFE = {0};         // VFP version of 10^iSFE
    HGLOBAL           hGlbRes = NULL;
    UBOOL             bRet,                 // TRUE iff the result is valid
                      bException;           // TRUE iff lclfuncXXX found an exception
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    LCL_PARAMS        params = {0};         // Local parameters
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    EXCEPTION_CODES   exCode;               // Exception code from lclfuncXXX

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Fill in the GSL function parameters
    V.function          = &lclFuncVfp;
    F.function          = &lclFuncFlt;
    A.function          = &lclFuncArb;
    V.params            =
    F.params            =
    A.params            = &params;
    params.lptkLftArg   =  lptkLftArg;
    params.exCode       =  EXCEPTION_SUCCESS;

    // Get the ptr to the Ctrl-Break flag
    params.lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, NULL, NULL);

    // Ensure the right arg is a simple singleton
    if (!IsSingleton (aplNELMRht))
        goto RIGHT_LENGTH_EXIT;
////if (!IsSimpleNum (aplTypeRht))
////    goto RIGHT_DOMAIN_EXIT;

    // If the right arg is VFP or RAT, ...
    if (IsAnyVfp (aplTypeRht)
     || IsAnyRat (aplTypeRht))
    {
        // Get the singleton value and check for errors
        aplVfpRht = GetNextVfpToken   (lptkRhtArg, 0, aplTypeRht, &bRet);

        // Set the result type
        aplTypeRes = ARRAY_VFP;
    } else
    // If the right arg is ARB, ...
    if (IsAnyArb (aplTypeRht))
    {
        // Get the singleton value and check for errors
        aplArbRht = GetNextArbToken   (lptkRhtArg, 0, aplTypeRht, &bRet);

        // Set the result type
        aplTypeRes = ARRAY_ARB;
    } else
    {
        // Get the singleton value and check for errors
        aplFltRht = GetNextFloatToken (lptkRhtArg, 0, aplTypeRht, &bRet);

        // Set the result type
        aplTypeRes = ARRAY_FLOAT;
    } // End IF

    // Check for errors
    if (!bRet)
        goto RIGHT_DOMAIN_EXIT;

    // Split cases based upon the result type
    switch (aplTypeRes)
    {
        case ARRAY_FLOAT:
            // If the Sampling Frequency is unset, ...
            if (iSFE EQ 0)
                // Use the default
                iSFE = -2;

            dSFE = pow (10, (double) iSFE);

            break;

        case ARRAY_VFP:
            // Initialize
            mpfr_init (&vfpSFE);

            // If the Sampling Frequency is unset, ...
            if (iSFE EQ 0)
                // Use the default
                mpfr_set_sj (&vfpSFE, -(int) floor (0.5 + mpfr_get_default_prec () / 32.0), MPFR_RNDN);
            else
                // Use the given SFE, translated into VFP
                mpfr_set_sj (&vfpSFE, iSFE, MPFR_RNDN);
            mpfr_exp10  (&vfpSFE, &vfpSFE, MPFR_RNDN);

            break;

        case ARRAY_ARB:
        {
            APLARB con10 = {0};

            // Initialize
            arb_init (&arbSFE);

            // If the Sampling Frequency is unset, ...
            if (iSFE EQ 0)
                // Use the default
                arb_set_sx (&arbSFE, -(int) floor (0.5 + ARB_PREC_FPC / 32.0));
            else
                // Use the given SFE, translated into VFP
                arb_set_sx (&arbSFE, iSFE);
            arb_init (&con10);
            arb_set_si (&con10, 10);
            arb_pow  (&arbSFE, &con10, &arbSFE, ARB_PREC_FPC);
            arb_clear (&con10);

            break;
        } // ARRAY_ARB

        defstop
            break;
    } // End SWITCH

    // Split cases depending on whether or not we're prototyping
    if (bPrototyping)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (lpPrimProtoLft EQ NULL)
            goto LEFT_OPERAND_NONCE_EXIT;

        // Save the function in <params> so it may be referenced by the
        //   local function called by GSL
        // Note that we cast the function strand to LPPL_YYSTYPE
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        params.lpYYFcnStrLft = (LPPL_YYSTYPE) lpPrimProtoLft;
    } else
        // Save the function in <params> so it may be referenced by the
        //   local function called by GSL
        params.lpYYFcnStrLft = lpYYFcnStrLft;

    // Mark as *NOT* signalling an exception
    bException = FALSE;

    __try
    {
        // Split cases based upon the storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
            case ARRAY_FLOAT:
                // Call the GSL derivative function
                ErrCode =
                  (*(GSL_DERIV) gsl_deriv)
                    (&F,
                      aplFltRht,
                      dSFE,
                     &resultFlt,
                     &abserrFlt);
                // ***FINISHME*** -- Check the absolute error in <abserrFlt>
                //                   In Ball Arithmetic, compare it against the Radius

                // Check for error
                if (ErrCode NE GSL_SUCCESS
                 || params.exCode NE EXCEPTION_SUCCESS)
                {
                    // If the lclFuncXXX code is success, ...
                    if (params.exCode EQ EXCEPTION_SUCCESS)
                        // Set the exception code
                        exCode = EXCEPTION_DOMAIN_ERROR;
                    else
                        // Copy the exception code
                        exCode = params.exCode;

                    // Mark as signalling an exception
                    bException = TRUE;

                    goto EXCEPTION_START;
                } // End IF

                break;

            case ARRAY_VFP:
                mpfr_init  (&resultVfp);
                mpfr_init  (&abserrVfp);

                // Call the VFP derivative function
                ErrCode =
                  (*(MPFR_DERIV) mpfr_deriv)
                    (&V,
                     &aplVfpRht,
                     &vfpSFE,
                     &resultVfp,
                     &abserrVfp);
                // ***FINISHME*** -- Check the absolute error in <abserrVfp>
                //                   In Ball Arithmetic, compare it against the Radius

                Myf_clear (&abserrVfp);
                Myf_clear (&aplVfpRht);

                // Check for error
                if (params.exCode NE EXCEPTION_SUCCESS)
                {
                    // Copy the exception code
                    exCode = params.exCode;

                    // Mark as signalling an exception
                    bException = TRUE;

                    goto EXCEPTION_START;
                } // End IF

                break;

            case ARRAY_ARB:
                arb_init  (&resultArb);
                arb_init  (&abserrArb);

                // Call the ARB derivative function
                ErrCode =
                  (*(ARB_DERIV) arb_deriv)
                    (&A,
                     &aplArbRht,
                     &arbSFE,
                     &resultArb,
                     &abserrArb);
                // ***FINISHME*** -- Check the absolute error in <abserrVfp>
                //                   In Ball Arithmetic, compare it against the Radius

                arb_clear (&abserrArb);
                arb_clear (&aplArbRht);

                // Check for error
                if (params.exCode NE EXCEPTION_SUCCESS)
                {
                    // Copy the exception code
                    exCode = params.exCode;

                    // Mark as signalling an exception
                    bException = TRUE;

                    goto EXCEPTION_START;
                } // End IF

                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), WFCN))
    {
        exCode = MyGetExceptionCode ();

        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L": %2d (%S#%d)", exCode, FNLN);

        // Mark as signalling an exception
        bException = TRUE;
    } // End __try/__except

EXCEPTION_START:
    // If lclfuncXXX found an exception, ...
    if (bException)
    // Split cases based upon the ExceptionCode
    switch (exCode)
    {
        case EXCEPTION_LENGTH_ERROR:
            goto LENGTH_EXIT;

        case EXCEPTION_DOMAIN_ERROR:
            goto DOMAIN_EXIT;

        case EXCEPTION_WS_FULL:
            goto WSFULL_EXIT;

        case EXCEPTION_CTRL_BREAK:
            goto ERROR_EXIT;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkFloat    = resultFlt;
            lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;

            break;

        case ARRAY_VFP:
            // Save resultVfp as a scalar in a global
            lpYYRes = SaveGlbNumeric_EM (aplTypeRes, &resultVfp, lpYYFcnStrLft->tkToken.tkCharIndex);

            // Check for errors
            if (lpYYRes EQ NULL)
                goto WSFULL_EXIT;

            break;

        case ARRAY_ARB:
            // Save resultArb as a scalar in a global
            lpYYRes = SaveGlbNumeric_EM (aplTypeRes, &resultArb, lpYYFcnStrLft->tkToken.tkCharIndex);

            // Check for errors
            if (lpYYRes EQ NULL)
                goto WSFULL_EXIT;

            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // If it's valid, ...
    arb_clear (&resultArb);
    arb_clear (&abserrArb);
    arb_clear (&arbSFE   );
    Myf_clear (&resultVfp);
    Myf_clear (&abserrVfp);
    Myf_clear (&vfpSFE   );
NORMAL_EXIT:
    return lpYYRes;
} // End PrimOpDerivGSLCommon_EM_YY


//***************************************************************************
//  $PrimOpDerivGSLCentral_EM_YY
//
//  Numerical Differentiation Central Differencing
//    using the GSL approximations
//***************************************************************************

LPPL_YYSTYPE PrimOpDerivGSLCentral_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic derived function)
                                    //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                    //     then this value is the original right arg
     LPPL_YYSTYPE lpYYFcnStrLft,    // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     UBOOL        bPrototyping,     // TRUE iff prototyping
     APLUINT      iOrder,           // Order ***UNUSED***
     APLINT       iSFE)             // Sampling frequency exponent (0 = default)

{
    return
      PrimOpDerivGSLCommon_EM_YY
        (lptkLftArg,       // Ptr to left arg token (may be NULL if monadic derived function)
                           //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                           //     then this value is the original right arg
         lpYYFcnStrLft,    // Ptr to function strand
         lptkRhtArg,       // Ptr to right arg token
         bPrototyping,     // TRUE iff prototyping
         iSFE,             // Sampling frequency exponent (0 = default)
         iOrder,           // Order ***UNUSED***
        &gsl_deriv_central,
        &mpfr_deriv_central2,
        &arb_deriv_central2);
} // End PrimOpDerivGSLCentral_EM_YY


//***************************************************************************
//  $PrimOpDerivGSLBackward_EM_YY
//
//  Numerical Differentiation Forward Differencing
//    using the GSL approximations
//***************************************************************************

LPPL_YYSTYPE PrimOpDerivGSLBackward_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic derived function)
                                    //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                    //     then this value is the original right arg
     LPPL_YYSTYPE lpYYFcnStrLft,    // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     UBOOL        bPrototyping,     // TRUE iff prototyping
     APLUINT      iOrder,           // Order ***UNUSED***
     APLINT       iSFE)             // Sampling frequency exponent (0 = default)

{
    return
      PrimOpDerivGSLCommon_EM_YY
        (lptkLftArg,       // Ptr to left arg token (may be NULL if monadic derived function)
                           //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                           //     then this value is the original right arg
         lpYYFcnStrLft,    // Ptr to function strand
         lptkRhtArg,       // Ptr to right arg token
         bPrototyping,     // TRUE iff prototyping
         iSFE,             // Sampling frequency exponent (0 = default)
         iOrder,           // Order ***UNUSED***
        &gsl_deriv_backward,
        &mpfr_deriv_backward,
        &arb_deriv_backward);
} // End PrimOpDerivGSLBackward_EM_YY


//***************************************************************************
//  $PrimOpDerivGSLForward_EM_YY
//
//  Numerical Differentiation Forward Differencing
//    using the GSL approximations
//***************************************************************************

LPPL_YYSTYPE PrimOpDerivGSLForward_EM_YY
    (LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic derived function)
                                    //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                                    //     then this value is the original right arg
     LPPL_YYSTYPE lpYYFcnStrLft,    // Ptr to function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     UBOOL        bPrototyping,     // TRUE iff prototyping
     APLUINT      iOrder,           // Order ***UNUSED***
     APLINT       iSFE)             // Sampling frequency exponent (0 = default)

{
    return
      PrimOpDerivGSLCommon_EM_YY
        (lptkLftArg,       // Ptr to left arg token (may be NULL if monadic derived function)
                           //   If this is an identity function call (lpPrimFlagsLft NE NULL),
                           //     then this value is the original right arg
         lpYYFcnStrLft,    // Ptr to function strand
         lptkRhtArg,       // Ptr to right arg token
         bPrototyping,     // TRUE iff prototyping
         iSFE,             // Sampling frequency exponent (0 = default)
         iOrder,           // Order ***UNUSED***
        &gsl_deriv_forward,
        &mpfr_deriv_forward,
        &arb_deriv_forward);
} // End PrimOpDerivGSLForward_EM_YY


//***************************************************************************
//  End of File: po_deriv.c
//***************************************************************************
