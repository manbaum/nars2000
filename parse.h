//***************************************************************************
//  NARS2000 -- Parse Header
//***************************************************************************


typedef struct tagYYSTYPE
{
    TOKEN   tkToken;        // Token info
    int     TknCount;       // Token count
    int     FcnCount;       // Function count
    BOOL    Indirect;       // Arg is indirect
    struct tagYYSTYPE *lpYYFcn; // Ptr to function/operator
    union
    {
        struct tagYYSTYPE *
            lpYYStrandBase; // Ptr to this token's strand base
    } unYYSTYPE;
} YYSTYPE, *LPYYSTYPE;      // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1

// Define indices into lpYYStrandStart[2];
typedef enum tagINDSTRAND
{
    VARSTRAND = 0,
    FCNSTRAND,
} INDSTRAND;

typedef struct tagPLLOCALVARS       // ParseLine Local Vars
{
    HGLOBAL     hGlbToken;          // Global memory handle
    UNION_TOKEN t2;                 // Locked base of hGlbToken
    LPTOKEN     lpStart,            // First available entry after the header
                lpNext;             // Next  ...
    UINT        tkErrorCharIndex;   // Error char index
    LPYYSTYPE   lpYYStrandStart[2], // Strand stack start (static)
                lpYYStrandBase[2],  // ...          base (dynamic)
                lpYYStrandNext[2];  // ...          next token (dynamic)
} PLLOCALVARS, *LPPLLOCALVARS;


//***************************************************************************
//  End of File: parse.h
//***************************************************************************
