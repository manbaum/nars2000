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

%token NAMEUNK ASSIGN

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

NoResHdr:
              NAMEUNK               // Niladic
    |         NAMEUNK NAMEUNK       // Monadic
    |         NAMEUNK List          // Monadic
    | NAMEUNK NAMEUNK NAMEUNK       // Dyadic
    | NAMEUNK NAMEUNK List          // Dyadic
    | LftArg  NAMEUNK NAMEUNK       // Dyadic
    | LftArg  NAMEUNK List          // Dyadic
    ;

Header:
             NoResHdr
    | Result NoResHdr
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
