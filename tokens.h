//***************************************************************************
//  NARS2000 -- Tokens
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

/*

Tokens are containers  for all sorts of objects such as operators,
functions, variables, grouping symbols, lists, list separators, etc.

Most tokens are simple in that they have type and data only.e,g,
TKT_LEFTPAREN and '('.  Some tokens are complex in that their data
points to other data.

The following tokens are complex:

TKT_VARNAMED This container's tkData is an LPSYMENTRY which names
            a variable.  The LPSYMENTRY may be immediate or not.
            If the latter, stData contains an HGLOBAL which points
            to a simple or nested or hetero array.

TKT_VARIMMED This container's tkData is one of Boolean, Integer,
            Floating Point, or Character as distinguished by
            the value in tkFlags.ImmType.

TKT_LSTIMMED This container's tkData is a single element which is
            an immediate token.

TKT_LSTARRAY This container's tkData is a single element which is
            a single element array.

TKT_LSTMULT This container's tkData is multiple TOKENs, one per
            each separate list element.

TKT_VARARRAY The main workhorse.  Its tkData is an HGLOBAL which
            contains an array which is either simple, nested, or
            hetero.

TKT_STRAND  Valid only during strand processing after which it
            is converted to TKT_VARARRAY.

TKT_STRING  Valid only during strand processing after which it
            is converted to TKT_VARARRAY.

Outside strand processing, only the tokens TKT_VARNAMED, TKT_VARIMMED,
TKT_LIST, and TKT_VARARRAY are present in a token which points to a
value.

 */

// Data Token Types
typedef enum tagTOKEN_TYPES
{TKT_UNUSED        ,        // 00: Unused token type -- an error if encountered
 TKT_VARNAMED      ,        // 01: Symbol table entry for a named var      (data is LPSYMENTRY)
 TKT_CHRSTRAND     ,        // 02: Character strand                        (data is HGLOBAL)
 TKT_NUMSTRAND     ,        // 03: Numeric   ...                           (data is HGLOBAL)
 TKT_VARIMMED      ,        // 04: An immediate constant (Boolean, Integer, Character, or Floating point,
                            //     distinguished by the value in tkFlags.ImmType)
 TKT_ASSIGN        ,        // 05: Assignment symbol                       (data is UTF16_LEFTARROW)
 TKT_LISTSEP       ,        // 06: List separator                          (data is ';')
 TKT_LABELSEP      ,        // 07: Label separator                         (data is ':')
 TKT_COLON         ,        // 08: Colon                                   (data is ':')
 TKT_FCNIMMED      ,        // 09: An immediate primitive function         (data is UTF16_***)
 TKT_OP1IMMED      ,        // 0A: An immediate monadic primitive operator (data is UTF16_***)
 TKT_OP2IMMED      ,        // 0B: ...          dyadic  ...
 TKT_OP3IMMED      ,        // 0C: ...          ambiguous  ...
 TKT_OPJOTDOT      ,        // 0D: Outer product monadic operator (with right scope) (data is NULL)
 TKT_LEFTPAREN     ,        // 0E: Left paren                              (data is TKT_LEFTPAREN)
 TKT_RIGHTPAREN    ,        // 0F: Right ...                                ...         RIGHTPAREN
 TKT_LEFTBRACKET   ,        // 10: Left bracket                             ...        LEFTBRACKET
 TKT_RIGHTBRACKET  ,        // 11: Right ...                                ...         RIGHTBRACKET
 TKT_LEFTBRACE     ,        // 12: Left brace                               ...         LEFTBRACE
 TKT_RIGHTBRACE    ,        // 13: Right ...                                ...         RIGHTBRACE
 TKT_EOS           ,        // 14: End-of-Stmt (data is length of stmt including this token)
 TKT_EOL           ,        // 15: End-of-Line  ...
 TKT_SOS           ,        // 16: Start-of-Stmt                           (data is NULL)
 TKT_LINECONT      ,        // 17: Line continuation                       (data is NULL)
 TKT_INPOUT        ,        // 18: Input/Output                            (data is UTF16_QUAD or UTF16_QUOTEQUAD symbol)
 TKT_VARARRAY      ,        // 19: Array of data                           (data is HGLOBAL)
 TKT_CS_ANDIF      ,        // 1A: Control Structure:  ANDIF               (data is Line/Stmt/Token #)
 TKT_CS_ASSERT     ,        // 1B: ...                 ASSERT     ...
 TKT_CS_CASE       ,        // 1C: ...                 CASE       ...
 TKT_CS_CASELIST   ,        // 1D: ...                 CASELIST   ...
 TKT_CS_CONTINUE   ,        // 1E: ...                 CONTINUE   ...
 TKT_CS_CONTINUEIF ,        // 1F: ...                 CONTINUEIF ...
 TKT_CS_ELSE       ,        // 20: ...                 ELSE       ...
 TKT_CS_ELSEIF     ,        // 21: ...                 ELSEIF     ...
 TKT_CS_END        ,        // 22: ...                 END        ...
 TKT_CS_ENDFOR     ,        // 23: ...                 ENDFOR     ...
 TKT_CS_ENDFORLCL  ,        // 24: ...                 ENDFORLCL  ...
 TKT_CS_ENDIF      ,        // 25: ...                 ENDIF      ...
 TKT_CS_ENDREPEAT  ,        // 26: ...                 ENDREPEAT  ...
 TKT_CS_ENDSELECT  ,        // 27: ...                 ENDSELECT  ...
 TKT_CS_ENDWHILE   ,        // 28: ...                 ENDWHILE   ...
 TKT_CS_FOR        ,        // 29: ...                 FOR        ...
 TKT_CS_FOR2       ,        // 2A: ...                 FOR2       ...
 TKT_CS_FORLCL     ,        // 2B: ...                 FORLCL     ...
 TKT_CS_GOTO       ,        // 2C: ...                 GOTO       ...
 TKT_CS_IF         ,        // 2D: ...                 IF         ...
 TKT_CS_IF2        ,        // 2E: ...                 IF2        ...
 TKT_CS_IN         ,        // 2F: ...                 IN         ...
 TKT_CS_LEAVE      ,        // 30: ...                 LEAVE      ...
 TKT_CS_LEAVEIF    ,        // 31: ...                 LEAVEIF    ...
 TKT_CS_ORIF       ,        // 32: ...                 ORIF       ...
 TKT_CS_REPEAT     ,        // 33: ...                 REPEAT     ...
 TKT_CS_REPEAT2    ,        // 34: ...                 REPEAT2    ...
 TKT_CS_RETURN     ,        // 35: ...                 RETURN     ...
 TKT_CS_SELECT     ,        // 36: ...                 SELECT     ...
 TKT_CS_SELECT2    ,        // 37: ...                 SELECT2    ...
 TKT_CS_UNTIL      ,        // 38: ...                 UNTIL      ...
 TKT_CS_WHILE      ,        // 39: ...                 WHILE      ...
 TKT_CS_WHILE2     ,        // 3A: ...                 WHILE2     ...
 TKT_CS_SKIPCASE   ,        // 3B: ...                 Special token
 TKT_CS_SKIPEND    ,        // 3C: ...                 Special token
 TKT_SYS_NS        ,        // 3D: System namespace                        (namespace level, 0 = none, 1 = top)
 TKT_SYNTERR       ,        // 3E: Syntax Error                            (data is symbol)

 // These token types are not generated by <Tokenize_EM>
 //   hence, they never appear in a saved token stream,
 //   only as a parse token created during <ParseLine>.
 TKT_STRAND        ,        // 3F: Strand accumulating                     (data is LPTOKEN)
 TKT_LISTINT       ,        // 40: List intermediate                       (data is HGLOBAL)
 TKT_LISTPAR       ,        // 41: List in parens                          (data is HGLOBAL)
 TKT_LSTIMMED      ,        // 42: List in brackets, single element, immed (data is immediate)
 TKT_LSTARRAY      ,        // 43: List in brackets, single element, array (data is HGLOBAL)
 TKT_LSTMULT       ,        // 44: List in brackets, multiple elements     (data is HGLOBAL)
 TKT_FCNARRAY      ,        // 45: Array of functions and/or operators     (data is HGLOBAL)
 TKT_FCNNAMED      ,        // 46: A named function                        (data is LPSYMENTRY)
 TKT_AXISIMMED     ,        // 47: An immediate axis specification         (data is immediate)
 TKT_AXISARRAY     ,        // 48: An array of  ...                        (data is HGLOBAL)
 TKT_OP1NAMED      ,        // 49: A named monadic primitive operator      (data is LPSYMENTRY)
 TKT_OP2NAMED      ,        // 4A: ...     dyadic  ...
 TKT_OP3NAMED      ,        // 4B: ...     ambiguous ...
 TKT_STRNAMED      ,        // 4C: A named strand
 TKT_CS_NEC        ,        // 4D: Control structure:  Special token (cs_yyparse only)
 TKT_CS_EOL        ,        // 4E: Control structure:  Special token (cs_yyparse only)
 TKT_CS_ENS        ,        // 4F: Control structure:  Special token (cs_yyparse only)
 TKT_FILLJOT       ,        // 50: Fill jot
                            // 51-7F:  Available entries (7 bits)
} TOKEN_TYPES;

// N.B.:  Whenever changing the above enum (TOKEN_TYPES),
//   be sure to make a corresponding change to
//   <Untokenize> and <AppendNewToken_EM> in <tokenize.c>,
//   <LookaheadAdjacent>, <LookaheadDyadicOp>, and <pl_yylex> in <pl_parse.y>,
//   <MakeVarStrand_EM_YY> in <strand.c>,
//   <GetTokenTypeName> in <dispdbg.c>,
//   and <TokenTypeFV>, <IsTknTypeNamed>, <IsTknTypeNamedVar>, and <IsTknImmed> in <primfns.c>.


typedef struct tagTKFLAGS
{
    UINT TknType:7,         // 0000007F:  Data token type (see TOKEN_TYPES)
         :1,                // 00000080:  Alignment bit (available if necessary
         ImmType:4,         // 00000F00:  Type of immediate data (see IMM_TYPES) (if .TknType is TKT_xxxIMMED)
         NoDisplay:1,       // 00001000:  Do not display this token
         SysNSLvl:3,        // 0000E000:  System namespace level (0 = none, 1 = top)
         :16;               // FFFF0000:  Available bits
} TKFLAGS, *LPTKFLAGS;


typedef struct tagLOCATION
{
    USHORT uLineNum,            // 00:  Line #      (origin-1)
           uStmtNum,            // 02:  Statement # (origin-0)
           uTknNum;             // 04:  Token #     (origin-0)
                                // 06:  Length
} LOCATION, *LPLOCATION;

typedef const LOCATION CLOCATION, *LPCLOCATION;

typedef union tagULOCATION
{
    const LOCATION c;           // 00:  Constant LOCATION
          LOCATION d;           // 00:  Dynamic  ...
                                // 06:  Length
} ULOCATION, *LPULOCATION;

#define ANON_CTRL_STRUC             \
  struct {ULOCATION Orig;           \
           LOCATION Next;           \
           USHORT   uCLIndex;       \
           USHORT   bSOS:1,         \
                    :15;            \
         }

typedef union tagTOKEN_DATA
{
    struct tagSYMENTRY
              *tkSym;               // 00:  Data is an LPSYMENTRY
    HGLOBAL    tkGlbData;           // 00:  ...     an HGLOBAL
    UINT       tkIndex;             // 00:  ...     an index
    APLBOOL    tkBoolean;           // 00:  ...     an APLBOOL
    APLINT     tkInteger;           // 00:  ...     an APLINT
    ANON_CTRL_STRUC;                // 00:  ...     Ctrl Struc data (8 bytes)
    struct tagTOKEN
              *lptkCSLink;          // 00:  ...     Ptr to previous token at start of stmt
                                    //              (found in TKT_CS_xxx2 only)
    APLFLOAT   tkFloat;             // 00:  ...     a floating point number
    APLCHAR    tkChar;              // 00:  ...     an APLCHAR
    LPVOID     tkVoid;              // 00:  ...     an abritrary ptr
    APLLONGEST tkLongest;           // 00:  Longest datatype (8 bytes) (so we can copy the entire data)
    ANON_CTRL_STRUC tkCtrlStruc;    // 00:  Longest Ctrl Struc (16 bytes)
                                    // 10:  Length
} TOKEN_DATA, *LPTOKEN_DATA;

typedef struct tagTOKEN
{
    TKFLAGS          tkFlags;       // 00:  The flags part
    TOKEN_DATA       tkData;        // 04:  The data part (16 bytes)
    int              tkCharIndex;   // 14:  Index into the input line of this token
                                    // 18:  Length
} TOKEN, *LPTOKEN;

#define TOKEN_HEADER_SIGNATURE      'NKOT'

typedef struct tagTOKEN_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  Token header signature
    UINT             Version,       // 04:  Version # of this header
                     TokenCnt,      // 08:  # tokens in lpToken
                     PrevGroup;     // 0C:  Index of the previous (to the left) grouping symbol
                                    //      (L/R paren, L/R bracket) where the index is relative
                                    //      to the first token after this header.
                                    // 10:  Length
                                    // 10:  Followed by a <TokenCnt> array of TOKENs
} TOKEN_HEADER, *LPTOKEN_HEADER;


//***************************************************************************
//  End of File: tokens.h
//***************************************************************************
