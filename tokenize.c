//***************************************************************************
//  NARS2000 -- Tokenizer
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

#define STRICT
#define _WIN32_WINNT    0x0400          // Needed for TryEnterCriticalSection
#include <windows.h>
#include <math.h>
#include "headers.h"


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

UBOOL bInUse = FALSE;

#define DEF_TOKEN_SIZE  1024    // Default initial amount of memory
                                //   allocated for the tokenized line
#define DEF_TOKEN_RESIZE 512    // Default increment when GlobalRealloc'ing


#define fnAlpInit   fnAlpha
#define fnAlpAccum  fnAlpha
#define fnSysInit   fnAlpha
#define fnSysAccum  fnAlpha
#define fnSysDone   fnAlpDone
#define fnQuo1Init  fnQuoInit
#define fnQuo2Init  fnQuoInit
#define fnQuo1Accum fnQuoAccum
#define fnQuo2Accum fnQuoAccum
#define fnQuo1Done  fnQuoDone
#define fnQuo2Done  fnQuoDone
#define fnQuo1Exit  fnQuoExit
#define fnQuo2Exit  fnQuoExit

FSA_ACTION fsaColTable [][COL_LENGTH]
#ifndef PROTO
 =
{   // FSA_SOS      Start of stmt ('')
 {{FSA_POINTNOT, NULL        , fnPointAcc  },   // '0123456789'
  {FSA_DOTAMBIG, NULL        , fnPointAcc  },   // '.'
  {FSA_ALPHA   , NULL        , fnAlpInit   },   // 'a..zA..Z'
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // Overbar
  {FSA_INIT    , NULL        , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , NULL        , fnSysInit   },   // Quad
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // Underbar
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // Infinity
  {FSA_INIT    , NULL        , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , NULL        , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , NULL        , fnClnDone   },   // Colon  ...
  {FSA_INIT    , NULL        , fnCtrlDone  },   // Control Structure
  {FSA_INIT    , NULL        , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , NULL        , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , NULL        , fnOp1Done   },   // ...       monadic/ambiguous operator
  {FSA_INIT    , NULL        , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, NULL        , NULL        },   // Jot
  {FSA_INIT    , NULL        , fnParInit   },   // Left paren
  {FSA_INIT    , NULL        , fnParDone   },   // Right ...
  {FSA_INIT    , NULL        , fnBrkInit   },   // Left bracket
  {FSA_INIT    , NULL        , fnBrkDone   },   // Right ...
  {FSA_INIT    , NULL        , fnBrcInit   },   // Left brace
  {FSA_INIT    , NULL        , fnBrcDone   },   // Right ...
  {FSA_INIT    , NULL        , fnSyntWhite },   // White space
  {FSA_QUOTE1A , NULL        , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , NULL        , fnQuo2Init  },   // Double ...
  {FSA_SOS     , NULL        , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , NULL        , fnComDone   },   // Comment symbol
  {FSA_EXIT    , NULL        , NULL        },   // EOL
  {FSA_INIT    , NULL        , fnUnkDone   },   // Unknown symbols
 },
    // FSA_INIT     Initial state ('')
 {{FSA_POINTNOT, NULL        , fnPointAcc  },   // '0123456789'
  {FSA_DOTAMBIG, NULL        , fnPointAcc  },   // '.'
  {FSA_ALPHA   , NULL        , fnAlpInit   },   // 'a..zA..Z'
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // Overbar
  {FSA_INIT    , NULL        , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , NULL        , fnSysInit   },   // Quad
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // Underbar
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // Infinity
  {FSA_INIT    , NULL        , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , NULL        , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , NULL        , fnClnDone   },   // Colon  ...
  {FSA_INIT    , NULL        , fnCtrlDone  },   // Control Structure
  {FSA_INIT    , NULL        , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , NULL        , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , NULL        , fnOp1Done   },   // ...       monadic/ambiguous operator
  {FSA_INIT    , NULL        , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, NULL        , NULL        },   // Jot
  {FSA_INIT    , NULL        , fnParInit   },   // Left paren
  {FSA_INIT    , NULL        , fnParDone   },   // Right ...
  {FSA_INIT    , NULL        , fnBrkInit   },   // Left bracket
  {FSA_INIT    , NULL        , fnBrkDone   },   // Right ...
  {FSA_INIT    , NULL        , fnBrcInit   },   // Left brace
  {FSA_INIT    , NULL        , fnBrcDone   },   // Right ...
  {FSA_INIT    , NULL        , fnSyntWhite },   // White space
  {FSA_QUOTE1A , NULL        , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , NULL        , fnQuo2Init  },   // Double ...
  {FSA_SOS     , NULL        , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , NULL        , fnComDone   },   // Comment symbol
  {FSA_EXIT    , NULL        , NULL        },   // EOL
  {FSA_INIT    , NULL        , fnUnkDone   },   // Unknown symbols
 },
    // FSA_POINTNOT Point Notation
 {{FSA_POINTNOT, NULL        , fnPointAcc  },   // '0123456789'
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // '.'
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // 'a..zA..Z'
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // Overbar
  {FSA_INIT    , fnPointDone , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnPointDone , fnSysInit   },   // Quad
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // Underbar
  {FSA_POINTNOT, NULL        , fnPointAcc  },   // Infinity
  {FSA_INIT    , fnPointDone , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnPointDone , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnPointDone , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnPointDone , fnCtrlDone  },   // Control Structure
  {FSA_INIT    , fnPointDone , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnPointDone , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnPointDone , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnPointDone , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnPointDone , NULL        },   // Jot
  {FSA_INIT    , fnPointDone , fnParInit   },   // Left paren
  {FSA_INIT    , fnPointDone , fnParDone   },   // Right ...
  {FSA_INIT    , fnPointDone , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnPointDone , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnPointDone , fnBrcInit   },   // Left brace
  {FSA_INIT    , fnPointDone , fnBrcDone   },   // Right ...
  {FSA_INIT    , fnPointDone , fnSyntWhite },   // White space
  {FSA_QUOTE1A , fnPointDone , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnPointDone , fnQuo2Init  },   // Double ...
  {FSA_SOS     , fnPointDone , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnPointDone , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnPointDone , NULL        },   // EOL
  {FSA_INIT    , fnPointDone , fnUnkDone   },   // Unknown symbols
 },
    // FSA_ALPHA    Alphabetic char
 {{FSA_ALPHA   , fnAlpAccum  , NULL        },   // '0123456789'
  {FSA_DOTAMBIG, fnAlpDone   , fnPointAcc  },   // '.'
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // 'a..zA..Z'
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // Overbar
  {FSA_INIT    , fnAlpDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnAlpDone   , fnSysInit   },   // Quad
  {FSA_ALPHA   , fnAlpAccum  , NULL        },   // Underbar
  {FSA_POINTNOT, fnAlpDone   , fnPointAcc  },   // Infinity
  {FSA_INIT    , fnAlpDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnAlpDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnAlpDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnAlpDone   , fnCtrlDone  },   // Control Structure
  {FSA_INIT    , fnAlpDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnAlpDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnAlpDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnAlpDone   , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnAlpDone   , NULL        },   // Jot
  {FSA_INIT    , fnAlpDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnAlpDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnAlpDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnAlpDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnAlpDone   , fnBrcInit   },   // Left brace
  {FSA_INIT    , fnAlpDone   , fnBrcDone   },   // Right ...
  {FSA_INIT    , fnAlpDone   , fnSyntWhite },   // White space
  {FSA_QUOTE1A , fnAlpDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnAlpDone   , fnQuo2Init  },   // Double ...
  {FSA_SOS     , fnAlpDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnAlpDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnAlpDone   , NULL        },   // EOL
  {FSA_INIT    , fnAlpDone   , fnUnkDone   },   // Unknown symbols
 },
    // FSA_SYSNAME  System name (begins with a Quad or Quote-quad)
 {{FSA_SYSNAME , fnSysAccum  , NULL        },   // '0123456789'
  {FSA_DOTAMBIG, fnSysDone   , fnPointAcc  },   // '.'
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // 'a..zA..Z'
  {FSA_POINTNOT, fnSysDone   , fnPointAcc  },   // Overbar
  {FSA_INIT    , fnSysDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnSysDone   , fnSysInit   },   // Quad
  {FSA_SYSNAME , fnSysAccum  , NULL        },   // Underbar
  {FSA_POINTNOT, fnSysDone   , fnPointAcc  },   // Infinity
  {FSA_INIT    , fnSysDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnSysDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnSysDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnSysDone   , fnCtrlDone  },   // Control Structure
  {FSA_INIT    , fnSysDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnSysDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnSysDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnSysDone   , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnSysDone   , NULL        },   // Jot
  {FSA_INIT    , fnSysDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnSysDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnSysDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnSysDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnSysDone   , fnBrcInit   },   // Left brace
  {FSA_INIT    , fnSysDone   , fnBrcDone   },   // Right ...
  {FSA_INIT    , fnSysDone   , fnSyntWhite },   // White space
  {FSA_QUOTE1A , fnSysDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnSysDone   , fnQuo2Init  },   // Double ...
  {FSA_SOS     , fnSysDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnSysDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnSysDone   , NULL        },   // EOL
  {FSA_INIT    , fnSysDone   , fnUnkDone   },   // Unknown symbols
 },
    // FSA_QUOTE1A  Start of or within single quoted char or char vector
 {{FSA_QUOTE1A , fnQuo1Accum , NULL        },   // '0123456789'
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // '.'
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // 'a..zA..Z'
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Overbar
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Alpha or Omega
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Quad
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Underbar
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Infinity
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Assignment symbol
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Semicolon  ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Colon  ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Control Structure
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Primitive monadic or dyadic function
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // ...       niladic           ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // ...       monadic operator
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // ...       dyadic  ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Jot
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Left paren
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Right ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Left bracket
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Right ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Left brace
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Right ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // White space
  {FSA_QUOTE1Z , NULL        , fnSyntQuote },   // Single quote
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Double ...
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Diamond symbol
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Comment symbol
  {FSA_EXIT    , fnQuo1Exit  , fnUnkDone   },   // EOL
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Unknown symbols
 },
    // FSA_QUOTE1Z  End of single quoted char or char vector
 {{FSA_POINTNOT, fnQuo1Done  , fnPointAcc  },   // '0123456789'
  {FSA_DOTAMBIG, fnQuo1Done  , fnPointAcc  },   // '.'
  {FSA_ALPHA   , fnQuo1Done  , fnAlpInit   },   // 'a..zA..Z'
  {FSA_POINTNOT, fnQuo1Done  , fnPointAcc  },   // Overbar
  {FSA_INIT    , fnQuo1Done  , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnQuo1Done  , fnSysInit   },   // Quad
  {FSA_POINTNOT, fnQuo1Done  , fnPointAcc  },   // Underbar
  {FSA_POINTNOT, fnQuo1Done  , fnPointAcc  },   // Infinity
  {FSA_INIT    , fnQuo1Done  , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnQuo1Done  , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnQuo1Done  , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnQuo1Done  , fnCtrlDone  },   // Control Structure
  {FSA_INIT    , fnQuo1Done  , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnQuo1Done  , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnQuo1Done  , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnQuo1Done  , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnQuo1Done  , NULL        },   // Jot
  {FSA_INIT    , fnQuo1Done  , fnParInit   },   // Left paren
  {FSA_INIT    , fnQuo1Done  , fnParDone   },   // Right ...
  {FSA_INIT    , fnQuo1Done  , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnQuo1Done  , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnQuo1Done  , fnBrcInit   },   // Left brace
  {FSA_INIT    , fnQuo1Done  , fnBrcDone   },   // Right ...
  {FSA_INIT    , fnQuo1Done  , fnSyntWhite },   // White space
  {FSA_QUOTE1A , fnQuo1Accum , NULL        },   // Single quote
  {FSA_QUOTE2A , fnQuo1Done  , fnQuo2Init  },   // Double ...
  {FSA_SOS     , fnQuo1Done  , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnQuo1Done  , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnQuo1Done  , NULL        },   // EOL
  {FSA_INIT    , fnQuo1Done  , fnUnkDone   },   // Unknown symbols
 },
    // FSA_QUOTE2A  Start of or within double quoted char or char vector
 {{FSA_QUOTE2A , fnQuo2Accum , NULL        },   // '0123456789'
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // '.'
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // 'a..zA..Z'
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Overbar
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Alpha or Omega
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Quad
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Underbar
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Infinity
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Assignment symbol
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Semicolon  ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Colon  ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Control Structure
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Primitive monadic or dyadic function
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // ...       niladic           ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // ...       monadic operator
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // ...       dyadic  ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Jot
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Left paren
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Right ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Left bracket
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Right ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Left brace
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Right ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // White space
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Single quote
  {FSA_QUOTE2Z , NULL        , fnSyntQuote },   // Double ...
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Diamond symbol
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Comment symbol
  {FSA_EXIT    , fnQuo2Exit  , fnUnkDone   },   // EOL
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Unknown symbols
 },
    // FSA_QUOTE2Z  End of double quoted char or char vector
 {{FSA_POINTNOT, fnQuo2Done  , fnPointAcc  },   // '0123456789'
  {FSA_DOTAMBIG, fnQuo2Done  , fnPointAcc  },   // '.'
  {FSA_ALPHA   , fnQuo2Done  , fnAlpInit   },   // 'a..zA..Z'
  {FSA_POINTNOT, fnQuo2Done  , fnPointAcc  },   // Overbar
  {FSA_INIT    , fnQuo2Done  , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnQuo2Done  , fnSysInit   },   // Quad
  {FSA_POINTNOT, fnQuo2Done  , fnPointAcc  },   // Underbar
  {FSA_POINTNOT, fnQuo2Done  , fnPointAcc  },   // Infinity
  {FSA_INIT    , fnQuo2Done  , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnQuo2Done  , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnQuo2Done  , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnQuo2Done  , fnCtrlDone  },   // Control Structure
  {FSA_INIT    , fnQuo2Done  , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnQuo2Done  , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnQuo2Done  , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnQuo2Done  , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnQuo2Done  , NULL        },   // Jot
  {FSA_INIT    , fnQuo2Done  , fnParInit   },   // Left paren
  {FSA_INIT    , fnQuo2Done  , fnParDone   },   // Right ...
  {FSA_INIT    , fnQuo2Done  , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnQuo2Done  , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnQuo2Done  , fnBrcInit   },   // Left brace
  {FSA_INIT    , fnQuo2Done  , fnBrcDone   },   // Right ...
  {FSA_INIT    , fnQuo2Done  , fnSyntWhite },   // White space
  {FSA_QUOTE1A , fnQuo2Done  , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnQuo2Accum , NULL        },   // Double ...
  {FSA_SOS     , fnQuo2Done  , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnQuo2Done  , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnQuo2Done  , NULL        },   // EOL
  {FSA_INIT    , fnQuo2Done  , fnUnkDone   },   // Unknown symbols
 },
    // FSA_DOTAMBIG Ambiguous dot:  either FSA_POINTNOT or FSA_INIT w/fnOp2Done ('+.' or 'name.' or '[]name.')
 {{FSA_POINTNOT, NULL        , fnPointAcc  },   // '0123456789'
  {FSA_DOTAMBIG, fnDotDone   , fnPointAcc  },   // '.'
  {FSA_ALPHA   , fnDotDone   , fnAlpInit   },   // 'a..zA..Z'
  {FSA_POINTNOT, fnDotDone   , fnPointAcc  },   // Overbar
  {FSA_INIT    , fnDotDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnDotDone   , fnSysInit   },   // Quad
  {FSA_POINTNOT, fnDotDone   , fnPointAcc  },   // Underbar
  {FSA_POINTNOT, fnDotDone   , fnPointAcc  },   // Infinity
  {FSA_INIT    , fnDotDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnDotDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnDotDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnDotDone   , fnCtrlDone  },   // Control Structure
  {FSA_INIT    , fnDotDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnDotDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnDotDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnDotDone   , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnDotDone   , NULL        },   // Jot
  {FSA_INIT    , fnDotDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnDotDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnDotDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnDotDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnDotDone   , fnBrcInit   },   // Left brace
  {FSA_INIT    , fnDotDone   , fnBrcDone   },   // Right ...
  {FSA_INIT    , fnDotDone   , fnSyntWhite },   // White space
  {FSA_QUOTE1A , fnDotDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnDotDone   , fnQuo2Init  },   // Double ...
  {FSA_SOS     , fnDotDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnDotDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnDotDone   , NULL        },   // EOL
  {FSA_INIT    , fnDotDone   , fnUnkDone   },   // Unknown symbols
 },
    // FSA_JOTAMBIG Ambiguous jot:  either FSA_OUTAMBIG or normal w/fnJotDone ('J')
 {{FSA_POINTNOT, fnJotDone   , fnPointAcc  },   // '0123456789'
  {FSA_OUTAMBIG, fnSyntPrm   , fnPointAcc  },   // '.'
  {FSA_ALPHA   , fnJotDone   , fnAlpInit   },   // 'a..zA..Z'
  {FSA_POINTNOT, fnJotDone   , fnPointAcc  },   // Overbar
  {FSA_INIT    , fnJotDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnJotDone   , fnSysInit   },   // Quad
  {FSA_POINTNOT, fnJotDone   , fnPointAcc  },   // Underbar
  {FSA_POINTNOT, fnJotDone   , fnPointAcc  },   // Infinity
  {FSA_INIT    , fnJotDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnJotDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnJotDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnJotDone   , fnCtrlDone  },   // Control Structure
  {FSA_INIT    , fnJotDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnJotDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnJotDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnJotDone   , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnJotDone   , NULL        },   // Jot
  {FSA_INIT    , fnJotDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnJotDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnJotDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnJotDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnJotDone   , fnBrcInit   },   // Left brace
  {FSA_INIT    , fnJotDone   , fnBrcDone   },   // Right ...
  {FSA_JOTAMBIG, NULL        , fnSyntWhite },   // White space
  {FSA_QUOTE1A , fnJotDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnJotDone   , fnQuo2Init  },   // Double ...
  {FSA_SOS     , fnJotDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnJotDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnJotDone   , NULL        },   // EOL
  {FSA_INIT    , fnJotDone   , fnUnkDone   },   // Unknown symbols
 },
    // FSA_OUTAMBIG Ambiguous outer product:  either FSA_INIT w/fnOutDone or FSA_POINTNOT w/fnOutDone ('J.')
 {{FSA_POINTNOT, fnJotDone0  , fnPointAcc  },   // '0123456789'
  {FSA_JOTAMBIG, fnOutDone   , fnPointAcc  },   // '.'
  {FSA_ALPHA   , fnOutDone   , fnAlpInit   },   // 'a..zA..Z'
  {FSA_POINTNOT, fnOutDone   , fnPointAcc  },   // Overbar
  {FSA_INIT    , fnOutDone   , fnDirIdent  },   // Alpha or Omega
  {FSA_SYSNAME , fnOutDone   , fnSysInit   },   // Quad
  {FSA_POINTNOT, fnOutDone   , fnPointAcc  },   // Underbar
  {FSA_POINTNOT, fnOutDone   , fnPointAcc  },   // Infinity
  {FSA_INIT    , fnOutDone   , fnAsnDone   },   // Assignment symbol
  {FSA_INIT    , fnOutDone   , fnLstDone   },   // Semicolon  ...
  {FSA_INIT    , fnOutDone   , fnClnDone   },   // Colon  ...
  {FSA_INIT    , fnOutDone   , fnCtrlDone  },   // Control Structure
  {FSA_INIT    , fnOutDone   , fnPrmDone   },   // Primitive monadic or dyadic function
  {FSA_INIT    , fnOutDone   , fnPrmDone   },   // ...       niladic           ...
  {FSA_INIT    , fnOutDone   , fnOp1Done   },   // ...       monadic operator
  {FSA_INIT    , fnOutDone   , fnOp2Done   },   // ...       dyadic  ...
  {FSA_JOTAMBIG, fnOutDone   , NULL        },   // Jot
  {FSA_INIT    , fnOutDone   , fnParInit   },   // Left paren
  {FSA_INIT    , fnOutDone   , fnParDone   },   // Right ...
  {FSA_INIT    , fnOutDone   , fnBrkInit   },   // Left bracket
  {FSA_INIT    , fnOutDone   , fnBrkDone   },   // Right ...
  {FSA_INIT    , fnOutDone   , fnBrcInit   },   // Left brace
  {FSA_INIT    , fnOutDone   , fnBrcDone   },   // Right ...
  {FSA_OUTAMBIG, NULL        , fnSyntWhite },   // White space
  {FSA_QUOTE1A , fnOutDone   , fnQuo1Init  },   // Single quote
  {FSA_QUOTE2A , fnOutDone   , fnQuo2Init  },   // Double ...
  {FSA_SOS     , fnOutDone   , fnDiaDone   },   // Diamond symbol
  {FSA_INIT    , fnOutDone   , fnComDone   },   // Comment symbol
  {FSA_EXIT    , fnOutDone   , NULL        },   // EOL
  {FSA_INIT    , fnOutDone   , fnUnkDone   },   // Unknown symbols
 },
}
#endif
;


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
    (LPTKLOCALVARS lptkLocalVars)

{
    // Integers & floating points
    lptkLocalVars->iNumLen    = 0;
    lptkLocalVars->iStrLen    = 0;
} // End InitAccumVars


//***************************************************************************
//  $CheckResizeNum_EM
//
//  See if we need to resize the Numeric global var
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckResizeNum_EM"
#else
#define APPEND_NAME
#endif

UBOOL CheckResizeNum_EM
    (LPTKLOCALVARS lptkLocalVars)

{
    UBOOL   bRet = FALSE;       // TRUE iff result is valid
    int     iNumLim;            // Temporary iNumLim
    HGLOBAL hGlbNum;            // Temporary hGlbNum

    // Check for need to resize hGlbNum
    //   allowing for one more char and a terminating zero
    if (lptkLocalVars->iNumLen >= (lptkLocalVars->iNumLim - 1))
    {
        // Get desired size
        iNumLim = lptkLocalVars->iNumLim + DEF_NUM_INCRNELM;

        // If it's out of range, ...
        if (iNumLim > DEF_NUM_MAXNELM)
            goto LIMIT_EXIT;
        else
        {
            // Attempt to realloc to that size
            hGlbNum =
              MyGlobalReAlloc (lptkLocalVars->hGlbNum, iNumLim * sizeof (char), GMEM_MOVEABLE);
            if (!hGlbNum)
                goto WSFULL_EXIT;

            // Save back in PTD var
            lptkLocalVars->iNumLim = iNumLim;
            lptkLocalVars->hGlbNum = hGlbNum;
        } // End IF
    } // End IF

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

LIMIT_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    goto ERROR_EXIT;

WSFULL_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return bRet;
} // End CheckResizeNum_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CheckResizeStr_EM
//
//  See if we need to resize the String global var
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckResizeStr_EM"
#else
#define APPEND_NAME
#endif

UBOOL CheckResizeStr_EM
    (LPTKLOCALVARS lptkLocalVars)

{
    UBOOL   bRet = FALSE;       // TRUE iff result is valid
    int     iStrLim;            // Temporary iStrLim
    HGLOBAL hGlbStr;            // Temporary hGlbStr

    // Check for need to resize hGlbStr
    //   allowing for one more char and a terminating zero
    if (lptkLocalVars->iStrLen >= (lptkLocalVars->iStrLim - 1))
    {
        // Get desired size
        iStrLim = lptkLocalVars->iStrLim + DEF_STR_INCRNELM;

        // If it's out of range, ...
        if (iStrLim > DEF_STR_MAXNELM)
            goto LIMIT_EXIT;
        else
        {
            // Attempt to realloc to that size
            hGlbStr =
              MyGlobalReAlloc (lptkLocalVars->hGlbStr, iStrLim * sizeof (APLCHAR), GMEM_MOVEABLE);
            if (!hGlbStr)
                goto WSFULL_EXIT;

            // Save back in PTD var
            lptkLocalVars->iStrLim = iStrLim;
            lptkLocalVars->hGlbStr = hGlbStr;
        } // End IF
    } // End IF

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

LIMIT_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    goto ERROR_EXIT;

WSFULL_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return bRet;
} // End CheckResizeStr_EM
#undef  APPEND_NAME


//***************************************************************************
//  $IsLocalName
//
//  Determine whether or not a give name is local to a function header
//***************************************************************************

UBOOL IsLocalName
    (LPWCHAR lpwszStr,                  // Ptr to the name
     UINT    iStrLen,                   // Length of the name
     HWND    hWndEC,                    // Handle to Edit Ctrl window
     LPWCHAR lpwszTemp,                 // Ptr to temp storage
     LPUINT  lpPosition)                // Ptr to save area for position in line
                                        // (if not found, position of 1st semicolon or EOL)
                                        // (may be NULL if position not desired)

{
    LPWCHAR lpwszName,                  // Ptr to name
            wp,                         // Ptr to temp char
            lpwBrkLead = L"({ });" WS_UTF16_LEFTARROW,
            lpwBrkTerm = L"({ });" WS_UTF16_LEFTARROW WS_UTF16_LAMP;
    WCHAR   sysName[32];                // Temp storage for sysnames in lowercase

    // If this is a sysname, ...
    if (IsSysName (lpwszStr))
    {
        Assert (iStrLen < countof (sysName));       // ***FIXME*** -- may overflow

        // Copy the sysname to local storage
        CopyMemory (sysName, lpwszStr, iStrLen * sizeof (lpwszStr[0]));

        // Convert it to lowercase
        CharLowerBuffW (sysName, iStrLen);

        lpwszName = sysName;
    } else
        lpwszName = lpwszStr;

    // Tell EM_GETLINE maximum # chars in the buffer
    lpwszTemp[0] = (WORD) -1;

    // Get the function header line
    SendMessageW (hWndEC, EM_GETLINE, 0, (LPARAM) lpwszTemp);

    // Append a trailing marker
    lstrcatW (lpwszTemp, WS_UTF16_LAMP);

    // Copy the base of the line
    wp = lpwszTemp;

    while (TRUE)
    {
        // Skip over leading ignorable chars
        while (strchrW (lpwBrkLead, *wp))
            wp++;

        // Check for ending char
        if (*wp EQ UTF16_LAMP)
            break;

        // Compare the incoming name with the header text
        if (strncmpW (lpwszName, wp, iStrLen) EQ 0
         && (wp[iStrLen] EQ L'\0'
          || strchrW (lpwBrkTerm, wp[iStrLen]) NE NULL))
        {
            // Mark as FOUND
            if (lpPosition)
               *lpPosition = (UINT) (wp - lpwszTemp);

            return TRUE;
        } // End IF

        // Find next terminating char
        wp = strpbrkW (wp, lpwBrkTerm);
    } // End WHILE

    // If position is desired, ...
    if (lpPosition)
    {
        // Remove the trailing lamp
        lpwszTemp[lstrlenW (lpwszTemp) - 1] = L'\0';

        // Zap any other lamp
        *SkipToCharW (lpwszTemp, UTF16_LAMP) = L'\0';

        // Find first semicolon
        wp = SkipToCharW (lpwszTemp, L';');

        // If no semicolon, backup over trailing blanks
        if (*wp NE L';')
            while (wp > lpwszTemp && wp[-1] EQ L' ')
                wp--;

        // Mark as NOT FOUND
        *lpPosition = (UINT) (wp - lpwszTemp);
    } // End IF

    return FALSE;
} // End IsLocalName


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

UBOOL fnAlpha
    (LPTKLOCALVARS lptkLocalVars)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    UBOOL        bRet;          // TRUE iff result is valid
    LPWCHAR      lpwszStr;      // Ptr to Str global memory

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnAlpha");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Check for need to resize hGlbStr
    bRet = CheckResizeStr_EM (lptkLocalVars);
    if (!bRet)
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpwszStr = MyGlobalLock (lptkLocalVars->hGlbStr);

    // Check for overflow -- LIMIT ERROR
    bRet = (lptkLocalVars->iStrLen < DEF_STR_MAXNELM);
    if (bRet)
        // Save the char
        lpwszStr[lptkLocalVars->iStrLen++] = *lptkLocalVars->lpwszCur;
    else
        // Save the error message
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        STFLAGS    stFlags = {0};           // STE flags
        UINT       uVar,                    // Loop counter
                   uLen,                    // Loop length
                   uClr;                    // Color index
        LPSYMENTRY lpSymEntry;              // Ptr to this name's SYMENTRY

        // Get the number of chars
        uLen = lptkLocalVars->iStrLen;

        // Save the column index
        lptkLocalVars->lpMemClrNxt->colIndex = COL_ALPHA;

        // If it's the first character, ...
        if (uLen EQ 1)
        {
            // Save index in lpMemClrIni of the name start
            lptkLocalVars->NameInit = (UINT) (lptkLocalVars->lpMemClrNxt - lptkLocalVars->lpMemClrIni);

            // If this is a SysName, ...
            if (IsSysName (lptkLocalVars->lpwszCur))
            {
                // Save the color
                lptkLocalVars->lpMemClrNxt++->syntClr =
                  gSyntaxColors[SC_PRIMITIVE];

                // Save the name type
                lptkLocalVars->scNameType = SC_NAMETYPE_PRIMITIVE;

                goto NORMAL_EXIT;
            } else
                // Save the name type
                lptkLocalVars->scNameType = SC_NAMETYPE_GLBNAME;
        } // End IF

        // Split cases based upon the name type
        switch (lptkLocalVars->scNameType)
        {
            case SC_NAMETYPE_GLBNAME:
            case SC_NAMETYPE_LCLNAME:
                // If we're called from FE, look in the function header
                if (IzitFE (GetParent (lptkLocalVars->hWndEC)))
                {
                    // Check for global vs. local
                    if (IsLocalName (lpwszStr, lptkLocalVars->iStrLen, lptkLocalVars->hWndEC, lpMemPTD->lpwszTemp, NULL))
                    {
                        uClr = SC_LCLNAME;

                        // Save the name type
                        lptkLocalVars->scNameType = SC_NAMETYPE_LCLNAME;
                    } else
                    {
                        uClr = SC_GLBNAME;

                        // Save the name type
                        lptkLocalVars->scNameType = SC_NAMETYPE_GLBNAME;
                    } // End IF/ELSE
                } else
                // If not, check the STE
                {
                    // Check the name in lpwszStr for global vs. local
                    lpSymEntry =
                      SymTabLookupNameLength (lpwszStr,
                                              lptkLocalVars->iStrLen,
                                             &stFlags);
                    // If there's a shadow entry, ...
                    if (lpSymEntry && lpSymEntry->stPrvEntry)
                    {
                        // Save the color index
                        uClr = SC_LCLNAME;

                        // Save the name type
                        lptkLocalVars->scNameType = SC_NAMETYPE_LCLNAME;
                    } else
                    {
                        // Save the color index
                        uClr = SC_GLBNAME;

                        // Save the name type
                        lptkLocalVars->scNameType = SC_NAMETYPE_GLBNAME;
                    } // End IF/ELSE
                } // End IF/ELSE

                // Skip over the current color
                *lptkLocalVars->lpMemClrNxt++;

                // Loop through the previous and current chars
                for (uVar = 0; uVar < uLen; uVar++)
                    // Resave the color
                    lptkLocalVars->lpMemClrIni[lptkLocalVars->NameInit + uVar].syntClr =
                      gSyntaxColors[uClr];
                break;

            case SC_NAMETYPE_SYSFCN:
            case SC_NAMETYPE_GLBSYSVAR:
            case SC_NAMETYPE_LCLSYSVAR:
            case SC_NAMETYPE_SYSUNK:
            case SC_NAMETYPE_PRIMITIVE:
                // Check the name in lpwszStr for sysfcn vs. sysvar
                lpSymEntry =
                  SymTabLookupNameLength (lpwszStr,
                                          lptkLocalVars->iStrLen,
                                         &stFlags);
                // Skip over the current color
                *lptkLocalVars->lpMemClrNxt++;

                // If the STE is available, ...
                if (lpSymEntry)
                    stFlags = lpSymEntry->stFlags;

                // Split cases based upon the nametype
                switch (stFlags.stNameType)
                {
                    case NAMETYPE_VAR:
                        // Check for global vs. local
                        if (IsLocalName (lpwszStr, lptkLocalVars->iStrLen, lptkLocalVars->hWndEC, lpMemPTD->lpwszTemp, NULL))
                        {
                            // Save the color index
                            uClr = SC_LCLSYSVAR;

                            // Save the name type
                            lptkLocalVars->scNameType = SC_NAMETYPE_LCLSYSVAR;
                        } else
                        {
                            // Save the color index
                            uClr = SC_GLBSYSVAR;

                            // Save the name type
                            lptkLocalVars->scNameType = SC_NAMETYPE_GLBSYSVAR;
                        } // End IF/ELSE

                        break;

                    case NAMETYPE_FN0:
                    case NAMETYPE_FN12:
                        // Save the color index
                        uClr = SC_SYSFCN;

                        // Save the name type
                        lptkLocalVars->scNameType = SC_NAMETYPE_SYSFCN;

                        break;

                    default:
                        // Save the color index
                        uClr = SC_UNK;

                        // Save the name type
                        lptkLocalVars->scNameType = SC_NAMETYPE_SYSUNK;

                        break;
                } // End SWITCH

                // Loop through the previous and current chars
                for (uVar = 0; uVar < uLen; uVar++)
                    // Resave the color
                    lptkLocalVars->lpMemClrIni[lptkLocalVars->NameInit + uVar].syntClr =
                      gSyntaxColors[uClr];
                break;

            defstop
                break;
        } // End SWITCH
    } // End IF
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (lptkLocalVars->hGlbStr); lpwszStr = NULL;
ERROR_EXIT:
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

UBOOL fnAlpDone
    (LPTKLOCALVARS lptkLocalVars)

{
    LPSYMENTRY   lpSymEntry;    // Ptr to the name's STE
    UBOOL        bRet = TRUE;   // TRUE iff the result is valid
    APLINT       aplInteger;    // A temporary integer
    TKFLAGS      tkFlags = {0}; // Token flags
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPWCHAR      lpwszStr;      // Ptr to Str global memory

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnAlpDone");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Lock the memory to get a ptr to it
    lpwszStr = MyGlobalLock (lptkLocalVars->hGlbStr);

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
        goto NORMAL_EXIT;

    // Ensure properly terminated
    lpwszStr[lptkLocalVars->iStrLen] = L'\0';

    // If this is a system name (starts with a Quad or Quote-Quad),
    //   convert it to lowercase as those names are
    //   case-insensitive
    if (IsSysName (lpwszStr))
    {
        // Handle Quad and QuoteQuad alone via a separate token
        if (lptkLocalVars->iStrLen EQ 1)
        {
            // Mark the data as Quad or QuoteQuad
            tkFlags.TknType = TKT_INPOUT;

            // Copy to local var so we may pass its address
            aplInteger = lpwszStr[0];

            // Attempt to append as new token, check for TOKEN TABLE FULL,
            //   and resize as necessary.
            bRet = AppendNewToken_EM (lptkLocalVars,
                                     &tkFlags,
                                     &aplInteger,
                                      -lptkLocalVars->iStrLen);
            goto NORMAL_EXIT;
        } else
            CharLowerBuffW (lpwszStr, lptkLocalVars->iStrLen);
    } // End IF

    // Lookup in or append to the symbol table
    lpSymEntry = SymTabAppendName_EM (lpwszStr, NULL);
    if (!lpSymEntry)
        lpSymEntry = lpMemPTD->steNoValue;

    // Mark the data as a symbol table entry
    tkFlags.TknType = TKT_VARNAMED;
////tkFlags.ImmType = IMMTYPE_ERROR;        // Already zero from {0}

    // Copy to local var so we may pass its address
    (LPSYMENTRY) aplInteger = MakePtrTypeSym (lpSymEntry);

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                             &tkFlags,
                             &aplInteger,
                              -lptkLocalVars->iStrLen);
NORMAL_EXIT:
    //  Initialize the accumulation variables for the next constant
    InitAccumVars (lptkLocalVars);

    // We no longer need this ptr
    MyGlobalUnlock (lptkLocalVars->hGlbStr); lpwszStr = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnAlpDone


//***************************************************************************
//  $fnDirIdent
//
//  End of direct identifier
//***************************************************************************

UBOOL fnDirIdent
    (LPTKLOCALVARS lptkLocalVars)

{
    LPSYMENTRY   lpSymEntry;        // Ptr to the SYMENTRY for the name
    UBOOL        bRet;              // TRUE iff the result is valid
    APLINT       aplInteger;
    TKFLAGS      tkFlags = {0};
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPWCHAR      lpwszStr;          // Ptr to Str global memory

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnDirIdent");
#endif

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the column index
        lptkLocalVars->lpMemClrNxt->colIndex = COL_PRIM_FN;

        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_GLBNAME];

        // Mark as successful
        bRet = TRUE;

        goto NORMAL_EXIT;
    } // End IF

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Lock the memory to get a ptr to it
    lpwszStr = MyGlobalLock (lptkLocalVars->hGlbStr);

    // Save the character in the string
    lpwszStr[0] = lptkLocalVars->lpwszCur[0];

    // Save the string length
    lptkLocalVars->iStrLen = 1;

    // Ensure properly terminated
    lpwszStr[lptkLocalVars->iStrLen] = L'\0';

    // Lookup in or append to the symbol table
    lpSymEntry = SymTabAppendName_EM (lpwszStr, NULL);
    if (!lpSymEntry)
        lpSymEntry = lpMemPTD->steNoValue;

    // Mark the data as a symbol table entry
    tkFlags.TknType = TKT_VARNAMED;
////tkFlags.ImmType = IMMTYPE_ERROR;        // Already zero from {0}

    // Copy to local var so we may pass its address
    (LPSYMENTRY) aplInteger = MakePtrTypeSym (lpSymEntry);

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    bRet = AppendNewToken_EM (lptkLocalVars,
                             &tkFlags,
                             &aplInteger,
                              -lptkLocalVars->iStrLen);
    // We no longer need this ptr
    MyGlobalUnlock (lptkLocalVars->hGlbStr); lpwszStr = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
NORMAL_EXIT:
    //  Initialize the accumulation variables for the next constant
    InitAccumVars (lptkLocalVars);

    return bRet;
} // End fnDirIdent


//***************************************************************************
//  $fnAsnDone
//
//  Start and end of an assignment (monadic or dyadic)
//***************************************************************************

UBOOL fnAsnDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnAsnDone");
#endif

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the column index
        lptkLocalVars->lpMemClrNxt->colIndex = COL_PRIM_FN;

        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_PRIMITIVE];

        return TRUE;
    } // End IF

    // Copy current WCHAR
    aplLongest = *lptkLocalVars->lpwszCur;

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

UBOOL fnLstDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (l"fnLstDone");
#endif

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_PRIMITIVE];

        return TRUE;
    } // End IF

    // Copy current WCHAR
    aplLongest = *lptkLocalVars->lpwszCur;

    // Mark the data as a list separator
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

UBOOL fnClnDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnClnDone");
#endif

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        UINT uVar,              // Loop counter
             uLblIni,           // Start of label (if any)
             uLen;              // Loop length

        // Get the # chars so far (excluding the colon)
        uLen = (UINT) (lptkLocalVars->lpMemClrNxt - lptkLocalVars->lpMemClrIni);

        // Skip over leading white space
        for (uVar = 0; uVar < uLen; uVar++)
        if (lptkLocalVars->lpMemClrIni[uVar].colIndex NE COL_SPACE)
            break;

        // Putative start of label
        uLblIni = uVar;

        // If the tokens up to this point have all been alpha, ...
        for (        ; uVar < uLen; uVar++)
        if (lptkLocalVars->lpMemClrIni[uVar].colIndex NE COL_ALPHA)
            break;

        // Save the column index
        lptkLocalVars->lpMemClrNxt->colIndex = COL_COLON;

        // If we're up to the current char, ...
        if (uVar EQ uLen && uLblIni NE uVar)
        {
            // Skip over the colon color
            lptkLocalVars->lpMemClrNxt++;;

            // Get the # chars so far (including the colon)
            uLen = (UINT) (lptkLocalVars->lpMemClrNxt - lptkLocalVars->lpMemClrIni);

            for (uVar = uLblIni; uVar < uLen; uVar++)
                // Save the color (label separator)
                lptkLocalVars->lpMemClrIni[uVar].syntClr =
                  gSyntaxColors[SC_LABEL];
            // Set the new state to FSA_SOS so we can accept Control Strucures
            SetTokenStates (lptkLocalVars, FSA_SOS);
        } else
            // Save the color (plain old colon)
            lptkLocalVars->lpMemClrNxt++->syntClr =
              gSyntaxColors[SC_PRIMITIVE];

        // Mark as successful
        return TRUE;
    } // End IF

    // Copy current WCHAR
    aplLongest = *lptkLocalVars->lpwszCur;

    // If the first token is a name, and
    //   this is the second token,
    //   then it's a label separator
    if (lptkLocalVars->lpStart[1].tkFlags.TknType EQ TKT_VARNAMED
     && (lptkLocalVars->lpNext - lptkLocalVars->lpStart) EQ 2)
    {
        // Mark the data as a label separator
        tkFlags.TknType = TKT_LABELSEP;

        // Set the new state to FSA_SOS so we can accept Control Strucures
        SetTokenStates (lptkLocalVars, FSA_SOS);

        // Save as index of new initial char
        lptkLocalVars->uCharIni = lptkLocalVars->uChar + 1;

        // Skip over leading blanks
        while (IsWhiteW (lptkLocalVars->lpwszOrig[lptkLocalVars->uCharIni]))
            lptkLocalVars->uCharIni++;

        // Check for Syntax Coloring
        if (lptkLocalVars->lpMemClrNxt)
        {
            UINT uVar,              // Loop counter
                 uLen;              // Loop length

            // Get the # chars
            uLen = (UINT) (lptkLocalVars->lpNext - lptkLocalVars->lpStart);

            // Loop through the chars
            for (uVar = 0; uVar < uLen; uVar++)
                // Save the color
                lptkLocalVars->lpMemClrNxt++->syntClr =
                  gSyntaxColors[SC_LABEL];

            // Mark as successful
            return TRUE;
        } // End IF
    } else
        // Mark the data as a colon
        tkFlags.TknType = TKT_COLON;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    return AppendNewToken_EM (lptkLocalVars,
                             &tkFlags,
                             &aplLongest,
                              0);
} // End fnClnDone


//***************************************************************************
//  $fnCtrlDone
//
//  Done with this Control Structure
//***************************************************************************

UBOOL fnCtrlDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS         tkFlags = {0};
    ANON_CTRL_STRUC aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnCtrlDone");
#endif

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        UINT uCnt;              // Loop counter

        // Use the smaller of the name length and the # entries
        //   as we might be selecting a subset of the line
        lptkLocalVars->CtrlStrucStrLen =
          min (lptkLocalVars->CtrlStrucStrLen, lptkLocalVars->uSyntClrLen);

        // Loop through the chars
        for (uCnt = 0; uCnt < lptkLocalVars->CtrlStrucStrLen; uCnt++)
            // Save the color
            lptkLocalVars->lpMemClrNxt++->syntClr =
              gSyntaxColors[SC_CTRLSTRUC];
        goto NORMAL_EXIT;
    } // End IF

    // For selected CS tokens, prepend an additional special token
    if (lptkLocalVars->CtrlStrucTknType EQ TKT_CS_ELSEIF
     || lptkLocalVars->CtrlStrucTknType EQ TKT_CS_CASE
     || lptkLocalVars->CtrlStrucTknType EQ TKT_CS_CASELIST)
    {
        APLINT aplInteger2 = 0;

        // Mark as a special token
        tkFlags.TknType =
          (lptkLocalVars->CtrlStrucTknType EQ TKT_CS_ELSEIF) ? TKT_CS_SKIPEND
                                                             : TKT_CS_SKIPCASE;
        // Save the line & stmt #s for later use
        aplInteger.uLineNum = lptkLocalVars->uLineNum;
        aplInteger.uStmtNum = lptkLocalVars->uStmtNum;
        aplInteger.uTknNum  = (USHORT) (lptkLocalVars->lpNext - lptkLocalVars->lpStart);
        aplInteger.bSOS     = lptkLocalVars->bSOS;
        aplInteger.uCLIndex = 0;

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        if (!AppendNewToken_EM (lptkLocalVars,
                               &tkFlags,
                    (LPAPLINT) &aplInteger,
                                0))
            return FALSE;

        // Mark as an SOS
        tkFlags.TknType = TKT_SOS;

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        if (!AppendNewToken_EM (lptkLocalVars,
                               &tkFlags,
                               &aplInteger2,
                                0))
            return FALSE;

        // Attempt to append an EOS token
        if (!AppendEOSToken_EM (lptkLocalVars, TRUE))
            return FALSE;
    } // End IF

    // Mark as a Control Structure
    tkFlags.TknType = lptkLocalVars->CtrlStrucTknType;

    // Save the line & stmt #s for later use
    aplInteger.uLineNum = lptkLocalVars->uLineNum;
    aplInteger.uStmtNum = lptkLocalVars->uStmtNum;
    aplInteger.uTknNum  = (USHORT) (lptkLocalVars->lpNext - lptkLocalVars->lpStart);
    aplInteger.bSOS     = lptkLocalVars->bSOS;
    aplInteger.uCLIndex = 0;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    if (!AppendNewToken_EM (lptkLocalVars,
                           &tkFlags,
                (LPAPLINT) &aplInteger,
                            0))
        return FALSE;

    // For selected CS tokens, append an additional special token
    if (lptkLocalVars->CtrlStrucTknType EQ TKT_CS_IF
     || lptkLocalVars->CtrlStrucTknType EQ TKT_CS_FOR
     || lptkLocalVars->CtrlStrucTknType EQ TKT_CS_FORLCL
     || lptkLocalVars->CtrlStrucTknType EQ TKT_CS_REPEAT
     || lptkLocalVars->CtrlStrucTknType EQ TKT_CS_SELECT
     || lptkLocalVars->CtrlStrucTknType EQ TKT_CS_WHILE)
    {
        APLINT aplInteger = 0;

        // SpLit cases based upon the token type
        switch (lptkLocalVars->CtrlStrucTknType)
        {
            case TKT_CS_IF:
                tkFlags.TknType = TKT_CS_IF2;

                break;

            case TKT_CS_FOR:
            case TKT_CS_FORLCL:
                tkFlags.TknType = TKT_CS_FOR2;

                break;

            case TKT_CS_REPEAT:
                tkFlags.TknType = TKT_CS_REPEAT2;

                break;

            case TKT_CS_SELECT:
                tkFlags.TknType = TKT_CS_SELECT2;

                break;

            case TKT_CS_WHILE:
                tkFlags.TknType = TKT_CS_WHILE2;

                break;

            defstop
                break;
        } // End SWITCH

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        if (!AppendNewToken_EM (lptkLocalVars,
                               &tkFlags,
                               &aplInteger,
                                0))
            return FALSE;
    } // End IF
NORMAL_EXIT:
    // Skip over the name
    // (" - 1" to account for the ++ at the end of the FOR stmt)
    lptkLocalVars->uChar += lptkLocalVars->CtrlStrucStrLen - 1;

    return TRUE;
} // End fnCtrlDone


//***************************************************************************
//  $fnPrmDone
//
//  Start and end of a primitive function (niladic, monadic, or dyadic)
//***************************************************************************

UBOOL fnPrmDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS tkFlags = {0};
    APLINT  aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnPrmDone");
#endif

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_PRIMITIVE];

        return TRUE;
    } // End IF

    // Copy current WCHAR
    aplInteger = *lptkLocalVars->lpwszCur;

    // For reasons which escape me now, this is
    //   where we handle {zilde}
    if (aplInteger EQ UTF16_ZILDE)
    {
        // Mark the data as a variable
        tkFlags.TknType   = TKT_VARARRAY;
////////tkFlags.ImmType   = IMMTYPE_ERROR;  // Already zero from {0}
////////tkFlags.NoDisplay = FALSE;          // Already zero from {0}
        (HGLOBAL) aplInteger = MakePtrTypeGlb (hGlbZilde);
    } else
    {
        // Mark the data as a primitive function
        tkFlags.TknType = TKT_FCNIMMED;
        tkFlags.ImmType = GetImmTypeFcn (*lptkLocalVars->lpwszCur);
    } // End IF/ELSE

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    return AppendNewToken_EM (lptkLocalVars,
                             &tkFlags,
                             &aplInteger,
                              0);
} // End fnPrmDone


//***************************************************************************
//  $fnPointAcc
//
//  A Point Notation symbol accumulator
//***************************************************************************

UBOOL fnPointAcc
    (LPTKLOCALVARS lptkLocalVars)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    UBOOL        bRet;          // TRUE iff result is valid
    WCHAR        wchCur;        // The current WCHAR

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnPointAcc");
#endif

////LCLODS ("fnPointAcc\r\n");

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Check for need to resize hGlbNum
    bRet = CheckResizeNum_EM (lptkLocalVars);
    if (!bRet)
        goto ERROR_EXIT;

    // Get the current char
    wchCur = *lptkLocalVars->lpwszCur;

    // Check for Overbar
    if (wchCur EQ UTF16_OVERBAR)
        wchCur = L'-';
    else
    // Check for Infinity
    if (wchCur EQ UTF16_INFINITY)
        wchCur = L'~';

    // Use subroutine
    bRet = fnPointSub (lptkLocalVars, lpMemPTD, wchCur);
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnPointAcc


//***************************************************************************
//  $fnPointSub
//
//  A Point Notation symbol subroutine
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnPointSub"
#else
#define APPEND_NAME
#endif

UBOOL fnPointSub
    (LPTKLOCALVARS lptkLocalVars,   // Ptr to TK Local Vars
     LPPERTABDATA  lpMemPTD,        // Ptr to PerTabData global memory
     WCHAR         wchCur)          // The char to accumulate

{
    LPCHAR       lpszNum;           // Ptr to Num global memory
    UBOOL        bRet;              // TRUE iff result is valid

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnPointSub");
#endif

    // Lock the memory to get a ptr to it
    lpszNum = MyGlobalLock (lptkLocalVars->hGlbNum);

    // Check for need to resize hGlbNum
    bRet = CheckResizeNum_EM (lptkLocalVars);
    if (!bRet)
        goto ERROR_EXIT;

    // Check for overflow -- LIMIT ERROR
    bRet = (lptkLocalVars->iNumLen < DEF_NUM_MAXNELM);
    if (bRet)
        // Save the current char
        lpszNum[lptkLocalVars->iNumLen++] = (char) wchCur;
    else
        // Save the error message
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (lptkLocalVars->hGlbNum); lpszNum = NULL;

    return bRet;
} // End fnPointSub
#undef  APPEND_NAME


//***************************************************************************
//  $fnPointDone
//
//  A Point Notation symbol done
//***************************************************************************

UBOOL fnPointDone
    (LPTKLOCALVARS lptkLocalVars)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    UBOOL        bRet,          // TRUE iff result is valid
                 bMerge;        // TRUE iff we merged with the previous token
    LPCHAR       lpszNum;       // Ptr to Num global memory
    PNLOCALVARS  pnLocalVars;   // PN Local vars
    TKFLAGS      tkFlags = {0}; // Token flags for AppendNewToken_EM
    APLLONGEST   aplLongest;    // Temporary var

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnPointDone");
#endif

////LCLODS ("fnPointDone\r\n");

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Lock the memory to get a ptr to it
    lpszNum = MyGlobalLock (lptkLocalVars->hGlbNum);

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        UINT uVar,              // Loop counter
             uLen;              // Loop length

        // Get the number of chars
        uLen = lptkLocalVars->iNumLen;

        // Loop through the chars
        for (uVar = 0; uVar < uLen; uVar++)
            // Save the color
            lptkLocalVars->lpMemClrNxt++->syntClr =
              gSyntaxColors[SC_NUMCONST];
        // Mark as successful
        bRet = TRUE;

        goto NORMAL_EXIT;
    } // End IF

    // Save the starting point of the character stream
    pnLocalVars.lpszStart    = lpszNum;
    pnLocalVars.uNumLen      = lptkLocalVars->iNumLen;
    pnLocalVars.uCharIndex   = lptkLocalVars->uCharStart;
    pnLocalVars.lpszAlphaInt = (LPCHAR) lpMemPTD->lpwszTemp;
    pnLocalVars.lpszNumAccum = (LPCHAR) lpMemPTD->lpwszFormat;

    // Call the parser to convert the PN to a number
    bRet = ParsePointNotation (&pnLocalVars);
    if (bRet)
    {
        // See if the current number can be merged with the previous token
        //   to form a TKT_NUMSTRAND.
        bMerge = MergeNumbers (lptkLocalVars, &pnLocalVars, &bRet);

        // If the result is valid and we did not merge, ...
        if (bRet && !bMerge)
        {
            // Split cases based upon the result type
            switch (pnLocalVars.chType)
            {
                case PN_NUMTYPE_BOOL:
                    // Mark the data as an immediate Boolean variable
                    tkFlags.TknType = TKT_VARIMMED;
                    tkFlags.ImmType = IMMTYPE_BOOL;

                    // Get the value
                    aplLongest = pnLocalVars.aplInteger;

                    break;

                case PN_NUMTYPE_INT:
                    // Mark the data as an immediate integer variable
                    tkFlags.TknType = TKT_VARIMMED;
                    tkFlags.ImmType = IMMTYPE_INT;

                    // Get the value
                    aplLongest = pnLocalVars.aplInteger;

                    break;

                case PN_NUMTYPE_FLT:
                    // Mark the data as an immediate float variable
                    tkFlags.TknType = TKT_VARIMMED;
                    tkFlags.ImmType = IMMTYPE_FLOAT;

                    // Get the value
                    aplLongest = *(LPAPLLONGEST) &pnLocalVars.aplFloat;

                    break;

                case PN_NUMTYPE_HC2:
                case PN_NUMTYPE_HC4:
                case PN_NUMTYPE_HC8:
                case PN_NUMTYPE_RAT:
                case PN_NUMTYPE_EXT:
                    goto NONCE_EXIT;

                defstop
                    break;
            } // End SWITCH

            // Attempt to append as new token, check for TOKEN TABLE FULL,
            //   and resize as necessary.
            bRet = AppendNewToken_EM (lptkLocalVars,
                                     &tkFlags,
                                     &aplLongest,
                                      0);
        } // End IF
    } else
        // Save the error character index
        lptkLocalVars->uChar = pnLocalVars.uCharIndex;

    goto NORMAL_EXIT;

NONCE_EXIT:
    // Set new state
    lptkLocalVars->State[0] = FSA_NONCE;
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (lptkLocalVars->hGlbNum); lpszNum = NULL;

    //  Initialize the accumulation variables for the next constant
    InitAccumVars (lptkLocalVars);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnPointDone


//***************************************************************************
//  $fnOp1Done
//
//  End of a monadic primitive operator
//***************************************************************************

UBOOL fnOp1Done
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    WCHAR      wch;
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnOp1Done");
#endif

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_PRIMITIVE];

        // Mark as successful
        return TRUE;
    } // End IF

    // Copy current WCHAR
    wch = *lptkLocalVars->lpwszCur;

    if (wch EQ UTF16_SLASH
     || wch EQ UTF16_SLOPE
     || wch EQ UTF16_SLASHBAR
     || wch EQ UTF16_SLOPEBAR)
    {
        // Mark the data as an ambiguous primitive operator
        tkFlags.TknType = TKT_OP3IMMED;
        tkFlags.ImmType = IMMTYPE_PRIMOP3;
    } else
    {
        // Mark the data as a monadic primitive operator
        tkFlags.TknType = TKT_OP1IMMED;
        tkFlags.ImmType = IMMTYPE_PRIMOP1;
    } // End IF/ELSE

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    aplLongest = *lptkLocalVars->lpwszCur;
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

UBOOL fnOp2Done
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnOp2Done");
#endif

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_PRIMITIVE];

        // Mark as successful
        return TRUE;
    } // End IF

    // Mark the data as a dyadic primitive operator
    tkFlags.TknType = TKT_OP2IMMED;
    tkFlags.ImmType = IMMTYPE_PRIMOP2;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    aplLongest = *lptkLocalVars->lpwszCur;
    return AppendNewToken_EM (lptkLocalVars,
                             &tkFlags,
                             &aplLongest,
                              0);
} // End fnOp2Done


//***************************************************************************
//  $fnDotDone
//
//  End of a dot as a dyadic primitive operator
//***************************************************************************

UBOOL fnDotDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnDotDone");
#endif

    //  Initialize the accumulation variables for the next constant
    InitAccumVars (lptkLocalVars);

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_PRIMITIVE];

        // Mark as successful
        return TRUE;
    } // End IF

    // Mark the data as a dyadic primitive operator
    tkFlags.TknType = TKT_OP2IMMED;
    tkFlags.ImmType = IMMTYPE_PRIMOP2;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    aplLongest = UTF16_DOT;
    return AppendNewToken_EM (lptkLocalVars,
                             &tkFlags,
                             &aplLongest,
                              0);
} // End fnDotDone


//***************************************************************************
//  $fnJotDone
//
//  End of a jot as a dyadic primitive operator
//***************************************************************************

UBOOL fnJotDone
    (LPTKLOCALVARS lptkLocalVars)

{
    return fnJotDoneSub (lptkLocalVars, TRUE);
} // End fnJotDone


//***************************************************************************
//  $fnJotDone0
//
//  End of a jot as a dyadic primitive operator
//   but do not initialize the accumulation vars
//***************************************************************************

UBOOL fnJotDone0
    (LPTKLOCALVARS lptkLocalVars)

{
    return fnJotDoneSub (lptkLocalVars, FALSE);
} // End fnJotDone0


//***************************************************************************
//  $fnJotDoneSub
//
//  End of a jot as a dyadic primitive operator
//***************************************************************************

UBOOL fnJotDoneSub
    (LPTKLOCALVARS lptkLocalVars,
     UBOOL         bInitAcc)

{
    TKFLAGS tkFlags = {0};
    APLINT  aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnJotDone");
#endif

    if (bInitAcc)
        //  Initialize the accumulation variables for the next constant
        InitAccumVars (lptkLocalVars);

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_PRIMITIVE];

        // Mark as successful
        return TRUE;
    } // End IF

    // Mark the data as a dyadic primitive operator
    tkFlags.TknType = TKT_OP2IMMED;
    tkFlags.ImmType = IMMTYPE_PRIMOP2;

    // Copy to local var so we may pass its address
    aplInteger = UTF16_JOT;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    return AppendNewToken_EM (lptkLocalVars,
                             &tkFlags,
                             &aplInteger,
                              -1);
} // End fnJotDoneSub


//***************************************************************************
//  $fnOutDone
//
//  End of a jot dot as an outer product monadic primitive operator
//***************************************************************************

UBOOL fnOutDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS tkFlags = {0};
    APLINT  aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnOutDone");
#endif

    //  Initialize the accumulation variables for the next constant
    InitAccumVars (lptkLocalVars);

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_PRIMITIVE];

        // Mark as successful
        return TRUE;
    } // End IF

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

UBOOL fnComDone
    (LPTKLOCALVARS lptkLocalVars)

{
    int     iLen;
    TKFLAGS tkFlags = {0};
    LPWCHAR wp;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnComDone");
#endif

    // Get the length of the comment (up to but not including any '\r\n')
    iLen = lptkLocalVars->uActLen - lptkLocalVars->uChar;   // Including the leading comment symbol

    // Because the incoming string might be in the middle of the Edit Ctrl buffer
    //   and thus have embedded \r\n in it, we need to use the smaller of the
    //   lstrlenW length and the first occurrence of \r or \n.
    wp = strpbrkW (lptkLocalVars->lpwszCur, L"\r\n");
    if (wp)
        iLen = min (iLen, (APLI3264) (wp - lptkLocalVars->lpwszCur));

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        int iVar;               // Loop counter

        // Copy the length
        iVar = iLen;

        // Loop through the chars
        while (iVar--)
            // Save the color
            lptkLocalVars->lpMemClrNxt++->syntClr =
              gSyntaxColors[SC_COMMENT];
        goto NORMAL_EXIT;
    } // End IF

    wp = wcspbrk (&lptkLocalVars->lpwszCur[1], L"\n");
    if (wp)
    {
        int iLen2;

        iLen2 = (UINT) (&wp[*wp EQ UTF16_LAMP] - lptkLocalVars->lpwszCur);
        iLen  = min (iLen2, iLen);
    } // End IF
NORMAL_EXIT:
    // Skip over the comment in the input stream
    // "-1" because the FOR loop in Tokenize_EM will
    //   increment it, too
    lptkLocalVars->uChar += iLen - 1;

    return TRUE;
} // End fnComDone
#undef  APPEND_NAME


//***************************************************************************
//  $fnQuoInit
//
//  Start of a string
//***************************************************************************

UBOOL fnQuoInit
    (LPTKLOCALVARS lptkLocalVars)

{
    return fnQuoAccumSub (lptkLocalVars, SC_UNMATCHGRP);
} // End fnQuoInit


//***************************************************************************
//  $fnQuoAccum
//
//  Next char in a string
//***************************************************************************

UBOOL fnQuoAccum
    (LPTKLOCALVARS lptkLocalVars)

{
    return fnQuoAccumSub (lptkLocalVars, SC_CHRCONST);
} // End fnQuoAccum


//***************************************************************************
//  $fnQuoAccumSub
//
//  Start of or next char in a string
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnQuoAccumSub"
#else
#define APPEND_NAME
#endif

UBOOL fnQuoAccumSub
    (LPTKLOCALVARS lptkLocalVars,
     SCTYPE        scType)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    UBOOL        bRet;          // TRUE iff result is valid
    LPWCHAR      lpwszStr;      // Ptr to Str global memory

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnQuoAccumSub");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the column index (COL_QUOTE1 marks the start,
        //   COL_QUOTE2 is a subsequent char)
        lptkLocalVars->lpMemClrNxt->colIndex =
          (scType EQ SC_UNMATCHGRP) ? COL_QUOTE1 : COL_QUOTE2;

        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[scType];
    } // End IF

    // Check for need to resize hGlbStr
    bRet = CheckResizeStr_EM (lptkLocalVars);
    if (!bRet)
        goto ERROR_EXIT;

    // Lock the memory to get a ptr to it
    lpwszStr = MyGlobalLock (lptkLocalVars->hGlbStr);

    // Check for overflow -- LIMIT ERROR
    bRet = (lptkLocalVars->iStrLen < DEF_STR_MAXNELM);
    if (bRet)
        lpwszStr[lptkLocalVars->iStrLen++] = *lptkLocalVars->lpwszCur;
    else
        // Save the error message
        ErrorMessageIndirect (ERRMSG_LIMIT_ERROR APPEND_NAME);

    // We no longer need this ptr
    MyGlobalUnlock (lptkLocalVars->hGlbStr); lpwszStr = NULL;
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnQuoAccumSub
#undef  APPEND_NAME


//***************************************************************************
//  $fnQuoDone
//
//  End of a char or char vector
//***************************************************************************

UBOOL fnQuoDone
    (LPTKLOCALVARS lptkLocalVars)

{
    return fnQuoDoneSub (lptkLocalVars, TRUE);
} // End fnQuoDone


//***************************************************************************
//  $fnQuoExit
//
//  Exit of a char or char vector
//***************************************************************************

UBOOL fnQuoExit
    (LPTKLOCALVARS lptkLocalVars)

{
    return fnQuoDoneSub (lptkLocalVars, FALSE);
} // End fnQuoExit


//***************************************************************************
//  $fnQuoDoneSub
//
//  End or exit of a char or char vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- fnQuoDoneSub"
#else
#define APPEND_NAME
#endif

UBOOL fnQuoDoneSub
    (LPTKLOCALVARS lptkLocalVars,
     UBOOL         bNormal)

{
    HGLOBAL      hGlb;
    TKFLAGS      tkFlags = {0};
    APLINT       aplInteger;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    UBOOL        bRet;          // TRUE iff result is valid
    LPWCHAR      lpwszStr;      // Ptr to Str global memory

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnQuoDoneSub");
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Lock the memory to get a ptr to it
    lpwszStr = MyGlobalLock (lptkLocalVars->hGlbStr);

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        LPCLRCOL lpMemClrCol;

        // If this is a normal completion, ...
        if (bNormal)
        {
            // Copy current ptr into lpMemClrIni
            lpMemClrCol = &lptkLocalVars->lpMemClrNxt[-1];

            // Search backwards to find the matching quote mark
            //  (it's COL_QUOTE1)
            while (lpMemClrCol->colIndex EQ COL_QUOTE2)
                lpMemClrCol--;

            // If we found it, ...
            if (lpMemClrCol->colIndex EQ COL_QUOTE1)
            {
                // Mark as matched
                lpMemClrCol->colIndex = COL_QUOTE2;
                lpMemClrCol->syntClr  = gSyntaxColors[SC_CHRCONST];
            } // End IF
        } // End IF

        goto NORMAL_EXIT;
    } // End IF

    // Ensure properly terminated
    lpwszStr[lptkLocalVars->iStrLen] = L'\0';

    // The initial FSA action for a string is to store the leading
    //   delimiter, that is a single or double quote, so we can
    //   tell the difference between the two at this point.
    // At the moment, I have no use for this information, but I'd
    //   like to leave the initial action as is and just skip over
    //   the first character in the saved string.  Note that this
    //   means the string length is one too big.

    // Take care of the string length
    lptkLocalVars->iStrLen--;             // Count out the leading string delimiter

    // If this string is of length zero, then store it as an empty vector
    if (lptkLocalVars->iStrLen EQ 0)
    {
        // Mark the data as a character strand in a global memory handle
        tkFlags.TknType = TKT_CHRSTRAND;

        aplInteger = 0;     // Zero high-order dword

        // Copy to local var so we may pass its address
        (HGLOBAL) aplInteger = MakePtrTypeGlb (hGlbV0Char);

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        bRet = AppendNewToken_EM (lptkLocalVars,
                                 &tkFlags,
                                 &aplInteger,
                                  0);
    } else
    // If this string is of length one, then store it as a char scalar
    if (lptkLocalVars->iStrLen EQ 1)
    {
        // Mark the data as an immediate chracter
        tkFlags.TknType = TKT_VARIMMED;
        tkFlags.ImmType = IMMTYPE_CHAR;

        // Copy to local var so we may pass its address
        aplInteger = lpwszStr[1];

        // Attempt to append as new token, check for TOKEN TABLE FULL,
        //   and resize as necessary.
        bRet = AppendNewToken_EM (lptkLocalVars,
                                 &tkFlags,
                                 &aplInteger,
                                  -lptkLocalVars->iStrLen);
    } else
    {
        APLUINT ByteRes;        // # bytes in the string vector

        // Calculate space needed for the string vector
        ByteRes = CalcArraySize (ARRAY_CHAR, lptkLocalVars->iStrLen, 1);

        //***************************************************************
        // Allocate global memory for the array header,
        //   one dimension (it's a vector), and the string
        //   excluding the terminating zero.
        // N.B.:  Conversion from APLUINT to UINT.
        //***************************************************************
        Assert (ByteRes EQ (APLU3264) ByteRes);
        hGlb = MyGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlb)
        {
            // Save the error message
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
////////////lpHeader->PermNdx    = PERMNDX_NONE;
////////////lpHeader->SysVar     = FALSE;
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = lptkLocalVars->iStrLen;
            lpHeader->Rank       = 1;
#undef  lpHeader

            *VarArrayBaseToDim (lpwsz) = lptkLocalVars->iStrLen;
            CopyMemory (VarArrayBaseToData (lpwsz, 1),
                       &lpwszStr[1],       // Skip over the string delimiter
                        lptkLocalVars->iStrLen * sizeof (APLCHAR));
            MyGlobalUnlock (hGlb); lpwsz = NULL;

            // Mark the data as a character strand in a global memory handle
            tkFlags.TknType = TKT_CHRSTRAND;

            // Copy to local var so we may pass its address
            (HGLOBAL) aplInteger = MakePtrTypeGlb (hGlb);

            // Attempt to append as new token, check for TOKEN TABLE FULL,
            //   and resize as necessary.
            bRet = AppendNewToken_EM (lptkLocalVars,
                                     &tkFlags,
                                     &aplInteger,
                                      -lptkLocalVars->iStrLen);
        } // End IF/ELSE
    } // End IF
NORMAL_EXIT:
    //  Initialize the accumulation variables for the next constant
    InitAccumVars (lptkLocalVars);

    // We no longer need this ptr
    MyGlobalUnlock (lptkLocalVars->hGlbStr); lpwszStr = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End fnQuoDoneSub
#undef  APPEND_NAME


//***************************************************************************
//  $fnParInit
//
//  Left end (start) of a parenthetical expression
//***************************************************************************

UBOOL fnParInit
    (LPTKLOCALVARS lptkLocalVars)

{

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnParInit");
#endif

    return GroupInitCom (lptkLocalVars, TKT_LEFTPAREN);
} // End fnParInit


//***************************************************************************
//  $fnBrkInit
//
//  Left end (start) of a bracketed expression
//***************************************************************************

UBOOL fnBrkInit
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnBrkInit");
#endif

    return GroupInitCom (lptkLocalVars, TKT_LEFTBRACKET);
} // End fnBrkInit


//***************************************************************************
//  $fnBrcInit
//
//  Left end (start) of a braced expression
//***************************************************************************

UBOOL fnBrcInit
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnBrcInit");
#endif

    return GroupInitCom (lptkLocalVars, TKT_LEFTBRACE);
} // End fnBrcInit


//***************************************************************************
//  $GroupInitCom
//
//  Group (Left paren/bracket/brace) common initialization
//
//  We maintain a linked list in the token stream to allow us to detect
//    mismatched or improperly nested grouping symbols (paren, brackets,
//    and braces) -- we could easily add other left/right-grouping symbols
//    (say, left/right vanes), if needed.
//***************************************************************************

UBOOL GroupInitCom
    (LPTKLOCALVARS lptkLocalVars,
     UINT          uType)

{
    TKFLAGS     tkFlags = {0};
    UBOOL       bRet = TRUE;
    LPTOKEN     lpNext;
    APLLONGEST  aplLongest;

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the color
        lptkLocalVars->lpMemClrNxt->syntClr =
          gSyntaxColors[SC_UNMATCHGRP];

        // Save the index of the previous grouping symbol
        lptkLocalVars->lpGrpSeqNxt->PrevGroup = lptkLocalVars->PrevGroup;

        // Save the index of this color entry and skip over it
        lptkLocalVars->lpGrpSeqNxt->clrIndex  = (UINT) (lptkLocalVars->lpMemClrNxt++ - lptkLocalVars->lpMemClrIni);

        // Save the the token type of this (left) grouping symbol
        lptkLocalVars->lpGrpSeqNxt->TknType   = uType;

        // Save the index of this GrpSeq entry and skip over it
        lptkLocalVars->PrevGroup              = (UINT) (lptkLocalVars->lpGrpSeqNxt++ - lptkLocalVars->lpGrpSeqIni);

        // Mark as successful
        return TRUE;
    } // End IF

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
    lptkLocalVars->t2.lpHeader->PrevGroup = (UINT) (lpNext - lptkLocalVars->lpStart);

    return bRet;
} // End GroupInitCom


//***************************************************************************
//  $fnParDone
//
//  Right end (stop) of a parenthetical expression
//***************************************************************************

UBOOL fnParDone
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnParDone");
#endif

    return GroupDoneCom (lptkLocalVars, TKT_RIGHTPAREN, TKT_LEFTPAREN);
} // End fnParDone


//***************************************************************************
//  $fnBrkDone
//
//  Right end (stop) of a bracketed expression
//***************************************************************************

UBOOL fnBrkDone
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnBrkDone");
#endif

    return GroupDoneCom (lptkLocalVars, TKT_RIGHTBRACKET, TKT_LEFTBRACKET);
} // End fnBrkDone


//***************************************************************************
//  $fnBrcDone
//
//  Right end (stop) of a braced expression
//***************************************************************************

UBOOL fnBrcDone
    (LPTKLOCALVARS lptkLocalVars)

{
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnBrcDone");
#endif

    return GroupDoneCom (lptkLocalVars, TKT_RIGHTBRACE, TKT_LEFTBRACE);
} // End fnBrcDone


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

UBOOL GroupDoneCom
    (LPTKLOCALVARS lptkLocalVars,
     TOKEN_TYPES   uTypeCurr,
     TOKEN_TYPES   uTypePrev)

{
    UINT        uPrevGroup;
    UBOOL       bRet = TRUE;
    APLLONGEST  aplLongest;

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        UINT PrevGroup,
             uCount,
             uMatchGrp;
        static UINT aMatchGrp[] = {SC_MATCHGRP1,
                                   SC_MATCHGRP2,
                                   SC_MATCHGRP3,
                                   SC_MATCHGRP4};
        Assert (NUM_MATCHGRPS EQ countof (aMatchGrp));

        // Get the index in lpGrpSeqIni of the previous grouping symbol
        PrevGroup = lptkLocalVars->PrevGroup;

        // If there's no previous grouping symbol, ...
        if (PrevGroup EQ NO_PREVIOUS_GROUPING_SYMBOL)
            // Save the color and skip over it
            lptkLocalVars->lpMemClrNxt++->syntClr =
              gSyntaxColors[SC_UNMATCHGRP];
        else
        {
            // Check for improper nesting
            if (lptkLocalVars->lpGrpSeqIni[PrevGroup].TknType NE uTypePrev)
                // Save the color and skip over it
                lptkLocalVars->lpMemClrNxt++->syntClr =
                  gSyntaxColors[SC_UNNESTED];
            else
            {
                // Initialize the count
                uCount = 0;

                // Count the nesting level of grouping symbol
                while (lptkLocalVars->lpGrpSeqIni[PrevGroup].PrevGroup NE NO_PREVIOUS_GROUPING_SYMBOL)
                {
                    if (lptkLocalVars->lpGrpSeqIni[PrevGroup].TknType EQ uTypePrev)
                        uCount++;
                    PrevGroup = lptkLocalVars->lpGrpSeqIni[PrevGroup].PrevGroup;
                } // End WHILE

                // Get the matching level color index
                uMatchGrp = aMatchGrp[uCount % NUM_MATCHGRPS];

                // Save the color and skip over it
                lptkLocalVars->lpMemClrNxt++->syntClr =
                  gSyntaxColors[uMatchGrp];

                // Get the index in lpGrpSeqIni of the previous grouping symbol
                PrevGroup = lptkLocalVars->PrevGroup;

                // Loop through the previous grouping symbols until we find one of our kind
                while (lptkLocalVars->lpGrpSeqIni[PrevGroup].TknType NE uTypePrev)
                    PrevGroup = lptkLocalVars->lpGrpSeqIni[PrevGroup].PrevGroup;

                // Reset the preceding matching symbol's color
                lptkLocalVars->lpMemClrIni[lptkLocalVars->lpGrpSeqIni[PrevGroup].clrIndex].syntClr =
                  gSyntaxColors[uMatchGrp];

                // Save the index of the previous previous grouping symbol
                lptkLocalVars->PrevGroup = lptkLocalVars->lpGrpSeqIni[PrevGroup].PrevGroup;
            } // End IF/ELSE
        } // End IF/ELSE

        // Mark as successful
        return TRUE;
    } // End IF

    // Get the index of the previous grouping symbol
    uPrevGroup = lptkLocalVars->t2.lpHeader->PrevGroup;

    // Ensure proper nesting
    if (uPrevGroup EQ NO_PREVIOUS_GROUPING_SYMBOL
     || lptkLocalVars->lpStart[uPrevGroup].tkFlags.TknType NE (UINT) uTypePrev)
    {
        // Save the error caret position
        SaveErrorPosition (lptkLocalVars->uChar);

        bRet = FALSE;

        goto SYNTAX_EXIT;
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

    goto NORMAL_EXIT;

SYNTAX_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);
NORMAL_EXIT:
    return bRet;
} // End GroupDoneCom
#undef  APPEND_NAME


//***************************************************************************
//  $MergeNumbers
//
//  See if the current number can be merged with the previous token
//    to form a TKT_NUMSTRAND.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MergeNumbers"
#else
#define APPEND_NAME
#endif

UBOOL MergeNumbers
    (LPTKLOCALVARS lptkLocalVars,   // Ptr to local vars
     LPPNLOCALVARS lppnLocalVars,   // Ptr to PNLOCALVARS global memory
     LPUBOOL       lpbRet)          // Ptr to TRUE iff the result is valid

{
    LPTOKEN    lptkPrv;             // Ptr to previous token
    APLSTYPE   aplTypePrv,          // Previous token storage type
               aplTypeNew,          // New            ...
               aplTypeRes;          // Result         ...
    APLNELM    aplNELMPrv;          // Previous token NELM
    APLRANK    aplRankPrv;          // Previous token rank
    APLUINT    ByteRes,             // # bytes in the result
               uPrv;                // Loop counter
    HGLOBAL    hGlbRes,             // Result global memory handle
               hGlbPrv;             // Previous token ...
    LPVOID     lpMemRes,            // Ptr to result global memory
               lpMemPrv;            // Ptr to previous token ...
    APLLONGEST aplLongestPrv;       // Previous token immediate value
    UINT       uBitMask;            // Bit mask for looping through Booleans
    UBOOL      bMerge = FALSE;      // TRUE iff we merged with the previous token

    // Get a ptr to the previous token (if any)
    lptkPrv = &lptkLocalVars->lpNext[-1];

    // See if we can merge this with the previous token
    if (lptkLocalVars->t2.lpHeader->TokenCnt
     && (lptkPrv->tkFlags.TknType EQ TKT_VARIMMED
      || lptkPrv->tkFlags.TknType EQ TKT_NUMSTRAND))
    {
        // Get the previous token's attrs
        AttrsOfToken (lptkPrv, &aplTypePrv, &aplNELMPrv, &aplRankPrv, NULL);

        // If the previous token is a simple numeric and
        //   it's either an immediate
        //   or non-empty (i.e. not zilde)
        if (IsSimpleNum (aplTypePrv)
         && (lptkPrv->tkFlags.TknType EQ TKT_VARIMMED
          || !IsEmpty (aplNELMPrv)))
        {
            // Catentate the current number with the previous token

            // Lock the previous token and get a ptr to it
            aplLongestPrv = GetGlbPtrs_LOCK (lptkPrv, &hGlbPrv, &lpMemPrv);

            // Get the new storage type
            // Split cases based upon the storage type
            switch (lppnLocalVars->chType)
            {
                case PN_NUMTYPE_BOOL:
                    aplTypeNew = ARRAY_BOOL;

                    break;

                case PN_NUMTYPE_INT:
                    aplTypeNew = ARRAY_INT;

                    break;

                case PN_NUMTYPE_FLT:
                    aplTypeNew = ARRAY_FLOAT;

                    break;

                case PN_NUMTYPE_HC2:
                case PN_NUMTYPE_HC4:
                case PN_NUMTYPE_HC8:
                case PN_NUMTYPE_RAT:
                case PN_NUMTYPE_EXT:
                defstop
                    break;
            } // End SWITCH

            // Calculate the result storage type
            aplTypeRes = aTypePromote[aplTypePrv][aplTypeNew];

            // Calculate space needed for the numeric vector
            ByteRes = CalcArraySize (aplTypeRes, aplNELMPrv + 1, 1);

            // Allocate global memory for the array
            Assert (ByteRes EQ (APLU3264) ByteRes);
            hGlbRes = MyGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (!hGlbRes)
            {
                // Save the error message
                ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

                // Mark as not successful
                lpbRet = FALSE;
            } else
            {
                // Lock the memory to get a ptr to it
                lpMemRes = MyGlobalLock (hGlbRes);

                // Fill in the header
#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = aplTypeRes;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;
////////////////lpHeader->SysVar     = FALSE;
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = aplNELMPrv + 1;
                lpHeader->Rank       = 1;
#undef  lpHeader
                // Save the dimension
                *VarArrayBaseToDim (lpMemRes) = aplNELMPrv + 1;

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayBaseToData (lpMemRes, 1);

                // Initialize the bit mask
                uBitMask = BIT0;

                // If the previous token is global, ...
                if (hGlbPrv)
                    // Skip over the header and dimension
                    lpMemPrv = VarArrayBaseToData (lpMemPrv, aplRankPrv);
                else
                    lpMemPrv = &aplLongestPrv;

                // Convert and copy the previous token's data to the new token
                // Split cases based upon the result storage type
                switch (aplTypeRes)
                {
                    case ARRAY_BOOL:
                        // Same type as the result, so just copy the Booleans
                        CopyMemory (lpMemRes, lpMemPrv, (APLU3264) RoundUpBitsToBytes (aplNELMPrv) * sizeof (APLBOOL));

                        // Save the new value as a Boolean
                        ((LPAPLBOOL) lpMemRes)[aplNELMPrv >> LOG2NBIB] |= (lppnLocalVars->aplInteger << (MASKLOG2NBIB & (UINT) aplNELMPrv));

                        break;

                    case ARRAY_INT:
                        // Split cases based upon the previous token's storage type
                        switch (aplTypePrv)
                        {
                            case ARRAY_BOOL:
                                // Loop through the previous token's values
                                for (uPrv = 0; uPrv < aplNELMPrv; uPrv++)
                                {
                                    if (uBitMask & *(LPAPLBOOL) lpMemPrv)
                                        ((LPAPLINT) lpMemRes)[uPrv] = 1;

                                    // Shift over the bit mask
                                    uBitMask <<= 1;

                                    // Check for end-of-byte
                                    if (uBitMask EQ END_OF_BYTE)
                                    {
                                        uBitMask = BIT0;            // Start over
                                        ((LPAPLBOOL) lpMemPrv)++;   // Skip to next byte
                                    } // End IF
                                } // End FOR

                                break;

                            case ARRAY_INT:
                                // Same type as the result, so just copy the integers
                                CopyMemory (lpMemRes, lpMemPrv, (APLU3264) aplNELMPrv * sizeof (APLINT));

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // Save the new value as an integer
                        ((LPAPLINT) lpMemRes)[aplNELMPrv] = lppnLocalVars->aplInteger;

                        break;

                    case ARRAY_FLOAT:
                        // Split cases based upon the previous token's storage type
                        switch (aplTypePrv)
                        {
                            case ARRAY_BOOL:
                                // Loop through the previous token's values
                                for (uPrv = 0; uPrv < aplNELMPrv; uPrv++)
                                {
                                    if (uBitMask & *(LPAPLBOOL) lpMemPrv)
                                        ((LPAPLFLOAT) lpMemRes)[uPrv] = 1;

                                    // Shift over the bit mask
                                    uBitMask <<= 1;

                                    // Check for end-of-byte
                                    if (uBitMask EQ END_OF_BYTE)
                                    {
                                        uBitMask = BIT0;            // Start over
                                        ((LPAPLBOOL) lpMemPrv)++;   // Skip to next byte
                                    } // End IF
                                } // End FOR

                                break;

                            case ARRAY_INT:
                                // Loop through the previous token's values
                                for (uPrv = 0; uPrv < aplNELMPrv; uPrv++)
                                    ((LPAPLFLOAT) lpMemRes)[uPrv] = (APLFLOAT) *((LPAPLINT) lpMemPrv)++;
                                break;

                            case ARRAY_FLOAT:
                                // Same type as the result, so just copy the floats
                                CopyMemory (lpMemRes, lpMemPrv, (APLU3264) aplNELMPrv * sizeof (APLFLOAT));

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        // Save the new value as a float
                        ((LPAPLFLOAT) lpMemRes)[aplNELMPrv] = lppnLocalVars->aplFloat;

                        break;

                    defstop
                        break;
                } // End SWITCH

                if (hGlbPrv)
                {
                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPrv); lpMemPrv = NULL;

                    // Free the previous token's storage
                    MyGlobalFree (hGlbPrv); hGlbPrv = NULL;
                } // End IF

                // Setup the previous token
                lptkPrv->tkFlags.TknType  = TKT_NUMSTRAND;
                lptkPrv->tkFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeRes);
                lptkPrv->tkData.tkGlbData = MakePtrTypeGlb (hGlbRes);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

                // Mark as succcessfully merged
                bMerge = TRUE;
            } // End IF/ELSE

            if (hGlbPrv)
            {
                // We no longer need this ptr
                MyGlobalUnlock (hGlbPrv); lpMemPrv = NULL;
            } // End IF
        } // End IF
    } // End IF

    return bMerge;
} // End MergeNumbers
#undef  APPEND_NAME


//***************************************************************************
//  $SaveErrorPosition
//
//  Save the error caret position
//***************************************************************************

void SaveErrorPosition
    (UINT uChar)            // Error caret position

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Save the caret position
    lpMemPTD->uCaret = uChar;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End SaveErrorPosition


//***************************************************************************
//  $fnDiaDone
//
//  Done with this stmt
//***************************************************************************

UBOOL fnDiaDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS tkFlags = {0};
    APLINT  aplInteger;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnDiaDone");
#endif

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_PRIMITIVE];

        goto NORMAL_EXIT;
    } // End IF

    // Test for mismatched or improperly nested grouping symbols
    if (!CheckGroupSymbols_EM (lptkLocalVars))
        return FALSE;

    // Mark as an SOS
    tkFlags.TknType = TKT_SOS;

    // Copy to a local var so we may pass its address
    aplInteger = 0;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    if (!AppendNewToken_EM (lptkLocalVars,
                           &tkFlags,
                           &aplInteger,
                            0))
        return FALSE;
NORMAL_EXIT:
    // Count in another stmt
    lptkLocalVars->uStmtNum++;

    // Start the initial char over again
    lptkLocalVars->uCharIni = lptkLocalVars->uChar + 1;

    // Skip over leading blanks
    while (IsWhiteW (lptkLocalVars->lpwszOrig[lptkLocalVars->uCharIni]))
        lptkLocalVars->uCharIni++;

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
        return TRUE;
    else
        // Append the EOS token
        return AppendEOSToken_EM (lptkLocalVars, TRUE);
} // End fnDiaDone


//***************************************************************************
//  $fnSyntQuote
//
//  Accumulate a CHRCONST color if Syntax Coloring is active.
//  Used to handle embedded double quotes.
//***************************************************************************

UBOOL fnSyntQuote
    (LPTKLOCALVARS lptkLocalVars)

{
    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        // Save the column index
        lptkLocalVars->lpMemClrNxt->colIndex = COL_QUOTE2;

        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_CHRCONST];
    } // End IF

    // Mark as successful
    return TRUE;
} // End fnSyntQuote


//***************************************************************************
//  $fnSyntWhite
//
//  Accumulate a white space color if Syntax Coloring is active.
//***************************************************************************

UBOOL fnSyntWhite
    (LPTKLOCALVARS lptkLocalVars)

{
    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColorBG;

    // Mark as successful
    return TRUE;
} // End fnSyntWhite


//***************************************************************************
//  $fnSyntPrm
//
//  Accumulate a primitive symbol color if Syntax Coloring is active.
//***************************************************************************

UBOOL fnSyntPrm
    (LPTKLOCALVARS lptkLocalVars)

{
    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
        // Save the color
        lptkLocalVars->lpMemClrNxt++->syntClr =
          gSyntaxColors[SC_PRIMITIVE];

    // Mark as successful
    return TRUE;
} // End fnSyntPrm


//***************************************************************************
//  $fnUnkDone
//
//  Accumulate an unknown color
//***************************************************************************

UBOOL fnUnkDone
    (LPTKLOCALVARS lptkLocalVars)

{
    TKFLAGS    tkFlags = {0};
    APLLONGEST aplLongest;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"fnUnkDone");
#endif

    // Check for Syntax Coloring
    if (lptkLocalVars->lpMemClrNxt)
    {
        SCTYPE scType;

        // If the col is not EOL, ...
        if (lptkLocalVars->colIndex NE COL_EOL)
        {
            // Split cases based upon the unknown char
            switch (*lptkLocalVars->lpwszCur)
            {
                case UTF16_LDC_LT_HORZ:         // Line drawing chars
                case UTF16_LDC_LT_VERT:         // ...
                case UTF16_LDC_LT_UL:           // ...
                case UTF16_LDC_LT_UR:           // ...
                case UTF16_LDC_LT_LL:           // ...
                case UTF16_LDC_LT_LR:           // ...
                case UTF16_LDC_LT_VERT_R:       // ...
                case UTF16_LDC_LT_VERT_L:       // ...
                case UTF16_LDC_LT_HORZ_D:       // ...
                case UTF16_LDC_LT_HORZ_U:       // ...
                    scType = SC_LINEDRAWING;

                    break;

                default:
                    scType = SC_UNK;

                    break;
            } // End SWITCH

            // Save the color
            lptkLocalVars->lpMemClrNxt++->syntClr =
              gSyntaxColors[scType];
        } // End IF

        // Mark as successful
        return TRUE;
    } // End IF

    // Mark the data as a SYNTAX ERROR
    tkFlags.TknType = TKT_SYNTERR;
    tkFlags.ImmType = IMMTYPE_ERROR;

    // Attempt to append as new token, check for TOKEN TABLE FULL,
    //   and resize as necessary.
    aplLongest = *lptkLocalVars->lpwszCur;
    return AppendNewToken_EM (lptkLocalVars,
                             &tkFlags,
                             &aplLongest,
                              0);
} // End fnUnkDone


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
    (LPAPLCHAR   lpwszLine,         // The line to tokenize (not necessarily zero-terminated)
     APLNELM     aplNELM,           // NELM of lpwszLine
     HWND        hWndEC,            // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
     UINT        uLineNum,          // Function line # (0 = header)
     LPERRHANDFN lpErrHandFn,       // Ptr to error handling function (may be NULL)
     UBOOL       bMF)               // TRUE iff we're tokenizing a Magic Function

{
    UINT         uChar;             // Loop counter
    WCHAR        wchOrig,           // The original char
                 wchColNum;         // The translated char for tokenization as a COL_*** value
    FNACTION     fnAction1_EM,      // Ptr to 1st action
                 fnAction2_EM;      // ...    2nd ...
    TKLOCALVARS  tkLocalVars = {0}; // Local vars
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPTOKEN      lptkCSNxt;         // Ptr to next token on the CS stack

    // Avoid re-entrant code
    EnterCriticalSection (&CSOTokenize);

////LCLODS ("Entering <Tokenize_EM>\r\n");

    // Check for re-entrant
    if (bInUse)
        DbgBrk ();
    else
        bInUse = TRUE;

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Save local vars in struct which we pass to each FSA action routine
    tkLocalVars.State[2] =
    tkLocalVars.State[1] =
    tkLocalVars.State[0] = FSA_SOS;
    tkLocalVars.uLineNum = uLineNum;
    tkLocalVars.uStmtNum = 0;
    tkLocalVars.bMF      = bMF;

    // If this is the function header (uLineNum EQ 0)
    //   save and restore the ptr to the next token
    //   on the CS stack as there are no CSs in the
    //   function header
    if (uLineNum EQ 0)
    {
        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Save the ptr to the next token on the CS stack
        lptkCSNxt = lpMemPTD->lptkCSNxt;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF

    // Allocate some memory for the tokens
    // If we need more, we'll GlobalRealloc
    // Note, we can't use DbgGlobalAlloc here as we
    //   might free this line by being called from the Master Frame
    //   via a system command, in which case there is
    //   no PTD for that thread.
    tkLocalVars.hGlbToken = MyGlobalAlloc (GHND, DEF_TOKEN_SIZE * sizeof (TOKEN));
    if (!tkLocalVars.hGlbToken)
    {
        // Mark as no caret
        uChar = NEG1U;

        goto WSFULL_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    UTLockAndSet (tkLocalVars.hGlbToken, &tkLocalVars.t2);

    // Set variables in the UNION_TOKEN
    tkLocalVars.t2.lpHeader->Sig.nature = TOKEN_HEADER_SIGNATURE;
    tkLocalVars.t2.lpHeader->Version    = 1;            // Initialize version # of this header
    tkLocalVars.t2.lpHeader->TokenCnt   = 0;            // ...        count of tokens
    tkLocalVars.t2.lpHeader->PrevGroup  = NO_PREVIOUS_GROUPING_SYMBOL;  // Initialize index of previous grouping symbol
    tkLocalVars.lpStart                 = TokenBaseToStart (tkLocalVars.t2.lpBase); // Skip over TOKEN_HEADER
    tkLocalVars.lpNext                  = &tkLocalVars.lpStart[tkLocalVars.t2.lpHeader->TokenCnt];
    tkLocalVars.lpwszOrig               = lpwszLine;    // Save ptr to start of input line

    // Initialize EOS
    tkLocalVars.lpLastEOS               = tkLocalVars.lpStart;
    tkLocalVars.lpwszCur                = &lpwszLine[0];// Just so it has a known value

    // Set initial limit for hGlbNum
    tkLocalVars.iNumLim = DEF_NUM_INITNELM;

    // Allocate storage for hGlbNum
    tkLocalVars.hGlbNum =
      MyGlobalAlloc (GHND, tkLocalVars.iNumLim * sizeof (char));
    if (!tkLocalVars.hGlbNum)
        goto ERROR_EXIT;

    // Set initial limit for hGlbStr
    tkLocalVars.iStrLim = DEF_STR_INITNELM;

    // Allocate storage for hGlbStr
    tkLocalVars.hGlbStr =
      MyGlobalAlloc (GHND, tkLocalVars.iStrLim * sizeof (APLCHAR));
    if (!tkLocalVars.hGlbStr)
        goto ERROR_EXIT;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    // Display the tokens so far
    DbgMsgW (L"*** Tokenize_EM Start");
    DisplayTokens (tkLocalVars.hGlbToken);
#endif

    // Initialize the accumulation variables for the next constant
    InitAccumVars (&tkLocalVars);

    // Skip over leading blanks (more to reduce clutter
    //   in the debugging window)
    for (uChar = 0; uChar < aplNELM; uChar++)
    if (!IsWhiteW (lpwszLine[uChar]))
        break;

    // Save pointer to current wch
    // We need this for AppendEOSToken_EM
    tkLocalVars.lpwszCur = &lpwszLine[uChar];

    // Attempt to append an EOS token
    if (!AppendEOSToken_EM (&tkLocalVars, TRUE))
        goto ERROR_EXIT;

    // Save initial char index to compare against
    //   in case we get a leading colon
    tkLocalVars.uCharStart =
    tkLocalVars.uCharIni   = uChar;
    tkLocalVars.uActLen    = (UINT) aplNELM;

    for (     ; uChar <= aplNELM; uChar++)
    {
        // Use a FSA to tokenize the line

        // Save current index (may be modified by an action)
        tkLocalVars.uChar = uChar;

        // Save pointer to current wch
        tkLocalVars.lpwszCur = &lpwszLine[uChar];

        /* The FSA works as follows:

           1.  Get the next WCHAR from the input;
           2.  Translate it into a COL_*** index;
           3.  Use the current state as a row index and the
               above index as a column index into fsaColTable,
               take the appropriate action.
           4.  Repeat until EOL or an error occurs.
         */

        if (uChar EQ aplNELM)
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

        // Strip out EOL check so we don't confuse a zero-value char with EOL
        if (uChar EQ aplNELM)
            wchColNum = COL_EOL;
        else
            wchColNum = CharTrans (wchOrig, &tkLocalVars);

        // Save the COL_xxx value
        tkLocalVars.colIndex = wchColNum;

#if (defined (DEBUG)) && (defined (EXEC_TRACE))
        wsprintfW (wszTemp,
                   L"wchO = %c (%d), wchT = %s (%d), CS = %d, NS = %d, Act1 = %p, Act2 = %p",
                   wchOrig ? wchOrig : UTF16_HORIZELLIPSIS,
                   wchOrig,
                   GetColName (wchColNum),
                   wchColNum,
                   tkLocalVars.State[0],
                   fsaColTable[tkLocalVars.State[0]][wchColNum].iNewState,
                   fsaColTable[tkLocalVars.State[0]][wchColNum].fnAction1,
                   fsaColTable[tkLocalVars.State[0]][wchColNum].fnAction2);
        DbgMsgW (wszTemp);
#endif

        // Get primary action and new state
        fnAction1_EM = fsaColTable[tkLocalVars.State[0]][wchColNum].fnAction1;
        fnAction2_EM = fsaColTable[tkLocalVars.State[0]][wchColNum].fnAction2;
        SetTokenStates (&tkLocalVars, fsaColTable[tkLocalVars.State[0]][wchColNum].iNewState);

        // Check for primary action
        if (fnAction1_EM
         && !(*fnAction1_EM) (&tkLocalVars))
            goto ERROR_EXIT;

        // Check for secondary action
        if (fnAction2_EM
         && !(*fnAction2_EM) (&tkLocalVars))
            goto ERROR_EXIT;

        // Split cases based upon the return code
        switch (tkLocalVars.State[0])
        {
            case FSA_NONCE:
                // Save the error caret position
                SaveErrorPosition (tkLocalVars.uChar);

                goto NONCE_EXIT;

            case FSA_EXIT:
            {
                UINT    uNext;              // Offset from Start to Next in units of sizeof (TOKEN)
                APLINT  aplInteger;         // Temporary integer for SOS token
                TKFLAGS tkFlags = {0};      // Token flags for SOS token

                // Test for mismatched or improperly nested grouping symbols
                if (!CheckGroupSymbols_EM (&tkLocalVars))
                    goto ERROR_EXIT;

                // Mark as an SOS
                tkFlags.TknType = TKT_SOS;

                // Copy to a local var so we may pass its address
                aplInteger = 0;

                // Attempt to append as new token, check for TOKEN TABLE FULL,
                //   and resize as necessary.
                if (!AppendNewToken_EM (&tkLocalVars,
                                        &tkFlags,
                                        &aplInteger,
                                         0))
                    goto ERROR_EXIT;

                // Calculate the # tokens in this last stmt
                AppendEOSToken_EM (&tkLocalVars, FALSE);

                uNext = (UINT) (tkLocalVars.lpNext - tkLocalVars.lpStart);

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
    } // End FOR

    // We should never get here as we process the
    //   trailing zero in the input line which should
    //   exit from one of the actions with FSA_EXIT.
    DbgStop ();

WSFULL_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

NONCE_EXIT:
    // Save the error message
    ErrorMessageIndirect (ERRMSG_NONCE_ERROR APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Signal an error
    if (lpErrHandFn)
        (*lpErrHandFn) (lpMemPTD->lpwszErrorMessage, lpwszLine, tkLocalVars.uChar, hWndEC);

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

        // Free the handle
        MyGlobalFree (tkLocalVars.hGlbToken); tkLocalVars.hGlbToken = NULL;
    } // End IF

    goto FREED_EXIT;

UNLOCKED_EXIT:
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    // Display the tokens so far
    DisplayTokens (tkLocalVars.hGlbToken);
#endif
FREED_EXIT:
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    DbgMsgW (L"*** Tokenize_EM End");
#endif

    // Free the global memory:  hGlbNum
    if (tkLocalVars.hGlbNum)
    {
        MyGlobalFree (tkLocalVars.hGlbNum); tkLocalVars.hGlbNum = NULL;
    } // End IF

    // Free the global memory:  hGlbStr
    if (tkLocalVars.hGlbStr)
    {
        MyGlobalFree (tkLocalVars.hGlbStr); tkLocalVars.hGlbStr = NULL;
    } // End IF

    // If this is the function header, ...
    if (uLineNum EQ 0)
    {
        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Restore the ptr to the next token on the CS stack
        lpMemPTD->lptkCSNxt = lptkCSNxt;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF

#ifdef DEBUG
    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Ensure numeric length has been reset
    if (tkLocalVars.iNumLen NE 0)
        DbgBrk ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
#endif
    // Mark as no longer in use
    bInUse = FALSE;
////LCLODS ("Exiting  <Tokenize_EM>\r\n");

    // Release the Critical Section
    LeaveCriticalSection (&CSOTokenize);

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

UBOOL CheckGroupSymbols_EM
    (LPTKLOCALVARS lptkLocalVars)

{
    if ((!OptionFlags.bCheckGroup)
     || lptkLocalVars->t2.lpHeader->PrevGroup EQ NO_PREVIOUS_GROUPING_SYMBOL)
        return TRUE;

    // Save the error caret position
    SaveErrorPosition (lptkLocalVars->uChar);

    // Save the error message
    ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

    return FALSE;
} // End CheckGroupSymbols_EM
#undef  APPEND_NAME


//***************************************************************************
//  $Untokenize
//
//  Free allocated memory and other such resources in a tokenized line
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- Untokenize"
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
            case TKT_OP3NAMED:          // ...
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

            case TKT_CHRSTRAND:         // Character strand  (data is HGLOBAL)
            case TKT_NUMSTRAND:         // Numeric   ...
            case TKT_VARARRAY:          // Array of data (data is HGLOBAL)
                // Free the array and all elements of it
                if (FreeResultGlobalVar (lpToken->tkData.tkGlbData))
                {
#ifdef DEBUG_ZAP
                    dprintfW (L"**Zapping in Untokenize: %p (%d) (%S#%d)",
                              ClrPtrTypeDir (lpToken->tkData.tkGlbData),
                              FNLN);
#endif
                    lpToken->tkData.tkGlbData = NULL;
                } // End IF

                break;

            case TKT_ASSIGN:            // Assignment symbol (data is UTF16_LEFTARROW)
            case TKT_LISTSEP:           // List separator    (...     ';')
            case TKT_LABELSEP:          // Label ...         (...     ':')
            case TKT_COLON:             // Colon             (...     ':')
            case TKT_VARIMMED:          // Immediate data (data is immediate)
            case TKT_FCNIMMED:          // Immediate primitive function (any valence) (data is UTF16_***)
            case TKT_OP1IMMED:          // ...       Monadic primitive operator (data is UTF16_***)
            case TKT_OP2IMMED:          // ...       Dyadic  ...
            case TKT_OP3IMMED:          // ...       Ambiguous  ...
            case TKT_OPJOTDOT:          // Outer product (data is NULL)
            case TKT_LEFTPAREN:         // Left paren (data is TKT_***)
            case TKT_RIGHTPAREN:        // Right ...   ...
            case TKT_LEFTBRACKET:       // Left bracket ...
            case TKT_RIGHTBRACKET:      // Right ...   ...
            case TKT_LEFTBRACE:         // Left brace  ...
            case TKT_RIGHTBRACE:        // Right ...   ...
            case TKT_EOS:               // End-of-Stmt (data is length of stmt including this token)
            case TKT_EOL:               // End-of-Line (data is NULL)
            case TKT_SOS:               // Start-of-Stmt (data is NULL)
            case TKT_LINECONT:          // Line continuation (data is NULL)
            case TKT_INPOUT:            // Input/Output (data is UTF16_QUAD or UTF16_QUOTEQUAD)
            case TKT_CS_ANDIF:          // Control structure:  ANDIF     (Data is Line/Stmt #)
            case TKT_CS_CASE:           // ...                 CASE
            case TKT_CS_CASELIST:       // ...                 CASELIST
            case TKT_CS_CONTINUE:       // ...                 CONTINUE
            case TKT_CS_ELSE:           // ...                 ELSE
            case TKT_CS_ELSEIF:         // ...                 ELSEIF
            case TKT_CS_END:            // ...                 END
            case TKT_CS_ENDFOR:         // ...                 ENDFOR
            case TKT_CS_ENDFORLCL:      // ...                 ENDFORLCL
            case TKT_CS_ENDIF:          // ...                 ENDIF
            case TKT_CS_ENDREPEAT:      // ...                 ENDREPEAT
            case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
            case TKT_CS_ENDWHILE:       // ...                 ENDWHILE
            case TKT_CS_FOR:            // ...                 FOR
            case TKT_CS_FOR2:           // ...                 FOR2
            case TKT_CS_FORLCL:         // ...                 FORLCL
            case TKT_CS_GOTO:           // ...                 GOTO
            case TKT_CS_IF:             // ...                 IF
            case TKT_CS_IF2:            // ...                 IF2
            case TKT_CS_IN:             // ...                 IN
            case TKT_CS_LEAVE:          // ...                 LEAVE
            case TKT_CS_ORIF:           // ...                 ORIF
            case TKT_CS_REPEAT:         // ...                 REPEAT
            case TKT_CS_REPEAT2:        // ...                 REPEAT2
            case TKT_CS_RETURN:         // ...                 RETURN
            case TKT_CS_SELECT:         // ...                 SELECT
            case TKT_CS_SELECT2:        // ...                 SELECT2
            case TKT_CS_UNTIL:          // ...                 UNTIL
            case TKT_CS_WHILE:          // ...                 WHILE
            case TKT_CS_WHILE2:         // ...                 WHILE2
            case TKT_CS_SKIPCASE:       // ...                 Special token
            case TKT_CS_SKIPEND:        // ...                 Special token
            case TKT_SYNTERR:           // Syntax Error
                break;                  // Nothing to do

            case TKT_CS_NEC:            // ...                 Special token
            case TKT_CS_EOL:            // ...                 Special token
            case TKT_LSTIMMED:          // List in brackets, single element, immediate
            case TKT_LSTARRAY:          // List in brackets, single element, array
            case TKT_LSTMULT:           // List in brackets, multiple elements
            defstop
#ifdef DEBUG
            {
                LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
                WCHAR        wszTemp[1024]; // Ptr to temporary output area

                // Lock the memory to get a ptr to it
                lpMemPTD = MyGlobalLock (hGlbPTD);

                wsprintfW (wszTemp,
                           L"Untokenize:  *** Unknown Token Value:  %d",
                           lpToken->tkFlags.TknType);
                DbgMsgW (wszTemp);

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
        WCHAR        wszTemp[1024]; // Ptr to temporary output area

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        wsprintfW (wszTemp,
                   L"Untokenize:  hGlobToken (%p) is invalid.",
                   hGlbToken);
        DbgMsgW (wszTemp);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
#endif
    } // End IF/ELSE
} // End Untokenize
#undef  APPEND_NAME


//***************************************************************************
//  $AppendEOSToken_EM
//
//  Append an EOS Token
//***************************************************************************

UBOOL AppendEOSToken_EM
    (LPTKLOCALVARS lptkLocalVars,
     UBOOL         bAppend)

{
    APLINT aplInteger;

    // Calculate the # tokens in this stmt
    lptkLocalVars->lpLastEOS->tkData.tkIndex = (UINT) (lptkLocalVars->lpNext - lptkLocalVars->lpLastEOS);

    if (lptkLocalVars->lpStart EQ lptkLocalVars->lpLastEOS
     && lptkLocalVars->lpNext >   lptkLocalVars->lpStart
     && lptkLocalVars->lpStart[2].tkFlags.TknType EQ TKT_LABELSEP)
        // Calculate the # tokens in the stmt after the label
        lptkLocalVars->lpStart[2].tkData.tkIndex = lptkLocalVars->lpStart->tkData.tkIndex - 2;

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
} // End AppendEOSToken_EM


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

UBOOL AppendNewToken_EM
    (LPTKLOCALVARS lptkLocalVars,       // Ptr to local vars
     LPTKFLAGS     lptkFlags,           // Ptr to token flags
     LPAPLLONGEST  lptkData,            // Ptr to token data (may be NULL)
     int           iCharOffset)         // Offset from lpwszCur of the token (where the caret goes)

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
            // Save the error message
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
    lptkLocalVars->lpNext->tkCharIndex = iCharOffset + (UINT) (lptkLocalVars->lpwszCur - lptkLocalVars->lpwszOrig);

    // Count in another token
    lptkLocalVars->t2.lpHeader->TokenCnt++;

    // Skip to next token
    lptkLocalVars->lpNext++;

    // Append a CS token

    // Split cases based upon the token type
    switch (lptkFlags->TknType)
    {
        case TKT_VARNAMED    :
        case TKT_CHRSTRAND   :
        case TKT_NUMSTRAND   :
        case TKT_VARIMMED    :
        case TKT_ASSIGN      :
        case TKT_LISTSEP     :
        case TKT_COLON       :
        case TKT_FCNIMMED    :
        case TKT_OP1IMMED    :
        case TKT_OP2IMMED    :
        case TKT_OP3IMMED    :
        case TKT_OPJOTDOT    :
        case TKT_LEFTPAREN   :
        case TKT_RIGHTPAREN  :
        case TKT_LEFTBRACKET :
        case TKT_RIGHTBRACKET:
        case TKT_LEFTBRACE   :
        case TKT_RIGHTBRACE  :
        case TKT_LINECONT    :
        case TKT_INPOUT      :
        case TKT_VARARRAY    :
            // Append the NEC token to the CS stack
            //   to allow for later parsing for SYNTAX ERRORs
            AppendNewCSToken_EM (TKT_CS_NEC,
                                 lptkLocalVars->uLineNum,
                                 lptkLocalVars->uStmtNum,
                       (USHORT) (lptkLocalVars->lpNext - lptkLocalVars->lpStart),
                                 FALSE,
                                 lptkLocalVars->uChar);
            break;

        case TKT_SYNTERR     :
            break;

        case TKT_LABELSEP    :
        case TKT_EOS         :
        case TKT_EOL         :
        case TKT_SOS         :
            // Append the EOS token to the CS stack
            //   to allow for later parsing for SYNTAX ERRORs
            AppendNewCSToken_EM (lptkFlags->TknType,
                                 lptkLocalVars->uLineNum,
                                 lptkLocalVars->uStmtNum,
                       (USHORT) (lptkLocalVars->lpNext - lptkLocalVars->lpStart),
                                 TRUE,
                                 lptkLocalVars->uChar);
            break;

        case TKT_CS_ANDIF    :
        case TKT_CS_CASE     :
        case TKT_CS_CASELIST :
        case TKT_CS_CONTINUE :
        case TKT_CS_ELSE     :
        case TKT_CS_ELSEIF   :
        case TKT_CS_END      :
        case TKT_CS_ENDFOR   :
        case TKT_CS_ENDFORLCL:
        case TKT_CS_ENDIF    :
        case TKT_CS_ENDREPEAT:
        case TKT_CS_ENDSELECT:
        case TKT_CS_ENDWHILE :
        case TKT_CS_FOR      :
        case TKT_CS_FOR2     :
        case TKT_CS_FORLCL   :
        case TKT_CS_IF       :
        case TKT_CS_IF2      :
        case TKT_CS_IN       :
        case TKT_CS_LEAVE    :
        case TKT_CS_ORIF     :
        case TKT_CS_REPEAT   :
        case TKT_CS_REPEAT2  :
        case TKT_CS_SELECT   :
        case TKT_CS_SELECT2  :
        case TKT_CS_UNTIL    :
        case TKT_CS_WHILE    :
        case TKT_CS_WHILE2   :
        case TKT_CS_SKIPCASE :
        case TKT_CS_SKIPEND  :
#define lptdAnon    ((ANON_CTRL_STRUC *) lptkData)
            // Append the Ctrl Struc token to the CS stack
            //   to allow for later parsing for SYNTAX ERRORs
            AppendNewCSToken_EM (lptkFlags->TknType,
                                 lptdAnon->uLineNum,
                                 lptdAnon->uStmtNum,
                                 lptdAnon->uTknNum,
                                 lptdAnon->bSOS,
                                 lptkLocalVars->uChar);
#undef  lptdAnon
            break;

        case TKT_CS_GOTO     :  // Ignore these tokens
        case TKT_CS_RETURN   :
            break;

        case TKT_CS_NEC      :
        case TKT_CS_EOL      :
        defstop
            break;
    } // End SWITCH

    return TRUE;
} // End AppendNewToken_EM
#undef  APPEND_NAME


//***************************************************************************
//  $AppendNewCSToken_EM
//
//  Append a new token to the CS stack
//***************************************************************************

UBOOL AppendNewCSToken_EM
    (TOKEN_TYPES TknType,       // CS token type (TKT_CS_xxx)
     USHORT      uLineNum,      // Line #
     USHORT      uStmtNum,      // Stmt #
     USHORT      uTknNum,       // Token #
     UBOOL       bSOS,          // TRUE iff the matching CS starts a stmt
     UINT        tkCharIndex)   // Index into the input line of this token

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    TOKEN        tkCS = {0};    // Control Structure token

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If there's a preceding token, ...
    if (lpMemPTD->lptkCSNxt >= &lpMemPTD->lptkCSIni[1])
    {
        // Coalesce CS_NEC ... CS_NEC -> CS_NEC
        //          EOS SOS           -> <empty>
        //          NEC LABELSEP      -> LABELSEP

        // Check for duplicate CS_NEC tokens
        if (TKT_CS_NEC EQ TknType
         && TKT_CS_NEC EQ lpMemPTD->lptkCSNxt[-1].tkFlags.TknType)
            goto SKIP_EXIT;

        // Check for EOS SOS pair (blank line)
        if (TKT_SOS EQ TknType
         && TKT_EOS EQ lpMemPTD->lptkCSNxt[-1].tkFlags.TknType)
        {
            // Delete the preceding EOS
            lpMemPTD->lptkCSNxt--;

            goto SKIP_EXIT;
        } // End IF

        if (TKT_LABELSEP EQ TknType
         && TKT_CS_NEC EQ lpMemPTD->lptkCSNxt[-1].tkFlags.TknType)
        {
            // Delete the preceding NEC
            lpMemPTD->lptkCSNxt--;

            goto SKIP_EXIT;
        } // End IF
    } // End IF

    // Fill in the token values
    tkCS.tkFlags.TknType = TknType;
    tkCS.tkData.uLineNum = uLineNum;
    tkCS.tkData.uStmtNum = uStmtNum;
    tkCS.tkData.uTknNum  = uTknNum;
    tkCS.tkData.bSOS     = bSOS;
    tkCS.tkData.uCLIndex = 0;
    tkCS.tkCharIndex     = tkCharIndex;

    // Save the token on the CS stack
    *lpMemPTD->lptkCSNxt++ = tkCS;
SKIP_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return TRUE;
} // End AppendNewCSToken_EM


//***************************************************************************
//  $CharTrans
//
//  Translate a character in preparation for tokenizing
//***************************************************************************

WCHAR CharTrans
    (WCHAR         wchOrig,             // Char to translate into a COL_xxx index
     LPTKLOCALVARS lptkLocalVars)       // Ptr to local vars

{
    // Split cases
    switch (wchOrig)
    {
        case L'a':
        case L'b':
        case L'c':
        case L'd':
        case L'e':
        case L'f':
        case L'g':
        case L'h':
        case L'i':
        case L'j':
        case L'k':
        case L'l':
        case L'm':
        case L'n':
        case L'o':
        case L'p':
        case L'q':
        case L'r':
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
        case L'E':
        case L'F':
        case L'G':
        case L'H':
        case L'I':
        case L'J':
        case L'K':
        case L'L':
        case L'M':
        case L'N':
        case L'O':
        case L'P':
        case L'Q':
        case L'R':
        case L'S':
        case L'T':
        case L'U':
        case L'V':
        case L'W':
        case L'X':
        case L'Y':
        case L'Z':

        case UTF16_A_:                  // Alphabet underbar
        case UTF16_B_:
        case UTF16_C_:
        case UTF16_D_:
        case UTF16_E_:
        case UTF16_F_:
        case UTF16_G_:
        case UTF16_H_:
        case UTF16_I_:
        case UTF16_J_:
        case UTF16_K_:
        case UTF16_L_:
        case UTF16_M_:
        case UTF16_N_:
        case UTF16_O_:
        case UTF16_P_:
        case UTF16_Q_:
        case UTF16_R_:
        case UTF16_S_:
        case UTF16_T_:
        case UTF16_U_:
        case UTF16_V_:
        case UTF16_W_:
        case UTF16_X_:
        case UTF16_Y_:
        case UTF16_Z_:

        case UTF16_DELTA:               // Alt-'h' - delta
        case UTF16_DELTAUNDERBAR:       // Alt-'H' - delta-underbar
            return COL_ALPHA;

        case L'_':
            return COL_UNDERBAR;

        case UTF16_INFINITY:
            return COL_INFINITY;

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

        case UTF16_ALPHA:               // Alt-'a' - alpha
        case UTF16_OMEGA:               // Alt-'w' - omega
            return COL_DIRIDENT;

////////case UTF16_ALPHA:               // Alt-'a' - alpha (COL_DIRIDENT)
        case UTF16_UPTACK:              // Alt-'b' - up tack
        case UTF16_UPSHOE:              // Alt-'c' - up shoe
        case UTF16_DOWNSTILE:           // Alt-'d' - down stile
        case UTF16_EPSILON:             // Alt-'e' - epsilon
////////case UTF16_INFINITY:            // Alt-'f' - infinity (COL_INFINITY)
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
////////case UTF16_CIRCLEMIDDLEDOT      // Alt-'@' - circle-middle-dot (COL_PRIM_OP1)
////////case UTF16_                     // Alt-'A' - (none)
////////case UTF16_                     // Alt-'B' - (none)
////////case UTF16_                     // Alt-'C' - (none)
////////case UTF16_                     // Alt-'D' - (none)
        case UTF16_EPSILONUNDERBAR:     // Alt-'E' - epsilon-underbar
////////case UTF16_                     // Alt-'F' - (none)
////////case UTF16_DIERESISDEL:         // Alt-'G' - dual (COL_PRIM_OP2)
////////case UTF16_DELTAUNDERBAR:       // Alt-'H' - delta-underbar (COL_ALPHA)
        case UTF16_IOTAUNDERBAR:        // Alt-'I' - iota-underbar
////////case UTF16_DIERESISJOT:         // Alt-'J' - rank (hoot) (COL_PRIM_OP2)
////////case UTF16_                     // Alt-'K' - (none)
        case UTF16_SQUAD:               // Alt-'L' - squad
////////case UTF16_                     // Alt-'M' - (none)
////////case UTF16_DIERESISDOWNTACK:    // Alt-'N' - convolution (COL_PRIM_OP2)
////////case UTF16_DIERESISCIRCLE:      // Alt-'O' - ??? (holler)
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
        case UTF16_CIRCLEMIDDLEDOT:     // Alt-'@' - circle-middle-dot
            return COL_PRIM_OP1;

        case UTF16_DIERESISDEL:         // Alt-'G' - dual
        case UTF16_DIERESISJOT:         // Alt-'J' - rank (hoot)
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

        case UTF16_LEFTPAREN:           //     '(' - left paren
            return COL_LEFTPAREN;

        case UTF16_RIGHTPAREN:          //     ')' - right paren
            return COL_RIGHTPAREN;

        case UTF16_LEFTBRACKET:         //     '[' - left bracket
            return COL_LEFTBRACKET;

        case UTF16_RIGHTBRACKET:        //     ']' - right bracket
            return COL_RIGHTBRACKET;

        case UTF16_SEMICOLON:           // Lists (bracketed and otherwise)
            return COL_SEMICOLON;

        case UTF16_COLON:               // Line labels
            // Handle odd cases such as L1:in{is}1
            if ((lptkLocalVars->State[1] EQ FSA_SOS && lptkLocalVars->State[0] EQ FSA_ALPHA)
             || (lptkLocalVars->State[2] EQ FSA_SOS && lptkLocalVars->State[1] EQ FSA_ALPHA && lptkLocalVars->State[0] EQ FSA_INIT))
                return COL_COLON;

            // Check the next few chars to see if they match any
            //   Control Structures
            if (CtrlStrucCmpi (lptkLocalVars, L":andif"    , TKT_CS_ANDIF    )
             || CtrlStrucCmpi (lptkLocalVars, L":case"     , TKT_CS_CASE     )
             || CtrlStrucCmpi (lptkLocalVars, L":caselist" , TKT_CS_CASELIST )
             || CtrlStrucCmpi (lptkLocalVars, L":continue" , TKT_CS_CONTINUE )
             || CtrlStrucCmpi (lptkLocalVars, L":else"     , TKT_CS_ELSE     )
             || CtrlStrucCmpi (lptkLocalVars, L":elseif"   , TKT_CS_ELSEIF   )
             || CtrlStrucCmpi (lptkLocalVars, L":end"      , TKT_CS_END      )
             || CtrlStrucCmpi (lptkLocalVars, L":endfor"   , TKT_CS_ENDFOR   )
             || CtrlStrucCmpi (lptkLocalVars, L":endforlcl", TKT_CS_ENDFORLCL)
             || CtrlStrucCmpi (lptkLocalVars, L":endif"    , TKT_CS_ENDIF    )
             || CtrlStrucCmpi (lptkLocalVars, L":endrepeat", TKT_CS_ENDREPEAT)
             || CtrlStrucCmpi (lptkLocalVars, L":endselect", TKT_CS_ENDSELECT)
             || CtrlStrucCmpi (lptkLocalVars, L":endwhile" , TKT_CS_ENDWHILE )
             || CtrlStrucCmpi (lptkLocalVars, L":for"      , TKT_CS_FOR      )
             || CtrlStrucCmpi (lptkLocalVars, L":forlcl"   , TKT_CS_FORLCL   )
             || CtrlStrucCmpi (lptkLocalVars, L":goto"     , TKT_CS_GOTO     )
             || CtrlStrucCmpi (lptkLocalVars, L":if"       , TKT_CS_IF       )
             || CtrlStrucCmpi (lptkLocalVars, L":in"       , TKT_CS_IN       )
             || CtrlStrucCmpi (lptkLocalVars, L":leave"    , TKT_CS_LEAVE    )
             || CtrlStrucCmpi (lptkLocalVars, L":orif"     , TKT_CS_ORIF     )
             || CtrlStrucCmpi (lptkLocalVars, L":repeat"   , TKT_CS_REPEAT   )
             || CtrlStrucCmpi (lptkLocalVars, L":return"   , TKT_CS_RETURN   )
             || CtrlStrucCmpi (lptkLocalVars, L":select"   , TKT_CS_SELECT   )
             || CtrlStrucCmpi (lptkLocalVars, L":until"    , TKT_CS_UNTIL    )
             || CtrlStrucCmpi (lptkLocalVars, L":while"    , TKT_CS_WHILE    ))
                return COL_CTRLSTRUC;
            else
                return COL_COLON;

        case UTF16_LEFTBRACE:           //     '{' - left brace
            return COL_LEFTBRACE;

        case UTF16_RIGHTBRACE:          //     '}' - right brace
            return COL_RIGHTBRACE;

        case L'#':
            // If we're tokenizing a Magic Function, ...
            if (lptkLocalVars->bMF)
                return COL_ALPHA;
            else
                return COL_UNK;

        case UTF16_DEL:                 // Alt-'g' - del
        case UTF16_DIERESISCIRCLE:      // Alt-'O' - ??? (holler)
        case L'`':
        case L'@':
        case L'$':
        case L'%':
        case L'&':
        case L'\0':
            return COL_UNK;

        default:
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
        {
            WCHAR wszTemp[64];

            wsprintfW (wszTemp,
                       L"CharTrans:  Unknown char: %c (%d)",
                       wchOrig,
                       wchOrig);
            DbgMsgW (wszTemp);
        }
#endif
            return COL_UNK;
    } // End SWITCH
} // End CharTrans


//***************************************************************************
//  $CtrlStrucCmpi
//
//  Case-insensitive comparison of a Control Structure name
//***************************************************************************

UBOOL CtrlStrucCmpi
    (LPTKLOCALVARS lptkLocalVars,       // Ptr to local vars
     LPWCHAR       lpwCSName,           // Ptr to Control Structure name
     TOKEN_TYPES   TknType)             // Matching token type

{
    UINT    uCSLen,                     // Length of lpwCSName
            uCnt;                       // Loop counter
    LPWCHAR lpwLine;                    // Ptr to line contents

    // Get the length of the name
    uCSLen = lstrlenW (lpwCSName);

    // Get a ptr to the line
    lpwLine = lptkLocalVars->lpwszCur;

    // Loop through the name
    for (uCnt = 0; uCnt < uCSLen; uCnt++)
    if (((WCHAR) CharLowerW ((LPWCHAR) *lpwLine++)) NE *lpwCSName++)
        return FALSE;

    // Ensure the next char after the name in the line is not valid
    //   in a name and thus terminates the name
    if (IsValid2ndCharInName (*lpwLine))
        return FALSE;

    // Save the token type and string length for later use
    lptkLocalVars->CtrlStrucTknType = TknType;
    lptkLocalVars->CtrlStrucStrLen  = uCSLen;
    lptkLocalVars->bSOS             = (lptkLocalVars->uChar EQ lptkLocalVars->uCharIni);

    return TRUE;
} // End CtrlStrucCmpi


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
{{L"DIGIT"       , COL_DIGIT       },   // 00: Digit
 {L"DOT"         , COL_DOT         },   // 01: Decimal number, inner & outer product separator
 {L"ALPHA"       , COL_ALPHA       },   // 02: Alphabetic
 {L"OVERBAR"     , COL_OVERBAR     },   // 03: Overbar
 {L"DIRIDENT"    , COL_DIRIDENT    },   // 04: Alpha or Omega
 {L"Q_QQ"        , COL_Q_QQ        },   // 05: Quad
 {L"UNDERBAR"    , COL_UNDERBAR    },   // 06: Underbar
 {L"INFINITY"    , COL_INFINITY    },   // 07: Infinity
 {L"ASSIGN"      , COL_ASSIGN      },   // 08: Assignment symbol
 {L"SEMICOLON"   , COL_SEMICOLON   },   // 09: Semicolon symbol
 {L"COLON"       , COL_COLON       },   // 0A: Colon symbol
 {L"CTRLSTRUC"   , COL_CTRLSTRUC   },   // 0B: Control Structure
 {L"PRIM_FN"     , COL_PRIM_FN     },   // 0C: Primitive monadic or dyadic function
 {L"PRIM_FN0"    , COL_PRIM_FN0    },   // 0D: ...       niladic function
 {L"PRIM_OP1"    , COL_PRIM_OP1    },   // 0E: ...       monadic operator
 {L"PRIM_OP2"    , COL_PRIM_OP2    },   // 0F: ...       dyadic  ...
 {L"JOT"         , COL_JOT         },   // 10: Jot symbol
 {L"LEFTPAREN"   , COL_LEFTPAREN   },   // 11: Left paren
 {L"RIGHTPAREN"  , COL_RIGHTPAREN  },   // 12: Right ...
 {L"LEFTBRACKET" , COL_LEFTBRACKET },   // 13: Left bracket
 {L"RIGHTBRACKET", COL_RIGHTBRACKET},   // 14: Right ...
 {L"LEFTBRACE"   , COL_LEFTBRACE   },   // 15: Left brace
 {L"RIGHTBRACE"  , COL_RIGHTBRACE  },   // 16: Right ...
 {L"SPACE"       , COL_SPACE       },   // 17: White space (' ' or '\t')
 {L"QUOTE1"      , COL_QUOTE1      },   // 18: Single quote symbol
 {L"QUOTE2"      , COL_QUOTE2      },   // 19: Double ...
 {L"DIAMOND"     , COL_DIAMOND     },   // 1A: Diamond symbol
 {L"LAMP"        , COL_LAMP        },   // 1B: Comment symbol
 {L"EOL"         , COL_EOL         },   // 1C: End-Of-Line
 {L"UNK"         , COL_UNK         },   // 1D: Unknown symbols
};
    if (COL_LENGTH > uType)
        return colNames[uType].lpwsz;
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
//  $SetTokenStates
//
//  Set tkLocalVars.State[0 1 2] based upon curState
//***************************************************************************

void SetTokenStates
    (LPTKLOCALVARS lptkLocalVars,       // Ptr to local vars
     FSATOKENS     curState)            // Incoming state

{
    // If the previous and current states differ, ...
    if (lptkLocalVars->State[1] NE lptkLocalVars->State[0]
     && lptkLocalVars->State[0] NE curState)
    {
        lptkLocalVars->State[2] =  lptkLocalVars->State[1];
        lptkLocalVars->State[1] =  lptkLocalVars->State[0];
    } // End IF

    // Save the current state
    lptkLocalVars->State[0] = curState;
} // End SetTokenStates


//***************************************************************************
//  End of File: tokenize.c
//***************************************************************************
