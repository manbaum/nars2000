//***************************************************************************
//  NARS2000 -- ParseLine Header
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

typedef struct tagPL_YYSTYPE        // YYSTYPE for ParseLine
{
    // N.B.:  The first item in this struct *MUST* be the TOKEN
    //   because when we pass it as an argument to an LPPRIMFNS
    //   we might need it to be an LPPL_YYSTYPE (for a function strand)
    //   or an LPTOKEN (single function).
    TOKEN   tkToken;                // 00:  Token info
    UINT    TknCount,               // 14:  Token count
            FcnCount;               // 18:  Function count
    UINT    YYInuse:1,              // 1C:  00000001:  This entry is in use
            YYIndirect:1,           //      00000002:  Arg is indirect
#ifdef DEBUG
            Avail:6,                //      000000FC:  Available bits
            YYIndex:23,             //      7FFFFF00:  Index #
            YYFlag:1;               //      80000000:  Flag to distinguish YYAlloc from yylex
#else
            Avail:30;               //      FFFFFFFC:  Available bits
#endif
    struct tagPL_YYSTYPE *
            lpYYFcn;                // 20:  Ptr to function/operator
    struct tagPL_YYSTYPE *
            lpYYStrandBase;         // 24:  Ptr to this token's strand base
#ifdef DEBUG
    UINT    SILevel;                // 2C:  SI Level (needed for YYResIsEmpty)
                                    // 30:  Length
#else
                                    // 28:  Length
#endif
} PL_YYSTYPE, *LPPL_YYSTYPE;        // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1

// Define indices into lpYYStrandStart/Base/Nest[STRAND_LEN];
typedef enum tagSTRAND_INDS
{
    STRAND_VAR = 0,                 // 00:  Index for var strands
    STRAND_FCN,                     // 01:  Index for function strands
    STRAND_LEN                      // 02:  # elements in this enum
} STRAND_INDS;

typedef struct tagPLLOCALVARS       // ParseLine Local Vars
{
    HGLOBAL      hGlbTxtLine,       // 00:  Line text global memory handle
                 hGlbToken;         // 04:  Tokenized line global memory handle
    UNION_TOKEN  t2;                // 08:  Locked base of hGlbToken
    LPTOKEN      lptkStart,         // 0C:  Ptr to first available entry after the header
                 lptkNext,          // 10:  Ptr to next  ...
                 lptkStop;          // 14:  Ptr to stop token if LookAhead
    LPAPLCHAR    lpwszLine;         // 18:  Ptr to line text (zero-terminated)
    UINT         tkErrorCharIndex,  // 1C:  Error char index
                 tkLACharIndex;     // 20:  LookAhead char index
    UINT         NameType:4,        // 24:  0000000F:  Object type (see NAME_TYPES)
                 bLookAhead:1,      //      00000010:  TRUE iff looking for object type within surrounding parens
                 ExitType:4,        //      000001E0:  Exit Type (see EXIT_TYPES)
                 bRet:1,            //      00000200   Various function results
                 Avail:22;          //      FFFFFC00:  Available bits
    BOOL         bCtrlBreak;        // 28:  TRUE iff Ctrl-Break pressed
    LPPL_YYSTYPE lpYYStrandStart[STRAND_LEN],   // 2C:  Strand stack start (static)
                 lpYYStrandBase [STRAND_LEN],   // 34:  ...          base (dynamic)
                 lpYYStrandNext [STRAND_LEN];   // 3C:  ...          next token (dynamic)
    HGLOBAL      hGlbPTD;           // 44:  Handle to PerTabData
    HWND         hWndSM;            // 4C:  Window handle to Session Manager
    LPPL_YYSTYPE lpYYStr,           // 50:  Ptr to PL_YYSTYPE strand
                 lpYYStrL,          // 54:  ...               left strand
                 lpYYStrR,          // 58:  ...               right strand
                 lpYYStrN,          // 5C:  ...               name strand
                 lpYYRes,           // 60:  ...               result
                 lpYYFcn,           // 64:  ...               function strand
                 lpYYLst,           // 68:  ...               list
                 lpYYAxis,          // 6C:  ...               axis
                 lpYYOp1,           // 70:  ...               monadic operator
                 lpYYOp2,           // 74:  ...               dyadic  ...
                 lpYYOp3,           // 78:  ...               ambiguous ...
                 lpYYLft,           // 7C:  ...               left operand
                 lpYYRht,           // 80:  ...               right operand
                 lpYYMak;           // 84:  ...               MakeXXX function
    struct tagPLLOCALVARS
                *lpPLPrev;          // 88:  Ptr to previous PLLOCALVARS struct
                                    //      in thread creation order (NULL = none)
                                    // 8C:  Length
} PLLOCALVARS, *LPPLLOCALVARS;


//***************************************************************************
//  End of File: parse.h
//***************************************************************************
