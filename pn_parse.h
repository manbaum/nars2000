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
    PN_NUMTYPE_INT,                 // 00:  Integer type
    PN_NUMTYPE_FLT,                 // 01:  Float   ...
    PN_NUMTYPE_HC2,                 // 02:  Complex ...
    PN_NUMTYPE_HC4,                 // 03:  Quaternion ...
    PN_NUMTYPE_HC8,                 // 04:  Octonion ...
    PN_NUMTYPE_RAT,                 // 05:  Rational ...
    PN_NUMTYPE_EXT,                 // 06:  Extended ...
                                    // 07:  Length
} PNNUMTYPE, *LPPNNUMTYPE;

typedef struct tagPN_YYSTYPE        // YYSTYPE for Point Notation parser
{
    APLINT   aplInteger,            // 00:  Real number as an integer            (8 bytes)
             aplRationalNum,        // 08:  Rational number, numerator   part    (8 bytes)
             aplRationalDen;        // 10:  ...              denominator part    (8 bytes)
    APLFLOAT aplFloat,              // 18:  Real number as a float               (8 bytes)
             aplHC2Real,            // 20:  Complex number,    real      part    (8 bytes)
             aplHC2Imag,            // 28:  ...                imaginary part    (8 bytes)
             aplHC4Real,            // 30:  Quaternion number, real      part    (8 bytes)
             aplHC4Imag1,           // 38:  ...                imaginary part #1 (8 bytes)
             aplHC4Imag2,           // 40:  ...                imaginary part #2 (8 bytes)
             aplHC4Imag3,           // 48:  ...                imaginary part #3 (8 bytes)
             aplHC8Real,            // 50:  Octonion number,   real      part    (8 bytes)
             aplHC8Imag1,           // 58:  ...                imaginary part #1 (8 bytes)
             aplHC8Imag2,           // 60:  ...                imaginary part #2 (8 bytes)
             aplHC8Imag3,           // 68:  ...                imaginary part #3 (8 bytes)
             aplHC8Imag4,           // 70:  ...                imaginary part #4 (8 bytes)
             aplHC8Imag5,           // 78:  ...                imaginary part #5 (8 bytes)
             aplHC8Imag6,           // 80:  ...                imaginary part #6 (8 bytes)
             aplHC8Imag7;           // 88:  ...                imaginary part #7 (8 bytes)
    UINT     uNumOff;               // 90:  Starting offset into lpszNumAccum
    UCHAR    chType;                // 94:  The numeric type (see PN_NUMTYPE)
    UCHAR    chCur,                 // 95:  A char for the accumulator
             chAlign[3];            // 96:  For alignment
                                    // 98:  Length
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
    APLINT   aplInteger,            // 20:  Real number as an integer            (8 bytes)
             aplRationalNum,        // 28:  Rational number, numerator   part    (8 bytes)
             aplRationalDen;        // 30:  ...              denominator part    (8 bytes)
    APLFLOAT aplFloat,              // 38:  Real number as a float               (8 bytes)
             aplHC2Real,            // 40:  Complex number,    real      part    (8 bytes)
             aplHC2Imag,            // 48:  ...                imaginary part    (8 bytes)
             aplHC4Real,            // 50:  Quaternion number, real      part    (8 bytes)
             aplHC4Imag1,           // 58:  ...                imaginary part #1 (8 bytes)
             aplHC4Imag2,           // 60:  ...                imaginary part #2 (8 bytes)
             aplHC4Imag3,           // 68:  ...                imaginary part #3 (8 bytes)
             aplHC8Real,            // 70:  Octonion number,   real      part    (8 bytes)
             aplHC8Imag1,           // 78:  ...                imaginary part #1 (8 bytes)
             aplHC8Imag2,           // 80:  ...                imaginary part #2 (8 bytes)
             aplHC8Imag3,           // 88:  ...                imaginary part #3 (8 bytes)
             aplHC8Imag4,           // 90:  ...                imaginary part #4 (8 bytes)
             aplHC8Imag5,           // 98:  ...                imaginary part #5 (8 bytes)
             aplHC8Imag6,           // A0:  ...                imaginary part #6 (8 bytes)
             aplHC8Imag7;           // A8:  ...                imaginary part #7 (8 bytes)
    UCHAR    chType,                // B0:  The numeric type (see PN_NUMTYPE)
             chAlign[3];            // B1:  For alignment
                                    // B4:  Length
} PNLOCALVARS, *LPPNLOCALVARS;


//***************************************************************************
//  End of File: pn_parse.h
//***************************************************************************
