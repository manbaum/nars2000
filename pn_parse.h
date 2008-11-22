//***************************************************************************
//  NARS2000 -- Point Notation Parser Header
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

typedef enum tagPN_NUMTYPE
{
    PN_NUMTYPE_INT,                 // Integer type
    PN_NUMTYPE_FLT,                 // Float type
};

typedef struct tagPN_YYSTYPE        // YYSTYPE for Point Notation parser
{
    APLINT   aplInteger;            // 00:  The integer (8 bytes)
    APLFLOAT aplFloat;              // 08:  The float   (8 bytes)
    UINT     uNumOff;               // 10:  Starting offset into lpszNumAccum
    UCHAR    chType;                // 14:  The numeric type (see PN_NUMTYPE)
    UCHAR    chCur;                 // 15:  A char for the accumulator
                                    // 16:  Length
} PN_YYSTYPE, *LPPN_YYSTYPE;        // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1


typedef struct tagPNLOCALVARS       // Point Notation Local Vars
{
    LPCHAR   lpszStart;             // 00:  Ptr to start of char stream
    UINT     uNumLen,               // 04:  # chars in lpszStart
             uNumCur,               // 08:  Current index into lpszStart
             uAlpAcc,               // 0C:  Current index into lpszAlphaInt
             uNumAcc,               // 10:  Current index into lpszNumAccum
             uCharIndex;            // 14:  Starting character index
    LPCHAR   lpszAlphaInt,          // 18:  Ptr to AlphaInt accumulator
             lpszNumAccum;          // 1C:  Ptr to numeric accumulator (in case of overflow)
    APLINT   aplInteger;            // 20:  The integer (8 bytes)
    APLFLOAT aplFloat;              // 28:  The float   (8 bytes)
    UCHAR    chType,                // 30:  The numeric type (see PN_NUMTYPE)
             chAlign;               // 31:  For alignment
                                    // 32:  Length
} PNLOCALVARS, *LPPNLOCALVARS;


//***************************************************************************
//  End of File: pn_parse.h
//***************************************************************************
