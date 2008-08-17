//***************************************************************************
//  NARS2000 -- Control Structures Processing
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

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

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "dfnhdr.h"
#include "pertab.h"
#include "sis.h"
#include "cs_parse.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CS_ChangeTokenType
//
//  Change the token type of a token (cs_yyparse)
//***************************************************************************

void CS_ChangeTokenType
    (LPCSLOCALVARS lpcsLocalVars,       // Ptr to PL local vars
     LPTOKEN       lptkArg1,            // Ptr to 1st argument
     TOKEN_TYPES   TknType)             // New token type

{
    LPTOKEN      lpMemTknLine;          // Ptr to tokenized line global memory
    LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator global memory
    LPFCNLINE    lpFcnLines;            // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HGLOBAL      hGlbTknLine;           // Tokenized line global memory handle

    // If this is execute or immexec, ...
    if (lpcsLocalVars->hGlbImmExec NE NULL)
    {
        Assert (lptkArg1->tkData.uLineNum EQ 1);

        // Get the execute/immexec global memory handle
        hGlbTknLine = lpcsLocalVars->hGlbImmExec;
    } else
    // It's a defined function/operator
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (lpcsLocalVars->hGlbDfnHdr);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        Assert (lptkArg1->tkData.uLineNum > 0);

        // Get the given line's tokenized global memory handle
        hGlbTknLine = lpFcnLines[lptkArg1->tkData.uLineNum - 1].hGlbTknLine;

        // We no longer need this ptr
        MyGlobalUnlock (lpcsLocalVars->hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF/ELSE

    // Lock the memory to get a ptr to it
    lpMemTknLine = MyGlobalLock (hGlbTknLine);

    // Skip over the header to the data
    lpMemTknLine = TokenBaseToStart (lpMemTknLine);

    // Change the given token number's type
    lpMemTknLine[lptkArg1->tkData.uTknNum].tkFlags.TknType = TknType;

    // Change the value on the CS stack
    lptkArg1->tkFlags.TknType = TknType;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbTknLine); lpMemTknLine = NULL;
} // End CS_ChangeTokenType


//***************************************************************************
//  $CS_ChainTokens
//
//  Chain together two CS tokens (cs_yyparse)
//***************************************************************************

void CS_ChainTokens
    (LPCSLOCALVARS lpcsLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg1,            // Ptr to 1st argument TOKEN_DATA
     LPTOKEN       lptkArg2)            // Ptr to 2nd argument TOKEN

{
    LPTOKEN      lpMemTknLine;          // Ptr to tokenized line global memory
    LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator global memory
    LPFCNLINE    lpFcnLines;            // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HGLOBAL      hGlbTknLine;           // Tokenized line global memory handle

    // If this is execute or immexec, ...
    if (lpcsLocalVars->hGlbImmExec NE NULL)
    {
        Assert (lptdArg1->uLineNum EQ 1
             && lptkArg2->tkData.uLineNum EQ 1);

        // Get the execute/immexec global memory handle
        hGlbTknLine = lpcsLocalVars->hGlbImmExec;
    } else
    // It's a defined function/operator
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (lpcsLocalVars->hGlbDfnHdr);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        Assert (lptdArg1->uLineNum > 0);

        // Get the given line's tokenized global memory handle
        hGlbTknLine = lpFcnLines[lptdArg1->uLineNum - 1].hGlbTknLine;

        // We no longer need this ptr
        MyGlobalUnlock (lpcsLocalVars->hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF/ELSE

    // Lock the memory to get a ptr to it
    lpMemTknLine = MyGlobalLock (hGlbTknLine);

    // Skip over the header to the data
    lpMemTknLine = TokenBaseToStart (lpMemTknLine);

#ifdef DEBUG
    dprintfW (L"!!Chaining %s to %s (%S#%d)",
              GetTokenTypeName (lpMemTknLine[lptdArg1->uTknNum].tkFlags.TknType),
              GetTokenTypeName (lptkArg2->tkFlags.TknType),
              FNLN);
#endif

    // Point the 1st token to the 2nd token
    lpMemTknLine[lptdArg1->uTknNum].tkData.uLineNum = lptkArg2->tkData.uLineNum;
    lpMemTknLine[lptdArg1->uTknNum].tkData.uStmtNum = lptkArg2->tkData.uStmtNum;
    lpMemTknLine[lptdArg1->uTknNum].tkData.uTknNum  = lptkArg2->tkData.uTknNum;

    // Change the values on the CS stack
    lptdArg1->uLineNum = lptkArg2->tkData.uLineNum;
    lptdArg1->uStmtNum = lptkArg2->tkData.uStmtNum;
    lptdArg1->uTknNum  = lptkArg2->tkData.uTknNum;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbTknLine); lpMemTknLine = NULL;
} // End CS_ChainTokens


//***************************************************************************
//  $CS_LinkStmt
//
//  Link together a new CS stmt (cs_yyparse)
//***************************************************************************

void CS_LinkStmt
    (LPCSLOCALVARS lpcsLocalVars,       // Ptr to local csLocalVars
     LPCS_YYSTYPE  lpYYLval)            // Ptr to yylval

{
    Assert (lpYYLval->lptkCur[1].tkFlags.TknType EQ TKT_CS_IF2
         || lpYYLval->lptkCur[1].tkFlags.TknType EQ TKT_CS_FOR2
         || lpYYLval->lptkCur[1].tkFlags.TknType EQ TKT_CS_REPEAT2
         || lpYYLval->lptkCur[1].tkFlags.TknType EQ TKT_CS_SELECT2
         || lpYYLval->lptkCur[1].tkFlags.TknType EQ TKT_CS_WHILE2);

    // Save as ptr to previous last stmt
    lpYYLval->lptkCur[1].tkData.lptkCSLink = lpcsLocalVars->lptkCSLink;

    // Save as new last stmt ptr
    lpcsLocalVars->lptkCSLink = lpYYLval->lptkCur;

    // Set the next .uCLIndex value
    CS_SetCLIndex (lpYYLval);
} // End CS_LinkStmt


//***************************************************************************
//  $CS_UnlinkStmt
//
//  Unlink an old CS stmt (cs_yyparse)
//***************************************************************************

void CS_UnlinkStmt
    (LPCSLOCALVARS lpcsLocalVars,       // Ptr to local csLocalVars
     LPCS_YYSTYPE  lpYYLval)            // Ptr to yylval

{
    // Unlink the stmt
    lpcsLocalVars->lptkCSLink = lpYYLval->lptkCur[1].tkData.lptkCSLink;
    lpYYLval->lptkCur[1].tkData.lptkCSLink = NULL;
} // End CS_UnlinkStmt


//***************************************************************************
//  $CS_SetCLIndex
//
//  Set the next .uCLIndex value (cs_yyparse)
//***************************************************************************

void CS_SetCLIndex
    (LPCS_YYSTYPE  lpYYLval)            // Ptr to yylval

{
    static UINT uCLIndex = 0;           // ContinueLeave index

    // Save the starting index common to this group
    lpYYLval->lptkCur->tkData.uCLIndex =
    lpYYLval->uCLIndex                 = ++uCLIndex;

    // The default value of .uCLIndex is zero, so we
    //   need to ensure that if the ++ wraps to zero
    //   we increment it again
    if (lpYYLval->lptkCur->tkData.uCLIndex EQ 0)
        lpYYLval->lptkCur->tkData.uCLIndex =
        lpYYLval->uCLIndex                 = ++uCLIndex;
} // End CS_SetCLIndex


//***************************************************************************
//  Above this point all the routines are called from cs_yyparse.
//***************************************************************************
//  Below this point all the routines are called from pl_yyparse.
//***************************************************************************


//***************************************************************************
//  $CS_CASE_Stmt
//
//  Process CASE/CASELIST stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_CASE_Stmt"
#else
#define APPEND_NAME
#endif

UBOOL CS_CASE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

    // Copy the result
    lpMemPTD->YYResExec = *lpYYRhtArg;

    // If the result is not immediate, ...
    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType NE TKT_VARIMMED)
        lpMemPTD->YYResExec.tkToken.tkData.tkGlbData =
          CopySymGlbDirAsGlb (lpMemPTD->YYResExec.tkToken.tkData.tkGlbData);

    // We no longer need this ptr
    MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

    return TRUE;
} // End CS_CASE_Stmt
#undef  APPEND_NAME


//***************************************************************************
//  $CS_CONTINUE_Stmt
//
//  Process CONTINUE stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_CONTINUE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYContinueArg)     // Ptr to CONTINUE arg

{
    // Tell the lexical analyzer to get the next token from
    //   the stmt at the token pointed to by the CONTINUE stmt
    CS_SetThisStmt (lpplLocalVars, &lpYYContinueArg->tkToken.tkData);

    // If the starting and ending stmts are not on the same line, ...
    if (lpYYContinueArg->tkToken.tkData.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    // If we're not stopping execution, ...
    if (!lpplLocalVars->bStopExec)
        // We're restarting
        lpplLocalVars->bRestart = TRUE;

    return TRUE;
} // End CS_CONTINUE_Stmt


//***************************************************************************
//  $CS_ELSE_Stmt
//
//  Process ELSE stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_ELSE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYElseArg)         // Ptr to ELSE arg

{
#ifdef DEBUG
    TOKEN_TYPES TknType;
#endif
    // Entry into this function means that we dropped through from above
    //   and should branch to the matching ENDIF/ENDSELECT stmt

    // Tell the lexical analyzer to get the next token from
    //   the stmt after the token pointed to by the ELSE stmt
    CS_SetNextStmt (lpplLocalVars, &lpYYElseArg->tkToken.tkData);

#ifdef DEBUG
    TknType = CS_GetTokenType (lpplLocalVars, &lpYYElseArg->tkToken.tkData);
    Assert (TknType EQ TKT_CS_ENDIF
         || TknType EQ TKT_CS_ENDSELECT);
#endif
    // If the starting and ending stmts are not on the same line, ...
    if (lpYYElseArg->tkToken.tkData.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    return TRUE;
} // End CS_ELSE_Stmt


//***************************************************************************
//  $CS_ENDFOR_Stmt
//
//  Process ENDFOR stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_ENDFOR_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYEndForArg)       // Ptr to ENDFOR arg

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPFORSTMT    lpForStmtNext;         // Ptr to next available entry on FORSTMT stack

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

    // Get ptr to current entry on FORSTMT stack
    lpForStmtNext = &lpMemPTD->lpSISCur->lpForStmtNext[-1];

    // We no longer need this ptr
    MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

    // Check the index against the limit
    if (lpForStmtNext->uIndex < lpForStmtNext->uNELM)
    {
        // Get the next value from the IN array token into the Named Var token
        //   and increment the index
        GetNextValueTokenIntoNamedVarToken (&lpForStmtNext->tkForArr,
                                             lpForStmtNext->uIndex++,
                                            &lpForStmtNext->tkForI);
        // Tell the lexical analyzer to get the next token from
        //   the stmt after the token pointed to by the ENDFOR stmt
        CS_SetNextStmt (lpplLocalVars, &lpYYEndForArg->tkToken.tkData);

        // If the starting and ending stmts are not on the same line, ...
        if (lpYYEndForArg->tkToken.tkData.uLineNum NE lpplLocalVars->uLineNum)
            // Tell the parser to stop executing this line
            lpplLocalVars->bStopExec = TRUE;
    } else
        // We're done with this FOR stmt
        CS_DoneFOR (lpplLocalVars);

        // By not changing the line/token #, we fall through to the next stmt

    return TRUE;
} // End CS_ENDFOR_Stmt


//***************************************************************************
//  $CS_DoneFOR
//
//  We're done with this FOR stmt-- free the global in the token (if any),
//    and delete this entry
//***************************************************************************

void CS_DoneFOR
    (LPPLLOCALVARS lpplLocalVars)       // Ptr to PL local vars

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPFORSTMT    lpForStmtNext;         // Ptr to next available entry on FORSTMT stack

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

    // Get ptr to current entry on FORSTMT stack
    //   and delete this entry
    lpForStmtNext = --lpMemPTD->lpSISCur->lpForStmtNext;

    // We no longer need this ptr
    MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

    // We're done -- free the global in the token (if any)
    if (!IsTknImmed (lpForStmtNext->tkForArr.tkFlags.TknType))
        FreeResultGlobalVar (lpForStmtNext->tkForArr.tkData.tkGlbData);

    // Zap the entry
    ZeroMemory (lpForStmtNext, sizeof (*lpForStmtNext));
} // End CS_DoneFOR


//***************************************************************************
//  $CS_ENDREPEAT_Stmt
//
//  Process ENDREPEAT stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_ENDREPEAT_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYEndRepeatArg)    // Ptr to ENDREPEAT arg

{
    // Tell the lexical analyzer to get the next token from
    //   the stmt after the token pointed to by the ENDREPEAT stmt
    CS_SetNextStmt (lpplLocalVars, &lpYYEndRepeatArg->tkToken.tkData);

    // If the starting and ending stmts are not on the same line, ...
    if (lpYYEndRepeatArg->tkToken.tkData.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    return TRUE;
} // End CS_ENDREPEAT_Stmt


//***************************************************************************
//  CS_FOR_Stmt_EM
//
//  Process FOR stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_FOR_Stmt_EM"
#else
#define APPEND_NAME
#endif

UBOOL CS_FOR_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYForArg,          // Ptr to FOR arg
     LPPL_YYSTYPE  lpYYNameArg,         // Ptr to Name arg
     LPPL_YYSTYPE  lpYYInArg,           // Ptr to IN arg
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPFORSTMT    lpForStmtNext;         // Ptr to next available entry on FORSTMT stack

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

    // Save ptr to next available entry on FORSTMT stack
    //   and skip over this to the next entry
    lpForStmtNext = lpMemPTD->lpSISCur->lpForStmtNext++;

    // We no longer need this ptr
    MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (&lpYYRhtArg->tkToken, NULL, &lpForStmtNext->uNELM, NULL, NULL);

    // Save initial values on the FORSTMT stack
    lpForStmtNext->uIndex   = 0;
    lpForStmtNext->tkForI   = lpYYNameArg->tkToken;
    lpForStmtNext->tkForArr = lpYYRhtArg->tkToken;

    // If the token is not immediate, ...
    if (!IsTknImmed (lpForStmtNext->tkForArr.tkFlags.TknType))
        // Increment the reference count
        lpForStmtNext->tkForArr.tkData.tkGlbData =
          CopySymGlbDir (lpForStmtNext->tkForArr.tkData.tkGlbData);

    // Tell the lexical analyzer to get the next token from
    //   the stmt at the token pointed to by the FOR stmt (the ENDFOR stmt)
    CS_SetThisStmt (lpplLocalVars, &lpYYForArg->tkToken.tkData);

    // If we're not stopping execution, ...
    if (!lpplLocalVars->bStopExec)
        // We're restarting
        lpplLocalVars->bRestart = TRUE;

    return TRUE;
} // End CS_FOR_Stmt_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CS_GOTO_Stmt_EM
//
//  Process GOTO stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_GOTO_Stmt_EM"
#else
#define APPEND_NAME
#endif

UBOOL CS_GOTO_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    APLSTYPE   aplTypeRht;              // Right arg storage type
    APLNELM    aplNELMRht;              // Right arg NELM
    APLRANK    aplRankRht;              // Right arg rank
    APLLONGEST aplLongestRht;           // Right arg longest if immediate
    UBOOL      bRet;                    // TRUE iff the result is valid
    TOKEN_DATA tdNxt = {0};             // TOKEN_DATA of the next stmt

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (&lpYYRhtArg->tkToken, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for LENGTH ERROR
    if (IsMultiNELM (aplNELMRht))
        goto LENGTH_EXIT;

    // Check for empty right arg
    if (IsEmpty (aplNELMRht))
    {
        if (!IsSimple (aplTypeRht))
            goto DOMAIN_EXIT;
    } else
    {
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
            case ARRAY_FLOAT:
                GetFirstItemToken (&lpYYRhtArg->tkToken,
                                   &aplLongestRht,
                                    NULL,
                                    NULL);
                // If the storage type is float, ...
                if (IsSimpleFlt (aplTypeRht))
                {
                    // Attempt to convert the float to an integer using System CT
                    aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
                    if (!bRet)
                        goto DOMAIN_EXIT;
                } // End IF

                break;

            case ARRAY_CHAR:
            case ARRAY_HETERO:
            case ARRAY_NESTED:
                goto DOMAIN_EXIT;

            defstop
                break;
        } // End SWITCH

        // Set the values for the target stmt
        tdNxt.uLineNum = (USHORT) aplLongestRht;
////////tdNxt.uTknNum  = 0;                         // Already zero from = {0}

        // In case aplLongestRht is wider than .uLineNum can hold, ...
        if (tdNxt.uLineNum NE aplLongestRht)
            tdNxt.uLineNum = 0;

        // Tell the lexical analyzer to get the next token from
        //   the preceding EOS token
        CS_SetNextToken (lpplLocalVars, &tdNxt);
    } // End IF

    return TRUE;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return FALSE;
} // End CS_GOTO_Stmt_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CS_IF_Stmt_EM
//
//  Process IF/ELSEIF/ANDIF/ORIF/UNTIL/WHILE stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_IF_Stmt"
#else
#define APPEND_NAME
#endif

UBOOL CS_IF_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYIfArg,           // Ptr to IF/... arg
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    APLSTYPE   aplTypeRht;              // Right arg storage type
    APLNELM    aplNELMRht;              // Right arg NELM
    APLRANK    aplRankRht;              // Right arg rank
    APLLONGEST aplLongestRht;           // Right arg longest if immediate
    UBOOL      bRet,                    // TRUE iff the result is valid
               bWhile;                  // TRUE iff the next token is WHILE
    TOKEN_DATA tdNxt,                   // TOKEN_DATA of the IF/... stmt
               tdPrv;                   // ...               previous stmt
    TOKEN      tkNxt;                   // Next token

    // The arg to the IF/... stmt must be a Boolean-valued scalar or
    //   one-element vector.
    // If the stmt pointed to by the IF/... stmt is ORIF or ANDIF, process
    //   that sequence depending upon the value of the arg to the IF/... stmt.
    // Otherwise, if the value of the arg to the IF/... stmt is 1, continue
    //   execution with the next stmt; if the value is 0, look at the
    //   token pointed to by the IF/... stmt:
    // If that token is an ELSEIF, start execution with that stmt.
    // If that token is an ELSE, start execution with the stmt after that.
    // If that token is an ENDIF, start execution with the stmt after that.
    // If that token is an ENDWHILE, start execution with the stmt to which that points.

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (&lpYYRhtArg->tkToken, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto LENGTH_EXIT;

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
            GetFirstItemToken (&lpYYRhtArg->tkToken,
                               &aplLongestRht,
                                NULL,
                                NULL);
            // If the storage type is float, ...
            if (IsSimpleFlt (aplTypeRht))
            {
                // Attempt to convert the float to an integer using System CT
                aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
            } // End IF

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto DOMAIN_EXIT;

        defstop
            break;
    } // End SWITCH

    // Check for DOMAIN ERROR
    if (!IsBooleanValue (aplLongestRht))
         goto DOMAIN_EXIT;

    // Copy the token data
    tdNxt = lpYYIfArg->tkToken.tkData;

    // Get the contents of the token pointed to by
    //   the IF/... token
    CS_GetNextToken (lpplLocalVars, &tdNxt, &tkNxt, NULL, NULL);

    // Set contrary value
    bWhile = (tkNxt.tkFlags.TknType EQ TKT_CS_REPEAT
           || tkNxt.tkFlags.TknType EQ TKT_CS_WHILE);

    // Split cases based upon the type of the token to which the IF/... stmt points
    switch (tkNxt.tkFlags.TknType)
    {
        case TKT_CS_ANDIF:
            // If the IF/... arg is 0, ...
            if (aplLongestRht EQ 0)
            {
                // Loop through the ANDIF chain stopping at the first token
                //   in the chain which is not ANDIF
                while (tkNxt.tkFlags.TknType EQ TKT_CS_ANDIF)
                {
                    // Get the TOKEN_DATA from the ANDIF token
                    tdNxt = tkNxt.tkData;

                    // Get the contents of the token pointed to by
                    //   the ANDIF token
                    CS_GetNextToken (lpplLocalVars, &tdNxt, &tkNxt, NULL, NULL);
                } // End WHILE

                Assert (tkNxt.tkFlags.TknType EQ TKT_CS_ELSE
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ENDIF
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ENDWHILE
                     || tkNxt.tkFlags.TknType EQ TKT_CS_UNTIL
                     || tkNxt.tkFlags.TknType EQ TKT_CS_WHILE);

                // If the next token is ELSEIF, execute that stmt
                if (tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF)
                    // Tell the lexical analyzer to get the next token from
                    //   the stmt at the token pointed to by the ELSEIF stmt
                    CS_SetThisStmt (lpplLocalVars, &tdNxt);
                else
                    // Tell the lexical analyzer to get the next token from
                    //   the stmt after the token pointed to by the ELSE/ENDIF stmt
                    CS_SetNextStmt (lpplLocalVars, &tdNxt);
            } else
                // Tell the lexical analyzer to get the next token from
                //   the ANDIF stmt
                CS_SetThisStmt (lpplLocalVars, &tdNxt);

            // If we're not stopping execution, ...
            if (!lpplLocalVars->bStopExec)
                // We're restarting
                lpplLocalVars->bRestart = TRUE;
            break;

        case TKT_CS_ORIF:
            // If the IF/... arg is 0, ...
            if (aplLongestRht EQ 0)
                // Tell the lexical analyzer to get the next token from
                //   the ORIF stmt
                CS_SetThisStmt (lpplLocalVars, &tdNxt);
            else
            {
                // Save as the previous token data
                tdPrv = tdNxt;

                // Loop through the ORIF chain stopping at the first token
                //   in the chain which is not ORIF
                while (tkNxt.tkFlags.TknType EQ TKT_CS_ORIF)
                {
                    // Get the TOKEN_DATA from the ORIF token
                    tdNxt = tkNxt.tkData;

                    // Get the contents of the token pointed to by
                    //   the ORIF token
                    CS_GetNextToken (lpplLocalVars, &tdNxt, &tkNxt, NULL, NULL);

                    // Save the token data if the token is ORIF
                    if (tkNxt.tkFlags.TknType EQ TKT_CS_ORIF)
                        tdPrv = tdNxt;
                } // End WHILE

                Assert (tkNxt.tkFlags.TknType EQ TKT_CS_ELSE
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ENDIF
                     || tkNxt.tkFlags.TknType EQ TKT_CS_ENDWHILE
                     || tkNxt.tkFlags.TknType EQ TKT_CS_UNTIL
                     || tkNxt.tkFlags.TknType EQ TKT_CS_WHILE);

                // Tell the lexical analyzer to get the next token from
                //   the stmt after the last ORIF stmt
                CS_SetNextStmt (lpplLocalVars, &tdPrv);
            } // End IF/ELSE

            // If we're not stopping execution, ...
            if (!lpplLocalVars->bStopExec)
                // We're restarting
                lpplLocalVars->bRestart = TRUE;
            break;

        case TKT_CS_ELSEIF:
        case TKT_CS_UNTIL:
        case TKT_CS_WHILE:
            // If the IF/... arg is bWhile, ...
            if (aplLongestRht EQ bWhile)
            {
                // Tell the lexical analyzer to get the next token from
                //   the stmt at the token pointed to by the ELSEIF stmt
                CS_SetThisStmt (lpplLocalVars, &tdNxt);

                // If we're not stopping execution, ...
                if (!lpplLocalVars->bStopExec)
                    // We're restarting
                    lpplLocalVars->bRestart = TRUE;
            } // End IF

            break;

        case TKT_CS_ELSE:
        case TKT_CS_ENDIF:
        case TKT_CS_ENDWHILE:
        case TKT_CS_REPEAT:
            // If the IF/... arg is 0, ...
            if (aplLongestRht EQ 0)
            {
                // Tell the lexical analyzer to get the next token from
                //   the stmt after the token pointed to by the ELSE/ENDxxx stmt
                CS_SetNextStmt (lpplLocalVars, &tdNxt);

                // If we're not stopping execution, ...
                if (!lpplLocalVars->bStopExec)
                    // We're restarting
                    lpplLocalVars->bRestart = TRUE;
            } // End IF

            break;

        defstop
            break;
    } // End SWITCH

    return TRUE;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYRhtArg->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    return FALSE;
} // End CS_IF_Stmt_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CS_LEAVE_Stmt
//
//  Process LEAVE stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_LEAVE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYLeaveArg)        // Ptr to LEAVE arg

{
    // Tell the lexical analyzer to get the next token from
    //   the stmt after the token pointed to by the LEAVE stmt
    CS_SetNextStmt (lpplLocalVars, &lpYYLeaveArg->tkToken.tkData);

    // If the starting and ending stmts are not on the same line, ...
    if (lpYYLeaveArg->tkToken.tkData.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    // If we're not stopping execution, ...
    if (!lpplLocalVars->bStopExec)
        // We're restarting
        lpplLocalVars->bRestart = TRUE;

    // If the stmt pointed to by the LEAVE stmt is ENDFOR, ...
    if (CS_GetTokenType (lpplLocalVars, &lpYYLeaveArg->tkToken.tkData) EQ TKT_CS_ENDFOR)
        // We're done with this FOR stmt
        CS_DoneFOR (lpplLocalVars);

    return TRUE;
} // End CS_LEAVE_Stmt


//***************************************************************************
//  $CS_RETURN_Stmt
//
//  Process RETURN stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_RETURN_Stmt
    (LPPLLOCALVARS lpplLocalVars)       // Ptr to PL local vars

{
    TOKEN_DATA tdNxt = {0};             // TOKEN_DATA of the next stmt

    // Set the values for the target stmt
////tdNxt.uLineNum = 0;                 // Already zero from = {0}
////tdNxt.uTknNum  = 0;                 // Already zero from = {0}

    // Tell the lexical analyzer to get the next token from
    //   the preceding EOS token
    CS_SetNextToken (lpplLocalVars, &tdNxt);

    return TRUE;
} // End CS_RETURN_Stmt


//***************************************************************************
//  $CS_SELECT_Stmt_EM
//
//  Process SELECT stmt (pl_yyparse)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CS_SELECT_Stmt_EM"
#else
#define APPEND_NAME
#endif

UBOOL CS_SELECT_Stmt_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYSelectArg,       // Ptr to SELECT arg
     LPPL_YYSTYPE  lpYYRhtArg)          // Ptr to right arg

{
    APLNELM      aplNELMCL;             // CASELIST NELM
    TOKEN_DATA   tdNxt,                 // TOKEN_DATA of the next stmt
                 tdSOS;                 // ...               SOS  ...
    TOKEN        tkNxt;                 // TOKEN of next stmt
    HGLOBAL      hGlbTknLine,           // Tokenized line global memory handle
                 hGlbTxtLine,           // Text      ...
                 hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYTmp;               // Ptr to right arg
    UBOOL        bCmp;                  // TRUE iff the comparison is TRUE

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Copy the token data
    tkNxt.tkData = lpYYSelectArg->tkToken.tkData;

    // Run through the chain of SELECT/CASE/CASELIST stmts looking for a match
    while (TRUE)
    {
        // Copy the token data
        tdNxt = tkNxt.tkData;

        // Get the contents of the token pointed to by
        //   the SELECT/CASE/CASELIST token
        CS_GetNextToken (lpplLocalVars, &tdNxt, &tkNxt, &hGlbTknLine, &hGlbTxtLine);

        // Split cases based upon the token type
        switch (tkNxt.tkFlags.TknType)
        {
            EXIT_TYPES exitType;

            case TKT_CS_CASE:
            case TKT_CS_CASELIST:
                // Copy token data to modify
                tdSOS = tdNxt;

                // Point to the SOS token
                CS_PointToSOSToken (lpplLocalVars, &tdSOS);

                // Execute the stmt and compare the result with lpYYRhtArg

                // Fill the SIS struc, execute the line via ParseLine, and cleanup
                exitType =
                  PrimFnMonUpTackJotPLParse (hGlbPTD,           // PerTabData global memory handle
                                             hGlbTknLine,       // Tokenized line global memory handle
                                             hGlbTxtLine,       // Text      ...
                                             NULL,              // Ptr to text of line to execute
                                             NULL,              // Ptr to Semaphore handle to signal (NULL if none)
                                             tdSOS.uLineNum,    // Function line #
                                             tdSOS.uTknNum,     // Starting token # in the above function line
                                             TRUE);             // TRUE iff executing only one stmt
                // Split cases based upon the exit type
                switch (exitType)
                {
                    case EXITTYPE_NONE:
                        // Match the SELECT arg against the CASE arg

                        // Lock the memory to get a ptr to it
                        lpMemPTD = MyGlobalLock (hGlbPTD);

                        // If it's CASE, ...
                        if (tkNxt.tkFlags.TknType EQ TKT_CS_CASE)
                        {
                            // Use match to determine equality
                            lpYYTmp =
                              PrimFnDydEqualUnderbar_EM_YY (&lpMemPTD->YYResExec.tkToken,   // Ptr to left arg token
                                                            &lpYYRhtArg->tkToken,           // Ptr to function token
                                                            &lpYYRhtArg->tkToken,           // Ptr to right arg token
                                                             NULL);                         // Ptr to axis token (may be NULL)
                            // Save the result of the comparison
                            bCmp = lpYYTmp->tkToken.tkData.tkBoolean;

                            // Free the temporary result
                            YYFree (lpYYTmp); lpYYTmp = NULL;
                        } else
                        // It's CASELIST
                        {
                            TOKEN tkCL = {0};       // CASELIST token for each element
                            UINT  uCnt;             // Loop counter

                            // Get the attributes (Type, NELM, and Rank)
                            //   of the CASELIST arg
                            AttrsOfToken (&lpMemPTD->YYResExec.tkToken, NULL, &aplNELMCL, NULL, NULL);

                            // Loop through the elements of the CASELIST arg
                            for (uCnt = 0, bCmp = FALSE; (!bCmp) && uCnt < aplNELMCL; uCnt++)
                            {
                                // Get the next value from the CASELIST result token into a token
                                GetNextValueTokenIntoToken (&lpMemPTD->YYResExec.tkToken,
                                                             uCnt,
                                                            &tkCL);
                                // Use match to determine equality
                                lpYYTmp =
                                  PrimFnDydEqualUnderbar_EM_YY (&tkCL,                          // Ptr to left arg token
                                                                &lpYYRhtArg->tkToken,           // Ptr to function token
                                                                &lpYYRhtArg->tkToken,           // Ptr to right arg token
                                                                 NULL);                         // Ptr to axis token (may be NULL)
                                // Save the result of the comparison
                                bCmp = lpYYTmp->tkToken.tkData.tkBoolean;

                                // Free the temporary result
                                YYFree (lpYYTmp); lpYYTmp = NULL;
                            } // End FOR
                        } // End IF/ELSE

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                        // If the two are equal, ...
                        if (bCmp)
                        {
                            // Tell the lexical analyzer to get the next token from
                            //   the stmt after the token pointed to by the CASE stmt
                            CS_SetNextStmt (lpplLocalVars, &tdNxt);

                            return TRUE;
                        } // End IF

                        break;

                    case EXITTYPE_ERROR:
                        return FALSE;

                    case EXITTYPE_GOTO_ZILDE:
                    case EXITTYPE_GOTO_LINE:
                    case EXITTYPE_RESET_ONE:
                    case EXITTYPE_RESET_ONE_INIT:
                    case EXITTYPE_RESET_ALL:
                    case EXITTYPE_QUADERROR_INIT:
                    case EXITTYPE_QUADERROR_EXEC:
                    case EXITTYPE_STOP:
                    case EXITTYPE_DISPLAY:
                    case EXITTYPE_NOVALUE:
                    case EXITTYPE_NODISPLAY:
                    defstop
                        return FALSE;
                } // End SWITCH

                break;

            case TKT_CS_ELSE:
            case TKT_CS_ENDSELECT:
                // Tell the lexical analyzer to get the next token from
                //   the stmt after the token pointed to by the ELSE stmt
                CS_SetNextStmt (lpplLocalVars, &tdNxt);

                // If we're not stopping execution, ...
                if (!lpplLocalVars->bStopExec)
                    // We're restarting
                    lpplLocalVars->bRestart = TRUE;
                return TRUE;

            defstop
                break;
        } // End SWITCH
    } // End WHILE

    return FALSE;
} // End CS_SELECT_Stmt_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CS_SKIPCASE_Stmt
//
//  Process SKIPCASE stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_SKIPCASE_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYSkipCaseArg)     // Ptr to SKIPCASE arg

{
    TOKEN      tkNxt;                   // Next token
    TOKEN_DATA tdNxt;                   // Next TOKEN_DATA

    // Get the contents of the (CASE/CASELIST) token pointed to by
    //   the SKIPCASE token
    CS_GetNextToken (lpplLocalVars, &lpYYSkipCaseArg->tkToken.tkData, &tkNxt, NULL, NULL);

    Assert (tkNxt.tkFlags.TknType EQ TKT_CS_CASE
         || tkNxt.tkFlags.TknType EQ TKT_CS_CASELIST);

    do
    {
        // Get the TOKEN_DATA from the previous token
        tdNxt = tkNxt.tkData;

        // Get the contents of the token pointed to by
        //   the previous token
        CS_GetNextToken (lpplLocalVars, &tdNxt, &tkNxt, NULL, NULL);
    } while (tkNxt.tkFlags.TknType EQ TKT_CS_CASE
          || tkNxt.tkFlags.TknType EQ TKT_CS_CASELIST
          || tkNxt.tkFlags.TknType EQ TKT_CS_ELSE);

    Assert (tkNxt.tkFlags.TknType EQ TKT_CS_ENDSELECT);

    // Tell the lexical analyzer to get the next token from
    //   the stmt after the token pointed to by the ENDSELECT stmt
    CS_SetNextStmt (lpplLocalVars, &tdNxt);

    // If the starting and ending stmts are not on the same line, ...
    if (tkNxt.tkData.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    return TRUE;
} // End CS_SKIPCASE_Stmt


//***************************************************************************
//  $CS_SKIPEND_Stmt
//
//  Process SKIPEND stmt (pl_yyparse)
//***************************************************************************

UBOOL CS_SKIPEND_Stmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPPL_YYSTYPE  lpYYSkipEndArg)      // Ptr to SKIPEND arg

{
    TOKEN      tkNxt;                   // Next token
    TOKEN_DATA tdNxt;                   // Next TOKEN_DATA

    // Get the contents of the (ELSEIF) token pointed to by
    //   the SKIPEND token
    CS_GetNextToken (lpplLocalVars, &lpYYSkipEndArg->tkToken.tkData, &tkNxt, NULL, NULL);

    Assert (tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF);

    do
    {
        // Get the TOKEN_DATA from the previous token
        tdNxt = tkNxt.tkData;

        // Get the contents of the token pointed to by
        //   the previous token
        CS_GetNextToken (lpplLocalVars, &tdNxt, &tkNxt, NULL, NULL);
    } while (tkNxt.tkFlags.TknType EQ TKT_CS_ANDIF
          || tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF
          || tkNxt.tkFlags.TknType EQ TKT_CS_ELSE
          || tkNxt.tkFlags.TknType EQ TKT_CS_ORIF);

    Assert (tkNxt.tkFlags.TknType EQ TKT_CS_ELSE
         || tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF
         || tkNxt.tkFlags.TknType EQ TKT_CS_ENDIF);

    // If the next token is ELSEIF, execute that stmt
    if (tkNxt.tkFlags.TknType EQ TKT_CS_ELSEIF)
        // Tell the lexical analyzer to get the next token from
        //   the stmt at the token pointed to by the ELSEIF stmt
        CS_SetThisStmt (lpplLocalVars, &tdNxt);
    else
        // Tell the lexical analyzer to get the next token from
        //   the stmt after the token pointed to by the ELSE/ENDIF stmt
        CS_SetNextStmt (lpplLocalVars, &tdNxt);

    // If the starting and ending stmts are not on the same line, ...
    if (tkNxt.tkData.uLineNum NE lpplLocalVars->uLineNum)
        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;

    return TRUE;
} // End CS_SKIPEND_Stmt


//***************************************************************************
//  $CS_GetNextToken
//
//  Get a ptr to the TOKEN_DATA of the token
//    to where a given token points (pl_yyparse)
//***************************************************************************

void CS_GetNextToken
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg1,            // Ptr to arg TOKEN_DATA
     LPTOKEN       lptkRes,             // Ptr to result token
     HGLOBAL      *lphGlbTknLine,       // Ptr to result tokenized line global memory handle
     HGLOBAL      *lphGlbTxtLine)       // Ptr to result text      ...

{
    LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator global memory
    LPFCNLINE    lpFcnLines;            // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HGLOBAL      hGlbTknLine,           // Tokenized line global memory handle
                 hGlbTxtLine;           // Text line      ...
    LPTOKEN      lpMemTknLine;          // Ptr to tokenized line global memory

    // If the stmts are on the same line, ...
    if (lptdArg1->uLineNum EQ lpplLocalVars->uLineNum)
    {
        // Get the token
        *lptkRes = lpplLocalVars->lptkStart[lptdArg1->uTknNum];

        // Copy the tokenized line's global memory handle
        hGlbTknLine = lpplLocalVars->hGlbTknLine;

        // Copy the text line's global memory handle
        hGlbTxtLine = lpplLocalVars->hGlbTxtLine;
    } else
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (lpplLocalVars->hGlbDfnHdr);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        Assert (lptdArg1->uLineNum > 0);

        // Get the given line's tokenized global memory handle
        hGlbTknLine = lpFcnLines[lptdArg1->uLineNum - 1].hGlbTknLine;

        // Copy the text line's global memory handle
        hGlbTxtLine = lpFcnLines[lptdArg1->uLineNum - 1].hGlbTxtLine;

        // We no longer need this ptr
        MyGlobalUnlock (lpplLocalVars->hGlbDfnHdr); lpMemDfnHdr = NULL;

        // Lock the memory to get a ptr to it
        lpMemTknLine = MyGlobalLock (hGlbTknLine);

        // Skip over the header to the data
        lpMemTknLine = TokenBaseToStart (lpMemTknLine);

        // Get the given token
        *lptkRes = lpMemTknLine[lptdArg1->uTknNum];

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTknLine); lpMemTknLine = NULL;
    } // End IF/ELSE

    // If the caller wants the tokenized line's global memory handle, ...
    if (lphGlbTknLine)
        *lphGlbTknLine = hGlbTknLine;

    // If the caller wants the text line's global memory handle, ...
    if (lphGlbTxtLine)
        *lphGlbTxtLine = hGlbTxtLine;
} // End CS_GetNextToken


//***************************************************************************
//  $CS_PointToSOSToken
//
//  Set the next token # of the SOS token of the stmt which contains
//    a given token.
//***************************************************************************

void CS_PointToSOSToken
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg)             // Ptr to arg TOKEN_DATA

{
    // The Arg1 token is at the start of the stmt, so backing up
    //   by one points to the preceding EOS/EOL stmt which contains the
    //   stmt length.
    Assert (lptdArg->uTknNum > 0);
    lptdArg->uTknNum--;

    // Skip to the end of the Arg1 stmt less one to address the SOS token
    lptdArg->uTknNum += CS_GetEOSTokenLen (lpplLocalVars, lptdArg) - 1;
} // End CS_PointToSOSToken


//***************************************************************************
//  $CS_SetThisStmt
//
//  Set the next token to the beginning of the stmt
//    at the token pointed to by the given token (pl_yyparse)
//***************************************************************************

void CS_SetThisStmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg1)            // Ptr to arg TOKEN_DATA

{
    TOKEN_DATA tdArg;                   // Next TOKEN_DATA

        // Copy the token contents to modify
    tdArg = *lptdArg1;

    // Point to the SOS token
    CS_PointToSOSToken (lpplLocalVars, &tdArg);

    // Tell the lexical analyzer to get the next token from
    //   the preceding EOS token
    CS_SetNextToken (lpplLocalVars, &tdArg);
} // End CS_SetThisStmt


//***************************************************************************
//  $CS_SetNextStmt
//
//  Set the next token to the stmt after
//    where a given token points (pl_yyparse)
//***************************************************************************

void CS_SetNextStmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg1)            // Ptr to arg TOKEN_DATA

{
    // The lptdArg1 token is at the start of the stmt

    Assert (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_EOS
         || lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_EOL);

    // Tell the lexical analyzer to get the next token from
    //   the preceding EOS token
    CS_SetNextToken (lpplLocalVars, lptdArg1);
} // End CS_SetNextStmt


//***************************************************************************
//  $CS_SetNextToken
//
//  Set the next token to where a given token points (pl_yyparse)
//***************************************************************************

void CS_SetNextToken
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg1)            // Ptr to arg TOKEN_DATA

{
    HGLOBAL      hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If the stmts are on the same line, ...
    if (lptdArg1->uLineNum EQ lpplLocalVars->uLineNum)
        // Set the next token to parse
        lpplLocalVars->lptkNext =
          &lpplLocalVars->lptkStart[lptdArg1->uTknNum];
    else
    {
        // Save as the next line & token #
        lpMemPTD->lpSISCur->NxtLineNum = lptdArg1->uLineNum;
        lpMemPTD->lpSISCur->NxtTknNum  = lptdArg1->uTknNum;

        // Tell the parser to stop executing this line
        lpplLocalVars->bStopExec = TRUE;
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End CS_SetNextToken


//***************************************************************************
//  $CS_GetTokenType
//
//  Return the token type of a given token identified by line and token #
//    (pl_yyparse)
//***************************************************************************

TOKEN_TYPES CS_GetTokenType
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg1)            // Ptr to arg TOKEN_DATA

{
    LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator global memory
    LPFCNLINE    lpFcnLines;            // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HGLOBAL      hGlbTknLine;           // Tokenized line global memory handle
    LPTOKEN      lpMemTknLine;          // Ptr to tokenized line global memory
    TOKEN_TYPES  TknType;               // Return value token type

    // If the stmts are on the same line, ...
    if (lptdArg1->uLineNum EQ lpplLocalVars->uLineNum)
       return lpplLocalVars->lptkStart[lptdArg1->uTknNum].tkFlags.TknType;
    else
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (lpplLocalVars->hGlbDfnHdr);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        Assert (lptdArg1->uLineNum > 0);

        // Get the given line's tokenized global memory handle
        hGlbTknLine = lpFcnLines[lptdArg1->uLineNum - 1].hGlbTknLine;

        // We no longer need this ptr
        MyGlobalUnlock (lpplLocalVars->hGlbDfnHdr); lpMemDfnHdr = NULL;

        // Lock the memory to get a ptr to it
        lpMemTknLine = MyGlobalLock (hGlbTknLine);

        // Skip over the header to the data
        lpMemTknLine = TokenBaseToStart (lpMemTknLine);

        // Get the given token's type
        TknType = lpMemTknLine[lptdArg1->uTknNum].tkFlags.TknType;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTknLine); lpMemTknLine = NULL;

        return TknType;
    } // End IF/ELSE
} // End CS_GetTokenType


//***************************************************************************
//  $CS_GetEOSTokenLen
//
//  Return the length of the EOS token which precedes a given token
//    (pl_yyparse)
//***************************************************************************

UINT CS_GetEOSTokenLen
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to PL local vars
     LPTOKEN_DATA  lptdArg1)            // Ptr to arg TOKEN_DATA

{
    LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator global memory
    LPFCNLINE    lpFcnLines;            // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HGLOBAL      hGlbTknLine;           // Tokenized line global memory handle
    LPTOKEN      lpMemTknLine;          // Ptr to tokenized line global memory
    UINT         TknLen;                // Return value token length

    // If the stmts are on the same line, ...
    if (lptdArg1->uLineNum EQ lpplLocalVars->uLineNum)
    {
        // Get the address of the token
        lpMemTknLine = &lpplLocalVars->lptkStart[lptdArg1->uTknNum];

        Assert (lpMemTknLine->tkFlags.TknType EQ TKT_EOS
             || lpMemTknLine->tkFlags.TknType EQ TKT_EOL);

        // Get the token length
        TknLen = lpMemTknLine->tkData.tkIndex;
    } else
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (lpplLocalVars->hGlbDfnHdr);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        Assert (lptdArg1->uLineNum > 0);

        // Get the given line's tokenized global memory handle
        hGlbTknLine = lpFcnLines[lptdArg1->uLineNum - 1].hGlbTknLine;

        // We no longer need this ptr
        MyGlobalUnlock (lpplLocalVars->hGlbDfnHdr); lpMemDfnHdr = NULL;

        // Lock the memory to get a ptr to it
        lpMemTknLine = MyGlobalLock (hGlbTknLine);

        // Skip over the header to the data
        lpMemTknLine = TokenBaseToStart (lpMemTknLine);

        // Get the address of the token
        lpMemTknLine = &lpMemTknLine[lptdArg1->uTknNum];

        Assert (lpMemTknLine->tkFlags.TknType EQ TKT_EOS
             || lpMemTknLine->tkFlags.TknType EQ TKT_EOL);

        // Get the token length
        TknLen = lpMemTknLine->tkData.tkIndex;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTknLine); lpMemTknLine = NULL;
    } // End IF/ELSE

    return TknLen;
} // End CS_GetEOSTokenLen


//***************************************************************************
//  End of File: cs_proc.c
//***************************************************************************
