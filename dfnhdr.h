//***************************************************************************
//  NARS2000 -- Defined Function Header
//***************************************************************************

// Function line structure, one per function line
typedef struct tagFCNLINE
{
    HGLOBAL         hGlbTxtLine;    // Text of the line (APLCHAR) global memory handle
    HGLOBAL         hGlbTknLine;    // Tokenized line (TOKEN)     ...
    HGLOBAL         hGlbMonInfo;    // Monitor information (??)   ... (may be NULL)
} FCNLINE, *LPFCNLINE;

typedef enum tagDFNTYPES        // Defined Function Types
{
    DFNTYPE_UNK = 0,            // 00:  Unknown
    DFNTYPE_OP1,                // 01:  Monadic operator
    DFNTYPE_OP2,                // 02:  Dyadic operator
    DFNTYPE_FCN,                // 03:  Niladic/monadic/dyadic/ambivalent function
} DFNTYPES;

typedef enum tagFCNVALENCES     // Defined/Derived Function Valence
{
    FCNVALENCE_NIL = 0,         // 00:  Niladic function
    FCNVALENCE_MON,             // 01:  Monadic function/derived function
    FCNVALENCE_DYD,             // 02:  Dyadic  ...
    FCNVALENCE_AMB,             // 03:  Ambivalent ...
} FCNVALENCES;


// Defined function header signature
#define DFN_HEADER_SIGNATURE   'SNFD'

// Function header structure
typedef struct tagDFN_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  Defined function header signature
    UINT             Version;       // 04:  Version # of this header
    UINT             DfnType:2,     // 08:  Defined Function Type (see DFNTYPES enum)
                     FcnValence:2;  //      Defined/Derived Function Valence (see FCNVALENCES enum)
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
} DFN_HEADER, *LPDFN_HEADER;


typedef struct tagFH_YYSTYPE
{
    TOKEN  tkToken;             // Token info
    UINT   uStrandLen;          // # elements in this strand
    struct tagFH_YYSTYPE *
           lpYYStrandBase;      // Ptr to this token's strand base
} FH_YYSTYPE, *FH_LPYYSTYPE;    // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1


typedef struct tagFHLOCALVARS       // Function Header Local Vars
{
    HWND         hWndEC;            // Window handle for Edit Control
    HGLOBAL      hGlbTknHdr,        // Tokenized header global memory handle
                 hGlbUndoBuff;      // Undo buffer      ...
    UNION_TOKEN  t2;                // Locked base of hGlbToken
    LPTOKEN      lpStart,           // First available entry after the header
                 lpNext,            // Next  ...
                 lpStop;            // Stopping token
    UINT         tkErrorCharIndex;  // Error char index
    UINT         DfnType:2,         // 08:  Defined Function Type (see enum tagDFNTYPE)
                 FcnValence:2;      //      Defined/Derived Function Valence (see enum tagFCNVALENCE)
    FH_LPYYSTYPE lpYYStrandStart,   // Strand stack start (static)
                 lpYYStrandBase,    // ...          base (dynamic)
                 lpYYStrandNext,    // ...          next token (dynamic)
                 lpYYResult,        // Ptr to result name or list
                 lpYYLftArg,        // ...    left arg name or list
                 lpYYLftOpr,        // ...    left operand name
                 lpYYFcnName,       // ...    function/operator name
                 lpYYRhtOpr,        // ...    right operand name
                 lpYYRhtArg,        // ...    right arg name or list
                 lpYYLocals;        // ...    locals name or list
} FHLOCALVARS, *LPFHLOCALVARS;


//***************************************************************************
//  End of File: dfnhdr.h
//***************************************************************************
