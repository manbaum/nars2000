//***************************************************************************
//  NARS2000 -- Tokenizer Header
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


// Defined constant for ...
#define NO_PREVIOUS_GROUPING_SYMBOL     NEG1U
#define NO_PREVIOUS_NAME                NEG1U


// The order of the values of these constants *MUST* match the
//   column order in fsaColTable.
#define COL_FIRST 0     // It's origin-0

typedef enum tagCOL_INDICES
{COL_DIGIT = COL_FIRST, // 00:  Digit
 COL_DOT         ,      // 01:  Decimal number, inner & outer product separator
 COL_ALPHA       ,      // 02:  Alphabetic
 COL_OVERBAR     ,      // 03:  Overbar
 COL_DIRIDENT    ,      // 04:  Alpha or Omega
 COL_Q_QQ        ,      // 05:  Quad
 COL_UNDERBAR    ,      // 06:  Underbar
 COL_INFINITY    ,      // 07:  Infinity
 COL_ASSIGN      ,      // 08:  Assignment symbol
 COL_SEMICOLON   ,      // 09:  Semicolon symbol
 COL_COLON       ,      // 0A:  Colon symbol
 COL_CTRLSTRUC   ,      // 0B:  Control Structure
 COL_PRIM_FN     ,      // 0C:  Primitive monadic or dyadic function
 COL_PRIM_FN0    ,      // 0D:  ...       niladic function
 COL_PRIM_OP1    ,      // 0E:  ...       monadic/ambiguous operator
 COL_PRIM_OP2    ,      // 0F:  ...       dyadic  ...
 COL_JOT         ,      // 10:  Jot symbol
 COL_LEFTPAREN   ,      // 11:  Left paren
 COL_RIGHTPAREN  ,      // 12:  Right ...
 COL_LEFTBRACKET ,      // 13:  Left bracket
 COL_RIGHTBRACKET,      // 14:  Right ...
 COL_LEFTBRACE   ,      // 15:  Left brace
 COL_RIGHTBRACE  ,      // 16:  Right ...
 COL_SPACE       ,      // 17:  White space (' ' or '\t')
 COL_QUOTE1      ,      // 18:  Single quote symbol
 COL_QUOTE2      ,      // 19:  Double ...
 COL_DIAMOND     ,      // 1A:  Diamond symbol
 COL_LAMP        ,      // 1B:  Comment symbol
 COL_EOL         ,      // 1C:  End-Of-Line
 COL_UNK         ,      // 1D:  Unknown symbols

 COL_LENGTH      ,      // 1E: # column indices (cols in fsaColTable) ***MUST*** BE THE LAST ENTRY
                        //     Because this enum is origin-0, this value is the # valid columns.
} COLINDICES, *LPCOLINDICES;

// Whenever you add a new COL_*** entry,
//   be sure to put code into <CharTrans> in <tokenize.c>
//   to return the newly defined value,
//   and insert a new entry into <GetColName> in <tokenize.c>.


// The order of the values of these constants *MUST* match the
//   row order in fsaColTable.
typedef enum tagFSA_TOKENS
{FSA_SOS = 0 ,  // 00:  Start of stmt
 FSA_INIT    ,  // 01:  Initial state
 FSA_POINTNOT,  // 02:  Point Notation
 FSA_ALPHA   ,  // 03:  Alphabetic char
 FSA_SYSNAME ,  // 04:  System name
 FSA_QUOTE1A ,  // 05:  Start of or within single quoted char or char vector
 FSA_QUOTE1Z ,  // 06:  End of   ...
 FSA_QUOTE2A ,  // 07:  Start of or within double quoted char or char vector
 FSA_QUOTE2Z ,  // 08:  End of   ...
 FSA_DOTAMBIG,  // 09:  Ambiguous dot:  either FSA_POINTNOT or FSA_INIT w/fnOp2Done
 FSA_JOTAMBIG,  // 0A:  Ambiguous jot:  either FSA_INIT w/fnOp2Done or FSA_OUTAMBIG
 FSA_OUTAMBIG,  // 0B:  Ambiguous outer product:  either FSA_INIT w/fnOutDone or FSA_POINTNOT w/fnOp2Done

 FSA_LENGTH     // 0C:  # FSA terminal states (rows in fsaColTable) ***MUST*** BE THE LAST ENTRY
                // Because this enum is origin-0, this value is the # valid columns.
} FSATOKENS, *LPFSATOKENS;

typedef struct tagSC_INDICES
{
    UINT        PrevGroup,          // 00:  Index into lpGrpSeqIni of previous grouping symbol
                                    //      (NO_PREVIOUS_GROUPING_SYMBOL = none)
                clrIndex;           // 04:  Matching index into lpMemClrIni
    TOKEN_TYPES TknType;            // 08:  Token type of this grouping symbol
} SCINDICES, *LPSCINDICES;


typedef enum tagSC_NAMETYPE
{
    SC_NAMETYPE_GLBNAME = 0,        // 00:  Name is global
    SC_NAMETYPE_LCLNAME,            // 01:  ...     local to a fcn suspended or being edited
    SC_NAMETYPE_SYSFCN,             // 02:  ...     system function
    SC_NAMETYPE_GLBSYSVAR,          // 03:  ...     global system variable
    SC_NAMETYPE_LCLSYSVAR,          // 04:  ...     local  ...    variable
    SC_NAMETYPE_SYSUNK,             // 05:  ...     ...    unknown
    SC_NAMETYPE_PRIMITIVE,          // 06:  ...     a primitive (Quad/QuoteQuad)
    SC_NAMETYPE_UNK,                // 07:  ...     unknown
                                    // 08:  Length
} SCNAMETYPE, *LPSCNAMETYPE;

typedef struct tagCLR_COL
{
    SYNTAXCOLORS syntClr;           // 00:  Foreground and background colors (8 bytes)
    COLINDICES   colIndex;          // 08:  Column index (see COL_INDICES)
} CLRCOL, *LPCLRCOL;

typedef struct tagTKLOCALVARS
{
    HGLOBAL     hGlbToken;          // 00:  Global memory handle
    UNION_TOKEN t2;                 // 04:  Locked base of hGlbToken
    LPTOKEN     lpStart,            // 08:  First available entry after the header
                lpNext,             // 0C:  Next  ...
                lpLastEOS;          // 10:  Ptr to last EOS token
    FSATOKENS   State[3];           // 14:  Current state (FSA_xxx) (12 bytes)
    UINT        uChar,              // 20:  ...     index into lpwszLine
                uCharStart,         // 24:  Initial ...                  (static)
                uCharIni,           // 28:  ...                          (dynamic)
                uActLen;            // 2C:  Actual length of lpwszLine (may be shorter than lstrlenW)
    LPWCHAR     lpwszOrig,          // 30:  Ptr to original lpwszLine
                lpwszCur;           // 34:  ...    current WCHAR in ...
    TOKEN_TYPES CtrlStrucTknType;   // 38:  Control Structure token type
    UINT        CtrlStrucStrLen;    // 3C:  ...               string length
    ANON_CTRL_STRUC;                // 40:  Ctrl Struc data (8 bytes)
    LPCLRCOL    lpMemClrIni,        // 48:  Ptr to initial array of Syntax Colors (NULL = not coloring) (static)
                lpMemClrNxt;        // 4C:  Ptr to next    ...                                          (dynamic)
    LPSCINDICES lpGrpSeqIni,        // 50:  Ptr to initial syntax coloring grouping sequence (static)
                lpGrpSeqNxt;        // 54:  Ptr to next    ...                               (dynamic)
    UINT        PrevGroup,          // 58:  Current index in lpGrpSeq of the previous grouping symbol
                                    //      (NO_PREVIOUS_GROUPING_SYMBOL = none)
                NameInit;           // 5C:  Index in lpMemClrIni of Start of name (including sysnames)
                                    //      (NO_PREVIOUS_NAME = none)
    SCNAMETYPE  scNameType;         // 60:  Type of name starting at NameInit
    HWND        hWndEC;             // 64:  Window handle of Edit Ctrl (parent is SM or FE)
    COLINDICES  colIndex;           // 68:  Current COL_xxx value
    UINT        uSyntClrLen;        // 6C:  # Syntax Color entries
                                    // 70:  Length
} TKLOCALVARS, *LPTKLOCALVARS;

typedef UBOOL (*FNACTION) (LPTKLOCALVARS);

typedef struct tagFSA_ACTION
{
    FSATOKENS iNewState;            // 00:  New state (see FSA_TOKENS)
    FNACTION  fnAction1;            // 04:  Primary action
    FNACTION  fnAction2;            // 08:  Secondary action
} FSA_ACTION;


//***************************************************************************
//  End of File: tokenize.h
//***************************************************************************
