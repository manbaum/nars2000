//***************************************************************************
//  NARS2000 -- Parser Grammar for XXX Point Notation
//***************************************************************************

//***************************************************************************
//  Parse a line of XXX Point Notation characters.
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

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
#define STRICT
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
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

int  pn_yylex       (LPPN_YYSTYPE lpYYLval, LPPNLOCALVARS lppnLocalVars);
void pn_yyerror     (LPPNLOCALVARS lppnLocalVars, LPCHAR s);
void pn_yyfprintf   (FILE *hfile, LPCHAR lpszFmt, ...);

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

////#define DbgMsgWP(a)         DbgMsgW(a)
////#define DbgMsgWP(a)         DbgMsgW(a); DbgBrk ()
////#define DbgMsgWP(a)         DbgMsgW(a)
    #define DbgMsgWP(a)

#define WSFULL          "memory exhausted"

// Define macro for Boolean or Integer type
#define IsIntegerType(a)        ((a) EQ PN_NUMTYPE_BOOL || (a) EQ PN_NUMTYPE_INT)
#define IsRatType(a)            ((a) EQ PN_NUMTYPE_RAT)
#define IsVfpType(a)            ((a) EQ PN_NUMTYPE_VFP)

%}

%pure-parser
%name-prefix="pn_yy"
%parse-param {LPPNLOCALVARS lppnLocalVars}
%lex-param   {LPPNLOCALVARS lppnLocalVars}
%token EXT INF OVR

%start Number

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
                                     $1.uNumOff = lppnLocalVars->uNumAcc;

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $1.chCur);

                                     $$ = $1;
                                    }
    | OVR     Digit                 {DbgMsgWP (L"%%Integer:  " WS_UTF16_OVERBAR L" Digit");
                                     // Mark starting offset
                                     $1.uNumOff = lppnLocalVars->uNumAcc;

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

AlphaInt:
               Alphabet             {DbgMsgWP (L"%%AlphaInt:  Alphabet");
                                     PN_ChrAcc (lppnLocalVars, $1.chCur);
                                    }
    |          Digit                {DbgMsgWP (L"%%AlphaInt:  Digit");
                                     PN_ChrAcc (lppnLocalVars, $1.chCur);
                                    }
    | AlphaInt Alphabet             {DbgMsgWP (L"%%AlphaInt:  Alphaint Alphabet");
                                     PN_ChrAcc (lppnLocalVars, $2.chCur);
                                    }
    | AlphaInt Digit                {DbgMsgWP (L"%%AlphaInt:  AlphaInt Digit");
                                     PN_ChrAcc (lppnLocalVars, $2.chCur);
                                    }
    ;

Decimal:
              '.' Digit             {DbgMsgWP (L"%%Decimal:  '.' Digit");
                                     // Mark starting offset
                                     $1.uNumOff = lppnLocalVars->uNumAcc;

                                     // Accumulate the decimal point
                                     PN_NumAcc (lppnLocalVars, '.');

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);

                                     $$ = $1;
                                    }
    | OVR     '.' Digit             {DbgMsgWP (L"%%Decimal:  '" WS_UTF16_OVERBAR L"' '.' Digit");
                                     // Mark starting offset
                                     $1.uNumOff = lppnLocalVars->uNumAcc;

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

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

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
          INF                       {DbgMsgWP (L"%%DecConstants:  " WS_UTF16_INFINITY);
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as float
                                     $1.chType = PN_NUMTYPE_FLT;
                                     $1.at.aplFloat = PosInfinity;

                                     $$ = $1;
                                    }
    | OVR INF                       {DbgMsgWP (L"%%DecConstants:  " WS_UTF16_OVERBAR WS_UTF16_INFINITY);
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as float
                                     $1.chType = PN_NUMTYPE_FLT;
                                     $1.at.aplFloat = NegInfinity;

                                     $$ = $1;
                                    }
    ;

VfpConstants:
          INF DEF_VFPSEP            {DbgMsgWP (L"%%VfpConstants:  INF 'v'");
                                     // Mark as positive infinity
                                     mpfr_set_inf (&$1.at.aplVfp, 1);

                                     // Mark the result as VFP
                                     $1.chType = PN_NUMTYPE_VFP;

                                     $$ = $1;
                                    }
    | OVR INF DEF_VFPSEP            {DbgMsgWP (L"%%VfpConstants:  OVR INF 'v'");
                                     // Mark as negative infinity
                                     mpfr_set_inf (&$1.at.aplVfp, -1);

                                     // Mark the result as VFP
                                     $1.chType = PN_NUMTYPE_VFP;

                                     $$ = $1;
                                    }
    ;

// Complex Point
Hc2Point:
      DecPoint 'i' DecPoint         {DbgMsgWP (L"%%Hc2Point:  DecPoint 'i' DecPoint");
                                     // If the real part is integer, ...
                                     if (IsIntegerType ($1.chType))
                                         // Convert it to float
                                         $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
                                     // If the imaginary part is integer, ...
                                     if (IsIntegerType ($3.chType))
                                         // Convert it to float
                                         $3.at.aplFloat = (APLFLOAT) $3.at.aplInteger;

                                     // Save the real & imaginary parts
                                     $1.at.aplHC2.Real = $1.at.aplFloat;
                                     $1.at.aplHC2.Imag = $3.at.aplFloat;

                                     // Mark the result as Complex
                                     $1.chType = PN_NUMTYPE_HC2;

                                     $$ = $1;
                                    }
    | DecPoint 'J' DecPoint         {DbgMsgWP (L"%%Hc2Point:  DecPoint 'J' DecPoint");
                                     // If the real part is integer, ...
                                     if (IsIntegerType ($1.chType))
                                         // Convert it to float
                                         $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
                                     // If the imaginary part is integer, ...
                                     if (IsIntegerType ($3.chType))
                                         // Convert it to float
                                         $3.at.aplFloat = (APLFLOAT) $3.at.aplInteger;

                                     // Save the real & imaginary parts
                                     $1.at.aplHC2.Real = $1.at.aplFloat;
                                     $1.at.aplHC2.Imag = $3.at.aplFloat;

                                     // Mark the result as Complex
                                     $1.chType = PN_NUMTYPE_HC2;

                                     $$ = $1;
                                    }
    | DecPoint 'a' 'd' DecPoint     {DbgMsgWP (L"%%Hc2Point:  DecPoint 'a' 'd' DecPoint");
                                     // If the real part is integer, ...
                                     if (IsIntegerType ($1.chType))
                                         // Convert it to float
                                         $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
                                     // If the imaginary part is integer, ...
                                     if (IsIntegerType ($4.chType))
                                         // Convert it to float
                                         $4.at.aplFloat = (APLFLOAT) $4.at.aplInteger;

                                     // Convert from degrees to radians
                                     $4.at.aplFloat = FloatPi * $4.at.aplFloat / 180;

                                     // Save the real & imaginary parts
                                     // ***FIXME*** -- Do we need to reduce the radians modulo Pi/2 ??
                                     $1.at.aplHC2.Real = $1.at.aplFloat * cos ($4.at.aplFloat);
                                     $1.at.aplHC2.Imag = $1.at.aplFloat * sin ($4.at.aplFloat);

                                     // Mark the result as Complex
                                     $1.chType = PN_NUMTYPE_HC2;

                                     $$ = $1;
                                    }
    | DecPoint 'a' 'r' DecPoint     {DbgMsgWP (L"%%Hc2Point:  DecPoint 'a' 'r' DecPoint");
                                     // If the real part is integer, ...
                                     if (IsIntegerType ($1.chType))
                                         // Convert it to float
                                         $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
                                     // If the imaginary part is integer, ...
                                     if (IsIntegerType ($4.chType))
                                         // Convert it to float
                                         $4.at.aplFloat = (APLFLOAT) $4.at.aplInteger;

                                     // Save the real & imaginary parts
                                     // ***FIXME*** -- Do we need to reduce the radians modulo Pi/2 ??
                                     $1.at.aplHC2.Real = $1.at.aplFloat * cos ($4.at.aplFloat);
                                     $1.at.aplHC2.Imag = $1.at.aplFloat * sin ($4.at.aplFloat);

                                     // Mark the result as Complex
                                     $1.chType = PN_NUMTYPE_HC2;

                                     $$ = $1;
                                    }
    ;

// Quaternion Point
Hc4Point:
      DecPoint 'i' DecPoint 'j' DecPoint 'i' 'j' DecPoint
                                    {DbgMsgWP (L"%%Hc4Point:  DecPoint 'i' DecPoint 'j' DecPoint 'i' 'j' DecPoint");
                                     // If the real part is integer, ...
                                     if (IsIntegerType ($1.chType))
                                         // Convert it to float
                                         $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
                                     // If the imaginary part #1 is integer, ...
                                     if (IsIntegerType ($3.chType))
                                         // Convert it to float
                                         $3.at.aplFloat = (APLFLOAT) $3.at.aplInteger;
                                     // If the imaginary part #2 is integer, ...
                                     if (IsIntegerType ($5.chType))
                                         // Convert it to float
                                         $5.at.aplFloat = (APLFLOAT) $5.at.aplInteger;
                                     // If the imaginary part #3 is integer, ...
                                     if (IsIntegerType ($8.chType))
                                         // Convert it to float
                                         $8.at.aplFloat = (APLFLOAT) $8.at.aplInteger;

                                     // Save the real & imaginary parts
                                     $1.at.aplHC4.Real  = $1.at.aplFloat;
                                     $1.at.aplHC4.Imag1 = $3.at.aplFloat;
                                     $1.at.aplHC4.Imag2 = $5.at.aplFloat;
                                     $1.at.aplHC4.Imag3 = $8.at.aplFloat;

                                     // Mark the result as Quaternion
                                     $1.chType = PN_NUMTYPE_HC4;

                                     $$ = $1;
                                    }
    | DecPoint 'i' DecPoint 'j' DecPoint 'k' DecPoint
                                    {DbgMsgWP (L"%%Hc4Point:  DecPoint 'i' DecPoint 'j' DecPoint 'k' DecPoint");
                                     // If the real part is integer, ...
                                     if (IsIntegerType ($1.chType))
                                         // Convert it to float
                                         $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
                                     // If the imaginary part #1 is integer, ...
                                     if (IsIntegerType ($3.chType))
                                         // Convert it to float
                                         $3.at.aplFloat = (APLFLOAT) $3.at.aplInteger;
                                     // If the imaginary part #2 is integer, ...
                                     if (IsIntegerType ($5.chType))
                                         // Convert it to float
                                         $5.at.aplFloat = (APLFLOAT) $5.at.aplInteger;
                                     // If the imaginary part #3 is integer, ...
                                     if (IsIntegerType ($7.chType))
                                         // Convert it to float
                                         $7.at.aplFloat = (APLFLOAT) $7.at.aplInteger;

                                     // Save the real & imaginary parts
                                     $1.at.aplHC4.Real  = $1.at.aplFloat;
                                     $1.at.aplHC4.Imag1 = $3.at.aplFloat;
                                     $1.at.aplHC4.Imag2 = $5.at.aplFloat;
                                     $1.at.aplHC4.Imag3 = $7.at.aplFloat;

                                     // Mark the result as Quaternion
                                     $1.chType = PN_NUMTYPE_HC4;

                                     $$ = $1;
                                    }
    ;

// Octonion Point
Hc8Point:
      DecPoint 'i' DecPoint 'j' DecPoint 'k' DecPoint 'l' DecPoint 'i' 'j' DecPoint 'i' 'k' DecPoint 'i' 'l' DecPoint
                                    {DbgMsgWP (L"%%Hc4Point:  DecPoint 'i' DecPoint 'j' DecPoint 'k' DecPoint 'l' DecPoint 'i' 'j' DecPoint 'i' 'k' DecPoint 'i' 'l' DecPoint");
                                     // If the real part is integer, ...
                                     if (IsIntegerType ($1.chType))
                                         // Convert it to float
                                         $1.at.aplFloat = (APLFLOAT) $1.at.aplInteger;
                                     // If the imaginary part #1 is integer, ...
                                     if (IsIntegerType ($3.chType))
                                         // Convert it to float
                                         $3.at.aplFloat = (APLFLOAT) $3.at.aplInteger;
                                     // If the imaginary part #2 is integer, ...
                                     if (IsIntegerType ($5.chType))
                                         // Convert it to float
                                         $5.at.aplFloat = (APLFLOAT) $5.at.aplInteger;
                                     // If the imaginary part #3 is integer, ...
                                     if (IsIntegerType ($7.chType))
                                         // Convert it to float
                                         $7.at.aplFloat = (APLFLOAT) $7.at.aplInteger;
                                     // If the imaginary part #4 is integer, ...
                                     if (IsIntegerType ($9.chType))
                                         // Convert it to float
                                         $9.at.aplFloat = (APLFLOAT) $9.at.aplInteger;
                                     // If the imaginary part #5 is integer, ...
                                     if (IsIntegerType ($12.chType))
                                         // Convert it to float
                                         $12.at.aplFloat = (APLFLOAT) $12.at.aplInteger;
                                     // If the imaginary part #6 is integer, ...
                                     if (IsIntegerType ($15.chType))
                                         // Convert it to float
                                         $15.at.aplFloat = (APLFLOAT) $15.at.aplInteger;
                                     // If the imaginary part #7 is integer, ...
                                     if (IsIntegerType ($18.chType))
                                         // Convert it to float
                                         $18.at.aplFloat = (APLFLOAT) $18.at.aplInteger;

                                     // Save the real & imaginary parts
                                     $1.at.aplHC8.Real  = $1.at.aplFloat;
                                     $1.at.aplHC8.Imag1 = $3.at.aplFloat;
                                     $1.at.aplHC8.Imag2 = $5.at.aplFloat;
                                     $1.at.aplHC8.Imag3 = $7.at.aplFloat;
                                     $1.at.aplHC8.Imag4 = $9.at.aplFloat;
                                     $1.at.aplHC8.Imag5 = $12.at.aplFloat;
                                     $1.at.aplHC8.Imag6 = $15.at.aplFloat;
                                     $1.at.aplHC8.Imag7 = $18.at.aplFloat;

                                     // Mark the result as Octonion
                                     $1.chType = PN_NUMTYPE_HC8;

                                     $$ = $1;
                                    }
    ;

Rational:
      Integer  DEF_RATSEP Digit     {DbgMsgWP (L"%%Rational:  Integer 'r' Digit");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, TRUE);

                                     // Mark starting offset
                                     $1.uNumOff = lppnLocalVars->uNumAcc;

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $3.chCur);

                                     $$ = $1;
                                    }
    | Integer  DEF_RATSEP OVR Digit {DbgMsgWP (L"%%Rational:  Integer 'r' '" WS_UTF16_OVERBAR L"' Digit");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, TRUE);

                                     // Mark starting offset
                                     $1.uNumOff = lppnLocalVars->uNumAcc;

                                     // Accumulate the negative sign
                                     PN_NumAcc (lppnLocalVars, OVERBAR1);

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $4.chCur);

                                     $$ = $1;
                                    }
    | Rational         Digit        {DbgMsgWP (L"%%Rational:  Rational Digit");
                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);
                                    }
    ;

RatPoint:
      Rational                      {DbgMsgWP (L"%%RatPoint:  Rational");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as rational
                                     $1.chType = PN_NUMTYPE_RAT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, TRUE);

                                     $$ = $1;
                                    }
    ;

RatConstants:
          INF EXT                   {DbgMsgWP (L"%%RatConstants:  INF EXT");
                                     // Mark as positive infinity
                                     mpq_set_infsub (&$1.at.aplRat, 1);

                                     // Mark the result as RAT
                                     $1.chType = PN_NUMTYPE_RAT;

                                     $$ = $1;
                                    }
    |     INF DEF_RATSEP Integer    {DbgMsgWP (L"%%RatConstants:  INF 'r' Integer");
                                     // Mark as positive infinity
                                     mpq_set_infsub (&$1.at.aplRat, 1);

                                     // Mark the result as RAT
                                     $1.chType = PN_NUMTYPE_RAT;

                                     $$ = $1;
                                    }
    | OVR INF EXT                   {DbgMsgWP (L"%%RatConstants:  OVR INF EXT");
                                     // Mark as negative infinity
                                     mpq_set_infsub (&$1.at.aplRat, -1);

                                     // Mark the result as RAT
                                     $1.chType = PN_NUMTYPE_RAT;

                                     $$ = $1;
                                    }
    | OVR INF DEF_RATSEP Integer    {DbgMsgWP (L"%%RatConstants:  OVR INF 'r' Integer");
                                     // Mark as negative infinity
                                     mpq_set_infsub (&$1.at.aplRat, -1);

                                     // Mark the result as RAT
                                     $1.chType = PN_NUMTYPE_RAT;

                                     $$ = $1;
                                    }
    | Integer DEF_RATSEP INF        {DbgMsgWP (L"%%RatConstants:  Integer 'r' INF");
                                     // Initialize to 0
                                     mpq_init (&$1.at.aplRat);

                                     // Mark the result as RAT
                                     $1.chType = PN_NUMTYPE_RAT;

                                     $$ = $1;
                                    }
    | Integer DEF_RATSEP OVR INF    {DbgMsgWP (L"%%RatConstants:  Integer 'r' OVR INF");
                                     // Initialize to 0
                                     mpq_init (&$1.at.aplRat);

                                     // Mark the result as RAT
                                     $1.chType = PN_NUMTYPE_RAT;

                                     $$ = $1;
                                    }
    ;

ExtPoint:
      Integer EXT                   {DbgMsgWP (L"%%ExtPoint:  Integer 'x'");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1, TRUE);

                                     // Set the denominator to 1
                                     mpz_init_set_ui (mpq_denref (&$1.at.aplRat), 1);

                                     // Mark the result as rational
                                     $1.chType = PN_NUMTYPE_RAT;

                                     $$ = $1;
                                    }
    ;

// Euler/Pi left and right arguments
EPArgs:
      DecPoint                      {DbgMsgWP (L"%%EPArgs:  DecPoint");
                                    }
    | ExpPoint                      {DbgMsgWP (L"%%EPArgs:  ExpPoint");
                                    }
    | Hc2Point                      {DbgMsgWP (L"%%EPArgs:  Hc2Point");
                                    }
    | Hc4Point                      {DbgMsgWP (L"%%EPArgs:  Hc4Point");
                                    }
    | Hc8Point                      {DbgMsgWP (L"%%EPArgs:  Hc8Point");
                                    }
    | RatPoint                      {DbgMsgWP (L"%%EPArgs:  RatPoint");
                                    }
    | VfpPoint                      {DbgMsgWP (L"%%EPArgs:  VfpPoint");
                                    }
    ;

PiPoint:
      EPArgs    'p' EPArgs          {DbgMsgWP (L"%%PiPoint:  EPArgs 'p' EPArgs");
                                     $$ = *PN_MakePiPoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

ExpPoint:
      DecPoint 'e' Integer          {DbgMsgWP (L"%%ExpPoint:  DecPoint 'e' Integer");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     $$ = *PN_MakeExpPoint (lppnLocalVars, &$1, &$3);
                                    }
    | DecPoint 'E' Integer          {DbgMsgWP (L"%%ExpPoint:  DecPoint 'E' Integer");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     $$ = *PN_MakeExpPoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

VfpPoint:
      DecPoint DEF_VFPSEP           {DbgMsgWP (L"%%VfpPoint:  DecPoint 'v'");
                                     $$ = *PN_MakeVfpPoint (lppnLocalVars, &$1, NULL);
                                    }
    | DecPoint 'e' Integer DEF_VFPSEP
                                    {DbgMsgWP (L"%%VfpPoint:  DecPoint 'e' Integer 'v'");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     $$ = *PN_MakeVfpPoint (lppnLocalVars, &$1, &$3);
                                    }
    | DecPoint 'E' Integer DEF_VFPSEP
                                    {DbgMsgWP (L"%%VfpPoint:  DecPoint 'E' Integer 'v'");
                                     // Terminate the (Exponent) argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     $$ = *PN_MakeVfpPoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

EulerPoint:
      EPArgs   'x' EPArgs           {DbgMsgWP (L"%%EulerPoint:  EPArgs 'x' EPArgs");
                                     $$ = *PN_MakeEulerPoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

BasePoint:
      DecPoint   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  DecPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | EulerPoint 'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  EulerPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | ExpPoint   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  ExpPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | PiPoint    'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  PiPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | RatPoint   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  RatPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | Hc2Point   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  Hc2Point 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | Hc4Point   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  Hc4Point 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | Hc8Point   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  Hc8Point 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    | VfpPoint   'b' AlphaInt       {DbgMsgWP (L"%%BasePoint:  VfpPoint 'b' AlphaInt");
                                     $$ = *PN_MakeBasePoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

Number:
      BasePoint                     {DbgMsgWP (L"%%Number:  BasePoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | DecPoint                      {DbgMsgWP (L"%%Number:  DecPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | EulerPoint                    {DbgMsgWP (L"%%Number:  EulerPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | ExpPoint                      {DbgMsgWP (L"%%Number:  ExpPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | ExtPoint                      {DbgMsgWP (L"%%Number:  ExtPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | PiPoint                       {DbgMsgWP (L"%%Number:  PiPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | RatPoint                      {DbgMsgWP (L"%%Number:  RatPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | Hc2Point                      {DbgMsgWP (L"%%Number:  Hc2Point");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | Hc4Point                      {DbgMsgWP (L"%%Number:  Hc4Point");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | Hc8Point                      {DbgMsgWP (L"%%Number:  Hc8Point");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | VfpPoint                      {DbgMsgWP (L"%%Number:  VfpPoint");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | DecConstants                  {DbgMsgWP (L"%%Number:  DecConstants");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | RatConstants                  {DbgMsgWP (L"%%Number:  RatConstants");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    | VfpConstants                  {DbgMsgWP (L"%%Number:  VfpConstants");
                                     lppnLocalVars->at             = $1.at;
                                     lppnLocalVars->chType         = $1.chType;
                                    }
    ;

%%

//***************************************************************************
//  Start of C program
//***************************************************************************

//***************************************************************************
//  $PN_actUNK
//***************************************************************************

void PN_actUNK
    (LPPN_YYSTYPE lpSrc)

{
    DbgStop ();
} // End PN_actUNK


//***************************************************************************
//  $PN_actBOOL_FLT
//***************************************************************************

void PN_actBOOL_FLT
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from BOOL to FLT
    lpSrc->at.aplFloat = (APLFLOAT) lpSrc->at.aplInteger;

    lpSrc->chType = PN_NUMTYPE_FLT;
} // End PN_actBOOL_FLT


//***************************************************************************
//  $PN_actBOOL_RAT
//***************************************************************************

void PN_actBOOL_RAT
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from BOOL to RAT
    mpq_init_set_sx (&lpSrc->at.aplRat, lpSrc->at.aplInteger, 1);

    lpSrc->chType = PN_NUMTYPE_RAT;
} // End PN_actBOOL_RAT


//***************************************************************************
//  $PN_actBOOL_VFP
//***************************************************************************

void PN_actBOOL_VFP
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from BOOL to VFP
    mpfr_init_set_sx (&lpSrc->at.aplVfp, lpSrc->at.aplInteger, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actBOOL_VFP


//***************************************************************************
//  $PN_actINT_FLT
//***************************************************************************

void PN_actINT_FLT
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from INT to FLT
    lpSrc->at.aplFloat = (APLFLOAT) lpSrc->at.aplInteger;

    lpSrc->chType = PN_NUMTYPE_FLT;
} // End PN_actINT_FLT


//***************************************************************************
//  $PN_actINT_RAT
//***************************************************************************

void PN_actINT_RAT
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from INT to RAT
    mpq_init_set_sx (&lpSrc->at.aplRat, lpSrc->at.aplInteger, 1);

    lpSrc->chType = PN_NUMTYPE_RAT;
} // End PN_actINT_RAT


//***************************************************************************
//  $PN_actINT_VFP
//***************************************************************************

void PN_actINT_VFP
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from INT to VFP
    mpfr_init_set_sx (&lpSrc->at.aplVfp, lpSrc->at.aplInteger, MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actINT_VFP


//***************************************************************************
//  $PN_actFLT_VFP
//***************************************************************************

void PN_actFLT_VFP
    (LPPN_YYSTYPE lpSrc)

{
    // Convert the value from FLT to VFP
    mpfr_init_set_d  (&lpSrc->at.aplVfp, lpSrc->at.aplFloat  , MPFR_RNDN);

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actFLT_VFP


//***************************************************************************
//  $PN_actRAT_VFP
//***************************************************************************

void PN_actRAT_VFP
    (LPPN_YYSTYPE lpSrc)

{
    APLVFP aplVfp = {0};

    // Convert the value from RAT to VFP
    mpfr_init_set_q  (&aplVfp, &lpSrc->at.aplRat, MPFR_RNDN);

    Myq_clear (&lpSrc->at.aplRat);

    lpSrc->at.aplVfp = aplVfp;

    lpSrc->chType = PN_NUMTYPE_VFP;
} // End PN_actRAT_VFP


#define PN_MAT                                                                                                              \
{/*     BOOL          INT          FLT          HC2          HC4          HC8          RAT          VFP     */              \
   {M(BOOL,BOOL),M(BOOL,INT ),M(BOOL,FLT ),M(BOOL,HC2 ),M(BOOL,HC4 ),M(BOOL,HC8 ),M(BOOL,RAT ),M(BOOL,VFP )},   /* BOOL */  \
   {M(INT ,INT ),M(INT ,INT ),M(INT ,FLT ),M(INT ,HC2 ),M(INT ,HC4 ),M(INT ,HC8 ),M(INT ,RAT ),M(INT ,VFP )},   /* INT  */  \
   {M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,HC2 ),M(FLT ,HC4 ),M(FLT ,HC8 ),M(FLT ,VFP ),M(FLT ,VFP )},   /* FLT  */  \
   {M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC2 ),M(HC2 ,HC4 ),M(HC2 ,HC8 ),M(HC2 ,HC2 ),M(HC2 ,HC2 )},   /* HC2  */  \
   {M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC4 ),M(HC4 ,HC8 ),M(HC4 ,HC4 ),M(HC4 ,HC4 )},   /* HC4  */  \
   {M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 ),M(HC8 ,HC8 )},   /* HC8  */  \
   {M(RAT ,RAT ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,HC2 ),M(RAT ,HC4 ),M(RAT ,HC8 ),M(RAT ,RAT ),M(RAT ,VFP )},   /* RAT  */  \
   {M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,HC2 ),M(VFP ,HC4 ),M(VFP ,HC8 ),M(VFP ,VFP ),M(VFP ,VFP )},   /* VFP  */  \
};
// PN_NUMTYPE_xxx promotion result matrix
PNNUMTYPE aNumTypePromote[PN_NUMTYPE_LENGTH][PN_NUMTYPE_LENGTH] =
#define M(a,b)  PN_NUMTYPE_##b
PN_MAT
#undef  M

typedef void (*PN_ACTION)(LPPN_YYSTYPE);

#define PN_actBOOL_BOOL     NULL
#define PN_actBOOL_INT      NULL
////ine PN_actBOOL_FLT      NULL
#define PN_actBOOL_HC2      PN_actUNK
#define PN_actBOOL_HC4      PN_actUNK
#define PN_actBOOL_HC8      PN_actUNK

#define PN_actINT_INT       NULL
////ine PN_actINT_FLT       NULL
#define PN_actINT_HC2       PN_actUNK
#define PN_actINT_HC4       PN_actUNK
#define PN_actINT_HC8       PN_actUNK

#define PN_actFLT_FLT       NULL
#define PN_actFLT_HC2       PN_actUNK
#define PN_actFLT_HC4       PN_actUNK
#define PN_actFLT_HC8       PN_actUNK

#define PN_actHC2_HC2       NULL
#define PN_actHC2_HC4       PN_actUNK
#define PN_actHC2_HC8       PN_actUNK

#define PN_actHC4_HC4       NULL
#define PN_actHC4_HC8       PN_actUNK

#define PN_actHC8_HC8       NULL

#define PN_actRAT_RAT       NULL
#define PN_actRAT_HC2       PN_actUNK
#define PN_actRAT_HC4       PN_actUNK
#define PN_actRAT_HC8       PN_actUNK

#define PN_actVFP_VFP       NULL
#define PN_actVFP_HC2       PN_actUNK
#define PN_actVFP_HC4       PN_actUNK
#define PN_actVFP_HC8       PN_actUNK

// PN_NUMTYPE_xxx promotion action matrix
PN_ACTION aNumTypeAction [PN_NUMTYPE_LENGTH][PN_NUMTYPE_LENGTH] =
#define M(a,b)  PN_act##a##_##b
PN_MAT
#undef  M


//***************************************************************************
//  $ParsePointNotation
//
//  Parse a sequence of Point Notation characters
//
//  Thanks to the designers of J for these clever ideas.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParsePointNotation"
#else
#define APPEND_NAME
#endif

UBOOL ParsePointNotation
    (LPPNLOCALVARS lppnLocalVars)           // Ptr to PN Local vars

{
    UBOOL bRet;                             // TRUE iff result is valid

    // Initialize the starting indices
    lppnLocalVars->uNumCur = 0;
    lppnLocalVars->uNumAcc = 0;
    lppnLocalVars->uAlpAcc = 0;
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

#if YYDEBUG
    // Disable debugging
    yydebug = FALSE;
#endif
    return bRet;
} // End ParsePointNotation
#undef  APPEND_NAME


//***************************************************************************
//  $PN_NumAcc
//
//  Accumulate another digit into the number
//***************************************************************************

void PN_NumAcc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     char          chCur)               // The next digit

{
    // Save the digit
    lppnLocalVars->lpszNumAccum[lppnLocalVars->uNumAcc++] = chCur;

    // Ensure properly terminated
    lppnLocalVars->lpszNumAccum[lppnLocalVars->uNumAcc  ] = '\0';
} // End PN_NumAcc


//***************************************************************************
//  $PN_NumCalc
//
//  Calculate the value of a number
//***************************************************************************

void PN_NumCalc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYArg,             // Ptr to incoming YYSTYPE
     UBOOL         bUseRat)             // TRUE iff this routine may blowup to Rational

{
    UBOOL  bRet = TRUE,                 // TRUE iff the result is valid
           bSigned;                     // TRUE iff the number is negative
    APLINT aplInteger;                  // Temporary integer
    UINT   uLen,                        // Loop length
           uOff,                        // Starting offset
           uAcc;                        // Loop counter

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return;

    // Initialize the starting offset
    uOff = lpYYArg->uNumOff;

    // Split cases based upon the current numeric type
    switch (lpYYArg->chType)
    {
        case PN_NUMTYPE_INT:
            // If we're forming the numerator of a Rational number, ...
            if (bUseRat)
            {
                // Convert the numerator to a GMP integer
                mpz_init_set_str (mpq_numref (&lpYYArg->at.aplRat), &lppnLocalVars->lpszNumAccum[uOff], 10);

                break;
            } else
            {
                // Initialize the accumulator
                aplInteger = 0;

                // Initialize the loop length
                uLen = lstrlen (&lppnLocalVars->lpszNumAccum[uOff]);

                // Handle negative sign
                uAcc = bSigned = (lppnLocalVars->lpszNumAccum[uOff] EQ OVERBAR1);

                // If the number is negative, ...
                if (bSigned)
                    // Loop through the digits
                    for (; bRet && uAcc < uLen; uAcc++)
                    {
                        aplInteger = _imul64 (aplInteger, 10, &bRet);
                        if (bRet)
                            aplInteger = _isub64 (aplInteger, lppnLocalVars->lpszNumAccum[uOff + uAcc] - '0', &bRet);
                    } // End FOR
                else
                    // Loop through the digits
                    for (; bRet && uAcc < uLen; uAcc++)
                    {
                        aplInteger = _imul64 (aplInteger, 10, &bRet);
                        if (bRet)
                            aplInteger = _iadd64 (aplInteger, lppnLocalVars->lpszNumAccum[uOff + uAcc] - '0', &bRet);
                    } // End FOR
                if (bRet)
                {
                    // Save in the result
                    lpYYArg->at.aplInteger = aplInteger;

                    if (IsBooleanValue (aplInteger))
                        lpYYArg->chType = PN_NUMTYPE_BOOL;
                    break;
                } // End IF
            } // End IF

            // Change the type to float
            lpYYArg->chType = PN_NUMTYPE_FLT;

            // Fall through to the float case

        case PN_NUMTYPE_FLT:
            // Use David Gay's routines
            lpYYArg->at.aplFloat = strtod (&lppnLocalVars->lpszNumAccum[uOff], NULL);

            break;

        case PN_NUMTYPE_RAT:
            // This type is used after the numerator has been calculated
            //   and is in mpq_numref (&lpYYArg->at.aplRat).  The denominator is
            //   in lppnLocalVars->NumAccum[uOff] and must be converted.

            // Convert the denominator to a GMP integer
            mpz_init_set_str (mpq_denref (&lpYYArg->at.aplRat), &lppnLocalVars->lpszNumAccum[uOff], 10);

            // If the denominator is zero, ...
            if (IsMpz0 (mpq_denref (&lpYYArg->at.aplRat)))
            {
                int iSgn;

                // Save the sign of the numerator
                iSgn = mpz_sgn (mpq_numref (&lpYYArg->at.aplRat));

                // We no longer need this storage
                Myq_clear (&lpYYArg->at.aplRat);

                // Set to the appropriate-signed infinity
                mpq_set_infsub (&lpYYArg->at.aplRat, iSgn);
            } else
                // Canonicalize the arg
                mpq_canonicalize (&lpYYArg->at.aplRat);
            break;

        case PN_NUMTYPE_HC2:
        case PN_NUMTYPE_HC4:
        case PN_NUMTYPE_HC8:
            break;

        defstop
            break;
    } // End SWITCH
} // End PN_NumCalc


//***************************************************************************
//  $PN_ChrAcc
//
//  Append a new char to the internal buffer
//***************************************************************************

void PN_ChrAcc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     char          chCur)               // The next char

{
    // Accumulate this char
    lppnLocalVars->lpszAlphaInt[lppnLocalVars->uAlpAcc++] = chCur;
} // End PN_ChrAcc


//***************************************************************************
//  $PN_MakeBasePoint
//
//  Merge the base and alphaint part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeBasePoint
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYBase,            // The base part
     LPPN_YYSTYPE  lpYYAlphaInt)        // The AlphaInt part

{
    UINT     uLen,                      // Length of lpszAlphaInt
             uAcc;                      // Loop counter
    LPCHAR   lpszAlphaInt;              // Ptr to next char in lpszAlphaInt
    APLINT   aplIntBase,                // Base as an integer
             aplIntPowBase,             // Base as successive powers
             aplIntTmp,                 // Temporary integer
             aplIntAcc;                 // Temporary accumulator
    APLFLOAT aplFltBase,                // Base as a  float
             aplFltPowBase,             // Base as successive powers
             aplFltAcc,                 // Temporary accumulator
             aplAccHC2Real,             // Accumulator real part
             aplAccHC2Imag,             // ...         imag ...
             aplBaseHC2Real,            // Base real part
             aplBaseHC2Imag,            // ...  imag ...
             aplPowBaseHC2Real,         // Base as successive powers real part
             aplPowBaseHC2Imag;         // ...                       imag ...
    UBOOL    bRet = TRUE;               // TRUE iff the result is valid
    UCHAR    chCur;                     // Temporary character

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Get the string length
    uLen = lppnLocalVars->uAlpAcc;

    // Get the ptr to the end of the AlphaInt string
    lpszAlphaInt = &lppnLocalVars->lpszAlphaInt[uLen - 1];

    // Split cases based upon the numeric type of the base
    switch (lpYYBase->chType)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // Get the base value
            aplIntBase = lpYYBase->at.aplInteger;

            // Initialize the accumulator and base power
            aplIntAcc = 0;
            aplIntPowBase = 1;

            // Loop through the AlphaInt arg
            for (uAcc = 0; bRet && uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base
                aplIntTmp = _imul64 (chCur, aplIntPowBase, &bRet);
                if (!bRet)
                    break;

                // Accumulate
                aplIntAcc = _iadd64 (aplIntAcc, aplIntTmp, &bRet);
                if (!bRet)
                    break;

                // Shift over the power base
                aplIntPowBase = _imul64 (aplIntPowBase, aplIntBase, &bRet);

                // Ignore spurious overflow on last digit
                bRet |= (uAcc EQ (uLen - 1));

////////////////if (!bRet)          // Not needed as we're at the end of the FOR loop
////////////////    break;
            } // End FOR

            if (bRet)
            {
                // Save the result
                lpYYBase->at.aplInteger = aplIntAcc;

                break;
            } // End IF

            // Convert the base value to Rational
            mpq_init_set_sx (&lpYYBase->at.aplRat, lpYYBase->at.aplInteger, 1);

            // Get the ptr to the end of the AlphaInt string
            lpszAlphaInt = &lppnLocalVars->lpszAlphaInt[uLen - 1];

            // Change the type to RAT
            lpYYBase->chType = PN_NUMTYPE_RAT;

            // Fall through to the rational case

        case PN_NUMTYPE_RAT:
        {
            APLRAT aplRatBase    = {0},
                   aplRatPowBase = {0},
                   aplRatTmp     = {0};

            // Initialize and set the base value
            mpq_init_set (&aplRatBase, &lpYYBase->at.aplRat);

            // Initialize the accumulator and base power
            mpq_set_ui (&lpYYBase->at.aplRat, 0, 1);
            mpq_init_set_ui (&aplRatPowBase, 1, 1);
            mpq_init (&aplRatTmp);

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                mpq_set_ui (&aplRatTmp, chCur, 1);
                mpq_mul (&aplRatTmp, &aplRatPowBase, &aplRatTmp);
                mpq_add (&lpYYBase->at.aplRat, &lpYYBase->at.aplRat, &aplRatTmp);
                mpq_canonicalize (&lpYYBase->at.aplRat);

                // Shift over the power base
                mpq_mul (&aplRatPowBase, &aplRatPowBase, &aplRatBase);
            } // End FOR

            // We no longer need this storage
            Myq_clear (&aplRatTmp);
            Myq_clear (&aplRatPowBase);
            Myq_clear (&aplRatBase);

            break;
        } // End PN_NUMTYPE_RAT

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpBase    = {0},
                   aplVfpPowBase = {0},
                   aplVfpTmp     = {0};

            // Initialize and set the base value
            mpfr_init_copy (&aplVfpBase, &lpYYBase->at.aplVfp);

            // Initialize the accumulator and base power
            mpfr_set_ui (&lpYYBase->at.aplVfp, 0, MPFR_RNDN);
            mpfr_init_set_ui (&aplVfpPowBase, 1, MPFR_RNDN);
            mpfr_init0 (&aplVfpTmp);

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                mpfr_set_ui (&aplVfpTmp, chCur, MPFR_RNDN);
                mpfr_mul (&aplVfpTmp, &aplVfpPowBase, &aplVfpTmp, MPFR_RNDN);
                mpfr_add (&lpYYBase->at.aplVfp, &lpYYBase->at.aplVfp, &aplVfpTmp, MPFR_RNDN);

                // Shift over the power base
                mpfr_mul (&aplVfpPowBase, &aplVfpPowBase, &aplVfpBase, MPFR_RNDN);
            } // End FOR

            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&aplVfpPowBase);
            Myf_clear (&aplVfpBase);

            break;
        } // End PN_NUMTYPE_VFP

        case PN_NUMTYPE_FLT:
            // Get the base value
            aplFltBase = lpYYBase->at.aplFloat;

            // Initialize the accumulator and base power
            aplFltAcc = 0;
            aplFltPowBase = 1;

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                aplFltAcc += chCur * aplFltPowBase;

                // Shift over the power base
                aplFltPowBase *= aplFltBase;
            } // End FOR

            // Save the result
            lpYYBase->at.aplFloat = aplFltAcc;

            break;

        case PN_NUMTYPE_HC2:
            // Get the base value
            aplBaseHC2Real = lpYYBase->at.aplHC2.Real;
            aplBaseHC2Imag = lpYYBase->at.aplHC2.Imag;

            // Initialize the accumulator and base power
            aplAccHC2Real =
            aplAccHC2Imag = 0;
            aplPowBaseHC2Real = 1;
            aplPowBaseHC2Imag = 0;

            // Loop through the AlphaInt arg
            for (uAcc = 0; uAcc < uLen; uAcc++)
            {
                // Get the next char as lowercase
                chCur = (UCHAR) CharLower ((LPCHAR) (*lpszAlphaInt--));

                // Convert to a number
                if ('0' <= chCur
                 &&        chCur <= '9')
                    chCur -= '0';
                else
                if ('a' <= chCur
                 &&        chCur <= 'z')
                    chCur -= 'a' - 10;

                // Times the power base and accumulate
                aplAccHC2Real += chCur * aplPowBaseHC2Real;
                aplAccHC2Imag += chCur * aplPowBaseHC2Imag;

                // Shift over the power base
                aplPowBaseHC2Real = aplBaseHC2Real * aplPowBaseHC2Real - aplBaseHC2Imag * aplPowBaseHC2Imag;
                aplPowBaseHC2Imag = aplBaseHC2Imag * aplPowBaseHC2Real + aplBaseHC2Real * aplPowBaseHC2Imag;
            } // End FOR

            // Save the result
            lpYYBase->at.aplHC2.Real = aplAccHC2Real;
            lpYYBase->at.aplHC2.Imag = aplAccHC2Imag;

            break;

        case PN_NUMTYPE_HC4:
        case PN_NUMTYPE_HC8:
            // ***FINISHME***

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYBase;
} // End PN_MakeBasePoint


//***************************************************************************
//  $PN_MakeEulerPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeEulerPoint
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYMultiplier,      // The multiplier part
     LPPN_YYSTYPE  lpYYExponent)        // The exponent part

{
    PNNUMTYPE pnTypeRes;

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Promote the multiplier and exponent to a common format
    pnTypeRes = aNumTypePromote[lpYYMultiplier->chType][lpYYExponent->chType];

    // If the result is BOOL/INT, ...
    if (pnTypeRes EQ PN_NUMTYPE_BOOL
     || pnTypeRes EQ PN_NUMTYPE_INT)
        // Make it FLT
        pnTypeRes = PN_NUMTYPE_FLT;

    // If the result is RAT, ...
    if (pnTypeRes EQ PN_NUMTYPE_RAT)
        // Make it VFP
        pnTypeRes = PN_NUMTYPE_VFP;

    // If the multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier);

    // If the exponent must be promoted, ...
    if (pnTypeRes NE lpYYExponent->chType
     && aNumTypeAction[lpYYExponent->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYExponent->chType  ][pnTypeRes]) (lpYYExponent);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYExponent->chType   = pnTypeRes;

    // Split cases based upon the numeric type of the result
    switch (pnTypeRes)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // The result is Multiplier x (*1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatE, (APLFLOAT) lpYYExponent->at.aplInteger);

            break;

        case PN_NUMTYPE_FLT:
            // The result is Multiplier x (*1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatE, lpYYExponent->at.aplFloat);

            break;

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpTmp = {0};

            // Initialize the temp array
            mpfr_init0 (&aplVfpTmp);

            // The result is Multiplier x (*1) * Exponent
            mpfr_pow (&aplVfpTmp, &GetMemPTD ()->mpfrE, &lpYYExponent->at.aplVfp, MPFR_RNDN);

            // Accumulate in the multiplier
            mpfr_mul (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &aplVfpTmp, MPFR_RNDN);

            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&lpYYExponent->at.aplVfp);

            break;
        } // End PN_NUMTYPE_VFP

        case PN_NUMTYPE_HC2:
        case PN_NUMTYPE_HC4:
        case PN_NUMTYPE_HC8:
            // ***FINISHME***

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYMultiplier;
} // End PN_MakeEulerPoint


//***************************************************************************
//  $PN_MakeExpPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeExpPoint
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYArg,             // The mantissa part
     LPPN_YYSTYPE  lpYYExponent)        // The exponent part

{
    UINT uOff;                          // Starting offset

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Terminate the (Exponent) argument
    PN_NumAcc (lppnLocalVars, '\0');

    // Initialize the starting offset
    uOff = lpYYArg->uNumOff;

    // Insert the exponent separator
    lppnLocalVars->lpszNumAccum[lpYYExponent->uNumOff - 1] = 'e';

    // Use David Gay's routines
    lpYYArg->at.aplFloat = strtod (&lppnLocalVars->lpszNumAccum[uOff], NULL);

    // Change the type to float
    lpYYArg->chType = PN_NUMTYPE_FLT;

    return lpYYArg;
} // End PN_MakeExpPoint


//***************************************************************************
//  $PN_MakePiPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakePiPoint
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYMultiplier,      // The multiplier part
     LPPN_YYSTYPE  lpYYExponent)        // The exponent part

{
    PNNUMTYPE pnTypeRes;

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Promote the multiplier and exponent to a common format
    pnTypeRes = aNumTypePromote[lpYYMultiplier->chType][lpYYExponent->chType];

    // If the result is BOOL/INT, ...
    if (pnTypeRes EQ PN_NUMTYPE_BOOL
     || pnTypeRes EQ PN_NUMTYPE_INT)
        // Make it FLT
        pnTypeRes = PN_NUMTYPE_FLT;

    // If the result is RAT, ...
    if (pnTypeRes EQ PN_NUMTYPE_RAT)
        // Make it VFP
        pnTypeRes = PN_NUMTYPE_VFP;

    // If the multiplier must be promoted, ...
    if (pnTypeRes NE lpYYMultiplier->chType
     && aNumTypeAction[lpYYMultiplier->chType][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYMultiplier->chType][pnTypeRes]) (lpYYMultiplier);

    // If the exponent must be promoted, ...
    if (pnTypeRes NE lpYYExponent->chType
     && aNumTypeAction[lpYYExponent->chType  ][pnTypeRes] NE NULL)
        (*aNumTypeAction[lpYYExponent->chType  ][pnTypeRes]) (lpYYExponent);

    // Set the result type
    lpYYMultiplier->chType =
    lpYYExponent->chType   = pnTypeRes;

    // Split cases based upon the numeric type
    switch (pnTypeRes)
    {
        case PN_NUMTYPE_BOOL:
        case PN_NUMTYPE_INT:
            // The result is Multiplier x (o1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatPi, (APLFLOAT) lpYYExponent->at.aplInteger);

            break;

        case PN_NUMTYPE_FLT:
            // The result is Multiplier x (o1) * Exponent
            lpYYMultiplier->at.aplFloat *= pow (FloatPi, lpYYExponent->at.aplFloat);

            break;

        case PN_NUMTYPE_VFP:
        {
            APLVFP aplVfpTmp = {0};

            // Initialize the temp array
            mpfr_init0 (&aplVfpTmp);

            // The result is Multiplier x (o1) * Exponent
            mpfr_pow (&aplVfpTmp, &GetMemPTD ()->mpfrPi, &lpYYExponent->at.aplVfp, MPFR_RNDN);

            // Accumulate in the multiplier
            mpfr_mul (&lpYYMultiplier->at.aplVfp, &lpYYMultiplier->at.aplVfp, &aplVfpTmp, MPFR_RNDN);

            // We no longer need this storage
            Myf_clear (&aplVfpTmp);
            Myf_clear (&lpYYExponent->at.aplVfp);

            break;
        } // End PN_NUMTYPE_VFP

        case PN_NUMTYPE_HC2:
        case PN_NUMTYPE_HC4:
        case PN_NUMTYPE_HC8:
            // ***FINISHME***

            break;

        defstop
            break;
    } // End SWITCH

    return lpYYMultiplier;
} // End PN_MakePiPoint


//***************************************************************************
//  $PN_MakeVfpPoint
//
//  Merge the multiplier and exponential part to form a number
//***************************************************************************

LPPN_YYSTYPE PN_MakeVfpPoint
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYArg,             // The mantissa part
     LPPN_YYSTYPE  lpYYExponent)        // The exponent part (may be NULL)

{
    UINT      uOff,                     // Starting offset
              uDig,                     // # significant digits
              uArg,                     // Loop counter
              uLen;                     // # accumulated chars
    mp_prec_t uOldPrec,                 // Old precision
              uNewPrec;                 // # significant bits

    // If there's been a YYERROR, ...
    if (lppnLocalVars->bYYERROR)
        return NULL;

    // Initialize the starting offset
    uOff = lpYYArg->uNumOff;

    // Get # accumulated chars
    uLen = lstrlen (&lppnLocalVars->lpszNumAccum[uOff]);

    // Calculate the # significant digits in the number
    for (uDig = uArg = 0; uArg < uLen; uArg++)
    if (isdigit (lppnLocalVars->lpszNumAccum[uOff + uArg]))
        uDig++;

    // Convert the # significant digits to # significant bits
    //   via the formula 1 + floor (log2 (10^N))
    //                 = 1 + floor (N x log2 (10))
    //   where log2 (10) = (ln (10)) / (ln (2))
    //                   = M_LN10 / M_LN2
    uNewPrec = 1 + (mp_prec_t) floor (uDig * M_LN10 / M_LN2);

    // If present, ...
    if (lpYYExponent)
        // Insert the exponent separator
        lppnLocalVars->lpszNumAccum[lpYYExponent->uNumOff - 1] = 'e';

    // Get and save the current precision
    uOldPrec = mpfr_get_default_prec ();

    // Set the default precision to the larger ...
    mpfr_set_default_prec (max (uNewPrec, uOldPrec));

    // Use MPFR routine
    mpfr_init_set_str (&lpYYArg->at.aplVfp, &lppnLocalVars->lpszNumAccum[uOff], 10, MPFR_RNDN);

    // Restore the default precision
    mpfr_set_default_prec (uOldPrec);

    // Change the type to VFP
    lpYYArg->chType = PN_NUMTYPE_VFP;

    return lpYYArg;
} // End PN_MakeVfpPoint


//***************************************************************************
//  $pn_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

int pn_yylex
    (LPPN_YYSTYPE  lpYYLval,            // Ptr to lval
     LPPNLOCALVARS lppnLocalVars)       // Ptr to local pnLocalVars

{
    // Check for EOL
    if (lppnLocalVars->uNumCur EQ lppnLocalVars->uNumLen)
        return '\0';

    // Get the next char
    lpYYLval->chCur = lppnLocalVars->lpszStart[lppnLocalVars->uNumCur++];

    // If the character is 'x' and
    //   is the last in the sequence, ...
    if (lpYYLval->chCur EQ 'x'
     && lppnLocalVars->uNumCur EQ lppnLocalVars->uNumLen)
        return EXT;

    // If the character is overbar, ...
    if (lpYYLval->chCur EQ OVERBAR1)
        return OVR;

    // If the character is infinity, ...
    if (lpYYLval->chCur EQ INFINITY1)
        return INF;

    // Return it
    return lpYYLval->chCur;
} // End pn_yylex


//***************************************************************************
//  $pn_yyerror
//
//  Error callback from Bison
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- pn_yyerror"
#else
#define APPEND_NAME
#endif

void pn_yyerror                     // Called for Bison syntax error
    (LPPNLOCALVARS lppnLocalVars,   // Ptr to local pnLocalVars
     LPCHAR        s)               // Ptr to error msg

{
    char    szTemp[1024];
    LPWCHAR lpwszTemp;

    DbgMsg (s);

    // Check for SYNTAX ERROR
#define ERR     "syntax error"
    if (lstrcpyn (szTemp, s, sizeof (ERR)),     // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
        lpwszTemp = ERRMSG_SYNTAX_ERROR APPEND_NAME;
    else
    // Check for NONCE ERROR
#define ERR     "nonce error"
    if (lstrcpyn (szTemp, s, sizeof (ERR)),     // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
    {
        lpwszTemp = ERRMSG_NONCE_ERROR APPEND_NAME;

        // Back off by one so the caret points to the symbol in error
        lppnLocalVars->uNumCur--;
    } else
    // Check for WS FULL
#define ERR     WSFULL
    if (lstrcpyn (szTemp, s, sizeof (ERR)),     // Note: Terminates the string, too
        lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
        lpwszTemp = ERRMSG_WS_FULL APPEND_NAME;

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
} // End pn_yyerror
#undef  APPEND_NAME


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
    va_list  vl;
    APLU3264 i1,
             i2,
             i3;
    static  char szTemp[256] = {'\0'};

    va_start (vl, lpszFmt);

    // Bison uses no more than three arguments.
    // Note we must grab them separately this way
    //   as using va_arg in the argument list to
    //   wsprintf pushes the arguments in reverse
    //   order.
    i1 = va_arg (vl, APLU3264);
    i2 = va_arg (vl, APLU3264);
    i3 = va_arg (vl, APLU3264);

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
    } // End IF/ELSE
#endif
} // End pn_yyfprintf


//***************************************************************************
//  End of File: pn_parse.y
//***************************************************************************
