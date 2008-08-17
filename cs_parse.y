//***************************************************************************
//  NARS2000 -- Parser Grammar for Control Structures
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

%{
#include <windows.h>
#include <stdio.h>
//#include <string.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "cs_parse.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

////#define YYLEX_DEBUG
////#define YYFPRINTF_DEBUG

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               cs_yyfprintf
#endif

#define    YYSTYPE     CS_YYSTYPE
#define  LPYYSTYPE   LPCS_YYSTYPE
#define tagYYSTYPE  tagCS_YYSTYPE

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         cs_yy_symbol_print
#define yy_symbol_value_print   cs_yy_symbol_value_print
#define yy_reduce_print         cs_yy_reduce_print
#define yydestruct              cs_yydestruct

////#define DbgMsgWP(a)         DbgMsgW(a)
////#define DbgMsgWP(a)         DbgMsgW(a); DbgBrk ()
////#define DbgMsgWP(a)         DbgMsgW(a)
    #define DbgMsgWP(a)

LPTOKEN     lptk1st, lptkPrv;
TOKEN_TYPES TknType;
TOKEN       tkTmp;

%}

%pure-parser
%name-prefix="cs_yy"
%parse-param {LPCSLOCALVARS lpcsLocalVars}
%lex-param   {LPCSLOCALVARS lpcsLocalVars}
%token  ANDIF       CASE        CASELIST    CONTINUE    ELSE        ELSEIF      END
%token  ENDFOR      ENDIF       ENDREPEAT   ENDSELECT   ENDWHILE    EOS         FOR
%token  GOTO        IF          INFOR       LEAVE       NEC         ORIF        REPEAT
%token  RETURN      SELECT      SKIPCASE    SKIPEND     SOS         UNK         UNTIL
%token  WHILE

%start CtrlStruc

%%

CtrlStruc:
    Stmts                                                       {DbgMsgWP (L"%%CtrlStruc:  Stmts");
                                                                    $$ = $1;
                                                                }
  | Stmts  CSRec                                                {DbgMsgWP (L"%%CtrlStruc:  Stmts CSRec");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $2;
                                                                }
  |        CSRec                                                {DbgMsgWP (L"%%CtrlStruc:  CSRec");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  ;

CSRec:
          ForStmt                                               {DbgMsgWP (L"%%CSRec:  ForStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       GotoStmt                                              {DbgMsgWP (L"%%CSRec:  GotoStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       IfStmt                                                {DbgMsgWP (L"%%CSRec:  IfStmt");
                                                                    // IfStmt is the only stmt allowed to have an unmatched ContinueLeave
                                                                    $$ = $1;
                                                                }
  |       RepeatStmt                                            {DbgMsgWP (L"%%CSRec:  RepeatStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       ReturnStmt                                            {DbgMsgWP (L"%%CSRec:  ReturnStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       SelectStmt                                            {DbgMsgWP (L"%%CSRec:  SelectStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  |       WhileStmt                                             {DbgMsgWP (L"%%CSRec:  WhileStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $1
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | CSRec ForStmt                                               {DbgMsgWP (L"%%CSRec:  CSRec ForStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In case $1 has an unmatched ContinueLeave in it,
                                                                    //   transfer the ptr and index to the result
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | CSRec GotoStmt                                              {DbgMsgWP (L"%%CSRec:  CSRec GotoStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In case $1 has an unmatched ContinueLeave in it,
                                                                    //   transfer the ptr and index to the result
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | CSRec IfStmt                                                {DbgMsgWP (L"%%CSRec:  CSRec IfStmt");
                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        // If $2 has an unmatched ContinueLeave, ...
                                                                        if ($2.lptkCL1st)
                                                                        // Loop through $2's unmatched ContinueLeave
                                                                        for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $1's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $2.uCLIndex                 =
                                                                        $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $2.lptkCL1st                =
                                                                        $2.lptk1st                  = $1.lptkCL1st;
                                                                    } // End IF

                                                                    $$ = $2;
                                                                }
  | CSRec RepeatStmt                                            {DbgMsgWP (L"%%CSRec:  CSRec RepeatStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In case $1 has an unmatched ContinueLeave in it,
                                                                    //   transfer the ptr and index to the result
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | CSRec ReturnStmt                                            {DbgMsgWP (L"%%CSRec:  CSRec ReturnStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In case $1 has an unmatched ContinueLeave in it,
                                                                    //   transfer the ptr and index to the result
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | CSRec SelectStmt                                            {DbgMsgWP (L"%%CSRec:  CSRec SelectStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In case $1 has an unmatched ContinueLeave in it,
                                                                    //   transfer the ptr and index to the result
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | CSRec WhileStmt                                             {DbgMsgWP (L"%%CSRec:  CSRec WhileStmt");
                                                                    // Ensure there is no unmatched ContinueLeave in $2
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In case $1 has an unmatched ContinueLeave in it,
                                                                    //   transfer the ptr and index to the result
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  ;

CSCLCSRec:
    /* Empty */                                                 {DbgMsgWP (L"%%CSCLCSRec:  <empty>");
                                                                    ZeroMemory (&$$, sizeof ($$));
                                                                }
  |             CSRec                                           {DbgMsgWP (L"%%CSCLCSRec:  CSRec");
                                                                    $$ = $1;
                                                                }
  |       CLRec                                                 {DbgMsgWP (L"%%CSCLCSRec:  CLRec");
                                                                    $$ = $1;
                                                                }
  |       CLRec CSRec                                           {DbgMsgWP (L"%%CSCLCSRec:  CLRec CSRec");
                                                                    // Note that righthand CSRec is never executed

                                                                    // If $2 has an unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    // Loop through $2's unmatched ContinueLeave
                                                                    for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $1's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // Copy the ContinueLeave index
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    // Move the ptr to encompass $1's ContinueLeave
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;

                                                                    $$ = $2;
                                                                }
  | CSRec CLRec CSRec                                           {DbgMsgWP (L"%%CSCLCSRec:  CSRec CLRec CSRec");
                                                                    // Note that righthand CSRec is never executed

                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                    // Loop through $1's unmatched ContinueLeave
                                                                    for (lptk1st = $1.lptkCL1st; lptk1st <= $1.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $1.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $2's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $2.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    // Loop through $3's unmatched ContinueLeave
                                                                    for (lptk1st = $3.lptkCL1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $2's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $2.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // Copy the ContinueLeave index
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $2.uCLIndex;

                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $3.lptkCL1st            =
                                                                        $3.lptk1st              = $1.lptkCL1st;
                                                                    else
                                                                        // Move the ptr to encompass $2's ContinueLeave
                                                                        $3.lptkCL1st            =
                                                                        $3.lptk1st              = $2.lptkCL1st;

                                                                    $$ = $3;
                                                                }
  ;

NS:
    NEC SOS                                                     {DbgMsgWP (L"%%NS:  NEC SOS");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  ;

NSS:
    NEC SOS                                                     {DbgMsgWP (L"%%NSS:  NEC SOS");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  | NEC SOS Stmts                                               {DbgMsgWP (L"%%NSS:  NEC SOS Stmts");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  ;

Stmts:
          EOS NEC SOS                                           {DbgMsgWP (L"%%Stmts:  EOS NEC SOS");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  | Stmts EOS NEC SOS                                           {DbgMsgWP (L"%%Stmts:  Stmts EOS NEC SOS");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  ;

SOSStmts:
    SOS                                                         {DbgMsgWP (L"%%SOSStmts:  SOS");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  | SOS Stmts                                                   {DbgMsgWP (L"%%SOSStmts:  SOS Stmts");
                                                                    // No return value needed
                                                                    $$ = $1;
                                                                }
  ;

EndFor:
    ENDFOR    SOSStmts                                          {DbgMsgWP (L"%%EndFor:  ENDFOR SOSStmts");
                                                                    // Ensure the ENDFOR token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndFor:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDFOR to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDFOR);

                                                                    $$ = $1;
                                                                }
  ;

EndIf:
    ENDIF     SOSStmts                                          {DbgMsgWP (L"%%EndIf:  ENDIF SOSStmts");
                                                                    // Ensure the ENDIF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndIf:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDIF to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDIF);

                                                                    $$ = $1;
                                                                }
  ;

EndRepeat:
    ENDREPEAT SOSStmts                                          {DbgMsgWP (L"%%EndRepeat:  ENDREPEAT SOSStmts");
                                                                    // Ensure the ENDREPEAT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndRepeat:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDREPEAT to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDREPEAT);

                                                                    $$ = $1;
                                                                }
  ;

EndSelect:
    ENDSELECT SOSStmts                                          {DbgMsgWP (L"%%EndSelect:  ENDSELECT SOSStmts");
                                                                    // Ensure the ENDSELECT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndSelect:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDSELECT to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDSELECT);

                                                                    $$ = $1;
                                                                }
  ;

EndWhile:
    ENDWHILE  SOSStmts                                          {DbgMsgWP (L"%%EndWhile:  ENDWHILE SOSStmts");
                                                                    // Ensure the ENDWHILE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | END       SOSStmts                                          {DbgMsgWP (L"%%EndWhile:  END SOSStmts");
                                                                    // Ensure the END token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Change token END to ENDWHILE to provide a consistent ending
                                                                    CS_ChangeTokenType (lpcsLocalVars, $1.lptkCur, TKT_CS_ENDWHILE);

                                                                    $$ = $1;
                                                                }
  ;

CLRec:
                CONTINUE SOSStmts                               {DbgMsgWP (L"%%CLRec:  CONTINUE SOSStmts");
                                                                    // Ensure the CONTINUE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save the starting index common to this stmt
                                                                    $1.uCLIndex                 =
                                                                    $1.lptkCur->tkData.uCLIndex = lpcsLocalVars->lptkCSLink->tkData.uCLIndex;
                                                                    lpcsLocalVars->lptkCSLink->tkData.uCLIndex = 0;
                                                                    $1.lptkCL1st = $1.lptk1st = $1.lptkCur;

                                                                    $$ = $1;
                                                                }
  |             LEAVE    SOSStmts                               {DbgMsgWP (L"%%CLRec:  LEAVE SOSStmts");
                                                                    // Ensure the LEAVE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save the starting index common to this stmt
                                                                    $1.uCLIndex                 =
                                                                    $1.lptkCur->tkData.uCLIndex = lpcsLocalVars->lptkCSLink->tkData.uCLIndex;
                                                                    lpcsLocalVars->lptkCSLink->tkData.uCLIndex = 0;
                                                                    $1.lptkCL1st = $1.lptk1st = $1.lptkCur;

                                                                    $$ = $1;
                                                                }
  | CLRec       CONTINUE SOSStmts                               {DbgMsgWP (L"%%CLRec:  CLRec CONTINUE SOSStmts");
                                                                    // Ensure the CONTINUE token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    //   and the index common to this group
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | CLRec CSRec CONTINUE SOSStmts                               {DbgMsgWP (L"%%CLRec:  CLRec CSRec CONTINUE SOSStmts");
                                                                    // Note that righthand CSRec is never executed

                                                                    // Ensure the CONTINUE token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $2 has unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        Assert ($2.lptkCur->tkFlags.TknType EQ TKT_CS_IF);
                                                                    } // End IF

                                                                    // If $2 has unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    // Loop through $2's unmatched ContinueLeave
                                                                    for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $1's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    //   and the index common to this group
                                                                    $3.lptkCL1st                =
                                                                    $3.lptk1st                  = $1.lptkCL1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  | CLRec       LEAVE    SOSStmts                               {DbgMsgWP (L"%%CLRec:  CLRec LEAVE SOSStmts");
                                                                    // Ensure the LEAVE token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    //   and the index common to this group
                                                                    $2.lptkCL1st                =
                                                                    $2.lptk1st                  = $1.lptkCL1st;
                                                                    $2.uCLIndex                 =
                                                                    $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $2;
                                                                }
  | CLRec CSRec LEAVE    SOSStmts                               {DbgMsgWP (L"%%CLRec:  CLRec CSRec LEAVE SOSStmts");
                                                                    // Note that righthand CSRec is never executed

                                                                    // Ensure the LEAVE token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $2 has unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        Assert ($2.lptkCur->tkFlags.TknType EQ TKT_CS_IF);
                                                                    } // End IF

                                                                    // If $2 has unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    // Loop through $2's unmatched ContinueLeave
                                                                    for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Convert to $1's CLIndex so they are all the same
                                                                            lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                    } // End IF/FOR/IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    //   and the index common to this group
                                                                    $3.lptkCL1st                =
                                                                    $3.lptk1st                  = $1.lptkCL1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  ;

ForStmt:
    FOR NEC INFOR NSS CSCLCSRec EndFor                          {DbgMsgWP (L"%%ForStmt:  FOR NEC INFOR NSS CSCLCSRec EndFor");
                                                                    // Ensure the FOR token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Ensure the INFOR token is not SOS
                                                                    if ($3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $5 has an unmatched ContinueLeave, ...
                                                                    if ($5.lptkCL1st)
                                                                    // Loop through $5's unmatched ContinueLeave
                                                                    for (lptk1st = $5.lptkCL1st; lptk1st <= $5.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $5.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the EndFor token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $6.lptkCur);
                                                                    } // End IF/FOR/IF

                                                                    // Copy the FOR token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkCur;

                                                                    // Chain together the FOR token and the EndFor token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $6.lptkCur);

                                                                    // Chain together the EndFor token and the FOR token
                                                                    CS_ChainTokens (lpcsLocalVars, &$6.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $6;
                                                                }
  ;

GotoStmt:
    GOTO NSS                                                    {DbgMsgWP (L"%%GotoStmt:  GOTO NSS");
                                                                    // Ensure the GOTO token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  ;

OrIfRec:
                 ORIF                                           {DbgMsgWP (L"%%OrIfRec:  ORIF");
                                                                    // Ensure the ORIF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save as 1st token in sequence
                                                                    $1.lptk1st = $1.lptkCur;

                                                                    $$ = $1;
                                                                }
  | OrIfRec  NS  ORIF                                           {DbgMsgWP (L"%%OrIfRec:  OrIfRec NS ORIF");
                                                                    // Ensure the ORIF token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptk1st = $1.lptk1st;

                                                                    $$ = $3;
                                                                }
  ;

AndIfRec:
                 ANDIF                                          {DbgMsgWP (L"%%AndIfRec:  ANDIF");
                                                                    // Ensure the ANDIF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save as 1st token in sequence
                                                                    $1.lptk1st = $1.lptkCur;

                                                                    $$ = $1;
                                                                }
  | AndIfRec NS  ANDIF                                          {DbgMsgWP (L"%%AndIfRec:  AndIfRec NS ANDIF");
                                                                    // Ensure the ANDIF token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptk1st = $1.lptk1st;

                                                                    $$ = $3;
                                                                }
  ;

OrIfAndIf:
    OrIfRec  NSS                                                {DbgMsgWP (L"%%OrIfAndIf: OrIfRec NSS");
                                                                    $$ = $1;
                                                                }
  | AndIfRec NSS                                                {DbgMsgWP (L"%%OrIfAndIf: AndIfRec NSS");
                                                                    $$ = $1;
                                                                }
  ;


SkipEnd:
    SKIPEND SOS                                                 {DbgMsgWP (L"%%SkipEnd:  SKIPEND SOS");
                                                                    $$ = $1;
                                                                }
  ;

ElseIf:
            SkipEnd ELSEIF NSS                                  {DbgMsgWP (L"%%ElseIf:  ELSEIF NSS");
                                                                    // Ensure the ELSEIF token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Set the next .uCLIndex value
                                                                    CS_SetCLIndex (&$2);

                                                                    // Chain together the SkipEnd token and the ELSEIF token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $2.lptk1st);

                                                                    $$ = $2;
                                                                }
  |         SkipEnd ELSEIF NS  OrIfAndIf                        {DbgMsgWP (L"%%ElseIf:  ELSEIF NS OrIfAndIf");
                                                                    // Ensure the ELSEIF token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Set the next .uCLIndex value
                                                                    CS_SetCLIndex (&$2);

                                                                    // Loop through $4's tokens setting that token's .uCLIndex to $2's
                                                                    for (lptk1st = $4.lptk1st; lptk1st <= $4.lptkCur; lptk1st++)
                                                                    // If it's an ANDIF or ORIF, ...
                                                                    if (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                     || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF)
                                                                        lptk1st->tkData.uCLIndex = $2.uCLIndex;

                                                                    // Chain together the SkipEnd token and the ELSEIF token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $2.lptk1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $4.lptk1st                  = $2.lptkCur;
                                                                    $4.uCLIndex                 =
                                                                    $4.lptkCur->tkData.uCLIndex = $2.uCLIndex;

                                                                    $$ = $4;
                                                                }
  ;

ElseIfRec:
    ElseIf                                                      {DbgMsgWP (L"%%ElseIfRec:  ElseIf");
                                                                    $$ = $1;
                                                                }
  | ElseIfRec CSRec ElseIf                                      {DbgMsgWP (L"%%ElseIfRec:  ElseIfRec CSRec ElseIf");
                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        // If $2 has an unmatched ContinueLeave, ...
                                                                        if ($2.lptkCL1st)
                                                                        // Loop through $2's unmatched ContinueLeave
                                                                        for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $1's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $2.uCLIndex                 =
                                                                        $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $2.lptkCL1st                =
                                                                        $2.lptk1st                  = $1.lptkCL1st;
                                                                    } // End IF

                                                                    // Loop through $3's tokens converting that token's .uCLIndex to $1's
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's the same .uCLIndex
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                        lptk1st->tkData.uCLIndex = $1.uCLIndex;

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptkCL1st                =
                                                                    $3.lptk1st                  = $1.lptkCL1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  ;

ElseIfElse:
                    ELSE   NSS                                  {DbgMsgWP (L"%%ElseIfElse:  ELSE NSS");
                                                                    // Ensure the ELSE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Set the next .uCLIndex value
                                                                    CS_SetCLIndex (&$1);

                                                                    $$ = $1;
                                                                }
  | ElseIfRec CSRec ELSE   NSS                                  {DbgMsgWP (L"%%ElseIfElse:  ElseIfRec CSRec ELSE NSS");
                                                                    // Ensure the ELSE token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $1 has an unmatched ContinueLeave, ...
                                                                    if ($1.lptkCL1st)
                                                                    {
                                                                        // If $2 has an unmatched ContinueLeave, ...
                                                                        if ($2.lptkCL1st)
                                                                        // Loop through $2's unmatched ContinueLeave
                                                                        for (lptk1st = $2.lptkCL1st; lptk1st <= $2.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $2.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $1's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $1.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $2.uCLIndex                 =
                                                                        $2.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $2.lptkCL1st                =
                                                                        $2.lptk1st                  = $1.lptkCL1st;
                                                                    } // End IF

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptkCL1st                =
                                                                    $3.lptk1st                  = $1.lptkCL1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  ;

IfStmt:
    IF NSS                                CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NSS CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the IF token and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $4.lptkCur);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $4.lptkCL1st                =
                                                                    $4.lptk1st                  = $3.lptkCL1st;
                                                                    $4.uCLIndex                 =
                                                                    $4.lptkCur->tkData.uCLIndex = $3.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  | IF NSS           CSCLCSRec ElseIfRec  CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NSS CSCLCSRec ElseIfRec CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    {
                                                                        // If $5 has an unmatched ContinueLeave, ...
                                                                        if ($5.lptkCL1st)
                                                                        // Loop through $5's unmatched ContinueLeave
                                                                        for (lptk1st = $5.lptkCL1st; lptk1st <= $5.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $5.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $3's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $3.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $5.uCLIndex                 =
                                                                        $5.lptkCur->tkData.uCLIndex = $3.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $5.lptkCL1st                =
                                                                        $5.lptk1st                  = $3.lptkCL1st;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // Loop through the tokens in ElseIfRec
                                                                    for (lptk1st = $4.lptk1st; lptk1st <= $4.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $4.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ELSEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF);

                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End FOR/IF

                                                                    // Chain together the last token in ElseIfRec and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $6.lptkCur);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $6.lptkCL1st                =
                                                                    $6.lptk1st                  = $5.lptkCL1st;
                                                                    $6.uCLIndex                 =
                                                                    $6.lptkCur->tkData.uCLIndex = $5.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $6;
                                                                }
  | IF NSS           CSCLCSRec ElseIfElse CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NSS CSCLCSRec ElseIfElse CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    {
                                                                        // If $5 has an unmatched ContinueLeave, ...
                                                                        if ($5.lptkCL1st)
                                                                        // Loop through $5's unmatched ContinueLeave
                                                                        for (lptk1st = $5.lptkCL1st; lptk1st <= $5.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $5.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $3's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $3.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $5.uCLIndex                 =
                                                                        $5.lptkCur->tkData.uCLIndex = $3.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $5.lptkCL1st                =
                                                                        $5.lptk1st                  = $3.lptkCL1st;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // Loop through the tokens in ElseIfElse
                                                                    for (lptk1st = $4.lptk1st; lptk1st <= $4.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $4.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ELSEIF
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF);

                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End FOR/IF

                                                                    // Chain together the last token in ElseIfElse and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $6.lptk1st);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $6.lptkCL1st                =
                                                                    $6.lptk1st                  = $5.lptkCL1st;
                                                                    $6.uCLIndex                 =
                                                                    $6.lptkCur->tkData.uCLIndex = $5.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $6;
                                                                }
  | IF NS  OrIfAndIf                      CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NS OrIfAndIf CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // Loop through the tokens in OrIfAndIf
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    if (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                     || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF)
                                                                    {
                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End FOR/IF

                                                                    // Chain together the last token in OrIfAndIf and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $5.lptkCur);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $5.lptkCL1st                =
                                                                    $5.lptk1st                  = $4.lptkCL1st;
                                                                    $5.uCLIndex                 =
                                                                    $5.lptkCur->tkData.uCLIndex = $4.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $5;
                                                                }
  | IF NS  OrIfAndIf CSCLCSRec ElseIfRec  CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NS OrIfAndIf CSCLCSRec ElseIfRec CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $4 has an unmatched ContinueLeave, ...
                                                                    if ($4.lptkCL1st)
                                                                    {
                                                                        // If $6 has an unmatched ContinueLeave, ...
                                                                        if ($6.lptkCL1st)
                                                                        // Loop through $6's unmatched ContinueLeave
                                                                        for (lptk1st = $6.lptkCL1st; lptk1st <= $6.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $6.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $4's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $4.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $6.uCLIndex                 =
                                                                        $6.lptkCur->tkData.uCLIndex = $4.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $6.lptkCL1st                =
                                                                        $6.lptk1st                  = $4.lptkCL1st;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // Loop through the tokens in OrIfAndIf
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    if (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                     || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF)
                                                                    {
                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End FOR/IF

                                                                    // Chain together the last token in OrIfAndIf and the 1st token in ElseIfRec
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $5.lptk1st);

                                                                    // Chain together the last token in ElseIfRec and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$5.lptkCur->tkData, $7.lptkCur);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $7.lptkCL1st                =
                                                                    $7.lptk1st                  = $6.lptkCL1st;
                                                                    $7.uCLIndex                 =
                                                                    $7.lptkCur->tkData.uCLIndex = $6.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $7;
                                                                }
  | IF NS  OrIfAndIf CSCLCSRec ElseIfElse CSCLCSRec EndIf       {DbgMsgWP (L"%%IfStmt:  IF NS OrIfAndIf CSCLCSRec ElseIfElse CSCLCSRec EndIf");
                                                                    // Ensure the IF token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $4 has an unmatched ContinueLeave, ...
                                                                    if ($4.lptkCL1st)
                                                                    {
                                                                        // If $6 has an unmatched ContinueLeave, ...
                                                                        if ($6.lptkCL1st)
                                                                        // Loop through $6's unmatched ContinueLeave
                                                                        for (lptk1st = $6.lptkCL1st; lptk1st <= $6.lptkCur; lptk1st++)
                                                                        // If it's a ContinueLeave, ...
                                                                        if (lptk1st->tkData.uCLIndex EQ $6.uCLIndex)
                                                                        {
                                                                            Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                                 || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                            // If it's not ENDIF, ...
                                                                            if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                                // Convert to $4's CLIndex so they are all the same
                                                                                lptk1st->tkData.uCLIndex = $4.uCLIndex;
                                                                        } // End IF/FOR/IF

                                                                        // Copy the ContinueLeave index
                                                                        $6.uCLIndex                 =
                                                                        $6.lptkCur->tkData.uCLIndex = $4.uCLIndex;

                                                                        // Move the ptr to encompass $1's ContinueLeave
                                                                        $6.lptkCL1st                =
                                                                        $6.lptk1st                  = $4.lptkCL1st;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // Loop through the tokens in OrIfAndIf
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    if (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                     || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF)
                                                                    {
                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End FOR/IF

                                                                    // Chain together the last token in OrIfAndIf and the 1st token in ElseIfElse
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $5.lptk1st);

                                                                    // Chain together the last token in ElseIfElse and the EndIf token
                                                                    CS_ChainTokens (lpcsLocalVars, &$5.lptkCur->tkData, $7.lptkCur);

                                                                    // In case there's an unmatched ContinueLeave in the IfBody,
                                                                    //   pass on the ptr and index
                                                                    $7.lptkCL1st                =
                                                                    $7.lptk1st                  = $6.lptkCL1st;
                                                                    $7.uCLIndex                 =
                                                                    $7.lptkCur->tkData.uCLIndex = $6.uCLIndex;

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $7;
                                                                }
  ;

Until:
    UNTIL NSS                                                    {DbgMsgWP (L"%%Until:  UNTIL NSS");
                                                                    // Ensure the UNTIL token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  | UNTIL NS  OrIfAndIf                                         {DbgMsgWP (L"%%Until:  UNTIL NS OrIfAndIf");
                                                                    // Ensure the UNTIL token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $3;
                                                                }
  ;

RepeatStmt:
    REPEAT SOSStmts CSCLCSRec EndRepeat                         {DbgMsgWP (L"%%RepeatStmt:  REPEAT SOSStmts CSCLCSRec EndRepeat");
                                                                    // Ensure the REPEAT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    // Loop through the ContinueLeave tokens
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the EndRepeat token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $4.lptkCur);
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the EndRepeat token and the REPEAT token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $1.lptkCur);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  | REPEAT SOSStmts CSCLCSRec Until                             {DbgMsgWP (L"%%RepeatStmt:  REPEAT SOSStmts CSCLCSRec Until");
                                                                    // Ensure the REPEAT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    // Loop through $3's unmatched ContinueLeave
                                                                    for (lptk1st = $3.lptkCL1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the 1st token in Until
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $4.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Chain together the last token in Until and the REPEAT token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $1.lptkCur);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  ;

ReturnStmt:
    RETURN SOSStmts                                             {DbgMsgWP (L"%%ReturnStmt:  RETURN SOSStmts");
                                                                    // Ensure the RETURN token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  ;

SkipCase:
    SKIPCASE SOS                                                {DbgMsgWP (L"%%SkipCase:  SKIPCASE SOS");
                                                                    $$ = $1;
                                                                }
  ;

CCListRec:
                    SkipCase CASE     NSS                       {DbgMsgWP (L"%%CCListRec:  SkipCase CASE NSS");
                                                                    // Ensure the CASE token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Set the next .uCLIndex value
                                                                    CS_SetCLIndex (&$2);

                                                                    // Chain together the SkipCase token and the CASE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $2.lptk1st);

                                                                    $$ = $2;
                                                                }
  |                 SkipCase CASELIST NSS                       {DbgMsgWP (L"%%CCListRec:  SkipCase CASELIST NSS");
                                                                    // Ensure the CASELIST token is SOS
                                                                    if (!$2.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Set the next .uCLIndex value
                                                                    CS_SetCLIndex (&$2);

                                                                    // Chain together the SkipCase token and the CASELIST token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $2.lptk1st);

                                                                    $$ = $2;
                                                                }
  | CCListRec       SkipCase CASE     NSS                       {DbgMsgWP (L"%%CCListRec:  CCListRec SkipCase CASE NSS");
                                                                    // Ensure the CASE token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the SkipCase token and the CASELIST token
                                                                    CS_ChainTokens (lpcsLocalVars, &$2.lptkCur->tkData, $3.lptk1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptk1st                  = $1.lptk1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  | CCListRec CSRec SkipCase CASE     NSS                       {DbgMsgWP (L"%%CCListRec:  CCListRec CSRec SkipCase CASE NSS");
                                                                    // Ensure the CASE token is SOS
                                                                    if (!$4.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$4.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $2 has an unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCL1st;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the SkipCase token and the CASELIST token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $4.lptk1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $4.lptk1st                  = $1.lptk1st;
                                                                    $4.uCLIndex                 =
                                                                    $4.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $4;
                                                                }
  | CCListRec       SkipCase CASELIST NSS                       {DbgMsgWP (L"%%CCListRec:  CCListRec SkipCase CASELIST NSS");
                                                                    // Ensure the CASELIST token is SOS
                                                                    if (!$3.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$3.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the SkipCase token and the CASELIST token
                                                                    CS_ChainTokens (lpcsLocalVars, &$2.lptkCur->tkData, $3.lptk1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $3.lptk1st                  = $1.lptk1st;
                                                                    $3.uCLIndex                 =
                                                                    $3.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $3;
                                                                }
  | CCListRec CSRec SkipCase CASELIST NSS                       {DbgMsgWP (L"%%CCListRec:  CCListRec CSRec SkipCase CASELIST NSS");
                                                                    // Ensure the CASELIST token is SOS
                                                                    if (!$4.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$4.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $2 has an unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCL1st;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Chain together the SkipCase token and the CASELIST token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $4.lptk1st);

                                                                    // In this partial sequence, pass on down a ptr to the first entry
                                                                    $4.lptk1st                  = $1.lptk1st;
                                                                    $4.uCLIndex                 =
                                                                    $4.lptkCur->tkData.uCLIndex = $1.uCLIndex;

                                                                    $$ = $4;
                                                                }
  ;

CCListCS:
    /* Empty */                                                 {DbgMsgWP (L"%%CCListCS:  <empty>");
                                                                    ZeroMemory (&$$, sizeof ($$));
                                                                }
  | CCListRec                                                   {DbgMsgWP (L"%%CCListCS:  CCListRec");
                                                                    $$ = $1;
                                                                }
  | CCListRec CSRec                                             {DbgMsgWP (L"%%CCListCS:  CCListRec CSRec");
                                                                    // If $2 has an unmatched ContinueLeave, ...
                                                                    if ($2.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$2.lptkCL1st;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    $$ = $1;
                                                                }
  ;

SelectStmt:
    SELECT NS CCListCS                     EndSelect            {DbgMsgWP (L"%%SelectStmt:  SELECT NS CCListCS EndSelect");
                                                                    // Ensure the SELECT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // Loop through the tokens in CCListCS
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_CASE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CASELIST);

                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End FOR/IF

                                                                    // Chain together the last token in CCListCS and the EndSelect token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $4.lptkCur);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  | SELECT NS CCListCS ELSE SOSStmts       EndSelect            {DbgMsgWP (L"%%SelectStmt:  SELECT NS CCListCS ELSE SOSStmts EndSelect");
                                                                    // Ensure the SELECT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Ensure the ELSE token is SOS
                                                                    if (!$4.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$4.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // Loop through the tokens in CCListCS
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_CASE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CASELIST);

                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End FOR/IF

                                                                    // Chain together the last token in CCListCS and the ELSE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $4.lptkCur);

                                                                    // Chain together the ELSE token and the EndSelect token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $6.lptkCur);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $6;
                                                                }
  | SELECT NS CCListCS ELSE SOSStmts CSRec EndSelect            {DbgMsgWP (L"%%SelectStmt:  SELECT NS CCListCS ELSE SOSStmts CSRec EndSelect");
                                                                    // Ensure the SELECT token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Ensure the ELSE token is SOS
                                                                    if (!$4.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$4.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $5 has an unmatched ContinueLeave, ...
                                                                    if ($5.lptkCL1st)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$5.lptkCL1st;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // Loop through the tokens in CCListCS
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's in the same sequence, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_CASE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CASELIST);

                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End FOR/IF

                                                                    // Chain together the last token in CCListCS and the ELSE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $4.lptkCur);

                                                                    // Chain together the ELSE token and the EndSelect token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, $7.lptkCur);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $7;
                                                                }
  ;

WhileStmt:
    WHILE NSS           CSCLCSRec EndWhile                      {DbgMsgWP (L"%%WhileStmt:  WHILE NSS CSCLCSRec EndWhile");
                                                                    // Ensure the WHILE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    // Loop through the ContinueLeave tokens
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the EndWhile token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $4.lptkCur);
                                                                    } // End IF/FOR/IF

                                                                    // Copy the WHILE token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkCur;

                                                                    // Chain together the WHILE token and the EndWhile token
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $4.lptkCur);

                                                                    // Chain together the EndWhile token and the WHILE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  | WHILE NS  OrIfAndIf CSCLCSRec EndWhile                      {DbgMsgWP (L"%%WhileStmt:  WHILE NS OrIfAndIf CSCLCSRec EndWhile");
                                                                    // Ensure the WHILE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $4 has an unmatched ContinueLeave, ...
                                                                    if ($4.lptkCL1st)
                                                                    // Loop through $4's unmatched ContinueLeave
                                                                    for (lptk1st = $4.lptkCL1st; lptk1st <= $4.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $4.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the EndWhile token
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $5.lptkCur);
                                                                    } // End IF/FOR/IF

                                                                    // Copy the WHILE token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkCur;

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // Loop through the tokens in OrIfAndIf
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    if (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                     || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF)
                                                                    {
                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End FOR/IF

                                                                    // Chain together the last token in OrIfAndIf and the EndWhile token
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $5.lptkCur);

                                                                    // Chain together the EndWhile token and the WHILE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$5.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $5;
                                                                }
  | WHILE NSS           CSCLCSRec Until                         {DbgMsgWP (L"%%WhileStmt:  WHILE NSS CSCLCSRec Until");
                                                                    // Ensure the WHILE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $3 has an unmatched ContinueLeave, ...
                                                                    if ($3.lptkCL1st)
                                                                    // Loop through $3's unmatched ContinueLeave
                                                                    for (lptk1st = $3.lptkCL1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $3.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the 1st token in Until
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $4.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Copy the WHILE token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkCur;

                                                                    // Chain together the WHILE token and the 1st token in Until
                                                                    CS_ChainTokens (lpcsLocalVars, &$1.lptkCur->tkData, $4.lptk1st);

                                                                    // Chain together the last token in Until and the WHILE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$4.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $4;
                                                                }
  | WHILE NS  OrIfAndIf CSCLCSRec Until                         {DbgMsgWP (L"%%WhileStmt:  WHILE NS OrIfAndIf CSCLCSRec Until");
                                                                    // Ensure the WHILE token is SOS
                                                                    if (!$1.lptkCur->tkData.bSOS)
                                                                    {
                                                                        lpcsLocalVars->tkCSErr = *$1.lptkCur;
                                                                        YYERROR;
                                                                    } // End IF

                                                                    // If $4 has an unmatched ContinueLeave, ...
                                                                    if ($4.lptkCL1st)
                                                                    // Loop through $4's unmatched ContinueLeave
                                                                    for (lptk1st = $4.lptkCL1st; lptk1st <= $4.lptkCur; lptk1st++)
                                                                    // If it's a ContinueLeave, ...
                                                                    if (lptk1st->tkData.uCLIndex EQ $4.uCLIndex)
                                                                    {
                                                                        Assert (lptk1st->tkFlags.TknType EQ TKT_CS_LEAVE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_CONTINUE
                                                                             || lptk1st->tkFlags.TknType EQ TKT_CS_ENDIF);

                                                                        // If it's not ENDIF, ...
                                                                        if (lptk1st->tkFlags.TknType NE TKT_CS_ENDIF)
                                                                            // Chain together the LEAVE/CONTINUE token and the 1st token in Until
                                                                            CS_ChainTokens (lpcsLocalVars, &lptk1st->tkData, $5.lptk1st);
                                                                    } // End IF/FOR/IF

                                                                    // Copy the WHILE token as the next call to CS_ChainTokens clobbers it
                                                                    tkTmp = *$1.lptkCur;

                                                                    // Save as the ptr to the previous token
                                                                    lptkPrv = $1.lptkCur;

                                                                    // Loop through the tokens in OrIfAndIf
                                                                    for (lptk1st = $3.lptk1st; lptk1st <= $3.lptkCur; lptk1st++)
                                                                    if (lptk1st->tkFlags.TknType EQ TKT_CS_ANDIF
                                                                     || lptk1st->tkFlags.TknType EQ TKT_CS_ORIF)
                                                                    {
                                                                        // Chain together lptkPrv and lptk1st
                                                                        CS_ChainTokens (lpcsLocalVars, &lptkPrv->tkData, lptk1st);

                                                                        // Save as the ptr to the previous token
                                                                        lptkPrv = lptk1st;
                                                                    } // End FOR/IF

                                                                    // Chain together the last token in OrIfAndIf and the 1st token in Until
                                                                    CS_ChainTokens (lpcsLocalVars, &$3.lptkCur->tkData, $5.lptk1st);

                                                                    // Chain together the last token in Until and the WHILE token
                                                                    CS_ChainTokens (lpcsLocalVars, &$5.lptkCur->tkData, &tkTmp);

                                                                    // Unlink this stmt
                                                                    CS_UnlinkStmt (lpcsLocalVars, &$1);

                                                                    $$ = $5;
                                                                }
  ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************

//***************************************************************************
//  $ParseCtrlStruc_EM
//
//  Parse function Control Structures
//***************************************************************************

UBOOL ParseCtrlStruc_EM
    (LPCSLOCALVARS lpcsLocalVars)           // Ptr to CS Local vars

{
    // Clear the error token
    ZeroMemory (&lpcsLocalVars->tkCSErr, sizeof (lpcsLocalVars->tkCSErr));

    // Append the CS_EOL token to the CS stack
    //   to allow for later parsing for SYNTAX ERRORs
    AppendNewCSToken_EM (TKT_CS_EOL,
                         0,
                         0,
                         0,
                         TRUE,
                         NEG1U);
    // Use VirtualAlloc for the parser stack
    // ***FIXME***














#if YYDEBUG
    // Enable debugging
    yydebug = TRUE;
#endif

    // Parse the CS stack, check for errors
    //   0 = success
    //   1 = YYABORT or APL error
    //   2 = memory exhausted
    return cs_yyparse (lpcsLocalVars) EQ 0;
} // End ParseCtrlStruc_EM


//***************************************************************************
//  $cs_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

int cs_yylex
    (LPCS_YYSTYPE  lpYYLval,            // Ptr to yylval
     LPCSLOCALVARS lpcsLocalVars)       // Ptr to local csLocalVars

{
    LPTOKEN     lptkCSCur;              // Ptr to current token (the one we return)
    TOKEN_TYPES TknType;                // Token type of current token

CS_YYLEX_START:
    // Save the ptr to the current CS_YYSTYPE and skip over it
    lptkCSCur = lpcsLocalVars->lptkCSNxt++;

    // Return the current token
    lpYYLval->lptkCur   =               // Mark as the current token
    lpYYLval->lptk1st   =               // Mark as the first in a sequence
    lpYYLval->lptkNxt   = lptkCSCur;    // ...         next  ...
    lpYYLval->lptkCL1st = NULL;         // No ContinueLeave ptr
    lpYYLval->uCLIndex  = 0;            // No ContinueLeave index

    // Split cases based upon the token type
    switch (lptkCSCur->tkFlags.TknType)
    {
        case TKT_CS_EOL:
            return '\0';

        case TKT_EOS:
        case TKT_EOL:
            // If the next token is LABELSEP, check the token after that
            if (lpcsLocalVars->lptkCSNxt->tkFlags.TknType EQ TKT_LABELSEP)
                TknType = lpcsLocalVars->lptkCSNxt[1].tkFlags.TknType;
            else
                TknType = lpcsLocalVars->lptkCSNxt[0].tkFlags.TknType;

            // If the next token of interest is not EOS, INFOR, NEC, or SOS,
            //   then skip this token and return the next token.
            // This avoids having to insert an EOS before almost every token.
            if (TknType EQ TKT_CS_ANDIF
             || TknType EQ TKT_CS_CASE
             || TknType EQ TKT_CS_CASELIST
             || TknType EQ TKT_CS_CONTINUE
             || TknType EQ TKT_CS_ELSE
             || TknType EQ TKT_CS_ELSEIF
             || TknType EQ TKT_CS_END
             || TknType EQ TKT_CS_ENDFOR
             || TknType EQ TKT_CS_ENDIF
             || TknType EQ TKT_CS_ENDREPEAT
             || TknType EQ TKT_CS_ENDSELECT
             || TknType EQ TKT_CS_ENDWHILE
             || TknType EQ TKT_CS_FOR
             || TknType EQ TKT_CS_GOTO
             || TknType EQ TKT_CS_IF
             || TknType EQ TKT_CS_LEAVE
             || TknType EQ TKT_CS_ORIF
             || TknType EQ TKT_CS_REPEAT
             || TknType EQ TKT_CS_RETURN
             || TknType EQ TKT_CS_SELECT
             || TknType EQ TKT_CS_SKIPCASE
             || TknType EQ TKT_CS_SKIPEND
             || TknType EQ TKT_CS_UNTIL
             || TknType EQ TKT_CS_WHILE)
                goto CS_YYLEX_START;

            return EOS;

        case TKT_SOS:
            return SOS;

        case TKT_CS_ANDIF:                  // Control structure:  ANDIF     (Data is Line/Stmt #)
            return ANDIF;

        case TKT_CS_CASE:                   // ...                 CASE
            return CASE;

        case TKT_CS_CASELIST:               // ...                 CASELIST
            return CASELIST;

        case TKT_CS_CONTINUE:               // ...                 CONTINUE
            return CONTINUE;

        case TKT_CS_ELSE:                   // ...                 ELSE
            return ELSE;

        case TKT_CS_ELSEIF:                 // ...                 ELSEIF
            return ELSEIF;

        case TKT_CS_END:                    // ...                 END
            return END;

        case TKT_CS_ENDFOR:                 // ...                 ENDFOR
            return ENDFOR;

        case TKT_CS_ENDIF:                  // ...                 ENDIF
            return ENDIF;

        case TKT_CS_ENDREPEAT:              // ...                 ENDREPEAT
            return ENDREPEAT;

        case TKT_CS_ENDSELECT:              // ...                 ENDSELECT
            return ENDSELECT;

        case TKT_CS_ENDWHILE:               // ...                 ENDWHILE
            return ENDWHILE;

        case TKT_CS_FOR:                    // ...                 FOR
            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return FOR;

        case TKT_CS_GOTO:                   // ...                 GOTO
            return GOTO;

        case TKT_CS_IF:                     // ...                 IF
            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return IF;

        case TKT_CS_IN:                     // ...                 IN
            return INFOR;

        case TKT_CS_LEAVE:                  // ...                 LEAVE
            return LEAVE;

        case TKT_CS_ORIF:                   // ...                 ORIF
            return ORIF;

        case TKT_CS_NEC:                    // ...                 NEC
            return NEC;

        case TKT_CS_REPEAT:                 // ...                 REPEAT
            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return REPEAT;

        case TKT_CS_RETURN:                 // ...                 RETURN
            return RETURN;

        case TKT_CS_SELECT:                 // ...                 SELECT
            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return SELECT;

        case TKT_CS_SKIPCASE:               // ...                 Special token
            return SKIPCASE;

        case TKT_CS_SKIPEND:                // ...                 Special token
            return SKIPEND;

        case TKT_CS_UNTIL:                  // ...                 UNTIL
            return UNTIL;

        case TKT_CS_WHILE:                  // ...                 WHILE
            // Set start of stmt
            CS_LinkStmt (lpcsLocalVars, lpYYLval);

            return WHILE;

        case TKT_LABELSEP:
            // If the preceding token is NEC, delete it and ignore this one
            if (lpcsLocalVars->lptkCSNxt[-1].tkFlags.TknType EQ TKT_CS_NEC)
                // Delete the NEC token
                lpcsLocalVars->lptkCSNxt--;

            // Fall through to common code

        case TKT_CS_FOR2:
        case TKT_CS_IF2:
        case TKT_CS_REPEAT2:
        case TKT_CS_SELECT2:
        case TKT_CS_WHILE2:
            goto CS_YYLEX_START;            // Ignore these tokens

        defstop
            return UNK;
    } // End SWITCH
} // End cs_yylex


//***************************************************************************
//  $cs_yyerror
//
//  Error callback from Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- cs_yyerror"
#else
#define APPEND_NAME
#endif

void cs_yyerror                     // Called for Bison syntax error
    (LPCSLOCALVARS lpcsLocalVars,   // Ptr to local csLocalVars
     LPCHAR        s)               // Ptr to error msg

{
    char szTemp[1024], *p;
    UINT uLineNum,
         uLinePos;

#ifdef DEBUG
    DbgMsg (s);
#endif

    // If the error token is not already set, ...
    if (lpcsLocalVars->tkCSErr.tkFlags.TknType EQ 0)
        lpcsLocalVars->tkCSErr = lpcsLocalVars->lptkCSNxt[-1];

    // Get and save the character index position
    uLinePos = lpcsLocalVars->tkCSErr.tkCharIndex;
    uLineNum = lpcsLocalVars->tkCSErr.tkData.uLineNum;

    // If the caller wants error messages displayed, ...
    if (lpcsLocalVars->DisplayErr)
    {
        // Check for SYNTAX ERROR
#define ERR     "syntax error"
        lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
        {
            wsprintf (szTemp,
                      "SYNTAX ERROR in line %d, position %d -- function NOT saved.",
                      uLineNum,
                      uLinePos);
            p = szTemp;

            goto DISPLAYCAT;
#undef  ERR
        } // End IF

        // Check for VALUE ERROR
#define ERR     "value error"
        lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
        {
            wsprintf (szTemp,
                      "VALUE ERROR in line %d, position %d -- function NOT saved.",
                      uLineNum,
                      uLinePos);
            p = szTemp;

            goto DISPLAYCAT;
#undef  ERR
        } // End IF

        // Check for LENGTH ERROR
#define ERR     "length error"
        lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
        {
            wsprintf (szTemp,
                      "LENGTH ERROR in line %d, position %d -- function NOT saved.",
                      uLineNum,
                      uLinePos);
            p = szTemp;

            goto DISPLAYCAT;
#undef  ERR
        } // End IF
    } else
        return;

#define ERR     "memory exhausted"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        wsprintf (szTemp,
                  "Insufficient memory -- function NOT saved.");
        p = szTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

    // Use the error message as given
    p = s;

    goto DISPLAY;

DISPLAYCAT:
#ifdef DEBUG
    lstrcat (szTemp, "(");
    lstrcat (szTemp,  s );
    lstrcat (szTemp, ")");
#endif
DISPLAY:
    // Display a message box
    MessageBox (lpcsLocalVars->hWndEC,
                p,
                lpszAppName,
                MB_OK | MB_ICONWARNING | MB_APPLMODAL);
} // End cs_yyerror
#undef  APPEND_NAME


//***************************************************************************
//  $cs_yyfprintf
//
//  Debugger output
//***************************************************************************

void cs_yyfprintf
    (FILE  *hfile,          // Ignore this
     LPCHAR lpszFmt,        // Format string
     ...)                   // Zero or more arguments

{
#if (defined (DEBUG)) && (defined (YYFPRINTF_DEBUG))
    va_list vl;
    int     i1,
            i2,
            i3;
    static char szTemp[256] = {'\0'};

    va_start (vl, lpszFmt);

    // Bison uses no more than three arguments.
    // Note we must grab them separately this way
    //   as using va_arg in the argument list to
    //   wsprintf pushes the arguments in reverse
    //   order.
    i1 = va_arg (vl, int);
    i2 = va_arg (vl, int);
    i3 = va_arg (vl, int);

    va_end (vl);

    // Accumulate into local buffer because
    //   Bison calls this function multiple
    //   times for the same line, terminating
    //   the last call for the line with a LF.
    wsprintf (&szTemp[lstrlen (szTemp)],
              lpszFmt,
              i1, i2, i3);
    // Check last character.
    i1 = lstrlen (szTemp);

    // If it's a LF, it's time to flush the buffer.
    if (szTemp[i1 - 1] EQ '\n')
    {
        szTemp[i1 - 1] = '\0';  // Remove trailing LF
                                //   because we're displaying
                                //   in a GUI.
        DbgMsg (szTemp);        // Display in my debugger window.

        szTemp[0] = '\0';       // Restart the buffer
    } // End IF/ELSE
#endif
} // End cs_yyfprintf


//***************************************************************************
//  End of File: cs_parse.y
//***************************************************************************
