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

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               pl_yyfprintf
#endif

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         pl_yy_symbol_print
#define yy_symbol_value_print   pl_yy_symbol_value_print
#define yy_reduce_print         pl_yy_reduce_print
#define yydestruct              pl_yydestruct

LPYYSTYPE lpYYStr, lpYYStrL, lpYYStrR, lpYYRes, lpYYFcn, lpYYLst, lpYYAxis, lpYYOp1, lpYYOp2;
BOOL      bRet;

#define DbgMsgW2(a) DbgMsgW(a)
////#define DbgMsgW2(a) DbgMsgW(a); DbgBrk ()

%}

%pure-parser
%name-prefix="pl_yy"
%parse-param {LPPLLOCALVARS lpplLocalVars}
%lex-param   {LPPLLOCALVARS lpplLocalVars}

%token UNK
%token NAMEVAR NAMEUNK CONSTANT STRING USRFN0 SYSFN0 QUAD QUOTEQUAD
%token DIAMOND

/*  Note that as we parse APL from right to left, these rules
    are all reversed as well as token associativity (long scope).
    That is, functions in APL have long right scope, so they are
    declared here in the %left list.  Similarly, operators in APL
    have long left scope, so they are declared in the %right list.
 */
%left  ASSIGN PRIMFCN NAMEFCN SYSFN12 GOTO
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
      //   that the call to pl_yyparse terminates with a non-zero error code.
      error                             {DbgMsgW2 (L"%%Stmts:  error"); YYABORT;}
    | Stmt                              {DbgMsgW2 (L"%%Stmts:  Stmt");
                                         Assert (YYResIsEmpty ());  // Likely not TRUE with non-empty SI
                                        }
    | Stmts DIAMOND Stmt                {DbgMsgW2 (L"%%Stmts:  Stmts " WS_UTF16_DIAMOND L" Stmt");}
    ;

/* Statement */
Stmt:     /* Empty */                   {DbgMsgW2 (L"%%Stmt:  <empty>");}
    | ArrExpr                           {DbgMsgW2 (L"%%Stmt:  ArrExpr");
                                         bRet = ArrayDisplay_EM (&$1.tkToken, TRUE);
                                         FreeResult (&$1.tkToken);
                                         if (!bRet)
                                             YYERROR;
                                        }
    | ArrExpr GOTO                      {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_RIGHTARROW L"ArrExpr");
                                         FreeResult (&$1.tkToken);




                                         PrimFnNonceError_EM (&$1.tkToken); YYERROR;
                                        }
    |         GOTO                      {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_RIGHTARROW);




                                         PrimFnNonceError_EM (&$1.tkToken); YYERROR;
                                        }
    | ArrExpr ASSIGN                    {DbgMsgW2 (L"%%Stmt:  " WS_UTF16_LEFTARROW L"ArrExpr");
                                         FreeResult (&$1.tkToken);
                                        }
    | FcnSpec                           {DbgMsgW2 (L"%%Stmt:  FcnSpec");
                                         $$ = $1;
                                        }
    | Op1Spec                           {DbgMsgW2 (L"%%Stmt:  Op1Spec");
                                         $$ = $1;
                                        }
    | Op2Spec                           {DbgMsgW2 (L"%%Stmt:  Op2Spec");
                                         $$ = $1;
                                        }
    ;

NameAnyVar:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyVar:  NAMEUNK");
                                         $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgW2 (L"%%NameAnyVar:  NAMEVAR");
                                         $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                         $$ = $1;
                                        }
    ;

NameAnyFcn:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyFcn:  NAMEUNK");
                                         $$ = $1;
                                        }
    | NAMEFCN                           {DbgMsgW2 (L"%%NameAnyFcn:  NAMEFCN");
                                         $1.tkToken.tkFlags.TknType = TKT_FCNNAMED;
                                         $$ = $1;
                                        }
    ;

NameAnyOp1:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEUNK");
                                         $$ = $1;
                                        }
    | NAMEOP1                           {DbgMsgW2 (L"%%NameAnyOp1:  NAMEOP1");
                                         $1.tkToken.tkFlags.TknType = TKT_OP1NAMED;
                                         $$ = $1;
                                        }
    ;

NameAnyOp2:
      NAMEUNK                           {DbgMsgW2 (L"%%NameAnyOp2:  NAMEUNK");
                                         $$ = $1;
                                        }
    | NAMEOP2                           {DbgMsgW2 (L"%%NameAnyOp2:  NAMEOP2");
                                         $1.tkToken.tkFlags.TknType = TKT_OP2NAMED;
                                         $$ = $1;
                                        }
    ;

// Function specification
FcnSpec:
      LeftFunc ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UTF16_LEFTARROW L"LeftFunc");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$1, FCNTYPE_FCN);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;
                                        }
    | AxisFunc ASSIGN NameAnyFcn        {DbgMsgW2 (L"%%FcnSpec:  NameAny" WS_UTF16_LEFTARROW L"AxisFunc");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$1, FCNTYPE_AXISFCN);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;

                                         if (!bRet)
                                             YYERROR;

                                         $$ = $3;
                                        }
    ;

// Monadic operator specification
Op1Spec:
      MonOp    ASSIGN NameAnyOp1        {DbgMsgW2 (L"%%Op1Spec:  NameAny" WS_UTF16_LEFTARROW L"MonOp");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$1, FCNTYPE_OP1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;
                                        }
    | MonOpAxis ASSIGN NameAnyOp1       {DbgMsgW2 (L"%%Op1Spec:  NameAny" WS_UTF16_LEFTARROW L"MonOpAxis");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$1, FCNTYPE_OP1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                            YYERROR;
                                         bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;
                                        }
    ;

// Dyadic operator specification
Op2Spec:
      DydOp    ASSIGN NameAnyOp2        {DbgMsgW2 (L"%%Op2Spec:  NameAny" WS_UTF16_LEFTARROW L"DydOp");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$1, FCNTYPE_OP2);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         bRet = AssignName_EM (&$3.tkToken, &lpYYFcn->tkToken);
/////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;
                                        }
    ;

/* Array expression */
ArrExpr:
      Strand                            {DbgMsgW2 (L"%%ArrExpr:  Strand -- MakeVarStrand_EM_YY");
                                         lpYYStr = MakeVarStrand_EM_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYStr; YYFree (lpYYStr); lpYYStr = NULL;
                                        }
    | ArrExpr1                          {DbgMsgW2 (L"%%ArrExpr:  ArrExpr1");
                                         $$ = $1;
                                        }
    ;

/* Array expression from function */
ArrExpr1:
      SimpExpr                          {DbgMsgW2 (L"%%ArrExpr1:  SimpExpr");
                                         $$ = $1;
                                        }
    | error   LeftFunc                  {DbgMsgW2 (L"%%ArrExpr1:  LeftFunc error");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                         FreeResult (&$2.tkToken);

                                         if (lpYYFcn)       // If defined, free it
                                         {
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr LeftFunc                  {DbgMsgW2 (L"%%ArrExpr1:  LeftFunc ArrExpr");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
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
                                        }
    | error   AxisFunc                  {DbgMsgW2 (L"%%ArrExpr1:  AxisFunc error");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
                                         FreeResult (&$2.tkToken);

                                         if (lpYYFcn)           // If defined, free it
                                         {
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr AxisFunc                  {DbgMsgW2 (L"%%ArrExpr1:  AxisFunc ArrExpr");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
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
                                        }
    | ArrExpr LeftFunc error            {DbgMsgW2 (L"%%ArrExpr1:  error LeftFunc ArrExpr");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                         FreeResult (&$2.tkToken);
                                         FreeResult (&$1.tkToken);

                                         if (lpYYFcn)           // If defined, free it
                                         {
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         } // End IF

                                         YYERROR;
                                        }
    | error   LeftFunc Strand           {DbgMsgW2 (L"%%ArrExpr1:  Strand LeftFunc error");
                                         lpYYStr = MakeVarStrand_EM_YY (&$3);
                                         FreeResult (&$3.tkToken);

                                         lpYYFcn = MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                         FreeResult (&$2.tkToken);

                                         if (lpYYStr)           // If defined, free it
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                         } // End IF

                                         if (lpYYFcn)           // If defined, free it
                                         {
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         } // End IF

                                         YYERROR;
                                        }
    | ArrExpr LeftFunc Strand           {DbgMsgW2 (L"%%ArrExpr1:  Strand LeftFunc ArrExpr");
                                         lpYYStr = MakeVarStrand_EM_YY (&$3);
                                         FreeResult (&$3.tkToken);

                                         lpYYFcn = MakeFcnStrand_EM_YY (&$2, FCNTYPE_FCN);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                         {
                                             if (lpYYFcn)       // If defined, free it
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF

                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$1.tkToken);
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         }
                                         lpYYRes = ExecFunc_EM_YY (&lpYYStr->tkToken, &lpYYFcn->tkToken, &$1.tkToken);
                                         FreeResult (&$1.tkToken);
                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | ArrExpr AxisFunc error            {DbgMsgW2 (L"%%ArrExpr1:  error AxisFunc ArrExpr");
                                         lpYYFcn = MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
                                         FreeResult (&$2.tkToken);
                                         FreeResult (&$1.tkToken);

                                         if (lpYYFcn)           // If defined, free it
                                         {
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         } // End IF

                                         YYERROR;
                                        }
    | error   AxisFunc Strand           {DbgMsgW2 (L"%%ArrExpr1:  Strand AxisFunc error");
                                         lpYYStr = MakeVarStrand_EM_YY (&$3);
                                         FreeResult (&$3.tkToken);

                                         lpYYFcn = MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
                                         FreeResult (&$2.tkToken);

                                         if (lpYYStr)       // If defined, free it
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                         } // End IF

                                         if (lpYYFcn)       // If defined, free it
                                         {
                                             FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         } // End IF
                                         YYERROR;
                                        }
    | ArrExpr AxisFunc Strand           {DbgMsgW2 (L"%%ArrExpr1:  Strand AxisFunc ArrExpr");
                                         lpYYStr = MakeVarStrand_EM_YY (&$3);
                                         FreeResult (&$3.tkToken);

                                         lpYYFcn = MakeFcnStrand_EM_YY (&$2, FCNTYPE_AXISFCN);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYStr || !lpYYFcn)      // If not defined, free args and YYERROR
                                         {
                                             if (lpYYFcn)       // If defined, free it
                                             {
                                                 FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                             } // End IF
                                             if (lpYYStr)       // If defined, free it
                                                FreeResult (&lpYYStr->tkToken);
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF

                                         lpYYRes = ExecFunc_EM_YY (&lpYYStr->tkToken, &lpYYFcn->tkToken, &$1.tkToken);
                                         FreeResult (&$1.tkToken);
                                         FreeYYFcn (lpYYFcn); lpYYFcn = NULL;
                                         FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    ;

/* Strand (including single names) */
Strand:           NAMEUNK               {DbgMsgW2 (L"%%Strand:  NAMEUNK");
                                         PrimFnValueError (&$1.tkToken);
                                         YYERROR;
                                        }
    |             QUAD                  {DbgMsgW2 (L"%%Strand:  QUAD");
                                         lpYYRes = WaitForInput (hWndSM, FALSE, &$1.tkToken);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |             QUOTEQUAD             {DbgMsgW2 (L"%%Strand:  QUOTEQUAD");
                                         lpYYRes = WaitForInput (hWndSM, TRUE, &$1.tkToken);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |             SingTokn              {DbgMsgW2 (L"%%Strand:  SingTokn -- InitVarStrand/PushVarStrand_YY");
                                         InitVarStrand (&$1);

                                         lpYYRes = PushVarStrand_YY (&$1);
/////////////////////////////////////////FreeResult (&$1.tkToken);  // Copied in PushVarStrand_YY

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |         ')' ArrExpr '('           {DbgMsgW2 (L"%%Strand:  (ArrExpr) -- InitVarStrand/PushVarStrand_YY");
                                         InitVarStrand (&$2);

                                         lpYYRes = PushVarStrand_YY (&$2);
/////////////////////////////////////////FreeResult (&$2.tkToken);  // Copied in PushVarStrand_YY

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | Strand      NAMEUNK               {DbgMsgW2 (L"%%Strand:  Strand NAMEUNK");
                                         PrimFnValueError (&$1.tkToken);
                                         FreeResult (&$1.tkToken);
                                         YYERROR;
                                        }
    | Strand      SingTokn              {DbgMsgW2 (L"%%Strand:  Strand SingTokn -- PushVarStrand_YY");
                                         lpYYRes = PushVarStrand_YY (&$2);
/////////////////////////////////////////FreeResult (&$2.tkToken);  // Copied in PushVarStrand_YY
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | Strand  ')' ArrExpr '('           {DbgMsgW2 (L"%%Strand:  Strand (ArrExpr) -- PushVarStrand_YY");
                                         lpYYRes = PushVarStrand_YY (&$3);
/////////////////////////////////////////FreeResult (&$3.tkToken);  // Copied in PushVarStrand_YY
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    ;

/* Simple array expression */
SimpExpr:
////           Strand                   {DbgMsgW2 (L"%%SimpExpr:  Strand -- MakeVarStrand_EM_YY");
////                                     lpYYStr = MakeVarStrand_EM_YY (&$1);
////                                     FreeResult (&$1.tkToken);
////
////                                     if (!lpYYStr)          // If not defined, free args and YYERROR
////                                         YYERROR;
////
////                                     $$ = *lpYYStr; YYFree (lpYYStr); lpYYStr = NULL;
////                                    }
      ILPAR                             {DbgMsgW2 (L"%%SimpExpr:  ILPAR");
                                         $$ = $1;}
    | ILBR     Strand                   {DbgMsgW2 (L"%%SimpExpr:  Strand ILBR");
                                         lpYYStr = MakeVarStrand_EM_YY (&$2);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF

                                         lpYYRes = ArrayIndex_EM_YY (&lpYYStr->tkToken, &$1.tkToken);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | ArrExpr ASSIGN       QUAD         {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUAD WS_UTF16_LEFTARROW L"ArrExpr");
                                         bRet = ArrayDisplay_EM (&$1.tkToken, TRUE);
/////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result

                                         if (!bRet)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                        }
    | ArrExpr ASSIGN       QUOTEQUAD    {DbgMsgW2 (L"%%SimpExpr:  " WS_UTF16_QUOTEQUAD WS_UTF16_LEFTARROW L"ArrExpr");
                                         bRet = ArrayDisplay_EM (&$1.tkToken, FALSE);
/////////////////////////////////////////FreeResult (&$1.tkToken);                  // DO NOT FREE:  Passed on as result

                                         if (!bRet)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = $1; $$.tkToken.tkFlags.NoDisplay = TRUE;
                                        }
    | ArrExpr ASSIGN       NameAnyVar   {DbgMsgW2 (L"%%SimpExpr:  NameAny" WS_UTF16_LEFTARROW L"ArrExpr");
                                         bRet = AssignName_EM (&$3.tkToken, &$1.tkToken);
/////////////////////////////////////////FreeResult (&$3.tkToken);                  // DO NOT FREE:  Passed on as result
                                         FreeResult (&$1.tkToken);
                                         if (!bRet)
                                             YYERROR;
                                         $$ = $3;
                                        }
    | ArrExpr ASSIGN ILBR  NAMEVAR      {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR" WS_UTF16_LEFTARROW L"ArrExpr");
                                         $4.tkToken.tkFlags.TknType = TKT_VARNAMED;



                                         PrimFnNonceError_EM (&$1.tkToken);
                                         FreeResult (&$1.tkToken);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN   ')' NameSpec '(' {DbgMsgW2 (L"%%SimpExpr:  (NameSpec)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         bRet = AssignNameSpec_EM (&$4.tkToken, &$1.tkToken);
                                         FreeResult (&$4.tkToken);
                                         if (!bRet)
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF
/////////////////////////////////////////FreeResult (&$1.tkToken);  // DO NOT FREE:  Passed on as result
                                         $$ = $1;
                                        }
    | ArrExpr ASSIGN   ')' ILBR NAMEVAR '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NAMEVAR ILBR)" WS_UTF16_LEFTARROW L"ArrExpr");
                                         $5.tkToken.tkFlags.TknType = TKT_VARNAMED;



                                         PrimFnNonceError_EM (&$1.tkToken);
                                         FreeResult (&$1.tkToken);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN LeftFunc  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR LeftFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         $4.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                         lpYYRes = ExecFunc_EM_YY (&$4.tkToken, &$3.tkToken, &$1.tkToken);
                                         FreeResult (&$1.tkToken);
                                         FreeResult (&$3.tkToken);
                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$4.tkToken);
                                             YYERROR;
                                         } // End IF

                                         bRet = AssignName_EM (&$4.tkToken, &lpYYRes->tkToken);
                                         FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;
/////////////////////////////////////////FreeResult (&$4.tkToken);                  // DO NOT FREE:  Passed on as result

                                         if (!bRet)
                                             YYERROR;
                                         $$ = $4;
                                        }
    | ArrExpr ASSIGN AxisFunc  NAMEVAR  {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         $4.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                         lpYYRes = ExecFunc_EM_YY (&$4.tkToken, &$3.tkToken, &$1.tkToken);
                                         FreeResult (&$1.tkToken);
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$4.tkToken);
                                             YYERROR;
                                         } // End IF

                                         bRet = AssignName_EM (&$4.tkToken, &lpYYRes->tkToken);
                                         FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;
/////////////////////////////////////////FreeResult (&$4.tkToken);                  // DO NOT FREE:  Passed on as result

                                         if (!bRet)
                                             YYERROR;
                                         $$ = $4;
                                        }
    | ArrExpr ASSIGN LeftFunc ')' NameVals '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NameVals) LeftFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         lpYYStr = MakeNameStrand_EM_YY (&$5);
                                         FreeResult (&$5.tkToken);


                                         PrimFnNonceError_EM (&$1.tkToken);

                                         if (lpYYStr)           // If defined, free it
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                         } // End IF

                                         FreeResult (&$1.tkToken);
                                         FreeResult (&$3.tkToken);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN AxisFunc ')' NameVals '('
                                        {DbgMsgW2 (L"%%SimpExpr:  (NameVals) AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         lpYYStr = MakeNameStrand_EM_YY (&$5);
                                         FreeResult (&$5.tkToken);

                                         PrimFnNonceError_EM (&$1.tkToken);


                                         if (lpYYStr)           // If defined, free it
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                         } // End IF

                                         FreeResult (&$1.tkToken);
                                         FreeResult (&$3.tkToken);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN LeftFunc ILBR NAMEVAR
                                        {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR LeftFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         $5.tkToken.tkFlags.TknType = TKT_VARNAMED;



                                         PrimFnNonceError_EM (&$1.tkToken);

                                         FreeResult (&$1.tkToken);
                                         FreeResult (&$3.tkToken);
                                         YYERROR;
                                        }
    | ArrExpr ASSIGN AxisFunc ILBR NAMEVAR
                                        {DbgMsgW2 (L"%%SimpExpr:  NAMEVAR ILBR AxisFunc" WS_UTF16_LEFTARROW L"ArrExpr");
                                         $5.tkToken.tkFlags.TknType = TKT_VARNAMED;



                                         PrimFnNonceError_EM (&$1.tkToken);

                                         FreeResult (&$1.tkToken);
                                         FreeResult (&$3.tkToken);
                                         YYERROR;
                                        }
    ;

/* Selective specification */
NameSpec:
      NameVars                          {DbgMsgW2 (L"%%NameSpec:  NameVars");
                                         lpYYRes = MakeNameStrand_EM_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | NameSpec LeftFunc                 {DbgMsgW2 (L"%%NameSpec:  LeftFunc NameSpec");



                                         PrimFnNonceError_EM (&$1.tkToken);

                                         FreeResult (&$2.tkToken);
                                         YYERROR;
                                        }
    | NameSpec AxisFunc                 {DbgMsgW2 (L"%%NameSpec:  AxisFunc NameSpec");



                                         PrimFnNonceError_EM (&$1.tkToken);

                                         FreeResult (&$2.tkToken);
                                         YYERROR;
                                        }
    | NameSpec LeftFunc Strand          {DbgMsgW2 (L"%%NameSpec:  Strand LeftFunc NameSpec");
                                         lpYYStr = MakeVarStrand_EM_YY (&$3);
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF


                                         PrimFnNonceError_EM (&$1.tkToken);

                                         FreeResult (&$2.tkToken);
                                         FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                         YYERROR;
                                        }
    | NameSpec AxisFunc Strand          {DbgMsgW2 (L"%%NameSpec:  Strand AxisFunc NameSpec");
                                         lpYYStr = MakeVarStrand_EM_YY (&$3);
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF


                                         PrimFnNonceError_EM (&$1.tkToken);

                                         FreeResult (&$2.tkToken);
                                         FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                         YYERROR;
                                        }
    ;

/* Name Values */
NameVals:
      NAMEVAR                           {DbgMsgW2 (L"%%NameVals:  NAMEVAR");
                                         $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                         InitNameStrand (&$1);

                                         lpYYRes = PushNameStrand_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | NameVals       NAMEVAR            {DbgMsgW2 (L"%%NameVals:  NameVals NAMEVAR");
                                         $2.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                         lpYYRes = PushNameStrand_YY (&$2);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
////|       ')' ILBR NAMEVAR '('        {DbgMsgW2 (L"%%NameVals:  (NAMEVAR ILBR)");
////                                     $3.tkToken.tkFlags.TknType = TKT_VARNAMED;
////
////
////
////
////                                     PrimFnNonceError_EM (&$3.tkToken); YYERROR;
////                                    }
////| NameVals ')' ILBR NAMEVAR '('     {DbgMsgW2 (L"%%NameVals:  NameVals (NAMEVAR ILBR)");
////                                     $4.tkToken.tkFlags.TknType = TKT_VARNAMED;
////
////
////
////
////                                     PrimFnNonceError_EM (&$4.tkToken); YYERROR;
////                                    }
    ;

/* Name Strand */
NameVars:
      NAMEVAR                           {DbgMsgW2 (L"%%NameVars:  NAMEVAR");
                                         $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                         InitNameStrand (&$1);

                                         lpYYRes = PushNameStrand_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | NAMEUNK                           {DbgMsgW2 (L"%%NameVars:  NAMEUNK");
                                         InitNameStrand (&$1);

                                         lpYYRes = PushNameStrand_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | NameVars       NAMEVAR            {DbgMsgW2 (L"%%NameVars:  NameVars NAMEVAR");
                                         $2.tkToken.tkFlags.TknType = TKT_VARNAMED;

                                         lpYYRes = PushNameStrand_YY (&$2);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | NameVars       NAMEUNK            {DbgMsgW2 (L"%%NameVars:  NameVars NAMEUNK");
                                         lpYYRes = PushNameStrand_YY (&$2);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    ;

/* Left function expression */
LeftFunc:
          PRIMFCN                       {DbgMsgW2 (L"%%LeftFunc:  PRIMFCN");
                                         lpYYFcn = MakePrimFcn_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 1, FALSE);   // Function
                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |     NAMEFCN                       {DbgMsgW2 (L"%%LeftFunc:  NAMEFCN");
                                         lpYYFcn = MakeNameFcn_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 1, FALSE);   // Function
                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |     SYSFN12                       {DbgMsgW2 (L"%%LeftFunc:  SYSFN12");
                                         lpYYFcn = MakeNameFcn_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 1, FALSE);   // Function
                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |     SimpFunc                      {DbgMsgW2 (L"%%LeftFunc:  SimpFunc");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$1, 1, TRUE);        // Function
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |     RightFunc    JOTDOT           {DbgMsgW2 (L"%%LeftFunc:  " WS_UTF16_JOT L". RightFunc");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 2, FALSE);       // Monadic operator
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$1, 1, TRUE);        // Operand
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |                  MonOp LeftFunc   {DbgMsgW2 (L"%%LeftFunc:  LeftFunc MonOp");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$1, 2, TRUE);        // Monadic operator
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Operand
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |              MonOpAxis LeftFunc   {DbgMsgW2 (L"%%LeftFunc:  LeftFunc MonOpAxis");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$1, 2, TRUE);        // Monadic operator
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Operand
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |                  MonOp AxisFunc   {DbgMsgW2 (L"%%LeftFunc:  AxisFunc MonOp");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$1, 2, TRUE);        // Monadic operator
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Operand
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |              MonOpAxis AxisFunc   {DbgMsgW2 (L"%%LeftFunc:  AxisFunc MonOpAxis");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$1, 2, TRUE);        // Monadic operator
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Operand
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |              '>' MonOp Strand '(' {DbgMsgW2 (L"%%LeftFunc:  (Strand MonOp)");
                                         lpYYStr = MakeVarStrand_EM_YY (&$3);
                                            FreeResult (&$3.tkToken);

                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF

                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 2, TRUE);        // Monadic operator
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYStr, 1, FALSE);   // Operand
                                         FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                         }
    |          '>' MonOpAxis Strand '(' {DbgMsgW2 (L"%%LeftFunc:  (Strand MonOpAxis)");
                                         lpYYStr = MakeVarStrand_EM_YY (&$3);
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF

                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 2, TRUE);        // Monadic operator
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYStr, 1, FALSE);   // Operand
                                         FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |     RightFunc DydOp LeftFunc      {DbgMsgW2 (L"%%LeftFunc:  LeftFunc DydOp RightFunc");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 3, TRUE);        // Dyadic operator
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$3, 1, TRUE);        // Left operand
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF

                                         YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$1, 1, TRUE);        // Right operand
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |     RightFunc DydOp AxisFunc      {DbgMsgW2 (L"%%LeftFunc:  AxisFunc DydOp RightFunc");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 3, TRUE);        // Dyadic operator
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$3, 1, TRUE);        // Left operand
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$1.tkToken);
                                             YYERROR;
                                         } // End IF
                                         YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$1, 1, TRUE);        // Right operand
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>'     Strand       DydOp LeftFunc '('
                                        {DbgMsgW2 (L"%%LeftFunc:  (LeftFunc DydOp Strand)");
                                         lpYYStr = MakeVarStrand_EM_YY (&$2);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);
                                             YYERROR;
                                         } // End IF

                                         lpYYRes =
                                         PushFcnStrand_YY (&$3, 3, TRUE);        // Dyadic operator
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$4, 1, TRUE);        // Left operand
                                         FreeResult (&$4.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYStr, 1, FALSE);   // Right operand
                                         FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>' '>' SimpExpr '(' DydOp LeftFunc '('
                                        {DbgMsgW2 (L"%%LeftFunc:  (LeftFunc DydOp (SimpExpr))");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$5, 3, TRUE);        // Dyadic operator
                                         FreeResult (&$5.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$6.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$6, 1, TRUE);        // Left operand
                                         FreeResult (&$6.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } // End IF

                                         YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$3, 1, FALSE);       // Right operand
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>' '>' SimpExpr '(' DydOp AxisFunc '('
                                        {DbgMsgW2 (L"%%LeftFunc:  (AxisFunc DydOp (SimpExpr))");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$5, 3, TRUE);        // Dyadic operator
                                         FreeResult (&$5.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$6.tkToken);
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$6, 1, TRUE);        // Left operand
                                         FreeResult (&$6.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$3.tkToken);
                                             YYERROR;
                                         } // End IF

                                         YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$3, 1, FALSE);       // Right operand
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>'     Strand       DydOp AxisFunc '('
                                        {DbgMsgW2 (L"%%LeftFunc:  (AxisFunc DydOp Strand)");
                                         lpYYStr = MakeVarStrand_EM_YY (&$2);
                                         FreeResult (&$2.tkToken);
                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&$4.tkToken);
                                             YYERROR;
                                         } // End IF

                                         lpYYRes =
                                         PushFcnStrand_YY (&$3, 3, TRUE);        // Dyadic operator
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$4.tkToken);
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$4, 1, TRUE);        // Left operand
                                         FreeResult (&$4.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYStr, 1, FALSE);   // Right operand
                                         FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                            }
    | '>' RightFunc DydOp Strand   '('  {DbgMsgW2 (L"%%LeftFunc:  (Strand DydOp RightFunc)");
                                         lpYYStr = MakeVarStrand_EM_YY (&$4);
                                         FreeResult (&$4.tkToken);
                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                         {
                                            FreeResult (&$2.tkToken);
                                            FreeResult (&$3.tkToken);
                                            YYERROR;
                                         } // End IF

                                         lpYYRes =
                                         PushFcnStrand_YY (&$3, 3, TRUE);        // Dyadic operator
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYStr, 1, FALSE);   // Left operand
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Right operand
                                         FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>' '>' SimpExpr '(' DydOp Strand '('
                                        {DbgMsgW2 (L"%%LeftFunc:  (Strand DydOp (SimpExpr))");
                                         lpYYStr = MakeVarStrand_EM_YY (&$6);
                                         FreeResult (&$6.tkToken);
                                         if (!lpYYStr)          // If not defined, free args and YYERROR
                                         {
                                            FreeResult (&$3.tkToken);
                                            FreeResult (&$5.tkToken);
                                            YYERROR;
                                         } // End IF

                                         lpYYRes =
                                         PushFcnStrand_YY (&$5, 3, TRUE);        // Dyadic operator
                                         FreeResult (&$5.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$3.tkToken);
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYStr, 1, FALSE);   // Left operand
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;
                                             YYERROR;
                                         } // End IF

                                         YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (&$3, 1, FALSE);       // Right operand
                                         FreeResult (&lpYYStr->tkToken); YYFree (lpYYStr); lpYYStr = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>'     Strand       DydOp Strand '('
                                        {DbgMsgW2 (L"%%LeftFunc:  (Strand DydOp Strand)");
                                         lpYYStrL = MakeVarStrand_EM_YY (&$4);
                                         FreeResult (&$4.tkToken);

                                         lpYYStrR = MakeVarStrand_EM_YY (&$2);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYStrL || !lpYYStrR)    // If not defined, free args and YYERROR
                                         {
                                         FreeResult (&$3.tkToken);

                                            if (lpYYStrL)       // If defined, free it
                                            {
                                                FreeResult (&lpYYStrL->tkToken); YYFree (lpYYStrL); lpYYStrL = NULL;
                                            } // End IF

                                            if (lpYYStrR)       // If defined, free it
                                            {
                                                FreeResult (&lpYYStrR->tkToken); YYFree (lpYYStrR); lpYYStrR = NULL;
                                            } // End IF

                                            YYERROR;
                                         } // End IF

                                         lpYYRes =
                                         PushFcnStrand_YY (&$3, 3, TRUE);        // Dyadic operator
                                         FreeResult (&$3.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&lpYYStrL->tkToken); YYFree (lpYYStrL); lpYYStrL = NULL;
                                             FreeResult (&lpYYStrR->tkToken); YYFree (lpYYStrR); lpYYStrR = NULL;
                                             YYERROR;
                                         } // End IF

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYStrL, 1, FALSE);  // Left operand
                                         FreeResult (&lpYYStrL->tkToken); YYFree (lpYYStrL); lpYYStrL = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&lpYYStrR->tkToken); YYFree (lpYYStrR); lpYYStrR = NULL;
                                             YYERROR;
                                         } // End IF

                                         YYFree (lpYYRes); lpYYRes = NULL;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYStrR, 1, FALSE);  // Right operand
                                         FreeResult (&lpYYStrR->tkToken); YYFree (lpYYStrR); lpYYStrR = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    ;

/* Simple function expression */
SimpFunc:
      '>' LeftFunc '('                  {DbgMsgW2 (L"%%SimpFunc:  (LeftFunc)");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Function
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>' AxisFunc '('                  {DbgMsgW2 (L"%%SimpFunc:  (AxisFunc)");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Function
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>' FcnSpec '('                   {DbgMsgW2 (L"%%SimpFunc:  (FcnSpec)");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Function
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    ;

/* Right argument to dyadic operator */
RightFunc:
          PRIMFCN                       {DbgMsgW2 (L"%%RightFunc:  PRIMFCN");
                                         lpYYFcn = MakePrimFcn_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 1, FALSE);   // Function
                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |     NAMEFCN                       {DbgMsgW2 (L"%%RightFunc:  NAMEFCN");
                                         lpYYFcn = MakeNameFcn_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 1, FALSE);   // Function
                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |     SYSFN12                       {DbgMsgW2 (L"%%RightFunc:  SYSFN12");
                                         lpYYFcn = MakeNameFcn_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 1, FALSE);   // Function
                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>' LeftFunc  '('                 {DbgMsgW2 (L"%%RightFunc:  (LeftFunc)");
                                         lpYYFcn = MakePrimFcn_YY (&$2);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 1, TRUE);    // Function
                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>' FcnSpec  '('                  {DbgMsgW2 (L"%%RightFunc:  (FcnSpec)");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Function
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '>' AxisFunc  '('                 {DbgMsgW2 (L"%%RightFunc:  (AxisFunc)");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Function
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    ;

/* Axis function expression */
AxisFunc:
      '}' ArrExpr '['  PRIMFCN          {DbgMsgW2 (L"%%AxisFunc:  PRIMFCN[ArrExpr]");
                                         lpYYFcn = MakePrimFcn_YY (&$4);
                                         FreeResult (&$4.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 2, FALSE);   // Function
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
                                         PushFcnStrand_YY (lpYYAxis, 1, FALSE);   // Axis
                                         FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '}' ArrExpr '['  NAMEFCN          {DbgMsgW2 (L"%%AxisFunc:  NAMEFCN[ArrExpr]");
                                         lpYYFcn = MakeNameFcn_YY (&$4);
                                         FreeResult (&$4.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 2, FALSE);   // Function
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
                                         PushFcnStrand_YY (lpYYAxis, 1, FALSE);   // Axis
                                         FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | '}' ArrExpr '['  SYSFN12          {DbgMsgW2 (L"%%AxisFunc:  SYSFN12[ArrExpr]");
                                         lpYYFcn = MakeNameFcn_YY (&$4);
                                         FreeResult (&$4.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                         {
                                             FreeResult (&$2.tkToken);
                                             YYERROR;
                                         } // End IF

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 2, FALSE);
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
                                         PushFcnStrand_YY (lpYYAxis, 1, FALSE);   // Axis
                                         FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    // ***FIXME*** -- what does this double index mean?
////| '}' ArrExpr '['  AxisFunc         {DbgMsgW2 (L"%%AxisFunc:  AxisFunc[ArrExpr]");
////                                     lpYYAxis = MakeAxis_YY (&$2);
////                                     FreeResult (&$2.tkToken);
////
////                                     if (!lpYYAxis)         // If not defined, free args and YYERROR
////                                     {
////                                     FreeResult (&$4.tkToken);
////                                        YYERROR;
////                                     } // End IF
////
////                                     lpYYFcn = MakeArrayFn2_EM (&$4, lpYYAxis);
////                                     FreeResult (&$4.tkToken);
////                                     FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;
////
////                                     if (!lpYYFcn)          // If not defined, free args and YYERROR
////                                        YYERROR;
////
////                                     $$ = *lpYYFcn; YYFree (lpYYFcn); lpYYFcn = NULL;
////                                    }
    | '}' ArrExpr '[' '>' FcnSpec '('   {DbgMsgW2 (L"%%AxisFunc:  (FcnSpec)[ArrExpr]");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$5, 2, TRUE);         // Function
                                         FreeResult (&$5.tkToken);

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
                                         PushFcnStrand_YY (lpYYAxis, 1, FALSE);   // Axis
                                         FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    ;

// Monadic operator
MonOp:
                       OP1              {DbgMsgW2 (L"%%MonOp:  OP1");
                                         lpYYOp1 = MakeOp1_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYOp1)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYOp1, 1, FALSE);   // Monadic operator
                                         FreeResult (&lpYYOp1->tkToken); YYFree (lpYYOp1); lpYYOp1 = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |                  NAMEOP1          {DbgMsgW2 (L"%%MonOp:  NAMEOP1");
                                         lpYYFcn = MakeNameOp1_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYFcn)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYFcn, 1, FALSE);   // Monadic operator
                                         FreeYYFcn1 (lpYYFcn); lpYYFcn = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |              '>' MonOp '('        {DbgMsgW2 (L"%%MonOp:  (MonOp)");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);        // Monadic operator
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |              '>' Op1Spec '('     {DbgMsgW2 (L"%%MonOp:  (Op1Spec)");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, TRUE);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    ;

// Monadic operator with axis
MonOpAxis:
      '}' ArrExpr '['  MonOp            {DbgMsgW2 (L"%%MonOpAxis:  MonOp[ArrExpr]");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$4, 2, TRUE);        // Monadic operator
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
                                         PushFcnStrand_YY (lpYYAxis, 1, FALSE);   // Axis
                                         FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         YYFree (lpYYRes); lpYYRes = NULL;
                                        }
////| '}' ArrExpr '['  OP1              {DbgMsgW2 (L"%%MonOpAxis:  OP1[ArrExpr]");
////                                     lpYYOp1 = MakeOp1_YY (&$4);
////                                     FreeResult (&$4.tkToken);
////
////                                     if (!lpYYOp1)          // If not defined, free args and YYERROR
////                                     {
////                                         FreeResult (&$2.tkToken);
////                                         YYERROR;
////                                     } // End IF
////
////                                     lpYYRes =
////                                     PushFcnStrand_YY (lpYYOp1, 2, FALSE);   // Monadic operator
////                                     FreeResult (&lpYYOp1->tkToken); YYFree (lpYYOp1); lpYYOp1 = NULL;
////
////                                     if (!lpYYRes)          // If not defined, free args and YYERROR
////                                     {
////                                         FreeResult (&$2.tkToken);
////                                         YYERROR;
////                                     } // End IF
////
////                                     $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////
////                                     lpYYAxis = MakeAxis_YY (&$2);
////                                     FreeResult (&$2.tkToken);
////
////                                     if (!lpYYAxis)         // If not defined, free args and YYERROR
////                                         YYERROR;
////
////                                     lpYYRes =
////                                     PushFcnStrand_YY (lpYYAxis, 1, FALSE);  // Axis
////                                     FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;
////
////                                     if (!lpYYRes)          // If not defined, free args and YYERROR
////                                         YYERROR;
////
////                                     YYFree (lpYYRes); lpYYRes = NULL;
////                                    }
////| '}' ArrExpr '['  NAMEOP1          {DbgMsgW2 (L"%%MonOpAxis:  NAMEOP1[ArrExpr]");
////                                     lpYYOp1 = MakeNameOp1_YY (&$4);
////                                     FreeResult (&$4.tkToken);
////
////                                     if (!lpYYOp1)          // If not defined, free args and YYERROR
////                                     {
////                                         FreeResult (&$2.tkToken);
////                                         YYERROR;
////                                     } // End IF
////
////                                     lpYYRes =
////                                     PushFcnStrand_YY (lpYYOp1, 2, FALSE);   // Monadic operator
////                                     FreeResult (&lpYYOp1->tkToken); YYFree (lpYYOp1); lpYYOp1 = NULL;
////
////                                     if (!lpYYRes)          // If not defined, free args and YYERROR
////                                     {
////                                         FreeResult (&$2.tkToken);
////                                         YYERROR;
////                                     } // End IF
////
////                                     $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////
////                                     lpYYAxis = MakeAxis_YY (&$2);
////                                     FreeResult (&$2.tkToken);
////
////                                     if (!lpYYAxis)         // If not defined, free args and YYERROR
////                                         YYERROR;
////
////                                     lpYYRes =
////                                     PushFcnStrand_YY (lpYYAxis, 1, FALSE;
////                                     FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;
////
////                                     if (!lpYYRes)          // If not defined, free args and YYERROR
////                                         YYERROR;
////
////                                     YYFree (lpYYRes); lpYYRes = NULL;
////                                    }
////| '}' ArrExpr '[' '>' MonOp '('     {DbgMsgW2 (L"%%MonOpAxis:  (MonOp)[ArrExpr]");
////                                     lpYYRes =
////                                     PushFcnStrand_YY (&$5, 2, TRUE);        // Monadic operator
////                                     FreeResult (&$5.tkToken);
////
////                                     if (!lpYYRes)          // If not defined, free args and YYERROR
////                                     {
////                                         FreeResult (&$2.tkToken);
////                                         YYERROR;
////                                     } // End IF
////
////                                     $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
////
////                                     lpYYAxis = MakeAxis_YY (&$2);
////                                     FreeResult (&$2.tkToken);
////
////                                     if (!lpYYAxis)         // If not defined, free args and YYERROR
////                                         YYERROR;
////
////                                     lpYYRes =
////                                     PushFcnStrand_YY (lpYYAxis, 1, FALSE);
////                                     FreeResult (&lpYYAxis->tkToken); YYFree (lpYYAxis); lpYYAxis = NULL;
////
////                                     if (!lpYYRes)          // If not defined, free args and YYERROR
////                                         YYERROR;
////
////                                     YYFree (lpYYRes); lpYYRes = NULL;
////                                    }
    ;

/* Dyadic operator */
DydOp:
                       OP2              {DbgMsgW2 (L"%%DydOp:  OP2");
                                         lpYYOp2 = MakeOp2_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYOp2)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYOp2, 1, FALSE);       // Dyadic operator
                                         FreeResult (&lpYYOp2->tkToken); YYFree (lpYYOp2); lpYYOp2 = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |                  NAMEOP2          {DbgMsgW2 (L"%%DydOp:  NAMEOP2");
                                         lpYYOp2 = MakeNameOp2_YY (&$1);
                                         FreeResult (&$1.tkToken);

                                         if (!lpYYOp2)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         lpYYRes =
                                         PushFcnStrand_YY (lpYYOp2, 1, FALSE);
                                         FreeResult (&lpYYOp2->tkToken); YYFree (lpYYOp2); lpYYOp2 = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |              '#' DydOp '('        {DbgMsgW2 (L"%%DydOp:  (DydOp)");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, FALSE);       // Dyadic operator
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |              '#' Op2Spec '('      {DbgMsgW2 (L"%%DydOp:  (Op2Spec)");
                                         lpYYRes =
                                         PushFcnStrand_YY (&$2, 1, FALSE);       // Dyadic operator
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                             YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
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
                                         lpYYLst = MakeList_EM_YY (&$2, TRUE);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYLst)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         $$ = *lpYYLst; YYFree (lpYYLst); lpYYLst = NULL;
                                        }
    | ILBR ']' ILWE '['                 {DbgMsgW2 (L"%%ILBR:  ILBR [ILWE]");




                                         PrimFnNonceError_EM (&$3.tkToken);
                                         FreeResult (&$1.tkToken);
                                         FreeResult (&$3.tkToken);
                                         YYERROR;
                                        }
    ;

/* Index list w/parens */
ILPAR:
      ')' ILNE '('                      {DbgMsgW2 (L"%%ILPAR:  (ILNE)");
                                         lpYYLst = MakeList_EM_YY (&$2, FALSE);
                                         FreeResult (&$2.tkToken);

                                         if (!lpYYLst)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         $$ = *lpYYLst; YYFree (lpYYLst); lpYYLst = NULL;
                                        }
    ;

/* Index list, no empties */
ILNE:
      ArrExpr ';' ArrExpr               {DbgMsgW2 (L"%%ILNE:  ArrExpr;ArrExpr");
                                         lpYYLst =
                                         InitList1_YY (&$1);

                                         if (!lpYYLst)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         lpYYRes =
                                         PushList_YY (lpYYLst, &$3);
                                         FreeResult (&lpYYLst->tkToken); YYFree (lpYYLst); lpYYLst = NULL;

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | ILNE    ';' ArrExpr               {DbgMsgW2 (L"%%ILNE:  ILNE;ArrExpr");
                                         lpYYRes =
                                         PushList_YY (&$1, &$3);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    ;

/* Index list, with empties */
ILWE:
      /* Empty */                       {DbgMsgW2 (L"%%ILWE:  <empty>");
                                         lpYYRes =
                                         InitList0_YY ();

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    |          ArrExpr                  {DbgMsgW2 (L"%%ILWE:  ArrExpr");
                                         lpYYRes =
                                         InitList1_YY (&$1);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | ILWE ';' ArrExpr                  {DbgMsgW2 (L"%%ILWE:  ILWE;ArrExpr");
                                         lpYYRes =
                                         PushList_YY (&$1, &$3);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | ILWE ';'                          {DbgMsgW2 (L"%%ILWE:  ILWE;");
                                         lpYYRes =
                                         PushList_YY (&$1, NULL);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    ;

/* A data expression which has a single token */
SingTokn:
      CONSTANT                          {DbgMsgW2 (L"%%SingTokn:  CONSTANT");
                                         $$ = $1;
                                        }
    | NAMEVAR                           {DbgMsgW2 (L"%%SingTokn:  NAMEVAR");
                                         $1.tkToken.tkFlags.TknType = TKT_VARNAMED;
                                         $$ = $1;
                                        }
    | USRFN0                            {DbgMsgW2 (L"%%SingTokn:  USRFN0");
                                         $1.tkToken.tkFlags.TknType = TKT_FCNNAMED;
                                         $1.tkToken.tkFlags.FcnDir  = 0;

                                         lpYYRes =
                                         ExecuteFn0 (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | SYSFN0                            {DbgMsgW2 (L"%%SingTokn:  SYSFN0");
                                         $1.tkToken.tkFlags.TknType = TKT_FCNNAMED;
                                         $1.tkToken.tkFlags.FcnDir  = 1;

                                         lpYYRes =
                                         ExecuteFn0 (&$1.tkToken);

                                         if (!lpYYRes)          // If not defined, free args and YYERROR
                                            YYERROR;

                                         $$ = *lpYYRes; YYFree (lpYYRes); lpYYRes = NULL;
                                        }
    | STRING                            {DbgMsgW2 (L"%%SingTokn:  STRING");
                                         lpYYStr = CopyString_EM_YY (&$1);

                                         if (!lpYYStr)
                                            YYERROR;

                                         $$ = *lpYYStr; YYFree (lpYYStr); lpYYStr = NULL;
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
} PL_THREAD, *LPPL_THREAD;

PL_THREAD plThread;             // Temporary global


//***************************************************************************
//  $ParseLine
//
//  Parse a line
//***************************************************************************

#ifdef DEBUG
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
    HANDLE      hSemaphore;     // Handle to the ParseLine semaphore
    HGLOBAL     hGlbToken;      // Handle to sequence of tokens to parse
    LPWCHAR     lpwszLine;      // Ptr to WCHARs in the line to parse
    HGLOBAL     hGlbPTD;        // Handle to PerTabData
    BOOL        bFreeToken;     // TRUE iff we should free hGlbToken on exit
    PLLOCALVARS plLocalVars;    // Re-entrant vars

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

    // If we don't have a valid handle, ...
    if (!hGlbToken)
        return;
#ifdef DEBUG
    { // ***DEBUG***
        // Display the tokens so far
        DisplayTokens (hGlbToken);
    } // ***DEBUG*** END
#endif

    // Save the PTD handle
    plLocalVars.hGlbPTD = hGlbPTD;

    // Lock the handle and set variables
    plLocalVars.hGlbToken = hGlbToken;
    UTLockAndSet (plLocalVars.hGlbToken, &plLocalVars.t2);

    // Skip over TOKEN_HEADER
    plLocalVars.lpStart = TokenBaseToStart (plLocalVars.t2.lpBase);

    // Skip to end of 1st stmt
    plLocalVars.lpNext  = &plLocalVars.lpStart[plLocalVars.lpStart->tkData.tkIndex];

    // Start off with no error
    plLocalVars.tkErrorCharIndex = NEG1U;

    // Allocate virtual memory for the Variable Strand accumulator
    plLocalVars.lpYYStrandStart[VARSTRAND] =
        VirtualAlloc (NULL,      // Any address
                      DEF_STRAND_MAXSIZE * sizeof (YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!plLocalVars.lpYYStrandStart[VARSTRAND])
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ParseLine:  VirtualAlloc for <pLocalVars.lpYYStrandStart[VARSTRAND]> failed");

        return;             // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (plLocalVars.lpYYStrandStart[VARSTRAND],
                  DEF_STRAND_INITSIZE * sizeof (YYSTYPE),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Allocate virtual memory for the Function Strand accumulator
    plLocalVars.lpYYStrandStart[FCNSTRAND] =
        VirtualAlloc (NULL,      // Any address
                      DEF_STRAND_MAXSIZE * sizeof (YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!plLocalVars.lpYYStrandStart[FCNSTRAND])
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ParseLine:  VirtualAlloc for <pLocalVars.lpYYStrandStart[FCNSTRAND]> failed");

        return;             // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (plLocalVars.lpYYStrandStart[FCNSTRAND],
                  DEF_STRAND_INITSIZE * sizeof (YYSTYPE),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Initialize the base & next strand ptrs
    plLocalVars.lpYYStrandBase[VARSTRAND] =
    plLocalVars.lpYYStrandNext[VARSTRAND] = plLocalVars.lpYYStrandStart[VARSTRAND];
    plLocalVars.lpYYStrandBase[FCNSTRAND] =
    plLocalVars.lpYYStrandNext[FCNSTRAND] = plLocalVars.lpYYStrandStart[FCNSTRAND];

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
    // Signal an error
    ErrorMessage (lpwszErrorMessage, lpwszLine, plLocalVars.tkErrorCharIndex);
NORMAL_EXIT:
    if (plLocalVars.lpYYStrandStart[FCNSTRAND])
    {
        VirtualFree (plLocalVars.lpYYStrandStart[FCNSTRAND], 0, MEM_RELEASE); plLocalVars.lpYYStrandStart[FCNSTRAND] = NULL;
    } // End IF

    if (plLocalVars.lpYYStrandStart[VARSTRAND])
    {
        VirtualFree (plLocalVars.lpYYStrandStart[VARSTRAND], 0, MEM_RELEASE); plLocalVars.lpYYStrandStart[VARSTRAND] = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (plLocalVars.hGlbToken); plLocalVars.t2.lpBase   = NULL;

    // Untokenize and free hGlbToken if requested
    if (bFreeToken)
    {
        Untokenize (hGlbToken);
        DbgGlobalFree (hGlbToken); hGlbToken = NULL;
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
        case TKT_INPOUT:
            return FALSE;

        case TKT_OP2IMMED:
        case TKT_OPJOTDOT:
            return TRUE;

        case TKT_VARNAMED:
            // Look inside the symbol table entry
            return lpNext->tkData.tkSym->stFlags.UsrOp2;

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
//  $LookaheadSurround
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
        case TKT_INPOUT:
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
        case TKT_OPJOTDOT:
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
        case TKT_LISTPAR:
        case TKT_LISTBR:
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
            return SymbTypeFV (lpNext);

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
//  $LookaheadAdjacent
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
        case TKT_INPOUT:
            return 'V';             // Variable

        case TKT_OP1IMMED:
        case TKT_OP1NAMED:
            return '1';

        case TKT_OP2IMMED:
        case TKT_OP2NAMED:
            return '2';

        case TKT_FCNIMMED:
        case TKT_OPJOTDOT:
            return 'F';             // Function

        case TKT_VARNAMED:
            return SymbTypeFV (lpNext);

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
            // We need a bit a lookahead here so handle the FCN vs. OP1 vs. OP2
            //   cases when the name is being assigned to.  In this case,
            //   because the current meaning of the name is irrelevant
            //   (it's about to be reassigned), we can accept any kind of name.
            //   LALR grammars have a hard time dealing with this, so we
            //   lookahead one token to see if it is ASSIGN.  If so, we return
            //   NAMEUNK, a universal named token.

            // If this is a UsrVar and either it has no value
            //   or the next token is ASSIGN, call it NAMEUNK.
            if (lpplLocalVars->lpNext->tkData.tkSym->stFlags.UsrVar
             && (!lpplLocalVars->lpNext->tkData.tkSym->stFlags.Value
              || lpplLocalVars->lpNext[1].tkFlags.TknType EQ TKT_ASSIGN))
                return NAMEUNK;
            else
            if (lpplLocalVars->lpNext->tkData.tkSym->stFlags.UsrVar
             || lpplLocalVars->lpNext->tkData.tkSym->stFlags.SysVar)
            {
////////////////lpplLocalVars->lpNext->tkFlags.TknType = TKT_VARNAMED;    // Already set

                return NAMEVAR;
            } else
            if (lpplLocalVars->lpNext->tkData.tkSym->stFlags.UsrFn0)
                return USRFN0;
            else
            if (lpplLocalVars->lpNext->tkData.tkSym->stFlags.SysFn0)
                return SYSFN0;
            else
            if (lpplLocalVars->lpNext->tkData.tkSym->stFlags.UsrFn12)
            {
                lpplLocalVars->lpNext->tkFlags.TknType = TKT_FCNNAMED;

                return NAMEFCN;
            } else
            if (lpplLocalVars->lpNext->tkData.tkSym->stFlags.SysFn12)
            {
                lpplLocalVars->lpNext->tkFlags.TknType = TKT_FCNNAMED;
                lpplLocalVars->lpNext->tkFlags.FcnDir  = 1;

                return SYSFN12;
            } else
            if (lpplLocalVars->lpNext->tkData.tkSym->stFlags.UsrOp1)
            {
                lpplLocalVars->lpNext->tkFlags.TknType = TKT_OP1NAMED;

                return NAMEOP1;
            } else
            if (lpplLocalVars->lpNext->tkData.tkSym->stFlags.UsrOp2)
            {
                lpplLocalVars->lpNext->tkFlags.TknType = TKT_OP2NAMED;

                return NAMEOP2;
            } else
            if (lpplLocalVars->lpNext->tkData.tkSym->stFlags.UsrName)
                return NAMEUNK;

            DbgStop ();         // We should never get here

            return UNK;

        case TKT_ASSIGN:
            return ASSIGN;

        case TKT_LISTSEP:
            return ';';

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
                // We really need an FSA for this
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
                } // End SWITCH
            } // End IF

            return OP1;

        case TKT_OP2IMMED:
            return OP2;

        case TKT_OPJOTDOT:
            return JOTDOT;

        case TKT_LPAREN:
            return '(';

        case TKT_RPAREN:
            // Lookahead to see if this right grouping symbol surrounds
            //   a function (return '>'), a variable (return ')'), or
            //   a dyadic operator (return '#')

            // We really need an FSA for this
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
            } // End SWITCH

            DbgStop ();                 // We should never get here

            return UNK;

        case TKT_LBRACKET:
            return '[';

        case TKT_RBRACKET:
            // Lookahead to see if this right grouping symbol is adjacent to
            //   a function (return '}') or a variable (return ']')

            // We really need an FSA for this
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
            } // End SWITCH

            DbgStop ();                 // We should never get here

            return UNK;

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
//  Error callback from YACC
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- pl_yyerror"
#else
#define APPEND_NAME
#endif

void pl_yyerror                     // Called for yacc syntax error
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


//***************************************************************************
//  End of File: parse.y
//***************************************************************************
// WE ***MUST*** KEEP A BLANK LINE AS THE LAST LINE IN THIS FILE
