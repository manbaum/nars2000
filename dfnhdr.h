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


// Defined function header signature
#define DFN_HEADER_SIGNATURE   'SNFD'

// Function header structure
typedef struct tagDFN_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  Defined function header signature
    UINT             Version;       // 04:  Version # of this header
    UINT             Type:2,        // 08:  Type:  0 = Unknown
                                    //             1 = Monadic operator
                                    //             2 = Dyadic operator
                                    //             3 = Function
                     FcnValence:2;  //      Function Valence:  0 = Niladic function
                                    //                         1 = Monadic function/derived function
                                    //                         2 = Dyadic  ...
                                    //                         3 = Ambivalent ...
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
                     steRhtOpr;     // 48:  Right ...
    HGLOBAL          hGlbTxtHdr,    // 4C:  Text of function header (APLCHAR) ...
                     hGlbTknHdr,    // 50:  Tokenized function header (TOKEN) ...
                     hGlbUndoBuff;  // 54:  Undo buffer                       ... (may be NULL)
                                    // 58:  Array of function line structures (FCNLINE[nLines])
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
                 lpNext;            // Next  ...
    UINT         tkErrorCharIndex,  // Error char index
                 FcnValence,        // Function valence:  0 = niladic
                                    //                    1 = monadic
                                    //                    2 = dyadic
                                    //                    3 = ambivalent
                 OprValence;        // Operator ...       0 = not an operator (it's a function)
                                    //                    1 = monadic operator
                                    //                    2 = dyadic  ...
                                    //                    3 = unknown
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
