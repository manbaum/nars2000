//***************************************************************************
//  NARS2000 -- Point Notation Parser Header
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

typedef enum tagPN_NUMTYPE
{
    // N.B.  The following values *MUST* be in order I before F, for all types
    //       so we can do arithmetic on them as in PN_NUMTYPE_INT + 1 to get
    //       the corresponding _FLT (or HCxF) type.
    PN_NUMTYPE_BOOL,                    // 00:  Boolean type
    PN_NUMTYPE_INT,                     // 01:  Integer ...
    PN_NUMTYPE_FLT,                     // 02:  Float   ...
    PN_NUMTYPE_RAT,                     // 03:  RAT     ...
    PN_NUMTYPE_VFP,                     // 04:  VFP     ...
    PN_NUMTYPE_HC2I,                    // 05:  Complex    INT coefficients
    PN_NUMTYPE_HC2F,                    // 06:  ...        FLT ...
    PN_NUMTYPE_HC2R,                    // 07:  ...        RAT ...
    PN_NUMTYPE_HC2V,                    // 08:  ...        VFP ...
    PN_NUMTYPE_HC4I,                    // 09:  Quaternion INT ...
    PN_NUMTYPE_HC4F,                    // 0A:  ...        FLT ...
    PN_NUMTYPE_HC4R,                    // 0B:  ...        RAT ...
    PN_NUMTYPE_HC4V,                    // 0C:  ...        VFP ...
    PN_NUMTYPE_HC8I,                    // 0D:  Octonion   INT ...
    PN_NUMTYPE_HC8F,                    // 0E:  ...        FLT ...
    PN_NUMTYPE_HC8R,                    // 0F:  ...        RAT ...
    PN_NUMTYPE_HC8V,                    // 10:  ...        VFP ...
    PN_NUMTYPE_ARB,                     // 11:  Ball Arithmetic Real
    PN_NUMTYPE_BA2F,                    // 12:  ...             Complex
    PN_NUMTYPE_BA4F,                    // 13:  ...             Quaternion
    PN_NUMTYPE_BA8F,                    // 14:  ...             Octonion
    PN_NUMTYPE_INIT,                    // 15:  Initial value (used in <aNumTypePromote>)
    PN_NUMTYPE_LENGTH,                  // 16:  Length
                                        //      *MUST* be the last non-error entry

    PN_NUMTYPE_ERR = -1,                // -1:  Error

    PN_NUMTYPE_HC1I = PN_NUMTYPE_INT ,  // To simplify common macros
    PN_NUMTYPE_HC1F = PN_NUMTYPE_FLT ,  // ...
    PN_NUMTYPE_HC1R = PN_NUMTYPE_RAT ,  // ...
    PN_NUMTYPE_HC1V = PN_NUMTYPE_VFP ,  // ...
    PN_NUMTYPE_BA1F = PN_NUMTYPE_ARB ,  // ...
} PNNUMTYPE, *LPPNNUMTYPE;

// N.B.:  Whenever changing the above tagPN_NUMTYPE enum,
//   be sure to make a corresponding change to
//   <PN_MAT> in <pn_proc.c>,
//   chComType++ in <PN_VectorRes> in <pn_proc.c>.


#define IsPnNumTypeBool(a)          \
    ((a) EQ PN_NUMTYPE_BOOL)

#define IsPnNumTypeInt(a)           \
    ((a) EQ PN_NUMTYPE_BOOL         \
  || (a) EQ PN_NUMTYPE_INT          \
  || (a) EQ PN_NUMTYPE_HC2I         \
  || (a) EQ PN_NUMTYPE_HC4I         \
  || (a) EQ PN_NUMTYPE_HC8I         \
    )

#define IsPnNumTypeFlt(a)           \
    ((a) EQ PN_NUMTYPE_FLT          \
  || (a) EQ PN_NUMTYPE_HC2F         \
  || (a) EQ PN_NUMTYPE_HC4F         \
  || (a) EQ PN_NUMTYPE_HC8F         \
    )

#define IsPnNumTypeRat(a)           \
    ((a) EQ PN_NUMTYPE_RAT          \
  || (a) EQ PN_NUMTYPE_HC2R         \
  || (a) EQ PN_NUMTYPE_HC4R         \
  || (a) EQ PN_NUMTYPE_HC8R         \
    )

#define IsPnNumTypeVfp(a)           \
    ((a) EQ PN_NUMTYPE_VFP          \
  || (a) EQ PN_NUMTYPE_HC2V         \
  || (a) EQ PN_NUMTYPE_HC4V         \
  || (a) EQ PN_NUMTYPE_HC8V         \
    )

#define IsPnNumTypeBA(a)            \
    ((a) EQ PN_NUMTYPE_BA1F         \
  || (a) EQ PN_NUMTYPE_BA2F         \
  || (a) EQ PN_NUMTYPE_BA4F         \
  || (a) EQ PN_NUMTYPE_BA8F         \
    )

typedef struct tagPN_YYSTYPE        // YYSTYPE for Point Notation parser
{
    ALLTYPES  at;                   // 00:  All datatypes as a union (64 bytes)
    size_t    uNumAcc;              // 40:  Starting offset into lpszNumAccum
    UINT      bSigned:1,            // 44:  00000001:  TRUE iff the value is negative
              bFiller:1;            //      00000002:  TRUE iff the value is the filler <pnBool>
    PNNUMTYPE chType;               // 48:  The numeric type (see PNNUMTYPE)
    size_t    uNumStart;            // 4C:  Starting offset into lpszStart
    UCHAR     chCur,                // 50:  A char for the accumulator
              chAlign[3];           // 51:  For alignment
                                    // 54:  Length
} PN_YYSTYPE, *LPPN_YYSTYPE;        // Data type of yacc stack

// N.B.:  Whenever changing the above tagPN_YYSTYPE enum,
//   be sure to make a corresponding change to
//   <pnBool> in <pn_proc.c>.

#define YYSTYPE_IS_DECLARED 1


typedef struct tagPN_VECTOR
{
    ALLTYPES  at;                   // 00:  All datatypes as a union (64 bytes)
    PNNUMTYPE chType;               // 44:  The numeric type (see PNNUMTYPE)
    LPCHAR    lpStart;              // 48:  Ptr to start of char stream
    size_t    uNumLen;              // 4C:  # chars in lpStart
    UINT      bRatExp:1,            // 50:  00000001:  TRUE iff the number is expressible as a Rat
              :31;                  //      FFFFFFFE:  Available bits
                                    // 54:  Length
} PN_VECTOR, *LPPN_VECTOR;

#define PNVECTOR_INIT       10      // Initial allocation count of PN_VECTOR
#define PNVECTOR_INCR        5      // Incremental ...


typedef struct tagPNLOCALVARS       // Point Notation Local Vars
{
    LPCHAR        lpszStart;        // 00:  Ptr to start of char stream
    size_t        uNumLen,          // 04:  # chars in lpszStart
                  uNumCur,          // 08:  Current index into lpszStart
                  uNumIni,          // 0C:  Initial index into lpszStart
                  uAlpAccInt,       // 10:  Current index into lpszAlphaInt
                  uAlpAccDec,       // 14:  Current index into lpszAlphaDec
                  uNumAcc,          // 18:  Current index into lpszNumAccum
                  uCharIndex;       // 1C:  Starting character index
    LPCHAR        lpszAlphaInt,     // 20:  Ptr to AlphaInt accumulator
                  lpszAlphaDec,     // 24:  ...    AlphaDec ...
                  lpszNumAccum;     // 28:  Ptr to numeric accumulator (in case of overflow)
    ALLTYPES      at;               // 2C:  All datatypes as a union (64 bytes)
    PNNUMTYPE     chType,           // 6C:  The numeric type (see PNNUMTYPE)
                  chComType;        // 70:  The common numeric type ...
    UBOOL         bYYERROR;         // 74:  TRUE iff there's been a YYERROR
    HGLOBAL       hGlbRes,          // 78:  Result global memory handle
                  hGlbVector;       // 7C:  PN_VECTOR global memory handle (NULL = none)
    UINT          uGlbVectorMaxLen, // 80:  Maximum length of memory in hGlbVector (units = PN_VECTOR)
                  uGlbVectorCurLen; // 84:  Current ...
    LPPN_YYSTYPE  lpYYRes;          // 88:  Temp ptr
    struct tagTKLOCALVARS *
                  lptkLocalVars;    // 8C:  Ptr to Tokenize_EM local vars
    LPVARARRAY_HEADER lpMemHdrRes;  // 90:  Ptr to hGlbRes header
    UINT          bRatSep:1,        // 94:  00000001:  TRUE iff there is at least one Rat sep and no Vfp sep
                  bNeedEOT:1,       //      00000002:  TRUE iff we need an EOT
                  bEOT:1,           //      00000004:  TRUE iff we just returned an EOT
                  bEOS:1,           //      00000008:  TRUE iff the EOT we returned was an EOS
                  :28;              //      FFFFFFF0:  Available bits
    LPPN_YYSTYPE  lpCoeff[8];       // 98:  Ptrs to the possible coefficients (32 bytes)
                                    // B8:  Length
} PNLOCALVARS, *LPPNLOCALVARS;

// Error messages
#define PN_SYNTAX       "syntax error"
#define PN_NONCE        "nonce error"
#define PN_WSFULL       "memory exhausted"
#define PN_DOMAIN       "domain error"

// Text strings
#define INT_CHARS       "0123456789-"
#define INT_CHARS_WS    WIDEN (INT_CHARS)
#define DEC_CHARS       INT_CHARS "eE."
#define DEC_CHARS_WS    WIDEN (DEC_CHARS)
#define ARB_CHARS       DEC_CHARS DEF_ARBSEP_S
#define ARB_CHARS_WS    WIDEN (ARB_CHARS)
#define HC_SEPS         "sijJklo"
#define HC_CHARS        HC_SEPS "adu0123456789rv.eE" OVERBAR1_STR


//***************************************************************************
//  End of File: pn_parse.h
//***************************************************************************
