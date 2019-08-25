//***************************************************************************
//  NARS2000 -- User-Defined Function/Operator Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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

typedef struct tagINTMONINFO            // Internal function line monitoring info
{
    APLINT  IncSubFns,                  // 00:  Total time including subfunctions (8 bytes)
            ExcSubFns;                  // 08:  ...        excluding ...          (8 bytes)
    UINT    Count:30,                   // 10:  3FFFFFFF:  # times executed
            IncActive:1,                //      40000000:  IncSubFns active
            ExcActive:1;                //      80000000:  ExcSubFns active
                                        // 14:  Length
} INTMONINFO, *LPINTMONINFO;

typedef struct tagEXTMONINFO_INT        // External function line monitoring info as integers
{
    APLINT  IncSubFns,                  // 00:  Total time including subfunctions (8 bytes)
            ExcSubFns,                  // 08:  ...        excluding ...          (8 bytes)
            Count;                      // 10:  # times executed                  (8 bytes)
                                        // 18:  Length
} EXTMONINFO_INT, *LPEXTMONINFO_INT;

typedef struct tagEXTMONINFO_FLT        // External function line monitoring info as floats
{
    APLFLOAT IncSubFns,                 // 00:  Total time including subfunctions (8 bytes)
             ExcSubFns,                 // 08:  ...        excluding ...          (8 bytes)
             Count;                     // 10:  # times executed                  (8 bytes)
                                        // 18:  Length
} EXTMONINFO_FLT, *LPEXTMONINFO_FLT;

typedef struct tagFCNLINE               // Function line structure, one per function line
{
    HGLOBAL hGlbTxtLine;                // 00:  Text of the line (APLCHAR) global memory handle
    UINT    offTknLine,                 // 04:  Offset to tokenized line (TOKEN_HEADER)
            numNxtLabel1;               // 08:  Line # of next labeled line (origin-1) (0 = none)
    UINT    bStop:1,                    // 0C:  00000001:  Stop on this line
            bTrace:1,                   //      00000002:  Trace this line
            bEmpty:1,                   //      00000004:  Empty line
            bLabel:1,                   //      00000008:  Labeled line
            :28;                        //      FFFFFFF0:  Available bits
                                        // 10:  Length
} FCNLINE, *LPFCNLINE;

typedef struct tagLBLENTRY              // Line label entries
{
    LPSYMENTRY lpSymEntry;              // 00:  Ptr to line label SYMENTRY
    UINT       uLineNum1;               // 04:  Line # (origin-1)
} LBLENTRY, *LPLBLENTRY;

typedef enum tagFCN_VALENCES            // User-Defined Function/Operator Valence
{
    FCNVALENCE_NIL = 0,                 // 00:  Niladic function
    FCNVALENCE_MON,                     // 01:  Monadic function/derived function
    FCNVALENCE_DYD,                     // 02:  Dyadic  ...
    FCNVALENCE_AMB,                     // 03:  Ambivalent ...
    FCNVALENCE_IMM,                     // 04:  Immediate execution
                                        // 05-07:  Available entries (3 bits)
} FCN_VALENCES, LPFCN_VALENCES;

#define FCNVALENCE_LENGTH   FCNVALENCE_IMM  // # UDFO-related entries in the above enum

// N.B.:  Whenever changing the above enum (tagFCN_VALENCES)
//   be sure to make a corresponding change to
//   <lpwFcnHdr>, <lpwOp1Hdr>, and <lpwOp2Hdr> in <savefcn.c>

typedef struct tagSYSLBL_ENTRY
{
    UINT sysLblLine,                    // 00:  Line # of the entry point (Origin-1)
         sysLblTkn;                     // 04:  Token # ...               (Origin-0)
} SYSLBLENTRY, *LPSYSLBLENTRY;

// User-defined function/operator header signature
#define DFN_HEADER_SIGNATURE   'SNFD'

typedef struct tagDFN_HEADER            // Function header structure
{
    HEADER_SIGNATURE Sig;               // 00:  User-defined function/operator header signature
    UINT             Version;           // 04:  Version # of this header
    UINT             DfnType:4,         // 08:  0000000F:  User-defined function/operator type (see DFN_TYPES)
                     FcnValence:3,      //      00000070:  User-defined function/operator valence (see FCN_VALENCES)
                     DfnAxis:1,         //      00000080:  User-defined function/operator accepts axis value
                     NoDispRes:1,       //      00000100:  TRUE iff the result is non-displayable
                     ListRes:1,         //      00000200:  TRUE iff the result is a list (unused so far)
                     ListLft:1,         //      00000400:  TRUE iff the left arg is a list
                     ListRht:1,         //      00000800:  TRUE iff the right arg is a list
                     MonOn:1,           //      00001000:  TRUE iff function line monitoring is on for this function
                     SaveSTEFlags:1,    //      00002000:  TRUE iff on free we are to save the function name STE flags
                     bAFO:1,            //      00004000:  TRUE iff this is an AFO
                     bAfoCtrlStruc:1,   //      00008000:  TRUE iff Ctrl Strucs in AFO
                     bAfoNoDispRes:1,   //      00010000:  TRUE iff the AFO result is non-displayable
                     bMFO:1,            //      00020000:  TRUE iff this is an MFO
                     bStopHdr:1,        //      00040000:  TRUE iff we're stopping on exit
                     bTraceHdr:1,       //      00080000:  TRUE iff we're tracing on exit
                     bLclRL:1,          //      00100000:  TRUE iff []RL is localized in this function
                     :11;               //      FFE00000:  Available bits
    UINT             RefCnt,            // 0C:  Reference count
                     numResultSTE,      // 10:  # result STEs (may be zero if no result)
                     offResultSTE,      // 14:  Offset to result STEs (ignored if numResultSTE is zero)
                     numLftArgSTE,      // 18:  # left arg STEs (may be zero if niladic/monadic)
                     offLftArgSTE,      // 1C:  Offset to left arg STEs (ignored if numLftArgSTE is zero)
                     numRhtArgSTE,      // 20:  # right arg STEs (may be zero if niladic)
                     offRhtArgSTE,      // 24:  Offset to right arg STEs (ignored if numRhtArgSTE is zero)
                     numLocalsSTE,      // 28:  # right arg STEs (may be zero if niladic)
                     offLocalsSTE,      // 2C:  Offset to start of function lines (FCNLINE[nLines])
                     numFcnLines,       // 30:  # Logical lines in the function (not counting the header)
                     numPhyLines,       // 34:  # Physical ...
                     offFcnLines,       // 38:  Offset to start of function lines (FCNLINE[nLines])
                     offTknLines,       // 3C:  Offset to start of tokenized function lines
                     numLblLines,       // 40:  # labeled lines in the function
                     num1stLabel1;      // 44:  Line # of 1st labeled line (origin-1)
    SYSLBLENTRY      aSysLblId,         // 48:  Line/token #s of the []ID  label (0 if not present) (8 bytes)
                     aSysLblInv,        // 50:  Line/token #s of the []INV ...
                     aSysLblMs,         // 58:  Line/token #s of the []MS  ...
                     aSysLblPro;        // 60:  Line/token #s of the []PRO ...
    LPSYMENTRY       steLftOpr,         // 68:  Left operand STE (may be NULL if not an operator)
                     steFcnName,        // 6C:  Function name STE
                     steAxisOpr,        // 70:  Axis operator STE
                     steRhtOpr,         // 74:  Right operand STE (may be NULL if monadic operator or not an operator)
                     steDel,            // 78:  Del function STE
                     steDelDel;         // 7C:  DelDel operator STE
    HGLOBAL          hGlbTxtHdr,        // 80:  Text of function header (APLCHAR) global memory handle
                     hGlbTknHdr,        // 84:  Tokenized function header (TOKEN) ...
                     hGlbUndoBuff,      // 88:  Undo buffer (UNDO_BUF)            ... (may be NULL)
                     hGlbMonInfo;       // 8C:  Function line monitor info (MONINFO)
    FILETIME         ftCreation,        // 90:  Time of creation (8 bytes)
                     ftLastMod;         // 98:  Time of last modification (8 bytes)
    HSHTABSTR        htsDFN;            // A0:  Local HTS (htsDFN.lpHshTab may be NULL if none) (124 = 0x7C bytes)
                                        //11C:  Length
                                        //11C:  Array of function line structures (FCNLINE[nLines])
} DFN_HEADER, *LPDFN_HEADER;

// Whenever changing the above struct, be sure to make a
//   corresponding change to
//   <CalcSymentrySize> in <qf_at.c>,
//   <SaveFunction> in <savefcn.c>,
//   <FEWndProc:WM_CREATE> in <editfcn.c>, and
//   <FreeResultGlobalDfn> in <free.c>.


typedef struct tagFH_YYSTYPE            // YYSTYPE for Function Header parser
{
    TOKEN  tkToken;                     // 00:  Token info (28 bytes)
    UINT   uStrandLen:30,               // 1C:  3FFFFFFF:  # elements in this strand
           Indirect:1,                  //      40000000:  Indirect entry
           ListAny:1;                   //      80000000:  Itsa list
                                        //      00000000:  No available bits
    struct tagFH_YYSTYPE *
           lpYYStrandIndirect;          // 20:  Ptr to the indirect strand if .Indirect is set
    struct tagFH_YYSTYPE *
           lpYYStrandBase;              // 24:  Ptr to this token's strand base
    UINT   offTknIndex;                 // 28:  Offset in tokens in hGlbTknHdr
                                        // 2C:  Length
} FH_YYSTYPE, *LPFH_YYSTYPE;            // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1


typedef struct tagFHLOCALVARS           // Function Header Local Vars
{
    HWND           hWndEC;              // 00:  Window handle for Edit Ctrl
    HGLOBAL        hGlbTknHdr,          // 04:  Tokenized header global memory handle
                   hGlbUndoBuff;        // 08:  Undo buffer      ...
    LPTOKEN_HEADER lpHeader;            // 0C:  Ptr to tokenized line header in global memory
    LPTOKEN        lptkStart,           // 10:  First available entry after the header
                   lptkNext,            // 14:  Next  ...
                   lptkStop;            // 18:  Stopping token
    UINT           tkErrorCharIndex,    // 1C:  Error char index
                   offTknBase,          // 20:  Offset in hGlbTknHdr of the current token
                   offFcnName,          // 24:  ...                         function name token
                   offFcnText;          // 28:  Offset in hGlbTxtLine of the function name
    UINT           DfnType:4,           // 2C:  0000000F:  User-defined function/operator type (see DFN_TYPES)
                   FcnValence:3,        //      00000070:  User-defined function/operator valence (see FCN_VALENCES)
                   DfnAxis:1,           //      00000080:  User-defined function/operator accepts axis value
                   DisplayErr:1,        //      00000100:  TRUE iff we should display error messages
                   NoDispRes:1,         //      00000200:  TRUE iff the result is non-displayable
                   ListRes:1,           //      00000400:  TRUE iff the result is a list
                   ListLft:1,           //      00000800:  TRUE iff the left arg ...
                   ListRht:1,           //      00001000:  TRUE iff the right arg ...
                   ParseFcnName:1,      //      00002000:  TRUE iff we're parsing the function name
                   fhNameType:4,        //      0003C000:  Function name type (see NAME_TYPES)
                   bAFO:1,              //      00040000:  TRUE iff we're parsing an AFO
                   bLclRL:1,            //      00080000:  TRUE iff []RL is localized in this function
                   :12;                 //      FFF00000:  Available bits
    LPFH_YYSTYPE   lpYYStrandStart,     // 30:  Strand stack start (static)
                   lpYYStrandBase,      // 34:  ...          base (dynamic)
                   lpYYStrandNext,      // 38:  ...          next token (dynamic)
                   lpYYResult,          // 3C:  Ptr to result name or list
                   lpYYLftArg,          // 40:  ...    left arg name or list
                   lpYYLftOpr,          // 44:  ...    left operand name
                   lpYYFcnName,         // 48:  ...    function/operator name
                   lpYYAxisOpr,         // 4C:  ...    axis operator name
                   lpYYRhtOpr,          // 50:  ...    right operand name
                   lpYYRhtArg,          // 54:  ...    right arg name or list
                   lpYYLocals;          // 58:  ...    locals name or list
    WCHAR          wszErrMsg[256];      // 5C:  Save area for error message
                                        //25C:  Length
} FHLOCALVARS, *LPFHLOCALVARS;


//***************************************************************************
//  End of File: fh_parse.h
//***************************************************************************
