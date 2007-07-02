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

typedef enum tagDFNTYPES            // Defined Function Types
{
    DFNTYPE_UNK = 0,                // 00:  Unknown
    DFNTYPE_OP1,                    // 01:  Monadic operator
    DFNTYPE_OP2,                    // 02:  Dyadic operator
    DFNTYPE_FCN,                    // 03:  Niladic/monadic/dyadic/ambivalent function
                                    // ??:  No available entries (2 bits)
} DFNTYPES;

typedef enum tagFCNVALENCES         // Defined/Derived Function Valence
{
    FCNVALENCE_NIL = 0,             // 00:  Niladic function
    FCNVALENCE_MON,                 // 01:  Monadic function/derived function
    FCNVALENCE_DYD,                 // 02:  Dyadic  ...
    FCNVALENCE_AMB,                 // 03:  Ambivalent ...
                                    // ??:  No available entries (2 bits)
} FCNVALENCES;


// Defined function header signature
#define DFN_HEADER_SIGNATURE   'SNFD'

typedef struct tagDFN_HEADER        // Function header structure
{
    HEADER_SIGNATURE Sig;           // 00:  Defined function header signature
    UINT             Version;       // 04:  Version # of this header
    UINT             DfnType:2,     // 08:  00000003:  Defined Function Type (see DFNTYPES enum)
                     FcnValence:2;  //      0000000C:  Defined/Derived Function Valence (see FCNVALENCES enum)
                                    //      FFFFFFF0:  Available bits
    UINT             RefCnt,        // 0C:  Reference count
                     nPrototypeLine,// 10:  Line # of the []PROTOTYPE label (0 if not present)
                     nInverseLine,  // 14:  Line # of the []INVERSE label (0 if not present)
                     nSingletonLine,// 18:  Line # of the []SINGLETON label (0 if not present)
                     numResultSTE,  // 1C:  # result STEs (may be zero if no result)
                     offResultSTE,  // 20:  Offset to result STEs (ignored if numResSTE is zero)
                     numLftArgSTE,  // 24:  # left arg STEs (may be zero if niladic/monadic)
                     offLftArgSTE,  // 28:  Offset to left arg STEs (ignored if numLftArgSTE is zero)
                     numRhtArgSTE,  // 2C:  # right arg STEs (may be zero if niladic)
                     offRhtArgSTE,  // 30   Offset to right arg STEs (ignored if numRhtArgSTE is zero)
                     numLocalsSTE,  // 34:  # right arg STEs (may be zero if niladic)
                     offLocalsSTE,  // 38:  Offset to start of function lines (FCNLINE[nLines])
                     numFcnLines,   // 3C:  # lines in the function (not counting the header)
                     offFcnLines;   // 40:  Offset to start of function lines (FCNLINE[nLines])
    LPSYMENTRY       steLftOpr,     // 44:  Left operand STE (may be NULL)
                     steFcnName,    // 48:  Function name STE
                     steRhtOpr;     // 4C:  Right operand STE (may be NULL)
    HGLOBAL          hGlbTxtHdr,    // 50:  Text of function header (APLCHAR) ...
                     hGlbTknHdr,    // 54:  Tokenized function header (TOKEN) ...
                     hGlbUndoBuff;  // 58:  Undo buffer                       ... (may be NULL)
                                    // 5C:  Array of function line structures (FCNLINE[nLines])
                                    // 60:  Length
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
    UINT   uStrandLen;              // 14:  # elements in this strand
    struct tagFH_YYSTYPE *
           lpYYStrandBase;          // 18:  Ptr to this token's strand base
                                    // 1C:  Length
} FH_YYSTYPE, *FH_LPYYSTYPE;        // Data type of yacc stack

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
    UINT         DfnType:2,         // 20:  00000003:  Defined Function Type (see enum DFNTYPE)
                 FcnValence:2;      //      0000000C:  Defined/Derived Function Valence (see enum FCNVALENCE)
                                    //      FFFFFFF0:  Available bits
    FH_LPYYSTYPE lpYYStrandStart,   // 24:  Strand stack start (static)
                 lpYYStrandBase,    // 28:  ...          base (dynamic)
                 lpYYStrandNext,    // 2C:  ...          next token (dynamic)
                 lpYYResult,        // 30:  Ptr to result name or list
                 lpYYLftArg,        // 34:  ...    left arg name or list
                 lpYYLftOpr,        // 38:  ...    left operand name
                 lpYYFcnName,       // 3C:  ...    function/operator name
                 lpYYRhtOpr,        // 40:  ...    right operand name
                 lpYYRhtArg,        // 44:  ...    right arg name or list
                 lpYYLocals;        // 48:  ...    locals name or list
                                    // 4C:  Length
} FHLOCALVARS, *LPFHLOCALVARS;


//***************************************************************************
//  End of File: dfnhdr.h
//***************************************************************************
