//***************************************************************************
//  NARS2000 -- ParseLine Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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
    UINT    TknCount;               // 10:  Token count
    UINT    YYInuse:1,              // 14:  00000001:  This entry is in use
            YYIndirect:1,           //      00000002:  Arg is indirect
#ifdef DEBUG
            :6,                     //      000000FC:  Available bits
            YYIndex:23,             //      7FFFFF00:  Index #
            YYFlag:1;               //      80000000:  Flag to distinguish YYAlloc from yylex
#else
            :30;                    //      FFFFFFFC:  Available bits
#endif
    struct tagPL_YYSTYPE
           *lpYYFcnBase,            // 18:  Ptr to base function/operator
                                    //      Not valid outside the enclosing
                                    //        invocation of ParseLine
           *lpYYStrandBase;         // 1C:  Ptr to this token's strand base
#ifdef DEBUG
    UINT    SILevel;                // 20:  SI Level (needed for YYResIsEmpty)
    LPCHAR  lpFileName;             // 24:  Ptr to filename where allocated
    UINT    uLineNum;               // 28:  Line # where allocated
                                    // 2C:  Length
#else
                                    // 20:  Length
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
    HGLOBAL      hGlbTknLine,       // 00:  Tokenized line global memory handle
                 hGlbTxtLine;       // 04:  Text      ...
    UNION_TOKEN  t2;                // 08:  Locked base of hGlbToken
    LPTOKEN      lptkStart,         // 0C:  Ptr to first available entry after the header
                 lptkNext,          // 10:  Ptr to next  ...
                 lptkEnd,           // 14:  Ptr to end of token stream
                 lptkStop;          // 18:  Ptr to stop token if LookAhead
    LPAPLCHAR    lpwszLine;         // 1C:  Ptr to line text (zero-terminated)
    UINT         tkErrorCharIndex,  // 20:  Error char index
                 tkLACharIndex;     // 24:  LookAhead char index
    UINT         plNameType:4,      // 28:  0000000F:  Object name type (see NAME_TYPES)
                 bLookAhead:1,      //      00000010:  TRUE iff looking for object type within surrounding parens
                 ExitType:4,        //      000001E0:  Exit Type (see EXIT_TYPES)
                 bRet:1,            //      00000200   Various function results
                 bStopExec:1,       //      00000400   TRUE iff we're to stop executing this line
                 bRestart:1,        //      00000800   TRUE iff we're to restart from a Control Structure
                 bExec1Stmt:1,      //      00001000   TRUE iff we're to execute one stmt only
                 bReturn:1,         //      00002000   TRUE iff we're faking a :return as {goto} 0
                 bYYERROR:1,        //      00004000   TRUE iff there's been a YYERROR
                 bSelSpec:1,        //      00008000   TRUE iff we're inside Selective Specification
                 bIniSpec:1,        //      00010000   TRUE iff we have yet to see the first name inside Selective Specification
                 :15;               //      FFFE0000:  Available bits
    UBOOL        bCtrlBreak;        // 2C:  TRUE iff Ctrl-Break pressed
    LPPL_YYSTYPE lpYYStrandStart[STRAND_LEN],   // 30:  Strand stack start (static) (8 bytes)
                 lpYYStrandBase [STRAND_LEN],   // 38:  ...          base (dynamic) ...
                 lpYYStrandNext [STRAND_LEN];   // 40:  ...          next token (dynamic)
    struct tagPERTABDATA *lpMemPTD; // 48:  Ptr to PerTabData global memory
    HWND         hWndSM;            // 50:  Window handle to Session Manager
    LPPL_YYSTYPE lpYYStr,           // 54:  Ptr to PL_YYSTYPE strand
                 lpYYStrL,          // 58:  ...               left strand
                 lpYYStrR,          // 5C:  ...               right strand
                 lpYYStrN,          // 60:  ...               name strand
                 lpYYRes,           // 64:  ...               result
                 lpYYFcn,           // 68:  ...               function strand
                 lpYYLst,           // 6C:  ...               list
                 lpYYAxis,          // 70:  ...               axis
                 lpYYOp1,           // 74:  ...               monadic operator
                 lpYYOp2,           // 78:  ...               dyadic  ...
                 lpYYOp3,           // 7C:  ...               ambiguous ...
                 lpYYLft,           // 80:  ...               left operand
                 lpYYRht,           // 84:  ...               right operand
                 lpYYMak;           // 88:  ...               MakeXXX function
    struct tagPLLOCALVARS
                *lpPLPrev;          // 8C:  Ptr to previous PLLOCALVARS struct
                                    //      in thread creation order (NULL = none)
    UINT         uLineNum,          // 90:  Function line # (1 for execute or immexec)
                 uTokenCnt;         // 94:  # tokens in the function line
    HGLOBAL      hGlbDfnHdr;        // 98:  User-defined functio/operator global memory handle (NULL = execute/immexec)
    TOKEN        tkSelSpec;         // 9C:  TOKEN for Selective Specification (16 bytes)
                                    // AC:  Length
} PLLOCALVARS, *LPPLLOCALVARS;


//***************************************************************************
//  End of File: pl_parse.h
//***************************************************************************
