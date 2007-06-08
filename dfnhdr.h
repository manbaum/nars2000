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
    HEADER_SIGNATURE Sign;          // 00:  Defined function header signature
    UINT             Version,       // 04:  Version # of this header
                     offFcnLines;   // 08:  Offset to start of function lines (FCNLINE[nLines])
    UINT             Type:2,        // 0C:  Type:  0 = Unknown
                                    //             1 = Monadic operator
                                    //             2 = Dyadic operator
                                    //             3 = Function
                     FcnValence:2;  //      Function Valence:  0 = Niladic function
                                    //                         1 = Monadic function/derived function
                                    //                         2 = Dyadic  ...
                                    //                         3 = Ambivalent ...
    UINT             RefCnt,        // 10:  Reference count
                     nLines,        // 14:  # lines in the function (not counting the header)
                     nProtoLine,    // 18:  Line # of the []PROTOTYPE label
                     nInverseLine,  // 1C:  Line # of the []INVERSE label
                     nSingletonLine;// 20:  Line # of the []SINGLETON label
    HGLOBAL          hGlbTxtHdr,    // 24:  Text of function header (APLCHAR) ...
                     hGlbTknHdr,    // 28:  Tokenized function header (TOKEN) ...
                     hGlbUndoBuff;  // 2C:  Undo buffer                       ... (may be NULL)
                                    // 30:  Array of function line structures (FCNLINE[nLines])
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
