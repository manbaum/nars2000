//***************************************************************************
//  NARS2000 -- Tokens
//***************************************************************************

/*

Tokens are containers  for all sorts of objects such as operators,
functions, variables, grouping symbols, lists, list separators, etc.

Most tokens are simple in that they have type and data only.e,g,
TKT_LPAREN and '('.  Some tokens are complex in that their data
points to other data.

The following tokens are complex:

TKT_VARNAMED This container's tkData is an LPSYMENTRY which names
            a variable.  The LPSYMENTRY may be immediate or not.
            If the latter, stData contains an HGLOBAL which points
            to a simple or nested or hetero array.

TKT_VARIMMED This container's tkData is one of Boolean, Integer,
            Floating Point, or Character as distinguished by
            the value in tkFlags.ImmType.

TKT_LIST    This container's tkData points to an HGLOBAL which
            contains a list array in global memory.

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
typedef enum tagTOKENTYPES
{TKT_VARNAMED = TKT_FIRST,  // 01: Symbol table entry for a named var (data is LPSYMENTRY)
 TKT_STRING   ,             // 02: String  (data is HGLOBAL)
 TKT_VARIMMED ,             // 03: An immediate constant (Boolean, Integer, Character, or Floating point,
                            //     distinguished by the value in tkFlags.ImmType)
 TKT_COMMENT  ,             // 04: Comment (data is HGLOBAL)
 TKT_ASSIGN   ,             // 05: Assignment symbol (data is UTF16_LEFTARROW)
 TKT_LISTSEP  ,             // 06: List separator    (data is ';')
 TKT_COLON    ,             // 07: Label separator   (data is ':')
 TKT_FCNIMMED ,             // 08: An immediate primitive function (data is UTF16_***)
 TKT_OP1IMMED ,             // 09: An immediate monadic primitive operator (data is UTF16_***)
 TKT_OP2IMMED ,             // 0A: ...          dyadic  ...
 TKT_OPJOTDOT ,             // 0B: Outer product monadic operator (with right scope) (data is NULL)
 TKT_LPAREN   ,             // 0C: Left paren (data is TKT_LPAREN)
 TKT_RPAREN   ,             // 0D: Right ...   ...         RPAREN
 TKT_LBRACKET ,             // 0E: Left bracket ...        LBRACKET
 TKT_RBRACKET ,             // 0F: Right ...   ...         RBRACKET
 TKT_EOS      ,             // 10: End-of-Stmt (data is length of stmt including this token)
 TKT_EOL      ,             // 11: End-of-Line  ...
 TKT_LINECONT ,             // 12: Line continuation (data is NULL)
 TKT_INPOUT   ,             // 13: Input/Output (data is UTF16_QUAD or UTF16_QUOTEQUAD symbol)

 TKT_LENGTHp1 ,             // 14: Last token type

 // These token types are not generated by <Tokenize_EM>
 //   hence, they never appear in a saved token stream,
 //   only as a parse token created during <ParseLine>.
 TKT_STRAND = TKT_LENGTHp1, // 14: Strand accumulating (data is LPTOKEN)
 TKT_LISTINT  ,             // 15: List intermediate (data is HGLOBAL)
 TKT_LISTPAR  ,             // 16: List in parens    (data is HGLOBAL)
 TKT_LISTBR   ,             // 17: List in brackets  (data is HGLOBAL)
 TKT_VARARRAY ,             // 18: Array of data (data is HGLOBAL)
 TKT_FCNARRAY ,             // 19: Array of functions and/or operators (data is HGLOBAL)
 TKT_FCNNAMED ,             // 1A: Symbol table entry for a named function (data is LPSYMENTRY)
 TKT_AXISIMMED,             // 1B: An immediate axis specification (data is immediate)
 TKT_AXISARRAY,             // 1C: An array of  ...   (data is HGLOBAL)
 TKT_OP1NAMED ,             // 1D: A named monadic primitive operator (data is LPSYMENTRY)
 TKT_OP2NAMED ,             // 1E: ...     dyadic  ...
 TKT_STRNAMED ,             // 1F: A named strand
                            // 20-FF:  Available entries (6 bits)
} TOKENTYPES;

#define TKT_LENGTH      (TKT_LENGTHp1 - 1)

// Whenever changing the above <enum> (TOKENTYPES), be sure to make a
//   corresponding change to <Untokenize> in <exec.c>,
//   <LookaheadDyadicOp>, <LookaheadAdjacent>, and <pl_yylex> in <parse.y>,
//   <MakeVarStrand_EM_YY> in <strand.c>,
//   <GetTokenTypeName> in <display.c>,
//   and <TokenTypeFV> in <primfns.c>.


typedef struct tagTKFLAGS
{
    UINT TknType:6,         // 0000003F:  Data token type (see TOKENTYPES)
         ImmType:4,         // 000003C0:  Type of immediate data (see IMMTYPES) (if .Type is TKT_VARIMMED/TKT_FCNIMMED)
         NoDisplay:1;       // 00000400:  Do not display this token
                            // FFFFF800:  Available bits
} TKFLAGS, *LPTKFLAGS;

typedef union tagTOKEN_DATA
{
    struct tagSYMENTRY *tkSym;      // 00:  Data is an LPSYMENTRY
    HGLOBAL    tkGlbData;           // 00:  ...     an HGLOBAL
    UINT       tkIndex;             // 00:  ...     an index
    APLBOOL    tkBoolean;           // 00:  ...     an APLBOOL
    APLINT     tkInteger;           // 00:  ...     an APLINT
    APLFLOAT   tkFloat;             // 00:  ...     a floating point number
    APLCHAR    tkChar;              // 00:  ...     an APLCHAR
    LPVOID     tkVoid;              // 00:  ...     an abritrary ptr
    APLLONGEST tkLongest;           // 00:  Longest datatype (so we can copy the entire data)
                                    // 08:  Length
} TOKEN_DATA, *LPTOKEN_DATA;

typedef struct tagTOKEN
{
    TKFLAGS          tkFlags;       // 00:  The flags part
    TOKEN_DATA       tkData;        // 04:  The data part
    int              tkCharIndex;   // 0C:  Index into the input line of this token
    struct tagTOKEN *lptkOrig;      // 10:  Ptr to original token
                                    // 14:  Length
} TOKEN, *LPTOKEN;

#define TOKEN_HEADER_SIGNATURE      'NKOT'

typedef struct tagTOKEN_HEADER
{
    UINT    Signature,              // 00:  Token Header signature
            Version,                // 04:  Version # of this header
            TokenCnt,               // 08:  # tokens in lpToken
            PrevGroup;              // 0C:  Index of the previous (to the left) grouping symbol
                                    //   (L/R paren, L/R bracket) where the index is relative
                                    //   to the first token after this header.
                                    // 10:  Length
} TOKEN_HEADER, *LPTOKEN_HEADER;

#define TokenBaseToStart(base)  (LPTOKEN) (((LPCHAR) base) + sizeof (TOKEN_HEADER))

typedef union tagUNION_TOKEN
{
    LPTOKEN        lpBase;          // 00:  Locked base of hGlbToken
    LPTOKEN_HEADER lpHeader;        // 00:  Token header
                                    // 04:  Length
} UNION_TOKEN, *LPUNION_TOKEN;


#define FSA_EXIT       -1       // FSA is done
#define FSA_NONCE      -2       // State not specified as yet
#define FSA_SYNTERR    -3       // A SYNTAX ERROR has occurred


//***************************************************************************
//  End of File: tokens.h
//***************************************************************************
