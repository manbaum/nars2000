//***************************************************************************
//  NARS2000 -- Defined Function Header
//***************************************************************************

typedef struct tagFCNLINE           // Function line structure, one per function line
{
    HGLOBAL hGlbTxtLine;            // 00:  Text of the line (APLCHAR) global memory handle
    HGLOBAL hGlbTknLine;            // 04:  Tokenized line (TOKEN)     ...
    HGLOBAL hGlbMonInfo;            // 08:  Monitor information (??)   ... (may be NULL)
                                    // 0C:  Length
} FCNLINE, *LPFCNLINE;

typedef enum tagDFN_TYPES            // Defined Function Types
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

typedef enum tagFCN_VALENCES        // Defined/Derived Function Valence
{
    FCNVALENCE_NIL = 0,             // 00:  Niladic function
    FCNVALENCE_MON,                 // 01:  Monadic function/derived function
    FCNVALENCE_DYD,                 // 02:  Dyadic  ...
    FCNVALENCE_AMB,                 // 03:  Ambivalent ...
    FCNVALENCE_IMM,                 // 04:  Immediate execution
                                    // 05-07:  Available entries (3 bits)
} FCN_VALENCES;

// Defined function header signature
#define DFN_HEADER_SIGNATURE   'SNFD'

typedef struct tagDFN_HEADER        // Function header structure
{
    HEADER_SIGNATURE Sig;           // 00:  Defined function header signature
    UINT             Version;       // 04:  Version # of this header
    UINT             DfnType:4,     // 08:  0000000F:  Defined Function Type (see enum DFN_TYPES)
                     FcnValence:3,  //      00000070:  Defined/Derived Function Valence (see enum FCN_VALENCES)
                     DfnAxis:1,     //      00000080:  Defined/Derived Function accepts axis value
                     Avail:24;      //      FFFFFF00:  Available bits
    UINT             RefCnt,        // 0C:  Reference count
                     nPrototypeLine,// 10:  Line # of the []PROTOTYPE label (0 if not present)
                     nInverseLine,  // 14:  Line # of the []INVERSE label (0 if not present)
                     nSingletonLine,// 18:  Line # of the []SINGLETON label (0 if not present)
                     numResultSTE,  // 1C:  # result STEs (may be zero if no result)
                     offResultSTE,  // 20:  Offset to result STEs (ignored if numResultSTE is zero)
                     numLftArgSTE,  // 24:  # left arg STEs (may be zero if niladic/monadic)
                     offLftArgSTE,  // 28:  Offset to left arg STEs (ignored if numLftArgSTE is zero)
                     numRhtArgSTE,  // 2C:  # right arg STEs (may be zero if niladic)
                     offRhtArgSTE,  // 30   Offset to right arg STEs (ignored if numRhtArgSTE is zero)
                     numLocalsSTE,  // 34:  # right arg STEs (may be zero if niladic)
                     offLocalsSTE,  // 38:  Offset to start of function lines (FCNLINE[nLines])
                     numFcnLines,   // 3C:  # lines in the function (not counting the header)
                     offFcnLines;   // 40:  Offset to start of function lines (FCNLINE[nLines])
    LPSYMENTRY       steLftOpr,     // 44:  Left operand STE (may be NULL if not an operator)
                     steFcnName,    // 48:  Function name STE
                     steAxisOpr,    // 4C:  Axis operator STE
                     steRhtOpr;     // 50:  Right operand STE (may be NULL if monadic operator or not an operator)
    HGLOBAL          hGlbTxtHdr,    // 54:  Text of function header (APLCHAR) ...
                     hGlbTknHdr,    // 58:  Tokenized function header (TOKEN) ...
                     hGlbUndoBuff;  // 5C:  Undo buffer                       ... (may be NULL)
                                    // 60:  Length
                                    // 60:  Array of function line structures (FCNLINE[nLines])
} DFN_HEADER, *LPDFN_HEADER;

// Whenever changing the above struct, be sure to make a
//   corresponding change to
//   <CalcSymentrySize> and <SysFnMonCR_EM> in <sysfns.c>,
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
    UINT         DfnType:4,         // 20:  0000000F:  Defined Function Type (see DFN_TYPES enum)
                 FcnValence:3,      //      00000070:  Defined/Derived Function Valence (see enum FCN_VALENCES)
                 DfnAxis:1,         //      00000080:  Defined/Derived Function accepts axis value
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
