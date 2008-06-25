//***************************************************************************
//  NARS2000 -- Parser Grammar for executable lines
//***************************************************************************

/****************************************************************************

Parse a line of pre-tokenized tokens.

Based upon "The Syntax of APL:  An Old Approach Revisited" by
Jean Jacques Giardot & Florence Rollin, ACM SIGAPL Quote-Quad APL 1987
heavily modified to work as an LALR grammar with the lookahead embedded
in the lexical analyser (pl_yylex).

****************************************************************************/

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
#include <string.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "dfnhdr.h"
#include "pertab.h"
#include "sis.h"
#include "threads.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

#define   DIRECT    FALSE           // Flags for PushFcnSTrand
#define INDIRECT    TRUE            // ...

////#define YYLEX_DEBUG
////#define YYFPRINTF_DEBUG

#ifdef DEBUG
////#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               pl_yyfprintf
#define fprintf                 pl_yyfprintf
////#define YYPRINT                 pl_yyprint
void pl_yyprint (FILE *yyoutput, unsigned short int yytoknum, PL_YYSTYPE const yyvaluep);
#endif

#define    YYSTYPE     PL_YYSTYPE
#define  LPYYSTYPE   LPPL_YYSTYPE
#define tagYYSTYPE  tagPL_YYSTYPE

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         pl_yy_symbol_print
#define yy_symbol_value_print   pl_yy_symbol_value_print
#define yy_reduce_print         pl_yy_reduce_print
#define yydestruct              pl_yydestruct

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

%token NAMEVAR NAMEUNK CONSTANT STRING USRFN0 SYSFN0 QUAD QUOTEQUAD SYSLBL
%token UNK EOL

/*  Note that as we parse APL from right to left, these rules
    are all reversed as well as token associativity (long scope).
    That is, functions in APL have long right scope, so they are
    declared here in the %left list.  Similarly, operators in APL
    have long left scope, so they are declared in the %right list.
 */
%right DIAMOND
%left  ASSIGN PRIMFCN NAMEFCN SYSFN12 GOTO
%right NULLOP NAMEOP1 OP1 NAMEOP2 OP2 NAMEOP3 OP3 JOTDOT OP3ASSIGN NAMEOP3ASSIGN

%start StmtMult

%%

/*  ToDo
    ----
    * Control structures
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
      // All errors propagate up to this point where we ABORT -- this ensures
      //   that the call to pl_yyparse terminates with a non-zero error code.
      error                             {DbgMsgWP (L"%%StmtMult:  error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             Assert (YYResIsEmpty ());

                                             // Lock the memory to get a ptr to it
                                             lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

                                             // If we're resetting, ...
                                             if (lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
                                                 lpplLocalVars->ExitType = TranslateResetFlagToExitType (lpMemPTD->lpSISCur->ResetFlag);

                                             // We no longer need this ptr
                                             MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

                                             // Set the exit type to error unless already set
                                             if (lpplLocalVars->ExitType EQ EXITTYPE_NONE)
                                                 lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYABORT;
                                         } else
                                             YYABORT;
                                        }
    |                  StmtSing         {DbgMsgWP (L"%%StmtMult:  StmtSing");
                                         if (lpplLocalVars->bCtrlBreak)
                                             YYERROR;
                                         else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             Assert (YYResIsEmpty ());
                                         } // End IF
                                        }
    | StmtMult DIAMOND StmtSing         {DbgMsgWP (L"%%StmtMult:  StmtSing " WS_UTF16_DIAMOND L" StmtMult");
                                         if (lpplLocalVars->bCtrlBreak)
                                             YYERROR;
                                         else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             Assert (YYResIsEmpty ());  // Likely not TRUE with non-empty SI
                                         } // End IF
                                        }
    ;

// Single statement
StmtSing:
      /* Empty */                       {DbgMsgWP (L"%%StmtSing:  <empty>");
                                         if (!lpplLocalVars->bLookAhead)
                                             lpplLocalVars->ExitType = EXITTYPE_NOVALUE;
                                        }
////| error                             //--Conflicts
    | ArrExpr                           {DbgMsgWP (L"%%StmtSing:  ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
                                             LPSIS_HEADER lpSISCur;          // Ptr to current SIS header

                                             // Lock the memory to get a ptr to it
                                             lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

                                             // Get the ptr to the current SIS header
                                             lpSISCur = lpMemPTD->lpSISCur;

                                             // We no longer need this ptr
                                             MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

                                             // Do not display if caller is Execute or Quad
                                             if (lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || (lpSISCur->DfnType EQ DFNTYPE_IMM
                                               && lpSISCur->lpSISPrv NE NULL
                                               && (lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_EXEC
                                                || lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_QUAD)))
                                             {
                                                 // Mark as not in error
                                                 lpplLocalVars->bRet = TRUE;

                                                 // If it's NoValue on Quad input, ...
                                                 if (IsTokenNoValue (&$1.tkToken)
                                                  && lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_QUAD)
                                                 {
                                                     // Check for NoValue
                                                     if (IsSymNoValue ($1.tkToken.tkData.tkSym))
                                                     {
                                                         // Signal a VALUE ERROR
                                                         PrimFnValueError_EM (&$1.tkToken);

                                                         // Mark as in error
                                                         lpplLocalVars->bRet = FALSE;
                                                         lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                                     } // End IF
                                                 } else
                                                 {
                                                     // Lock the memory to get a ptr to it
                                                     lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

                                                     // Copy the result
                                                     lpplLocalVars->lpYYRes = CopyPL_YYSTYPE_EM_YY (&$1, FALSE);
                                                     lpMemPTD->YYResExec = *lpplLocalVars->lpYYRes;
                                                     YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                                     // We no longer need this ptr
                                                     MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;
                                                 } // End IF/ELSE
                                             } else
                                                 lpplLocalVars->bRet =
                                                   ArrayDisplay_EM (&$1.tkToken, TRUE, &lpplLocalVars->bCtrlBreak);

                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;
                                             // If the exit type isn't GOTO_LINE, mark it as already displayed
                                             if (lpplLocalVars->ExitType NE EXITTYPE_GOTO_LINE)
                                                 lpplLocalVars->ExitType = EXITTYPE_NODISPLAY;
                                         } // End IF/ELSE
                                        }
    | error   GOTO                      {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_RIGHTARROW L"error");
                                         if (lpplLocalVars->bCtrlBreak)
                                             YYERROR;
                                         else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr GOTO                      {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_RIGHTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             lpplLocalVars->ExitType = GotoLine_EM (&$1.tkToken, &$2.tkToken);
                                             FreeResult (&$1.tkToken);

                                             // Lock the memory to get a ptr to it
                                             lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

                                             // Make a PL_YYSTYPE NoValue entry
                                             lpplLocalVars->lpYYMak =
                                               MakeNoValue_YY (&$2.tkToken);
                                             lpMemPTD->YYResExec = *lpplLocalVars->lpYYMak; YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             // We no longer need this ptr
                                             MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

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
                                                         lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                                         YYERROR;
                                                     } else
                                                         YYACCEPT;          // Stop executing this line

                                                 case EXITTYPE_ERROR:       // Error
                                                     YYERROR;               // Stop on error

                                                 defstop
                                                     break;
                                             } // End SWITCH
                                         } // End IF/ELSE/IF
                                        }
    |         GOTO                      {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_RIGHTARROW);
                                         if (lpplLocalVars->bCtrlBreak || lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
                                             LPSIS_HEADER lpSISCur;          // Ptr to current SIS layer

                                             // If we're not at the EOS or EOL, YYERROR
                                             if (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOL
                                              && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOS)
                                             {
                                                 PrimFnSyntaxError_EM (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // Lock the memory to get a ptr to it
                                             lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

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
                                             } // End IF

                                             // We no longer need this ptr
                                             MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

                                             YYACCEPT;              // Stop executing this line
                                         } // End IF/ELSE
                                        }
    | error   ASSIGN                    {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_LEFTARROW L"error");
                                         if (lpplLocalVars->bCtrlBreak)
                                             YYERROR;
                                         else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN                    {DbgMsgWP (L"%%StmtSing:  " WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_NODISPLAY;
                                         } // End IF/ELSE/IF
                                        }
////     | ArrExpr NAMEOP3                   {DbgMsgWP (L"%%StmtSing:  NAMEOP3 ArrExpr");
////                                              DbgBrk ();
////
////                                              lpplLocalVars->lpYYMak =
////                                                MakeNameFcnOpr_YY (&$2);
//// /////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY
////
////                                              if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
////                                              {
////                                                  FreeResult (&$1.tkToken);
////                                                  FreeResult (&$2.tkToken);
////                                                  YYERROR;
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
////                                                  FreeResult (&$2.tkToken);
////                                                  YYERROR;
////                                              } // End IF
////
////                                              lpplLocalVars->lpYYFcn =
////                                                MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp3, NAMETYPE_FN12, FALSE);
////                                              YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
////
////                                              if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
////                                              {
////                                                  FreeResult (&$1.tkToken);
////                                                  FreeResult (&$2.tkToken);
////                                                  YYERROR;
////                                              } // End IF
////
////                                              lpplLocalVars->lpYYRes =
////                                                ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken);
////                                              FreeResult (&$1.tkToken);
////                                              FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
////
////                                              if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
////                                                  YYERROR;
////
////                                              $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
////                                         }
    | FcnSpec                           {DbgMsgWP (L"%%StmtSing:  FcnSpec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Lock the memory to get a ptr to it
                                             lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;
                                             // We no longer need this ptr
                                             MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

                                             $$ = $1;
                                         } // End IF/ELSE/IF
                                        }
    | Op1Spec                           {DbgMsgWP (L"%%StmtSing:  Op1Spec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Lock the memory to get a ptr to it
                                             lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;
                                             // We no longer need this ptr
                                             MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

                                             $$ = $1;
                                         }
                                        }
    | Op2Spec                           {DbgMsgWP (L"%%StmtSing:  Op2Spec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Lock the memory to get a ptr to it
                                             lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;
                                             // We no longer need this ptr
                                             MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

                                             $$ = $1;
                                         }
                                        }
    | Op3Spec                           {DbgMsgWP (L"%%StmtSing:  Op3Spec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

                                             // Lock the memory to get a ptr to it
                                             lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                 lpplLocalVars->ExitType = EXITTYPE_NOVALUE;
                                             // We no longer need this ptr
                                             MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

                                             $$ = $1;
                                         }
                                        }
// The following productions ending in EOL are for bLookAhead only
    |     error   EOL                   {DbgMsgWP (L"%%StmtSing:  EOL error");
                                         if (lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
////|     IndexListWE EOL               {DbgMsgWP (L"%%StmtSing:  EOL IndexListWE");
////                                     if (lpplLocalVars->bLookAhead)
////                                     {
////                                         lpplLocalVars->plNameType = NAMETYPE_LST;
////                                         YYACCEPT;
////                                     } else
////                                         YYERROR;
////                                    }
    |     ArrExpr EOL                   {DbgMsgWP (L"%%StmtSing:  EOL ArrExpr");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_VAR;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     FcnSpec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL FcnSpec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     Op1Spec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL Op1Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP1;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     Op1Spec LeftOper EOL          {DbgMsgWP (L"%%StmtSing:  EOL LeftOper Op1Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     Op2Spec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL Op2Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP2;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     Op3Spec EOL                   {DbgMsgWP (L"%%StmtSing:  EOL Op3Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP1;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     MonOp   EOL                   {DbgMsgWP (L"%%StmtSing:  EOL MonOp");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP1;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     DydOp   EOL                   {DbgMsgWP (L"%%StmtSing:  EOL DydOp");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP2;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     AmbOp   EOL                   {DbgMsgWP (L"%%StmtSing:  EOL AmbOp");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_OP3;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     LeftOper EOL                  {DbgMsgWP (L"%%StmtSing:  EOL LeftOper");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     AxisFunc EOL                  {DbgMsgWP (L"%%StmtSing:  EOL AxisFunc");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     Drv1Func EOL                  {DbgMsgWP (L"%%StmtSing:  EOL Drv1Func");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     Drv2Func EOL                  {DbgMsgWP (L"%%StmtSing:  EOL Drv2Func");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->plNameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
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
    | NAMEFCN                           {DbgMsgWP (L"%%NameAnyOpN:  NAMEFCN");
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
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| error    ASSIGN NameAnyOpN        //--Conflicts
    | Drv1Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv1Func");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| error    ASSIGN NameAnyOpN        //--Conflicts
    | Drv2Func ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv2Func");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| error    ASSIGN NameAnyOpN        //--Conflicts
    | AxisFunc ASSIGN NameAnyOpN        {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"AxisFunc");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | MonOp Drv1Func ASSIGN NameAnyOpN  {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv1Func MonOp");

                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp1, NAMETYPE_FN12, TRUE);
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | MonOp Drv2Func ASSIGN NameAnyOpN  {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv2Func MonOp");

                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp1, NAMETYPE_FN12, TRUE);
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | AmbOp Drv1Func ASSIGN NameAnyOpN  {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv1Func AmbOp");

                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp1, NAMETYPE_FN12, TRUE);
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | AmbOp Drv2Func ASSIGN NameAnyOpN  {DbgMsgWP (L"%%FcnSpec:  NameAnyOpN" WS_UTF16_LEFTARROW L"Drv2Func AmbOp");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp1, NAMETYPE_FN12, TRUE);
                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    ;

Op1Spec:
      MonOp    ASSIGN NameAnyOpN        {DbgMsgWP (L"%%Op1Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"MonOp");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP1, TRUE);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = $1; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
// The following assignment can be made to work if we switch the order of the arguments
//   to a dyadic operator in a function strand from D L R to D R L and make all corresponding
//   changes elsewhere such as whenever parsing a function strand.
////| RightOper DydOp ASSIGN NameAnyOpN {DbgMsgWP (L"%%Op1Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"DydOp RightOper");
////                                     if (lpplLocalVars->bCtrlBreak)
////                                     {
////                                         FreeResult (&$1.tkToken);
////                                         FreeResult (&$2.tkToken);
////                                         FreeResult (&$4.tkToken);               // Validation only
////                                         YYERROR;
////                                     } else
////                                     if (!lpplLocalVars->bLookAhead)
////                                     {
////                                         lpplLocalVars->lpYYOp2 =
////                                           PushFcnStrand_YY (&$2, 2, INDIRECT);  // Dyadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                         lpplLocalVars->lpYYRht =
////                                           PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
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
////                                             FreeResult (&$4.tkToken);           // Validation only
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpplLocalVars->lpYYFcn =
////                                           MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp2, NAMETYPE_OP1, TRUE);
////                                         FreeYYFcn1 (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
////                                         FreeYYFcn1 (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
////
////                                         if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$4.tkToken);           // Validation only
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpplLocalVars->bRet =
////                                           AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
////                                         FreeResult (&$4.tkToken);               // Validation only
////
////                                         if (!lpplLocalVars->bRet)
////                                         {
////                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
////                                             YYERROR;
////                                         } // End IF
////
////                                         // The result is always the root of the function tree
////                                         $$ = *lpplLocalVars->YYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
////                                         $$.tkToken.tkFlags.NoDisplay = TRUE;
////                                     } // End IF
////                                    }
    ;

// Dyadic operator specification
Op2Spec:
      DydOp    ASSIGN NameAnyOpN        {DbgMsgWP (L"%%Op2Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"DydOp");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP2, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    ;


// Ambiguous operator assignment
AmbOpAssign:
      OP3ASSIGN                         {DbgMsgWP (L"%%AmbOpAssign:  OP3ASSIGN");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimOp3_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT);  // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3; YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;

                                         } // End IF
                                        }
    | NAMEOP3ASSIGN                     {DbgMsgWP (L"%%AmbOpAssign:  NAMEOP3ASSIGN");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3; YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    | AmbOp                             {DbgMsgWP (L"%%AmbOpAssign:  (AmbOp)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Monadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3; YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    | AmbOpAssign OP3ASSIGN             {DbgMsgWP (L"%%AmbOpAssign:  OP3ASSIGN AmbOpAssign");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimOp3_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Ambiguous operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3; YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT);  // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    | AmbOpAssign NAMEOP3ASSIGN         {DbgMsgWP (L"%%AmbOpAssign:  NAMEOP3ASSIGN AmbOpAssign");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Ambiguous operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3; YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    ;


// Ambiguous operator specification
Op3Spec:
      AmbOpAssign   ASSIGN NameAnyOpN   {DbgMsgWP (L"%%Op3Spec:  NameAnyOpN" WS_UTF16_LEFTARROW L"AmbOp");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP3, TRUE);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = $1; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
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
    | error   LeftOper                  {DbgMsgWP (L"%%ArrExpr:  LeftOper error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| ArrExpr error                     //--Conflicts
    | ArrExpr LeftOper                  {DbgMsgWP (L"%%ArrExpr:  LeftOper ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | error   Drv1Func                  {DbgMsgWP (L"%%ArrExpr:  Drv1Func error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| ArrExpr error                     //--Conflicts
    | ArrExpr Drv1Func                  {DbgMsgWP (L"%%ArrExpr:  Drv1Func ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | error   AxisFunc                  {DbgMsgWP (L"%%ArrExpr:  AxisFunc error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| ArrExpr error                     //--Conflicts
    | ArrExpr AxisFunc                  {DbgMsgWP (L"%%ArrExpr:  AxisFunc ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr LeftOper error            {DbgMsgWP (L"%%ArrExpr:  error LeftOper ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | error   LeftOper StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst LeftOper error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| ArrExpr LeftOper error            //--Conflicts
////| ArrExpr error    StrandInst       //--Conflicts
    | ArrExpr LeftOper StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst LeftOper ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr AmbOp    error            {DbgMsgWP (L"%%ArrExpr:  error AmbOp ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | error   AmbOp StrandInst          {DbgMsgWP (L"%%ArrExpr:  StrandInst AmbOp error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr AmbOp NAMEOP3ASSIGN       {DbgMsgWP (L"%%ArrExpr:  OP3ASSIGN AmbOp ArrExpr");
                                         DbgBrk ();     // ***FIXME*** -- Can we ever get here??



                                        }
    | ArrExpr AmbOp OP3ASSIGN           {DbgMsgWP (L"%%ArrExpr:  OP3ASSIGN AmbOp ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$3);
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimOp3_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (lpplLocalVars->lpYYMak);

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (&$2, 2, INDIRECT);  // Ambiguous operator (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT);  // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (lpplLocalVars->lpYYOp3, NAMETYPE_FN12, FALSE);
                                             YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr AmbOp StrandInst          {DbgMsgWP (L"%%ArrExpr:  StrandInst AmbOp ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token from ambiguous operator to a function
                                             //   and swap the first two tokens
                                             if (!AmbOpSwap_EM (&$2))
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // Because MakeFcnStrand might have brought in a function strand from
                                             //   a global which still has TKT_OP3IMMED, change the token type to
                                             //   TKT_FCNIMMED.
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (lpplLocalVars->lpYYFcn);

                                             lpplLocalVars->lpYYRes =
                                               ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr AxisFunc error            {DbgMsgWP (L"%%ArrExpr:  error AxisFunc ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | error   AxisFunc StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst AxisFunc error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| ArrExpr AxisFunc error            //--Conflicts
////| ArrExpr error    StrandInst       //--Conflicts
    | ArrExpr AxisFunc StrandInst       {DbgMsgWP (L"%%ArrExpr:  StrandInst AxisFunc ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Single var (including single names)
SingVar:
                  NAMEUNK               {DbgMsgWP (L"%%SingVar:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$1.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
    |             QUAD                  {DbgMsgWP (L"%%SingVar:  QUAD");
                                         if (lpplLocalVars->bCtrlBreak || lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                         {
                                             lpplLocalVars->lpYYRes =
                                               WaitForInput (lpplLocalVars->hWndSM, FALSE, &$1.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |             QUOTEQUAD             {DbgMsgWP (L"%%SingVar:  QUOTEQUAD");
                                         if (lpplLocalVars->bCtrlBreak || lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                         {
                                             lpplLocalVars->lpYYRes =
                                               WaitForInput (lpplLocalVars->hWndSM, TRUE, &$1.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |             SingTokn              {DbgMsgWP (L"%%SingVar:  SingTokn");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |         ')' error   '('           {DbgMsgWP (L"%%SingVar:  error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    |         ')' ArrExpr '('           {DbgMsgWP (L"%%SingVar:  (ArrExpr)");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    ;

// Strand Recursion
StrandRec:
////  error                             //--Conflicts
      SingVar                           {DbgMsgWP (L"%%StrandRec:  SingVar -- InitVarStrand/PushVarStrand_YY");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             InitVarStrand (&$1);

                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushVarStrand_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | StrandRec   error                 {DbgMsgWP (L"%%StrandRec:  error StrandRec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStr =
                                               MakeVarStrand_EM_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYStr)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             FreeResult (&lpplLocalVars->lpYYStr->tkToken); YYFree (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;

                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | StrandRec   SingVar               {DbgMsgWP (L"%%StrandRec:  SingVar StrandRec -- PushVarStrand_YY");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushVarStrand_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushVarStrand_YY
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Strand instance
StrandInst:
////  error                             //--Conflicts
      StrandRec                         {DbgMsgWP (L"%%StrandInst:  StrandRec -- MakeVarStrand_EM_YY");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStr =
                                               MakeVarStrand_EM_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYStr)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYStr; YYFree (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;
                                         } // End IF
                                        }
    | IndexListBR     error             {DbgMsgWP (L"%%StrandInst:  error IndexListBR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | IndexListBR     StrandRec         {DbgMsgWP (L"%%StrandInst:  StrandRec IndexListBR");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStr =
                                               MakeVarStrand_EM_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYStr)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               ArrayIndexRef_EM_YY (&lpplLocalVars->lpYYStr->tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             YYFree (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | error           StrandRec         {DbgMsgWP (L"%%StrandInst:  StrandRec error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStr =
                                               MakeVarStrand_EM_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYStr)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             FreeResult (&lpplLocalVars->lpYYStr->tkToken); YYFree (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;

                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | CONSTANT   ':'                    {DbgMsgWP (L"%%StrandInst:  :CONSTANT");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             HGLOBAL            hGlbPTD,                 // PerTabData global memory handle
                                                                hGlbObj;                 // Object global memory handle
                                             LPPERTABDATA       lpMemPTD;                // Ptr to PerTabData global memory
                                             LPLOADWSGLBVARCONV lpLoadWsGlbVarConv;      // Ptr to function to convert a FMTSTR_GLBOBJ to an HGLOBAL
                                             LPLOADWSGLBVARPARM lpLoadWsGlbVarParm;      // Extra parms for LoadWsGlbVarConv

                                             // Get the PerTabData global memory handle
                                             hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

                                             // Lock the memory to get a ptr to it
                                             lpMemPTD = MyGlobalLock (hGlbPTD);

                                             // Get the LoadWsGlbVarXXX ptrs
                                             lpLoadWsGlbVarConv = lpMemPTD->lpLoadWsGlbVarConv;
                                             lpLoadWsGlbVarParm = lpMemPTD->lpLoadWsGlbVarParm;

                                             // We no longer need this ptr
                                             MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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
                                                 $$.tkToken.tkFlags.ImmType   = 0;
                                                 $$.tkToken.tkFlags.NoDisplay = FALSE;
                                                 $$.tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbObj);
                                             ////$$.tkToken.tkCharIndex       =         // Set in $$ = $1 above
                                             } else
                                             {
                                                 // Signal an error
                                                 ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                                                           &$2.tkToken);
                                                 YYERROR;
                                             } // End IF/ELSE
                                         } // End IF
                                        }
    ;

// Simple array expression
SimpExpr:
      error   ASSIGN       QUAD         {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN       QUAD         {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $1.tkToken.tkFlags.NoDisplay = FALSE;
                                             lpplLocalVars->bRet =
                                               ArrayDisplay_EM (&$1.tkToken, TRUE, &lpplLocalVars->bCtrlBreak);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN       QUOTEQUAD    {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN       QUOTEQUAD    {DbgMsgWP (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $1.tkToken.tkFlags.NoDisplay = FALSE;
                                             lpplLocalVars->bRet =
                                               ArrayDisplay_EM (&$1.tkToken, FALSE, &lpplLocalVars->bCtrlBreak);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN       NameAnyVar   {DbgMsgWP (L"%%SimpExpr:  NameAny" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN       NameAnyVar   {DbgMsgWP (L"%%SimpExpr:  NameAny" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN IndexListBR  NAMEVAR {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN IndexListBR NAMEVAR {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->bRet =
                                               ArrayIndexSet_EM (&$4.tkToken, &$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN error        NAMEVAR {DbgMsgWP (L"%%SimpExpr:  NAMEVAR error" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | error ASSIGN IndexListBR  NAMEUNK {DbgMsgWP (L"%%SimpExpr:  NAMEUNK IndexListBR" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN IndexListBR  NAMEUNK {DbgMsgWP (L"%%SimpExpr:  NAMEUNK IndexListBR" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$4.tkToken);

                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN error      NAMEUNK {DbgMsgWP (L"%%SimpExpr:  NAMEUNK error" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | error   ASSIGN   ')' SelectSpec '(' {DbgMsgWP (L"%%SimpExpr:  (SelectSpec)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' error    '(' {DbgMsgWP (L"%%SimpExpr:  (error)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' SelectSpec '('
                                        {DbgMsgWP (L"%%SimpExpr:  (SelectSpec)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->bRet =
                                               AssignSelectSpec_EM (&$4.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeResult (&$4.tkToken);

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN   ')' IndexListBR NAMEVAR '(' {DbgMsgWP (L"%%SimpExpr:  (NAMEVAR IndexListBR)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' IndexListBR NAMEVAR '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEVAR IndexListBR)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->bRet =
                                               ArrayIndexSet_EM (&$5.tkToken, &$4.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN   ')' error NAMEVAR '(' {DbgMsgWP (L"%%SimpExpr:  (NAMEVAR error)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | error   ASSIGN   ')' IndexListBR NAMEUNK '(' {DbgMsgWP (L"%%SimpExpr:  (NAMEUNK IndexListBR)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NAMEUNK IndexListBR)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$5.tkToken);

                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only

                                             YYERROR;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN   ')' error       NAMEUNK '(' {DbgMsgWP (L"%%SimpExpr:  (NAMEUNK error)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | error   ASSIGN LeftOper  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| ArrExpr ASSIGN error     NAMEVAR  //--Conflicts
    | ArrExpr ASSIGN LeftOper  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               ExecFunc_EM_YY (&$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // DO NOT FREE:  Used below

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
                                             FreeResult (&lpplLocalVars->lpYYRes->tkToken); YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN AxisFunc  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| ArrExpr ASSIGN error     NAMEVAR  //--Conflicts
    | ArrExpr ASSIGN AxisFunc  NAMEVAR  {DbgMsgWP (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               ExecFunc_EM_YY (&$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // DO NOT FREE:  Used below

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
                                             FreeResult (&lpplLocalVars->lpYYRes->tkToken); YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN LeftOper ')' NameVals '(' {DbgMsgWP (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN LeftOper ')' error '(' {DbgMsgWP (L"%%SimpExpr:  (error) LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN error    ')' NameVals '(' {DbgMsgWP (L"%%SimpExpr:  (NameVals) error" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$5.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN LeftOper ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN =
                                               MakeNameStrand_EM_YY (&$5);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameStrand_EM_YY

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYStrN            // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYFcn)
                                             {
                                                 if (lpplLocalVars->lpYYStrN)        // If defined, free it
                                                 {
                                                     FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYFcn)         // If defined, free it
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 } // End IF

                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               ModifyAssignNameVals_EM (&lpplLocalVars->lpYYStrN->tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN AxisFunc ')' NameVals '(' {DbgMsgWP (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN AxisFunc ')' error '(' {DbgMsgWP (L"%%SimpExpr:  (error) AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| ArrExpr ASSIGN error ')' NameVals '(' //--Conflicts
    | ArrExpr ASSIGN AxisFunc ')' NameVals '('
                                        {DbgMsgWP (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN =
                                               MakeNameStrand_EM_YY (&$5);
/////////////////////////////////////////////FreeResult (&$5.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameStrand_EM_YY

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYStrN            // If not defined, free args and YYERROR
                                              || !lpplLocalVars->lpYYFcn)
                                             {
                                                 if (lpplLocalVars->lpYYStrN)        // If defined, free it
                                                 {
                                                     FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                                 } // End IF

                                                 if (lpplLocalVars->lpYYFcn)         // If defined, free it
                                                 {
                                                     FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 } // End IF

                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               ModifyAssignNameVals_EM (&lpplLocalVars->lpYYStrN->tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN LeftOper IndexListBR NAMEVAR {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN error    IndexListBR NAMEVAR {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR error" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN LeftOper IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$4.tkToken);
                                                 FreeResult (&$5.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               ArrayIndexFcnSet_EM (&$5.tkToken, &$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN LeftOper error NAMEVAR {DbgMsgWP (L"%%SimpExpr:  NAMEVAR error LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN LeftOper error NAMEUNK {DbgMsgWP (L"%%SimpExpr:  NAMEUNK error LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | error   ASSIGN AxisFunc IndexListBR NAMEVAR {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| ArrExpr ASSIGN error    IndexListBR NAMEVAR -- Conflicts
    | ArrExpr ASSIGN AxisFunc IndexListBR NAMEVAR
                                        {DbgMsgWP (L"%%SimpExpr:  NAMEVAR IndexListBR AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$4.tkToken);
                                                 FreeResult (&$5.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet =
                                               ArrayIndexFcnSet_EM (&$5.tkToken, &$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN AxisFunc error NAMEVAR {DbgMsgWP (L"%%SimpExpr:  NAMEVAR error AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    ;

// Selective specification
SelectSpec:
      NameVars                          {DbgMsgWP (L"%%SelectSpec:  NameVars");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN =
                                               MakeNameStrand_EM_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameStrand_EM_YY

                                             if (!lpplLocalVars->lpYYStrN)           // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYStrN; YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                         } // End IF
                                        }
////| SelectSpec error                  //--Conflicts
    | SelectSpec LeftOper               {DbgMsgWP (L"%%SelectSpec:  LeftOper SelectSpec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             PrimFnNonceError_EM (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$1.tkToken);

                                             YYERROR;
                                         } // End IF
                                        }
////| SelectSpec error                  //--Conflicts
    | SelectSpec AxisFunc               {DbgMsgWP (L"%%SelectSpec:  AxisFunc SelectSpec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             PrimFnNonceError_EM (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$1.tkToken);

                                             YYERROR;
                                         } // End IF
                                        }
    | SelectSpec LeftOper error         {DbgMsgWP (L"%%SelectSpec:  error LeftOper SelectSpec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| SelectSpec error    StrandInst    //--Conflicts
    | SelectSpec LeftOper StrandInst    {DbgMsgWP (L"%%SelectSpec:  StrandInst LeftOper SelectSpec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             PrimFnNonceError_EM (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$1.tkToken);

                                             YYERROR;
                                         } // End IF
                                        }
    | SelectSpec AxisFunc error         {DbgMsgWP (L"%%SelectSpec:  error AxisFunc SelectSpec");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| SelectSpec error    StrandInst    //--Conflicts
    | SelectSpec AxisFunc StrandInst    {DbgMsgWP (L"%%SelectSpec:  StrandInst AxisFunc SelectSpec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             PrimFnNonceError_EM (&$1.tkToken);
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$1.tkToken);

                                             YYERROR;
                                         } // End IF
                                        }
    ;

// Named Variables Strand w/Value
// Skip Ctrl-Break checking here so the Name Strand processing isn't interrupted
NameVals:
      NAMEVAR                           {DbgMsgWP (L"%%NameVals:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             InitNameStrand (&$1);

                                             lpplLocalVars->lpYYRes = PushNameStrand_YY (&$1);
                                             FreeResult (&$1.tkToken);               // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NAMEUNK                           {DbgMsgWP (L"%%NameVals:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$1.tkToken);
                                             FreeResult (&$1.tkToken);               // Validation only

                                             YYERROR;
                                         } // End IF
                                        }
    | NameVals       NAMEVAR            {DbgMsgWP (L"%%NameVals:  NAMEVAR NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $2.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYRes = PushNameStrand_YY (&$2);
                                             FreeResult (&$2.tkToken);               // Validation only

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NameVals       NAMEUNK            {DbgMsgWP (L"%%NameVals:  NAMEUNK NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$2.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);               // Validation only

                                             YYERROR;
                                         } // End IF
                                        }
    |       ')' IndexListBR NAMEVAR '('
                                        {DbgMsgWP (L"%%NameVals:  (NAMEVAR IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $3.tkToken.tkFlags.TknType = TKT_VARNAMED;




                                             PrimFnNonceError_EM (&$2.tkToken);
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    |       ')' error       NAMEVAR '(' {DbgMsgWP (L"%%NameVals:  (NAMEVAR error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    |       ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%NameVals:  (NAMEUNK IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$2.tkToken);
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    |       ')' error       NAMEUNK '(' {DbgMsgWP (L"%%NameVals:  (NAMEUNK error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | NameVals ')' IndexListBR NAMEVAR '('
                                        {DbgMsgWP (L"%%NameVals:  (NAMEVAR IndexListBR) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;




                                             PrimFnNonceError_EM (&$3.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    | NameVals ')' error    NAMEVAR '(' {DbgMsgWP (L"%%NameVals:  (NAMEVAR error) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | NameVals ')' IndexListBR NAMEUNK '('
                                        {DbgMsgWP (L"%%NameVals:  (NAMEUNK IndexListBR) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$4.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only

                                             YYERROR;
                                         } // End IF
                                        }
    | NameVals ')' error    NAMEUNK '(' {DbgMsgWP (L"%%NameVals:  (NAMEUNK error) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } // End IF
                                        }
    ;

// Named Variables Strand with or w/o Value
// Skip Ctrl-Break checking here so the Name Strand processing isn't interrupted
NameVars:
      NAMEVAR                           {DbgMsgWP (L"%%NameVars:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             InitNameStrand (&$1);

                                             lpplLocalVars->lpYYRes = PushNameStrand_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NAMEUNK                           {DbgMsgWP (L"%%NameVars:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             InitNameStrand (&$1);

                                             lpplLocalVars->lpYYRes = PushNameStrand_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ')' IndexListBR NAMEVAR '('       {DbgMsgWP (L"%%NameVars:  (NAMEVAR IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $3.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             PrimFnNonceError_EM (&$3.tkToken);


                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    | ')' error       NAMEVAR '('       {DbgMsgWP (L"%%NameVars:  (NAMEVAR error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ')' IndexListBR NAMEUNK '('       {DbgMsgWP (L"%%NameVars:  (NAMEUNK IndexListBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$3.tkToken);
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    | ')' error       NAMEUNK '('       {DbgMsgWP (L"%%NameVars:  (NAMEUNK error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);               // Validation only
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | NameVars       NAMEVAR            {DbgMsgWP (L"%%NameVars:  NameVars NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $2.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpplLocalVars->lpYYRes = PushNameStrand_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | NameVars       NAMEUNK            {DbgMsgWP (L"%%NameVars:  NameVars NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes = PushNameStrand_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 1 (Valid in ArrExpr: and FcnSpec:)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv1Func:
      RightOper DydOp error             {DbgMsgWP (L"%%Drv1Func:  error DydOp RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | RightOper DydOp StrandInst        {DbgMsgWP (L"%%Drv1Func:  StrandInst DydOp RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                         } // End IF
                                        }
    |           MonOp error             {DbgMsgWP (L"%%Drv1Func:  error MonOp");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    |           MonOp StrandInst        {DbgMsgWP (L"%%Drv1Func:  StrandInst MonOp");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Left operand (Direct)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 2 (valid in FcnSpec: but not ArrExpr: because of Strand on the right)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv2Func:
      StrandInst DydOp error            {DbgMsgWP (L"%%Drv2Func:  error DydOp StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | error      DydOp LeftOper         {DbgMsgWP (L"%%Drv2Func:  LeftOper DydOp error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | StrandInst DydOp LeftOper         {DbgMsgWP (L"%%Drv2Func:  LeftOper DydOp StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                         } // End IF
                                        }
    | error      DydOp AxisFunc         {DbgMsgWP (L"%%Drv2Func:  AxisFunc DydOp error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | StrandInst DydOp AxisFunc         {DbgMsgWP (L"%%Drv2Func:  AxisFunc DydOp StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                         } // End IF
                                        }
    | error      DydOp StrandInst       {DbgMsgWP (L"%%Drv2Func:  StrandInst DydOp error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
////| StrandInst DydOp error            --See above
    | StrandInst DydOp StrandInst       {DbgMsgWP (L"%%Drv2Func:  StrandInst DydOp StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, DIRECT);    // Left operand (Direct)
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, DIRECT);    // Right operand (Direct)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                         } // End IF
                                        }
    ;

// Parenthesized function expression
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
ParenFunc:
////  '>' error    '('                  //--Conflicts
      '>' Drv2Func                 '('  {DbgMsgWP (L"%%ParenFunc:  (Drv2Func)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '>' LeftOper '('                  {DbgMsgWP (L"%%ParenFunc:  (LeftOper)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $$ = $2;
////                                              lpplLocalVars->lpYYFcn =
////                                                PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)
//// /////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                              if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
////                                              {
////                                                  FreeResult (&$2.tkToken);
////                                                  YYERROR;
////                                              } // End IF
////
////                                              // The result is always the root of the function tree
////                                              $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
////| '>' error    '('                  //--Conflicts
    | '>' AxisFunc '('                  {DbgMsgWP (L"%%ParenFunc:  (AxisFunc)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $$ = $2;
////                                              lpplLocalVars->lpYYFcn =
////                                                PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)
//// /////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                              if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
////                                                  YYERROR;
////
////                                              // The result is always the root of the function tree
////                                              $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '>' FcnSpec '('                   {DbgMsgWP (L"%%ParenFunc:  (FcnSpec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Function (Direct)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     '>' Op1Spec error    '('      {DbgMsgWP (L"%%ParenFunc:  (error Op1Spec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$2.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    |     '>' Op1Spec LeftOper '('      {DbgMsgWP (L"%%ParenFunc:  (LeftOper Op1Spec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$2, 2, DIRECT);    // Monadic operator (Direct)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    ;

// Left operand function
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
LeftOper:
          PRIMFCN                       {DbgMsgWP (L"%%LeftOper:  PRIMFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimFcn_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT);// Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN                       {DbgMsgWP (L"%%LeftOper:  NAMEFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT);// Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     SYSFN12                       {DbgMsgWP (L"%%LeftOper:  SYSFN12");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT);// Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     ParenFunc                     {DbgMsgWP (L"%%LeftOper:  ParenFunc");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $$ = $1;
////                                              lpplLocalVars->lpYYFcn =
////                                                PushFcnStrand_YY (&$1, 1, INDIRECT);  // Function (Indirect)
//// /////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                              if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
////                                                  YYERROR;
////
////                                              // The result is always the root of the function tree
////                                              $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    | '>' error                    '('  {DbgMsgWP (L"%%LeftOper:  (error)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '>' Drv1Func                 '('  {DbgMsgWP (L"%%LeftOper:  (Drv1Func)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $$ = $2;
////                                              lpplLocalVars->lpYYFcn =
////                                                PushFcnStrand_YY (&$2, 1, INDIRECT);  // Function (Indirect)
//// /////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                              if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
////                                                  YYERROR;
////
////                                              // The result is always the root of the function tree
////                                              $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     RightOper    JOTDOT           {DbgMsgWP (L"%%LeftOper:  " WS_UTF16_JOT L". RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$2, 2, DIRECT);    // Monadic operator (Direct)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                         } // End IF
                                        }
    | NULLOP OP3                        {DbgMsgWP (L"%%LeftOper:  OP3 NULLOP");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a function
                                             AmbOpToFcn (&$2);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, DIRECT);    // Monadic operator (Direct)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Left operand (Direct)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeYYFcn1 (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
////|                  MonOp error      //--Conflicts
    |                  MonOp LeftOper   {DbgMsgWP (L"%%LeftOper:  LeftOper MonOp");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                         } // End IF
                                        }
////|                  MonOp error      //--Conflicts
    |                  MonOp AxisFunc   {DbgMsgWP (L"%%LeftOper:  AxisFunc MonOp");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                         } // End IF
                                        }
////|                  AmbOp error      //--Conflicts
    |                  AmbOp LeftOper   {DbgMsgWP (L"%%LeftOper:  LeftOper AmbOp"); //***FIXME***
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Change the first token in the function strand
                                             //   from ambiguous operator to a monadic operator
                                             AmbOpToOp1 (&$1);

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$1, 2, INDIRECT);  // Monadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$2, 1, INDIRECT);  // Left operand (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;
                                         } // End IF
                                        }
////|     RightOper DydOp error         //--Conflicts
    |     RightOper DydOp LeftOper      {DbgMsgWP (L"%%LeftOper:  LeftOper DydOp RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                         } // End IF
                                        }
////|     RightOper DydOp error         //--Conflicts
    |     RightOper DydOp AxisFunc      {DbgMsgWP (L"%%LeftOper:  AxisFunc DydOp RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 3, INDIRECT);  // Dyadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;

                                             lpplLocalVars->lpYYLft =
                                               PushFcnStrand_YY (&$3, 1, INDIRECT);  // Left operand (Indirect)
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYLft)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYLft); lpplLocalVars->lpYYLft = NULL;

                                             lpplLocalVars->lpYYRht =
                                               PushFcnStrand_YY (&$1, 1, INDIRECT);  // Right operand (Indirect)
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYRht)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRht); lpplLocalVars->lpYYRht = NULL;
                                         } // End IF
                                        }
    ;

// Right operand function (short right scope)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
RightOper:
          PRIMFCN                       {DbgMsgWP (L"%%RightOper:  PRIMFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimFcn_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN                       {DbgMsgWP (L"%%RightOper:  NAMEFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     SYSFN12                       {DbgMsgWP (L"%%RightOper:  SYSFN12");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    |     ParenFunc                     {DbgMsgWP (L"%%RightOper:  ParenFunc");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $$ = $1;
////                                              lpplLocalVars->lpYYFcn =
////                                                PushFcnStrand_YY (&$1, 1, INDIRECT);  // Function (Indirect)
//// /////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                              if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
////                                              {
////                                                  FreeResult (&$1.tkToken);
////                                                  YYERROR;
////                                              } // End IF
////
////                                              // The result is always the root of the function tree
////                                              $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                         } // End IF
                                        }
    ;

// Axis function expression
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
AxisFunc:
      '}' error   '['  PRIMFCN          {DbgMsgWP (L"%%AxisFunc:  PRIMFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  PRIMFCN          {DbgMsgWP (L"%%AxisFunc:  PRIMFCN[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimFcn_YY (&$4);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimFcn_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYMak->tkToken); YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  NAMEFCN          {DbgMsgWP (L"%%AxisFunc:  NAMEFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  NAMEFCN          {DbgMsgWP (L"%%AxisFunc:  NAMEFCN[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$4);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYMak->tkToken); YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  SYSFN12          {DbgMsgWP (L"%%AxisFunc:  SYSFN12[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  SYSFN12          {DbgMsgWP (L"%%AxisFunc:  SYSFN12[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$4);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT); // Function (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYMak->tkToken); YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  ParenFunc        {DbgMsgWP (L"%%AxisFunc:  ParenFunc[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  ParenFunc        {DbgMsgWP (L"%%AxisFunc:  ParenFunc[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Function (Indirect)

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYFcn; YYFree (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYMak->tkToken); YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                         } // End IF
                                        }
    ;

// Ambiguous operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
AmbOp:
          OP3                           {DbgMsgWP (L"%%AmbOp:  OP3");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimOp3_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3; YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
    | NAMEOP3                           {DbgMsgWP (L"%%AmbOp:  NAMEOP3");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Ambiguous operator (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3; YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
                                         } // End IF
                                        }
////|              error                //--Conflicts
    |              AmbOpAxis            {DbgMsgWP (L"%%AmbOp:  AmbOpAxis");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
////                                          {
////                                              lpplLocalVars->lpYYOp3 =
////                                                PushFcnStrand_YY (&$2, 1, INDIRECT);  // Monadic operator (Indirect)
//// /////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                              if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
////                                                  YYERROR;
////
////                                              // The result is always the root of the function tree
////                                              $$ = *lpplLocalVars->lpYYOp3; YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
////                                          } // End IF
                                        }
    |              '>' AmbOp '('        {DbgMsgWP (L"%%AmbOp:  (AmbOp)");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
////                                          {
////                                              lpplLocalVars->lpYYOp3 =
////                                                PushFcnStrand_YY (&$2, 1, INDIRECT);  // Monadic operator (Indirect)
//// /////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                              if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
////                                                  YYERROR;
////
////                                              // The result is always the root of the function tree
////                                              $$ = *lpplLocalVars->lpYYOp3; YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;
////                                          } // End IF
                                        }
    | '>' Op3Spec '('                   {DbgMsgWP (L"%%AmbOp:  (Op3Spec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Ambiguous operator (Direct)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
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
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  AmbOp            {DbgMsgWP (L"%%AmbOpAxis:  AmbOp[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp3_YY (&$4);
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimOp3_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYOp3 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 2, DIRECT);  // Ambiguous operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp3)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp3; YYFree (lpplLocalVars->lpYYOp3); lpplLocalVars->lpYYOp3 = NULL;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYAxis =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYMak->tkToken); YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;
                                         } // End IF
                                        }
    ;

// Monadic operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
MonOp:
                       OP1              {DbgMsgWP (L"%%MonOp:  OP1");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp1_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimOp1_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |                  NULLOP           {DbgMsgWP (L"%%MonOp:  NULLOP");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp1_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimOp1_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Monadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
    |                  NAMEOP1          {DbgMsgWP (L"%%MonOp:  NAMEOP1");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Monadic operator (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                         } // End IF
                                        }
////|              error                //--Conflicts
    |              MonOpAxis            {DbgMsgWP (L"%%MonOp:  MonOpAxis");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |              '>' MonOp '('        {DbgMsgWP (L"%%MonOp:  (MonOp)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $$ = $2;
////                                              lpplLocalVars->lpYYOp1 =
////                                                PushFcnStrand_YY (&$2, 1, INDIRECT);  // Monadic operator (Indirect)
//// /////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                              if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
////                                                  YYERROR;
////
////                                              // The result is always the root of the function tree
////                                              $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
                                              } // End IF
                                        }
    |              '>' Op1Spec '('      {DbgMsgWP (L"%%MonOp:  (Op1Spec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Monadic operator (Direct)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;
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
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  MonOp            {DbgMsgWP (L"%%MonOpAxis:  MonOp[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Monadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp1; YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYMak->tkToken); YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Dyadic operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
DydOp:
                       OP2              {DbgMsgWP (L"%%DydOp:  OP2");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakePrimOp2_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakePrimOp2_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Dyadic operator (Direct)
                                             FreeResult (&lpplLocalVars->lpYYMak->tkToken); YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |                  NAMEOP2          {DbgMsgWP (L"%%DydOp:  NAMEOP2");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYMak =
                                               MakeNameFcnOpr_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by MakeNameFcnOpr_YY

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Dyadic operator (Direct)
                                             YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
////|                  error            //--Conflicts
    |                  DydOpAxis        {DbgMsgWP (L"%%DydOp:  DydOpAxis");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $$ = $1;
////                                              lpplLocalVars->lpYYOp2 =
////                                                PushFcnStrand_YY (&$1, 1, INDIRECT);  // Dyadic operator (Indirect)
//// /////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                              if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
////                                                  YYERROR;
////
////                                              // The result is always the root of the function tree
////                                              $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |              '#' DydOp '('        {DbgMsgWP (L"%%DydOp:  (DydOp)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $$ = $2;
////                                              lpplLocalVars->lpYYOp2 =
////                                                PushFcnStrand_YY (&$2, 1, INDIRECT);  // Dyadic operator (Indirect)
//// /////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY
////
////                                              if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
////                                                  YYERROR;
////
////                                              // The result is always the root of the function tree
////                                              $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
                                         } // End IF
                                        }
    |              '#' Op2Spec '('      {DbgMsgWP (L"%%DydOp:  (Op2Spec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$2, 1, DIRECT);    // Dyadic operator (Direct)
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;
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
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '?' ArrExpr '['  DydOp            {DbgMsgWP (L"%%DydOpAxis:  DydOp[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 =
                                               PushFcnStrand_YY (&$4, 2, INDIRECT);  // Dyadic operator (Indirect)
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // DO NOT FREE:  RefCnt not incremented by PushFcnStrand_YY

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYOp2; YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;

                                             lpplLocalVars->lpYYMak =
                                               MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYMak)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               PushFcnStrand_YY (lpplLocalVars->lpYYMak, 1, DIRECT); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYMak->tkToken); YYFree (lpplLocalVars->lpYYMak); lpplLocalVars->lpYYMak = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Index list w/brackets, allowing for A[A][A]...
IndexListBR:
           ']'      '['                 {DbgMsgWP (L"%%IndexListBR:  []");
                                         if (lpplLocalVars->bCtrlBreak)
                                             YYERROR;
                                         else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst =
                                               InitList0_YY ();
                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                               MakeList_EM_YY (lpplLocalVars->lpYYLst, TRUE);
                                             YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |      ']' error       '['          {DbgMsgWP (L"%%IndexListBR:  [error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    |      ']' IndexListWE '['          {DbgMsgWP (L"%%IndexListBR:  [IndexListWE]");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$2.tkToken);               // ***FIXME*** -- Do we need to free the list stack??
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst =
                                               MakeList_EM_YY (&$2, TRUE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYLst; YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;
                                         } // End IF
                                        }
    | IndexListBR ']'      '['          {DbgMsgWP (L"%%IndexListBR:  [] IndexListBR ");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);               // ***FIXME*** -- Do we need to free the list stack??
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | IndexListBR ']' error       '['   {DbgMsgWP (L"%%IndexListBR:  [error] IndexListBR ");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | IndexListBR ']' IndexListWE '['   {DbgMsgWP (L"%%IndexListBR:  [IndexListWE] IndexListBR ");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst =
                                               MakeList_EM_YY (&$3, TRUE);
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                               ListIndexRef_EM_YY (&lpplLocalVars->lpYYLst->tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&lpplLocalVars->lpYYLst->tkToken); YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Index list, with empties (meaning no ArrExpr between semicolons)
// Skip Ctrl-Break checking here so the List processing isn't interrupted
IndexListWE:
      IndexListWE1                      {DbgMsgWP (L"%%IndexListWE:  IndexListWE1");
                                         $$ = $1;
                                        }
    | IndexListWE2                      {DbgMsgWP (L"%%IndexListWE:  IndexListWE2");
                                         $$ = $1;
                                        }
    ;

IndexListWE1:
                   ';'                  {DbgMsgWP (L"%%IndexListWE1:  ;");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the list with an empty item
                                             lpplLocalVars->lpYYLst =
                                               InitList1_YY (NULL);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // Push an empty item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (lpplLocalVars->lpYYLst, NULL);
                                             FreeResult (&lpplLocalVars->lpYYLst->tkToken); YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |              ';' error            {DbgMsgWP (L"%%IndexListWE1:  error;");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    |              ';' ArrExpr          {DbgMsgWP (L"%%IndexListWE1:  ArrExpr;");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the list with an empty item
                                             lpplLocalVars->lpYYLst =
                                               InitList1_YY (NULL);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // Push an item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (lpplLocalVars->lpYYLst, &$2);
/////////////////////////////////////////////FreeResult (&$2.tkToken);               // Copied w/o IncrRefCnt in PushList_YY
                                             FreeResult (&lpplLocalVars->lpYYLst->tkToken); YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }



    | IndexListWE1 ';'                  {DbgMsgWP (L"%%IndexListWE1:  ;IndexListWE1");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Push an empty item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, NULL);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE1 ';' error            {DbgMsgWP (L"%%IndexListWE1:  error;IndexListWE1");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | IndexListWE1 ';' ArrExpr          {DbgMsgWP (L"%%IndexListWE1:  ArrExpr;IndexListWE1");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, &$3);
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Copied w/o IncrRefCnt in PushList_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

IndexListWE2:
                       ArrExpr          {DbgMsgWP (L"%%IndexListWE2:  ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Initialize the list with the arg
                                             lpplLocalVars->lpYYRes =
                                               InitList1_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // Copied w/o IncrRefCnt in PushList_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE2 ';'                  {DbgMsgWP (L"%%IndexListWE2:  ;IndexListWE2");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Push an empty item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, NULL);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // Copied w/o IncrRefCnt in PushList_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | IndexListWE2 ';' error            {DbgMsgWP (L"%%IndexListWE2:  error;IndexListWE2");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | IndexListWE2 ';' ArrExpr          {DbgMsgWP (L"%%IndexListWE2:  ArrExpr;IndexListWE2");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             // Push an item onto the list
                                             lpplLocalVars->lpYYRes =
                                               PushList_YY (&$1, &$3);
/////////////////////////////////////////////FreeResult (&$3.tkToken);               // Copied w/o IncrRefCnt in PushList_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// A data expression which has a single token
SingTokn:
      CONSTANT                          {DbgMsgWP (L"%%SingTokn:  CONSTANT");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgWP (L"%%SingTokn:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | SYSLBL                            {DbgMsgWP (L"%%SingTokn:  SYSLBL");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | USRFN0                            {DbgMsgWP (L"%%SingTokn:  USRFN0");
                                         if (lpplLocalVars->bCtrlBreak)
                                             YYERROR;
                                         else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               ExecuteFn0 (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | SYSFN0                            {DbgMsgWP (L"%%SingTokn:  SYSFN0");
                                         if (lpplLocalVars->bCtrlBreak)
                                             YYERROR;
                                         else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                               ExecuteFn0 (&$1);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | STRING                            {DbgMsgWP (L"%%SingTokn:  STRING");
                                         if (lpplLocalVars->bCtrlBreak)
                                             YYERROR;
                                         else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStr = CopyString_EM_YY (&$1);

                                             if (!lpplLocalVars->lpYYStr)
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYStr; YYFree (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;
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
//  The result of parsing the line is in the return value (plLocalvars.ExitType)
//    as well as in lpMemPTD->YYResExec if there is a value (EXITTYPE_DISPLAY
//    or EXITTYPE_NODISPLAY).
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseLine"
#else
#define APPEND_NAME
#endif

EXIT_TYPES ParseLine
    (HWND    hWndSM,                // Session Manager window handle
     HGLOBAL hGlbTxtLine,           // Line text global memory handle
     HGLOBAL hGlbToken,             // Tokenized line global memory handle (may be NULL)
     LPWCHAR lpwszLine,             // Ptr to the line text (may be NULL)
     HGLOBAL hGlbPTD,               // PerTabData global memory handle
     BOOL    bActOnErrors)          // TRUE iff errors are acted upon

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    PLLOCALVARS  plLocalVars = {0}; // Re-entrant vars
    UINT         oldTlsPlLocalVars, // Ptr to previous value of dwTlsPlLocalVars
                 oldTlsType,        // Previous value of dwTlsType
                 uError,            // Error code
                 uRet,              // The result from pl_yyparse
                 uCnt;              // Loop counter
#define MVS_CNT     2
    MEMVIRTSTR   lclMemVirtStr[MVS_CNT] = {0};// Room for MVS_CNT GuardAllocs

    // Save the previous value of dwTlsType
    oldTlsType = PtrToUlong (TlsGetValue (dwTlsType));

    // Save the thread type ('PL')
    TlsSetValue (dwTlsType, (LPVOID) 'PL');

    // Save the previous value of dwTlsPlLocalVars
    oldTlsPlLocalVars = PtrToUlong (TlsGetValue (dwTlsPlLocalVars));

    // Save the thread's ptr to local vars
    TlsSetValue (dwTlsPlLocalVars, (LPVOID) &plLocalVars);

    // Save the thread's PerTabData global memory handle
    TlsSetValue (dwTlsPerTabData, (LPVOID) hGlbPTD);

    DBGENTER;           // Must be placed after the TlsSetValue for hGlbPTD

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    EnterCriticalSection (&CSOPL);

    // Link this plLocalVars into the chain of such objects
    plLocalVars.lpPLPrev = lpMemPTD->lpPLCur;

    // If there's a previous ptr, transfer its Ctrl-Break flag
    if (lpMemPTD->lpPLCur)
        plLocalVars.bCtrlBreak = lpMemPTD->lpPLCur->bCtrlBreak;

    // Save as new current ptr
    lpMemPTD->lpPLCur = &plLocalVars;

    LeaveCriticalSection (&CSOPL);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Initialize the error code
    uError = ERRORCODE_NONE;

    // If we don't have a valid handle, ...
    if (!hGlbToken)
    {
        plLocalVars.ExitType = EXITTYPE_ERROR;

        goto NORMAL_EXIT;
    } // End IF
#ifdef DEBUG
    { // ***DEBUG***
        // Display the tokens so far
        DisplayTokens (hGlbToken);
    } // ***DEBUG*** END
#endif

    // Save values in the LocalVars
    plLocalVars.hGlbPTD     = hGlbPTD;
    plLocalVars.hWndSM      = hWndSM;
    plLocalVars.hGlbTxtLine = hGlbTxtLine;
    plLocalVars.hGlbToken   = hGlbToken;
    plLocalVars.lpwszLine   = lpwszLine;
    plLocalVars.bLookAhead  = FALSE;
    plLocalVars.ExitType    = EXITTYPE_NONE;

    // Lock the memory to get a ptr to it, and set the variables
    UTLockAndSet (plLocalVars.hGlbToken, &plLocalVars.t2);

    // Skip over TOKEN_HEADER
    plLocalVars.lptkStart = TokenBaseToStart (plLocalVars.t2.lpBase);

    // Skip to end of 1st stmt
    plLocalVars.lptkNext  = &plLocalVars.lptkStart[plLocalVars.lptkStart->tkData.tkIndex];

    // Start off with no error
    plLocalVars.tkErrorCharIndex =
    plLocalVars.tkLACharIndex    = NEG1U;

    // Allocate virtual memory for the Variable Strand accumulator
#ifdef DEBUG
    lclMemVirtStr[0].lpText   = "plLocalvars.lpYYStrandStart[STRAND_VAR] in <ParseLine>";
#endif
    lclMemVirtStr[0].IncrSize = DEF_STRAND_INCRSIZE * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].MaxSize  = DEF_STRAND_MAXSIZE  * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].IniAddr  = (LPUCHAR)
    plLocalVars.lpYYStrandStart[STRAND_VAR] =
      GuardAlloc (NULL,             // Any address
                  lclMemVirtStr[0].MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lclMemVirtStr[0].IniAddr)
    {
        DbgMsgW2 (L"ParseLine:  GuardAlloc for <plLocalVars.lpYYStrandStart[STRAND_VAR]> failed");

        goto ERROR_EXIT;
    } // End IF

    // Link this struc into the chain
    LinkMVS (&lclMemVirtStr[0]);

    // Commit the intial size
    MyVirtualAlloc (lclMemVirtStr[0].IniAddr,
                    DEF_STRAND_INITSIZE * sizeof (PL_YYSTYPE),
                    MEM_COMMIT,
                    PAGE_READWRITE);
    // Allocate virtual memory for the Function Strand accumulator
#ifdef DEBUG
    lclMemVirtStr[1].lpText   = "plLocalvars.lpYYStrandStart[STRAND_FCN] in <ParseLine>";
#endif
    lclMemVirtStr[1].IncrSize = DEF_STRAND_INCRSIZE * sizeof (PL_YYSTYPE);
    lclMemVirtStr[1].MaxSize  = DEF_STRAND_MAXSIZE  * sizeof (PL_YYSTYPE);
    lclMemVirtStr[1].IniAddr  = (LPUCHAR)
    plLocalVars.lpYYStrandStart[STRAND_FCN] =
      GuardAlloc (NULL,             // Any address
                  lclMemVirtStr[1].MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lclMemVirtStr[1].IniAddr)
    {
        DbgMsgW2 (L"ParseLine:  GuardAlloc for <pLocalVars.lpYYStrandStart[STRAND_FCN]> failed");

        goto ERROR_EXIT;
    } // End IF

    // Link this struc into the chain
    LinkMVS (&lclMemVirtStr[1]);

    // Commit the intial size
    MyVirtualAlloc (lclMemVirtStr[1].IniAddr,
                    DEF_STRAND_INITSIZE * sizeof (PL_YYSTYPE),
                    MEM_COMMIT,
                    PAGE_READWRITE);
    // Initialize the base & next strand ptrs
    plLocalVars.lpYYStrandBase[STRAND_VAR] =
    plLocalVars.lpYYStrandNext[STRAND_VAR] = plLocalVars.lpYYStrandStart[STRAND_VAR];
    plLocalVars.lpYYStrandBase[STRAND_FCN] =
    plLocalVars.lpYYStrandNext[STRAND_FCN] = plLocalVars.lpYYStrandStart[STRAND_FCN];

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
            // Parse the file, check for errors
            //   0 = success
            //   1 = YYABORT or APL error
            //   2 = memory exhausted
            uRet = pl_yyparse (&plLocalVars);
        } __except (CheckVirtAlloc (GetExceptionInformation (),
                                    L"ParseLine"))
        {} // End __try/__Except
    } __except (CheckException (GetExceptionInformation (), L"ParseLine"))
    {
        // Display message for unhandled exception
        DisplayException ();

        // Mark as in error
        uRet = 1;
        uError = ERRORCODE_ELX;

        goto NORMAL_EXIT;
    } // End __try/__Except

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

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
            LPSIS_HEADER lpSISCur;

            // Get a ptr to the current SIS header
            lpSISCur = lpMemPTD->lpSISCur;

            // If it's a permanent function (i.e. Magic Function), don't suspend at this level
            if (lpSISCur->PermFn)
            {
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_QUADERROR_INIT;

                // Set the reset flag
                lpSISCur->ResetFlag = RESETFLAG_QUADERROR_INIT;

                break;
            } // End IF

            // If this level or an adjacent preceding level is from
            //   the Execute primitive or immediate execution mode,
            //   peel back to the preceding level
            while (lpSISCur && lpSISCur->DfnType EQ DFNTYPE_EXEC
                || lpSISCur && lpSISCur->DfnType EQ DFNTYPE_IMM)
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

        defstop
            break;
    } // End IF/SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // If Ctrl-Break was pressed, ...
    if (plLocalVars.bCtrlBreak)
    {
        LPSIS_HEADER lpSISCur;

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

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

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF

    if (uRet EQ 0 || uError EQ ERRORCODE_ALX)
        goto NORMAL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If we're not resetting, ...
    if (lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_NONE)
    {
        // If called from Immediate Execution/Execute, ...
        if (lpwszLine)
            // Signal an error
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                                lpwszLine,                      // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex,   // Position of caret (origin-0)
                                hWndSM);                        // Window handle to the Session Manager
        else
        {
            LPMEMTXT_UNION lpMemTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Signal an error
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                               &lpMemTxtLine->C,                // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex,   // Position of caret (origin-0)
                                hWndSM);                        // Window handle to the Session Manager
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF/ELSE

        // Mark as in error
        uError = ERRORCODE_ELX;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Set the stop in motion
    plLocalVars.ExitType = EXITTYPE_STOP;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Set the reset flag
    lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_STOP;

    // Set the result
    ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

////     // Signal an error
////     ErrorMessageDirect (ERRMSG_WS_FULL APPEND_NAME,     // Ptr to error message text
////                         L"",                            // Ptr to the line which generated the error
////                         NEG1U,                          // Position of caret (origin-0)
////                         hWndSM);                        // Window handle to the Session Manager
////     // Execute []ELX
////     plLocalVars.lpYYRes = PrimFnMonUpTackJotCommon_EM_YY (WS_UTF16_QUAD L"ELX", FALSE, NULL);
////
////     // Lock the memory to get a ptr to it
////     lpMemPTD = MyGlobalLock (hGlbPTD);
////
////     lpMemPTD->YYResExec = *plLocalVars.lpYYRes;
////     YYFree (plLocalVars.lpYYRes); plLocalVars.lpYYRes = NULL;
////
////     // We no longer need this ptr
////     MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
NORMAL_EXIT:
    for (uCnt = 0; uCnt < MVS_CNT; uCnt++)
    if (lclMemVirtStr[uCnt].IniAddr)
    {
        // Free the virtual storage
        MyVirtualFree (lclMemVirtStr[uCnt].IniAddr, 0, MEM_RELEASE); lclMemVirtStr[uCnt].IniAddr = NULL;

        // Unlink this entry from the chain
        UnlinkMVS (&lclMemVirtStr[uCnt]);
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (plLocalVars.hGlbToken); plLocalVars.t2.lpBase   = NULL;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    EnterCriticalSection (&CSOPL);

    // Unlink this plLocalVars from the chain of such objects
    lpMemPTD->lpPLCur = plLocalVars.lpPLPrev;

    // Transfer the Ctrl-Break flag in case it hasn't been acted upon
    if (lpMemPTD->lpPLCur)
        lpMemPTD->lpPLCur->bCtrlBreak = plLocalVars.bCtrlBreak;

    LeaveCriticalSection (&CSOPL);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Restore the previous value of dwTlsPlLocalVars
    TlsSetValue (dwTlsPlLocalVars, ULongToPtr (oldTlsPlLocalVars));

    // Restore the previous value of dwTlsType
    TlsSetValue (dwTlsType, ULongToPtr (oldTlsType));

    // If there's an error to be signalled, ...
    if (uError NE ERRORCODE_NONE
     && bActOnErrors)
    {
        EXIT_TYPES exitType;    // Return code from ImmExecStmt

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

        // Execute the statement in immediate execution mode
        exitType =
          ImmExecStmt (lpwszLine,   // Ptr to line to execute
                       FALSE,       // TRUE iff free the lpwszLine on completion
                       TRUE,        // TRUE iff wait until finished
                       (HWND) (HANDLE_PTR) GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC),// Edit Control window handle
                       TRUE);       // TRUE iff errors are acted upon
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

            case EXITTYPE_ERROR:        // Nothing more to do with these
            case EXITTYPE_DISPLAY:      // ...
            case EXITTYPE_NODISPLAY:    // ...
            case EXITTYPE_NOVALUE:      // ...
            case EXITTYPE_GOTO_ZILDE:   // ...
            case EXITTYPE_NONE:         // ...
                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

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

    if (IsNameTypeVar (stFlags.stNameType)
     || stFlags.stNameType EQ NAMETYPE_FN0)
        return 'V';

    // Note we must perform the following test
    //   AFTER the one above so as not to catch the FN0 case.
    if (IsNameTypeFn (stFlags.stNameType))
        return 'F';

    if (stFlags.stNameType EQ NAMETYPE_OP1)
        return '1';

    if (stFlags.stNameType EQ NAMETYPE_OP2)
        return '2';

    if (stFlags.stNameType EQ NAMETYPE_OP3)
        return '3';

    // After having checked all the other .stNameType possibilities,
    //   if it's a NAMETYPE_UNK, then it's really a VALUE ERROR, but this
    //   isn't the time to signal that.
    if (stFlags.stNameType EQ NAMETYPE_UNK)
        return 'V';

    return '?';             // SYNTAX ERROR
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
    dprintfW (L"==Exiting  LookaheadSurround:  %c", cRes);
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
     BOOL          bSkipBrackets)   // TRUE if we're to skip over left/right brackets first

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
        case TKT_STRING:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
        case TKT_INPOUT:
            // Lookahead to see if the next token is a dyadic operator:
            //   if so, return 'F'; otherwise return 'V'.

            // Split cases based upon the lookahead result
            switch (LookaheadDyadicOp (&plLocalVars, &plLocalVars.lptkNext[-1]))
            {
                case TRUE:
                    cRes = 'F';     // Function

                    break;

                case FALSE:
                    cRes = 'V';     // Variable

                    break;

                defstop
                    break;
            } // End SWITCH

            goto NORMAL_EXIT;

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_OPJOTDOT:
            cRes = '1';             // Monadic operator

            goto NORMAL_EXIT;

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
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

        case TKT_LINECONT:
        case TKT_COMMENT:
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
        case TKT_LISTBR:
        case TKT_LEFTBRACE:
        case TKT_RIGHTBRACE:
        case TKT_SOS:
            cRes = '?';             // SYNTAX ERROR

            goto NORMAL_EXIT;

        defstop
            cRes = '?';             // SYNTAX ERROR

            goto NORMAL_EXIT;
    } // End WHILE/SWITCH
NORMAL_EXIT:
#ifdef DEBUG
    dprintfW (L"==Exiting  LookaheadAdjacent:  %c", cRes);
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
//  Lookahead (backwards) in the token stream to see if
//    the next non-right grouping symbol is a dyadic operator or not.
//***************************************************************************

BOOL LookaheadDyadicOp
    (LPPLLOCALVARS lpplLocalVars,
     LPTOKEN       lptkNext)

{
    BOOL bRet;          // The result

    DbgMsgW2 (L"==Entering LookaheadDyadicOp");

    while (TRUE)
    // Split cases based upon the token type
    switch (lptkNext->tkFlags.TknType)
    {
        case TKT_STRING:
        case TKT_VARIMMED:
        case TKT_COMMENT:
        case TKT_LEFTPAREN:
        case TKT_FCNIMMED:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_SOS:
        case TKT_ASSIGN:
        case TKT_LISTSEP:
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
    dprintfW (L"==Exiting  LookaheadDyadicOp:  %d", bRet);
#endif
    return bRet;
} // End LookaheadDyadicOp


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
PL_YYLEX_START:
    // Because we're parsing the stmt from right to left
    lpplLocalVars->lptkNext--;
#if (defined (DEBUG)) && (defined (YYLEX_DEBUG))
    dprintfW (L"==pl_yylex:  TknType = %S, CharIndex = %d",
              GetTokenTypeName (lpplLocalVars->lptkNext->tkFlags.TknType),
              lpplLocalVars->lptkNext->tkCharIndex);
    DbgBrk ();
#endif

    // Return the current token
    lpYYLval->tkToken        = *lpplLocalVars->lptkNext;

    // Initialize the rest of the fields
    lpYYLval->TknCount       =
    lpYYLval->YYInuse        =
    lpYYLval->YYIndirect     =
    lpYYLval->Avail          = 0;
    lpYYLval->lpYYFcnBase    = NULL;
    lpYYLval->lpYYStrandBase = NULL;
#ifdef DEBUG
    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);

    lpYYLval->YYIndex        = ++YYIndex;
    lpYYLval->YYFlag         = TRUE;      // Mark as a pl_yylex Index
    lpYYLval->SILevel        = lpMemPTD->SILevel;

    // We no longer need this ptr
    MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;
#endif

    // Split cases based upon the token type
    switch (lpplLocalVars->lptkNext->tkFlags.TknType)
    {
        case TKT_VARIMMED:
        case TKT_VARARRAY:
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

            // Test various cases mostly vased on the ObjType
            if (IsNameTypeVar (stFlags.stNameType))
            {
////////////////lpYYLval->tkToken.tkFlags.TknType                     =
////////////////lpplLocalVars->lptkNext->tkFlags.TknType              = TKT_VARNAMED;    // Already set

                return NAMEVAR;
            } else
            if (stFlags.stNameType EQ NAMETYPE_FN0)
            {
                lpYYLval->tkToken.tkFlags.TknType                     =
                lpplLocalVars->lptkNext->tkFlags.TknType              = TKT_VARNAMED; // Instead of TKT_FCNNAMED because it'll be executed right away

                if (stFlags.ObjName EQ OBJNAME_SYS)
                    return SYSFN0;
                else
                    return USRFN0;
            } else
            if (stFlags.stNameType EQ NAMETYPE_FN12)
            {
                lpYYLval->tkToken.tkFlags.TknType                     =
                lpplLocalVars->lptkNext->tkFlags.TknType              = TKT_FCNNAMED;

                if (stFlags.ObjName EQ OBJNAME_SYS)
                    return SYSFN12;
                else
                    return NAMEFCN;
            } else
            if (stFlags.stNameType EQ NAMETYPE_OP1)
            {
                lpYYLval->tkToken.tkFlags.TknType                     =
                lpplLocalVars->lptkNext->tkFlags.TknType              = TKT_OP1NAMED;

                return NAMEOP1;
            } else
            if (stFlags.stNameType EQ NAMETYPE_OP2)
            {
                lpYYLval->tkToken.tkFlags.TknType                     =
                lpplLocalVars->lptkNext->tkFlags.TknType              = TKT_OP2NAMED;

                return NAMEOP2;
            } else
            if (stFlags.stNameType EQ NAMETYPE_OP3)
            {
                lpYYLval->tkToken.tkFlags.TknType                     =
                lpplLocalVars->lptkNext->tkFlags.TknType              = TKT_OP3NAMED;

                return NAMEOP3;
            } else
            if (stFlags.DfnSysLabel)
                return SYSLBL;
            else
                return NAMEUNK;
        } // End TKT_VARNAMED

        case TKT_ASSIGN:
            return ASSIGN;

        case TKT_LISTSEP:
            return ';';

        case TKT_FCNIMMED:
            if (lpplLocalVars->lptkNext->tkData.tkIndex EQ UTF16_RIGHTARROW)
                return GOTO;
            else
                return PRIMFCN;

        case TKT_COMMENT:
        case TKT_LINECONT:
        case TKT_SOS:
            goto PL_YYLEX_START;

        case TKT_STRING:
            return STRING;

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
            switch (LookaheadAdjacent (lpplLocalVars, FALSE))
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
            {
                // Split cases based upon the operator symbol
                switch (lpYYLval->tkToken.tkData.tkChar)
                {
                    case UTF16_DIERESISCIRCLE:
                        lpYYLval->tkToken.tkData.tkChar = INDEX_DIERESISCIRCLE;

                        break;

                    default:
                        break;
                } // End SWITCH

                return OP1;
            } // End IF/ELSE

        case TKT_OP2IMMED:
            // Split cases based upon the operator symbol
            switch (lpYYLval->tkToken.tkData.tkChar)
            {
                case UTF16_DIERESISJOT:
                    lpYYLval->tkToken.tkData.tkChar = INDEX_DIERESISJOT;

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
            switch (LookaheadAdjacent (lpplLocalVars, FALSE))
            {
                case '1':               // If the next token is a monadic operator, or
                case 'F':               // If the next token is a function,
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
            switch (LookaheadAdjacent (lpplLocalVars, TRUE))
            {
                case '2':               // If the next token is a dyadic operator,
                    return '?';         //   then this token is an axis operator

                case '1':               // If the next token is a monadic operator, or
                case '3':               //   an ambiguous operator, or
                case 'A':               //   an assignment arrow, or
                case 'F':               //   a function,
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
            return '{';

        case TKT_RIGHTBRACE:
            return '}';

        defstop
            return UNK;
    } // End SWITCH
} // End pl_yylex


//***************************************************************************
//  $CheckNullOp3
//
//  Check for NULLOP followed by an ambiguous operator possible with axis
//***************************************************************************

BOOL CheckNullOp3
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

    // Skip this is we're in lookahead
    //   but save the char index
    if (lpplLocalVars->bLookAhead)
    {
        lpplLocalVars->tkLACharIndex = lpplLocalVars->lptkNext[1].tkCharIndex;
        lpplLocalVars->ExitType = EXITTYPE_ERROR;

        return;
    } // End IF

    // Set the caret ptr and exit type
    if (lpplLocalVars->tkLACharIndex NE NEG1U)
        lpplLocalVars->tkErrorCharIndex = lpplLocalVars->tkLACharIndex;
    else
        lpplLocalVars->tkErrorCharIndex = lpplLocalVars->lptkNext[1].tkCharIndex;
    lpplLocalVars->ExitType = EXITTYPE_ERROR;

    // Check for SYNTAX ERROR
#define ERR     "syntax error"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
        ErrorMessageIndirect (L"SYNTAX ERROR" APPEND_NAME);
#undef  ERR
    else
#define ERR     "memory exhausted"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
        ErrorMessageIndirect (L"WS FULL" APPEND_NAME);
#undef  ERR
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
    if (szTemp[i1 - 1] EQ '\n')
    {
        szTemp[i1 - 1] = '\0';  // Remove trailing LF
                                //   because we're displaying
                                //   in a GUI.
        DbgMsg (szTemp);        // Display in my debugger window.

        szTemp[0] = '\0';       // Restart the buffer
    } // End IF/ELSE
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
            lpwsz[-1] = L'\0';
            lpszFmt = lpszFmt2;

            break;

        default:
            wszTemp[0] = L'\0';
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
     BOOL    bQuoteQuad,            // TRUE iff Quote-Quad input (FALSE if Quad input)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    HANDLE         hSemaphore;      // Semaphore handle
    LPPL_YYSTYPE   lpYYRes;         // Ptr to the result
    HGLOBAL        hGlbPTD;         // PerTabData global memory handle
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory
    UINT           uLinePos,        // Char position of start of line
                   uCharPos;        // Current char position
    HWND           hWndEC;          // Edit Control window handle

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Create a semaphore
    hSemaphore =
    CreateSemaphore (NULL,              // No security attrs
                     0,                 // Initial count (non-signalled)
                     64*1024,           // Maximum count
                     NULL);             // No name
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

    // Get the Edit Control window handle
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWndEC);

    // Get the position of the start of the line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, NEG1U, 0);

    // Save the Quote-Quad input prompt length
    lpMemPTD->lpSISCur->QQPromptLen = uCharPos - uLinePos;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Tell the Session Manager to display the appropriate prompt
    PostMessageW (hWndSM, MYWM_QUOTEQUAD, bQuoteQuad, 14);
#ifdef DEBUG
    dprintfW (L"~~WaitForSingleObject (ENTRY):  %s (%S#%d)", L"WaitForInput", FNLN);
#endif
    // Wait for the semaphore to trigger
    WaitForSingleObject (hSemaphore,    // Handle to wait for
                         INFINITE);     // Timeout value in milliseconds
#ifdef DEBUG
    dprintfW (L"~~WaitForSingleObject (EXIT):   %s (%S#%d)", L"WaitForInput", FNLN);
#endif
    // Close the semaphore handle as it is no longer needed
    CloseHandle (hSemaphore); hSemaphore = NULL;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    Unlocalize ();

    // If we're resetting, ...
    if (lpMemPTD->lpSISCur
     && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
        lpYYRes = NULL;
    else
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Copy the result
        *lpYYRes = lpMemPTD->YYResExec;
        ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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

BOOL AmbOpSwap_EM
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

            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
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
            hGlbFcn = ClrPtrTypeDirAsGlb (lpYYFcn->tkToken.tkData.tkGlbData);

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
//  End of File: parse.y
//***************************************************************************
