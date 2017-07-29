//***************************************************************************
//  NARS2000 -- Primitive Operator -- Variant
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
#include <gsl/gsl_eigen.h>


//***************************************************************************
//  $PrimOpVariant_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Variant ("variant" and "variant")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpVariant_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpVariant_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_VARIANT);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return
          PrimOpMonVariant_EM_YY (lpYYFcnStrOpr,        // Ptr to operator function strand
                                  lptkRhtArg);          // Ptr to right arg token
    else
        return
          PrimOpDydVariant_EM_YY (lptkLftArg,           // Ptr to left arg token
                                  lpYYFcnStrOpr,        // Ptr to operator function strand
                                  lptkRhtArg);          // Ptr to right arg token
} // End PrimOpVariant_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpVariant_EM_YY
//
//  Generate a prototype for the derived functions from
//    dyadic operator Variant ("variant" and "variant")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpVariant_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoOpVariant_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    if (lptkAxis EQ NULL)
        // Check for axis operator
        lptkAxis = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive
    //   to the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return
          PrimOpMonVariantCommon_EM_YY (lpYYFcnStrOpr,          // Ptr to operator function strand
                                        lptkRhtArg,             // Ptr to right arg token
                                        TRUE);                  // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return
          PrimOpDydVariantCommon_EM_YY (lptkLftArg,             // Ptr to left arg token
                                        lpYYFcnStrOpr,          // Ptr to operator function strand
                                        lptkRhtArg,             // Ptr to right arg token
                                        TRUE);                  // TRUE iff prototyping
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimProtoOpVariant_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimIdentOpVariant_EM_YY
//
//  Generate an identity element for the primitive operator dyadic Variant
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimIdentOpVariant_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimIdentOpVariant_EM_YY
    (LPTOKEN      lptkRhtOrig,      // Ptr to original right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,    // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)      // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYFcnStrLft,     // Ptr to left operand function strand
                 lpYYFcnStrRht;     // Ptr to right ...
    LPPRIMFLAGS  lpPrimFlagsLft;    // Ptr to left operand primitive flags
    LPTOKEN      lptkAxisLft;       // Ptr to axis operator token (if any)

    // The right arg is the prototype item from
    //   the original empty arg.

    Assert (lptkRhtOrig   NE NULL);
    Assert (lpYYFcnStrOpr NE NULL);
    Assert (lptkRhtArg    NE NULL);

    //***************************************************************
    // This operator is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // The (left) identity function for dyadic Variant
    //   (L f {variant} R) ("scalar op") is
    //   ({primops} f) R.

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, lptkAxisOpr);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrRht);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Ensure the right operand is a variable
    if (IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Check for left operand axis operator
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // Get the appropriate primitive flags ptr
    lpPrimFlagsLft = GetPrimFlagsPtr (&lpYYFcnStrLft->tkToken);

    // Check for error
    if (lpPrimFlagsLft EQ NULL || lpPrimFlagsLft->lpPrimOps EQ NULL)
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Execute the left operand identity function on the right arg
    return
      (*lpPrimFlagsLft->lpPrimOps)
                        (lptkRhtOrig,           // Ptr to original right arg token
                         lpYYFcnStrLft,         // Ptr to function strand
                         lptkRhtArg,            // Ptr to right arg token
                         lptkAxisLft);          // Ptr to axis token (may be NULL)
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End PrimIdentOpVariant_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonVariant_EM_YY
//
//  Primitive operator for monadic derived function from Variant ("variant")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonVariant_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return
      PrimOpMonVariantCommon_EM_YY (lpYYFcnStrOpr,      // Ptr to operator function strand
                                    lptkRhtArg,         // Ptr to right arg token
                                    FALSE);             // TRUE iff prototyping
} // End PrimOpMonVariant_EM_YY


//***************************************************************************
//  $PrimOpMonVariantCommon_EM_YY
//
//  Primitive operator for monadic derived function from Variant ("variant")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonVariantCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, CheckAxisOper (lpYYFcnStrOpr));
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrRht);

    return
      PrimOpVariantCommon_EM_YY (NULL,                  // Ptr to left arg token (may be NULL if monadic derived function)
                                 lpYYFcnStrLft,         // Ptr to left operand function strand
                                 lpYYFcnStrOpr,         // Ptr to operator function strand
                                 lpYYFcnStrRht,         // Ptr to right operand function strand
                                 lptkRhtArg,            // Ptr to right arg token
                                 bPrototyping);         // TRUE iff protoyping
} // End PrimOpMonVariantCommon_EM_YY


//***************************************************************************
//  $PrimOpVariantCommon_EM_YY
//
//  Primitive operator for monadic & dyadic derived function
//    from Variant ("variant")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpVariantCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpVariantCommon_EM_YY
    (LPTOKEN      lptkLftArg,               // Ptr to left arg token (may be NULL if monadic derived function)
     LPPL_YYSTYPE lpYYFcnStrLft,            // Ptr to left operand function strand
     LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPPL_YYSTYPE lpYYFcnStrRht,            // Ptr to right operand function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token
     UBOOL        bPrototyping)             // TRUE iff protoyping

{
    LPTOKEN             lptkAxisOpr;                // Ptr to axis token (may be NULL)
    LPPL_YYSTYPE        lpYYRes = NULL;             // Ptr to the result
////                    lpYYRes2;                   // Ptr to secondary result
    LPTOKEN             lptkAxisLft;                // Ptr to axis token on the left operand
    APLSTYPE            aplTypeRhtOpr,              // Right operand storage type
                        aplTypeRht;                 // right arg     ...
    APLNELM             aplNELMRhtOpr,              // Right operand NELM
                        aplNELMRht;                 // Right arg     ...
    APLRANK             aplRankRhtOpr,              // Right operand rank
                        aplRankRht;                 // Right arg     ...
    APLDIM              aplColsRht,                 // Right arg # cols
                        aplRowsRht;                 // ...         rows
    APLINT              aplIntegerRhtOpr;           // Right operand integer value
    APLCHAR             aplCharRhtOpr;              // Right operand character value
    APLFLOAT            aplFloatRhtOpr;             // Right operand float value
    UBOOL               bRet = TRUE,                // TRUE iff the result is valid
                        bQuadCTFound = FALSE,       // TRUE iff []CT value found
                        bQuadIOFound = FALSE,       // ...      []IO ...
                        bQuadDQFound = FALSE,       // ...      []DQ ...
                        bQuadDTFound = FALSE;       // ...      []DT ...
    APLFLOAT            fQuadCT;                    // []CT
    APLCHAR             cQuadDQ,                    // []DQ
                        cQuadDT;                    // []DT
    APLBOOL             bQuadIO;                    // []IO
    APLINT              uQuadPPV;                   // []PP for VFPs
    TOKEN               tkFcn = {0},                // Function token
                        tkRht = {0};                // Right arg token
    HGLOBAL             hGlbRhtOpr,                 // Right operand global memory handle
                        hGlbRht = NULL,             // Right arg     ...
                        hGlbRes = NULL,             // Result        ...
                        hGlbEval = NULL,            // Eigenvalues   ...
                        hGlbEvec = NULL;            // Eigenvectors  ...
    LPAPLFLOAT          lpMemEval = NULL,
                        lpMemEvec = NULL;
    LPVARARRAY_HEADER   lpMemHdrRht = NULL,         // Ptr to right arg global memory header
                        lpMemHdrRes = NULL,         // ...    result    ...
                        lpMemHdrEval = NULL,        // ...    Evalues   ...
                        lpMemHdrEvec = NULL;        // ...    Evectors  ...
    LPVOID              lpMemRhtOpr,                // Ptr to right operand memory
                        lpMemRht = NULL,            // Ptr to right arg global memory data
                        lpMemRes = NULL;            // Ptr to result    ...
    HGLOBAL             hGlbMFO;                    // Magic function/operator global memory handle
    LPPERTABDATA        lpMemPTD;                   // Ptr to PerTabData global memory
    ENUM_HCMUL          eHCMul;                     // Hypercomplex Arithmetic Multiplication choice
    int                 i,                          // Loop counter
                        ErrCode;                    // Error code
    APLDIM              NxN[2],                     // The array dimensions
                        iTwo = 2;                   // Constant 2
    APLLONGEST          aplLongestRht;              // Right arg if immediate
    gsl_matrix         *lpGslMatrixA = NULL;        // GSL Temp
    gsl_vector_complex *lpGslCVectorEval = NULL;    // Eigenvalues
    gsl_matrix_complex *lpGslCMatrixEvec = NULL;    // Eigenvectors
    gsl_eigen_nonsymmv_workspace *lpGslEigenWs = NULL;          // Ptr to the GSL workspace

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Check for axis operator on the left operand
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Ensure the right operand is a variable
    if (IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    //***************************************************************
    // Get the attributes (Type, NELM, and Rank) of the right operand
    //    and right arg
    //***************************************************************
    AttrsOfToken (&lpYYFcnStrRht->tkToken, &aplTypeRhtOpr, &aplNELMRhtOpr, &aplRankRhtOpr, NULL);
    AttrsOfToken (lptkRhtArg             , &aplTypeRht   , &aplNELMRht   , &aplRankRht   , &aplColsRht);

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If the right arg is a global, ...
    if (lpMemHdrRht NE NULL)
        // Point to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        // Point to the data
        lpMemRht = &aplLongestRht;

    // If the left operand is not an immediate function, or
    //    the right operand is nested or hetero, ...
    if (!IsTknImmed (&lpYYFcnStrLft->tkToken)
      || IsPtrArray (aplTypeRhtOpr))
        return
          PrimOpVariantKeyword_EM_YY (lptkLftArg,       // Ptr to left arg token (may be NULL if monadic derived function)
                                      lpYYFcnStrLft,    // Ptr to left operand function strand
                                      lpYYFcnStrRht,    // Ptr to right operand function strand
                                      lptkRhtArg,       // Ptr to right arg token
                                      aplTypeRhtOpr,    // Right operand storage type
                                      aplNELMRhtOpr,    // ...           NELM
                                      aplRankRhtOpr,    // ...           rank
                                      lpMemPTD);        // Ptr to PerTabData global memory
    // ***TESTME*** -- Handle axis operator on a PSDF
    // Split cases based upon the immediate function
    switch (lpYYFcnStrLft->tkToken.tkData.tkChar)
    {
        // []IO first, []CT second (if present)
        case UTF16_IOTA:                    // Monadic or dyadic
        case UTF16_IOTAUNDERBAR:            // ...
            // Validate the right operand as
            //   a simple numeric scalar or one- or two-element vector
            if (IsMultiRank (aplRankRhtOpr))
                goto RIGHT_OPERAND_RANK_EXIT;
            if (aplNELMRhtOpr NE 1
             && aplNELMRhtOpr NE 2)
                goto RIGHT_OPERAND_LENGTH_EXIT;
            if (!IsNumeric (aplTypeRhtOpr))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Get the first value as an integer from the token
            aplIntegerRhtOpr =
              GetNextIntegerToken (&lpYYFcnStrRht->tkToken, // Ptr to arg token
                                    0,                      // Index
                                    aplTypeRhtOpr,          // Arg storage type
                                   &bRet);                  // Ptr to TRUE iff the result is valid
            // Check for error
            if (!bRet)
                goto RIGHT_OPERAND_DOMAIN_EXIT;
            // Validate the value
            if (!ValidateIntegerTest (&aplIntegerRhtOpr,
                                       DEF_MIN_QUADIO,      // Minimum value
                                       DEF_MAX_QUADIO,      // Maximum ...
                                       bRangeLimit.IO))     // TRUE iff range limiting
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // If there's a second element, ...
            if (aplNELMRhtOpr EQ 2)
            {
                // Get the second value as a float from the token
                aplFloatRhtOpr =
                  GetNextFloatToken (&lpYYFcnStrRht->tkToken,   // Ptr to arg token
                                      1,                        // Index
                                      aplTypeRhtOpr,            // Arg storage type
                                     &bRet);                    // Ptr to TRUE iff the result is valid
                // Check for error
                if (!bRet)
                    goto RIGHT_OPERAND_DOMAIN_EXIT;
                // Validate the value
                if (!ValidateFloatTest (&aplFloatRhtOpr,    //
                                         DEF_MIN_QUADCT,        // Minimum value
                                         DEF_MAX_QUADCT,        // Maximum ...
                                         bRangeLimit.CT))       // TRUE iff range limiting
                    goto RIGHT_OPERAND_DOMAIN_EXIT;
            } // End IF

            // Save the current values for []IO and []CT
            bQuadIO = GetQuadIO ();
            fQuadCT = GetQuadCT ();

            // Put the new value(s) into effect
            SetQuadIO ((APLBOOL) aplIntegerRhtOpr);
            if (aplNELMRhtOpr EQ 2)
                SetQuadCT (aplFloatRhtOpr);

            // ***TESTME*** -- At some point we'll need to worry about multiple threads in the same workspace.

            // Execute the function
            lpYYRes =
              ExecFunc_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
                              lpYYFcnStrLft,        // Ptr to function strand
                              lptkRhtArg);          // Ptr to right arg token
            // Restore the original values
            SetQuadIO (bQuadIO);
            SetQuadCT (fQuadCT);

            break;

        // []IO
        case UTF16_SQUAD:                   // Dyadic only  (Indexing)
        case UTF16_CIRCLESLOPE:             // ...          (Dyadic transpose)
        case UTF16_RIGHTSHOE:               // ...          (Pick)
        case UTF16_PI:                      // ...          (Number theoretic)
            // Ensure there's a left arg
            if (lptkLftArg EQ NULL)
                goto LEFT_VALENCE_EXIT;

            // Fall through to common code

        case UTF16_DELSTILE:                // Monadic:  Grade down, Dyadic: Grade down
        case UTF16_DELTASTILE:              // ...             up    ...           up
        case UTF16_DOUBLESHRIEK:            // ...       Combinatorial
            // Validate the right operand as
            //   a simple numeric scalar or one-element vector
            if (IsMultiRank (aplRankRhtOpr))
                goto RIGHT_OPERAND_RANK_EXIT;
            if (aplNELMRhtOpr NE 1)
                goto RIGHT_OPERAND_LENGTH_EXIT;
            if (!IsNumeric (aplTypeRhtOpr))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Get the first value as an integer from the token
            aplIntegerRhtOpr =
              GetNextIntegerToken (&lpYYFcnStrRht->tkToken, // Ptr to arg token
                                    0,                      // Index
                                    aplTypeRhtOpr,          // Arg storage type
                                   &bRet);                  // Ptr to TRUE iff the result is valid
            // Check for error
            if (!bRet)
                goto RIGHT_OPERAND_DOMAIN_EXIT;
            // Validate the value
            if (!ValidateIntegerTest (&aplIntegerRhtOpr,
                                       DEF_MIN_QUADIO,      // Minimum value
                                       DEF_MAX_QUADIO,      // Maximum ...
                                       bRangeLimit.IO))     // TRUE iff range limiting
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Save the current values for []IO
            bQuadIO = GetQuadIO ();

            // Put the new value into effect
            SetQuadIO ((APLBOOL) aplIntegerRhtOpr);

            // ***TESTME*** -- At some point we'll need to worry about multiple threads in the same workspace.

            // Execute the function
            lpYYRes =
              ExecFunc_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
                              lpYYFcnStrLft,        // Ptr to function strand
                              lptkRhtArg);          // Ptr to right arg token
            // Restore the original values
            SetQuadIO (bQuadIO);

            break;

        // []PP
        case UTF16_DOWNTACKJOT:             // Monadic or dyadic
            // Validate the right operand as
            //   a simple numeric scalar or one-element vector
            if (IsMultiRank (aplRankRhtOpr))
                goto RIGHT_OPERAND_RANK_EXIT;
            if (aplNELMRhtOpr NE 1)
                goto RIGHT_OPERAND_LENGTH_EXIT;
            if (!IsNumeric (aplTypeRhtOpr))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Get the first value as an integer from the token
            aplIntegerRhtOpr =
              GetNextIntegerToken (&lpYYFcnStrRht->tkToken, // Ptr to arg token
                                    0,                      // Index
                                    aplTypeRhtOpr,          // Arg storage type
                                   &bRet);                  // Ptr to TRUE iff the result is valid
            // Check for error
            if (!bRet)
                goto RIGHT_OPERAND_DOMAIN_EXIT;
            // Validate the value
            if (!ValidateIntegerTest (&aplIntegerRhtOpr,
                                       DEF_MIN_QUADPP,      // Minimum value
                                       DEF_MAX_QUADPP_VFP,  // Maximum ...   for VFPs
                                       bRangeLimit.PP))     // TRUE iff range limiting
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Save the current values for []PP for VFPs
            uQuadPPV = GetQuadPPV ();

            // Put the new value into effect
            SetQuadPPV (aplIntegerRhtOpr);

            // ***TESTME*** -- At some point we'll need to worry about multiple threads in the same workspace.

            // Execute the function
            lpYYRes =
              ExecFunc_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
                              lpYYFcnStrLft,        // Ptr to function strand
                              lptkRhtArg);          // Ptr to right arg token
            // Restore the original value for VFPs
            SetQuadPPV (uQuadPPV);

            break;

        // []DQ:  Division Quotient
        case UTF16_COLONBAR:                // Dyadic only (Division)
        case UTF16_DOWNCARET:               // ...         (GCD)
        case UTF16_UPCARET:                 // ...         (LCM)
        case UTF16_CIRCLE:                  // ...         (Trig)
        case UTF16_DOWNTACK:                // ...         (Encode)
            // Ensure there's a left arg
            if (lptkLftArg EQ NULL)
                goto LEFT_VALENCE_EXIT;

            // Validate the right operand as
            //   a simple char scalar or one-element vector
            if (IsMultiRank (aplRankRhtOpr))
                goto RIGHT_OPERAND_RANK_EXIT;
            if (aplNELMRhtOpr NE 1)
                goto RIGHT_OPERAND_LENGTH_EXIT;
            if (!IsSimpleChar (aplTypeRhtOpr))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Validate the value
            if (!ValidateCharScalar_EM (NULL,                   // Ptr to name token
                                       &lpYYFcnStrRht->tkToken, // Ptr to value token
                                        DEF_QUADDQ_CWS[0],      // Default value
                                        DEF_QUADDQ_ALLOW,       // Ptr to vector of allowed values
                                       &aplCharRhtOpr))         // Ptr to return value
                goto RIGHT_OPERAND_DOMAIN_EXIT;
            // Save the current value for []DQ
            cQuadDQ = GetQuadDQ ();

            // Put the new value into effect
            SetQuadDQ (aplCharRhtOpr);

            // ***TESTME*** -- At some point we'll need to worry about multiple threads in the same workspace.

            // Execute the function
            lpYYRes =
              ExecFunc_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
                              lpYYFcnStrLft,        // Ptr to function strand
                              lptkRhtArg);          // Ptr to right arg token
            // Restore the original value
            SetQuadDQ (cQuadDQ);

            break;

        // []CT & []DQ:  Division Quotient:  'l' (left quotient) or 'r' (right quotient)
        case UTF16_STILE:                   // Dyadic only  (Residue)
        case UTF16_STILE2:                  // ...          (Residue)
            // Ensure there's a left arg
            if (lptkLftArg EQ NULL)
                goto LEFT_VALENCE_EXIT;

            // Validate the right operand as
            //   a simple numeric scalar or one-element vector, or
            //   a simple character scalar or one-element vector, or
            //   a hetero two-element vector
            switch (aplRankRhtOpr)
            {
                case 0:
                    // If it's not simple global numeric and not simple char, ...
                    if (!IsNumeric (aplTypeRhtOpr)
                     && !IsSimpleChar (aplTypeRhtOpr))
                        goto RIGHT_OPERAND_DOMAIN_EXIT;
                    break;

                case 1:
                    if (((aplNELMRhtOpr EQ 1)
                      && !IsNumeric (aplTypeRhtOpr)
                      && !IsSimpleChar (aplTypeRhtOpr))
                     || ((aplNELMRhtOpr EQ 2)
                      && !IsSimpleHet (aplTypeRhtOpr)))
                        goto RIGHT_OPERAND_LENGTH_EXIT;
                    break;

                default:
                    goto RIGHT_OPERAND_RANK_EXIT;
            } // End SWITCH

            // Split cases based upon the right operand storage type
            switch (aplTypeRhtOpr)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_APA:
                case ARRAY_FLOAT:
                case ARRAY_RAT:
                case ARRAY_VFP:
                    // Get the first value as a float from the token
                    aplFloatRhtOpr =
                      GetNextFloatToken (&lpYYFcnStrRht->tkToken,   // Ptr to arg token
                                          0,                        // Index
                                          aplTypeRhtOpr,            // Arg storage type
                                         &bRet);                    // Ptr to TRUE iff the result is valid
                    // Check for error
                    if (!bRet)
                        goto RIGHT_OPERAND_DOMAIN_EXIT;
                    // Validate the value
                    if (!ValidateFloatTest (&aplFloatRhtOpr,
                                             DEF_MIN_QUADCT,        // Minimum value
                                             DEF_MAX_QUADCT,        // Maximum ...
                                             bRangeLimit.CT))       // TRUE iff range limiting
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    // Set the flag
                    bQuadCTFound = TRUE;    // In aplFloatRhtOpr

                    break;

                case ARRAY_CHAR:
                    // Get the first value from the right operand (Division Quotient)
                    GetNextValueToken (&lpYYFcnStrRht->tkToken, // Ptr to the token
                                        0,                      // Index to use
                                        NULL,                   // Ptr to the integer (or Boolean) (may be NULL)
                                        NULL,                   // ...        float (may be NULL)
                                       &aplCharRhtOpr,          // ...        char (may be NULL)
                                        NULL,                   // ...        longest (may be NULL)
                                        NULL,                   // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                                        NULL,                   // ...        immediate type (see IMM_TYPES) (may be NULL)
                                        NULL);                  // ...        array type:  ARRAY_TYPES (may be NULL)
                    // Set the flag
                    bQuadDQFound = TRUE;    // In aplCharRhtOpr

                    break;

                case ARRAY_HETERO:
                    // Get right operand's global ptrs
                    GetGlbPtrs_LOCK (&lpYYFcnStrRht->tkToken, &hGlbRhtOpr, (LPVOID *) &lpMemRhtOpr);

                    // Check the first entry
                    if (!PrimOpVariantCheckHetero (lpMemRhtOpr,         // Ptr to right operand global memory data
                                                   0,                   // Index into lpMemRhtOpr to use
                                                  &bQuadCTFound,        // Ptr to bQuadCTFound
                                                  &bQuadDQFound,        // ...    ...  DQ ...
                                                   NULL,
                                                  &aplFloatRhtOpr,      // ...    aplFloatRhtOpr
                                                  &aplCharRhtOpr))      // ...    ...Char ...
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    // Check the second entry
                    if (!PrimOpVariantCheckHetero (lpMemRhtOpr,         // Ptr to right operand global memory data
                                                   1,                   // Index into lpMemRhtOpr to use
                                                  &bQuadCTFound,        // Ptr to bQuadCTFound
                                                  &bQuadDQFound,        // ...    ...  DQ ...
                                                   NULL,
                                                  &aplFloatRhtOpr,      // ...    aplFloatRhtOpr
                                                  &aplCharRhtOpr))      // ...    ...Char ...
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    // Test the value
                    if (bQuadCTFound
                     && !ValidateFloatTest (&aplFloatRhtOpr,            // Ptr to the integer to test
                                             DEF_MIN_QUADCT,            // Low range value (inclusive)
                                             DEF_MAX_QUADCT,            // High ...
                                             bRangeLimit.CT))           // TRUE iff we're range limiting
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    // Test the value
                    if (bQuadDQFound
                     && (strchrW (DEF_QUADDQ_ALLOW, aplCharRhtOpr) EQ NULL))
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    break;

                case ARRAY_NESTED:
                defstop
                    break;
            } // End SWITCH

            // Save the current values for []CT & []DQ
            fQuadCT = GetQuadCT ();
            cQuadDQ = GetQuadDQ ();

            // Put the new value(s) into effect
            if (bQuadCTFound)
                SetQuadCT (aplFloatRhtOpr);
            if (bQuadDQFound)
                SetQuadDQ (aplCharRhtOpr);

            // ***TESTME*** -- At some point we'll need to worry about multiple threads in the same workspace.

            // Execute the function
            lpYYRes =
              ExecFunc_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
                              lpYYFcnStrLft,        // Ptr to function strand
                              lptkRhtArg);          // Ptr to right arg token
            // Restore the original values
            SetQuadDQ (cQuadDQ);
            SetQuadCT (fQuadCT);

            break;

        // []CT:  Comparison Tolerance
        case UTF16_LEFTCARET:               // Dyadic only  (Less than)
        case UTF16_LEFTCARETUNDERBAR:       // ...          (Less than or equal)
        case UTF16_LEFTCARETUNDERBAR2:      // ...          (Less than or equal)
        case UTF16_EQUAL:                   // ...          (Equal)
        case UTF16_NOTEQUAL:                // ...          (Not equal)
        case UTF16_RIGHTCARETUNDERBAR:      // ...          (Greater than or equal)
        case UTF16_RIGHTCARETUNDERBAR2:     // ...          (Greater than or equal)
        case UTF16_RIGHTCARET:              // ...          (Greater than)
        case UTF16_EPSILON:                 // ...          (Membership)
        case UTF16_EQUALUNDERBAR:           // ...          (Match)
        case UTF16_NOTEQUALUNDERBAR:        // ...          (Mismatch)
        case UTF16_UPSHOE:                  // ...          (Set intersection)
        case UTF16_LEFTSHOEUNDERBAR:        // ...          (Subset)
        case UTF16_RIGHTSHOEUNDERBAR:       // ...          (Superset)
        case UTF16_SECTION:                 // ...          (Set asymmetric difference)
        case UTF16_TILDE:                   // ...          (Without)
        case UTF16_TILDE2:                  // ...          (Without)
        case UTF16_EPSILONUNDERBAR:         // ...          (Find)
            // Ensure there's a left arg
            if (lptkLftArg EQ NULL)
                goto LEFT_VALENCE_EXIT;

            // Fall through to common code

        case UTF16_DOWNSHOE:                // Monadic:  Unique,   Dyadic:  Set union
        case UTF16_DOWNSTILE:               // ...       Minimum   ...      Floor
        case UTF16_UPSTILE:                 // ...       Maximum   ...      Ceiling
            // Validate the right operand as
            //   a simple numeric scalar or one-element vector
            if (IsMultiRank (aplRankRhtOpr))
                goto RIGHT_OPERAND_RANK_EXIT;
            if (aplNELMRhtOpr NE 1)
                goto RIGHT_OPERAND_LENGTH_EXIT;
            if (!IsNumeric (aplTypeRhtOpr))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Get the first value as a float from the token
            aplFloatRhtOpr =
              GetNextFloatToken (&lpYYFcnStrRht->tkToken,   // Ptr to arg token
                                  0,                        // Index
                                  aplTypeRhtOpr,            // Arg storage type
                                 &bRet);                    // Ptr to TRUE iff the result is valid
            // Check for error
            if (!bRet)
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Validate the value
            if (!ValidateFloatTest (&aplFloatRhtOpr,        // Ptr to float value
                                     DEF_MIN_QUADCT,        // Minimum value
                                     DEF_MAX_QUADCT,        // Maximum ...
                                     bRangeLimit.CT))       // TRUE iff range limiting
                goto RIGHT_OPERAND_DOMAIN_EXIT;
            // Save the current value for []CT
            fQuadCT = GetQuadCT ();

            // Put the new value into effect
            SetQuadCT (aplFloatRhtOpr);

            // ***TESTME*** -- At some point we'll need to worry about multiple threads in the same workspace.

            // Execute the function
            lpYYRes =
              ExecFunc_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
                              lpYYFcnStrLft,        // Ptr to function strand
                              lptkRhtArg);          // Ptr to right arg token
            // Restore the original value
            SetQuadCT (fQuadCT);

            break;

        // []IO & []DT:  Distribution Type:  'r' (rectangular dist), 'g' (Gaussian dist), or 'p' (Poisson dist)
        case UTF16_QUERY:                   // Monadic or dyadic
            // Validate the right operand as
            //   a simple numeric scalar or one-element vector, or
            //   a simple character scalar or one-element vector, or
            //   a hetero two-element vector
            switch (aplRankRhtOpr)
            {
                case 0:
                    // If it's not simple global numeric and not simple char, ...
                    if (!IsNumeric (aplTypeRhtOpr)
                     && !IsSimpleChar (aplTypeRhtOpr))
                        goto RIGHT_OPERAND_DOMAIN_EXIT;
                    break;

                case 1:
                    if (((aplNELMRhtOpr EQ 1)
                      && !IsNumeric (aplTypeRhtOpr)
                      && !IsSimpleChar (aplTypeRhtOpr))
                     || ((aplNELMRhtOpr EQ 2)
                      && !IsSimpleHet (aplTypeRhtOpr)))
                        goto RIGHT_OPERAND_LENGTH_EXIT;
                    break;

                default:
                    goto RIGHT_OPERAND_RANK_EXIT;
            } // End SWITCH

            // Split cases based upon the right operand storage type
            switch (aplTypeRhtOpr)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_APA:
                case ARRAY_FLOAT:
                case ARRAY_RAT:
                case ARRAY_VFP:
                    // Get the first value as an integer from the token
                    aplIntegerRhtOpr =
                      GetNextIntegerToken (&lpYYFcnStrRht->tkToken, // Ptr to arg token
                                            0,                      // Index
                                            aplTypeRhtOpr,          // Arg storage type
                                           &bRet);                  // Ptr to TRUE iff the result is valid
                    // Check for error
                    if (!bRet)
                        goto RIGHT_OPERAND_DOMAIN_EXIT;
                    // Validate the value
                    if (!ValidateIntegerTest (&aplIntegerRhtOpr,
                                               DEF_MIN_QUADIO,      // Minimum value
                                               DEF_MAX_QUADIO,      // Maximum ...
                                               bRangeLimit.IO))     // TRUE iff range limiting
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    // Set the flag
                    bQuadIOFound = TRUE;    // In aplIntegerRhtOpr

                    break;

                case ARRAY_CHAR:
                    // Get the first value from the right operand (Distribution Type)
                    GetNextValueToken (&lpYYFcnStrRht->tkToken, // Ptr to the token
                                        0,                      // Index to use
                                        NULL,                   // Ptr to the integer (or Boolean) (may be NULL)
                                        NULL,                   // ...        float (may be NULL)
                                       &aplCharRhtOpr,          // ...        char (may be NULL)
                                        NULL,                   // ...        longest (may be NULL)
                                        NULL,                   // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                                        NULL,                   // ...        immediate type (see IMM_TYPES) (may be NULL)
                                        NULL);                  // ...        array type:  ARRAY_TYPES (may be NULL)
                    // Set the flag
                    bQuadDTFound = TRUE;    // In aplCharRhtOpr

                    break;

                case ARRAY_HETERO:
                    // Get right operand's global ptrs
                    GetGlbPtrs_LOCK (&lpYYFcnStrRht->tkToken, &hGlbRhtOpr, (LPVOID *) &lpMemRhtOpr);

                    // Check the first entry
                    if (!PrimOpVariantCheckHetero (lpMemRhtOpr,         // Ptr to right operand global memory data
                                                   0,                   // Index into lpMemRhtOpr to use
                                                  &bQuadIOFound,        // Ptr to bQuadIOFound
                                                  &bQuadDTFound,        // ...    ...  DT ...
                                                  &aplIntegerRhtOpr,    // ...    aplIntegerRhtOpr
                                                   NULL,
                                                  &aplCharRhtOpr))      // ...    ...Char ...
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    // Check the second entry
                    if (!PrimOpVariantCheckHetero (lpMemRhtOpr,         // Ptr to right operand global memory data
                                                   1,                   // Index into lpMemRhtOpr to use
                                                  &bQuadIOFound,        // Ptr to bQuadIOFound
                                                  &bQuadDTFound,        // ...    ...  DT ...
                                                  &aplIntegerRhtOpr,    // ...    aplIntegerRhtOpr
                                                   NULL,
                                                  &aplCharRhtOpr))      // ...    ...Char ...
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    // Test the value
                    if (bQuadIOFound
                     && !ValidateIntegerTest (&aplIntegerRhtOpr,        // Ptr to the integer to test
                                               DEF_MIN_QUADIO,          // Low range value (inclusive)
                                               DEF_MAX_QUADIO,          // High ...
                                               bRangeLimit.IO))         // TRUE iff we're range limiting
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    // Test the value
                    if (bQuadDTFound
                     && (strchrW (DEF_QUADDT_ALLOW, aplCharRhtOpr) EQ NULL))
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    break;

                case ARRAY_NESTED:
                defstop
                    break;
            } // End SWITCH

            // Save the current values for []IO & []DT
            bQuadIO = GetQuadIO ();
            cQuadDT = GetQuadDT ();

            // Put the new value(s) into effect
            if (bQuadIOFound)
                SetQuadIO ((APLBOOL) aplIntegerRhtOpr);
            if (bQuadDTFound)
                SetQuadDT (aplCharRhtOpr);

            // ***TESTME*** -- At some point we'll need to worry about multiple threads in the same workspace.

            // Execute the function
            lpYYRes =
              ExecFunc_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
                              lpYYFcnStrLft,        // Ptr to function strand
                              lptkRhtArg);          // Ptr to right arg token
            // Restore the original values
            SetQuadDT (cQuadDT);
            SetQuadCT (bQuadIO);

            break;

        // Pochhammer Symbol (Rising and Falling factorials):
        // See https://en.wikipedia.org/wiki/Falling_and_rising_factorials
        case UTF16_QUOTEDOT:                // Monadic only
            // Ensure there's no left arg
            if (lptkLftArg NE NULL)
                goto LEFT_VALENCE_EXIT;

            // Validate the right operand as
            //   a simple or global numeric scalar or one- or two-element vector
            if (IsMultiRank (aplRankRhtOpr))
                goto RIGHT_OPERAND_RANK_EXIT;
            if (aplNELMRhtOpr NE 1
             && aplNELMRhtOpr NE 2)
                goto RIGHT_OPERAND_LENGTH_EXIT;
            if (!IsNumeric (aplTypeRhtOpr))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Compute the rising/falling factorial using the Shriek primitive

            // Get the magic function/operator global memory handle
            hGlbMFO = lpMemPTD->hGlbMFO[MFOE_DydVOFact];

            lpYYRes =
              ExecuteMagicFunction_EM_YY (lptkLftArg,               // Ptr to left arg token
                                         &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                          lpYYFcnStrOpr,            // Ptr to function strand
                                          lptkRhtArg,               // Ptr to right arg token
                                         &lpYYFcnStrRht->tkToken,   // Ptr to axis token (from right operand)
                                          hGlbMFO,                  // Magic function/operator global memory handle
                                          NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                          bPrototyping
                                        ? LINENUM_PRO
                                        : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
            break;

        // []FPC:  Execute
        case UTF16_UPTACKJOT:
            // Validate the right operand as
            //   a simple numeric scalar or one-element vector
            if (IsMultiRank (aplRankRhtOpr))
                goto RIGHT_OPERAND_RANK_EXIT;
            if (aplNELMRhtOpr NE 1)
                goto RIGHT_OPERAND_LENGTH_EXIT;
            if (!IsNumeric (aplTypeRhtOpr))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Ensure there's no left arg
            if (lptkLftArg NE NULL)
                goto LEFT_VALENCE_EXIT;

            // Check for axis operator
            lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

            // Set ptr to right operand,
            //   skipping over the operator and axis token (if present)
            lptkLftArg = GetDydRhtOper (lpYYFcnStrOpr, lptkAxisOpr).tkToken;

            // Get the magic function/operator global memory handle
            hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MonExecute];

            lpYYRes =
              ExecuteMagicFunction_EM_YY (lptkLftArg,               // Ptr to left arg token
                                         &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                          lpYYFcnStrOpr,            // Ptr to function strand
                                          lptkRhtArg,               // Ptr to right arg token
                                          lptkAxisOpr,              // Ptr to axis token
                                          hGlbMFO,                  // Magic function/operator global memory handle
                                          NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                          bPrototyping
                                        ? LINENUM_PRO
                                        : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
            break;

            // []HCAM:  Hypercomplex Arithmetic Multiplication choice
        case UTF16_TIMES:                   // Dyadic only
            // Ensure there's a left arg
            if (lptkLftArg EQ NULL)
                goto LEFT_VALENCE_EXIT;

            // Validate the right operand as
            //   a simple character scalar
            if (IsMultiRank (aplRankRhtOpr))
                goto RIGHT_OPERAND_RANK_EXIT;
            if (aplNELMRhtOpr NE 1)
                goto RIGHT_OPERAND_LENGTH_EXIT;
            if (!IsSimpleChar (aplTypeRhtOpr))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Get the first value from the right operand
            GetNextValueToken (&lpYYFcnStrRht->tkToken, // Ptr to the token
                                0,                      // Index to use
                                NULL,                   // Ptr to the integer (or Boolean) (may be NULL)
                                NULL,                   // ...        float (may be NULL)
                               &aplCharRhtOpr,          // ...        char (may be NULL)
                                NULL,                   // ...        longest (may be NULL)
                                NULL,                   // ...        LPSYMENTRY or HGLOBAL (may be NULL)
                                NULL,                   // ...        immediate type (see IMM_TYPES) (may be NULL)
                                NULL);                  // ...        array type:  ARRAY_TYPES (may be NULL)
            // Save the old value of eHCMul
            eHCMul = lpMemPTD->eHCMul;

            // Ensure the value is valid
            switch (tolowerW (aplCharRhtOpr))
            {
                case L'i':
                    lpMemPTD->eHCMul = ENUMHCM_INT;

                    break;

                case L'e':
                    lpMemPTD->eHCMul = ENUMHCM_EXT;

                    break;

                default:
                    goto RIGHT_OPERAND_DOMAIN_EXIT;
            } // End SWITCH

            // Multiply the operands as per the Hypercomplex Arithmetic Multiplication choice
            lpYYRes =
              ExecFunc_EM_YY (lptkLftArg,
                              lpYYFcnStrLft,
                              lptkRhtArg);
            // Restore the original value
            lpMemPTD->eHCMul = eHCMul;

            break;

        // Eigenvalues/Eigenvectors
        case UTF16_DOMINO:
            // Validate the right operand as
            //   a simple numeric scalar or one-element vector
            if (IsMultiRank (aplRankRhtOpr))
                goto RIGHT_OPERAND_RANK_EXIT;
            if (aplNELMRhtOpr NE 1)
                goto RIGHT_OPERAND_LENGTH_EXIT;

            // Get the first value as an integer from the token
            aplIntegerRhtOpr =
              GetNextIntegerToken (&lpYYFcnStrRht->tkToken, // Ptr to arg token
                                    0,                      // Index
                                    aplTypeRhtOpr,          // Arg storage type
                                   &bRet);                  // Ptr to TRUE iff the result is valid
            // Check for error
            if (!bRet)
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Validate the right operand as the number 1, 2, or 3
            if (!(1 <= aplIntegerRhtOpr
                &&     aplIntegerRhtOpr <= 3))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Validate the right arg as a square matrix
            if (!IsMatrix (aplRankRht))
                goto RIGHT_RANK_EXIT;

            if (IsZeroDim (aplColsRht))
                aplRowsRht = 0;
            else
                aplRowsRht = aplNELMRht / aplColsRht;

            if (aplColsRht NE aplRowsRht)
                goto RIGHT_LENGTH_EXIT;

            // Save as the two dimensions of the matrix
            NxN[0] =
            NxN[1] = aplColsRht;

            // Check for DOMAIN ERROR
            if (!(IsSimpleInt (aplTypeRht)
               || IsSimpleFlt (aplTypeRht)))
                goto RIGHT_DOMAIN_EXIT;

            // Sadly, GSL ABORTS! if asked to handle an empty array:  Boo
            if (!IsZeroDim (aplColsRht))
            {
                // Allocate GSL arrays
                lpGslMatrixA     = gsl_matrix_alloc         ((int) aplRowsRht, (int) aplColsRht);   // N x N
                lpGslCVectorEval = gsl_vector_complex_alloc ((int) aplColsRht                  );   // N
                lpGslCMatrixEvec = gsl_matrix_complex_alloc ((int) aplRowsRht, (int) aplColsRht);   // N x N
                lpGslEigenWs     = gsl_eigen_nonsymmv_alloc ((int) aplColsRht                  );   // N

                // Check the return codes for the above allocations
                if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixA
                 || GSL_ENOMEM EQ (HANDLE_PTR) lpGslCVectorEval
                 || GSL_ENOMEM EQ (HANDLE_PTR) lpGslCMatrixEvec
                 || GSL_ENOMEM EQ (HANDLE_PTR) lpGslEigenWs)
                    goto WSFULL_EXIT;

                // Loop through all of the elements
                for (i = 0; i < aplNELMRht; i++)
                    // Copy the next element from the right arg as a float
                    lpGslMatrixA->data[i] = GetNextFloat (lpMemRht, aplTypeRht, i);

                // Calculate the Eigenvalues and Eigenvectors
                ErrCode =
                  gsl_eigen_nonsymmv (lpGslMatrixA,
                                      lpGslCVectorEval,
                                      lpGslCMatrixEvec,
                                      lpGslEigenWs);
                // Check the error code
                if (ErrCode NE GSL_SUCCESS)
                    goto RIGHT_DOMAIN_EXIT;
            } // End IF

            // Split cases based upon the right operand value
            switch (aplIntegerRhtOpr)
            {
                case 1:
                    // Allocate a global array for the result
                    hGlbRes = AllocateGlobalArray (ARRAY_HC2F, aplColsRht, 1, &NxN[0]);

                    break;

                case 2:
                    // Allocate a global array for the result
                    hGlbRes = AllocateGlobalArray (ARRAY_HC2F, aplNELMRht, 2, &NxN[0]);

                    break;

                case 3:
                    // Allocate a global array for the result
                    hGlbRes = AllocateGlobalArray (ARRAY_NESTED, iTwo, 1, &iTwo);

                    break;

                defstop
                    break;
            } // End SWITCH

            // Check for error
            if (hGlbRes EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemHdrRes = MyGlobalLockVar (hGlbRes);

            // Skip over the header and dimensions to the data
            lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

            // Split cases based upon the right operand value
            switch (aplIntegerRhtOpr)
            {
                case 1:
                    // Copy the Eigenvalues to the result
                    CopyMemory (lpMemRes, lpGslCVectorEval->data, (APLU3264) (aplColsRht * 2 * sizeof (APLFLOAT)));

                    break;

                case 2:
                    // Copy the Eigenvectors to the result
                    CopyMemory (lpMemRes, lpGslCMatrixEvec->data, (APLU3264) (aplNELMRht * 2 * sizeof (APLFLOAT)));

                    break;

                case 3:
                    // Allocate a global array for the Eigenvalues
                    hGlbEval = AllocateGlobalArray (ARRAY_HC2F, aplColsRht, 1, &NxN[0]);

                    // Allocate a global array for the Eigenvectors
                    hGlbEvec = AllocateGlobalArray (ARRAY_HC2F, aplNELMRht, 2, &NxN[0]);

                    // Check for errors
                    if (hGlbEval EQ NULL
                     || hGlbEvec EQ NULL)
                        goto WSFULL_EXIT;
                    // Save the global memory handles in the result
                    ((LPAPLNESTED) lpMemRes)[0] = MakePtrTypeGlb (hGlbEval);
                    ((LPAPLNESTED) lpMemRes)[1] = MakePtrTypeGlb (hGlbEvec);

                    // If there are any cols, ...
                    if (!IsZeroDim (aplColsRht))
                    {
                        // Lock the memory to get a ptr to it
                        lpMemHdrEval = MyGlobalLockVar (hGlbEval);

                        // Skip over the header and dimensions to the data
                        lpMemEval = VarArrayDataFmBase (lpMemHdrEval);

                        // Copy the Eigenvalues to the result
                        CopyMemory (lpMemEval, lpGslCVectorEval->data, (APLU3264) (aplColsRht * 2 * sizeof (APLFLOAT)));

                        // Lock the memory to get a ptr to it
                        lpMemHdrEvec = MyGlobalLockVar (hGlbEvec);

                        // Skip over the header and dimensions to the data
                        lpMemEvec = VarArrayDataFmBase (lpMemHdrEvec);

                        // Copy the Eigenvectors to the result
                        CopyMemory (lpMemEvec, lpGslCMatrixEvec->data, (APLU3264) (aplNELMRht * 2 * sizeof (APLFLOAT)));
                    } // End IF

                    break;

                defstop
                    break;
            } // End SWITCH

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

            break;

////////// []CF:  Circular Functions divisor:
//////////   L{circle}{variant}X R   is   L{circle}R{divide}X{divide}{circle}0.5
////////case UTF16_CIRCLE:                  // Dyadic w/ L=-3 -2 -1 1 2 3
////////case UTF16_CIRCLE2:                 // ...
////////    // Validate the right operand as
////////    //   a simple numeric scalar or one-element vector
////////    if (IsMultiRank (aplRankRhtOpr))
////////        goto RIGHT_OPERAND_RANK_EXIT;
////////    if (aplNELMRhtOpr NE 1)
////////        goto RIGHT_OPERAND_LENGTH_EXIT;
////////    if (!IsNumeric (aplTypeRhtOpr))
////////        goto RIGHT_OPERAND_DOMAIN_EXIT;
////////
////////    // Get the magic function/operator global memory handle
////////    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_DydVOCirc];
////////
////////    lpYYRes =
////////      ExecuteMagicFunction_EM_YY (lptkLftArg,               // Ptr to left arg token
////////                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
////////                                  lpYYFcnStrOpr,            // Ptr to function strand
////////                                  lptkRhtArg,               // Ptr to right arg token
////////                                 &lpYYFcnStrRht->tkToken,   // Ptr to axis token
////////                                  hGlbMFO,                  // Magic function/operator global memory handle
////////                                  NULL,                     // Ptr to HSHTAB struc (may be NULL)
////////                                  bPrototyping
////////                                ? LINENUM_PRO
////////                                : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
////////    break;

////////// []RA:  Residue arithmetic:  Operand | L f R
////////case UTF16_STAR:                    // Dyadic only
////////case UTF16_STAR2:                   // Dyadic only
////////    // Ensure there's a left arg
////////    if (lptkLftArg EQ NULL)
////////        goto LEFT_VALENCE_EXIT;
////////
////////    // Validate the right operand as
////////    //   a simple numeric scalar or one- or two-element vector
////////    if (IsMultiRank (aplRankRhtOpr))
////////        goto RIGHT_OPERAND_RANK_EXIT;
////////    if (aplNELMRhtOpr NE 1
////////     && aplNELMRhtOpr NE 2)
////////        goto RIGHT_OPERAND_LENGTH_EXIT;
////////    if (!IsNumeric (aplTypeRhtOpr))
////////        goto RIGHT_OPERAND_DOMAIN_EXIT;
////////
////////    // Fall through to other Residue Arithmetic code
////////
////////// []RA:  Residue arithmetic:  Operand | L f R
////////case UTF16_PLUS:                    // Dyadic only
////////case UTF16_BAR:                     // Monadic or dyadic
////////case UTF16_BAR2:                    // Monadic or dyadic
////////case UTF16_TIMES:                   // Dyadic only
////////    // Ensure there's a left arg
////////    if (lpYYFcnStrLft->tkToken.tkData.tkChar NE UTF16_BAR
////////     && lpYYFcnStrLft->tkToken.tkData.tkChar NE UTF16_BAR2
////////     && lptkLftArg EQ NULL)
////////        goto LEFT_VALENCE_EXIT;
////////
////////    // Validate the right operand as
////////    //   a simple numeric scalar or one-element vector
////////    if (IsMultiRank (aplRankRhtOpr))
////////        goto RIGHT_OPERAND_RANK_EXIT;
////////    if (aplNELMRhtOpr NE 1)
////////        goto RIGHT_OPERAND_LENGTH_EXIT;
////////    if (!IsNumeric (aplTypeRhtOpr))
////////        goto RIGHT_OPERAND_DOMAIN_EXIT;
////////
////////    // Execute the function
////////    lpYYRes2 =
////////      ExecFunc_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
////////                      lpYYFcnStrLft,        // Ptr to function strand
////////                      lptkRhtArg);          // Ptr to right arg token
////////    // If the result is valid, ...
////////    if (lpYYRes2)
////////    {
////////        // Setup a token with the {modulus} function
////////        tkFcn.tkFlags.TknType   = TKT_FCNIMMED;
////////        tkFcn.tkFlags.ImmType   = IMMTYPE_PRIMFCN;
////////////////tkFcn.tkFlags.NoDisplay = FALSE;           // Already zero from = {0}
////////        tkFcn.tkData.tkChar     = UTF16_STILE;
////////        tkFcn.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;
////////
////////        // Execute the function
////////        lpYYRes =
////////          PrimFnStile_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
////////                            &tkFcn,                 // Ptr to function token
////////                            &lpYYRes2->tkToken,     // Ptr to right arg token
////////                             NULL);                 // Ptr to axis token (may be NULL)
////////        // Free the secondary result
////////        FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
////////    } // End IF
////////
////////    break;

////////// []FE:  Fill Element:  ***FIXME*** -- Default value is either 0 or ' '
////////case UTF16_UPARROW:                 // Dyadic only
////////case UTF16_SLOPE:                   // ...
////////    // DbgBrk ();           // ***FINISHME*** -- []FE
////////
////////    PrimFnNonceError_EM (&lpYYFcnStrLft->tkToken APPEND_NAME_ARG);
////////
////////
////////
////////
////////
////////
////////
////////
////////
////////
////////
////////
////////
////////
////////    break;

        default:
            goto LEFT_OPERAND_DOMAIN_EXIT;
    } // End SWITCH

    goto NORMAL_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

LEFT_VALENCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_OPERAND_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (lpYYRes NE NULL)
    {
        // Free the YYRes
        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF

    if (hGlbEval NE NULL)
    {
        if (lpMemHdrEval NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbEval); lpMemHdrEval = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbEval); hGlbEval = NULL;
    } // End IF

    if (hGlbEvec NE NULL)
    {
        if (lpMemHdrEvec NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbEvec); lpMemHdrEvec = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbEvec); hGlbEvec = NULL;
    } // End IF

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
    if (hGlbEval NE NULL
     && lpMemHdrEval NE NULL)
    {
         // We no longer need this ptr
        MyGlobalUnlock (hGlbEval); lpMemHdrEval = NULL;
    } // End IF

    if (hGlbEvec NE NULL
     && lpMemHdrEvec NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbEvec); lpMemHdrEvec = NULL;
    } // End IF

    if (hGlbRes NE NULL
     && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbRht NE NULL
     && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (lpGslCMatrixEvec NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_complex_free (lpGslCMatrixEvec); lpGslCMatrixEvec = NULL;
    } // End IF

    if (lpGslCVectorEval NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_vector_complex_free (lpGslCVectorEval); lpGslCVectorEval = NULL;
    } // End IF

    if (lpGslMatrixA NE NULL)
    {
        // We no longer need this storage and ptr
        gsl_matrix_free (lpGslMatrixA); lpGslMatrixA = NULL;
    } // End IF

    return lpYYRes;
} // End PrimOpVariantCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  Magic functions/operators for Variant Operator
//***************************************************************************

#include "mf_variant.h"

/// //***************************************************************************
/// //  Magic function/operator for circular function divisor from the Variant Operator
/// //***************************************************************************
///
/// static APLCHAR DydVOCircHeader[] =
///   L"Z" $IS L"L " MFON_DydVOCirc L"[X] R;" $QUAD_IO;
///
/// static APLCHAR DydVOCircLine1[] =
///   L"Z" $IS L"L" $CIRCLE $CIRCLE L"R" $DIVIDE L"2" $TIMES L"X";
///
/// static LPAPLCHAR DydVOCircBody[] =
/// {DydVOCircLine1,
/// };
///
/// MAGIC_FCNOPR MFO_DydVOCirc =
/// {DydVOCircHeader,
///  DydVOCircBody,
///  countof (DydVOCircBody),
/// };


//***************************************************************************
//  $PrimOpVariantCheckHetero
//
//  Check a hetero arg for a char or an integer
//***************************************************************************

UBOOL PrimOpVariantCheckHetero
    (LPVOID     lpMemRhtOpr,            // Ptr to right operand global memory data
     APLINT     uIndex,                 // Index into lpMemRhtOpr to use
     LPUBOOL    lpbNumFound,            // Ptr to bNumFound (Numeric found)
     LPUBOOL    lpbChrFound,            // ...    bChrFound (Char found)
     LPAPLINT   lpaplInteger,           // ...    aplInteger (May be NULL if FLT requested)
     LPAPLFLOAT lpaplFloat,             // ...    aplFloat   (May be NULL if INT requested
     LPAPLCHAR  lpaplChar)              // ...    aplChar

{
    APLSTYPE aplTypeItm;
    APLINT   aplIntegerItm;
    APLFLOAT aplFloatItm;
    APLCHAR  aplCharItm;
    HGLOBAL  hGlbItm;
    UBOOL    bRet = TRUE;

    Assert ((lpaplInteger EQ NULL) NE (lpaplFloat EQ NULL));

    aplTypeItm =
      GetNextHetero (lpMemRhtOpr, uIndex, &aplIntegerItm, &aplFloatItm, &aplCharItm, &hGlbItm);

    // Split cases based upon the item storage type
    switch (aplTypeItm)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
            // Check for duplicate entry
            if (*lpbNumFound)
                goto ERROR_EXIT;

            // Set the flag
            *lpbNumFound = TRUE;        // In aplIntegerRhtOpr

            break;

        case ARRAY_FLOAT:
            // Check for duplicate entry
            if (*lpbNumFound)
                goto ERROR_EXIT;

            if (lpaplInteger NE NULL)
            {
                // Attempt to convert the float to an integer using System []CT
                aplIntegerItm =
                  FloatToAplint_SCT (aplFloatItm,
                                    &bRet);
                // Check for error
                if (!bRet)
                    goto ERROR_EXIT;
            } // End IF

            // Set the flag
            *lpbNumFound = TRUE;        // In *lpaplInteger or *lpaplFloat

            break;

        case ARRAY_RAT:
            // Check for duplicate entry
            if (*lpbNumFound)
                goto ERROR_EXIT;

            if (lpaplInteger NE NULL)
            {
                // Attempt to convert the RAT to an integer
                aplIntegerItm = mpq_get_sx ((LPAPLRAT) &hGlbItm, &bRet);

                // Check for error
                if (!bRet)
                    goto ERROR_EXIT;
            } else
                // Convert the RAT to a float
                aplFloatItm = mpq_get_d ((LPAPLRAT) &hGlbItm);

            // Set the flag
            *lpbNumFound = TRUE;        // In aplIntegerRhtOpr

            break;

        case ARRAY_VFP:
            // Check for duplicate entry
            if (*lpbNumFound)
                goto ERROR_EXIT;

            if (lpaplInteger NE NULL)
            {
                // Attempt to convert the VFP to an integer
                aplIntegerItm = mpfr_get_sx ((LPAPLVFP) &hGlbItm, &bRet);

                // Check for error
                if (!bRet)
                    goto ERROR_EXIT;
            } else
                mpfr_get_d ((LPAPLVFP) &hGlbItm, MPFR_RNDN);

            // Set the flag
            *lpbNumFound = TRUE;        // In aplIntegerRhtOpr

            break;

        case ARRAY_CHAR:
            // Check for duplicate entry
            if (*lpbChrFound)
                goto ERROR_EXIT;

            // Set the flag
            *lpbChrFound = TRUE;        // In aplCharRhtOpr

            break;

        case ARRAY_APA:             // Can't happen
        case ARRAY_HETERO:          // ...
        case ARRAY_NESTED:          // ...
        defstop
            break;
    } // End SWITCH

    // Save in globals
    if (*lpbNumFound)
    {

        if (lpaplInteger NE NULL)
            *lpaplInteger = aplIntegerItm;
        else
            *lpaplFloat   = aplFloatItm;
    } // End IF

    if (*lpbChrFound)
        *lpaplChar    = aplCharItm;

    return TRUE;

ERROR_EXIT:
    return FALSE;
} // End PrimOpVariantCheckHetero


//***************************************************************************
//  $PrimOpDydVariant_EM_YY
//
//  Primitive operator for dyadic derived function from Variant ("variant")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydVariant_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return
      PrimOpDydVariantCommon_EM_YY (lptkLftArg,         // Ptr to left arg token
                                    lpYYFcnStrOpr,      // Ptr to operator function strand
                                    lptkRhtArg,         // Ptr to right arg token
                                    FALSE);             // TRUE iff prototyping
} // End PrimOpDydVariant_EM_YY


//***************************************************************************
//  $PrimOpDydVariantCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Variant ("variant")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydVariantCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, CheckAxisOper (lpYYFcnStrOpr));
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrRht);

    return
      PrimOpVariantCommon_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if monadic derived function)
                                 lpYYFcnStrLft,         // Ptr to left operand function strand
                                 lpYYFcnStrOpr,         // Ptr to operator function strand
                                 lpYYFcnStrRht,         // Ptr to right operand function strand
                                 lptkRhtArg,            // Ptr to right arg token
                                 bPrototyping);         // TRUE iff protoyping
} // End PrimOpDydVariantCommon_EM_YY


//***************************************************************************
//  $PrimOpVariantKeyword_EM_YY
//
//  Primitve operator variant using keyword right arg
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpVariantKeyword_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimOpVariantKeyword_EM_YY
    (LPTOKEN      lptkLftArg,                           // Ptr to left arg token (may be NULL if monadic derived function)
     LPPL_YYSTYPE lpYYFcnStrLft,                        // Ptr to left operand function strand
     LPPL_YYSTYPE lpYYFcnStrRht,                        // Ptr to right operand function strand
     LPTOKEN      lptkRhtArg,                           // Ptr to right arg token
     APLSTYPE     aplTypeRhtOpr,                        // Right operand storage type
     APLNELM      aplNELMRhtOpr,                        // ...           NELM
     APLRANK      aplRankRhtOpr,                        // ...           rank
     LPPERTABDATA lpMemPTD)                             // Ptr to PerTabData global memory

{
    LPAPLNESTED   lpMemRhtOpr;                          // Ptr to right operand global memory
    HGLOBAL       hGlbRhtOpr = NULL;                    // Right operand global memory handle
    APLINT        iRhtOpr;                              // Loop counter
    VARIANTKEYS   varKey;                               // ...
    LPPL_YYSTYPE  lpYYRes = NULL;                       // Ptr to result
    VARIANTUSESTR varUseStr[VARIANT_KEY_LENGTH] = {0};  // In use struc
    UBOOL         bQuery;                               // TRUE iff the left operand is the immediate function query

    // The right operand is one of the following cases:
    //   1.  'IO' n
    //   2.  {enclose} 'IO n
    //   3.  ('IO' n) ('CT' n) ...

    // If the right operand is multirank, ...
    if (IsMultiRank (aplRankRhtOpr))
        goto RIGHT_OPERAND_RANK_EXIT;

    // If the right operand is empty, ...
    if (IsEmpty (aplNELMRhtOpr))
        goto RIGHT_OPERAND_LENGTH_EXIT;

    // If the right operand is a simple scalar, ...
    if (IsNumeric (aplTypeRhtOpr)
     && IsScalar (aplRankRhtOpr))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Check for query
    bQuery = lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_QUERY;

    // Get right operand's global ptrs
    GetGlbPtrs_LOCK (&lpYYFcnStrRht->tkToken, &hGlbRhtOpr, (LPVOID *) &lpMemRhtOpr);

    // If the global is not a two-element vector,
    //   or it is but doesn't validate, ...
    if (aplNELMRhtOpr NE 2
     || !PrimOpVariantValidateGlb_EM (hGlbRhtOpr,               // Right operand global memory handle
                                      bQuery,                   // TRUE iff the left operand is the immediate function query
                                     &lpYYFcnStrRht->tkToken,   // Ptr to function token
                                     &varUseStr[0],             // Ptr to variant use struc
                                      lpMemPTD))                // Ptr to PerTabData global memory
    {
        // Skip over the header and dimensions to the data
        lpMemRhtOpr = VarArrayDataFmBase (lpMemRhtOpr);

        // Loop through the items backwards
        for (iRhtOpr = aplNELMRhtOpr - 1; iRhtOpr >= 0; iRhtOpr--)
        // If it fails,
        if (!PrimOpVariantValidateGlb_EM (lpMemRhtOpr[iRhtOpr],     // Item global memory handle
                                          bQuery,                   // TRUE iff the left operand is the immediate function query
                                         &lpYYFcnStrRht->tkToken,   // Ptr to function token
                                         &varUseStr[0],             // Ptr to variant use struc
                                          lpMemPTD))                // Ptr to PerTabData global memory
        {
            // If the function is immediate and is ?, ...
            if (IsTknImmed (&lpYYFcnStrLft->tkToken)                // If the function is immediate,
             && lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_QUERY // and it's query
             && IsSimpleHet (aplTypeRhtOpr)                         // and the right operand is hetero
             && aplNELMRhtOpr EQ 2)                                 // and the right operand is a pair, ...
            {
                UBOOL    bNumFound = FALSE,
                         bChrFound = FALSE;
                APLINT   aplIntegerRhtOpr;
                APLCHAR  aplCharRhtOpr;

                // Here we need to test for the following possibilities
                // 'g' 0
                // 0 'g'

                // Check the first entry
                if (!PrimOpVariantCheckHetero (lpMemRhtOpr,         // Ptr to right operand global memory data
                                               0,                   // Index into lpMemRhtOpr to use
                                              &bNumFound,           // Ptr to bNumFound
                                              &bChrFound,           // ...    bChrFound
                                              &aplIntegerRhtOpr,    // ...    aplIntegerRhtOpr
                                               NULL,                // ...    aplFloatRhtOpr
                                              &aplCharRhtOpr))      // ...    ...Char ...
                    goto RIGHT_OPERAND_DOMAIN_EXIT;

                // Check the second entry
                if (!PrimOpVariantCheckHetero (lpMemRhtOpr,         // Ptr to right operand global memory data
                                               1,                   // Index into lpMemRhtOpr to use
                                              &bNumFound,           // Ptr to bNumFound
                                              &bChrFound,           // ...    bChrFound
                                              &aplIntegerRhtOpr,    // ...    aplIntegerRhtOpr
                                               NULL,                // ...    aplFloatRhtOpr
                                              &aplCharRhtOpr))      // ...    ...Char ...
                    goto RIGHT_OPERAND_DOMAIN_EXIT;

                // If neither value was found, ...
                if (!bNumFound
                 && !bChrFound)
                    goto RIGHT_OPERAND_DOMAIN_EXIT;

                // If we found a value for []IO
                if (bNumFound
                    // Save the SymEntry, validate the value
                 && !VariantValidateSymVal_EM (IMMTYPE_INT,             // Item immediate type
                                               NULL,                    // Item global memory handle
                              *(LPAPLLONGEST) &aplIntegerRhtOpr,        // Immediate value
                                               FALSE,                   // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
                                              &varUseStr[0],            // Ptr to variant use struc
                                               VARIANT_KEY_IO,          // Variant key index
                                               lpMemPTD,                // Ptr to PerTabData global memory
                                              &lpYYFcnStrRht->tkToken)) // Ptr to function token
                    goto ERROR_EXIT;

                // If we found a value for []DT
                if (bChrFound
                    // Save the SymEntry, validate the value
                 && !VariantValidateSymVal_EM (IMMTYPE_CHAR,            // Item immediate type
                                               NULL,                    // Item global memory handle
                              *(LPAPLLONGEST) &aplCharRhtOpr,           // Immediate value
                                               FALSE,                   // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
                                              &varUseStr[0],            // Ptr to variant use struc
                                               VARIANT_KEY_DT,          // Variant key index
                                               lpMemPTD,                // Ptr to PerTabData global memory
                                              &lpYYFcnStrRht->tkToken)) // Ptr to function token
                    goto ERROR_EXIT;

                break;
            } else
            // If the function is immediate and is |, ...
            if (IsTknImmed (&lpYYFcnStrLft->tkToken)                    // If the function is immediate,
             && (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_STILE    // and it's stile/stile2
              || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_STILE2)  // ...
             && IsSimpleHet (aplTypeRhtOpr)                             // and the right operand is hetero
             && aplNELMRhtOpr EQ 2)                                     // and the right operand is a pair, ...
            {
                UBOOL    bNumFound = FALSE,
                         bChrFound = FALSE;
                APLFLOAT aplFloatRhtOpr;
                APLCHAR  aplCharRhtOpr;

                // Here we need to test for the following possibilities
                // 'g' 0
                // 0 'g'

                // Check the first entry
                if (!PrimOpVariantCheckHetero (lpMemRhtOpr,         // Ptr to right operand global memory data
                                               0,                   // Index into lpMemRhtOpr to use
                                              &bNumFound,           // Ptr to bNumFound
                                              &bChrFound,           // ...    bChrFound
                                               NULL,                // ...    aplIntegerRhtOpr
                                              &aplFloatRhtOpr,      // ...    aplFloatRhtOpr
                                              &aplCharRhtOpr))      // ...    ...Char ...
                    goto RIGHT_OPERAND_DOMAIN_EXIT;

                // Check the second entry
                if (!PrimOpVariantCheckHetero (lpMemRhtOpr,         // Ptr to right operand global memory data
                                               1,                   // Index into lpMemRhtOpr to use
                                              &bNumFound,           // Ptr to bNumFound
                                              &bChrFound,           // ...    bChrFound
                                               NULL,                // ...    aplIntegerRhtOpr
                                              &aplFloatRhtOpr,      // ...    aplFloatRhtOpr
                                              &aplCharRhtOpr))      // ...    ...Char ...
                    goto RIGHT_OPERAND_DOMAIN_EXIT;

                // If neither value was found, ...
                if (!bNumFound
                 && !bChrFound)
                    goto RIGHT_OPERAND_DOMAIN_EXIT;

                // If we found a value for []CT
                if (bNumFound
                    // Save the SymEntry, validate the value
                 && !VariantValidateSymVal_EM (IMMTYPE_FLOAT,           // Item immediate type
                                               NULL,                    // Item global memory handle
                              *(LPAPLLONGEST) &aplFloatRhtOpr,          // Immediate value
                                               FALSE,                   // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
                                              &varUseStr[0],            // Ptr to variant use struc
                                               VARIANT_KEY_CT,          // Variant key index
                                               lpMemPTD,                // Ptr to PerTabData global memory
                                              &lpYYFcnStrRht->tkToken)) // Ptr to function token
                    goto ERROR_EXIT;

                // If we found a value for []DQ
                if (bChrFound
                    // Save the SymEntry, validate the value
                 && !VariantValidateSymVal_EM (IMMTYPE_CHAR,            // Item immediate type
                                               NULL,                    // Item global memory handle
                              *(LPAPLLONGEST) &aplCharRhtOpr,           // Immediate value
                                               FALSE,                   // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
                                              &varUseStr[0],            // Ptr to variant use struc
                                               VARIANT_KEY_DQ,          // Variant key index
                                               lpMemPTD,                // Ptr to PerTabData global memory
                                              &lpYYFcnStrRht->tkToken)) // Ptr to function token
                    goto ERROR_EXIT;

                break;
            } else
                goto ERROR_EXIT;
        } // End FOR/IF
    } // End IF

    // At this point, we've localised all of the system vars
    //   and changed their current values to what the user specified

    // Execute the function
    lpYYRes =
      ExecFuncStr_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
                         lpYYFcnStrLft,        // Ptr to function strand
                         lptkRhtArg,           // Ptr to right arg token
                         NULL);                 // Ptr to axis token (may be NULL)
    goto NORMAL_EXIT;

RIGHT_OPERAND_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbRhtOpr && lpMemRhtOpr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRhtOpr); lpMemRhtOpr = NULL;
    } // End IF

    // Restore the old SYMENTRYs
    for (varKey = 0; varKey < countof (varUseStr); varKey++)
    if (varUseStr[varKey].bInuse)
        *lpMemPTD->lphtsPTD->lpSymQuad[aVariantKeyStr[varKey].sysVarIndex] =
          varUseStr[varKey].OldSymEntry;

    return lpYYRes;
} // End PrimOpVariantKeyword_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpVariantValidateGlb_EM
//
//  Validate a single item as either
//      {enclose}'IO' n
// or an element of
//      ('IO' n) ('CT' n) ...
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpVariantValidate_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimOpVariantValidateGlb_EM
    (HGLOBAL         hGlbRhtOpr,        // Right operand global memory handle
     UBOOL           bQuery,            // TRUE iff the left operand is the immediate function query
     LPTOKEN         lptkFunc,          // Ptr to function token
     LPVARIANTUSESTR lpVarUseStr,       // Ptr to variant use struc
     LPPERTABDATA    lpMemPTD)          // Ptr to PerTabData global memory

{
    LPAPLNESTED lpMemRhtOpr = NULL;     // Right operand global memory handle
    UBOOL       bRet = FALSE,           // TRUE iff the result is valid
                bReset;                 // TRUE iff the second item is empty
    APLSTYPE    aplTypeRhtOpr;          // Right operand array storage type
    APLNELM     aplNELMRhtOpr;          // Right operand NELM
    HGLOBAL     hGlbItm;                // Item global memory handle
    APLLONGEST  aplLongestItm;          // Item immediate value
    IMM_TYPES   immTypeItm;             // Item immediate type
    VARIANTKEYS varKey;                 // Variant key index

    // Split cases based upon the ptr type bits
    switch (GetPtrTypeDir (hGlbRhtOpr))
    {
        case PTRTYPE_STCONST:
            goto RIGHT_OPERAND_DOMAIN_EXIT;

        case PTRTYPE_HGLOBAL:
            // Lock the memory to get a ptr to it
            lpMemRhtOpr = MyGlobalLockVar (hGlbRhtOpr);

            // Get the storage type
            aplTypeRhtOpr = ((LPVARARRAY_HEADER) lpMemRhtOpr)->ArrType;
            aplNELMRhtOpr = ((LPVARARRAY_HEADER) lpMemRhtOpr)->NELM;

            // If it's not a vector, ...
            if (!IsVector (((LPVARARRAY_HEADER) lpMemRhtOpr)->Rank))
                goto RIGHT_OPERAND_RANK_EXIT;

            // If it's not a singleton or pair, ...
            if (aplNELMRhtOpr NE 1
             && aplNELMRhtOpr NE 2)
                goto RIGHT_OPERAND_LENGTH_EXIT;

            // If it's not hetero or nested, ...
            if (!IsPtrArray (aplTypeRhtOpr))
                goto RIGHT_OPERAND_DOMAIN_EXIT;

            // Skip over the header and dimensions to the data
            lpMemRhtOpr = VarArrayDataFmBase (lpMemRhtOpr);

            break;

        defstop
            break;
    } // End SWITCH

    // Get the first value from the right operand
    GetNextValueMem (lpMemRhtOpr,               // Ptr to right operand global memory
                      aplTypeRhtOpr,            // Right operand storage type
                      aplNELMRhtOpr,            // Right operand NELM
                      0,                        // Index to use
                     &hGlbItm,                  // Ptr to the LPSYMENTRY or HGLOBAL (may be NULL)
                     &aplLongestItm,            // ...        immediate value (may be NULL)
                     &immTypeItm);              // ...        immediate type:  IMM_TYPES (may be NULL)
    // If the left operand is query, ...
    if (bQuery)
    {
        UBOOL   bQuadIOFound = FALSE,       // TRUE iff []IO value found
                bQuadDTFound = FALSE;       // ...      []DT ...
        APLCHAR aplCharItm;                 // Immediate char

        // The right operand may be of the form
        //   'g' 0      or
        //    0 'g'     or
        //   'IO' 0
        // The following code checks for the first two cases
        //   and falls through for the third case

        if (hGlbItm EQ NULL             // Item is immediate
         && IsImmChr (immTypeItm))      //   and char
        {
            // Mark as found
            bQuadDTFound = TRUE;
            aplCharItm = (APLCHAR) aplLongestItm;
        } // End IF

        if (hGlbItm EQ NULL             // Item is immediate
         && IsImmInt (immTypeItm))      //   and integer
        {
            // Mark as found
            bQuadIOFound = TRUE;
        } // End IF

        // If there's a second item, ...
        if (aplNELMRhtOpr EQ 2)
        {
            // Get the second value from the right operand
            GetNextValueMem (lpMemRhtOpr,               // Ptr to right operand global memory
                             aplTypeRhtOpr,             // Right operand storage type
                             aplNELMRhtOpr,             // Right operand NELM
                             1,                         // Index to use
                            &hGlbItm,                   // Ptr to the LPSYMENTRY or HGLOBAL (may be NULL)
                            &aplLongestItm,             // ...        immediate value (may be NULL)
                            &immTypeItm);               // ...        immediate type:  IMM_TYPES (may be NULL)
            if (hGlbItm EQ NULL             // Item is immediate
             && IsImmChr (immTypeItm))      //   and char
            {
                // Check for duplicate, ...
                if (bQuadDTFound)
                    goto RIGHT_OPERAND_DOMAIN_EXIT;

                // Mark as found
                bQuadDTFound = TRUE;
                aplCharItm = (APLCHAR) aplLongestItm;
            } // End IF

            if (hGlbItm EQ NULL             // Item is immediate
             && IsImmInt (immTypeItm))      //   and integer
            {
                // Check for duplicate, ...
                if (bQuadIOFound)
                    goto RIGHT_OPERAND_DOMAIN_EXIT;

                // Mark as found
                bQuadIOFound = TRUE;
            } // End IF
        } // End IF

        // If we found []DT, ...
        if (bQuadDTFound)
            // Save the SymEntry, validate the value
            if (!VariantValidateSymVal_EM (immTypeItm,          // Item immediate type
                                           hGlbItm,             // Item global memory handle
                          *(LPAPLLONGEST) &aplCharItm,          // Immediate value
                                           FALSE,               // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
                                           lpVarUseStr,         // Ptr to variant use struc
                                           VARIANT_KEY_DT,      // Variant key index
                                           lpMemPTD,            // Ptr to PerTabData global memory
                                           lptkFunc))           // Ptr to function token
                goto RIGHT_OPERAND_DOMAIN_EXIT;
        // If we found []IO, ...
        if (bQuadIOFound)
            // Save the SymEntry, validate the value
            if (!VariantValidateSymVal_EM (immTypeItm,          // Item immediate type
                                           hGlbItm,             // Item global memory handle
                                           aplLongestItm,       // Immediate value
                                           FALSE,               // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
                                           lpVarUseStr,         // Ptr to variant use struc
                                           VARIANT_KEY_IO,      // Variant key index
                                           lpMemPTD,            // Ptr to PerTabData global memory
                                           lptkFunc))           // Ptr to function token
                goto RIGHT_OPERAND_DOMAIN_EXIT;
        // If either found, ...
        if (bQuadIOFound
         || bQuadDTFound)
            goto NORMAL_EXIT;
    } // End IF

    // Check for char
    //   and pair
    if (!IsImmChr (immTypeItm)
     || aplNELMRhtOpr NE 2)
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Validate the keyword
    varKey = PrimOpVariantValKeyGlb (lpMemRhtOpr[0]);
    if (varKey EQ VARIANT_KEY_ERROR)
        goto ERROR_EXIT;

    // Get the second value from the right operand
    GetNextValueMem (lpMemRhtOpr,               // Ptr to right operand global memory
                     aplTypeRhtOpr,             // Right operand storage type
                     aplNELMRhtOpr,             // Right operand NELM
                     1,                         // Index to use
                    &hGlbItm,                   // Ptr to the LPSYMENTRY or HGLOBAL (may be NULL)
                    &aplLongestItm,             // ...        immediate value (may be NULL)
                    &immTypeItm);               // ...        immediate type:  IMM_TYPES (may be NULL)
    // Check for error
    if (IsImmErr (immTypeItm))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Set the reset flag
    if (hGlbItm EQ NULL             // Immediate value
     || IsImmGlbNum (immTypeItm))   // Simple global numeric
        bReset = FALSE;
    else
    {
        APLNELM aplNELMItm;

        AttrsOfGlb (hGlbItm, NULL, &aplNELMItm, NULL, NULL);

        bReset = (aplNELMItm EQ 0);
    } // End IF/ELSE

    // Save the SymEntry, validate the value
    if (!VariantValidateSymVal_EM (immTypeItm,          // Item immediate type
                                   hGlbItm,             // Item global memory handle
                                   aplLongestItm,       // Immediate value
                                   bReset,              // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
                                   lpVarUseStr,         // Ptr to variant use struc
                                   varKey,              // Variant key index
                                   lpMemPTD,            // Ptr to PerTabData global memory
                                   lptkFunc))           // Ptr to function token
        goto ERROR_EXIT;

    goto NORMAL_EXIT;

RIGHT_OPERAND_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

RIGHT_OPERAND_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NORMAL_EXIT:
    // Mark as valid
    bRet = TRUE;
ERROR_EXIT:
    if (lpMemRhtOpr NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRhtOpr); lpMemRhtOpr = NULL;
    } // End IF

    return bRet;
} // End PrimOpVariantValidateGlb_EM
#undef  APPEND_NAME


//***************************************************************************
//  $VariantValidateSymVal
//
//  Save the SymEntry, validate the value
//***************************************************************************

UBOOL VariantValidateSymVal_EM
    (IMM_TYPES       immTypeItm,        // Item immediate type
     HGLOBAL         hGlbItm,           // Item global memory handle
     APLLONGEST      aplLongestItm,     // Item immediate value
     UBOOL           bReset,            // TRUE iff the assignment value is empty (we're resetting to CLEAR WS/System)
     LPVARIANTUSESTR lpVarUseStr,       // Ptr to variant use struc
     VARIANTKEYS     varKey,            // Variant key index
     LPPERTABDATA    lpMemPTD,          // Ptr to PerTabData global memory
     LPTOKEN         lptkFunc)          // Ptr to function token

{
    LPSYMENTRY  lpSymEntry;             // Ptr to sysvar SYMENTRY

    // Get a ptr to the SYMENTRY
    lpSymEntry = lpMemPTD->lphtsPTD->lpSymQuad[aVariantKeyStr[varKey].sysVarIndex];

    // If this variant property isn't in use as yet, ...
    if (!lpVarUseStr[varKey].bInuse)
    {
        // Mark as in use
        lpVarUseStr[varKey].bInuse = TRUE;

        // Back up the corresponding SYMENTRY to restore after executing the Variant Operator
        lpVarUseStr[varKey].OldSymEntry = *lpSymEntry;
    } // End IF

    // Validate the value
    return VariantValidateCom_EM (immTypeItm,                               // Immediate type
                                  hGlbItm,                                  //
                                 &aplLongestItm,                            // Ptr to immediate value (ignored if bReset)
                                  bReset,                                   // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
                                  aVariantKeyStr[varKey].aSysVarValidSet,   // Ptr to validate set function
                                  lpSymEntry,                               // Ptr to sysvar SYMENTRY
                                  lptkFunc);                                // Ptr to function token
} // End VariantValidateSymVal_EM


//***************************************************************************
//  $PrimOpVariantValKeyGlb
//
//  Validate a keyword in a global memory handle
//***************************************************************************

VARIANTKEYS PrimOpVariantValKeyGlb
    (HGLOBAL hGlbKey)                   // Keyword global memory handle

{
    VARIANTKEYS       varKey;               // Loop counter and the result
    LPVARARRAY_HEADER lpMemHdrKey = NULL;   // Ptr to key header
    LPAPLCHAR         lpMemKey;             // Ptr to key global memory data

    // Lock the memory to get a ptr to it
    lpMemHdrKey = MyGlobalLockVar (hGlbKey);

    // Skip over header and dimensions to the data
    lpMemKey = VarArrayDataFmBase (lpMemHdrKey);

    // Compare the keyword with valid ones
    for (varKey = 0; varKey < countof (aVariantKeyStr); varKey++)
    if (lstrcmpW (lpMemKey, aVariantKeyStr[varKey].lpKeyName) EQ 0)
        break;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbKey); lpMemHdrKey = NULL;

    return varKey;
} // End PrimOpVariantValKeyGlb


//***************************************************************************
//  End of File: po_variant.c
//***************************************************************************
