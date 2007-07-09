//***************************************************************************
//  NARS2000 -- ParseLine Header
//***************************************************************************


typedef struct tagYYSTYPE
{
    // N.B.:  The first item in this struct *MUST* be the TOKEN
    //   because when we pass it as an argument to an LPPRIMFNS
    //   we might need it to be an LPYYSTYPE (for a function strand)
    //   or an LPTOKEN (single function).
    TOKEN   tkToken;        // 00:  Token info
    UINT    TknCount,       // 14:  Token count
            FcnCount;       // 18:  Function count
    UINT    YYInuse:1,      // 1C:  00000001:  This entry is in use
            YYIndirect:1,   //      00000002:  Arg is indirect
#ifdef DEBUG
            Avail:6,        //      000000FC:  Available bits
            YYIndex:23,     //      7FFFFF00:  Index #
            YYFlag:1;       //      80000000:  Flag to distinguish YYAlloc from yylex
#else
            Avail:30;       //      FFFFFFFC:  Available bits
#endif
    struct tagYYSTYPE *lpYYFcn; // 20:  Ptr to function/operator
    union
    {
        struct tagYYSTYPE *
            lpYYStrandBase; // 24:  Ptr to this token's strand base
    } unYYSTYPE;
} YYSTYPE, *LPYYSTYPE;      // Data type of yacc stack

#define YYSTYPE_IS_DECLARED 1

// Define indices into lpYYStrandStart/Base/Nest[STRAND_LEN];
typedef enum tagINDSTRAND
{
    STRAND_VAR = 0,                 // 00:  Index for var strands
    STRAND_FCN,                     // 01:  Index for function strands
    STRAND_LEN                      // 02:  # elements in this enum
} INDSTRAND;

typedef struct tagPLLOCALVARS       // ParseLine Local Vars
{
    HGLOBAL     hGlbToken;          // 00:  Global memory handle
    UNION_TOKEN t2;                 // 04:  Locked base of hGlbToken
    LPTOKEN     lpStart,            // 08:  Ptr to first available entry after the header
                lpNext,             // 0C:  Ptr to next  ...
                lpStop;             // 10:  Ptr to stop token if LookAhead
    LPAPLCHAR   lpwszLine;          // 14:  Ptr to line text (zero-terminated)
    UINT        tkErrorCharIndex;   // 18:  Error char index
    UINT        NameType:3,         // 1C:  00000007:  Object type (see enum NAMETYPE)
                bLookAhead:1,       //      00000008:  TRUE iff looking for object type within surrounding parens
                Avail:28;           //      FFFFFFF0:  Available bits
    LPYYSTYPE   lpYYStrandStart[STRAND_LEN],    // 20:  Strand stack start (static)
                lpYYStrandBase [STRAND_LEN],    // 28:  ...          base (dynamic)
                lpYYStrandNext [STRAND_LEN];    // 30:  ...          next token (dynamic)
    HGLOBAL     hGlbPTD;            // 38:  Handle to PerTabData
    HWND        hWndSM;             // 3C:  Window handle to Session Manager
                                    // 40:  Length
} PLLOCALVARS, *LPPLLOCALVARS;


//***************************************************************************
//  End of File: parse.h
//***************************************************************************
