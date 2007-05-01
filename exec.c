//***************************************************************************
//  NARS2000 -- Interpreter
//***************************************************************************

#define STRICT
#define _WIN32_WINDOWS 0x0401   // Needed for CreateWaitableTimer, etc.
#include <windows.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

//#define EXEC_TRACE

/*

The input line is first tokenized so we can store
those partially digested values along with the WCHARs
of the input in function definition.  Of course, if
this is immediate execution, the tokenized line is
discarded after being parsed.

The tokenizer uses a Finite State Automata (FSA) to
do the conversion.

The parser then traverses the tokenized line executing
functions, etc. as necessary.

ToDo

* Control structures
* Bracket indexing
* Lists
*

 */

#define DEF_TOKEN_SIZE  1024    // Default initial amount of memory
                                //   allocated for the tokenized line
#define DEF_TOKEN_RESIZE 512    // Default increment when GlobalRealloc'ing

// Accumulation vars for constant integer, floating point, and string
APLINT  aplInteger;             // 8-byte Integers
BOOL    bNegative,              // Sign bit for integer part
        bNegExp;                // ...          exponent ...
int     iNumAlpLen,             // # chars in lpszNumAlp
        iStringLen;             // ...        lpwszString

// The order of the values of these constants *MUST* match the
//   column order in fsaColTable.
#define COL_FIRST 0             // It's origin-0

enum COL_INDICES
{COL_DIGIT = COL_FIRST, //  0: digit
 COL_DOT       ,        //  1: decimal number, inner & outer product separator
 COL_FPEXP     ,        //  2: floating point exponent separator
 COL_ALPHA     ,        //  3: alphabetic
 COL_Q_QQ      ,        //  4: quad
 COL_UNDERBAR  ,        //  5: underbar (infinity)
 COL_OVERBAR   ,        //  6: overbar
 COL_COMPLEX   ,        //  7: complex number separator
 COL_RATIONAL  ,        //  8: rational number separator
 COL_ASSIGN    ,        //  9: assignment symbol
 COL_SEMICOLON ,        // 10: semicolon symbol
 COL_PRIM_FN   ,        // 11: primitive monadic or dyadic function
 COL_PRIM_FN0  ,        // 12: ...       niladic function
 COL_PRIM_OP1  ,        // 13: ...       monadic operator
 COL_PRIM_OP2  ,        // 14: ...       dyadic  ...
 COL_JOT       ,        // 15: jot symbol
 COL_LPAREN    ,        // 16: left paren
 COL_RPAREN    ,        // 17: right ...
 COL_LBRACKET  ,        // 18: left bracket
 COL_RBRACKET  ,        // 19: right ...
 COL_SPACE     ,        // 20: White space (' ' or '\t')
 COL_QUOTE1    ,        // 21: single quote symbol
 COL_QUOTE2    ,        // 22: double ...
 COL_DIAMOND   ,        // 23: diamond symbol
 COL_LAMP      ,        // 24: comment symbol
 COL_EOL       ,        // 25: End-Of-Line
 COL_UNK       ,        // 26: unknown symbols

 COL_LENGTH    ,        // 27: # column indices (cols in fsaColTable) ***MUST*** BE THE LAST ENTRY
                        // Because these enums are origin-0, this value is the # valid columns.
};

// Whenever you add a new COL_*** define,
//   be sure to put code into CharTrans
//   to return the newly defined value.


// The order of the values of these constants *MUST* match the
//   row order in fsaColTable.
enum FSA_TOKENS
{FSA_INIT = 0,  // Initial state
 FSA_OVERBAR ,  // Overbar
 FSA_INTEGER ,  // Numeric integer
 FSA_BIGINT  ,  // Big numeric integer (from integer overflow in fnIntAccum)
 FSA_FPFRACT1,  // Floating point number, fractional part, 1st time
 FSA_FPFRACT2,  // ...                                     2nd & subsequent times
 FSA_FPEXP1  ,  // ...                    exponent   part, 1st time
 FSA_FPEXP2  ,  // ...                                     negative exponent
 FSA_FPEXP3  ,  // ...                                     2nd & subsequent times
 FSA_ALPHA   ,  // Alphabetic char
 FSA_SYSNAME ,  // System name
 FSA_QUOTE1A ,  // Start of or within single quoted char or char vector
 FSA_QUOTE1Z ,  // End of   ...
 FSA_QUOTE2A ,  // Start of or within double quoted char or char vector
 FSA_QUOTE2Z ,  // End of   ...
 FSA_DOTAMBIG,  // Ambiguous dot:  either FSA_FPFRACT2 or FSA_INIT w/fnOp2Done
 FSA_JOTAMBIG,  // Ambiguous jot:  either FSA_INIT w/fnOp2Done or FSA_OUTAMBIG
 FSA_OUTAMBIG,  // Ambiguous outer product:  either FSA_INIT w/fnOutDone or FSA_FPFRACT2 w/fnOp2Done

 FSA_LENGTH     // # FSA terminal states (rows in fsaColTable) ***MUST*** BE THE LAST ENTRY
                // Because these enums are origin-0, this value is the # valid FSA states.
};

char cComplexSep = 'j';             // Complex number separator (as well as uppercase)
                                    //   stored in var in case the user prefers 'i'

// Defined constant for ...
#define NO_PREVIOUS_GROUPING_SYMBOL -1

typedef struct tagTKLOCALVARS
{
    HGLOBAL     hGlbToken;          // Global memory handle
    UNION_TOKEN t2;                 // Locked base of hGlbToken
    LPTOKEN     lpStart,            // First available entry after the header
                lpNext,             // Next  ...
                lpLastEOS;          // Ptr to last EOS token
    int         State,              // Current state
                iNdex,              // ...     index into lpwszLine
                ActionNum;          // Action # (1 or 2)
    LPWCHAR     lpwszOrig,          // Ptr to original lpwszLine
                lpwsz;              // ...    current WCHAR in ...
    WCHAR       PrevWchar;          // Value of previous WCHAR
} TKLOCALVARS, *LPTKLOCALVARS;

typedef struct tagFSA_ACTION
{
    int  iNewState;
    BOOL (*fnAction1) (LPTKLOCALVARS);
    BOOL (*fnAction2) (LPTKLOCALVARS);
} FSA_ACTION;

#define fnIntInit   fnIntAccum
#define fnFrcInit   fnFPAccum
#define fnFrcAccum  fnFPAccum
#define fnFrcDone   fnFPDone
#define fnExpInit   fnFPAccum
#define fnExpAccum  fnFPAccum
#define fnExpDone   fnFPDone
#define fnBigDone   fnFPDone
#define fnBigAccum  fnFPAccum
#define fnAlpInit   fnAlpha
#define fnAlpAccum  fnAlpha
#define fnSysInit   fnAlpha
#define fnSysAccum  fnAlpha
#define fnSysDone   fnAlpDone
#define fnQuo1Init  fnQuoAccum
#define fnQuo2Init  fnQuoAccum
#define fnQuo1Accum fnQuoAccum
#define fnQuo2Accum fnQuoAccum
#define fnQuo1Done  fnQuoDone
#define fnQuo2Done  fnQuoDone

BOOL fnIntAccum   (LPTKLOCALVARS);
BOOL fnFPAccum    (LPTKLOCALVARS);
BOOL fnNegInit    (LPTKLOCALVARS);
BOOL fnNegExp     (LPTKLOCALVARS);
BOOL fnIntDone    (LPTKLOCALVARS);
BOOL fnFPDone     (LPTKLOCALVARS);
BOOL fnAlpha      (LPTKLOCALVARS);
BOOL fnAlpDone    (LPTKLOCALVARS);
BOOL fnSysName    (LPTKLOCALVARS);
BOOL fnAlpDone    (LPTKLOCALVARS);
BOOL fnPrmDone    (LPTKLOCALVARS);
BOOL fnComDone    (LPTKLOCALVARS);
BOOL fnQuoAccum   (LPTKLOCALVARS);
BOOL fnQuoDone    (LPTKLOCALVARS);
BOOL fnOp1Done    (LPTKLOCALVARS);
BOOL fnOp2Done    (LPTKLOCALVARS);
BOOL fnJotDone    (LPTKLOCALVARS);
BOOL fnOutDone    (LPTKLOCALVARS);
BOOL fnParInit    (LPTKLOCALVARS);
BOOL fnParDone    (LPTKLOCALVARS);
BOOL fnBrkInit    (LPTKLOCALVARS);
BOOL fnBrkDone    (LPTKLOCALVARS);
BOOL fnDiaDone    (LPTKLOCALVARS);
BOOL fnAsnDone    (LPTKLOCALVARS);
BOOL fnLstDone    (LPTKLOCALVARS);
BOOL fnInfinity   (LPTKLOCALVARS);

FSA_ACTION fsaColTable [][COL_LENGTH] =
{   // FSA_INIT     Initial state ('')
 {{FSA_INTEGER , NULL        , fnIntInit   },   // '0123456789'
  {FSA_DOTAMBIG, NULL        , fnFrcInit   },   // '.'
  {FSA_ALPHA   , NULL        , fnAlpInit   },   // 'E' or 'e'
  {FSA_ALPHA   , NULL        , fnAlpInit   },   // 'a..zA..Z'
  {FSA_SYSNAME , NULL        , fnSysInit   },   // Quad
  {FSA_INIT    , NULL        , fnInfinity  },   // Underbar
  {FSA_OVERBAR , NULL        , fnNegInit   },   // Overbar
  {FSA_ALPHA   , NULL        , fnAlpInit   },   // Complex separator
  {FSA_ALPHA   , NULL        , fnAlpInit   },   // 'R' or 'r'
  {FSA_INIT    , NULL        , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , NULL        , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , NULL        , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , NULL        , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , NULL        , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , NULL        , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, NULL        , NULL        },   // Jot
  {FSA_INIT    , NULL        , fnParInit   },   // Left paren
  {FSA_INIT    , NULL        , fnParDone   },   // Right ...
  {FSA_INIT    , NULL        , fnBrkInit   },   // Left bracket
  {FSA_INIT    , NULL        , fnBrkDone   },   // Right ...
  {FSA_INIT    , NULL        , NULL        },   // White space
  {FSA_QUOTE1A , NULL        , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , NULL        , fnQuo2Init  },   // Double ...
  {FSA_INIT    , NULL        , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , NULL        , fnComDone   },   // Comment symbol
  {FSA_EXIT    , NULL        , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_OVERBAR  Overbar
 {{FSA_INTEGER , NULL        , fnIntAccum  },   // '0123456789'
  {FSA_FPFRACT1, NULL        , fnFrcInit   },   // '.'
  {FSA_FPEXP1  , NULL        , fnExpInit   },   // 'E' or 'e'
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYSNAME , fnIntDone   , fnSysInit   },   // Quad
  {FSA_INIT    , NULL        , fnInfinity  },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator
  {FSA_NONCE   , NULL        , NULL        },   // 'R' or 'r'
  {FSA_INIT    , fnIntDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnIntDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnIntDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnIntDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnIntDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_NONCE   , fnIntDone   , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnIntDone   , NULL        },   // Jot
  {FSA_INIT    , fnIntDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnIntDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnIntDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnIntDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnIntDone   , NULL        },   // White space
  {FSA_QUOTE1A , fnIntDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnIntDone   , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnIntDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnIntDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnIntDone   , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_INTEGER  Number, integer part ('n')
 {{FSA_INTEGER , NULL        , fnIntAccum  },   // '0123456789'
  {FSA_FPFRACT1, NULL        , fnFrcInit   },   // '.'
  {FSA_FPEXP1  , NULL        , fnExpInit   },   // 'E' or 'e'
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYSNAME , fnIntDone   , fnSysInit   },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator
  {FSA_NONCE   , NULL        , NULL        },   // 'R' or 'r'
  {FSA_INIT    , fnIntDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnIntDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnIntDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnIntDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnIntDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_NONCE   , fnIntDone   , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnIntDone   , NULL        },   // Jot
  {FSA_INIT    , fnIntDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnIntDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnIntDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnIntDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnIntDone   , NULL        },   // White space
  {FSA_QUOTE1A , fnIntDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnIntDone   , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnIntDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnIntDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnIntDone   , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_BIGINT   Big number, integer part ('n')
 {{FSA_BIGINT  , NULL        , fnBigAccum  },   // '0123456789'
  {FSA_FPFRACT1, NULL        , fnFrcInit   },   // '.'
  {FSA_FPEXP1  , NULL        , fnExpInit   },   // 'E' or 'e'
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYSNAME , fnBigDone   , fnSysInit   },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator
  {FSA_NONCE   , NULL        , NULL        },   // 'R' or 'r'
  {FSA_INIT    , fnBigDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnBigDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnBigDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnBigDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnBigDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_NONCE   , fnBigDone   , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnBigDone   , NULL        },   // Jot
  {FSA_INIT    , fnBigDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnBigDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnBigDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnBigDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnBigDone   , NULL        },   // White space
  {FSA_QUOTE1A , fnBigDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnBigDone   , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnBigDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnBigDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnBigDone   , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_FPFRACT1 Floating Point Number, fractional part, 1st time ('n.')
 {{FSA_FPFRACT2, fnFrcAccum  , NULL        },   // '0123456789'
  {FSA_SYNTERR , NULL        , NULL        },   // '.'
  {FSA_FPEXP1  , fnExpInit   , NULL        },   // 'E' or 'e'
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYSNAME , fnFrcDone   , fnSysInit   },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator
  {FSA_NONCE   , NULL        , NULL        },   // 'R' or 'r'
  {FSA_INIT    , fnFrcDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnFrcDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnFrcDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnFrcDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnFrcDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_NONCE   , fnFrcDone   , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnFrcDone   , NULL        },   // Jot
  {FSA_INIT    , fnFrcDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnFrcDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnFrcDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnFrcDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnFrcDone   , NULL        },   // White space
  {FSA_QUOTE1A , fnFrcDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnFrcDone   , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnFrcDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnFrcDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnFrcDone   , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_FPFRACT2 Floating Point Number, fractional part, 2nd+ times ('n.n' or '.n')
 {{FSA_FPFRACT2, fnFrcAccum  , NULL        },   // '0123456789'
  {FSA_SYNTERR , NULL        , NULL        },   // '.'
  {FSA_FPEXP1  , fnExpInit   , NULL        },   // 'E' or 'e'
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYSNAME , fnFrcDone   , fnSysInit   },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator
  {FSA_NONCE   , NULL        , NULL        },   // 'R' or 'r'
  {FSA_INIT    , fnFrcDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnFrcDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnFrcDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnFrcDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnFrcDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_NONCE   , fnFrcDone   , NULL        },   // ..        dyadic  ...
  {FSA_JOTAMBIG, fnFrcDone   , NULL        },   // Jot
  {FSA_INIT    , fnFrcDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnFrcDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnFrcDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnFrcDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnFrcDone   , NULL        },   // White space
  {FSA_QUOTE1A , fnFrcDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnFrcDone   , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnFrcDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnFrcDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnFrcDone   , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_FPEXP1   Floating Point Number, exponent part, 1st time ('nE')
 {{FSA_FPEXP3  , fnExpAccum  , NULL        },   // '0123456789'
  {FSA_SYNTERR , NULL        , NULL        },   // '.'
  {FSA_SYNTERR , NULL        , NULL        },   // 'E' or 'e'
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYNTERR , NULL        , NULL        },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_FPEXP2  , fnNegExp    , NULL        },   // Overbar
  {FSA_SYNTERR , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_SYNTERR , NULL        , NULL        },   // 'R' or 'r'
  {FSA_SYNTERR , NULL        , NULL        },   // Assignment symbol
  {FSA_SYNTERR , NULL        , NULL        },   // Semicolon  ...
  {FSA_SYNTERR , NULL        , NULL        },   // Primitive monadic or dyadic function
  {FSA_SYNTERR , NULL        , NULL        },   // ...       niladic           ...
  {FSA_SYNTERR , NULL        , NULL        },   // ...       monadic operator
  {FSA_SYNTERR , NULL        , NULL        },   // ...       dyadic  ...
  {FSA_SYNTERR , NULL        , NULL        },   // Jot
  {FSA_SYNTERR , NULL        , NULL        },   // Left paren
  {FSA_SYNTERR , NULL        , NULL        },   // Right ...
  {FSA_SYNTERR , NULL        , NULL        },   // Left bracket
  {FSA_SYNTERR , NULL        , NULL        },   // Right ...
  {FSA_SYNTERR , NULL        , NULL        },   // White space
  {FSA_SYNTERR , NULL        , NULL        },   // Single quote
  {FSA_SYNTERR , NULL        , NULL        },   // Double ...
  {FSA_SYNTERR , NULL        , NULL        },   // Diamond symbol
  {FSA_SYNTERR , NULL        , NULL        },   // Comment symbol
  {FSA_SYNTERR , NULL        , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_FPEXP2   Floating Point Number, exponent part, negative exponent ('E-')
 {{FSA_FPEXP3  , fnExpAccum  , NULL        },   // '0123456789'
  {FSA_SYNTERR , NULL        , NULL        },   // '.'
  {FSA_SYNTERR , NULL        , NULL        },   // 'E' or 'e'
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYNTERR , NULL        , NULL        },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_SYNTERR , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_SYNTERR , NULL        , NULL        },   // Assignment symbol
  {FSA_SYNTERR , NULL        , NULL        },   // Semicolon  ...
  {FSA_SYNTERR , NULL        , NULL        },   // 'R' or 'r'
  {FSA_SYNTERR , NULL        , NULL        },   // Primitive monadic or dyadic function
  {FSA_SYNTERR , NULL        , NULL        },   // ...       niladic           ...
  {FSA_SYNTERR , NULL        , NULL        },   // ...       monadic operator
  {FSA_SYNTERR , NULL        , NULL        },   // ...       dyadic  ...
  {FSA_SYNTERR , NULL        , NULL        },   // Jot
  {FSA_SYNTERR , NULL        , NULL        },   // Left paren
  {FSA_SYNTERR , NULL        , NULL        },   // Right ...
  {FSA_SYNTERR , NULL        , NULL        },   // Left bracket
  {FSA_SYNTERR , NULL        , NULL        },   // Right ...
  {FSA_SYNTERR , NULL        , NULL        },   // White space
  {FSA_SYNTERR , NULL        , NULL        },   // Single quote
  {FSA_SYNTERR , NULL        , NULL        },   // Double ...
  {FSA_SYNTERR , NULL        , NULL        },   // Diamond symbol
  {FSA_SYNTERR , NULL        , NULL        },   // Comment symbol
  {FSA_SYNTERR , NULL        , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_FPEXP3   Floating Point Number, exponent part, 2nd+ times ('nEn' or 'nE-n')
 {{FSA_FPEXP3  , fnExpAccum  , NULL        },   // '0123456789'
  {FSA_SYNTERR , NULL        , NULL        },   // '.'
  {FSA_SYNTERR , NULL        , NULL        },   // 'E' or 'e'
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYSNAME , fnExpDone   , fnSysInit   },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_NONCE   , NULL        , NULL        },   // 'R' or 'r'
  {FSA_INIT    , fnExpDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnExpDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnExpDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnExpDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnExpDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_NONCE   , fnExpDone   , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnExpDone   , NULL        },   // Jot
  {FSA_INIT    , fnExpDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnExpDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnExpDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnExpDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnExpDone   , NULL        },   // White space
  {FSA_QUOTE1A , fnExpDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnExpDone   , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnExpDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnExpDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnExpDone   , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_ALPHA    Alphabetic char
 {{FSA_ALPHA   , fnAlpAccum  , NULL        },   // '0123456789'
  {FSA_DOTAMBIG, fnAlpDone   , fnFrcInit   },   // '.'
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // 'E' or 'e'
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // 'a..zA..Z'
  {FSA_SYSNAME , fnAlpDone   , fnSysInit   },   // Quad
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // Complex separator (iIjJ)
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // 'R' or 'r'
  {FSA_INIT    , fnAlpDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnAlpDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnAlpDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnAlpDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnAlpDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_NONCE   , fnAlpDone   , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnAlpDone   , NULL        },   // Jot
  {FSA_INIT    , fnAlpDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnAlpDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnAlpDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnAlpDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnAlpDone   , NULL        },   // White space
  {FSA_QUOTE1A , fnAlpDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnAlpDone   , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnAlpDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnAlpDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnAlpDone   , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_SYSNAME  System name (begins with a Quad or Quote-quad)
 {{FSA_SYSNAME , fnSysAccum  , NULL        },   // '0123456789'
  {FSA_DOTAMBIG, fnSysDone   , fnFrcInit   },   // '.'
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // 'E' or 'e'
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // 'a..zA..Z'
  {FSA_SYSNAME , fnSysDone   , fnSysInit   },   // Quad
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // Underbar
  {FSA_INTEGER , fnSysDone   , fnNegInit   },   // Overbar
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // Complex separator (iIjJ)
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // 'R' or 'r'
  {FSA_INIT    , fnSysDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnSysDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnSysDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnSysDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnSysDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_NONCE   , fnSysDone   , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnSysDone   , NULL        },   // Jot
  {FSA_INIT    , fnSysDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnSysDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnSysDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnSysDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnSysDone   , NULL        },   // White space
  {FSA_QUOTE1A , fnSysDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnSysDone   , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnSysDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnSysDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnSysDone   , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_QUOTE1A  Start of or within single quoted char or char vector
 {{FSA_QUOTE1A , fnQuo1Accum , NULL        },   // '0123456789'
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // '.'
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // 'E' or 'e'
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // 'a..zA..Z'
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Quad
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Underbar
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Overbar
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Complex separator (iIjJ)
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Assignment symbol
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Semicolon  ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // 'R' or 'r'
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Primitive monadic or dyadic function
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // ...       niladic           ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // ...       monadic operator
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // ...       dyadic  ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Jot
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Left paren
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Right ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Left bracket
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Right ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // White space
  {FSA_QUOTE1Z , NULL        , NULL        },   // Single quote
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Double ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Diamond symbol
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Comment symbol
  {FSA_SYNTERR , NULL        , NULL        },   // EOL
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Unknown symbols
 },
    // FSA_QUOTE1Z  End of single quoted char or char vector
 {{FSA_INTEGER , fnQuo1Done  , fnIntInit   },   // '0123456789'
  {FSA_DOTAMBIG, fnQuo1Done  , fnFrcInit   },   // '.'
  {FSA_ALPHA   , fnQuo1Done  , fnAlpInit   },   // 'E' or 'e'
  {FSA_ALPHA   , fnQuo1Done  , fnAlpInit   },   // 'a..zA..Z'
  {FSA_SYSNAME , fnQuo1Done  , fnSysInit   },   // Quad
  {FSA_INIT    , fnQuo1Done  , fnInfinity  },   // Underbar
  {FSA_INTEGER , fnQuo1Done  , fnNegInit   },   // Overbar
  {FSA_ALPHA   , fnQuo1Done  , fnAlpInit   },   // Complex separator
  {FSA_ALPHA   , fnQuo1Done  , fnAlpInit   },   // 'R' or 'r'
  {FSA_INIT    , fnQuo1Done  , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnQuo1Done  , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnQuo1Done  , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnQuo1Done  , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnQuo1Done  , fnOp1Done   },   // ...       monadic operator
  {FSA_NONCE   , fnQuo1Done  , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnQuo1Done  , NULL        },   // Jot
  {FSA_INIT    , fnQuo1Done  , fnParInit   },   // Left paren
  {FSA_INIT    , fnQuo1Done  , fnParDone   },   // Right ...
  {FSA_INIT    , fnQuo1Done  , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnQuo1Done  , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnQuo1Done  , NULL        },   // White space
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Single quote
  {FSA_QUOTE2A , fnQuo1Done  , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnQuo1Done  , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnQuo1Done  , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnQuo1Done  , NULL        },   // EOL
  {FSA_SYNTERR , fnQuo1Done  , NULL        },   // Unknown symbols
 },
    // FSA_QUOTE2A  Start of or within double quoted char or char vector
 {{FSA_QUOTE2A , fnQuo2Accum , NULL        },   // '0123456789'
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // '.'
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // 'E' or 'e'
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // 'a..zA..Z'
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Quad
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Underbar
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Overbar
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Complex separator (iIjJ)
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Assignment symbol
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Semicolon  ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // 'R' or 'r'
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Primitive monadic or dyadic function
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // ...       niladic           ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // ...       monadic operator
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // ...       dyadic  ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Jot
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Left paren
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Right ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Left bracket
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Right ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // White space
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Single quote
  {FSA_QUOTE2Z , NULL        , NULL        },   // Double ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Diamond symbol
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Comment symbol
  {FSA_SYNTERR , NULL        , NULL        },   // EOL
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Unknown symbols
 },
    // FSA_QUOTE2Z  End of double quoted char or char vector
 {{FSA_INTEGER , fnQuo2Done  , fnIntInit   },   // '0123456789'
  {FSA_DOTAMBIG, fnQuo2Done  , fnFrcInit   },   // '.'
  {FSA_ALPHA   , fnQuo2Done  , fnAlpInit   },   // 'E' or 'e'
  {FSA_ALPHA   , fnQuo2Done  , fnAlpInit   },   // 'a..zA..Z'
  {FSA_SYSNAME , fnQuo2Done  , fnSysInit   },   // Quad
  {FSA_INIT    , fnQuo2Done  , fnInfinity  },   // Underbar
  {FSA_INTEGER , fnQuo2Done  , fnNegInit   },   // Overbar
  {FSA_ALPHA   , fnQuo2Done  , fnAlpInit   },   // Complex separator
  {FSA_ALPHA   , fnQuo2Done  , fnAlpInit   },   // 'R' or 'r'
  {FSA_INIT    , fnQuo2Done  , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnQuo2Done  , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnQuo2Done  , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnQuo2Done  , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnQuo2Done  , fnOp1Done   },   // ...       monadic operator
  {FSA_NONCE   , fnQuo2Done  , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnQuo2Done  , NULL        },   // Jot
  {FSA_INIT    , fnQuo2Done  , fnParInit   },   // Left paren
  {FSA_INIT    , fnQuo2Done  , fnParDone   },   // Right ...
  {FSA_INIT    , fnQuo2Done  , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnQuo2Done  , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnQuo2Done  , NULL        },   // White space
  {FSA_QUOTE1A , fnQuo2Done  , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Double ...
  {FSA_INIT    , fnQuo2Done  , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnQuo2Done  , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnQuo2Done  , NULL        },   // EOL
  {FSA_SYNTERR , fnQuo2Done  , NULL        },   // Unknown symbols
 },
    // FSA_DOTAMBIG Ambiguous dot:  either FSA_FPFRACT2 or FSA_INIT w/fnOp2Done ('+.' or 'name.' or '[]name.')
 {{FSA_FPFRACT2, fnFrcAccum  , NULL        },   // '0123456789'
  {FSA_SYNTERR , NULL        , NULL        },   // '.'
  {FSA_ALPHA   , fnOp2Done   , fnAlpInit   },   // 'E' or 'e'
  {FSA_ALPHA   , fnOp2Done   , fnAlpInit   },   // 'a..zA..Z'
  {FSA_SYSNAME , fnOp2Done   , fnSysInit   },   // Quad
  {FSA_INIT    , fnOp2Done   , fnInfinity  },   // Underbar
  {FSA_INTEGER , fnOp2Done   , fnNegInit   },   // Overbar
  {FSA_ALPHA   , fnOp2Done   , fnAlpInit   },   // Complex separator
  {FSA_ALPHA   , fnOp2Done   , fnAlpInit   },   // 'R' or 'r'
  {FSA_INIT    , fnOp2Done   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnOp2Done   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnOp2Done   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnOp2Done   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnOp2Done   , fnOp1Done   },   // ...       monadic operator
  {FSA_SYNTERR , NULL        , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnOp2Done   , NULL        },   // Jot
  {FSA_INIT    , fnOp2Done   , fnParInit   },   // Left paren
  {FSA_INIT    , fnOp2Done   , fnParDone   },   // Right ...
  {FSA_INIT    , fnOp2Done   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnOp2Done   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnOp2Done   , NULL        },   // White space
  {FSA_QUOTE1A , fnOp2Done   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnOp2Done   , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnOp2Done   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnOp2Done   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnOp2Done   , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_JOTAMBIG Ambiguous jot:  either FSA_OUTAMBIG or normal w/fnJotDone ('J')
 {{FSA_INTEGER , fnJotDone   , fnIntAccum  },   // '0123456789'
  {FSA_OUTAMBIG, NULL        , NULL        },   // '.'
  {FSA_ALPHA   , fnJotDone   , fnAlpInit   },   // 'E' or 'e'
  {FSA_ALPHA   , fnJotDone   , fnAlpInit   },   // 'a..zA..Z'
  {FSA_SYSNAME , fnJotDone   , fnSysInit   },   // Quad
  {FSA_INIT    , fnJotDone   , fnInfinity  },   // Underbar
  {FSA_INTEGER , fnJotDone   , fnNegInit   },   // Overbar
  {FSA_ALPHA   , fnJotDone   , fnAlpInit   },   // Complex separator
  {FSA_ALPHA   , fnJotDone   , fnAlpInit   },   // 'R' or 'r'
  {FSA_INIT    , fnJotDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnJotDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnJotDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnJotDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnJotDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_SYNTERR , NULL        , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnJotDone   , NULL        },   // Jot
  {FSA_INIT    , fnJotDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnJotDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnJotDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnJotDone   , fnBrkDone   },   // Right ...
  {FSA_JOTAMBIG, NULL        , NULL        },   // White space
  {FSA_QUOTE1A , fnJotDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnJotDone   , fnQuo2Init  },   // Double ...
  {FSA_INIT    , fnJotDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnJotDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnJotDone   , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
    // FSA_OUTAMBIG Ambiguous outer product:  either FSA_INIT w/fnOutDone or FSA_FPFRACT2 w/fnOutDone ('J.')
 {{FSA_FPFRACT2, fnOutDone   , fnFrcAccum  },   // '0123456789'
  {FSA_SYNTERR , NULL        , NULL        },   // '.'
  {FSA_ALPHA   , fnOutDone   , fnAlpInit   },   // 'E' or 'e'
  {FSA_ALPHA   , fnOutDone   , fnAlpInit   },   // 'a..zA..Z'
  {FSA_SYSNAME , fnOutDone   , fnSysInit   },   // Quad
  {FSA_INIT    , fnOutDone   , fnInfinity  },   // Underbar
  {FSA_INTEGER , fnOutDone   , fnNegInit   },   // Overbar
  {FSA_ALPHA   , fnOutDone   , fnAlpInit   },   // Complex separator
  {FSA_ALPHA   , fnOutDone   , fnAlpInit   },   // 'R' or 'r'
  {FSA_INIT    , fnOutDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnOutDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnOutDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnOutDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_SYNTERR , NULL        , NULL        },   // ...       monadic operator
  {FSA_SYNTERR , NULL        , NULL        },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnOutDone   , NULL        },   // Jot
  {FSA_INIT    , fnOutDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnOutDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnOutDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnOutDone   , fnBrkDone   },   // Right ...
  {FSA_OUTAMBIG, NULL        , NULL        },   // White space
  {FSA_QUOTE1A , fnOutDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnOutDone   , fnQuo2Init  },   // Double ...
  {FSA_SYNTERR , NULL        , NULL        },   // Diamond symbol
  {FSA_SYNTERR , NULL        , NULL        },   // Comment symbol
  {FSA_SYNTERR , NULL        , NULL        },   // EOL
  {FSA_SYNTERR , NULL        , NULL        },   // Unknown symbols
 },
};


//***************************************************************************
//  ExecuteLine
//
//  Execute a line (sys/user command, fn defn, etc.)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecuteLine"
#else
#define APPEND_NAME
#endif

HGLOBAL ExecuteLine
    (UINT        uLineNum,      // Line #
     LPEXECSTATE esState,       // EXECSTATE enum
     HWND        hWndEC)        // Handle of Edit Control window

{
    LPWCHAR   lpwszCompLine,    // Ptr to complete line
              lpwszLine;
    HGLOBAL   hGlbToken;        // Handle of tokenized line
    UINT      uLinePos,         // Char position of start of line
              uLineLen;         // Line length
    HWND      hWndMC;           // Window handle of MDI Client

    // Get the window handle of the MDI Client
    hWndMC = GetParent (GetParent (hWndEC));

    // Ensure we calculated the lengths properly
    if (sizeof (fsaColTable) NE (COL_LENGTH * sizeof (FSA_ACTION) * FSA_LENGTH))
    {
        DbgStop ();             // We should never get here
    } // End IF

////if (0 !=  (sizeof (TOKEN_HEADER) % sizeof (TOKEN)))
////{
////    DbgMsg ("TOKEN_HEADER not properly packed");
////
////    return NULL;
////} // End IF

    // Get the position of the start of the line
    uLinePos = SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the line length
    uLineLen = SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Allocate virtual memory for the line (along with its continuations)
    lpwszCompLine = VirtualAlloc (NULL,             // Any address
                                  (uLineLen + 1) * sizeof (WCHAR),  // "+1" for the terminating zero
                                  MEM_COMMIT,
                                  PAGE_READWRITE);
    if (!lpwszCompLine)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ExecuteLine:  VirtualAlloc for <lpwszCompLine> failed");

        return NULL;        // Mark as failed
    } // End IF

    // Specify the maximum # chars in the buffer
    ((LPWORD) lpwszCompLine)[0] = uLineLen;

    // Get the contents of the line
    SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwszCompLine);

    // Ensure properly terminated
    lpwszCompLine[uLineLen] = L'\0';

    // Strip off leading blanks
    for (lpwszLine = lpwszCompLine;
         lpwszLine[0] && lpwszLine[0] EQ L' ';
         lpwszLine++)
    {};

    // Split off system commands
    if (esState->exType EQ EX_IMMEX)
    {
        switch (lpwszLine[0])
        {
            case ')':           // System commands
                // Execute the command
                if (!ExecSysCmd (lpwszLine))
                {
                    // ***FIXME*** -- Anything to do here??




                } // End IF

                hGlbToken = NULL;

                break;

            case ']':           // User commands
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
                // ***FIXME***
                DbgMsg ("User command");
#endif
                hGlbToken = NULL;

                break;

            case UTF16_DEL:     // Function definition
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
                // ***FIXME***
                DbgMsg ("Function definition");
#endif
                if (!CreateFcnWindow (lpwszLine, hWndMC))
                {
                    // ***FIXME*** -- Anything to do here??




                } // End IF

                hGlbToken = NULL;

                break;

            case UTF16_DELTILDE: // Locked function definition
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
                // ***FIXME***
                DbgMsg ("Locked function definition");
#endif
                hGlbToken = NULL;

                break;

            default:
                // Tokenize, parse, and untokenize the line
                hGlbToken = Tokenize_EM (lpwszCompLine);

                // If it's invalid, ...
                if (hGlbToken EQ NULL)
                    break;

                // Run the parser in a separate thread
                ParseLine (hGlbToken,
                           lpwszCompLine,
                           (HGLOBAL) GetProp (hWndMC, "PTD"),
                           TRUE);           // Parseline to free hGlbToken on exit
////////////////Untokenize (hGlbToken);
////////////////DbgGlobalFree (hGlbToken); hGlbToken = NULL;

                break;
        } // End SWITCH
    } else
    {
        // Tokenize and parse the line
        hGlbToken = Tokenize_EM (lpwszCompLine);
        ParseLine (hGlbToken,
                   lpwszCompLine,
                   (HGLOBAL) GetProp (hWndMC, "PTD"),
                   FALSE);                  // ParseLine NOT to free hGlbToken on exit
    } // End IF/ELSE

    // Free the virtual memory for the complete line
    VirtualFree (lpwszCompLine, 0, MEM_RELEASE); lpwszCompLine = NULL;

    return hGlbToken;
} // End ExecuteLine
#undef  APPEND_NAME


//***************************************************************************
//  UTLockAndSet
//
//  Lock and set the variables in a UNION_TOKEN
//***************************************************************************

void UTLockAndSet
    (HGLOBAL       hGlbToken,
     LPUNION_TOKEN lpUT)

{
    // Lock the handle and save the ptr
    lpUT->lpBase = MyGlobalLock (hGlbToken);
} // End UTLockAndSet


//***************************************************************************
//  UTRelockAndSet
//
//  Relock and set the variables in a UNION_TOKEN
//***************************************************************************

void UTRelockAndSet
    (HGLOBAL       hGlbToken,
     LPTKLOCALVARS lptkLocalVars)

{
    // Lock the handle and save the ptr
    UTLockAndSet (lptkLocalVars->hGlbToken, &lptkLocalVars->t2);
    lptkLocalVars->lpStart = TokenBaseToStart (lptkLocalVars->t2.lpBase);   // Skip over TOKEN_HEADER
    lptkLocalVars->lpNext  = &lptkLocalVars->lpStart[lptkLocalVars->t2.lpHeader->iTokenCnt];
} // End UTRelockAndSet

//***************************************************************************
//  InitAccumVars
//
//  Initialize the accumulation variables for the next constant
//***************************************************************************

void InitAccumVars
    (void)

{
    // Integers & floating points
////lpszNumAlp[0] = '\0';
////lpwszString[0] = L'\0';
    iNumAlpLen = 0;
    iStringLen = 0;
    aplInteger = 0;
    bNegative  =
    bNegExp    = FALSE;
} // End InitAccumVars


//***************************************************************************
//  NumAlpAccum
//
//  Accumulate into lpszNumAlp, checking for
//
//  The suffix _EM means that this function generates its own error message
//    so the caller doesn't need to.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- NumAlpAccum"
#else
#define APPEND_NAME
#endif

BOOL NumAlpAccum_EM
    (WCHAR wch)

{
    // In case we overflow, or convert to FP, save the chars
    // Check for overflow -- LIMIT ERROR
    if (iNumAlpLen < iMaxNumAlp)
    {
        lpszNumAlp[iNumAlpLen++] = (char) wch;

        return TRUE;
    } else
    {
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

        return FALSE;
    } // End IF/ELSE
} // End NumAlpAccum_EM
#undef  APPEND_NAME


//***************************************************************************
//  fnIntAccum
//
//  Start of or next digit in integer number
//***************************************************************************

BOOL fnIntAccum
    (LPTKLOCALVARS lptkLocalVars)

{
    WCHAR  wch;
    APLINT aplInt;
    BOOL   bRet = TRUE;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnIntAccum");
#endif

    // Copy current WCHAR
    wch = *lptkLocalVars->lpwsz;

    // Shift over by one digit
    // We'd like to do the following, but C has no way to
    //   detect integer overflow for the multiplication.
////aplInteger *= 10;
////aplInteger += wch - L'0';
    //   so, instead, we do the following:
    aplInt = imul64 (aplInteger, 10, &bRet);
    if (!bRet)
////aplInt = aplInteger * 10;
////if (aplInteger NE (aplInt / 10)
//// || aplInt < 0)
    {
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
        DbgMsg ("Integer overflow!");
#endif
        lptkLocalVars->State = FSA_BIGINT;
    } else
    {
        aplInteger = aplInt + (wch - L'0');

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
        { // ***DEBUG***
            wsprintf (lpszTemp,
                      "aplInteger = %08X%08X",
                      HIDWORD (aplInteger),
                      LODWORD (aplInteger));
            DbgMsg (lpszTemp);
        } // ***DEBUG*** END
#endif
    } // End IF/ELSE

    return NumAlpAccum_EM (wch);
} // End fnIntAccum


//***************************************************************************
//  fnIntDone
//
//  Integer constant -- save as token
//***************************************************************************

BOOL fnIntDone
    (LPTKLOCALVARS lptkLocalVars)

{
    BOOL    bRet = TRUE;
    TKFLAGS tkFlags = {0};

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnIntDone");
#endif

    // Ensure properly terminated
    lpszNumAlp[iNumAlpLen] = '\0';

    // Include the sign bit
    // (N.B.  can't overflow as there are more negative #s than positive #s)
    if (bNegative)
        aplInteger = -aplInteger;

    // Mark as an immediate Boolean/integer
    tkFlags.TknType = TKT_VARIMMED;

    if (IsBooleanValue (aplInteger))
        tkFlags.ImmType = IMMTYPE_BOOL;
    else
        tkFlags.ImmType = IMMTYPE_INT;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplInteger,
                              -iNumAlpLen);
    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    return bRet;
} // End fnIntDone


//***************************************************************************
//  fnFPAccum
//
//  Start of or next digit in floating point number, fractional part.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnFPAccum"
#else
#define APPEND_NAME
#endif

BOOL fnFPAccum
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnFPAccum");
#endif

    // Check for overflow -- LIMIT ERROR
    if (iNumAlpLen < iMaxNumAlp)
        lpszNumAlp[iNumAlpLen++] = (char) *lptkLocalVars->lpwsz;
    else
    {
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

        return FALSE;
    } // End IF/ELSE

    return TRUE;
} // End fnFPAccum
#undef  APPEND_NAME


//***************************************************************************
//  fnNegInit
//
//  Start of negative number, integer part,
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnNegInit"
#else
#define APPEND_NAME
#endif

BOOL fnNegInit
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnNegInit");
#endif

    bNegative = TRUE;

    // Check for overflow -- LIMIT ERROR
    if (iNumAlpLen >= iMaxNumAlp)
    {
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

        return FALSE;
    } // End IF/ELSE
    else
        lpszNumAlp[iNumAlpLen++] = '-';

    return TRUE;
} // End fnNegInit
#undef  APPEND_NAME


//***************************************************************************
//  fnNegExp
//
//  Start of negative number, exponent part,
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnNegExp"
#else
#define APPEND_NAME
#endif

BOOL fnNegExp
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnNegExp");
#endif

    bNegExp = TRUE;

    // Check for overflow -- LIMIT ERROR
    if (iNumAlpLen >= iMaxNumAlp)
    {
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

        return FALSE;
    } // End IF/ELSE
    else
        lpszNumAlp[iNumAlpLen++] = L'-';
////    lpszNumAlp[iNumAlpLen++] = (char) *lptkLocalVars->lpwsz;

    return TRUE;
} // End fnNegExp
#undef  APPEND_NAME


//***************************************************************************
//  fnFPDone
//
//  Floating point constant -- save as token
//***************************************************************************

BOOL fnFPDone
    (LPTKLOCALVARS lptkLocalVars)

{
    BOOL       bRet = TRUE;
    APLFLOAT   aplFloat;
////APLFLOAT   aplDiv, aplExp;
    TKFLAGS    tkFlags = {0};
////int        i;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnFPDone");
#endif

    // Ensure properly terminated
    lpszNumAlp[iNumAlpLen] = '\0';

    // ***FIXME*** -- Is this whole approach very accurate??

////// Loop through the digits
////for (i = (lpszNumAlp[0] EQ '-'), aplFloat = 0;
////     lpszNumAlp[i] NE '\0'
////  && lpszNumAlp[i] NE '.'
////  && lpszNumAlp[i] NE 'e'
////  && lpszNumAlp[i] NE 'E';
////     i++)
////    aplFloat = (aplFloat * 10) + (lpszNumAlp[i] - '0');
////
////if (lpszNumAlp[i] EQ '.')
////for (i = i + 1, aplDiv = 10;
////     i < iNumAlpLen
////  && lpszNumAlp[i] NE 'e'
////  && lpszNumAlp[i] NE 'E';
////     i++, aplDiv *= 10)
////    aplFloat += (lpszNumAlp[i] - '0') / aplDiv;
////
////// Check for exponent
////if (lpszNumAlp[i] EQ 'e'
//// || lpszNumAlp[i] EQ 'E')
////{
////    for (i = i + 1 + (lpszNumAlp[i + 1] EQ L'-'), aplExp = 0;
////         i < iNumAlpLen;
////         i++)
////        aplExp = (aplExp * 10) + (lpszNumAlp[i] - '0');
////
////    if (bNegExp)
////        aplExp = -aplExp;
////
////    aplFloat *= pow (10, aplExp);
////} // End IF
////
////// Include the mantissa's sign
////if (bNegative)
////    aplFloat = -aplFloat;
////
////Assert (aplFloat EQ atof (lpszNumAlp));

    aplFloat = atof (lpszNumAlp);

    // Mark as an immediate float or Boolean
    tkFlags.TknType = TKT_VARIMMED;
    if (aplFloat EQ 0)
        tkFlags.ImmType = IMMTYPE_BOOL;
    else
        tkFlags.ImmType = IMMTYPE_FLOAT;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              (LPAPLLONGEST) &aplFloat,
                              -iNumAlpLen);
    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    return bRet;
} // End fnFPDone


//***************************************************************************
//  fnAlpha
//
//  Start of or next char in name
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnAlpha"
#else
#define APPEND_NAME
#endif

BOOL fnAlpha
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnAlpha");
#endif

    // Check for overflow -- LIMIT ERROR
    if (iStringLen < iMaxString)
        // Save the char
        lpwszString[iStringLen++] = *lptkLocalVars->lpwsz;
    else
    {
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

        return FALSE;
    } // End IF/ELSE

    return TRUE;
} // End fnAlpha
#undef  APPEND_NAME


//***************************************************************************
//  fnAlpDone
//
//  End of name
//***************************************************************************

BOOL fnAlpDone
    (LPTKLOCALVARS lptkLocalVars)

{
    LPSYMENTRY lpSymEntry;
    BOOL       bRet = TRUE;
    APLINT     aplInteger;
    TKFLAGS    tkFlags = {0};

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnAlpDone");
#endif

    // Ensure properly terminated
    lpwszString[iStringLen] = '\0';

    // If this is a system name (starts with a Quad),
    //   convert it to lowercase as those names are
    //   case-insensitive
    if (lpwszString[0] EQ UTF16_QUAD
     || lpwszString[0] EQ UTF16_QUOTEQUAD)
    {
        // Handle Quad and QuoteQuad via a separate token
        if (iStringLen EQ 1)
        {
            // Mark the data as Quad or QuoteQuad
            tkFlags.TknType = TKT_INPOUT;

            // Copy to local var so we may pass its address
            aplInteger = lpwszString[0];

            // Attempt to append as new token, check for TOKEN TABLE FULL,
            //   and resize as necessary.
            bRet = AppendNewToken_EM (lptkLocalVars,
                                      &tkFlags,
                                      &aplInteger,
                                      -iStringLen);
            goto NORMAL_EXIT;
        } else
            CharLowerW (&lpwszString[1]);
    } // End IF

    // Lookup in or append to the symbol table
    lpSymEntry = SymTabAppendName_EM (lpwszString);
    if (lpSymEntry)
    {
        // Mark the data as a symbol table entry
        tkFlags.TknType = TKT_VARNAMED;

        // Copy to local var so we may pass its address
        aplInteger = MakeSymType (lpSymEntry);

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        bRet = AppendNewToken_EM (lptkLocalVars,
                                  &tkFlags,
                                  &aplInteger,
                                  -iStringLen);
    } else
        bRet = FALSE;
NORMAL_EXIT:
    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    return bRet;
} // End fnAlpDone


//***************************************************************************
//  fnAsnDone
//
//  Start and end of an assignment (monadic or dyadic)
//***************************************************************************

BOOL fnAsnDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS     tkFlags = {0};
    BOOL        bRet = TRUE;
    APLLONGEST  aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnAsnDone");
#endif

    // Copy current WCHAR
    aplLongest = *lptkLocalVars->lpwsz;

    // Mark the data as an assignment
    tkFlags.TknType = TKT_ASSIGN;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              0);
    return bRet;
} // End fnAsnDone


//***************************************************************************
//  fnLstDone
//
//  Start and end of a list
//***************************************************************************

BOOL fnLstDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS     tkFlags = {0};
    BOOL        bRet = TRUE;
    APLLONGEST  aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnLstDone");
#endif

    // Copy current WCHAR
    aplLongest = *lptkLocalVars->lpwsz;

    // Mark the data as an assignment
    tkFlags.TknType = TKT_LISTSEP;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              0);
    return bRet;
} // End fnLstDone


//***************************************************************************
//  fnPrmDone
//
//  Start and end of a primitive function (niladic, monadic, or dyadic)
//***************************************************************************

BOOL fnPrmDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS tkFlags = {0};
    BOOL    bRet = TRUE;
    APLINT  aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnPrmDone");
#endif

    // Copy current WCHAR
    aplInteger = *lptkLocalVars->lpwsz;

    // For reasons which escape me now, this is
    //   where we handle {zilde}
    if (aplInteger EQ UTF16_ZILDE)
    {
        // Mark the data as a variable
        tkFlags.TknType   = TKT_VARARRAY;
////////tkFlags.ImmType   = 0;      // Already zero from {0}
////////tkFlags.NoDisplay = 0;      // Already zero from {0}
        aplInteger        = MakeGlbType (hGlbZilde);
    } else
    {
        // Mark the data as a primitive function
        tkFlags.TknType = TKT_FCNIMMED;
        tkFlags.ImmType = GetImmTypeFcn (*lptkLocalVars->lpwsz);
    } // End IF/ELSE

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplInteger,
                              0);
    return bRet;
} // End fnPrmDone


//***************************************************************************
//  fnInfinity
//
//  Infinity symbol (_)
//***************************************************************************

BOOL fnInfinity
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS tkFlags = {0};
    BOOL    bRet = TRUE;
    APLFLOAT aplFloat;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnInfinity");
#endif

    // Mark the data as an immediate floating point variable
    tkFlags.TknType = TKT_VARIMMED;
    tkFlags.ImmType = IMMTYPE_FLOAT;
    if (bNegative)
        aplFloat = NegInfinity;
    else
        aplFloat = PosInfinity;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              (LPAPLLONGEST) &aplFloat,
                              0);
    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    return bRet;
} // End fnInfinity


//***************************************************************************
//  fnOp1Done
//
//  End of a monadic primitive operator
//***************************************************************************

BOOL fnOp1Done
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS     tkFlags = {0};
    BOOL        bRet = TRUE;
    WCHAR       wch;
    APLLONGEST  aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnOp1Done");
#endif

    // Copy current WCHAR
    wch = *lptkLocalVars->lpwsz;

    // If this is a slash/slope and the preceding token is
    //   a left paren or a dyadic operator, convert this
    //   symbol into a primitive function
    if ((wch EQ '/'
      || wch EQ '\\'
      || wch EQ UTF16_SLASHBAR
      || wch EQ UTF16_SLOPEBAR)
     && lptkLocalVars->lpStart NE lptkLocalVars->lpNext
     && (lptkLocalVars->lpNext[-1].tkFlags.TknType EQ TKT_LPAREN
      || lptkLocalVars->lpNext[-1].tkFlags.TknType EQ TKT_OP2IMMED))
        return fnPrmDone (lptkLocalVars);

    // Mark the data as a monadic primtive operator
    tkFlags.TknType = TKT_OP1IMMED;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    aplLongest = wch;
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              0);
    return bRet;
} // End fnOp1Done


//***************************************************************************
//  fnOp2Done
//
//  End of a dyadic primitive operator
//***************************************************************************

BOOL fnOp2Done
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS     tkFlags = {0};
    BOOL        bRet = TRUE;
    APLLONGEST  aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnOp2Done");
#endif

    // Mark the data as a dyadic primtive operator
    tkFlags.TknType = TKT_OP2IMMED;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    aplLongest = lptkLocalVars->PrevWchar;
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              -1);
    return bRet;
} // End fnOp2Done


//***************************************************************************
//  fnJotDone
//
//  End of a jot as a dyadic primitive operator
//***************************************************************************

BOOL fnJotDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS tkFlags = {0};
    BOOL    bRet = TRUE;
    APLINT  aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnJotDone");
#endif

    // Mark the data as a dyadic primtive operator
    tkFlags.TknType = TKT_OP2IMMED;

    // Copy to local var so we may pass its address
    aplInteger = UTF16_JOT;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplInteger,
                              -1);
    return bRet;
} // End fnJotDone


//***************************************************************************
//  fnOutDone
//
//  End of a jot dot as an outer product monadic primitive operator
//***************************************************************************

BOOL fnOutDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS tkFlags = {0};
    BOOL    bRet = TRUE;
    APLINT  aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnOutDone");
#endif

    // Mark the data as an outer product monadic primtive operator (with right scope)
    tkFlags.TknType = TKT_JOTDOT;

    // Copy to local var so we may pass its address
    aplInteger = UTF16_JOT;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplInteger,
                              -1);
    return bRet;
} // End fnOutDone


//***************************************************************************
//  fnComDone
//
//  Start (and end) of a comment
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnComDone"
#else
#define APPEND_NAME
#endif

BOOL fnComDone
    (LPTKLOCALVARS lptkLocalVars)

{
    int     iLen, iLen2;
    HGLOBAL hGlb;
    BOOL    bRet = TRUE;
    APLINT  aplInteger;
    LPWCHAR lpwch;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnComDone");
#endif

    // Get the length of the comment (up to but not including any '\n')
    iLen  = lstrlenW (lptkLocalVars->lpwsz); // Including the comment symbol
    lpwch = strchrW (lptkLocalVars->lpwsz, L'\n');
    if (lpwch)
    {
        iLen2 = lpwch - lptkLocalVars->lpwsz;
        iLen  = min (iLen2, iLen);
    } // End IF

    // Skip over the comment in the input stream
    // "-1" because the FOR loop in Tokenize_EM will
    //   increment it, too
    lptkLocalVars->iNdex += iLen - 1;

    // Allocate global memory for the comment ("+1" for the terminating zero)
    hGlb = DbgGlobalAlloc (GHND, (iLen + 1) * sizeof (WCHAR));
    if (!hGlb)
    {
        ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

        bRet = FALSE;
    } else
    {
        LPWCHAR lpwsz;
        TKFLAGS tkFlags = {0};

        // Lock the handle and copy the comment to the global memory
        lpwsz = MyGlobalLock (hGlb);
        lstrcpyW (lpwsz, lptkLocalVars->lpwsz);
        MyGlobalUnlock (hGlb); lpwsz = NULL;

        // Mark the data as a comment in a global memory handle
        tkFlags.TknType = TKT_COMMENT;

        // Copy to local var so we may pass its address
        aplInteger = MakeGlbType (hGlb);

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        bRet = AppendNewToken_EM (lptkLocalVars,
                                  &tkFlags,
                                  &aplInteger,
                                  0);
    } // End IF

    return bRet;
} // End fnComDone
#undef  APPEND_NAME


//***************************************************************************
//  fnQuoAccum
//
//  Start of or next char in a string
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnQuoAccum"
#else
#define APPEND_NAME
#endif

BOOL fnQuoAccum
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnQuoAccum");
#endif

    // Check for overflow -- LIMIT ERROR
    if (iStringLen < iMaxString)
        lpwszString[iStringLen++] = *lptkLocalVars->lpwsz;
    else
    {
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

        return FALSE;
    } // End IF/ELSE

    return TRUE;
} // End fnQuoAccum
#undef  APPEND_NAME


//***************************************************************************
//  fnQuoDone
//
//  End of a char or char vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnQuoDone"
#else
#define APPEND_NAME
#endif

BOOL fnQuoDone
    (LPTKLOCALVARS lptkLocalVars)

{
    HGLOBAL hGlb;
    BOOL    bRet = TRUE;
    TKFLAGS tkFlags = {0};
    APLINT  aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnQuoDone");
#endif
    // Ensure properly terminated
    lpwszString[iStringLen] = L'\0';

    // The initial FSA action for a string is to store the leading
    //   delimiter, that is a single or double quote, so we can
    //   tell the difference between the two at this point.
    // At the moment, I have no use for this information, but I'd
    //   like to leave the initial action as is and just skip over
    //   the first character in the saved string.  Note that this
    //   means the string length is one too big.

    // Take care of the string length
    iStringLen--;                   // Count out the leading string delimiter

    // If this string is of length zero, then store it as an empty vector
    if (iStringLen EQ 0)
    {
        // Mark the data as a string in a global memory handle
        tkFlags.TknType = TKT_STRING;

        // Copy to local var so we may pass its address
        aplInteger = MakeGlbType (hGlbMTChar);

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        bRet = AppendNewToken_EM (lptkLocalVars,
                                  &tkFlags,
                                  &aplInteger,
                                  0);
    } else
    // If this string is of length one, then store it as a char scalar
    if (iStringLen EQ 1)
    {
        // Mark the data as an immediate chracter
        tkFlags.TknType = TKT_VARIMMED;
        tkFlags.ImmType = IMMTYPE_CHAR;

        // Copy to local var so we may pass its address
        aplInteger = lpwszString[1];

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        bRet = AppendNewToken_EM (lptkLocalVars,
                                  &tkFlags,
                                  &aplInteger,
                                  -iStringLen);
    } else
    {
        // Allocate global memory for the array header,
        //   one dimension (it's a vector), and the string
        //   excluding the terminating zero.
        hGlb = DbgGlobalAlloc (GHND, (UINT) CalcArraySize (ARRAY_CHAR, iStringLen, 1));
        if (!hGlb)
        {
            ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

            bRet = FALSE;
        } else
        {
            LPWCHAR lpwsz;

            // Lock the handle, setup the header, and copy
            //   the string to the global memory
            lpwsz = MyGlobalLock (hGlb);

#define lpHeader    ((LPVARARRAY_HEADER) lpwsz)

            lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = ARRAY_CHAR;
////        lpHeader->Perm       = 0;
////        lpHeader->SysVar     = 0;
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = iStringLen;
            lpHeader->Rank       = 1;

#undef  lpHeader

            *VarArrayBaseToDim (lpwsz) = iStringLen;
            CopyMemory (VarArrayBaseToData (lpwsz, 1),
                        &lpwszString[1],    // Skip over the string delimiter
                        iStringLen * sizeof (APLCHAR));
            MyGlobalUnlock (hGlb); lpwsz = NULL;

            // Mark the data as a string in a global memory handle
            tkFlags.TknType = TKT_STRING;

            // Copy to local var so we may pass its address
            aplInteger = MakeGlbType (hGlb);

            // Attempt to append as new token, check for TOKEN TABLE FULL,
            //   and resize as necessary.
            bRet = AppendNewToken_EM (lptkLocalVars,
                                      &tkFlags,
                                      &aplInteger,
                                      -iStringLen);
        } // End IF/ELSE
    } // End IF

    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    return bRet;
} // End fnQuoDone
#undef  APPEND_NAME


//***************************************************************************
//  fnParInit
//
//  Left end (start) of a parenthetical expression
//***************************************************************************

BOOL fnParInit
    (LPTKLOCALVARS lptkLocalVars)

{

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnParInit");
#endif

    return GroupInitCom (lptkLocalVars, TKT_LPAREN);
} // End fnParInit


//***************************************************************************
//  fnBrkInit
//
//  Left end (start) of a bracketed expression
//***************************************************************************

BOOL fnBrkInit
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnBrkInit");
#endif

    return GroupInitCom (lptkLocalVars, TKT_LBRACKET);
} // End fnBrkInit


//***************************************************************************
//  GroupInitCom
//
//  Group (Left paren/bracket) common initialization
//
//  We maintain a linked list in the token stream to allow us to detect
//    mismatched or improperly nested grouping symbols (parens and brackets
//    -- we could easily add braces or even left/right-tacks, if needed).
//***************************************************************************

BOOL GroupInitCom
    (LPTKLOCALVARS lptkLocalVars,
     UINT          uType)

{
    TKFLAGS     tkFlags = {0};
    BOOL        bRet = TRUE;
    LPTOKEN     lpNext;
    APLLONGEST  aplLongest;

    // Mark the data as a left grouping symbol
    tkFlags.TknType = uType;

    // Copy the address of the token we're about to create
    lpNext = lptkLocalVars->lpNext;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    aplLongest = lptkLocalVars->t2.lpHeader->iPrevGroup;
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              0);
    // Save as location of previous left grouping symbol
    lptkLocalVars->t2.lpHeader->iPrevGroup = lpNext - lptkLocalVars->lpStart;

    return bRet;
} // End GroupInitCom


//***************************************************************************
//  fnParDone
//
//  Right end (stop) of a parenthetical expression
//***************************************************************************

BOOL fnParDone
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnParDone");
#endif

    return GroupDoneCom (lptkLocalVars, TKT_RPAREN, TKT_LPAREN);
} // End fnParDone


//***************************************************************************
//  fnBrkDone
//
//  Right end (stop) of a bracketed expression
//***************************************************************************

BOOL fnBrkDone
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnBrkDone");
#endif

    return GroupDoneCom (lptkLocalVars, TKT_RBRACKET, TKT_LBRACKET);
} // End fnBrkDone


//***************************************************************************
//  GroupDoneCom
//
//  Group (Right paren/bracket) common ending
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GroupDoneCom"
#else
#define APPEND_NAME
#endif

BOOL GroupDoneCom
    (LPTKLOCALVARS lptkLocalVars,
     TOKEN_TYPES   uTypeCurr,
     TOKEN_TYPES   uTypePrev)

{
    int         iPrevGroup;
    BOOL        bRet = TRUE;
    APLLONGEST  aplLongest;

    // Get the index of the previous grouping symbol
    iPrevGroup = lptkLocalVars->t2.lpHeader->iPrevGroup;

    // Ensure proper nesting
    if (iPrevGroup EQ NO_PREVIOUS_GROUPING_SYMBOL
     || lptkLocalVars->lpStart[iPrevGroup].tkFlags.TknType NE (UINT) uTypePrev)
    {
        ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

        bRet = FALSE;
    } else
    {
        LPTOKEN lpNext;
        TKFLAGS tkFlags = {0};

        // Mark the data as a right grouping symbol
        tkFlags.TknType = uTypeCurr;

        // Copy the address of the token we're about to create
        lpNext = lptkLocalVars->lpNext;

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        aplLongest = iPrevGroup;
        bRet = AppendNewToken_EM (lptkLocalVars,
                                  &tkFlags,
                                  &aplLongest,
                                  0);
        // Save index of previous grouping symbol
        lptkLocalVars->t2.lpHeader->iPrevGroup = lptkLocalVars->lpStart[iPrevGroup].tkData.tkIndex;
    } // End IF/ELSE

    return bRet;
} // End GroupDoneCom
#undef  APPEND_NAME


//***************************************************************************
//  fnDiaDone
//
//  Done with this stmt
//***************************************************************************

BOOL fnDiaDone
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnDiaDone");
#endif

    // Test for mismatched or improperly nested grouping symbols
    if (!CheckGroupSymbols_EM (lptkLocalVars))
        return FALSE;

    // Append the EOS token
    return AppendEOSToken (lptkLocalVars, TRUE);
} // End fnDiaDone


//***************************************************************************
//  ErrorMessageIndirect
//
//  Signal an error message, indirectly
//***************************************************************************

void ErrorMessageIndirect
    (LPWCHAR lpwszMsg)

{
    // Save in global for later reference
    lpwszErrorMessage = lpwszMsg;
} // End ErrorMessageIndirect


//***************************************************************************
//  IncorrectCommand
//
//  Signal an incorrect command
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- IncorrectCommand"
#else
#define APPEND_NAME
#endif

void IncorrectCommand
    (void)

{
    AppendLine (ERRMSG_INCORRECT_COMMAND APPEND_NAME, FALSE, TRUE);
} // End IncorrectCommand
#undef  APPEND_NAME


//***************************************************************************
//  ErrorMessage
//
//  Signal an error message
//***************************************************************************

void ErrorMessage
    (LPWCHAR lpwszMsg,
     LPWCHAR lpwszLine,
     int     iCaret)            // Position of caret (origin-0)

{
    // ***FIXME*** -- put this into []DM and then signal an error
    //                which should execute []ELX which will then
    //                display this text (if []ELX is '[]DM')

////#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    { // ***DEBUG***
        AppendLine (lpwszMsg, FALSE, TRUE);
        AppendLine (lpwszLine, FALSE, TRUE);

        // If the caret is not -1, display a caret
        if (iCaret NE -1)
        {
            int i;

            for (i = 0; i < iCaret; i++)
                lpwszTemp[i] = L' ';
            lpwszTemp[i] = UTF16_UPCARET;
            lpwszTemp[i + 1] = '\0';

            AppendLine (lpwszTemp, FALSE, TRUE);
        } // End IF
    } // ***DEBUG*** END
////#endif
} // End ErrorMessage


//***************************************************************************
//  WaitForInput
//
//  Wait for either Quad or Quote-quad input
//***************************************************************************

LPYYSTYPE WaitForInput
    (HWND    hWnd,                  // Window handle to Session Manager
     BOOL    bQuoteQuadInput,       // TRUE iff Quote-Quad input (FALSE if Quad input)
     LPTOKEN lptkFunc)              // Ptr to "function" token

{
    HANDLE        hTimer;
    char          szTimer[32];
    LARGE_INTEGER waitTime;
    DWORD         dwWaitRes = WAIT_TIMEOUT;
    LPYYSTYPE     lpYYRes;

    DbgBrk ();

    // Create a unique name for the timer object
    wsprintf (szTimer, "%s%08X", bQuoteQuadInput ? "QuoteQuadInput" : "QuadInput", hWnd);

    // Create a timer object
    hTimer = CreateWaitableTimer (NULL,         // No SECURITY_ATTRIBUTES
                                  FALSE,        // ***FIXME*** -- Is this correct??
                                  szTimer);
    // Set the wait time to a large integer
    waitTime.QuadPart = -1;

    SetWaitableTimer (hTimer,           // Handle to timer object
                      &waitTime,        // Ptr to wait time
                      0L,               // 0 = not periodic
                      NULL,             // No completion routine
                      NULL,             // No arg to completion routine
                      TRUE);            // Resume from suspended state

    while (dwWaitRes EQ WAIT_TIMEOUT)
        dwWaitRes = WaitForSingleObject (hTimer,    // Handle to timer object
                                         10000);    // 10000 x 1 millisecond = 10 second

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
  //lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);     // ***FIXME***
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End WaitForInput


//***************************************************************************
//  Tokenize_EM
//
//  Tokenize a line of input
//
//  The suffix _EM means that this function generates its own error message
//    so the caller doesn't need to.
//***************************************************************************
/*

A tokenized line consists of a count and length, followed by
a series of one or two tokens.

A single token consists of flags.
A double token consists of flags preceded by data.

Double tokens have the data precede the flags because the
tokenized line is parsed from right to left, so the flags
indicate whether or not there is a preceding data token.

The flags are defined in tokens.h

*/

#ifdef DEBUG
#define APPEND_NAME     L" -- Tokenize_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL Tokenize_EM
    (LPWCHAR lpwszLine)

{
    int         i, iLen;
    WCHAR       wchOrig,        // The original char
                wchColNum;      // The translated char for tokenization as a COL_*** value
    BOOL        (*fnAction1) (LPTKLOCALVARS);
    BOOL        (*fnAction2) (LPTKLOCALVARS);
    TKLOCALVARS tkLocalVars;    // Local vars
    BOOL        bFreeGlb = FALSE;
    APLINT      aplInteger;

    // Save local vars in struct which we pass to each FSA action routine
    tkLocalVars.State = FSA_INIT;

    // Allocate some memory for the tokens
    // If we need more, we'll GlobalRealloc
    tkLocalVars.hGlbToken = DbgGlobalAlloc (GHND, DEF_TOKEN_SIZE * sizeof (TOKEN));
    if (!tkLocalVars.hGlbToken)
    {
        ErrorMessage (ERRMSG_WS_FULL APPEND_NAME, lpwszLine, -1);

        return tkLocalVars.hGlbToken;
    } // End IF

    // Lock the memory and set variables in the UNION_TOKEN
    UTLockAndSet (tkLocalVars.hGlbToken, &tkLocalVars.t2);
    tkLocalVars.t2.lpHeader->Signature  = TOKEN_HEADER_SIGNATURE;
    tkLocalVars.t2.lpHeader->iVersion   = 1;    // Initialize version # of this header
    tkLocalVars.t2.lpHeader->iTokenCnt  = 0;    // ...        count of tokens
    tkLocalVars.t2.lpHeader->iPrevGroup = NO_PREVIOUS_GROUPING_SYMBOL; // Initialize index of previous grouping symbol
    tkLocalVars.lpStart = TokenBaseToStart (tkLocalVars.t2.lpBase); // Skip over TOKEN_HEADER
    tkLocalVars.lpNext  = &tkLocalVars.lpStart[tkLocalVars.t2.lpHeader->iTokenCnt];
    tkLocalVars.PrevWchar = L'\0';              // Initialize previous WCHAR
    tkLocalVars.lpwszOrig = lpwszLine;          // Save ptr to start of input line

    // Initialize EOS
    tkLocalVars.lpLastEOS = tkLocalVars.lpStart;
    tkLocalVars.lpwsz = &lpwszLine[0];          // Just so it has a known value
    AppendEOSToken (&tkLocalVars, TRUE);

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    // Display the tokens so far
    DbgMsg ("*** Tokenize_EM Start");
    DisplayTokens (tkLocalVars.hGlbToken);
#endif

    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    // Loop through the chars including the
    //   terminating zero so we can catch
    //   endings in mid-state.
    iLen = 1 + lstrlenW (lpwszLine);

    // Skip over leading blanks (more to reduce clutter
    //   in the debugging window)
    for (i = 0; i < iLen; i++)
    if (lpwszLine[i] NE L' ')
        break;

    for (     ; i < iLen; i++)
    {
        // Use a FSA to tokenize the line

        /* The FSA works as follows:

           1.  Get the next WCHAR from the input;
           2.  Translate it into a COL_*** index;
           3.  Use the current state as a row index and the
               above index as a column index into fsaColTable,
               take the appropriate action.
           4.  Repeat until EOL or an error occurs.
         */

        wchOrig = lpwszLine[i];

        // Check for line continuation char
        if (wchOrig EQ '\n')
        {
            TKFLAGS tkFlags = {0};

            // Mark as a symbol table constant
            tkFlags.TknType = TKT_LINECONT;

            // Copy to local var so we may pass its address
            aplInteger = 0;

            // Attempt to append as new token, check for TOKEN TABLE FULL,
            //   and resize as necessary.
            if (AppendNewToken_EM (&tkLocalVars,
                                   &tkFlags,
                                   &aplInteger,
                                   0))
                continue;
            else
                goto ERROR_EXIT;
        } // End IF

        wchColNum = CharTrans (wchOrig);

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
        wsprintfW (lpwszTemp,
                   L"wchO = %c (%d), wchT = %s (%d), CS = %d, NS = %d, Act1 = %08X, Act2 = %08X",
                   wchOrig ? wchOrig : 8230,
                   wchOrig,
                   GetColName (wchColNum),
                   wchColNum,
                   tkLocalVars.State,
                   fsaColTable[tkLocalVars.State][wchColNum].iNewState,
                   fsaColTable[tkLocalVars.State][wchColNum].fnAction1,
                   fsaColTable[tkLocalVars.State][wchColNum].fnAction2);
        DbgMsgW (lpwszTemp);
#endif

        // Get primary action and new state
        fnAction1 = fsaColTable[tkLocalVars.State][wchColNum].fnAction1;
        fnAction2 = fsaColTable[tkLocalVars.State][wchColNum].fnAction2;
        tkLocalVars.State = fsaColTable[tkLocalVars.State][wchColNum].iNewState;

        // Save pointer to current wch
        tkLocalVars.lpwsz = &lpwszLine[i];

        // Save current index (may be modified by an action)
        tkLocalVars.iNdex = i;

        // Check for primary action
        tkLocalVars.ActionNum = 1;
        if (fnAction1
         && !(*fnAction1) (&tkLocalVars))
            goto ERROR_EXIT;

        // Check for secondary action
        tkLocalVars.ActionNum = 2;
        if (fnAction2
         && !(*fnAction2) (&tkLocalVars))
            goto ERROR_EXIT;

        // Split cases based upon the return code
        switch (tkLocalVars.State)
        {
            case FSA_NONCE:
                ErrorMessageIndirect (ERRMSG_NONCE_ERROR APPEND_NAME);

                goto ERROR_EXIT;

            case FSA_SYNTERR:
                ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

                goto ERROR_EXIT;

            case FSA_EXIT:
            {
                // Test for mismatched or improperly nested grouping symbols
                if (!CheckGroupSymbols_EM (&tkLocalVars))
                    goto ERROR_EXIT;

                // Calculate the # tokens in this last stmt
                AppendEOSToken (&tkLocalVars, FALSE);

                goto NORMAL_EXIT;
            } // End FSA_EXIT
        } // End SWITCH

        // Get next index (may have been modified by an action)
        i = tkLocalVars.iNdex;

        // Save this character as the previous one
        tkLocalVars.PrevWchar = wchOrig;
    } // End FOR

    // We should never get here as we process the
    //   trailing zero in the input line which should
    //   exit from one of the actions with FSA_EXIT.
    Assert (FALSE);

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Signal an error
    ErrorMessage (lpwszErrorMessage, lpwszLine, i);

    bFreeGlb = TRUE;
NORMAL_EXIT:
    // Unlock the handle
    MyGlobalUnlock (tkLocalVars.hGlbToken);
    tkLocalVars.t2.lpBase   = NULL;
    tkLocalVars.lpStart     = NULL;
    tkLocalVars.lpNext      = NULL;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    // Display the tokens so far
    DisplayTokens (tkLocalVars.hGlbToken);
    DbgMsg ("*** Tokenize_EM End");
#endif
    // Free the handle, if requested
    if (bFreeGlb)
    {
        DbgGlobalFree (tkLocalVars.hGlbToken); tkLocalVars.hGlbToken = NULL;
    } // End IF

    return tkLocalVars.hGlbToken;
} // End Tokenize_EM
#undef  APPEND_NAME


//***************************************************************************
//  CheckGroupSymbols_EM
//
//  Check for mismatched or improperly nested grouping symbols
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckGroupSymbols_EM"
#else
#define APPEND_NAME
#endif

BOOL CheckGroupSymbols_EM
    (LPTKLOCALVARS lptkLocalVars)

{
    if (lptkLocalVars->t2.lpHeader->iPrevGroup NE NO_PREVIOUS_GROUPING_SYMBOL)
    {
        ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

        return FALSE;
    } // End IF

    return TRUE;
} // End CheckGroupSymbols_EM
#undef  APPEND_NAME


//***************************************************************************
//  Untokenize
//
//  Free allocated memory and other such resources in a tokenized line
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- Untokenize_EM"
#else
#define APPEND_NAME
#endif

void Untokenize
    (HGLOBAL hGlbToken)

{
    LPTOKEN lpToken;
    int     i, iLen;

    if (hGlbToken && (lpToken = MyGlobalLock (hGlbToken)))
    {

#define lpHeader    ((LPTOKEN_HEADER) lpToken)

        // It's a token
        Assert (lpHeader->Signature EQ TOKEN_HEADER_SIGNATURE);

        // Get the # tokens
        iLen = lpHeader->iTokenCnt;

#undef  lpHeader

        lpToken = TokenBaseToStart (lpToken);   // Skip over TOKEN_HEADER

        for (i = 0; i < iLen; i++, lpToken++)
        switch (lpToken->tkFlags.TknType)
        {
            case TKT_VARNAMED:          // Symbol table name (data is LPSYMENTRY)
            case TKT_FCNNAMED:          // ...
            case TKT_OP1NAMED:          // ...
            case TKT_OP2NAMED:          // ...
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpToken->tkData.lpVoid) EQ PTRTYPE_STCONST);

                // Skip it if no value
                if (!lpToken->tkData.tkSym->stFlags.Value)
                    break;

                // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
                if (!lpToken->tkData.tkSym->stFlags.Imm)
                {
                    // stData is a valid HGLOBAL variable or function array
                    Assert (IsGlbTypeVarDir (lpToken->tkData.tkSym->stData.stGlbData)
                         || IsGlbTypeFcnDir (lpToken->tkData.tkSym->stData.stGlbData));
                } // End IF

                // Don't free a name's contents
                break;

            case TKT_STRING:            // String  (data is HGLOBAL)
            case TKT_VARARRAY:          // Array of data (data is HGLOBAL)
                // Free the array and all elements of it
                if (FreeResultGlobalVar (ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData)))
                {
#ifdef DEBUG
                    dprintf ("**Zapping in Untokenize: %08X (%d)",
                             ClrPtrTypeDir (lpToken->tkData.tkGlbData),
                             __LINE__);
#endif
                    lpToken->tkData.tkGlbData = NULL;
                } // End IF

                break;

            case TKT_COMMENT:           // Comment (data is HGLOBAL)
                // Free the global
                DbgGlobalFree (ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData));
#ifdef DEBUG
                dprintf ("**Zapping in Untokenize: %08X (%d)",
                         ClrPtrTypeDir (lpToken->tkData.tkGlbData),
                         __LINE__);
#endif
                lpToken->tkData.tkGlbData = NULL;

                break;

            case TKT_ASSIGN:            // Assignment symbol (data is UTF16_LEFTARROW)
            case TKT_LISTSEP:           // List separator    (...     ';')
            case TKT_VARIMMED:          // Immediate data (data is immediate)
            case TKT_FCNIMMED:          // Immediate primitive function (any valence) (data is UTF16_***)
            case TKT_OP1IMMED:          // ...       Monadic primitive operator (data is UTF16_***)
            case TKT_OP2IMMED:          // ...       Dyadic  ...
            case TKT_JOTDOT:            // Outer product (data is NULL)
            case TKT_LPAREN:            // Left paren (data is TKT_***)
            case TKT_RPAREN:            // Right ...   ...
            case TKT_LBRACKET:          // Left bracket ...
            case TKT_RBRACKET:          // Right ...   ...
            case TKT_EOS:               // End-of-Stmt (data is length of stmt including this token)
            case TKT_EOL:               // End-of-Line (data is NULL)
            case TKT_LINECONT:          // Line continuation (data is NULL)
            case TKT_INPOUT:            // Input/Output (data is UTF16_QUAD or UTF16_QUOTEQUAD)
                break;                  // Nothing to do

            defstop
#ifdef DEBUG
                wsprintf (lpszTemp,
                          "Untokenize:  *** Unknown Token Value:  %d",
                          lpToken->tkFlags.TknType);
                DbgMsg (lpszTemp);
#endif
                break;
        } // End FOR/SWITCH

        // Unlock the handle
        MyGlobalUnlock (hGlbToken); lpToken = NULL;
    } else
    {
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
        wsprintf (lpszTemp,
                  "Untokenize:  hGlobToken (%08X) is invalid.",
                  hGlbToken);
        DbgMsg (lpszTemp);
#endif
    } // End IF/ELSE
} // End Untokenize
#undef  APPEND_NAME


//***************************************************************************
//  AppendEOSToken
//
//  Append an EOS Token
//***************************************************************************

BOOL AppendEOSToken
    (LPTKLOCALVARS lptkLocalVars,
     BOOL          bAppend)

{
    APLINT aplInteger;

    // Calculate the # tokens in this stmt
    lptkLocalVars->lpLastEOS->tkData.tkIndex = lptkLocalVars->lpNext - lptkLocalVars->lpLastEOS;

    // If no append, mark this one as the last
    if (!bAppend)
        lptkLocalVars->lpLastEOS->tkFlags.TknType = TKT_EOL;

    // Point to the next token as the start of the next stmt
    lptkLocalVars->lpLastEOS = lptkLocalVars->lpNext;

    // Append EOS token, if requested
    if (bAppend)
    {
        TKFLAGS tkFlags = {0};

        // Mark as an EOS
        tkFlags.TknType = TKT_EOS;

        // Copy to a local var so we may pass its address
        aplInteger = 0;

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        return AppendNewToken_EM (lptkLocalVars,
                                  &tkFlags,
                                  &aplInteger,
                                  0);
    } else
        return TRUE;
} // Ens AppendEOSToken


//***************************************************************************
//  AppendNewToken_EM
//
//  Attempt to append as new token, check for TOKEN TABLE FULL,
//    and resize as necessary.
//
//  The suffix _EM means that this function generates its own error message
//    so the caller doesn't need to.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- AppendNewToken_EM"
#else
#define APPEND_NAME
#endif

BOOL AppendNewToken_EM
    (LPTKLOCALVARS lptkLocalVars,
     LPTKFLAGS     lptkFlags,
     LPAPLLONGEST  lptkData,
     int           iCharOffset)

{
    // Check for TOKEN TABLE FULL
    if (((lptkLocalVars->t2.lpHeader->iTokenCnt + 1) * sizeof (TOKEN) + sizeof (TOKEN_HEADER))
      > (int) MyGlobalSize (lptkLocalVars->hGlbToken))
    {
        HGLOBAL hGlbToken;

        // Resize the token stream

        // Unlock the global handle so we can resize it
        MyGlobalUnlock (lptkLocalVars->hGlbToken);
        lptkLocalVars->t2.lpBase   = NULL;
        lptkLocalVars->lpStart     = NULL;
        lptkLocalVars->lpNext      = NULL;

        // Increase the size by DEF_TOKEN_RESIZE
        hGlbToken = MyGlobalReAlloc (lptkLocalVars->hGlbToken,
                                     MyGlobalSize (lptkLocalVars->hGlbToken) + DEF_TOKEN_RESIZE,
                                     GMEM_ZEROINIT);
        if (hGlbToken EQ NULL)
        {
            // We ran into TOKEN TABLE FULL and couldn't resize
            ErrorMessageIndirect (ERRMSG_TOKEN_TABLE_FULL APPEND_NAME);

            return FALSE;
        } // End IF

        lptkLocalVars->hGlbToken = hGlbToken;
        UTRelockAndSet (hGlbToken, lptkLocalVars);
    } // End IF

    // Insert this token into the stream:
    lptkLocalVars->lpNext->tkData.tkLongest = *lptkData;
    lptkLocalVars->lpNext->tkFlags          = *lptkFlags;// Append the flags

    // Save index in input line of this token
    lptkLocalVars->lpNext->tkCharIndex = iCharOffset + lptkLocalVars->lpwsz - lptkLocalVars->lpwszOrig;
    lptkLocalVars->t2.lpHeader->iTokenCnt++;        // Count in another token
    lptkLocalVars->lpNext++;                        // Skip to next token

    return TRUE;
} // End AppendNewToken_EM
#undef  APPEND_NAME


//***************************************************************************
//  CharTrans
//
//  Translate a character in preparation for tokenizing
//***************************************************************************

WCHAR CharTrans
    (WCHAR wchOrig)

{
    // Split cases
    switch (wchOrig)
    {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
////////case 'e':                       // See below (COL_FPEXP)
        case 'f':
        case 'g':
        case 'h':
////////case 'i':                       // See below (COL_COMPLEX)
////////case 'j':                       // ...       ...
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
////////case 'r':                       // See below (COL_RATIONAL)
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':

        case 'A':
        case 'B':
        case 'C':
        case 'D':
////////case 'E':                       // See below (COL_FPEXP)
        case 'F':
        case 'G':
        case 'H':
////////case 'I':                       // See below (COL_COMPLEX)
////////case 'J':                       // ...       ...
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
////////case 'R':                       // See below (COL_RATIONAL)
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case UTF16_DELTA:               // Alt-'h' - delta
        case UTF16_DELTAUNDERBAR:       // Alt-'H' - delta-underbar
            return COL_ALPHA;

        case 'e':
        case 'E':
            return COL_FPEXP;

        case 'i':
        case 'I':
        case 'j':
        case 'J':
            // Split off the complex number separator
            //   as it can be either 'i', 'I', 'j', or 'J'
            //   and as such doesn't fit into a switch
            //   statement easily.
            if (cComplexSep EQ *CharLowerW (&wchOrig))
                return COL_COMPLEX;
            else
                return COL_ALPHA;

        case 'r':
        case 'R':
            return COL_RATIONAL;

        case '_':
            return COL_UNDERBAR;

        case '.':
            return COL_DOT;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return COL_DIGIT;

        case ' ':
        case '\t ':
            return COL_SPACE;

        case L'\0':
            return COL_EOL;

        case UTF16_ALPHA:               // Alt-'a' - alpha
        case UTF16_UPTACK:              // Alt-'b' - up tack
        case UTF16_UPSHOE:              // Alt-'c' - up shoe
        case UTF16_DOWNSTILE:           // Alt-'d' - down stile
        case UTF16_EPSILON:             // Alt-'e' - epsilon
////////case UTF16_UNDERBAR:            // Alt-'f' - underbar (COL_ALPHA)
////////case UTF16_DEL:                 // Alt-'g' - del (COL_UNK)
////////case UTF16_DELTA:               // Alt-'h' - delta (COL_ALPHA)
        case UTF16_IOTA:                // Alt-'i' - iota
////////case UTF16_JOT:                 // Alt-'j' - jot (COL_JOT)
////////case UTF16_APOSTROPHE:          // Alt-'k' - single quote (COL_QUOTE1)
////////case UTF16_QUAD:                // Alt-'l' - quad (COL_Q_QQ)
        case UTF16_STILE:               // Alt-'m' - stile
        case UTF16_DOWNTACK:            // Alt-'n' - down tack
        case UTF16_CIRCLE:              // Alt-'o' - circle
        case UTF16_STAR:                // Alt-'p' - star
        case UTF16_QUERY:               // Alt-'q' - question mark
        case UTF16_RHO:                 // Alt-'r' - rho
        case UTF16_UPSTILE:             // Alt-'s' - up stile
        case UTF16_TILDE:               // Alt-'t' - tilde
        case UTF16_DOWNARROW:           // Alt-'u' - down arrow
        case UTF16_DOWNSHOE:            // Alt-'v' - down shoe
        case UTF16_OMEGA:               // Alt-'w' - omega
        case UTF16_RIGHTSHOE:           // Alt-'x' - right shoe
        case UTF16_UPARROW:             // Alt-'y' - up arrow
        case UTF16_LEFTSHOE:            // Alt-'z' - left shoe

        case UTF16_EQUALUNDERBAR:       // Alt-'!' - match
////////case UTF16_                     // Alt-'"' - (none)
        case UTF16_DELSTILE:            // Alt-'#' - grade-down
        case UTF16_DELTASTILE:          // Alt-'$' - grade-up
        case UTF16_CIRCLESTILE:         // Alt-'%' - rotate
        case UTF16_CIRCLEBAR:           // Alt-'&' - circle-bar
        case UTF16_UPTACKJOT:           // Alt-'\''- execute
        case UTF16_DOWNCARETTILDE:      // Alt-'(' - nor
        case UTF16_UPCARETTILDE:        // Alt-')' - nand
        case UTF16_CIRCLESTAR:          // Alt-'*' - log
        case UTF16_DOMINO:              // Alt-'+' - domino
////////case UTF16_LAMP:                // Alt-',' - comment (COL_LAMP)
        case UTF16_TIMES:               // Alt-'-' - times
////////case UTF16_SLOPEBAR:            // Alt-'.' - slope-bar (COL_PRIM_OP1)
////////case UTF16_SLASHBAR:            // Alt-'/' - slash-bar (COL_PRIM_OP1)
        case UTF16_UPCARET:             // Alt-'0' - and (94??)
////////case UTF16_DIERESIS:            // Alt-'1' - dieresis (COL_PRIM_OP1)
////////case UTF16_OVERBAR:             // Alt-'2' - overbar (COL_OVERBAR)
        case UTF16_LEFTCARET:           // Alt-'3' - less
        case UTF16_LEFTCARETUNDERBAR:   // Alt-'4' - not more
        case UTF16_EQUAL:               // Alt-'5' - equal
        case UTF16_RIGHTCARETUNDERBAR:  // Alt-'6' - not less
        case UTF16_RIGHTCARET:          // Alt-'7' - more
        case UTF16_NOTEQUAL:            // Alt-'8' - not equal
        case UTF16_DOWNCARET:           // Alt-'9' - or
////////case UTF16_                     // Alt-':' - (none)
        case UTF16_DOWNTACKJOT:         // Alt-';' - format
////////case UTF16_                     // Alt-'<' - (none)
        case UTF16_COLONBAR:            // Alt-'=' - divide
////////case UTF16_                     // Alt-'>' - (none)
////////case UTF16_                     // Alt-'?' - (none)
////////case UTF16_DELTILDE:            // Alt-'@' - del-tilde (COL_UNK)
////////case UTF16_                     // Alt-'A' - (none)
////////case UTF16_                     // Alt-'B' - (none)
////////case UTF16_                     // Alt-'C' - (none)
////////case UTF16_                     // Alt-'D' - (none)
        case UTF16_EPSILONUNDERBAR:     // Alt-'E' - epsilon-underbar
////////case UTF16_                     // Alt-'F' - (none)
////////case UTF16_DIERESISDEL:         // Alt-'G' - dual (COL_PRIM_OP2)
////////case UTF16_DELTAUNDERBAR:       // Alt-'H' - delta-underbar (COL_ALPHA)
        case UTF16_IOTAUNDERBAR:        // Alt-'I' - iota-underbar
////////case UTF16_DIERESISJOT:         // Alt-'J' - rank (COL_PRIM_OP2)
////////case UTF16_                     // Alt-'K' - (none)
        case UTF16_SQUAD:               // Alt-'L' - squad
////////case UTF16_                     // Alt-'M' - (none)
////////case UTF16_DIERESISDOWNTACK:    // Alt-'N' - convolution (COL_PRIM_OP2)
////////case UTF16_DIERESISCIRCLE:      // Alt-'O' - holler (COL_UNK)
////////case UTF16_DIERESISSTAR:        // Alt-'P' - power  (COL_PRIM_OP2)
////////case UTF16_                     // Alt-'Q' - (none)
////////case UTF16_                     // Alt-'R' - (none)
////////case UTF16_                     // Alt-'S' - (none)
////////case UTF16_DIERESISTILDE:       // Alt-'T' - commute (COL_PRIM_OP1)
////////case UTF16_                     // Alt-'U' - (none)
////////case UTF16_                     // Alt-'V' - (none)
////////case UTF16_                     // Alt-'W' - (none)
////////case UTF16_                     // Alt-'X' - (none)
////////case UTF16_                     // Alt-'Y' - (none)
////////case UTF16_                     // Alt-'Z' - (none)
////////case UTF16_LEFTARROW:           // Alt-'[' - left arrow (COL_ASSIGN)
        case UTF16_LEFTTACK:            // Alt-'\' - left tack
        case UTF16_RIGHTARROW:          // Alt-']' - right arrow
        case UTF16_CIRCLESLOPE:         // Alt-'^' - transpose
        case UTF16_QUOTEDOT:            // Alt-'_' - quote-dot
////////case UTF16_DIAMOND:             // Alt-'`' - diamond (COL_DIAMOND)
////////case UTF16_QUOTEQUAD:           // Alt-'{' - quote-quad
        case UTF16_RIGHTTACK:           // Alt-'|' - right tack
////////case UTF16_ZILDE:               // Alt-'}' - zilde (COL_PRIM_FN0)
        case UTF16_COMMABAR:            // Alt-'~' - comma-bar
////////case UTF16_TILDE:               //     '~' - tilde
////////case UTF16_QUOTEDOT:            //     '!' - shreik
        case UTF16_CIRCUMFLEX:          //     '^' - up caret
////////case UTF16_STAR:                //     '*' - star
        case UTF16_BAR:                 //     '-' - bar
////////case UTF16_EQUAL:               //     '=' - equal
        case UTF16_PLUS:                //     '+' - plus
        case UTF16_STILE2:              //     '|' - stile2
        case UTF16_COMMA:               //     ',' - comma
////////case UTF16_LEFTCARET:           //     '<' - left caret
////////case UTF16_RIGHTCARET:          //     '>' - right caret
////////case UTF16_QUERY:               //     '?' - query
            return COL_PRIM_FN;

        case UTF16_SLOPE:               //     '\' - slope
        case UTF16_SLOPEBAR:            // Alt-'.' - slope-bar
        case UTF16_SLASH:               //     '/' - slash
        case UTF16_SLASHBAR:            // Alt-'/' - slash-bar
        case UTF16_DIERESIS:            // Alt-'1' - dieresis
        case UTF16_DIERESISTILDE:       // Alt-'T' - commute
            return COL_PRIM_OP1;

        case UTF16_DIERESISDEL:         // Alt-'G' - dual
        case UTF16_DIERESISJOT:         // Alt-'J' - rank
        case UTF16_DIERESISDOWNTACK:    // Alt-'N' - convolution
        case UTF16_DIERESISSTAR:        // Alt-'P' - power
            return COL_PRIM_OP2;

        case UTF16_JOT:                 // Alt-'j' - jot
            return COL_JOT;

        case UTF16_APOSTROPHE:          // Alt-'k' - single quote
            return COL_QUOTE1;

        case UTF16_DOUBLEQUOTE:         //     '"' - Double quote
            return COL_QUOTE2;

        case UTF16_QUAD:                // Alt-'l' - quad
        case UTF16_QUOTEQUAD:           // Alt-'{' - quote-quad
            return COL_Q_QQ;

        case UTF16_LAMP:                // Alt-',' - comment
            return COL_LAMP;

        case UTF16_LEFTARROW:           // Alt-'[' - left arrow
            return COL_ASSIGN;

        case UTF16_OVERBAR:             // Alt-'2' - high minus
            return COL_OVERBAR;

        case UTF16_DIAMOND:             // Alt-'`' - diamond
            return COL_DIAMOND;

        case UTF16_ZILDE:               // Alt-'}' - zilde
            return COL_PRIM_FN0;

        case UTF16_LPAREN:              //     '(' - left paren
            return COL_LPAREN;

        case UTF16_RPAREN:              //     ')' - right paren
            return COL_RPAREN;

        case UTF16_LBRACKET:            //     '[' - left bracket
            return COL_LBRACKET;

        case UTF16_RBRACKET:            //     ']' - right bracket
            return COL_RBRACKET;

        case ';':                       // Lists (bracketed and otherwise)
            return COL_SEMICOLON;

        case UTF16_DELTILDE:            // Alt-'@' - del-tilde
        case UTF16_DEL:                 // Alt-'g' - del
        case UTF16_DIERESISCIRCLE:      // Alt-'O' - holler
        case '`':
        case '@':
        case '#':
        case '$':
        case '%':
        case '&':
        case '{':
        case '}':
        case ':':                       // Control structures ***FIXME***
            return COL_UNK;

        default:
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
        { // ***DEBUG***
            WCHAR wszTemp[64];

            wsprintfW (wszTemp,
                       L"CharTrans:  Unknown char: %c (%d)",
                       wchOrig,
                       wchOrig);
            DbgMsgW (wszTemp);
        } // ***DEBUG*** END
#endif
            return COL_UNK;
    } // End SWITCH
} // End CharTrans


#ifdef DEBUG
//***************************************************************************
//  GetColName
//
//  Convert a column number to a name
//***************************************************************************

LPWCHAR GetColName
    (UINT uType)

{
typedef struct tagCOLNAMES
{
    LPWCHAR lpwsz;
    UINT    uColNum;
} COLNAMES, *LPCOLNAMES;

static COLNAMES colNames[] =
{{L"DIGIT"     , COL_DIGIT     },   //  0: digit
 {L"DOT"       , COL_DOT       },   //  1: decimal number, inner & outer product separator
 {L"FPEXP"     , COL_FPEXP     },   //  2: floating point exponent separator
 {L"ALPHA"     , COL_ALPHA     },   //  3: alphabetic
 {L"Q_QQ"      , COL_Q_QQ      },   //  4: quad
 {L"UNDERBAR"  , COL_UNDERBAR  },   //  5: underbar (infinity)
 {L"OVERBAR"   , COL_OVERBAR   },   //  6: overbar
 {L"COMPLEX"   , COL_COMPLEX   },   //  7: complex number separator
 {L"RATIONAL"  , COL_RATIONAL  },   //  8: rational number separator
 {L"ASSIGN"    , COL_ASSIGN    },   //  9: assignment symbol
 {L"SEMICOLON" , COL_SEMICOLON },   // 10: semicolon symbol
 {L"PRIM_FN"   , COL_PRIM_FN   },   // 11: primitive monadic or dyadic function
 {L"PRIM_FN0"  , COL_PRIM_FN0  },   // 12: ...       niladic function
 {L"PRIM_OP1"  , COL_PRIM_OP1  },   // 13: ...       monadic operator
 {L"PRIM_OP2"  , COL_PRIM_OP2  },   // 14: ...       dyadic  ...
 {L"JOT"       , COL_JOT       },   // 15: jot symbol
 {L"LPAREN"    , COL_LPAREN    },   // 16: left paren
 {L"RPAREN"    , COL_RPAREN    },   // 17: right ...
 {L"LBRACKET"  , COL_LBRACKET  },   // 18: left bracket
 {L"RBRACKET"  , COL_RBRACKET  },   // 19: right ...
 {L"SPACE"     , COL_SPACE     },   // 20: White space (' ' or '\t')
 {L"QUOTE1"    , COL_QUOTE1    },   // 21: single quote symbol
 {L"QUOTE2"    , COL_QUOTE2    },   // 22: double ...
 {L"DIAMOND"   , COL_DIAMOND   },   // 23: diamond symbol
 {L"LAMP"      , COL_LAMP      },   // 24: comment symbol
 {L"EOL"       , COL_EOL       },   // 25: End-Of-Line
 {L"UNK"       , COL_UNK       },   // 26: unknown symbols
};
    if (COL_LENGTH > (uType - COL_FIRST))
        return colNames[uType - COL_FIRST].lpwsz;
    else
    {
        static WCHAR wszTemp[64];

        wsprintfW (wszTemp,
                   L"GetColName:  *** Unknown Column Number:  %d",
                   uType);
        return wszTemp;
    } // End IF/ELSE
} // End GetColName
#endif


//***************************************************************************
//  End of File: exec.c
//***************************************************************************
