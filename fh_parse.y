//***************************************************************************
//  NARS2000 -- Parser Grammar for function headers
//***************************************************************************

//***************************************************************************
//  Parse a line of pre-tokenized tokens.
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
#include <string.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "fh_parse.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

////#define YYLEX_DEBUG
////#define YYFPRINTF_DEBUG

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               fh_yyfprintf
#endif

#define    YYSTYPE     FH_YYSTYPE
#define  LPYYSTYPE   LPFH_YYSTYPE
#define tagYYSTYPE  tagFH_YYSTYPE

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         fh_yy_symbol_print
#define yy_symbol_value_print   fh_yy_symbol_value_print
#define yy_reduce_print         fh_yy_reduce_print
#define yydestruct              fh_yydestruct

////#define DbgMsgWP(a)         DbgMsgW(a)
////#define DbgMsgWP(a)         DbgMsgW(a); DbgBrk ()
////#define DbgMsgWP(a)         DbgMsgW(a)
    #define DbgMsgWP(a)

%}

%pure-parser
%name-prefix="fh_yy"
%parse-param {LPFHLOCALVARS lpfhLocalVars}
%lex-param   {LPFHLOCALVARS lpfhLocalVars}
%token NAMEUNK NAMEOPR NAMESYS ASSIGN LINECONT UNK COMMENT SOS

%start HeaderComm

%%

/*

The set of cases we must handle is as follows:

Niladic functions
-----------------

Result
------
    1.  **Empty**
    2.  Z{is}
    3.  (Z1 Z2 ...){is}
    4.  {Z}{is}
    5.  {(Z1 Z2 ...)}{is}
    6.  ({Z1 Z2 ...}){is}

Fcn
---
    1.  FOO


Monadic/Dyadic Functions/Operators
----------------------------------

Result
------
    1.  **Empty**
    2.  Z{is}
    3.  (Z1 Z2 ...){is}
    4.  {Z}{is}
    5.  {(Z1 Z2 ...)}{is}
    6.  ({Z1 Z2 ...}){is}


Left Arg
--------
    1.  **Empty**
    2.  L
    3.  (L)
    4.  (L1 L2 ...)
    5.  {L}
    6.  {L1 L2 ...}
    7.  {(L)}
    8.  {(L1 L2 ...)}
    9.  ({L})
   10.  ({L1 L2 ...})

Fcn/Opr
-------
    1.  FOO
    2.  (FOO)
    3.  (LO FOO)
    4.  (LO FOO RO)
    5.  FOO[X]
    6.  (FOO[X])
    7.  (LO FOO[X])
    8.  (LO FOO[X] RO)

Right Arg
---------
    1.  R
    2.  (R)
    3.  (R1 R2 ...)

This yields 1440 (=6 x 10 x 8 x 3) distinct Monadic/Dyadic Function/Operator headers
and            6 (=6      x 1    ) distinct Niladic Function headers
for a total of 1446 user-defined function/operator headers,
not counting the presence/absence of locals and presence/absence of a comment.

 */


AxisOpr:
      NAMEOPR '[' NAMEUNK ']'   {DbgMsgWP (L"%%AxisOpr:  NAMEUNK[NAMEUNK]");
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 PushHdrStrand_YY (&$3);
                                 MakeHdrStrand_YY (&$1);
                                 lpfhLocalVars->DfnAxis  = TRUE;
                                 lpfhLocalVars->lpYYAxisOpr = $3.lpYYStrandIndirect;
                                 $$ = $1;
                                }
    ;

OpenList:
               NAMEUNK          {DbgMsgWP (L"%%OpenList:  NAMEUNK");
                                 InitHdrStrand (&$1);
                                 $$ = *PushHdrStrand_YY (&$1);
                                }
    | OpenList NAMEUNK          {DbgMsgWP (L"%%OpenList:  OpenList NAMEUNK");
                                 $$ = *PushHdrStrand_YY (&$2);
                                }
    ;

AxisList:
      '('          AxisOpr          ')'
                                {DbgMsgWP (L"%%AxisList:  (AxisOpr)");
                                 InitHdrStrand (&$2);
                                 $2.Indirect = TRUE;
                                 PushHdrStrand_YY (&$2);
                                 $$ = *MakeHdrStrand_YY (&$2);
                                }
    | '(' NAMEUNK  AxisOpr          ')'
                                {DbgMsgWP (L"%%AxisList:  (NAMEUNK AxisOpr)");
                                 PushHdrStrand_YY (&$2);
                                 $3.Indirect = TRUE;
                                 PushHdrStrand_YY (&$3);
                                 $$ = *MakeHdrStrand_YY (&$2);
                                }
    | '(' NAMEUNK  AxisOpr NAMEUNK ')'
                                {DbgMsgWP (L"%%AxisList:  (NAMEUNK AxisOpr NAMEUNK)");
                                 PushHdrStrand_YY (&$2);
                                 $3.Indirect = TRUE;
                                 PushHdrStrand_YY (&$3);
                                 PushHdrStrand_YY (&$4);
                                 $$ = *MakeHdrStrand_YY (&$2);
                                }
    ;

List:
      '(' OpenList ')'          {DbgMsgWP (L"%%List:  '(' OpenList ')'");
                                 $$ = *MakeHdrStrand_YY (&$2);
                                 $$.List = TRUE;                                    // Set the List bit
                                }
    ;

Result:
      List    ASSIGN            {DbgMsgWP (L"%%Result:  List" WS_UTF16_LEFTARROW);
                                 if ($1.lpYYStrandBase->uStrandLen EQ 1 && $1.List)
                                 {
                                     fh_yyerror (lpfhLocalVars, "length error");
                                     YYERROR;
                                 } // End IF

                                 lpfhLocalVars->lpYYResult = $1.lpYYStrandBase;
                                 lpfhLocalVars->ListRes    = $1.List;               // Copy the List bit
                                 $$ = $1;
                                }
    | OptArg  ASSIGN            {DbgMsgWP (L"%%Result:  OptArg" WS_UTF16_LEFTARROW);
                                 if ($1.lpYYStrandBase->uStrandLen EQ 1 && $1.List)
                                 {
                                     fh_yyerror (lpfhLocalVars, "length error");
                                     YYERROR;
                                 } // End IF

                                 lpfhLocalVars->lpYYResult = $1.lpYYStrandBase;
                                 lpfhLocalVars->ListRes    = $1.List;               // Copy the List bit
                                 lpfhLocalVars->NoDispRes  = TRUE;
                                 $$ = $1;
                                }
    | NAMEUNK ASSIGN            {DbgMsgWP (L"%%Result:  NAMEUNK" WS_UTF16_LEFTARROW);
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 $$ = *MakeHdrStrand_YY (&$1);
                                 lpfhLocalVars->lpYYResult = $1.lpYYStrandBase;
                                }
    ;

OptArg:
      '{' List     '}'          {DbgMsgWP (L"%%OptArg:  {List}");
                                 $$ = $2;
                                }
    | '{' OpenList '}'          {DbgMsgWP (L"%%OptArg:  {OpenList}");
                                 $$ = *MakeHdrStrand_YY (&$2);
                                 // This is the only odd case where we can't use the List bit
                                 //   because there are no parens, so we use the strand length
                                 $$.List = $$.lpYYStrandBase->uStrandLen > 1;       // Set the List bit
                                }
    | '(' '{' OpenList '}' ')'  {DbgMsgWP (L"%%OptArg:  ({OpenList})");
                                 $$ = *MakeHdrStrand_YY (&$3);
                                 $$.List = TRUE;                                    // Set the List bit
                                }
    ;

RhtArg:
      List                      {DbgMsgWP (L"%%RhtArg:  List");
                                 lpfhLocalVars->ListRht = $1.List;                  // Copy the List bit
                                 $$ = $1;
                                }
    | NAMEUNK                   {DbgMsgWP (L"%%RhtArg:  NAMEUNK");
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 $$ = *MakeHdrStrand_YY (&$1);
                                }
    ;

NoResHdr:                       // N.B. that this production does not need to return a result (in $$)
                                //   because it calculates all the information we need into
                                //   lpfhLocalVars->lpYY... and lpfhLocalVars->u???Valence
              NAMEUNK           {DbgMsgWP (L"%%NoResHdr:  NAMEUNK");                // Niladic function
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_NIL;       // Mark as niladic
                                }

    |         AxisOpr  RhtArg   {DbgMsgWP (L"%%NoResHdr:  AxisOpr RhtArg");         // Monadic function w/axis operator
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
                                }

    |         NAMEUNK  RhtArg   {DbgMsgWP (L"%%NoResHdr:  NAMEUNK RhtArg");         // Monadic function
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYFcnName = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
                                }

    | NAMEUNK AxisOpr  RhtArg   {DbgMsgWP (L"%%NoResHdr:  NAMEUNK AxisOpr RhtArg"); // Dyadic function w/axis operator
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | NAMEUNK NAMEUNK  RhtArg   {DbgMsgWP (L"%%NoResHdr:  NAMEUNK NAMEUNK RhtArg"); // Dyadic function
                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | List    AxisOpr  RhtArg   {DbgMsgWP (L"%%NoResHdr:  List AxisOpr RhtArg");    // Dyadic function w/axis operator
                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | List    NAMEUNK  RhtArg   {DbgMsgWP (L"%%NoResHdr:  List NAMEUNK RhtArg");    // Dyadic function
                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | OptArg  AxisOpr  RhtArg   {DbgMsgWP (L"%%NoResHdr:  OptArg AxisOpr RhtArg");  // Bivalent function w/axis operator
                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_AMB;       // Mark as ambivalent
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | OptArg  NAMEUNK  RhtArg   {DbgMsgWP (L"%%NoResHdr:  OptArg NAMEUNK RhtArg");  // Bivalent function
                                 InitHdrStrand (&$2);
                                 PushHdrStrand_YY (&$2);
                                 MakeHdrStrand_YY (&$2);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYFcnName = $2.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->DfnType     = DFNTYPE_FCN;          // Mark as a function
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_AMB;       // Mark as ambivalent
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    |        AxisList  RhtArg   {DbgMsgWP (L"%%NoResHdr:  AxisList RhtArg");        // Mon/Dyd operator, monadic derived function w/axis operator
                                 if (!GetOprName_EM (&$1))
                                     YYERROR;

                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
                                }
    |         List     RhtArg   {DbgMsgWP (L"%%NoResHdr:  List RhtArg");            // Mon/Dyd operator, monadic derived function
                                 if (!GetOprName_EM (&$1))
                                     YYERROR;

                                 lpfhLocalVars->lpYYRhtArg  = $2.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_MON;       // Mark as monadic
                                }
    | NAMEUNK AxisList RhtArg   {DbgMsgWP (L"%%NoResHdr:  NAMEUNK AxisList RhtArg");// Mon/Dyd operator, dyadic derived function w/axis operator
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | NAMEUNK List     RhtArg   {DbgMsgWP (L"%%NoResHdr:  NAMEUNK List RhtArg");    // Mon/Dyd operator, dyadic derived function
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 InitHdrStrand (&$1);
                                 PushHdrStrand_YY (&$1);
                                 MakeHdrStrand_YY (&$1);

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                }
    | List    AxisList RhtArg   {DbgMsgWP (L"%%NoResHdr:  List AxisList RhtArg");   // Mon/Dyd operator, dyadic derived function w/axis operator
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | List    List     RhtArg   {DbgMsgWP (L"%%NoResHdr:  List List RhtArg");       // Mon/Dyd operator, dyadic derived function
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_DYD;       // Mark as dyadic
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | OptArg  AxisList RhtArg   {DbgMsgWP (L"%%NoResHdr:  OptArg AxisList RhtArg"); // Mon/Dyd operator, ambivalent derived function w/axis operator
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_AMB;       // Mark as ambivalent
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    | OptArg  List     RhtArg   {DbgMsgWP (L"%%NoResHdr:  OptArg List RhtArg");     // Mon/Dyd operator, ambivalent derived function
                                 if (!GetOprName_EM (&$2))
                                     YYERROR;

                                 lpfhLocalVars->lpYYLftArg  = $1.lpYYStrandBase;
                                 lpfhLocalVars->lpYYRhtArg  = $3.lpYYStrandBase;
                                 lpfhLocalVars->FcnValence  = FCNVALENCE_AMB;       // Mark as ambivalent
                                 lpfhLocalVars->ListLft     = $1.List;              // Copy the List bit
                                }
    ;

Locals:
              LINECONT          {DbgMsgWP (L"%%Locals:  LINECONT");
                                }
    |         ';'      NAMEUNK  {DbgMsgWP (L"%%Locals:  ';' NAMEUNK");
                                 InitHdrStrand (&$2);
                                 $$ = *PushHdrStrand_YY (&$2);
                                }
    |         ';'      NAMESYS  {DbgMsgWP (L"%%Locals:  ';' NAMESYS");
                                 if (!$2.tkToken.tkData.tkSym->stFlags.Value)
                                 {
                                     fh_yyerror (lpfhLocalVars, "value error");
                                     YYERROR;
                                 } // End IF

                                 InitHdrStrand (&$2);
                                 $$ = *PushHdrStrand_YY (&$2);
                                }
    | Locals  LINECONT          {DbgMsgWP (L"%%Locals:  Locals LINECONT");
                                }
    | Locals  ';'      NAMEUNK  {DbgMsgWP (L"%%Locals:  Locals ';' NAMEUNK");
                                 $$ = *PushHdrStrand_YY (&$3);
                                }
    | Locals  ';'      NAMESYS  {DbgMsgWP (L"%%Locals:  Locals ';' NAMESYS");
                                 if (!$3.tkToken.tkData.tkSym->stFlags.Value)
                                 {
                                     fh_yyerror (lpfhLocalVars, "value error");
                                     YYERROR;
                                 } // End IF

                                 $$ = *PushHdrStrand_YY (&$3);
                                }
    ;

Header:
        /* Empty */             {DbgMsgWP (L"%%Header:  <empty>");
                                }
    |         error             {DbgMsgWP (L"%%Header:  error");
                                 YYABORT;
                                }
    |         NoResHdr          {DbgMsgWP (L"%%Header:  NoResHdr");
                                }
    |         NoResHdr error    {DbgMsgWP (L"%%Header:  NoResHdr error");
                                 YYABORT;
                                }
    |         NoResHdr Locals   {DbgMsgWP (L"%%Header:  NoResHdr Locals");
                                 lpfhLocalVars->lpYYLocals = MakeHdrStrand_YY (&$2);
                                }
    | error   NoResHdr          {DbgMsgWP (L"%%Header:  error NoResHdr");
                                 YYABORT;
                                }
    | Result  NoResHdr          {DbgMsgWP (L"%%Header:  Result NoResHdr");
                                }
    | error   NoResHdr Locals   {DbgMsgWP (L"%%Header:  error  NoResHdr Locals");
                                 YYABORT;
                                }
    | Result  NoResHdr error    {DbgMsgWP (L"%%Header:  Result NoResHdr error");
                                 YYABORT;
                                }
    | Result  NoResHdr Locals   {DbgMsgWP (L"%%Header:  Result NoResHdr Locals");
                                 lpfhLocalVars->lpYYLocals = MakeHdrStrand_YY (&$3);
                                }
    ;

HeaderComm:
      Header SOS                {DbgMsgWP (L"%%HeaderComm:  Header SOS");
#ifdef DEBUG
                                 DisplayFnHdr (lpfhLocalVars);
#endif
                                }
    | Header COMMENT SOS        {DbgMsgWP (L"%%HeaderComm:  Header COMMENT SOS");
#ifdef DEBUG
                                 DisplayFnHdr (lpfhLocalVars);
#endif
                                }
    ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************

//***************************************************************************
//  $ParseHeader
//
//  Parse a function header
//***************************************************************************

BOOL ParseHeader
    (HWND          hWndEC,          // Window handle of Edit Ctrl
     HGLOBAL       hGlbTknHdr,      // Tokenized header global memory handle
     LPFHLOCALVARS lpfhLocalVars,   // Ptr to Local vars
     UBOOL         bDisplayErr)     // TRUE iff want error messages displayed

{
    UBOOL bRet = FALSE,             // TRUE iff result is valid
          OldDisplayErr;            // Save area for old DisplayErr

    // Save the window handle
    lpfhLocalVars->hWndEC = hWndEC;

    // Save the error display flag
    OldDisplayErr = lpfhLocalVars->DisplayErr;
    lpfhLocalVars->DisplayErr = bDisplayErr;

    // Save the thread's ptr to local vars
    TlsSetValue (dwTlsFhLocalVars, (LPVOID) lpfhLocalVars);

    // Save the token header global memory handle
    lpfhLocalVars->hGlbTknHdr = hGlbTknHdr;

    // Lock the handle to get a ptr to it, and set the variables
    UTLockAndSet (lpfhLocalVars->hGlbTknHdr, &lpfhLocalVars->t2);

    // Initialize the base & next strand ptrs
    lpfhLocalVars->lpYYStrandBase =
    lpfhLocalVars->lpYYStrandNext = lpfhLocalVars->lpYYStrandStart;

    // Skip over TOKEN_HEADER
    lpfhLocalVars->lptkStart = TokenBaseToStart (lpfhLocalVars->t2.lpBase);

    // Skip over the starting EOL
    lpfhLocalVars->lptkNext  = &lpfhLocalVars->lptkStart[1];

    // Mark the stopping point
    lpfhLocalVars->lptkStop  = &lpfhLocalVars->lptkStart[lpfhLocalVars->lptkStart->tkData.tkChar];

    // Start off with no error
    lpfhLocalVars->tkErrorCharIndex = NEG1U;

    // ***FIXME*** -- Use VirtualAlloc for the parser stack














#if YYDEBUG
    // Enable debugging
    yydebug = TRUE;
#endif

    // Parse the header, check for errors
    //   0 = success
    //   1 = YYABORT or APL error
    //   2 = memory exhausted
    bRet = fh_yyparse (lpfhLocalVars) EQ 0;

    // Restore the error display flag
    lpfhLocalVars->DisplayErr = OldDisplayErr;

    // We no longer need this ptr
    MyGlobalUnlock (lpfhLocalVars->hGlbTknHdr); lpfhLocalVars->t2.lpBase = NULL;

    return bRet;
} // End ParseHeader


//***************************************************************************
//  $ValidSysName
//
//  Validate the name as a system variable name
//***************************************************************************

BOOL ValidSysName
    (LPTOKEN lptkName)      // Ptr to name token

{
    return (lptkName->tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS);
} // End ValidSysName


//***************************************************************************
//  $fh_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

int fh_yylex
    (LPFH_YYSTYPE  lpYYLval,            // Ptr to lval
     LPFHLOCALVARS lpfhLocalVars)       // Ptr to local fhLocalVars

{
    // Check for stopping point
    if (lpfhLocalVars->lptkStop EQ lpfhLocalVars->lptkNext)
        return '\0';

#if (defined (DEBUG)) && (defined (YYLEX_DEBUG))
    dprintfW (L"==fh_yylex:  TknType = %S, CharIndex = %d",
              GetTokenTypeName (lpfhLocalVars->lptkNext->tkFlags.TknType),
              lpfhLocalVars->lptkNext->tkCharIndex);
#endif

    // Return the current token
    lpYYLval->tkToken            = *lpfhLocalVars->lptkNext;
    lpYYLval->uStrandLen         =
    lpYYLval->Indirect           = FALSE;
    lpYYLval->List               = FALSE;
    lpYYLval->lpYYStrandIndirect = NULL;
    lpYYLval->lpYYStrandBase     = lpfhLocalVars->lpYYStrandBase;

    // Split cases based upon the token type
    switch (lpfhLocalVars->lptkNext++->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // If the token is a sysname, return NAMESYS
            if (lpfhLocalVars->lptkNext[-1].tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS)
                return NAMESYS;
            else
            {
                // If the next token is a left bracket, return NAMEOPR
                if (lpfhLocalVars->lptkNext->tkFlags.TknType EQ TKT_LEFTBRACKET)
                    return NAMEOPR;
                else
                    return NAMEUNK;
            } // End IF

        case TKT_ASSIGN:
            return ASSIGN;

        case TKT_LISTSEP:
            return ';';

        case TKT_LEFTPAREN:
            return '(';

        case TKT_RIGHTPAREN:
            return ')';

        case TKT_LEFTBRACKET:
            return '[';

        case TKT_RIGHTBRACKET:
            return ']';

        case TKT_LEFTBRACE:
            return '{';

        case TKT_RIGHTBRACE:
            return '}';

        case TKT_COMMENT:
            return COMMENT;

        case TKT_SOS:
            return SOS;

        default:
            return UNK;
    } // End SWITCH
} // End fh_yylex


//***************************************************************************
//  $fh_yyerror
//
//  Error callback from Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fh_yyerror"
#else
#define APPEND_NAME
#endif

void fh_yyerror                     // Called for Bison syntax error
    (LPFHLOCALVARS lpfhLocalVars,   // Ptr to local fhLocalVars
     LPCHAR        s)               // Ptr to error msg

{
    char   szTemp[1024];
    WCHAR wszTemp[1024], *wp;
    UINT uCharIndex;

#ifdef DEBUG
    DbgMsg (s);
#endif
    // Get and save the character index position
    uCharIndex = lpfhLocalVars->lptkNext->tkCharIndex;
    lpfhLocalVars->tkErrorCharIndex = uCharIndex;

    // Check for SYNTAX ERROR
#define ERR     "syntax error"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        wsprintfW (wszTemp, L"SYNTAX ERROR in header position %d -- function NOT saved.", uCharIndex);
        wp = wszTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

    // Check for VALUE ERROR
#define ERR     "value error"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        wsprintfW (wszTemp, L"VALUE ERROR in header position %d -- function NOT saved.", uCharIndex);
        wp = wszTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

    // Check for LENGTH ERROR
#define ERR     "length error"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        wsprintfW (wszTemp, L"LENGTH ERROR in header position %d -- function NOT saved.", uCharIndex);
        wp = wszTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

#define ERR     "memory exhausted"
    lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
    if (lstrcmp (szTemp, ERR) EQ 0)
    {
        wsprintfW (wszTemp, L"Insufficient memory to parse header -- function NOT saved.");
        wp = wszTemp;

        goto DISPLAYCAT;
#undef  ERR
    } // End IF

    // Use the error message as given
    wsprintfW (wszTemp, L"%S", s);
    wp = wszTemp;

    goto DISPLAY;

DISPLAYCAT:
    // If the caller doesn't wants error messages displayed, ...
    if (!lpfhLocalVars->DisplayErr)
    {
        // Pass the error message back to the caller
        lstrcpyW (lpfhLocalVars->wszErrMsg, wszTemp);

        return;
    } // End IF

#ifdef DEBUG
    // Append the original error message
    wsprintfW (&wszTemp[lstrlenW (wszTemp)], L"(%S)", s);
#endif
DISPLAY:
    // Display a message box
    MessageBoxW (lpfhLocalVars->hWndEC,
                 wp,
                 lpwszAppName,
                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
} // End fh_yyerror
#undef  APPEND_NAME


//***************************************************************************
//  $fh_yyfprintf
//
//  Debugger output
//***************************************************************************

void fh_yyfprintf
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
} // End fh_yyfprintf


//***************************************************************************
//  $InitHdrStrand
//
//  Initialize the header strand stack.
//***************************************************************************

void InitHdrStrand
    (LPFH_YYSTYPE lpYYArg)          // Ptr to the incoming argument

{
    LPFHLOCALVARS lpfhLocalVars;    // Ptr to local fhLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Set the base of this strand to the next available location
    lpYYArg->lpYYStrandBase       =
    lpfhLocalVars->lpYYStrandBase = lpfhLocalVars->lpYYStrandNext;
} // End InitHdrStrand


//***************************************************************************
//  $PushHdrStrand_YY
//
//  Push a header token onto the strand stack.
//***************************************************************************

LPFH_YYSTYPE PushHdrStrand_YY
    (LPFH_YYSTYPE lpYYArg)          // Ptr to the incoming argument

{
    LPFHLOCALVARS lpfhLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Copy the strand base to the result
    lpYYArg->lpYYStrandBase = lpfhLocalVars->lpYYStrandBase;

    // Return our own position so the next user
    //   of this token can refer to it.
    if (!lpYYArg->lpYYStrandIndirect)
        lpYYArg->lpYYStrandIndirect = lpfhLocalVars->lpYYStrandNext;

    // Save this token on the strand stack
    //   and skip over it
    *lpfhLocalVars->lpYYStrandNext++ = *lpYYArg;

    return lpYYArg;
} // End PushHdrStrand_YY


//***************************************************************************
//  $MakeHdrStrand_YY
//
//  Make a header strand
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakeHdrStrand_YY"
#else
#define APPEND_NAME
#endif

LPFH_YYSTYPE MakeHdrStrand_YY
    (LPFH_YYSTYPE lpYYArg)          // Ptr to incoming token

{
    LPFHLOCALVARS lpfhLocalVars;    // Ptr to local fhLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Save the # elements in the strand
    lpYYArg->lpYYStrandBase->uStrandLen =
      (UINT) (lpfhLocalVars->lpYYStrandNext - lpYYArg->lpYYStrandBase);

    // Save the next position as the new base
    lpfhLocalVars->lpYYStrandBase = lpfhLocalVars->lpYYStrandNext;

    // Return the base of the current strand
    return lpYYArg->lpYYStrandBase;
} // End MakeHdrStrand_YY
#undef  APPEND_NAME


//***************************************************************************
//  $GetOprName_EM
//
//  Extract the function/operator/operand names from a list
//***************************************************************************

BOOL GetOprName_EM
    (LPFH_YYSTYPE lpYYArg)

{
    LPFHLOCALVARS lpfhLocalVars;    // Ptr to local fhLocalVars

    // Get this thread's LocalVars ptr
    lpfhLocalVars = (LPFHLOCALVARS) TlsGetValue (dwTlsFhLocalVars);

    // Split cases based upon the strand length
    switch (lpYYArg->uStrandLen)
    {
        case 1:         // Function
            lpfhLocalVars->DfnType     = DFNTYPE_FCN;
            lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[0];

            return TRUE;

        case 2:         // Monadic operator
            lpfhLocalVars->DfnType     = DFNTYPE_OP1;
            lpfhLocalVars->lpYYLftOpr  = &lpYYArg->lpYYStrandBase[0];
            lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[1];

            return TRUE;

        case 3:         // Dyadic operator
            lpfhLocalVars->DfnType     = DFNTYPE_OP2;
            lpfhLocalVars->lpYYLftOpr  = &lpYYArg->lpYYStrandBase[0];
            lpfhLocalVars->lpYYFcnName = &lpYYArg->lpYYStrandBase[1];
            lpfhLocalVars->lpYYRhtOpr  = &lpYYArg->lpYYStrandBase[2];

            return TRUE;

        default:
            if (lpfhLocalVars->DisplayErr)
            {
                lpfhLocalVars->lptkNext->tkCharIndex = lpYYArg->tkToken.tkCharIndex;
                fh_yyerror (lpfhLocalVars, "syntax error");
            } // End IF

            return FALSE;
    } // End SWITCH
} // End GetOprName_EM


//***************************************************************************
//  End of File: fh_parse.y
//***************************************************************************