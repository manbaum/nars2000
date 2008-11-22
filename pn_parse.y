//***************************************************************************
//  NARS2000 -- Parser Grammar for XXX Point Notation
//***************************************************************************

//***************************************************************************
//  Parse a line of XXX Point Notation characters.
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
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pn_parse.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

////#define YYLEX_DEBUG
////#define YYFPRINTF_DEBUG

#ifdef DEBUG
#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF               pn_yyfprintf
#endif

#define    YYSTYPE     PN_YYSTYPE
#define  LPYYSTYPE   LPPN_YYSTYPE
#define tagYYSTYPE  tagPN_YYSTYPE

// The following #defines are needed to allow multiple parses
//   to coexist in the same file
#define yy_symbol_print         pn_yy_symbol_print
#define yy_symbol_value_print   pn_yy_symbol_value_print
#define yy_reduce_print         pn_yy_reduce_print
#define yydestruct              pn_yydestruct

////#define DbgMsgWP(a)         DbgMsgW(a)
////#define DbgMsgWP(a)         DbgMsgW(a); DbgBrk ()
////#define DbgMsgWP(a)         DbgMsgW(a)
    #define DbgMsgWP(a)

%}

%pure-parser
%name-prefix="pn_yy"
%parse-param {LPPNLOCALVARS lppnLocalVars}
%lex-param   {LPPNLOCALVARS lppnLocalVars}

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
    | '-'     Digit                 {DbgMsgWP (L"%%Integer:  '-' Digit");
                                     // Mark starting offset
                                     $1.uNumOff = lppnLocalVars->uNumAcc;

                                     // Accumulate the negative sign
                                     PN_NumAcc (lppnLocalVars, '-');

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
                                     $2.uNumOff = lppnLocalVars->uNumAcc;

                                     // Accumulate the decimal point
                                     PN_NumAcc (lppnLocalVars, '.');

                                     // Accumulate the digit
                                     PN_NumAcc (lppnLocalVars, $2.chCur);

                                     $$ = $2;
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
                                     PN_NumCalc (lppnLocalVars, &$1);

                                     $$ = $1;
                                    }
    | Integer '.'                   {DbgMsgWP (L"%%DecPoint:  Integer '.'");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as integer
                                     $1.chType = PN_NUMTYPE_INT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1);

                                     $$ = $1;
                                    }
    | Decimal                       {DbgMsgWP (L"%%DecPoint:  Decimal");
                                     // Terminate the argument
                                     PN_NumAcc (lppnLocalVars, '\0');

                                     // Mark the result as float
                                     $1.chType = PN_NUMTYPE_FLT;

                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1);

                                     $$ = $1;
                                    }
    ;

PiPoint:
      DecPoint 'p' DecPoint         {DbgMsgWP (L"%%PiPoint:  DecPoint 'p' DecPoint");
                                     $$ = *PN_MakePiPoint (lppnLocalVars, &$1, &$3);
                                    }
    | DecPoint 'p' ExpPoint         {DbgMsgWP (L"%%PiPoint:  DecPoint 'p' ExpPoint");
                                     $$ = *PN_MakePiPoint (lppnLocalVars, &$1, &$3);
                                    }
    | ExpPoint 'p' DecPoint         {DbgMsgWP (L"%%PiPoint:  ExpPoint 'p' DecPoint");
                                     $$ = *PN_MakePiPoint (lppnLocalVars, &$1, &$3);
                                    }
    | ExpPoint 'p' ExpPoint         {DbgMsgWP (L"%%PiPoint:  ExpPoint 'p' ExpPoint");
                                     $$ = *PN_MakePiPoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

ExpPoint:
      DecPoint 'e' DecPoint         {DbgMsgWP (L"%%ExpPoint:  DecPoint 'e' DecPoint");
                                     $$ = *PN_MakeExpPoint (lppnLocalVars, &$1, &$3);
                                    }
    | DecPoint 'E' DecPoint         {DbgMsgWP (L"%%ExpPoint:  DecPoint 'E' DecPoint");
                                     $$ = *PN_MakeExpPoint (lppnLocalVars, &$1, &$3);
                                    }
    ;

EulerPoint:
      DecPoint 'x' DecPoint         {DbgMsgWP (L"%%EulerPoint:  DecPoint 'x' DecPoint");
                                     $$ = *PN_MakeEulerPoint (lppnLocalVars, &$1, &$3);
                                    }
    | DecPoint 'x' ExpPoint         {DbgMsgWP (L"%%EulerPoint:  DecPoint 'x' ExpPoint");
                                     $$ = *PN_MakeEulerPoint (lppnLocalVars, &$1, &$3);
                                    }
    | ExpPoint 'x' DecPoint         {DbgMsgWP (L"%%EulerPoint:  ExpPoint 'x' DecPoint");
                                     $$ = *PN_MakeEulerPoint (lppnLocalVars, &$1, &$3);
                                    }
    | ExpPoint 'x' ExpPoint         {DbgMsgWP (L"%%EulerPoint:  ExpPoint 'x' ExpPoint");
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
    ;

Number:
      BasePoint                     {DbgMsgWP (L"%%Number:  BasePoint");
                                     lppnLocalVars->aplInteger = $1.aplInteger;
                                     lppnLocalVars->aplFloat   = $1.aplFloat;
                                     lppnLocalVars->chType     = $1.chType;
                                    }
    | DecPoint                      {DbgMsgWP (L"%%Number:  DecPoint");
                                     // Calculate the number
                                     PN_NumCalc (lppnLocalVars, &$1);

                                     lppnLocalVars->aplInteger = $1.aplInteger;
                                     lppnLocalVars->aplFloat   = $1.aplFloat;
                                     lppnLocalVars->chType     = $1.chType;
                                    }
    | EulerPoint                    {DbgMsgWP (L"%%Number:  EulerPoint");
                                     lppnLocalVars->aplInteger = $1.aplInteger;
                                     lppnLocalVars->aplFloat   = $1.aplFloat;
                                     lppnLocalVars->chType     = $1.chType;
                                    }
    | ExpPoint                      {DbgMsgWP (L"%%Number:  ExpPoint");
                                     lppnLocalVars->aplInteger = $1.aplInteger;
                                     lppnLocalVars->aplFloat   = $1.aplFloat;
                                     lppnLocalVars->chType     = $1.chType;
                                    }
    | PiPoint                       {DbgMsgWP (L"%%Number:  PiPoint");
                                     lppnLocalVars->aplInteger = $1.aplInteger;
                                     lppnLocalVars->aplFloat   = $1.aplFloat;
                                     lppnLocalVars->chType     = $1.chType;
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
//***************************************************************************

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

    // Parse the header, check for errors
    //   0 = success
    //   1 = YYABORT or APL error
    //   2 = memory exhausted
    bRet = pn_yyparse (lppnLocalVars) EQ 0;

#if YYDEBUG
    // Disable debugging
    yydebug = FALSE;
#endif
    return bRet;
} // End ParsePointNotation


//***************************************************************************
//  $PN_NumAcc
//
//  Assumulate another digit into the number
//***************************************************************************

void PN_NumAcc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     char          chCur)               // The next digit

{
    lppnLocalVars->lpszNumAccum[lppnLocalVars->uNumAcc++] = chCur;
} // End PN_NumAcc


//***************************************************************************
//  $PN_NumCalc
//
//  Calculate the value of a number
//***************************************************************************

void PN_NumCalc
    (LPPNLOCALVARS lppnLocalVars,       // Ptr to local pnLocalVars
     LPPN_YYSTYPE  lpYYArg)             // Ptr to incoming YYSTYPE

{
    UBOOL  bRet = TRUE,                 // TRUE iff the result is valid
           bSigned;                     // TRUE iff the number is negative
    APLINT aplInteger;                  // Temporary integer
    UINT   uLen,                        // Loop length
           uOff,                        // Starting offset
           uAcc;                        // Loop counter

    // Initialize the starting offset
    uOff = lpYYArg->uNumOff;

    // Split cases based upon the current numeric type
    switch (lpYYArg->chType)
    {
        case PN_NUMTYPE_INT:
            // Initialize the accumulator
            aplInteger = 0;

            // Initialize the loop length
            uLen = lstrlen (&lppnLocalVars->lpszNumAccum[uOff]);

            // Handle negative sign
            uAcc = bSigned = (lppnLocalVars->lpszNumAccum[uOff] EQ '-');

            // Loop through the digits
            for (; bRet && uAcc < uLen; uAcc++)
            {
                aplInteger = _imul64 (aplInteger, 10, &bRet);
                if (bRet)
                    aplInteger = _iadd64 (aplInteger, lppnLocalVars->lpszNumAccum[uOff + uAcc] - '0', &bRet);
            } // End FOR

            if (bRet)
            {
                if (bSigned)
                    // can't overflow as there are more negative than positive numbers
                    aplInteger = -aplInteger;

                // Save in the result
                lpYYArg->aplInteger = aplInteger;

                break;
            } // End IF

            // Change the type to float
            lpYYArg->chType = PN_NUMTYPE_FLT;

            // Fall through to the float case

        case PN_NUMTYPE_FLT:
            // Use David Gay's routines
            lpYYArg->aplFloat = strtod (&lppnLocalVars->lpszNumAccum[uOff], NULL);

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
             aplFltAcc;                 // Temporary accumulator
    UBOOL    bRet = TRUE;               // TRUE iff the result is valid
    UCHAR    chCur;                     // Temporary character

    // Get the string length
    uLen = lppnLocalVars->uAlpAcc;

    // Get the ptr to the end of the AlphaInt string
    lpszAlphaInt = &lppnLocalVars->lpszAlphaInt[uLen - 1];

    // Split cases based upon the numeric type of the base
    switch (lpYYBase->chType)
    {
        case PN_NUMTYPE_INT:
            // Get the base value
            aplIntBase = lpYYBase->aplInteger;

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
////////////////if (!bRet)          // Not needed as we're at the end of the FOR loop
////////////////    break;
            } // End FOR

            if (bRet)
            {
                // Save the result
                lpYYBase->aplInteger = aplIntAcc;

                break;
            } // End IF

            // Change the type to float
            lpYYBase->chType = PN_NUMTYPE_FLT;

            // Convert the base to float
            lpYYBase->aplFloat = (APLFLOAT) lpYYBase->aplInteger;

            // Fall through to the float case

        case PN_NUMTYPE_FLT:
            // Get the base value
            aplFltBase = lpYYBase->aplFloat;

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
            lpYYBase->aplFloat = aplFltAcc;

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
    // If the multiplier is integer, ...
    if (lpYYMultiplier->chType EQ PN_NUMTYPE_INT)
    {
        // Convert to float
        lpYYMultiplier->aplFloat = (APLFLOAT) lpYYMultiplier->aplInteger;

        // Change the type to float
        lpYYMultiplier->chType = PN_NUMTYPE_FLT;
    } // End IF

    // If the exponent is integer, ...
    if (lpYYExponent->chType EQ PN_NUMTYPE_INT)
        // Convert to float
        lpYYExponent->aplFloat = (APLFLOAT) lpYYExponent->aplInteger;

    // The result is Multiplier x (*1) * Exponent
    lpYYMultiplier->aplFloat *= pow (FloatE, lpYYExponent->aplFloat);

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

    // Initialize the starting offset
    uOff = lpYYArg->uNumOff;

    // Insert the exponent separator
    lppnLocalVars->lpszNumAccum[lpYYExponent->uNumOff - 1] = 'e';

    // Use David Gay's routines
    lpYYArg->aplFloat = strtod (&lppnLocalVars->lpszNumAccum[uOff], NULL);

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
    // If the multiplier is integer, ...
    if (lpYYMultiplier->chType EQ PN_NUMTYPE_INT)
    {
        // Convert to float
        lpYYMultiplier->aplFloat = (APLFLOAT) lpYYMultiplier->aplInteger;

        // Change the type to float
        lpYYMultiplier->chType = PN_NUMTYPE_FLT;
    } // End IF

    // If the exponent is integer, ...
    if (lpYYExponent->chType EQ PN_NUMTYPE_INT)
        // Convert to float
        lpYYExponent->aplFloat = (APLFLOAT) lpYYExponent->aplInteger;

    // The result is Multiplier x (o1) * Exponent
    lpYYMultiplier->aplFloat *= pow (FloatPi, lpYYExponent->aplFloat);

    return lpYYMultiplier;
} // End PN_MakePiPoint


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
    {
#define ERR     "memory exhausted"
        lstrcpyn (szTemp, s, sizeof (ERR));     // Note: Terminates the string, too
        if (lstrcmp (szTemp, ERR) EQ 0)
#undef  ERR
            lpwszTemp = ERRMSG_WS_FULL APPEND_NAME;
    } // End IF

    // Save the error message
    ErrorMessageIndirect (lpwszTemp);

    // Set the error char index
    lppnLocalVars->uCharIndex += lppnLocalVars->uNumCur;
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
} // End pn_yyfprintf


//***************************************************************************
//  End of File: pn_parse.y
//***************************************************************************
