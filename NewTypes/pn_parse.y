//***************************************************************************
//  NARS2000 -- Parser Grammar for XXX Point Notation
//***************************************************************************

//***************************************************************************
//  Parse a line of XXX Point Notation characters.
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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

/***************************************************************************

Build 1227 accumulates a strand of numeric constants all at once by calling
<ParsePointNotation> once on the entire scalar/vector (with intervening spaces),
then looping through the individual elements to determine the common storage
type taking into account that some FLTs might be expressible either as rational
integers or as more precise VFP numbers, and then calling <ParsePointNotation>
again on only those elements that need to be promoted to a higher storage type.
This allows (say) 1111111111111111111111111111111111111 1r2 to be parsed as FLT
RAT on the first pass, and then re-parse the FLT text as RAT so that the final
result is a RAT that displays identically to the format in which it was
entered.  Without this change, the FLT RAT pair would be promoted to VFP and
the FLT value would be converted to a VFP with imprecise trailing digits as in
1111111111111111174642258481095114752 0.5

***************************************************************************/

%{
#define STRICT
#include <windows.h>
#include <math.h>
#include "headers.h"

////#define YYLEX_DEBUG
////#define YYFPRINTF_DEBUG

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               pn_yyfprintf
#define fprintf                 pn_yyfprintf
////#define YYPRINT                 pn_yyprint
void pn_yyprint     (FILE *yyoutput, unsigned short int yytoknum, PN_YYSTYPE const yyvaluep);
#endif

#define YYMALLOC    malloc
#define YYFREE      free

#include "pn_parse.proto"

#define    YYSTYPE     PN_YYSTYPE
#define  LPYYSTYPE   LPPN_YYSTYPE
#define tagYYSTYPE  tagPN_YYSTYPE

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         pn_yy_symbol_print
#define yy_symbol_value_print   pn_yy_symbol_value_print
#define yy_reduce_print         pn_yy_reduce_print
#define yydestruct              pn_yydestruct

#define YYERROR2        {lppnLocalVars->bYYERROR = TRUE; YYERROR;}

#ifdef DEBUG
    #define DbgMsgWP(a)         if (bDebugPN) DbgMsgW (a)
#else
    #define DbgMsgWP(a)
#endif

#define NEED_EOT    lppnLocalVars->bNeedEOT = TRUE;

%}

%pure-parser
%name-prefix "pn_yy"
%parse-param {LPPNLOCALVARS lppnLocalVars}
%lex-param   {LPPNLOCALVARS lppnLocalVars}
%token EXT INF OVR EOT I J K L IJ JK KL S OS OI OJ OK NaN

%start VectorRes

%%

Digit:
      '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
    ;

LowerAlphabet:
      'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm'
    | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z'
    ;

UpperAlphabet:
      'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M'
    | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'
    ;

DEF_RATSEP:     'r'
DEF_VFPSEP:     'v'

Alphabet:
      LowerAlphabet
    | UpperAlphabet
    ;

Integer:
              Digit                 {DbgMsgWP (L"%%Integer:  Digit");
                                     // Mark starting offset
                                     $1.uNumAcc   = lppnLocalVars->uNumAcc;

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $1.chCur);

                                     $$ = $1;
                                    }
    | OVR     Digit                 {DbgMsgWP (L"%%Integer:  " WS_UTF16_OVERBAR L" Digit");
                                     // Mark starting offset
                                     $1.uNumAcc   = lppnLocalVars->uNumAcc;

                                     // Accumulate the negative sign
                                     PN_NumAcc (lppnLocalVars, OVERBAR1);

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);

                                     $$ = $1;
                                    }
    | Integer Digit                 {DbgMsgWP (L"%%Integer:  Integer Digit");
                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);
                                    }
    ;

// Unsigned integer
UnInteger:
              Digit                 {DbgMsgWP (L"%%UnInteger:  Digit");
                                     // Mark starting offset
                                     $1.uNumAcc = lppnLocalVars->uNumAcc;

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $1.chCur);

                                     $$ = $1;
                                    }
    | Integer Digit                 {DbgMsgWP (L"%%UnInteger:  Integer Digit");
                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);
                                    }
    ;

AlphaInt:
      {lppnLocalVars->uAlpAccInt = 0;}
          AlphaPos                  {DbgMsgWP (L"%%AlphaInt:  AlphaPos");
                                    }
    |
      {lppnLocalVars->uAlpAccInt = 0; PN_ChrAccInt (lppnLocalVars, OVERBAR1);}
      OVR AlphaPos                  {DbgMsgWP (L"%%AlphaInt:  " WS_UTF16_OVERBAR L"AlphaPos");
                                    }
    ;

AlphaPos:
               Alphabet             {DbgMsgWP (L"%%AlphaPos:  Alphabet");
                                     PN_ChrAccInt (lppnLocalVars, $1.chCur);
                                    }
    |          Digit                {DbgMsgWP (L"%%AlphaPos:  Digit");
                                     PN_ChrAccInt (lppnLocalVars, $1.chCur);
                                    }
    | AlphaPos Alphabet             {DbgMsgWP (L"%%AlphaPos:  AlphaPos Alphabet");
                                     PN_ChrAccInt (lppnLocalVars, $2.chCur);
                                    }
    | AlphaPos Digit                {DbgMsgWP (L"%%AlphaPos:  AlphaPos Digit");
                                     PN_ChrAccInt (lppnLocalVars, $2.chCur);
                                    }
    ;

AlphaDec:
               Alphabet             {DbgMsgWP (L"%%AlphaDec:  Alphabet");
                                     // Initialize lpszAlphaDec
                                     lppnLocalVars->lpszAlphaDec = &lppnLocalVars->lpszAlphaInt[lppnLocalVars->uAlpAccInt];
                                     lppnLocalVars->uAlpAccDec = 0;

                                     // Accumulate the character
                                     PN_ChrAccDec (lppnLocalVars, $1.chCur);
                                    }
    |          Digit                {DbgMsgWP (L"%%AlphaDec:  Digit");
                                     // Initialize lpszAlphaDec
                                     lppnLocalVars->lpszAlphaDec = &lppnLocalVars->lpszAlphaInt[lppnLocalVars->uAlpAccInt];
                                     lppnLocalVars->uAlpAccDec = 0;

                                     // Accumulate the character
                                     PN_ChrAccDec (lppnLocalVars, $1.chCur);
                                    }
    | AlphaDec Alphabet             {DbgMsgWP (L"%%AlphaDec:  AlphaDec Alphabet");
                                     PN_ChrAccDec (lppnLocalVars, $2.chCur);
                                    }
    | AlphaDec Digit                {DbgMsgWP (L"%%AlphaDec:  AlphaDec Digit");
                                     PN_ChrAccDec (lppnLocalVars, $2.chCur);
                                    }
    ;

Decimal:
              '.' Digit             {DbgMsgWP (L"%%Decimal:  '.' Digit");
                                     // Mark starting offset
                                     $1.uNumAcc   = lppnLocalVars->uNumAcc;

                                     // Accumulate the decimal point
                                     PN_NumAcc (lppnLocalVars, '.');

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);

                                     $$ = $1;
                                    }
    | OVR     '.' Digit             {DbgMsgWP (L"%%Decimal:  '" WS_UTF16_OVERBAR L"' '.' Digit");
                                     // Mark starting offset
                                     $1.uNumAcc   = lppnLocalVars->uNumAcc;

                                     // Accumulate the negative sign
                                     PN_NumAcc (lppnLocalVars, OVERBAR1);

                                     // Accumulate the decimal point
                                     PN_NumAcc (lppnLocalVars, '.');

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $3.chCur);

                                     $$ = $1;
                                    }
    | Integer '.' Digit             {DbgMsgWP (L"%%Decimal:  Integer '.' Digit");
                                     // Accumulate the decimal point
                                     PN_NumAcc (lppnLocalVars, '.');

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $3.chCur);
                                    }
    | Decimal     Digit             {DbgMsgWP (L"%%Decimal:  Decimal Digit");
                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);
                                    }
    ;

IntPoint:
      Integer                       {DbgMsgWP (L"%%IntPoint:  Integer");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, FALSE);

                                     $$ = $1;
                                    }
    ;

UnIntPoint:
      UnInteger                     {DbgMsgWP (L"%%UnIntPoint:  UnInteger");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, FALSE);

                                     $$ = $1;
                                    }
    ;

DecPoint:
      Integer                       {DbgMsgWP (L"%%DecPoint:  Integer");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, FALSE);

                                     $$ = $1;
                                    }
    | Integer '.'                   {DbgMsgWP (L"%%DecPoint:  Integer '.'");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as float
                                     $1.chType = PN_NUMTYPE_FLT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, FALSE);

                                     $$ = $1;
                                    }
    | Decimal                       {DbgMsgWP (L"%%DecPoint:  Decimal");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as float
                                     $1.chType = PN_NUMTYPE_FLT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, FALSE);

                                     $$ = $1;
                                    }
    ;

DecConstants:
          INF                       {DbgMsgWP (L"%%DecConstants:  INF");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_FLT, $1.uNumStart,  1, NULL);
                                    }
    | OVR INF                       {DbgMsgWP (L"%%DecConstants:  OVR INF");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_FLT, $1.uNumStart, -1, NULL);
                                    }
    | NaN                           {DbgMsgWP (L"%%DecConstants:  NaN");
                                     // Set constant NaN
                                     $$ = PN_SetNaN      (lppnLocalVars, PN_NUMTYPE_FLT, $1.uNumStart,     NULL);
                                    }
    ;

VfpConstants:
          INF DEF_VFPSEP            {DbgMsgWP (L"%%VfpConstants:  INF 'v'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_VFP, $1.uNumStart,  1, NULL);
                                    }
    |     INF DEF_VFPSEP UnIntPoint {DbgMsgWP (L"%%VfpConstants:  INF 'v' UnIntPoint");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_VFP, $1.uNumStart,  1, &$3.at.aplInteger);
                                    }
    | OVR INF DEF_VFPSEP            {DbgMsgWP (L"%%VfpConstants:  OVR INF 'v'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_VFP, $1.uNumStart, -1, NULL);
                                     if (lppnLocalVars->bYYERROR)
                                        YYERROR;
                                    }
    | OVR INF DEF_VFPSEP UnIntPoint {DbgMsgWP (L"%%VfpConstants:  OVR INF 'v' UnIntPoint");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_VFP, $1.uNumStart, -1, &$4.at.aplInteger);
                                     if (lppnLocalVars->bYYERROR)
                                        YYERROR;
                                    }
    |     NaN DEF_VFPSEP            {DbgMsgWP (L"%%VfpConstants:  NaN 'v'");
                                     // Set constant NaN
                                     $$ = PN_SetNaN      (lppnLocalVars, PN_NUMTYPE_VFP, $1.uNumStart, NULL);
                                    }
    |     NaN DEF_VFPSEP UnIntPoint {DbgMsgWP (L"%%VfpConstants:  NaN 'v' UnIntPoint");
                                     // Set constant NaN
                                     $$ = PN_SetNaN      (lppnLocalVars, PN_NUMTYPE_VFP, $1.uNumStart, &$3.at.aplInteger);
                                    }
    ;

// DecPoint & ExpPoint & RatPoint & ExtPoint & VfpPoint & Constants
DRVPoint:
      DecPoint                      {DbgMsgWP (L"%%DRVPoint:  DecPoint");
                                    }
    | ExpPoint                      {DbgMsgWP (L"%%DRVPoint:  ExpPoint");
                                    }
    | RatPoint                      {DbgMsgWP (L"%%DRVPoint:  RatPoint");
                                    }
    | ExtPoint                      {DbgMsgWP (L"%%DRVPoint:  ExtPoint");
                                    }
    | VfpPoint                      {DbgMsgWP (L"%%DRVPoint:  VfpPoint");
                                    }
    | DecConstants                  {DbgMsgWP (L"%%DRVPoint:  DecConstants");
                                    }
    | RatConstantsInt               {DbgMsgWP (L"%%DRVPoint:  RatConstantsInt");
                                    }
    | RatConstantsExt               {DbgMsgWP (L"%%DRVPoint:  RatConstantsExt");
                                    }
    | VfpConstants                  {DbgMsgWP (L"%%DRVPoint:  VfpConstants");
                                    }
    ;

// Hypercomplex Point
HcxPoint:
      DRVPoint 'J'     DRVPoint     {DbgMsgWP (L"%%HcxPoint:  DRVPoint 'J' DRVPoint");
                                     $$ = PN_MakeHc2Point (&$1, &$3, 'J', lppnLocalVars);
                                    }
    | DRVPoint 'a' 'd' DRVPoint     {DbgMsgWP (L"%%HcxPoint:  DRVPoint 'a' 'd' DRVPoint");
                                     $$ = PN_MakeHc2Point (&$1, &$4, 'd', lppnLocalVars);
                                    }
    | DRVPoint 'a' 'r' DRVPoint     {DbgMsgWP (L"%%HcxPoint:  DRVPoint 'a' 'r' DRVPoint");
                                     $$ = PN_MakeHc2Point (&$1, &$4, 'r', lppnLocalVars);
                                    }
    | DRVPoint 'a' 'u' DRVPoint     {DbgMsgWP (L"%%HcxPoint:  DRVPoint 'a' 'u' DRVPoint");
                                     $$ = PN_MakeHc2Point (&$1, &$4, 'u', lppnLocalVars);
                                    }
    | DRVPoint Co1X                 {DbgMsgWP (L"%%HcxPoint:  DRVPoint Co1X");
                                     $$ = PN_MakeHcxPoint (&$1, 1, &$2, lppnLocalVars);
                                    }
    | DRVPoint Co2X                 {DbgMsgWP (L"%%HcxPoint:  DRVPoint Co2X");
                                     $$ = PN_MakeHcxPoint (&$1, 2, &$2, lppnLocalVars);
                                    }
    | DRVPoint Co3X                 {DbgMsgWP (L"%%HcxPoint:  DRVPoint Co3X");
                                     $$ = PN_MakeHcxPoint (&$1, 3, &$2, lppnLocalVars);
                                    }
    | DRVPoint Co4X                 {DbgMsgWP (L"%%HcxPoint:  DRVPoint Co4X");
                                     $$ = PN_MakeHcxPoint (&$1, 4, &$2, lppnLocalVars);
                                    }
    | DRVPoint Co5X                 {DbgMsgWP (L"%%HcxPoint:  DRVPoint Co5X");
                                     $$ = PN_MakeHcxPoint (&$1, 5, &$2, lppnLocalVars);
                                    }
    | DRVPoint Co6X                 {DbgMsgWP (L"%%HcxPoint:  DRVPoint Co6X");
                                     $$ = PN_MakeHcxPoint (&$1, 6, &$2, lppnLocalVars);
                                    }
    | DRVPoint Co7X                 {DbgMsgWP (L"%%HcxPoint:  DRVPoint C07X");
                                     $$ = PN_MakeHcxPoint (&$1, 7, &$2, lppnLocalVars);
                                    }
    | DoxX EOT
    ;

DoxX:
      Do1X                          {DbgMsgWP (L"%%DoxX:  Do1X");
                                     $$ = PN_MakeHcxPoint (NULL, 1, &$1 , lppnLocalVars);
                                    }
    | Do2X                          {DbgMsgWP (L"%%DoxX:  Do2X");
                                     $$ = PN_MakeHcxPoint (NULL, 2, &$1 , lppnLocalVars);
                                    }
    | Do3X                          {DbgMsgWP (L"%%DoxX:  Do3X");
                                     $$ = PN_MakeHcxPoint (NULL, 3, &$1 , lppnLocalVars);
                                    }
    | Do4X                          {DbgMsgWP (L"%%DoxX:  Do4X");
                                     $$ = PN_MakeHcxPoint (NULL, 4, &$1 , lppnLocalVars);
                                    }
    | Do5X                          {DbgMsgWP (L"%%DoxX:  Do5X");
                                     $$ = PN_MakeHcxPoint (NULL, 5, &$1 , lppnLocalVars);
                                    }
    | Do6X                          {DbgMsgWP (L"%%DoxX:  Do6X");
                                     $$ = PN_MakeHcxPoint (NULL, 6, &$1 , lppnLocalVars);
                                    }
    | Do7X                          {DbgMsgWP (L"%%DoxX:  Do7X");
                                     $$ = PN_MakeHcxPoint (NULL, 7, &$1 , lppnLocalVars);
                                    }
    | DRVPoint S                    {DbgMsgWP (L"%%DoxX:  DRVPoint 's'");
                                     $$ = PN_MakeHcxPoint (&$1 , 0, NULL, lppnLocalVars);
                                    }
    | DRVPoint S Do1X               {DbgMsgWP (L"%%DoxX:  DRVPoint 's' Do1X");
                                     $$ = PN_MakeHcxPoint (&$1 , 1, &$3 , lppnLocalVars);
                                    }
    | DRVPoint S Do2X               {DbgMsgWP (L"%%DoxX:  DRVPoint 's' Do2X");
                                     $$ = PN_MakeHcxPoint (&$1 , 2, &$3 , lppnLocalVars);
                                    }
    | DRVPoint S Do3X               {DbgMsgWP (L"%%DoxX:  DRVPoint 's' Do3X");
                                     $$ = PN_MakeHcxPoint (&$1 , 3, &$3 , lppnLocalVars);
                                    }
    | DRVPoint S Do4X               {DbgMsgWP (L"%%DoxX:  DRVPoint 's' Do4X");
                                     $$ = PN_MakeHcxPoint (&$1 , 4, &$3 , lppnLocalVars);
                                    }
    | DRVPoint S Do5X               {DbgMsgWP (L"%%DoxX:  DRVPoint 's' Do5X");
                                     $$ = PN_MakeHcxPoint (&$1 , 5, &$3 , lppnLocalVars);
                                    }
    | DRVPoint S Do6X               {DbgMsgWP (L"%%DoxX:  DRVPoint 's' Do6X");
                                     $$ = PN_MakeHcxPoint (&$1 , 6, &$3 , lppnLocalVars);
                                    }
    | DRVPoint S Do7X               {DbgMsgWP (L"%%DoxX:  DRVPoint 's' Do7X");
                                     $$ = PN_MakeHcxPoint (&$1 , 7, &$3 , lppnLocalVars);
                                    }
    ;

Do1X:
      Do1                           {DbgMsgWP (L"%%Do1X:  Do1");}
    | Do1 Do2X                      {DbgMsgWP (L"%%Do1X:  Do1 Do2X");}
    | Do1 Do3X                      {DbgMsgWP (L"%%Do1X:  Do1 Do3X");}
    | Do1 Do4X                      {DbgMsgWP (L"%%Do1X:  Do1 Do4X");}
    | Do1 Do5X                      {DbgMsgWP (L"%%Do1X:  Do1 Do5X");}
    | Do1 Do6X                      {DbgMsgWP (L"%%Do1X:  Do1 Do6X");}
    | Do1 Do7X                      {DbgMsgWP (L"%%Do1X:  Do1 Do7X");}
    ;

Do2X:
      Do2                           {DbgMsgWP (L"%%Do2X:  Do2");}
    | Do2 Do3X                      {DbgMsgWP (L"%%Do2X:  Do2 Do3X");}
    | Do2 Do4X                      {DbgMsgWP (L"%%Do2X:  Do2 Do4X");}
    | Do2 Do5X                      {DbgMsgWP (L"%%Do2X:  Do2 Do5X");}
    | Do2 Do6X                      {DbgMsgWP (L"%%Do2X:  Do2 Do6X");}
    | Do2 Do7X                      {DbgMsgWP (L"%%Do2X:  Do2 Do7X");}
    ;

Do3X:
      Do3                           {DbgMsgWP (L"%%Do3X:  Do3");}
    | Do3 Do4X                      {DbgMsgWP (L"%%Do3X:  Do3 Do4X");}
    | Do3 Do5X                      {DbgMsgWP (L"%%Do3X:  Do3 Do5X");}
    | Do3 Do6X                      {DbgMsgWP (L"%%Do3X:  Do3 Do6X");}
    | Do3 Do7X                      {DbgMsgWP (L"%%Do3X:  Do3 Do7X");}
    ;

Do4X:
      Do4                           {DbgMsgWP (L"%%Do4X:  Do4");}
    | Do4 Do5X                      {DbgMsgWP (L"%%Do4X:  Do4 Do5X");}
    | Do4 Do6X                      {DbgMsgWP (L"%%Do4X:  Do4 Do6X");}
    | Do4 Do7X                      {DbgMsgWP (L"%%Do4X:  Do4 Do7X");}
    ;

Do5X:
      Do5                           {DbgMsgWP (L"%%Do5X:  Do5");}
    | Do5 Do6X                      {DbgMsgWP (L"%%Do5X:  Do5 Do6X");}
    | Do5 Do7X                      {DbgMsgWP (L"%%Do5X:  Do5 Do7X");}
    ;

Do6X:
      Do6                           {DbgMsgWP (L"%%Do6X:  Do6");}
    | Do6 Do7X                      {DbgMsgWP (L"%%Do6X:  Do6 Do7X");}
    ;

Do7X:
      Do7                           {DbgMsgWP (L"%%Do7X:  Do7");}
    ;

Do1:
      DRVPoint I                    {DbgMsgWP (L"%%Do1:  DRVPoint I");
                                     $$ = PN_MakeHcxCo (1, &$1, lppnLocalVars);
                                    }
    ;

Do2:
      DRVPoint J                    {DbgMsgWP (L"%%Do2:  DRVPoint J");
                                     $$ = PN_MakeHcxCo (2, &$1, lppnLocalVars);
                                    }
    ;

Do3:
      DRVPoint K                    {DbgMsgWP (L"%%Do3:  DRVPoint K");
                                     $$ = PN_MakeHcxCo (3, &$1, lppnLocalVars);
                                    }
    ;

Do4:
      DRVPoint L                    {DbgMsgWP (L"%%Do4:  DRVPoint L");
                                     $$ = PN_MakeHcxCo (4, &$1, lppnLocalVars);
                                    }
    | DRVPoint OS                   {DbgMsgWP (L"%%Do4:  DRVPoint OS");
                                     $$ = PN_MakeHcxCo (4, &$1, lppnLocalVars);
                                    }
    ;

Do5:
      DRVPoint IJ                   {DbgMsgWP (L"%%Do5:  DRVPoint IJ");
                                     $$ = PN_MakeHcxCo (5, &$1, lppnLocalVars);
                                    }
    | DRVPoint OI                   {DbgMsgWP (L"%%Do5:  DRVPoint OI");
                                     $$ = PN_MakeHcxCo (5, &$1, lppnLocalVars);
                                    }
    ;

Do6:
      DRVPoint JK                   {DbgMsgWP (L"%%Do6:  DRVPoint JK");
                                     $$ = PN_MakeHcxCo (6, &$1, lppnLocalVars);
                                    }
    | DRVPoint OJ                   {DbgMsgWP (L"%%Do6:  DRVPoint OJ");
                                     $$ = PN_MakeHcxCo (6, &$1, lppnLocalVars);
                                    }
    ;

Do7:
      DRVPoint KL                   {DbgMsgWP (L"%%Do7:  DRVPoint KL");
                                     $$ = PN_MakeHcxCo (7, &$1, lppnLocalVars);
                                    }
    | DRVPoint OK                   {DbgMsgWP (L"%%Do7:  DRVPoint OK");
                                     $$ = PN_MakeHcxCo (7, &$1, lppnLocalVars);
                                    }
    ;

Co1X:
      Co1                           {DbgMsgWP (L"%%Co1X:  Co1");}
    | Co1 Co2X                      {DbgMsgWP (L"%%Co1X:  Co1 Co2X");}
    | Co1 Co3X                      {DbgMsgWP (L"%%Co1X:  Co1 Co3X");}
    | Co1 Co4X                      {DbgMsgWP (L"%%Co1X:  Co1 Co4X");}
    | Co1 Co5X                      {DbgMsgWP (L"%%Co1X:  Co1 Co5X");}
    | Co1 Co6X                      {DbgMsgWP (L"%%Co1X:  Co1 Co6X");}
    | Co1 Co7X                      {DbgMsgWP (L"%%Co1X:  Co1 Co7X");}
    ;

Co2X:
      Co2                           {DbgMsgWP (L"%%Co2X:  Co2");}
    | Co2 Co3X                      {DbgMsgWP (L"%%Co2X:  Co2 Co3X");}
    | Co2 Co4X                      {DbgMsgWP (L"%%Co2X:  Co2 Co4X");}
    | Co2 Co5X                      {DbgMsgWP (L"%%Co2X:  Co2 Co5X");}
    | Co2 Co6X                      {DbgMsgWP (L"%%Co2X:  Co2 Co6X");}
    | Co2 Co7X                      {DbgMsgWP (L"%%Co2X:  Co2 Co7X");}
    ;

Co3X:
      Co3                           {DbgMsgWP (L"%%Co3X:  Co3");}
    | Co3 Co4X                      {DbgMsgWP (L"%%Co3X:  Co3 Co4X");}
    | Co3 Co5X                      {DbgMsgWP (L"%%Co3X:  Co3 Co5X");}
    | Co3 Co6X                      {DbgMsgWP (L"%%Co3X:  Co3 Co6X");}
    | Co3 Co7X                      {DbgMsgWP (L"%%Co3X:  Co3 Co7X");}
    ;

Co4X:
      Co4                           {DbgMsgWP (L"%%Co4X:  Co4");}
    | Co4 Co5X                      {DbgMsgWP (L"%%Co4X:  Co4 Co5X");}
    | Co4 Co6X                      {DbgMsgWP (L"%%Co4X:  Co4 Co6X");}
    | Co4 Co7X                      {DbgMsgWP (L"%%Co4X:  Co4 Co7X");}
    ;

Co5X:
      Co5                           {DbgMsgWP (L"%%Co5X:  Co5");}
    | Co5 Co6X                      {DbgMsgWP (L"%%Co5X:  Co5 Co6X");}
    | Co5 Co7X                      {DbgMsgWP (L"%%Co5X:  Co5 Co7X");}
    ;

Co6X:
      Co6                           {DbgMsgWP (L"%%Co6X:  Co6");}
    | Co6 Co7X                      {DbgMsgWP (L"%%Co6X:  Co6 Co7X");}
    ;

Co7X:
      Co7                           {DbgMsgWP (L"%%Co7X:  Co7");}
    ;

Co1:
      'i'     DRVPoint              {DbgMsgWP (L"%%Co1:  'i' DRVPoint");
                                     $$ = PN_MakeHcxCo (1, &$2, lppnLocalVars);
                                    }
    ;

Co2:
      'j'     DRVPoint              {DbgMsgWP (L"%%Co2:  'j' DRVPoint");
                                     $$ = PN_MakeHcxCo (2, &$2, lppnLocalVars);
                                    }
    ;

Co3:
      'k'     DRVPoint              {DbgMsgWP (L"%%Co3:  'k' DRVPoint");
                                     $$ = PN_MakeHcxCo (3, &$2, lppnLocalVars);
                                    }
    ;

Co4:
      'l'     DRVPoint              {DbgMsgWP (L"%%Co4:  'l' DRVPoint");
                                     $$ = PN_MakeHcxCo (4, &$2, lppnLocalVars);
                                    }
    | 'o' 's' DRVPoint              {DbgMsgWP (L"%%Co4:  'o' 's' DRVPoint");
                                     $$ = PN_MakeHcxCo (4, &$3, lppnLocalVars);
                                    }
    ;

Co5:
      'i' 'i' DRVPoint              {DbgMsgWP (L"%%Co5:  'i' 'i' DRVPoint");
                                     $$ = PN_MakeHcxCo (5, &$3, lppnLocalVars);
                                    }
    | 'i' 'j' DRVPoint              {DbgMsgWP (L"%%Co5:  'i' 'j' DRVPoint");
                                     $$ = PN_MakeHcxCo (5, &$3, lppnLocalVars);
                                    }
    | 'o' 'i' DRVPoint              {DbgMsgWP (L"%%Co5:  'o' 'i' DRVPoint");
                                     $$ = PN_MakeHcxCo (5, &$3, lppnLocalVars);
                                    }
    ;

Co6:
      'i' 'k' DRVPoint              {DbgMsgWP (L"%%Co6:  'i' 'k' DRVPoint");
                                     $$ = PN_MakeHcxCo (6, &$3, lppnLocalVars);
                                    }
    | 'j' 'j' DRVPoint              {DbgMsgWP (L"%%Co6:  'j' 'j' DRVPoint");
                                     $$ = PN_MakeHcxCo (6, &$3, lppnLocalVars);
                                    }
    | 'j' 'k' DRVPoint              {DbgMsgWP (L"%%Co6:  'j' 'k' DRVPoint");
                                     $$ = PN_MakeHcxCo (6, &$3, lppnLocalVars);
                                    }
    | 'o' 'j' DRVPoint              {DbgMsgWP (L"%%Co6:  'o' 'j' DRVPoint");
                                     $$ = PN_MakeHcxCo (6, &$3, lppnLocalVars);
                                    }
    ;

Co7:
      'i' 'l' DRVPoint              {DbgMsgWP (L"%%Co7:  'i' 'l' DRVPoint");
                                     $$ = PN_MakeHcxCo (7, &$3, lppnLocalVars);
                                    }
    | 'k' 'k' DRVPoint              {DbgMsgWP (L"%%Co7:  'k' 'k' DRVPoint");
                                     $$ = PN_MakeHcxCo (7, &$3, lppnLocalVars);
                                    }
    | 'k' 'l' DRVPoint              {DbgMsgWP (L"%%Co7:  'k' 'l' DRVPoint");
                                     $$ = PN_MakeHcxCo (7, &$3, lppnLocalVars);
                                    }
    | 'o' 'k' DRVPoint              {DbgMsgWP (L"%%Co7:  'o' 'k' DRVPoint");
                                     $$ = PN_MakeHcxCo (7, &$3, lppnLocalVars);
                                    }
    ;

// RatPoint left and right arguments
// ExtPoint left arg
RatArgs:
      DecPoint                      {DbgMsgWP (L"RatArgs:  DecPoint");}
    | ExpPoint                      {DbgMsgWP (L"RatArgs:  ExpPoint");}
    ;

RatPoint:
      RatArgs  DEF_RATSEP RatArgs   {DbgMsgWP (L"%%RatPoint:  RatArgs 'r' RatArgs");
                                     // Make it into a RatPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeRatPoint   (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

RatConstantsInt:
          INF  DEF_RATSEP RatArgs   {DbgMsgWP (L"%%RatConstantsInt:  INF 'r' RatArgs");
                                     // Set constant infinity
                                     //   taking into account negative integer and -0
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT, $1.uNumStart,  1 - 2 * (lppnLocalVars->lpszNumAccum[0] EQ OVERBAR1), NULL);
                                    }
    | OVR INF  DEF_RATSEP RatArgs   {DbgMsgWP (L"%%RatConstantsInt:  OVR INF 'r' RatArgs");
                                     // Set constant infinity
                                     //   taking into account negative integer and -0
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT, $1.uNumStart,  (2 * (lppnLocalVars->lpszNumAccum[0] EQ OVERBAR1)) - 1, NULL);
                                    }
    |     NaN  DEF_RATSEP RatArgs   {DbgMsgWP (L"%%RatConstantsInt:  NaN 'r' RatArgs");
                                     // Set constant NaN
                                     //   taking into account negative integer and -0
                                     $$ = PN_SetNaN      (lppnLocalVars, PN_NUMTYPE_RAT, $1.uNumStart,  NULL);
                                    }
    | RatArgs  DEF_RATSEP INF       {DbgMsgWP (L"%%RatConstantsInt:  RatArgs 'r' INF");
                                     // If the integer is signed, ...
                                     if (gAllowNeg0
                                      && lppnLocalVars->lpszNumAccum[0] EQ OVERBAR1)
                                     {
                                         // Initialize the result and set to -0
                                         mpfr_init_set_str (&$$.at.aplVfp, "-0", 10, MPFR_RNDN);

                                         // Mark the result as VFP
                                         $$.chType = PN_NUMTYPE_VFP;
                                     } else
                                     {
                                         // Initialize the result to 0/1
                                         mpq_init (&$$.at.aplRat);

                                         // Mark the result as RAT
                                         $$.chType = PN_NUMTYPE_RAT;
                                     } // End IF/ELSE
                                    }
    | RatArgs  DEF_RATSEP OVR INF   {DbgMsgWP (L"%%RatConstantsInt:  RatArgs 'r' OVR INF");
                                     // If the integer is NOT signed, ...
                                     if (gAllowNeg0
                                      && lppnLocalVars->lpszNumAccum[0] NE OVERBAR1)
                                     {
                                         // Initialize the result and set to -0
                                         mpfr_init_set_str (&$$.at.aplVfp, "-0", 10, MPFR_RNDN);

                                         // Mark the result as VFP
                                         $$.chType = PN_NUMTYPE_VFP;
                                     } else
                                     {
                                         // Initialize the result to 0/1
                                         mpq_init (&$$.at.aplRat);

                                         // Mark the result as RAT
                                         $$.chType = PN_NUMTYPE_RAT;
                                     } // End IF/ELSE
                                    }
    ;

RatConstantsExt:
          INF EXT                   {DbgMsgWP (L"%%RatConstantsExt:  INF 'x'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT, $1.uNumStart,  1, NULL);
                                    }
    | OVR INF EXT                   {DbgMsgWP (L"%%RatConstantsExt:  OVR INF 'x'");
                                     // Set constant infinity
                                     $$ = PN_SetInfinity (lppnLocalVars, PN_NUMTYPE_RAT, $1.uNumStart, -1, NULL);
                                    }
    |     NaN EXT                   {DbgMsgWP (L"%%RatConstantsExt:  NaN 'x'");
                                     // Set constant NaN
                                     $$ = PN_SetNaN      (lppnLocalVars, PN_NUMTYPE_RAT, $1.uNumStart, NULL);
                                    }
    ;

ExtPoint:
      RatArgs  EXT                  {DbgMsgWP (L"%%ExtPoint:  RatArgs 'x'");
                                     // Make it into a RatPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeRatPoint   (&$1, NULL, lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

// Euler/Pi/Gamma left and right arguments
EPGArgs:
      DecPoint                      {DbgMsgWP (L"EPGArgs:  DecPoint");}
    | ExpPoint                      {DbgMsgWP (L"EPGArgs:  ExpPoint");}
    | HcxPoint                      {DbgMsgWP (L"EPGArgs:  HcxPoint");}
    | RatPoint                      {DbgMsgWP (L"EPGArgs:  RatPoint");}
    | VfpPoint                      {DbgMsgWP (L"EPGArgs:  VfpPoint");}
    ;

GammaPoint:
      EPGArgs   'g' EPGArgs         {DbgMsgWP (L"%%GammaPoint:  EPGArgs 'g' EPGArgs");
                                     // Make it into a GammaPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeGammaPoint (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | EPGArgs   'g' ExtPoint        {DbgMsgWP (L"%%GammaPoint:  EPGArgs 'g' ExtPoint");
                                     // Make it into a GammaPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeGammaPoint (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

PiPoint:
      EPGArgs   'p' EPGArgs         {DbgMsgWP (L"%%PiPoint:  EPGArgs 'p' EPGArgs");
                                     // Make it into a PiPoint number
                                     lppnLocalVars->lpYYRes = PN_MakePiPoint    (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | EPGArgs   'p' ExtPoint        {DbgMsgWP (L"%%PiPoint:  EPGArgs 'p' ExtPoint");
                                     // Make it into a PiPoint number
                                     lppnLocalVars->lpYYRes = PN_MakePiPoint    (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

eE:
      'e'
    | 'E'
    ;

ExpPoint:
      DecPoint eE  IntPoint         {DbgMsgWP (L"%%ExpPoint:  DecPoint 'e' IntPoint");
                                     // Make it into a ExpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeExpPoint   (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

VfpPoint:
      DecPoint DEF_VFPSEP           {DbgMsgWP (L"%%VfpPoint:  DecPoint 'v'");
                                     // Make it into a VfpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeVfpPoint   (&$1, NULL, lppnLocalVars, NULL);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | DecPoint DEF_VFPSEP UnIntPoint{DbgMsgWP (L"%%VfpPoint:  DecPoint 'v' UnIntPoint");
                                     // Make it into a VfpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeVfpPoint   (&$1, NULL, lppnLocalVars, &$3.at.aplInteger);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | DecPoint eE  IntPoint DEF_VFPSEP
                                    {DbgMsgWP (L"%%VfpPoint:  DecPoint 'e' IntPoint 'v'");
                                     // Make it into a VfpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeVfpPoint   (&$1, &$3,  lppnLocalVars, NULL);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | DecPoint eE  IntPoint DEF_VFPSEP UnIntPoint
                                    {DbgMsgWP (L"%%VfpPoint:  DecPoint 'e' IntPoint 'v' UnIntPoint");
                                     // Make it into a VfpPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeVfpPoint   (&$1, &$3,  lppnLocalVars, &$5.at.aplInteger);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

EulerPoint:
      EPGArgs  'x' EPGArgs          {DbgMsgWP (L"%%EulerPoint:  EPGArgs 'x' EPGArgs");
                                     // Make it into a EulerPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeEulerPoint (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | EPGArgs  'x' ExtPoint         {DbgMsgWP (L"%%EulerPoint:  EPGArgs 'x' ExtPoint");
                                     // Make it into a EulerPoint number
                                     lppnLocalVars->lpYYRes = PN_MakeEulerPoint (&$1, &$3,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

// BasePoint left args
BaseArgs:
      EPGArgs                       {DbgMsgWP (L"BaseArgs:  EPGArgs");}
    | EulerPoint                    {DbgMsgWP (L"BaseArgs:  EulerPoint");}
    | GammaPoint                    {DbgMsgWP (L"BaseArgs:  GammaPoint");}
    | PiPoint                       {DbgMsgWP (L"BaseArgs:  PiPoint");}
    ;

BasePoint:
      BaseArgs   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  BaseArgs 'b' AlphaInt");
                                     // Make it into a BasePoint number
                                     lppnLocalVars->lpYYRes = PN_MakeBasePoint  (&$1, &$3, NULL, lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | BaseArgs   'b' AlphaInt '.'   {DbgMsgWP (L"%%BasePoint:  BaseArgs 'b' AlphaInt '.'");
                                     // Make it into a BasePoint number
                                     lppnLocalVars->lpYYRes = PN_MakeBasePoint  (&$1, &$3, NULL, lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | BaseArgs   'b'          '.' AlphaDec
                                    {DbgMsgWP (L"%%BasePoint:  BaseArgs 'b' '.' AlphaDec");
                                     // Make it into a BasePoint number
                                     lppnLocalVars->lpYYRes = PN_MakeBasePoint  (&$1, NULL, &$4,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    | BaseArgs   'b' AlphaInt '.' AlphaDec
                                    {DbgMsgWP (L"%%BasePoint:  BaseArgs 'b' AlphaInt '.' AlphaDec");
                                     // Make it into a BasePoint number
                                     lppnLocalVars->lpYYRes = PN_MakeBasePoint  (&$1, &$3, &$5,  lppnLocalVars);
                                     if (lppnLocalVars->lpYYRes EQ NULL)
                                         YYERROR2;
                                     $$ = *lppnLocalVars->lpYYRes;
                                    }
    ;

Number:
      BasePoint                     {DbgMsgWP (L"%%Number:  BasePoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | BaseArgs                      {DbgMsgWP (L"%%Number:  BaseArgs");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | ExtPoint                      {DbgMsgWP (L"%%Number:  ExtPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | DecConstants                  {DbgMsgWP (L"%%Number:  DecConstants");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | RatConstantsExt               {DbgMsgWP (L"%%Number:  RatConstantsExt");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | RatConstantsInt               {DbgMsgWP (L"%%Number:  RatConstantsInt");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | VfpConstants                  {DbgMsgWP (L"%%Number:  VfpConstants");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    ;

// Accumulate into a vector
VectorAcc:
      Number                        {DbgMsgWP (L"%%VectorAcc:  Number");
                                     // Accumulate the current value into the vector
                                     if (!PN_VectorAcc (&$1, lppnLocalVars))
                                         YYERROR2;
                                    }
    | VectorAcc Space Number        {DbgMsgWP (L"%%VectorAcc:  VectorAcc Space Number");
                                     // Accumulate the current value into the vector
                                     if (!PN_VectorAcc (&$3, lppnLocalVars))
                                         YYERROR2;
                                    }
    ;

Space:
            ' '                     {DbgMsgWP (L"%%Space:  ' '");
                                     // Skip over the space
                                     lppnLocalVars->uNumIni++;
                                    }
    | Space ' '                     {DbgMsgWP (L"%%Space:  Space ' '");
                                     // Skip over the space
                                     lppnLocalVars->uNumIni++;
                                    }
    ;

// The resulting vector
VectorRes:
      VectorAcc                     {DbgMsgWP (L"%%VectorRes:  VectorAcc");
                                     // Create the scalar or vector result
                                     if (!PN_VectorRes (lppnLocalVars))
                                        YYERROR2;
                                    }
    ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************

//***************************************************************************
//  $ParsePointNotation
//
//  Parse a sequence of Point Notation characters
//
//  Thanks to the designers of J for these clever ideas.
//***************************************************************************

UBOOL ParsePointNotation
    (LPPNLOCALVARS lppnLocalVars)           // Ptr to PN Local vars

{
    UBOOL bRet;                             // TRUE iff result is valid
    int   i;                                // Loop counter

    // Initialize the starting indices
    lppnLocalVars->uNumCur    =
    lppnLocalVars->uNumIni    =
    lppnLocalVars->uAlpAccInt =
    lppnLocalVars->uAlpAccDec =
    lppnLocalVars->uNumAcc    =
    lppnLocalVars->bNeedEOT   =
    lppnLocalVars->bEOT       =
    lppnLocalVars->bEOS       = 0;

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
        lppnLocalVars->lpCoeff[i] = NULL;

    // While the last char is a blank, ...
    while (lppnLocalVars->uNumLen && IsWhite (lppnLocalVars->lpszStart[lppnLocalVars->uNumLen - 1]))
        // Delete it
        lppnLocalVars->uNumLen--;

    // Check for Rat but not Vfp nor Complex Angle separator
    if (strchr (lppnLocalVars->lpszStart, 'v'  ) EQ NULL
     && strstr (lppnLocalVars->lpszStart, "ad" ) EQ NULL
     && strstr (lppnLocalVars->lpszStart, "ar" ) EQ NULL
     && (strchr (lppnLocalVars->lpszStart, 'r' ) NE NULL
      || strstr (lppnLocalVars->lpszStart, "x ") NE NULL
      || lppnLocalVars->lpszStart[lppnLocalVars->uNumLen - 1] EQ 'x'))
    {
        // Weed out non-RAT expressions such as 2g3 and 2p3
        if (strchr (lppnLocalVars->lpszStart, 'g' ) EQ NULL
         && strchr (lppnLocalVars->lpszStart, 'p' ) EQ NULL)
        {
            LPCHAR p;

            // Weed out non-RAT expressions such as 2x3
            p = strchr (lppnLocalVars->lpszStart, 'x');
            while (p NE NULL && (p[1] EQ ' ' || p[1] EQ AC_EOS))
                p = strchr (p + 1, 'x');
            if (p EQ NULL)
                // Save for later use
                lppnLocalVars->bRatSep = TRUE;
        } // End IF
    } // End IF

#if YYDEBUG
    // Enable debugging
    yydebug = TRUE;
#endif
    __try
    {
        // Parse the header, check for errors
        //   0 = success
        //   1 = YYABORT or APL error
        //   2 = memory exhausted
        bRet = pn_yyparse (lppnLocalVars) EQ 0;
    } __except (CheckException (GetExceptionInformation (), L"ParsePointNotation"))
    {
        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_NONCE_ERROR:
                // Mark as in error
                bRet = FALSE;
                ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                           NULL);
                break;

            default:
                // Display message for unhandled exception
                DisplayException ();

                // Mark as in error
                bRet = FALSE;

                break;
        } // End SWITCH
    } // End __try/__except

    // Loop through all of the parts
    for (i = 0; i < 8; i++)
    if (lppnLocalVars->lpCoeff[i] NE NULL)
    {
        MyGlobalFree (lppnLocalVars->lpCoeff[i]); lppnLocalVars->lpCoeff[i] = NULL;
    } // End FOR/IF

#if YYDEBUG
    // Disable debugging
    yydebug = FALSE;
#endif
    return bRet;
} // End ParsePointNotation


//***************************************************************************
//  $pn_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

int pn_yylex
    (LPPN_YYSTYPE  lpYYLval,            // Ptr to lval
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    UCHAR uChar;
    static UINT uCnt = 0;

#if (defined (DEBUG)) && (defined (YYLEX_DEBUG))
    {
        char *lp;
        char ch[2] = {'\0'};

        if (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur]EQ AC_EOS)
            lp = "\\0";
        else
        {
            ch[0] = lppnLocalVars->lpszStart[lppnLocalVars->uNumCur];
            lp = &ch[0];
        } // End IF/ELSE

        dprintfWL0 (L"pn_yylex(%u):  '%S'",
                     ++uCnt,
                     lp);
    }
#endif
    // Check for EOT
    if (lppnLocalVars->bEOT)
    {
        int iRes;

        // Clear the flag for next time
        lppnLocalVars->bEOT = FALSE;

        // Get the proper return value
        iRes = lppnLocalVars->bEOS ? AC_EOS : ' ';

        // Clear the flag for next time
        lppnLocalVars->bEOS = FALSE;

        return iRes;
    } // End IF

    // Save the index position in lpszStart
    lpYYLval->uNumStart = lppnLocalVars->uNumCur;

    // Check for EOL
    if (lppnLocalVars->uNumCur EQ lppnLocalVars->uNumLen)
        lpYYLval->chCur = AC_EOS;
    else
    {
        // Get the next char
        lpYYLval->chCur = lppnLocalVars->lpszStart[lppnLocalVars->uNumCur++];

        // If the character is 'x' and
        //   is the last in the sequence, ...
        if (lpYYLval->chCur EQ 'x'
         && (lppnLocalVars->uNumCur EQ lppnLocalVars->uNumLen
          || IsWhite (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur])))
            return EXT;

        // If the character is overbar, ...
        if (lpYYLval->chCur EQ OVERBAR1)
            return OVR;

        // If the character is infinity, ...
        if (lpYYLval->chCur EQ INFINITY1)
            return INF;

        // If the character is NaN, ...
        if (lpYYLval->chCur EQ NaN1)
            return NaN;
    } // End IF/ELSE

    // If we need an EOT, ...
    if (lppnLocalVars->bNeedEOT)
    {
        // If the current token is a white space or EOS, ...
        if (IsWhite (lpYYLval->chCur) || lpYYLval->chCur EQ AC_EOS)
        {
            // Mark as no longer needing an EOT
            lppnLocalVars->bNeedEOT = FALSE;

            // Mark as just returned an EOT
            lppnLocalVars->bEOT     = TRUE;

            // Mark if we just returned an EOS
            lppnLocalVars->bEOS     = (lpYYLval->chCur EQ AC_EOS);

            return EOT;
        } // End IF
    } // End IF

    // Get the current char
    uChar = lpYYLval->chCur;

    // If the current char is a HC separator, ...
    if (uChar NE AC_EOS
     && strchr (HC_SEPS, uChar) NE NULL)
    {
        LPCHAR lpChar;
        UCHAR  uCharPrv,
               uCharNxt,
               uCharZap;

        // It's possible for lpszStart to contain more chars than ->uNumLen
        //   would indicate, so we terminate the string here so as not to overread.
        uCharZap = lppnLocalVars->lpszStart[lppnLocalVars->uNumLen];

        // Point to the char after the HC notation chars
        //   starting with the current char;  "uNumCur"
        //   is the index of the next char and "uNumCur - 1"
        //   is the index of the current char from which
        //   "uChar" came.
        lppnLocalVars->lpszStart[lppnLocalVars->uNumLen] = AC_EOS;
        lpChar = SkipPastStr (&lppnLocalVars->lpszStart[lppnLocalVars->uNumCur - 1],
                               HC_CHARS);
        // Get the char at the end of and after the HC notation chars
        uCharPrv = lpChar[-1];
        uCharNxt = lpChar[ 0];

        // Restore zapped char
        lppnLocalVars->lpszStart[lppnLocalVars->uNumLen] = uCharZap;

        // Restart the initial char
        lppnLocalVars->uNumIni =
        lppnLocalVars->uNumCur;

        // If the ending char is a HC separator,
        //   and the next char is an EOS or white space, ...
        if (strchr (HC_SEPS, uCharPrv) NE NULL
         && (uCharNxt EQ AC_EOS || IsWhite (uCharNxt)))
        {
            // Mark as needing an EOT
            lppnLocalVars->bNeedEOT = TRUE;

            // If the current char is a HC separator (possibly combined with the next token), ...
            switch (lpYYLval->chCur)
            {
                case 'i':
                    // If this is a digraph, ...
                    if (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur] EQ 'j')
                    {
                        // Account for absorbing this token
                        lppnLocalVars->uNumCur++;

                        return IJ;
                    } else
                        return I;

                case 'j':
                    // If this is a digraph, ...
                    if (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur] EQ 'k')
                    {
                        // Account for absorbing this token
                        lppnLocalVars->uNumCur++;

                        return JK;
                    } else
                        return J;

                case 'k':
                    // If this is a digraph, ...
                    if (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur] EQ 'l')
                    {
                        // Account for absorbing this token
                        lppnLocalVars->uNumCur++;

                        return KL;
                    } else
                        return K;

                case 'l':
                    return L;

                case 's':
                    return S;

                case 'o':
                    // If this is a digraph OS, ...
                    if (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur] EQ 's')
                    {
                        // Account for absorbing this token
                        lppnLocalVars->uNumCur++;

                        return OS;
                    } else
                    // If this is a digraph OI, ...
                    if (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur] EQ 'i')
                    {
                        // Account for absorbing this token
                        lppnLocalVars->uNumCur++;

                        return OI;
                    } else
                    // If this is a digraph OJ, ...
                    if (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur] EQ 'j')
                    {
                        // Account for absorbing this token
                        lppnLocalVars->uNumCur++;

                        return OJ;
                    } else
                    // If this is a digraph OK, ...
                    if (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur] EQ 'k')
                    {
                        // Account for absorbing this token
                        lppnLocalVars->uNumCur++;

                        return OK;
                    } else
                        return 0;

                defstop
                    break;
            } // End SWITCH
        } // End IF
    } // End IF

    // Return it
    return lpYYLval->chCur;
} // End pn_yylex


//***************************************************************************
//  $pn_yyerror
//
//  Error callback from Bison
//***************************************************************************

void pn_yyerror                     // Called for Bison syntax error
    (LPPNLOCALVARS lppnLocalVars,   // Ptr to local pnLocalVars
     const char   *s)               // Ptr to error msg

{
    char    szTemp[1024];
    LPWCHAR lpwszTemp;

    DbgMsg ((char *) s);

    // Check for SYNTAX ERROR
#define ERR     PN_SYNTAX
    if (MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR)),    // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
        lpwszTemp = ERRMSG_SYNTAX_ERROR APPEND_NAME;
    else
    // Check for NONCE ERROR
#define ERR     PN_NONCE
    if (MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR)),    // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
    {
        lpwszTemp = ERRMSG_NONCE_ERROR APPEND_NAME;

        // Back off by one so the caret points to the symbol in error
        lppnLocalVars->uNumCur--;
    } else
    // Check for WS FULL
#define ERR     PN_WSFULL
    if (MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR)),    // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
        lpwszTemp = ERRMSG_WS_FULL APPEND_NAME;
    else
    // Check for DOMAIN ERROR
#define ERR     PN_DOMAIN
    if (MyStrcpyn (szTemp, sizeof (szTemp), s, strsizeof (ERR)),    // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
        lpwszTemp = ERRMSG_DOMAIN_ERROR APPEND_NAME;
    else
        lpwszTemp = ERRMSG_PN_PARSE APPEND_NAME;

    // Save the error message
    ErrorMessageIndirect (lpwszTemp);

    // Set the error char index
    lppnLocalVars->uCharIndex += lppnLocalVars->uNumCur;

    // Set the error flag
    lppnLocalVars->bYYERROR = TRUE;

    // If we're at the end, ...
    if (lppnLocalVars->lpszStart[lppnLocalVars->uNumCur] EQ WC_EOS)
        // Back off to the last char
        lppnLocalVars->uCharIndex--;

    if (lppnLocalVars->hGlbRes NE NULL)
    {
        if (lppnLocalVars->lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (lppnLocalVars->hGlbRes); lppnLocalVars->lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (lppnLocalVars->hGlbRes); lppnLocalVars->hGlbRes = NULL;
    } // End IF
} // End pn_yyerror


//***************************************************************************
//  $pn_yyfprintf
//
//  Debugger output
//***************************************************************************

void pn_yyfprintf
    (FILE  *hfile,          // Ignore this
     LPCHAR lpszFmt,        // Format string
     ...)                   // Zero or more arguments

{
#if (defined (DEBUG)) && (defined (YYFPRINTF_DEBUG))
    HRESULT  hResult;       // The result of <StringCbVPrintf>
    va_list  vl;
    APLU3264 i1;
    static   char szTemp[256] = {'\0'};

    // Initialize the variable list
    va_start (vl, lpszFmt);

    // Accumulate into local buffer because
    //   Bison calls this function multiple
    //   times for the same line, terminating
    //   the last call for the line with a LF.
    hResult = StringCbVPrintf (&szTemp[lstrlen (szTemp)],
                                sizeof (szTemp),
                                lpszFmt,
                                vl);
    // End the variable list
    va_end (vl);

    // If it failed, ...
    if (FAILED (hResult))
        DbgBrk ();                  // #ifdef DEBUG

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
    } // End IF/ELSE
#endif
} // End pn_yyfprintf


//***************************************************************************
//  End of File: pn_parse.y
//***************************************************************************
