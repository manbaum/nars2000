//***************************************************************************
//  NARS2000 -- Primitive Operator -- DieresisJot
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
//  $PrimOpDieresisJot_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    dyadic operator DieresisJot ("rank" and "rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDieresisJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token (may be NULL if niladic)

{
    Assert (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ UTF16_DIERESISJOT);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonDieresisJot_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg);     // Ptr to right arg token
    else
        return PrimOpDydDieresisJot_EM_YY (lptkLftArg,      // Ptr to left arg token
                                           lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg);     // Ptr to right arg token
} // End PrimOpDieresisJot_EM_YY


//***************************************************************************
//  $PrimProtoOpDieresisJot_EM_YY
//
//  Generate a prototype for the derived functions from
//    dyadic operator DieresisJot ("rank" and "rank")
//***************************************************************************

LPPL_YYSTYPE PrimProtoOpDieresisJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxisOpr)          // Ptr to axis operator token always NULL)

{
    if (lptkAxisOpr EQ NULL)
        // Check for axis operator
        lptkAxisOpr = CheckAxisOper (lpYYFcnStrOpr);

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically or niladically
        //***************************************************************
        return PrimOpMonDieresisJotCommon_EM_YY (lpYYFcnStrOpr,     // Ptr to operator function strand
                                                 lptkRhtArg,        // Ptr to right arg token (may be NULL if niladic)
                                                 lptkAxisOpr,       // Ptr to axis operator token (may be NULL)
                                                 TRUE);             // TRUE iff prototyping
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimOpDydDieresisJotCommon_EM_YY (lptkLftArg,        // Ptr to left arg token
                                                 lpYYFcnStrOpr,     // Ptr to operator function strand
                                                 lptkRhtArg,        // Ptr to right arg token
                                                 lptkAxisOpr,       // Ptr to axis operator token (may be NULL)
                                                 TRUE);             // TRUE iff prototyping
} // End PrimProtoOpDieresisJot_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisJot_EM_YY
//
//  Primitive operator for monadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresisJot_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token (may be NULL if niladic)

{
    return PrimOpMonDieresisJotCommon_EM_YY (lpYYFcnStrOpr,                 // Ptr to operator function strand
                                             lptkRhtArg,                    // Ptr to right arg token (may be NULLif niladic)
                                             CheckAxisOper (lpYYFcnStrOpr), // Ptr to axis oeprator (may be NULL)
                                             FALSE);                        // TRUE iff prototyping
} // End PrimOpMonDieresisJot_EM_YY


//***************************************************************************
//  $PrimOpMonDieresisJotCommon_EM_YY
//
//  Primitive operator for monadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpMonDieresisJotCommon_EM_YY
    (LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     LPTOKEN      lptkAxisOpr,          // Ptr to operator axis operator token
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, lptkAxisOpr               ); Assert (lpYYFcnStrRht NE NULL);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrOpr, lptkAxisOpr, lpYYFcnStrRht); Assert (lpYYFcnStrLft NE NULL);

    return
      PrimOpDieresisJotCommon_EM_YY (NULL,              // Ptr to left arg token (may be NULL if monadic derived function)
                                     lpYYFcnStrLft,     // Ptr to left operand function strand
                                     lpYYFcnStrOpr,     // Ptr to operator function strand
                                     lpYYFcnStrRht,     // Ptr to right operand function strand
                                     lptkRhtArg,        // Ptr to right arg token (may be NULL if niladic)
                                     lptkAxisOpr,       // Ptr to axis operator token (may be NULL)
                                     bPrototyping);     // TRUE iff protoyping
} // End PrimOpMonDieresisJotCommon_EM_YY


//***************************************************************************
//  $PrimOpDieresisJotCommon_EM_YY
//
//  Primitive operator for monadic & dyadic derived function
//    from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDieresisJotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic derived function)
     LPPL_YYSTYPE lpYYFcnStrLft,        // Ptr to left operand function strand
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPPL_YYSTYPE lpYYFcnStrRht,        // Ptr to right operand function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token (may be NULL if niladic)
     LPTOKEN      lptkAxisOpr,          // Ptr to axis operator token (may be NULL)
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    HGLOBAL       hGlbMFO1,             // Magic function/operator #1 global memory handle
                  hGlbOprRht;           // Right operand global memory handle
    LPPERTABDATA  lpMemPTD;             // Ptr to PerTabData global memory
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to result
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Ensure the left operand is a function
    if (!IsTknFcnOpr (&lpYYFcnStrLft->tkToken)
     || IsTknFillJot (&lpYYFcnStrLft->tkToken))
        goto LEFT_OPERAND_SYNTAX_EXIT;

    // Ensure the right operand is a variable
    if (IsTknFcnOpr (&lpYYFcnStrRht->tkToken)
     || IsTknFillJot (&lpYYFcnStrRht->tkToken))
        goto RIGHT_OPERAND_SYNTAX_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the magic function/operator global memory handles
    hGlbMFO1 = (lptkLftArg EQ NULL) ? lpMemPTD->hGlbMFO[MFOE_MonRank]
                                    : lpMemPTD->hGlbMFO[MFOE_DydRank];
    // Get right operand global ptrs
    hGlbOprRht = GetGlbHandle (&lpYYFcnStrRht->tkToken);

    // If the right operand is not immediate, ...
    if (hGlbOprRht NE NULL)
    {
        APLSTYPE aplTypeOprRht;             // Right operand storage type
        APLNELM  aplNELMOprRht;             // Right operand NELM
        APLRANK  aplRankOprRht;             // Right operand rank

        // Get the attributes (Type, NELM, and Rank)
        //   of the right arg token
        AttrsOfGlb (hGlbOprRht, &aplTypeOprRht, &aplNELMOprRht, &aplRankOprRht, NULL);

        // Check for RIGHT OPERAND RANK ERROR
        if (1 < aplRankOprRht)
            goto RIGHT_OPERAND_RANK_EXIT;

        // Check for RIGHT OPERAND LENGTH ERROR
        if (1 > aplNELMOprRht
         ||     aplNELMOprRht > 3)
            goto RIGHT_OPERAND_LENGTH_EXIT;

        // Check for RIGHT OPERAND DOMAIN ERROR
        if (!IsNumeric (aplTypeOprRht))
            goto RIGHT_OPERAND_DOMAIN_EXIT;
    } else
    // Check for RIGHT OPERAND DOMAIN ERROR
    if (!IsImmNum (lpYYFcnStrRht->tkToken.tkFlags.ImmType))
        goto RIGHT_OPERAND_DOMAIN_EXIT;

    lpYYRes =
      ExecuteMagicOperator_EM_YY (lptkLftArg,               // Ptr to left arg token (may be NULL if monadic/niladic)
                                 &lpYYFcnStrOpr->tkToken,   // Ptr to function token
                                  lpYYFcnStrLft,            // Ptr to left operand function strand
                                  lpYYFcnStrOpr,            // Ptr to function strand
                                  lpYYFcnStrRht,            // Ptr to right operand function strand (may be NULL)
                                  lptkRhtArg,               // Ptr to right arg token (may be NULL if niladic)
                                  lptkAxisOpr,              // Ptr to axis operator token (may be NULL)
                                  hGlbMFO1,                 // Magic function/operator global memory handle
                                  NULL,                     // Ptr to HSHTAB struc (may be NULL)
                                  bPrototyping
                                ? LINENUM_PRO
                                : LINENUM_ONE);             // Starting line # type (see LINE_NUMS)
    goto NORMAL_EXIT;

LEFT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStrLft->tkToken);
    goto ERROR_EXIT;

RIGHT_OPERAND_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
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

RIGHT_OPERAND_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStrRht->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (lpYYRes NE NULL)
    {
        // Free the first YYRes
        FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
    } // End IF
NORMAL_EXIT:
    return lpYYRes;
} // End PrimOpDieresisJotCommon_EM_YY


//***************************************************************************
//  Magic function/operator for monadic derived function from the rank dyadic operator
//
//  This operator was originally based upon code found in the paper by J. Philip Benkhard,
//    "Extending structure, type, and expression in APL2", ACM SIGAPL APL Quote Quad,
//    v.21 n.4, p.20-38, Aug. 1991, but later was modified to reflect the
//    definition of the Rank operator in the ISO-IEC 13751 Extended APL Standard.
//***************************************************************************

//***************************************************************************
//  Magic function/operator for dyadic derived function from the rank dyadic operator
//
//  This operator was originally based upon code found in the paper by J. Philip Benkhard,
//    "Extending structure, type, and expression in APL2", ACM SIGAPL APL Quote Quad,
//    v.21 n.4, p.20-38, Aug. 1991, but later was modified to reflect the
//    definition of the Rank operator in the ISO-IEC 13751 Extended APL Standard.
//***************************************************************************

#include "mf_dijot.h"



//***************************************************************************
//  $PrimOpDydDieresisJot_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresisJot_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg)           // Ptr to right arg token

{
    return PrimOpDydDieresisJotCommon_EM_YY (lptkLftArg,                    // Ptr to left arg token
                                             lpYYFcnStrOpr,                 // Ptr to operator function strand
                                             lptkRhtArg,                    // Ptr to right arg token
                                             CheckAxisOper (lpYYFcnStrOpr), // Ptr to axis operator (may be NULL)
                                             FALSE);                        // TRUE iff prototyping
} // End PrimOpDydDieresisJot_EM_YY


//***************************************************************************
//  $PrimOpDydDieresisJotCommon_EM_YY
//
//  Primitive operator for dyadic derived function from DieresisJot ("rank")
//***************************************************************************

LPPL_YYSTYPE PrimOpDydDieresisJotCommon_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxisOpr,          // Ptr to axis operator token (may be NULL)
     UBOOL        bPrototyping)         // TRUE iff protoyping

{
    LPPL_YYSTYPE lpYYFcnStrLft,         // Ptr to left operand function strand
                 lpYYFcnStrRht;         // Ptr to right ...

    // Set ptr to left & right operands,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrRht = GetDydRhtOper (lpYYFcnStrOpr, lptkAxisOpr               ); Assert (lpYYFcnStrRht NE NULL);
    lpYYFcnStrLft = GetDydLftOper (lpYYFcnStrOpr, lptkAxisOpr, lpYYFcnStrRht); Assert (lpYYFcnStrLft NE NULL);

    return
      PrimOpDieresisJotCommon_EM_YY (lptkLftArg,        // Ptr to left arg token (may be NULL if monadic derived function)
                                     lpYYFcnStrLft,     // Ptr to left operand function strand
                                     lpYYFcnStrOpr,     // Ptr to operator function strand
                                     lpYYFcnStrRht,     // Ptr to right operand function strand
                                     lptkRhtArg,        // Ptr to right arg token
                                     lptkAxisOpr,       // Ptr to axis operator token (may be NULL)
                                     bPrototyping);     // TRUE iff protoyping
} // End PrimOpDydDieresisJotCommon_EM_YY


//***************************************************************************
//  End of File: po_dijot.c
//***************************************************************************
