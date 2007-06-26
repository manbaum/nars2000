//***************************************************************************
//  NARS2000 -- Parser Grammar for executable lines
//***************************************************************************

/****************************************************************************

Parse a line of pre-tokenized tokens.

Based upon "The Syntax of APL:  An Old Approach Revisited" by
Jean Jacques Giardot & Florence Rollin, ACM SIGAPL Quote-Quad APL 1987
modified to work as an LALR grammar with the lookahead embedded
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
#include "pertab.h"

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

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         pl_yy_symbol_print
#define yy_symbol_value_print   pl_yy_symbol_value_print
#define yy_reduce_print         pl_yy_reduce_print
#define yydestruct              pl_yydestruct

LPYYSTYPE lpYYStr, lpYYStrL, lpYYStrR, lpYYStrN, lpYYRes, lpYYFcn, lpYYLst, lpYYAxis, lpYYOp1, lpYYOp2;
BOOL      bRet;

#define DbgMsgW2(a) DbgMsgW(a)
////#define DbgMsgW2(a) DbgMsgW(a); DbgBrk ()

%}

%pure-parser
%name-prefix="pl_yy"
%parse-param {LPPLLOCALVARS lpplLocalVars}
%lex-param   {LPPLLOCALVARS lpplLocalVars}

%token UNK SOL EOL
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

// Line
Line:
      Stmts                             {DbgMsgW2 (L"%%Line:  Stmts");}
    | Stmts ':' NAMEUNK                 {DbgMsgW2 (L"%%Line:  NAMEUNK: Stmts");}
    | Stmts ':' NAMEVAR                 {DbgMsgW2 (L"%%Line:  NAMEVAR: Stmts");}
    | Stmts ':' SYSLBL                  {DbgMsgW2 (L"%%Line:  SYSLBL: Stmts");}
    ;

// Statements
Stmts:
      // All errors propogate up to this point where we ABORT which ensures
      //   that the call to pl_yyparse terminates with a non-zero error code.
      error                             {DbgMsgW2 (L"%%Stmts:  error");
                                         YYABORT;
                                        }
    | Stmt                              {DbgMsgW2 (L"%%Stmts:  Stmt");
                                         if (!lpplLocalVars->LookAhead)
                                             Assert (YYResIsEmpty ());  // Likely not TRUE with non-empty SI
                                        }
    | Stmts DIAMOND Stmt                {DbgMsgW2 (L"%%Stmts:  Stmts " WS_UTF16_DIAMOND L" Stmt");}
    ;

// Statement
Stmt:
          /* Empty */                   {DbgMsgW2 (L"%%Stmt:  <empty>");}
    | ArrExpr                           {DbgMsgW2 (L"%%Stmt:  ArrExpr");
                                         if (lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                         {
                                             bRet = ArrayDisplay_EM (&$1.tkToken, TRUE);
                                             FreeResult (&$1.tkToken);
                                             if (!bRet)
                                                 YYERROR;
                                         } // End IF/ELSE
                                        }
////| StrandInst                        {DbgMsgW2 (L"%%Stmt:  StrandInst");
////                                     if (lpplLocalVars->LookAhead)
////                                         YYERROR;
////                                     else
////                                     {
////                                         bRet = ArrayDisplay_EM (&$1.tkToken, TRUE);
////                                         FreeResult (&$1.tkToken);
////                                         if (!bRet)
////                                             YYERROR;
////                                     } // End IF/ELSE
////                                    }
    | error   GOTO                      {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_RIGHTARROW L"error");
                                         if (lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    | ArrExpr GOTO                      {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_RIGHTARROW L"ArrExpr");
                                         if (lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                         {
                                             FreeResult (&$1.tkToken);




                                             PrimFnNonceError_EM (&$1.tkToken); YYERROR;
                                         } // End IF/ELSE
                                        }
////| StrandInst  GOTO                  {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_RIGHTARROW L"StrandInst");
////                                     if (lpplLocalVars->LookAhead)
////                                         YYERROR;
////                                     else
////                                     {
////                                         FreeResult (&$1.tkToken);
////
////
////
////
////                                         PrimFnNonceError_EM (&lpYYStr->tkToken); YYERROR;
////                                     } // End IF/ELSE
////                                    }
    |         GOTO                      {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_RIGHTARROW);
                                         if (lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                         {




                                             PrimFnNonceError_EM (&$1.tkToken); YYERROR;
                                         } // End IF/ELSE
                                        }
    | error   ASSIGN                    {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_LEFTARROW L"error");
                                         if (lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN                    {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                         {
                                             FreeResult (&$1.tkToken);
                                         } // End IF/ELSE
                                        }
////| StrandInst  ASSIGN                {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (lpplLocalVars->LookAhead)
////                                         YYERROR;
////                                     else
////                                         FreeResult (&$1.tkToken);
////                                    }
    | FcnSpec                           {DbgMsgW2 (L"%%Stmt:  FcnSpec");
                                         if (lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                             $$ = $1;
                                        }
    | Op1Spec                           {DbgMsgW2 (L"%%Stmt:  Op1Spec");
                                         if (lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                             $$ = $1;
                                        }
    | Op2Spec                           {DbgMsgW2 (L"%%Stmt:  Op2Spec");
                                         if (lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                             $$ = $1;
                                        }
    | SOL error   EOL                   {DbgMsgW2 (L"%%Stmt:  EOL error");
                                         if (lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    | SOL ArrExpr EOL                   {DbgMsgW2 (L"%%Stmt:  EOL ArrExpr");
                                         if (lpplLocalVars->LookAhead)
                                         {
                                             lpplLocalVars->ObjType = OBJTYPE_VAR;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
////| SOL StrandInst EOL                {DbgMsgW2 (L"%%Stmt:  EOL StrandInst");
////                                     if (lpplLocalVars->LookAhead)
////                                     {
////                                         lpplLocalVars->ObjType = OBJTYPE_VAR;
////                                         YYACCEPT;
////                                     } else
////                                         YYERROR;
////                                    }
    | SOL FcnSpec EOL                   {DbgMsgW2 (L"%%Stmt:  EOL FcnSpec");
                                         if (lpplLocalVars->LookAhead)
                                         {
                                             lpplLocalVars->ObjType = OBJTYPE_FCN;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    | SOL Op1Spec EOL                   {DbgMsgW2 (L"%%Stmt:  EOL Op1Spec");
                                         if (lpplLocalVars->LookAhead)
                                         {
                                             lpplLocalVars->ObjType = OBJTYPE_OP1;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    | SOL Op2Spec EOL                   {DbgMsgW2 (L"%%Stmt:  EOL Op2Spec");
                                         if (lpplLocalVars->LookAhead)
                                         {
                                             lpplLocalVars->ObjType = OBJTYPE_OP2;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    | SOL LeftOper EOL                  {DbgMsgW2 (L"%%Stmt:  EOL LeftOper");
                                         if (lpplLocalVars->LookAhead)
                                         {
                                             lpplLocalVars->ObjType = OBJTYPE_FCN;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    | SOL Drv1Func EOL                  {DbgMsgW2 (L"%%Stmt:  EOL Drv1Func");
                                         if (lpplLocalVars->LookAhead)
                                         {
                                             lpplLocalVars->ObjType = OBJTYPE_FCN;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    | SOL Drv2Func EOL                  {DbgMsgW2 (L"%%Stmt:  EOL Drv2Func");
                                         if (lpplLocalVars->LookAhead)
                                         {
                                             lpplLocalVars->ObjType = OBJTYPE_FCN;
                                             YYACCEPT;
                                         } else
                                             YYERROR;
                                        }
    ;

NameAnyVar:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyVar:  NAMEUNK");
                                         if (!lpplLocalVars->LookAhead)
                                            $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgW2 (L"%%NameAnyVar:  NAMEVAR");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

NameAnyFcn:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyFcn:  NAMEUNK");
                                         if (!lpplLocalVars->LookAhead)
                                             $$ = $1;
                                        }
    | NAMEFCN                           {DbgMsgW2 (L"%%NameAnyFcn:  NAMEFCN");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_FCNNAMED;
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

NameAnyOp1:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEUNK");
                                         if (!lpplLocalVars->LookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP1                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEOP1");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_OP1NAMED;
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

NameAnyOp2:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyOp2:  NAMEUNK");
                                         if (!lpplLocalVars->LookAhead)
                                             $$ = $1;
                                        }
    | NAMEOP2                           {DbgMsgW2 (L"%%NameAnyOp2:  NAMEOP2");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_OP2NAMED;
                                             $$ = $1;
                                         } // End IF
                                        }
    ;

// Function specification
FcnSpec:
      LeftOper ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UTF16_LEFTARROW L"LeftOper");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, FCNTYPE_FCN);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                             if (!bRet)
                                                 YYERROR;

                                             $$ = $3;
                                         } // End IF
                                        }
    | Drv1Func ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UTF16_LEFTARROW L"Drv1Func");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, FCNTYPE_FCN);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                             if (!bRet)
                                                 YYERROR;

                                             $$ = $3;
                                         } // End IF
                                        }
    | Drv2Func ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UTF16_LEFTARROW L"Drv2Func");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, FCNTYPE_FCN);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                             if (!bRet)
                                                 YYERROR;

                                             $$ = $3;
                                         } // End IF
                                        }
    | AxisFunc ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UTF16_LEFTARROW L"AxisFunc");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, FCNTYPE_AXISFCN);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                             if (!bRet)
                                                 YYERROR;

                                             $$ = $3;
                                         } // End IF
                                        }
    ;

// Monadic operator specification
Op1Spec:
      MonOp    ASSIGN NameAnyOp1        {DbgMsgW2 (L"%%Op1Spec:  NameAny" WS_UTF16_LEFTARROW L"MonOp");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, FCNTYPE_OP1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                             if (!bRet)
                                                 YYERROR;

                                             $$ = $3;
                                         } // End IF
                                        }
    | MonOpAxis ASSIGN NameAnyOp1       {DbgMsgW2 (L"%%Op1Spec:  NameAny" WS_UTF16_LEFTARROW L"MonOpAxis");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, FCNTYPE_OP1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                             if (!bRet)
                                                 YYERROR;

                                             $$ = $3;
                                         } // End IF
                                        }
    ;

// Dyadic operator specification
Op2Spec:
      DydOp    ASSIGN NameAnyOp2        {DbgMsgW2 (L"%%Op2Spec:  NameAny" WS_UTF16_LEFTARROW L"DydOp");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$1, FCNTYPE_OP2);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                             if (!bRet)
                                                 YYERROR;

                                             $$ = $3;
                                         } // End IF
                                        }
    ;

// Array expression
ArrExpr:
      SimpExpr                          {DbgMsgW2 (L"%%ArrExpr:  SimpExpr");
                                         if (!lpplLocalVars->LookAhead)
                                             $$ = $1;
                                        }
    | StrandInst                        {DbgMsgW2 (L"%%ArrExpr:  StrandInst");
                                         if (!lpplLocalVars->LookAhead)
                                             $$ = $1;
                                        }
    | error   LeftOper                  {DbgMsgW2 (L"%%ArrExpr:  LeftOper error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                             FreeResult (&$2.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr LeftOper                  {DbgMsgW2 (L"%%ArrExpr:  LeftOper ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpYYRes = ExecFunc_EM_YY (NULL, &lpYYFcn->tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| StrandInst LeftOper               {DbgMsgW2 (L"%%ArrExpr:  LeftOper StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYFcn =
////                                         MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$1.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes = ExecFunc_EM_YY (NULL, &lpYYFcn->tkToken, &$1.tkToken);
////                                         FreeResult (&$1.tkToken);
////                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    | error   Drv1Func                  {DbgMsgW2 (L"%%ArrExpr:  Drv1Func error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                             FreeResult (&$2.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr Drv1Func                  {DbgMsgW2 (L"%%ArrExpr:  Drv1Func ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpYYRes = ExecFunc_EM_YY (NULL, &lpYYFcn->tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| StrandInst Drv1Func               {DbgMsgW2 (L"%%ArrExpr:  Drv1Func StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYFcn =
////                                         MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$1.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes = ExecFunc_EM_YY (NULL, &lpYYFcn->tkToken, &$1.tkToken);
////                                         FreeResult (&$1.tkToken);
////                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    | error   AxisFunc                  {DbgMsgW2 (L"%%ArrExpr:  AxisFunc error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
                                             FreeResult (&$2.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr AxisFunc                  {DbgMsgW2 (L"%%ArrExpr:  AxisFunc ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpYYRes = ExecFunc_EM_YY (NULL, &lpYYFcn->tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| StrandInst AxisFunc               {DbgMsgW2 (L"%%ArrExpr:  AxisFunc StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYFcn =
////                                         MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$1.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes = ExecFunc_EM_YY (NULL, &lpYYFcn->tkToken, &$1.tkToken);
////                                         FreeResult (&$1.tkToken);
////                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    | ArrExpr LeftOper error            {DbgMsgW2 (L"%%ArrExpr:  error LeftOper ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$1.tkToken);

                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                             FreeResult (&$2.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
////| StrandInst LeftOper error         {DbgMsgW2 (L"%%ArrExpr:  error LeftOper StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         FreeResult (&$1.tkToken);
////
////                                         lpYYFcn =
////                                         MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (lpYYFcn)           // If defined, free it
////                                         {
////                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
////                                         } // End IF
////                                     } // End IF
////
////                                     YYERROR;
////                                    }
    | error   LeftOper StrandInst       {DbgMsgW2 (L"%%ArrExpr:  StrandInst LeftOper error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$3.tkToken);

                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                             FreeResult (&$2.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr LeftOper StrandInst       {DbgMsgW2 (L"%%ArrExpr:  StrandInst LeftOper ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpYYRes = ExecFunc_EM_YY (&$3.tkToken, &lpYYFcn->tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| StrandInst LeftOper StrandInst    {DbgMsgW2 (L"%%ArrExpr:  StrandInst LeftOper StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYFcn =
////                                         MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
////                                             FreeResult (&$1.tkToken);
////                                             FreeResult (&$3.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes = ExecFunc_EM_YY (&$3.tkToken, &lpYYFcn->tkToken, &$1.tkToken);
////                                         FreeResult (&$1.tkToken);
////                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
////                                         FreeResult (&$3.tkToken);
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    | ArrExpr AxisFunc error            {DbgMsgW2 (L"%%ArrExpr:  error AxisFunc ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$1.tkToken);

                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
                                             FreeResult (&$2.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
////| StrandInst AxisFunc error         {DbgMsgW2 (L"%%ArrExpr:  error AxisFunc StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         FreeResult (&$1.tkToken);
////
////                                         lpYYFcn =
////                                         MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (lpYYFcn)           // If defined, free it
////                                         {
////                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
////                                         } // End IF
////                                     } // End IF
////
////                                     YYERROR;
////                                    }
    | error   AxisFunc StrandInst       {DbgMsgW2 (L"%%ArrExpr:  StrandInst AxisFunc error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$3.tkToken);

                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
                                             FreeResult (&$2.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr AxisFunc StrandInst       {DbgMsgW2 (L"%%ArrExpr:  StrandInst AxisFunc ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpYYRes = ExecFunc_EM_YY (&$3.tkToken, &lpYYFcn->tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             FreeResult (&$3.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| StrandInst AxisFunc StrandInst    {DbgMsgW2 (L"%%ArrExpr:  StrandInst AxisFunc StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYFcn =
////                                         MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
////                                             FreeResult (&$1.tkToken);
////                                             FreeResult (&$3.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes = ExecFunc_EM_YY (&$3.tkToken, &lpYYFcn->tkToken, &$1.tkToken);
////                                         FreeResult (&$1.tkToken);
////                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
////                                         FreeResult (&$3.tkToken);
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    ;

// Single var (including single names)
SingVar:
                  NAMEUNK               {DbgMsgW2 (L"%%SingVar:  NAMEUNK");
                                         if (!lpplLocalVars->LookAhead)
                                             PrimFnValueError (&$1.tkToken);

                                         YYERROR;
                                        }
    |             QUAD                  {DbgMsgW2 (L"%%SingVar:  QUAD");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes = WaitForInput (lpplLocalVars->hWndSM, FALSE, &$1.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |             QUOTEQUAD             {DbgMsgW2 (L"%%SingVar:  QUOTEQUAD");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes = WaitForInput (lpplLocalVars->hWndSM, TRUE, &$1.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |             SingTokn              {DbgMsgW2 (L"%%SingVar:  SingTokn");
                                         if (!lpplLocalVars->LookAhead)
                                             $$ = $1;
                                        }
    |         ')' error   '('           {DbgMsgW2 (L"%%SingVar:  error");
                                         if (!lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    |         ')' ArrExpr '('           {DbgMsgW2 (L"%%SingVar:  (ArrExpr)");
                                         if (!lpplLocalVars->LookAhead)
                                             $$ = $2;
                                        }
////|         ')' StrandInst '('        {DbgMsgW2 (L"%%SingVar:  (StrandInst)");
////                                     if (!lpplLocalVars->LookAhead)
////                                         $$ = $2;
////                                    }
    ;

// Strand
Strand:
      SingVar                           {DbgMsgW2 (L"%%Strand:  SingVar -- InitVarStrand/PushVarStrand_YY");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             InitVarStrand (&$1);

                                             lpYYRes = PushVarStrand_YY (&$1);
/////////////////////////////////////////////FreeResult (&$1.tkToken);  // Copied w/o IncrRefCnt in PushVarStrand_YY

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | Strand      SingVar               {DbgMsgW2 (L"%%Strand:  SingVar Strand -- PushVarStrand_YY");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes = PushVarStrand_YY (&$2);
/////////////////////////////////////////////FreeResult (&$2.tkToken);  // Copied w/o IncrRefCnt in PushVarStrand_YY
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Strand instance
StrandInst:
      Strand                            {DbgMsgW2 (L"%%StrandInst:  Strand -- MakeVarStrand_EM_YY");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYStr =
                                             MakeVarStrand_EM_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYStr)           // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYStr; YYFree (lpYYStr); lpYYStr = NULL;
                                         } // End IF
                                        }
    ;

// Simple array expression
SimpExpr:
      ILPAR                             {DbgMsgW2 (L"%%SimpExpr:  ILPAR");
                                         if (!lpplLocalVars->LookAhead)
                                             $$ = $1;
                                        }
    | ILBR     StrandInst               {DbgMsgW2 (L"%%SimpExpr:  StrandInst ILBR");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes = ArrayIndex_EM_YY (&$2.tkToken, &$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | error   ASSIGN       QUAD         {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN       QUAD         {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             bRet = ArrayDisplay_EM (&$1.tkToken, TRUE);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result

                                             if (!bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| StrandInst ASSIGN    QUAD         {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         bRet = ArrayDisplay_EM (&$1.tkToken, TRUE);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result
////
////                                         if (!bRet)
////                                         {
////                                             FreeResult (&$1.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
////                                     } // End IF
////                                    }
    | error   ASSIGN       QUOTEQUAD    {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN       QUOTEQUAD    {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             bRet = ArrayDisplay_EM (&$1.tkToken, FALSE);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result

                                             if (!bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| StrandInst ASSIGN    QUOTEQUAD    {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         bRet = ArrayDisplay_EM (&$1.tkToken, FALSE);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result
////
////                                         if (!bRet)
////                                         {
////                                             FreeResult (&$1.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
////                                     } // End IF
////                                    }
    | error   ASSIGN       NameAnyVar   {DbgMsgW2 (L"%%SimpExpr:  NameAny" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN       NameAnyVar   {DbgMsgW2 (L"%%SimpExpr:  NameAny" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             bRet = AssignName_EM (&$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeResult (&$3.tkToken);

                                             if (!bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                         } // End IF
                                        }
////| StrandInst ASSIGN    NameAnyVar   {DbgMsgW2 (L"%%SimpExpr:  NameAny" WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         bRet = AssignName_EM (&$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result
////                                         FreeResult (&$3.tkToken);
////
////                                         if (!bRet)
////                                         {
////                                             FreeResult (&$1.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
////                                     } // End IF
////                                    }
    | error   ASSIGN ILBR  NAMEVAR      {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$4.tkToken);          // Validation only
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN ILBR  NAMEVAR      {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;



                                             PrimFnNonceError_EM (&$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$4.tkToken);          // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
////| StrandInst ASSIGN ILBR NAMEVAR    {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR" WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         $4.tkToken.tkFlags.TknType = TKT_VARNAMED;
////
////
////
////                                         PrimFnNonceError_EM (&$1.tkToken);
////                                         FreeResult (&$1.tkToken);
////
////                                         YYERROR;
////                                     } // End IF
////                                    }
    | error   ASSIGN   ')' NameSpec '(' {DbgMsgW2 (L"%%SimpExpr:  (NameSpec)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$4.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' NameSpec '(' {DbgMsgW2 (L"%%SimpExpr:  (NameSpec)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             bRet = AssignNameSpec_EM (&$4.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeResult (&$4.tkToken);

                                             if (!bRet)
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = $1;
                                         } // End IF
                                        }
////| StrandInst ASSIGN ')' NameSpec '('
////                                    {DbgMsgW2 (L"%%SimpExpr:  (NameSpec)" WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         bRet = AssignNameSpec_EM (&$4.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result
////                                         FreeResult (&$4.tkToken);
////
////                                         if (!bRet)
////                                         {
////                                             FreeResult (&$1.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         $$ = $1;
////                                     } // End IF
////                                    }
    | error   ASSIGN   ')' ILBR NAMEVAR '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NAMEVAR ILBR)" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$5.tkToken);          // Validation only
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' ILBR NAMEVAR '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NAMEVAR ILBR)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;



                                             PrimFnNonceError_EM (&$1.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$5.tkToken);          // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
////| StrandInst ASSIGN ')' ILBR NAMEVAR '('
////                                    {DbgMsgW2 (L"%%SimpExpr:  (NAMEVAR ILBR)" WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         $5.tkToken.tkFlags.TknType = TKT_VARNAMED;
////
////
////
////                                         PrimFnNonceError_EM (&lpYYStr->tkToken);
////                                         FreeResult (&$1.tkToken);
////                                         YYERROR;
////                                     } // End IF
////                                    }
    | error   ASSIGN LeftOper  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);          // Validation only
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN LeftOper  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             lpYYRes = ExecFunc_EM_YY (&$4.tkToken, &$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);                  // DO NOT FREE:  Used below

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$4.tkToken);      // Validation only
                                                 YYERROR;
                                             } // End IF

                                             bRet = AssignName_EM (&$4.tkToken, &lpYYRes->tkToken);
                                             FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;
                                             FreeResult (&$4.tkToken);

                                             if (!bRet)
                                                 YYERROR;

                                             $$ = $1;
                                         } // End IF
                                        }
////| StrandInst ASSIGN LeftOper NAMEVAR
////                                    {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR LeftOper" WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         $4.tkToken.tkFlags.TknType = TKT_VARNAMED;
////                                         lpYYRes = ExecFunc_EM_YY (&$4.tkToken, &$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result
////                                         FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);                  // DO NOT FREE:  Used below
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$1.tkToken);
////                                             FreeResult (&$4.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         bRet = AssignName_EM (&$4.tkToken, &lpYYRes->tkToken);
////                                         FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;
////                                         FreeResult (&$4.tkToken);
////
////                                         if (!bRet)
////                                             YYERROR;
////
////                                         $$ = $1;
////                                     } // End IF
////                                    }
    | error   ASSIGN AxisFunc  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN AxisFunc  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpYYRes = ExecFunc_EM_YY (&$4.tkToken, &$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result
                                             FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);                  // DO NOT FREE:  Used below

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             bRet = AssignName_EM (&$4.tkToken, &lpYYRes->tkToken);
                                             FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;
                                             FreeResult (&$4.tkToken);

                                             if (!bRet)
                                                 YYERROR;
                                             $$ = $1;
                                         } // End IF
                                        }
////| StrandInst ASSIGN AxisFunc NAMEVAR
////                                    {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         $4.tkToken.tkFlags.TknType = TKT_VARNAMED;
////
////                                         lpYYRes = ExecFunc_EM_YY (&$4.tkToken, &$3.tkToken, &$1.tkToken);
/////////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result
////                                         FreeResult (&$3.tkToken);
/////////////////////////////////////////////FreeResult (&$4.tkToken);                  // DO NOT FREE:  Used below
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$1.tkToken);
////                                             FreeResult (&$4.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         bRet = AssignName_EM (&$4.tkToken, &lpYYRes->tkToken);
////                                         FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;
////                                         FreeResult (&$4.tkToken);
////
////                                         if (!bRet)
////                                             YYERROR;
////
////                                         $$ = $1;
////                                     } // End IF
////                                    }
    | error   ASSIGN LeftOper ')' NameVals '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYStrN = MakeNameStrand_EM_YY (&$5);
                                             FreeResult (&$5.tkToken);

                                             if (lpYYStrN)          // If defined, free it
                                             {
                                                 FreeResult (&lpYYStrN->tkToken); YYFree (lpYYStrN); lpYYStrN = NULL;
                                             } // End IF

                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN LeftOper ')' NameVals '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYStrN = MakeNameStrand_EM_YY (&$5);
                                             FreeResult (&$5.tkToken);


                                             PrimFnNonceError_EM (&$1.tkToken);

                                             if (lpYYStrN)          // If defined, free it
                                             {
                                                 FreeResult (&lpYYStrN->tkToken); YYFree (lpYYStrN); lpYYStrN = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
////| StrandInst ASSIGN LeftOper ')' NameVals '('
////                                    {DbgMsgW2 (L"%%SimpExpr:  (NameVals) LeftOper" WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYStrN = MakeNameStrand_EM_YY (&$5);
////                                         FreeResult (&$5.tkToken);
////
////
////                                         PrimFnNonceError_EM (&lpYYStr->tkToken);
////
////                                         if (lpYYStrN)          // If defined, free it
////                                         {
////                                             FreeResult (&lpYYStrN->tkToken); YYFree (lpYYStrN); lpYYStrN = NULL;
////                                         } // End IF
////
////                                         FreeResult (&$1.tkToken);
////                                         FreeResult (&$3.tkToken);
////                                         YYERROR;
////                                     } // End IF
////                                    }
    | error   ASSIGN AxisFunc ')' NameVals '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYStrN = MakeNameStrand_EM_YY (&$5);
                                             FreeResult (&$5.tkToken);

                                             if (lpYYStrN)          // If defined, free it
                                             {
                                                 FreeResult (&lpYYStrN->tkToken); YYFree (lpYYStrN); lpYYStrN = NULL;
                                             } // End IF

                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN AxisFunc ')' NameVals '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYStrN = MakeNameStrand_EM_YY (&$5);
                                             FreeResult (&$5.tkToken);

                                             PrimFnNonceError_EM (&$1.tkToken);


                                             if (lpYYStrN)          // If defined, free it
                                             {
                                                 FreeResult (&lpYYStrN->tkToken); YYFree (lpYYStrN); lpYYStrN = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
////| StrandInst ASSIGN AxisFunc ')' NameVals '('
////                                    {DbgMsgW2 (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYStrN = MakeNameStrand_EM_YY (&$5);
////                                         FreeResult (&$5.tkToken);
////
////                                         PrimFnNonceError_EM (&lpYYStr->tkToken);
////
////
////                                         if (lpYYStrN)          // If defined, free it
////                                         {
////                                             FreeResult (&lpYYStrN->tkToken); YYFree (lpYYStrN); lpYYStrN = NULL;
////                                         } // End IF
////
////                                         FreeResult (&$1.tkToken);
////                                         FreeResult (&$3.tkToken);
////                                         YYERROR;
////                                     } // End IF
////                                    }
    | error   ASSIGN LeftOper ILBR NAMEVAR
                                        {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR LeftOper" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);          // Validation only
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ASSIGN LeftOper ILBR NAMEVAR
                                        {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR LeftOper" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;



                                             PrimFnNonceError_EM (&$1.tkToken);

                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
////| StrandInst ASSIGN LeftOper ILBR NAMEVAR
////                                    {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR LeftOper" WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         $5.tkToken.tkFlags.TknType = TKT_VARNAMED;
////
////
////
////                                         PrimFnNonceError_EM (&lpYYStr->tkToken);
////
////                                         FreeResult (&$1.tkToken);
////                                         FreeResult (&$3.tkToken);
////                                         YYERROR;
////                                     } // End IF
////                                    }
    | error   ASSIGN AxisFunc ILBR NAMEVAR
                                        {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR AxisFunc" WS_UTF16_LEFTARROW L"error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$5.tkToken);          // Validation only
                                             YYERROR;
                                         } // End IF
                                        }
    | ArrExpr ASSIGN AxisFunc ILBR NAMEVAR
                                        {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $5.tkToken.tkFlags.TknType = TKT_VARNAMED;



                                             PrimFnNonceError_EM (&$1.tkToken);

                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
////| StrandInst ASSIGN AxisFunc ILBR NAMEVAR
////                                    {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR AxisFunc" WS_UTF16_LEFTARROW L"StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         $5.tkToken.tkFlags.TknType = TKT_VARNAMED;
////
////
////
////                                         PrimFnNonceError_EM (&lpYYStr->tkToken);
////
////                                         FreeResult (&$1.tkToken);
////                                         FreeResult (&$3.tkToken);
////                                         YYERROR;
////                                     } // End IF
////                                    }
    ;

// Selective specification
NameSpec:
      NameVars                          {DbgMsgW2 (L"%%NameSpec:  NameVars");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYStrN = MakeNameStrand_EM_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYStrN)         // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYStrN; YYFree (lpYYStrN); lpYYStrN = NULL;
                                         } // End IF
                                        }
    | NameSpec LeftOper                 {DbgMsgW2 (L"%%NameSpec:  LeftOper NameSpec");
                                         if (!lpplLocalVars->LookAhead)
                                         {



                                             PrimFnNonceError_EM (&$1.tkToken);

                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
    | NameSpec AxisFunc                 {DbgMsgW2 (L"%%NameSpec:  AxisFunc NameSpec");
                                         if (!lpplLocalVars->LookAhead)
                                         {



                                             PrimFnNonceError_EM (&$1.tkToken);

                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
    | NameSpec LeftOper StrandInst      {DbgMsgW2 (L"%%NameSpec:  StrandInst LeftOper NameSpec");
                                         if (!lpplLocalVars->LookAhead)
                                         {



                                             PrimFnNonceError_EM (&$1.tkToken);

                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
    | NameSpec AxisFunc StrandInst      {DbgMsgW2 (L"%%NameSpec:  StrandInst AxisFunc NameSpec");
                                         if (!lpplLocalVars->LookAhead)
                                         {



                                             PrimFnNonceError_EM (&$1.tkToken);

                                             FreeResult (&$2.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
    ;

// Name Values
NameVals:
      NAMEVAR                           {DbgMsgW2 (L"%%NameVals:  NAMEVAR");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             InitNameStrand (&$1);

                                             lpYYRes = PushNameStrand_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | NameVals       NAMEVAR            {DbgMsgW2 (L"%%NameVals:  NameVals NAMEVAR");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $2.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpYYRes = PushNameStrand_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |       ')' ILBR NAMEVAR '('        {DbgMsgW2 (L"%%NameVals:  (NAMEVAR ILBR)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $3.tkToken.tkFlags.TknType = TKT_VARNAMED;




                                             PrimFnNonceError_EM (&$3.tkToken); YYERROR;
                                         } // End IF
                                        }
    | NameVals ')' ILBR NAMEVAR '('     {DbgMsgW2 (L"%%NameVals:  NameVals (NAMEVAR ILBR)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $4.tkToken.tkFlags.TknType = TKT_VARNAMED;




                                             PrimFnNonceError_EM (&$4.tkToken); YYERROR;
                                         } // End IF
                                        }
    ;

// Name Strand
NameVars:
      NAMEVAR                           {DbgMsgW2 (L"%%NameVars:  NAMEVAR");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             InitNameStrand (&$1);

                                             lpYYRes = PushNameStrand_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | NAMEUNK                           {DbgMsgW2 (L"%%NameVars:  NAMEUNK");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             InitNameStrand (&$1);

                                             lpYYRes = PushNameStrand_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | NameVars       NAMEVAR            {DbgMsgW2 (L"%%NameVars:  NameVars NAMEVAR");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $2.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                             lpYYRes = PushNameStrand_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | NameVars       NAMEUNK            {DbgMsgW2 (L"%%NameVars:  NameVars NAMEUNK");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes = PushNameStrand_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 1 (Valid in ArrExpr: and FcnSpec:)
Drv1Func:
      RightOper DydOp StrandInst        {DbgMsgW2 (L"%%Drv1Func:  StrandInst DydOp RightOper");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$3, 1, FALSE);      // Left operand (Direct)
                                             FreeResult (&$3.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Right operand (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |                  MonOp StrandInst {DbgMsgW2 (L"%%Drv1Func:  StrandInst MonOp");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, FALSE);      // Only operand (Direct)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |              MonOpAxis StrandInst {DbgMsgW2 (L"%%Drv1Func:  StrandInst MonOpAxis");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, FALSE);      // Only operand (Direct)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Derived left function expression, Type 2 (valid in FcnSpec: but not ArrExpr: because of Strand on the right)
Drv2Func:
      StrandInst DydOp LeftOper         {DbgMsgW2 (L"%%Drv2Func:  LeftOper DydOp StrandInst");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$3, 1, TRUE);       // Left operand (Indirect)
                                             FreeResult (&$3.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, FALSE);      // Right operand (Direct)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | StrandInst DydOp AxisFunc         {DbgMsgW2 (L"%%Drv2Func:  AxisFunc DydOp StrandInst");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 FreeResult (&$3.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$3, 1, TRUE);       // Left operand (Indirect)
                                             FreeResult (&$3.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, FALSE);      // Right operand (Direct)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Parenthesized function expression
ParenFunc:
      '>' LeftOper '('                  {DbgMsgW2 (L"%%ParenFunc:  (LeftOper)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | '>' AxisFunc '('                  {DbgMsgW2 (L"%%ParenFunc:  (AxisFunc)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | '>' FcnSpec '('                   {DbgMsgW2 (L"%%ParenFunc:  (FcnSpec)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Left operand function
LeftOper:
          PRIMFCN                       {DbgMsgW2 (L"%%LeftOper:  PRIMFCN");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakePrimFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN                       {DbgMsgW2 (L"%%LeftOper:  NAMEFCN");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakeNameFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |     SYSFN12                       {DbgMsgW2 (L"%%LeftOper:  SYSFN12");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakeNameFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |     ParenFunc                     {DbgMsgW2 (L"%%LeftOper:  ParenFunc");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |     RightOper    JOTDOT           {DbgMsgW2 (L"%%LeftOper:  " WS_UTF16_JOT L". RightOper");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 2, FALSE);      // Monadic operator (Direct)
                                         FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Operand (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |                  MonOp LeftOper   {DbgMsgW2 (L"%%LeftOper:  LeftOper MonOp");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Only operand (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |              MonOpAxis LeftOper   {DbgMsgW2 (L"%%LeftOper:  LeftOper MonOpAxis");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Only operand (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |                  MonOp AxisFunc   {DbgMsgW2 (L"%%LeftOper:  AxisFunc MonOp");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Only operand (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |              MonOpAxis AxisFunc   {DbgMsgW2 (L"%%LeftOper:  AxisFunc MonOpAxis");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Only operand (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |     RightOper DydOp LeftOper      {DbgMsgW2 (L"%%LeftOper:  LeftOper DydOp RightOper");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$3, 1, TRUE);       // Left operand (Indirect)
                                             FreeResult (&$3.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Right operand (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |     RightOper DydOp AxisFunc      {DbgMsgW2 (L"%%LeftOper:  AxisFunc DydOp RightOper");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 3, TRUE);       // Dyadic operator (Indirect)
                                         FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$3.tkToken);
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$3, 1, TRUE);       // Left operand (Indirect)
                                             FreeResult (&$3.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$1.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Right operand (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | '>' Drv1Func                 '('  {DbgMsgW2 (L"%%LeftOper:  (Drv1Func)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | '>' Drv2Func                 '('  {DbgMsgW2 (L"%%LeftOper:  (Drv2Func)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | '>'     StrandInst   DydOp StrandInst '('
                                        {DbgMsgW2 (L"%%LeftOper:  (StrandInst DydOp StrandInst)");  // ***FIXME*** -- Put into Drv2Func??
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$3, 3, TRUE);       // Dyadic operator (Indirect)
                                             FreeResult (&$3.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$4, 1, FALSE);      // Left operand (Direct)
                                             FreeResult (&$4.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, FALSE);      // Right operand (Direct)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Right operand function (short right scope)
RightOper:
          PRIMFCN                       {DbgMsgW2 (L"%%RightOper:  PRIMFCN");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakePrimFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |     NAMEFCN                       {DbgMsgW2 (L"%%RightOper:  NAMEFCN");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakeNameFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |     SYSFN12                       {DbgMsgW2 (L"%%RightOper:  SYSFN12");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakeNameFcn_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYFcn, 1, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |     ParenFunc                     {DbgMsgW2 (L"%%RightOper:  ParenFunc");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$1, 1, TRUE);       // Function (Indirect)
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Axis function expression
AxisFunc:
      '}' error   '['  PRIMFCN          {DbgMsgW2 (L"%%AxisFunc:  PRIMFCN[error]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakePrimFcn_YY (&$4);
                                             FreeResult (&$4.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
      '}' ArrExpr '['  PRIMFCN          {DbgMsgW2 (L"%%AxisFunc:  PRIMFCN[ArrExpr]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakePrimFcn_YY (&$4);
                                             FreeResult (&$4.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYFcn, 2, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYAxis = MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYAxis)         // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYAxis, 1, FALSE); // Axis value (Direct)
                                             FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| '}' StrandInst '[' PRIMFCN        {DbgMsgW2 (L"%%AxisFunc:  PRIMFCN[StrandInst]");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYFcn = MakePrimFcn_YY (&$4);
////                                         FreeResult (&$4.tkToken);
////
////                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$2.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes =
////                                         PushFcnStrand_YY (lpYYFcn, 2, FALSE);  // Function (Direct)
////                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$2.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////
////                                         lpYYAxis = MakeAxis_YY (&$2);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (!lpYYAxis)         // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         lpYYRes =
////                                         PushFcnStrand_YY (lpYYAxis, 1, FALSE); // Axis value (Direct)
////                                         FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    | '}' error   '['  NAMEFCN          {DbgMsgW2 (L"%%AxisFunc:  NAMEFCN[error]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakeNameFcn_YY (&$4);
                                             FreeResult (&$4.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  NAMEFCN          {DbgMsgW2 (L"%%AxisFunc:  NAMEFCN[ArrExpr]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakeNameFcn_YY (&$4);
                                             FreeResult (&$4.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYFcn, 2, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYAxis = MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYAxis)         // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYAxis, 1, FALSE); // Axis value (Direct)
                                             FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| '}' StrandInst '[' NAMEFCN        {DbgMsgW2 (L"%%AxisFunc:  NAMEFCN[StrandInst]");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYFcn = MakeNameFcn_YY (&$4);
////                                         FreeResult (&$4.tkToken);
////
////                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$2.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes =
////                                         PushFcnStrand_YY (lpYYFcn, 2, FALSE);  // Function (Direct)
////                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$2.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////
////                                         lpYYAxis = MakeAxis_YY (&$2);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (!lpYYAxis)         // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         lpYYRes =
////                                         PushFcnStrand_YY (lpYYAxis, 1, FALSE); // Axis value (Direct)
////                                         FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    | '}' error   '['  SYSFN12          {DbgMsgW2 (L"%%AxisFunc:  SYSFN12[error]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakeNameFcn_YY (&$4);
                                             FreeResult (&$1.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  SYSFN12          {DbgMsgW2 (L"%%AxisFunc:  SYSFN12[ArrExpr]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakeNameFcn_YY (&$4);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYFcn, 2, FALSE);  // Function (Direct)
                                             FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYAxis = MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYAxis)         // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYAxis, 1, FALSE); // Axis value (Direct)
                                             FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| '}' StrandInst '[' SYSFN12        {DbgMsgW2 (L"%%AxisFunc:  SYSFN12[StrandInst]");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYFcn = MakeNameFcn_YY (&$4);
////                                         FreeResult (&$4.tkToken);
////
////                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$2.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes =
////                                         PushFcnStrand_YY (lpYYFcn, 2, FALSE);  // Function (Direct)
////                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$2.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////
////                                         lpYYAxis = MakeAxis_YY (&$2);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (!lpYYAxis)         // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         lpYYRes =
////                                         PushFcnStrand_YY (lpYYAxis, 1, FALSE); // Axis value (Direct)
////                                         FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    | '}' error   '['  ParenFunc        {DbgMsgW2 (L"%%AxisFunc:  ParenFunc[error]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$4, FCNTYPE_FCN);
                                             FreeResult (&$4.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | '}' ArrExpr '['  ParenFunc        {DbgMsgW2 (L"%%AxisFunc:  ParenFunc[ArrExpr]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYAxis = MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYAxis)         // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYAxis, 1, FALSE); // Axis value (Direct)
                                             FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$4.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             YYFree (lpYYRes); lpYYRes = NULL;

                                             $$ = $4;
                                         } // End IF
                                        }
////| '}' StrandInst '[' ParenFunc      {DbgMsgW2 (L"%%AxisFunc:  ParenFunc[StrandInst]");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYAxis = MakeAxis_YY (&$2);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (!lpYYAxis)         // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$4.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes =
////                                         PushFcnStrand_YY (lpYYAxis, 1, FALSE); // Axis value (Direct)
////                                         FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$4.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         YYFree (lpYYRes); lpYYRes = NULL;
////
////                                         $$ = $4;
////                                     } // End IF
////                                    }
    ;

// Monadic operator
MonOp:
                       OP1              {DbgMsgW2 (L"%%MonOp:  OP1");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYOp1 = MakeOp1_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYOp1)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYOp1, 1, FALSE);  // Monadic operator (Direct)
                                             FreeResult (&lpYYOp1->tkToken); YYFree (lpYYOp1); lpYYOp1 = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |                  NAMEOP1          {DbgMsgW2 (L"%%MonOp:  NAMEOP1");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn = MakeNameOp1_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYFcn)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYFcn, 1, FALSE);  // Monadic operator (Direct)
                                             FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |              '>' MonOp '('        {DbgMsgW2 (L"%%MonOp:  (MonOp)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |              '>' Op1Spec '('     {DbgMsgW2 (L"%%MonOp:  (Op1Spec)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Monadic operator with axis
MonOpAxis:
      '}' error   '['  MonOp            {DbgMsgW2 (L"%%MonOpAxis:  MonOp[error]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYFcn =
                                             MakeFcnStrand_EM_YY (&$4, FCNTYPE_FCN);
                                             FreeResult (&$4.tkToken);

                                             if (lpYYFcn)           // If defined, free it
                                             {
                                                 FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF

                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
      '}' ArrExpr '['  MonOp            {DbgMsgW2 (L"%%MonOpAxis:  MonOp[ArrExpr]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$4, 2, TRUE);       // Monadic operator (Indirect)
                                             FreeResult (&$4.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                             {
                                                 FreeResult (&$2.tkToken);
                                                 YYERROR;
                                             } // End IF

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                             lpYYAxis = MakeAxis_YY (&$2);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYAxis)         // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYAxis, 1, FALSE); // Axis value (Direct)
                                             FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| '}' StrandInst '[' MonOp          {DbgMsgW2 (L"%%MonOp:  MonOp[StrandInst]");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYRes =
////                                         PushFcnStrand_YY (&$4, 2, TRUE);       // Monadic operator (Indirect)
////                                         FreeResult (&$4.tkToken);
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$2.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////
////                                         lpYYAxis = MakeAxis_YY (&$2);
////                                         FreeResult (&$2.tkToken);
////
////                                         if (!lpYYAxis)         // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         lpYYRes =
////                                         PushFcnStrand_YY (lpYYAxis, 1, FALSE); // Axis value (Direct)
////                                         FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    ;

// Dyadic operator
DydOp:
                       OP2              {DbgMsgW2 (L"%%DydOp:  OP2");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYOp2 = MakeOp2_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYOp2)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYOp2, 1, FALSE);  // Dyadic operator (Direct)
                                             FreeResult (&lpYYOp2->tkToken); YYFree (lpYYOp2); lpYYOp2 = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |                  NAMEOP2          {DbgMsgW2 (L"%%DydOp:  NAMEOP2");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYOp2 = MakeNameOp2_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYOp2)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushFcnStrand_YY (lpYYOp2, 1, FALSE);  // Dyadic operator (Direct)
                                             FreeResult (&lpYYOp2->tkToken); YYFree (lpYYOp2); lpYYOp2 = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |              '#' DydOp '('        {DbgMsgW2 (L"%%DydOp:  (DydOp)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, FALSE);      // Dyadic operator (Direct)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |              '#' Op2Spec '('      {DbgMsgW2 (L"%%DydOp:  (Op2Spec)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushFcnStrand_YY (&$2, 1, FALSE);      // Dyadic operator (Direct)
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// Index list w/brackets, allowing for A[A][A]...
ILBR:
           ']' ILWE '['                 {DbgMsgW2 (L"%%ILBR:  [ILWE]");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYLst = MakeList_EM_YY (&$2, TRUE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYLst)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYLst; YYFree (lpYYLst); lpYYLst = NULL;
                                         } // End IF
                                        }
    | ILBR ']' ILWE '['                 {DbgMsgW2 (L"%%ILBR:  ILBR [ILWE]");
                                         if (!lpplLocalVars->LookAhead)
                                         {




                                             PrimFnNonceError_EM (&$3.tkToken);
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } // End IF
                                        }
    ;

// Index list w/parens
ILPAR:
      ')' ILNE '('                      {DbgMsgW2 (L"%%ILPAR:  (ILNE)");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYLst = MakeList_EM_YY (&$2, FALSE);
                                             FreeResult (&$2.tkToken);

                                             if (!lpYYLst)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYLst; YYFree (lpYYLst); lpYYLst = NULL;
                                         } // End IF
                                        }
    ;

// Index list, no empties
ILNE:
      error   ';' ArrExpr               {DbgMsgW2 (L"%%ILNE:  ArrExpr;error");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ArrExpr ';' ArrExpr               {DbgMsgW2 (L"%%ILNE:  ArrExpr;ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYLst =
                                             InitList1_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYLst)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             lpYYRes =
                                             PushList_YY (lpYYLst, &$3);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&lpYYLst->tkToken); YYFree (lpYYLst); lpYYLst = NULL;

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| ArrExpr ';' StrandInst            {DbgMsgW2 (L"%%ILNE:  StrandInst;ArrExpr");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYLst =
////                                         InitList1_YY (&$1);
////                                         FreeResult (&$1.tkToken);
////
////                                         if (!lpYYLst)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$3.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes =
////                                         PushList_YY (lpYYLst, &$3);
////                                         FreeResult (&$3.tkToken);
////                                         FreeResult (&lpYYLst->tkToken); YYFree (lpYYLst); lpYYLst = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
////| StrandInst ';' ArrExpr            {DbgMsgW2 (L"%%ILNE:  ArrExpr;StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYLst =
////                                         InitList1_YY (&$1);
////                                         FreeResult (&$1.tkToken);
////
////                                         if (!lpYYLst)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$3.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes =
////                                         PushList_YY (lpYYLst, &$3);
////                                         FreeResult (&$3.tkToken);
////                                         FreeResult (&lpYYLst->tkToken); YYFree (lpYYLst); lpYYLst = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
////| StrandInst ';' StrandInst         {DbgMsgW2 (L"%%ILNE:  StrandInst;StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYLst =
////                                         InitList1_YY (&$1);
////                                         FreeResult (&$1.tkToken);
////
////                                         if (!lpYYLst)          // If not defined, free args and YYERROR
////                                         {
////                                             FreeResult (&$3.tkToken);
////                                             YYERROR;
////                                         } // End IF
////
////                                         lpYYRes =
////                                         PushList_YY (lpYYLst, &$3);
////                                         FreeResult (&$3.tkToken);
////                                         FreeResult (&lpYYLst->tkToken); YYFree (lpYYLst); lpYYLst = NULL;
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    | ILNE    ';' error                 {DbgMsgW2 (L"%%ILNE:  error;ILNE");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ILNE    ';' ArrExpr               {DbgMsgW2 (L"%%ILNE:  ArrExpr;ILNE");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushList_YY (&$1, &$3);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| ILNE    ';' StrandInst            {DbgMsgW2 (L"%%ILNE:  StrandInst;ILNE");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYRes =
////                                         PushList_YY (&$1, &$3);
////                                         FreeResult (&$3.tkToken);
////                                         FreeResult (&$1.tkToken);
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    ;

// Index list, with empties
ILWE:
      /* Empty */                       {DbgMsgW2 (L"%%ILWE:  <empty>");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             InitList0_YY ();

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    |          error                    {DbgMsgW2 (L"%%ILWE:  error");
                                         if (!lpplLocalVars->LookAhead)
                                             YYERROR;
                                         else
                                             YYERROR;
                                        }
    |          ArrExpr                  {DbgMsgW2 (L"%%ILWE:  ArrExpr");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             InitList1_YY (&$1);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////|          StrandInst               {DbgMsgW2 (L"%%ILWE:  StrandInst");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYRes =
////                                         InitList1_YY (&$1);
////                                         FreeResult (&$1.tkToken);
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    | ILWE ';' error                    {DbgMsgW2 (L"%%ILWE:  error;ILWE");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } else
                                             YYERROR;
                                        }
    | ILWE ';' ArrExpr                  {DbgMsgW2 (L"%%ILWE:  ArrExpr;ILWE");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushList_YY (&$1, &$3);
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
////| ILWE ';' StrandInst               {DbgMsgW2 (L"%%ILWE:  StrandInst;ILWE");
////                                     if (!lpplLocalVars->LookAhead)
////                                     {
////                                         lpYYRes =
////                                         PushList_YY (&$1, &$3);
////                                         FreeResult (&$3.tkToken);
////                                         FreeResult (&$1.tkToken);
////
////                                         if (!lpYYRes)          // If not defined, free args and YYERROR
////                                             YYERROR;
////
////                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////                                     } // End IF
////                                    }
    | ILWE ';'                          {DbgMsgW2 (L"%%ILWE:  ILWE;");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYRes =
                                             PushList_YY (&$1, NULL);
                                             FreeResult (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    ;

// A data expression which has a single token
SingTokn:
      CONSTANT                          {DbgMsgW2 (L"%%SingTokn:  CONSTANT");
                                         if (!lpplLocalVars->LookAhead)
                                             $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgW2 (L"%%SingTokn:  NAMEVAR");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                             $$ = $1;
                                         } // End IF
                                        }
    | USRFN0                            {DbgMsgW2 (L"%%SingTokn:  USRFN0");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_FCNNAMED;
                                             $1.tkToken.tkFlags.FcnDir  = 0;

                                             lpYYRes =
                                             ExecuteFn0 (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | SYSFN0                            {DbgMsgW2 (L"%%SingTokn:  SYSFN0");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             $1.tkToken.tkFlags.TknType = TKT_FCNNAMED;
                                             $1.tkToken.tkFlags.FcnDir  = 1;

                                             lpYYRes =
                                             ExecuteFn0 (&$1.tkToken);

                                             if (!lpYYRes)          // If not defined, free args and YYERROR
                                                 YYERROR;

                                             $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                         } // End IF
                                        }
    | STRING                            {DbgMsgW2 (L"%%SingTokn:  STRING");
                                         if (!lpplLocalVars->LookAhead)
                                         {
                                             lpYYStr = CopyString_EM_YY (&$1);

                                             if (!lpYYStr)
                                                 YYERROR;

                                             $$ = *lpYYStr; YYFree (lpYYStr); lpYYStr = NULL;
                                         } // End IF
                                        }
    ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************

typedef struct tagPL_THREAD
{
    HANDLE  hSemaphore;         // Handle of semaphore for this thread
    HGLOBAL hGlbToken;          // Handle of tokenization of line to parse
    LPWCHAR lpwszLine;          // Ptr to tokens of line to parse (GlobalLock of above)
    HGLOBAL hGlbPTD;            // Handle of PerTabData for this thread
    BOOL    bFreeToken;         // TRUE iff ParseLine is to free hGlbToken on exit
    HWND    hWndSM;             // Window handle to Session Manager
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
    (HWND    hWndSM,            // Window handle to Session Manager
     HGLOBAL hGlbToken,         // The token handle
     LPWCHAR lpwszLine,         // The line to parse
     HGLOBAL hGlbPTD,           // PTD handle
     BOOL    bFreeToken)        // TRUE iff ParseLine is to free hGlbToken on exit

{
    DWORD  dwThreadId;
    HANDLE hThread,
           hSemaphore;

    // Create a semaphore
    hSemaphore = CreateSemaphore (NULL,             // No security attrs
                                  0,                // Initial count (non-signalled)
                                  65535,            // Maximum count
                                  NULL);            // No name
    // Save args in struc to pass to thread func
    plThread.hSemaphore = hSemaphore;
    plThread.hGlbToken  = hGlbToken;
    plThread.lpwszLine  = lpwszLine;
    plThread.hGlbPTD    = hGlbPTD;
    plThread.bFreeToken = bFreeToken;
    plThread.hWndSM     = hWndSM;

    // Create a new thread
    hThread = CreateThread (NULL,                   // No security attrs
                            0,                      // Use default stack size
                            (LPTHREAD_START_ROUTINE) &ParseLineInThread,
                            (LPVOID) &plThread,     // Param to thread func
                            0,                      // Creation flag
                            &dwThreadId);           // Returns thread id
    Sleep (0);              // To cut short the caller's thread slice

    // Tell the SM to wait for PL to finish
    PostMessage (hWndSM, MYWM_WFMO, (WPARAM) hThread, (LPARAM) hSemaphore);
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

void ParseLineInThread
    (LPPL_THREAD lpplThread)

{
    HANDLE       hSemaphore;        // Handle to the ParseLine semaphore
    HGLOBAL      hGlbToken;         // Handle to sequence of tokens to parse
    LPWCHAR      lpwszLine;         // Ptr to WCHARs in the line to parse
    HGLOBAL      hGlbPTD;           // Handle to PerTabData
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    BOOL         bFreeToken;        // TRUE iff we should free hGlbToken on exit
    PLLOCALVARS  plLocalVars = {0}; // Re-entrant vars
    HWND         hWndSM;            // Window handle to Session Manager

    // Make sure we can communicate between windows
    AttachThreadInput (GetCurrentThreadId (), dwMainThreadId, TRUE);

    // Save the thread type ('PL')
    TlsSetValue (dwTlsType, (LPVOID) 'PL');

    // Save the thread's semaphore handle
    TlsSetValue (dwTlsSemaphore, (LPVOID) lpplThread->hSemaphore);

    // Save the thread's ptr to local vars
    TlsSetValue (dwTlsPlLocalVars, (LPVOID) &plLocalVars);

    DBGENTER;

    // Extract values from the arg struc
    hSemaphore = lpplThread->hSemaphore;
    hGlbToken  = lpplThread->hGlbToken;
    lpwszLine  = lpplThread->lpwszLine;
    hGlbPTD    = lpplThread->hGlbPTD;
    bFreeToken = lpplThread->bFreeToken;
    hWndSM     = lpplThread->hWndSM;

    // Save the thread's PerTabData global memory handle
    TlsSetValue (dwTlsPerTabData, (LPVOID) hGlbPTD);

    // If we don't have a valid handle, ...
    if (!hGlbToken)
        return;
#ifdef DEBUG
    { // ***DEBUG***
        // Display the tokens so far
        DisplayTokens (hGlbToken);
    } // ***DEBUG*** END
#endif

    // Save values in the LocalVars
    plLocalVars.hGlbPTD   = hGlbPTD;
    plLocalVars.hWndSM    = hWndSM;
    plLocalVars.hGlbToken = hGlbToken;
    plLocalVars.LookAhead = LOOKAHEAD_NRM;

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
                      DEF_STRAND_MAXSIZE * sizeof (YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!plLocalVars.lpYYStrandStart[STRAND_VAR])
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ParseLine:  VirtualAlloc for <pLocalVars.lpYYStrandStart[STRAND_VAR]> failed");

        return;             // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (plLocalVars.lpYYStrandStart[STRAND_VAR],
                  DEF_STRAND_INITSIZE * sizeof (YYSTYPE),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Allocate virtual memory for the Function Strand accumulator
    plLocalVars.lpYYStrandStart[STRAND_FCN] =
        VirtualAlloc (NULL,      // Any address
                      DEF_STRAND_MAXSIZE * sizeof (YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!plLocalVars.lpYYStrandStart[STRAND_FCN])
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ParseLine:  VirtualAlloc for <pLocalVars.lpYYStrandStart[STRAND_FCN]> failed");

        return;             // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (plLocalVars.lpYYStrandStart[STRAND_FCN],
                  DEF_STRAND_INITSIZE * sizeof (YYSTYPE),
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
    if (!pl_yyparse (&plLocalVars))
        goto NORMAL_EXIT;

//ERROR_EXIT:
    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Signal an error
    ErrorMessage (lpMemPTD->lpwszErrorMessage, lpwszLine, plLocalVars.tkErrorCharIndex);

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

    // Untokenize and free hGlbToken if requested
    if (bFreeToken)
    {
        // Free the tokens
        Untokenize (hGlbToken);

        // We no longer need this storage
        MyGlobalFree (hGlbToken); hGlbToken = NULL;
    } // End IF

    // Free the virtual memory for the complete line
    VirtualFree (lpwszLine, 0, MEM_RELEASE); lpwszLine = NULL;

    DBGLEAVE;

    // Release it
    if (!ReleaseSemaphore (hSemaphore, 1, NULL))
        DbgBrk ();
} // End ParseLineInThread
#undef  APPEND_NAME


//***************************************************************************
//  $SymbTypeFV
//
//  Get the type ('F' or 'V', or '?') of a symbol table name token.
//  Used for TKT_VARNAMEDs.
//***************************************************************************

char SymbTypeFV
    (LPTOKEN lpNext)

{
    LPSTFLAGS lpstFlags;

    // Get a ptr to the symbol table flags
    lpstFlags = &lpNext->tkData.tkSym->stFlags;

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
} // SymbTypeFV


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
    (LPPLLOCALVARS lpplLocalVars)

{
    PLLOCALVARS plLocalVars;    // Local copy of outer PLLOCALVARS
    char        cRes;           // The result char

    DbgMsgW (L"==Entering LookaheadSurround");

    // Copy outer lpplLocalVars
    plLocalVars = *lpplLocalVars;

    // Mark as looking ahead for token type inside surrounding parens
    plLocalVars.LookAhead = LOOKAHEAD_SUR;

    // Mark as needing initialization
    plLocalVars.InitAhead = TRUE;

    // Save the address of the stop token
    // Each right grouping symbol has the index of the matching
    //   left grouping symbol in its tkData.tkIndex
    plLocalVars.lpStop = &plLocalVars.lpStart[plLocalVars.lpNext->tkData.tkIndex];

    // Parse the file, check for errors
    if (pl_yyparse (&plLocalVars))
        cRes = '?';
    else
        cRes = "V12F"[plLocalVars.ObjType];

#ifdef DEBUG
    dprintfW (L"==Exiting  LookaheadSurround:  %c", cRes);
#endif
    return cRes;
////     while (TRUE)
////     switch (lpNext->tkFlags.TknType)
////     {
////         case TKT_STRING:
////         case TKT_VARIMMED:
////         case TKT_VARARRAY:
////         case TKT_INPOUT:
////             // Lookahead to see if the next token is a dyadic operator:
////             //   if so, return 'F'; otherwise return 'V'.
////             switch (LookaheadDyadicOp (lpplLocalVars, &lpNext[-1]))
////             {
////                 case TRUE:
////                     return 'F';     // Function
////
////                 case FALSE:
////                     return 'V';     // Variable
////
////                 default:
////                     return '?';     // SYNTAX ERROR
////             } // End SWITCH
////
////         case TKT_OP1IMMED:
////         case TKT_OP1NAMED:
////             return '1';             // Monadic operator
////
////         case TKT_OP2IMMED:
////         case TKT_OP2NAMED:
////             return '2';             // Dyadic operator
////
////         case TKT_FCNIMMED:
////         case TKT_OPJOTDOT:
////         case TKT_ASSIGN:
////         case TKT_EOS:
////         case TKT_EOL:
////             return 'F';             // Function
////
//// ////////// The EOS/EOL cases are for when we're called for /1 or \1
//// //////////   (that is, monadic slash or backslash).
//// ////////// I prefer not to trigger a SYNTAX ERROR during the parse
//// //////////   but instead during semantic analysis.
//// ////////case TKT_EOS:
//// ////////case TKT_EOL:
////         case TKT_LISTPAR:
////         case TKT_LISTBR:
////             return 'V';             // Variable
////
////         case TKT_VARNAMED:
////             // Get the token type of the symbol table name
////             return SymbTypeFV (lpNext);
////
////         case TKT_RBRACKET:
////             // Each right grouping symbol has the index of the matching
////             //   left grouping symbol in its tkData.tkIndex
////             // Get a ptr to the token adjacent to ("-1") the matching left
////             //   grouping symbol.
////             lpNext = &lpplLocalVars->lpStart[lpNext->tkData.tkIndex - 1];
////
////             break;
////
////         case TKT_LINECONT:
////         case TKT_RPAREN:
////             lpNext--;               // Ignore this token
////
////             break;                  // Go around again
////
////         case TKT_LISTSEP:
////         case TKT_COLON:
////         case TKT_COMMENT:
////         case TKT_LPAREN:
////         case TKT_LBRACKET:
////         case TKT_STRAND:
////             return '?';             // SYNTAX ERROR
////
////         defstop
////             return '?';
////     } // End WHILE/SWITCH
} // LookaheadSurround
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
    (LPPLLOCALVARS lpplLocalVars)

{
    PLLOCALVARS plLocalVars;    // Local copy of outer PLLOCALVARS
    char        cRes;           // The result char

    DbgMsgW (L"==Entering LookaheadAdjacent");

    // Copy outer lpplLocalVars
    plLocalVars = *lpplLocalVars;

////// Mark as looking ahead for token type past the adjacent left bracket
////plLocalVars.LookAhead = LOOKAHEAD_ADJ;

    // Each right grouping symbol has the index of the matching
    //   left grouping symbol in its tkData.tkIndex.
    // Get a ptr to the token adjacent to ("-1") the matching left
    //   grouping symbol.
    plLocalVars.lpNext = &plLocalVars.lpStart[plLocalVars.lpNext->tkData.tkIndex - 1];

////// Parse the file, check for errors
////if (pl_yyparse (&plLocalVars))
////    cRes = '?';
////else
////    cRes = "V12F"[plLocalVars.ObjType];
////
////DbgMsgW (L"==Exiting  LookaheadAdjacent");
////
////return cRes;

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

                default:
                    cRes = '?';     // SYNTAX ERROR

                    break;
            } // End SWITCH

            goto NORMAL_EXIT;

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
            cRes = '1';

            goto NORMAL_EXIT;

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
            cRes = '2';

            goto NORMAL_EXIT;

        case TKT_FCNIMMED:
        case TKT_OPJOTDOT:
            cRes = 'F';             // Function

            goto NORMAL_EXIT;

        case TKT_VARNAMED:
            // Get the token type of the symbol table name
            cRes = SymbTypeFV (plLocalVars.lpNext);

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
            plLocalVars.lpNext--;   // Ignore this token

            break;                  // Go around again

        case TKT_ASSIGN:
        case TKT_LISTSEP:
        case TKT_COLON:
        case TKT_COMMENT:
        case TKT_LPAREN:
        case TKT_LBRACKET:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_STRAND:
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
            bRet = lpNext->tkData.tkSym->stFlags.UsrOp2;

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
    (LPYYSTYPE     lpYYLval,        // Ptr to lval
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
#ifdef DEBUG
    static UINT Index = 0;
#endif
    WCHAR       wchFn;

    // Check for LookAhead initialization   // ***FIXME*** -- Omit SOL??
    if (lpplLocalVars->InitAhead)
    {
        lpplLocalVars->InitAhead = FALSE;
        return SOL;
    } // End IF

    // Because we're parsing the stmt from right to left
    lpplLocalVars->lpNext--;

    // Return the current token
    lpYYLval->tkToken  = *lpplLocalVars->lpNext;
    lpYYLval->Inuse    =
    lpYYLval->Indirect =
    lpYYLval->Rsvd     =
    lpYYLval->TknCount =
    lpYYLval->FcnCount = 0;
    lpYYLval->lpYYFcn  = NULL;
#ifdef DEBUG
    lpYYLval->Flag     = 1;         // Mark as a pl_yylex Index
    lpYYLval->Index    = ++Index;
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

        case TKT_VARNAMED:
        {
            STFLAGS stFlags;        // STE flags

            stFlags = lpplLocalVars->lpNext->tkData.tkSym->stFlags;

            // We need a bit a lookahead here so handle the FCN vs. OP1 vs. OP2
            //   cases when the name is being assigned to.  In this case,
            //   because the current meaning of the name is irrelevant
            //   (it's about to be reassigned), we can accept any kind of name.
            //   LALR grammars have a hard time dealing with this, so we
            //   lookahead one token to see if it is ASSIGN.  If so, we return
            //   NAMEUNK, a universal named token.

            // If this is a UsrVar and either it has no value
            //   or the next token is ASSIGN, call it NAMEUNK.
            if (stFlags.UsrVar
             && (!stFlags.Value
              || lpplLocalVars->lpNext[1].tkFlags.TknType EQ TKT_ASSIGN))
                return NAMEUNK;
            else
            if (stFlags.UsrVar
             || stFlags.SysVar)
            {
////////////////lpplLocalVars->lpNext->tkFlags.TknType = TKT_VARNAMED;    // Already set

                return NAMEVAR;
            } else
            if (stFlags.UsrFn0)
                return USRFN0;
            else
            if (stFlags.SysFn0)
                return SYSFN0;
            else
            if (stFlags.UsrFn12)
            {
                lpplLocalVars->lpNext->tkFlags.TknType = TKT_FCNNAMED;

                return NAMEFCN;
            } else
            if (stFlags.SysFn12)
            {
                lpplLocalVars->lpNext->tkFlags.TknType = TKT_FCNNAMED;
                lpplLocalVars->lpNext->tkFlags.FcnDir  = 1;

                return SYSFN12;
            } else
            if (stFlags.UsrOp1)
            {
                lpplLocalVars->lpNext->tkFlags.TknType = TKT_OP1NAMED;

                return NAMEOP1;
            } else
            if (stFlags.UsrOp2)
            {
                lpplLocalVars->lpNext->tkFlags.TknType = TKT_OP2NAMED;

                return NAMEOP2;
            } else
            if (stFlags.DfnSysLabel)
                return SYSLBL;
            else
            if (stFlags.UsrName)
                return NAMEUNK;

            DbgStop ();         // We should never get here

            return UNK;
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
                switch (LookaheadSurround (lpplLocalVars))
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

                    case 'V':               // If the next token is a variable,
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
            if (lpplLocalVars->LookAhead EQ LOOKAHEAD_SUR
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
            switch (LookaheadAdjacent (lpplLocalVars))
            {
                case '1':               // If the next token is a monadic operator,
                case 'F':               // If the next token is a function,
                    return '}';         //   then this token is an axis operator

                case 'V':               // If the next token is a variable,
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
#ifdef DEBUG
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
    (FILE         *yyoutput,
     yytype_uint16 yytoknum,
     YYSTYPE const yyvaluep)

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
