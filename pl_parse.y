//***************************************************************************
//  NARS2000 -- Parser Grammar for executable lines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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

/****************************************************************************

Parse a line of pre-tokenized tokens.

Based upon "The Syntax of APL:  An Old Approach Revisited" by
Jean Jacques Giardot & Florence Rollin, ACM SIGAPL Quote-Quad APL 1987
heavily modified to work as an LALR grammar with the lookahead embedded
in the lexical analyser (pl_yylex).

****************************************************************************/

%{
#define STRICT
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "headers.h"


#define   DIRECT    FALSE           // Flags for PushFcnStrand_YY
#define INDIRECT    TRUE            // ...

////#define YYLEX_DEBUG
////#define YYFPRINTF_DEBUG

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               pl_yyfprintf
#define fprintf                 pl_yyfprintf
////#define YYPRINT                 pl_yyprint
void pl_yyprint     (FILE *yyoutput, unsigned short int yytoknum, PL_YYSTYPE const yyvaluep);
#endif

#define YYMALLOC    malloc
#define YYFREE      free

int  pl_yylex       (LPPL_YYSTYPE lpYYLval, LPPLLOCALVARS lpplLocalVars);
void pl_yyerror     (LPPLLOCALVARS lpplLocalVars, LPCHAR s);
void pl_yyfprintf   (FILE  *hfile, LPCHAR lpszFmt, ...);

#define    YYSTYPE     PL_YYSTYPE
#define  LPYYSTYPE   LPPL_YYSTYPE
#define tagYYSTYPE  tagPL_YYSTYPE

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         pl_yy_symbol_print
#define yy_symbol_value_print   pl_yy_symbol_value_print
#define yy_reduce_print         pl_yy_reduce_print
#define yydestruct              pl_yydestruct

#define YYERROR2        {lpplLocalVars->bYYERROR = TRUE; YYERROR;}
#define YYERROR3        {lpplLocalVars->ExitType = EXITTYPE_ERROR; YYERROR2;}

////#define DbgMsgWP(a)         DbgMsgW2 (lpplLocalVars->bLookAhead ? L"==" a : a)
////#define DbgMsgWP(a)         DbgMsgW  (lpplLocalVars->bLookAhead ? L"==" a : a); DbgBrk ()
////#define DbgMsgWP(a)         DbgMsgW  (lpplLocalVars->bLookAhead ? L"==" a : a)
    #define DbgMsgWP(a)

#ifdef DEBUG
#define APPEND_NAME     L" -- pl_yyparse"
#else
#define APPEND_NAME
#endif

%}

%pure-parser
%name-prefix="pl_yy"
%parse-param {LPPLLOCALVARS lpplLocalVars}
%lex-param   {LPPLLOCALVARS lpplLocalVars}

%token NAMEVAR NAMEUNK CONSTANT CHRSTRAND NUMSTRAND USRFN0 SYSFN0 QUAD QUOTEQUAD SYSLBL
%token LBRACE RBRACE
%token UNK EOL
%token CS_ANDIF
%token CS_CASE
%token CS_CASELIST
%token CS_CONTINUE
%token CS_ELSE
%token CS_ELSEIF
%token CS_ENDFOR
%token CS_ENDFORLCL
%token CS_ENDREPEAT
%token CS_ENDWHILE
%token CS_FOR
%token CS_FORLCL
%token CS_IF
%token CS_IN
%token CS_LEAVE
%token CS_ORIF
%token CS_SELECT
%token CS_SKIPCASE
%token CS_SKIPEND
%token CS_UNTIL
%token CS_WHILE
%token FILLJOT

/*  Note that as we parse APL from right to left, these rules
    are all reversed as well as token associativity (long scope).
    That is, functions in APL have long right scope, so they are
    declared here in the %left list.  Similarly, operators in APL
    have long left scope, so they are declared in the %right list.
 */
%right DIAMOND
%left  ASSIGN PRIMFCN NAMEFCN NAMETRN SYSFN12 GOTO SYSNS
%right NULLOP NAMEOP1 OP1 NAMEOP2 OP2 NAMEOP3 OP3 JOTDOT OP3ASSIGN NAMEOP3ASSIGN
%left  OP1CONSTANT OP1NAMEVAR OP1CHRSTRAND OP1NUMSTRAND
%right OP2CONSTANT OP2NAMEVAR OP2CHRSTRAND OP2NUMSTRAND

%start StmtMult

%%

/*  ToDo
    ----
    * J/Dyalog's dynamic functions
    * Allow f{is}MonOp MonOp
    * ...   f{is}      DydOp FcnOrVar

    Notes
    -----
    * A monadic operator with a var left arg must be
      enclosed in parens, such as (2 op1) because
      1 2 op1 can be interpreted as 1 (2 op1) as well
      as (1 2) op1.
    * A dyadic operator with either argument as a var
      must be enclosed in parens for the same reasons
      as above, otherwise 1 2 op2 3 4 is way too ambiguous.
 */

// One or more statements
StmtMult:
      // All single stmt errors propagate up to this point where we ABORT -- this ensures
      //   that the call to pl_yyparse terminates wth a non-zero error code.
                       error            {DbgMsgWP (L"%%StmtMult:  error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             Assert (YYResIsEmpty ());

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // If we're resetting via {goto}, ...
                                             if (lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_ONE
                                              || lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_ONE_INIT)
                                             {
                                                // Mark as not resetting
                                                 lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                                                 // Mark as a SYNTAX ERROR
                                                 if (!lpplLocalVars->bYYERROR)
                                                     ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR);
                                             } // End IF

                                             // If we're resetting, ...
                                             if (lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
                                                 lpplLocalVars->ExitType = TranslateResetFlagToExitType (lpMemPTD->lpSISCur->ResetFlag);

                                             // Set the exit type to error unless already set
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_NONE)
                                                 lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYABORT;
                                         } else
                                             YYABORT;
                                        }
    |                  StmtSing         {DbgMsgWP (L"%%StmtMult:  StmtSing");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             Assert (YYResIsEmpty ());
                                         } // End IF
                                        }
    | StmtMult DIAMOND StmtSing         {DbgMsgWP (L"%%StmtMult:  StmtSing " WS_UTF16_DIAMOND L" StmtMult");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             Assert (YYResIsEmpty ());
                                         } // End IF
                                        }
      // All multiple stmt errors propagate up to this point where we ABORT -- this ensures
      //   that the call to pl_yyparse terminates wth a non-zero error code.
    | StmtMult DIAMOND error            {DbgMsgWP (L"%%StmtMult:  error" WS_UTF16_DIAMOND L" StmtMult");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             Assert (YYResIsEmpty ());

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // If we're resetting via {goto}, ...
                                             if (lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_ONE
                                              || lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_ONE_INIT)
                                             {
                                                // Mark as not resetting
                                                 lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                                                 // Mark as a SYNTAX ERROR
                                                 if (!lpplLocalVars->bYYERROR)
                                                     ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR);
                                             } // End IF

                                             // If we're resetting, ...
                                             if (lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
                                                 lpplLocalVars->ExitType = TranslateResetFlagToExitType (lpMemPTD->lpSISCur->ResetFlag);

                                             // Set the exit type to error unless already set
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_NONE)
                                                 lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYABORT;
                                         } else
                                             YYABORT;
                                        }
    ;

// Single statement
StmtSing:
      /* Empty */                       {DbgMsgWP (L"%%StmtSing:  <empty>");
                                         if (!lpplLocalVars->bLookAhead)
                                             lpplLocalVars->ExitType = EXITTYPE_NOVALUE;
                                        }
    | error   CS_ANDIF                  {DbgMsgWP (L"%%StmtSing:  CS_ANDIF error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr CS_ANDIF                  {DbgMsgWP (L"%%StmtSing:  CS_ANDIF ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ANDIF statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_CASE                   {DbgMsgWP (L"%%StmtSing:  CS_CASE error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr CS_CASE                   {DbgMsgWP (L"%%StmtSing:  CS_CASE ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle CASE/CASELIST statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_CASE_Stmt (lpplLocalVars, &$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_CASELIST               {DbgMsgWP (L"%%StmtSing:  CS_CASELIST error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr CS_CASELIST               {DbgMsgWP (L"%%StmtSing:  CS_CASELIST ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle CASE/CASELIST statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_CASE_Stmt (lpplLocalVars, &$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_CONTINUE                       {DbgMsgWP (L"%%StmtSing:  CS_CONTINUE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle CONTINUE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_CONTINUE_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_ENDREPEAT                      {DbgMsgWP (L"%%StmtSing:  CS_ENDREPEAT");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ENDREPEAT statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ENDREPEAT_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_ENDWHILE                       {DbgMsgWP (L"%%StmtSing:  CS_ENDWHILE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ENDWHILE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ENDWHILE_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_ELSE                           {DbgMsgWP (L"%%StmtSing:  CS_ELSE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ELSE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ELSE_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_ELSEIF                 {DbgMsgWP (L"%%StmtSing:  CS_ELSEIF error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr CS_ELSEIF                 {DbgMsgWP (L"%%StmtSing:  CS_ELSEIF ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ELSEIF statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_ENDFOR                         {DbgMsgWP (L"%%StmtSing:  CS_ENDFOR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ENDFOR statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ENDFOR_Stmt (lpplLocalVars, &$1, FALSE);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_ENDFORLCL                      {DbgMsgWP (L"%%StmtSing:  CS_ENDFORLCL");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ENDFORLCL statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_ENDFOR_Stmt (lpplLocalVars, &$1, TRUE);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | ArrExpr CS_IN NameAnyVar CS_FOR   {DbgMsgWP (L"%%StmtSing:  CS_FOR NameAnyVar CS_IN ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle FOR statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_FOR_Stmt_EM (lpplLocalVars, &$4, &$3, &$2, &$1, FALSE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | ArrExpr CS_IN NameAnyVar CS_FORLCL{DbgMsgWP (L"%%StmtSing:  CS_FORLCL NameAnyVar CS_IN ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle FORLCL statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_FOR_Stmt_EM (lpplLocalVars, &$4, &$3, &$2, &$1, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_IF                     {DbgMsgWP (L"%%StmtSing:  CS_IF error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr CS_IF                     {DbgMsgWP (L"%%StmtSing:  CS_IF ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle IF statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_LEAVE                          {DbgMsgWP (L"%%StmtSing:  CS_LEAVE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle LEAVE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_LEAVE_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_ORIF                   {DbgMsgWP (L"%%StmtSing:  CS_ORIF error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr CS_ORIF                   {DbgMsgWP (L"%%StmtSing:  CS_ORIF ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle ORIF statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_SELECT                 {DbgMsgWP (L"%%StmtSing:  CS_SELECT error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr CS_SELECT                 {DbgMsgWP (L"%%StmtSing:  CS_SELECT ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle SELECT statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_SELECT_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_SKIPCASE                       {DbgMsgWP (L"%%StmtSing:  CS_SKIPCASE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle SKIPCASE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_SKIPCASE_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | CS_SKIPEND                        {DbgMsgWP (L"%%StmtSing:  CS_SKIPEND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle SKIPEND statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_SKIPEND_Stmt (lpplLocalVars, &$1);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_UNTIL                  {DbgMsgWP (L"%%StmtSing:  CS_UNTIL error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr CS_UNTIL                  {DbgMsgWP (L"%%StmtSing:  CS_UNTIL ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle UNTIL statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | error   CS_WHILE                  {DbgMsgWP (L"%%StmtSing:  CS_WHILE error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr CS_WHILE                  {DbgMsgWP (L"%%StmtSing:  CS_WHILE ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Handle WHILE statement
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   CS_IF_Stmt_EM (lpplLocalVars, &$2, &$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3
                                             else
                                             if (lpplLocalVars->bStopExec)
                                                 YYACCEPT;          // Stop executing this line

                                             // No return value needed
                                         } // End IF
                                        }
    | LeftOper                          {DbgMsgWP (L"%%StmtSing:  LeftOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 // Mark as a SYNTAX ERROR
                                                 PrimFnSyntaxError_EM (&$1.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| error                             //--Conflicts
    | ArrExpr                           {DbgMsgWP (L"%%StmtSing:  ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
                                             LPSIS_HEADER lpSISCur;          // Ptr to current SIS header

                                             if ((CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR) && !$1.tkToken.tkFlags.NoDisplay)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                             {
                                                 // Get ptr to PerTabData global memory
                                                 lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                                 // Get the ptr to the current SIS header
                                                 lpSISCur = lpMemPTD->lpSISCur;

                                                 // Do not display if caller is Execute or Quad
                                                 //   and the current statement is the last one on the line
                                                 if (IsLastStmt (lpplLocalVars, $1.tkToken.tkCharIndex)
                                                  && (lpSISCur->DfnType EQ DFNTYPE_EXEC
                                                   || (lpSISCur->DfnType EQ DFNTYPE_IMM
                                                    && lpSISCur->lpSISPrv NE NULL
                                                    && (lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_EXEC
                                                     || lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_QUAD))))
                                                     // Handle ArrExpr if caller is Execute or quad
                                                     ArrExprCheckCaller (lpplLocalVars, lpSISCur, &$1);
                                                 else
                                                     lpplLocalVars->bRet =
                                                       ArrayDisplay_EM (&$1.tkToken, TRUE, &lpplLocalVars->bCtrlBreak);
                                             } // End IF/ELSE

                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR3

                                             // If we're resetting all levels, ...
                                             if (lpSISCur->ResetFlag EQ RESETFLAG_ALL)
                                                 lpplLocalVars->ExitType = EXITTYPE_RESET_ALL;
                                             else
                                             // If the exit type isn't GOTO_LINE, mark it as already displayed
                                             if (lpplLocalVars->ExitType NE EXITTYPE_GOTO_LINE)
                                                 lpplLocalVars->ExitType = EXITTYPE_NODISPLAY;
                                         } // End IF
                                        }
    | error   GOTO                      {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_RIGHTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr GOTO                      {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_RIGHTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->ExitType = GotoLine_EM (&$1.tkToken, &$2.tkToken);
                                             FreeResult (&$1.tkToken);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // Make a PL_YYSTYPE NoValue entry
                                             lpplLocalVars->lpYYMak =
                                               MakeNoValue_YY (&$2.tkToken);
                                             lpMemPTD->YYResExec = *lpplLocalVars->lpYYMak;
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             // Split cases based upon the Exit Type
                                             switch (lpplLocalVars->ExitType)
                                             {
                                                 case EXITTYPE_GOTO_ZILDE:  // {zilde}
                                                     break;

                                                 case EXITTYPE_GOTO_LINE:   // Valid line #
                                                     // If we're not at a LABELSEP, EOS or EOL, YYERROR
                                                     if (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_LABELSEP
                                                      && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOS
                                                      && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOL)
                                                     {
                                                         PrimFnSyntaxError_EM (&$2.tkToken);
                                                         YYERROR3
                                                     } else
                                                         YYACCEPT;          // Stop executing this line

                                                 case EXITTYPE_ERROR:       // Error
                                                     YYERROR3               // Stop on error

                                                 defstop
                                                     break;
                                             } // End SWITCH
                                         } // End IF
                                        }
    |         GOTO                      {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_RIGHTARROW);
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
                                             LPSIS_HEADER lpSISCur;          // Ptr to current SIS layer

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3

                                             // If we're not at the EOS or EOL, YYERROR
                                             if (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOL
                                              && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOS)
                                             {
                                                 if (!lpplLocalVars->bYYERROR)
                                                     PrimFnSyntaxError_EM (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // Get a ptr to the current SIS layer
                                             lpSISCur = lpMemPTD->lpSISCur;

                                             // Peel back to the first non-Exec layer
                                             while (lpSISCur->DfnType EQ DFNTYPE_EXEC)
                                                lpSISCur = lpSISCur->lpSISPrv;

                                             // Set the reset flag to ONE if user-defined function/operator
                                             //   as we'll stop at the next ImmExec
                                             if (lpSISCur->DfnType EQ DFNTYPE_OP1
                                              || lpSISCur->DfnType EQ DFNTYPE_OP2
                                              || lpSISCur->DfnType EQ DFNTYPE_FCN)
                                             {
                                                 lpSISCur->ResetFlag = RESETFLAG_ONE;
                                                 lpplLocalVars->ExitType = EXITTYPE_RESET_ONE;
                                             } else
                                             {
                                                 lpSISCur->ResetFlag = RESETFLAG_ONE_INIT;
                                                 lpplLocalVars->ExitType = EXITTYPE_RESET_ONE_INIT;
                                             } // End IF/ELSE

                                             YYACCEPT;              // Stop executing this line
                                         } // End IF
                                        }
    | error   ASSIGN                    {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN                    {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
                                             LPSIS_HEADER lpSISCur;          // Ptr to current SIS header

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // Get the ptr to the current SIS header
                                             lpSISCur = lpMemPTD->lpSISCur;

                                             // Do not free if caller is Execute or Quad
                                             //   and the current statement is the last one on the line
                                             if (IsLastStmt (lpplLocalVars, $1.tkToken.tkCharIndex)
                                              && (lpSISCur->DfnType EQ DFNTYPE_EXEC
                                               || (lpSISCur->DfnType EQ DFNTYPE_IMM
                                                && lpSISCur->lpSISPrv NE NULL
                                                && (lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_EXEC
                                                 || lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_QUAD))))
                                             {
                                                 // Handle ArrExpr if caller is Execute or quad
                                                 ArrExprCheckCaller (lpplLocalVars, lpSISCur, &$1);

                                                 // Mark as already displayed
                                                 $1.tkToken.tkFlags.NoDisplay = TRUE;

                                                 // Pass on as result
                                                 $$ = $1;
                                             } else
                                                 FreeResult (&$1.tkToken);

                                             lpplLocalVars->ExitType = EXITTYPE_NODISPLAY;
                                         } // End IF/ELSE/IF
                                        }
////     | ArrExpr NAMEOP3                   {DbgMsgWP (L"%%StmtSing:  NAMEOP3 ArrExpr");
////                                              DbgBrk ();
////
////                                              lpplLocalVars->lpYYMak =
////                                                MakeNameFcnOpr_YY (&$2, TRUE);
////
////                                              if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
////                                              {
////                                                  FreeResult (&$1.tkToken);
////                                                  YYERROR3
////                                              } // End IF
////
////                                              // Change the first token in the function strand
////                                              //   from ambiguous operator to a function
////                                              AmbOpToFcn (lpplLocalVars->lpYYMak);
////
////                                              lpplLocalVars->lpYYOp3 =
////                                                PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
////                                              YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
////
////                                              if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
////                                              {
////                                                  FreeResult (&$1.tkToken);
////                                                  YYERROR3
////                                              } // End IF
////
////                                              lpplLocalVars->lpYYFcn =
////                                                MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp3, NAMETYPE_FN12, FALSE);
////
////                                              if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
////                                              {
////                                                  FreeResult (&$1.tkToken);
////                                                  YYERROR3
////                                              } // End IF
////
////                                              lpplLocalVars->lpYYRes =
////                                                ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
////                                              FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
////
////                                              if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
////                                                  YYERROR3
////
////                                              $$ = *lpplLocalVars->lpYYRes;
////                                              YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
////                                              YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
////                                         }
    | FcnSpec                           {DbgMsgWP (L"%%StmtSing:  FcnSpec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Free the result as the arg to the name was passed on
                                             FreeResult (&$1.tkToken);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;

                                             $$ = $1;
                                         } // End IF
                                        }
    | Op1Spec                           {DbgMsgWP (L"%%StmtSing:  Op1Spec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Free the result as the arg to the name was passed on
                                             FreeResult (&$1.tkToken);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;

                                             $$ = $1;
                                         } // End IF
                                        }
    | Op2Spec                           {DbgMsgWP (L"%%StmtSing:  Op2Spec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Free the result as the arg to the name was passed on
                                             FreeResult (&$1.tkToken);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;

                                             $$ = $1;
                                         } // End IF
                                        }
    | Op3Spec                           {DbgMsgWP (L"%%StmtSing:  Op3Spec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Free the result as the arg to the name was passed on
                                             FreeResult (&$1.tkToken);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;

                                             $$ = $1;
                                         } // End IF
                                        }
// The following productions ending in EOL are for bLookAhead only
    |     error   EOL                   {DbgMsgWP (L"%%StmtSing:  EOL error");
                                         if (lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
////|     IndexListWE EOL               {DbgMsgWP (L"%%StmtSing:  EOL IndexListWE");
////                                     if (lpplLocalVars->bLookAhead)
////                                     {
////                                         lpplLocalVars->plNameType = NAMETYPE_LST;
////                                         YYACCEPT;              // Stop executing this line
////                                     } else
////                                         YYERROR2
////                                    }
    |     ArrExpr EOL                   {DbgMsgWP (L"%%StmtSing:  EOL ArrExpr");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_VAR;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     FcnSpec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL FcnSpec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Op1Spec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL Op1Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP1;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Op1Spec LeftOper EOL          {DbgMsgWP (L"%%StmtSing:  EOL LeftOper Op1Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Op2Spec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL Op2Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP2;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Op3Spec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL Op3Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP3;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     MonOp   EOL                   {DbgMsgWP (L"%%StmtSing:  EOL MonOp");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP1;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     DydOp   EOL                   {DbgMsgWP (L"%%StmtSing:  EOL DydOp");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP2;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     AmbOpX  EOL                   {DbgMsgWP (L"%%StmtSing:  EOL AmbOpX");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP3;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     LeftOper EOL                  {DbgMsgWP (L"%%StmtSing:  EOL LeftOper");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     AxisFunc EOL                  {DbgMsgWP (L"%%StmtSing:  EOL AxisFunc");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Drv1Func EOL                  {DbgMsgWP (L"%%StmtSing:  EOL Drv1Func");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Drv2Func EOL                  {DbgMsgWP (L"%%StmtSing:  EOL Drv2Func");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Drv4Func EOL                  {DbgMsgWP (L"%%StmtSing:  EOL Drv4Func");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Drv5Func EOL                  {DbgMsgWP (L"%%StmtSing:  EOL Drv5Func");    // Includes Drv3Func
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    |     Train EOL                     {DbgMsgWP (L"%%StmtSing:  EOL Train");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_TRN;
                                             YYACCEPT;              // Stop executing this line
                                         } else
                                             YYERROR2
                                        }
    ;

NameAnyVar:
      NAMEUNK                           {DbgMsgWP (L"%%NameAnyVar:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgWP (L"%%NameAnyVar:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

NameAnyOpN:
      NAMEUNK                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEFCN                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMETRN                           {DbgMsgWP (L"%%NameAnyOpN:  NAMETRN");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP1                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEOP1");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP2                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEOP2");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP3                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEOP3");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

// Function specification
FcnSpec:
////  error    ASSIGN NameAnyOpN        //--Conflicts
      LeftOper ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"LeftOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| error    ASSIGN NameAnyOpN        //--Conflicts
    | Drv1Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv1Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| error    ASSIGN NameAnyOpN        //--Conflicts
    | Drv2Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv2Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | Drv3Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv3Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | Drv4Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv4Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| error    ASSIGN NameAnyOpN        //--Conflicts
    | AxisFunc ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"AxisFunc");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | MonOp Drv1Func ASSIGN NameAnyOpN  {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv1Func MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | MonOp Drv3Func ASSIGN NameAnyOpN  {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv3Func MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv1Func ASSIGN NameAnyOpN {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv1Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv2Func ASSIGN NameAnyOpN {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv2Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv3Func ASSIGN NameAnyOpN {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv3Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv4Func ASSIGN NameAnyOpN {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv4Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp1, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

Op1Spec:
      MonOp    ASSIGN NameAnyOpN        {DbgMsgWP (L"%%Op1Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP1, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
/////////////////////////////////////////////////FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = $1;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
// The following assignment can be made to work if we switch the order of the arguments
//   to a dyadic operator in a function strand from D L R to D R L and make all corresponding
//   changes elsewhere such as whenever parsing a function strand.
////| RightOper DydOp ASSIGN NameAnyOpN {DbgMsgWP (L"%%Op1Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"DydOp RightOper");
////                                     // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
////                                     if (!lpplLocalVars->bLookAhead)
////                                     {
////                                         lpplLocalVars->lpYYOp2 =
////                                           PushFcnStrand_YY (&$2, 2, INDIRECT);  // Dyadic operator (Indirect)
////
////                                         lpplLocalVars->lpYYRht =
////                                           PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)
////
////                                         if (!lpplLocalVars->lpYYOp2             // If not defined, free args and YYERROR
////                                          || !lpplLocalVars->lpYYRht)
////                                         {
////                                             if (lpplLocalVars->lpYYOp2)         // If defined, free it
////                                             {
////                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
////                                             } // End IF
////
////                                             if (lpplLocalVars->lpYYRht)         // If defined, free it
////                                             {
////                                                 FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
////                                             } // End IF
////
//// ////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
////                                             YYERROR3
////                                         } // End IF
////
////                                         lpplLocalVars->lpYYRes =
////                                           MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp2, NAMETYPE_OP1, TRUE);
////
////                                         if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
////                                         {
////                                             FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
////                                             FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
//// ////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
////                                             YYERROR3
////                                         } // End IF
////
////                                         if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
////                                             lpplLocalVars->bRet = FALSE;
////                                         else
////                                             lpplLocalVars->bRet =
////                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
//// ////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;     // DO NOT FREE:  Passed on as result
//// ////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
////
////                                         if (!lpplLocalVars->bRet)
////                                             YYERROR3
////
////                                         // The result is always the root of the function tree
////                                         $$ = *lpplLocalVars->YYRes;
////
////                                         // Mark as already displayed
////                                         $$.tkToken.tkFlags.NoDisplay = TRUE;
////
////                                         YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
////                                         YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
////                                         YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
////                                     } // End IF
////                                    }
    ;

// Dyadic operator specification
Op2Spec:
      DydOp    ASSIGN NameAnyOpN        {DbgMsgWP (L"%%Op2Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"DydOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP2, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
/////////////////////////////////////////////////FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    ;


// Ambiguous operator assignment
AmbOpAssign:
      OP3ASSIGN                         {DbgMsgWP (L"%%AmbOpAssign:  OP3ASSIGN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    | NAMEOP3ASSIGN                     {DbgMsgWP (L"%%AmbOpAssign:  NAMEOP3ASSIGN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    | AmbOpX                            {DbgMsgWP (L"%%AmbOpAssign:  AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | AmbOpAssign OP3ASSIGN             {DbgMsgWP (L"%%AmbOpAssign:  OP3ASSIGN AmbOpAssign");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Ambiguous operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    | AmbOpAssign NAMEOP3ASSIGN         {DbgMsgWP (L"%%AmbOpAssign:  NAMEOP3ASSIGN AmbOpAssign");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$2, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Ambiguous operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    ;


// Ambiguous operator specification
Op3Spec:
      AmbOpAssign   ASSIGN NameAnyOpN   {DbgMsgWP (L"%%Op3Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"AmbOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP3, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
/////////////////////////////////////////////////FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
/////////////////////////////////////////////FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;     // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = $1;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    ;

// Array expression
ArrExpr:
//////error                             //--Conflicts
      SimpExpr                          {DbgMsgWP (L"%%ArrExpr:  SimpExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | StrandInst                        {DbgMsgWP (L"%%ArrExpr:  StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | error   LeftMonOper               {DbgMsgWP (L"%%ArrExpr:  LeftMonOper error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // If this is not a named function/operator, ...
                                             if (!IsTknTypeNamedFcnOpr ($2.tkToken.tkFlags.TknType))
                                                 FreeResult (&$2.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrExpr error                     //--Conflicts
    | StrandOp2 LeftMonOper             {DbgMsgWP (L"%%ArrExpr:  LeftMonOper StrandOp2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Increment the RefCnt because StrandOp2 doesn't
                                             //   go through MakeFcnStrand_EM_YY as this is a
                                             //   variable not an operand.
                                             DbgIncrRefCntTkn (&$1.tkToken);

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF/ELSE/...
                                        }
    | ArrExpr LeftMonOper               {DbgMsgWP (L"%%ArrExpr:  LeftMonOper ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 if (lpplLocalVars->ExitType EQ EXITTYPE_RETURNxLX)
                                                     YYABORT;
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF/ELSE/...
                                        }
    | ArrExpr LeftOper                  {DbgMsgWP (L"%%ArrExpr:  LeftOper ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 if (lpplLocalVars->ExitType EQ EXITTYPE_RETURNxLX)
                                                     YYABORT;
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | error   AxisFunc                  {DbgMsgWP (L"%%ArrExpr:  AxisFunc error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrExpr error                     //--Conflicts
    | ArrExpr AxisFunc                  {DbgMsgWP (L"%%ArrExpr:  AxisFunc ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr LeftOper error            {DbgMsgWP (L"%%ArrExpr:  error LeftOper ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             // If this is not a named function/operator, ...
                                             if (!IsTknTypeNamedFcnOpr ($2.tkToken.tkFlags.TknType))
                                                 FreeResult (&$2.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   LeftOper StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst LeftOper error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // If this is not a named function/operator, ...
                                             if (!IsTknTypeNamedFcnOpr ($2.tkToken.tkFlags.TknType))
                                                 FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrExpr LeftOper error            //--Conflicts
////| ArrExpr error    StrandInst       //--Conflicts
    | ArrExpr LeftOper StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst LeftOper ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr AmbOpX   error            {DbgMsgWP (L"%%ArrExpr:  error AmbOpX ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   AmbOpX StrandInst         {DbgMsgWP (L"%%ArrExpr:  StrandInst AmbOpX error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   AmbOpX StrandOp1          {DbgMsgWP (L"%%ArrExpr:  StrandOp1 AmbOpX error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);   // DO NOT FREE:  RefCnt not incremented in StrandOp1
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr AmbOpX NAMEOP3ASSIGN      {DbgMsgWP (L"%%ArrExpr:  OP3ASSIGN AmbOpX ArrExpr");
                                         DbgBrk ();     // ***FIXME*** -- Can we ever get here??



                                        }
    | error   AmbOpX NAMEOP3ASSIGN      {DbgMsgWP (L"%%ArrExpr:  OP3ASSIGN AmbOpX error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr AmbOpX OP3ASSIGN          {DbgMsgWP (L"%%ArrExpr:  OP3ASSIGN AmbOpX ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$3);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (lpplLocalVars->lpYYMak);

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$2, 2, INDIRECT);  // Ambiguous operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }

    | ArrExpr AmbOpX AmbOpX StrandOp1   {DbgMsgWP (L"%%ArrExpr:  StrandOp1 AmbOpX AmbOpX ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Increment the RefCnt because StrandOp1 doesn't
                                             //   go through MakeFcnStrand_EM_YY as this is a
                                             //   function not an operator.
                                             DbgIncrRefCntTkn (&$4.tkToken);

                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (&$3);

                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$2);

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$2, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr AmbOpX StrandOp1          {DbgMsgWP (L"%%ArrExpr:  StrandOp1 AmbOpX ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Increment the RefCnt because StrandOp1 doesn't
                                             //   go through MakeFcnStrand_EM_YY as this is a
                                             //   function not an operator.
                                             DbgIncrRefCntTkn (&$3.tkToken);

                                             // Change the first token from ambiguous operator to a function
                                             //   and swap the first two tokens
                                             if (!AmbOpSwap_EM (&$2))
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Because MakeFcnStrand might have brought in a function strand from
                                             //   a global which still has TKT_OP3IMMED, change the token type to
                                             //   TKT_FCNIMMED.
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (lpplLocalVars->lpYYFcn);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr AmbOpX StrandInst         {DbgMsgWP (L"%%ArrExpr:  StrandInst AmbOpX ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token from ambiguous operator to a function
                                             //   and swap the first two tokens
                                             if (!AmbOpSwap_EM (&$2))
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Because MakeFcnStrand might have brought in a function strand from
                                             //   a global which still has TKT_OP3IMMED, change the token type to
                                             //   TKT_FCNIMMED.
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (lpplLocalVars->lpYYFcn);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | error   Drv5Func    StrandInst    {DbgMsgWP (L"%%ArrExpr:  StrandInst Drv5Func error");    // Also Drv3Func
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr Drv5Func    error         {DbgMsgWP (L"%%ArrExpr:  error Drv5Func ArrExpr");       // Also Drv3Func
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr Drv5Func    StrandOp1     {DbgMsgWP (L"%%ArrExpr:  StrandOp1 Drv5Func ArrExpr");  // Also Drv3Func
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Increment the RefCnt because StrandOp1 doesn't
                                             //   go through MakeFcnStrand_EM_YY as this is a
                                             //   function not an operator.
                                             DbgIncrRefCntTkn (&$3.tkToken);

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr Drv5Func    StrandInst    {DbgMsgWP (L"%%ArrExpr:  StrandInst Drv5Func ArrExpr");  // Also Drv3Func
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr AxisFunc error            {DbgMsgWP (L"%%ArrExpr:  error AxisFunc ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   AxisFunc StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst AxisFunc error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrExpr AxisFunc error            //--Conflicts
////| ArrExpr error    StrandInst       //--Conflicts
    | ArrExpr AxisFunc StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst AxisFunc ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Strand instance to the left of a monadic or dyadic op
StrandOp1:
          OP1CONSTANT                   {DbgMsgWP (L"%%StrandOp1:  OP1CONSTANT");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     OP1NAMEVAR                    {DbgMsgWP (L"%%StransOp1:  OP1NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     OP1CHRSTRAND                  {DbgMsgWP (L"%%StrandOp1:  OP1CHRSTRAND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     OP1NUMSTRAND                  {DbgMsgWP (L"%%StrandOp1:  OP1NUMSTRAND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

// Strand instance to the right of a dyadic op
StrandOp2:
          OP2CONSTANT                   {DbgMsgWP (L"%%StrandOp2:  OP2CONSTANT");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     OP2NAMEVAR                    {DbgMsgWP (L"%%StrandOp2:  OP2NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     OP2CHRSTRAND                  {DbgMsgWP (L"%%StrandOp2:  OP2CHRSTRAND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     OP2NUMSTRAND                  {DbgMsgWP (L"%%StrandOp2:  OP2NUMSTRAND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

// Single var (including single names)
SingVar:
          NAMEUNK                       {DbgMsgWP (L"%%SingVar:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$1.tkToken);
                                             YYERROR3
                                         } // End IF
                                        }
    |     QUAD                          {DbgMsgWP (L"%%SingVar:  QUAD");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
                                             RESET_FLAGS  resetFlag;             // The current Reset Flag

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3

                                             lpplLocalVars->lpYYRes =
                                               WaitForInput (lpplLocalVars->hWndSM, FALSE, &$1.tkToken);
                                             FreeResult (&$1.tkToken);

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // Get the Reset Flag
                                             resetFlag = lpMemPTD->lpSISCur->ResetFlag;

                                             // If we're resetting, ...
                                             if (resetFlag NE RESETFLAG_NONE)
                                                 lpplLocalVars->ExitType = TranslateResetFlagToExitType (resetFlag);

                                             // If we're resetting, ...
                                             if (resetFlag NE RESETFLAG_NONE)
                                                 YYACCEPT;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     QUOTEQUAD                     {DbgMsgWP (L"%%SingVar:  QUOTEQUAD");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3

                                             lpplLocalVars->lpYYRes =
                                               WaitForInput (lpplLocalVars->hWndSM, TRUE, &$1.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     CONSTANT                      {DbgMsgWP (L"%%SingVar:  CONSTANT");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |     NAMEVAR                       {DbgMsgWP (L"%%SingVar:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |     SYSLBL                        {DbgMsgWP (L"%%SingVar:  SYSLBL");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |     USRFN0                        {DbgMsgWP (L"%%SingVar:  USRFN0");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecuteFn0 (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeVarStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&$$.tkToken);
                                         } // End IF
                                        }
    |     SYSFN0                        {DbgMsgWP (L"%%SingVar:  SYSFN0");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecuteFn0 (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeVarStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&$$.tkToken);
                                         } // End IF
                                        }
    |     CHRSTRAND                     {DbgMsgWP (L"%%SingVar:  CHRSTRAND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    |     NUMSTRAND                     {DbgMsgWP (L"%%SingVar:  NUMSTRAND");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 YYERROR3
                                             $$ = $1;
                                         } // End IF
                                        }
    | ')' error   '('                   {DbgMsgWP (L"%%SingVar:  error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ')' ArrExpr '('                   {DbgMsgWP (L"%%SingVar:  (ArrExpr)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $$ = $2;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeVarStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&$$.tkToken);
                                         } // End IF
                                        }
    ;

// Index var with brackets
IndexVarBR:
      IndexListBR     error             {DbgMsgWP (L"%%IndexVarBR:  error IndexListBR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | IndexListBR     SingVar           {DbgMsgWP (L"%%IndexVarBR:  SingVar IndexListBR");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             InitVarStrand (&$2);

                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$2);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYStr =
                                               MakeVarStrand_EM_YY (lpplLocalVars->lpYYRes);
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             if (!lpplLocalVars->lpYYStr)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ArrayIndexRef_EM_YY (&lpplLocalVars->lpYYStr->tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Strand Recursion
StrandRec:
////  error                             //--Conflicts
      SingVar                           {DbgMsgWP (L"%%StrandRec:  SingVar");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             InitVarStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexVarBR                        {DbgMsgWP (L"%%StrandRec:  IndexVarBR");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeVarStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&$1.tkToken);

                                             InitVarStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | StrandRec   error                 {DbgMsgWP (L"%%StrandRec:  error StrandRec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStr =
                                               MakeVarStrand_EM_YY (&$1);

                                             if (!lpplLocalVars->lpYYStr)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             FreeYYFcn1 (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;

                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | StrandRec   SingVar               {DbgMsgWP (L"%%StrandRec:  SingVar StrandRec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$2);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | StrandRec IndexVarBR              {DbgMsgWP (L"%%StrandRec:  IndexVarBR StrandRec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$2);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Strand instance
StrandInst:
////  error                             //--Conflicts
      StrandRec                         {DbgMsgWP (L"%%StrandInst:  StrandRec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStr =
                                               MakeVarStrand_EM_YY (&$1);

                                             if (!lpplLocalVars->lpYYStr)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYStr;
                                             YYFree (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;
                                         } // End IF
                                        }
    | CONSTANT   ':'                    {DbgMsgWP (L"%%StrandInst:  :CONSTANT");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             HGLOBAL            hGlbObj;                 // Object global memory handle
                                             LPPERTABDATA       lpMemPTD;                // Ptr to PerTabData global memory
                                             LPLOADWSGLBVARCONV lpLoadWsGlbVarConv;      // Ptr to function to convert a FMTSTR_GLBOBJ to an HGLOBAL
                                             LPLOADWSGLBVARPARM lpLoadWsGlbVarParm;      // Extra parms for LoadWsGlbVarConv

                                             // Get ptr to PerTabData global memory
                                             lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

                                             // Get the LoadWsGlbVarXXX ptrs
                                             lpLoadWsGlbVarConv = lpMemPTD->lpLoadWsGlbVarConv;
                                             lpLoadWsGlbVarParm = lpMemPTD->lpLoadWsGlbVarParm;

                                             // If we're converting, ...
                                             if (lpLoadWsGlbVarConv)
                                             {
                                                 // Convert the constant to an HGLOBAL
                                                 hGlbObj =
                                                   (*lpLoadWsGlbVarConv) ((UINT) $1.tkToken.tkData.tkInteger,
                                                                          lpLoadWsGlbVarParm);
                                                 // Fill in the result token
                                                 $$ = $1;                               // Set common fields
                                                 $$.tkToken.tkFlags.TknType   = TKT_VARARRAY;
                                                 $$.tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
                                                 $$.tkToken.tkFlags.NoDisplay = FALSE;
                                                 $$.tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbObj);
/////////////////////////////////////////////////$$.tkToken.tkCharIndex       =         // Set in $$ = $1 above
                                             } else
                                             {
                                                 // Signal an error
                                                 if (!lpplLocalVars->bYYERROR)
                                                     PrimFnSyntaxError_EM (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF/ELSE
                                         } // End IF
                                        }
    ;

// Simple array expression
SimpExpr:
      error   ASSIGN       QUAD         {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN       QUAD         {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as NOT already displayed
                                             $1.tkToken.tkFlags.NoDisplay = FALSE;
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ArrayDisplay_EM (&$1.tkToken, TRUE, &lpplLocalVars->bCtrlBreak);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN       QUOTEQUAD    {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN       QUOTEQUAD    {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as NOT already displayed
                                             $1.tkToken.tkFlags.NoDisplay = FALSE;
                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ArrayDisplay_EM (&$1.tkToken, FALSE, &lpplLocalVars->bCtrlBreak);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN       NameAnyVar   {DbgMsgWP (L"%%SimpExpr:  NameAnyVar" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN       NameAnyVar   {DbgMsgWP (L"%%SimpExpr:  NameAnyVar" WS_UTF16_LEFTARROW L"ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   AssignName_EM (&$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN IndexListBR  NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR" WS_UTF16_LEFTARROW L"ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ArrayIndexSet_EM (&$4.tkToken, &$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN error        NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR error" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   ASSIGN IndexListBR  NAMEUNK
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEUNK IndexListBR" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN IndexListBR  NAMEUNK
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEUNK IndexListBR" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$4.tkToken);

                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | ArrExpr ASSIGN error      NAMEUNK {DbgMsgWP (L"%%SimpExpr:  NAMEUNK error" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   ASSIGN   ')' SelectSpec '('
                                        {DbgMsgWP (L"%%SimpExpr:  (SelectSpec)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$4.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN   ')' error    '(' {DbgMsgWP (L"%%SimpExpr:  (error)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$1.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN   ')' SelectSpec '('
                                        {DbgMsgWP (L"%%SimpExpr:  (SelectSpec)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                             // If it's Selective Specification, ...
                                             if (lpplLocalVars->bSelSpec && !lpplLocalVars->bIniSpec)
                                             {
                                                 // Mark as no longer doing Selective Specification
                                                 lpplLocalVars->bSelSpec = FALSE;

                                                 // Remove Selective Specification until I figure it out
                                                 #if FALSE
                                                     lpplLocalVars->bRet =
                                                       ArrayIndexSet_EM (&lpplLocalVars->tkSelSpec, &$4.tkToken, &$1.tkToken);
                                                 #else
                                                     lpplLocalVars->bRet = FALSE;
                                                     ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                                                               &$3.tkToken);
                                                 #endif
                                             } else
                                             {
                                                 // Mark as no longer doing Selective Specification
                                                 lpplLocalVars->bSelSpec =
                                                 lpplLocalVars->bIniSpec = FALSE;

                                                 lpplLocalVars->bRet =
                                                   AssignNamedVars_EM (&$4.tkToken, &$1.tkToken);
                                             } // End IF/ELSE/...

/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeResult (&$4.tkToken);

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN   ')' IndexListBR NAMEVAR '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEVAR IndexListBR)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$4.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN   ')' IndexListBR NAMEVAR '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEVAR IndexListBR)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ArrayIndexSet_EM (&$5.tkToken, &$4.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeResult (&$4.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN   ')' error NAMEVAR '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEVAR error)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   ASSIGN   ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEUNK IndexListBR)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$4.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN   ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEUNK IndexListBR)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$5.tkToken);

                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | ArrExpr ASSIGN   ')' error       NAMEUNK '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEUNK error)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Mark as no longer doing Selective Specification
                                             lpplLocalVars->bSelSpec = FALSE;

                                             FreeResult (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   ASSIGN LeftOper  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrExpr ASSIGN error     NAMEVAR  //--Conflicts
    | ArrExpr ASSIGN LeftOper  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, FALSE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
                                             FreeResult (&lpplLocalVars->lpYYRes->tkToken); YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN AxisFunc  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrExpr ASSIGN error     NAMEVAR  //--Conflicts
    | ArrExpr ASSIGN AxisFunc  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, FALSE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
/////////////////////////////////////////////////FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN LeftOper ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN LeftOper ')' error '('
                                        {DbgMsgWP (L"%%SimpExpr:  (error) LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN error    ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) error" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$5.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN LeftOper ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN =
                                               MakeNameStrand_EM_YY (&$5);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameStrand_EM_YY

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             if (!lpplLocalVars->lpYYStrN            // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYFcn)
                                             {
                                                 if (lpplLocalVars->lpYYStrN)        // If defined, free it
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYFcn)         // If defined, free it
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 } // End IF

                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ModifyAssignNameVals_EM (&lpplLocalVars->lpYYStrN->tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn);  lpplLocalVars->lpYYFcn = NULL;
                                             FreeYYFcn1 (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN AxisFunc ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN AxisFunc ')' error '('
                                        {DbgMsgWP (L"%%SimpExpr:  (error) AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrExpr ASSIGN error    ')' NameVals '(' //--Conflicts
    | ArrExpr ASSIGN AxisFunc ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             lpplLocalVars->lpYYStrN =
                                               MakeNameStrand_EM_YY (&$5);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameStrand_EM_YY

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYStrN            // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYFcn)
                                             {
                                                 if (lpplLocalVars->lpYYStrN)        // If defined, free it
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYFcn)         // If defined, free it
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYFcn);  lpplLocalVars->lpYYFcn = NULL;
                                                 } // End IF

                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ModifyAssignNameVals_EM (&lpplLocalVars->lpYYStrN->tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn);  lpplLocalVars->lpYYFcn = NULL;
                                             FreeYYFcn1 (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN LeftOper IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN error    IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR error" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN LeftOper IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$4.tkToken);
/////////////////////////////////////////////////FreeResult (&$5.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ArrayIndexFcnSet_EM (&$5.tkToken, &$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$4.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN LeftOper error NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR error LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             // If this is not a named function/operator, ...
                                             if (!IsTknTypeNamedFcnOpr ($3.tkToken.tkFlags.TknType))
                                                 FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ArrExpr ASSIGN LeftOper error NAMEUNK
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEUNK error LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             // If this is not a named function/operator, ...
                                             if (!IsTknTypeNamedFcnOpr ($3.tkToken.tkFlags.TknType))
                                                 FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error   ASSIGN AxisFunc IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| ArrExpr ASSIGN error    IndexListBR NAMEVAR //-- Conflicts:  redundant
    | ArrExpr ASSIGN AxisFunc IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             // Copy as temporary var in case it's named
                                             $$ = MakeTempCopy (&$1);

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$4.tkToken);
/////////////////////////////////////////////////FreeResult (&$5.tkToken);           // Validation only
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->bRet = FALSE;
                                             else
                                                 lpplLocalVars->bRet =
                                                   ArrayIndexFcnSet_EM (&$5.tkToken, &$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$4.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Mark as already displayed
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN AxisFunc error NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR error AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    ;

// Selective specification
SelectSpec:
      NameVars                          {DbgMsgWP (L"%%SelectSpec:  NameVars");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN =
                                               MakeNameStrand_EM_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameStrand_EM_YY

                                             if (!lpplLocalVars->lpYYStrN)           // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYStrN;
                                             YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                         } // End IF
                                        }
////| SelectSpec error                  //--Conflicts
    | SelectSpec LeftOper               {DbgMsgWP (L"%%SelectSpec:  LeftOper SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
////| SelectSpec error                  //--Conflicts
    | SelectSpec Drv1Func               {DbgMsgWP (L"%%SelectSpec:  Drv1Func SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | SelectSpec Drv3Func               {DbgMsgWP (L"%%SelectSpec:  Drv3Func SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | SelectSpec AxisFunc               {DbgMsgWP (L"%%SelectSpec:  AxisFunc SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken, FALSE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | SelectSpec LeftOper error         {DbgMsgWP (L"%%SelectSpec:  error LeftOper SelectSpec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             // If this is not a named function/operator, ...
                                             if (!IsTknTypeNamedFcnOpr ($2.tkToken.tkFlags.TknType))
                                                 FreeResult (&$2.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| SelectSpec error    StrandInst    //--Conflicts
    | SelectSpec LeftOper StrandInst    {DbgMsgWP (L"%%SelectSpec:  StrandInst LeftOper SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | SelectSpec AxisFunc error         {DbgMsgWP (L"%%SelectSpec:  error AxisFunc SelectSpec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| SelectSpec error    StrandInst    //--Conflicts
    | SelectSpec AxisFunc StrandInst    {DbgMsgWP (L"%%SelectSpec:  StrandInst AxisFunc SelectSpec");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Check on Selective Specification first (and only) name
                                             if (!CheckSelSpec_EM (lpplLocalVars, &$1.tkToken))
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken, TRUE, TRUE);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Named Variables Strand w/Value
// Skip Ctrl-Break checking here so the Name Strand processing isn't interrupted
NameVals:
      NAMEVAR                           {DbgMsgWP (L"%%NameVals:  NAMEVAR");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             InitNameStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NAMEUNK                           {DbgMsgWP (L"%%NameVals:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | NameVals       NAMEVAR            {DbgMsgWP (L"%%NameVals:  NAMEVAR NameVals");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $2.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NameVals       NAMEUNK            {DbgMsgWP (L"%%NameVals:  NAMEUNK NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$2.tkToken);
                                             FreeResult (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    |       ')' IndexListBR NAMEVAR '(' {DbgMsgWP (L"%%NameVals:  (NAMEVAR IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $3.tkToken.tkFlags.TknType = TKT_VARNAMED;




                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnNonceError_EM (&$2.tkToken);
                                             FreeResult (&$2.tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    |       ')' error       NAMEVAR '(' {DbgMsgWP (L"%%NameVals:  (NAMEVAR error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    |       ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%NameVals:  (NAMEUNK IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$2.tkToken);
                                             FreeResult (&$2.tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    |       ')' error       NAMEUNK '(' {DbgMsgWP (L"%%NameVals:  (NAMEUNK error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | NameVals ')' IndexListBR NAMEVAR '('
                                        {DbgMsgWP (L"%%NameVals:  (NAMEVAR IndexListBR) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;




                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnNonceError_EM (&$3.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | NameVals ')' error    NAMEVAR '(' {DbgMsgWP (L"%%NameVals:  (NAMEVAR error) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | NameVals ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%NameVals:  (NAMEUNK IndexListBR) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$4.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | NameVals ')' error    NAMEUNK '(' {DbgMsgWP (L"%%NameVals:  (NAMEUNK error) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    ;

// Named Variables Strand with or w/o Value
// Skip Ctrl-Break checking here so the Name Strand processing isn't interrupted
NameVars:
      NAMEVAR                           {DbgMsgWP (L"%%NameVars:  NAMEVAR");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             InitNameStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NAMEUNK                           {DbgMsgWP (L"%%NameVars:  NAMEUNK");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             InitNameStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ')' IndexListBR NAMEVAR '('       {DbgMsgWP (L"%%NameVars:  (NAMEVAR IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $3.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnNonceError_EM (&$3.tkToken);


                                             FreeResult (&$2.tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | ')' error       NAMEVAR '('       {DbgMsgWP (L"%%NameVars:  (NAMEVAR error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | ')' IndexListBR NAMEUNK '('       {DbgMsgWP (L"%%NameVars:  (NAMEUNK IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                                                 PrimFnValueError_EM (&$3.tkToken);
                                             FreeResult (&$2.tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR3
                                         } // End IF
                                        }
    | ')' error       NAMEUNK '('       {DbgMsgWP (L"%%NameVars:  (NAMEUNK error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | NameVars       NAMEVAR            {DbgMsgWP (L"%%NameVars:  NameVars NAMEVAR");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $2.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NameVars       NAMEUNK            {DbgMsgWP (L"%%NameVars:  NameVars NAMEUNK");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushNameStrand_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 1
//   valid in ArrExpr:      ArrExpr Drv1Func
//            FcnSpec:      ...
//            ParenFunc:    (Drv1Func)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv1Func:
      RightOper DydOp error             {DbgMsgWP (L"%%Drv1Func:  error DydOp RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$2.tkToken);       // Don't free named fcn/opr on error
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | FILLJOT   DydOp StrandOp1         {DbgMsgWP (L"%%Drv1Func:  StrandOp1 DydOp FILLJOT");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | FILLJOT   DydOp StrandInst        {DbgMsgWP (L"%%Drv1Func:  StrandInst DydOp FILLJOT");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | RightOper DydOp StrandOp1         {DbgMsgWP (L"%%Drv1Func:  StrandOp1 DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | RightOper DydOp StrandInst        {DbgMsgWP (L"%%Drv1Func:  StrandInst DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | FILLJOT   DydOp Drv1Func          {DbgMsgWP (L"%%Drv1Func:  Drv1Func DydOp FILLJOT");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | RightOper DydOp Drv1Func          {DbgMsgWP (L"%%Drv1Func:  Drv1Func DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |           MonOp error             {DbgMsgWP (L"%%Drv1Func:  error MonOp");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    |           MonOp StrandOp1         {DbgMsgWP (L"%%Drv1Func:  StrandOp1 MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |           MonOp StrandInst        {DbgMsgWP (L"%%Drv1Func:  StrandInst MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 2
//   valid in FcnSpec:      ...
//   but not ArrExpr: because of Strand on the right
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv2Func:
      StrandInst DydOp error            {DbgMsgWP (L"%%Drv2Func:  error DydOp StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$2.tkToken);       // Don't free named fcn/opr on error
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | error      DydOp LeftOper         {DbgMsgWP (L"%%Drv2Func:  LeftOper DydOp error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$2.tkToken);       // Don't free named fcn/opr on error
                                             FreeResult (&$3.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | StrandInst DydOp FILLJOT          {DbgMsgWP (L"%%Drv2Func:  FILLJOT DydOp StrandInst");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$3);
                                             FreeResult (&$3.tkToken);

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Left operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | StrandInst DydOp LeftOper         {DbgMsgWP (L"%%Drv2Func:  LeftOper DydOp StrandInst");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | error      DydOp AxisFunc         {DbgMsgWP (L"%%Drv2Func:  AxisFunc DydOp error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$2.tkToken);       // Don't free named fcn/opr on error
                                             FreeResult (&$3.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | FILLJOT    DydOp AxisFunc         {DbgMsgWP (L"%%Drv2Func:  AxisFunc DydOp StrandInst");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT);    // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | StrandInst DydOp AxisFunc         {DbgMsgWP (L"%%Drv2Func:  AxisFunc DydOp StrandInst");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 3
//   valid in FcnSpec:      ...
//            ArrExpr:      ArrExpr Drv3Func
//   because the Strand on the right is bound to the DydOp)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv3Func:
      StrandOp2  DydOp error            {DbgMsgWP (L"%%Drv3Func:  error DydOp StrandOp2");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
/////////////////////////////////////////////FreeResult (&$2.tkToken);       // Don't free named fcn/opr on error
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | StrandOp2  DydOp FILLJOT          {DbgMsgWP (L"%%Drv3Func:  FILLJOT DydOp StrandOp2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$3);
                                             FreeResult (&$3.tkToken);

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Left operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | StrandOp2  DydOp LeftOper         {DbgMsgWP (L"%%Drv3Func:  LeftOper DydOp StrandOp2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | StrandOp2  DydOp AxisFunc         {DbgMsgWP (L"%%Drv3Func:  AxisFunc DydOp StrandOp2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 4
//    valid in FcnSpec:     ...
//             ArrExpr:     ArrExpr Drv4Func
//             LeftOper:    MonOp Drv4Func
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv4Func:
////| StrandInst DydOp error            --See above
      StrandOp2  DydOp StrandOp1        {DbgMsgWP (L"%%Drv4Func:  StrandOp1 DydOp StrandOp2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | StrandOp2  DydOp StrandInst       {DbgMsgWP (L"%%Drv4Func:  StrandInst DydOp StrandOp2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 5
//    valid in ArrExpr:     ArrExpr Drv5Func StrandInst
//             ParenFunc:   (Drv5Func)
Drv5Func:
      MonOp AmbOpX                      {DbgMsgWP (L"%%Drv5Func:  AmbOpX MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (&$2);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | Drv3Func                          {DbgMsgWP (L"%%Drv5Func:  Drv3Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | MonOp Drv5Func                    {DbgMsgWP (L"%%Drv5Func:  Drv5Func MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | AmbOpX Drv5Func                   {DbgMsgWP (L"%%Drv5Func:  Drv5Func AmbOpX");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Parenthesized function expression
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
ParenFunc:
      '>' error    '('                  {DbgMsgWP (L"%%ParenFunc:  (error)");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | '>' Drv1Func '('                  {DbgMsgWP (L"%%ParenFunc:  (Drv1Func)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' Drv2Func '('                  {DbgMsgWP (L"%%ParenFunc:  (Drv2Func)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' Drv4Func '('                  {DbgMsgWP (L"%%ParenFunc:  (Drv4Func)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' Drv5Func '('                  {DbgMsgWP (L"%%ParenFunc:  (Drv5Func)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' LeftOper '('                  {DbgMsgWP (L"%%ParenFunc:  (LeftOper)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' AxisFunc '('                  {DbgMsgWP (L"%%ParenFunc:  (AxisFunc)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' FcnSpec '('                   {DbgMsgWP (L"%%ParenFunc:  (FcnSpec)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Function (Direct)

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '>' Op1Spec error    '('          {DbgMsgWP (L"%%ParenFunc:  (error Op1Spec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '>' Op1Spec LeftOper '('          {DbgMsgWP (L"%%ParenFunc:  (LeftOper Op1Spec)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$2, 2, DIRECT);    // Monadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Left monadic operand function
LeftMonOper:
      Drv1Func                          {DbgMsgWP (L"%%LeftMonOper:  Drv1Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | Drv3Func                          {DbgMsgWP (L"%%LeftMonOper:  Drv3Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | Drv4Func                          {DbgMsgWP (L"%%LeftMonOper:  Drv4Func");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | MonOp LeftMonOper                 {DbgMsgWP (L"%%LeftMonOper:  LeftMonOper MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Left operand function
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
LeftOper:
          PRIMFCN  SYSNS                {DbgMsgWP (L"%%LeftOper:  SYSNS PRIMFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
        | PRIMFCN                       {DbgMsgWP (L"%%LeftOper:  PRIMFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN  SYSNS                {DbgMsgWP (L"%%LeftOper:  SYSNS NAMEFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    |     NAMEFCN                       {DbgMsgWP (L"%%LeftOper:  NAMEFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMETRN                       {DbgMsgWP (L"%%LeftOper:  NAMETRN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     SYSFN12  SYSNS                {DbgMsgWP (L"%%LeftOper:  SYSNS SYSFN12");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    |     SYSFN12                       {DbgMsgWP (L"%%LeftOper:  SYSFN12");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     ParenFunc                     {DbgMsgWP (L"%%LeftOper:  ParenFunc");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |     RightOper    JOTDOT           {DbgMsgWP (L"%%LeftOper:  " WS_UTF16_JOT L". RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$2, 2, DIRECT);    // Monadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    | NULLOP OP3                        {DbgMsgWP (L"%%LeftOper:  OP3 NULLOP");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (&$2);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, DIRECT);    // Monadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Left operand (Direct)

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|                  MonOp error      //--Conflicts
    |                  MonOp FILLJOT    {DbgMsgWP (L"%%LeftOper:  FILLJOT MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Left operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |                  MonOp Drv2Func   {DbgMsgWP (L"%%LeftOper:  Drv2Func MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |                  MonOp LeftOper   {DbgMsgWP (L"%%LeftOper:  LeftOper MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|                  MonOp error      //--Conflicts
    |                  MonOp AxisFunc   {DbgMsgWP (L"%%LeftOper:  AxisFunc MonOp");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|                  AmbOpX error     //--Conflicts
    |                  AmbOpX LeftOper  {DbgMsgWP (L"%%LeftOper:  LeftOper AmbOpX"); //***FIXME***
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|     RightOper DydOp error         //--Conflicts
    |     RightOper DydOp FILLJOT       {DbgMsgWP (L"%%LeftOper:  FILLJOT DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$3);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT);     // Left operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |     FILLJOT   DydOp LeftOper      {DbgMsgWP (L"%%LeftOper:  LeftOper DydOp FILLJOT");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |     FILLJOT   DydOp FILLJOT       {DbgMsgWP (L"%%LeftOper:  FILLJOT DydOp FILLJOT");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$3);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Left operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             lpplLocalVars->lpYYMak =
                                               MakeFillJot_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Right operand (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |     RightOper DydOp LeftOper      {DbgMsgWP (L"%%LeftOper:  LeftOper DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
////|     RightOper DydOp error         //--Conflicts
    |     RightOper DydOp AxisFunc      {DbgMsgWP (L"%%LeftOper:  AxisFunc DydOp RightOper");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    | '%' Train '('                     {DbgMsgWP (L"%%LeftOper:  (Train)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $2.lpYYStrandBase = $2.lpYYFcnBase;

                                             lpplLocalVars->lpYYLft =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_TRN, TRUE);

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYStrL =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYLft, 1, DIRECT); // Lefthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYStrL;
                                             YYFree (lpplLocalVars->lpYYStrL); lpplLocalVars->lpYYStrL = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&$$.tkToken);
#ifdef DEBUG
                                             // Display the strand stack
                                             DisplayStrand (STRAND_FCN);
#endif
                                         } // End IF
                                        }
    ;

Train:
      LeftOper error                    {DbgMsgWP (L"%%Train:  error LeftOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $1.lpYYStrandBase = $1.lpYYFcnBase;

                                             FreeResult (&$1.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | AxisFunc error                    {DbgMsgWP (L"%%Train:  error AxisFunc");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $1.lpYYStrandBase = $1.lpYYFcnBase;

                                             FreeResult (&$1.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
////| error    LeftOper                 //--Conflicts
    | AxisFunc AxisFunc                 {DbgMsgWP (L"%%Train:  AxisFunc AxisFunc");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $2.lpYYStrandBase = $2.lpYYFcnBase;

                                             lpplLocalVars->lpYYLft =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             // Initialize the function strand (Train) base
                                             $1.lpYYStrandBase = $1.lpYYFcnBase;

                                             lpplLocalVars->lpYYRht =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             lpplLocalVars->lpYYMak =
                                               MakeTrainOp_YY (&$1);

                                             if (!lpplLocalVars->lpYYLft             // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYRht
                                              || !lpplLocalVars->lpYYMak)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);

                                                 if (lpplLocalVars->lpYYLft)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYMak)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 } // End IF

                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 3, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             lpplLocalVars->lpYYStrR =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYRht, 1, DIRECT); // Righthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;

                                             lpplLocalVars->lpYYStrL =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYLft, 1, DIRECT); // Lefthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrL->tkToken);

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrR->tkToken);

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             YYFree (lpplLocalVars->lpYYStrL); lpplLocalVars->lpYYStrL = NULL;
                                             YYFree (lpplLocalVars->lpYYStrR); lpplLocalVars->lpYYStrR = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1 ); lpplLocalVars->lpYYOp1  = NULL;
#ifdef DEBUG
                                             // Display the strand stack
                                             DisplayStrand (STRAND_FCN);
#endif
                                         } // End IF
                                        }
    | AxisFunc LeftOper                 {DbgMsgWP (L"%%Train:  LeftOper AxisFunc");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $2.lpYYStrandBase = $2.lpYYFcnBase;

                                             lpplLocalVars->lpYYLft =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             // Initialize the function strand (Train) base
                                             $1.lpYYStrandBase = $1.lpYYFcnBase;

                                             lpplLocalVars->lpYYRht =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             lpplLocalVars->lpYYMak =
                                               MakeTrainOp_YY (&$1);

                                             if (!lpplLocalVars->lpYYLft             // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYRht
                                              || !lpplLocalVars->lpYYMak)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);

                                                 if (lpplLocalVars->lpYYLft)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYMak)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 } // End IF

                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 3, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             lpplLocalVars->lpYYStrR =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYRht, 1, DIRECT); // Righthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;

                                             lpplLocalVars->lpYYStrL =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYLft, 1, DIRECT); // Lefthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrL->tkToken);

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrR->tkToken);

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             YYFree (lpplLocalVars->lpYYStrL); lpplLocalVars->lpYYStrL = NULL;
                                             YYFree (lpplLocalVars->lpYYStrR); lpplLocalVars->lpYYStrR = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1 ); lpplLocalVars->lpYYOp1  = NULL;
#ifdef DEBUG
                                             // Display the strand stack
                                             DisplayStrand (STRAND_FCN);
#endif
                                         } // End IF
                                        }
    | LeftOper AxisFunc                 {DbgMsgWP (L"%%Train:  AxisFunc LeftOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $2.lpYYStrandBase = $2.lpYYFcnBase;

                                             lpplLocalVars->lpYYLft =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             // Initialize the function strand (Train) base
                                             $1.lpYYStrandBase = $1.lpYYFcnBase;

                                             lpplLocalVars->lpYYRht =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             lpplLocalVars->lpYYMak =
                                               MakeTrainOp_YY (&$1);

                                             if (!lpplLocalVars->lpYYLft             // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYRht
                                              || !lpplLocalVars->lpYYMak)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);

                                                 if (lpplLocalVars->lpYYLft)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYMak)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 } // End IF

                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 3, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             lpplLocalVars->lpYYStrR =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYRht, 1, DIRECT); // Righthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;

                                             lpplLocalVars->lpYYStrL =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYLft, 1, DIRECT); // Lefthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrL->tkToken);

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrR->tkToken);

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             YYFree (lpplLocalVars->lpYYStrL); lpplLocalVars->lpYYStrL = NULL;
                                             YYFree (lpplLocalVars->lpYYStrR); lpplLocalVars->lpYYStrR = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1 ); lpplLocalVars->lpYYOp1  = NULL;
#ifdef DEBUG
                                             // Display the strand stack
                                             DisplayStrand (STRAND_FCN);
#endif
                                         } // End IF
                                        }
    | LeftOper LeftOper                 {DbgMsgWP (L"%%Train:  LeftOper LeftOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $2.lpYYStrandBase = $2.lpYYFcnBase;

                                             lpplLocalVars->lpYYLft =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             // Initialize the function strand (Train) base
                                             $1.lpYYStrandBase = $1.lpYYFcnBase;

                                             lpplLocalVars->lpYYRht =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);

                                             lpplLocalVars->lpYYMak =
                                               MakeTrainOp_YY (&$1);

                                             if (!lpplLocalVars->lpYYLft             // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYRht
                                              || !lpplLocalVars->lpYYMak)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);

                                                 if (lpplLocalVars->lpYYLft)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYMak)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 } // End IF

                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 3, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             lpplLocalVars->lpYYStrR =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYRht, 1, DIRECT); // Righthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;

                                             lpplLocalVars->lpYYStrL =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYLft, 1, DIRECT); // Lefthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrL->tkToken);

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrR->tkToken);

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             YYFree (lpplLocalVars->lpYYStrL); lpplLocalVars->lpYYStrL = NULL;
                                             YYFree (lpplLocalVars->lpYYStrR); lpplLocalVars->lpYYStrR = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1 ); lpplLocalVars->lpYYOp1  = NULL;
#ifdef DEBUG
                                             // Display the strand stack
                                             DisplayStrand (STRAND_FCN);
#endif
                                         } // End IF
                                        }
    | Train    error                    {DbgMsgWP (L"%%Train:  error Train");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the function strand (Train) base
                                             $1.lpYYStrandBase = $1.lpYYFcnBase;

                                             FreeResult (&$1.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | Train    LeftOper                 {DbgMsgWP (L"%%Train:  LeftOper Train");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Increment the token count
                                             $1.TknCount++; $1.lpYYFcnBase->TknCount++;

                                             // Initialize the function strand (Train) base
                                             $2.lpYYStrandBase = $2.lpYYFcnBase;

                                             lpplLocalVars->lpYYLft =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 // Initialize the function strand (Train) base
                                                 $1.lpYYStrandBase = $1.lpYYFcnBase;

                                                 lpplLocalVars->lpYYRht =
                                                   MakeFcnStrand_EM_YY (&$1, NAMETYPE_TRN, TRUE);

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYStrL =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYLft, 1, DIRECT); // Lefthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             if (!lpplLocalVars->lpYYStrL)           // If not defined, free args and YYERROR
                                             {
                                                 // Initialize the function strand (Train) base
                                                 $1.lpYYStrandBase = $1.lpYYFcnBase;

                                                 lpplLocalVars->lpYYRht =
                                                   MakeFcnStrand_EM_YY (&$1, NAMETYPE_TRN, TRUE);

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrL->tkToken);

                                             YYFree (lpplLocalVars->lpYYStrL); lpplLocalVars->lpYYStrL = NULL;
#ifdef DEBUG
                                             // Display the strand stack
                                             DisplayStrand (STRAND_FCN);
#endif
                                             // The result is always the root of the function tree
                                             $$ = $1;
                                         } // End IF
                                        }
    | Train    AxisFunc                 {DbgMsgWP (L"%%Train:  AxisFunc Train");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Increment the token count
                                             $1.TknCount++; $1.lpYYFcnBase->TknCount++;

                                             // Initialize the function strand (Train) base
                                             $2.lpYYStrandBase = $2.lpYYFcnBase;

                                             lpplLocalVars->lpYYLft =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, TRUE);

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 // Initialize the function strand (Train) base
                                                 $1.lpYYStrandBase = $1.lpYYFcnBase;

                                                 lpplLocalVars->lpYYRht =
                                                   MakeFcnStrand_EM_YY (&$1, NAMETYPE_TRN, TRUE);

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYStrL =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYLft, 1, DIRECT); // Lefthand function (Direct)
                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             if (!lpplLocalVars->lpYYStrL)           // If not defined, free args and YYERROR
                                             {
                                                 // Initialize the function strand (Train) base
                                                 $1.lpYYStrandBase = $1.lpYYFcnBase;

                                                 lpplLocalVars->lpYYRht =
                                                   MakeFcnStrand_EM_YY (&$1, NAMETYPE_TRN, TRUE);

                                                 if (lpplLocalVars->lpYYRht)
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                                 } // End IF

                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Because there's no matching DecrRefCnt to the IncrRefCnt
                                             //   in MakeFcnStrand_EM_YY, we mark this array as skipping
                                             //   the next IncrRefCnt.
                                             SetVFOArraySRCIFlag (&lpplLocalVars->lpYYStrL->tkToken);

                                             YYFree (lpplLocalVars->lpYYStrL); lpplLocalVars->lpYYStrL = NULL;
#ifdef DEBUG
                                             // Display the strand stack
                                             DisplayStrand (STRAND_FCN);
#endif
                                             // The result is always the root of the function tree
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

// Right operand function (short right scope)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
RightOper:
          PRIMFCN SYSNS                 {DbgMsgWP (L"%%RightOper:  SYSNS PRIMFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
        | PRIMFCN                       {DbgMsgWP (L"%%RightOper:  PRIMFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN SYSNS                 {DbgMsgWP (L"%%RightOper:  SYSNS NAMEFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    |     NAMEFCN                       {DbgMsgWP (L"%%RightOper:  NAMEFCN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMETRN                       {DbgMsgWP (L"%%RightOper:  NAMETRN");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     SYSFN12 SYSNS                 {DbgMsgWP (L"%%RightOper:  SYSNS SYSFN12");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $2.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    |     SYSFN12                       {DbgMsgWP (L"%%RightOper:  SYSFN12");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     ParenFunc                     {DbgMsgWP (L"%%RightOper:  ParenFunc");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

// Axis function expression
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
AxisFunc:
      '}' error   '['  PRIMFCN SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS PRIMFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' error   '['  PRIMFCN          {DbgMsgWP (L"%%AxisFunc:  PRIMFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrExpr '['  PRIMFCN SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS PRIMFCN[ArrExpr]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$4);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $5.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    | '}' ArrExpr '['  PRIMFCN          {DbgMsgWP (L"%%AxisFunc:  PRIMFCN[ArrExpr]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$4);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  NAMEFCN SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS NAMEFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' error   '['  NAMEFCN          {DbgMsgWP (L"%%AxisFunc:  NAMEFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrExpr '['  NAMEFCN SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS NAMEFCN[ArrExpr]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$4, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $5.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    | '}' ArrExpr '['  NAMEFCN          {DbgMsgWP (L"%%AxisFunc:  NAMEFCN[ArrExpr]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$4, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  SYSFN12 SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS SYSFN12[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' error   '['  SYSFN12          {DbgMsgWP (L"%%AxisFunc:  SYSFN12[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrExpr '['  SYSFN12 SYSNS    {DbgMsgWP (L"%%AxisFunc:  SYSNS SYSFN12[ArrExpr]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$4, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             // Copy the system namespace level
                                             $$.tkToken.tkFlags.SysNSLvl = $5.tkToken.tkFlags.SysNSLvl;
                                         } // End IF
                                        }
    | '}' ArrExpr '['  SYSFN12          {DbgMsgWP (L"%%AxisFunc:  SYSFN12[ArrExpr]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$4, FALSE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  ParenFunc        {DbgMsgWP (L"%%AxisFunc:  ParenFunc[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrExpr '['  ParenFunc        {DbgMsgWP (L"%%AxisFunc:  ParenFunc[ArrExpr]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Function (Indirect)

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    ;

// Ambiguous operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
AmbOp:
          OP3                           {DbgMsgWP (L"%%AmbOp:  OP3");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    |     NAMEOP3                       {DbgMsgWP (L"%%AmbOp:  NAMEOP3");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
////|     error                         //--Conflicts
    | '>' AmbOpX '('                    {DbgMsgWP (L"%%AmbOp:  (AmbOpX)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    | '>' Op3Spec '('                   {DbgMsgWP (L"%%AmbOp:  (Op3Spec)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Ambiguous operator (Direct)

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Ambiguous operator w/axis
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
AmbOpAxis:
      '}' error   '['  AmbOp            {DbgMsgWP (L"%%AmbOpAxis:  AmbOp[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrExpr '['  AmbOp            {DbgMsgWP (L"%%AmbOpAxis:  AmbOp[ArrExpr]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Ambiguous operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    ;

// Ambiguous operator with or w/o axis
AmbOpX:
      AmbOp                             {DbgMsgWP (L"%%AmbX:  AmbOp");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | AmbOpAxis                         {DbgMsgWP (L"%%AmbX:  AmbOpAxis");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

// Monadic operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
MonOp:
                       OP1              {DbgMsgWP (L"%%MonOp:  OP1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp1_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |                  NULLOP           {DbgMsgWP (L"%%MonOp:  NULLOP");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp1_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |                  NAMEOP1          {DbgMsgWP (L"%%MonOp:  NAMEOP1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|              error                //--Conflicts
    |              MonOpAxis            {DbgMsgWP (L"%%MonOp:  MonOpAxis");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |              '>' MonOp '('        {DbgMsgWP (L"%%MonOp:  (MonOp)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    |              '>' Op1Spec '('      {DbgMsgWP (L"%%MonOp:  (Op1Spec)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Monadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Monadic operator w/axis
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
MonOpAxis:
      '}' error   '['  MonOp            {DbgMsgWP (L"%%MonOpAxis:  MonOp[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '}' ArrExpr '['  MonOp            {DbgMsgWP (L"%%MonOpAxis:  MonOp[ArrExpr]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Monadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    ;

// Dyadic operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
DydOp:
                       OP2              {DbgMsgWP (L"%%DydOp:  OP2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp2_YY (&$1);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Dyadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |                  NAMEOP2          {DbgMsgWP (L"%%DydOp:  NAMEOP2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1, TRUE);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Dyadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
////|                  error            //--Conflicts
    |                  DydOpAxis        {DbgMsgWP (L"%%DydOp:  DydOpAxis");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |              '#' DydOp '('        {DbgMsgWP (L"%%DydOp:  (DydOp)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    |              '#' Op2Spec '('      {DbgMsgWP (L"%%DydOp:  (Op2Spec)");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Dyadic operator (Direct)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Dyadic operator w/axis
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
DydOpAxis:
      '?' error   '['  DydOp            {DbgMsgWP (L"%%DydOpAxis:  DydOp[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | '?' ArrExpr '['  DydOp            {DbgMsgWP (L"%%DydOpAxis:  DydOp[ArrExpr]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Dyadic operator (Indirect)

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                                 YYERROR3
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    ;

// Index list w/brackets, allowing for A[A][A]...
IndexListBR:
                  ']'             '['   {DbgMsgWP (L"%%IndexListBR:  []");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst =
                                               InitList0_YY ();
                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             lpplLocalVars->lpYYRes =
                                               MakeList_EM_YY (lpplLocalVars->lpYYLst, TRUE);
                                             YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |             ']' error       '['   {DbgMsgWP (L"%%IndexListBR:  [error]");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    |             ']' IndexListWE '['   {DbgMsgWP (L"%%IndexListBR:  [IndexListWE]");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst =
                                               MakeList_EM_YY (&$2, TRUE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYLst;
                                             YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;
                                         } // End IF
                                        }
    | IndexListBR ']'             '['   {DbgMsgWP (L"%%IndexListBR:  [] IndexListBR ");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | IndexListBR ']' error       '['   {DbgMsgWP (L"%%IndexListBR:  [error] IndexListBR ");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    | IndexListBR ']' IndexListWE '['   {DbgMsgWP (L"%%IndexListBR:  [IndexListWE] IndexListBR ");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst =
                                               MakeList_EM_YY (&$3, TRUE);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                                                 lpplLocalVars->lpYYRes = NULL;
                                             else
                                                 lpplLocalVars->lpYYRes =
                                                   ListIndexRef_EM_YY (&lpplLocalVars->lpYYLst->tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Index list, with empties (meaning no ArrExpr between semicolons)
// Skip Ctrl-Break checking here so the List processing isn't interrupted
IndexListWE:
      IndexListWE1                      {DbgMsgWP (L"%%IndexListWE:  IndexListWE1");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | IndexListWE2                      {DbgMsgWP (L"%%IndexListWE:  IndexListWE2");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

// Index list, with empties, starting with a semicolon
IndexListWE1:
                   ';'                  {DbgMsgWP (L"%%IndexListWE1:  ;");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the list with an empty item
                                             lpplLocalVars->lpYYLst =
                                               InitList1_YY (NULL);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             // Push an empty item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (lpplLocalVars->lpYYLst, NULL);
                                             FreeYYFcn1 (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |              ';' error            {DbgMsgWP (L"%%IndexListWE1:  error;");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR3
                                         else
                                             YYERROR2
                                        }
    |              ';' ArrExpr          {DbgMsgWP (L"%%IndexListWE1:  ArrExpr;");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the list with an empty item
                                             lpplLocalVars->lpYYLst =
                                               InitList1_YY (NULL);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             // Push an item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (lpplLocalVars->lpYYLst, &$2);
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // Copied w/o IncrRefCnt in PushList_YY
                                             FreeYYFcn1 (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR3

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE1 ';'                  {DbgMsgWP (L"%%IndexListWE1:  ;IndexListWE1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Push an empty item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, NULL);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE1 ';' error            {DbgMsgWP (L"%%IndexListWE1:  error;IndexListWE1");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | IndexListWE1 ';' ArrExpr          {DbgMsgWP (L"%%IndexListWE1:  ArrExpr;IndexListWE1");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, &$3);
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Copied w/o IncrRefCnt in PushList_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Index list, with empties, starting with an array expr
IndexListWE2:
                       ArrExpr          {DbgMsgWP (L"%%IndexListWE2:  ArrExpr");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the list with the arg
                                             lpplLocalVars->lpYYRes =
                                               InitList1_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // Copied w/o IncrRefCnt in PushList_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE2 ';'                  {DbgMsgWP (L"%%IndexListWE2:  ;IndexListWE2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Push an empty item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, NULL);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE2 ';' error            {DbgMsgWP (L"%%IndexListWE2:  error;IndexListWE2");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR3
                                         } else
                                             YYERROR2
                                        }
    | IndexListWE2 ';' ArrExpr          {DbgMsgWP (L"%%IndexListWE2:  ArrExpr;IndexListWE2");
                                         // No leading check for Ctrl-Break so as not to interrupt function/variable strand processing
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Push an item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, &$3);
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Copied w/o IncrRefCnt in PushList_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR3
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes;
                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

%%
#undef  APPEND_NAME

//***************************************************************************
//  Start of C program
//***************************************************************************


//***************************************************************************
//  $ParseLine
//
//  Parse a line
//  The result of parsing the line is in the return value (plLocalVars.ExitType)
//    as well as in lpMemPTD->YYResExec if there is a value (EXITTYPE_DISPLAY
//    or EXITTYPE_NODISPLAY).
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseLine"
#else
#define APPEND_NAME
#endif

EXIT_TYPES ParseLine
    (HWND         hWndSM,               // Session Manager window handle
     HGLOBAL      hGlbTknLine,          // Tokenized line global memory handle (may be NULL)
     HGLOBAL      hGlbTxtLine,          // Text      ...
     LPWCHAR      lpwszLine,            // Ptr to the line text (may be NULL)
     LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     UINT         uLineNum,             // Function line #
     UINT         uTknNum,              // Starting token # in the above function line
     HGLOBAL      hGlbDfnHdr,           // User-defined function/operator global memory handle (NULL = execute/immexec)
     UBOOL        bActOnErrors,         // TRUE iff errors are acted upon
     UBOOL        bExec1Stmt)           // TRUE iff executing only one stmt

{
    PLLOCALVARS   plLocalVars = {0};    // ParseLine local vars
    LPPLLOCALVARS oldTlsPlLocalVars;    // Ptr to previous value of dwTlsPlLocalVars
    UINT          oldTlsType,           // Previous value of dwTlsType
                  uRet;                 // The result from pl_yyparse
    ERROR_CODES   uError;               // Error code
    UBOOL         bOldExecuting;        // Old value of bExecuting
    HWND          hWndEC;               // Edit Ctrl window handle
    LPSIS_HEADER  lpSISCur,             // Ptr to current SI Stack Header
                  lpSISPrv;             // Ptr to previous ...

    // Save the previous value of dwTlsType
    oldTlsType = PtrToUlong (TlsGetValue (dwTlsType));

    // Save the thread type ('PL')
    TlsSetValue (dwTlsType, TLSTYPE_PL);

    // Save the previous value of dwTlsPlLocalVars
    oldTlsPlLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Save ptr to ParseLine local vars
    TlsSetValue (dwTlsPlLocalVars, (LPVOID) &plLocalVars);

    // Save ptr to PerTabData global memory
    TlsSetValue (dwTlsPerTabData, (LPVOID) lpMemPTD);

    DBGENTER;           // Must be placed after the TlsSetValue for lpMemPTD

    // Get the Edit Ctrl window handle
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Indicate that we're executing
    bOldExecuting = lpMemPTD->bExecuting; lpMemPTD->bExecuting = TRUE;

    EnterCriticalSection (&CSOPL);

    // Link this plLocalVars into the chain of such objects
    plLocalVars.lpPLPrev = lpMemPTD->lpPLCur;

    // If there's a previous ptr, transfer its Ctrl-Break flag
    if (lpMemPTD->lpPLCur)
        plLocalVars.bCtrlBreak = lpMemPTD->lpPLCur->bCtrlBreak;

    // Save as new current ptr
    lpMemPTD->lpPLCur = &plLocalVars;

    LeaveCriticalSection (&CSOPL);

    // Initialize the error code
    uError = ERRORCODE_NONE;

    // If we don't have a valid handle, ...
    if (!hGlbTknLine)
    {
        plLocalVars.ExitType = EXITTYPE_ERROR;

        goto NORMAL_EXIT;
    } // End IF
#ifdef DEBUG
    // Display the tokens so far
    DisplayTokens (hGlbTknLine);
#endif

    // Save values in the LocalVars
    plLocalVars.lpMemPTD    = lpMemPTD;
    plLocalVars.hWndSM      = hWndSM;
    plLocalVars.hGlbTxtLine = hGlbTxtLine;
    plLocalVars.hGlbTknLine = hGlbTknLine;
    plLocalVars.lpwszLine   = lpwszLine;
    plLocalVars.bLookAhead  = FALSE;
    plLocalVars.ExitType    = EXITTYPE_NONE;
    plLocalVars.uLineNum    = uLineNum;
    plLocalVars.hGlbDfnHdr  = hGlbDfnHdr;
    plLocalVars.bExec1Stmt  = bExec1Stmt;

    // Lock the memory to get a ptr to it, and set the variables
    UTLockAndSet (plLocalVars.hGlbTknLine, &plLocalVars.t2);

    // Get # tokens in the line
    plLocalVars.uTokenCnt = ((LPTOKEN_HEADER) plLocalVars.t2.lpBase)->TokenCnt;

    // If the starting token # is outside the token count, ...
    if (uTknNum >= plLocalVars.uTokenCnt)
    {
        // Set the exit type to exit normally
        plLocalVars.ExitType = EXITTYPE_GOTO_ZILDE;

        goto NORMAL_EXIT;
    } // End IF

    // Skip over TOKEN_HEADER
    plLocalVars.lptkStart = TokenBaseToStart (plLocalVars.t2.lpBase);
    plLocalVars.lptkEnd   = &plLocalVars.lptkStart[plLocalVars.uTokenCnt];

    Assert (plLocalVars.lptkStart->tkFlags.TknType EQ TKT_EOL
         || plLocalVars.lptkStart->tkFlags.TknType EQ TKT_EOS);

    // If we're not starting at the first token, ...
    if (uTknNum)
    {
        // Skip to the starting token
        plLocalVars.lptkNext  = &plLocalVars.lptkStart[uTknNum];

        // If this token is an EOS/EOL, skip to the end of the stmt
        //   and start executing there
        if (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOS
         || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOL)
            plLocalVars.lptkNext = &plLocalVars.lptkNext[plLocalVars.lptkNext->tkData.tkIndex - 1];
    } else
        // Skip to end of 1st stmt
        plLocalVars.lptkNext  = &plLocalVars.lptkStart[plLocalVars.lptkStart->tkData.tkIndex];

    // Start off with no error
    plLocalVars.tkErrorCharIndex =
    plLocalVars.tkLACharIndex    = NEG1U;

    // Initialize the strand starts
    if (oldTlsPlLocalVars)
    {
        plLocalVars.lpYYStrArrStart[STRAND_VAR] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_VAR];
        plLocalVars.lpYYStrArrStart[STRAND_FCN] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_FCN];
        plLocalVars.lpYYStrArrStart[STRAND_LST] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_LST];
        plLocalVars.lpYYStrArrStart[STRAND_NAM] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_NAM];
    } else
    {
        plLocalVars.lpYYStrArrStart[STRAND_VAR] = lpMemPTD->lpStrand[STRAND_VAR];
        plLocalVars.lpYYStrArrStart[STRAND_FCN] = lpMemPTD->lpStrand[STRAND_FCN];
        plLocalVars.lpYYStrArrStart[STRAND_LST] = lpMemPTD->lpStrand[STRAND_LST];
        plLocalVars.lpYYStrArrStart[STRAND_NAM] = lpMemPTD->lpStrand[STRAND_NAM];
    } // End IF/ELSE

    // Initialize the base & next strand ptrs
    plLocalVars.lpYYStrArrStart[STRAND_VAR]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_VAR]                 =
    plLocalVars.lpYYStrArrNext [STRAND_VAR]                 = plLocalVars.lpYYStrArrStart[STRAND_VAR];
    plLocalVars.lpYYStrArrStart[STRAND_FCN]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_FCN]                 =
    plLocalVars.lpYYStrArrNext [STRAND_FCN]                 = plLocalVars.lpYYStrArrStart[STRAND_FCN];
    plLocalVars.lpYYStrArrStart[STRAND_LST]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_LST]                 =
    plLocalVars.lpYYStrArrNext [STRAND_LST]                 = plLocalVars.lpYYStrArrStart[STRAND_LST];
    plLocalVars.lpYYStrArrStart[STRAND_NAM]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_NAM]                 =
    plLocalVars.lpYYStrArrNext [STRAND_NAM]                 = plLocalVars.lpYYStrArrStart[STRAND_NAM];

    // Use VirtualAlloc for the stack
    // ***FIXME***














#if YYDEBUG
    // Enable debugging
    yydebug = TRUE;
#endif

    __try
    {
        __try
        {
            // Parse the line, check for errors
            //   0 = success
            //   1 = YYABORT or APL error
            //   2 = memory exhausted
            uRet = pl_yyparse (&plLocalVars);
        } __except (CheckVirtAlloc (GetExceptionInformation (),
                                    L"ParseLine"))
        {} // End __try/__except
    } __except (CheckException (GetExceptionInformation (), L"ParseLine"))
    {
        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_STACK_OVERFLOW:
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_ERROR;

                // Mark as in error
                uRet = 1;
                uError = ERRORCODE_ELX;
                ErrorMessageIndirectToken (ERRMSG_STACK_OVERFLOW APPEND_NAME,
                                           NULL);
                break;

            default:
                // Display message for unhandled exception
                DisplayException ();

                // Mark as in error
                uRet = 1;
                uError = ERRORCODE_ELX;

                goto NORMAL_EXIT;
        } // End SWITCH
    } // End __try/__except

#if YYDEBUG
    // Disable debugging
    yydebug = FALSE;
#endif

    if (!plLocalVars.bCtrlBreak
     && !plLocalVars.bLookAhead)
    // Split cases based upon the exit type
    switch (plLocalVars.ExitType)
    {
        case EXITTYPE_QUADERROR_INIT:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_QUADERROR_INIT;

            break;

        case EXITTYPE_RESET_ALL:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ALL;

            break;

        case EXITTYPE_RESET_ONE:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE;

            break;

        case EXITTYPE_RESET_ONE_INIT:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE_INIT;

            break;

        case EXITTYPE_QUADERROR_EXEC:
            // Set the exit type
            plLocalVars.ExitType = EXITTYPE_ERROR;

            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

            // Fall through to common code

        case EXITTYPE_ERROR:        // Mark user-defined function/operator as suspended
        case EXITTYPE_STOP:
        {
            // Get a ptr to the current SIS header
            lpSISCur = lpMemPTD->lpSISCur;

            // If it's a permanent function (i.e. Magic Function/Operator), don't suspend at this level
            if (lpSISCur->PermFn)
            {
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_QUADERROR_INIT;

                // Set the reset flag
                lpSISCur->ResetFlag = RESETFLAG_QUADERROR_INIT;

                break;
            } // End IF

            // If this level or an adjacent preceding level is from
            //   Execute or immediate execution mode,
            //   peel back to the preceding level
            while (lpSISCur
                && (lpSISCur->DfnType EQ DFNTYPE_EXEC
                 || lpSISCur->DfnType EQ DFNTYPE_IMM))
                lpSISCur = lpSISCur->lpSISPrv;

            // If this level is a user-defined function/operator,
            //   mark it as suspended
            if (lpSISCur
             && (lpSISCur->DfnType EQ DFNTYPE_OP1
              || lpSISCur->DfnType EQ DFNTYPE_OP2
              || lpSISCur->DfnType EQ DFNTYPE_FCN))
                lpSISCur->Suspended = TRUE;
            break;
        } // End EXITTYPE_ERROR

        case EXITTYPE_DISPLAY:      // Nothing more to do with these types
        case EXITTYPE_NODISPLAY:    // ...
        case EXITTYPE_NOVALUE:      // ...
        case EXITTYPE_GOTO_ZILDE:   // ...
        case EXITTYPE_GOTO_LINE:    // ...
        case EXITTYPE_NONE:         // ...
            // Check on user-defined function/operator exit error
            if (CheckDfnExitError_EM (lpMemPTD))
            {
                // Mark as an APL error
                plLocalVars.ExitType = EXITTYPE_ERROR;
                uRet = 1;
            } // End IF

            break;

        case EXITTYPE_RETURNxLX:
            uRet = 0;

            break;

        defstop
            break;
    } // End IF/SWITCH

    // If Ctrl-Break was pressed, ...
    if (plLocalVars.bCtrlBreak)
    {
        // Mark as in error
        uError = ERRORCODE_ALX;

        // Reset the flag
        plLocalVars.bCtrlBreak = FALSE;

        // Peel back to first user-defined function/operator
        for (lpSISCur = lpMemPTD->lpSISCur;
             lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt;
             lpSISCur = lpSISCur->lpSISPrv)
        if (lpSISCur->DfnType EQ DFNTYPE_OP1
         || lpSISCur->DfnType EQ DFNTYPE_OP2
         || lpSISCur->DfnType EQ DFNTYPE_FCN)
            break;

        // If called from a user-defined function/operator, ...
        if (lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt)
            // Signal an error
            BreakMessage (hWndSM, lpSISCur);
    } // End IF

    if (uRet EQ 0 || uError EQ ERRORCODE_ALX)
        goto NORMAL_EXIT;

    // If we're not resetting, ...
    if (lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_NONE)
    {
        // If called from Immediate Execution/Execute, ...
        if (lpwszLine)
            // Signal an error
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                                lpwszLine,                      // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
        else
        {
            LPMEMTXT_UNION lpMemTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Signal an error
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                               &lpMemTxtLine->C,                // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF/ELSE

        // Mark as in error
        uError = ERRORCODE_ELX;
    } // End IF
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (plLocalVars.hGlbTknLine); plLocalVars.t2.lpBase   = NULL;

    EnterCriticalSection (&CSOPL);

    // Unlink this plLocalVars from the chain of such objects
    lpMemPTD->lpPLCur = plLocalVars.lpPLPrev;

    // Transfer the Ctrl-Break flag in case it hasn't been acted upon
    if (lpMemPTD->lpPLCur)
        lpMemPTD->lpPLCur->bCtrlBreak = plLocalVars.bCtrlBreak;

    LeaveCriticalSection (&CSOPL);

    // Restore the previous value of dwTlsType
    TlsSetValue (dwTlsType, ULongToPtr (oldTlsType));

    // If there's an error to be signalled, ...
    if (uError NE ERRORCODE_NONE
     && bActOnErrors)
    {
        EXIT_TYPES exitType;        // Return code from ImmExecStmt

#ifdef DEBUG
        if (uError EQ ERRORCODE_ELX)
        {
            DbgMsgW2 (L"~~Start []ELX on");
        } else
        {
            DbgMsgW2 (L"~~Start []ALX on");
        } // End IF/ELSE -- DO NOT REMOVE as the DbgMsgW2 macro needs
        //                  this because of the trailing semicolon

        if (lpwszLine)
        {
            DbgMsgW2 (lpwszLine);
        } else
        {
            LPMEMTXT_UNION lpMemTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Display the function line
            DbgMsgW2 (&lpMemTxtLine->C);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF/ELSE
#endif
        // Set the text for the line
        if (uError EQ ERRORCODE_ELX)
            lpwszLine = WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"ELX";
        else
            lpwszLine = WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"ALX";

        // Execute the statement
        exitType =
          PrimFnMonUpTackJotCSPLParse (hWndEC,      // Edit Ctrl window handle
                                       lpMemPTD,    // Ptr to PerTabData global memory
                                       lpwszLine,   // Ptr to text of line to execute
                                       NULL);       // Ptr to function token
        // Split cases based upon the exit type
        switch (exitType)
        {
            case EXITTYPE_GOTO_LINE:
            case EXITTYPE_RESET_ALL:
            case EXITTYPE_RESET_ONE:
            case EXITTYPE_RESET_ONE_INIT:
                // Pass on to caller
                plLocalVars.ExitType = exitType;

                break;

            case EXITTYPE_NODISPLAY:    // Display the result (if any)
            case EXITTYPE_DISPLAY:      // ...
                // If the Execute/Quad result is present, display it
                if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType)
                {
                    // Display the array
                    ArrayDisplay_EM (&lpMemPTD->YYResExec.tkToken, TRUE, &plLocalVars.bCtrlBreak);

                    // Free the result
                    FreeResult (&lpMemPTD->YYResExec.tkToken);

                    // We no longer need these values
                    ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
                } // End IF

                // Fall through to common code

            case EXITTYPE_ERROR:        // Display the prompt unless called by Quad or User fcn/opr
            case EXITTYPE_NOVALUE:      // ...
                // Get the ptr to the current SIS header
                lpSISPrv =
                lpSISCur = lpMemPTD->lpSISCur;

                // If this level is Immediate Execution Mode, peel back
                while (lpSISPrv
                    && lpSISPrv->DfnType EQ DFNTYPE_IMM)
                    lpSISPrv = lpSISPrv->lpSISPrv;

                // If this level is Execute, skip the prompt
                //   as Execute will handle that
                if (lpSISPrv
                 && lpSISPrv->DfnType EQ DFNTYPE_EXEC)
                {
                    // If the result of {execute}[]xLX is a normal
                    //   result (EXITTYPE_NODISPLAY),
                    //   return  EXITTYPE_RETURNxLX so the caller
                    //   can avoid displaying a prompt until the
                    //   result of executing []xLX is handled
                    if (exitType EQ EXITTYPE_NODISPLAY)
                        exitType =  EXITTYPE_RETURNxLX;

                    // Pass on this exit type to the caller
                    plLocalVars.ExitType = exitType;

                    break;
                } // End IF

                // If this level or an adjacent preceding level is from
                //   Execute or Immediate Execution Mode,
                //   peel back to the preceding level
                while (lpSISPrv
                    && (lpSISPrv->DfnType EQ DFNTYPE_EXEC
                     || lpSISPrv->DfnType EQ DFNTYPE_IMM))
                    lpSISPrv = lpSISPrv->lpSISPrv;
                break;

            case EXITTYPE_GOTO_ZILDE:   // Nothing more to do with these
            case EXITTYPE_NONE:         // ...
                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

    // Restore the previous value of dwTlsPlLocalVars
    TlsSetValue (dwTlsPlLocalVars, oldTlsPlLocalVars);

    // Restore the previous executing state
    lpMemPTD->bExecuting = bOldExecuting;

    DBGLEAVE;

    return plLocalVars.ExitType;
} // End ParseLine
#undef  APPEND_NAME


//***************************************************************************
//  $SymbTypeVFO
//
//  Get the type ('V', 'F', '1', '2', '3', or '?') of a symbol table name token.
//  Used for TKT_VARNAMEDs.
//***************************************************************************

char SymbTypeVFO
    (LPTOKEN lptkNext)

{
    STFLAGS stFlags;

    // Get the symbol table flags
    stFlags = lptkNext->tkData.tkSym->stFlags;

    if (stFlags.Imm)        // IMMTYPE_BOOL, IMMTYPE_INT, IMMTYPE_CHAR, IMMTYPE_FLOAT, IMMTYPE_PRIMFCN, ...
    switch (stFlags.ImmType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
        case IMMTYPE_CHAR:
        case IMMTYPE_FLOAT:
            return 'V';

        case IMMTYPE_PRIMFCN:
            return 'F';

        case IMMTYPE_PRIMOP1:
            return '1';

        case IMMTYPE_PRIMOP2:
            return '2';

        case IMMTYPE_PRIMOP3:
            return '3';

        defstop
            return '?';
    } // End IF/SWITCH

    // Split cases based upon the NAMETYPE_xxx
    switch (stFlags.stNameType)
    {
        case NAMETYPE_UNK:
            // If it's a NAMETYPE_UNK, then it's really a VALUE ERROR, but this
            //   isn't the time to signal that.
            return 'V';

        case NAMETYPE_VAR:
        case NAMETYPE_FN0:
            return 'V';

        case NAMETYPE_FN12:
        case NAMETYPE_TRN:
            return 'F';

        case NAMETYPE_OP1:
            return '1';

        case NAMETYPE_OP2:
            return '2';

        case NAMETYPE_OP3:
            return '3';

        case NAMETYPE_FILL1:
        case NAMETYPE_LST:
        case NAMETYPE_FILL2:
        default:
            return '?';             // SYNTAX ERROR
    } // End SWITCH
} // SymbTypeVFO


//***************************************************************************
//  $LookaheadSurround
//
//  Lookahead (backwards) in the token stream to see if
//    the next non-grouping symbol is a function or a var.
//  Used for TKT_RIGHTPARENs to see if it and the matching
//    TKT_LEFTPAREN surrounds a function, variable, monadic or dyadic
//    operator, or for / & \ to see if the next non-grouping
//    symbol is a function, variable, or dyadic operator.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- LookaheadSurround"
#else
#define APPEND_NAME
#endif

char LookaheadSurround
    (LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    PLLOCALVARS plLocalVars;        // Local copy of outer PLLOCALVARS
    char        cRes;               // The result char
    UINT        uRet;               // The result from pl_yyparse

#ifdef DEBUG
    DbgMsgW2 (L"==Entering LookaheadSurround");
#endif
    // Copy outer lpplLocalVars
    plLocalVars = *lpplLocalVars;

    // Mark as looking ahead for token type inside surrounding parens
    plLocalVars.bLookAhead = TRUE;

    // Save the address of the stop token
    // Each right grouping symbol has the index of the matching
    //   left grouping symbol in its tkData.tkIndex
    plLocalVars.lptkStop = &plLocalVars.lptkStart[plLocalVars.lptkNext->tkData.tkIndex];

    // Parse the file, check for errors
    //   0 = success
    //   1 = YYABORT or APL error
    //   2 = memory xhausted
    uRet = pl_yyparse (&plLocalVars);

    // If the LookAhead char index is filled in, pass it on up
    if (plLocalVars.tkLACharIndex NE NEG1U)
        lpplLocalVars->tkLACharIndex = plLocalVars.tkLACharIndex;

    // Check for errors
    if (uRet)
        cRes = '?';
    else
        cRes = NAMETYPE_STRING[plLocalVars.plNameType];
#ifdef DEBUG
    dprintfWL9 (L"==Exiting  LookaheadSurround:  %c", cRes);
#endif
    return cRes;
} // End LookaheadSurround
#undef  APPEND_NAME


//***************************************************************************
//  $LookaheadAdjacent
//
//  Lookahead (backwards) in the token stream to see if
//    the matching grouping symbol is adjacent to a function, variable,
//    monadic or dyadic operator.
//  Used for TKT_RIGHTBRACKETs to see if this is indexing or axis.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- LookaheadAdjacent"
#else
#define APPEND_NAME
#endif

char LookaheadAdjacent
    (LPPLLOCALVARS lpplLocalVars,   // Ptr to local plLocalVars
     UBOOL         bSkipBrackets,   // TRUE iff we're to skip over left/right brackets first
     UBOOL         bCheckLeft)      // TRUE iff we're to check to the left of the current symbol

{
    PLLOCALVARS plLocalVars;    // Local copy of outer PLLOCALVARS
    char        cRes;           // The result char

    if (bSkipBrackets)
    {
        DbgMsgW2 (L"==Entering LookaheadAdjacent:  TRUE");
    } else
    {
        DbgMsgW2 (L"==Entering LookaheadAdjacent:  FALSE");
    } // End IF/ELSE

    // Copy outer lpplLocalVars
    plLocalVars = *lpplLocalVars;

    // Each right grouping symbol has the index of the matching
    //   left grouping symbol in its tkData.tkIndex.
    // Get a ptr to the token adjacent to ("-1") the matching left
    //   grouping symbol.
    if (bSkipBrackets)
        plLocalVars.lptkNext = &plLocalVars.lptkStart[plLocalVars.lptkNext->tkData.tkIndex - 1];
    else
        plLocalVars.lptkNext--; // Skip over the function/operator symbol
    while (TRUE)
    // Split cases based upon the token type
    switch (plLocalVars.lptkNext->tkFlags.TknType)
    {
        case TKT_FILLJOT:
            // If the caller doesn't want us to check to the left of the jot, ...
            if (!bCheckLeft)
            {
                cRes = 'J';             // Jot

                goto NORMAL_EXIT;
            } // End IF

            // Otherwise, fall through to check to the left

        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
        case TKT_INPOUT:
            // Lookahead to see if the next token is a dyadic operator:
            //   if so, return 'F'; otherwise return 'V'.

            // If the next token is a dyadic op, ...
            if (LookaheadDyadicOp (&plLocalVars, &plLocalVars.lptkNext[-1]))
                cRes = 'F';     // Function
            else
                cRes = 'V';     // Variable

            goto NORMAL_EXIT;

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OPJOTDOT:
            cRes = '1';             // Monadic operator

            goto NORMAL_EXIT;

        case TKT_OP2IMMED:
            // If the OP2 is JOT, ...
            if (plLocalVars.lptkNext->tkData.tkChar EQ UTF16_JOT)
            {
                // If the caller wants us to check to the left of the jot, ...
                if (bCheckLeft)
                {
                    // Lookahead to see if the next token is a dyadic operator:
                    //   if so, return 'F'; otherwise return 'V'.

                    // If the next token is a dyadic op, ...
                    if (LookaheadDyadicOp (&plLocalVars, &plLocalVars.lptkNext[-1]))
                        cRes = 'F';     // Function
                    else
                        cRes = 'V';     // Variable
                } else
                    cRes = 'J';             // Jot
            } else
                cRes = '2';             // Dyadic operator

            goto NORMAL_EXIT;

        case TKT_OP2NAMED:
            // If the OP2 is JOT, ...
            if (plLocalVars.lptkNext->tkData.tkSym->stData.stChar EQ UTF16_JOT)
            {
                // If the caller wants us to check to the left of the jot, ...
                if (bCheckLeft)
                {
                    // Lookahead to see if the next token is a dyadic operator:
                    //   if so, return 'F'; otherwise return 'V'.

                    // If the next token is a dyadic op, ...
                    if (LookaheadDyadicOp (&plLocalVars, &plLocalVars.lptkNext[-1]))
                        cRes = 'F';     // Function
                    else
                        cRes = 'V';     // Variable
                } else
                    cRes = 'J';             // Jot
            } else
                cRes = '2';             // Dyadic operator

            goto NORMAL_EXIT;

        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
            cRes = '3';             // Ambiguous operator

            goto NORMAL_EXIT;

        case TKT_FCNIMMED:
        case TKT_FCNARRAY:
        case TKT_FCNNAMED:
            cRes = 'F';             // Function

            goto NORMAL_EXIT;

        case TKT_VARNAMED:
            // Get the token type of the symbol table name
            cRes = SymbTypeVFO (plLocalVars.lptkNext);

            goto NORMAL_EXIT;

        case TKT_RIGHTBRACKET:
            // Each right grouping symbol has the index of the matching
            //   left grouping symbol in its tkData.tkIndex
            // Get a ptr to the token adjacent to ("-1") the matching left
            //   grouping symbol.
            plLocalVars.lptkNext = &plLocalVars.lptkStart[plLocalVars.lptkNext->tkData.tkIndex - 1];

            break;                  // Go around again

        case TKT_RIGHTPAREN:
            cRes = LookaheadSurround (&plLocalVars);

            goto NORMAL_EXIT;

        case TKT_LINECONT:          // Line continuation marker
        case TKT_SYS_NS:            // System namespace
            plLocalVars.lptkNext--; // Ignore these tokens

            break;                  // Go around again

        case TKT_EOS:
        case TKT_EOL:
            cRes = 'E';             // EOS/EOL

            goto NORMAL_EXIT;

        case TKT_LEFTPAREN:         // To allow (//R)
            if (!bSkipBrackets)
            {
                cRes = '(';         // Left paren

                goto NORMAL_EXIT;
            } // End IF

            cRes = '?';             // SYNTAX ERROR

            goto NORMAL_EXIT;

        case TKT_ASSIGN:            // To allow f{is}/[1]
            cRes = 'A';             // Assignment arrow

            goto NORMAL_EXIT;

        case TKT_LISTSEP:
        case TKT_LABELSEP:
        case TKT_COLON:
        case TKT_LEFTBRACKET:
        case TKT_STRAND:
        case TKT_AXISIMMED:
        case TKT_AXISARRAY:
        case TKT_LISTINT:
        case TKT_LISTPAR:
        case TKT_LSTIMMED:
        case TKT_LSTARRAY:
        case TKT_LSTMULT:
        case TKT_LEFTBRACE:
        case TKT_RIGHTBRACE:
        case TKT_SOS:
        case TKT_CS_ANDIF:          // Control structure:  ANDIF     (Data is Line/Stmt #)
        case TKT_CS_CASE:           // ...                 CASE
        case TKT_CS_CASELIST:       // ...                 CASELIST
        case TKT_CS_CONTINUE:       // ...                 CONTINUE
        case TKT_CS_ELSE:           // ...                 ELSE
        case TKT_CS_ELSEIF:         // ...                 ELSEIF
        case TKT_CS_END:            // ...                 END
        case TKT_CS_ENDFOR:         // ...                 ENDFOR
        case TKT_CS_ENDFORLCL:      // ...                 ENDFORLCL
        case TKT_CS_ENDIF:          // ...                 ENDIF
        case TKT_CS_ENDREPEAT:      // ...                 ENDREPEAT
        case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
        case TKT_CS_ENDWHILE:       // ...                 ENDWHILE
        case TKT_CS_FOR:            // ...                 FOR
        case TKT_CS_FOR2:           // ...                 FOR2
        case TKT_CS_FORLCL:         // ...                 FORLCL
        case TKT_CS_GOTO:           // ...                 GOTO
        case TKT_CS_IF:             // ...                 IF
        case TKT_CS_IF2:            // ...                 IF2
        case TKT_CS_IN:             // ...                 IN
        case TKT_CS_LEAVE:          // ...                 LEAVE
        case TKT_CS_ORIF:           // ...                 ORIF
        case TKT_CS_REPEAT:         // ...                 REPEAT
        case TKT_CS_REPEAT2:        // ...                 REPEAT2
        case TKT_CS_RETURN:         // ...                 RETURN
        case TKT_CS_SELECT:         // ...                 SELECT
        case TKT_CS_SELECT2:        // ...                 SELECT2
        case TKT_CS_UNTIL:          // ...                 UNTIL
        case TKT_CS_WHILE:          // ...                 WHILE
        case TKT_CS_WHILE2:         // ...                 WHILE2
        case TKT_CS_SKIPCASE:       // ...                 Special token
        case TKT_CS_SKIPEND:        // ...                 Special token
        case TKT_CS_NEC:            // ...                 Special token
        case TKT_CS_EOL:            // ...                 Special token
        case TKT_SYNTERR:           // Syntax Error
            cRes = '?';             // SYNTAX ERROR

            goto NORMAL_EXIT;

        defstop
            cRes = '?';             // SYNTAX ERROR

            goto NORMAL_EXIT;
    } // End WHILE/SWITCH
NORMAL_EXIT:
#ifdef DEBUG
    dprintfWL9 (L"==Exiting  LookaheadAdjacent:  %c", cRes);
#endif
    return cRes;
} // LookaheadAdjacent
#undef  APPEND_NAME


//***************************************************************************
//  $LookaheadDyadicOp
//
//  The current token is a variable, but it could be part of a function
//    if the next non-grouping symbol is a dyadic operator.
//
//  Lookahead (to the left) in the token stream to see if
//    the next non-right grouping symbol is a dyadic operator or not.
//***************************************************************************

UBOOL LookaheadDyadicOp
    (LPPLLOCALVARS lpplLocalVars,
     LPTOKEN       lptkNext)

{
    UBOOL bRet;         // The result

    DbgMsgW2 (L"==Entering LookaheadDyadicOp");

    while (TRUE)
    // Split cases based upon the token type
    switch (lptkNext->tkFlags.TknType)
    {
        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_VARIMMED:
        case TKT_LEFTPAREN:
        case TKT_FCNIMMED:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_SOS:
        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_LISTINT:
        case TKT_LISTPAR:
        case TKT_LSTIMMED:
        case TKT_LSTARRAY:
        case TKT_LSTMULT:
        case TKT_LABELSEP:
        case TKT_COLON:
        case TKT_FCNNAMED:
        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
        case TKT_LEFTBRACKET:
        case TKT_LEFTBRACE:
        case TKT_RIGHTBRACE:
        case TKT_VARARRAY:
        case TKT_INPOUT:
        case TKT_CS_ANDIF:          // Control structure:  ANDIF     (Data is Line/Stmt #)
        case TKT_CS_CASE:           // ...                 CASE
        case TKT_CS_CASELIST:       // ...                 CASELIST
        case TKT_CS_CONTINUE:       // ...                 CONTINUE
        case TKT_CS_ELSE:           // ...                 ELSE
        case TKT_CS_ELSEIF:         // ...                 ELSEIF
        case TKT_CS_END:            // ...                 END
        case TKT_CS_ENDFOR:         // ...                 ENDFOR
        case TKT_CS_ENDFORLCL:      // ...                 ENDFORLCL
        case TKT_CS_ENDIF:          // ...                 ENDIF
        case TKT_CS_ENDREPEAT:      // ...                 ENDREPEAT
        case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
        case TKT_CS_ENDWHILE:       // ...                 ENDWHILE
        case TKT_CS_FOR:            // ...                 FOR
        case TKT_CS_FOR2:           // ...                 FOR2
        case TKT_CS_FORLCL:         // ...                 FORLCL
        case TKT_CS_GOTO:           // ...                 GOTO
        case TKT_CS_IF:             // ...                 IF
        case TKT_CS_IF2:            // ...                 IF2
        case TKT_CS_IN:             // ...                 IN
        case TKT_CS_LEAVE:          // ...                 LEAVE
        case TKT_CS_ORIF:           // ...                 ORIF
        case TKT_CS_REPEAT:         // ...                 REPEAT
        case TKT_CS_REPEAT2:        // ...                 REPEAT2
        case TKT_CS_RETURN:         // ...                 RETURN
        case TKT_CS_SELECT:         // ...                 SELECT
        case TKT_CS_SELECT2:        // ...                 SELECT2
        case TKT_CS_UNTIL:          // ...                 UNTIL
        case TKT_CS_WHILE:          // ...                 WHILE
        case TKT_CS_WHILE2:         // ...                 WHILE2
        case TKT_CS_SKIPCASE:       // ...                 Special token
        case TKT_CS_SKIPEND:        // ...                 Special token
        case TKT_CS_NEC:            // ...                 Special token
        case TKT_CS_EOL:            // ...                 Special token
        case TKT_SYNTERR:           // Syntax Error
        case TKT_SYS_NS:            // System namespace
        case TKT_FILLJOT:           // Fill jot
            bRet = FALSE;

            goto NORMAL_EXIT;

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
        case TKT_OPJOTDOT:
            bRet = TRUE;

            goto NORMAL_EXIT;

        case TKT_VARNAMED:
            // Look inside the symbol table entry
            bRet = (lptkNext->tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2);

            goto NORMAL_EXIT;

        case TKT_RIGHTPAREN:
            lptkNext--;             // Ignore this token

            break;                  // Go around again

        case TKT_RIGHTBRACKET:
            // Each right grouping symbol has the index of the matching
            //   left grouping symbol in its tkData.tkIndex
            // Get a ptr to the token adjacent to ("-1") the matching left
            //   grouping symbol.
            lptkNext = &lpplLocalVars->lptkStart[lptkNext->tkData.tkIndex - 1];

            break;                  // Go around again

        case TKT_LINECONT:
            lptkNext--;             // Ignore this token

            break;                  // Go around again

        case TKT_STRAND:
        defstop
            break;
    } // End WHILE/SWITCH
NORMAL_EXIT:
#ifdef DEBUG
    dprintfWL9 (L"==Exiting  LookaheadDyadicOp:  %d", bRet);
#endif
    return bRet;
} // End LookaheadDyadicOp


//***************************************************************************
//  $LookbehindOp
//
//  The current token is a variable, but it could be part of a function
//    if the prev non-grouping symbol is a monadic or dyadic operator.
//
//  Lookbehind (to the right) in the token stream to see if
//    the next non-right grouping symbol is a monadic or dyadic operator or not.
//***************************************************************************

UBOOL LookbehindOp
    (LPPLLOCALVARS lpplLocalVars,
     LPTOKEN       lptkPrev)

{
    UBOOL bRet;         // The result

    DbgMsgW2 (L"==Entering LookbehindOp");

    while (TRUE)
    // Split cases based upon the token type
    switch (lptkPrev->tkFlags.TknType)
    {
        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_VARIMMED:
        case TKT_LEFTPAREN:
        case TKT_RIGHTPAREN:
        case TKT_FCNIMMED:
        case TKT_OPJOTDOT:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_SOS:
        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_LISTINT:
        case TKT_LISTPAR:
        case TKT_LSTIMMED:
        case TKT_LSTARRAY:
        case TKT_LSTMULT:
        case TKT_LABELSEP:
        case TKT_COLON:
        case TKT_FCNNAMED:
        case TKT_LEFTBRACKET:
        case TKT_RIGHTBRACKET:
        case TKT_LEFTBRACE:
        case TKT_RIGHTBRACE:
        case TKT_VARARRAY:
        case TKT_INPOUT:
        case TKT_CS_ANDIF:          // Control structure:  ANDIF     (Data is Line/Stmt #)
        case TKT_CS_CASE:           // ...                 CASE
        case TKT_CS_CASELIST:       // ...                 CASELIST
        case TKT_CS_CONTINUE:       // ...                 CONTINUE
        case TKT_CS_ELSE:           // ...                 ELSE
        case TKT_CS_ELSEIF:         // ...                 ELSEIF
        case TKT_CS_END:            // ...                 END
        case TKT_CS_ENDFOR:         // ...                 ENDFOR
        case TKT_CS_ENDFORLCL:      // ...                 ENDFORLCL
        case TKT_CS_ENDIF:          // ...                 ENDIF
        case TKT_CS_ENDREPEAT:      // ...                 ENDREPEAT
        case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
        case TKT_CS_ENDWHILE:       // ...                 ENDWHILE
        case TKT_CS_FOR:            // ...                 FOR
        case TKT_CS_FOR2:           // ...                 FOR2
        case TKT_CS_FORLCL:         // ...                 FORLCL
        case TKT_CS_GOTO:           // ...                 GOTO
        case TKT_CS_IF:             // ...                 IF
        case TKT_CS_IF2:            // ...                 IF2
        case TKT_CS_IN:             // ...                 IN
        case TKT_CS_LEAVE:          // ...                 LEAVE
        case TKT_CS_ORIF:           // ...                 ORIF
        case TKT_CS_REPEAT:         // ...                 REPEAT
        case TKT_CS_REPEAT2:        // ...                 REPEAT2
        case TKT_CS_RETURN:         // ...                 RETURN
        case TKT_CS_SELECT:         // ...                 SELECT
        case TKT_CS_SELECT2:        // ...                 SELECT2
        case TKT_CS_UNTIL:          // ...                 UNTIL
        case TKT_CS_WHILE:          // ...                 WHILE
        case TKT_CS_WHILE2:         // ...                 WHILE2
        case TKT_CS_SKIPCASE:       // ...                 Special token
        case TKT_CS_SKIPEND:        // ...                 Special token
        case TKT_CS_NEC:            // ...                 Special token
        case TKT_CS_EOL:            // ...                 Special token
        case TKT_SYNTERR:           // Syntax Error
        case TKT_SYS_NS:            // System namespace
        case TKT_FILLJOT:           // Fill jot
            bRet = FALSE;

            goto NORMAL_EXIT;

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
        case TKT_OP3IMMED:
        case TKT_OP3NAMED:
            bRet = TRUE;

            goto NORMAL_EXIT;

        case TKT_VARNAMED:
            // Look inside the symbol table entry
            bRet = (lptkPrev->tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP1)
                || (lptkPrev->tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2);

            goto NORMAL_EXIT;

        case TKT_LINECONT:
            lptkPrev++;             // Ignore this token

            break;                  // Go around again

        case TKT_STRAND:
        defstop
            break;
    } // End WHILE/SWITCH
NORMAL_EXIT:
#ifdef DEBUG
    dprintfWL9 (L"==Exiting  LookbehindOp:  %d", bRet);
#endif
    return bRet;
} // End LookbehindOp


//***************************************************************************
//  $pl_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

int pl_yylex
    (LPPL_YYSTYPE  lpYYLval,        // Ptr to lval
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
#ifdef DEBUG
    static UINT YYIndex = 0;        // Unique index for each YYRes
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
#endif
    // If we're restarting from a Control Structure, ...
    if (lpplLocalVars->bRestart)
    {
        lpplLocalVars->bRestart = FALSE;

        return DIAMOND;
    } // End IF
PL_YYLEX_START:
    // Because we're parsing the stmt from right to left
    lpplLocalVars->lptkNext--;

#if (defined (DEBUG)) && (defined (YYLEX_DEBUG))
    dprintfWL9 (L"==pl_yylex:  TknType = %S, CharIndex = %d",
              GetTokenTypeName (lpplLocalVars->lptkNext->tkFlags.TknType),
              lpplLocalVars->lptkNext->tkCharIndex);
#endif

    // Clear the fields
    ZeroMemory (lpYYLval, sizeof (lpYYLval[0]));

    // Return the current token
    lpYYLval->tkToken        = *lpplLocalVars->lptkNext;

    // Initialize the rest of the fields
////lpYYLval->TknCount       =              // Already zero from ZeroMemory
////lpYYLval->YYInuse        =              // ...
////lpYYLval->YYIndirect     = 0;           // ...
////lpYYLval->YYCopyArray    = FALSE;       // ...
////lpYYLval->lpYYFcnBase    = NULL;        // ...
////lpYYLval->lpYYStrandBase = NULL;        // ...
#ifdef DEBUG
    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    lpYYLval->YYIndex        = ++YYIndex;
    lpYYLval->YYFlag         = TRUE;      // Mark as a pl_yylex Index
    lpYYLval->SILevel        = lpMemPTD->SILevel;
    lpYYLval->lpFileName     = FileNameOnly (__FILE__);
    lpYYLval->uLineNum       = __LINE__;
#endif

    // Split cases based upon the token type
    switch (lpplLocalVars->lptkNext->tkFlags.TknType)
    {
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            // If the next token is a dyadic op, ...
            if (LookaheadDyadicOp (lpplLocalVars, &lpplLocalVars->lptkNext[-1]))
                return OP2CONSTANT;
            else
            // If the prev token is a monadic or dyadic op, ...
            if (LookbehindOp (lpplLocalVars, &lpplLocalVars->lptkNext[1]))
                return OP1CONSTANT;
            else
                return CONSTANT;

        case TKT_INPOUT:
            if (lpplLocalVars->lptkNext->tkData.tkChar EQ UTF16_QUAD)
                return QUAD;
            else
                return QUOTEQUAD;

        case TKT_FCNNAMED:
            return NAMEFCN;

        case TKT_VARNAMED:
        {
            STFLAGS stFlags;        // STE flags

            // Get the STE flags
            stFlags = lpplLocalVars->lptkNext->tkData.tkSym->stFlags;

            // Split cases based upon the NAMETYPE_xxx
            switch (stFlags.stNameType)
            {
                case NAMETYPE_UNK:
                    if (stFlags.DfnSysLabel)
                        return SYSLBL;
                    else
                        return NAMEUNK;

                case NAMETYPE_VAR:
                    // If the next token is a dyadic op, ...
                    if (LookaheadDyadicOp (lpplLocalVars, &lpplLocalVars->lptkNext[-1]))
                        return OP2NAMEVAR;
                    // If the prev token is a monadic or dyadic op, ...
                    if (LookbehindOp (lpplLocalVars, &lpplLocalVars->lptkNext[1]))
                        return OP1NAMEVAR;

////////////////////// Call this one TKT_VARNAMED
////////////////////lpYYLval->tkToken.tkFlags.TknType = TKT_VARNAMED;   // Already set

                    return NAMEVAR;

                case NAMETYPE_FN0:
                    // If the previous token is TKT_ASSIGN, ...
                    //   and the current one is not a system function
                    //   and the next one is not TKT_VARNAMED (NAMETYPE_VAR),
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_ASSIGN
                     && stFlags.ObjName NE OBJNAME_SYS
                     && (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_VARNAMED
                      || lpplLocalVars->lptkNext[-1].tkData.tkSym->stFlags.stNameType NE NAMETYPE_VAR))
                    {
////////////////////////// Call this one TKT_VARNAMED
////////////////////////lpYYLval->tkToken.tkFlags.TknType = TKT_VARNAMED;   // Already set

                        return NAMEVAR;
                    } // End IF

                    // Call this one TKT_FCNNAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;

                    if (stFlags.ObjName EQ OBJNAME_SYS)
                        return SYSFN0;
                    else
                        return USRFN0;

                case NAMETYPE_FN12:
                    // If the previous token is TKT_ASSIGN, ...
                    //   and the current one is not a system function
                    //   and the next one is not TKT_VARNAMED (NAMETYPE_VAR),
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_ASSIGN
                     && stFlags.ObjName NE OBJNAME_SYS
                     && (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_VARNAMED
                      || lpplLocalVars->lptkNext[-1].tkData.tkSym->stFlags.stNameType NE NAMETYPE_VAR))
                    {
////////////////////////// Call this one TKT_VARNAMED
////////////////////////lpYYLval->tkToken.tkFlags.TknType = TKT_VARNAMED;   // Already set

                        return NAMEVAR;
                    } // End IF

                    // Call this one TKT_FCNNAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;

                    if (stFlags.ObjName EQ OBJNAME_SYS)
                        return SYSFN12;
                    else
                        return NAMEFCN;

                case NAMETYPE_OP1:
                    // Call this one TKT_OP1NAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_OP1NAMED;

                    return NAMEOP1;

                case NAMETYPE_OP2:
                    // Call this one TKT_OP2NAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_OP2NAMED;

                    return NAMEOP2;

                case NAMETYPE_OP3:
                    // Call this one TKT_OP3NAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_OP3NAMED;

                    return NAMEOP3;

                case NAMETYPE_TRN:
                    // If the previous token is TKT_ASSIGN, ...
                    //   and the current one is not a system function
                    //   and the next one is not TKT_VARNAMED (NAMETYPE_VAR),
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_ASSIGN
                     && stFlags.ObjName NE OBJNAME_SYS
                     && (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_VARNAMED
                      || lpplLocalVars->lptkNext[-1].tkData.tkSym->stFlags.stNameType NE NAMETYPE_VAR))
                    {
////////////////////////// Call this one TKT_VARNAMED
////////////////////////lpYYLval->tkToken.tkFlags.TknType = TKT_VARNAMED;   // Already set

                        return NAMEVAR;
                    } // End IF

                    // Call this one TKT_FCNNAMED
                    lpYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;

                    return NAMETRN;

                case NAMETYPE_LST:
                defstop
                    return UNK;
            } // End SWITCH

            return UNK;
        } // End TKT_VARNAMED

        case TKT_ASSIGN:
            // If the next token is a right paren, we're starting Selective Specification
            if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_RIGHTPAREN)
            {
                // If we're already doing Selective Specification, signal an error
                if (lpplLocalVars->bSelSpec)
                    return UNK;
                // Mark as doing Selective Specification
                lpplLocalVars->bSelSpec =
                lpplLocalVars->bIniSpec = TRUE;
            } // End IF

            return ASSIGN;

        case TKT_LISTSEP:
            return ';';

        case TKT_FCNIMMED:
            if (lpplLocalVars->lptkNext->tkData.tkIndex EQ UTF16_RIGHTARROW)
                return GOTO;
            else
                return PRIMFCN;

        case TKT_LINECONT:
        case TKT_SOS:
            goto PL_YYLEX_START;    // Ignore these tokens

        case TKT_CHRSTRAND:
            // If the next token is a dyadic op, ...
            if (LookaheadDyadicOp (lpplLocalVars, &lpplLocalVars->lptkNext[-1]))
                return OP2CHRSTRAND;
            else
            // If the prev token is a monadic or dyadic op, ...
            if (LookbehindOp (lpplLocalVars, &lpplLocalVars->lptkNext[1]))
                return OP1CHRSTRAND;
            else
                return CHRSTRAND;

        case TKT_NUMSTRAND:
            // If the next token is a dyadic op, ...
            if (LookaheadDyadicOp (lpplLocalVars, &lpplLocalVars->lptkNext[-1]))
                return OP2NUMSTRAND;
            else
            // If the prev token is a monadic or dyadic op, ...
            if (LookbehindOp (lpplLocalVars, &lpplLocalVars->lptkNext[1]))
                return OP1NUMSTRAND;
            else
                return NUMSTRAND;

        case TKT_OP1NAMED:
            // Check for / or /[I] or /[A]
            if (CheckNullOp3 (lpplLocalVars))
                return NULLOP;
            else
                return NAMEOP1;

        case TKT_OP2NAMED:
            return NAMEOP2;

        case TKT_OP3NAMED:
            // Lookahead to see if this ambiguous symbol is adjacent to
            //   an assignment arrow            (return NAMEOP3ASSIGN)

            // Split cases based upon the lookahead result
            switch (LookaheadAdjacent (lpplLocalVars, FALSE, FALSE))
            {
                case 'A':                   // If the next token is an assignment arrow
                    return NAMEOP3ASSIGN;   //   then this token is a named ambiguous assignment

                default:
                    return NAMEOP3;
            } // End SWITCH

        case TKT_OP1IMMED:
            // Check for / or /[I] or /[A]
            if (CheckNullOp3 (lpplLocalVars))
                return NULLOP;
            else
                return OP1;

        case TKT_OP2IMMED:
            // Split cases based upon the operator symbol
            switch (lpYYLval->tkToken.tkData.tkChar)
            {
                case UTF16_JOT:
                    // If either the next or previous token is OP2NAMED,
                    //   or the previous token is OP1NAMED, ...
                    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_OP2NAMED
                     || lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP2NAMED
                     || lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP1NAMED)
                        return FILLJOT;

                    // If the previous token is VARNAMED and the name type is OP1 or OP2, ...
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_VARNAMED
                     && (lpplLocalVars->lptkNext[ 1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP1
                      || lpplLocalVars->lptkNext[ 1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2))
                        return FILLJOT;

                    // If the next     token is VARNAMED and the name type is OP2, ...
                    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_VARNAMED
                     && lpplLocalVars->lptkNext[-1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2)
                        return FILLJOT;

                    // If the previous token is OP2IMMED and the value is not JOT, ...
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP2IMMED
                     && lpplLocalVars->lptkNext[ 1].tkData.tkChar NE UTF16_JOT)
                        return FILLJOT;

                    // If the next     token is OP2IMMED and the value is not JOT, ...
                    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_OP2IMMED
                     && lpplLocalVars->lptkNext[-1].tkData.tkChar NE UTF16_JOT)
                        return FILLJOT;

                    // If the previous token is OP1IMMED, ...
                    if (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP1IMMED)
                        return FILLJOT;
                    break;

                default:
                    break;
            } // End SWITCH

            return OP2;

        case TKT_OP3IMMED:
            // Lookahead to see if this ambiguous symbol is adjacent to
            //   a function or monadic operator (return OP1), or
            //   a dyadic operator              (return PRIMFCN), or
            //   an ambiguous operator          (return OP3), or
            //   a variable or niladic function (return OP3), or
            //   an assignment arrow            (return OP3ASSIGN)

            // Split cases based upon the lookahead result
            switch (LookaheadAdjacent (lpplLocalVars, FALSE, TRUE))
            {
                case '1':               // If the next token is a monadic operator, or
                case 'F':               // If the next token is a function,
                case 'T':               // If the next token is a train,
                                        //   then this token is a monadic operator
                    // Replace the function symbol with the
                    //   corresponding monadic operator symbol/index

                    // Split cases based upon the function symbol
                    switch (lpYYLval->tkToken.tkData.tkChar)
                    {
                        case UTF16_SLASH:
                            lpYYLval->tkToken.tkData.tkChar = INDEX_OPSLASH;

                            break;

                        case UTF16_SLASHBAR:
                            lpYYLval->tkToken.tkData.tkChar = INDEX_OPSLASHBAR;

                            break;

                        case UTF16_SLOPE:
                            lpYYLval->tkToken.tkData.tkChar = INDEX_OPSLOPE;

                            break;

                        case UTF16_SLOPEBAR:
                            lpYYLval->tkToken.tkData.tkChar = INDEX_OPSLOPEBAR;

                            break;

                        defstop
                            break;
                    } // End SWITCH

                    lpYYLval->tkToken.tkFlags.TknType = TKT_OP1IMMED;
                    lpYYLval->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP1;

                    return OP1;

                case '(':               // If the next token is left paren,
                    // If the preceding token is OP3, ...
                    // ***FIXME*** -- This doesn't catch the OP3NAMED case
                    //                or the tricky /(f{is}/)3 4 -- note that f isn't assigned as yet
                    if (lpplLocalVars->lptkNext[1].tkFlags.TknType EQ TKT_OP3IMMED)
                    {
                        // Then this token is a function
                        lpYYLval->tkToken.tkFlags.TknType = TKT_FCNIMMED;
                        lpYYLval->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;

                        return PRIMFCN;
                    } // End IF

                    return OP3;

                case 'E':               // If the next token is EOL/EOS, or
                case '2':               // If the next token is a dyadic operator,
                                        //   then this token is a function
                    lpYYLval->tkToken.tkFlags.TknType = TKT_FCNIMMED;
                    lpYYLval->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;

                    return PRIMFCN;

                case '3':               // If the next token is an ambiguous operator,
                case 'V':               //   or a variable
                case '0':               //   or a niladic function,
                    return OP3;         //   then this token is ambiguous

                case 'A':               // If the next token is an assignment arrow
                    return OP3ASSIGN;   //   then this token is an ambiguous assignment

                case '?':               // SYNTAX ERROR
                    return UNK;

                defstop
                    return UNK;
            } // End SWITCH

        case TKT_OPJOTDOT:
            return JOTDOT;

        case TKT_LEFTPAREN:
            // If we're in LookaheadSurround and this is the stop token, mark as EOL
            if (lpplLocalVars->bLookAhead
             && lpplLocalVars->lptkStop EQ lpplLocalVars->lptkNext)
                return EOL;
            else
                return '(';

        case TKT_RIGHTPAREN:
            // Lookahead to see if this right grouping symbol surrounds
            //   a dyadic operator              (return '#'), or
            //   a function or monadic operator (return '>'), or
            //   a train                        (return '%'), or
            //   a variable or niladic function (return ')')

            // Split cases based upon the lookahead result
            switch (LookaheadSurround (lpplLocalVars))
            {
                case '2':               // Dyadic operator
                    return '#';

                case '1':               // Monadic operator
                case '3':               // Ambiguous ...
                case 'F':               // Function
                    return '>';

                case 'V':               // Variable
                case '0':               // Niladic function
                case 'L':               // List
                    return ')';

                case 'T':               // Train
                    return '%';

                case '?':               // SYNTAX ERROR
                    return UNK;

                defstop
                    return UNK;
            } // End SWITCH

        case TKT_LEFTBRACKET:
            return '[';

        case TKT_RIGHTBRACKET:
            // Lookahead to see if this right grouping symbol is adjacent to
            //   a dyadic operator              (return '?'), or
            //   a function or monadic operator (return '}'), or
            //   a variable or niladic function (return ']')

            // Split cases based upon the lookahead result
            switch (LookaheadAdjacent (lpplLocalVars, TRUE, FALSE))
            {
                case '2':               // If the next token is a dyadic operator,
                    return '?';         //   then this token is an axis operator

                case '1':               // If the next token is a monadic operator, or
                case '3':               //   an ambiguous operator, or
                case 'A':               //   an assignment arrow, or
                case 'F':               //   a function,
                case 'T':               //   a train,
                case 'J':               //   a jot,
                    return '}';         //   then this token is an axis operator

                case 'E':               // If the next token is EOL/EOS, or
                case 'V':               // If the next token is a variable
                case '0':               //   or a niladic function,
                    return ']';         //   then this token is an indexing symbol

                case '?':               // SYNTAX ERROR
                    return UNK;

                defstop
                    return UNK;
            } // End SWITCH

        case TKT_LABELSEP:
            // If we're at the EOL, return '\0'
            if (lpplLocalVars->lptkNext[-2].tkFlags.TknType EQ TKT_EOL)
                return '\0';

            // Fall through to common code

        case TKT_EOS:
            // If we're to execute only one stmt, ...
            if (lpplLocalVars->bExec1Stmt)
                return '\0';

            // Skip to end of the current stmt
            lpplLocalVars->lptkNext = &lpplLocalVars->lptkNext[lpplLocalVars->lptkNext->tkData.tkIndex];

            // And again to the end of the next stmt
            lpplLocalVars->lptkNext = &lpplLocalVars->lptkNext[lpplLocalVars->lptkNext->tkData.tkIndex];

            return DIAMOND;

        case TKT_EOL:
            return '\0';

        case TKT_COLON:
            return ':';

        case TKT_LEFTBRACE:
            return LBRACE;

        case TKT_RIGHTBRACE:
            return RBRACE;

        case TKT_SYS_NS:
            return SYSNS;           // System namespace

        case TKT_CS_ANDIF:          // Control structure:  ANDIF
            return CS_ANDIF;

        case TKT_CS_CASE:           // Control Structure:  CASE
            return CS_CASE;

        case TKT_CS_CASELIST:       // Control Structure:  CASELIST
            return CS_CASELIST;

        case TKT_CS_CONTINUE:       // Control Structure:  CONTINUE
            return CS_CONTINUE;

        case TKT_CS_ELSE:           // Control Structure:  ELSE
            return CS_ELSE;

        case TKT_CS_ELSEIF:         // Control Structure:  ELSEIF
            return CS_ELSEIF;

        case TKT_CS_ENDFOR:         // Control Structure:  ENDFOR
            return CS_ENDFOR;

        case TKT_CS_ENDFORLCL:      // Control Structure:  ENDFORLCL
            return CS_ENDFORLCL;

        case TKT_CS_ENDREPEAT:      // Control Structure:  ENDREPEAT
            return CS_ENDREPEAT;

        case TKT_CS_ENDWHILE:       // Control Structure:  ENDWHILE
            return CS_ENDWHILE;

        case TKT_CS_FOR:            // Control Structure:  FOR
            return CS_FOR;

        case TKT_CS_FORLCL:         // Control Structure:  FORLCL
            return CS_FORLCL;

        case TKT_CS_GOTO:           // Control Structure:  GOTO
            return GOTO;

        case TKT_CS_IF:             // Control Structure:  IF
            return CS_IF;

        case TKT_CS_IN:             // Control Structure:  IN
            return CS_IN;

        case TKT_CS_LEAVE:          // Control Structure:  LEAVE
            return CS_LEAVE;

        case TKT_CS_ORIF:           // Control Structure:  ORIF
            return CS_ORIF;

        case TKT_CS_RETURN:         // Control Structure:  RETURN
            // If this is the first time through for this keyword,
            //   return a CONSTANT
            if (!lpplLocalVars->bReturn)
            {
                // Return a Boolean zero
                lpYYLval->tkToken.tkFlags.TknType = TKT_VARIMMED;
                lpYYLval->tkToken.tkFlags.ImmType = IMMTYPE_BOOL;
                lpYYLval->tkToken.tkData.tkBoolean = 0;

                // Increment so we see this token again
                lpplLocalVars->lptkNext++;

                // Mark for the next time
                lpplLocalVars->bReturn = TRUE;

                return CONSTANT;
            } else
            {
                // Unmark for the next time
                lpplLocalVars->bReturn = FALSE;

                return GOTO;
            } // End IF/ELSE

        case TKT_CS_SELECT:         // Control Structure:  SELECT
            return CS_SELECT;

        case TKT_CS_SKIPCASE:       // Control Structure:  Special token
            return CS_SKIPCASE;

        case TKT_CS_SKIPEND:        // Control Structure:  Special token
            return CS_SKIPEND;

        case TKT_CS_UNTIL:          // Control Structure:  UNTIL
            return CS_UNTIL;

        case TKT_CS_WHILE:          // Control Structure:  WHILE
            return CS_WHILE;

        case TKT_CS_ENDIF:          // Control Structure:  ENDIF
        case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
        case TKT_CS_FOR2:           // ...                 FOR2
        case TKT_CS_IF2:            // ...                 IF2
        case TKT_CS_REPEAT:         // ...                 REPEAT
        case TKT_CS_REPEAT2:        // ...                 REPEAT2
        case TKT_CS_SELECT2:        // ...                 SELECT2
        case TKT_CS_WHILE2:         // ...                 WHILE2
            goto PL_YYLEX_START;    // Ignore these tokens

        case TKT_SYNTERR:           // Syntax Error
            return UNK;

        case TKT_CS_END:            // Control Structure:  END
        defstop
            return UNK;
    } // End SWITCH
} // End pl_yylex


//***************************************************************************
//  $CheckNullOp3
//
//  Check for NULLOP followed by an ambiguous operator possible with axis
//***************************************************************************

UBOOL CheckNullOp3
    (LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    LPTOKEN lptkNext;               // Ptr to next token to check

    // Split cases based upon the current token type
    switch (lpplLocalVars->lptkNext->tkFlags.TknType)
    {
        case TKT_OP1IMMED:
            // First of all, the current token must be NULLOP
            if (lpplLocalVars->lptkNext->tkData.tkChar NE UTF16_CIRCLEMIDDLEDOT)
                return FALSE;
            break;

        case TKT_OP1NAMED:
            Assert (lpplLocalVars->lptkNext->tkData.tkSym->stFlags.Imm);

////        // First of all, the current token must be NULLOP
////        if (lpplLocalVars->lptkNext->tkData.tkSym->stData.stChar NE UTF16_CIRCLEMIDDLEDOT)
                return FALSE;

            DbgBrk ();      // ***FIXME*** -- Just because it's OP1NAMED, doesn't mean
                            //                it isn't about to be assigned some other
                            //                value or type

            break;

        defstop
            break;
    } // End SWITCH

    // Then, either the next token is OP3, ...
    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_OP3IMMED)
        lptkNext = &lpplLocalVars->lptkNext[-2];
    else
    // or the next four tokens are /[I] or /[A]
    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_RIGHTBRACKET
     && (lpplLocalVars->lptkNext[-2].tkFlags.TknType EQ TKT_VARIMMED
      || lpplLocalVars->lptkNext[-2].tkFlags.TknType EQ TKT_VARNAMED
      || lpplLocalVars->lptkNext[-2].tkFlags.TknType EQ TKT_VARARRAY)
     && lpplLocalVars->lptkNext[-3].tkFlags.TknType EQ TKT_LEFTBRACKET
     && lpplLocalVars->lptkNext[-4].tkFlags.TknType EQ TKT_OP3IMMED)
        lptkNext = &lpplLocalVars->lptkNext[-5];
    else
        return FALSE;

    if (lptkNext->tkFlags.TknType EQ TKT_LEFTPAREN
     || lptkNext->tkFlags.TknType EQ TKT_EOS
     || lptkNext->tkFlags.TknType EQ TKT_EOL
     || lptkNext->tkFlags.TknType EQ TKT_ASSIGN
     || lptkNext->tkFlags.TknType EQ TKT_VARIMMED
     || lptkNext->tkFlags.TknType EQ TKT_VARNAMED
     || lptkNext->tkFlags.TknType EQ TKT_VARARRAY)
    {
        // Change the first token from ambiguous operator to a function
        lptkNext[1].tkFlags.TknType = TKT_FCNIMMED;
        lptkNext[1].tkFlags.ImmType = IMMTYPE_PRIMFCN;

        return TRUE;
    } else
        return FALSE;
} // End CheckNullOp3


//***************************************************************************
//  $pl_yyerror
//
//  Error callback from Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- pl_yyerror"
#else
#define APPEND_NAME
#endif

void pl_yyerror                     // Called for Bison syntax error
    (LPPLLOCALVARS lpplLocalVars,   // Ptr to local plLocalVars
     LPCHAR        s)               // Ptr to error msg

{
    char szTemp[32];

#ifdef DEBUG
    DbgMsg (s);
#endif

    // Skip this if we're in lookahead
    //   but save the char index
    if (lpplLocalVars->bLookAhead)
    {
        if (lpplLocalVars->lptkEnd <= &lpplLocalVars->lptkNext[1])
            lpplLocalVars->tkLACharIndex = NEG1U;
        else
            lpplLocalVars->tkLACharIndex = lpplLocalVars->lptkNext[1].tkCharIndex;
        lpplLocalVars->ExitType = EXITTYPE_ERROR;
    } else
    // If we've not already encountered an error, ...
    if (lpplLocalVars->ExitType NE EXITTYPE_ERROR)
    {
        // Set the caret ptr and exit type
        if (lpplLocalVars->tkLACharIndex NE NEG1U)
            lpplLocalVars->tkErrorCharIndex = lpplLocalVars->tkLACharIndex;
        else
        if (lpplLocalVars->lptkEnd <= &lpplLocalVars->lptkNext[1])
            lpplLocalVars->tkErrorCharIndex = NEG1U;
        else
            lpplLocalVars->tkErrorCharIndex = lpplLocalVars->lptkNext[1].tkCharIndex;

        lpplLocalVars->ExitType = EXITTYPE_ERROR;

        // Check for SYNTAX ERROR
#define ERR     "syntax error"
        lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
            ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);
#undef  ERR
        else
#define ERR     "memory exhausted"
        lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
            ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);
#undef  ERR
    } // End IF/ELSE/IF
} // End pl_yyerror
#undef  APPEND_NAME


//***************************************************************************
//  $pl_yyfprintf
//
//  Debugger output
//***************************************************************************

void pl_yyfprintf
    (FILE  *hfile,          // Ignore this
     LPCHAR lpszFmt,        // Format string
     ...)                   // Zero or more arguments

{
#if (defined (DEBUG)) && (defined (YYFPRINTF_DEBUG))
    va_list vl;
    int     i1,
            i2,
            i3;
    static  char szTemp[256] = {'\0'};

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
    if (szTemp[i1 - 1] EQ AC_LF)
    {
        szTemp[i1 - 1] = AC_EOS;    // Remove trailing LF
                                    //   because we're displaying
                                    //   in a GUI.
        DbgMsg (szTemp);            // Display in my debugger window.

        szTemp[0] = AC_EOS;         // Restart the buffer
    } // End IF
#endif
} // End pl_yyfprintf


#if (defined (DEBUG) & defined (YYPRINT))
//***************************************************************************
//  $pl_yyprint
//
//  Debugger output
//***************************************************************************

void pl_yyprint
    (FILE            *yyoutput,
     yytype_uint16    yytoknum,
     PL_YYSTYPE const yyvaluep)

{
    WCHAR wszTemp[32],
         *lpwsz;
    char *lpszFmt1 = "TKT_%s",
         *lpszFmt2 = "TKT_%s:  %S";
    char *lpszFmt;

    // Split cases based upon the token type
    switch (yyvaluep.tkToken.tkFlags.TknType)
    {
        case TKT_VARIMMED:
            lpwsz =
            FormatImmed (wszTemp,
                         yyvaluep.tkToken.tkFlags.ImmType,
         (LPAPLLONGEST) &yyvaluep.tkToken.tkData.tkLongest);
            lpwsz[-1] = WC_EOS;
            lpszFmt = lpszFmt2;

            break;

        default:
            wszTemp[0] = WC_EOS;
            lpszFmt = lpszFmt1;

            break;
    } // End SWITCH

    pl_yyfprintf (NULL,
                  lpszFmt,
                  GetTokenTypeName (yyvaluep.tkToken.tkFlags.TknType),
                  wszTemp);
} // End pl_yyprint
#endif


//***************************************************************************
//  $WaitForInput
//
//  Wait for either Quad or Quote-quad input
//***************************************************************************

LPPL_YYSTYPE WaitForInput
    (HWND    hWndSM,                // Window handle to Session Manager
     UBOOL   bQuoteQuad,            // TRUE iff Quote-Quad input (FALSE if Quad input)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    HANDLE         hSemaphore;      // Semaphore handle
    LPPL_YYSTYPE   lpYYRes;         // Ptr to the result
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory
    UINT           uLinePos,        // Char position of start of line
                   uCharPos;        // Current char position
    HWND           hWndEC;          // Edit Ctrl window handle

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Create a semaphore
    hSemaphore =
      CreateSemaphoreW (NULL,           // No security attrs
                        0,              // Initial count (non-signalled)
                        64*1024,        // Maximum count
                        NULL);          // No name
    // Fill in the SIS header for Quote-Quad/Quad Input Mode
    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                hSemaphore,             // Semaphore handle
                bQuoteQuad ? DFNTYPE_QQUAD: DFNTYPE_QUAD, // DfnType
                FCNVALENCE_NIL,         // FcnValence
                TRUE,                   // Suspended
                TRUE,                   // Restartable
                TRUE);                  // LinkIntoChain
    // Save a ptr to the function token
    lpMemPTD->lpSISCur->lptkFunc = lptkFunc;

    // Get the Edit Ctrl window handle
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWndEC);

    // Get the position of the start of the line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, NEG1U, 0);

    // Save the Quote-Quad input prompt length
    lpMemPTD->lpSISCur->QQPromptLen = uCharPos - uLinePos;

    // Tell the Session Manager to display the appropriate prompt
    PostMessageW (hWndSM, MYWM_QUOTEQUAD, bQuoteQuad, 14);
#ifdef DEBUG
    dprintfWL9 (L"~~WaitForSingleObject (ENTRY):  %s (%S#%d)", L"WaitForInput", FNLN);
#endif
    // Wait for the semaphore to trigger
    WaitForSingleObject (hSemaphore,    // Handle to wait for
                         INFINITE);     // Timeout value in milliseconds
#ifdef DEBUG
    dprintfWL9 (L"~~WaitForSingleObject (EXIT):   %s (%S#%d)", L"WaitForInput", FNLN);
#endif
    // Close the semaphore handle as it is no longer needed
    CloseHandle (hSemaphore); hSemaphore = NULL;

    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    UnlocalizeSTEs ();

    // If we're resetting, ...
    if (lpMemPTD->lpSISCur
     && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
        lpYYRes = NULL;
    else
    // If there's no result from the expression, ...
    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType EQ 0)
        // Make a PL_YYSTYPE NoValue entry
        lpYYRes = MakeNoValue_YY (lptkFunc);
    else
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Copy the result
        *lpYYRes = lpMemPTD->YYResExec;
    } // End IF/ELSE

    // We no longer need these values
    ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));

    return lpYYRes;
} // End WaitForInput


//***************************************************************************
//  $AmbOpSwap_EM
//
// Change the first token from ambiguous operator to a function
//   and swap the first two tokens
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AmbOpSwap_EM"
#else
#define APPEND_NAME
#endif

UBOOL AmbOpSwap_EM
    (LPPL_YYSTYPE lpYYFcn)              // Ptr to function strand

{
    DbgMsgW2 (L"==Entering AmbOpSwap");

    // Change the first token in the function strand
    //   from ambiguous operator to a function
    AmbOpToFcn (lpYYFcn);

    // Split cases based upon the token count
    switch (YYCountFcnStr (lpYYFcn->lpYYFcnBase))
    {
        case 1:                             // Single token already converted to a function
            break;

        case 2:
            // If there's an axis operator in the function strand, ...
            if (YYIsFcnStrAxis (lpYYFcn->lpYYFcnBase))
                break;                      //   we're done

            // Fall through to multiple element case

        case 3:
            // ***FIXME*** -- Multiple element function strand -- modify internal RPN structure

            ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                      &lpYYFcn->tkToken);
            return FALSE;

        defstop
            break;
    } // End SWITCH

    return TRUE;
} // End AmbOpSwap_EM
#undef  APPEND_NAME


//***************************************************************************
//  $AmbOpToFcn
//
// Change the first token in the function strand
//   from ambiguous operator to a function
//***************************************************************************

void AmbOpToFcn
    (LPPL_YYSTYPE lpYYFcn)              // Ptr to function strand

{
    HGLOBAL      hGlbFcn;               // Function array global memory handle
    LPPL_YYSTYPE lpMemFcn;              // Ptr to function array global memory

    DbgMsgW2 (L"==Entering AmbOpToFcn");

    // Split cases based upon token type
    switch (lpYYFcn->tkToken.tkFlags.TknType)
    {
        case TKT_OP3IMMED:
            // Change the first token from ambiguous operator to an immediate function
            lpYYFcn->tkToken.tkFlags.TknType = TKT_FCNIMMED;
            lpYYFcn->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
            if (lpYYFcn->lpYYFcnBase)
            {
                lpYYFcn->lpYYFcnBase->tkToken.tkFlags.TknType = TKT_FCNIMMED;
                lpYYFcn->lpYYFcnBase->tkToken.tkFlags.ImmType = IMMTYPE_PRIMFCN;
            } // End IF

            break;

        case TKT_OP3NAMED:
            // Change the first token from ambiguous operator to a named function
            lpYYFcn->tkToken.tkFlags.TknType = TKT_FCNNAMED;
            if (lpYYFcn->lpYYFcnBase)
                lpYYFcn->lpYYFcnBase->tkToken.tkFlags.TknType = TKT_FCNNAMED;

            // If the STE is an immediate, ...
            if (lpYYFcn->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Change the STE from ambiguous operator to an immediate function
                lpYYFcn->tkToken.tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMFCN;
                if (lpYYFcn->lpYYFcnBase)
                    lpYYFcn->lpYYFcnBase->tkToken.tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMFCN;
            } // End IF

            break;

        case TKT_FCNNAMED:
            // If it's a global, ...
            if (!lpYYFcn->tkToken.tkData.tkSym->stFlags.Imm)
            {
                DbgBrk ();          // ***FIXME*** -- Can this ever occur?






            } // End IF

            break;

        case TKT_FCNARRAY:
            // Get the global and clear the type bits
            hGlbFcn = ClrPtrTypeDir (lpYYFcn->tkToken.tkData.tkGlbData);

            // Lock the memory to get a ptr to it
            lpMemFcn = MyGlobalLock (hGlbFcn);

            // Skip over the function header to the data
            lpMemFcn = FcnArrayBaseToData (lpMemFcn);

            // If it's an immediate ambiguous operator, ...
            if (lpMemFcn->tkToken.tkFlags.TknType EQ TKT_OP3IMMED)
                // Change it to be an immediate function
                lpMemFcn->tkToken.tkFlags.TknType = TKT_FCNIMMED;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;

            break;

        case TKT_FCNIMMED:
            break;

        defstop
            break;
    } // End SWITCH
} // End AmbOpToFcn


//***************************************************************************
//  $AmbOpToOp1
//
// Change the first token in the function strand
//   from ambiguous operator to a monadic operator
//***************************************************************************

void AmbOpToOp1
    (LPPL_YYSTYPE lpYYFcn)              // Ptr to function strand

{
    Assert (lpYYFcn->tkToken.tkFlags.TknType EQ TKT_OP3IMMED
         || lpYYFcn->tkToken.tkFlags.TknType EQ TKT_OP3NAMED);

    if (lpYYFcn->tkToken.tkFlags.TknType EQ TKT_OP3IMMED)
    {
        // Change the first token from ambiguous operator to an immediate monadic operator
        lpYYFcn->tkToken.tkFlags.TknType = TKT_OP1IMMED;
        lpYYFcn->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP1;
        if (lpYYFcn->lpYYFcnBase)
        {
            lpYYFcn->lpYYFcnBase->tkToken.tkFlags.TknType = TKT_OP1IMMED;
            lpYYFcn->lpYYFcnBase->tkToken.tkFlags.ImmType = IMMTYPE_PRIMOP1;
        } // End IF
    } else
    {
        // Change the first token from ambiguous operator to a named monadic operator
        lpYYFcn->tkToken.tkFlags.TknType = TKT_OP1NAMED;
        if (lpYYFcn->lpYYFcnBase)
            lpYYFcn->lpYYFcnBase->tkToken.tkFlags.TknType = TKT_OP1NAMED;

        // If the STE is an immediate, ...
        if (lpYYFcn->tkToken.tkData.tkSym->stFlags.Imm)
        {
            // Change the STE from ambiguous operator to an immediate monadic operator
            lpYYFcn->tkToken.tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMOP1;
            if (lpYYFcn->lpYYFcnBase)
                lpYYFcn->lpYYFcnBase->tkToken.tkData.tkSym->stFlags.ImmType = IMMTYPE_PRIMOP1;
        } // End IF
    } // End IF/ELSE
} // End AmbOpToOp1


//***************************************************************************
//  $ArrExprCheckCaller
//
//  Tuck away the result from Execute/Quad.
//***************************************************************************

void ArrExprCheckCaller
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     LPSIS_HEADER  lpSISCur,            // Ptr to current SIS header
     LPPL_YYSTYPE  lpYYArg)             // Ptr to ArrExpr

{
    LPPERTABDATA  lpMemPTD;             // Ptr to PerTabData global memory

    // Mark as not in error
    lpplLocalVars->bRet = TRUE;

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // If it's NoValue, ...
    if (IsTokenNoValue (&lpYYArg->tkToken))
        // Copy the result
        lpplLocalVars->lpYYRes = lpYYArg;
    else
        // Copy the result
        lpplLocalVars->lpYYRes = CopyPL_YYSTYPE_EM_YY (lpYYArg, FALSE);

    // If the Execute/Quad result is present, display it
    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType)
        lpplLocalVars->bRet =
          ArrayDisplay_EM (&lpMemPTD->YYResExec.tkToken, TRUE, &lpplLocalVars->bCtrlBreak);

    // Save the Execute/Quad result
    lpMemPTD->YYResExec = *lpplLocalVars->lpYYRes;

    // If it's not NoValue, ...
    if (!IsTokenNoValue (&lpYYArg->tkToken))
    {
        YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
    } // End IF
} // End ArrExprCheckCaller


//***************************************************************************
//  $CheckSelSpec_EM
//
//  Check on Selective Specification of the first (and only) name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckSelSpec_EM"
#else
#define APPEND_NAME
#endif

UBOOL CheckSelSpec_EM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     LPTOKEN       lptkNameVars)        // Ptr to NameVars token

{
    HGLOBAL hGlbNameVars;               // NameVars global memory handle
    LPTOKEN lpMemNameVars;              // Ptr to NameVars global memory
    UBOOL   bRet;                       // TRUE iff the result is valid
    UBOOL   bSelSpec = FALSE;           // TRUE iff Selective Specification is in error ***DEBUG***
    UINT    uCharIndex;                 // Char index

    // If it's Selective Specification, and
    //   we have yet to see the first name, ...
    if (lpplLocalVars->bSelSpec && lpplLocalVars->bIniSpec)
    {
        // Clear the flag
        lpplLocalVars->bIniSpec = FALSE;

        // Get the global memory handle
        hGlbNameVars = lptkNameVars->tkData.tkGlbData;

        Assert (IsGlbTypeNamDir_PTB (hGlbNameVars));

        // Clear the type bits
        hGlbNameVars = ClrPtrTypeDir (lptkNameVars->tkData.tkGlbData);

        // Lock the memory to get a ptr to it
        lpMemNameVars = MyGlobalLock (hGlbNameVars);

        // Ensure there is only one name
        bRet = (((LPVARNAMED_HEADER) lpMemNameVars)->NELM EQ 1);
        if (bRet)
        {
            LPPL_YYSTYPE lpYYRes,       // Ptr to result
                         lpYYRes2;      // ...

            // Ensure the SelectSpec is properly marked
            bRet = IsSelectSpec (lptkNameVars);
            bSelSpec = !bRet;
            if (bRet)
            {
                // Skip over the header to the data
                lpMemNameVars = VarNamedBaseToData (lpMemNameVars);

                // Save the token
                lpplLocalVars->tkSelSpec = *lpMemNameVars;

                // Change the one (and only) token to an APA
                //   which maps the indices of the array
                lpYYRes2 =
                  PrimFnMonRho_EM_YY (&lpplLocalVars->tkSelSpec,
                                      &lpplLocalVars->tkSelSpec,
                                       NULL);
                if (lpYYRes2)
                {
                    lpYYRes =
                      PrimFnMonIotaVector_EM_YY (&lpplLocalVars->tkSelSpec, // Ptr to function token
                                                 &lpYYRes2->tkToken,        // Ptr to right arg token
                                                  NULL);                    // Ptr to axis token (may be NULL)
                    FreeYYFcn1 (lpYYRes2); lpYYRes2 = NULL;

                    if (lpYYRes)
                    {
                        LPVARARRAY_HEADER lpMemData;

                        // Save the char index
                        uCharIndex = lptkNameVars->tkCharIndex;

                        // Lock the memory to get a ptr to it
                        lpMemData = MyGlobalLock (ClrPtrTypeDir (lpYYRes->tkToken.tkData.tkGlbData));

                        // Mark as a Selective Specification array
                        lpMemData->bSelSpec = TRUE;

                        // We no longer need this ptr
                        MyGlobalUnlock (ClrPtrTypeDir (lpYYRes->tkToken.tkData.tkGlbData)); lpMemData = NULL;

                        // Fill in the result token
                        ZeroMemory (lptkNameVars, sizeof (lptkNameVars[0]));
                        lptkNameVars->tkFlags.TknType   = TKT_VARARRAY;
////////////////////////lptkNameVars->tkFlags.ImmType   = IMMTYPE_ERROR;    // Already zero from ZeroMemory
////////////////////////lptkNameVars->tkFlags.NoDisplay = FALSE;            // Already zero from ZeroMemory
                        lptkNameVars->tkData.tkGlbData  = lpYYRes->tkToken.tkData.tkGlbData;
                        lptkNameVars->tkCharIndex       = uCharIndex;

                        YYFree (lpYYRes); lpYYRes = NULL;
                    } else
                        // Mark as unsuccessful
                        bRet = FALSE;
                } else
                    // Mark as unsuccessful
                    bRet = FALSE;
            } // End IF
        } // End IF

        if (!bRet)
        {
            if (!(CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR))
                ErrorMessageIndirectToken (bSelSpec ? L"SelSpec ERROR" APPEND_NAME          // ***DEBUG***
                                                    : ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                           lptkNameVars);
            lpplLocalVars->ExitType = EXITTYPE_ERROR;
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbNameVars); lpMemNameVars = NULL;

        return bRet;
    } else
        return TRUE;

    return FALSE;
} // End CheckSelSpec_EM
#undef  APPEND_NAME


//***************************************************************************
//  $IsSelectSpec
//
//  Determine whether or not the arg is a Select Specification array
//***************************************************************************

UBOOL IsSelectSpec
    (LPTOKEN lptkArg)               // Ptr to arg token

{
    UBOOL bRet = FALSE;             // The result

    // If the arg is not immediate, ...
    if (lptkArg->tkFlags.TknType EQ TKT_VARARRAY)
    {
        HGLOBAL           hGlbArg;
        LPVARARRAY_HEADER lpMemHdr;

        // Get the global memory handle
        hGlbArg = ClrPtrTypeDir (lptkArg->tkData.tkGlbData);

        // Lock the memory to get a ptr to it
        lpMemHdr = MyGlobalLock (hGlbArg);

        // Get the Selective Specification array bit
        bRet = lpMemHdr->bSelSpec;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbArg); lpMemHdr = NULL;
    } // End IF

    return bRet;
} // End IsSelectSpec


//***************************************************************************
//  $MakeTempCopy
//
//  Make a temporary copy of a named var
//***************************************************************************

PL_YYSTYPE MakeTempCopy
    (LPPL_YYSTYPE lpYYTmp)              // Ptr to incoming var

{
    static PL_YYSTYPE YYTmp;

    // If the token is not a named var, pass it on as the result
    if (lpYYTmp->tkToken.tkFlags.TknType NE TKT_VARNAMED)
        return *lpYYTmp;

    // Copy the non-TOKEN fields
    YYTmp = *lpYYTmp;

    // If the named var is immediate, ...
    if (lpYYTmp->tkToken.tkData.tkSym->stFlags.Imm)
    {
        // Fill in the result token
        YYTmp.tkToken.tkFlags.TknType   = TKT_VARIMMED;
        YYTmp.tkToken.tkFlags.ImmType   = lpYYTmp->tkToken.tkData.tkSym->stFlags.ImmType;
        YYTmp.tkToken.tkFlags.NoDisplay = FALSE;
        YYTmp.tkToken.tkData.tkLongest  = lpYYTmp->tkToken.tkData.tkSym->stData.stLongest;
////////YYTmp.tkToken.tkCharIndex       =                   // Set by above YYTmp =
    } else
    {
        // Fill in the result token
        YYTmp.tkToken.tkFlags.TknType   = TKT_VARARRAY;
        YYTmp.tkToken.tkFlags.ImmType   = IMMTYPE_ERROR;
        YYTmp.tkToken.tkFlags.NoDisplay = FALSE;
        YYTmp.tkToken.tkData.tkGlbData  = lpYYTmp->tkToken.tkData.tkSym->stData.stGlbData;
////////YYTmp.tkToken.tkCharIndex       =                   // Set by above YYTmp =
    } // End IF/ELSE

    return YYTmp;
} // End MakeTempCopy


//***************************************************************************
//  $GetEOSIndex
//
//  Return the index of the EOS stmt for a char index
//***************************************************************************

UINT GetEOSIndex
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     int           tkCharIndex)         // Character index of array to display/free

{
    UINT uCnt,                          // Loop counter
         uLen;                          // # tokens in the stmt

    // Loop through the tokens looking for this stmt's char index
    for (uCnt = 0; uCnt < lpplLocalVars->uTokenCnt; uCnt += uLen)
    {
        Assert (lpplLocalVars->lptkStart[uCnt].tkFlags.TknType EQ TKT_EOL
             || lpplLocalVars->lptkStart[uCnt].tkFlags.TknType EQ TKT_EOS);

        // Get the # tokens in this stmt
        uLen = lpplLocalVars->lptkStart[uCnt].tkData.tkIndex;

        Assert (lpplLocalVars->lptkStart[uCnt + uLen - 1].tkFlags.TknType EQ TKT_SOS);

        if (lpplLocalVars->lptkStart[uCnt].tkCharIndex < tkCharIndex
         &&                                              tkCharIndex < lpplLocalVars->lptkStart[uCnt + uLen - 1].tkCharIndex)
            break;
    } // End FOR

    // If we're still within the line, ...
    if (uCnt < lpplLocalVars->uTokenCnt)
        return uCnt;
    else
        return NEG1U;
} // End GetEOSIndex


//***************************************************************************
//  $IsLastStmt
//
//  Return TRUE iff the current stmt is the last one on the line
//  We need this to determine whether or not to print or return a result
//    from within an Execute or Quad stmt.
//***************************************************************************

UBOOL IsLastStmt
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local vars
     int           tkCharIndex)         // Character index of array to display/free

{
    UINT uIndex;                        // Index of the corresponding EOS stmt

    // Get the index of the EOS stmt
    uIndex = GetEOSIndex (lpplLocalVars, tkCharIndex);

    // If we're still within the line, ...
    if (uIndex NE NEG1U)
        return (lpplLocalVars->uTokenCnt EQ (uIndex + lpplLocalVars->lptkStart[uIndex].tkData.tkIndex));
    else
        return TRUE;
} // End IsLastStmt


//***************************************************************************
//  End of File: pl_parse.y
//***************************************************************************
