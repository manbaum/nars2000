//***************************************************************************
//  NARS2000 -- ParseLine Header
//***************************************************************************


typedef struct tagPL_YYSTYPE        // YYSTYPE for ParseLine
{
    // N.B.:  The first item in this struct *MUST* be the TOKEN
    //   because when we pass it as an argument to an LPPRIMFNS
    //   we might need it to be an LPPL_YYSTYPE (for a function strand)
    //   or an LPTOKEN (single function).
    TOKEN   tkToken;                // 00:  Token info
    UINT    TknCount,               // 14:  Token count
            FcnCount;               // 18:  Function count
    UINT    YYInuse:1,              // 1C:  00000001:  This entry is in use
            YYIndirect:1,           //      00000002:  Arg is indirect
#ifdef DEBUG
            Avail:6,                //      000000FC:  Available bits
            YYIndex:23,             //      7FFFFF00:  Index #
            YYFlag:1;               //      80000000:  Flag to distinguish YYAlloc from yylex
#else
            Avail:30;               //      FFFFFFFC:  Available bits
#endif
    struct tagPL_YYSTYPE *
            lpYYFcn;                // 20:  Ptr to function/operator
    struct tagPL_YYSTYPE *
            lpYYStrandBase;         // 24:  Ptr to this token's strand base
#ifdef DEBUG
    UINT    SILevel;                // 2C:  SI Level (needed for YYResIsEmpty)
                                    // 30:  Length
#else
                                    // 28:  Length
#endif
} PL_YYSTYPE, *LPPL_YYSTYPE;        // Data type of yacc stack

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
    HGLOBAL      hGlbTxtLine,       // 00:  Line text global memory handle
                 hGlbToken;         // 04:  Tokenized line global memory handle
    UNION_TOKEN  t2;                // 08:  Locked base of hGlbToken
    LPTOKEN      lpStart,           // 0C:  Ptr to first available entry after the header
                 lpNext,            // 10:  Ptr to next  ...
                 lpStop;            // 14:  Ptr to stop token if LookAhead
    LPAPLCHAR    lpwszLine;         // 18:  Ptr to line text (zero-terminated)
    UINT         tkErrorCharIndex;  // 1C:  Error char index
    UINT         NameType:3,        // 20:  00000007:  Object type (see enum NAMETYPE)
                 bLookAhead:1,      //      00000008:  TRUE iff looking for object type within surrounding parens
                 ExitType:4,        //      000000F0:  Exit Type (see enum EXIT_TYPES)
                 bRet:1,            //      00000100   Various function results
                 bCtrlBreak:1,      //      00000200   TRUE iff Ctrl-Break pressed
                 Avail:22;          //      FFFFFC00:  Available bits
    LPPL_YYSTYPE lpYYStrandStart[STRAND_LEN],   // 24:  Strand stack start (static)
                 lpYYStrandBase [STRAND_LEN],   // 2C:  ...          base (dynamic)
                 lpYYStrandNext [STRAND_LEN];   // 34:  ...          next token (dynamic)
    HGLOBAL      hGlbPTD;           // 3C:  Handle to PerTabData
    HWND         hWndSM;            // 40:  Window handle to Session Manager
    LPPL_YYSTYPE lpYYStr,           // 44:  Ptr to PL_YYSTYPE strand
                 lpYYStrL,          // 48:  ...               left strand
                 lpYYStrR,          // 4C:  ...               right strand
                 lpYYStrN,          // 50:  ...               name strand
                 lpYYRes,           // 54:  ...               result
                 lpYYFcn,           // 58:  ...               function strand
                 lpYYLst,           // 5C:  ...               list
                 lpYYAxis,          // 60:  ...               axis
                 lpYYOp1,           // 64:  ...               monadic operator
                 lpYYOp2;           // 68:  ...               dyadic  ...
    struct tagPLLOCALVARS
                *lpPLPrev;          // 70:  Ptr to previous PLLOCALVARS struct
                                    //      in thread creation order (NULL = none)
                                    // 74:  Length
} PLLOCALVARS, *LPPLLOCALVARS;


//***************************************************************************
//  End of File: parse.h
//***************************************************************************
