//***************************************************************************
//  NARS2000 -- Tokens
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
#define TKT_FIRST 1         // This way, we can catch uninitialized
                            //   token types (i.e., .TknType EQ 0)
typedef enum tagTOKEN_TYPES
{TKT_VARNAMED = TKT_FIRST,  // 01: Symbol table entry for a named var      (data is LPSYMENTRY)
 TKT_CHRSTRAND   ,          // 02: Character strand                        (data is HGLOBAL)
 TKT_NUMSTRAND   ,          // 03: Numeric   ...                           (data is HGLOBAL)
 TKT_VARIMMED    ,          // 04: An immediate constant (Boolean, Integer, Character, or Floating point,
                            //     distinguished by the value in tkFlags.ImmType)
 TKT_ASSIGN      ,          // 05: Assignment symbol                       (data is UTF16_LEFTARROW)
 TKT_LISTSEP     ,          // 06: List separator                          (data is ';')
 TKT_LABELSEP    ,          // 07: Label separator                         (data is ':')
 TKT_COLON       ,          // 08: Colon                                   (data is ':')
 TKT_FCNIMMED    ,          // 09: An immediate primitive function         (data is UTF16_***)
 TKT_OP1IMMED    ,          // 0A: An immediate monadic primitive operator (data is UTF16_***)
 TKT_OP2IMMED    ,          // 0B: ...          dyadic  ...
 TKT_OP3IMMED    ,          // 0C: ...          ambiguous  ...
 TKT_OPJOTDOT    ,          // 0D: Outer product monadic operator (with right scope) (data is NULL)
 TKT_LEFTPAREN   ,          // 0E: Left paren                              (data is TKT_LEFTPAREN)
 TKT_RIGHTPAREN  ,          // 0F: Right ...                                ...         RIGHTPAREN
 TKT_LEFTBRACKET ,          // 10: Left bracket                             ...        LEFTBRACKET
 TKT_RIGHTBRACKET,          // 11: Right ...                                ...         RIGHTBRACKET
 TKT_LEFTBRACE   ,          // 12: Left brace                               ...         LEFTBRACE
 TKT_RIGHTBRACE  ,          // 13: Right ...                                ...         RIGHTBRACE
 TKT_EOS         ,          // 14: End-of-Stmt (data is length of stmt including this token)
 TKT_EOL         ,          // 15: End-of-Line  ...
 TKT_SOS         ,          // 16: Start-of-Stmt                           (data is NULL)
 TKT_LINECONT    ,          // 17: Line continuation                       (data is NULL)
 TKT_INPOUT      ,          // 18: Input/Output                            (data is UTF16_QUAD or UTF16_QUOTEQUAD symbol)
 TKT_VARARRAY    ,          // 19: Array of data                           (data is HGLOBAL)
 TKT_CS_ANDIF    ,          // 1A: Control Structure:  ANDIF               (Data is Line/Stmt #)
 TKT_CS_CASE     ,          // 1B: ...                 CASE       ...
 TKT_CS_CASELIST ,          // 1C: ...                 CASELIST   ...
 TKT_CS_CONTINUE ,          // 1D: ...                 CONTINUE   ...
 TKT_CS_ELSE     ,          // 1E: ...                 ELSE       ...
 TKT_CS_ELSEIF   ,          // 1F: ...                 ELSEIF     ...
 TKT_CS_END      ,          // 20: ...                 END        ...
 TKT_CS_ENDFOR   ,          // 21: ...                 ENDFOR     ...
 TKT_CS_ENDFORLCL,          // 22: ...                 ENDFORLCL  ...
 TKT_CS_ENDIF    ,          // 23: ...                 ENDIF      ...
 TKT_CS_ENDREPEAT,          // 24: ...                 ENDREPEAT  ...
 TKT_CS_ENDSELECT,          // 25: ...                 ENDSELECT  ...
 TKT_CS_ENDWHILE ,          // 26: ...                 ENDWHILE   ...
 TKT_CS_FOR      ,          // 27: ...                 FOR        ...
 TKT_CS_FOR2     ,          // 28: ...                 FOR2       ...
 TKT_CS_FORLCL   ,          // 29: ...                 FORLCL     ...
 TKT_CS_GOTO     ,          // 2A: ...                 GOTO       ...
 TKT_CS_IF       ,          // 2B: ...                 IF         ...
 TKT_CS_IF2      ,          // 2C: ...                 IF2        ...
 TKT_CS_IN       ,          // 2D: ...                 IN         ...
 TKT_CS_LEAVE    ,          // 2E: ...                 LEAVE      ...
 TKT_CS_ORIF     ,          // 2F: ...                 ORIF       ...
 TKT_CS_REPEAT   ,          // 30: ...                 REPEAT     ...
 TKT_CS_REPEAT2  ,          // 31: ...                 REPEAT2    ...
 TKT_CS_RETURN   ,          // 32: ...                 RETURN     ...
 TKT_CS_SELECT   ,          // 33: ...                 SELECT     ...
 TKT_CS_SELECT2  ,          // 34: ...                 SELECT2    ...
 TKT_CS_UNTIL    ,          // 35: ...                 UNTIL      ...
 TKT_CS_WHILE    ,          // 36: ...                 WHILE      ...
 TKT_CS_WHILE2   ,          // 37: ...                 WHILE2     ...
 TKT_CS_SKIPCASE ,          // 38: ...                 Special token
 TKT_CS_SKIPEND  ,          // 39: ...                 Special token
 TKT_SYS_NS      ,          // 3A: System namespace                        (namespace level, 0 = none, 1 = top)
 TKT_SYNTERR     ,          // 3B: Syntax Error                            (data is symbol)

 TKT_LENGTHp1    ,          // 3C: Last token type

 // These token types are not generated by <Tokenize_EM>
 //   hence, they never appear in a saved token stream,
 //   only as a parse token created during <ParseLine>.
 TKT_STRAND = TKT_LENGTHp1, // 3C: Strand accumulating                     (data is LPTOKEN)
 TKT_LISTINT     ,          // 3D: List intermediate                       (data is HGLOBAL)
 TKT_LISTPAR     ,          // 3E: List in parens                          (data is HGLOBAL)
 TKT_LSTIMMED    ,          // 3F: List in brackets, single element, immed (data is immediate)
 TKT_LSTARRAY    ,          // 40: List in brackets, single element, array (data is HGLOBAL)
 TKT_LSTMULT     ,          // 41: List in brackets, multiple elements     (data is HGLOBAL)
 TKT_FCNARRAY    ,          // 42: Array of functions and/or operators     (data is HGLOBAL)
 TKT_FCNNAMED    ,          // 43: A named function                        (data is LPSYMENTRY)
 TKT_AXISIMMED   ,          // 44: An immediate axis specification         (data is immediate)
 TKT_AXISARRAY   ,          // 45: An array of  ...                        (data is HGLOBAL)
 TKT_OP1NAMED    ,          // 46: A named monadic primitive operator      (data is LPSYMENTRY)
 TKT_OP2NAMED    ,          // 47: ...     dyadic  ...
 TKT_OP3NAMED    ,          // 48: ...     ambiguous ...
 TKT_STRNAMED    ,          // 49: A named strand
 TKT_CS_NEC      ,          // 4A: Control structure:  Special token (cs_yyparse only)
 TKT_CS_EOL      ,          // 4B: Control structure:  Special token (cs_yyparse only)
                            // 4C-7F:  Available entries (7 bits)
} TOKEN_TYPES;

#define TKT_LENGTH      (TKT_LENGTHp1 - 1)

// N.B.:  Whenever changing the above enum (TOKEN_TYPES),
//   be sure to make a corresponding change to
//   <Untokenize> and <AppendNewToken_EM> in <tokenize.c>,
//   <LookaheadAdjacent>, <LookaheadDyadicOp>, and <pl_yylex> in <pl_parse.y>,
//   <MakeVarStrand_EM_YY> in <strand.c>,
//   <GetTokenTypeName> in <dispdbg.c>,
//   and <TokenTypeFV>, <IsTknNamed>, and <IsTknImmed> in <primfns.c>.


typedef struct tagTKFLAGS
{
    UINT TknType:7,         // 0000007F:  Data token type (see TOKEN_TYPES)
         :1,                // 00000080:  Alignment bit (available if necessary
         ImmType:4,         // 00000F00:  Type of immediate data (see IMM_TYPES) (if .TknType is TKT_xxxIMMED)
         NoDisplay:1,       // 00001000:  Do not display this token
         SysNSLvl:3,        // 0000E000:  System namespace level (0 = none, 1 = top)
         :16;               // FFFF0000:  Available bits
} TKFLAGS, *LPTKFLAGS;

#define ANON_CTRL_STRUC         \
  struct {USHORT uLineNum;      \
          USHORT uStmtNum;      \
          USHORT uTknNum;       \
          USHORT uCLIndex:15,   \
                 bSOS:1;        \
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
    APLLONGEST tkLongest;           // 00:  Longest datatype (so we can copy the entire data)
                                    // 08:  Length
} TOKEN_DATA, *LPTOKEN_DATA;

typedef struct tagTOKEN
{
    TKFLAGS          tkFlags;       // 00:  The flags part
    TOKEN_DATA       tkData;        // 04:  The data part (8 bytes)
    int              tkCharIndex;   // 0C:  Index into the input line of this token
                                    // 10:  Length
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


typedef union tagUNION_TOKEN
{
    LPTOKEN        lpBase;          // 00:  Locked base of hGlbToken
    LPTOKEN_HEADER lpHeader;        // 00:  Token header
                                    // 04:  Length
} UNION_TOKEN, *LPUNION_TOKEN;


//***************************************************************************
//  End of File: tokens.h
//***************************************************************************
