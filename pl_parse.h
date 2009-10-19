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
            YYCopyArray:1,          //      00000004:  it's been copied, so it needs to be freed
#ifdef DEBUG
            :5,                     //      000000F8:  Available bits
            YYIndex:23,             //      7FFFFF00:  Index #
            YYFlag:1;               //      80000000:  Flag to distinguish YYAlloc from yylex
#else
            :29;                    //      FFFFFFF8:  Available bits
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
    STRAND_LST,                     // 02:  Index for list strands
    STRAND_NAM,                     // 03:  Index for name strands
    STRAND_LEN                      // 04:  # elements in this enum
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
    LPPL_YYSTYPE lpYYStrArrStart[STRAND_LEN],   // 30:  Strand stack start (static) (16 bytes)
                 lpYYStrArrBase [STRAND_LEN],   // 40:  ...          base (dynamic) ...
                 lpYYStrArrNext [STRAND_LEN];   // 50:  ...          next token (dynamic)
    struct tagPERTABDATA *lpMemPTD; // 58:  Ptr to PerTabData global memory
    HWND         hWndSM;            // 60:  Window handle to Session Manager
    LPPL_YYSTYPE lpYYStr,           // 64:  Ptr to PL_YYSTYPE strand
                 lpYYStrL,          // 68:  ...               left strand
                 lpYYStrR,          // 6C:  ...               right strand
                 lpYYStrN,          // 70:  ...               name strand
                 lpYYRes,           // 74:  ...               result
                 lpYYFcn,           // 78:  ...               function strand
                 lpYYLst,           // 7C:  ...               list
                 lpYYAxis,          // 80:  ...               axis
                 lpYYOp1,           // 84:  ...               monadic operator
                 lpYYOp2,           // 88:  ...               dyadic  ...
                 lpYYOp3,           // 8C:  ...               ambiguous ...
                 lpYYLft,           // 90:  ...               left operand
                 lpYYRht,           // 94:  ...               right operand
                 lpYYMak;           // 98:  ...               MakeXXX function
    struct tagPLLOCALVARS
                *lpPLPrev;          // 9C:  Ptr to previous PLLOCALVARS struct
                                    //      in thread creation order (NULL = none)
    UINT         uLineNum,          // A0:  Function line # (1 for execute or immexec)
                 uTokenCnt;         // A4:  # tokens in the function line
    HGLOBAL      hGlbDfnHdr;        // A8:  User-defined functio/operator global memory handle (NULL = execute/immexec)
    TOKEN        tkSelSpec;         // AC:  TOKEN for Selective Specification (16 bytes)
                                    // BC:  Length
} PLLOCALVARS, *LPPLLOCALVARS;


//***************************************************************************
//  End of File: pl_parse.h
//***************************************************************************
