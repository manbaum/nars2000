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

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               pl_yyfprintf
//#define YYPRINT                 pl_yyprint
#define fprintf                 pl_yyfprintf
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

typedef enum tagERRORCODES
{
    ERRORCODE_NONE = 0,     // 00:  No error
    ERRORCODE_ALX,          // 01:  Signal []ALX
    ERRORCODE_ELX,          // 02:  Signal []ELX
} ERRORCODES;

#define DbgMsgW2(a) if (gDbgLvl > 2) DbgMsgW(a)
////#define DbgMsgW2(a) DbgMsgW(a); DbgBrk ()
%}

%pure-parser
%name-prefix="pl_yy"
%parse-param {LPPLLOCALVARS lpplLocalVars}
%lex-param   {LPPLLOCALVARS lpplLocalVars}

%token UNK EOL
%token NAMEVAR NAMEUNK CONSTANT STRING USRFN0 SYSFN0 QUAD QUOTEQUAD SYSLBL
%token DIAMOND

/*  Note that as we parse APL from right to left, these rules
    are all reversed as well as token associativity (long scope).
    That is, functions in APL have long right scope, so they are
    declared here in the %left list.  Similarly, operators in APL
    have long left scope, so they are declared in the %right list.
 */
%left  ASSIGN PRIMFCN NAMEFCN SYSFN12 GOTO
%right NAMEOP1 OP1 NAMEOP2 OP2 JOTDOT

%start Line

%%

/*  ToDo
    ----
    * Control structures
    * J/Dyalog's dynamic functions
    *

    Notes
    ----
    * A monadic operator with a var left arg must be
      enclosed in parens, such as (2 op1) because
      1 2 op1 can be interpreted as 1 (2 op1) as well
      as (1 2) op1.
    * A dyadic operator with either argument as a var
      must be enclosed in parens for the same reasons
      as above, otherwise 1 2 op2 3 4 is way too ambiguous.
 */

// Line
Line:
      Stmts                             {DbgMsgW2 (L"%%Line:  Stmts");}
    | Stmts ':' NAMEUNK                 {DbgMsgW2 (L"%%Line:  NAMEUNK: Stmts");}
    | Stmts ':' NAMEVAR                 {DbgMsgW2 (L"%%Line:  NAMEVAR: Stmts");}
    | Stmts ':' SYSLBL                  {DbgMsgW2 (L"%%Line:  SYSLBL: Stmts");}
    ;

// Statements
Stmts:
      // All errors propagate up to this point where we ABORT -- this ensures
      //   that the call to pl_yyparse terminates with a non-zero error code.
      error                             {DbgMsgW2 (L"%%Stmts:  error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             Assert (YYResIsEmpty ());
                                         } // End IF

                                         YYABORT;
                                        }
    | Stmt                              {DbgMsgW2 (L"%%Stmts:  Stmt");
                                         if (lpplLocalVars->bCtrlBreak)
                                             YYERROR;
                                         else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             Assert (YYResIsEmpty ());
                                         } // End IF
                                        }
    | Stmts DIAMOND Stmt                {DbgMsgW2 (L"%%Stmts:  Stmts " WS_UTF16_DIAMOND L" Stmt");
                                         if (lpplLocalVars->bCtrlBreak)
                                             YYERROR;
                                         else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             Assert (YYResIsEmpty ());  // Likely not TRUE with non-empty SI
                                         } // End IF
                                        }
    ;

// Statement
Stmt:
          /* Empty */                   {DbgMsgW2 (L"%%Stmt:  <empty>");}
    | ArrExpr                           {DbgMsgW2 (L"%%Stmt:  ArrExpr");
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

                                             // Do not display if caller is Execute or Quad
                                             if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                              || (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                               && lpMemPTD->lpSISCur->lpSISPrv NE NULL
                                               && (lpMemPTD->lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_EXEC
                                                || lpMemPTD->lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_QUAD)))
                                             {
                                                 // If it's NoValue on Quad input, ...
                                                 if (IsTokenNoValue (&$1.tkToken)
                                                  && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                     PostMessage (lpMemPTD->hWndSM, MYWM_QUOTEQUAD, FALSE, 11);
                                                 else
                                                 {
                                                     LPSIS_HEADER lpSISCur;          // Ptr to current SIS header

                                                     // Peel back to the first non-Imm/Exec layer
                                                     //   starting with the previous SIS header
                                                     lpSISCur = GetSISLayer (lpMemPTD->lpSISCur->lpSISPrv);

                                                     // Copy the result
                                                     lpplLocalVars->lpYYRes = CopyPL_YYSTYPE_EM_YY (&$1, FALSE);
                                                     lpMemPTD->YYResExec = *lpplLocalVars->lpYYRes;
                                                     YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                                 } // End IF/ELSE

                                                 // Mark as not in error
                                                 lpplLocalVars->bRet = TRUE;
                                             } else
                                                 lpplLocalVars->bRet = ArrayDisplay_EM (&$1.tkToken, TRUE);

                                             // We no longer need this ptr
                                             MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

                                             lpplLocalVars->ExitType = EXITTYPE_NODISPLAY;
                                             FreeResult (&$1.tkToken);
                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;
                                         } // End IF/ELSE
                                        }
    | error   GOTO                      {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_RIGHTARROW L"error");
                                         if (lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } // End IF/ELSE
                                        }
    | ArrExpr GOTO                      {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_RIGHTARROW L"ArrExpr");
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
                                             lpplLocalVars->lpYYRes = MakeNoValue_YY (&$2.tkToken);
                                             lpMemPTD->YYResExec = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             // We no longer need this ptr
                                             MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

                                             // Split cases based upon the Exit Type
                                             switch (lpplLocalVars->ExitType)
                                             {
                                                 case EXITTYPE_GOTO_ZILDE:  // {zilde}
                                                     break;

                                                 case EXITTYPE_GOTO_LINE:   // Valid line #
                                                     // Lock the memory to get a ptr to it
                                                     lpMemPTD = MyGlobalLock (lpplLocalVars->hGlbPTD);


                                                     // If we're in Immediate Execution mode,
                                                     //   Execute primitive, or Quad input, ...
                                                     if (lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_IMM
                                                      || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_EXEC
                                                      || lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                                                     {
                                                         LPSIS_HEADER lpSISCur;          // Ptr to current SIS header

                                                         // Peel back to the first non-Imm/Exec layer
                                                         //   starting with the previous SIS header
                                                         lpSISCur = GetSISLayer (lpMemPTD->lpSISCur->lpSISPrv);

                                                         Assert (lpSISCur NE NULL);

                                                         // Save the semaphore handle to signal the suspended thread to restart execution
                                                         lpMemPTD->lpSISCur->hSigaphore = lpSISCur->hSemaphore;
                                                     } // End IF

                                                     // We no longer need this ptr
                                                     MyGlobalUnlock (lpplLocalVars->hGlbPTD); lpMemPTD = NULL;

                                                     YYACCEPT;              // Stop executing this line

                                                 case EXITTYPE_ERROR:       // Error
                                                     YYERROR;               // Stop on error

                                                 defstop
                                                     break;
                                             } // End SWITCH
                                         } // End IF/ELSE/IF
                                        }
    |         GOTO                      {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_RIGHTARROW);
                                         if (lpplLocalVars->bCtrlBreak || lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                         {
                                             GotoReset ();
                                             lpplLocalVars->ExitType = EXITTYPE_GOTO_RESET;
                                         } // End IF/ELSE
                                        }
    | error   ASSIGN                    {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_LEFTARROW L"error");
                                         if (lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                         {
                                             lpplLocalVars->ExitType = EXITTYPE_ERROR;
                                             YYERROR;
                                         } // End IF/ELSE
                                        }
    | ArrExpr ASSIGN                    {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_LEFTARROW L"ArrExpr");
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
    | FcnSpec                           {DbgMsgW2 (L"%%Stmt:  FcnSpec");
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
    | Op1Spec                           {DbgMsgW2 (L"%%Stmt:  Op1Spec");
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
    | Op2Spec                           {DbgMsgW2 (L"%%Stmt:  Op2Spec");
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
    |     error   EOL                   {DbgMsgW2 (L"%%Stmt:  EOL error");
                                         if (lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    |     ArrExpr EOL                   {DbgMsgW2 (L"%%Stmt:  EOL ArrExpr");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->NameType = NAMETYPE_VAR;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     FcnSpec EOL                   {DbgMsgW2 (L"%%Stmt:  EOL FcnSpec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->NameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     Op1Spec EOL                   {DbgMsgW2 (L"%%Stmt:  EOL Op1Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->NameType = NAMETYPE_OP1;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     Op2Spec EOL                   {DbgMsgW2 (L"%%Stmt:  EOL Op2Spec");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->NameType = NAMETYPE_OP2;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     MonOp   EOL                   {DbgMsgW2 (L"%%Stmt:  EOL MonOp");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->NameType = NAMETYPE_OP1;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     DydOp   EOL                   {DbgMsgW2 (L"%%Stmt:  EOL DydOp");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->NameType = NAMETYPE_OP2;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     LeftOper EOL                  {DbgMsgW2 (L"%%Stmt:  EOL LeftOper");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->NameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     AxisFunc EOL                  {DbgMsgW2 (L"%%Stmt:  EOL AxisFunc");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->NameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     Drv1Func EOL                  {DbgMsgW2 (L"%%Stmt:  EOL Drv1Func");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->NameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    |     Drv2Func EOL                  {DbgMsgW2 (L"%%Stmt:  EOL Drv2Func");
                                         if (lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->NameType = NAMETYPE_FN12;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    ;

NameAnyVar:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyVar:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgW2 (L"%%NameAnyVar:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

NameAnyFcn:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyFcn:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEFCN                           {DbgMsgW2 (L"%%NameAnyFcn:  NAMEFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

NameAnyOp1:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEFCN                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP1                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEOP1");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP2                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEOP2");
                                         if (!lpplLocalVars->bLookAhead)
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

NameAnyOp2:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyOp2:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEFCN                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP1                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEOP1");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP2                           {DbgMsgW2 (L"%%NameAnyOp2:  NAMEOP2");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    ;

// Function specification
FcnSpec:
      LeftOper ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UTF16_LEFTARROW L"LeftOper");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->bRet = AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | Drv1Func ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UTF16_LEFTARROW L"Drv1Func");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->bRet = AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | Drv2Func ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UTF16_LEFTARROW L"Drv2Func");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->bRet = AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | AxisFunc ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UTF16_LEFTARROW L"AxisFunc");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_FN12, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->bRet = AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    ;

// Monadic operator specification
Op1Spec:
      MonOp     ASSIGN NameAnyOp1       {DbgMsgW2 (L"%%Op1Spec:  NameAny" WS_UTF16_LEFTARROW L"MonOp");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP1, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP1, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->bRet = AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    ;

// Dyadic operator specification
Op2Spec:
      DydOp    ASSIGN NameAnyOp2        {DbgMsgW2 (L"%%Op2Spec:  NameAny" WS_UTF16_LEFTARROW L"DydOp");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP2, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, NAMETYPE_OP2, TRUE);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->bRet = AssignName_EM (&$3.tkToken, &lpplLocalVars->lpYYFcn->tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    ;

// Array expression
ArrExpr:
      SimpExpr                          {DbgMsgW2 (L"%%ArrExpr:  SimpExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | StrandInst                        {DbgMsgW2 (L"%%ArrExpr:  StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | error   LeftOper                  {DbgMsgW2 (L"%%ArrExpr:  LeftOper error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr LeftOper                  {DbgMsgW2 (L"%%ArrExpr:  LeftOper ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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

                                             lpplLocalVars->lpYYRes = ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | error   Drv1Func                  {DbgMsgW2 (L"%%ArrExpr:  Drv1Func error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr Drv1Func                  {DbgMsgW2 (L"%%ArrExpr:  Drv1Func ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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

                                             lpplLocalVars->lpYYRes = ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | error   AxisFunc                  {DbgMsgW2 (L"%%ArrExpr:  AxisFunc error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr AxisFunc                  {DbgMsgW2 (L"%%ArrExpr:  AxisFunc ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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

                                             lpplLocalVars->lpYYRes = ExecFunc_EM_YY (NULL, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr LeftOper error            {DbgMsgW2 (L"%%ArrExpr:  error LeftOper ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);

                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | error   LeftOper StrandInst       {DbgMsgW2 (L"%%ArrExpr:  StrandInst LeftOper error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);

                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr LeftOper StrandInst       {DbgMsgW2 (L"%%ArrExpr:  StrandInst LeftOper ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes = ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ArrExpr AxisFunc error            {DbgMsgW2 (L"%%ArrExpr:  error AxisFunc ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);

                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | error   AxisFunc StrandInst       {DbgMsgW2 (L"%%ArrExpr:  StrandInst AxisFunc error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);

                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr AxisFunc StrandInst       {DbgMsgW2 (L"%%ArrExpr:  StrandInst AxisFunc ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes = ExecFunc_EM_YY (&$3.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Single var (including single names)
SingVar:
                  NAMEUNK               {DbgMsgW2 (L"%%SingVar:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$1.tkToken);

                                             YYERROR;
                                         } // End IF
                                        }
    |             QUAD                  {DbgMsgW2 (L"%%SingVar:  QUAD");
                                         if (lpplLocalVars->bCtrlBreak || lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                         {
                                             lpplLocalVars->lpYYRes = WaitForInput (lpplLocalVars->hWndSM, FALSE, &$1.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |             QUOTEQUAD             {DbgMsgW2 (L"%%SingVar:  QUOTEQUAD");
                                         if (lpplLocalVars->bCtrlBreak || lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                         {
                                             lpplLocalVars->lpYYRes = WaitForInput (lpplLocalVars->hWndSM, TRUE, &$1.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |             SingTokn              {DbgMsgW2 (L"%%SingVar:  SingTokn");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    |         ')' error   '('           {DbgMsgW2 (L"%%SingVar:  error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    |         ')' ArrExpr '('           {DbgMsgW2 (L"%%SingVar:  (ArrExpr)");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $2;
                                        }
    ;

// Strand
Strand:
      SingVar                           {DbgMsgW2 (L"%%Strand:  SingVar -- InitVarStrand/PushVarStrand_YY");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             InitVarStrand (&$1);

                                             lpplLocalVars->lpYYRes = PushVarStrand_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);  // Copied w/o IncrRefCnt in PushVarStrand_YY

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | Strand      SingVar               {DbgMsgW2 (L"%%Strand:  SingVar Strand -- PushVarStrand_YY");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes = PushVarStrand_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2.tkToken);  // Copied w/o IncrRefCnt in PushVarStrand_YY
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Strand instance
StrandInst:
      Strand                            {DbgMsgW2 (L"%%StrandInst:  Strand -- MakeVarStrand_EM_YY");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYStr =
                                             MakeVarStrand_EM_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYStr)             // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYStr->tkToken); YYFree (lpplLocalVars->lpYYStr); lpplLocalVars->lpYYStr = NULL;
                                             } // End IF

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
    ;

// Simple array expression
SimpExpr:
      ILPAR                             {DbgMsgW2 (L"%%SimpExpr:  ILPAR");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | ILBR     StrandInst               {DbgMsgW2 (L"%%SimpExpr:  StrandInst ILBR");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes = ArrayIndex_EM_YY (&$2.tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | error   ASSIGN       QUAD         {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN       QUAD         {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->bRet = ArrayDisplay_EM (&$1.tkToken, TRUE);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN       QUOTEQUAD    {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN       QUOTEQUAD    {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->bRet = ArrayDisplay_EM (&$1.tkToken, FALSE);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result

                                             if (!lpplLocalVars->bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN       NameAnyVar   {DbgMsgW2 (L"%%SimpExpr:  NameAny" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN       NameAnyVar   {DbgMsgW2 (L"%%SimpExpr:  NameAny" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->bRet = AssignName_EM (&$3.tkToken, &$1.tkToken);
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
    | error   ASSIGN ILBR  NAMEVAR      {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN ILBR  NAMEVAR      {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR" WS_UTF16_LEFTARROW L"ArrExpr");
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



                                             PrimFnNonceError_EM (&$3.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    | error   ASSIGN   ')' NameSpec '(' {DbgMsgW2 (L"%%SimpExpr:  (NameSpec)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' NameSpec '(' {DbgMsgW2 (L"%%SimpExpr:  (NameSpec)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->bRet = AssignNameSpec_EM (&$4.tkToken, &$1.tkToken);
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
    | error   ASSIGN   ')' ILBR NAMEVAR '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NAMEVAR ILBR)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' ILBR NAMEVAR '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NAMEVAR ILBR)" WS_UTF16_LEFTARROW L"ArrExpr");
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



                                             PrimFnNonceError_EM (&$4.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    | error   ASSIGN LeftOper  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN LeftOper  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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

                                             lpplLocalVars->lpYYRes = ExecFunc_EM_YY (&$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // DO NOT FREE:  Used below

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet = AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
                                             FreeResult (&lpplLocalVars->lpYYRes->tkToken); YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN AxisFunc  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN AxisFunc  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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

                                             lpplLocalVars->lpYYRes = ExecFunc_EM_YY (&$4.tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
/////////////////////////////////////////////FreeResult (&$4.tkToken);               // DO NOT FREE:  Used below

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$4.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet = AssignName_EM (&$4.tkToken, &lpplLocalVars->lpYYRes->tkToken);
                                             FreeResult (&lpplLocalVars->lpYYRes->tkToken); YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                             FreeResult (&$4.tkToken);               // Validation only

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN LeftOper ')' NameVals '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN = MakeNameStrand_EM_YY (&$5);
                                             FreeResult (&$5.tkToken);

                                             if (lpplLocalVars->lpYYStrN)            // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN LeftOper ')' NameVals '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYStrN = MakeNameStrand_EM_YY (&$5);
                                             FreeResult (&$5.tkToken);

                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYStrN)            // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                             } // End IF

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN = MakeNameStrand_EM_YY (&$5);
                                             FreeResult (&$5.tkToken);

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
                                                     FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 } // End IF

                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet = ModifyAssignNameVals_EM (&lpplLocalVars->lpYYStrN->tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN AxisFunc ')' NameVals '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN = MakeNameStrand_EM_YY (&$5);
                                             FreeResult (&$5.tkToken);

                                             if (lpplLocalVars->lpYYStrN)            // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                             } // End IF

                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN AxisFunc ')' NameVals '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYStrN = MakeNameStrand_EM_YY (&$5);
                                             FreeResult (&$5.tkToken);

                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYStrN)            // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                             } // End IF

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN = MakeNameStrand_EM_YY (&$5);
                                             FreeResult (&$5.tkToken);

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
                                                     FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 } // End IF

                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet = ModifyAssignNameVals_EM (&lpplLocalVars->lpYYStrN->tkToken, lpplLocalVars->lpYYFcn, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);               // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
    | error   ASSIGN LeftOper ILBR NAMEVAR
                                        {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN LeftOper ILBR NAMEVAR
                                        {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$4.tkToken);
                                                 FreeResult (&$5.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet = FALSE;
                                             PrimFnNonceError_EM (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             FreeResult (&$4.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             YYERROR;
                                         } // End IF
                                        }
    | error   ASSIGN AxisFunc ILBR NAMEVAR
                                        {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN AxisFunc ILBR NAMEVAR
                                        {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$3, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                                 FreeResult (&$4.tkToken);
                                                 FreeResult (&$5.tkToken);           // Validation only
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->bRet = FALSE;
                                             PrimFnNonceError_EM (&$4.tkToken);
                                             FreeResult (&$5.tkToken);               // Validation only
                                             FreeResult (&$4.tkToken);
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->bRet)
                                                 YYERROR;

                                             YYERROR;
                                         } // End IF
                                        }
    ;

// Selective specification
NameSpec:
      NameVars                          {DbgMsgW2 (L"%%NameSpec:  NameVars");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYStrN = MakeNameStrand_EM_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYStrN)            // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                             } // End IF

                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN = MakeNameStrand_EM_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYStrN)           // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYStrN; YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                         } // End IF
                                        }
    | NameSpec LeftOper                 {DbgMsgW2 (L"%%NameSpec:  LeftOper NameSpec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$1.tkToken);

                                             YYERROR;
                                         } // End IF
                                        }
    | NameSpec AxisFunc                 {DbgMsgW2 (L"%%NameSpec:  AxisFunc NameSpec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$1.tkToken);

                                             YYERROR;
                                         } // End IF
                                        }
    | NameSpec LeftOper StrandInst      {DbgMsgW2 (L"%%NameSpec:  StrandInst LeftOper NameSpec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$1.tkToken);

                                             YYERROR;
                                         } // End IF
                                        }
    | NameSpec AxisFunc StrandInst      {DbgMsgW2 (L"%%NameSpec:  StrandInst AxisFunc NameSpec");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
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
                                             FreeYYFcn (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$1.tkToken);

                                             YYERROR;
                                         } // End IF
                                        }
    ;

// Named Variables Strand w/Value
// Skip Ctrl-Break checking here so the Name Strand processing isn't interrupted
NameVals:
      NAMEVAR                           {DbgMsgW2 (L"%%NameVals:  NAMEVAR");
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
    | NAMEUNK                           {DbgMsgW2 (L"%%NameVals:  NAMEUNK");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$1.tkToken);
                                             FreeResult (&$1.tkToken);               // Validation only

                                             YYERROR;
                                         } // End IF
                                        }
    | NameVals       NAMEVAR            {DbgMsgW2 (L"%%NameVals:  NAMEVAR NameVals");
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
    | NameVals       NAMEUNK            {DbgMsgW2 (L"%%NameVals:  NAMEUNK NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN = MakeNameStrand_EM_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYStrN)            // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                             } // End IF

                                             PrimFnValueError_EM (&$2.tkToken);
                                             FreeResult (&$2.tkToken);               // Validation only

                                             YYERROR;
                                         } // End IF
                                        }
    |       ')' ILBR NAMEVAR '('        {DbgMsgW2 (L"%%NameVals:  (NAMEVAR ILBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $3.tkToken.tkFlags.TknType = TKT_VARNAMED;




                                             PrimFnNonceError_EM (&$2.tkToken);
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only

                                             YYERROR;
                                         } // End IF
                                        }
    |       ')' ILBR NAMEUNK '('        {DbgMsgW2 (L"%%NameVals:  (NAMEUNK ILBR)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             PrimFnValueError_EM (&$2.tkToken);
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);               // Validation only

                                             YYERROR;
                                         } // End IF
                                        }
    | NameVals ')' ILBR NAMEVAR '('     {DbgMsgW2 (L"%%NameVals:  (NAMEVAR ILBR) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;




                                             PrimFnNonceError_EM (&$3.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    | NameVals ')' ILBR NAMEUNK '('     {DbgMsgW2 (L"%%NameVals:  (NAMEUNK ILBR) NameVals");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYStrN = MakeNameStrand_EM_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYStrN)            // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYStrN->tkToken); YYFree (lpplLocalVars->lpYYStrN); lpplLocalVars->lpYYStrN = NULL;
                                             } // End IF

                                             PrimFnValueError_EM (&$4.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);               // Validation only

                                             YYERROR;
                                         } // End IF
                                        }
    ;

// Named Variables Strand with or w/o Value
// Skip Ctrl-Break checking here so the Name Strand processing isn't interrupted
NameVars:
      NAMEVAR                           {DbgMsgW2 (L"%%NameVars:  NAMEVAR");
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
    | NAMEUNK                           {DbgMsgW2 (L"%%NameVars:  NAMEUNK");
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
    | NameVars       NAMEVAR            {DbgMsgW2 (L"%%NameVars:  NameVars NAMEVAR");
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
    | NameVars       NAMEUNK            {DbgMsgW2 (L"%%NameVars:  NameVars NAMEUNK");
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
      RightOper DydOp StrandInst        {DbgMsgW2 (L"%%Drv1Func:  StrandInst DydOp RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$3, 1, FALSE);      // Left operand (Direct)
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Right operand (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |                  MonOp StrandInst {DbgMsgW2 (L"%%Drv1Func:  StrandInst MonOp");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, FALSE);      // Only operand (Direct)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 2 (valid in FcnSpec: but not ArrExpr: because of Strand on the right)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
Drv2Func:
      StrandInst DydOp LeftOper         {DbgMsgW2 (L"%%Drv2Func:  LeftOper DydOp StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$3, 1, TRUE);       // Left operand (Indirect)
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, FALSE);      // Right operand (Direct)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | StrandInst DydOp AxisFunc         {DbgMsgW2 (L"%%Drv2Func:  AxisFunc DydOp StrandInst");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$3, 1, TRUE);       // Left operand (Indirect)
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, FALSE);      // Right operand (Direct)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Parenthesized function expression
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
ParenFunc:
      '>' LeftOper '('                  {DbgMsgW2 (L"%%ParenFunc:  (LeftOper)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | '>' AxisFunc '('                  {DbgMsgW2 (L"%%ParenFunc:  (AxisFunc)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | '>' FcnSpec '('                   {DbgMsgW2 (L"%%ParenFunc:  (FcnSpec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Left operand function
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
LeftOper:
          PRIMFCN                       {DbgMsgW2 (L"%%LeftOper:  PRIMFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakePrimFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN                       {DbgMsgW2 (L"%%LeftOper:  NAMEFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakeNameFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     SYSFN12                       {DbgMsgW2 (L"%%LeftOper:  SYSFN12");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakeNameFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     ParenFunc                     {DbgMsgW2 (L"%%LeftOper:  ParenFunc");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     RightOper    JOTDOT           {DbgMsgW2 (L"%%LeftOper:  " WS_UTF16_JOT L". RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 2, FALSE);      // Monadic operator (Direct)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // ***FIXME** -- Should it be JOTDOT here??
                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Operand (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |                  MonOp LeftOper   {DbgMsgW2 (L"%%LeftOper:  LeftOper MonOp");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Only operand (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |                  MonOp AxisFunc   {DbgMsgW2 (L"%%LeftOper:  AxisFunc MonOp");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Only operand (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     RightOper DydOp LeftOper      {DbgMsgW2 (L"%%LeftOper:  LeftOper DydOp RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$3, 1, TRUE);       // Left operand (Indirect)
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Right operand (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     RightOper DydOp AxisFunc      {DbgMsgW2 (L"%%LeftOper:  AxisFunc DydOp RightOper");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$3, 1, TRUE);       // Left operand (Indirect)
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Right operand (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | '>' Drv1Func                 '('  {DbgMsgW2 (L"%%LeftOper:  (Drv1Func)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | '>' Drv2Func                 '('  {DbgMsgW2 (L"%%LeftOper:  (Drv2Func)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | '>'     StrandInst   DydOp StrandInst '('
                                        {DbgMsgW2 (L"%%LeftOper:  (StrandInst DydOp StrandInst)");  // ***FIXME*** -- Put into Drv2Func??
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$3, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$3.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$4, 1, FALSE);      // Left operand (Direct)
                                             FreeResult (&$4.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, FALSE);      // Right operand (Direct)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Right operand function (short right scope)
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
RightOper:
          PRIMFCN                       {DbgMsgW2 (L"%%RightOper:  PRIMFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakePrimFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN                       {DbgMsgW2 (L"%%RightOper:  NAMEFCN");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakeNameFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     SYSFN12                       {DbgMsgW2 (L"%%RightOper:  SYSFN12");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakeNameFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |     ParenFunc                     {DbgMsgW2 (L"%%RightOper:  ParenFunc");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Axis function expression
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
AxisFunc:
      '}' error   '['  PRIMFCN          {DbgMsgW2 (L"%%AxisFunc:  PRIMFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakePrimFcn_YY (&$4);
                                             FreeResult (&$4.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  PRIMFCN          {DbgMsgW2 (L"%%AxisFunc:  PRIMFCN[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakePrimFcn_YY (&$4);
                                             FreeResult (&$4.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYFcn, 2, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYAxis = MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYAxis, 1, FALSE); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYAxis->tkToken); YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  NAMEFCN          {DbgMsgW2 (L"%%AxisFunc:  NAMEFCN[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakeNameFcn_YY (&$4);
                                             FreeResult (&$4.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  NAMEFCN          {DbgMsgW2 (L"%%AxisFunc:  NAMEFCN[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakeNameFcn_YY (&$4);
                                             FreeResult (&$4.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYFcn, 2, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYAxis = MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYAxis, 1, FALSE); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYAxis->tkToken); YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  SYSFN12          {DbgMsgW2 (L"%%AxisFunc:  SYSFN12[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakeNameFcn_YY (&$4);
                                             FreeResult (&$1.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  SYSFN12          {DbgMsgW2 (L"%%AxisFunc:  SYSFN12[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakeNameFcn_YY (&$4);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYFcn, 2, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYAxis = MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYAxis, 1, FALSE); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYAxis->tkToken); YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | '}' error   '['  ParenFunc        {DbgMsgW2 (L"%%AxisFunc:  ParenFunc[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$4, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$4.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  ParenFunc        {DbgMsgW2 (L"%%AxisFunc:  ParenFunc[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYAxis = MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYAxis, 1, FALSE); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYAxis->tkToken); YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             // The result is always the root of the function tree
                                             $$ = $4;
                                         } // End IF
                                        }
    ;

// Monadic operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
MonOp:
                       OP1              {DbgMsgW2 (L"%%MonOp:  OP1");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp1 = MakeOp1_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYOp1)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYOp1, 1, FALSE);  // Monadic operator (Direct)
                                             FreeResult (&lpplLocalVars->lpYYOp1->tkToken); YYFree (lpplLocalVars->lpYYOp1); lpplLocalVars->lpYYOp1 = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |                  NAMEOP1          {DbgMsgW2 (L"%%MonOp:  NAMEOP1");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn = MakeNameOp1_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYFcn)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYFcn, 1, FALSE);  // Monadic operator (Direct)
                                             FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |              MonOpAxis            {DbgMsgW2 (L"%%MonOp:  MonOpAxis");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |              '>' MonOp '('        {DbgMsgW2 (L"%%MonOp:  (MonOp)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |              '>' Op1Spec '('      {DbgMsgW2 (L"%%MonOp:  (Op1Spec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, FALSE);      // Monadic operator (Direct)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Monadic operator with axis
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
MonOpAxis:
      '}' error   '['  MonOp            {DbgMsgW2 (L"%%MonOpAxis:  MonOp[error]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$4, NAMETYPE_FN12, FALSE);
                                             FreeResult (&$4.tkToken);

                                             if (lpplLocalVars->lpYYFcn)             // If defined, free it
                                             {
                                                 FreeYYFcn1 (lpplLocalVars->lpYYFcn); lpplLocalVars->lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  MonOp            {DbgMsgW2 (L"%%MonOpAxis:  MonOp[ArrExpr]");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$4, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$4.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;

                                             lpplLocalVars->lpYYAxis = MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYAxis)           // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYAxis, 1, FALSE); // Axis value (Direct)
                                             FreeResult (&lpplLocalVars->lpYYAxis->tkToken); YYFree (lpplLocalVars->lpYYAxis); lpplLocalVars->lpYYAxis = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Dyadic operator
// Skip Ctrl-Break checking here so the Function Strand processing isn't interrupted
DydOp:
                       OP2              {DbgMsgW2 (L"%%DydOp:  OP2");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 = MakeOp2_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYOp2, 1, FALSE);  // Dyadic operator (Direct)
                                             FreeResult (&lpplLocalVars->lpYYOp2->tkToken); YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |                  NAMEOP2          {DbgMsgW2 (L"%%DydOp:  NAMEOP2");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYOp2 = MakeNameOp2_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYOp2)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (lpplLocalVars->lpYYOp2, 1, FALSE);  // Dyadic operator (Direct)
                                             FreeResult (&lpplLocalVars->lpYYOp2->tkToken); YYFree (lpplLocalVars->lpYYOp2); lpplLocalVars->lpYYOp2 = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |              '#' DydOp '('        {DbgMsgW2 (L"%%DydOp:  (DydOp)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |              '#' Op2Spec '('      {DbgMsgW2 (L"%%DydOp:  (Op2Spec)");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, FALSE);      // Dyadic operator (Direct)
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             // The result is always the root of the function tree
                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Index list w/brackets, allowing for A[A][A]...
ILBR:
           ']' ILWE '['                 {DbgMsgW2 (L"%%ILBR:  [ILWE]");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYLst = MakeList_EM_YY (&$2, TRUE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYLst)             // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYLst->tkToken); YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst = MakeList_EM_YY (&$2, TRUE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYLst; YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;
                                         } // End IF
                                        }
    | ILBR ']' ILWE '['                 {DbgMsgW2 (L"%%ILBR:  [ILWE] ILBR ");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYLst = MakeList_EM_YY (&$3, TRUE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYLst)             // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYLst->tkToken); YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst = MakeList_EM_YY (&$3, TRUE);
                                             FreeResult (&$3.tkToken);

                                             if (lpplLocalVars->lpYYLst)             // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYLst->tkToken); YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;
                                             } // End IF




                                             PrimFnNonceError_EM (&$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
    ;

// Index list w/parens
ILPAR:
      ')' ILNE '('                      {DbgMsgW2 (L"%%ILPAR:  (ILNE)");
                                         if (lpplLocalVars->bCtrlBreak)
                                         {
                                             lpplLocalVars->lpYYLst = MakeList_EM_YY (&$2, TRUE);
                                             FreeResult (&$2.tkToken);

                                             if (lpplLocalVars->lpYYLst)             // If defined, free it
                                             {
                                                 FreeResult (&lpplLocalVars->lpYYLst->tkToken); YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst = MakeList_EM_YY (&$2, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYLst; YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;
                                         } // End IF
                                        }
    ;

// Index list, no empties
// Skip Ctrl-Break checking here so the List processing isn't interrupted
ILNE:
      error   ';' ArrExpr               {DbgMsgW2 (L"%%ILNE:  ArrExpr;error");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ';' ArrExpr               {DbgMsgW2 (L"%%ILNE:  ArrExpr;ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYLst =
                                             InitList1_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYLst)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpplLocalVars->lpYYRes =
                                             PushList_YY (lpplLocalVars->lpYYLst, &$3);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&lpplLocalVars->lpYYLst->tkToken); YYFree (lpplLocalVars->lpYYLst); lpplLocalVars->lpYYLst = NULL;

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ILNE    ';' error                 {DbgMsgW2 (L"%%ILNE:  error;ILNE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ILNE    ';' ArrExpr               {DbgMsgW2 (L"%%ILNE:  ArrExpr;ILNE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushList_YY (&$1, &$3);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Index list, with empties
// Skip Ctrl-Break checking here so the List processing isn't interrupted
ILWE:
      /* Empty */                       {DbgMsgW2 (L"%%ILWE:  <empty>");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             InitList0_YY ();

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    |          error                    {DbgMsgW2 (L"%%ILWE:  error");
                                         if (!lpplLocalVars->bLookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    |          ArrExpr                  {DbgMsgW2 (L"%%ILWE:  ArrExpr");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             InitList1_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ILWE ';' error                    {DbgMsgW2 (L"%%ILWE:  error;ILWE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ILWE ';' ArrExpr                  {DbgMsgW2 (L"%%ILWE:  ArrExpr;ILWE");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushList_YY (&$1, &$3);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    | ILWE ';'                          {DbgMsgW2 (L"%%ILWE:  ILWE;");
                                         if (!lpplLocalVars->bLookAhead)
                                         {
                                             lpplLocalVars->lpYYRes =
                                             PushList_YY (&$1, NULL);
                                             FreeResult (&$1.tkToken);

                                             if (!lpplLocalVars->lpYYRes)            // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpplLocalVars->lpYYRes; YYFree (lpplLocalVars->lpYYRes); lpplLocalVars->lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// A data expression which has a single token
SingTokn:
      CONSTANT                          {DbgMsgW2 (L"%%SingTokn:  CONSTANT");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgW2 (L"%%SingTokn:  NAMEVAR");
                                         if (!lpplLocalVars->bLookAhead)
                                             $$ = $1;
                                        }
    | USRFN0                            {DbgMsgW2 (L"%%SingTokn:  USRFN0");
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
    | SYSFN0                            {DbgMsgW2 (L"%%SingTokn:  SYSFN0");
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
    | STRING                            {DbgMsgW2 (L"%%SingTokn:  STRING");
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

//***************************************************************************
//  Start of C program
//***************************************************************************

typedef struct tagPL_THREAD
{
    HGLOBAL hGlbTxtLine,        // 00:  Line text global memory handle (may be NULL)
            hGlbToken;          // 04:  Tokenized line global memory handle
    LPWCHAR lpwszLine;          // 08:  Ptr to tokens of line to parse (GlobalLock of above) (may be NULL)
    HGLOBAL hGlbPTD;            // 0C:  Handle of PerTabData for this thread
    HWND    hWndSM;             // 10:  Window handle to Session Manager
                                // 14:  Length
} PL_THREAD, *LPPL_THREAD;

PL_THREAD plThread;             // Temporary global


//***************************************************************************
//  $ParseLine
//
//  Parse a line
//***************************************************************************

#ifdef debug
#define APPEND_NAME     L" -- ParseLine"
#else
#define APPEND_NAME
#endif

void ParseLine
    (HWND    hWndSM,            // Session Manager window handle
     HGLOBAL hGlbTxtLine,       // Line text global memory handle
     HGLOBAL hGlbToken,         // Tokenized line global memory handle (may be NULL)
     LPWCHAR lpwszLine,         // Ptr to the line text (may be NULL)
     HGLOBAL hGlbPTD)           // PerTabData global memory handle

{
    HANDLE hThread;             // Thread handle
    DWORD  dwThreadId;          // Thread ID

    // Save args in struc to pass to thread func
    plThread.hGlbTxtLine = hGlbTxtLine;
    plThread.hGlbToken   = hGlbToken;
    plThread.lpwszLine   = lpwszLine;
    plThread.hGlbPTD     = hGlbPTD;
    plThread.hWndSM      = hWndSM;

    // Create a new thread
    hThread = CreateThread (NULL,               // No security attrs
                            0,                  // Use default stack size
                           &ParseLineInThread,  // Starting routine
                           &plThread,           // Param to thread func
                            0,                  // Creation flag
                           &dwThreadId);        // Returns thread id
    // Wait for the thread to terminate
    WaitForSingleObject (hThread,               // Ptr to handle to wait for
                         INFINITE);             // Timeout value in milliseconds
    // Close the thread handle as it has terminated
    CloseHandle (hThread); hThread = NULL;
} // End ParseLine
#undef  APPEND_NAME


//***************************************************************************
//  $ParseLineInThread
//
//  Parse a line in a thread
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseLineInThread"
#else
#define APPEND_NAME
#endif

DWORD WINAPI ParseLineInThread
    (LPPL_THREAD lpplThread)

{
    HGLOBAL      hGlbTxtLine,       // Line text global memory handle
                 hGlbToken;         // Tokenized line global memory handle
    LPWCHAR      lpwszLine;         // Ptr to WCHARs in the line to parse
    HGLOBAL      hGlbPTD;           // Handle to PerTabData
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    PLLOCALVARS  plLocalVars = {0}; // Re-entrant vars
    HWND         hWndSM;            // Window handle to Session Manager
    UINT         uError;            // Error code
    BOOL         bRet;              // TRUE iff result is valid

    // Save the thread type ('PL')
    TlsSetValue (dwTlsType, (LPVOID) 'PL');

    // Save the thread's ptr to local vars
    TlsSetValue (dwTlsPlLocalVars, (LPVOID) &plLocalVars);

    // Extract values from the arg struc
    hGlbTxtLine = lpplThread->hGlbTxtLine;
    hGlbToken   = lpplThread->hGlbToken;
    lpwszLine   = lpplThread->lpwszLine;
    hGlbPTD     = lpplThread->hGlbPTD;
    hWndSM      = lpplThread->hWndSM;

    // Save the thread's PerTabData global memory handle
    TlsSetValue (dwTlsPerTabData, (LPVOID) hGlbPTD);

    DBGENTER;           // Must be placed after the TlsSetValue for hGlbPTD
#ifdef DEBUG
    dprintfW (L"--Starting thread in <ParseLineInThread>.");
#endif
RESTART_FROM_ERROR:
    // Initialize the error code
    uError = ERRORCODE_NONE;

    // If we don't have a valid handle, ...
    if (!hGlbToken)
        return NEG1U;
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

    // Lock the memory to get a ptr to it, and set the variables
    UTLockAndSet (plLocalVars.hGlbToken, &plLocalVars.t2);

    // Skip over TOKEN_HEADER
    plLocalVars.lpStart = TokenBaseToStart (plLocalVars.t2.lpBase);

    // Skip to end of 1st stmt
    plLocalVars.lpNext  = &plLocalVars.lpStart[plLocalVars.lpStart->tkData.tkIndex];

    // Start off with no error
    plLocalVars.tkErrorCharIndex = NEG1U;

    // Allocate virtual memory for the Variable Strand accumulator
    plLocalVars.lpYYStrandStart[STRAND_VAR] =
        VirtualAlloc (NULL,      // Any address
                      DEF_STRAND_MAXSIZE * sizeof (PL_YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!plLocalVars.lpYYStrandStart[STRAND_VAR])
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ParseLine:  VirtualAlloc for <plLocalVars.lpYYStrandStart[STRAND_VAR]> failed");

        goto ERROR_EXIT;
    } // End IF

    // Commit the intial size
    VirtualAlloc (plLocalVars.lpYYStrandStart[STRAND_VAR],
                  DEF_STRAND_INITSIZE * sizeof (PL_YYSTYPE),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Allocate virtual memory for the Function Strand accumulator
    plLocalVars.lpYYStrandStart[STRAND_FCN] =
        VirtualAlloc (NULL,      // Any address
                      DEF_STRAND_MAXSIZE * sizeof (PL_YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!plLocalVars.lpYYStrandStart[STRAND_FCN])
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ParseLine:  VirtualAlloc for <pLocalVars.lpYYStrandStart[STRAND_FCN]> failed");

        goto ERROR_EXIT;
    } // End IF

    // Commit the intial size
    VirtualAlloc (plLocalVars.lpYYStrandStart[STRAND_FCN],
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
    yydebug = 1;
#endif

    // Parse the file, check for errors
    //   0 = success
    //   1 = YYABORT or APL error
    //   2 = memory xhausted
    bRet = pl_yyparse (&plLocalVars);

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

        // Peel back to first defined function
        for (lpSISCur = lpMemPTD->lpSISCur;
             lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt;
             lpSISCur = lpSISCur->lpSISPrv)
        if (lpSISCur->DfnType EQ DFNTYPE_OP1
         || lpSISCur->DfnType EQ DFNTYPE_OP2
         || lpSISCur->DfnType EQ DFNTYPE_FCN)
            break;

        // If called from a defined function/operator, ...
        if (lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt)
            // Signal an error
            BreakMessage (hWndSM, lpSISCur);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF

    if (bRet EQ 0 || uError EQ ERRORCODE_ALX)
        goto NORMAL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If called from Immediate Execution/Execute, ...
    if (lpwszLine)
        // Signal an error
        ErrorMessage (lpMemPTD->lpwszErrorMessage,
                      lpwszLine,
                      plLocalVars.tkErrorCharIndex,
                      hWndSM);
    else
    {
        LPMEMTXTUNION lpMemTxtLine;

        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

        // Signal an error
        ErrorMessage (lpMemPTD->lpwszErrorMessage,
                     &lpMemTxtLine->C,
                      plLocalVars.tkErrorCharIndex,
                      hWndSM);
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
    } // End IF/ELSE

    // Mark as in error
    uError = ERRORCODE_ELX;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    goto NORMAL_EXIT;

ERROR_EXIT:
    DbgBrk ();

    // Signal an error
    ErrorMessage (ERRMSG_WS_FULL L"-- ParseLineInThread",
                  L"",
                  NEG1U,
                  hWndSM);

    // Execute []ELX
    plLocalVars.lpYYRes = PrimFnMonUpTackJotCommon_EM_YY (WS_UTF16_QUAD L"ELX", FALSE);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    lpMemPTD->YYResExec = *plLocalVars.lpYYRes;
    YYFree (plLocalVars.lpYYRes); plLocalVars.lpYYRes = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
NORMAL_EXIT:
    if (plLocalVars.lpYYStrandStart[STRAND_FCN])
    {
        VirtualFree (plLocalVars.lpYYStrandStart[STRAND_FCN], 0, MEM_RELEASE); plLocalVars.lpYYStrandStart[STRAND_FCN] = NULL;
    } // End IF

    if (plLocalVars.lpYYStrandStart[STRAND_VAR])
    {
        VirtualFree (plLocalVars.lpYYStrandStart[STRAND_VAR], 0, MEM_RELEASE); plLocalVars.lpYYStrandStart[STRAND_VAR] = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (plLocalVars.hGlbToken); plLocalVars.t2.lpBase   = NULL;

    // If there's an error to be signalled, ...
    if (uError NE ERRORCODE_NONE)
    {
        HWND hWndEC;            // Edit Control window handle

        // Get the Edit Control window handle
        hWndEC = (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC);
#ifdef DEBUG
        if (uError EQ ERRORCODE_ELX)
            DbgMsgW (L"~~Start []ELX on");
        else
            DbgMsgW (L"~~Start []ALX on");

        if (lpwszLine)
            DbgMsgW (lpwszLine);
        else
        {
            LPMEMTXTUNION lpMemTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Display the function line
            DbgMsgW (&lpMemTxtLine->C);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF/ELSE
#endif
        // Mark as not executing function line
        hGlbTxtLine = NULL;

        // Set the text for the line
        if (uError EQ ERRORCODE_ELX)
            lpwszLine = WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"ELX";
        else
            lpwszLine = WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"ALX";
        // Tokenize the line
        hGlbToken = Tokenize_EM (lpwszLine,
                                 lstrlenW (lpwszLine),
                                 hWndEC,
                                &ErrorMessage);
        goto RESTART_FROM_ERROR;
    } // End IF

#ifdef DEBUG
    dprintfW (L"--Ending   thread in <ParseLineInThread>.");
#endif
    DBGLEAVE;

    return 0;
} // End ParseLineInThread
#undef  APPEND_NAME


//***************************************************************************
//  $SymbTypeVFO
//
//  Get the type ('V', 'F', '1', '2', or '?') of a symbol table name token.
//  Used for TKT_VARNAMEDs.
//***************************************************************************

char SymbTypeVFO
    (LPTOKEN lpNext)

{
    STFLAGS stFlags;

    // Get the symbol table flags
    stFlags = lpNext->tkData.tkSym->stFlags;

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

        defstop
            return '?';
    } // End IF/SWITCH

    if (IsNameTypeVar (stFlags.ObjType)
     || stFlags.ObjType EQ NAMETYPE_FN0)
        return 'V';

    // Note we must perform the following test
    //   AFTER the one above so as not to catch the FN0 case.
    if (IsNameTypeFn (stFlags.ObjType))
        return 'F';

    if (stFlags.ObjType EQ NAMETYPE_OP1)
        return '1';

    if (stFlags.ObjType EQ NAMETYPE_OP2)
        return '2';

    // After having checked all the other .ObjType possibilities,
    //   if it's a OBJNAME_NONE, then it's really a VALUE ERROR, but this
    //   isn't the time to signal that.
    if (stFlags.ObjName EQ OBJNAME_NONE)
        return 'V';

    return '?';             // SYNTAX ERROR
} // SymbTypeVFO


//***************************************************************************
//  $LookaheadSurround
//
//  Lookahead (backwards) in the token stream to see if
//    the next non-grouping symbol is a function or a var.
//  Used for TKT_RPARENs to see if it and the matching
//    TKT_LPAREN surrounds a function, variable, monadic or dyadic
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
    PLLOCALVARS plLocalVars;    // Local copy of outer PLLOCALVARS
    char        cRes;           // The result char

////DbgMsgW (L"==Entering LookaheadSurround");

    // Copy outer lpplLocalVars
    plLocalVars = *lpplLocalVars;

    // Mark as looking ahead for token type inside surrounding parens
    plLocalVars.bLookAhead = TRUE;

    // Save the address of the stop token
    // Each right grouping symbol has the index of the matching
    //   left grouping symbol in its tkData.tkIndex
    plLocalVars.lpStop = &plLocalVars.lpStart[plLocalVars.lpNext->tkData.tkIndex];

    // Parse the file, check for errors
    if (pl_yyparse (&plLocalVars))
        cRes = '?';
    else
        cRes = NAMETYPE_STRING[plLocalVars.NameType];

#ifdef DEBUG
////dprintfW (L"==Exiting  LookaheadSurround:  %c", cRes);
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
//  Used for TKT_RBRACKETs to see if this is indexing or axis.
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

    DbgMsgW (L"==Entering LookaheadAdjacent");

    // Copy outer lpplLocalVars
    plLocalVars = *lpplLocalVars;

    // Each right grouping symbol has the index of the matching
    //   left grouping symbol in its tkData.tkIndex.
    // Get a ptr to the token adjacent to ("-1") the matching left
    //   grouping symbol.
    if (bSkipBrackets)
        plLocalVars.lpNext = &plLocalVars.lpStart[plLocalVars.lpNext->tkData.tkIndex - 1];
    else
        plLocalVars.lpNext--;   // Skip over the function/operator symbol
    while (TRUE)
    switch (plLocalVars.lpNext->tkFlags.TknType)
    {
        case TKT_STRING:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
        case TKT_INPOUT:
            // Lookahead to see if the next token is a dyadic operator:
            //   if so, return 'F'; otherwise return 'V'.
            switch (LookaheadDyadicOp (&plLocalVars, &plLocalVars.lpNext[-1]))
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
            cRes = '1';

            goto NORMAL_EXIT;

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
            cRes = '2';

            goto NORMAL_EXIT;

        case TKT_FCNIMMED:
        case TKT_FCNARRAY:
        case TKT_FCNNAMED:
            cRes = 'F';             // Function

            goto NORMAL_EXIT;

        case TKT_VARNAMED:
            // Get the token type of the symbol table name
            cRes = SymbTypeVFO (plLocalVars.lpNext);

            goto NORMAL_EXIT;

        case TKT_RBRACKET:
            // Each right grouping symbol has the index of the matching
            //   left grouping symbol in its tkData.tkIndex
            // Get a ptr to the token adjacent to ("-1") the matching left
            //   grouping symbol.
            plLocalVars.lpNext = &plLocalVars.lpStart[plLocalVars.lpNext->tkData.tkIndex - 1];

            break;                  // Go around again

        case TKT_RPAREN:
            cRes = LookaheadSurround (&plLocalVars);

            goto NORMAL_EXIT;

        case TKT_LINECONT:
        case TKT_COMMENT:
            plLocalVars.lpNext--;   // Ignore these tokens

            break;                  // Go around again

        case TKT_ASSIGN:            // To allow f{is}/[1]
            if (!bSkipBrackets)
            {
                cRes = 'F';

                goto NORMAL_EXIT;
            } // End IF

            // Fall through to common code

        case TKT_LISTSEP:
        case TKT_COLON:
        case TKT_LPAREN:
        case TKT_LBRACKET:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_STRAND:
        case TKT_AXISIMMED:
        case TKT_AXISARRAY:
        case TKT_LISTINT:
        case TKT_LISTPAR:
        case TKT_LISTBR:
            cRes = '?';             // SYNTAX ERROR

            goto NORMAL_EXIT;

        defstop
            cRes = '?';

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
     LPTOKEN       lpNext)

{
    BOOL bRet;          // The result

    DbgMsgW (L"==Entering LookaheadDyadicOp");

    while (TRUE)
    switch (lpNext->tkFlags.TknType)
    {
        case TKT_STRING:
        case TKT_VARIMMED:
        case TKT_COMMENT:
        case TKT_LPAREN:
        case TKT_FCNIMMED:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_COLON:
        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
        case TKT_LBRACKET:
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
            bRet = (lpNext->tkData.tkSym->stFlags.ObjType EQ NAMETYPE_OP2);

            goto NORMAL_EXIT;

        case TKT_RPAREN:
        case TKT_RBRACKET:
            // Each right grouping symbol has the index of the matching
            //   left grouping symbol in its tkData.tkIndex
            // Get a ptr to the token adjacent to ("-1") the matching left
            //   grouping symbol.
            lpNext = &lpplLocalVars->lpStart[lpNext->tkData.tkIndex - 1];

            break;                  // Go around again

        case TKT_LINECONT:
            lpNext--;               // Ignore this token

            break;                  // Go around again

        case TKT_STRAND:
        defstop
            break;
    } // End WHILE/SWITCH
NORMAL_EXIT:
    DbgMsgW (L"==Exiting  LookaheadDyadicOp");

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
    static UINT YYIndex = 0;
#endif
    WCHAR       wchFn;

    // Because we're parsing the stmt from right to left
    lpplLocalVars->lpNext--;

    // Return the current token
    lpYYLval->tkToken    = *lpplLocalVars->lpNext;
    lpYYLval->YYInuse    =
    lpYYLval->YYIndirect =
    lpYYLval->Avail      =
    lpYYLval->TknCount   =
    lpYYLval->FcnCount   = 0;
    lpYYLval->lpYYFcn    = NULL;
#ifdef DEBUG
    lpYYLval->YYFlag     = 1;         // Mark as a pl_yylex Index
    lpYYLval->YYIndex    = ++YYIndex;
#endif

    // Split cases based upon the token type
    switch (lpplLocalVars->lpNext->tkFlags.TknType)
    {
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            return CONSTANT;

        case TKT_INPOUT:
            if (lpplLocalVars->lpNext->tkData.tkChar EQ UTF16_QUAD)
                return QUAD;
            else
                return QUOTEQUAD;

        case TKT_FCNNAMED:
            return NAMEFCN;

        case TKT_VARNAMED:
        {
            STFLAGS stFlags;        // STE flags

            // Get the STE flags
            stFlags = lpplLocalVars->lpNext->tkData.tkSym->stFlags;

            // Test various cases mostly vased on the ObjType
            if (IsNameTypeVar (stFlags.ObjType))
            {
////////////////lpYYLval->tkToken.tkFlags.TknType                   =
////////////////lpplLocalVars->lpNext->lptkOrig->tkFlags.TknType    =
////////////////lpplLocalVars->lpNext->tkFlags.TknType              = TKT_VARNAMED;    // Already set

                return NAMEVAR;
            } else
            if (stFlags.ObjType EQ NAMETYPE_FN0)
            {
                lpYYLval->tkToken.tkFlags.TknType                   =
                lpplLocalVars->lpNext->lptkOrig->tkFlags.TknType    =
                lpplLocalVars->lpNext->tkFlags.TknType              = TKT_VARNAMED; // Instead of TKT_FCNNAMED because it'll be executed right away
                lpplLocalVars->lpNext->tkData.tkSym->stFlags.FcnDir = (stFlags.ObjName EQ OBJNAME_SYS);

                if (stFlags.ObjName EQ OBJNAME_SYS)
                    return SYSFN0;
                else
                    return USRFN0;
            } else
            if (stFlags.ObjType EQ NAMETYPE_FN12)
            {
                lpYYLval->tkToken.tkFlags.TknType                   =
                lpplLocalVars->lpNext->lptkOrig->tkFlags.TknType    =
                lpplLocalVars->lpNext->tkFlags.TknType              = TKT_FCNNAMED;
                lpplLocalVars->lpNext->tkData.tkSym->stFlags.FcnDir = (stFlags.ObjName EQ OBJNAME_SYS);

                if (stFlags.ObjName EQ OBJNAME_SYS)
                    return SYSFN12;
                else
                    return NAMEFCN;
            } else
            if (stFlags.ObjType EQ NAMETYPE_OP1)
            {
                lpYYLval->tkToken.tkFlags.TknType                   =
                lpplLocalVars->lpNext->lptkOrig->tkFlags.TknType    =
                lpplLocalVars->lpNext->tkFlags.TknType              = TKT_OP1NAMED;

                return NAMEOP1;
            } else
            if (stFlags.ObjType EQ NAMETYPE_OP2)
            {
                lpYYLval->tkToken.tkFlags.TknType                   =
                lpplLocalVars->lpNext->lptkOrig->tkFlags.TknType    =
                lpplLocalVars->lpNext->tkFlags.TknType              = TKT_OP2NAMED;

                return NAMEOP2;
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

        case TKT_COLON:
            return ':';

        case TKT_FCNIMMED:
            if (lpplLocalVars->lpNext->tkData.tkIndex EQ UTF16_RIGHTARROW)
                return GOTO;
            else
                return PRIMFCN;

        case TKT_COMMENT:
        case TKT_LINECONT:
            return pl_yylex (lpYYLval, lpplLocalVars); // Ignore these tokens

        case TKT_STRING:
            return STRING;

        case TKT_OP1IMMED:
            // Get the symbol
            wchFn = lpplLocalVars->lpNext->tkData.tkChar;

            // Handle the ambiguous slash(-bar) and slope(-bar) symbols
            if (wchFn EQ UTF16_SLASH
             || wchFn EQ UTF16_SLOPE
             || wchFn EQ UTF16_SLASHBAR
             || wchFn EQ UTF16_SLOPEBAR)
            {
                // We really need an LALR grammar for this
                switch (LookaheadAdjacent (lpplLocalVars, FALSE))
                {
                    case '2':               // If the next token is a dyadic operator,
                        return PRIMFCN;     //   then this token is a function

                    case '1':               // If the next token is a monadic operator,
                    case 'F':               // If the next token is a function,
                        // Replace the function symbol with the
                        //   corresponding operator symbol

                        // Split cases based upon the function symbol
                        switch (wchFn)
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

                        return OP1;         //   then this token is an operator

                    case 'V':               // If the next token is a variable
                    case '0':               //   or a niladic function,
                        return PRIMFCN;     //   then this token is a function

                    case '?':               // SYNTAX ERROR
                        return UNK;

                    defstop
                        return UNK;
                } // End SWITCH
            } // End IF

            return OP1;

        case TKT_OP2IMMED:
            return OP2;

        case TKT_OPJOTDOT:
            return JOTDOT;

        case TKT_LPAREN:
            // If we're in LookaheadSurround and this is the stop token, mark as EOL
            if (lpplLocalVars->bLookAhead
             && lpplLocalVars->lpStop EQ lpplLocalVars->lpNext)
                return EOL;
            else
                return '(';

        case TKT_RPAREN:
            // Lookahead to see if this right grouping symbol surrounds
            //   a function (return '>'), a variable (return ')'), or
            //   a dyadic operator (return '#')

            // We really need an LALR grammar for this
            switch (LookaheadSurround (lpplLocalVars))
            {
                case '2':               // Dyadic operator
                    return '#';

                case '1':               // Monadic operator
                case 'F':               // Function
                    return '>';

                case 'V':               // Variable
                case '0':               // Niladic function
                    return ')';

                case '?':               // SYNTAX ERROR
                    return UNK;

                defstop
                    return UNK;
            } // End SWITCH

        case TKT_LBRACKET:
            return '[';

        case TKT_RBRACKET:
            // Lookahead to see if this right grouping symbol is adjacent to
            //   a function (return '}') or a variable (return ']')

            // We really need an LALR grammar for this
            switch (LookaheadAdjacent (lpplLocalVars, TRUE))
            {
                case '1':               // If the next token is a monadic operator
                case 'F':               //   or a function,
                    return '}';         //   then this token is an axis operator

                case 'V':               // If the next token is a variable
                case '0':               //   or a niladic function,
                    return ']';         //   then this token is an indexing symbol

                case '2':               // If the next token is a dyadic operator,
                case '?':               // SYNTAX ERROR
                    return UNK;

                defstop
                    return UNK;
            } // End SWITCH

        case TKT_EOS:
            // Skip to end of this stmt
            lpplLocalVars->lpNext = &lpplLocalVars->lpNext[lpplLocalVars->lpNext->tkData.tkIndex];
            lpplLocalVars->lpNext = &lpplLocalVars->lpNext[lpplLocalVars->lpNext->tkData.tkIndex];

            return DIAMOND;

        case TKT_EOL:
            return '\0';

        case TKT_STRAND:
        defstop
            return UNK;
    } // End SWITCH
} // End pl_yylex


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

    lpplLocalVars->tkErrorCharIndex = lpplLocalVars->lpNext[1].tkCharIndex;

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
///#ifdef DEBUG
#if FALSE
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

    wsprintf (lpszDebug,
              lpszFmt,
              i1, i2, i3);
    // Accumulate into local buffer because
    //   Bison calls this function multiple
    //   times for the same line, terminating
    //   the last call for the line with a LF.
    lstrcat (szTemp, lpszDebug);

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

    va_end (vl);
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
//  End of File: parse.y
//***************************************************************************
