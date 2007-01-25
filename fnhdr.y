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
#include "datatype.h"
#include "symtab.h"
#include "tokens.h"
#include "resdebug.h"
#include "Unicode.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF yyfprintf

#define DbgMsgW2(a) DbgMsgW(a)
////#define DbgMsgW2(a) DbgMsgW(a); DbgBrk ()

%}

%token NAMEUNK ASSIGN LINECONT

%start Header

%%

OpenList:
               NAMEUNK
    | OpenList NAMEUNK
    ;

List:
      '(' OpenList ')'
    ;

Result:
      List    ASSIGN
    | NAMEUNK ASSIGN
    ;

LftArg:
          List
    | '[' List ']'
    | '[' NAMEUNK ']'
    ;

MonOp:
      '(' NAMEUNK NAMEUNK ')'
    ;

DydOp:
      '(' NAMEUNK NAMEUNK NAMEUNK ')'
    ;

NoResHdr:
              NAMEUNK               // Niladic function

    |         NAMEUNK NAMEUNK       // Monadic function
    |         NAMEUNK List          // ...

    | NAMEUNK NAMEUNK NAMEUNK       // Dyadic function
    | NAMEUNK NAMEUNK List          // ...
    | LftArg  NAMEUNK NAMEUNK       // ...
    | LftArg  NAMEUNK List          // ...

//  |         MonOp   NAMEUNK       // Monadic operator, monadic function
//  |         MonOp   List          // ...

//  | NAMEUNK MonOp   NAMEUNK       // Monadic operator, dyadic function
//  | NAMEUNK MonOp   List          // ...
    | LftArg  MonOp   NAMEUNK       // ...
    | LftArg  MonOp   List          // ...

//  |         DydOp   NAMEUNK       // Dyadic operator, monadic function
//  |         DydOp   List          // ...

//  | NAMEUNK DydOp   NAMEUNK       // Dyadic operator, dyadic function
//  | NAMEUNK DydOp   List          // ...
    | LftArg  DydOp   NAMEUNK       // ...
    | LftArg  DydOp   List          // ...
    ;

Locals:
             LINECONT
    |        ';' NAMEUNK
    | Locals LINECONT
    | Locals ';' NAMEUNK
    ;

Header:
             NoResHdr
    |        NoResHdr Locals
    | Result NoResHdr
    | Result NoResHdr Locals
    ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************


//***************************************************************************
//  yylex
//
//  Lexical analyzer for YACC
//***************************************************************************

int yylex
    (void)

{

    return 12;
} // End yylex


//***************************************************************************
//  End of File: fnhdr.y
//***************************************************************************
// WE ***MUST*** KEEP A BLANK LINE AS THE LAST LINE IN THIS FILE
