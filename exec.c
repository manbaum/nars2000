//***************************************************************************
//  NARS2000 -- Interpreter
//***************************************************************************

#define STRICT
#define _WIN32_WINDOWS 0x0401   // Needed for CreateWaitableTimer, etc.
#define _WIN32_WINNT   0x0500   // Needed for RegisterWaitForSingleObject
#include <windows.h>
#include <math.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "dfnhdr.h"
#include "sis.h"
#include "threads.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

////#define EXEC_TRACE

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
*

 */

#define DEF_TOKEN_SIZE  1024    // Default initial amount of memory
                                //   allocated for the tokenized line
#define DEF_TOKEN_RESIZE 512    // Default increment when GlobalRealloc'ing

// The order of the values of these constants *MUST* match the
//   column order in fsaColTable.
#define COL_FIRST 0             // It's origin-0

enum COL_INDICES
{COL_DIGIT = COL_FIRST, // 00: Digit
 COL_DOT       ,        // 01: Decimal number, inner & outer product separator
 COL_FPEXP     ,        // 02: Floating point exponent separator
 COL_ALPHA     ,        // 03: Alphabetic
 COL_DIRIDENT  ,        // 04: Alpha or Omega
 COL_Q_QQ      ,        // 05: Quad
 COL_UNDERBAR  ,        // 06: Underbar (infinity)
 COL_OVERBAR   ,        // 07: Overbar
 COL_COMPLEX   ,        // 08: Complex number separator
 COL_RATIONAL  ,        // 09: Rational number separator
 COL_ASSIGN    ,        // 0A: Assignment symbol
 COL_SEMICOLON ,        // 0B: Semicolon symbol
 COL_COLON     ,        // 0C: Colon symbol
 COL_PRIM_FN   ,        // 0D: Primitive monadic or dyadic function
 COL_PRIM_FN0  ,        // 0E: ...       niladic function
 COL_PRIM_OP1  ,        // 0F: ...       monadic operator
 COL_PRIM_OP2  ,        // 10: ...       dyadic  ...
 COL_JOT       ,        // 11: Jot symbol
 COL_LPAREN    ,        // 12: Left paren
 COL_RPAREN    ,        // 13: Right ...
 COL_LBRACKET  ,        // 14: Left bracket
 COL_RBRACKET  ,        // 15: Right ...
 COL_SPACE     ,        // 16: White space (' ' or '\t')
 COL_QUOTE1    ,        // 17: Single quote symbol
 COL_QUOTE2    ,        // 18: Double ...
 COL_DIAMOND   ,        // 19: Diamond symbol
 COL_LAMP      ,        // 1A: Comment symbol
 COL_EOL       ,        // 1B: End-Of-Line
 COL_UNK       ,        // 1C: Unknown symbols

 COL_LENGTH    ,        // 1D: # column indices (cols in fsaColTable) ***MUST*** BE THE LAST ENTRY
                        // Because these enums are origin-0, this value is the # valid columns.
};

// Whenever you add a new COL_*** entry,
//   be sure to put code into <CharTrans> in <exec.c>
//   to return the newly defined value,
//   and insert a new entry into <GetColName> in <exec.c>.


// The order of the values of these constants *MUST* match the
//   row order in fsaColTable.
enum FSA_TOKENS
{FSA_INIT = 0,  // Initial state
 FSA_OVERBAR ,  // Initial overbar
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
#define NO_PREVIOUS_GROUPING_SYMBOL     NEG1U

typedef struct tagTKLOCALVARS
{
    HGLOBAL     hGlbToken;          // Global memory handle
    UNION_TOKEN t2;                 // Locked base of hGlbToken
    LPTOKEN     lpStart,            // First available entry after the header
                lpNext,             // Next  ...
                lpLastEOS;          // Ptr to last EOS token
    UINT        State,              // Current state
                uChar,              // ...     index into lpwszLine
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

FSA_ACTION fsaColTable [][COL_LENGTH]
#ifndef PROTO
 =
{   // FSA_INIT     Initial state ('')
 {{FSA_INTEGER , NULL        , fnIntInit   },   // '0123456789'
  {FSA_DOTAMBIG, NULL        , fnFrcInit   },   // '.'
  {FSA_ALPHA   , NULL        , fnAlpInit   },   // Exponent separator (eE)
  {FSA_ALPHA   , NULL        , fnAlpInit   },   // 'a..zA..Z'
  {FSA_INIT    , NULL        , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , NULL        , fnSysInit   },   // Quad
  {FSA_INIT    , NULL        , fnInfinity  },   // Underbar
  {FSA_OVERBAR , fnAlpInit   , fnNegInit   },   // Overbar
  {FSA_ALPHA   , NULL        , fnAlpInit   },   // Complex separator (iIjJ)
  {FSA_ALPHA   , NULL        , fnAlpInit   },   // Rational separator (rR)
  {FSA_INIT    , NULL        , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , NULL        , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , NULL        , fnClnDone   },   // Colon  ...
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
  {FSA_SYNTERR , NULL        , NULL        },   // Exponent separator (eE)
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYNTERR , NULL        , NULL        },   // Alpha or Omega
  {FSA_SYSNAME , fnAlpDone   , fnSysInit   },   // Quad
  {FSA_INIT    , NULL        , fnInfinity  },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_SYNTERR , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_SYNTERR , NULL        , NULL        },   // Rational separator (rR)
  {FSA_INIT    , fnAlpDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnAlpDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnAlpDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnAlpDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnAlpDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnAlpDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnAlpDone   , fnOp2Done   },   // ...       dyadic  ...
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
    // FSA_INTEGER  Number, integer part ('n')
 {{FSA_INTEGER , NULL        , fnIntAccum  },   // '0123456789'
  {FSA_FPFRACT1, NULL        , fnFrcInit   },   // '.'
  {FSA_FPEXP1  , NULL        , fnExpInit   },   // Exponent separator (eE)
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_INIT    , fnIntDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnIntDone   , fnSysInit   },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_NONCE   , NULL        , NULL        },   // Rational separator (rR)
  {FSA_INIT    , fnIntDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnIntDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnIntDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnIntDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnIntDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnIntDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnIntDone   , fnOp2Done   },   // ...       dyadic  ...
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
  {FSA_FPEXP1  , NULL        , fnExpInit   },   // Exponent separator (eE)
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_INIT    , fnBigDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnBigDone   , fnSysInit   },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_NONCE   , NULL        , NULL        },   // Rational separator (rR)
  {FSA_INIT    , fnBigDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnBigDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnBigDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnBigDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnBigDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnBigDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnBigDone   , fnOp2Done   },   // ...       dyadic  ...
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
  {FSA_FPEXP1  , fnExpInit   , NULL        },   // Exponent separator (eE)
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_INIT    , fnFrcDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnFrcDone   , fnSysInit   },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_NONCE   , NULL        , NULL        },   // Rational separator (rR)
  {FSA_INIT    , fnFrcDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnFrcDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnFrcDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnFrcDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnFrcDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnFrcDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnFrcDone   , fnOp2Done   },   // ...       dyadic  ...
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
  {FSA_FPEXP1  , fnExpInit   , NULL        },   // Exponent separator (eE)
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_INIT    , fnFrcDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnFrcDone   , fnSysInit   },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_NONCE   , NULL        , NULL        },   // Rational separator (rR)
  {FSA_INIT    , fnFrcDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnFrcDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnFrcDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnFrcDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnFrcDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnFrcDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnFrcDone   , fnOp2Done   },   // ..        dyadic  ...
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
  {FSA_SYNTERR , NULL        , NULL        },   // Exponent separator (eE)
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYNTERR , NULL        , NULL        },   // Alpha or Omega
  {FSA_SYNTERR , NULL        , NULL        },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_FPEXP2  , fnNegExp    , NULL        },   // Overbar
  {FSA_SYNTERR , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_SYNTERR , NULL        , NULL        },   // Rational separator (rR)
  {FSA_SYNTERR , NULL        , NULL        },   // Assignment symbol
  {FSA_SYNTERR , NULL        , NULL        },   // Semicolon  ...
  {FSA_SYNTERR , NULL        , NULL        },   // Colon  ...
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
  {FSA_SYNTERR , NULL        , NULL        },   // Exponent separator (eE)
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_SYNTERR , NULL        , NULL        },   // Alpha or Omega
  {FSA_SYNTERR , NULL        , NULL        },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_SYNTERR , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_SYNTERR , NULL        , NULL        },   // Rational separator (rR)
  {FSA_SYNTERR , NULL        , NULL        },   // Assignment symbol
  {FSA_SYNTERR , NULL        , NULL        },   // Semicolon  ...
  {FSA_SYNTERR , NULL        , NULL        },   // Colon  ...
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
  {FSA_SYNTERR , NULL        , NULL        },   // Exponent separator (eE)
  {FSA_SYNTERR , NULL        , NULL        },   // 'a..zA..Z'
  {FSA_INIT    , fnExpDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnExpDone   , fnSysInit   },   // Quad
  {FSA_SYNTERR , NULL        , NULL        },   // Underbar
  {FSA_SYNTERR , NULL        , NULL        },   // Overbar
  {FSA_NONCE   , NULL        , NULL        },   // Complex separator (iIjJ)
  {FSA_NONCE   , NULL        , NULL        },   // Rational separator (rR)
  {FSA_INIT    , fnExpDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnExpDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnExpDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnExpDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnExpDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnExpDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnExpDone   , fnOp2Done   },   // ...       dyadic  ...
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
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // Exponent separator (eE)
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // 'a..zA..Z'
  {FSA_INIT    , fnAlpDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnAlpDone   , fnSysInit   },   // Quad
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // Underbar
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // Overbar
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // Complex separator (iIjJ)
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // Rational separator (rR)
  {FSA_INIT    , fnAlpDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnAlpDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnAlpDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnAlpDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnAlpDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnAlpDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnAlpDone   , fnOp2Done   },   // ...       dyadic  ...
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
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // Exponent separator (eE)
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // 'a..zA..Z'
  {FSA_INIT    , fnSysDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnSysDone   , fnSysInit   },   // Quad
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // Underbar
  {FSA_INTEGER , fnSysDone   , fnNegInit   },   // Overbar
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // Complex separator (iIjJ)
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // Rational separator (rR)
  {FSA_INIT    , fnSysDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnSysDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnSysDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnSysDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnSysDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnSysDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnSysDone   , fnOp2Done   },   // ...       dyadic  ...
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
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Exponent separator (eE)
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // 'a..zA..Z'
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Alpha or Omega
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Quad
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Underbar
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Overbar
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Complex separator (iIjJ)
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Rational separator (rR)
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Assignment symbol
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Semicolon  ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Colon  ...
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
  {FSA_ALPHA   , fnQuo1Done  , fnAlpInit   },   // Exponent separator (eE)
  {FSA_ALPHA   , fnQuo1Done  , fnAlpInit   },   // 'a..zA..Z'
  {FSA_INIT    , fnQuo1Done  , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnQuo1Done  , fnSysInit   },   // Quad
  {FSA_INIT    , fnQuo1Done  , fnInfinity  },   // Underbar
  {FSA_INTEGER , fnQuo1Done  , fnNegInit   },   // Overbar
  {FSA_ALPHA   , fnQuo1Done  , fnAlpInit   },   // Complex separator (iIjJ)
  {FSA_ALPHA   , fnQuo1Done  , fnAlpInit   },   // Rational separator (rR)
  {FSA_INIT    , fnQuo1Done  , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnQuo1Done  , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnQuo1Done  , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnQuo1Done  , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnQuo1Done  , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnQuo1Done  , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnQuo1Done  , fnOp2Done   },   // ...       dyadic  ...
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
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Exponent separator (eE)
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // 'a..zA..Z'
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Alpha or Omega
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Quad
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Underbar
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Overbar
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Complex separator (iIjJ)
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Rational separator (rR)
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Assignment symbol
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Semicolon  ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Colon  ...
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
  {FSA_ALPHA   , fnQuo2Done  , fnAlpInit   },   // Exponent separator (eE)
  {FSA_ALPHA   , fnQuo2Done  , fnAlpInit   },   // 'a..zA..Z'
  {FSA_INIT    , fnQuo2Done  , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnQuo2Done  , fnSysInit   },   // Quad
  {FSA_INIT    , fnQuo2Done  , fnInfinity  },   // Underbar
  {FSA_INTEGER , fnQuo2Done  , fnNegInit   },   // Overbar
  {FSA_ALPHA   , fnQuo2Done  , fnAlpInit   },   // Complex separator (iIjJ)
  {FSA_ALPHA   , fnQuo2Done  , fnAlpInit   },   // Rational separator (rR)
  {FSA_INIT    , fnQuo2Done  , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnQuo2Done  , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnQuo2Done  , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnQuo2Done  , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnQuo2Done  , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnQuo2Done  , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnQuo2Done  , fnOp2Done   },   // ...       dyadic  ...
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
  {FSA_ALPHA   , fnOp2Done   , fnAlpInit   },   // Exponent separator (eE)
  {FSA_ALPHA   , fnOp2Done   , fnAlpInit   },   // 'a..zA..Z'
  {FSA_INIT    , fnOp2Done   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnOp2Done   , fnSysInit   },   // Quad
  {FSA_INIT    , fnOp2Done   , fnInfinity  },   // Underbar
  {FSA_INTEGER , fnOp2Done   , fnNegInit   },   // Overbar
  {FSA_ALPHA   , fnOp2Done   , fnAlpInit   },   // Complex separator (iIjJ)
  {FSA_ALPHA   , fnOp2Done   , fnAlpInit   },   // Rational separator (rR)
  {FSA_INIT    , fnOp2Done   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnOp2Done   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnOp2Done   , fnClnDone   },   // Colon  ...
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
  {FSA_ALPHA   , fnJotDone   , fnAlpInit   },   // Exponent separator (eE)
  {FSA_ALPHA   , fnJotDone   , fnAlpInit   },   // 'a..zA..Z'
  {FSA_INIT    , fnJotDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnJotDone   , fnSysInit   },   // Quad
  {FSA_INIT    , fnJotDone   , fnInfinity  },   // Underbar
  {FSA_INTEGER , fnJotDone   , fnNegInit   },   // Overbar
  {FSA_ALPHA   , fnJotDone   , fnAlpInit   },   // Complex separator (iIjJ)
  {FSA_ALPHA   , fnJotDone   , fnAlpInit   },   // Rational separator (rR)
  {FSA_INIT    , fnJotDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnJotDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnJotDone   , fnClnDone   },   // Colon  ...
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
  {FSA_ALPHA   , fnOutDone   , fnAlpInit   },   // Exponent separator (eE)
  {FSA_ALPHA   , fnOutDone   , fnAlpInit   },   // 'a..zA..Z'
  {FSA_INIT    , fnOutDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnOutDone   , fnSysInit   },   // Quad
  {FSA_INIT    , fnOutDone   , fnInfinity  },   // Underbar
  {FSA_INTEGER , fnOutDone   , fnNegInit   },   // Overbar
  {FSA_ALPHA   , fnOutDone   , fnAlpInit   },   // Complex separator (iIjJ)
  {FSA_ALPHA   , fnOutDone   , fnAlpInit   },   // Rational separator (rR)
  {FSA_INIT    , fnOutDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnOutDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnOutDone   , fnClnDone   },   // Colon  ...
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
}
#endif
;

typedef struct tagWFSO      // Struct for WaitForSingleObject
{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    HANDLE       WaitHandle,    // WaitHandle from RegisterWaitForSingleObject
                 hSigaphore,    // Handle to signal next (may be NULL)
                 hThread;       // Thread handle
} WFSO, *LPWFSO;

IE_THREAD ieThread;             // Temporary global


//***************************************************************************
//  $WaitForImmExecLine
//
//  Wait callback for ImmExecLine
//***************************************************************************

VOID CALLBACK WaitForImmExecLine
    (LPVOID  lpParameter,           // Thread data
     BOOLEAN TimerOrWaitFired)      // Reason

{
    LPWFSO lpMemWFSO;       // Ptr to WFSO global memory
#ifdef DEBUG
    DWORD  dwRet;           // Return code from UnregisterWait
#endif

#define hGlbWFSO    ((HGLOBAL) lpParameter)

    // Lock the memory to get a ptr to it
    lpMemWFSO = MyGlobalLock (hGlbWFSO);

    // Save the thread's PerTabData global memory handle
    TlsSetValue (dwTlsPerTabData, lpMemWFSO->hGlbPTD);
#ifdef DEBUG
    dprintfW (L"~~WaitForImmExecLine (%08X)", lpMemWFSO->hThread);
#endif
    // Cancel the wait operation
#ifdef DEBUG
    dwRet =
#endif
    UnregisterWait (lpMemWFSO->WaitHandle); lpMemWFSO->WaitHandle = NULL;
    Assert (dwRet EQ 0 || dwRet EQ ERROR_IO_PENDING);

    // Signal the next level (if appropriate)
    if (lpMemWFSO->hSigaphore)
        ReleaseSemaphore (lpMemWFSO->hSigaphore, 1, NULL);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;

    // Free the allocated memory
    DbgGlobalFree (hGlbWFSO); hGlbWFSO = NULL;
#undef  hGlbWFSO
} // End WaitForImmExecLine


//***************************************************************************
//  $ImmExecLine
//
//  Execute a line (sys/user command, fn defn, etc.)
//    in immediate execution mode
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ImmExecLine"
#else
#define APPEND_NAME
#endif

void ImmExecLine
    (UINT uLineNum,             // Line #
     HWND hWndEC)               // Handle of Edit Control window

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPWCHAR      lpwszCompLine, // Ptr to complete line
                 lpwszLine;     // Ptr to line following leading blanks
    UINT         uLinePos,      // Char position of start of line
                 uLineLen;      // Line length
    HWND         hWndMC;        // Window handle of MDI Client

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Get the position of the start of the line
    uLinePos = SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the line length
    uLineLen = SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Allocate virtual memory for the line (along with its continuations)
    lpwszCompLine = VirtualAlloc (NULL,             // Any address
                                  (uLineLen + 1) * sizeof (WCHAR),  // "+ 1" for the terminating zero
                                  MEM_COMMIT,
                                  PAGE_READWRITE);
    if (!lpwszCompLine)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ImmExecLine:  VirtualAlloc for <lpwszCompLine> failed");

        return;                 // Mark as failed
    } // End IF

    // Tell EM_GETLINE maximum # chars in the buffer
    // Because we allocated space for the terminating zero,
    //   we don't have to worry about overwriting the
    //   allocation limits of the buffer
    ((LPWORD) lpwszCompLine)[0] = (WORD) uLineLen;

    // Get the contents of the line
    SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwszCompLine);

    // Ensure properly terminated
    lpwszCompLine[uLineLen] = L'\0';

    // Strip off leading blanks
    for (lpwszLine = lpwszCompLine;
         lpwszLine[0] && lpwszLine[0] EQ L' ';
         lpwszLine++)
    {};

    // Split cases based upon the first non-blank char
    switch (lpwszLine[0])
    {
        case L')':          // System commands
            // Execute the command (ignore the result)
            ExecSysCmd (lpwszLine);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // If it's Quad input, and we're not resetting, ...
            if (lpMemPTD->lpSISCur
             && lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_NONE
             && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                // Tell the SM to display the Quad Input Prompt
                PostMessage (lpMemPTD->hWndSM, MYWM_QUOTEQUAD, FALSE, 10);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            break;

        case L']':          // User commands
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
            // ***FIXME***
            DbgMsg ("User command");
#endif
            AppendLine (ERRMSG_NONCE_ERROR, FALSE, TRUE);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // If it's Quad input, ...
            if (lpMemPTD->lpSISCur
             && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                // Tell the SM to display the Quad Input Prompt
                PostMessage (lpMemPTD->hWndSM, MYWM_QUOTEQUAD, FALSE, 11);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            break;

        case UTF16_DEL:     // Function definition
            // Get the window handle of the MDI Client
            hWndMC = GetParent (GetParent (hWndEC));

            // Create the Function Editor Window (ignore the result)
            CreateFcnWindow (lpwszLine, hWndMC);

            // Fall through to empty line case

        case L'\0':         // Empty line
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // If it's Quad input, ...
            if (lpMemPTD->lpSISCur
             && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                // Tell the SM to display the Quad Input Prompt
                PostMessage (lpMemPTD->hWndSM, MYWM_QUOTEQUAD, FALSE, 12);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            break;

        default:
            // Execute the statement
            ImmExecStmt (lpwszCompLine,     // Ptr to line to execute
                         TRUE,              // Free lpwszCompLine on completion
                         FALSE,             // TRUE iff wait until finished
                         hWndEC);           // Edit Control window handle
            return;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If no SIS layer or not Quad input and not reset all, ...
    if (lpMemPTD->lpSISCur EQ NULL
     || (lpMemPTD->lpSISCur->DfnType NE DFNTYPE_QUAD
      && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_ALL))
        // Display the default prompt
        DisplayPrompt (hWndEC, 4);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Free the virtual memory for the complete line
    VirtualFree (lpwszCompLine, 0, MEM_RELEASE); lpwszCompLine = NULL;
} // End ImmExecLine
#undef  APPEND_NAME


//***************************************************************************
//  $ImmExecStmt
//
//  Execute a statement
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ImmExecStmt"
#else
#define APPEND_NAME
#endif

EXIT_TYPES ImmExecStmt
    (LPWCHAR lpwszCompLine, // Ptr to line to execute
     BOOL    bFreeLine,     // TRUE iff free lpwszCompLine on completion
     BOOL    bWaitUntilFini,// TRUE iff wait until finished
     HWND    hWndEC)        // Edit Control window handle

{
    HANDLE     hThread;     // Thread handle
    DWORD      dwThreadId;  // The thread ID #
    LPWFSO     lpMemWFSO;   // Ptr to WFSO global memory
    EXIT_TYPES exitType;    // Exit code from thread

    // Save args in struc to pass to thread func
    ieThread.hWndEC         = hWndEC;
    ieThread.hGlbPTD        = TlsGetValue (dwTlsPerTabData); Assert (ieThread.hGlbPTD NE NULL);
    ieThread.lpwszCompLine  = lpwszCompLine;
    ieThread.hGlbWFSO       = DbgGlobalAlloc (GHND, sizeof (WFSO));
    ieThread.bFreeLine      = bFreeLine;
    ieThread.bWaitUntilFini = bWaitUntilFini;

    // Lock the memory to get a ptr to it
    lpMemWFSO = MyGlobalLock (ieThread.hGlbWFSO);

    // Fill in the struct
    lpMemWFSO->hGlbPTD = ieThread.hGlbPTD;

    // Create a new thread
    hThread = CreateThread (NULL,                   // No security attrs
                            0,                      // Use default stack size
                           &ImmExecLineInThread,    // Starting routine
                           &ieThread,               // Param to thread func
                            CREATE_SUSPENDED,       // Creation flag
                           &dwThreadId);            // Returns thread id
#ifdef DEBUG
    // Save the thread handle
    lpMemWFSO->hThread = hThread;
#endif
    // Should we wait until finished?
    if (bWaitUntilFini)
    {
        // Start 'er up
        ResumeThread (hThread);
#ifdef DEBUG
        dprintfW (L"~~WaitForSingleObject (ENTRY):  %08X -- %s (%S#%d)", hThread, L"ImmExecStmt", FNLN);
#endif
        // Wait until this thread terminates
        WaitForSingleObject (hThread,              // Handle to wait on
                             INFINITE);            // Wait time in milliseconds
#ifdef DEBUG
        dprintfW (L"~~WaitForSingleObject (EXIT):   %08X -- %s (%S#%d)", hThread, L"ImmExecStmt", FNLN);
#endif
        // Get the exit code
        GetExitCodeThread (hThread, &exitType);
    } else
    {
#ifdef DEBUG
        dprintfW (L"~~RegisterWaitForSingleObject (%08X) (%S#%d)", hThread, FNLN);
#endif
        // Tell W to callback when this thread terminates
        RegisterWaitForSingleObject (&lpMemWFSO->WaitHandle,// Return wait handle
                                      hThread,              // Handle to wait on
                                     &WaitForImmExecLine,   // Callback function
                                      ieThread.hGlbWFSO,    // Callback function parameter
                                      INFINITE,             // Wait time in milliseconds
                                      WT_EXECUTEONLYONCE);  // Options
        // We no longer need this ptr
        MyGlobalUnlock (ieThread.hGlbWFSO); lpMemWFSO = NULL;

        // Start 'er up
        ResumeThread (hThread);

        exitType = EXITTYPE_NONE;
    } // End IF/ELSE

    return exitType;
} // End ImmExecStmt
#undef  APPEND_NAME


//***************************************************************************
//  $ImmExecLineInThread
//
//  Execute a line (sys/user command, fn defn, etc.)
//    in immediate execution mode
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ImmExecLineInThread"
#else
#define APPEND_NAME
#endif

DWORD WINAPI ImmExecLineInThread
    (LPIE_THREAD lpieThread)            // Ptr to IE_THREAD struc

{
    HANDLE        hSigaphore = NULL;    // Semaphore handle to signal (NULL if none)
    LPWCHAR       lpwszCompLine;        // Ptr to complete line
    HGLOBAL       hGlbToken,            // Handle of tokenized line
                  hGlbWFSO;             // WaitForSingleObject callback global memory handle
    HWND          hWndEC,               // Handle of Edit Control window
                  hWndSM;               // ...       Session Manager ...
    HGLOBAL       hGlbPTD;              // Handle to this window's PerTabData
    LPPERTABDATA  lpMemPTD;             // Ptr to ...
    RESET_FLAGS   resetFlag;            // Reset flag (see RESET_FLAGS)
    BOOL          bFreeLine,            // TRUE iff we should free lpszCompLine on completion
                  bWaitUntilFini;       // TRUE iff wait until finished
    EXIT_TYPES    exitType;             // Return code from ParseLine
    LPWFSO        lpMemWFSO;            // Ptr to WFSO global memory
    LPSIS_HEADER  lpSISPrv;             // Ptr to previous SIS header

    // Save the thread type ('IE')
    TlsSetValue (dwTlsType, (LPVOID) 'IE');

    // Extract values from the arg struc
    hWndEC         = lpieThread->hWndEC;
    hGlbPTD        = lpieThread->hGlbPTD;
    lpwszCompLine  = lpieThread->lpwszCompLine;
    hGlbWFSO       = lpieThread->hGlbWFSO;
    bFreeLine      = lpieThread->bFreeLine;
    bWaitUntilFini = lpieThread->bWaitUntilFini;

    // Save the thread's PerTabData global memory handle
    TlsSetValue (dwTlsPerTabData, hGlbPTD);
#ifdef DEBUG
    dprintfW (L"--Starting thread in <ImmExecLineInThread>.");
#endif
    // Get the window handle of the Session Manager
    hWndSM = GetParent (hWndEC);

    // Initialize the Reset Flag
    resetFlag = RESETFLAG_NONE;

    // Tokenize, parse, and untokenize the line

    // Tokenize the line
    hGlbToken = Tokenize_EM (lpwszCompLine,
                             lstrlenW (lpwszCompLine),
                             hWndEC,
                            &ErrorMessageDirect);
    // If it's invalid, ...
    if (hGlbToken EQ NULL)
    {
        // Execute the statement in immediate execution mode
        exitType =
        ImmExecStmt (WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"ELX", // Ptr to line to execute
                     FALSE,         // TRUE iff free the lpwszLine on completion
                     TRUE,          // TRUE iff wait until finished
                     (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC)); // Edit Control window handle
        // Split cases based upon the exit type
        switch (exitType)
        {
            case EXITTYPE_GOTO_LINE:        // Pass on to caller
            case EXITTYPE_RESET_ALL:        // ...
            case EXITTYPE_RESET_ONE:        // ...
            case EXITTYPE_RESET_ONE_INIT:   // ...
                break;

            case EXITTYPE_ERROR:            // Mark as in error (from the error in Tokenize_EM)
            case EXITTYPE_DISPLAY:          // ...
            case EXITTYPE_NODISPLAY:        // ...
            case EXITTYPE_NOVALUE:          // ...
            case EXITTYPE_GOTO_ZILDE:       // ...
                exitType = EXITTYPE_ERROR;

                break;

            case EXITTYPE_NONE:
            defstop
                break;
        } // End SWITCH
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Fill in the SIS header for Immediate Execution Mode
    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                NULL,                   // Semaphore handle
                DFNTYPE_IMM,            // DfnType
                FCNVALENCE_IMM,         // FcnValence
                FALSE);                 // Suspended
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Run the parser in a separate thread
    exitType =
    ParseLine (hWndSM,              // Session Manager window handle
               NULL,                // Line text global memory handle
               hGlbToken,           // Tokenized line global memory handle
               lpwszCompLine,       // Ptr to the complete line
               hGlbPTD);            // PerTabData global memory handle
    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Start with the preceding layer (if any)
    lpSISPrv = lpMemPTD->lpSISCur->lpSISPrv;

    // Split cases based upon the exit type
    switch (exitType)
    {
        case EXITTYPE_QUADERROR_INIT:
            DbgBrk ();

            // If there are no more SI layers, ...
            if (lpSISPrv EQ NULL)
            {
                // ***FIXME*** -- Set the tkCharIndex for the error???

                // Set the error message
                ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                                    lpwszCompLine,                  // Ptr to the line which generated the error
                                    lpMemPTD->tkErrorCharIndex,     // Position of caret (origin-0)
                                    hWndSM);                        // Window handle to the Session Manager
                // Execute []ELX in immediate execution mode
////////////////exitType =
                ImmExecStmt (WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"ELX",   // Ptr to line to execute
                             FALSE,         // TRUE iff free the lpwszLine on completion
                             TRUE,          // TRUE iff wait until finished
                             (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC)); // Edit Control window handle
                // Set the reset flag
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                exitType = EXITTYPE_NONE;

                break;
            } // End IF

            // If the previous layer in the SI stack is a
            //   User-Defined Function/Operator or Quad Input,
            //   signal it to handle this action
            if (lpSISPrv
             && (lpSISPrv->DfnType EQ DFNTYPE_OP1
              || lpSISPrv->DfnType EQ DFNTYPE_OP2
              || lpSISPrv->DfnType EQ DFNTYPE_FCN
              || lpSISPrv->DfnType EQ DFNTYPE_QUAD))
            {
                // Lock the memory to get a ptr to it
                lpMemWFSO = MyGlobalLock (hGlbWFSO);

                Assert (lpSISPrv->hSemaphore NE NULL);

                // Tell the wait handler to signal this layer
                hSigaphore = lpMemWFSO->hSigaphore = lpSISPrv->hSemaphore;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;
            } // End IF

            break;

        case EXITTYPE_QUADERROR_EXEC:
            DbgBrk ();

            // Execute []ELX in immediate execution mode
////////////exitType =
            ImmExecStmt (WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"ELX",   // Ptr to line to execute
                         FALSE,         // TRUE iff free the lpwszLine on completion
                         TRUE,          // TRUE iff wait until finished
                         (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC)); // Edit Control window handle
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

            exitType = EXITTYPE_NONE;

            break;

        case EXITTYPE_ERROR:        // If from Quad Input, tell SM to redisplay the prompt
            if (lpSISPrv
             && lpSISPrv->DfnType EQ DFNTYPE_QUAD)
                PostMessage (hWndSM, MYWM_QUOTEQUAD, FALSE, 13);
            break;

        case EXITTYPE_GOTO_ZILDE:   // Nothing more to do with these types
        case EXITTYPE_DISPLAY:      // ...
        case EXITTYPE_NOVALUE:      // ...
            break;

        case EXITTYPE_NODISPLAY:    // Signal previous SI layer's semaphore if it's Quad input
            // If the previous layer in the SI stack is Quad input,
            //   signal it to receive this value
            if (lpSISPrv
             && lpSISPrv->DfnType EQ DFNTYPE_QUAD)
            {
                // Lock the memory to get a ptr to it
                lpMemWFSO = MyGlobalLock (hGlbWFSO);

                Assert (lpSISPrv->hSemaphore NE NULL);

                // Tell the wait handler to signal this layer
                hSigaphore = lpMemWFSO->hSigaphore = lpSISPrv->hSemaphore;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;
            } // End IF

            break;

        case EXITTYPE_RESET_ONE:        // Stop at this level
            exitType = EXITTYPE_NOVALUE;
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

            break;

        case EXITTYPE_RESET_ONE_INIT:   // Change to EXITTYPE_RESET_ONE
            exitType = EXITTYPE_RESET_ONE;
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE;

            // Fall through to common code

        case EXITTYPE_RESET_ALL:        // Continue resetting if more layers
            // If there are no more SI layers, ...
            if (lpSISPrv EQ NULL)
            {
                // Display the default prompt
                DisplayPrompt (hWndEC, 6);

                break;
            } // End IF

            // Fall through to common code

        case EXITTYPE_GOTO_LINE:        // Signal previous SI layer's semaphore if it's user-defined function/operator
            // If the previous layer in the SI stack is a
            //   User-Defined Function/Operator or Quad Input,
            //   signal it to handle this action
            if (lpSISPrv
             && (lpSISPrv->DfnType EQ DFNTYPE_OP1
              || lpSISPrv->DfnType EQ DFNTYPE_OP2
              || lpSISPrv->DfnType EQ DFNTYPE_FCN
              || lpSISPrv->DfnType EQ DFNTYPE_QUAD))
            {
                // Lock the memory to get a ptr to it
                lpMemWFSO = MyGlobalLock (hGlbWFSO);

                Assert (lpSISPrv->hSemaphore NE NULL);

                // Tell the wait handler to signal this layer
                hSigaphore = lpMemWFSO->hSigaphore = lpSISPrv->hSemaphore;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;
            } // End IF

            break;

        case EXITTYPE_NONE:
        defstop
            break;
    } // End SWITCH

    // Get the reset flag
    resetFlag = lpMemPTD->lpSISCur->ResetFlag;

    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    Unlocalize ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Untokenize the temporary line and free its memory
    Untokenize (hGlbToken);
    MyGlobalFree (hGlbToken); hGlbToken = NULL;
ERROR_EXIT:
#ifdef DEBUG
    dprintfW (L"--Ending   thread in <ImmExecLineInThread>.");
#endif
    // Free the virtual memory for the complete line
    if (bFreeLine)
    {
        VirtualFree (lpwszCompLine, 0, MEM_RELEASE); lpwszCompLine = NULL;
    } // End IF

    // Display the default prompt
    //   if we're not resetting, and
    //   the caller isn't waiting for us to finish, and
//////   the exit type isn't error, and
    //   there's no semaphore to signal
    if (resetFlag EQ RESETFLAG_NONE
     && !bWaitUntilFini
/////&& exitType NE EXITTYPE_ERROR
     && hSigaphore EQ NULL)
        DisplayPrompt (hWndEC, 3);
    return exitType;
} // End ImmExecLineInThread
#undef  APPEND_NAME


//***************************************************************************
//  $UTLockAndSet
//
//  Lock and set the variables in a UNION_TOKEN
//***************************************************************************

void UTLockAndSet
    (HGLOBAL       hGlbToken,
     LPUNION_TOKEN lpUT)

{
    // Lock the memory to get a ptr to it
    lpUT->lpBase = MyGlobalLock (hGlbToken);
} // End UTLockAndSet


//***************************************************************************
//  $UTRelockAndSet
//
//  Relock and set the variables in a UNION_TOKEN
//***************************************************************************

void UTRelockAndSet
    (HGLOBAL       hGlbToken,
     LPTKLOCALVARS lptkLocalVars)

{
    // Lock the memory to get a ptr to it
    UTLockAndSet (lptkLocalVars->hGlbToken, &lptkLocalVars->t2);
    lptkLocalVars->lpStart = TokenBaseToStart (lptkLocalVars->t2.lpBase);   // Skip over TOKEN_HEADER
    lptkLocalVars->lpNext  = &lptkLocalVars->lpStart[lptkLocalVars->t2.lpHeader->TokenCnt];
} // End UTRelockAndSet

//***************************************************************************
//  $InitAccumVars
//
//  Initialize the accumulation variables for the next constant
//***************************************************************************

void InitAccumVars
    (void)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Integers & floating points
////lpMemPTD->lpszNumAlp[0]  = '\0';
////lpMemPTD->lpwszString[0] = L'\0';
    lpMemPTD->iNumAlpLen = 0;
    lpMemPTD->iStringLen = 0;
    lpMemPTD->aplInteger = 0;
    lpMemPTD->bNegative  =
    lpMemPTD->bNegExp    = FALSE;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End InitAccumVars


//***************************************************************************
//  $NumAlpAccum
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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet;          // TRUE iff result is valid

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // In case we overflow, or convert to FP, save the chars
    // Check for overflow -- LIMIT ERROR
    bRet = (lpMemPTD->iNumAlpLen < lpMemPTD->iMaxNumAlp);
    if (bRet)
        lpMemPTD->lpszNumAlp[lpMemPTD->iNumAlpLen++] = (char) wch;
    else
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End NumAlpAccum_EM
#undef  APPEND_NAME


//***************************************************************************
//  $fnIntAccum
//
//  Start of or next digit in integer number
//***************************************************************************

BOOL fnIntAccum
    (LPTKLOCALVARS lptkLocalVars)

{
    WCHAR        wch;
    APLINT       aplInt;
    BOOL         bRet = TRUE;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnIntAccum");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Copy current WCHAR
    wch = *lptkLocalVars->lpwsz;

    // Shift over by one digit
    // We'd like to do the following, but C has no way to
    //   detect integer overflow for the multiplication.
////lpMemPTD->aplInteger *= 10;
////lpMemPTD->aplInteger += wch - L'0';
    //   so, instead, we do the following:
    aplInt = imul64 (lpMemPTD->aplInteger, 10, &bRet);
    if (!bRet)
////aplInt = lpMemPTD->aplInteger * 10;
////if (lpMemPTD->aplInteger NE (aplInt / 10)
//// || aplInt < 0)
    {
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
        DbgMsg ("Integer overflow!");
#endif
        lptkLocalVars->State = FSA_BIGINT;
    } else
    {
        lpMemPTD->aplInteger = aplInt + (wch - L'0');

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
        { // ***DEBUG***
            wsprintf (lpszDebug,
                      "aplInteger = %08X%08X",
                      HIDWORD (lpMemPTD->aplInteger),
                      LODWORD (lpMemPTD->aplInteger));
            DbgMsg (lpszDebug);
        } // ***DEBUG*** END
#endif
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return NumAlpAccum_EM (wch);
} // End fnIntAccum


//***************************************************************************
//  $fnIntDone
//
//  Integer constant -- save as token
//***************************************************************************

BOOL fnIntDone
    (LPTKLOCALVARS lptkLocalVars)

{
    BOOL         bRet = TRUE;
    TKFLAGS      tkFlags = {0};
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnIntDone");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Ensure properly terminated
    lpMemPTD->lpszNumAlp[lpMemPTD->iNumAlpLen] = '\0';

    // Include the sign bit
    // (N.B.  can't overflow as there are more negative #s than positive #s)
    if (lpMemPTD->bNegative)
        lpMemPTD->aplInteger = -lpMemPTD->aplInteger;

    // Mark as an immediate Boolean/integer
    tkFlags.TknType = TKT_VARIMMED;

    if (IsBooleanValue (lpMemPTD->aplInteger))
        tkFlags.ImmType = IMMTYPE_BOOL;
    else
        tkFlags.ImmType = IMMTYPE_INT;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &lpMemPTD->aplInteger,
                              -lpMemPTD->iNumAlpLen);
    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnIntDone


//***************************************************************************
//  $fnFPAccum
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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet;          // TRUE iff result is valid

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnFPAccum");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Check for overflow -- LIMIT ERROR
    bRet = (lpMemPTD->iNumAlpLen < lpMemPTD->iMaxNumAlp);
    if (bRet)
        lpMemPTD->lpszNumAlp[lpMemPTD->iNumAlpLen++] = (char) *lptkLocalVars->lpwsz;
    else
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnFPAccum
#undef  APPEND_NAME


//***************************************************************************
//  $fnNegInit
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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet;          // TRUE iff result is valid

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnNegInit");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    lpMemPTD->bNegative = TRUE;

    // Check for overflow -- LIMIT ERROR
    bRet = (lpMemPTD->iNumAlpLen < lpMemPTD->iMaxNumAlp);
    if (bRet)
        lpMemPTD->lpszNumAlp[lpMemPTD->iNumAlpLen++] = '-';
    else
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnNegInit
#undef  APPEND_NAME


//***************************************************************************
//  $fnNegExp
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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet;          // TRUE iff result is valid

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnNegExp");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    lpMemPTD->bNegExp = TRUE;

    // Check for overflow -- LIMIT ERROR
    bRet = (lpMemPTD->iNumAlpLen < lpMemPTD->iMaxNumAlp);
    if (bRet)
        lpMemPTD->lpszNumAlp[lpMemPTD->iNumAlpLen++] = L'-';
    else
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnNegExp
#undef  APPEND_NAME


//***************************************************************************
//  $fnFPDone
//
//  Floating point constant -- save as token
//***************************************************************************

BOOL fnFPDone
    (LPTKLOCALVARS lptkLocalVars)

{
    APLFLOAT     aplFloat;
    TKFLAGS      tkFlags = {0};
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet;          // TRUE iff result is valid

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnFPDone");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Ensure properly terminated
    lpMemPTD->lpszNumAlp[lpMemPTD->iNumAlpLen] = '\0';

    // Use David Gay's routines
    aplFloat = strtod (lpMemPTD->lpszNumAlp, NULL);

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
                              -lpMemPTD->iNumAlpLen);
    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnFPDone


//***************************************************************************
//  $fnAlpha
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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet;          // TRUE iff result is valid

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnAlpha");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Check for overflow -- LIMIT ERROR
    bRet = (lpMemPTD->iStringLen < lpMemPTD->iMaxString);
    if (bRet)
        // Save the char
        lpMemPTD->lpwszString[lpMemPTD->iStringLen++] = *lptkLocalVars->lpwsz;
    else
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnAlpha
#undef  APPEND_NAME


//***************************************************************************
//  $fnAlpDone
//
//  End of name
//***************************************************************************

BOOL fnAlpDone
    (LPTKLOCALVARS lptkLocalVars)

{
    LPSYMENTRY   lpSymEntry;
    BOOL         bRet;
    APLINT       aplInteger;
    TKFLAGS      tkFlags = {0};
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnAlpDone");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Ensure properly terminated
    lpMemPTD->lpwszString[lpMemPTD->iStringLen] = L'\0';

    // If this is a system name (starts with a Quad),
    //   convert it to lowercase as those names are
    //   case-insensitive
    if (lpMemPTD->lpwszString[0] EQ UTF16_QUAD
     || lpMemPTD->lpwszString[0] EQ UTF16_QUOTEQUAD)
    {
        // Handle Quad and QuoteQuad via a separate token
        if (lpMemPTD->iStringLen EQ 1)
        {
            // Mark the data as Quad or QuoteQuad
            tkFlags.TknType = TKT_INPOUT;

            // Copy to local var so we may pass its address
            aplInteger = lpMemPTD->lpwszString[0];

            // Attempt to append as new token, check for TOKEN TABLE FULL,
            //   and resize as necessary.
            bRet = AppendNewToken_EM (lptkLocalVars,
                                     &tkFlags,
                                     &aplInteger,
                                      -lpMemPTD->iStringLen);
            goto NORMAL_EXIT;
        } else
            CharLowerW (&lpMemPTD->lpwszString[1]);
    } // End IF

    // Lookup in or append to the symbol table
    lpSymEntry = SymTabAppendName_EM (lpMemPTD->lpwszString);
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
                                  -lpMemPTD->iStringLen);
    } else
        bRet = FALSE;
NORMAL_EXIT:
    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnAlpDone


//***************************************************************************
//  $fnDirIdent
//
//  End of direct identifier
//***************************************************************************

BOOL fnDirIdent
    (LPTKLOCALVARS lptkLocalVars)

{
    LPSYMENTRY   lpSymEntry;
    BOOL         bRet;
    APLINT       aplInteger;
    TKFLAGS      tkFlags = {0};
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnDirIdent");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Save the character in the string
    lpMemPTD->lpwszString[0] = lptkLocalVars->lpwsz[0];

    // Save the string length
    lpMemPTD->iStringLen = 1;

    // Ensure properly terminated
    lpMemPTD->lpwszString[lpMemPTD->iStringLen] = L'\0';

    // Lookup in or append to the symbol table
    lpSymEntry = SymTabAppendName_EM (lpMemPTD->lpwszString);
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
                                  -lpMemPTD->iStringLen);
    } else
        bRet = FALSE;

    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnDirIdent


//***************************************************************************
//  $fnAsnDone
//
//  Start and end of an assignment (monadic or dyadic)
//***************************************************************************

BOOL fnAsnDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnAsnDone");
#endif

    // Copy current WCHAR
    aplLongest = *lptkLocalVars->lpwsz;

    // Mark the data as an assignment
    tkFlags.TknType = TKT_ASSIGN;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    return AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              0);
} // End fnAsnDone


//***************************************************************************
//  $fnLstDone
//
//  Start and end of a list
//***************************************************************************

BOOL fnLstDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnLstDone");
#endif

    // Copy current WCHAR
    aplLongest = *lptkLocalVars->lpwsz;

    // Mark the data as an assignment
    tkFlags.TknType = TKT_LISTSEP;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    return AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              0);
} // End fnLstDone


//***************************************************************************
//  $fnClnDone
//
//  Start and end of a label or control structure
//***************************************************************************

BOOL fnClnDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnClnDone");
#endif

    // Copy current WCHAR
    aplLongest = *lptkLocalVars->lpwsz;

    // Mark the data as an assignment
    tkFlags.TknType = TKT_COLON;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    return AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              0);
} // End fnClnDone


//***************************************************************************
//  $fnPrmDone
//
//  Start and end of a primitive function (niladic, monadic, or dyadic)
//***************************************************************************

BOOL fnPrmDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS tkFlags = {0};
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
    return AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplInteger,
                              0);
} // End fnPrmDone


//***************************************************************************
//  $fnInfinity
//
//  Infinity symbol (_)
//***************************************************************************

BOOL fnInfinity
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS      tkFlags = {0};
    BOOL         bRet;          // TRUE iff result is valid
    APLFLOAT     aplFloat;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnInfinity");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Mark the data as an immediate floating point variable
    tkFlags.TknType = TKT_VARIMMED;
    tkFlags.ImmType = IMMTYPE_FLOAT;
    if (lpMemPTD->bNegative)
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

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnInfinity


//***************************************************************************
//  $fnOp1Done
//
//  End of a monadic primitive operator
//***************************************************************************

BOOL fnOp1Done
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
////WCHAR      wch;
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnOp1Done");
#endif

////// Copy current WCHAR
////wch = *lptkLocalVars->lpwsz;
////
////// If this is a slash/slope and the preceding token is
//////   a left paren or a dyadic operator, convert this
//////   symbol into a primitive function
////if ((wch EQ '/'
////  || wch EQ '\\'
////  || wch EQ UTF16_SLASHBAR
////  || wch EQ UTF16_SLOPEBAR)
//// && lptkLocalVars->lpStart NE lptkLocalVars->lpNext
//// && (lptkLocalVars->lpNext[-1].tkFlags.TknType EQ TKT_LPAREN
////  || lptkLocalVars->lpNext[-1].tkFlags.TknType EQ TKT_OP2IMMED))
////    return fnPrmDone (lptkLocalVars);

    // Mark the data as a monadic primitive operator
    tkFlags.TknType = TKT_OP1IMMED;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    aplLongest = *lptkLocalVars->lpwsz;
    return AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              0);
} // End fnOp1Done


//***************************************************************************
//  $fnOp2Done
//
//  End of a dyadic primitive operator
//***************************************************************************

BOOL fnOp2Done
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnOp2Done");
#endif

    // Mark the data as a dyadic primitive operator
    tkFlags.TknType = TKT_OP2IMMED;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    aplLongest = lptkLocalVars->PrevWchar;
    return AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              -1);
} // End fnOp2Done


//***************************************************************************
//  $fnJotDone
//
//  End of a jot as a dyadic primitive operator
//***************************************************************************

BOOL fnJotDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS tkFlags = {0};
    APLINT  aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnJotDone");
#endif

    // Mark the data as a dyadic primitive operator
    tkFlags.TknType = TKT_OP2IMMED;

    // Copy to local var so we may pass its address
    aplInteger = UTF16_JOT;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    return AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplInteger,
                              -1);
} // End fnJotDone


//***************************************************************************
//  $fnOutDone
//
//  End of a jot dot as an outer product monadic primitive operator
//***************************************************************************

BOOL fnOutDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS tkFlags = {0};
    APLINT  aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnOutDone");
#endif

    // Mark the data as an outer product monadic primitive operator (with right scope)
    tkFlags.TknType = TKT_OPJOTDOT;

    // Copy to local var so we may pass its address
    aplInteger = UTF16_JOTDOT;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    return AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplInteger,
                              -1);
} // End fnOutDone


//***************************************************************************
//  $fnComDone
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
////HGLOBAL hGlb;
////APLINT  aplInteger;
    TKFLAGS tkFlags = {0};
    LPWCHAR lpwch;
////APLUINT ByteRes;

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
    lptkLocalVars->uChar += iLen - 1;

////     // Calculate space needed for the result ("+1" for the terminating zero)
////     ByteRes = CalcArraySize (ARRAY_CHAR, iLen + 1, 1);
////
////     // Allocate global memory for the comment
////     hGlb = DbgGlobalAlloc (GHND, (UINT) ByteRes);
////     if (!hGlb)
////     {
////         ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);
////
////         return FALSE;
////     } else
////     {
////         LPWCHAR lpwsz;
////         TKFLAGS tkFlags = {0};
////
////         // Lock the memory to get a ptr to it
////         lpwsz = MyGlobalLock (hGlb);
////
////         // Fill in the result header
//// #define lpHeader        ((LPVARARRAY_HEADER) lpwsz)
////         lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
////         lpHeader->ArrType    = ARRAY_CHAR;
//// ////////lpHeader->Perm       = 0;
//// ////////lpHeader->SysVar     = 0;
////         lpHeader->RefCnt     = 1;
////         lpHeader->NELM       = iLen + 1;
////         lpHeader->Rank       = 1;
//// #undef  lpHeader
////
////         // Fill in the dimension
////         *VarArrayBaseToDim (lpwsz) = iLen + 1;
////
////         // Skip over the header and dimension to the data
////         lpwsz = VarArrayBaseToData (lpwsz, 1);
////
////         // Copy the comment to the global memory
////         lstrcpyW (lpwsz, lptkLocalVars->lpwsz);
////
////         // We no longer need this ptr
////         MyGlobalUnlock (hGlb); lpwsz = NULL;

        // Mark the data as a comment in a global memory handle
        tkFlags.TknType = TKT_COMMENT;

////         // Copy to local var so we may pass its address
////         aplInteger = MakeGlbType (hGlb);

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        return AppendNewToken_EM (lptkLocalVars,
                                 &tkFlags,
                                  NULL,
                                  0);
////     } // End IF
} // End fnComDone
#undef  APPEND_NAME


//***************************************************************************
//  $fnQuoAccum
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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet;          // TRUE iff result is valid

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnQuoAccum");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Check for overflow -- LIMIT ERROR
    bRet = (lpMemPTD->iStringLen < lpMemPTD->iMaxString);
    if (bRet)
        lpMemPTD->lpwszString[lpMemPTD->iStringLen++] = *lptkLocalVars->lpwsz;
    else
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnQuoAccum
#undef  APPEND_NAME


//***************************************************************************
//  $fnQuoDone
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
    HGLOBAL      hGlb;
    TKFLAGS      tkFlags = {0};
    APLINT       aplInteger;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet;          // TRUE iff result is valid

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("fnQuoDone");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Ensure properly terminated
    lpMemPTD->lpwszString[lpMemPTD->iStringLen] = L'\0';

    // The initial FSA action for a string is to store the leading
    //   delimiter, that is a single or double quote, so we can
    //   tell the difference between the two at this point.
    // At the moment, I have no use for this information, but I'd
    //   like to leave the initial action as is and just skip over
    //   the first character in the saved string.  Note that this
    //   means the string length is one too big.

    // Take care of the string length
    lpMemPTD->iStringLen--;             // Count out the leading string delimiter

    // If this string is of length zero, then store it as an empty vector
    if (lpMemPTD->iStringLen EQ 0)
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
    if (lpMemPTD->iStringLen EQ 1)
    {
        // Mark the data as an immediate chracter
        tkFlags.TknType = TKT_VARIMMED;
        tkFlags.ImmType = IMMTYPE_CHAR;

        // Copy to local var so we may pass its address
        aplInteger = lpMemPTD->lpwszString[1];

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        bRet = AppendNewToken_EM (lptkLocalVars,
                                 &tkFlags,
                                 &aplInteger,
                                  -lpMemPTD->iStringLen);
    } else
    {
        APLUINT ByteRes;

        // Allocate global memory for the array header,
        //   one dimension (it's a vector), and the string
        //   excluding the terminating zero.
        ByteRes = CalcArraySize (ARRAY_CHAR, lpMemPTD->iStringLen, 1);
        hGlb = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlb)
        {
            ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

            bRet = FALSE;
        } else
        {
            LPWCHAR lpwsz;

            // Lock the memory to get a ptr to it
            lpwsz = MyGlobalLock (hGlb);

            // Setup the header, and copy
            //   the string to the global memory

#define lpHeader    ((LPVARARRAY_HEADER) lpwsz)
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = ARRAY_CHAR;
////////////lpHeader->Perm       = 0;
////////////lpHeader->SysVar     = 0;
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = lpMemPTD->iStringLen;
            lpHeader->Rank       = 1;
#undef  lpHeader

            *VarArrayBaseToDim (lpwsz) = lpMemPTD->iStringLen;
            CopyMemory (VarArrayBaseToData (lpwsz, 1),
                       &lpMemPTD->lpwszString[1],       // Skip over the string delimiter
                        lpMemPTD->iStringLen * sizeof (APLCHAR));
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
                                      -lpMemPTD->iStringLen);
        } // End IF/ELSE
    } // End IF

    //  Initialize the accumulation variables for the next constant
    InitAccumVars ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnQuoDone
#undef  APPEND_NAME


//***************************************************************************
//  $fnParInit
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
//  $fnBrkInit
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
//  $GroupInitCom
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
    aplLongest = lptkLocalVars->t2.lpHeader->PrevGroup;
    bRet = AppendNewToken_EM (lptkLocalVars,
                              &tkFlags,
                              &aplLongest,
                              0);
    // Save as location of previous left grouping symbol
    lptkLocalVars->t2.lpHeader->PrevGroup = lpNext - lptkLocalVars->lpStart;

    return bRet;
} // End GroupInitCom


//***************************************************************************
//  $fnParDone
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
//  $fnBrkDone
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
//  $GroupDoneCom
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
    UINT        uPrevGroup;
    BOOL        bRet = TRUE;
    APLLONGEST  aplLongest;

    // Get the index of the previous grouping symbol
    uPrevGroup = lptkLocalVars->t2.lpHeader->PrevGroup;

    // Ensure proper nesting
    if (uPrevGroup EQ NO_PREVIOUS_GROUPING_SYMBOL
     || lptkLocalVars->lpStart[uPrevGroup].tkFlags.TknType NE (UINT) uTypePrev)
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
        aplLongest = uPrevGroup;
        bRet = AppendNewToken_EM (lptkLocalVars,
                                  &tkFlags,
                                  &aplLongest,
                                  0);
        // Save index of previous grouping symbol
        lptkLocalVars->t2.lpHeader->PrevGroup = lptkLocalVars->lpStart[uPrevGroup].tkData.tkIndex;
    } // End IF/ELSE

    return bRet;
} // End GroupDoneCom
#undef  APPEND_NAME


//***************************************************************************
//  $fnDiaDone
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
//  $IncorrectCommand
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
//  $FormatQQuadInput
//
//  Format QQ input and save in global memory
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FormatQQuadInput"
#else
#define APPEND_NAME
#endif

void FormatQQuadInput
    (UINT          uLineNum,        // Line #
     HWND          hWndEC,          // Handle of Edit Control window
     LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    UINT         uLinePos,      // Char position of start of line
                 uLineLen;      // Line length
    APLUINT      ByteRes;       // # bytes in the result
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPAPLCHAR    lpMemRes;      // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Get the position of the start of the line
    uLinePos = SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the line length
    uLineLen = SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Calculate space needed for the result
    // N.B.:  max is needed because, in order to get the line,
    //        we need to tell EM_GETLINE the buffer size which
    //        takes up one APLCHAR (WORD) at the start of the buffer.
    ByteRes = CalcArraySize (ARRAY_CHAR, max (uLineLen, 1), 1);

    // Allocate space for the result
    // N.B.:  Conversion from APLUINT to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lpMemPTD->lpSISCur->lptkFunc);
        // Make a PL_YYSTYPE NoValue entry
        lpYYRes = MakeNoValue_YY (lpMemPTD->lpSISCur->lptkFunc);
    } else
    {
        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeaderRes->ArrType    = ARRAY_CHAR;
////////lpHeaderRes->Perm       = 0;        // Already zero from GHND
////////lpHeaderRes->SysVar     = 0;        // Already zero from GHND
        lpHeaderRes->RefCnt     = 1;
        lpHeaderRes->NELM       = uLineLen;
        lpHeaderRes->Rank       = 1;
#undef  lpHeaderRes

        // Save the dimension in the result
        *VarArrayBaseToDim (lpMemRes) = uLineLen;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, 1);

        // Tell EM_GETLINE maximum # chars in the buffer
        // Because we allocated space for max (uLineLen, 1)
        //   chars, we don't have to worry about overwriting
        //   the allocation limits of the buffer
        ((LPWORD) lpMemRes)[0] = (WORD) uLineLen;

        // Get the contents of the line
        SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpMemRes);

        // Replace leading Prompt Replacement chars
        if (lpMemPTD->cQuadPR NE L'\0')
        {
            UINT QQPromptLen,   // Length of QQ prompt
                 u;             // Loop counter

            // Get the length of the QQ prompt
            QQPromptLen = lpMemPTD->lpSISCur->QQPromptLen;

            // ***FIXME*** -- we're supposed to save the actual prompt
            //                and compare it with the chars after the
            //                user has responded to the request for
            //                Quote-Quad input.

            // Replace all prompt chars
            for (u = 0; u < QQPromptLen; u++)
                lpMemRes[u] = lpMemPTD->cQuadPR;
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;             // Already zero from YYAlloc
        lpYYRes->tkToken.tkFlags.NoDisplay = 1;
        lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lpMemPTD->lpSISCur->lptkFunc->tkCharIndex;
    } // End IF/ELSE

    // Save the result in PerTabData
    lpMemPTD->YYResExec = *lpYYRes;

    // Free the YYRes we allocated
    YYFree (lpYYRes); lpYYRes = NULL;

    Assert (lpMemPTD->lpSISCur->hSemaphore NE NULL);

    if (lpMemPTD->lpSISCur->hSemaphore)
    {
#ifdef DEBUG
        dprintfW (L"~~Releasing semaphore:  %08X (%S#%d)", lpMemPTD->lpSISCur->hSemaphore, FNLN);
#endif
        // Signal WaitForInput that we have a result
        ReleaseSemaphore (lpMemPTD->lpSISCur->hSemaphore, 1, NULL);

        // Release our time slice so the released thread can act
        Sleep (0);
    } // End IF
} // End FormatQQuadInput
#undef  APPEND_NAME


//***************************************************************************
//  $WaitForInput
//
//  Wait for either Quad or Quote-quad input
//***************************************************************************

LPPL_YYSTYPE WaitForInput
    (HWND    hWndSM,                // Window handle to Session Manager
     BOOL    bQuoteQuad,            // TRUE iff Quote-Quad input (FALSE if Quad input)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    HANDLE         hSemaphore;      // Semaphore handle
    LPPL_YYSTYPE   lpYYRes;         // Ptr to the result
    HGLOBAL        hGlbPTD;         // PerTabData global memory handle
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory
    UINT           uLinePos,        // Char position of start of line
                   uCharPos;        // Current char position
    HWND           hWndEC;          // Edit Control window handle

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Create a semaphore
    hSemaphore =
    CreateSemaphore (NULL,              // No security attrs
                     0,                 // Initial count (non-signalled)
                     64*1024,           // Maximum count
                     NULL);             // No name
    // Fill in the SIS header for Quote-Quad/Quad Input Mode
    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                hSemaphore,             // Semaphore handle
                bQuoteQuad ? DFNTYPE_QQUAD: DFNTYPE_QUAD, // DfnType
                FCNVALENCE_NIL,         // FcnValence
                TRUE);                  // Suspended
    // Save a ptr to the function token
    lpMemPTD->lpSISCur->lptkFunc = lptkFunc;

    // Get the Edit Control window handle
    hWndEC = (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWndEC);

    // Get the position of the start of the line
    uLinePos = SendMessageW (hWndEC, EM_LINEINDEX, NEG1U, 0);

    // Save the Quote-Quad input prompt length
    lpMemPTD->lpSISCur->QQPromptLen = uCharPos - uLinePos;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Tell the Session Manager to display the appropriate prompt
    PostMessage (hWndSM, MYWM_QUOTEQUAD, bQuoteQuad, 14);
#ifdef DEBUG
    dprintfW (L"~~WaitForSingleObject (ENTRY):  %s (%S#%d)", L"WaitForInput", FNLN);
#endif
    // Wait for the semaphore to trigger
    WaitForSingleObject (hSemaphore,    // Handle to wait for
                         INFINITE);     // Timeout value in milliseconds
#ifdef DEBUG
    dprintfW (L"~~WaitForSingleObject (EXIT):   %s (%S#%d)", L"WaitForInput", FNLN);
#endif
    // Close the semaphore handle as it is no longer needed
    CloseHandle (hSemaphore); hSemaphore = NULL;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    Unlocalize ();

    // If we're resetting, ...
    if (lpMemPTD->lpSISCur
     && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
        lpYYRes = NULL;
    else
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Copy the result
        *lpYYRes = lpMemPTD->YYResExec;
        ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpYYRes;
} // End WaitForInput


//***************************************************************************
//  $Tokenize_EM
//
//  Tokenize a line of input
//
//  The suffix _EM means that this function generates its own error message
//    so the caller doesn't need to.
//***************************************************************************
/*

A tokenized line consists of a count and length, followed by
a series of tokens.

The format of a token is defined in tokens.h

*/

#ifdef DEBUG
#define APPEND_NAME     L" -- Tokenize_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL Tokenize_EM
    (LPAPLCHAR   lpwszLine,     // The line to tokenize (not necessarily zero-terminated)
     UINT        uLen,          // The length of the above line
     HWND        hWndEC,        // Window handle for Edit Control
     LPERRHANDFN lpErrHandFn)   // Ptr to error handling function

{
    UINT         uChar;         // Loop counter
    WCHAR        wchOrig,       // The original char
                 wchColNum;     // The translated char for tokenization as a COL_*** value
    BOOL         (*fnAction1_EM) (LPTKLOCALVARS);
    BOOL         (*fnAction2_EM) (LPTKLOCALVARS);
    TKLOCALVARS  tkLocalVars;   // Local vars
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Save local vars in struct which we pass to each FSA action routine
    tkLocalVars.State = FSA_INIT;

    // Allocate some memory for the tokens
    // If we need more, we'll GlobalRealloc
    // Note, we can't use DbgGlobalAlloc here as we
    //   might free this line by being called from the Master Frame
    //   via a system command, in which case there is
    //   no PTD for that thread.
    tkLocalVars.hGlbToken = MyGlobalAlloc (GHND, DEF_TOKEN_SIZE * sizeof (TOKEN));
    if (!tkLocalVars.hGlbToken)
    {
        if (lpErrHandFn)
            (*lpErrHandFn) (ERRMSG_WS_FULL APPEND_NAME, lpwszLine, NEG1U, hWndEC);

        return tkLocalVars.hGlbToken;
    } // End IF

    // Lock the memory to get a ptr to it
    UTLockAndSet (tkLocalVars.hGlbToken, &tkLocalVars.t2);

    // Set variables in the UNION_TOKEN
    tkLocalVars.t2.lpHeader->Sig.nature = TOKEN_HEADER_SIGNATURE;
    tkLocalVars.t2.lpHeader->Version    = 1;    // Initialize version # of this header
    tkLocalVars.t2.lpHeader->TokenCnt   = 0;    // ...        count of tokens
    tkLocalVars.t2.lpHeader->PrevGroup  = NO_PREVIOUS_GROUPING_SYMBOL; // Initialize index of previous grouping symbol
    tkLocalVars.lpStart = TokenBaseToStart (tkLocalVars.t2.lpBase); // Skip over TOKEN_HEADER
    tkLocalVars.lpNext  = &tkLocalVars.lpStart[tkLocalVars.t2.lpHeader->TokenCnt];
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

    // Skip over leading blanks (more to reduce clutter
    //   in the debugging window)
    for (uChar = 0; uChar < uLen; uChar++)
    if (lpwszLine[uChar] NE L' ')
        break;

    for (     ; uChar <= uLen; uChar++)
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

        if (uChar EQ uLen)
            wchOrig = L'\0';
        else
            wchOrig = lpwszLine[uChar];

        // Check for line continuation char
        if (wchOrig EQ '\n')
        {
            TKFLAGS tkFlags = {0};
            APLINT  aplInteger;     // Temporary integer for AppendNewToken_EM

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
        wsprintfW (lpwszDebug,
                   L"wchO = %c (%d), wchT = %s (%d), CS = %d, NS = %d, Act1 = %08X, Act2 = %08X",
                   wchOrig ? wchOrig : 8230,
                   wchOrig,
                   GetColName (wchColNum),
                   wchColNum,
                   tkLocalVars.State,
                   fsaColTable[tkLocalVars.State][wchColNum].iNewState,
                   fsaColTable[tkLocalVars.State][wchColNum].fnAction1,
                   fsaColTable[tkLocalVars.State][wchColNum].fnAction2);
        DbgMsgW (lpwszDebug);
#endif

        // Get primary action and new state
        fnAction1_EM = fsaColTable[tkLocalVars.State][wchColNum].fnAction1;
        fnAction2_EM = fsaColTable[tkLocalVars.State][wchColNum].fnAction2;
        tkLocalVars.State = fsaColTable[tkLocalVars.State][wchColNum].iNewState;

        // Save pointer to current wch
        tkLocalVars.lpwsz = &lpwszLine[uChar];

        // Save current index (may be modified by an action)
        tkLocalVars.uChar = uChar;

        // Check for primary action
        tkLocalVars.ActionNum = 1;
        if (fnAction1_EM
         && !(*fnAction1_EM) (&tkLocalVars))
            goto ERROR_EXIT;

        // Check for secondary action
        tkLocalVars.ActionNum = 2;
        if (fnAction2_EM
         && !(*fnAction2_EM) (&tkLocalVars))
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
                UINT uNext;             // Offset from Start to Next in units of sizeof (TOKEN)

                // Test for mismatched or improperly nested grouping symbols
                if (!CheckGroupSymbols_EM (&tkLocalVars))
                    goto ERROR_EXIT;

                // Calculate the # tokens in this last stmt
                AppendEOSToken (&tkLocalVars, FALSE);

                uNext = tkLocalVars.lpNext - tkLocalVars.lpStart;

                // We no longer need this ptr
                MyGlobalUnlock (tkLocalVars.hGlbToken);
                tkLocalVars.t2.lpBase   = NULL;
////////////////tkLocalVars.t2.lpHeader = NULL;
                tkLocalVars.lpStart     = NULL;
                tkLocalVars.lpNext      = NULL;
                tkLocalVars.lpLastEOS   = NULL;

                // Reallocate the tokenized line to the actual size
                tkLocalVars.hGlbToken =
                MyGlobalReAlloc (tkLocalVars.hGlbToken,
                                 sizeof (TOKEN_HEADER)
                               + uNext * sizeof (TOKEN),
                                 GHND);
                goto UNLOCKED_EXIT;
            } // End FSA_EXIT
        } // End SWITCH

        // Get next index (may have been modified by an action)
        uChar = tkLocalVars.uChar;

        // Save this character as the previous one
        tkLocalVars.PrevWchar = wchOrig;
    } // End FOR

    // We should never get here as we process the
    //   trailing zero in the input line which should
    //   exit from one of the actions with FSA_EXIT.
    Assert (FALSE);

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Signal an error
    if (lpErrHandFn)
        (*lpErrHandFn) (lpMemPTD->lpwszErrorMessage, lpwszLine, uChar, hWndEC);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    if (tkLocalVars.hGlbToken)
    {
        // We no longer need this ptr
        MyGlobalUnlock (tkLocalVars.hGlbToken);
        tkLocalVars.t2.lpBase   = NULL;
////////tkLocalVars.t2.lpHeader = NULL;
        tkLocalVars.lpStart     = NULL;
        tkLocalVars.lpNext      = NULL;
        tkLocalVars.lpLastEOS   = NULL;
    } // End IF

    // Free the handle
    MyGlobalFree (tkLocalVars.hGlbToken); tkLocalVars.hGlbToken = NULL;

    goto FREED_EXIT;

NORMAL_EXIT:
    if (tkLocalVars.hGlbToken)
    {
        // We no longer need this ptr
        MyGlobalUnlock (tkLocalVars.hGlbToken);
        tkLocalVars.t2.lpBase   = NULL;
////////tkLocalVars.t2.lpHeader = NULL;
        tkLocalVars.lpStart     = NULL;
        tkLocalVars.lpNext      = NULL;
        tkLocalVars.lpLastEOS   = NULL;
    } // End IF
UNLOCKED_EXIT:
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    // Display the tokens so far
    DisplayTokens (tkLocalVars.hGlbToken);
#endif
FREED_EXIT:
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsg ("*** Tokenize_EM End");
#endif

    return tkLocalVars.hGlbToken;
} // End Tokenize_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CheckGroupSymbols_EM
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
    if (lptkLocalVars->t2.lpHeader->PrevGroup NE NO_PREVIOUS_GROUPING_SYMBOL)
    {
        ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

        return FALSE;
    } // End IF

    return TRUE;
} // End CheckGroupSymbols_EM
#undef  APPEND_NAME


//***************************************************************************
//  $Untokenize
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
    HGLOBAL hGlbPTD;        // PerTabData global memory handle

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    if (hGlbToken && (lpToken = MyGlobalLock (hGlbToken)))
    {

#define lpHeader    ((LPTOKEN_HEADER) lpToken)
        // It's a token
        Assert (lpHeader->Sig.nature EQ TOKEN_HEADER_SIGNATURE);

        // Get the # tokens
        iLen = lpHeader->TokenCnt;
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
                Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

                // Skip it if a variable with no value
                if (lpToken->tkFlags.TknType EQ TKT_VARNAMED
                 && !lpToken->tkData.tkSym->stFlags.Value)
                    break;

                // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
                if (!lpToken->tkData.tkSym->stFlags.Imm)
                {
                    // stData is an internal function, a valid HGLOBAL variable or function array,
                    //   or user-defined function/operator
                    Assert (lpToken->tkData.tkSym->stFlags.FcnDir
                         || IsGlbTypeVarDir (lpToken->tkData.tkSym->stData.stGlbData)
                         || IsGlbTypeFcnDir (lpToken->tkData.tkSym->stData.stGlbData)
                         || IsGlbTypeDfnDir (lpToken->tkData.tkSym->stData.stGlbData));
                } // End IF

                // Don't free a name's contents
                break;

            case TKT_STRING:            // String  (data is HGLOBAL)
            case TKT_VARARRAY:          // Array of data (data is HGLOBAL)
                // Free the array and all elements of it
                if (FreeResultGlobalVar (ClrPtrTypeDirGlb (lpToken->tkData.tkGlbData)))
                {
#ifdef DEBUG_ZAP
                    dprintfW (L"**Zapping in Untokenize: %08X (%d) (%S#%d)",
                              ClrPtrTypeDir (lpToken->tkData.tkGlbData),
                              FNLN);
#endif
                    lpToken->tkData.tkGlbData = NULL;
                } // End IF

                break;

            case TKT_COMMENT:           // Comment (data is NULL)
            case TKT_ASSIGN:            // Assignment symbol (data is UTF16_LEFTARROW)
            case TKT_LISTSEP:           // List separator    (...     ';')
            case TKT_COLON:             // Label ...         (...     ':')
            case TKT_VARIMMED:          // Immediate data (data is immediate)
            case TKT_FCNIMMED:          // Immediate primitive function (any valence) (data is UTF16_***)
            case TKT_OP1IMMED:          // ...       Monadic primitive operator (data is UTF16_***)
            case TKT_OP2IMMED:          // ...       Dyadic  ...
            case TKT_OPJOTDOT:          // Outer product (data is NULL)
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
            {
                LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

                // Lock the memory to get a ptr to it
                lpMemPTD = MyGlobalLock (hGlbPTD);

                wsprintf (lpszDebug,
                          "Untokenize:  *** Unknown Token Value:  %d",
                          lpToken->tkFlags.TknType);
                DbgMsg (lpszDebug);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
            }
#endif
                break;
        } // End FOR/SWITCH

        // Unlock the handle
        MyGlobalUnlock (hGlbToken); lpToken = NULL;
    } else
    {
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
        LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        wsprintf (lpszDebug,
                  "Untokenize:  hGlobToken (%08X) is invalid.",
                  hGlbToken);
        DbgMsg (lpszDebug);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
#endif
    } // End IF/ELSE
} // End Untokenize
#undef  APPEND_NAME


//***************************************************************************
//  $AppendEOSToken
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
//  $AppendNewToken_EM
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
     LPAPLLONGEST  lptkData,            // Ptr to token data (may be NULL)
     int           iCharOffset)

{
    // Check for TOKEN TABLE FULL
    if (((lptkLocalVars->t2.lpHeader->TokenCnt + 1) * sizeof (TOKEN) + sizeof (TOKEN_HEADER))
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
    if (lptkData)
        lptkLocalVars->lpNext->tkData.tkLongest = *lptkData;
    else
        lptkLocalVars->lpNext->tkData.tkLongest = 0;
    lptkLocalVars->lpNext->tkFlags          = *lptkFlags;// Append the flags

    // Save index in input line of this token
    lptkLocalVars->lpNext->tkCharIndex = iCharOffset + lptkLocalVars->lpwsz - lptkLocalVars->lpwszOrig;

    // Save the ptr to the original token
    lptkLocalVars->lpNext->lptkOrig = lptkLocalVars->lpNext;

    // Count in another token
    lptkLocalVars->t2.lpHeader->TokenCnt++;

    // Skip to next token
    lptkLocalVars->lpNext++;

    return TRUE;
} // End AppendNewToken_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CharTrans
//
//  Translate a character in preparation for tokenizing
//***************************************************************************

WCHAR CharTrans
    (WCHAR wchOrig)

{
    // Split cases
    switch (wchOrig)
    {
        case L'a':
        case L'b':
        case L'c':
        case L'd':
////////case L'e':                      // See below (COL_FPEXP)
        case L'f':
        case L'g':
        case L'h':
////////case L'i':                      // See below (COL_COMPLEX)
////////case L'j':                      // ...       ...
        case L'k':
        case L'l':
        case L'm':
        case L'n':
        case L'o':
        case L'p':
        case L'q':
////////case L'r':                      // See below (COL_RATIONAL)
        case L's':
        case L't':
        case L'u':
        case L'v':
        case L'w':
        case L'x':
        case L'y':
        case L'z':

        case L'A':
        case L'B':
        case L'C':
        case L'D':
////////case L'E':                      // See below (COL_FPEXP)
        case L'F':
        case L'G':
        case L'H':
////////case L'I':                      // See below (COL_COMPLEX)
////////case L'J':                      // ...       ...
        case L'K':
        case L'L':
        case L'M':
        case L'N':
        case L'O':
        case L'P':
        case L'Q':
////////case L'R':                      // See below (COL_RATIONAL)
        case L'S':
        case L'T':
        case L'U':
        case L'V':
        case L'W':
        case L'X':
        case L'Y':
        case L'Z':
        case UTF16_DELTA:               // Alt-'h' - delta
        case UTF16_DELTAUNDERBAR:       // Alt-'H' - delta-underbar
            return COL_ALPHA;

        case L'e':
        case L'E':
            return COL_FPEXP;

        case L'i':
        case L'I':
        case L'j':
        case L'J':
            // Split off the complex number separator
            //   as it can be either 'i', 'I', 'j', or 'J'
            //   and as such doesn't fit into a switch
            //   statement easily.
            if (cComplexSep EQ *CharLowerW (&wchOrig))
                return COL_COMPLEX;
            else
                return COL_ALPHA;

        case L'r':
        case L'R':
            return COL_RATIONAL;

        case L'_':
            return COL_UNDERBAR;

        case L'.':
            return COL_DOT;

        case L'0':
        case L'1':
        case L'2':
        case L'3':
        case L'4':
        case L'5':
        case L'6':
        case L'7':
        case L'8':
        case L'9':
            return COL_DIGIT;

        case L' ':
        case L'\t':
            return COL_SPACE;

        case L'\0':
            return COL_EOL;

        case UTF16_ALPHA:               // Alt-'a' - alpha
        case UTF16_OMEGA:               // Alt-'w' - omega
            return COL_DIRIDENT;

////////case UTF16_ALPHA:               // Alt-'a' - alpha (COL_DIRIDENT)
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
////////case UTF16_OMEGA:               // Alt-'w' - omega (COL_DIRIDENT)
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
        case UTF16_DIERESISTILDE:       // Alt-'T' - commute/duplicate
            return COL_PRIM_OP1;

        case UTF16_DIERESISDEL:         // Alt-'G' - dual
        case UTF16_DIERESISDOWNTACK:    // Alt-'N' - convolution
        case UTF16_DIERESISSTAR:        // Alt-'P' - power
            return COL_PRIM_OP2;

        case UTF16_JOT:                 // Alt-'j' - jot
            return COL_JOT;

        case UTF16_APOSTROPHE:          // Alt-'k' - single quote
            return COL_QUOTE1;

        case UTF16_DOUBLEQUOTE:         //     '"' - double quote
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

        case L';':                      // Lists (bracketed and otherwise)
            return COL_SEMICOLON;

        case L':':                      // Control structures ***FIXME***
            return COL_COLON;

        case UTF16_DELTILDE:            // Alt-'@' - del-tilde
        case UTF16_DEL:                 // Alt-'g' - del
        case UTF16_DIERESISJOT:         // Alt-'J' - rank (hoot)
        case UTF16_DIERESISCIRCLE:      // Alt-'O' - holler
        case L'`':
        case L'@':
        case L'#':
        case L'$':
        case L'%':
        case L'&':
        case L'{':
        case L'}':
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
//  $GetColName
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
{{L"DIGIT"     , COL_DIGIT     },   // 00: Digit
 {L"DOT"       , COL_DOT       },   // 01: Decimal number, inner & outer product separator
 {L"FPEXP"     , COL_FPEXP     },   // 02: Floating point exponent separator
 {L"ALPHA"     , COL_ALPHA     },   // 03: Alphabetic
 {L"DIRIDENT"  , COL_DIRIDENT  },   // 04: Alpha or Omega
 {L"Q_QQ"      , COL_Q_QQ      },   // 05: Quad
 {L"UNDERBAR"  , COL_UNDERBAR  },   // 06: Underbar (infinity)
 {L"OVERBAR"   , COL_OVERBAR   },   // 07: Overbar
 {L"COMPLEX"   , COL_COMPLEX   },   // 08: Complex number separator
 {L"RATIONAL"  , COL_RATIONAL  },   // 09: Rational number separator
 {L"ASSIGN"    , COL_ASSIGN    },   // 0A: Assignment symbol
 {L"SEMICOLON" , COL_SEMICOLON },   // 0B: Semicolon symbol
 {L"COLON"     , COL_COLON     },   // 0C: Colon symbol
 {L"PRIM_FN"   , COL_PRIM_FN   },   // 0D: Primitive monadic or dyadic function
 {L"PRIM_FN0"  , COL_PRIM_FN0  },   // 0E: ...       niladic function
 {L"PRIM_OP1"  , COL_PRIM_OP1  },   // 0F: ...       monadic operator
 {L"PRIM_OP2"  , COL_PRIM_OP2  },   // 10: ...       dyadic  ...
 {L"JOT"       , COL_JOT       },   // 11: Jot symbol
 {L"LPAREN"    , COL_LPAREN    },   // 12: Left paren
 {L"RPAREN"    , COL_RPAREN    },   // 13: Right ...
 {L"LBRACKET"  , COL_LBRACKET  },   // 14: Left bracket
 {L"RBRACKET"  , COL_RBRACKET  },   // 15: Right ...
 {L"SPACE"     , COL_SPACE     },   // 16: White space (' ' or '\t')
 {L"QUOTE1"    , COL_QUOTE1    },   // 17: Single quote symbol
 {L"QUOTE2"    , COL_QUOTE2    },   // 18: Double ...
 {L"DIAMOND"   , COL_DIAMOND   },   // 19: Diamond symbol
 {L"LAMP"      , COL_LAMP      },   // 1A: Comment symbol
 {L"EOL"       , COL_EOL       },   // 1B: End-Of-Line
 {L"UNK"       , COL_UNK       },   // 1C: Unknown symbols
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


#ifdef DEBUG
//***************************************************************************
//  $InitFsaTabs
//
//  Ensure the FSA tables have been properly setup
//***************************************************************************

void InitFsaTabs
    (void)

{
    // Ensure we calculated the lengths properly
    Assert (sizeof (fsaColTable) EQ (COL_LENGTH * sizeof (FSA_ACTION) * FSA_LENGTH));
} // End InitFsaTabs
#endif


//***************************************************************************
//  End of File: exec.c
//***************************************************************************
