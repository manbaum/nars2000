//***************************************************************************
//  NARS2000 -- Primitive Operator -- Dot
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
#include "headers.h"
#include "gsl/gsl_matrix_double.h"
#include "gsl/gsl_permutation.h"
#include "gsl/gsl_linalg.h"
#include "arb_mat.h"
#include "acb_mat.h"


//***************************************************************************
//  $PrimOpDot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Dot ("determinant" and "inner product")
//***************************************************************************

LPPL_YYSTYPE PrimOpDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if niladic/monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token (may be NULL if niladic)

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DOT);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return
          PrimOpMonDot_EM_YY (lpYYFcnStrOpr,    // Ptr to operator function strand
                              lptkRhtArg);      // Ptr to right arg (may be NULL if niladic)
    else
        return
          PrimOpDydDot_EM_YY (lptkLftArg,       // Ptr to left arg token
                              lpYYFcnStrOpr,    // Ptr to operator function strand
                              lptkRhtArg);      // Ptr to right arg token
} // End PrimOpDot_EM_YY


//***************************************************************************
//  $PrimProtoOpDot_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Dot ("determinant" and "inner product")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token (always NULL)

{
    Assert (lptkAxis EQ NULL);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return
          PrimOpMonDotCommon_EM_YY (lpYYFcnStrOpr,          // Ptr to operator function strand
                                    lptkRhtArg,             // Ptr to right arg token
                                    TRUE);                  // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return
          PrimOpDydDotCommon_EM_YY (lptkLftArg,             // Ptr to left arg token (may be NULL if monadic)
                                    lpYYFcnStrOpr,          // Ptr to operator function strand
                                    lptkRhtArg,             // Ptr to right arg token
                                    TRUE);                  // TRUE iff prototyping
} // End PrimProtoOpDot_EM_YY


//***************************************************************************
//  $PrimIdentOpDot_EM_YY
//
//  Generate an identity element for the dyadic derived function from the
//    primitive operator Dot
//***************************************************************************

LPPL_YYSTYPE PrimIdentOpDot_EM_YY
    (LPTOKEN      lptkRhtOrig,          // Ptr to original right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)          // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht,         // Ptr to right ...
                 lpYYRes;               // Ptr to result
    HGLOBAL      hGlbMFO;               // Magic function/operator global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    UBOOL        bNegate = FALSE;       // TRUE iff we should negate the result

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

    // The (right) identity function for dyadic Dot
    //   (L f.g R) ("inner product") is
    //   for +.{times} is
    //   ({iota}{neg}1{take}{rho} R){jot}.={iota}{neg}1{take}{rho} R.

    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, lptkAxisOpr               ); Assert (lpYYFcnStrRht NE NULL);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrOpr, lptkAxisOpr, lpYYFcnStrRht); Assert (lpYYFcnStrLft NE NULL);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Pick off the inner products we know how to handle
    if (lpYYFcnStrLft->TknCount EQ 1
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && lpYYFcnStrRht->TknCount EQ 1
     && lpYYFcnStrRht->tkToken.tkFlags.TknType EQ TKT_FCNIMMED)
    {
        APLCHAR aplCharLft,
                aplCharRht;

        // Get the left & right single char functions
        aplCharLft = lpYYFcnStrLft->tkToken.tkData.tkChar;
        aplCharRht = lpYYFcnStrRht->tkToken.tkData.tkChar;

        // Check for +.{times}
        if (aplCharLft EQ UTF16_PLUS
         && aplCharRht EQ UTF16_TIMES)
            goto IDENT1;

        // Check for {or}.^
        if (aplCharLft EQ UTF16_DOWNCARET
         && IsAPLCharUpCaret (aplCharRht))
            goto IDENT1;

        // Check for ^.{ge}
        if (IsAPLCharUpCaret (aplCharLft)
         && IsAPLCharNotLess (aplCharRht))
            goto IDENT1;

        // Check for =.{ge}
        if (aplCharLft EQ UTF16_EQUAL
         && IsAPLCharNotLess (aplCharRht))
            goto IDENT1;

        // Check for {ne}.^
        if (aplCharLft EQ UTF16_NOTEQUAL
         && IsAPLCharUpCaret (aplCharRht))
            goto IDENT1;

        // Check for ^.>
        if (IsAPLCharUpCaret (aplCharLft)
         && aplCharRht EQ UTF16_RIGHTCARET)
            goto IDENT2;

        // Check for ^.{or}
        if (IsAPLCharUpCaret (aplCharLft)
         && aplCharRht EQ UTF16_DOWNCARET)
            goto IDENT2;

        // Check for =.{or}
        if (aplCharLft EQ UTF16_EQUAL
         && aplCharRht EQ UTF16_DOWNCARET)
            goto IDENT2;

        // Check for {ne}.>
        if (aplCharLft EQ UTF16_NOTEQUAL
         && aplCharRht EQ UTF16_RIGHTCARET)
            goto IDENT2;

        goto NONCE_EXIT;
    } else
        goto NONCE_EXIT;
IDENT2:
    bNegate = TRUE;
IDENT1:
    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the magic function/operator global memory handles
    hGlbMFO = lpMemPTD->hGlbMFO[MFOE_IdnDot];

    lpYYRes =
      ExecuteMagicFunction_EM_YY (NULL,                     // Ptr to left arg token (may be NULL)
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  lptkRhtArg,               // Ptr to right arg token
                                  lptkAxisOpr,              // Ptr to axis token
                                  hGlbMFO,                  // Magic function/operator global memory handle
                                  NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ID);              // Starting line # type (see LINE_NUMS)
    if (bNegate)
    {
        HGLOBAL           hGlbRes;              // Result global memory handle
        LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
        LPAPLBOOL         lpMemRes;             // Ptr to result global memory
        APLNELM           aplNELMRes,           // Result NELM in bits
                          aplNELMBytes;         // ...            bytes
        APLRANK           aplRankRes;           // ...    rank
        APLUINT           uRes;                 // Loop counter

        // Get the result global ptr
        GetGlbPtrs_LOCK (&lpYYRes->tkToken, &hGlbRes, &lpMemHdrRes);

        // Get the NELM & rank
        aplNELMRes = lpMemHdrRes->NELM;
        aplRankRes = lpMemHdrRes->Rank;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

        // Round up the NELM (in bits) to bytes to use as a loop counter
        aplNELMBytes = RoundUpBitsToBytes (aplNELMRes);

        // Loop through the result bytes
        for (uRes = 0; uRes < aplNELMBytes; uRes++)
            // Negate each byte
            *lpMemRes++ ^= 0xFF;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End PrimIdentOpDot_EM_YY


//***************************************************************************
//  Magic function/operator for identity function from the
//    inner product operator
//***************************************************************************

#include "mf_dot.h"


//***************************************************************************
//  $PrimOpMonDot_EM_YY
//
//  Primitive operator for monadic derived function from Dot ("determinant")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDot_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return
      PrimOpMonDotCommon_EM_YY (lpYYFcnStrOpr,          // Ptr to operator function strand
                                lptkRhtArg,             // Ptr to right arg token (may be NULL if niladic)
                                FALSE);                 // TRUE iff prototyping
} // End PrimOpMonDot_EM_YY


//***************************************************************************
//  $PrimOpMonDotCommon_EM_YY
//
//  Primitive operator for monadic derived function from Dot ("determinant")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDotCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)             // TRUE iff prototyping

{
    LPPLLOCALVARS       lpplLocalVars;          // Ptr to re-entrant vars
    LPUBOOL             lpbCtrlBreak;           // Ptr to Ctrl-Break flag
    LPPL_YYSTYPE        lpYYFcnStrLft,          // Ptr to left operand function strand
                        lpYYFcnStrRht;          // Ptr to right ...
    LPTOKEN             lptkAxisOpr,            // Ptr to axis token (may be NULL)
                        lptkFunc;               // ...    function token
    APLSTYPE            aplTypeRht,             // Right arg storage type
                        aplTypeRes;             // Result    ...
    APLNELM             aplNELMRht;             // Right arg NELM
    APLRANK             aplRankRht;             // Right arg rank
    APLLONGEST          aplLongestRht;          // Immediate right arg
    HGLOBAL             hGlbRht = NULL,         // Right arg global memory handle
                        lpSymGlbRht,            // Ptr to right arg global numeric
                        hGlbArb = NULL,         // Temp global memory handle for ARBs
                        hGlbTmp = NULL;         // ...                           RATs
    LPVARARRAY_HEADER   lpMemHdrRht = NULL,     // Ptr to right arg header
                        lpMemHdrRes = NULL;     // ...
    LPVOID              lpMemRht,               // Ptr to right arg global memory
                        lpMemRes;               // ...    result    ...
    LPAPLRAT            lpMemTmp;               // Ptr to temp global memory
    LPAPLARB            lpMemArb = NULL;        // ...
    LPPL_YYSTYPE        lpYYRes = NULL;         // Ptr to the result
    APLUINT             ByteRes;                // # bytes in the result
    APLINT              apaOffRht,              // Right arg APA offset
                        apaMulRht,              // ...           multiplier
                        aplIntegerRht;          // Right arg temporary integer
    APLFLOAT            aplFloatRht;            // Right arg temporary float
    APLDIM              aplDimRows,             // # rows in the right arg
                        aplDimCols,             // # cols ...
                        uRow,                   // Loop counter
                        uCol;                   // ...
    UINT                uBitMask;               // Bit mask for marching through Booleans
    IMM_TYPES           immTypeRes;             // Immediate result storage type
    APLRAT              aplRatRes = {0};        // Temp result if RAT
    APLVFP              aplVfpRes = {0};        // ...            VFP
    APLARB              aplArbRes = {0};        // ...            ARB
    UBOOL               bMaxFcn,                // TRUE iff the left operand is max in ??.+
                        bRet,                   // TRUE iff the result is valid
                        bMinMaxAfo = FALSE;     // TRUE iff the left operand is a min/max AFO
    gsl_matrix         *lpGslMatrixU = NULL;    // GSL temporary
    gsl_matrix_complex *lpGslCMatrixU = NULL;   // ...
    gsl_complex         gslHC2FRes;             // ...
    gsl_permutation    *lpGslPermP = NULL;      // ...
    int                 ErrCode,                // Error code from gsl_* functions
                        iSizeofRes,             // Sizeof an item
                        iHCDimRes,              // HC Dimension (1, 2, 4, 8)
                        signum;                 // Sign of the LU permutation
    APLHC8F             aplHC8FRes;             // Temp result
    APLHC8R             aplHC8RRes;             // ...
    APLHC8V             aplHC8VRes;             // ...
    APLBA8F             aplBA8FRes;             // ...

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for axis operator
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, lptkAxisOpr               ); Assert (lpYYFcnStrRht NE NULL);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrOpr, lptkAxisOpr, lpYYFcnStrRht); Assert (lpYYFcnStrLft NE NULL);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Set the function token ptr
    lptkFunc = &lpYYFcnStrOpr->tkToken;

    // If we're called niladically, ...
    if (lptkRhtArg EQ NULL)
        goto VALENCE_EXIT;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Check for RANK ERROR
    if (IsRank3P (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Calculate the # rows & cols in the result
    switch (aplRankRht)
    {
        case 0:
            aplDimRows =
            aplDimCols = 1;

            break;

        case 1:
            aplDimRows = (VarArrayBaseToDim (lpMemHdrRht))[0];
            aplDimCols = 1;

            break;

        case 2:
            aplDimRows = (VarArrayBaseToDim (lpMemHdrRht))[0];
            aplDimCols = (VarArrayBaseToDim (lpMemHdrRht))[1];

            break;

        defstop
            break;
    } // End SWITCH

    // Check for multi-element vector
    if (IsVector (aplRankRht))
        goto GENERAL_DET;

    // If the left operand is the immediate function (+), ...
    // If the right operand is the immediate function (x), ...
    if (IsTknImmed (&lpYYFcnStrLft->tkToken)
     && lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_PLUS
     && IsTknImmed (&lpYYFcnStrRht->tkToken)
     && lpYYFcnStrRht->tkToken.tkData.tkChar EQ UTF16_TIMES)
    {
        HGLOBAL hGlbMFO;

        //***************************************************************
        // From here on, the derived function is +.x
        //***************************************************************

        // Get the magic function/operator global memory handle
        hGlbMFO = GetMemPTD ()->hGlbMFO[MFOE_DetPerm];

        lpYYRes =
          ExecuteMagicFunction_EM_YY (NULL,                     // Ptr to left arg token (may be NULL)
                                     &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                      lpYYFcnStrOpr,            // Ptr to function strand
                                      lptkRhtArg,               // Ptr to right arg token
                                      lptkAxisOpr,              // Ptr to axis token
                                      hGlbMFO,                  // Magic function/operator global memory handle
                                      NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                      LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
        goto NORMAL_EXIT;
    } // End IF

    // Check for non-square matrix or multi-element vector
    if (aplDimRows NE aplDimCols)
        goto GENERAL_DET;

    // Calculate the result storage type
    if (IsSimpleNum (aplTypeRht))
        aplTypeRes = ARRAY_FLOAT;
    else
    // If the right arg is HCxI, ...
    if (IsHCxI (aplTypeRht))
        // The result is HCxF
        aplTypeRes = aplTypeRht + 1;    // *** ASSUME*** -- order of ARRAY_TYPES as in I++ == F
    else
        aplTypeRes = aplTypeRht;

    // Calculate the item size and HC Dimension
    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);
    iHCDimRes  = TranslateArrayTypeToHCDim  (aplTypeRes);

    // If the right arg is not immediate, ...
    if (lpMemHdrRht NE NULL)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        // Point to the data
        lpMemRht = &aplLongestRht;

    // If the right arg is a singleton scalar/vector/matrix,
    //   the result is an immediate or scalar global numeric
    if (!IsRank3P (aplRankRht)
      && IsSingleton (aplDimRows))
    {
        IMM_TYPES immTypeRes;
        HGLOBAL   hGlbMFO;

        // According to Reduction of Singletons, the result is actually
        //   LO/RO/,R  where LO is the left operand and RO is the right operand.

        // Get the magic function/operator global memory handle
        hGlbMFO = GetMemPTD ()->hGlbMFO[MFOE_DetSing];

        // Calculate LO/RO/,R
        lpYYRes =
          ExecuteMagicOperator_EM_YY (NULL,                     // Ptr to left arg token (may be NULL)
                                     &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                      lpYYFcnStrLft,            // Ptr to left operand function strand
                                      lpYYFcnStrOpr,            // Ptr to function strand
                                      lpYYFcnStrRht,            // Ptr to right operand function strand (may be NULL)
                                      lptkRhtArg,               // Ptr to right arg token
                                      NULL,                     // Ptr to axis token (may be NULL)
                                      hGlbMFO,                  // Magic function/operator global memory handle
                                      NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                      LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
        // Check for error
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;

        // Get the type of the result
        AttrsOfToken (&lpYYRes->tkToken, &aplTypeRes, NULL, NULL, NULL);

        // Get the first value from a token into aplFloatRht or lpSymGlbRht
        GetFirstValueToken (&lpYYRes->tkToken,  // Ptr to right arg token
                            &aplIntegerRht,     // Ptr to integer result
                            &aplFloatRht,       // Ptr to float ...
                             NULL,              // Ptr to WCHAR ...
                             NULL,              // Ptr to longest ...
                            &lpSymGlbRht,       // Ptr to lpSym/Glb ...
                             NULL,              // Ptr to ...immediate type ...
                             NULL);             // Ptr to array type ...
        // YYFree the YYRes
        YYFree (lpYYRes); lpYYRes = NULL;

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
////////lpYYRes->tkToken.tkFlags.TknType   =            // Filled in below
////////lpYYRes->tkToken.tkFlags.ImmType   =            // Filled in below
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkData.tkFloat    =            // Filled in below
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Fill in the result token
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkData.tkInteger  = aplIntegerRht;

                break;

            case ARRAY_FLOAT:
                // Fill in the result token
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkData.tkFloat    = aplFloatRht;

                break;

            case ARRAY_HC2I:
            case ARRAY_HC4I:
            case ARRAY_HC8I:

            case ARRAY_HC2F:
            case ARRAY_HC4F:
            case ARRAY_HC8F:

            case ARRAY_RAT:
            case ARRAY_HC2R:
            case ARRAY_HC4R:
            case ARRAY_HC8R:

            case ARRAY_VFP:
            case ARRAY_HC2V:
            case ARRAY_HC4V:
            case ARRAY_HC8V:

            case ARRAY_ARB:
            case ARRAY_BA2F:
            case ARRAY_BA4F:
            case ARRAY_BA8F:
                // Set the immediate type
                immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);

                // Save in the result
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
                lpYYRes->tkToken.tkData.tkGlbData  =
                  MakeGlbEntry_EM (aplTypeRes,
                                   lpSymGlbRht,
                                   FALSE,               // Note that lpSymGlbRht came from the calculation of LO/RO/,R
                                                        // This is my way of freeing it
                                   lptkFunc);
                // Check for errors
                if (lpYYRes->tkToken.tkData.tkGlbData EQ NULL)
                    goto WSFULL_EXIT;
                break;

            defstop
                break;
        } // End SWITCH

        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is a square matrix
    //***************************************************************

    // If this is Min.Plus  or  Max.Plus, ...
    if (IsTknImmed (&lpYYFcnStrRht->tkToken)
     && lpYYFcnStrRht->tkToken.tkData.tkChar EQ UTF16_PLUS
     && IsTknImmed (&lpYYFcnStrLft->tkToken))
    {
        // Determine if the left immediate operand is Max
        bMaxFcn = (lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_UPSTILE);

        // If it's either Max or Min, ...
        if (bMaxFcn
         || lpYYFcnStrLft->tkToken.tkData.tkChar EQ UTF16_DOWNSTILE)
            // Handle specially
            goto MINDOTPLUS;
    } // End IF

    // If this is MinMaxAfo.Comma, ...
    if (IsTknImmed (&lpYYFcnStrRht->tkToken)
     && lpYYFcnStrRht->tkToken.tkData.tkChar EQ UTF16_COMMA
     && (bMinMaxAfo = IzitMinMaxAfo (lpYYFcnStrLft, &bMaxFcn)))
        // Handle specially
        goto MINDOTPLUS;

    // If the left operand is NOT the immediate function (-), ...
    if (!IsTknImmed (&lpYYFcnStrLft->tkToken)
     || !IsAPLCharBar (lpYYFcnStrLft->tkToken.tkData.tkChar))
        // Handle as a generalized determinant
        goto GENERAL_DET;

    // If the right operand is NOT the immediate function (x), ...
    if (!IsTknImmed (&lpYYFcnStrRht->tkToken)
     || lpYYFcnStrRht->tkToken.tkData.tkChar NE UTF16_TIMES)
        // Handle as a generalized determinant
        goto GENERAL_DET;

    //***************************************************************
    // From here on, the derived function is -.x
    //***************************************************************

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    Assert (aplDimRows EQ (APLU3264) aplDimRows);
    Assert (aplDimCols EQ (APLU3264) aplDimCols);

    // Copy the right arg to the GSL matrix U
    // The scalar case is handled above, and the vector
    //   and matrix cases are handled the same

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
            // Allocate space for the GSL matrices
            lpGslMatrixU = gsl_matrix_alloc      ((APLU3264) aplDimRows, (APLU3264) aplDimCols);    // M x N
            lpGslPermP   = gsl_permutation_alloc ((APLU3264) aplDimRows);                           // M

            // Check the return codes for the above allocations
            if (GSL_ENOMEM EQ (HANDLE_PTR) lpGslMatrixU
             || GSL_ENOMEM EQ (HANDLE_PTR) lpGslPermP)
                goto WSFULL_EXIT;

            // Copy the right arg to the GSL matrix U
            // Split cases based upon the right arg's rank
            switch (aplRankRht)
            {
////////////////case 0:         // Scalar
////////////////    lpGslMatrixU->data[0] = aplFloatRht;
////////////////
////////////////    break;
////////////////
                case 1:         // Vector
                case 2:         // Matrix
                    // Split cases based upon the right arg storage type
                    switch (aplTypeRht)
                    {
                        case ARRAY_BOOL:
                            uBitMask = BIT0;

                            // Loop through the entire right arg
                            for (uCol = 0; uCol < aplNELMRht; uCol++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                lpGslMatrixU->data[uCol] = (uBitMask & *(LPAPLBOOL) lpMemRht) ? TRUE : FALSE;

                                // Shift over the bit mask
                                uBitMask <<= 1;

                                // Check for end-of-byte
                                if (uBitMask EQ END_OF_BYTE)
                                {
                                    uBitMask = BIT0;            // Start over
                                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                                } // End IF
                            } // End FOR

                            break;

                        case ARRAY_INT:
                            // Loop through the entire right arg
                            for (uCol = 0; uCol < aplNELMRht; uCol++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                lpGslMatrixU->data[uCol] = (APLFLOAT) ((LPAPLINT) lpMemRht)[uCol];
                            } // End FOR

                            break;

                        case ARRAY_FLOAT:
                            // Calculate space needed to copy the right arg
                            ByteRes = aplDimRows * aplDimCols * sizeof (APLFLOAT);

                            // Check for overflow
                            if (ByteRes NE (APLU3264) ByteRes)
                                goto WSFULL_EXIT;

                            CopyMemory (lpGslMatrixU->data, lpMemRht, (APLU3264) ByteRes);

////////////////////////////// Loop through the entire right arg
////////////////////////////for (uCol = 0; uCol < aplNELMRht; uCol++)
////////////////////////////{
////////////////////////////    // Check for Ctrl-Break
////////////////////////////    if (CheckCtrlBreak (lpbCtrlBreak))
////////////////////////////        goto ERROR_EXIT;
////////////////////////////
////////////////////////////    lpGslMatrixU->data[uCol] = ((LPAPLFLOAT) lpMemRht)[uCol];
////////////////////////////} // End FOR

                            break;

                        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
                            // Get the APA parameters
                            apaOffRht = lpAPA->Off;
                            apaMulRht = lpAPA->Mul;
#undef  lpAPA
                            // Loop through the entire right arg
                            for (uCol = 0; uCol < aplNELMRht; uCol++)
                            {
                                // Check for Ctrl-Break
                                if (CheckCtrlBreak (lpbCtrlBreak))
                                    goto ERROR_EXIT;

                                lpGslMatrixU->data[uCol] = (APLFLOAT) (APLINT) (apaOffRht + apaMulRht * uCol);
                            } // End FOR

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            // Calculate the LU (Lower/Upper) Decomposition of the square matrix U
            //   with the result in U and P (P is a temporary used by LU decomp only)
            ErrCode =
              gsl_linalg_LU_decomp (lpGslMatrixU,       // N x N
                                    lpGslPermP,         // N
                                   &signum);            // Ptr to sign of the perm
            // Check the error code
            if (ErrCode NE GSL_SUCCESS)
                goto RIGHT_DOMAIN_EXIT;

            // Calculate the determinant of the LU decomposition
            aplFloatRht =
              gsl_linalg_LU_det (lpGslMatrixU,          // N x N
                                 signum);               // Sign of the perm
            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
            lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkFloat    = aplFloatRht;
            lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

            break;

        case ARRAY_RAT:
            // Allocate temporary space for a copy of the right arg
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) aplNELMRht * sizeof (APLRAT));
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemTmp = MyGlobalLock000 (hGlbTmp);

            // Loop through the entire right arg
            for (uCol = 0; uCol < aplNELMRht; uCol++)
                // Initialize the matrix to 0/1
                mpq_init_set (&            lpMemTmp [uCol],
                              &((LPAPLRAT) lpMemRht)[uCol]);
            // Use Gauss-Jordan elimination to calculate the determinant (into aplRatRes)
            if (!GaussJordanDetHC1R (lpMemTmp, aplDimRows, &aplRatRes, lpbCtrlBreak))
                goto RIGHT_DOMAIN_EXIT;

            // Point to the result
            lpMemRes = &aplRatRes;

            // Set the immediate type
            immTypeRes = IMMTYPE_RAT;

            goto YYALLOC_EXIT;

        case ARRAY_VFP:
            // Allocate a temp array to hold the VFPs when converted to RATs
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) (aplDimRows * aplDimCols * sizeof (APLRAT)));
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemTmp = MyGlobalLock000 (hGlbTmp);

            // Loop through the entire right arg
            for (uCol = 0; uCol < aplNELMRht; uCol++)
            {
                // Initialize the entry
                mpq_init   (&lpMemTmp[uCol]);

                // Copy and convert the entry
                mpq_set_fr (&            lpMemTmp [uCol],
                            &((LPAPLVFP) lpMemRht)[uCol]);
            } // End FOR

            // Use Gauss-Jordan elimination to calculate the determinant (into aplRatRes)
            if (!GaussJordanDetHC1R (lpMemTmp, aplDimRows, &aplRatRes, lpbCtrlBreak))
                goto RIGHT_DOMAIN_EXIT;

            // Initialize the VFP result to 0
            mpfr_init0 (&aplVfpRes);

            // Convert the result (in lpMemTmp) back to VFP
            mpfr_set_q (&aplVfpRes,
                        &aplRatRes,
                         MPFR_RNDN);
            // We no longer need this resource
            Myq_clear (&aplRatRes);

            // Point to the result
            lpMemRes = &aplVfpRes;

            // Set the immediate type
            immTypeRes = IMMTYPE_VFP;

            goto YYALLOC_EXIT;

        case ARRAY_ARB:
        {
            APLDIM    uItm;         // Loop counter
            arb_mat_t A;            // The input
            mp_limb_signed_t prec = ARB_PREC_FPC;

            Assert (aplDimRows EQ (slong) aplDimRows);
            Assert (aplDimCols EQ (slong) aplDimCols);

            // Initialize
            arb_init     (&aplArbRes);
            arb_mat_init (A, (slong) aplDimRows, (slong) aplDimCols);

            // Loop through the rows & cols of the right arg
            for (uItm = uRow = 0; uRow < aplDimRows; uRow++)
            for (       uCol = 0; uCol < aplDimCols; uCol++)
                // Copy the right arg to the matrix A
                arb_set (arb_mat_entry (A, uRow, uCol), &((LPAPLARB) lpMemRht)[uItm++]);

            // Calculate the determinant
            arb_mat_det (&aplArbRes, A, prec);

            // We no longer need this storage
            arb_mat_clear (A);

            // Point to the result
            lpMemRes = &aplArbRes;

            // Set the immediate type
            immTypeRes = IMMTYPE_ARB;

            goto YYALLOC_EXIT;
        } // End ARRAY_BA2F

////////////// Allocate a temp array to hold the ARBs
////////////hGlbArb = DbgGlobalAlloc (GHND, (APLU3264) (aplDimRows * aplDimCols * sizeof (APLARB)));
////////////if (hGlbArb EQ NULL)
////////////    goto WSFULL_EXIT;
////////////
////////////// Lock the memory to get a ptr to it
////////////lpMemArb = MyGlobalLock000 (hGlbArb);
////////////
////////////// Loop through the entire right arg
////////////for (uCol = 0; uCol < aplNELMRht; uCol++)
////////////    // Copy the entry
////////////    arb_set (&lpMemArb[uCol], &((LPAPLARB) lpMemRht)[uCol]);
////////////
////////////// Use Gauss-Jordan elimination to calculate the determinant (into aplRatRes)
////////////if (!GaussJordanDetBA1F (lpMemArb, aplDimRows, &aplArbRes, lpbCtrlBreak))
////////////    goto RIGHT_DOMAIN_EXIT;
////////////
////////////// Point to the result
////////////lpMemRes = &aplArbRes;
////////////
////////////// Set the immediate type
////////////immTypeRes = IMMTYPE_ARB;
////////////
////////////goto YYALLOC_EXIT;

        case ARRAY_HC2I:
        case ARRAY_HC2F:
            // Allocate space for a copy of the right arg
            lpGslCMatrixU = gsl_matrix_complex_alloc ((APLU3264) aplDimRows, (APLU3264) aplDimCols);   // M x N
            lpGslPermP    = gsl_permutation_alloc    ((APLU3264) aplDimRows);                          // M

            // Calculate space needed to copy the right arg
            ByteRes = aplDimRows * aplDimCols * sizeof (APLHC2F);

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_HC2I:
                    // Loop through the entire right arg
                    for (uCol = 0; uCol < (2 * aplNELMRht); uCol++)
                        // Convert the HC2I to HC2F
                        lpGslCMatrixU->data[uCol] = (APLFLOAT) ((LPAPLINT) lpMemRht)[uCol];

                    break;

                case ARRAY_HC2F:
                    // Copy the right arg to the GSL temp
                    CopyMemory (lpGslCMatrixU->data, lpMemRht, (APLU3264) ByteRes);

                    break;

                defstop
                    break;
            } // End SWITCH

            // Calculate the LU (Lower/Upper) Decomposition of the square matrix U
            //   with the result in U and P (P is a temporary used by LU decomp only)
            ErrCode =
              gsl_linalg_complex_LU_decomp (lpGslCMatrixU,  // N x N
                                            lpGslPermP,     // N
                                           &signum);        // Ptr to sign of the perm
            // Check the error code
            if (ErrCode NE GSL_SUCCESS)
                goto RIGHT_DOMAIN_EXIT;

            // Calculate the determinant of the LU decomposition
            gslHC2FRes =
              gsl_linalg_complex_LU_det (lpGslCMatrixU,     // N x N
                                         signum);           // Sign of the perm
            // Point to the result
            lpMemRes = &gslHC2FRes.dat;

            // Set the immediate type
            immTypeRes = IMMTYPE_HC2F;

            // Free the temps
            gsl_matrix_complex_free (lpGslCMatrixU); lpGslCMatrixU = NULL;
            gsl_permutation_free (lpGslPermP); lpGslPermP = NULL;

            goto YYALLOC_EXIT;

        case ARRAY_HC4I:
        case ARRAY_HC4F:
        case ARRAY_HC8I:
        case ARRAY_HC8F:
            // Allocate a temp array to hold a copy of the right arg (possibly converted)
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) (aplDimRows * aplDimCols * iSizeofRes));
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemTmp = MyGlobalLock000 (hGlbTmp);

            // Split cases based upon the right arg storage type
            switch (aplTypeRht)
            {
                case ARRAY_HC4I:
                case ARRAY_HC8I:
                    // Loop through the entire right arg
                    for (uCol = 0; uCol < (iHCDimRes * aplNELMRht); uCol++)
                        // Convert the HC2I to HC2F
                        ((LPAPLFLOAT) lpMemTmp)[uCol] = (APLFLOAT) ((LPAPLINT) lpMemRht)[uCol];

                    break;

                case ARRAY_HC4F:
                case ARRAY_HC8F:
                    // Copy the right arg to the temp
                    CopyMemory (lpMemTmp, lpMemRht, (APLU3264) (aplDimRows * aplDimCols * iSizeofRes));

                    break;

                defstop
                    break;
            } // End SWITCH

            // Use Gauss-Jordan elimination to calculate the determinant (into aplHC8FRes)
            bRet = GaussJordanDetHCxF ((LPVOID) lpMemTmp, aplDimRows, (LPVOID) &aplHC8FRes, iHCDimRes, lpbCtrlBreak);

            // Free the temp
            UnlFreeGlbName (hGlbTmp, lpMemTmp);

            // Point to the result
            lpMemRes = &aplHC8FRes;

            // Set the immediate type
            immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);

            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;

            goto YYALLOC_EXIT;

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            // Allocate a temp array to hold a copy of the right arg
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) (aplDimRows * aplDimCols * iSizeofRes));
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemTmp = MyGlobalLock000 (hGlbTmp);

            // Loop through the entire right arg
            for (uCol = 0; uCol < (iHCDimRes * aplNELMRht); uCol++)
                // Convert the next item
                mpq_init_set (&((LPAPLRAT) lpMemTmp)[uCol], &((LPAPLRAT) lpMemRht)[uCol]);

            // Use Gauss-Jordan elimination to calculate the determinant (into aplHC8RRes)
            bRet = GaussJordanDetHCxR ((LPVOID) lpMemTmp, aplDimRows, (LPVOID) &aplHC8RRes, iHCDimRes, lpbCtrlBreak);

            // Free the temp
            UnlFreeGlbName (hGlbTmp, lpMemTmp);

            // Point to the result
            lpMemRes = &aplHC8RRes;

            // Set the immediate type
            immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);

            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;

            goto YYALLOC_EXIT;

        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Allocate a temp array to hold a copy of the right arg
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) (aplDimRows * aplDimCols * iSizeofRes));
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemTmp = MyGlobalLock000 (hGlbTmp);

            // Loop through the entire right arg
            for (uCol = 0; uCol < (iHCDimRes * aplNELMRht); uCol++)
                // Copy the next item
                mpfr_init_set (&((LPAPLVFP) lpMemTmp)[uCol], &((LPAPLVFP) lpMemRht)[uCol], MPFR_RNDN);

            // Use Gauss-Jordan elimination to calculate the determinant (into aplHC8VRes)
            bRet = GaussJordanDetHCxV ((LPVOID) lpMemTmp, aplDimRows, (LPVOID) &aplHC8VRes, iHCDimRes, lpbCtrlBreak);

            // Free the temp
            UnlFreeGlbName (hGlbTmp, lpMemTmp);

            // Point to the result
            lpMemRes = &aplHC8VRes;

            // Set the immediate type
            immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);

            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;

            goto YYALLOC_EXIT;

        case ARRAY_BA2F:
        {
            APLDIM    uItm;         // Loop counter
            acb_mat_t A;            // The input
            acb_t     aplAcbRes = {0};
            mp_limb_signed_t prec = ARB_PREC_FPC;

            Assert (aplDimRows EQ (slong) aplDimRows);
            Assert (aplDimCols EQ (slong) aplDimCols);

            // Initialize
            acb_init     (aplAcbRes);
            acb_mat_init (A, (slong) aplDimRows, (slong) aplDimCols);

            // Loop through the rows & cols of the right arg
            for (uItm = uRow = 0; uRow < aplDimRows; uRow++)
            for (       uCol = 0; uCol < aplDimCols; uCol++)
                // Copy the right arg to the matrix A
                acb_set (acb_mat_entry (A, uRow, uCol), (const acb_struct *) &((LPAPLBA2F) lpMemRht)[uItm++]);

            // Calculate the determinant
            acb_mat_det (aplAcbRes, A, prec);

            // We no longer need this storage
            acb_mat_clear (A);

            // Point to the result
            lpMemRes = &aplAcbRes;

            // Set the immediate type
            immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);

            goto YYALLOC_EXIT;
        } // End ARRAY_BA2F

        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // Allocate a temp array to hold a copy of the right arg
            hGlbTmp = DbgGlobalAlloc (GHND, (APLU3264) (aplDimRows * aplDimCols * iSizeofRes));
            if (hGlbTmp EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemTmp = MyGlobalLock000 (hGlbTmp);

            // Loop through the entire right arg
            for (uCol = 0; uCol < (iHCDimRes * aplNELMRht); uCol++)
                // Copy the next item
                arb_init_set (&((LPAPLARB) lpMemTmp)[uCol], &((LPAPLARB) lpMemRht)[uCol]);

            // Use Gauss-Jordan elimination to calculate the determinant (into aplBA8FRes)
            bRet = GaussJordanDetBAxF ((LPVOID) lpMemTmp, aplDimRows, (LPVOID) &aplBA8FRes, iHCDimRes, lpbCtrlBreak);

            // Free the temp
            UnlFreeGlbName (hGlbTmp, lpMemTmp);

            // Point to the result
            lpMemRes = &aplBA8FRes;

            // Set the immediate type
            immTypeRes = TranslateArrayTypeToImmType (aplTypeRes);

            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;

            goto YYALLOC_EXIT;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

MINDOTPLUS:
{
    LPROWCOL resLong;
    APLUINT  uRes;                      // Loop counter
    APLUINT  ByteRes;                   // # bytes in the result
    HGLOBAL  hGlbRes;                   // result global memory handle

    //***************************************************************
    // From here on, the derived function is {min}.+ or {max}.+ or
    //   MinMaxAfo.,
    //***************************************************************

    // Check for DOMAIN ERROR
    if (!IsNumeric (aplTypeRht)
     || IsHCAny (aplTypeRht))       // Min not defined on HCxy
        goto RIGHT_DOMAIN_EXIT;

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_APA:
        case ARRAY_INT:
            // Set the result type
            aplTypeRes = ARRAY_INT;

            resLong =
              kuhn_start_int  (lpMemRht, (long) aplDimRows, (long) aplDimCols, bMaxFcn, aplTypeRht);

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // If it's MinMaxAfo, ...
            if (bMinMaxAfo)
            {
                LPAPLINT lpMemRes;

                // Calculate space needed for the result
                ByteRes = CalcArraySize (aplTypeRes, aplDimRows, 1);

////////////////// Check for overflow
////////////////if (ByteRes NE (APLU3264) ByteRes)
////////////////    goto WSFULL_EXIT;

                hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                if (hGlbRes EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
                // Fill in the header
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = aplTypeRes;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////lpHeader->SysVar     = 0;           // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = aplDimRows;
                lpHeader->Rank       = 1;
#undef  lpHeader
                // Fill in the dimension
                (VarArrayBaseToDim (lpMemHdrRes))[0] = aplDimRows;

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                // Copy the data to the result
                for (uRes = 0; uRes < aplDimRows; uRes++)
                    lpMemRes[resLong[uRes].col] = GetNextInteger (lpMemRht, aplTypeRht, resLong[uRes].row * aplDimCols + resLong[uRes].col);

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
            } else
            {
                APLINT sumI = 0;

                for (uRes = 0; uRes < aplDimRows; uRes++)
                    sumI += GetNextInteger (lpMemRht, aplTypeRht, resLong[uRes].row * aplDimCols + resLong[uRes].col);

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_INT;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkInteger  = sumI;
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
            } // End IF/ELSE

            // We no longer need this resource
            DbgGlobalFree (resLong); resLong = NULL;

            break;

        case ARRAY_FLOAT:
            // Set the result type
            aplTypeRes = ARRAY_FLOAT;

            resLong =
              kuhn_start_dbl (lpMemRht, (long) aplDimRows, (long) aplDimCols, bMaxFcn);

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // If it's MinMaxAfo, ...
            if (bMinMaxAfo)
            {
                LPAPLFLOAT lpMemRes;

                // Calculate space needed for the result
                ByteRes = CalcArraySize (aplTypeRes, aplDimRows, 1);

////////////////// Check for overflow
////////////////if (ByteRes NE (APLU3264) ByteRes)
////////////////    goto WSFULL_EXIT;

                hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                if (hGlbRes EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
                // Fill in the header
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = aplTypeRes;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////lpHeader->SysVar     = 0;           // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = aplDimRows;
                lpHeader->Rank       = 1;
#undef  lpHeader
                // Fill in the dimension
                (VarArrayBaseToDim (lpMemHdrRes))[0] = aplDimRows;

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                // Copy the data to the result
                for (uRes = 0; uRes < aplDimRows; uRes++)
                    lpMemRes[resLong[uRes].col] = ((LPAPLFLOAT) lpMemRht)[resLong[uRes].row * aplDimCols + resLong[uRes].col];

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
            } else
            {
                APLFLOAT sumF = 0;

                for (uRes = 0; uRes < aplDimRows; uRes++)
                    sumF += ((LPAPLFLOAT) lpMemRht)[resLong[uRes].row * aplDimCols + resLong[uRes].col];

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_FLOAT;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkFloat    = sumF;
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
            } // End IF/ELSE

            // We no longer need this resource
            DbgGlobalFree (resLong); resLong = NULL;

            break;

        case ARRAY_RAT:
            // Set the result type
            aplTypeRes = ARRAY_RAT;

            resLong =
              kuhn_start_rat (lpMemRht, (long) aplDimRows, (long) aplDimCols, bMaxFcn);

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // If it's MinMaxAfo, ...
            if (bMinMaxAfo)
            {
                LPAPLRAT lpMemRes;

                // Calculate space needed for the result
                ByteRes = CalcArraySize (aplTypeRes, aplDimRows, 1);

////////////////// Check for overflow
////////////////if (ByteRes NE (APLU3264) ByteRes)
////////////////    goto WSFULL_EXIT;

                hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                if (hGlbRes EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
                // Fill in the header
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = aplTypeRes;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////lpHeader->SysVar     = 0;           // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = aplDimRows;
                lpHeader->Rank       = 1;
#undef  lpHeader
                // Fill in the dimension
                (VarArrayBaseToDim (lpMemHdrRes))[0] = aplDimRows;

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                // Copy the data to the result
                for (uRes = 0; uRes < aplDimRows; uRes++)
                    mpq_init_set (&lpMemRes[resLong[uRes].col], &((LPAPLRAT) lpMemRht)[resLong[uRes].row * aplDimCols + resLong[uRes].col]);

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
            } else
            {
                APLRAT sumR = {0};

                // Initialize the result to 0/1
                mpq_init (&sumR);

                for (uRes = 0; uRes < aplDimRows; uRes++)
                    mpq_add (&sumR, &sumR, &((LPAPLRAT) lpMemRht)[resLong[uRes].row * aplDimCols + resLong[uRes].col]);

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_RAT;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbEntry_EM (aplTypeRes,
                                                                     &sumR,
                                                                      FALSE,
                                                                     &lpYYFcnStrLft->tkToken);
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
            } // End IF/ELSE

            // We no longer need this resource
            DbgGlobalFree (resLong); resLong = NULL;

            break;

        case ARRAY_VFP:
            // Set the result type
            aplTypeRes = ARRAY_VFP;

            resLong =
              kuhn_start_vfp (lpMemRht, (long) aplDimRows, (long) aplDimCols, bMaxFcn);

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // If it's MinMaxAfo, ...
            if (bMinMaxAfo)
            {
                LPAPLVFP lpMemRes;

                // Calculate space needed for the result
                ByteRes = CalcArraySize (aplTypeRes, aplDimRows, 1);

////////////////// Check for overflow
////////////////if (ByteRes NE (APLU3264) ByteRes)
////////////////    goto WSFULL_EXIT;

                hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                if (hGlbRes EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
                // Fill in the header
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = aplTypeRes;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////lpHeader->SysVar     = 0;           // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = aplDimRows;
                lpHeader->Rank       = 1;
#undef  lpHeader
                // Fill in the dimension
                (VarArrayBaseToDim (lpMemHdrRes))[0] = aplDimRows;

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                // Copy the data to the result
                for (uRes = 0; uRes < aplDimRows; uRes++)
                    mpfr_init_set (&lpMemRes[resLong[uRes].col], &((LPAPLVFP) lpMemRht)[resLong[uRes].row * aplDimCols + resLong[uRes].col], MPFR_RNDN);

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
            } else
            {
                APLVFP sumV = {0};

                // Initialize the result to 0
                mpfr_init0 (&sumV);

                for (uRes = 0; uRes < aplDimRows; uRes++)
                    mpfr_add (&sumV, &sumV, &((LPAPLVFP) lpMemRht)[resLong[uRes].row * aplDimCols + resLong[uRes].col], MPFR_RNDN);

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_VFP;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbEntry_EM (aplTypeRes,
                                                                     &sumV,
                                                                      FALSE,
                                                                     &lpYYFcnStrLft->tkToken);
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
            } // End IF/ELSE

            // We no longer need this resource
            DbgGlobalFree (resLong); resLong = NULL;

            break;

        case ARRAY_ARB:
            // Set the result type
            aplTypeRes = ARRAY_ARB;

            resLong =
              kuhn_start_arb (lpMemRht, (long) aplDimRows, (long) aplDimCols, bMaxFcn);

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // If it's MinMaxAfo, ...
            if (bMinMaxAfo)
            {
                LPAPLARB lpMemRes;

                // Calculate space needed for the result
                ByteRes = CalcArraySize (aplTypeRes, aplDimRows, 1);

////////////////// Check for overflow
////////////////if (ByteRes NE (APLU3264) ByteRes)
////////////////    goto WSFULL_EXIT;

                hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                if (hGlbRes EQ NULL)
                    goto WSFULL_EXIT;

                // Lock the memory to get a ptr to it
                lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
                // Fill in the header
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = aplTypeRes;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////lpHeader->SysVar     = 0;           // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = aplDimRows;
                lpHeader->Rank       = 1;
#undef  lpHeader
                // Fill in the dimension
                (VarArrayBaseToDim (lpMemHdrRes))[0] = aplDimRows;

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

                // Copy the data to the result
                for (uRes = 0; uRes < aplDimRows; uRes++)
                    arb_init_set (&lpMemRes[resLong[uRes].col], &((LPAPLARB) lpMemRht)[resLong[uRes].row * aplDimCols + resLong[uRes].col]);

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
            } else
            {
                APLARB sumV = {0};
                mp_limb_signed_t prec = ARB_PREC_FPC;

                // Initialize the result to 0/0
                Myarb_init (&sumV);

                for (uRes = 0; uRes < aplDimRows; uRes++)
                    arb_add (&sumV, &sumV, &((LPAPLARB) lpMemRht)[resLong[uRes].row * aplDimCols + resLong[uRes].col], prec);

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ARB;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbEntry_EM (aplTypeRes,
                                                                     &sumV,
                                                                      FALSE,
                                                                     &lpYYFcnStrLft->tkToken);
                lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;
            } // End IF/ELSE

            // We no longer need this resource
            DbgGlobalFree (resLong); resLong = NULL;

            break;

        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:

        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:

        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:

        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:

        case ARRAY_BA2F:
        case ARRAY_BA4F:
        case ARRAY_BA8F:
            // These datatypes don't support {min} or {max}
            goto RIGHT_DOMAIN_EXIT;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;
}

YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
    lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkData.tkFloat    =            // Filled in below
    lpYYRes->tkToken.tkData.tkGlbData  =
      MakeGlbEntry_EM (TranslateImmTypeToArrayType (immTypeRes),
                       lpMemRes,
                       FALSE,
                       lptkFunc);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Check for errors
    if (lpYYRes->tkToken.tkData.tkGlbData EQ NULL)
        goto WSFULL_EXIT;

    goto NORMAL_EXIT;

GENERAL_DET:
    {
        LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
        HGLOBAL      hGlbMFO;               // Magic function/operator global memory handle

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Get the magic function/operator global memory handles
        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_MonDot];

        lpYYRes =
          ExecuteMagicOperator_EM_YY (NULL,                     // Ptr to left arg token (may be NULL)
                                     &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                      lpYYFcnStrLft,            // Ptr to left operand function strand
                                      lpYYFcnStrOpr,            // Ptr to function strand
                                      lpYYFcnStrRht,            // Ptr to right operand function strand (may be NULL)
                                      lptkRhtArg,               // Ptr to right arg token
                                      NULL,                     // Ptr to axis token
                                      hGlbMFO,                  // Magic function/operator global memory handle
                                      NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                      bPrototyping
                                    ? LINENUM_PRO
                                    : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
        goto NORMAL_EXIT;
    } // End GENERAL_DET

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

VALENCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpGslPermP NE NULL)
    {
        gsl_permutation_free (lpGslPermP); lpGslPermP = NULL;
    } // End IF

    if (lpGslMatrixU NE NULL)
    {
        gsl_matrix_free (lpGslMatrixU); lpGslMatrixU = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbTmp NE NULL)
    {
        if (lpMemTmp NE NULL)
        {
            // Loop through the rows and cols
            for (uRow = 0; uRow < aplDimRows; uRow++)
            for (uCol = 0; uCol < aplDimCols; uCol++)
                Myq_clear (&lpMemTmp[uRow * aplDimRows + uCol]);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmp); lpMemTmp = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbTmp); hGlbTmp = NULL;
    } // End IF

    if (hGlbArb NE NULL)
    {
        if (lpMemArb NE NULL)
        {
            // Loop through the rows and cols
            for (uRow = 0; uRow < aplDimRows; uRow++)
            for (uCol = 0; uCol < aplDimCols; uCol++)
                Myarb_clear (&lpMemArb[uRow * aplDimRows + uCol]);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbArb); lpMemArb = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbArb); hGlbArb = NULL;
    } // End IF

    return lpYYRes;
} // End PrimOpMonDotCommon_EM_YY


//***************************************************************************
//  $IzitMinMaxAfo
//
//  Determine whether or not the left perand is a MinMaxAfo
//***************************************************************************

UBOOL IzitMinMaxAfo
    (LPPL_YYSTYPE lpYYFcnStrLft,        // Ptr to left operand
     LPUBOOL      lpbMaxFcn)            // Ptr to TRUE iff the Afo is for Max

{
    UBOOL bRet = FALSE;                 // TRUE iff the result is valid

    // Ensure it's a proper AFO
    if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_GLBDFN
     || lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNAFO
     || lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNARRAY)
    {
        HGLOBAL        hGlbDfnHdr;
        LPDFN_HEADER   lpMemDfnHdr;
        LPFCNLINE      lpFcnLines;      // Ptr to array of function line structs (FCNLINE[numFcnLines])
        LPTOKEN_HEADER lpMemTknHdr;     // Ptr to header of tokenized line
        LPTOKEN        lpMemTknLine;    // Ptr to tokenized line
        UINT           uTokenCnt,       // # tokens in the function line
                       uCnt;            // Loop counter
 static TOKEN tkMinMaxAfo[] =
 //        tkFlags         tkSynObj    tkData
        {{{TKT_EOS       , }, 0,                  },   // 00:
         {{TKT_AFOGUARD  , }, 0,                  },   // 01:
         {{TKT_LEFTPAREN , }, 0,                  },   // 02:
         {{TKT_FCNIMMED  , }, 0, (LPSYMENTRY) L'+'},   // 03:
         {{TKT_OP3IMMED  , }, 0, (LPSYMENTRY) L'/'},   // 04:
         {{TKT_VARNAMED  , }, 0,                  },   // 05:  Alpha
         {{TKT_RIGHTPAREN, }, 0,                  },   // 06:
         {{TKT_FCNIMMED  , }, 0, (LPSYMENTRY) L'<'},   // 07:  <  <=  >  >=
         {{TKT_FCNIMMED  , }, 0, (LPSYMENTRY) L'+'},   // 08:
         {{TKT_OP3IMMED  , }, 0, (LPSYMENTRY) L'/'},   // 09:
         {{TKT_VARNAMED  , }, 0,                  },   // 0A:  Omega
         {{TKT_SOS       , }, 0,                  },   // 0B:
         {{TKT_EOS       , }, 0,                  },   // 0C:
         {{TKT_AFORETURN , }, 0,                  },   // 0D:
         {{TKT_VARNAMED  , }, 0,                  },   // 0E:  Alpha
         {{TKT_SOS       , }, 0,                  },   // 0F:
         {{TKT_EOL       , }, 0,                  },   // 10:
         {{TKT_NOP       , }, 0,                  },   // 11:
         {{TKT_VARNAMED  , }, 0,                  },   // 12:  Omega
         {{TKT_SOS       , }, 0,                  },   // 13:
        };

        // Initialize the bMaxFcn flag
        *lpbMaxFcn = FALSE;

        // Get the global memory handle
        hGlbDfnHdr = GetGlbHandle (&lpYYFcnStrLft->tkToken);

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

        // If it's an AFO, ...
        if (lpMemDfnHdr->bAFO)
        {
            // Get ptr to array of function line structs (FCNLINE[numFcnLines])
            lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

            // Get ptr to the starting line's tokens
            lpMemTknHdr = (LPTOKEN_HEADER) ByteAddr (lpMemDfnHdr, lpFcnLines[0].offTknLine);

            // It's a token header
            Assert (lpMemTknHdr->Sig.nature EQ TOKEN_HEADER_SIGNATURE);

            // Get the token count of this line
            uTokenCnt = lpMemTknHdr->TokenCnt;

            // If we have the same # tokens, ...
            if (uTokenCnt EQ countof (tkMinMaxAfo))
            {
                // Skip over the header to the token start
                lpMemTknLine = TokenBaseToStart (lpMemTknHdr);

                // Loop through the tokens verifying the token type
                for (uCnt = 0; uCnt < uTokenCnt; uCnt++)
                if (tkMinMaxAfo[uCnt].tkFlags.TknType NE lpMemTknLine[uCnt].tkFlags.TknType)
                    goto ERROR_EXIT;

                // Check the immediate functions/operators
                if (tkMinMaxAfo[0x03].tkData.tkChar NE lpMemTknLine[0x03].tkData.tkChar
                 || tkMinMaxAfo[0x04].tkData.tkChar NE lpMemTknLine[0x04].tkData.tkChar
                 || tkMinMaxAfo[0x08].tkData.tkChar NE lpMemTknLine[0x08].tkData.tkChar
                 || tkMinMaxAfo[0x09].tkData.tkChar NE lpMemTknLine[0x09].tkData.tkChar)
                    goto ERROR_EXIT;

                // Check the 1st name (s.b. Alpha)
                if (!lpMemTknLine[0x05].tkData.tkSym->stFlags.bIsAlpha)
                {
                    // Check the 1st name (m.b. Omega)
                    if (!lpMemTknLine[0x05].tkData.tkSym->stFlags.bIsOmega
                    // Check the 2nd name (m.b. Alpha)
                     || !lpMemTknLine[0x0A].tkData.tkSym->stFlags.bIsAlpha)
                        goto ERROR_EXIT;

                    // Flip the bit
                    *lpbMaxFcn = 1 - *lpbMaxFcn;
                } else
                // Check the 2nd name (m.b. Omega)
                if (!lpMemTknLine[0x0A].tkData.tkSym->stFlags.bIsOmega)
                    goto ERROR_EXIT;

                // Check the 3rd name (s.b. Alpha)
                if (!lpMemTknLine[0x0E].tkData.tkSym->stFlags.bIsAlpha)
                {
                    // Check the 3rd name (m.b. Omega)
                    if (!lpMemTknLine[0x0E].tkData.tkSym->stFlags.bIsOmega
                    // Check the 4th name (m.b. Alpha)
                     || !lpMemTknLine[0x12].tkData.tkSym->stFlags.bIsAlpha)
                        goto ERROR_EXIT;

                    // Flip the bit
                    *lpbMaxFcn = 1 - *lpbMaxFcn;
                } else
                // Check the 4th name (m.b. Omega)
                if (!lpMemTknLine[0x12].tkData.tkSym->stFlags.bIsOmega)
                    goto ERROR_EXIT;

                // Check the immediate comparison function (s.b. < or <=)
                if (lpMemTknLine[0x07].tkData.tkChar NE UTF16_LEFTCARET
                 && !IsAPLCharNotMore (lpMemTknLine[0x07].tkData.tkChar))
                {
                    // The immediate comparison function must be > or >=
                    if (lpMemTknLine[0x07].tkData.tkChar NE UTF16_RIGHTCARET
                     && !IsAPLCharNotLess (lpMemTknLine[0x07].tkData.tkChar))
                        goto ERROR_EXIT;
                    // Flip the bit
                    *lpbMaxFcn = 1 - *lpbMaxFcn;
                } // End IF

                // Mark as successful
                bRet = TRUE;
            } // End IF
        } // End IF
ERROR_EXIT:
        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF

    return bRet;
} // End IzitMinMaxAfo


//***************************************************************************
//  $GaussJordanDetHC1R
//
//  Perform Gauss-Jordan elimination on a square rational matrix
//***************************************************************************

UBOOL GaussJordanDetHC1R
    (LPAPLRAT lpMemRht,             // Ptr to copy of right arg as APLRAT matrix
     APLDIM   aplDimRows,           // # rows/cols in the right arg
     LPAPLRAT lpMemRes,             // Ptr to result as a single APLRAT (uninitialized)
     LPUBOOL  lpbCtrlBreak)         // Ptr to Ctrl-Break flag

{
    APLRAT   aplRatTmp = {0},       // Temporary RAT
             aplRatXch,             // ...
             aplRatScale = {0};     // Scale factor
    LPAPLRAT lpaplRatDiv;           // Divisor
    APLDIM   uRow,                  // Loop counter
             uCol,                  // ...
             uTmp;                  // ...
    int      sign = 1;              // Sign of the result
    UBOOL    bRet = FALSE;          // TRUE iff the result is valid

    // Initialize the temps
    mpq_init (&aplRatTmp);
    mpq_init (&aplRatScale);

    // Loop through the rows in a copy of the right arg
    for (uRow = 0; uRow < aplDimRows; uRow++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the diagonal element to be used as a divisor
        lpaplRatDiv = &lpMemRht[uRow * aplDimRows + uRow];

        // If it's zero,
        if (mpq_sgn (lpaplRatDiv) EQ 0)
        {
            // Loop through subsequent rows in the same column
            //   looking for a non-zero divisor for scaling
            for (uTmp = uRow + 1; uTmp < aplDimRows; uTmp++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Get the diagonal element to be used as a divisor (if non-zero)
                lpaplRatDiv = &lpMemRht[uTmp * aplDimRows + uRow];

                if (mpq_sgn (lpaplRatDiv) NE 0)
                    break;
            } // End FOR

            // If the divisor is still zero, ...
            if (mpq_sgn (lpaplRatDiv) EQ 0)
            {
                // The result is 0/1
                mpq_init_set_ui (lpMemRes, 0, 1);

                goto NORMAL_EXIT;
            } // End IF

            // Loop through the cols in rows <uTmp> and <uRow>
            for (uCol = 0; uCol < aplDimRows; uCol++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Exchange the two rows <uTmp> and <uRow>
                aplRatXch = lpMemRht[uTmp * aplDimRows + uCol];
                            lpMemRht[uTmp * aplDimRows + uCol] = lpMemRht[uRow * aplDimRows + uCol];
                                                                 lpMemRht[uRow * aplDimRows + uCol] = aplRatXch;
            } // End FOR

            // Get the diagonal element to be used as a divisor
            lpaplRatDiv = &lpMemRht[uRow * aplDimRows + uRow];

            // Reverse the sign after an exchange
            sign = -sign;
        } // End IF

        // Loop through the remaining rows
        for (uTmp = uRow + 1; uTmp < aplDimRows; uTmp++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Divide the lpMemRht[uTmp][uRow] by the divisor
            //   to get the scalar factor
            mpq_div (&aplRatScale, &lpMemRht[uTmp * aplDimRows + uRow], lpaplRatDiv);

            // Subtract row <uRow> scaled by <aplRatScale> from row <uTmp>
            for (uCol = 0; uCol < aplDimRows; uCol++)
            {
                // Scale the next value from row <uRow>
                mpq_mul (&aplRatTmp, &aplRatScale, &lpMemRht[uRow * aplDimRows + uCol]);

                // Subtract it from the corresponding value from row <uTmp>
                mpq_sub (&lpMemRht[uTmp * aplDimRows + uCol],
                         &lpMemRht[uTmp * aplDimRows + uCol],
                         &aplRatTmp);
            } // End FOR
        } // End FOR
    } // End FOR

    // Initialize to the first element
    mpq_init_set (lpMemRes, lpMemRht);

    // Loop through the diagonal elements in a copy of the right arg
    for (uRow = 1; uRow < aplDimRows; uRow++)
        // Evaluate the determinant by multiplying the diagonal elements
        mpq_mul (lpMemRes, lpMemRes, &lpMemRht[uRow * aplDimRows + uRow]);

    // If we had a odd # exchanges, ...
    if (sign < 0)
        // Negate the result
        mpq_neg (lpMemRes, lpMemRes);
NORMAL_EXIT:
    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    // Free the temps
    Myq_clear (&aplRatScale);
    Myq_clear (&aplRatTmp);

    return bRet;
} // End GaussJordanDetHC1R


//***************************************************************************
//  $GaussJordanDetBA1F
//
//  Perform Gauss-Jordan elimination on a square ARB matrix
//***************************************************************************

UBOOL GaussJordanDetBA1F
    (LPAPLARB lpMemRht,             // Ptr to copy of right arg as APLARB matrix
     APLDIM   aplDimRows,           // # rows/cols in the right arg
     LPAPLARB lpMemRes,             // Ptr to result as a single APLARB (uninitialized)
     LPUBOOL  lpbCtrlBreak)         // Ptr to Ctrl-Break flag

{
    APLARB   aplArbTmp = {0},       // Temporary ARB
             aplArbScale = {0};     // Scale factor
    LPAPLARB lpaplArbDiv;           // Divisor
    APLDIM   uRow,                  // Loop counter
             uCol,                  // ...
             uTmp;                  // ...
    int      sign = 1;              // Sign of the result
    UBOOL    bRet = FALSE;          // TRUE iff the result is valid
    mp_limb_signed_t prec = ARB_PREC_FPC;

    // Initialize the temps
    Myarb_init (&aplArbTmp);
    Myarb_init (&aplArbScale);

    // Loop through the rows in a copy of the right arg
    for (uRow = 0; uRow < aplDimRows; uRow++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (lpbCtrlBreak))
            goto ERROR_EXIT;

        // Get the diagonal element to be used as a divisor
        lpaplArbDiv = &lpMemRht[uRow * aplDimRows + uRow];

        // If it's zero,
        if (arb_sign (lpaplArbDiv) EQ 0)
        {
            // Loop through subsequent rows in the same column
            //   looking for a non-zero divisor for scaling
            for (uTmp = uRow + 1; uTmp < aplDimRows; uTmp++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Get the diagonal element to be used as a divisor (if non-zero)
                lpaplArbDiv = &lpMemRht[uTmp * aplDimRows + uRow];

                if (arb_sign (lpaplArbDiv) NE 0)
                    break;
            } // End FOR

            // If the divisor is still zero, ...
            if (arb_sign (lpaplArbDiv) EQ 0)
            {
                // The result is 0/1
                arb_init_set_sx (lpMemRes, 0);

                goto NORMAL_EXIT;
            } // End IF

            // Loop through the cols in rows <uTmp> and <uRow>
            for (uCol = 0; uCol < aplDimRows; uCol++)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Exchange the two rows <uTmp> and <uRow>
                arb_swap (&lpMemRht[uTmp * aplDimRows + uCol],
                          &lpMemRht[uRow * aplDimRows + uCol]);
            } // End FOR

            // Get the diagonal element to be used as a divisor
            lpaplArbDiv = &lpMemRht[uRow * aplDimRows + uRow];

            // Reverse the sign after an exchange
            sign = -sign;
        } // End IF

        // Loop through the remaining rows
        for (uTmp = uRow + 1; uTmp < aplDimRows; uTmp++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Divide the lpMemRht[uTmp][uRow] by the divisor
            //   to get the scalar factor
            arb_div (&aplArbScale, &lpMemRht[uTmp * aplDimRows + uRow], lpaplArbDiv, prec);

            // Subtract row <uRow> scaled by <aplArbScale> from row <uTmp>
            for (uCol = 0; uCol < aplDimRows; uCol++)
            {
                // Scale the next value from row <uRow>
                arb_mul (&aplArbTmp, &aplArbScale, &lpMemRht[uRow * aplDimRows + uCol], prec);

                // Subtract it from the corresponding value from row <uTmp>
                arb_sub (&lpMemRht[uTmp * aplDimRows + uCol],
                         &lpMemRht[uTmp * aplDimRows + uCol],
                         &aplArbTmp,
                          prec);
            } // End FOR
        } // End FOR
    } // End FOR

    // Initialize to the first element
    arb_init_set (lpMemRes, lpMemRht);

    // Loop through the diagonal elements in a copy of the right arg
    for (uRow = 1; uRow < aplDimRows; uRow++)
        // Evaluate the determinant by multiplying the diagonal elements
        arb_mul (lpMemRes, lpMemRes, &lpMemRht[uRow * aplDimRows + uRow], prec);

    // If we had a odd # exchanges, ...
    if (sign < 0)
        // Negate the result
        arb_neg (lpMemRes, lpMemRes);
NORMAL_EXIT:
    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    // Free the temps
    Myarb_clear (&aplArbScale);
    Myarb_clear (&aplArbTmp);

    return bRet;
} // End GaussJordanDetBA1F


//***************************************************************************
//  $MacGaussJordanDetHCxF
//
//  Macro to perform Gauss-Jordan elimination on a square HCxF matrix
//***************************************************************************

#define MacGaussJordanDetHCxF(lpMemRht,aplDimRows,lpMemRes,N,lpbCtrlBreak)                                  \
{                                                                                                           \
    APLHC##N##F aplTmp = {0},       /* Temporary HC##N##F                                                 */\
              aplXch,               /* ...                                                                */\
              aplScale = {0};       /* Scale factor                                                       */\
    LPAPLHC##N##F lpaplDiv;         /* Divisor                                                            */\
    APLDIM    uRow,                 /* Loop counter                                                       */\
              uCol,                 /* ...                                                                */\
              uTmp;                 /* ...                                                                */\
    int       sign = 1;             /* Sign of the result                                                 */\
    UBOOL     bRet = FALSE;         /* TRUE iff the result is valid                                       */\
                                                                                                            \
    /* Initialize the temps                                                                               */\
    fpXf_init (&aplTmp);                                                                                    \
    fpXf_init (&aplScale);                                                                                  \
                                                                                                            \
    /* Loop through the rows in a copy of the right arg                                                   */\
    for (uRow = 0; uRow < aplDimRows; uRow++)                                                               \
    {                                                                                                       \
        /* Check for Ctrl-Break                                                                           */\
        if (CheckCtrlBreak (lpbCtrlBreak))                                                                 \
            goto ERROR_EXIT##N;                                                                             \
                                                                                                            \
        /* Get the diagonal element to be used as a divisor                                               */\
        lpaplDiv = &lpMemRht[uRow * aplDimRows + uRow];                                                     \
                                                                                                            \
        /* If it's zero,                                                                                  */\
        if (fpXf_zero_p (lpaplDiv))                                                                         \
        {                                                                                                   \
            /* Loop through subsequent rows in the same column                                            */\
            /*   looking for a non-zero divisor for scaling                                               */\
            for (uTmp = uRow + 1; uTmp < aplDimRows; uTmp++)                                                \
            {                                                                                               \
                /* Check for Ctrl-Break                                                                   */\
                if (CheckCtrlBreak (lpbCtrlBreak))                                                         \
                    goto ERROR_EXIT##N;                                                                     \
                                                                                                            \
                /* Get the diagonal element to be used as a divisor (if non-zero)                         */\
                lpaplDiv = &lpMemRht[uTmp * aplDimRows + uRow];                                             \
                                                                                                            \
                if (!fpXf_zero_p (lpaplDiv))                                                                \
                    break;                                                                                  \
            } /* End FOR                                                                                  */\
                                                                                                            \
            /* If the divisor is still zero, ...                                                          */\
            if (fpXf_zero_p (lpaplDiv))                                                                     \
            {                                                                                               \
                /* The result is 0                                                                        */\
                fpXf_init0 (lpMemRes);                                                                      \
                                                                                                            \
                goto NORMAL_EXIT##N;                                                                        \
            } /* End IF                                                                                   */\
                                                                                                            \
            /* Loop through the cols in rows <uTmp> and <uRow>                                            */\
            for (uCol = 0; uCol < aplDimRows; uCol++)                                                       \
            {                                                                                               \
                /* Check for Ctrl-Break                                                                   */\
                if (CheckCtrlBreak (lpbCtrlBreak))                                                         \
                    goto ERROR_EXIT##N;                                                                     \
                                                                                                            \
                /* Exchange the two rows <uTmp> and <uRow>                                                */\
                aplXch = lpMemRht[uTmp * aplDimRows + uCol];                                                \
                         lpMemRht[uTmp * aplDimRows + uCol] = lpMemRht[uRow * aplDimRows + uCol];           \
                                                              lpMemRht[uRow * aplDimRows + uCol] = aplXch;  \
            } /* End FOR                                                                                  */\
                                                                                                            \
            /* Get the diagonal element to be used as a divisor                                           */\
            lpaplDiv = &lpMemRht[uRow * aplDimRows + uRow];                                                 \
                                                                                                            \
            /* Reverse the sign after an exchange                                                         */\
            sign = -sign;                                                                                   \
        } /* End IF                                                                                       */\
                                                                                                            \
        /* Loop through the remaining rows                                                                */\
        for (uTmp = uRow + 1; uTmp < aplDimRows; uTmp++)                                                    \
        {                                                                                                   \
            /* Check for Ctrl-Break                                                                       */\
            if (CheckCtrlBreak (lpbCtrlBreak))                                                             \
                goto ERROR_EXIT##N;                                                                         \
                                                                                                            \
            /* Divide the lpMemRht[uTmp][uRow] by the divisor                                             */\
            /*   to get the scalar factor                                                                 */\
            fpXf_div (&aplScale, &lpMemRht[uTmp * aplDimRows + uRow], lpaplDiv);                            \
                                                                                                            \
            /* Subtract row <uRow> scaled by <aplScale> from row <uTmp>                                   */\
            for (uCol = 0; uCol < aplDimRows; uCol++)                                                       \
            {                                                                                               \
                /* Scale the next value from row <uRow>                                                   */\
                fpXf_mul (&aplTmp, &aplScale, &lpMemRht[uRow * aplDimRows + uCol]);                         \
                                                                                                            \
                /* Subtract it from the corresponding value from row <uTmp>                               */\
                fpXf_sub (&lpMemRht[uTmp * aplDimRows + uCol],                                              \
                          &lpMemRht[uTmp * aplDimRows + uCol],                                              \
                          &aplTmp);                                                                         \
            } /* End FOR                                                                                  */\
        } /* End FOR                                                                                      */\
    } /* End FOR                                                                                          */\
                                                                                                            \
    /* Initialize to the first element                                                                    */\
    fpXf_init_set (lpMemRes, lpMemRht);                                                                     \
                                                                                                            \
    /* Loop through the diagonal elements in a copy of the right arg                                      */\
    for (uRow = 1; uRow < aplDimRows; uRow++)                                                               \
        /* Evaluate the determinant by multiplying the diagonal elements                                  */\
        fpXf_mul (lpMemRes, lpMemRes, &lpMemRht[uRow * aplDimRows + uRow]);                                 \
                                                                                                            \
    /* If we had a odd # exchanges, ...                                                                   */\
    if (sign < 0)                                                                                           \
        /* Negate the result                                                                              */\
        fpXf_neg (lpMemRes, lpMemRes);                                                                      \
NORMAL_EXIT##N:                                                                                             \
    /* Mark as successful                                                                                 */\
    bRet = TRUE;                                                                                            \
ERROR_EXIT##N:                                                                                              \
    /* Free the temps                                                                                     */\
    fpXf_clear (&aplScale);                                                                                 \
    fpXf_clear (&aplTmp);                                                                                   \
                                                                                                            \
    return bRet;                                                                                            \
} /* End MacGaussJordanDetHCxF                                                                            */


//***************************************************************************
//  $GaussJordanDetHCxF
//
//  Perform Gauss-Jordan elimination on a square HCxF matrix
//***************************************************************************

UBOOL GaussJordanDetHCxF
    (LPVOID    lpMemRht,            // Ptr to copy of right arg as APLHCxF matrix
     APLDIM    aplDimRows,          // # rows/cols in the right arg
     LPVOID    lpMemRes,            // Ptr to result as a single APLHCxF (uninitialized)
     int       iHCDimRes,           // HC Dimension (1, 2, 4, 8)
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    // Split cases based upon the HC Dimension (1, 2, 4, 8)
    switch (iHCDimRes)
    {
        case 4:
#define fpXf_zero_p(a)          IsZeroHCxF((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivHC4F_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulHC4F_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubHC4F_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegHC4F_RE (*(b))
#define fpXf_init(a)            ZeroMemory ((a), sizeof (*(a)))
#define fpXf_clear(a)
#define fpXf_init_set(a,b)      *(a) = *(b)
#define fpXf_init0(a)           fpXf_init(a)

            MacGaussJordanDetHCxF (((LPAPLHC4F) lpMemRht),      // Ptr to copy of right arg as APLHCxF matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLHC4F) lpMemRes),      // Ptr to result as a single APLHCxF (uninitialized)
                                                4,              // HC Dimension (1, 2, 4, 8)
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 8:
#define fpXf_zero_p(a)          IsZeroHCxF((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivHC8F_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulHC8F_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubHC8F_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegHC8F_RE (*(b))
#define fpXf_init(a)            ZeroMemory ((a), sizeof (*(a)))
#define fpXf_clear(a)
#define fpXf_init_set(a,b)      *(a) = *(b)
#define fpXf_init0(a)           fpXf_init(a)

            MacGaussJordanDetHCxF (((LPAPLHC8F) lpMemRht),      // Ptr to copy of right arg as APLHCxF matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLHC8F) lpMemRes),      // Ptr to result as a single APLHCxF (uninitialized)
                                                8,              // HC Dimension (1, 2, 4, 8)
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 1:
        case 2:
        defstop
            return FALSE;
    } // End SWITCH
} // End GaussJordanDetHCxF


//***************************************************************************
//  $MacGaussJordanDetHCxR
//
//  Macro to perform Gauss-Jordan elimination on a square HCxR matrix
//***************************************************************************

#define MacGaussJordanDetHCxR(lpMemRht,aplDimRows,lpMemRes,N,lpbCtrlBreak)                                  \
{                                                                                                           \
    APLHC##N##R aplTmp = {0},       /* Temporary HC##N##R                                                 */\
              aplXch,               /* ...                                                                */\
              aplScale = {0};       /* Scale factor                                                       */\
    LPAPLHC##N##R lpaplDiv;         /* Divisor                                                            */\
    APLDIM    uRow,                 /* Loop counter                                                       */\
              uCol,                 /* ...                                                                */\
              uTmp;                 /* ...                                                                */\
    int       sign = 1;             /* Sign of the result                                                 */\
    UBOOL     bRet = FALSE;         /* TRUE iff the result is valid                                       */\
                                                                                                            \
    /* Initialize the temps                                                                               */\
    fpXf_init (&aplTmp);                                                                                    \
    fpXf_init (&aplScale);                                                                                  \
                                                                                                            \
    /* Loop through the rows in a copy of the right arg                                                   */\
    for (uRow = 0; uRow < aplDimRows; uRow++)                                                               \
    {                                                                                                       \
        /* Check for Ctrl-Break                                                                           */\
        if (CheckCtrlBreak (lpbCtrlBreak))                                                                 \
            goto ERROR_EXIT##N;                                                                             \
                                                                                                            \
        /* Get the diagonal element to be used as a divisor                                               */\
        lpaplDiv = &lpMemRht[uRow * aplDimRows + uRow];                                                     \
                                                                                                            \
        /* If it's zero,                                                                                  */\
        if (fpXf_zero_p (lpaplDiv))                                                                         \
        {                                                                                                   \
            /* Loop through subsequent rows in the same column                                            */\
            /*   looking for a non-zero divisor for scaling                                               */\
            for (uTmp = uRow + 1; uTmp < aplDimRows; uTmp++)                                                \
            {                                                                                               \
                /* Check for Ctrl-Break                                                                   */\
                if (CheckCtrlBreak (lpbCtrlBreak))                                                         \
                    goto ERROR_EXIT##N;                                                                     \
                                                                                                            \
                /* Get the diagonal element to be used as a divisor (if non-zero)                         */\
                lpaplDiv = &lpMemRht[uTmp * aplDimRows + uRow];                                             \
                                                                                                            \
                if (!fpXf_zero_p (lpaplDiv))                                                                \
                    break;                                                                                  \
            } /* End FOR                                                                                  */\
                                                                                                            \
            /* If the divisor is still zero, ...                                                          */\
            if (fpXf_zero_p (lpaplDiv))                                                                     \
            {                                                                                               \
                /* The result is 0                                                                        */\
                fpXf_init0 (lpMemRes);                                                                      \
                                                                                                            \
                goto NORMAL_EXIT##N;                                                                        \
            } /* End IF                                                                                   */\
                                                                                                            \
            /* Loop through the cols in rows <uTmp> and <uRow>                                            */\
            for (uCol = 0; uCol < aplDimRows; uCol++)                                                       \
            {                                                                                               \
                /* Check for Ctrl-Break                                                                   */\
                if (CheckCtrlBreak (lpbCtrlBreak))                                                         \
                    goto ERROR_EXIT##N;                                                                     \
                                                                                                            \
                /* Exchange the two rows <uTmp> and <uRow>                                                */\
                aplXch = lpMemRht[uTmp * aplDimRows + uCol];                                                \
                         lpMemRht[uTmp * aplDimRows + uCol] = lpMemRht[uRow * aplDimRows + uCol];           \
                                                              lpMemRht[uRow * aplDimRows + uCol] = aplXch;  \
            } /* End FOR                                                                                  */\
                                                                                                            \
            /* Get the diagonal element to be used as a divisor                                           */\
            lpaplDiv = &lpMemRht[uRow * aplDimRows + uRow];                                                 \
                                                                                                            \
            /* Reverse the sign after an exchange                                                         */\
            sign = -sign;                                                                                   \
        } /* End IF                                                                                       */\
                                                                                                            \
        /* Loop through the remaining rows                                                                */\
        for (uTmp = uRow + 1; uTmp < aplDimRows; uTmp++)                                                    \
        {                                                                                                   \
            /* Check for Ctrl-Break                                                                       */\
            if (CheckCtrlBreak (lpbCtrlBreak))                                                             \
                goto ERROR_EXIT##N;                                                                         \
                                                                                                            \
            /* Divide the lpMemRht[uTmp][uRow] by the divisor                                             */\
            /*   to get the scalar factor                                                                 */\
            fpXf_div (&aplScale, &lpMemRht[uTmp * aplDimRows + uRow], lpaplDiv);                            \
                                                                                                            \
            /* Subtract row <uRow> scaled by <aplScale> from row <uTmp>                                   */\
            for (uCol = 0; uCol < aplDimRows; uCol++)                                                       \
            {                                                                                               \
                /* Scale the next value from row <uRow>                                                   */\
                fpXf_mul (&aplTmp, &aplScale, &lpMemRht[uRow * aplDimRows + uCol]);                         \
                                                                                                            \
                /* Subtract it from the corresponding value from row <uTmp>                               */\
                fpXf_sub (&lpMemRht[uTmp * aplDimRows + uCol],                                              \
                          &lpMemRht[uTmp * aplDimRows + uCol],                                              \
                          &aplTmp);                                                                         \
            } /* End FOR                                                                                  */\
        } /* End FOR                                                                                      */\
    } /* End FOR                                                                                          */\
                                                                                                            \
    /* Initialize to the first element                                                                    */\
    fpXf_init_set (lpMemRes, lpMemRht);                                                                     \
                                                                                                            \
    /* Loop through the diagonal elements in a copy of the right arg                                      */\
    for (uRow = 1; uRow < aplDimRows; uRow++)                                                               \
        /* Evaluate the determinant by multiplying the diagonal elements                                  */\
        fpXf_mul (lpMemRes, lpMemRes, &lpMemRht[uRow * aplDimRows + uRow]);                                 \
                                                                                                            \
    /* If we had a odd # exchanges, ...                                                                   */\
    if (sign < 0)                                                                                           \
        /* Negate the result                                                                              */\
        fpXf_neg (lpMemRes, lpMemRes);                                                                      \
NORMAL_EXIT##N:                                                                                             \
    /* Mark as successful                                                                                 */\
    bRet = TRUE;                                                                                            \
ERROR_EXIT##N:                                                                                              \
    /* Free the temps                                                                                     */\
    fpXf_clear (&aplScale);                                                                                 \
    fpXf_clear (&aplTmp);                                                                                   \
                                                                                                            \
    return bRet;                                                                                            \
} /* End MacGaussJordanDetHCxR                                                                            */


//***************************************************************************
//  $GaussJordanDetHCxR
//
//  Perform Gauss-Jordan elimination on a square HCxR matrix
//***************************************************************************

UBOOL GaussJordanDetHCxR
    (LPVOID    lpMemRht,            // Ptr to copy of right arg as APLHCxR matrix
     APLDIM    aplDimRows,          // # rows/cols in the right arg
     LPVOID    lpMemRes,            // Ptr to result as a single APLHCxR (uninitialized)
     int       iHCDimRes,           // HC Dimension (1, 2, 4, 8)
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    // Split cases based upon the HC Dimension (1, 2, 4, 8)
    switch (iHCDimRes)
    {
        case 2:
#define fpXf_zero_p(a)          IsZeroHCxR((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivHC2R_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulHC2R_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubHC2R_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegHC2R_RE (*(b))
#define fpXf_init(a)            mphc2r_init (a)
#define fpXf_clear(a)           Myhc2r_clear (a)
#define fpXf_init_set(a,b)      mphc2r_init_set ((a), (b))
#define fpXf_init0(a)           fpXf_init(a)

            MacGaussJordanDetHCxR (((LPAPLHC2R) lpMemRht),      // Ptr to copy of right arg as APLHCxR matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLHC2R) lpMemRes),      // Ptr to result as a single APLHCxR (uninitialized)
                                                2,              // HC Dimension (1, 2, 4, 8)
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 4:
#define fpXf_zero_p(a)          IsZeroHCxR((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivHC4R_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulHC4R_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubHC4R_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegHC4R_RE (*(b))
#define fpXf_init(a)            mphc4r_init (a)
#define fpXf_clear(a)           Myhc4r_clear (a)
#define fpXf_init_set(a,b)      mphc4r_init_set ((a), (b))
#define fpXf_init0(a)           fpXf_init(a)

            MacGaussJordanDetHCxR (((LPAPLHC4R) lpMemRht),      // Ptr to copy of right arg as APLHCxR matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLHC4R) lpMemRes),      // Ptr to result as a single APLHCxR (uninitialized)
                                                4,              // HC Dimension (1, 2, 4, 8)
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 8:
#define fpXf_zero_p(a)          IsZeroHCxR((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivHC8R_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulHC8R_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubHC8R_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegHC8R_RE (*(b))
#define fpXf_init(a)            mphc8r_init (a)
#define fpXf_clear(a)           Myhc8r_clear (a)
#define fpXf_init_set(a,b)      mphc8r_init_set ((a), (b))
#define fpXf_init0(a)           fpXf_init(a)

            MacGaussJordanDetHCxR (((LPAPLHC8R) lpMemRht),      // Ptr to copy of right arg as APLHCxR matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLHC8R) lpMemRes),      // Ptr to result as a single APLHCxR (uninitialized)
                                                8,              // HC Dimension (1, 2, 4, 8)
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 1:
        defstop
            return FALSE;
    } // End SWITCH
} // End GaussJordanDetHCxR


//***************************************************************************
//  $MacGaussJordanDetHCxV
//
//  Macro to perform Gauss-Jordan elimination on a square HCxV matrix
//***************************************************************************

#define MacGaussJordanDetHCxV(lpMemRht,aplDimRows,lpMemRes,HC,N,V,lpbCtrlBreak)                             \
{                                                                                                           \
    APL##HC##N##V aplTmp = {0},     /* Temporary HC##N##V                                                 */\
              aplXch,               /* ...                                                                */\
              aplScale = {0};       /* Scale factor                                                       */\
    LPAPL##HC##N##V lpaplDiv;       /* Divisor                                                            */\
    APLDIM    uRow,                 /* Loop counter                                                       */\
              uCol,                 /* ...                                                                */\
              uTmp;                 /* ...                                                                */\
    int       sign = 1;             /* Sign of the result                                                 */\
    UBOOL     bRet = FALSE;         /* TRUE iff the result is valid                                       */\
                                                                                                            \
    /* Initialize the temps                                                                               */\
    fpXf_init (&aplTmp);                                                                                    \
    fpXf_init (&aplScale);                                                                                  \
                                                                                                            \
    /* Loop through the rows in a copy of the right arg                                                   */\
    for (uRow = 0; uRow < aplDimRows; uRow++)                                                               \
    {                                                                                                       \
        /* Check for Ctrl-Break                                                                           */\
        if (CheckCtrlBreak (lpbCtrlBreak))                                                                 \
            goto ERROR_EXIT##N;                                                                             \
                                                                                                            \
        /* Get the diagonal element to be used as a divisor                                               */\
        lpaplDiv = &lpMemRht[uRow * aplDimRows + uRow];                                                     \
                                                                                                            \
        /* If it's zero,                                                                                  */\
        if (fpXf_zero_p (lpaplDiv))                                                                         \
        {                                                                                                   \
            /* Loop through subsequent rows in the same column                                            */\
            /*   looking for a non-zero divisor for scaling                                               */\
            for (uTmp = uRow + 1; uTmp < aplDimRows; uTmp++)                                                \
            {                                                                                               \
                /* Check for Ctrl-Break                                                                   */\
                if (CheckCtrlBreak (lpbCtrlBreak))                                                         \
                    goto ERROR_EXIT##N;                                                                     \
                                                                                                            \
                /* Get the diagonal element to be used as a divisor (if non-zero)                         */\
                lpaplDiv = &lpMemRht[uTmp * aplDimRows + uRow];                                             \
                                                                                                            \
                if (!fpXf_zero_p (lpaplDiv))                                                                \
                    break;                                                                                  \
            } /* End FOR                                                                                  */\
                                                                                                            \
            /* If the divisor is still zero, ...                                                          */\
            if (fpXf_zero_p (lpaplDiv))                                                                     \
            {                                                                                               \
                /* The result is 0                                                                        */\
                fpXf_init0 (lpMemRes);                                                                      \
                                                                                                            \
                goto NORMAL_EXIT##N;                                                                        \
            } /* End IF                                                                                   */\
                                                                                                            \
            /* Loop through the cols in rows <uTmp> and <uRow>                                            */\
            for (uCol = 0; uCol < aplDimRows; uCol++)                                                       \
            {                                                                                               \
                /* Check for Ctrl-Break                                                                   */\
                if (CheckCtrlBreak (lpbCtrlBreak))                                                         \
                    goto ERROR_EXIT##N;                                                                     \
                                                                                                            \
                /* Exchange the two rows <uTmp> and <uRow>                                                */\
                aplXch = lpMemRht[uTmp * aplDimRows + uCol];                                                \
                         lpMemRht[uTmp * aplDimRows + uCol] = lpMemRht[uRow * aplDimRows + uCol];           \
                                                              lpMemRht[uRow * aplDimRows + uCol] = aplXch;  \
            } /* End FOR                                                                                  */\
                                                                                                            \
            /* Get the diagonal element to be used as a divisor                                           */\
            lpaplDiv = &lpMemRht[uRow * aplDimRows + uRow];                                                 \
                                                                                                            \
            /* Reverse the sign after an exchange                                                         */\
            sign = -sign;                                                                                   \
        } /* End IF                                                                                       */\
                                                                                                            \
        /* Loop through the remaining rows                                                                */\
        for (uTmp = uRow + 1; uTmp < aplDimRows; uTmp++)                                                    \
        {                                                                                                   \
            /* Check for Ctrl-Break                                                                       */\
            if (CheckCtrlBreak (lpbCtrlBreak))                                                             \
                goto ERROR_EXIT##N;                                                                         \
                                                                                                            \
            /* Divide the lpMemRht[uTmp][uRow] by the divisor                                             */\
            /*   to get the scalar factor                                                                 */\
            fpXf_div (&aplScale, &lpMemRht[uTmp * aplDimRows + uRow], lpaplDiv);                            \
                                                                                                            \
            /* Subtract row <uRow> scaled by <aplScale> from row <uTmp>                                   */\
            for (uCol = 0; uCol < aplDimRows; uCol++)                                                       \
            {                                                                                               \
                /* Scale the next value from row <uRow>                                                   */\
                fpXf_mul (&aplTmp, &aplScale, &lpMemRht[uRow * aplDimRows + uCol]);                         \
                                                                                                            \
                /* Subtract it from the corresponding value from row <uTmp>                               */\
                fpXf_sub (&lpMemRht[uTmp * aplDimRows + uCol],                                              \
                          &lpMemRht[uTmp * aplDimRows + uCol],                                              \
                          &aplTmp);                                                                         \
            } /* End FOR                                                                                  */\
        } /* End FOR                                                                                      */\
    } /* End FOR                                                                                          */\
                                                                                                            \
    /* Initialize to the first element                                                                    */\
    fpXf_init_set (lpMemRes, lpMemRht);                                                                     \
                                                                                                            \
    /* Loop through the diagonal elements in a copy of the right arg                                      */\
    for (uRow = 1; uRow < aplDimRows; uRow++)                                                               \
        /* Evaluate the determinant by multiplying the diagonal elements                                  */\
        fpXf_mul (lpMemRes, lpMemRes, &lpMemRht[uRow * aplDimRows + uRow]);                                 \
                                                                                                            \
    /* If we had a odd # exchanges, ...                                                                   */\
    if (sign < 0)                                                                                           \
        /* Negate the result                                                                              */\
        fpXf_neg (lpMemRes, lpMemRes);                                                                      \
NORMAL_EXIT##N:                                                                                             \
    /* Mark as successful                                                                                 */\
    bRet = TRUE;                                                                                            \
ERROR_EXIT##N:                                                                                              \
    /* Free the temps                                                                                     */\
    fpXf_clear (&aplScale);                                                                                 \
    fpXf_clear (&aplTmp);                                                                                   \
                                                                                                            \
    return bRet;                                                                                            \
} /* End MacGaussJordanDetHCxV                                                                            */


//***************************************************************************
//  $GaussJordanDetHCxV
//
//  Perform Gauss-Jordan elimination on a square HCxV matrix
//***************************************************************************

UBOOL GaussJordanDetHCxV
    (LPVOID    lpMemRht,            // Ptr to copy of right arg as APLHCxV matrix
     APLDIM    aplDimRows,          // # rows/cols in the right arg
     LPVOID    lpMemRes,            // Ptr to result as a single APLHCxV (uninitialized)
     int       iHCDimRes,           // HC Dimension (1, 2, 4, 8)
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    // Split cases based upon the HC Dimension (1, 2, 4, 8)
    switch (iHCDimRes)
    {
        case 2:
#define fpXf_zero_p(a)          IsZeroHCxV((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivHC2V_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulHC2V_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubHC2V_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegHC2V_RE (*(b))
#define fpXf_init(a)            mphc2v_init0 (a)
#define fpXf_clear(a)           Myhc2v_clear (a)
#define fpXf_init_set(a,b)      mphc2v_init_set ((a), (b))
#define fpXf_init0(a)           fpXf_init(a)

            MacGaussJordanDetHCxV (((LPAPLHC2V) lpMemRht),      // Ptr to copy of right arg as APLHCxV matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLHC2V) lpMemRes),      // Ptr to result as a single APLHCxV (uninitialized)
                                                HC,             // Prefix
                                                2,              // HC Dimension (1, 2, 4, 8)
                                                V,              // Suffix
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 4:
#define fpXf_zero_p(a)          IsZeroHCxV((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivHC4V_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulHC4V_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubHC4V_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegHC4V_RE (*(b))
#define fpXf_init(a)            mphc4v_init0 (a)
#define fpXf_clear(a)           Myhc4v_clear (a)
#define fpXf_init_set(a,b)      mphc4v_init_set ((a), (b))
#define fpXf_init0(a)           fpXf_init(a)

            MacGaussJordanDetHCxV (((LPAPLHC4V) lpMemRht),      // Ptr to copy of right arg as APLHCxV matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLHC4V) lpMemRes),      // Ptr to result as a single APLHCxV (uninitialized)
                                                HC,             // Prefix
                                                4,              // HC Dimension (1, 2, 4, 8)
                                                V,              // Suffix
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 8:
#define fpXf_zero_p(a)          IsZeroHCxV((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivHC8V_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulHC8V_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubHC8V_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegHC8V_RE (*(b))
#define fpXf_init(a)            mphc8v_init0 (a)
#define fpXf_clear(a)           Myhc8v_clear (a)
#define fpXf_init_set(a,b)      mphc8v_init_set ((a), (b))
#define fpXf_init0(a)           fpXf_init(a)

            MacGaussJordanDetHCxV (((LPAPLHC8V) lpMemRht),      // Ptr to copy of right arg as APLHCxV matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLHC8V) lpMemRes),      // Ptr to result as a single APLHCxV (uninitialized)
                                                HC,             // Prefix
                                                8,              // HC Dimension (1, 2, 4, 8)
                                                V,              // Suffix
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 1:
        defstop
            return FALSE;
    } // End SWITCH
} // End GaussJordanDetHCxV


//***************************************************************************
//  $GaussJordanDetBAxF
//
//  Perform Gauss-Jordan elimination on a square BAxF matrix
//***************************************************************************

UBOOL GaussJordanDetBAxF
    (LPVOID    lpMemRht,            // Ptr to copy of right arg as APLBAxF matrix
     APLDIM    aplDimRows,          // # rows/cols in the right arg
     LPVOID    lpMemRes,            // Ptr to result as a single APLBAxF (uninitialized)
     int       iHCDimRes,           // HC Dimension (1, 2, 4, 8)
     LPUBOOL   lpbCtrlBreak)        // Ptr to Ctrl-Break flag

{
    // Split cases based upon the HC Dimension (1, 2, 4, 8)
    switch (iHCDimRes)
    {
        case 2:
#define fpXf_zero_p(a)          IsZeroBAxF((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivBA2F_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulBA2F_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubBA2F_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegBA2F_RE (*(b))
#define fpXf_init(a)            arb2f_init (a)
#define fpXf_clear(a)           arb2f_clear (a)
#define fpXf_init_set(a,b)      arb2f_init_set ((a), (b))
#define fpXf_init0(a)           arb2f_init (a)

            MacGaussJordanDetHCxV (((LPAPLBA2F) lpMemRht),      // Ptr to copy of right arg as APLBAxF matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLBA2F) lpMemRes),      // Ptr to result as a single APLBAxF (uninitialized)
                                                BA,             // Prefix
                                                2,              // HC Dimension (1, 2, 4, 8)
                                                F,              // Suffix
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 4:
#define fpXf_zero_p(a)          IsZeroBAxF((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivBA4F_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulBA4F_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubBA4F_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegBA4F_RE (*(b))
#define fpXf_init(a)            arb4f_init (a)
#define fpXf_clear(a)           arb4f_clear (a)
#define fpXf_init_set(a,b)      arb4f_init_set ((a), (b))
#define fpXf_init0(a)           arb4f_init (a)

            MacGaussJordanDetHCxV (((LPAPLBA4F) lpMemRht),      // Ptr to copy of right arg as APLBAxF matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLBA4F) lpMemRes),      // Ptr to result as a single APLBAxF (uninitialized)
                                                BA,             // Prefix
                                                4,              // HC Dimension (1, 2, 4, 8)
                                                F,              // Suffix
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 8:
#define fpXf_zero_p(a)          IsZeroBAxF((a), iHCDimRes)
#define fpXf_div(a,b,c)         *(a) = DivBA8F_RE (*(b), *(c))
#define fpXf_mul(a,b,c)         *(a) = MulBA8F_RE (*(b), *(c))
#define fpXf_sub(a,b,c)         *(a) = SubBA8F_RE (*(b), *(c))
#define fpXf_neg(a,b)           *(a) = NegBA8F_RE (*(b))
#define fpXf_init(a)            arb8f_init (a)
#define fpXf_clear(a)           arb8f_clear (a)
#define fpXf_init_set(a,b)      arb8f_init_set ((a), (b))
#define fpXf_init0(a)           arb8f_init (a)

            MacGaussJordanDetHCxV (((LPAPLBA8F) lpMemRht),      // Ptr to copy of right arg as APLBAxF matrix
                                                aplDimRows,     // # rows/cols in the right arg
                                   ((LPAPLBA8F) lpMemRes),      // Ptr to result as a single APLBAxF (uninitialized)
                                                BA,             // Prefix
                                                8,              // HC Dimension (1, 2, 4, 8)
                                                F,              // Suffix
                                                lpbCtrlBreak);  // Ptr to Ctrl-Break flag
#undef  fpXf_init0
#undef  fpXf_init_set
#undef  fpXf_clear
#undef  fpXf_init
#undef  fpXf_neg
#undef  fpXf_sub
#undef  fpXf_mul
#undef  fpXf_div
#undef  fpXf_zero_p

        case 1:
        defstop
            return FALSE;
    } // End SWITCH
} // End GaussJordanDetBAxF


//***************************************************************************
//  $PrimOpDydDot_EM_YY
//
//  Primitive operator for dyadic derived function from Dot ("inner product")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return
      PrimOpDydDotCommon_EM_YY (lptkLftArg,             // Ptr to left arg token (may be NULL if niladic/monadic)
                                lpYYFcnStrOpr,          // Ptr to operator function strand
                                lptkRhtArg,             // Ptr to right arg token (may be NULL if niladic)
                                FALSE);                 // TRUE iff prototyping
} // End PrimOpDydDot_EM_YY


//***************************************************************************
//  $PrimOpDydDotCommon_EM_YY
//
//  Primitive operator for dyadic derived function from Dot ("inner product")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDotCommon_EM_YY
    (LPTOKEN      lptkLftArg,               // Ptr to left arg token (may be NULL if niladic/monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,            // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)             // TRUE iff prototyping

{
    APLSTYPE          aplTypeLft,               // Left arg storage type
                      aplTypeRht,               // Right ...
                      aplTypeCmp,               // Comparison ...
                      aplTypeCom,               // Common
                      aplTypeRes;               // Result   ...
    APLNELM           aplNELMLft,               // Left arg NELM
                      aplNELMRht,               // Right ...
                      aplNELMRes;               // Result   ...
    APLRANK           aplRankLft,               // Left arg rank
                      aplRankRht,               // Right ...
                      aplRankRes;               // Result   ...
    APLDIM            aplColsLft,               // Left arg last dim
                      aplRestLft,               // Left arg product of remaining dims
                      aplFrstRht,               // Right arg 1st dim
                      aplRestRht,               // Right arg product of remaining dims
                      aplInnrMax;               // Larger of inner dimensions
    APLLONGEST        aplLongestLft,            // Left arg immediate value
                      aplLongestRht;            // Right ...
    HGLOBAL           hGlbLft = NULL,           // Left arg global memory handle
                      hGlbRht = NULL,           // Right ...
                      hGlbRes = NULL,           // Result   ...
                      hGlbItm,                  // Arg item ...
                      hGlbPro = NULL;           // Prototype global memory handle
    LPVARARRAY_HEADER lpMemHdrLft = NULL,       // Ptr to left arg header
                      lpMemHdrRht = NULL,       // ...    right ...
                      lpMemHdrRes = NULL;       // ...    result   ...
    LPVOID            lpMemLft,                 // Ptr to left arg global memory
                      lpMemRht,                 // Ptr to right ...
                      lpMemRes;                 // Ptr to result   ...
    LPAPLDIM          lpMemDimLft,              // Ptr to left arg dimensions
                      lpMemDimRht,              // Ptr to right ...
                      lpMemDimRes;              // Ptr to result   ...
    APLUINT           ByteRes,                  // # bytes in the result
                      uRes,                     // Loop counter
                      uOutLft,                  // Loop counter
                      uOutRht,                  // Loop counter
                      uDimCopy;                 // # dimensions to copy
    APLINT            iInnMax,                  // Loop counter
                      apaOffLft,                // Left arg APA offset
                      apaMulLft,                // ...          multiplier
                      apaOffRht,                // Right arg APA offset
                      apaMulRht;                // ...           multiplier
    APLFLOAT          aplFloatIdent;            // Identity element
    LPPL_YYSTYPE      lpYYRes = NULL,           // Ptr to the result
                      lpYYRes2,                 // Ptr to secondary result
                      lpYYFcnStrLft,            // Ptr to left operand function strand
                      lpYYFcnStrRht;            // Ptr to right ...
    LPTOKEN           lptkAxisOpr,              // Ptr to operator axis token (may be NULL)
                      lptkAxisLft,              // ...    left operand axis token (may be NULL)
                      lptkAxisRht;              // ...    right ...
    LPPRIMFNS         lpPrimProtoLft = NULL,    // Ptr to left operand prototype function
                      lpPrimProtoRht = NULL;    // Ptr to right ...
    TOKEN             tkItmLft = {0},           // Left arg item token
                      tkItmRht = {0},           // Right ...
                      tkItmRed,                 // Reduction ...
                      tkProLft = {0},           // Left arg prototype token
                      tkProRht = {0};           // Right ...
    IMM_TYPES         immTypeItm;               // Arg item immediate type
    LPSYMENTRY        lpSymTmp;                 // Ptr to temporary LPSYMENTRY
    LPPLLOCALVARS     lpplLocalVars;            // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;             // Ptr to Ctrl-Break flag
    LPPRIMSPEC        lpPrimSpecLft,            // Ptr to left operand local PRIMSPEC
                      lpPrimSpecRht;            // ...    right ...
    LPPRIMFLAGS       lpPrimFlagsLft,           // Ptr to left operand PrimFlags entry
                      lpPrimFlagsRht;           // ...    right ...
    LPPRIMIDENT       lpPrimIdentLft;           // Ptr to left operand PrimIdent entry
    UBOOL             bRet = TRUE,              // TRUE iff result is valid
                      bNoDisplay = FALSE,       // TRUE iff the result should be marked as NoDisplay
                      bNrmIdent = FALSE,        // TRUE iff reducing an empty array with a primitive scalar dyadic function
                      bPrimIdent = FALSE,       // TRUE iff reducing an empty array with a primitive or
                                                //   user-defined function/operator
                      bDimDemote = FALSE;       // TRUE iff dimension demotion allowed
    ALLTYPES          atLft    = {0},           // Left arg as ALLTYPES
                      atCmpLft = {0},           // ...  compare ...
                      atRht    = {0},           // Right arg    ...
                      atCmpRht = {0};           // ...   compare ...
    int               i,                        // Loop counter
                      iHCDimRes,                // HC Dimension (1, 2, 4, 8)
                      iSizeofRes;               // # bytes in each result

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Check for operator axis token
    lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    //***************************************************************
    // The derived functions from this operator are not sensitive to
    //   the axis operator, so signal a syntax error if present
    //***************************************************************
    if (lptkAxisOpr NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, lptkAxisOpr               ); Assert (lpYYFcnStrRht NE NULL);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrOpr, lptkAxisOpr, lpYYFcnStrRht); Assert (lpYYFcnStrLft NE NULL);

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // Ensure the right operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // If we're using APL2's defn of Inner Product, ...
    if (gUseAPL2IP)
    {
        HGLOBAL   hGlbMFO;

        // Get the magic function/operator global memory handle
        hGlbMFO = GetMemPTD ()->hGlbMFO[MFOE_DydDot2];

        // Use APL2's defn of Inner Product
        lpYYRes =
          ExecuteMagicOperator_EM_YY (lptkLftArg,               // Ptr to left arg token (may be NULL)
                                     &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                      lpYYFcnStrLft,            // Ptr to left operand function strand
                                      lpYYFcnStrOpr,            // Ptr to function strand
                                      lpYYFcnStrRht,            // Ptr to right operand function strand (may be NULL)
                                      lptkRhtArg,               // Ptr to right arg token
                                      NULL,                     // Ptr to axis token (may be NULL)
                                      hGlbMFO,                  // Magic function/operator global memory handle
                                      NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                      LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
        // Check for error
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;
        else
            goto NORMAL_EXIT;
    } // End IF

    // Check for left operand axis operator
    lptkAxisLft = CheckAxisOper (lpYYFcnStrLft);

    // Check for right operand axis operator
    lptkAxisRht = CheckAxisOper (lpYYFcnStrRht);

    // Get a ptr to the left & right prototype function
    if (bPrototyping)
    {
       // Get the appropriate prototype function ptrs
       lpPrimProtoLft = GetPrototypeFcnPtr (GetLftFcnTkn (lpYYFcnStrLft));
       lpPrimProtoRht = GetPrototypeFcnPtr (GetLftFcnTkn (lpYYFcnStrRht));

        if (lpPrimProtoLft EQ NULL)
            goto LEFT_OPERAND_NONCE_EXIT;

        if (lpPrimProtoRht EQ NULL)
            goto RIGHT_OPERAND_NONCE_EXIT;
    } // End IF

    // Get a ptr to the Primitive Function Flags
    lpPrimFlagsLft = GetPrimFlagsPtr (GetLftFcnTkn (lpYYFcnStrLft));
    lpPrimFlagsRht = GetPrimFlagsPtr (GetLftFcnTkn (lpYYFcnStrRht));
    if (lpPrimFlagsLft NE NULL)
        lpPrimIdentLft = &PrimIdent[lpPrimFlagsLft->Index];
    else
        lpPrimIdentLft = NULL;

    // If we're called niladically, ...
    if (lptkRhtArg EQ NULL)
        goto VALENCE_EXIT;

    // Get the attributes (Type,NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, &aplColsLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get left & right arg global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Calculate length of right arg first dimension
    if (hGlbRht && !IsScalar (aplRankRht))
        aplFrstRht = *VarArrayBaseToDim (lpMemHdrRht);
    else
        aplFrstRht = 1;

    // Check for LENGTH ERROR
    if (aplColsLft NE aplFrstRht
     && !IsUnitDim (aplColsLft)     // Note that the EAS extends scalars or one-element vectors only
     && !IsUnitDim (aplFrstRht))    // ...
        goto LENGTH_EXIT;

    // Calc length of inner dimensions (in case of scalar extension)
    if (IsZeroDim (aplColsLft)
     || IsZeroDim (aplFrstRht))
        aplInnrMax = 0;
    else
        aplInnrMax = max (aplColsLft, aplFrstRht);

    // Calc product of the remaining dimensions in left arg
    if (!IsZeroDim (aplColsLft))
        aplRestLft = aplNELMLft / aplColsLft;
    else
    for (aplRestLft = 1, uOutLft = 0; uOutLft < (aplRankLft - 1); uOutLft++)
        aplRestLft *= (VarArrayBaseToDim (lpMemHdrLft))[uOutLft];

    // Calc product of the remaining dimensions in right arg
    if (!IsZeroDim (aplFrstRht))
        aplRestRht = aplNELMRht / aplFrstRht;
    else
    for (aplRestRht = 1, uOutRht = 1; uOutRht < aplRankRht; uOutRht++)
        aplRestRht *= (VarArrayBaseToDim (lpMemHdrRht))[uOutRht];

    // Calc result rank
    aplRankRes = max (aplRankLft, 1) + max (aplRankRht, 1) - 2;

    // Calc result NELM
    aplNELMRes = imul64 (aplRestLft, aplRestRht, &bRet, EXCEPTION_RESULT_FLOAT);
    if (!bRet)
        goto WSFULL_EXIT;

    // Handle prototypes specially
    if (IsEmpty (aplNELMRes)
     || bPrototyping)
    {
        if (lpPrimProtoLft EQ NULL)
            // Get the appropriate prototype function ptr
            lpPrimProtoLft = GetPrototypeFcnPtr (GetLftFcnTkn (lpYYFcnStrLft));
        if (lpPrimProtoLft EQ NULL)
            goto LEFT_OPERAND_NONCE_EXIT;
    } else
        lpPrimProtoLft = NULL;

    // If the result is empty or the axis dimension is zero, ...
    if ((IsEmpty (aplNELMRes)
      || IsZeroDim (aplInnrMax)))
    {
        // If we're also prototyping, ...
        if (bPrototyping)
            goto LEFT_OPERAND_NONCE_EXIT;

        // If both functions are primitive scalar dyadic, ...
        if (lpPrimFlagsLft->bDydScalar
         && lpPrimFlagsRht->bDydScalar)
        {
            // If it's not an immediate primitive function,
            //   or it is, but is without an identity element,
            //   signal a DOMAIN ERROR
            if (lpYYFcnStrLft->tkToken.tkFlags.TknType NE TKT_FCNIMMED
             || !lpPrimFlagsLft->bIdentElem)
                goto LEFT_OPERAND_DOMAIN_EXIT;

            // Get the identity element
            aplFloatIdent = lpPrimIdentLft->fIdentElem;

            // If either arg is a global numeric, ...
            if (IsGlbNum (aplTypeLft)
             || IsGlbNum (aplTypeRht))
                aplTypeRes = aTypePromote[aplTypeLft][aplTypeRht];
            else
            // If the identity element is Boolean or we're prototyping,
            //   the result is, too
            if (lpPrimIdentLft->IsBool
             || lpPrimProtoLft)
                aplTypeRes = ARRAY_BOOL;
            else
                aplTypeRes = ARRAY_FLOAT;

            // Mark as reducing empty array with a primitive scalar dyadic function
            //   to produce its identity element
            bNrmIdent = TRUE;
        } else
        {
            // The result is nested
            aplTypeRes = ARRAY_NESTED;

            // Mark as reducing empty array with a primitive or
            //   user-defined function/operator
            //   to produce its identity element
            bPrimIdent = TRUE;
        } // End IF/ELSE
    } else
    // If the comparison function is scalar dyadic, and
    //   and there's no right operand axis,
    //   both args are simple non-hetero or global numeric, and
    //   the reduction function is scalar dyadic, ...
    if (lpPrimFlagsRht->bDydScalar
     && lptkAxisRht EQ NULL
     && IsSimpleNHGlbNum (aplTypeLft)
     && IsSimpleNHGlbNum (aplTypeRht)
     && lpPrimFlagsLft->bDydScalar)
    {
        // Get the left & right arg lpPrimSpec
        lpPrimSpecLft = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];
        lpPrimSpecRht = PrimSpecTab[SymTrans (&lpYYFcnStrRht->tkToken)];

        // Save the dimension demotion flags
        bDimDemote = lpPrimSpecLft->bDydDimDemote
                  && lpPrimSpecRht->bDydDimDemote;

        // Calculate the storage type of the comparison result
        aplTypeCmp =
          (*lpPrimSpecRht->StorageTypeDyd) (aplColsLft,
                                           &aplTypeLft,
                                           &lpYYFcnStrRht->tkToken,
                                            aplFrstRht,
                                           &aplTypeRht);
        if (IsNonceType (aplTypeCmp))
            goto RIGHT_OPERAND_NONCE_EXIT;

        if (IsErrorType (aplTypeCmp))
            goto RIGHT_OPERAND_DOMAIN_EXIT;
        // For the moment, APA is treated as INT
        if (IsSimpleAPA (aplTypeCmp))
            aplTypeCmp = ARRAY_INT;
RESTART_INNERPROD_CMP:
        // Calculate the storage type of the reduction result
        aplTypeRes =
          (*lpPrimSpecLft->StorageTypeDyd) (aplInnrMax,
                                           &aplTypeCmp,
                                           &lpYYFcnStrLft->tkToken,
                                            aplInnrMax,
                                           &aplTypeCmp);
        if (IsNonceType (aplTypeRes))
            goto LEFT_OPERAND_NONCE_EXIT;

        if (IsErrorType (aplTypeRes))
            goto LEFT_OPERAND_DOMAIN_EXIT;

        // If the result is empty, make it Boolean
        if (IsEmpty (aplNELMRes))
            aplTypeRes = ARRAY_BOOL;
    } else
        // The result storage type is assumed to be NESTED,
        //   but we'll call TypeDemote at the end just in case.
        aplTypeRes = ARRAY_NESTED;
RESTART_INNERPROD_RES:
    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimRes = TranslateArrayTypeToHCDim (aplTypeRes);

    // Calculate the # bytes in each item in the result
    iSizeofRes = TranslateArrayTypeToSizeof (aplTypeRes);

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result
    //***************************************************************
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        lpMemHdrRes
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    if (hGlbLft NE NULL)
        lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);
    if (hGlbRht NE NULL)
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);
    lpMemDimRes = VarArrayBaseToDim (lpMemHdrRes);

    // Fill in the result's dimension
    //   by copying the left arg dimensions (except for the last)
    //   and then the right arg dimensions  (except for the first)
    if (hGlbLft NE NULL)
    {
        // Calc # dimensions to copy
        uDimCopy = max (aplRankLft, 1) - 1;

        // Copy the dimensions
        CopyMemory (lpMemDimRes, lpMemDimLft, (APLU3264) uDimCopy * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += uDimCopy;
    } else
    {
        // Fill in the left arg item token
        tkItmLft.tkFlags.TknType   = TKT_VARIMMED;
        tkItmLft.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeLft);
////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
        tkItmLft.tkData.tkLongest  = aplLongestLft;
    } // End IF/ELSE

    // Fill in common fields
    tkItmLft.tkCharIndex = lptkLftArg->tkCharIndex;

    if (hGlbRht NE NULL)
    {
        // Calc # dimensions to copy
        uDimCopy = max (aplRankRht, 1) - 1;

        // Copy the dimensions
        CopyMemory (lpMemDimRes, &lpMemDimRht[1], (APLU3264) uDimCopy * sizeof (APLDIM));

        // Skip over the copied dimensions
        lpMemDimRes += uDimCopy;
    } else
    {
        // Fill in the right arg item token
        tkItmRht.tkFlags.TknType   = TKT_VARIMMED;
        tkItmRht.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeRht);
////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
        tkItmRht.tkData.tkLongest  = aplLongestRht;
    } // End IF/ELSE

    // Fill in common fields
    tkItmRht.tkCharIndex = lptkRhtArg->tkCharIndex;

    // Skip over the dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);;

    // Check for empty result
    if (IsEmpty (aplNELMRes))
    {
        // Fill in the prototype which is
        //   leftOperand / {first}leftarg rightOperand {first}rightArg

        // If the left arg is an array, ...
        if (hGlbLft NE NULL)
        {
            // If the left arg is empty, get its prototype
            if (IsEmpty (aplNELMLft))
            {
                // Split cases based upon the left arg storage type
                switch (aplTypeLft)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                        // Fill in the left arg item token
                        tkItmLft.tkFlags.TknType   = TKT_VARIMMED;
                        tkItmLft.tkFlags.ImmType   = IMMTYPE_BOOL;
////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmLft.tkData.tkLongest  = 0;

                        break;

                    case ARRAY_CHAR:
                        // Fill in the left arg item token
                        tkItmLft.tkFlags.TknType   = TKT_VARIMMED;
                        tkItmLft.tkFlags.ImmType   = IMMTYPE_CHAR;
////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmLft.tkData.tkLongest  = L' ';

                        break;

                    case ARRAY_NESTED:
                        // Get the first left arg value
                        GetNextValueGlb (hGlbLft,               // The global memory handle
                                         0,                     // Index into item
                                        &hGlbItm,               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                        &aplLongestLft,         // Ptr to result immediate value (may be NULL)
                                        &immTypeItm);           // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        // If the item is an array, ...
                        if (hGlbItm NE NULL)
                        {
                            // Fill in the left arg item token
                            tkItmLft.tkFlags.TknType   = TKT_VARARRAY;
                            tkItmLft.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                            tkItmLft.tkData.tkGlbData  = CopySymGlbDir_PTB (hGlbItm);
                        } else
                        {
                            // Fill in the left arg item token
                            tkItmLft.tkFlags.TknType   = TKT_VARIMMED;
                            tkItmLft.tkFlags.ImmType   = immTypeItm;
////////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                            tkItmLft.tkData.tkLongest  = aplLongestLft;
                        } // End IF/ELSE

                        break;

                    case ARRAY_HC2I:
                    case ARRAY_HC4I:
                    case ARRAY_HC8I:

                    case ARRAY_HC2F:
                    case ARRAY_HC4F:
                    case ARRAY_HC8F:

                    case ARRAY_RAT:
                    case ARRAY_HC2R:
                    case ARRAY_HC4R:
                    case ARRAY_HC8R:

                    case ARRAY_VFP:
                    case ARRAY_HC2V:
                    case ARRAY_HC4V:
                    case ARRAY_HC8V:

                    case ARRAY_ARB:
                    case ARRAY_BA2F:
                    case ARRAY_BA4F:
                    case ARRAY_BA8F:
                        // Fill in the left arg item token
                        tkItmLft.tkFlags.TknType   = TKT_VARARRAY;
                        tkItmLft.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmLft.tkData.tkGlbData  = CopySymGlbDir_PTB (hGlbLft);

                        break;

                    case ARRAY_HETERO:          // Can't occur:  there are no empty HETEROs
                    defstop
                        break;
                } // End SWITCH
            } else
            {
                // Get the first left arg value
                GetNextValueGlb (hGlbLft,               // The global memory handle
                                 0,                     // Index into item
                                &hGlbItm,               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                &aplLongestLft,         // Ptr to result immediate value (may be NULL)
                                &immTypeItm);           // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                // If the item is an array, ...
                if (hGlbItm NE NULL)
                {
                    // Fill in the left arg item token
                    tkItmLft.tkFlags.TknType   = TKT_VARARRAY;
                    tkItmLft.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                    tkItmLft.tkData.tkGlbData  = CopySymGlbDir_PTB (hGlbItm);
                } else
                {
                    // Fill in the left arg item token
                    tkItmLft.tkFlags.TknType   = TKT_VARIMMED;
                    tkItmLft.tkFlags.ImmType   = immTypeItm;
////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                    tkItmLft.tkData.tkLongest  = aplLongestLft;
                } // End IF/ELSE
            } // End IF/ELSE
        } // End IF

        // If the right arg is an array, ...
        if (hGlbRht NE NULL)
        {
            // If the right arg is empty, get its prototype
            if (IsEmpty (aplNELMRht))
            {
                // Split cases based upon the right arg storage type
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                        // Fill in the right arg item token
                        tkItmRht.tkFlags.TknType   = TKT_VARIMMED;
                        tkItmRht.tkFlags.ImmType   = IMMTYPE_BOOL;
////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmRht.tkData.tkLongest  = 0;

                        break;

                    case ARRAY_CHAR:
                        // Fill in the right arg item token
                        tkItmRht.tkFlags.TknType   = TKT_VARIMMED;
                        tkItmRht.tkFlags.ImmType   = IMMTYPE_CHAR;
////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmRht.tkData.tkLongest  = L' ';

                        break;

                    case ARRAY_NESTED:
                        // Get the first right arg value
                        GetNextValueGlb (hGlbRht,               // The global memory handle
                                         0,                     // Index into item
                                        &hGlbItm,               // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                        &aplLongestRht,         // Ptr to result immediate value (may be NULL)
                                        &immTypeItm);           // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                        // If the item is an array, ...
                        if (hGlbItm NE NULL)
                        {
                            // Fill in the left arg item token
                            tkItmRht.tkFlags.TknType   = TKT_VARARRAY;
                            tkItmRht.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                            tkItmRht.tkData.tkGlbData  = CopySymGlbDir_PTB (hGlbItm);
                        } else
                        {
                            // Fill in the left arg item token
                            tkItmRht.tkFlags.TknType   = TKT_VARIMMED;
                            tkItmRht.tkFlags.ImmType   = immTypeItm;
////////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                            tkItmRht.tkData.tkLongest  = aplLongestRht;
                        } // End IF/ELSE

                        break;

                    case ARRAY_HC2I:
                    case ARRAY_HC4I:
                    case ARRAY_HC8I:

                    case ARRAY_HC2F:
                    case ARRAY_HC4F:
                    case ARRAY_HC8F:

                    case ARRAY_RAT:
                    case ARRAY_HC2R:
                    case ARRAY_HC4R:
                    case ARRAY_HC8R:

                    case ARRAY_VFP:
                    case ARRAY_HC2V:
                    case ARRAY_HC4V:
                    case ARRAY_HC8V:

                    case ARRAY_ARB:
                    case ARRAY_BA2F:
                    case ARRAY_BA4F:
                    case ARRAY_BA8F:
                        // Fill in the left arg item token
                        tkItmRht.tkFlags.TknType   = TKT_VARARRAY;
                        tkItmRht.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmRht.tkData.tkGlbData  = CopySymGlbDir_PTB (hGlbRht);

                        break;

                    case ARRAY_HETERO:          // Can't occur:  there are no empty HETEROs
                    defstop
                        break;
                } // End SWITCH
            } else
            {
                // Get the first right arg value
                GetNextValueGlb (hGlbRht,                       // The global memory handle
                                 0,                             // Index into item
                                &hGlbItm,                       // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                &aplLongestRht,                 // Ptr to result immediate value (may be NULL)
                                &immTypeItm);                   // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                // If the item is an array, ...
                if (hGlbItm NE NULL)
                {
                    // Fill in the right arg item token
                    tkItmRht.tkFlags.TknType   = TKT_VARARRAY;
                    tkItmRht.tkFlags.ImmType   = IMMTYPE_ERROR;
////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                    tkItmRht.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbItm);
                } else
                {
                    // Fill in the right arg item token
                    tkItmRht.tkFlags.TknType   = TKT_VARIMMED;
                    tkItmRht.tkFlags.ImmType   = immTypeItm;
////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                    tkItmRht.tkData.tkLongest  = aplLongestRht;
                } // End IF/ELSE
            } // End IF/ELSE
        } // End IF

        // If the right operand has no prototype function, ...
        if (lpPrimProtoRht EQ NULL)
        {
            // Get the appropriate prototype function ptr
            lpPrimProtoRht = GetPrototypeFcnPtr (GetLftFcnTkn (lpYYFcnStrRht));
            if (lpPrimProtoRht EQ NULL)
                goto RIGHT_OPERAND_NONCE_EXIT;
        } // End IF

        // Execute the right operand between the left & right items

        // Note that we cast the function strand to LPTOKEN
        //   to bridge the two types of calls -- one to a primitive
        //   function which takes a function token, and one to a
        //   primitive operator which takes a function strand
        lpYYRes = (*lpPrimProtoRht) (&tkItmLft,         // Ptr to left arg token
                            (LPTOKEN) lpYYFcnStrRht,    // Ptr to right operand function strand
                                     &tkItmRht,         // Ptr to right arg token
                                      lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
        // Free the left & right arg tokens
        FreeResultTkn (&tkItmLft);
        FreeResultTkn (&tkItmRht);

        // If it succeeded, ...
        if (lpYYRes NE NULL)
        {
            // Check for NoValue
            if (IsTokenNoValue (&lpYYRes->tkToken))
            {
                // Free the YYRes (but not the storage)
                YYFree (lpYYRes); lpYYRes = NULL;

                goto VALUE_EXIT;
            } // End IF

            // If the result is nested, ...
            if (IsNested (aplTypeRes))
            {
                // If the result is an immediate, ...
                if (lpYYRes->tkToken.tkFlags.TknType EQ TKT_VARIMMED)
                {
                    *((LPAPLNESTED) lpMemRes)++ =
                    lpSymTmp =
                      MakeSymEntry_EM (lpYYRes->tkToken.tkFlags.ImmType,    // Immediate type
                                      &lpYYRes->tkToken.tkData.tkLongest,   // Ptr to immediate value
                                      &lpYYRes->tkToken);                   // Ptr to function token
                    if (lpSymTmp EQ NULL)
                        goto ERROR_EXIT;
                } else
                    *((LPAPLNESTED) lpMemRes)++ =
                      CopySymGlbDir_PTB (lpYYRes->tkToken.tkData.tkGlbData);
            } // End IF

            // Free the result item
            FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
        } else
            goto ERROR_EXIT;

        goto YYALLOC_EXIT;
    } // End IF

    // If the left arg is not immediate, ...
    if (lpMemHdrLft NE NULL)
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    else
        lpMemLft = &aplLongestLft;

    // If the right arg is not immediate, ...
    if (lpMemHdrRht NE NULL)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        lpMemRht = &aplLongestRht;

    // If the left arg is APA, fill in the offset and multiplier
    if (IsSimpleAPA (aplTypeLft))
    {
#define lpAPA       ((LPAPLAPA) lpMemLft)
        // Get the APA parameters
        apaOffLft = lpAPA->Off;
        apaMulLft = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the right arg is APA, fill in the offset and multiplier
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // The left arg is treated as a two-dimensional array of shape
    //   aplRestLft aplColsLft

    // The right arg is treated as a two-dimensional array of shape
    //   aplFrstRht aplRestRht

    // If this is a primitive or user-defined function/operator identity element, ...
    if (bPrimIdent)
    {
        if (!FillIdentityElement_EM (lpMemRes,              // Ptr to result global memory
                                     aplNELMRes,            // Result NELM
                                     aplTypeRes,            // Result storage type
                                     lpYYFcnStrLft,         // Ptr to left operand function strand
                                     lptkLftArg,            // Ptr to left arg token
                                     lpYYFcnStrRht,         // Ptr to right operand function strand (may be NULL if Scan)
                                     lptkRhtArg,            // Ptr to right arg token
                                    &bNoDisplay))           // Ptr to TRUE iff the result should be marked as NoDisplay
            goto ERROR_EXIT;
    } else
    // If this is primitive scalar dyadic function identity element, ...
    if (bNrmIdent)
    {
        // The zero case is done (GHND)

        // If we're not doing prototypes, ...
        if (lpPrimProtoLft EQ NULL)
        {
            // Check for Boolean identity element
            if (lpPrimIdentLft->IsBool)
            {
                // Check for identity element 1
                //  or is a global numeric
                if (lpPrimIdentLft->bIdentElem
                 || IsGlbNum (aplTypeRes))
                // Split cases based upon the result storage type
                switch (aplTypeRes)
                {
                    APLNELM uNELMRes;

                    case ARRAY_BOOL:
                        // Calculate the # bytes in the result, rounding up
                        uNELMRes = (aplNELMRes + (NBIB - 1)) >> LOG2NBIB;

                        for (uRes = 0; uRes < uNELMRes; uRes++)
                            *((LPAPLBOOL) lpMemRes)++ = 0xFF;
                        break;

                    case ARRAY_HC2I:
                    case ARRAY_HC4I:
                    case ARRAY_HC8I:
                        for (uRes = 0; uRes < aplNELMRes; uRes++)
                        {
                            // Set the real part of the result
                            ((LPAPLHC8I) lpMemRes)->parts[0] = lpPrimIdentLft->bIdentElem;

                            // Skip to the next item
                            ((LPBYTE) lpMemRes) += iSizeofRes;
                        } // End FOR

                        break;

                    case ARRAY_HC2F:
                    case ARRAY_HC4F:
                    case ARRAY_HC8F:
                        for (uRes = 0; uRes < aplNELMRes; uRes++)
                        {
                            // Set the real part of the result
                            ((LPAPLHC8F) lpMemRes)->parts[0] = (APLFLOAT) lpPrimIdentLft->bIdentElem;

                            // Skip to the next item
                            ((LPBYTE) lpMemRes) += iSizeofRes;
                        } // End FOR

                        break;

                    case ARRAY_RAT:
                    case ARRAY_HC2R:
                    case ARRAY_HC4R:
                    case ARRAY_HC8R:
                        for (uRes = 0; uRes < aplNELMRes; uRes++)
                        {
                            // Initialize the result to 0/1
                            mphcXr_init (lpMemRes, iHCDimRes);

                            // Set the real part of the result
                            mpq_set_ui (&((LPAPLHC8R) lpMemRes)->parts[0], lpPrimIdentLft->bIdentElem, 1);

                            // Skip to the next item
                            ((LPBYTE) lpMemRes) += iSizeofRes;
                        } // End FOR

                        break;

                    case ARRAY_VFP:
                    case ARRAY_HC2V:
                    case ARRAY_HC4V:
                    case ARRAY_HC8V:
                        for (uRes = 0; uRes < aplNELMRes; uRes++)
                        {
                            // Initialize the result to 0
                            mphcXv_init0 (lpMemRes, iHCDimRes);

                            // Set the real part of the result
                            mpfr_set_ui (&((LPAPLHC8V) lpMemRes)->parts[0], lpPrimIdentLft->bIdentElem, MPFR_RNDN);

                            // Skip to the next item
                            ((LPBYTE) lpMemRes) += iSizeofRes;
                        } // End FOR

                        break;

                    case ARRAY_ARB:
                    case ARRAY_BA2F:
                    case ARRAY_BA4F:
                    case ARRAY_BA8F:
                        for (uRes = 0; uRes < aplNELMRes; uRes++)
                        {
                            // Initialize the result to 0
                            arbX_init (lpMemRes, iHCDimRes);

                            // Set the real part of the result
                            arb_set_sx (&((LPAPLBA8F) lpMemRes)->parts[0], lpPrimIdentLft->bIdentElem);

                            // Skip to the next item
                            ((LPBYTE) lpMemRes) += iSizeofRes;
                        } // End FOR

                        break;

                    defstop
                        break;
                } // End IF/SWITCH
            } else
            {
                // The identity element is FLT

                // Split cases based upon the result storage type
                switch (aplTypeRes)
                {
                    case ARRAY_FLOAT:
                    case ARRAY_HC2F:
                    case ARRAY_HC4F:
                    case ARRAY_HC8F:
                        for (uRes = 0; uRes < aplNELMRes; uRes++)
                        {
                            // Set the real part of the result
                            ((LPAPLHC8F) lpMemRes)->parts[0] = lpPrimIdentLft->fIdentElem;

                            // Skip to the next item
                            ((LPBYTE) lpMemRes) += iSizeofRes;
                        } // End FOR

                        break;

                    case ARRAY_VFP:
                    case ARRAY_HC2V:
                    case ARRAY_HC4V:
                    case ARRAY_HC8V:
                        for (uRes = 0; uRes < aplNELMRes; uRes++)
                        {
                            // Initialize the result to 0
                            mphcXv_init0 (lpMemRes, iHCDimRes);

                            // Set the real part of the result
                            mpfr_set_d (&((LPAPLHC8V) lpMemRes)->parts[0], lpPrimIdentLft->fIdentElem, MPFR_RNDN);

                            // Skip to the next item
                            ((LPBYTE) lpMemRes) += iSizeofRes;
                        } // End FOR

                        break;

                    case ARRAY_ARB:
                    case ARRAY_BA2F:
                    case ARRAY_BA4F:
                    case ARRAY_BA8F:
                        for (uRes = 0; uRes < aplNELMRes; uRes++)
                        {
                            // Initialize the result to 0/0
                            arbX_init (lpMemRes, iHCDimRes);

                            // Set the real part of the result
                            arb_set_d (&((LPAPLBA8F) lpMemRes)->parts[0], lpPrimIdentLft->fIdentElem);

                            // Skip to the next item
                            ((LPBYTE) lpMemRes) += iSizeofRes;
                        } // End FOR

                        break;

                    case ARRAY_RAT:
                    case ARRAY_HC2R:
                    case ARRAY_HC4R:
                    case ARRAY_HC8R:
                        //  Promote the type to handle a FLT result
                        aplTypeRes++;   // ***ASSUME*** -- order of ARRAY_TYPES allows this

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                        // We no longer need this storage
                        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                        goto RESTART_INNERPROD_RES;

                    defstop
                        break;
                } // End IF/SWITCH
            } // End IF/ELSE
        } // End IF
    } else
    // If the result is simple non-hetero or global numeric, ...
    if (IsSimpleNHGlbNum (aplTypeRes))
    {
        APLUINT  uInnLft,               // Index into left arg
                 uInnRht;               // ...        right ...
        TOKEN    tkRes = {0};           // Temporary token result
        UINT     uBitIndex;             // Bit index for looping through Boolean result
        HGLOBAL  lpSymGlbLft = NULL,    // Ptr to left arg global numeric
                 lpSymGlbRht = NULL,    // ...    right ...
                 lpSymGlbCmpLft = NULL, // ...    Left  comparison arg global numeric
                 lpSymGlbCmpRht = NULL; // ...    right ...
        APLSTYPE aplTypeNew,            // New storage type
                 aplTypeCmpRht;         // Comparison/Reduction storage type

        // Initialize the bit index
        uBitIndex = 0;

        // Get the common storage type between the left & right args
        aplTypeCom = aTypePromote[aplTypeLft][aplTypeRht];

        // Initialize the <atCmpRht> storage type as the Comparison storage type
        aplTypeCmpRht = aplTypeCmp;

        // Trundle through the left & right arg remaining dimensions
        for (uOutLft = 0; uOutLft < aplRestLft; uOutLft++)
        for (uOutRht = 0; uOutRht < aplRestRht; uOutRht++)
        {
////////////// Calc result index -- unneeded as we compute the result in row major order
////////////uRes = 1 * uOutRht + aplRestRht * uOutLft;
////////////
            // Trundle through the inner dimensions, back to front
            for (iInnMax = aplInnrMax - 1; iInnMax >= 0; iInnMax--)
            {
                // Check for Ctrl-Break
                if (CheckCtrlBreak (lpbCtrlBreak))
                    goto ERROR_EXIT;

                // Calc left inner index, taking into account scalar extension
                if (IsUnitDim (aplColsLft))
                    uInnLft = 1 * 0       + aplColsLft * uOutLft;
                else
                    uInnLft = 1 * iInnMax + aplColsLft * uOutLft;

                // Calc right inner index, taking into account scalar extension
                if (IsUnitDim (aplFrstRht))
                    uInnRht = 1 * uOutRht + aplRestRht * 0      ;
                else
                    uInnRht = 1 * uOutRht + aplRestRht * iInnMax;

////////////////// Promote the left arg to the common type
////////////////(*aTypeActPromote[aplTypeLft][aplTypeCom]) (lpMemLft, uInnLft, &atLft);
                // Copy the left arg to the ALLTYPE var
                (*aTypeActPromote[aplTypeLft][aplTypeLft]) (lpMemLft, uInnLft, &atLft);

////////////////// Promote the right arg to the common storage type
////////////////(*aTypeActPromote[aplTypeRht][aplTypeCom]) (lpMemRht, uInnRht, &atRht);
                // Copy the right arg to the ALLTYPE var
                (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, uInnRht, &atRht);

                // Execute the comparison function between the left and right args
                bRet =
                    PrimFnDydSiScSiScSub_EM (&tkRes,                        // Result token
                                              NULL,                         // Result as ALLTYPES (may be NULL)
                                             &lpYYFcnStrRht->tkToken,       // Comparison function
                                              NULL,                         // Result global memory handle (may be NULL)
                                              aplTypeCmp,                   // Comparison storage type
                                              aplTypeLft,                   // Left arg storage type
                                             &atLft,                        // ...      as ALLTYPES
                                              aplTypeRht,                   // Right arg storage type
                                             &atRht,                        // ...      as ALLTYPES
                                             &aplTypeNew,                   // New storage type
                                              FALSE,                        // TRUE iff the outer <hGlbRes> is nested
                                              lpPrimSpecRht);               // Ptr to comparison function PRIMSPEC
                // Free the old atLft
                (*aTypeFree[aplTypeLft]) (&atLft, 0);

                // Free the old atRht
                (*aTypeFree[aplTypeRht]) (&atRht, 0);

                if (bRet)
                {
                    // Check for type promotion
                    if (aplTypeCmp NE aplTypeNew)
                    {
                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                        // We no longer need this resource
                        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                        if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
                        {
                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

                            // Lock the memory to get a ptr to it
                            lpMemHdrLft = MyGlobalLockVar (hGlbLft);
                        } // End IF

                        if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
                        {
                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                            // Lock the memory to get a ptr to it
                            lpMemHdrRht = MyGlobalLockVar (hGlbRht);
                        } // End IF

                        // Save as the new storage type
                        aplTypeCmp = aplTypeNew;

                        goto RESTART_INNERPROD_CMP;
                    } // End IF
                } else
                    goto ERROR_EXIT;

                // If the reduction is on a singleton, ...
                if (IsSingleton (aplInnrMax))
                {
                    // Reduce it
                    lpYYRes2 =
                      PrimOpRedOfSing_EM_YY (&tkRes,                // Ptr to right arg token
                                              lpYYFcnStrOpr,        // Ptr to operator function strand
                                              lpYYFcnStrLft,        // Ptr to left operand
                                              NULL,                 // Ptr to the reduction axis value (may be NULL if scan)
                                              FALSE,                // TRUE iff we should treat as Scan
                                              TRUE,                 // TRUE iff the item must be enclosed before reducing it
                                              bPrototyping);        // TRUE iff prototyping
                    // If it succeeded, ...
                    if (lpYYRes2 NE NULL)
                    {
                        // Check for NoValue
                        if (IsTokenNoValue (&lpYYRes2->tkToken))
                        {
                            // Free the YYRes (but not the storage)
                            YYFree (lpYYRes2); lpYYRes2 = NULL;

                            goto VALUE_EXIT;
                        } // End IF

                        Assert (lpYYRes2->tkToken.tkFlags.TknType EQ TKT_VARIMMED
                             || lpYYRes2->tkToken.tkFlags.TknType EQ TKT_VARARRAY);

                        // Free the previous tkRes before overwriting it
                        FreeResultTkn (&tkRes);

                        // Copy the result to local storage
                        CopyAll (&tkRes, CopyToken (&lpYYRes2->tkToken, FALSE));

                        // Free the result item
                        FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
                    } else
                        goto ERROR_EXIT;
                } else
                // If this is not the first time, do the reduction
                if (iInnMax NE (APLINT) (aplInnrMax - 1))
                {
                    // If the token type is an immediate, ...
                    if (tkRes.tkFlags.TknType EQ TKT_VARIMMED)
                    {
                        // Copy the immediate value to atCmpLft
                        (*aTypeActPromote[aplTypeCmp][aplTypeCmp]) (&tkRes.tkData.tkLongest, 0, &atCmpLft);

                        // Point to it
                        lpSymGlbCmpLft = &atCmpLft;
                    } else
                    {
                        HGLOBAL            hGlb;
                        LPVARARRAY_HEADER  lpMemHdr = NULL;
                        LPVOID             lpMem;

                        // Get the result global ptr
                        GetGlbPtrs_LOCK (&tkRes, &hGlb, &lpMemHdr);

                        Assert (hGlb NE NULL);

                        // Skip over the header and dimensions to the data
                        lpMem = VarArrayDataFmBase (lpMemHdr);

                        // Copy the item to atCmpLft
                        (*aTypeActPromote[aplTypeCmp][aplTypeCmp]) (lpMem, 0, &atCmpLft);

                        // Point to it
                        lpSymGlbCmpLft = &atCmpLft;

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlb); lpMemHdr = NULL;

                        // We no longer need this storage
                        FreeResultGlobalVar (hGlb); hGlb = NULL;
                    } // End IF/ELSE

                    // Execute the left operand between the item result and the accumulated reduction
                    bRet =
                        PrimFnDydSiScSiScSub_EM (&tkRes,                    // Result token
                                                  NULL,                     // Result as ALLTYPES (may be NULL)
                                                 &lpYYFcnStrLft->tkToken,   // Reduction function
                                                  NULL,                     // Result global memory handle (may be NULL)
                                                  aplTypeRes,               // Result storage type
                                                  aplTypeCmp,               // Comparison storage type
                                                 &atCmpLft,                 // Left arg as ALLTYPES
                                                  aplTypeCmpRht,            // Comparison or Reduction storage type
                                                 &atCmpRht,                 // Left arg as ALLTYPES
                                                 &aplTypeNew,               // New storage type
                                                  FALSE,                    // TRUE iff the outer <hGlbRes> is nested
                                                  lpPrimSpecLft);           // Ptr to reduction function PRIMSPEC
                    // Free the old atCmpLft and atCmpRht
                    (*aTypeFree[aplTypeCmp]) (&atCmpLft, 0);
                    (*aTypeFree[aplTypeCmp]) (&atCmpRht, 0);

                    // Save the <atCmpRht> storage type as the reduction storage type <aplTypeRes>
                    aplTypeCmpRht = aplTypeRes;

                    if (bRet)
                    {
                        // Check for type promotion
                        if (aplTypeRes NE aplTypeNew)
                        {
                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

                            // We no longer need this resource
                            FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                            if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
                            {
                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;

                                // Lock the memory to get a ptr to it
                                lpMemHdrLft = MyGlobalLockVar (hGlbLft);
                            } // End IF

                            if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
                            {
                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;

                                // Lock the memory to get a ptr to it
                                lpMemHdrRht = MyGlobalLockVar (hGlbRht);
                            } // End IF

                            // Save as the new storage type
                            aplTypeRes = aplTypeNew;

                            goto RESTART_INNERPROD_RES;
                        } // End IF
                    } else
                        goto ERROR_EXIT;
                } // End IF/ELSE/...

                // Copy the last reduction result as the new reduction right arg

                // If the token type is an immediate, ...
                if (tkRes.tkFlags.TknType EQ TKT_VARIMMED)
                {
                    APLSTYPE aplTypetkRes = TranslateImmTypeToArrayType (tkRes.tkFlags.ImmType);

                    // Copy the immediate value to atCmpRht
                    (*aTypeActPromote[aplTypetkRes][aplTypeCmpRht]) (&tkRes.tkData.tkLongest, 0, &atCmpRht);

                    // Point to it
                    lpSymGlbCmpRht = &atCmpRht;
                } else
                // If the token type is a global numeric, ...
                if (tkRes.tkFlags.TknType EQ TKT_VARARRAY)
                {
                    HGLOBAL            hGlb;
                    LPVARARRAY_HEADER  lpMemHdr = NULL;
                    LPVOID             lpMem;

                    // Get the result global ptr
                    GetGlbPtrs_LOCK (&tkRes, &hGlb, &lpMemHdr);

                    Assert (hGlb NE NULL);

                    // Skip over the header and dimensions to the data
                    lpMem = VarArrayDataFmBase (lpMemHdr);

                    // Copy the item to atCmpRht
                    (*aTypeActPromote[aplTypeCmpRht][aplTypeCmpRht]) (lpMem, 0, &atCmpRht);

                    // Point to it
                    lpSymGlbCmpRht = &atCmpRht;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlb); lpMemHdr = NULL;

                    // We no longer need this storage
                    FreeResultGlobalVar (hGlb); hGlb = NULL;
                } else
                    DbgStop ();         // We should never get here
            } // End FOR

            // Split cases based upon the result storage type
            switch (aplTypeRes)
            {
                case ARRAY_BOOL:
                    // Save in the result
                    *((LPAPLBOOL) lpMemRes) |= (BIT0 & *(LPAPLINT) lpSymGlbCmpRht) << uBitIndex;

                    // Check for end-of-byte
                    if (++uBitIndex EQ NBIB)
                    {
                        uBitIndex = 0;              // Start over
                        ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                    } // End IF

                    break;

                case ARRAY_INT:
                case ARRAY_HC2I:
                case ARRAY_HC4I:
                case ARRAY_HC8I:
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Save the accumulated reduction in the result
                        *((LPAPLINT)   lpMemRes)++ = ((LPAPLHC8I) lpSymGlbCmpRht)->parts[i];

                    break;

                case ARRAY_FLOAT:
                case ARRAY_HC2F:
                case ARRAY_HC4F:
                case ARRAY_HC8F:
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Save the accumulated reduction in the result
                        *((LPAPLFLOAT) lpMemRes)++ = ((LPAPLHC8F) lpSymGlbCmpRht)->parts[i];

                    break;

                case ARRAY_RAT:
                case ARRAY_HC2R:
                case ARRAY_HC4R:
                case ARRAY_HC8R:
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Save the accumulated reduction in the result
                        mpq_init_set (((LPAPLRAT) lpMemRes)++, &((LPAPLHC8R) lpSymGlbCmpRht)->parts[i]);

                    break;

                case ARRAY_VFP:
                case ARRAY_HC2V:
                case ARRAY_HC4V:
                case ARRAY_HC8V:
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Save the accumulated reduction in the result
                        mpfr_init_copy (((LPAPLVFP) lpMemRes)++, &((LPAPLHC8V) lpSymGlbCmpRht)->parts[i]);

                    break;

                case ARRAY_ARB:
                case ARRAY_BA2F:
                case ARRAY_BA4F:
                case ARRAY_BA8F:
                    // Loop through all of the parts
                    for (i = 0; i < iHCDimRes; i++)
                        // Save the accumulated reduction in the result
                        arb_init_set (((LPAPLARB) lpMemRes)++, &((LPAPLBA8F) lpSymGlbCmpRht)->parts[i]);

                    break;

                defstop
                    break;
            } // End SWITCH

            // Free the old atCmpLft and atCmpRht
////////////(*aTypeFree[aplTypeCmp]) (&atCmpLft, 0);
            (*aTypeFree[aplTypeCmp]) (&atCmpRht, 0);
        } // End FOR/FOR
    } else
    {
        // Trundle through the left & right arg remaining dimensions
        for (uOutLft = 0; uOutLft < aplRestLft; uOutLft++)
        for (uOutRht = 0; uOutRht < aplRestRht; uOutRht++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

////////////// Calc result index -- unneeded as we compute the result in row major order
////////////uRes = 1 * uOutRht + aplRestRht * uOutLft;
////////////
            // Trundle through the inner dimensions, back to front
            for (iInnMax = aplInnrMax - 1; iInnMax >= 0; iInnMax--)
            {
                APLUINT uInnLft,            // Index into left arg
                        uInnRht;            // ...        right ...

                // Calc left inner index, taking into account scalar extension
                if (IsUnitDim (aplColsLft))
                    uInnLft = 1 * 0       + aplColsLft * uOutLft;
                else
                    uInnLft = 1 * iInnMax + aplColsLft * uOutLft;

                // Calc right inner index, taking into account scalar extension
                if (IsUnitDim (aplFrstRht))
                    uInnRht = 1 * uOutRht + aplRestRht * 0      ;
                else
                    uInnRht = 1 * uOutRht + aplRestRht * iInnMax;

                // If the left arg is an array, ...
                if (hGlbLft NE NULL)
                {
                    // Get the next left arg value
                    GetNextValueGlb (hGlbLft,                       // The global memory handle
                                     uInnLft,                       // Index into item
                                    &hGlbItm,                       // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                    &aplLongestLft,                 // Ptr to result immediate value (may be NULL)
                                    &immTypeItm);                   // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                    // If the item is an array, ...
                    if (hGlbItm NE NULL)
                    {
                        // Fill in the left arg item token
                        tkItmLft.tkFlags.TknType = TKT_VARARRAY;
                        tkItmLft.tkFlags.ImmType = IMMTYPE_ERROR;
////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmLft.tkData.tkGlbData = CopySymGlbDir_PTB (hGlbItm);
                    } else
                    {
                        // Fill in the left arg item token
                        tkItmLft.tkFlags.TknType = TKT_VARIMMED;
                        tkItmLft.tkFlags.ImmType = immTypeItm;
////////////////////////tkItmLft.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmLft.tkData.tkLongest = aplLongestLft;
                    } // End IF/ELSE
                } // End IF

                // If the right arg is an array, ...
                if (hGlbRht NE NULL)
                {
                    // Get the next right arg value
                    GetNextValueGlb (hGlbRht,                       // The global memory handle
                                     uInnRht,                       // Index into item
                                    &hGlbItm,                       // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                    &aplLongestRht,                 // Ptr to result immediate value (may be NULL)
                                    &immTypeItm);                   // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                    // If the item is an array, ...
                    if (hGlbItm NE NULL)
                    {
                        // Fill in the right arg item token
                        tkItmRht.tkFlags.TknType = TKT_VARARRAY;
                        tkItmRht.tkFlags.ImmType = IMMTYPE_ERROR;
////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmRht.tkData.tkGlbData = CopySymGlbDir_PTB (hGlbItm);
                    } else
                    {
                        // Fill in the right arg item token
                        tkItmRht.tkFlags.TknType = TKT_VARIMMED;
                        tkItmRht.tkFlags.ImmType = immTypeItm;
////////////////////////tkItmRht.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
                        tkItmRht.tkData.tkLongest = aplLongestRht;
                    } // End IF/ELSE
                } // End IF

                // Execute the right operand between the left & right items
                if (lpPrimProtoRht NE NULL)
                    // Note that we cast the function strand to LPTOKEN
                    //   to bridge the two types of calls -- one to a primitive
                    //   function which takes a function token, and one to a
                    //   primitive operator which takes a function strand
                    lpYYRes =
                      (*lpPrimProtoRht) (&tkItmLft,         // Ptr to left arg token
                                (LPTOKEN) lpYYFcnStrRht,    // Ptr to right operand function strand
                                         &tkItmRht,         // Ptr to right arg token
                                          lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                else
                    lpYYRes =
                      ExecFuncStr_EM_YY (&tkItmLft,         // Ptr to left arg token
                                          lpYYFcnStrRht,    // Ptr to right operand function strand
                                         &tkItmRht,         // Ptr to right arg token
                                          lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                // Free the left & right arg tokens
                FreeResultTkn (&tkItmLft);
                FreeResultTkn (&tkItmRht);

                // If it succeeded, ...
                if (lpYYRes NE NULL)
                {
                    // Check for NoValue
                    if (IsTokenNoValue (&lpYYRes->tkToken))
                    {
                        // Free the YYRes (but not the storage)
                        YYFree (lpYYRes); lpYYRes = NULL;

                        goto VALUE_EXIT;
                    } // End IF

                    // If the reduction is on a singleton, ...
                    if (IsSingleton (aplInnrMax))
                    {
                        // Reduce it
                        lpYYRes2 =
                          PrimOpRedOfSing_EM_YY (&lpYYRes->tkToken,     // Ptr to right arg token
                                                  lpYYFcnStrOpr,        // Ptr to operator function strand
                                                  lpYYFcnStrLft,        // Ptr to left operand
                                                  NULL,                 // Ptr to the reduction axis value (may be NULL if scan)
                                                  FALSE,                // TRUE iff we should treat as Scan
                                                  TRUE,                 // TRUE iff the item must be enclosed before reducing it
                                                  bPrototyping);        // TRUE iff prototyping
                        // Free the result item
                        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;

                        // If it succeeded, ...
                        if (lpYYRes2 NE NULL)
                        {
                            // Check for NoValue
                            if (IsTokenNoValue (&lpYYRes2->tkToken))
                            {
                                // Free the YYRes (but not the storage)
                                YYFree (lpYYRes2); lpYYRes2 = NULL;

                                goto VALUE_EXIT;
                            } // End IF

                            Assert (lpYYRes2->tkToken.tkFlags.TknType EQ TKT_VARIMMED
                                 || lpYYRes2->tkToken.tkFlags.TknType EQ TKT_VARARRAY);

                            // Copy the result to the accumulated reduction token
                            CopyAll (&tkItmRed, CopyToken (&lpYYRes2->tkToken, FALSE));

                            // Free the result item (but not the storage)
                            YYFree (lpYYRes2); lpYYRes2 = NULL;
                        } else
                            goto ERROR_EXIT;
                    } else
                    // If this is the first time, there's no reduction
                    if (iInnMax EQ (APLINT) (aplInnrMax - 1))
                    {
                        // Copy the result to the accumulated reduction token
                        CopyAll (&tkItmRed, &lpYYRes->tkToken);

                        // Free the result item (but not the storage)
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } else
                    {
                        // Execute the left operand between the item result and the accumulated reduction
                        if (lpPrimProtoLft NE NULL)
                            // Note that we cast the function strand to LPTOKEN
                            //   to bridge the two types of calls -- one to a primitive
                            //   function which takes a function token, and one to a
                            //   primitive operator which takes a function strand
                            lpYYRes2 =
                              (*lpPrimProtoLft) (&lpYYRes->tkToken, // Ptr to left arg token
                                        (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                                 &tkItmRed,         // Ptr to right arg token
                                                  lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                        else
                            lpYYRes2 =
                              ExecFuncStr_EM_YY (&lpYYRes->tkToken, // Ptr to left arg token
                                                  lpYYFcnStrLft,    // Ptr to left operand function strand
                                                 &tkItmRed,         // Ptr to right arg token
                                                  lptkAxisOpr);     // Ptr to operator axis token (may be NULL)
                        // Free the result item & reduction tokens
                        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
                        FreeResultTkn (&tkItmRed);

                        // If it succeeded, ...
                        if (lpYYRes2 NE NULL)
                        {
                            // Check for NoValue
                            if (IsTokenNoValue (&lpYYRes2->tkToken))
                            {
                                // Free the YYRes (but not the storage)
                                YYFree (lpYYRes2); lpYYRes2 = NULL;

                                goto VALUE_EXIT;
                            } // End IF

                            // Copy the result to the accumulated reduction token
                            CopyAll (&tkItmRed, &lpYYRes2->tkToken);

                            // Free the result item (but not the storage)
                            YYFree (lpYYRes2); lpYYRes2 = NULL;
                        } else
                            goto ERROR_EXIT;
                    } // End IF/ELSE/...
                } else
                {
                    // If this is not the first time, free the reduction result
                    if (iInnMax NE (APLINT) (aplInnrMax - 1))
                        FreeResultTkn (&tkItmRed);

                    goto ERROR_EXIT;
                } // End IF/ELSE
            } // End FOR

            // Save the accumulated reduction in the result

            // If the accumulated reduction is an immediate, ...
            if (tkItmRed.tkFlags.TknType EQ TKT_VARIMMED)
            {
                *((LPAPLNESTED) lpMemRes)++ =
                lpSymTmp =
                  MakeSymEntry_EM (tkItmRed.tkFlags.ImmType,    // Immediate type
                                  &tkItmRed.tkData.tkLongest,   // Ptr to immediate value
                                  &tkItmRed);                   // Ptr to function token
                if (lpSymTmp EQ NULL)
                    goto ERROR_EXIT;
            } else
                *((LPAPLNESTED) lpMemRes)++ =
                  CopySymGlbDir_PTB (tkItmRed.tkData.tkGlbData);
            // Free the accumulated reduction token
            FreeResultTkn (&tkItmRed);
        } // End FOR/FOR
    } // End IF/ELSE/...
YYALLOC_EXIT:
    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
    lpYYRes->tkToken.tkFlags.NoDisplay = bNoDisplay;
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken, bDimDemote);

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

LEFT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

VALENCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
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
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbPro NE NULL)
    {
        FreeResultGlobalVar (hGlbPro); hGlbPro = NULL;
    } // End IF

    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimOpDydDotCommon_EM_YY


//***************************************************************************
//  $ExecDydProto_EM
//
//  Execute a dyadic function (strand) between left & right prototypes
//***************************************************************************

UBOOL ExecDydProto_EM
    (LPTOKEN       lptkLftArg,              // Ptr to left arg token
     LPPL_YYSTYPE  lpYYFcnStrLft,           // Ptr to left operand function strand (for tkCharIndex only)
     LPTOKEN       lptkRhtArg,              // Ptr to right arg token
     LPPL_YYSTYPE  lpYYFcnStrRht,           // Ptr to right operand function strand
     LPPRIMFNS     lpPrimProtoRht,          // Ptr to right operand prototype function
     LPPL_YYSTYPE *lplpYYRes)               // Ptr to ptr to the result

{
    APLLONGEST    aplLongestProLft,         // Left arg prototype immediate value
                  aplLongestProRht;         // Right ...
    HGLOBAL       hGlbProLft,               // Left arg prototype global memory handle
                  hGlbProRht;               // Right ...
    TOKEN         tkProLft = {0},           // Left arg prototype token
                  tkProRht = {0};           // Right ...
    IMM_TYPES     immTypeProLft,            // Left arg prototype immediate type
                  immTypeProRht;            // Right ...

    // Get the prototype of the left arg ({take} L)
    GetFirstValueToken (lptkLftArg,         // Ptr to left arg token
                        NULL,               // Ptr to integer result
                        NULL,               // Ptr to float ...
                        NULL,               // Ptr to WCHAR ...
                       &aplLongestProLft,   // Ptr to longest ...
                       &hGlbProLft,         // Ptr to lpSym/Glb ...
                       &immTypeProLft,      // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
    // If the prototype is a global, ...
    if (hGlbProLft NE NULL)
    {
        // Setup a token for the left arg prototype to pass to the function
        tkProLft.tkFlags.TknType   = TKT_VARARRAY;
////////tkProLft.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////tkProLft.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkProLft.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbProLft);
        tkProLft.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;
    } else
    {
        // Setup a token for the left arg prototype to pass to the function
        tkProLft.tkFlags.TknType   = TKT_VARIMMED;
        tkProLft.tkFlags.ImmType   = immTypeProLft;
////////tkProLft.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkProLft.tkData.tkLongest  = aplLongestProLft;
        tkProLft.tkCharIndex       = lpYYFcnStrLft->tkToken.tkCharIndex;
    } // End IF/ELSE

    // Get the prototype of the right arg ({take} R)
    GetFirstValueToken (lptkRhtArg,         // Ptr to right arg token
                        NULL,               // Ptr to integer result
                        NULL,               // Ptr to float ...
                        NULL,               // Ptr to WCHAR ...
                       &aplLongestProRht,   // Ptr to longest ...
                       &hGlbProRht,         // Ptr to lpSym/Glb ...
                       &immTypeProRht,      // Ptr to ...immediate type ...
                        NULL);              // Ptr to array type ...
    // If the prototype is a global, ...
    if (hGlbProRht NE NULL)
    {
        // Setup a token for the right arg prototype to pass to the function
        tkProRht.tkFlags.TknType   = TKT_VARARRAY;
////////tkProRht.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////tkProRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkProRht.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbProRht);
        tkProRht.tkCharIndex       = lpYYFcnStrRht->tkToken.tkCharIndex;
    } else

    {
        // Setup a token for the right arg prototype to pass to the function
        tkProRht.tkFlags.TknType   = TKT_VARIMMED;
        tkProRht.tkFlags.ImmType   = immTypeProRht;
////////tkProRht.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkProRht.tkData.tkLongest  = aplLongestProRht;
        tkProRht.tkCharIndex       = lpYYFcnStrRht->tkToken.tkCharIndex;
    } // End IF/ELSE

    // Execute the right operand between the left & right prototypes
    //   (({take} L) RhtOpr {take} R)
    // Note that we cast the function strand to LPTOKEN
    //   to bridge the two types of calls -- one to a primitive
    //   function which takes a function token, and one to a
    //   primitive operator which takes a function strand
    *lplpYYRes =
      (*lpPrimProtoRht) (&tkProLft,         // Ptr to left arg token
                (LPTOKEN) lpYYFcnStrRht,    // Ptr to right operand function strand
                         &tkProRht,         // Ptr to right arg token
                          NULL);            // Ptr to axis token (may be NULL)
    // Free the left & right arg tokens
    FreeResultTkn (&tkProLft);
    FreeResultTkn (&tkProRht);

    // Return TRUE if it worked, FALSE otherwise
    return (*lplpYYRes NE NULL);
} // End ExecDydProto_EM


//***************************************************************************
//  End of File: po_dot.c
//***************************************************************************
