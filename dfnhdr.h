//***************************************************************************
//  NARS2000 -- User-Defined Function/Operator Header
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

typedef struct tagFCNLINE           // Function line structure, one per function line
{
    HGLOBAL hGlbTxtLine,            // 00:  Text of the line (APLCHAR) global memory handle
            hGlbTknLine,            // 04:  Tokenized line (TOKEN)     ...
            hGlbMonInfo;            // 08:  Monitor information (??)   ... (may be NULL)
    UINT    bStop:1,                // 0C:  00000001:  Stop on this line
            bTrace:1,               //      00000002:  Trace this line
            Avail:30;               //      FFFFFFFC:  Available bits
                                    // 0C:  Length
} FCNLINE, *LPFCNLINE;

typedef enum tagDFN_TYPES            // User-Defined Function/Operator Types
{
    DFNTYPE_UNK = 0,                // 00:  Unknown
    DFNTYPE_OP1,                    // 01:  Monadic operator
    DFNTYPE_OP2,                    // 02:  Dyadic operator
    DFNTYPE_FCN,                    // 03:  Niladic/monadic/dyadic/ambivalent function
    DFNTYPE_IMM,                    // 04:  Immediate execution
    DFNTYPE_EXEC,                   // 05:  Execute primitive
    DFNTYPE_QUAD,                   // 06:  Quad input
    DFNTYPE_QQUAD,                  // 07:  Quote-Quad input
                                    // 08-0F:  Available entries (4 bits)
} DFN_TYPES;

typedef enum tagFCN_VALENCES        // User-Defined Function/Operator Valence
{
    FCNVALENCE_NIL = 0,             // 00:  Niladic function
    FCNVALENCE_MON,                 // 01:  Monadic function/derived function
    FCNVALENCE_DYD,                 // 02:  Dyadic  ...
    FCNVALENCE_AMB,                 // 03:  Ambivalent ...
    FCNVALENCE_IMM,                 // 04:  Immediate execution
                                    // 05-07:  Available entries (3 bits)
} FCN_VALENCES;

// User-defined function/operator header signature
#define DFN_HEADER_SIGNATURE   'SNFD'

typedef struct tagDFN_HEADER        // Function header structure
{
    HEADER_SIGNATURE Sig;           // 00:  User-defined function/operator header signature
    UINT             Version;       // 04:  Version # of this header
    UINT             DfnType:4,     // 08:  0000000F:  User-defined function/operator type (see DFN_TYPES)
                     FcnValence:3,  //      00000070:  User-defined function/operator valence (see FCN_VALENCES)
                     DfnAxis:1,     //      00000080:  User-defined function/operator accepts axis value
                     PermFn:1,      //      00000100:  Permanent function (i.e. Magic Function)
                     Avail:23;      //      FFFFFE00:  Available bits
    UINT             RefCnt,        // 0C:  Reference count
                     nInverseLine,  // 10:  Line # of the []IDENTITY label (0 if not present)
                     nIdentityLine, // 14:  Line # of the []INVERSE label (0 if not present)
                     nPrototypeLine,// 18:  Line # of the []PROTOTYPE label (0 if not present)
                     nSingletonLine,// 1C:  Line # of the []SINGLETON label (0 if not present)
                     numResultSTE,  // 20:  # result STEs (may be zero if no result)
                     offResultSTE,  // 24:  Offset to result STEs (ignored if numResultSTE is zero)
                     numLftArgSTE,  // 28:  # left arg STEs (may be zero if niladic/monadic)
                     offLftArgSTE,  // 2C:  Offset to left arg STEs (ignored if numLftArgSTE is zero)
                     numRhtArgSTE,  // 30:  # right arg STEs (may be zero if niladic)
                     offRhtArgSTE,  // 34   Offset to right arg STEs (ignored if numRhtArgSTE is zero)
                     numLocalsSTE,  // 38:  # right arg STEs (may be zero if niladic)
                     offLocalsSTE,  // 3C:  Offset to start of function lines (FCNLINE[nLines])
                     numFcnLines,   // 40:  # lines in the function (not counting the header)
                     offFcnLines;   // 44:  Offset to start of function lines (FCNLINE[nLines])
    LPSYMENTRY       steLftOpr,     // 48:  Left operand STE (may be NULL if not an operator)
                     steFcnName,    // 4C:  Function name STE
                     steAxisOpr,    // 50:  Axis operator STE
                     steRhtOpr;     // 54:  Right operand STE (may be NULL if monadic operator or not an operator)
    HGLOBAL          hGlbTxtHdr,    // 58:  Text of function header (APLCHAR) global memory handle
                     hGlbTknHdr,    // 5C:  Tokenized function header (TOKEN) ...
                     hGlbUndoBuff;  // 60:  Undo buffer (UNDO_BUF)            ... (may be NULL)
    FILETIME         ftCreation,    // 64:  Time of creation
                     ftLastMod;     // 6C:  Time of last modification
                                    // 74:  Length
                                    // 74:  Array of function line structures (FCNLINE[nLines])
} DFN_HEADER, *LPDFN_HEADER;

// Whenever changing the above struct, be sure to make a
//   corresponding change to
//   <CalcSymentrySize> and <SysFnMonCR_EM_YY> in <sysfns.c>,
//   <FEWndProc:WM_CREATE>, <SaveFunction>,
//   <GetSpecialLabelNums> in <editfcn.c>, and
//   <FreeResultGlobalDfn> in <free.c>.


typedef struct tagFH_YYSTYPE        // YYSTYPE for Function Header parser
{
    TOKEN  tkToken;                 // 00:  Token info
    UINT   uStrandLen:31,           // 14:  7FFFFFFF:  # elements in this strand
           Indirect:1;              //      80000000:  Indirect entry
                                    //      00000000:  No available bits
    struct tagFH_YYSTYPE *
           lpYYStrandIndirect;      // 18:  Ptr to the indirect strand if .Indirect is set
    struct tagFH_YYSTYPE *
           lpYYStrandBase;          // 18:  Ptr to this token's strand base
                                    // 1C:  Length
} FH_YYSTYPE, *LPFH_YYSTYPE;        // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1


typedef struct tagFHLOCALVARS       // Function Header Local Vars
{
    HWND         hWndEC;            // 00:  Window handle for Edit Control
    HGLOBAL      hGlbTknHdr,        // 04:  Tokenized header global memory handle
                 hGlbUndoBuff;      // 08:  Undo buffer      ...
    UNION_TOKEN  t2;                // 0C:  Locked base of hGlbToken
    LPTOKEN      lpStart,           // 10:  First available entry after the header
                 lpNext,            // 14:  Next  ...
                 lpStop;            // 18:  Stopping token
    UINT         tkErrorCharIndex;  // 1C:  Error char index
    UINT         DfnType:4,         // 20:  0000000F:  User-defined function/operator type (see DFN_TYPES)
                 FcnValence:3,      //      00000070:  User-defined function/operator valence (see FCN_VALENCES)
                 DfnAxis:1,         //      00000080:  User-defined function/operator accepts axis value
                 DisplayErr:1,      //      00000100:  TRUE iff we should display error messages
                 Avail:23;          //      FFFFFE00:  Available bits
    LPFH_YYSTYPE lpYYStrandStart,   // 24:  Strand stack start (static)
                 lpYYStrandBase,    // 28:  ...          base (dynamic)
                 lpYYStrandNext,    // 2C:  ...          next token (dynamic)
                 lpYYResult,        // 30:  Ptr to result name or list
                 lpYYLftArg,        // 34:  ...    left arg name or list
                 lpYYLftOpr,        // 38:  ...    left operand name
                 lpYYFcnName,       // 3C:  ...    function/operator name
                 lpYYAxisOpr,       // 40:  ...    axis operator name
                 lpYYRhtOpr,        // 44:  ...    right operand name
                 lpYYRhtArg,        // 48:  ...    right arg name or list
                 lpYYLocals;        // 4C:  ...    locals name or list
                                    // 50:  Length
} FHLOCALVARS, *LPFHLOCALVARS;


//***************************************************************************
//  End of File: dfnhdr.h
//***************************************************************************
