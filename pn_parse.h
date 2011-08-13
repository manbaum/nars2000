//***************************************************************************
//  NARS2000 -- Point Notation Parser Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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

typedef enum tagPN_NUMTYPE
{
    PN_NUMTYPE_BOOL,                // 00:  Boolean type
    PN_NUMTYPE_INT,                 // 01:  Integer ...
    PN_NUMTYPE_FLT,                 // 02:  Float   ...
    PN_NUMTYPE_HC2,                 // 03:  Complex ...
    PN_NUMTYPE_HC4,                 // 04:  Quaternion ...
    PN_NUMTYPE_HC8,                 // 05:  Octonion ...
    PN_NUMTYPE_RAT,                 // 06:  RAT ...
    PN_NUMTYPE_VFP,                 // 07:  VFP ...
    PN_NUMTYPE_LENGTH               // 08:  Length
} PNNUMTYPE, *LPPNNUMTYPE;

typedef struct tagPN_YYSTYPE        // YYSTYPE for Point Notation parser
{
    ALLTYPES  at;                   // 00:  All datatypes as a union (64 bytes)
    UINT      uNumOff;              // 40:  Starting offset into lpszNumAccum
    PNNUMTYPE chType;               // 44:  The numeric type (see PNNUMTYPE)
    UCHAR     chCur,                // 48:  A char for the accumulator
              chAlign[3];           // 49:  For alignment
                                    // 4C:  Length
} PN_YYSTYPE, *LPPN_YYSTYPE;        // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1


typedef struct tagPNLOCALVARS       // Point Notation Local Vars
{
    LPCHAR    lpszStart;            // 00:  Ptr to start of char stream
    UINT      uNumLen,              // 04:  # chars in lpszStart
              uNumCur,              // 08:  Current index into lpszStart
              uAlpAcc,              // 0C:  Current index into lpszAlphaInt
              uNumAcc,              // 10:  Current index into lpszNumAccum
              uCharIndex;           // 14:  Starting character index
    LPCHAR    lpszAlphaInt,         // 18:  Ptr to AlphaInt accumulator
              lpszNumAccum;         // 1C:  Ptr to numeric accumulator (in case of overflow)
    ALLTYPES  at;                   // 20:  All datatypes as a union (64 bytes)
    PNNUMTYPE chType;               // 60:  The numeric type (see PNNUMTYPE)
    UBOOL     bYYERROR;             // 64:  TRUE iff there's been a YYERROR
                                    // 68:  Length
} PNLOCALVARS, *LPPNLOCALVARS;


//***************************************************************************
//  End of File: pn_parse.h
//***************************************************************************
