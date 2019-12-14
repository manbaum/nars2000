//***************************************************************************
//  NARS2000 -- Primitive Operator -- Jot
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


//***************************************************************************
//  $PrimIdentOpJot_EM_YY
//
//  Generate an identity element for the dyadic derived function from the
//    primitive operator Jot
//***************************************************************************

LPPL_YYSTYPE PrimIdentOpJot_EM_YY
    (LPTOKEN      lptkRhtOrig,          // Ptr to original right arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)          // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht,         // Ptr to right ...
                 lpYYRes;               // Ptr to result
    HGLOBAL      hGlbMFO;               // Magic function/operator global memory handle

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

    // The (right) identity function for dyadic Jot
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

    // Get the magic function/operator global memory handles
    hGlbMFO = GetMemPTD()->hGlbMFO[MFOE_IdnJot];

    lpYYRes =
      ExecuteMagicOperator_EM_YY (NULL,                     // Ptr to left arg token (may be NULL)
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to operator token
                                  lpYYFcnStrLft,            // Ptr to left operand function strand
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  lpYYFcnStrRht,            // Ptr to right operand function strand (may be NULL)
                                  lptkRhtArg,               // Ptr to right arg token
                                  lptkAxisOpr,              // Ptr to axis token (may be NULL)
                                  hGlbMFO,                  // Magic function/operator global memory handle
                                  NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                  LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
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

ERROR_EXIT:
    return NULL;
} // End PrimIdentOpJot_EM_YY


//***************************************************************************
//  $PrimOpJot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Jot ("compose" and "compose")
//***************************************************************************

LPPL_YYSTYPE PrimOpJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_JOT);

    return
      PrimOpJotCommon_EM_YY (lptkLftArg,    // Ptr to left arg token (may be NULL if niladic/monadic)
                             lpYYFcnStrOpr, // Ptr to operator function strand
                             lptkRhtArg,    // Ptr to right arg token (may be NULL if niladic)
                             FALSE);        // TRUE iff prototyping
} // End PrimOpJot_EM_YY


//***************************************************************************
//  $PrimProtoOpJot_EM_YY
//
//  Generate a prototype for the derived functions from Jot ("compose")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStr,           // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token always NULL)

{
    Assert (lptkAxis EQ NULL);

    return
      PrimOpJotCommon_EM_YY (lptkLftArg,    // Ptr to left arg token
                             lpYYFcnStr,    // Ptr to operator function strand
                             lptkRhtArg,    // Ptr to right arg token
                             TRUE);         // TRUE iff prototyping
} // End PrimProtoOpJot_EM_YY


//***************************************************************************
//  $PrimOpJotCommon_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator Jot ("compose" and "compose")
//***************************************************************************

LPPL_YYSTYPE PrimOpJotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if niladic/monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)         // TRUE iff prototyping

{
    LPPL_YYSTYPE      lpYYFcnStrLft,        // Ptr to left operand function strand
                      lpYYFcnStrRht,        // Ptr to right ...
                      lpYYRes = NULL,       // Ptr to the result
                      lpYYRes2;             // Ptr to secondary result
    UBOOL             bLftOpr,              // TRUE iff left operand is a function/operator
                      bRhtOpr,              //          right ...
                      bRet;                 // TRUE iff the result is valid
    LPPRIMFNS         lpPrimProtoLft;       // Ptr to left operand prototype function
    LPPRIMFNS         lpPrimProtoRht;       // Ptr to right ...
    LPTOKEN           lptkAxisOpr,          // Ptr to axis token (may be NULL)
                      lptkAxisLft,          // Ptr to left operand axis token (may be NULL)
                      lptkAxisRht;          // Ptr to right ...
    HGLOBAL           hGlbLft = NULL,       // Global memory handle
                      hGlbRht = NULL,       // ...
                      hGlbAxis = NULL;      // ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to global memory header
                      lpMemHdrRht = NULL,   // ...
                      lpMemHdrAxis = NULL;  // ...
    LPVOID            lpMemAxis;            // Ptr to axis global memory data
    LPAPLDIM          lpMemDimLft,          // Ptr to dimensions
                      lpMemDimRht;          // ...

    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_JOT);

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

    if (IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    if (IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Test for fcn/opr vs. var
    bLftOpr = IsTknFcnOpr (&lpYYFcnStrLft->tkToken);
    bRhtOpr = IsTknFcnOpr (&lpYYFcnStrRht->tkToken);

    // Get a ptr to the prototype function for the left operand
    if (bPrototyping && bLftOpr)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoLft = GetPrototypeFcnPtr (&lpYYFcnStrLft->tkToken);
        if (lpPrimProtoLft EQ NULL)
            goto LEFT_OPERAND_NONCE_EXIT;
    } else
        lpPrimProtoLft = NULL;

    // Get a ptr to the prototype function for the right operand
    if (bPrototyping && bRhtOpr)
    {
        // Get the appropriate prototype function ptr
        lpPrimProtoRht = GetPrototypeFcnPtr (&lpYYFcnStrRht->tkToken);
        if (lpPrimProtoRht EQ NULL)
            goto RIGHT_OPERAND_NONCE_EXIT;
    } else
        lpPrimProtoRht = NULL;

    // Handle both monadic & dyadic derived functions together

    // Determine if the left & right arg tokens are functions/operators
    // Check for axis operator in the left operand
    if (bLftOpr
     && lpYYFcnStrLft->TknCount > 1
     && IsTknTypeAxis (lpYYFcnStrLft[1].tkToken.tkFlags.TknType))
        lptkAxisLft = &lpYYFcnStrLft[1].tkToken;
    else
        lptkAxisLft = NULL;

    // Check for axis operator in the right operand
    if (bRhtOpr
     && lpYYFcnStrRht->TknCount > 1
     && IsTknTypeAxis (lpYYFcnStrRht[1].tkToken.tkFlags.TknType))
        lptkAxisRht = &lpYYFcnStrRht[1].tkToken;
    else
        lptkAxisRht = NULL;

    // If we're called niladically, ...
    if (lptkRhtArg EQ NULL)
        goto VALENCE_EXIT;

    // Split cases based upon the type (V or F) of
    //   the left and right operands
    switch (bLftOpr * 2 + bRhtOpr * 1)
    {
        case 1 * 2 + 1 * 1:     // F1 Jot F2 -> F1 F2 R or L F1 F2 R
            // Execute the right operand monadically
            //   on the right arg
            if (bPrototyping)
                // Note that we cast the function strand to LPTOKEN
                //   to bridge the two types of calls -- one to a primitive
                //   function which takes a function token, and one to a
                //   primitive operator which takes a function strand
                lpYYRes2 = (*lpPrimProtoRht) (NULL,                     // Ptr to left arg token (may be NULL if niladic/monadic)
                                    (LPTOKEN) lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
                                              lptkAxisRht);             // Ptr to right operand axis token (may be NULL)
            else
                lpYYRes2 = ExecFuncStr_EM_YY (NULL,                     // Ptr to left arg token (may be NULL if niladic/monadic)
                                              lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
                                              lptkAxisRht);             // Ptr to right operand axis token (may be NULL)
            if (lpYYRes2 NE NULL)
            {
                // Execute the left operand dyadically
                //   between the (optional) left arg and the
                //   above result from the right operand.
                if (bPrototyping)
                    // Note that we cast the function strand to LPTOKEN
                    //   to bridge the two types of calls -- one to a primitive
                    //   function which takes a function token, and one to a
                    //   primitive operator which takes a function strand
                    lpYYRes = (*lpPrimProtoLft) (lptkLftArg,            // Ptr to left arg token (may be NULL if niladic/monadic)
                                       (LPTOKEN) lpYYFcnStrLft,         // Ptr to left operand function strand
                                                &lpYYRes2->tkToken,     // Ptr to right arg token (may be NULL if niladic)
                                                 lptkAxisLft);          // Ptr to left operand axis token (may be NULL)
                else
                    lpYYRes = ExecFuncStr_EM_YY (lptkLftArg,            // Ptr to left arg token (may be NULL if niladic/monadic)
                                                 lpYYFcnStrLft,         // Ptr to left operand function strand
                                                &lpYYRes2->tkToken,     // Ptr to right arg token (may be NULL if niladic)
                                                 lptkAxisLft);          // Ptr to left operand axis token (may be NULL)
                FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
            } else
                lpYYRes = NULL;

            break;

        case 0 * 2 + 1 * 1:     // V Jot F -> V F R or SYNTAX ERROR
            // If there's a left arg, ...
            if (lptkLftArg NE NULL)
            {
                // Check for the right operand as / or \ or some Axis operator variant
                if (lpYYFcnStrRht->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
                 && (IsAPLCharSlash (lpYYFcnStrRht->tkToken.tkData.tkChar)
                  || IsAPLCharSlope (lpYYFcnStrRht->tkToken.tkData.tkChar)))
                {
                    HGLOBAL           hGlbMFO;              // Magic function/operator global memory handle
                    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
                    APLNELM           uCnt,                 // Loop counter
                                      aplAxis;
                    APLLONGEST        aplLongestAxis;       // Axis immediate value
                    APLSTYPE          aplTypeLOP,           // Left operand storage type
                                      aplTypeAxis;          // Axis         ...
                    APLRANK           aplRankLft,           // Rank of left arg
                                      aplRankRht,           // ...     right ...
                                      aplRankLOP,           // ...     Left Operand
                                      aplRankAxis;          // ...     Axis
                    APLNELM           aplNELMAxis;
                    TOKEN             tkAxis = {0};         // Temporary Axis token

                    // Get ptr to PerTabData global memory
                    lpMemPTD = GetMemPTD ();

                    // Get the attributes (Type, NELM, and Rank)
                    //   of the left & right args and left operand
                    AttrsOfToken ( lptkLftArg            , NULL       , NULL       , &aplRankLft, NULL);
                    AttrsOfToken ( lptkRhtArg            , NULL       , NULL       , &aplRankRht, NULL);
                    AttrsOfToken (&lpYYFcnStrLft->tkToken, &aplTypeLOP, NULL       , &aplRankLOP, NULL);

                    // If the Axis operator is present on the right Operand, ...
                    if (lptkAxisRht NE NULL)
                    {
                        // Get the attributes (Type, NELM, and Rank)
                        AttrsOfToken ( lptkAxisRht           , &aplTypeAxis, &aplNELMAxis, &aplRankAxis, NULL);

                        // Check for errors in the Axis operator
                        if (IsRank2P (aplRankAxis))
                            goto RIGHT_AXIS_RANK_EXIT;
                        if (!IsSingleton (aplNELMAxis))
                            goto RIGHT_AXIS_LENGTH_EXIT;
                        if (!IsNumeric (aplTypeAxis))
                            goto RIGHT_AXIS_DOMAIN_EXIT;
                    } // End IF

                    // Check for errors in the left operand
                    if (IsRank2P (aplRankLOP))
                        goto LEFT_OPERAND_RANK_EXIT;

                    if (!IsNumeric (aplTypeLOP))
                        goto LEFT_OPERAND_DOMAIN_EXIT;

                    // Get Left & right arg's global ptrs
                    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
                    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

                    // Check for slashbar/slopebar with no explicit axis
                    if (lptkAxisRht EQ NULL
                     && (lpYYFcnStrRht->tkToken.tkData.tkChar EQ UTF16_SLASHBAR
                      || lpYYFcnStrRht->tkToken.tkData.tkChar EQ UTF16_SLOPEBAR))
                    {
                        // Create a first coordinate axis value
                        tkAxis.tkFlags.TknType  = TKT_VARIMMED;
                        tkAxis.tkFlags.ImmType  = IMMTYPE_INT;
                        tkAxis.tkSynObj         = soA;
                        tkAxis.tkData.tkInteger = GetQuadIO ();

                        // Point to a first coordinate axis
                        lptkAxisRht = &tkAxis;
                    } // End IF

                    // Is it Mask or Mesh?
                    if (IsAPLCharSlash (lpYYFcnStrRht->tkToken.tkData.tkChar))
                    {
                        // Check for errors In Mask
                        if (!IsScalar (aplRankLft)
                         && !IsScalar (aplRankRht))
                        {
                            // Must be the same rank
                            if (aplRankLft NE aplRankRht)
                                goto RANK_EXIT;

                            // Skip over the header to dimensions
                            lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);
                            lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

                            // Must be the same shape
                            for (uCnt = 0; uCnt < aplRankLft; uCnt++)
                            if (lpMemDimLft[uCnt] NE lpMemDimRht[uCnt])
                                goto LENGTH_EXIT;
                        } // End IF

                        // Select the magic function/operator global memory handle
                        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_DydMask];
                    } else
                    {
                        // Check for errors in Mesh
                        if (!IsScalar (aplRankLft)
                         && !IsScalar (aplRankRht))
                        {
                            // Must be the same rank
                            if (aplRankLft NE aplRankRht)
                                goto RANK_EXIT;

                            // Skip over the header to dimensions
                            lpMemDimLft = VarArrayBaseToDim (lpMemHdrLft);
                            lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

                            // Get the axis value
                            if (lptkAxisRht NE NULL)
                            {
                                // Get the Axis
                                aplLongestAxis = GetGlbPtrs_LOCK (lptkAxisRht, &hGlbAxis, &lpMemHdrAxis);

                                // If the Axis value is a global, ...
                                if (lpMemHdrAxis NE NULL)
                                    // Point to axis value(s)
                                    lpMemAxis = VarArrayDataFmBase (lpMemHdrAxis);
                                else
                                    // Point to axis value
                                    lpMemAxis = &aplLongestAxis;

                                // Attempt to convert to an integer
                                aplAxis = ConvertToInteger_SCT (aplTypeAxis, lpMemAxis, 0, &bRet);

                                if (!bRet)
                                    goto RIGHT_AXIS_DOMAIN_EXIT;

                                // Convert to origin-0
                                aplAxis -= GetQuadIO ();

                                // The Axis value may be out-or-range, but that'll be caught in the Magic Function
                            } else
                                aplAxis = max (aplRankLft, aplRankRht) - 1;

                            // Must be the same shape except for the axis
                            for (uCnt = 0; uCnt < aplRankLft; uCnt++)
                            if (uCnt NE aplAxis
                             && lpMemDimLft[uCnt] NE lpMemDimRht[uCnt])
                                goto LENGTH_EXIT;
                        } // End IF

                        // Select the magic function/operator global memory handle
                        hGlbMFO = lpMemPTD->hGlbMFO[MFOE_DydMesh];
                    } // End IF/ELSE

                    //  Use an internal magic function/operator.
                    lpYYRes =
                      ExecuteMagicOperator_EM_YY (lptkLftArg,               // Ptr to left arg token (may be NULL)
                                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                                  lpYYFcnStrLft,            // Ptr to left operand function strand
                                                  lpYYFcnStrOpr,            // Ptr to function strand
                                                  NULL,                     // Ptr to right operand function strand (may be NULL)
                                                  lptkRhtArg,               // Ptr to right arg token
                                                  lptkAxisRht,              // Ptr to axis token (may be NULL)
                                                  hGlbMFO,                  // Magic function/operator global memory handle
                                                  NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                   bPrototyping ? LINENUM_PRO               // Starting line # type (see LINE_NUMS)
                                                : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
                    break;
                } else
                    // Signal a SYNTAX ERROR
                    goto LEFT_SYNTAX_EXIT;
            } // End IF

            // Execute the right operand dyadically
            //   between the left operand and the right arg.
            if (bPrototyping)
                // Note that we cast the function strand to LPTOKEN
                //   to bridge the two types of calls -- one to a primitive
                //   function which takes a function token, and one to a
                //   primitive operator which takes a function strand
                lpYYRes = (*lpPrimProtoRht) (&lpYYFcnStrLft->tkToken,   // Ptr to left arg token (may be NULL if niladic/monadic)
                                    (LPTOKEN) lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
                                              lptkAxisRht);             // Ptr to right operand axis token (may be NULL)
            else
                lpYYRes = ExecFuncStr_EM_YY (&lpYYFcnStrLft->tkToken,   // Ptr to left arg token (may be NULL if niladic/monadic)
                                              lpYYFcnStrRht,            // Ptr to right operand function strand
                                              lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
                                              lptkAxisRht);             // Ptr to right operand axis token (may be NULL)
            break;

        case 1 * 2 + 0 * 1:     // F Jot V -> R F V
            // If there's a left arg, signal a SYNTAX ERROR
            if (lptkLftArg NE NULL)
                goto LEFT_SYNTAX_EXIT;

            // Execute the left operand dyadically
            //   between the right arg and the right operand.
            if (bPrototyping)
                // Note that we cast the function strand to LPTOKEN
                //   to bridge the two types of calls -- one to a primitive
                //   function which takes a function token, and one to a
                //   primitive operator which takes a function strand
                lpYYRes = (*lpPrimProtoLft) (lptkRhtArg,                // Ptr to left arg token (may be NULL if niladic/monadic)
                                   (LPTOKEN) lpYYFcnStrLft,             // Ptr to left operand function strand
                                            &lpYYFcnStrRht->tkToken,    // Ptr to right arg token (may be NULL if niladic)
                                             lptkAxisLft);              // Ptr to left operand axis token (may be NULL)
            else
                lpYYRes = ExecFuncStr_EM_YY (lptkRhtArg,                // Ptr to left arg token (may be NULL if niladic/monadic)
                                             lpYYFcnStrLft,             // Ptr to left operand function strand
                                            &lpYYFcnStrRht->tkToken,    // Ptr to right arg token (may be NULL if niladic)
                                             lptkAxisLft);              // Ptr to left operand axis token (may be NULL)
            break;

        case 0 * 2 + 0 * 1:     // V op2 V -> SYNTAX ERROR
            goto LEFT_SYNTAX_EXIT;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxisOpr);
    goto ERROR_EXIT;

RIGHT_AXIS_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkAxisRht);
    goto ERROR_EXIT;

RIGHT_AXIS_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkAxisRht);
    goto ERROR_EXIT;

RIGHT_AXIS_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkAxisRht);
    goto ERROR_EXIT;

LEFT_OPERAND_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
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

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // If the axis is a global, ...
    if (hGlbAxis NE NULL && lpMemHdrAxis NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbAxis); lpMemHdrAxis = NULL;
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
} // End PrimOpJotCommon_EM_YY


//***************************************************************************
//  Magic function/operator for dyadic mask and mesh
//
//  Dyadic Mask & Mesh
//***************************************************************************

#include "mf_jot.h"


//***************************************************************************
//  End of File: po_jot.c
//***************************************************************************
