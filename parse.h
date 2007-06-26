//***************************************************************************
//  NARS2000 -- ParseLine Header
//***************************************************************************


typedef struct tagYYSTYPE
{
    // N.B.:  The first item in this struct *MUST* be the TOKEN
    //   because when we pass it as an argument to an LPPRIMFNS
    //   we might need it to be an LPYYSTYPE (for a function strand)
    //   or an LPTOKEN (single function).
    TOKEN   tkToken;        // Token info
    int     TknCount;       // Token count
    int     FcnCount;       // Function count
    UINT    Inuse:1,        // This entry is in use
            Indirect:1,     // Arg is indirect
#ifdef DEBUG
            Rsvd:6,         // Unused as yet
            Index:23,       // Index #
            Flag:1;         // Flag to distinguish YYAlloc from yylex
#else
            Rsvd:30;        // Unused as yet
#endif
    struct tagYYSTYPE *lpYYFcn; // Ptr to function/operator
    union
    {
        struct tagYYSTYPE *
            lpYYStrandBase; // Ptr to this token's strand base
    } unYYSTYPE;
} YYSTYPE, *LPYYSTYPE;      // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1

// Define indices into lpYYStrandStart/Base/Nest[2];
typedef enum tagINDSTRAND
{
    STRAND_VAR = 0,                 // Index for var strands
    STRAND_FCN,                     // Index for function strands
    STRAND_LEN                      // # elements in this enum
} INDSTRAND;

typedef enum tagOBJTYPE
{
    OBJTYPE_VAR = 0,                // Object type is Variable
    OBJTYPE_OP1,                    // ...            Monadic Operator
    OBJTYPE_OP2,                    // ...            Dyadic Operator
    OBJTYPE_FCN,                    // ...            Function (primitive or defined)
} OBJTYPE;

typedef enum tagLOOKAHEAD
{
    LOOKAHEAD_NRM = 0,              // No lookahead
    LOOKAHEAD_SUR,                  // Lookahead for within surrounding parens
    LOOKAHEAD_ADJ,                  // ...           adjacent to brackets
} LOOKAHEAD;

typedef struct tagPLLOCALVARS       // ParseLine Local Vars
{
    HGLOBAL     hGlbToken;          // Global memory handle
    UNION_TOKEN t2;                 // Locked base of hGlbToken
    LPTOKEN     lpStart,            // Ptr to first available entry after the header
                lpNext,             // Ptr to next  ...
                lpStop;             // Ptr to stop token if LookAhead
    UINT        tkErrorCharIndex;   // Error char index
    UINT        InitAhead:1,        // TRUE iff we're to initialize (SOL)
                ObjType:2,          // Object type (see enum tagOBJTYPE)
                LookAhead:2;        // TRUE iff looking for object type within surrounding parens
    LPYYSTYPE   lpYYStrandStart[STRAND_LEN],    // Strand stack start (static)
                lpYYStrandBase [STRAND_LEN],    // ...          base (dynamic)
                lpYYStrandNext [STRAND_LEN];    // ...          next token (dynamic)
    HGLOBAL     hGlbPTD;            // Handle to PerTabData
    HWND        hWndSM;             // Window handle to Session Manager
} PLLOCALVARS, *LPPLLOCALVARS;


//***************************************************************************
//  End of File: parse.h
//***************************************************************************
