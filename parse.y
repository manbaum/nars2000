//***************************************************************************
//  NARS2000 -- Parser Grammar
//***************************************************************************

/****************************************************************************

Parse a line of pre-tokenized tokens.

Based upon "The Syntax of APL:  An Old Approach Revisited" by
Jean Jacques Giardot & Florence Rollin, ACM SIGAPL Quote-Quad APL 1987
modified to work as an LALR grammar with the lookahead embedded
in the lexical analyser (yylex).

****************************************************************************/

%{
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "Unicode.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF yyfprintf

LPYYSTYPE lpYYStr, lpYYRes, lpYYFcn, lpYYLst;
BOOL      bRet;

#define DbgMsgW2(a) DbgMsgW(a)
////#define DbgMsgW2(a) DbgMsgW(a); DbgBrk ()

%}

%token UNK
%token NAMEVAR NAMEUNK CONSTANT STRING FN0
%token DIAMOND

/*  Note that as we parse APL from right to left, these rules
    are all reversed as well as token associativity (long scope).
    That is, functions in APL have long right scope, so they are
    declared here in the %left list.  Similarly, operators in APL
    have long left scope, so they are declared in the %right list.
 */
%left  ASSIGN PRIMFCN NAMEFCN GOTO
%right NAMEOP1 OP1 NAMEOP2 OP2 JOTDOT

%start Stmts

%%

/*  ToDo
    ----
    * Control structures
    * J/Dyalog's dynamic functions
    *

    Done
    ----
    * Outer product
    * Distinguish between / & \ as functions vs. operators
    * Selective assignment, also using bracketed indexing
    * Axis operator -- e.g.,   +/[2]
    * Internal assignment -- e.g.,   (a {is} 1) + 2
    * Composite assignment -- e.g.,  A[IL] +{is} A
                                 and A +{is} A
    * Function assignment -- e.g.,   F {is} +/[2]
    * Sink
    * Lists as in '...' #fmt (...;...;...)

    Notes
    ----
    * A monadic operator with a var left arg must be
      enclosed in parens, such as (1 2 op1) because
      1 2 op1 can be interpreted as 1 (2 op1) as well
      as (1 2) op1.
    * A dyadic operator with either argument as a var
      must be enclosed in parens for the same reasons
      as above, otherwise 1 2 op2 3 4 is way too ambiguous.
 */

/* Statements */
Stmts:
      // All errors propogate up to this point where we ABORT which ensures
      //   that the call to yyparse terminates with a non-zero error code.
      error                             {DbgMsgW2 (L"%%Stmts:  error"); YYABORT;}
    | Stmt                              {DbgMsgW2 (L"%%Stmts:  Stmt");}
    | Stmts DIAMOND Stmt                {DbgMsgW2 (L"%%Stmts:  Stmts " WS_UCS2_DIAMOND L" Stmt");}
    ;

/* Statement */
Stmt:     /* Empty */                   {DbgMsgW2 (L"%%Stmt:  <empty>");}
    | ArrExpr                           {DbgMsgW2 (L"%%Stmt:  ArrExpr");
                                         bRet = ArrayDisplay_EM (&$1);
                                         FreeResult (&$1);
                                         if (!bRet)
                                             YYERROR;
                                        }
    | ArrExpr GOTO                      {DbgMsgW2 (L"%%Stmt:  " WS_UCS2_RIGHTARROW L"ArrExpr");
                                         FreeResult (&$1);




                                         NonceError (&$1); YYERROR;
                                        }
    |         GOTO                      {DbgMsgW2 (L"%%Stmt:  " WS_UCS2_RIGHTARROW);




                                         NonceError (&$1); YYERROR;
                                        }
    | ArrExpr ASSIGN                    {DbgMsgW2 (L"%%Stmt:  " WS_UCS2_LEFTARROW L"ArrExpr");
                                         FreeResult (&$1);}
    | FcnSpec                           {DbgMsgW2 (L"%%Stmt:  FcnSpec");
                                         $$ = $1;}
    | Op1Spec                           {DbgMsgW2 (L"%%Stmt:  Op1Spec");
                                         $$ = $1;}
    | Op2Spec                           {DbgMsgW2 (L"%%Stmt:  Op2Spec");
                                         $$ = $1;}
    ;

NameAnyVar:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyVar:  NAMEUNK");
                                         $$ = $1;}
    | NAMEVAR                           {DbgMsgW2 (L"%%NameAnyVar:  NAMEVAR");
                                         $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                         $$ = $1;}
    ;

NameAnyFcn:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyFcn:  NAMEUNK");
                                         $$ = $1;}
    | NAMEFCN                           {DbgMsgW2 (L"%%NameAnyFcn:  NAMEFCN");
                                         $1.tkToken.tkFlags.TknType = TKT_FCNNAMED;
                                         $$ = $1;}
    ;

NameAnyOp1:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEUNK");
                                         $$ = $1;}
    | NAMEOP1                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEOP1");
                                         $1.tkToken.tkFlags.TknType = TKT_OP1NAMED;
                                         $$ = $1;}
    ;

NameAnyOp2:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyOp2:  NAMEUNK");
                                         $$ = $1;}
    | NAMEOP2                           {DbgMsgW2 (L"%%NameAnyOp2:  NAMEOP2");
                                         $1.tkToken.tkFlags.TknType = TKT_OP2NAMED;
                                         $$ = $1;}
    ;

// Function specification
FcnSpec:
      LeftFunc ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UCS2_LEFTARROW L"LeftFunc");
                                         lpYYFcn = MakeFcnStrand_EM (&$1, FCNTYPE_FCN);
                                         if (!lpYYFcn)
                                            YYERROR;
                                         bRet = AssignName_EM (&$3, lpYYFcn);
                                         FreeResult (lpYYFcn);
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;}
    | AxisFunc ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UCS2_LEFTARROW L"AxisFunc");
                                         lpYYFcn = MakeFcnStrand_EM (&$1, FCNTYPE_AXISFCN);
                                         if (!lpYYFcn)
                                            YYERROR;
                                         bRet = AssignName_EM (&$3, lpYYFcn);
                                         FreeResult (lpYYFcn);
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;}
    ;

// Monadic operator specification
Op1Spec:
      MonOp    ASSIGN NameAnyOp1        {DbgMsgW2 (L"%%Op1Spec:  NameAny" WS_UCS2_LEFTARROW L"MonOp");
                                         lpYYFcn = MakeFcnStrand_EM (&$1, FCNTYPE_OP1);
                                         if (!lpYYFcn)
                                            YYERROR;
                                         bRet = AssignName_EM (&$3, lpYYFcn);
                                         FreeResult (lpYYFcn);
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;}
    | MonOpAxis ASSIGN NameAnyOp1       {DbgMsgW2 (L"%%Op1Spec:  NameAny" WS_UCS2_LEFTARROW L"MonOpAxis");
                                         lpYYFcn = MakeFcnStrand_EM (&$1, FCNTYPE_OP1);
                                         if (!lpYYFcn)
                                            YYERROR;
                                         bRet = AssignName_EM (&$3, lpYYFcn);
                                         FreeResult (lpYYFcn);
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;}
    ;

// Dyadic operator specification
Op2Spec:
      DydOp    ASSIGN NameAnyOp2        {DbgMsgW2 (L"%%Op2Spec:  NameAny" WS_UCS2_LEFTARROW L"DydOp");
                                         lpYYFcn = MakeFcnStrand_EM (&$1, FCNTYPE_OP2);
                                         if (!lpYYFcn)
                                            YYERROR;
                                         bRet = AssignName_EM (&$3, lpYYFcn);
                                         FreeResult (lpYYFcn);
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;}
    ;

/* Array expression */
ArrExpr:
      SimpExpr                          {DbgMsgW2 (L"%%ArrExpr:  SimpExpr");
                                         $$ = $1;}
    | error   LeftFunc                  {DbgMsgW2 (L"%%ArrExpr:  LeftFunc error");
                                         lpYYFcn = MakeFcnStrand_EM (&$2, FCNTYPE_FCN);
                                         if (lpYYFcn)
                                             FreeResult (lpYYFcn);
                                         YYERROR;
                                        }
    | ArrExpr LeftFunc                  {DbgMsgW2 (L"%%ArrExpr:  LeftFunc ArrExpr");
                                         lpYYFcn = MakeFcnStrand_EM (&$2, FCNTYPE_FCN);
                                         if (!lpYYFcn)
                                         {
                                            FreeResult (&$1);
                                            YYERROR;
                                         }
                                         lpYYRes = ExecFunc_EM (NULL, lpYYFcn, &$1.tkToken, NULL);
                                         FreeResult (&$1);
                                         FreeResult (lpYYFcn);
                                         if (!lpYYRes)
                                             YYERROR;
                                         $$ = *lpYYRes;}
    | error   AxisFunc                  {DbgMsgW2 (L"%%ArrExpr:  AxisFunc error");
                                         lpYYFcn = MakeFcnStrand_EM (&$2, FCNTYPE_AXISFCN);
                                         if (lpYYFcn)
                                             FreeResult (lpYYFcn);
                                         YYERROR;
                                        }
    | ArrExpr AxisFunc                  {DbgMsgW2 (L"%%ArrExpr:  AxisFunc ArrExpr");
                                         lpYYFcn = MakeFcnStrand_EM (&$2, FCNTYPE_AXISFCN);
                                         if (!lpYYFcn)
                                         {
                                            FreeResult (&$1);
                                            YYERROR;
                                         }
                                         lpYYRes = ExecFunc_EM (NULL, lpYYFcn, &$1.tkToken, NULL);
                                         FreeResult (&$1);
                                         FreeResult (lpYYFcn);
                                         if (!lpYYRes)
                                             YYERROR;
                                         $$ = *lpYYRes;}
    | ArrExpr LeftFunc error            {DbgMsgW2 (L"%%ArrExpr:  error LeftFunc ArrExpr");
                                         lpYYFcn = MakeFcnStrand_EM (&$2, FCNTYPE_FCN);
                                         if (lpYYFcn)
                                            FreeResult (lpYYFcn);
                                         FreeResult (&$1);
                                         YYERROR;
                                        }
    | error   LeftFunc Strand           {DbgMsgW2 (L"%%ArrExpr:  Strand LeftFunc error");
                                         lpYYStr = MakeVarStrand_EM (&$3);
                                         lpYYFcn = MakeFcnStrand_EM (&$2, FCNTYPE_FCN);
                                         if (lpYYStr)
                                             FreeResult (lpYYStr);
                                         if (lpYYFcn)
                                             FreeResult (lpYYFcn);
                                         YYERROR;
                                        }
    | ArrExpr LeftFunc Strand           {DbgMsgW2 (L"%%ArrExpr:  Strand LeftFunc ArrExpr");
                                         lpYYStr = MakeVarStrand_EM (&$3);
                                         lpYYFcn = MakeFcnStrand_EM (&$2, FCNTYPE_FCN);
                                         if (!lpYYStr)
                                         {
                                             if (lpYYFcn)
                                                 FreeResult (lpYYFcn);
                                             FreeResult (&$1);
                                             YYERROR;
                                         }
                                         if (!lpYYFcn)
                                         {
                                             FreeResult (&$1);
                                             FreeResult (lpYYStr);
                                             YYERROR;
                                         }
                                         lpYYRes = ExecFunc_EM (&lpYYStr->tkToken, lpYYFcn, &$1.tkToken, NULL);
                                         FreeResult (&$1);
                                         FreeResult (lpYYFcn);
                                         FreeResult (lpYYStr);
                                         if (!lpYYRes)
                                             YYERROR;
                                         $$ = *lpYYRes;}
    | ArrExpr AxisFunc error            {DbgMsgW2 (L"%%ArrExpr:  error AxisFunc ArrExpr");
                                         lpYYFcn = MakeFcnStrand_EM (&$2, FCNTYPE_AXISFCN);
                                         if (lpYYFcn)
                                             FreeResult (lpYYFcn);
                                         FreeResult (&$1);
                                         YYERROR;
                                        }
    | error   AxisFunc Strand           {DbgMsgW2 (L"%%ArrExpr:  Strand AxisFunc error");
                                         lpYYStr = MakeVarStrand_EM (&$3);
                                         lpYYFcn = MakeFcnStrand_EM (&$2, FCNTYPE_AXISFCN);
                                         if (lpYYStr)
                                             FreeResult (lpYYStr);
                                         if (lpYYFcn)
                                             FreeResult (lpYYFcn);
                                         YYERROR;
                                        }
    | ArrExpr AxisFunc Strand           {DbgMsgW2 (L"%%ArrExpr:  Strand AxisFunc ArrExpr");
                                         lpYYStr = MakeVarStrand_EM (&$3);
                                         lpYYFcn = MakeFcnStrand_EM (&$2, FCNTYPE_AXISFCN);
                                         if (!lpYYStr)
                                         {
                                             if (lpYYFcn)
                                                 FreeResult (lpYYFcn);
                                             FreeResult (&$1);
                                             YYERROR;
                                         }
                                         lpYYRes = ExecFunc_EM (&lpYYStr->tkToken, lpYYFcn, &$1.tkToken, NULL);
                                         FreeResult (&$1);
                                         FreeResult (lpYYFcn);
                                         FreeResult (lpYYStr);
                                         if (!lpYYRes)
                                             YYERROR;
                                         $$ = *lpYYRes;
                                        }
    ;

/* Strand (including single names) */
Strand:           NAMEUNK               {DbgMsgW2 (L"%%Strand:  NAMEUNK");
                                         ValueError (&$1); YYERROR;
                                        }
    |             SingTokn              {DbgMsgW2 (L"%%Strand:  SingTokn -- InitVarStrand/PushVarStrand");
                                         InitVarStrand (&$1);
                                         $$ = *PushVarStrand (&$1);}
    |         ')' ArrExpr '('           {DbgMsgW2 (L"%%Strand:  (ArrExpr) -- InitVarStrand/PushVarStrand");
                                         InitVarStrand (&$2);
                                         $$ = *PushVarStrand (&$2);}
    | Strand      SingTokn              {DbgMsgW2 (L"%%Strand:  Strand SingTokn -- PushVarStrand");
                                         $1 = $1;
                                         $$ = *PushVarStrand (&$2);}
    | Strand  ')' ArrExpr '('           {DbgMsgW2 (L"%%Strand:  Strand (ArrExpr) -- PushVarStrand");
                                         $1 = $1;
                                         $$ = *PushVarStrand (&$3);}
    ;

/* Simple array expression */
SimpExpr:
               Strand                   {DbgMsgW2 (L"%%SimpExpr:  Strand -- MakeVarStrand_EM");
                                         lpYYStr = MakeVarStrand_EM (&$1);
                                         if (!lpYYStr)
                                             YYERROR;
                                         $$ = *lpYYStr;}

    | ILPAR                             {DbgMsgW2 (L"%%SimpExpr:  ILPAR");
                                         $$ = $1;}
    | ILBR     Strand                   {DbgMsgW2 (L"%%SimpExpr:  Strand ILBR");
                                         lpYYStr = MakeVarStrand_EM (&$2);
                                         if (!lpYYStr)
                                             YYERROR;


                                         NonceError (&$2); YYERROR;
                                        }
    | ArrExpr ASSIGN       NameAnyVar   {DbgMsgW2 (L"%%SimpExpr:  NameAny" WS_UCS2_LEFTARROW L"ArrExpr");
                                         bRet = AssignName_EM (&$3, &$1);
                                         FreeResult (&$1);
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;
                                        }
    | ArrExpr ASSIGN ILBR  NAMEVAR      {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR" WS_UCS2_LEFTARROW L"ArrExpr");



                                         NonceError (&$1);
                                         FreeResult (&$1);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' NameSpec '(' {DbgMsgW2 (L"%%SimpExpr:  (NameSpec)" WS_UCS2_LEFTARROW L"ArrExpr");



                                         NonceError (&$1);
                                         FreeResult (&$1);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' ILBR NAMEVAR '(' {DbgMsgW2 (L"%%SimpExpr:  (NAMEVAR ILBR)" WS_UCS2_LEFTARROW L"ArrExpr");



                                         NonceError (&$1);
                                         FreeResult (&$1);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN LeftFunc  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR LeftFunc" WS_UCS2_LEFTARROW L"ArrExpr");
                                         lpYYRes = ExecFunc_EM (&$4.tkToken, &$3, &$1.tkToken, NULL);
                                         FreeResult (&$1);
                                         FreeResult (&$3);
                                         if (!lpYYRes)
                                             YYERROR;
                                         bRet = AssignName_EM (&$4, lpYYRes);
                                         FreeResult (lpYYRes);
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $4;
                                        }
    | ArrExpr ASSIGN AxisFunc  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UCS2_LEFTARROW L"ArrExpr");
                                         lpYYRes = ExecFunc_EM (&$4.tkToken, &$3, &$1.tkToken, NULL);
                                         FreeResult (&$1);
                                         FreeResult (&$3);
                                         if (!lpYYRes)
                                             YYERROR;
                                         bRet = AssignName_EM (&$4, lpYYRes);
                                         FreeResult (lpYYRes);
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $4;
                                        }
    | ArrExpr ASSIGN LeftFunc ')' NameVars '('{DbgMsgW2 (L"%%SimpExpr:  (NameVars) LeftFunc" WS_UCS2_LEFTARROW L"ArrExpr");



                                         NonceError (&$1);

                                         FreeResult (&$1);
                                         FreeResult (&$3);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN AxisFunc ')' NameVars '('{DbgMsgW2 (L"%%SimpExpr:  (NameVars) AxisFunc" WS_UCS2_LEFTARROW L"ArrExpr");



                                         NonceError (&$1);

                                         FreeResult (&$1);
                                         FreeResult (&$3);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN LeftFunc ILBR NAMEVAR {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR LeftFunc" WS_UCS2_LEFTARROW L"ArrExpr");



                                         NonceError (&$1);

                                         FreeResult (&$1);
                                         FreeResult (&$3);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN AxisFunc ILBR NAMEVAR {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR AxisFunc" WS_UCS2_LEFTARROW L"ArrExpr");



                                         NonceError (&$1);

                                         FreeResult (&$1);
                                         FreeResult (&$3);
                                         YYERROR;
                                        }
    ;

/* Selective specification */
NameSpec:
      NameVars                          {DbgMsgW2 (L"%%NameSpec:  NameVars");
                                         $$ = $1;}
    | NameSpec LeftFunc                 {DbgMsgW2 (L"%%NameSpec:  LeftFunc NameSpec");



                                         NonceError (&$1);

                                         FreeResult (&$2);
                                         YYERROR;
                                        }
    | NameSpec AxisFunc                 {DbgMsgW2 (L"%%NameSpec:  AxisFunc NameSpec");



                                         NonceError (&$1);

                                         FreeResult (&$2);
                                         YYERROR;
                                        }
    | NameSpec LeftFunc Strand          {DbgMsgW2 (L"%%NameSpec:  Strand LeftFunc NameSpec");
                                         lpYYStr = MakeVarStrand_EM (&$3);
                                         if (!lpYYStr)
                                             YYERROR;



                                         NonceError (&$1);

                                         FreeResult (&$2);
                                         FreeResult (lpYYStr);
                                         YYERROR;
                                        }
    | NameSpec AxisFunc Strand          {DbgMsgW2 (L"%%NameSpec:  Strand AxisFunc NameSpec");
                                         lpYYStr = MakeVarStrand_EM (&$3);
                                         if (!lpYYStr)
                                             YYERROR;



                                         NonceError (&$1);

                                         FreeResult (&$2);
                                         FreeResult (lpYYStr);
                                         YYERROR;
                                        }
    ;

/* Name Strand */
NameVars:
      NAMEVAR                           {DbgMsgW2 (L"%%NameVars:  NAMEVAR");
                                         $$ = $1;}
    | NAMEUNK                           {DbgMsgW2 (L"%%NameVars:  NAMEUNK");
                                         $$ = $1;}
    |       ')' ILBR NAMEVAR '('        {DbgMsgW2 (L"%%NameVars:  (NAMEVAR ILBR)");




                                         NonceError (&$3); YYERROR;
                                        }
    | NameVars       NAMEVAR            {DbgMsgW2 (L"%%NameVars:  NameVars NAMEVAR");




                                         NonceError (&$2); YYERROR;
                                        }
    | NameVars ')' ILBR NAMEVAR '('     {DbgMsgW2 (L"%%NameVars:  NameVars (NAMEVAR ILBR)");




                                         NonceError (&$4); YYERROR;
                                        }
    ;

/* Left function expression */
LeftFunc:
          PRIMFCN                       {DbgMsgW2 (L"%%LeftFunc:  PRIMFCN");
                                         $$ = *PushFcnStrand (MakePrimFcn (&$1), 1, FALSE);
                                         FreeResult (&$1);
                                        }
    |     NAMEFCN                       {DbgMsgW2 (L"%%LeftFunc:  NAMEFCN");
                                         $$ = *PushFcnStrand (MakeNameFcn (&$1), 1, FALSE);
                                         FreeResult (&$1);
                                        }
    |     SimpFunc                      {DbgMsgW2 (L"%%LeftFunc:  SimpFunc");
                                         $$ = *PushFcnStrand (&$1, 1, TRUE);
                                         FreeResult (&$1);
                                        }
    |     RightFunc    JOTDOT           {DbgMsgW2 (L"%%LeftFunc:  " WS_UCS2_JOT L". RightFunc");
                                         $$ = *PushFcnStrand (&$2, 2, FALSE);
                                               PushFcnStrand (&$1, 1, TRUE);
                                         FreeResult (&$1);
                                         FreeResult (&$2);
                                        }
    |                  MonOp LeftFunc   {DbgMsgW2 (L"%%LeftFunc:  LeftFunc MonOp");
                                         $$ = *PushFcnStrand (&$1, 2, TRUE);
                                               PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                         FreeResult (&$1);
                                        }
    |              MonOpAxis LeftFunc   {DbgMsgW2 (L"%%LeftFunc:  LeftFunc MonOpAxis");
                                         $$ = *PushFcnStrand (&$1, 2, TRUE);
                                               PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                         FreeResult (&$1);
                                        }
    |                  MonOp AxisFunc   {DbgMsgW2 (L"%%LeftFunc:  AxisFunc MonOp");
                                         $$ = *PushFcnStrand (&$1, 2, TRUE);
                                               PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                         FreeResult (&$1);
                                        }
    |              MonOpAxis AxisFunc   {DbgMsgW2 (L"%%LeftFunc:  AxisFunc MonOpAxis");
                                         $$ = *PushFcnStrand (&$1, 2, TRUE);
                                               PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                         FreeResult (&$1);
                                        }
    |              '>' MonOp Strand '(' {DbgMsgW2 (L"%%LeftFunc:  (Strand MonOp)");
                                         lpYYStr = MakeVarStrand_EM (&$3);
                                         if (!lpYYStr)
                                         {
                                            FreeResult (&$2);
                                            YYERROR;
                                         }
                                         $$ = *PushFcnStrand (&$2, 2, TRUE);
                                               PushFcnStrand (lpYYStr, 1, FALSE);
                                         FreeResult (lpYYStr);
                                         FreeResult (&$2);
                                        }
    |          '>' MonOpAxis Strand '(' {DbgMsgW2 (L"%%LeftFunc:  (Strand MonOpAxis)");
                                         lpYYStr = MakeVarStrand_EM (&$3);
                                         if (!lpYYStr)
                                         {
                                            FreeResult (&$2);
                                            YYERROR;
                                         }
                                         $$ = *PushFcnStrand (&$2, 2, TRUE);
                                               PushFcnStrand (lpYYStr, 1, FALSE);
                                         FreeResult (lpYYStr);
                                         FreeResult (&$2);
                                        }
    |     RightFunc DydOp LeftFunc      {DbgMsgW2 (L"%%LeftFunc:  LeftFunc DydOp RightFunc");
                                         $$ = *PushFcnStrand (&$2, 3, TRUE);
                                               PushFcnStrand (&$3, 1, TRUE);
                                               PushFcnStrand (&$1, 1, TRUE);
                                         FreeResult (&$1);
                                         FreeResult (&$3);
                                         FreeResult (&$2);
                                        }
    |     RightFunc DydOp AxisFunc      {DbgMsgW2 (L"%%LeftFunc:  AxisFunc DydOp RightFunc");
                                         $$ = *PushFcnStrand (&$2, 3, TRUE);
                                               PushFcnStrand (&$3, 1, TRUE);
                                               PushFcnStrand (&$1, 1, TRUE);
                                         FreeResult (&$1);
                                         FreeResult (&$3);
                                         FreeResult (&$2);
                                        }
    | '>' '>' SimpExpr '(' DydOp LeftFunc '(' {DbgMsgW2 (L"%%LeftFunc:  (LeftFunc DydOp (SimpExpr))");
                                         $$ = *PushFcnStrand (&$5, 3, TRUE);
                                               PushFcnStrand (&$6, 1, TRUE);
                                               PushFcnStrand (&$3, 1, FALSE);
                                         FreeResult (&$3);
                                         FreeResult (&$6);
                                         FreeResult (&$5);
                                        }
    | '>' '>' SimpExpr '(' DydOp AxisFunc '(' {DbgMsgW2 (L"%%LeftFunc:  (AxisFunc DydOp (SimpExpr))");
                                         $$ = *PushFcnStrand (&$5, 3, TRUE);
                                               PushFcnStrand (&$6, 1, TRUE);
                                               PushFcnStrand (&$3, 1, FALSE);
                                         FreeResult (&$3);
                                         FreeResult (&$6);
                                         FreeResult (&$5);
                                        }
    | '>' RightFunc DydOp Strand   '('  {DbgMsgW2 (L"%%LeftFunc:  (Strand DydOp RightFunc)");
                                         lpYYStr = MakeVarStrand_EM (&$4);
                                         if (!lpYYStr)
                                         {
                                            FreeResult (&$2);
                                            FreeResult (&$3);
                                            YYERROR;
                                         }
                                         $$ = *PushFcnStrand (&$3, 3, TRUE);
                                               PushFcnStrand (lpYYStr, 1, FALSE);
                                               PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                         FreeResult (lpYYStr);
                                         FreeResult (&$3);
                                        }
    | '>' '>' SimpExpr '(' DydOp Strand '(' {DbgMsgW2 (L"%%LeftFunc:  (Strand DydOp (SimpExpr))");
                                         lpYYStr = MakeVarStrand_EM (&$6);
                                         if (!lpYYStr)
                                         {
                                            FreeResult (&$3);
                                            FreeResult (&$5);
                                            YYERROR;
                                         }
                                         $$ = *PushFcnStrand (&$5, 3, TRUE);
                                               PushFcnStrand (lpYYStr, 1, FALSE);
                                               PushFcnStrand (&$3, 1, FALSE);
                                         FreeResult (&$3);
                                         FreeResult (lpYYStr);
                                         FreeResult (&$5);
                                        }
    ;

/* Simple function expression */
SimpFunc:
      '>' LeftFunc '('                  {DbgMsgW2 (L"%%SimpFunc:  (LeftFunc)");
                                         $$ = *PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                        }
    | '>' AxisFunc '('                  {DbgMsgW2 (L"%%SimpFunc:  (AxisFunc)");
                                         $$ = *PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                        }
    | '>' FcnSpec '('                   {DbgMsgW2 (L"%%SimpFunc:  (FcnSpec)");
                                         $$ = *PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                        }
    ;

/* Right argument to dyadic operator */
RightFunc:
          PRIMFCN                       {DbgMsgW2 (L"%%RightFunc:  PRIMFCN");
                                         $$ = *PushFcnStrand (MakePrimFcn (&$1), 1, FALSE);
                                         FreeResult (&$1);
                                        }
    |     NAMEFCN                       {DbgMsgW2 (L"%%RightFunc:  NAMEFCN");
                                         $$ = *PushFcnStrand (MakeNameFcn (&$1), 1, FALSE);
                                         FreeResult (&$1);
                                        }
    | '>' LeftFunc  '('                 {DbgMsgW2 (L"%%RightFunc:  (LeftFunc)");
                                         $$ = *PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                        }
    | '>' FcnSpec  '('                  {DbgMsgW2 (L"%%RightFunc:  (FcnSpec)");
                                         $$ = *PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                        }
    | '>' AxisFunc  '('                 {DbgMsgW2 (L"%%RightFunc:  (AxisFunc)");
                                         $$ = *PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                        }
    ;

/* Axis function expression */
AxisFunc:
      '}' ArrExpr '['  PRIMFCN          {DbgMsgW2 (L"%%AxisFunc:  PRIMFCN[ArrExpr]");
                                         $$ = *PushFcnStrand (MakePrimFcn (&$4), 2, FALSE);
                                               PushFcnStrand (MakeAxis  (&$2), 1, FALSE);
                                         FreeResult (&$2);
                                         FreeResult (&$4);
                                        }
    | '}' ArrExpr '['  NAMEFCN          {DbgMsgW2 (L"%%AxisFunc:  NAMEFCN[ArrExpr]");
                                         $$ = *PushFcnStrand (MakeNameFcn (&$4), 2, FALSE);
                                               PushFcnStrand (MakeAxis  (&$2), 1, FALSE);
                                         FreeResult (&$2);
                                         FreeResult (&$4);
                                        }
    // ***FIXME*** -- what does this double index mean?
////| '}' ArrExpr '['  AxisFunc         {DbgMsgW2 (L"%%AxisFunc:  (AxisFunc)[ArrExpr]");
////                                     lpYYFcn = MakeArrayFn2_EM (&$4, MakeAxis (&$2));
////                                     FreeResult (&$2);
////                                     FreeResult (&$4);
////                                     if (!lpYYFcn)
////                                        YYERROR;
////                                     $$ = *lpYYFcn;
////                                    }
    | '}' ArrExpr '[' '>' FcnSpec '('   {DbgMsgW2 (L"%%AxisFunc:  (FcnSpec)[ArrExpr]");
                                         $$ = *PushFcnStrand (&$5, 2, TRUE);
                                               PushFcnStrand (MakeAxis (&$2), 1, FALSE);
                                         FreeResult (&$2);
                                         FreeResult (&$5);
                                        }
    ;

// Monadic operator
MonOp:
                       OP1              {DbgMsgW2 (L"%%MonOp:  OP1");
                                         $$ = *PushFcnStrand (MakeOp1 (&$1), 1, FALSE);
                                         FreeResult (&$1);
                                        }
    |                  NAMEOP1          {DbgMsgW2 (L"%%MonOp:  NAMEOP1");
                                         $$ = *PushFcnStrand (MakeNameOp1 (&$1), 1, FALSE);
                                         FreeResult (&$1);
                                        }
    |              '>' MonOp '('        {DbgMsgW2 (L"%%MonOp:  (MonOp)");
                                         $$ = *PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                        }
    |              '>' Op1Spec '('     {DbgMsgW2 (L"%%MonOp:  (Op1Spec)");
                                         $$ = *PushFcnStrand (&$2, 1, TRUE);
                                         FreeResult (&$2);
                                        }
    ;

// Monadic operator with axis
MonOpAxis:
      '}' ArrExpr '['  MonOp            {DbgMsgW2 (L"%%MonOpAxis:  MonOp[ArrExpr]");
                                         $$ = *PushFcnStrand (&$4, 2, TRUE);
                                               PushFcnStrand (MakeAxis (&$2), 1, FALSE);
                                         FreeResult (&$2);
                                         FreeResult (&$4);
                                        }
////| '}' ArrExpr '['  OP1              {DbgMsgW2 (L"%%MonOpAxis:  OP1[ArrExpr]");
////                                     $$ = *PushFcnStrand (MakeOp1  (&$4), 2, FALSE);
////                                           PushFcnStrand (MakeAxis (&$2), 1, FALSE);
////                                     FreeResult (&$2);
////                                     FreeResult (&$4);
////                                    }
////| '}' ArrExpr '['  NAMEOP1          {DbgMsgW2 (L"%%MonOpAxis:  NAMEOP1[ArrExpr]");
////                                     $$ = *PushFcnStrand (MakeNameOp1 (&$4), 2, FALSE);
////                                           PushFcnStrand (MakeAxis  (&$2), 1, FALSE);
////                                     FreeResult (&$2);
////                                     FreeResult (&$4);
////                                    }
////| '}' ArrExpr '[' '>' MonOp '('     {DbgMsgW2 (L"%%MonOpAxis:  (MonOp)[ArrExpr]");
////                                     $$ = *PushFcnStrand (&$5, 2, TRUE);
////                                           PushFcnStrand (MakeAxis (&$2), 1, FALSE);
////                                     FreeResult (&$2);
////                                     FreeResult (&$5);
////                                    }
    ;

/* Dyadic operator */
DydOp:
                       OP2              {DbgMsgW2 (L"%%DydOp:  OP2");
                                         $$ = *PushFcnStrand (MakeOp2 (&$1), 1, FALSE);
                                         FreeResult (&$1);
                                        }
    |                  NAMEOP2          {DbgMsgW2 (L"%%DydOp:  NAMEOP2");
                                         $$ = *PushFcnStrand (MakeNameOp2 (&$1), 1, FALSE);
                                         FreeResult (&$1);
                                        }
    |              '#' DydOp '('        {DbgMsgW2 (L"%%DydOp:  (DydOp)");
                                         $$ = *PushFcnStrand (&$2, 1, FALSE);
                                         FreeResult (&$2);
                                        }
    |              '#' Op2Spec '('      {DbgMsgW2 (L"%%DydOp:  (Op2Spec)");
                                         $$ = *PushFcnStrand (&$2, 1, FALSE);
                                         FreeResult (&$2);
                                        }
// What do these rules mean?
/***| '}' ArrExpr '['     OP2           {DbgMsgW2 (L"%%DydOp:  OP2[ArrExpr]");}                   ***/
/***| '}' ArrExpr '['     NAMEOP2       {DbgMsgW2 (L"%%DydOp:  NAMEOP2[ArrExpr]");}               ***/
/***| '}' ArrExpr '[' '>' DydOp   '('   {DbgMsgW2 (L"%%DydOp:  (DydOp)[ArrExpr]");}               ***/
/***| '}' ArrExpr '[' '>' Op2Spec '('   {DbgMsgW2 (L"%%DydOp:  (Op2Spec)[ArrExpr]");}               ***/
    ;

/* Index list w/brackets, allowing for A[A][A]... */
ILBR:
           ']' ILWE '['                 {DbgMsgW2 (L"%%ILBR:  [ILWE]");
                                         lpYYLst = MakeList_EM (&$2);
                                         FreeResult (&$2);
                                         if (!lpYYLst)
                                            YYERROR;
                                         $$ = *lpYYLst;
                                        }
    | ILBR ']' ILWE '['                 {DbgMsgW2 (L"%%ILBR:  ILBR [ILWE]");




                                         NonceError (&$3);
                                         FreeResult (&$1);
                                         FreeResult (&$3);
                                         YYERROR;
                                        }
    ;

/* Index list w/parens */
ILPAR:
      ')' ILNE '('                      {DbgMsgW2 (L"%%ILPAR:  (ILNE)");
                                         lpYYLst = MakeList_EM (&$2);
                                         FreeResult (&$2);
                                         if (!lpYYLst)
                                            YYERROR;
                                         $$ = *lpYYLst;
                                         }
    ;

/* Index list, no empties */
ILNE:
      ArrExpr ';' ArrExpr               {DbgMsgW2 (L"%%ILNE:  ArrExpr;ArrExpr");
                                         $$ = *InitList1 (&$1);
                                         $$ = *PushList (&$$, &$3);}
    | ILNE    ';' ArrExpr               {DbgMsgW2 (L"%%ILNE:  ILNE;ArrExpr");
                                         $$ = *PushList (&$1, &$3);}
    ;

/* Index list, with empties */
ILWE:
      /* Empty */                       {DbgMsgW2 (L"%%ILWE:  <empty>");
                                         $$ = *InitList0 ();}
    |          ArrExpr                  {DbgMsgW2 (L"%%ILWE:  ArrExpr");
                                         $$ = *InitList1 (&$1);}
    | ILWE ';' ArrExpr                  {DbgMsgW2 (L"%%ILWE:  ILWE;ArrExpr");
                                         $$ = *PushList (&$1, &$3);}
    | ILWE ';'                          {DbgMsgW2 (L"%%ILWE:  ILWE;");
                                         $$ = *PushList (&$1, NULL);}
    ;

/* A data expression which has a single token */
SingTokn:
      CONSTANT                          {DbgMsgW2 (L"%%SingTokn:  CONSTANT");
                                         $$ = $1;}
    | NAMEVAR                           {DbgMsgW2 (L"%%SingTokn:  NAMEVAR");
                                         $$ = $1;}
    | FN0                               {DbgMsgW2 (L"%%SingTokn:  FN0");
                                         $$ = *ExecuteFn0 (&$1);}
    | STRING                            {DbgMsgW2 (L"%%SingTokn:  STRING");
                                         lpYYStr = CopyString_EM (&$1);
                                         if (!lpYYStr)
                                            YYERROR;
                                         $$ = *lpYYStr;}
    ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************



//***************************************************************************
//  NonceError
//
//  I'll get right on this...
//***************************************************************************

void NonceError
    (LPYYSTYPE lpYYError)

#ifdef DEBUG
#define APPEND_NAME     L" -- NonceError"
#else
#define APPEND_NAME
#endif

{
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME, &lpYYError->tkToken);
} // End NonceError
#undef  APPEND_NAME


//***************************************************************************
//  ValueError
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValueError"
#else
#define APPEND_NAME
#endif

void ValueError
    (LPYYSTYPE lpYYError)

{
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME, &lpYYError->tkToken);
} // End ValueError
#undef  APPEND_NAME


//***************************************************************************
//  ParseLine
//
//  Parse a line
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseLine"
#else
#define APPEND_NAME
#endif

void ParseLine
    (HGLOBAL hGlbToken,
     WCHAR   *lpwszLine)

{
    PLLOCALVARS rplLocalVars;   // Re-entrant vars

    DBGENTER;

    // If we don't have a valid handle, ...
    if (!hGlbToken)
        return;
#ifdef DEBUG
    { // ***DEBUG***
        // Display the tokens so far
        DisplayTokens (hGlbToken);
    } // ***DEBUG*** END
#endif

    // Make a backup copy of the global values
    rplLocalVars = gplLocalVars;

    // Lock the handle and set variables
    gplLocalVars.hGlbToken = hGlbToken;
    UTLockAndSet (gplLocalVars.hGlbToken, &gplLocalVars.t2);

    // Skip over TOKEN_HEADER
    gplLocalVars.lpStart = TokenBaseToStart (gplLocalVars.t2.lpBase);

    // Skip to end of 1st stmt
    gplLocalVars.lpNext  = &gplLocalVars.lpStart[gplLocalVars.lpStart->tkData.tkIndex];

    // Start off with no error
    gplLocalVars.lpError = NULL;

    // Allocate virtual memory for the Function & Variable Strand accumulator
    gplLocalVars.lpYYStrandStart =
        VirtualAlloc (NULL,      // Any address
                      DEF_STRAND_MAXSIZE * sizeof (YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!gplLocalVars.lpYYStrandStart)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ParseLine:  VirtualAlloc for <gpLocalVars.lpYYStrandStart> failed");

        return;             // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (gplLocalVars.lpYYStrandStart,
                  DEF_STRAND_INITSIZE * sizeof (YYSTYPE),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Initialize the base & next strand ptrs
    gplLocalVars.lpYYStrandBase =
    gplLocalVars.lpYYStrandNext = gplLocalVars.lpYYStrandStart;

    // Use VirtualAlloc for the stack
    // ***FIXME***














#ifdef YYDEBUG
    // Enable debugging
    yydebug = 1;
#endif

    // Parse the file, check for errors
    if (!yyparse ())
        goto NORMAL_EXIT;

//ERROR_EXIT:
    // Signal an error
    ErrorMessage (lpwszErrorMessage, lpwszLine, gplLocalVars.lpError->tkCharIndex);
NORMAL_EXIT:

    if (gplLocalVars.lpYYStrandStart)
    {
        VirtualFree (gplLocalVars.lpYYStrandStart, 0, MEM_RELEASE); gplLocalVars.lpYYStrandStart = NULL;
    } // End IF

    // Unlock the handle
    MyGlobalUnlock (gplLocalVars.hGlbToken); gplLocalVars.t2.lpBase   = NULL;

    // Restore global token ptrs so we're re-entrant
    gplLocalVars = rplLocalVars;

    DBGEXIT;
} // End ParseLine
#undef  APPEND_NAME


//***************************************************************************
//  TokenTypeFV
//
//  Get the type ('F' or 'V', or '?') of a symbol table name token.
//  Used for TKT_VARNAMEDs.
//***************************************************************************

char TokenTypeFV
    (LPTOKEN lpNext)

{
    LPSTFLAGS lpstFlags;

    // Get a ptr to the symbol table flags
    lpstFlags = &lpNext->tkData.lpSym->stFlags;

    if (lpstFlags->Imm)     // IMMTYPE_BOOL, IMMTYPE_INT, IMMTYPE_CHAR, IMMTYPE_FLOAT, IMMTYPE_PRIMFCN, ...
    switch (lpstFlags->ImmType)
    {
        case IMMTYPE_BOOL:
        case IMMTYPE_INT:
        case IMMTYPE_CHAR:
        case IMMTYPE_FLOAT:
            return 'V';

        case IMMTYPE_PRIMFCN:
        case IMMTYPE_PRIMOP1:
        case IMMTYPE_PRIMOP2:
            return 'F';

        defstop
            return '?';
    } // End IF/SWITCH

    if (lpstFlags->UsrVar
     || lpstFlags->UsrFn0
     || lpstFlags->SysVar
     || lpstFlags->SysFn0)
        return 'V';

    if (lpstFlags->SysFn12
     || lpstFlags->UsrFn12
     || lpstFlags->UsrOp1
     || lpstFlags->UsrOp2)
        return 'F';

    // After having checked all the other .Usrxxx possibilities,
    //   if it's a UsrName, then it's really a VALUE ERROR, but this
    //   isn't the time to signal that.
    if (lpstFlags->UsrName)
        return 'V';

    return '?';             // SYNTAX ERROR
} // TokenTypeFV


//***************************************************************************
//  LookaheadDyadicOp
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
        case TKT_OP1IMMED:
        case TKT_LBRACKET:
        case TKT_VARARRAY:
            return FALSE;

        case TKT_OP2IMMED:
        case TKT_JOTDOT:
            return TRUE;

        case TKT_VARNAMED:
            // Look inside the symbol table entry
            return lpNext->tkData.lpSym->stFlags.UsrOp2;

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
} // End LookaheadDyadicOp


//***************************************************************************
//  LookaheadSurround
//
//  Lookahead (backwards) in the token stream to see if
//    the next non-grouping symbol is a function or a var.
//  Used for TKT_RPARENs to see if it and the matching
//    TKT_LPAREN surrounds a function, variable, monadic or dyadic
//    operator, or for / & \ to see if the next non-grouping
//    symbol is a function, variable, or dyadic operator.
//***************************************************************************

char LookaheadSurround
    (LPPLLOCALVARS lpplLocalVars)

{
    LPTOKEN lpNext;

    // Get a ptr to the next token
    lpNext = &lpplLocalVars->lpNext[-1];

    while (TRUE)
    switch (lpNext->tkFlags.TknType)
    {
        case TKT_STRING:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            // Lookahead to see if the next token is a dyadic operator:
            //   if so, return 'F'; otherwise return 'V'.
            switch (LookaheadDyadicOp (lpplLocalVars, &lpNext[-1]))
            {
                case TRUE:
                    return 'F';     // Function

                case FALSE:
                    return 'V';     // Variable

                default:
                    return '?';     // SYNTAX ERROR
            } // End SWITCH

        case TKT_OP1IMMED:
            return '1';             // Monadic operator

        case TKT_OP2IMMED:
            return '2';             // Dyadic operator

        case TKT_FCNIMMED:
        case TKT_JOTDOT:
        case TKT_ASSIGN:
        case TKT_EOS:
        case TKT_EOL:
            return 'F';             // Function

////////// The EOS/EOL cases are for when we're called for /1 or \1
//////////   (that is, monadic slash or backslash).
////////// I prefer not to trigger a SYNTAX ERROR during the parse
//////////   but instead during semantic analysis.
////////case TKT_EOS:
////////case TKT_EOL:
        case TKT_LIST:
            return 'V';             // Variable

        case TKT_RBRACKET:
            // Each right grouping symbol has the index of the matching
            //   left grouping symbol in its tkData.tkIndex
            // Get a ptr to the token adjacent to ("-1") the matching left
            //   grouping symbol.
            lpNext = &lpplLocalVars->lpStart[lpNext->tkData.tkIndex - 1];

            break;

        case TKT_RPAREN:
        case TKT_LINECONT:
            lpNext--;               // Ignore this token

            break;                  // Go around again

        case TKT_VARNAMED:
            // Get the token type of the symbol table name
            return TokenTypeFV (lpNext);

        case TKT_LISTSEP:
        case TKT_COMMENT:
        case TKT_LPAREN:
        case TKT_LBRACKET:
            return '?';             // SYNTAX ERROR

        case TKT_STRAND:
        defstop
            return '?';
    } // End WHILE/SWITCH
} // LookaheadSurround


//***************************************************************************
//  LookaheadAdjacent
//
//  Lookahead (backwards) in the token stream to see if
//    the matching grouping symbol is adjacent to a function, variable,
//    monadic or dyadic operator.
//  Used for TKT_RBRACKETs to see if this is indexing or axis.
//***************************************************************************

char LookaheadAdjacent
    (LPPLLOCALVARS lpplLocalVars)

{
    LPTOKEN lpNext;

    // Each right grouping symbol has the index of the matching
    //   left grouping symbol in its tkData.tkIndex.
    // Get a ptr to the token adjacent to ("-1") the matching left
    //   grouping symbol.
    lpNext = &lpplLocalVars->lpStart[lpplLocalVars->lpNext->tkData.tkIndex - 1];

    while (TRUE)
    switch (lpNext->tkFlags.TknType)
    {
        case TKT_STRING:
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            return 'V';             // Variable

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
            return '1';

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
            return '2';

        case TKT_FCNIMMED:
        case TKT_JOTDOT:
            return 'F';             // Function

        case TKT_VARNAMED:
            return TokenTypeFV (lpNext);

        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_COMMENT:
        case TKT_LPAREN:
        case TKT_LBRACKET:
        case TKT_EOS:
        case TKT_EOL:
            return '?';             // SYNTAX ERROR

        case TKT_LINECONT:
        case TKT_RPAREN:
            lpNext--;               // Ignore this token

            break;                  // Go around again

        case TKT_RBRACKET:
            lpNext = &lpplLocalVars->lpStart[lpplLocalVars->lpNext->tkData.tkIndex - 1];

            break;                  // Go around again

        case TKT_STRAND:
        defstop
            return '?';
    } // End WHILE/SWITCH
} // LookaheadAdjacent


//***************************************************************************
//  yylex
//
//  Lexical analyzer for YACC
//***************************************************************************

int yylex
    (void)

{
    // Because we're parsing the stmt from right to left
    gplLocalVars.lpNext--;

    // Return the current token
    yylval.tkToken = *gplLocalVars.lpNext;

    // Split cases based upon the token type
    switch (gplLocalVars.lpNext->tkFlags.TknType)
    {
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            return CONSTANT;

        case TKT_VARNAMED:
////             // We need a bit a lookahead here so handle the FCN vs. OP1 vs. OP2
////             //   cases when the name is being assigned to.  In this case,
////             //   because the current meaning of the name is irrelevant
////             //   (it's about to be reassigned), we can accept any kind of name.
////             //   LALR grammars have a hard time dealing with this, so we
////             //   lookahead one token to see if it is ASSIGN.  If so, we return
////             //   the type of name corresponding to the token beyond ASSIGN.
////
////             // Check for ASSIGN as the next token
////             if (gplLocalVars.lpNext[1].tkFlags.TknType EQ TKT_ASSIGN)
////             {
////                 // Split cases based upon the next token type
////                 switch (gplLocalVars.lpNext[2].tkFlags.TknType)
////                 {
////                     case TKT_VARIMMED:
////                     case TKT_VARNAMED:
////                     case TKT_VARARRAY:
//// ////////////////////////gplLocalVars.lpNext->tkFlags.TknType = TKT_VARNAMED;
////
////                         return NAMEVAR;
////
////                     case TKT_FCNIMMED:
////                     case TKT_FCNNAMED:
////                     case TKT_JOTDOT:
////                         gplLocalVars.lpNext->tkFlags.TknType = TKT_FCNNAMED;
////
////                         return NAMEFCN;
////
////                     case TKT_OP1IMMED:
////                     case TKT_OP1NAMED:
////                         gplLocalVars.lpNext->tkFlags.TknType = TKT_OP1NAMED;
////
////                         return NAMEOP1;
////
////                     case TKT_OP2IMMED:
////                     case TKT_OP2NAMED:
////                         gplLocalVars.lpNext->tkFlags.TknType = TKT_OP2NAMED;
////
////                         return NAMEOP2;
////
////                     case TKT_FCNARRAY:
////                         // Split cases based upon the underlying FCNTYPE_xxx
////                         switch (GetFcnType (&gplLocalVars.lpNext[2]))
////                         {
////                             case FCNTYPE_FCN:
////                             case FCNTYPE_AXISFCN:
////                                 gplLocalVars.lpNext->tkFlags.TknType = TKT_FCNNAMED;
////
////                                 return NAMEFCN;
////
////                             case FCNTYPE_OP1:
////                                 gplLocalVars.lpNext->tkFlags.TknType = TKT_OP1NAMED;
////
////                                 return NAMEOP1;
////
////                             case FCNTYPE_OP2:
////                                 gplLocalVars.lpNext->tkFlags.TknType = TKT_OP2NAMED;
////
////                                 return NAMEOP2;
////
////                             defstop
////                                 break;
////                         } // End SWITCH
////
////                     defstop
////                         return UNK;
////                 } // End SWITCH
////             } else
            // We need a bit a lookahead here so handle the FCN vs. OP1 vs. OP2
            //   cases when the name is being assigned to.  In this case,
            //   because the current meaning of the name is irrelevant
            //   (it's about to be reassigned), we can accept any kind of name.
            //   LALR grammars have a hard time dealing with this, so we
            //   lookahead one token to see if it is ASSIGN.  If so, we return
            //   NAMEUNK, a universal named token.

            // Check for ASSIGN as the next token
            if ((!gplLocalVars.lpNext->tkData.lpSym->stFlags.Value
              && gplLocalVars.lpNext->tkData.lpSym->stFlags.UsrVar)
             || gplLocalVars.lpNext[1].tkFlags.TknType EQ TKT_ASSIGN)
                return NAMEUNK;
            else
            {
                if (gplLocalVars.lpNext->tkData.lpSym->stFlags.UsrVar
                 || gplLocalVars.lpNext->tkData.lpSym->stFlags.SysVar)
                {
////////////////////gplLocalVars.lpNext->tkFlags.TknType = TKT_VARNAMED;

                    return NAMEVAR;
                } else
                if (gplLocalVars.lpNext->tkData.lpSym->stFlags.UsrFn0
                 || gplLocalVars.lpNext->tkData.lpSym->stFlags.SysFn0)
                    return FN0;
                else
                if (gplLocalVars.lpNext->tkData.lpSym->stFlags.UsrFn12
                 || gplLocalVars.lpNext->tkData.lpSym->stFlags.SysFn12)
                {
                    gplLocalVars.lpNext->tkFlags.TknType = TKT_FCNNAMED;

                    return NAMEFCN;
                } else
                if (gplLocalVars.lpNext->tkData.lpSym->stFlags.UsrOp1)
                {
                    gplLocalVars.lpNext->tkFlags.TknType = TKT_OP1NAMED;

                    return NAMEOP1;
                } else
                if (gplLocalVars.lpNext->tkData.lpSym->stFlags.UsrOp2)
                {
                    gplLocalVars.lpNext->tkFlags.TknType = TKT_OP2NAMED;

                    return NAMEOP2;
                } // End IF/ELSE/...

                DbgStop ();         // We should never get here

                return UNK;
            } // End IF

        case TKT_ASSIGN:
            return ASSIGN;

        case TKT_LISTSEP:
            return ';';

        case TKT_FCNIMMED:
            if (gplLocalVars.lpNext->tkData.tkIndex EQ UCS2_RIGHTARROW)
                return GOTO;
            else
                return PRIMFCN;

        case TKT_COMMENT:
        case TKT_LINECONT:
            return yylex ();                // Ignore these tokens

        case TKT_STRING:
            return STRING;

        case TKT_OP1IMMED:
            // Handle the ambiguous slash(-bar) and slope(-bar) symbols
            if (gplLocalVars.lpNext->tkData.tkChar EQ '/'
             || gplLocalVars.lpNext->tkData.tkChar EQ '\\'
             || gplLocalVars.lpNext->tkData.tkChar EQ UCS2_SLASHBAR
             || gplLocalVars.lpNext->tkData.tkChar EQ UCS2_SLOPEBAR)
            {
                // We really need an FSA for this
                switch (LookaheadSurround (&gplLocalVars))
                {
                    case '2':               // If the next token is a dyadic operator,
                        return PRIMFCN;     //   then this token is a function

                    case '1':               // If the next token is a monadic operator,
                    case 'F':               // If the next token is a function,
                        return OP1;         //   then this token is an operator

                    case 'V':               // If the next token is a variable,
                        return PRIMFCN;     //   then this token is a function

                    case '?':               // SYNTAX ERROR
                        return UNK;
                } // End SWITCH
            } // End IF

            return OP1;

        case TKT_OP2IMMED:
            return OP2;

        case TKT_JOTDOT:
            return JOTDOT;

        case TKT_LPAREN:
            return '(';

        case TKT_RPAREN:
            // Lookahead to see if this right grouping symbol surrounds
            //   a function (return '>'), a variable (return ')'), or
            //   a dyadic operator (return '#')

            // We really need an FSA for this
            switch (LookaheadSurround (&gplLocalVars))
            {
                case '2':               // Dyadic operator
                    return '#';

                case '1':               // Monadic operator
                case 'F':               // Function
                    return '>';

                case 'V':               // Variable
                    return ')';

                case '?':               // SYNTAX ERROR
                    return UNK;
            } // End SWITCH

            DbgStop ();                 // We should never get here

            return UNK;

        case TKT_LBRACKET:
            return '[';

        case TKT_RBRACKET:
            // Lookahead to see if this right grouping symbol is adjacent to
            //   a function (return '}') or a variable (return ']')

            // We really need an FSA for this
            switch (LookaheadAdjacent (&gplLocalVars))
            {
                case '1':               // If the next token is a monadic operator,
                case 'F':               // If the next token is a function,
                    return '}';         //   then this token is an axis operator

                case 'V':               // If the next token is a variable,
                    return ']';         //   then this token is an indexing symbol

                case '2':               // If the next token is a dyadic operator,
                case '?':               // SYNTAX ERROR
                    return UNK;
            } // End SWITCH

            DbgStop ();                 // We should never get here

            return UNK;

        case TKT_EOS:
            // Skip to end of this stmt
            gplLocalVars.lpNext = &gplLocalVars.lpNext[gplLocalVars.lpNext->tkData.tkIndex];
            gplLocalVars.lpNext = &gplLocalVars.lpNext[gplLocalVars.lpNext->tkData.tkIndex];

            return DIAMOND;

        case TKT_EOL:
            return '\0';

        case TKT_STRAND:
        defstop
            return UNK;
    } // End SWITCH
} // End yylex


//***************************************************************************
//  yyerror
//
//  Error callback from YACC
//***************************************************************************

void yyerror                                // Called for yacc syntax error
    (LPCHAR s)

{
    char szTemp[32];
#ifdef DEBUG
    DbgMsg (s);
#endif

    gplLocalVars.lpError = &gplLocalVars.lpNext[1];

    // Check for SYNTAX ERROR
#define ERR     "syntax error"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
        ErrorMessageIndirect (L"SYNTAX ERROR"
#ifdef DEBUG
                              L" -- yyerror"
#endif
        );
#undef  ERR
    else
#define ERR     "memory exhausted"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
        ErrorMessageIndirect (L"WS FULL"
#ifdef DEBUG
                              L" -- yyerror"
#endif
        );
#undef  ERR
} // End yyerror


//***************************************************************************
//  yyfprintf
//
//  Debugger output
//***************************************************************************

void yyfprintf
    (FILE  *hfile,          // Ignore this
     LPCHAR lpszFmt,        // Format string
     ...)                   // Zero or more arguments

{
#if FALSE
    va_list vl;
    int     i1, i2, i3;
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

    wsprintf (lpszTemp,
              lpszFmt,
              i1, i2, i3);
    // Accumulate into local buffer because
    //   Bison calls this function multiple
    //   times for the same line, terminating
    //   the last call for the line with a LF.
    lstrcat (szTemp, lpszTemp);

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
} // End yyfprintf


//***************************************************************************
//  End of File: parse.y
//***************************************************************************
// WE ***MUST*** KEEP A BLANK LINE AS THE LAST LINE IN THIS FILE
