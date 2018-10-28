//***************************************************************************
//  NARS2000 -- Primitive Operator -- DieresisStar
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
#include "headers.h"


//***************************************************************************
//  $PrimOpDieresisStar_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator DieresisStar (ERROR and "Power")
//***************************************************************************

LPPL_YYSTYPE PrimOpDieresisStar_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DIERESISSTAR);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return
          PrimOpMonDieresisStar_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                       lptkRhtArg);     // Ptr to right arg token (may be NULL if niladic)
    else
        return
          PrimOpDydDieresisStar_EM_YY (lptkLftArg,      // Ptr to left arg token
                                       lpYYFcnStrOpr,   // Ptr to operator function strand
                                       lptkRhtArg);     // Ptr to right arg token
} // End PrimOpDieresisStar_EM_YY


//***************************************************************************
//  $PrimProtoOpDieresisStar_EM_YY
//
//  Generate a prototype for the derived functions from
//    dyadic operator DieresisStar (ERROR and "Power")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpDieresisStar_EM_YY
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
        // Called niladically/monadically
        //***************************************************************
        return
          PrimOpMonDieresisStar_EM_YY (lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg);         // Ptr to right arg token (may be NULL if niladic)
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return
          PrimOpDydDieresisStarCommon_EM_YY (lptkLftArg,    // Ptr to left arg token
                                             lpYYFcnStrOpr, // Ptr to operator function strand
                                             lptkRhtArg,    // Ptr to right arg token
                                             TRUE);         // TRUE iff prototyping
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End PrimProtoOpDieresisStar_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisStar_EM_YY
//
//  Primitive operator for monadic derived function from DieresisStar (ERROR)
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresisStar_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token (may be NULL if niladic)

{
    return
      PrimOpDydDieresisStar_EM_YY (NULL,            // Ptr to left arg token (may be NULL if niladic/monadic)
                                   lpYYFcnStrOpr,   // Ptr to operator function strand
                                   lptkRhtArg);     // Ptr to right arg token (may be NULL if niladic)
} // End PrimOpMonDieresisStar_EM_YY


//***************************************************************************
//  $PrimOpDieresisStarCommon_EM_YY
//
//  Primitive operator for monadic & dyadic derived function
//    from DieresisStar ("Power")
//***************************************************************************

LPPL_YYSTYPE PrimOpDieresisStarCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if niladic/monadic derived function)
     LPPL_YYSTYPE lpYYFcnStrLft,        // Ptr to left operand function strand
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPPL_YYSTYPE lpYYFcnStrRht,        // Ptr to right operand function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE      lpYYRes = NULL,           // Ptr to the result
                      lpYYRes2 = NULL,          // ...
                      lpYYFcn;                  // Bound left operand
    LPPERTABDATA      lpMemPTD;                 // Ptr to PerTabData global memory
    LPPLLOCALVARS     lpplLocalVars;            // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;             // Ptr to Ctrl-Break flag
    APLSTYPE          aplTypeOprRht;            // Right operand storage type
    APLNELM           aplNELMOprRht;            // Right operand NELM
    APLRANK           aplRankOprRht;            // Right operand rank
    HGLOBAL           hGlbOprRht = NULL,        // Right operand global memory handle (may be NULL)
                      hGlbRes2 = NULL;          // Result global memory handle (may be NULL)
    LPVARARRAY_HEADER lpMemHdrOprRht = NULL,    // Ptr to right operand's header
                      lpMemHdrRes2 = NULL;      // Ptr to result's header
    LPVOID            lpMemOprRht;              // Ptr to right operand's global memory data
    APLLONGEST        aplLongestOprRht,         // Right operand's immediate value
                      uCnt;                     // Loop counter
    UBOOL             bRet;                     // TRUE iff the result is valid
    TOKEN             tkRes = {0};              // The result token
    LPTOKEN           lptkRes;                  // Ptr to the result token

    // If the left operand is NOT a function, ...
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_DOMAIN_EXIT;

    // If the right operand is neither a function, nor a variable, ...
    if (!IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     && !IsTknTypeVar (lpYYFcnStrRht->tkToken.tkFlags.TknType))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If the right operand is a variable, ...
    if (IsTknTypeVar (lpYYFcnStrRht->tkToken.tkFlags.TknType))
    {
        // Get the right operand's attributes (type, NELM, and Rank)
        AttrsOfToken (&lpYYFcnStrRht->tkToken, &aplTypeOprRht, &aplNELMOprRht, &aplRankOprRht, NULL);

        // Check for RIGHT OPERAND RANK ERROR
        if (IsMultiRank (aplRankOprRht))
            goto RIGHT_OPERAND_RANK_EXIT;

        // Check for RIGHT OPERAND LENGTH ERROR
        if (!IsSingleton (aplNELMOprRht))
            goto RIGHT_OPERAND_LENGTH_EXIT;

        // Check for RIGHT OPERAND DOMAIN ERROR
        if (!IsNumeric (aplTypeOprRht))
            goto RIGHT_OPERAND_DOMAIN_EXIT;

        // Get and lock the right operand's global ptrs
        aplLongestOprRht = GetGlbPtrs_LOCK (&lpYYFcnStrRht->tkToken, &hGlbOprRht, &lpMemHdrOprRht);

        // If the right operand is a global, ...
        if (hGlbOprRht NE NULL)
            // Skip over the header and dimensions
            lpMemOprRht = VarArrayDataFmBase (lpMemHdrOprRht);
        else
            // Point to the data
            lpMemOprRht = &aplLongestOprRht;

        // Attempt to convert the right operand to an integer
        aplLongestOprRht = ConvertToInteger_SCT (aplTypeOprRht, lpMemOprRht, 0, &bRet);

        // Check for error
        if (!bRet)
            goto RIGHT_OPERAND_DOMAIN_EXIT;

        // Calculate the left operand's function inverse
        if (0 > (APLINT) aplLongestOprRht)
        {
            // Use the absolute value as the loop limit
            aplLongestOprRht = abs64 ((APLINT) aplLongestOprRht);
#ifdef DEBUG
            DbgBrk ();                  // #ifdef DEBUG
#endif















            goto RIGHT_OPERAND_NONCE_EXIT;
        } // End IF
    } // End IF

    // If there's a left arg, ...
    if (lptkLftArg NE NULL)
    {
        TOKEN        tkOper = {0};
        LPPL_YYSTYPE lpYYLftArg;

        // Save the Operator token as a PL_YYSTYPE
        lpYYLftArg = YYAllocTkn (CopyToken (lptkLftArg, FALSE));

        // Save the operator as a token
        tkOper.tkFlags.TknType  = TKT_OP2IMMED;
        tkOper.tkFlags.ImmType  = IMMTYPE_PRIMOP2;
        tkOper.tkData.tkChar    = UTF16_JOT;

        // Bind the left arg to the left operand
        lpYYFcn =
          MakeFcnStr_EM_YY (lpYYLftArg,         // Ptr to new left operand
                           &tkOper,             // Ptr to new operator
                            lpYYFcnStrLft);     // Ptr to new right operand (may be NULL if monadic operator)
        // YYFree it, but do not Free
        YYFree (lpYYLftArg); lpYYLftArg = NULL;

        // Check for error
        if (lpYYFcn EQ NULL)
            goto LEFT_SYNTAX_EXIT;

        // Increment the RefCnt for the left operand so we don't free it when we free <lpYYFcn>
        DbgIncrRefCntTkn (&lpYYFcnStrLft->tkToken);
    } else
        lpYYFcn = lpYYFcnStrLft;

    // If there's a right arg, ...
    if (lptkRhtArg NE NULL)
    {
        // Initialize the result token
        CopyAll (&tkRes, CopyToken (lptkRhtArg, FALSE));

        // Point to it
        lptkRes = &tkRes;
    } else
        // Mark as such
        lptkRes = NULL;

    // If the right operand is a variable, ...
    if (IsTknTypeVar (lpYYFcnStrRht->tkToken.tkFlags.TknType))
    {
        // Loop through the appropriate number of times
        for (uCnt = 0; uCnt < aplLongestOprRht; uCnt++)
        {
            // Check for Ctrl-break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Apply the left operand to the right arg
            lpYYRes =
              ExecFunc_EM_YY (NULL,             // Ptr to left arg token (may be NULL if monadic)
                              lpYYFcn,          // Ptr to function strand
                              lptkRes);         // Ptr to right arg token
            // If the result is valid, ...
            if (lpYYRes NE NULL)
            {
                // If there's a right arg, ...
                if (lptkRes NE NULL)
                {
                    // Free the previous right arg
                    FreeResultTkn (lptkRes); lptkRes = NULL;
                } // End IF

                // Copy the token from the new result
                CopyAll (&tkRes, CopyToken (&lpYYRes->tkToken, FALSE));

                // Point to it
                lptkRes = &tkRes;

                // Free and YYFree the previous result
                FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
            } else
                goto ERROR_EXIT;
        } // End FOR
    } else
    // If the right operand is a function, ...
    {
        APLLONGEST aplLongestRes2 = 0;      // Result's immediate value

        // Loop until done
        while (aplLongestRes2 EQ 0)
        {
            // Check for Ctrl-break
            if (CheckCtrlBreak (lpbCtrlBreak))
                goto ERROR_EXIT;

            // Apply the left operand to the right arg
            lpYYRes =
              ExecFunc_EM_YY (NULL,             // Ptr to left arg token (may be NULL if monadic)
                              lpYYFcn,          // Ptr to function strand
                              lptkRes);         // Ptr to right arg token
            // If the result is valid, ...
            if (lpYYRes NE NULL)
            {
                // If there's a right arg, ...
                if (lptkRes NE NULL)
                {
                    APLSTYPE aplTypeRes2;                   // Result storage type
                    APLNELM  aplNELMRes2;                   // Result NELM
                    APLRANK  aplRankRes2;                   // Result rank
                    LPVOID   lpMemRes2;                     // Ptr to result's global memory data

                    // Apply the right operand between the new and old result
                    lpYYRes2 =
                      ExecFunc_EM_YY (&lpYYRes->tkToken,    // Ptr to new result
                                       lpYYFcnStrRht,       // Ptr to function strand
                                       lptkRes);            // Ptr to old result
                    // Check for error
                    if (lpYYRes2 EQ NULL)
                        goto ERROR_EXIT;

                    // Get the new result's attributes (type, NELM, and Rank)
                    AttrsOfToken (&lpYYRes2->tkToken, &aplTypeRes2, &aplNELMRes2, &aplRankRes2, NULL);

                    // Check for RIGHT OPERAND RANK ERROR
                    if (IsMultiRank (aplRankRes2))
                        goto RIGHT_OPERAND_RANK_EXIT;

                    // Check for RIGHT OPERAND LENGTH ERROR
                    if (!IsSingleton (aplNELMRes2))
                        goto RIGHT_OPERAND_LENGTH_EXIT;

                    // Check for RIGHT OPERAND DOMAIN ERROR
                    if (!IsNumeric (aplTypeRes2))
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    // Get and lock the result's global ptrs
                    aplLongestRes2 = GetGlbPtrs_LOCK (&lpYYRes2->tkToken, &hGlbRes2, &lpMemHdrRes2);

                    // If the right operand is a global, ...
                    if (hGlbRes2 NE NULL)
                        // Skip over the header and dimensions
                        lpMemRes2 = VarArrayDataFmBase (lpMemHdrRes2);
                    else
                        // Point to the data
                        lpMemRes2 = &aplLongestRes2;

                    // Attempt to convert the right operand to an integer
                    aplLongestRes2 = ConvertToInteger_SCT (aplTypeRes2, lpMemRes2, 0, &bRet);

                    // Check for error
                    if (!bRet)
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    // Check for simple Boolean
                    if (!bRet || !IsBooleanValue (aplLongestRes2))
                        goto RIGHT_OPERAND_DOMAIN_EXIT;

                    if (hGlbRes2 NE NULL
                     && lpMemHdrRes2 NE NULL)
                    {
                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbRes2); lpMemHdrRes2 = NULL;
                    } // End IF

                    // Free and YYFree the result
                    FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL; hGlbRes2 = NULL;

                    // Free the previous right arg
                    FreeResultTkn (lptkRes); lptkRes = NULL;
                } // End IF

                // Copy the token from the new result
                CopyAll (&tkRes, CopyToken (&lpYYRes->tkToken, FALSE));

                // Point to it
                lptkRes = &tkRes;

                // Free and YYFree the previous result
                FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
            } else
                goto ERROR_EXIT;
        } // End WHILE
    } // End IF/ELSE

    // If there's a right arg, ...
    if (lptkRes NE NULL)
        // Allocate a new YYRes
        lpYYRes = YYAllocTkn (lptkRes);
    else
        // Mark as NoValue
        lpYYRes = MakeNoValue_YY (&lpYYFcnStrOpr->tkToken);

    goto NORMAL_EXIT;

LEFT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

LEFT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

ERROR_EXIT:
    // If the right operand is a locked global, ...
    if (hGlbOprRht NE NULL
     && lpMemHdrOprRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbOprRht); lpMemHdrOprRht = NULL;
    } // End IF

    if (lpYYRes NE NULL)
    {
        // Free the first YYRes
        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF
NORMAL_EXIT:
    // If there's a left arg, ...
    if (lptkLftArg NE NULL
     && lpYYFcn NE NULL)
    {
        // Free and YYFree it
        FreeResult (lpYYFcn); YYFree (lpYYFcn); lpYYFcn = NULL;
    } // End IF

    // If there's a comparison result, ...
    if (lpYYRes2 NE NULL)
    {
        if (hGlbRes2 NE NULL
         && lpMemHdrRes2 NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes2); lpMemHdrRes2 = NULL;
        } // End IF

        // Free the YYRes
        FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
    } // End IF

    return lpYYRes;
} // End PrimOpDieresisStarCommon_EM_YY


//***************************************************************************
//  $PrimOpDydDieresisStar_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisStar ("Power")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresisStar_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return
      PrimOpDydDieresisStarCommon_EM_YY (lptkLftArg,        // Ptr to left arg token
                                         lpYYFcnStrOpr,     // Ptr to operator function strand
                                         lptkRhtArg,        // Ptr to right arg token
                                         FALSE);            // TRUE iff prototyping
} // End PrimOpDydDieresisStar_EM_YY


//***************************************************************************
//  $PrimOpDydDieresisStarCommon_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisStar ("Power")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresisStarCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, NULL               ); Assert (lpYYFcnStrRht NE NULL);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrOpr, NULL, lpYYFcnStrRht); Assert (lpYYFcnStrLft NE NULL);

    return
      PrimOpDieresisStarCommon_EM_YY (lptkLftArg,           // Ptr to left arg token (may be NULL if monadic derived function)
                                      lpYYFcnStrLft,        // Ptr to left operand function strand
                                      lpYYFcnStrOpr,        // Ptr to operator function strand
                                      lpYYFcnStrRht,        // Ptr to right operand function strand
                                      lptkRhtArg,           // Ptr to right arg token
                                      bPrototyping);        // TRUE iff protoyping
} // End PrimOpDydDieresisStarCommon_EM_YY


//***************************************************************************
//  End of File: po_distar.c
//***************************************************************************
