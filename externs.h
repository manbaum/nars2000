//***************************************************************************
//  NARS2000 -- Extern Variables
//***************************************************************************

// Default definitions
#define DEF_TCFONTNAME      "Georgia"           // Or "SImPL"
#define DEF_SMFONTNAME      "APL385 Unicode"
#define DEF_FEFONTNAME      "APL385 Unicode"
#define DEF_MEFONTNAME      "APL385 Unicode"
#define DEF_VEFONTNAME      "APL385 Unicode"

#define DEF_TCLOGFONT       0,0,0,0,FW_BOLD  ,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,VARIABLE_PITCH | FF_ROMAN ,DEF_TCFONTNAME
#define DEF_SMLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_SMFONTNAME
#define DEF_FELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_FEFONTNAME
#define DEF_MELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_MEFONTNAME
#define DEF_VELOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,DEF_VEFONTNAME

#define DEF_TCPTSIZE        8           // Point size for TC font
#define DEF_SMPTSIZE       13           // ...            SM ...
#define DEF_FEPTSIZE       13           // ...            FE ...
#define DEF_MEPTSIZE       13           // ...            ME ...
#define DEF_VEPTSIZE       13           // ...            VE ...

#define DEF_INDENT          6           // Prompt indent
#define DEF_TABS            4           // Tab insertion
#define DEF_CURWID_INS      5           // Cursor width for insert mode
#define DEF_CURWID_REP      2           // ...              replace ...
#define DEF_HISTLINES    3000           // # lines in history buffer
#define DEF_CURLINE_MAXLEN   1024       // Maximum current line length
#define DEF_CURLINE_MAXSIZE  ((DEF_CURLINE_MAXLEN + 1) * sizeof (WCHAR)) // Maximum line size in bytes
#define DEF_CURLINE_INITSIZE DEF_CURLINE_MAXSIZE
#define DEF_TEXT_FG_COLOR   COLOR_RED
#define DEF_TEXT_BG_COLOR   COLOR_WHITE
#define DEF_NUMALP_MAXSIZE  64*1024*sizeof (char)   // Max size for lpszNumAlp
#define DEF_NUMALP_INITSIZE 64*1024*sizeof (char)   // Initial ...
#define DEF_STRING_MAXSIZE  64*1024*sizeof (WCHAR)  // Max size for lpwszString
#define DEF_STRING_INITSIZE 64*1024*sizeof (WCHAR)  // Initial ...
#define DEF_TOKENSTACK_MAXSIZE 64*1024*sizeof (TOKEN)   // Maximum size of token stack
#define DEF_TOKENSTACK_INITSIZE 64*1024*sizeof (TOKEN)  // Initial ...

// Global Options
#define DEF_NEWTABONCLEAR   TRUE
#define DEF_NEWTABONLOAD    TRUE

#include <commctrl.h>
#include "datatype.h"
#include "primfns.h"
#include "symtab.h"
#include "tokens.h"
#include "parse.h"
#include "Unicode.h"

// Define variables which are also used in the per tab structure
#include "pertabdefs.h"
#include "primspec.h"


#ifdef DEFINE_VARS
#define EXTERN
#else
#define EXTERN extern
#endif

//***************************************************************************
//  Below this point, define variables which do not need to be saved
//    in the per tab structure
//***************************************************************************

//***************************************************************************
//  Default global values of system variables -- these values
//    are used to set the variables in a CLEAR WS.
//***************************************************************************

EXTERN
HGLOBAL  hGlbQuadALX_CWS    ,           // []ALX    ([]dm)
         hGlbQuadELX_CWS    ,           // []ELX    ([]dm)
         hGlbQuadLX_CWS     ,           // []LX     ("")
         hGlbQuadSA_CWS     ,           // []SA     ("")
         hGlbQuadWSID_CWS   ,           // []WSID   ("")
         hGlbQuadPR_CWS     ;           // []PR     ("") (When an empty vector)
EXTERN
APLFLOAT fQuadCT_CWS        ;           // []CT

EXTERN
APLBOOL  bQuadIO_CWS        ,           // []IO
         bQuadxSA_CWS       ;           // []SA (in its index form)

EXTERN
APLUINT  uQuadPP_CWS        ,           // []PP
         uQuadPW_CWS        ,           // []PW
         uQuadRL_CWS        ;           // []RL

EXTERN
APLCHAR  cQuadPR_CWS        ;           // []PR     (' ') (When a char scalar)

EXTERN
LPSYMENTRY lpSymQuadRL;                 // Ptr to STE for QuadRL

//***************************************************************************
//  Application values
//***************************************************************************

EXTERN
HINSTANCE _hInstance;                   // Global instance handle

EXTERN
DWORD dwMainThreadId;                   // Thread ID of the main application

EXTERN
HANDLE hAccel;                          // Keyboard accelerators

#ifdef DEBUG
#define  APPEND_DEBUG  " (DEBUG)"
#define LAPPEND_DEBUG L" (DEBUG)"
#else
#define  APPEND_DEBUG
#define LAPPEND_DEBUG
#endif

EXTERN
char pszAppName[]                       // Application name for MessageBox
#ifdef DEFINE_VALUES
 = "NARS2000" APPEND_DEBUG
#endif
,
     szAppDPFE  [_MAX_PATH],            // .EXE drive, path, filename.ext
     szHlpDPFE  [_MAX_PATH],            // .HLP ...
     szInitDir  [_MAX_PATH],            // Initial directory for File Open & Save
     szOpenFile [_MAX_PATH];            // Save area for multiple files to open


//***************************************************************************
// Thread Local Storage indices
//***************************************************************************

EXTERN
DWORD dwTlsType,                        // Thread type (e.g.
                                        //   'MF' for Master Frame,
                                        //   'TC' for Tab Control,
                                        //   'PL' for ParseLine,
                                        //   etc.)
      dwTlsSemaphore,                   // Handle to semaphore for PL thread only
      dwTlsLocalVars;                   // Ptr to lpplLocalVars for PL thread only

//***************************************************************************
// Temporary storage
//***************************************************************************

EXTERN
LPCHAR lpszTemp;                        // Used for temporary char storage

EXTERN
LPWCHAR lpwszTemp,                      // Used for temporary WCHAR storage
        lpwszFormat;                    // Used for formatting

#ifdef DEBUG
EXTERN
LPCHAR lpszDebug;                       // Used for temporary storage of char
                                        //   debug output
EXTERN
LPWCHAR lpwszDebug;                     // Used for temporary storage of WCHAR
                                        //   debug output
#endif

//***************************************************************************
//  Primitive function and operator tables
//***************************************************************************

EXTERN
LPPRIMFNS PrimFnsTab[256];              // The jump table for all primitive functions

EXTERN
LPPRIMFNS PrimProtoFnsTab[256];         // The jump table for all primitive functions/operator prototypes

EXTERN
LPPRIMSPEC PrimSpecTab[256];            // The table of corresponding LPPRIMSPECs
                                        //   for all of the primitive scalar functions
typedef enum tagFBFNS                   // Fast Boolean functions
{   PF_IND_UNK = 0  ,                   // 00 = No entry so we can catch this as an error
    PF_IND_AND      ,                   // 01 = Index for "and"   ...
    PF_IND_OR       ,                   // 02 = ...       "or"    ...
    PF_IND_NAND     ,                   // 03 = ...       "nand"  ...
    PF_IND_NOR      ,                   // 04 = ...       "nor"   ...
    PF_IND_LESS     ,                   // 05 = ...       "less than" ...
    PF_IND_LESSEQ   ,                   // 06 = ...       "less than or equal" ...
    PF_IND_EQUAL    ,                   // 07 = ...       "equal" ...
    PF_IND_MORE     ,                   // 08 = ...       "more than" ...
    PF_IND_MOREEQ   ,                   // 09 = ...       "more than or equal" ...
    PF_IND_NOTEQUAL ,                   // 0A = ...       "not equal" ...
    PF_IND_MIN      ,                   // 0B = ...       "min"   ...
    PF_IND_MAX      ,                   // 0C = ...       "max"   ...
    PF_IND_PLUS     ,                   // 0D = ...       "plus"  ...
    PF_IND_MINUS    ,                   // 0E = ...       "minus" ...
    PF_IND_DIVIDE   ,                   // 0F = ...       "divide" ...
    PF_IND_NEXT                         // 10 = Next available index
                                        // If another entry is made, be sure
                                        //   to increase Index:4 to Index:5
                                        //   in tagPRIMFLAGS
} FBFNS;

EXTERN
APLFLOAT PrimIdent[PF_IND_NEXT];        // Primitive scalar function identity elements
                                        //   in the same order as enum FBFNS

typedef struct tagPRIMFLAGS
{
    WORD Index:4,                       // Function index (see enum FBFNS)
         Available:7,                   // Available flag bits
         DydScalar:1,                   // Function is scalar dyadic
         MonScalar:1,                   // ...         ...    monadic
         Alter:1,                       // ...         alternating
         Assoc:1,                       // ...         associative
         FastBool:1;                    // Boolean function w/reduction & scan can be sped up
} PRIMFLAGS, *LPPRIMFLAGS;

EXTERN
PRIMFLAGS PrimFlags[256];               // The flag tables for all primitive functions/operators


//***************************************************************************
//  Fast Boolean Reduction and Scan tables
//***************************************************************************

typedef void (FASTBOOLFCN) (APLSTYPE  aplTypeRht,       // Right arg storage type
                            LPVOID    lpMemRht,         // Ptr to right arg global memory
                            LPVOID    lpMemRes,         // Ptr to result    ...
                            APLUINT   uDimLo,           // Product of dimensions below axis
                            APLUINT   uDimAxRht,        // Length of right arg axis dimension
                            APLUINT   uDimAxRes,        // ...       result    ...
                            LPYYSTYPE lpYYFcnStrOpr);   // Ptr to operator function strand
typedef FASTBOOLFCN *LPFASTBOOLFCN;

typedef struct tagFASTBOOLFNS
{
    LPFASTBOOLFCN lpReduction;          // Ptr to Fast Boolean reduction routine
    LPFASTBOOLFCN lpScan;               // ...                 scan      ...
} FASTBOOLFNS, *LPFASTBOOLFNS;

// This array translates a byte index into the sum of its bits
EXTERN
UCHAR FastBoolRed[256]
#ifdef DEFINE_VALUES
 = {0+0+0+0+0+0+0+0,   // 00000000
    0+0+0+0+0+0+0+1,   // 00000001
    0+0+0+0+0+0+1+0,   // 00000010
    0+0+0+0+0+0+1+1,   // 00000011
    0+0+0+0+0+1+0+0,   // 00000100
    0+0+0+0+0+1+0+1,   // 00000101
    0+0+0+0+0+1+1+0,   // 00000110
    0+0+0+0+0+1+1+1,   // 00000111

    0+0+0+0+1+0+0+0,   // 00001000
    0+0+0+0+1+0+0+1,   // 00001001
    0+0+0+0+1+0+1+0,   // 00001010
    0+0+0+0+1+0+1+1,   // 00001011
    0+0+0+0+1+1+0+0,   // 00001100
    0+0+0+0+1+1+0+1,   // 00001101
    0+0+0+0+1+1+1+0,   // 00001110
    0+0+0+0+1+1+1+1,   // 00001111

    0+0+0+1+0+0+0+0,   // 00010000
    0+0+0+1+0+0+0+1,   // 00010001
    0+0+0+1+0+0+1+0,   // 00010010
    0+0+0+1+0+0+1+1,   // 00010011
    0+0+0+1+0+1+0+0,   // 00010100
    0+0+0+1+0+1+0+1,   // 00010101
    0+0+0+1+0+1+1+0,   // 00010110
    0+0+0+1+0+1+1+1,   // 00010111

    0+0+0+1+1+0+0+0,   // 00011000
    0+0+0+1+1+0+0+1,   // 00011001
    0+0+0+1+1+0+1+0,   // 00011010
    0+0+0+1+1+0+1+1,   // 00011011
    0+0+0+1+1+1+0+0,   // 00011100
    0+0+0+1+1+1+0+1,   // 00011101
    0+0+0+1+1+1+1+0,   // 00011110
    0+0+0+1+1+1+1+1,   // 00011111

    0+0+1+0+0+0+0+0,   // 00100000
    0+0+1+0+0+0+0+1,   // 00100001
    0+0+1+0+0+0+1+0,   // 00100010
    0+0+1+0+0+0+1+1,   // 00100011
    0+0+1+0+0+1+0+0,   // 00100100
    0+0+1+0+0+1+0+1,   // 00100101
    0+0+1+0+0+1+1+0,   // 00100110
    0+0+1+0+0+1+1+1,   // 00100111

    0+0+1+0+1+0+0+0,   // 00101000
    0+0+1+0+1+0+0+1,   // 00101001
    0+0+1+0+1+0+1+0,   // 00101010
    0+0+1+0+1+0+1+1,   // 00101011
    0+0+1+0+1+1+0+0,   // 00101100
    0+0+1+0+1+1+0+1,   // 00101101
    0+0+1+0+1+1+1+0,   // 00101110
    0+0+1+0+1+1+1+1,   // 00101111

    0+0+1+1+0+0+0+0,   // 00110000
    0+0+1+1+0+0+0+1,   // 00110001
    0+0+1+1+0+0+1+0,   // 00110010
    0+0+1+1+0+0+1+1,   // 00110011
    0+0+1+1+0+1+0+0,   // 00110100
    0+0+1+1+0+1+0+1,   // 00110101
    0+0+1+1+0+1+1+0,   // 00110110
    0+0+1+1+0+1+1+1,   // 00110111

    0+0+1+1+1+0+0+0,   // 00111000
    0+0+1+1+1+0+0+1,   // 00111001
    0+0+1+1+1+0+1+0,   // 00111010
    0+0+1+1+1+0+1+1,   // 00111011
    0+0+1+1+1+1+0+0,   // 00111100
    0+0+1+1+1+1+0+1,   // 00111101
    0+0+1+1+1+1+1+0,   // 00111110
    0+0+1+1+1+1+1+1,   // 00111111

    0+1+0+0+0+0+0+0,   // 01000000
    0+1+0+0+0+0+0+1,   // 01000001
    0+1+0+0+0+0+1+0,   // 01000010
    0+1+0+0+0+0+1+1,   // 01000011
    0+1+0+0+0+1+0+0,   // 01000100
    0+1+0+0+0+1+0+1,   // 01000101
    0+1+0+0+0+1+1+0,   // 01000110
    0+1+0+0+0+1+1+1,   // 01000111

    0+1+0+0+1+0+0+0,   // 01001000
    0+1+0+0+1+0+0+1,   // 01001001
    0+1+0+0+1+0+1+0,   // 01001010
    0+1+0+0+1+0+1+1,   // 01001011
    0+1+0+0+1+1+0+0,   // 01001100
    0+1+0+0+1+1+0+1,   // 01001101
    0+1+0+0+1+1+1+0,   // 01001110
    0+1+0+0+1+1+1+1,   // 01001111

    0+1+0+1+0+0+0+0,   // 01010000
    0+1+0+1+0+0+0+1,   // 01010001
    0+1+0+1+0+0+1+0,   // 01010010
    0+1+0+1+0+0+1+1,   // 01010011
    0+1+0+1+0+1+0+0,   // 01010100
    0+1+0+1+0+1+0+1,   // 01010101
    0+1+0+1+0+1+1+0,   // 01010110
    0+1+0+1+0+1+1+1,   // 01010111

    0+1+0+1+1+0+0+0,   // 01011000
    0+1+0+1+1+0+0+1,   // 01011001
    0+1+0+1+1+0+1+0,   // 01011010
    0+1+0+1+1+0+1+1,   // 01011011
    0+1+0+1+1+1+0+0,   // 01011100
    0+1+0+1+1+1+0+1,   // 01011101
    0+1+0+1+1+1+1+0,   // 01011110
    0+1+0+1+1+1+1+1,   // 01011111

    0+1+1+0+0+0+0+0,   // 01100000
    0+1+1+0+0+0+0+1,   // 01100001
    0+1+1+0+0+0+1+0,   // 01100010
    0+1+1+0+0+0+1+1,   // 01100011
    0+1+1+0+0+1+0+0,   // 01100100
    0+1+1+0+0+1+0+1,   // 01100101
    0+1+1+0+0+1+1+0,   // 01100110
    0+1+1+0+0+1+1+1,   // 01100111

    0+1+1+0+1+0+0+0,   // 01101000
    0+1+1+0+1+0+0+1,   // 01101001
    0+1+1+0+1+0+1+0,   // 01101010
    0+1+1+0+1+0+1+1,   // 01101011
    0+1+1+0+1+1+0+0,   // 01101100
    0+1+1+0+1+1+0+1,   // 01101101
    0+1+1+0+1+1+1+0,   // 01101110
    0+1+1+0+1+1+1+1,   // 01101111

    0+1+1+1+0+0+0+0,   // 01110000
    0+1+1+1+0+0+0+1,   // 01110001
    0+1+1+1+0+0+1+0,   // 01110010
    0+1+1+1+0+0+1+1,   // 01110011
    0+1+1+1+0+1+0+0,   // 01110100
    0+1+1+1+0+1+0+1,   // 01110101
    0+1+1+1+0+1+1+0,   // 01110110
    0+1+1+1+0+1+1+1,   // 01110111

    0+1+1+1+1+0+0+0,   // 01111000
    0+1+1+1+1+0+0+1,   // 01111001
    0+1+1+1+1+0+1+0,   // 01111010
    0+1+1+1+1+0+1+1,   // 01111011
    0+1+1+1+1+1+0+0,   // 01111100
    0+1+1+1+1+1+0+1,   // 01111101
    0+1+1+1+1+1+1+0,   // 01111110
    0+1+1+1+1+1+1+1,   // 01111111

    1+0+0+0+0+0+0+0,   // 10000000
    1+0+0+0+0+0+0+1,   // 10000001
    1+0+0+0+0+0+1+0,   // 10000010
    1+0+0+0+0+0+1+1,   // 10000011
    1+0+0+0+0+1+0+0,   // 10000100
    1+0+0+0+0+1+0+1,   // 10000101
    1+0+0+0+0+1+1+0,   // 10000110
    1+0+0+0+0+1+1+1,   // 10000111

    1+0+0+0+1+0+0+0,   // 10001000
    1+0+0+0+1+0+0+1,   // 10001001
    1+0+0+0+1+0+1+0,   // 10001010
    1+0+0+0+1+0+1+1,   // 10001011
    1+0+0+0+1+1+0+0,   // 10001100
    1+0+0+0+1+1+0+1,   // 10001101
    1+0+0+0+1+1+1+0,   // 10001110
    1+0+0+0+1+1+1+1,   // 10001111

    1+0+0+1+0+0+0+0,   // 10010000
    1+0+0+1+0+0+0+1,   // 10010001
    1+0+0+1+0+0+1+0,   // 10010010
    1+0+0+1+0+0+1+1,   // 10010011
    1+0+0+1+0+1+0+0,   // 10010100
    1+0+0+1+0+1+0+1,   // 10010101
    1+0+0+1+0+1+1+0,   // 10010110
    1+0+0+1+0+1+1+1,   // 10010111

    1+0+0+1+1+0+0+0,   // 10011000
    1+0+0+1+1+0+0+1,   // 10011001
    1+0+0+1+1+0+1+0,   // 10011010
    1+0+0+1+1+0+1+1,   // 10011011
    1+0+0+1+1+1+0+0,   // 10011100
    1+0+0+1+1+1+0+1,   // 10011101
    1+0+0+1+1+1+1+0,   // 10011110
    1+0+0+1+1+1+1+1,   // 10011111

    1+0+1+0+0+0+0+0,   // 10100000
    1+0+1+0+0+0+0+1,   // 10100001
    1+0+1+0+0+0+1+0,   // 10100010
    1+0+1+0+0+0+1+1,   // 10100011
    1+0+1+0+0+1+0+0,   // 10100100
    1+0+1+0+0+1+0+1,   // 10100101
    1+0+1+0+0+1+1+0,   // 10100110
    1+0+1+0+0+1+1+1,   // 10100111

    1+0+1+0+1+0+0+0,   // 10101000
    1+0+1+0+1+0+0+1,   // 10101001
    1+0+1+0+1+0+1+0,   // 10101010
    1+0+1+0+1+0+1+1,   // 10101011
    1+0+1+0+1+1+0+0,   // 10101100
    1+0+1+0+1+1+0+1,   // 10101101
    1+0+1+0+1+1+1+0,   // 10101110
    1+0+1+0+1+1+1+1,   // 10101111

    1+0+1+1+0+0+0+0,   // 10110000
    1+0+1+1+0+0+0+1,   // 10110001
    1+0+1+1+0+0+1+0,   // 10110010
    1+0+1+1+0+0+1+1,   // 10110011
    1+0+1+1+0+1+0+0,   // 10110100
    1+0+1+1+0+1+0+1,   // 10110101
    1+0+1+1+0+1+1+0,   // 10110110
    1+0+1+1+0+1+1+1,   // 10110111

    1+0+1+1+1+0+0+0,   // 10111000
    1+0+1+1+1+0+0+1,   // 10111001
    1+0+1+1+1+0+1+0,   // 10111010
    1+0+1+1+1+0+1+1,   // 10111011
    1+0+1+1+1+1+0+0,   // 10111100
    1+0+1+1+1+1+0+1,   // 10111101
    1+0+1+1+1+1+1+0,   // 10111110
    1+0+1+1+1+1+1+1,   // 10111111

    1+1+0+0+0+0+0+0,   // 11000000
    1+1+0+0+0+0+0+1,   // 11000001
    1+1+0+0+0+0+1+0,   // 11000010
    1+1+0+0+0+0+1+1,   // 11000011
    1+1+0+0+0+1+0+0,   // 11000100
    1+1+0+0+0+1+0+1,   // 11000101
    1+1+0+0+0+1+1+0,   // 11000110
    1+1+0+0+0+1+1+1,   // 11000111

    1+1+0+0+1+0+0+0,   // 11001000
    1+1+0+0+1+0+0+1,   // 11001001
    1+1+0+0+1+0+1+0,   // 11001010
    1+1+0+0+1+0+1+1,   // 11001011
    1+1+0+0+1+1+0+0,   // 11001100
    1+1+0+0+1+1+0+1,   // 11001101
    1+1+0+0+1+1+1+0,   // 11001110
    1+1+0+0+1+1+1+1,   // 11001111

    1+1+0+1+0+0+0+0,   // 11010000
    1+1+0+1+0+0+0+1,   // 11010001
    1+1+0+1+0+0+1+0,   // 11010010
    1+1+0+1+0+0+1+1,   // 11010011
    1+1+0+1+0+1+0+0,   // 11010100
    1+1+0+1+0+1+0+1,   // 11010101
    1+1+0+1+0+1+1+0,   // 11010110
    1+1+0+1+0+1+1+1,   // 11010111

    1+1+0+1+1+0+0+0,   // 11011000
    1+1+0+1+1+0+0+1,   // 11011001
    1+1+0+1+1+0+1+0,   // 11011010
    1+1+0+1+1+0+1+1,   // 11011011
    1+1+0+1+1+1+0+0,   // 11011100
    1+1+0+1+1+1+0+1,   // 11011101
    1+1+0+1+1+1+1+0,   // 11011110
    1+1+0+1+1+1+1+1,   // 11011111

    1+1+1+0+0+0+0+0,   // 11100000
    1+1+1+0+0+0+0+1,   // 11100001
    1+1+1+0+0+0+1+0,   // 11100010
    1+1+1+0+0+0+1+1,   // 11100011
    1+1+1+0+0+1+0+0,   // 11100100
    1+1+1+0+0+1+0+1,   // 11100101
    1+1+1+0+0+1+1+0,   // 11100110
    1+1+1+0+0+1+1+1,   // 11100111

    1+1+1+0+1+0+0+0,   // 11101000
    1+1+1+0+1+0+0+1,   // 11101001
    1+1+1+0+1+0+1+0,   // 11101010
    1+1+1+0+1+0+1+1,   // 11101011
    1+1+1+0+1+1+0+0,   // 11101100
    1+1+1+0+1+1+0+1,   // 11101101
    1+1+1+0+1+1+1+0,   // 11101110
    1+1+1+0+1+1+1+1,   // 11101111

    1+1+1+1+0+0+0+0,   // 11110000
    1+1+1+1+0+0+0+1,   // 11110001
    1+1+1+1+0+0+1+0,   // 11110010
    1+1+1+1+0+0+1+1,   // 11110011
    1+1+1+1+0+1+0+0,   // 11110100
    1+1+1+1+0+1+0+1,   // 11110101
    1+1+1+1+0+1+1+0,   // 11110110
    1+1+1+1+0+1+1+1,   // 11110111

    1+1+1+1+1+0+0+0,   // 11111000
    1+1+1+1+1+0+0+1,   // 11111001
    1+1+1+1+1+0+1+0,   // 11111010
    1+1+1+1+1+0+1+1,   // 11111011
    1+1+1+1+1+1+0+0,   // 11111100
    1+1+1+1+1+1+0+1,   // 11111101
    1+1+1+1+1+1+1+0,   // 11111110
    1+1+1+1+1+1+1+1,   // 11111111
   }
#endif
;


//***************************************************************************
//
//***************************************************************************

// Default six-space indent
EXTERN
WCHAR wszIndent[DEF_INDENT + 1]
#ifdef DEFINE_VALUES
 = {L' ',L' ',L' ',L' ',L' ',L' ',L'\0'}
#endif
;

EXTERN
HGLOBAL hGlbCurTab;                     // Global handle of current tab

EXTERN
HWND hWndTC,                            // Global Tab Control window handle
     hWndMF,                            // ...    Master Frame ...
     hWndTT;                            // ...    ToolTip      ...

EXTERN
HGLOBAL hGlbZilde,
        hGlbQuadDM,
        hGlbMTChar,
        hGlbSAEmpty,
        hGlbSAClear,
        hGlbSAError,
        hGlbSAExit,
        hGlbSAOff,
        hGlbQuadWSID_CWS;

EXTERN
APLFLOAT PosInfinity,                   // Positive infinity
         NegInfinity,                   // Negative ...
         Float2Pow53;                   // 2*53 in floating point

#ifndef ENUMS_DEFINED
typedef enum tagSYSVAR_VALID
{
    SYSVAR_VALID_ALX = 0,               //  0:
    SYSVAR_VALID_CT  ,                  //  1:
    SYSVAR_VALID_DF  ,                  //  2:
    SYSVAR_VALID_ELX ,                  //  3:
    SYSVAR_VALID_IF  ,                  //  4:
    SYSVAR_VALID_IO  ,                  //  5:
    SYSVAR_VALID_LX  ,                  //  6:
    SYSVAR_VALID_PP  ,                  //  7:
    SYSVAR_VALID_PR  ,                  //  8:
    SYSVAR_VALID_PW  ,                  //  9:
    SYSVAR_VALID_RL  ,                  // 10:
    SYSVAR_VALID_SA  ,                  // 11:
    SYSVAR_VALID_WSID,                  // 12:

    SYSVAR_VALID_LENGTH                 // 13: Length of the enum *MUST* be last
} SYSVAR_VALID;
#endif

EXTERN
// Use as in:  (*aSysVarValid[SYSVAR_VALID_IO]) (lptkName, lptkExpr);
BOOL (*aSysVarValid[SYSVAR_VALID_LENGTH]) (LPTOKEN, LPTOKEN);

EXTERN
char lpszVersion[]
#ifdef DEFINE_VALUES
 = "NARS2000\nVersion %s"
#endif
;

EXTERN
int MFSizeState                         // Size state for MF (SIZE_xxx)
#ifdef DEFINE_VALUES
 = SIZE_RESTORED
#endif
;

EXTERN
POINT MFPosCtr;                         // X- and Y- center of Master Frame Window position

EXTERN
SIZE  MFSize;                           // Size of Master Frame Window window rectangle

EXTERN
int iLogPixelsY;                        // # logical pixels/inch in screen Y-dimension

EXTERN
HBITMAP hBitMapLineCont;                // Bitmap for the line continuation char

EXTERN
int     iLCWidth;                       // Width of the line continuation column

EXTERN
BITMAP  bmLineCont;                     // Bitmap metrics for the line continuation char

// FONTS
EXTERN
LOGFONT lfSM                            // LOGFONT for the SM
#ifdef DEFINE_VALUES
 = {DEF_SMLOGFONT}
#endif
,
        lfTC                            // LOGFONT for the TC
#ifdef DEFINE_VALUES
 = {DEF_TCLOGFONT}
#endif
,
        lfFE                            // LOGFONT for the FE
#ifdef DEFINE_VALUES
 = {DEF_FELOGFONT}
#endif
,
        lfME                            // LOGFONT for the ME
#ifdef DEFINE_VALUES
 = {DEF_MELOGFONT}
#endif
,
        lfVE                            // LOGFONT for the VE
#ifdef DEFINE_VALUES
 = {DEF_VELOGFONT}
#endif
;

;

EXTERN
HFONT hFontTC,                          // Handle to font for the TC
      hFontSM,                          // ...                    SM
      hFontFE,                          // ...                    FE
      hFontME,                          // ...                    ME
      hFontVE;                          // ...                    VE

EXTERN
CHOOSEFONT cfTC,                        // Global for ChooseFont for the TC
           cfSM,                        // ...                           SM
           cfFE,                        // ...                           FE
           cfME,                        // ...                           ME
           cfVE;                        // ...                           VE

EXTERN
TEXTMETRIC tmTC,                        // Global for TEXTMETRIC for the TC
           tmSM,                        // ...                           SM
           tmFE,                        // ...                           FE
           tmME,                        // ...                           ME
           tmVE;                        // ...                           VE

EXTERN
long cxAveCharTC, cyAveCharTC,          // Size of an average character in the TC font
     cxAveCharSM, cyAveCharSM,          // ...                                 SM ...
     cxAveCharFE, cyAveCharFE,          // ...                                 FE ...
     cxAveCharME, cyAveCharME,          // ...                                 ME ...
     cxAveCharVE, cyAveCharVE;          // ...                                 VE ...

EXTERN
int iOverTab                            // Index of the tab the mouse is over
#ifdef DEFINE_VALUES
 = -1
#endif
;

EXTERN
WNDPROC lpfnOldTabCtrlWndProc;          // Save area for old Tab Control procedure

#ifdef DEBUG_ODS
EXTERN
char DebugFile[];

EXTERN
HANDLE DebugHandle;
#endif

// Define global options
EXTERN
BOOL bNewTabOnClear
#ifdef DEFINE_VALUES
 = DEF_NEWTABONCLEAR
#endif
,
    bNewTabOnLoad
#ifdef DEFINE_VALUES
 = DEF_NEWTABONLOAD
#endif
;

EXTERN
char pszNoInsertTCTab[]
#ifdef DEFINE_VALUES
 = "Unable to create a new Tab"
#endif
,
     pszNoCreateFEEditCtrl[]
#ifdef DEFINE_VALUES
 = "Unable to create the Edit Control in the Function Editor"
#endif
,
     pszNoCreateSMEditCtrl[]
#ifdef DEFINE_VALUES
 = "Unable to create the Edit Control in the Session Manager"
#endif
,
     pszNoCreateMCWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create MDI Client window"
#endif
,
     pszNoCreateSMWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create Session Manager window"
#endif
#ifdef DEBUG
,
     pszNoCreateDBWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create Debugger window"
#endif
#endif
,
     pszNoCreateFEWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create Function Editor window"
#endif
,
     pszNoCreateMEWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create Matrix Editor window"
#endif
,
     pszNoCreateVEWnd[]
#ifdef DEFINE_VALUES
 = "Unable to create Vector Editor window"
#endif
;

EXTERN
char szMCTitle[]                        // MDI Client ... (for debugging purposes only)
#ifdef DEFINE_VALUES
 = "NARS2000 MDI Client Window" APPEND_DEBUG
#endif
,
     szSMTitle[]                        // Session Manager ...
#ifdef DEFINE_VALUES
 = "NARS2000 Session Manager" APPEND_DEBUG
#endif
#ifdef DEBUG
,
     szDBTitle[]                        // Debugger ...
#ifdef DEFINE_VALUES
 = "NARS2000 Debugger Window" APPEND_DEBUG
#endif
#endif
;

EXTERN
WCHAR wszFETitle[]                      // Function Editor ...
#ifdef DEFINE_VALUES
 = L"NARS2000 Function Editor" LAPPEND_DEBUG
#endif
,
      wszMETitle[]                      // Matrix Editor ...
#ifdef DEFINE_VALUES
 = L"NARS2000 Matrix Editor" LAPPEND_DEBUG
#endif
,
      wszVETitle[]                      // Vector Editor ...
#ifdef DEFINE_VALUES
 = L"NARS2000 Vector Editor" LAPPEND_DEBUG
#endif
;

#define MCWNDCLASS      "MDIClient"     // MDI Client window class
#define SMWNDCLASS      "SMClass"       // Session Manager ...
#ifdef DEBUG
#define DBWNDCLASS      "DBClass"       // Debugger     ...
#endif
#define  FEWNDCLASS     "FEClass"       // Function Editor ...
#define LFEWNDCLASS    L"FEClass"       // Function Editor ...
#define  MEWNDCLASS     "MEClass"       // Matrix Editor ...
#define LMEWNDCLASS    L"MEClass"       // Matrix Editor ...
#define  VEWNDCLASS     "VEClass"       // Vector Editor ...
#define LVEWNDCLASS    L"VEClass"       // Vector Editor ...
#define  ECWNDCLASS     "ECClass"       // Edit Control Window class
#define LECWNDCLASS    L"ECClass"       // Edit Control Window class

EXTERN
char szMCClass[]                        // MDI Client window class
#ifdef DEFINE_VALUES
 = MCWNDCLASS
#endif
,
     szSMClass[]                        // Session Manager ...
#ifdef DEFINE_VALUES
 = SMWNDCLASS
#endif
#ifdef DEBUG
,
     szDBClass[]                        // Debugger     ...
#ifdef DEFINE_VALUES
 = DBWNDCLASS
#endif
#endif
,
     szFEClass[]                        // Function Editor ...
#ifdef DEFINE_VALUES
 = FEWNDCLASS
#endif
,
     szMEClass[]                        // Matrix Editor ...
#ifdef DEFINE_VALUES
 = MEWNDCLASS
#endif
,
     szVEClass[]                        // Vector Editor ...
#ifdef DEFINE_VALUES
 = VEWNDCLASS
#endif
;

EXTERN
HIMAGELIST hImageList;                  // Handle to the common image list

// Same order as in ARRAY_TYPES
// so that BOOL < INT < FLOAT < APA < CHAR < HETERO < NESTED
EXTERN
UINT uTypeMap[]
#ifdef DEFINE_VALUES
//  BOOL, INT, FLOAT, CHAR, HETERO, NESTED, LIST, APA
 = {   0,   1,     2,    4,      5,      6,    7,   3}
#endif
;

typedef struct
{
    char  nrm;      // Normal           (shifted & unshifted) (unused)
    WCHAR alt;      // Alt key pressed  (shifted & unshifted)
} CHARCODE;

// If you are looking for places on the keyboard to put new symbols,
//   there are fifteen free Alt-Shift- combinations:
//     Alt-Shift-A
//     Alt-Shift-B
//     Alt-Shift-C
//     Alt-Shift-D
//     Alt-Shift-F
//     Alt-Shift-K
//     Alt-Shift-S
//     Alt-Shift-U
//     Alt-Shift-Q
//     Alt-Shift-R
//     Alt-Shift-V
//     Alt-Shift-W
//     Alt-Shift-X
//     Alt-Shift-Y
//     Alt-Shift-Z
//     Alt-Shift-LeftCaret
//     Alt-Shift-RightCaret
//     Alt-Shift-QuestionMark
//
//   as well as eleven duplicated symbols:
//
//     Asterisk
//     Equal
//     Left Caret
//     Question Mark
//     Right Caret
//     Shreik
//     Single Quote
//     Stile
//     Tilde
//     Underscore
//     Up Caret (Circumflex)

EXTERN
CHARCODE aCharCode[1+126-32]    // This ordering follows the ASCII charset
                                //   from 32 to 126 inclusive
#ifdef DEFINE_VALUES
=
{
//Nrm Alt
{' ', 0                       },  // Space             32
{'!', UTF16_EQUALUNDERBAR     },  // Quote-dot         33
{'"', 0                       },  // Quotation mark    34
{'#', UTF16_DELSTILE          },  // Number sign       35
{'$', UTF16_DELTASTILE        },  // Dollar sign       36
{'%', UTF16_CIRCLESTILE       },  // Percent sign      37
{'&', UTF16_CIRCLEBAR         },  // Ampersand         38
{'\'',UTF16_DOWNTACKJOT       },  // Apostrophe        39
{'(', UTF16_DOWNCARETTILDE    },  // Left paren        40
{')', UTF16_UPCARETTILDE      },  // Right paren       41
{'*', UTF16_CIRCLESTAR        },  // Star              42
{'+', UTF16_DOMINO            },  // Plus sign         43
{',', UTF16_LAMP              },  // Comma             44
{'-', UTF16_TIMES             },  // Bar               45
{'.', UTF16_SLOPEBAR          },  // Dot               46
{'/', UTF16_SLASHBAR          },  // Slash             47
{'0', UTF16_UPCARET           },  // 0                 48
{'1', UTF16_DIERESIS          },  // 1                 49
{'2', UTF16_OVERBAR           },  // 2                 50
{'3', UTF16_LEFTCARET         },  // 3                 51
{'4', UTF16_LEFTCARETUNDERBAR },  // 4                 52
{'5', UTF16_EQUAL             },  // 5                 53
{'6', UTF16_RIGHTCARETUNDERBAR},  // 6                 54
{'7', UTF16_RIGHTCARET        },  // 7                 55
{'8', UTF16_NOTEQUAL          },  // 8                 56
{'9', UTF16_DOWNCARET         },  // 9                 57
{':', 0                       },  // Colon             58
{';', UTF16_UPTACKJOT         },  // Semicolon         59
{'<', 0                       },  // Less              60
{'=', UTF16_COLONBAR          },  // Equal             61
{'>', 0                       },  // More              62
{'?', 0                       },  // Query             63
{'@', UTF16_DELTILDE          },  // At sign           64
{'A', 0                       },  // A                 65
{'B', 0                       },  // B                 66
{'C', 0                       },  // C                 67
{'D', 0                       },  // D                 68
{'E', UTF16_EPSILONUNDERBAR   },  // E                 69
{'F', 0                       },  // F                 70
{'G', UTF16_DIERESISDEL       },  // G                 71
{'H', UTF16_DELTAUNDERBAR     },  // H                 72
{'I', UTF16_IOTAUNDERBAR      },  // I                 73
{'J', UTF16_DIERESISJOT       },  // J                 74
{'K', 0                       },  // K                 75
{'L', UTF16_SQUAD             },  // L                 76
{'M', UTF16_STILETILDE        },  // M                 77
{'N', UTF16_DIERESISDOWNTACK  },  // N                 78
{'O', UTF16_DIERESISCIRCLE    },  // O                 79
{'P', UTF16_DIERESISSTAR      },  // P                 80
{'Q', 0                       },  // Q                 81
{'R', 0                       },  // R                 82
{'S', 0                       },  // S                 83
{'T', UTF16_DIERESISTILDE     },  // T                 84
{'U', 0                       },  // U                 85
{'V', 0                       },  // V                 86
{'W', 0                       },  // W                 87
{'X', 0                       },  // X                 88
{'Y', 0                       },  // Y                 89
{'Z', 0                       },  // Z                 90
{'[', UTF16_LEFTARROW         },  // Left bracket      91
{'\\',UTF16_RIGHTTACK         },  // Slope             92
{']', UTF16_RIGHTARROW        },  // Right bracket     93
{'^', UTF16_CIRCLESLOPE       },  // Up caret          94
{'_', UTF16_QUOTEDOT          },  // Underbar          95
{'`', UTF16_DIAMOND           },  // Grave accent      96
{'a', UTF16_ALPHA             },  // a                 97
{'b', UTF16_UPTACK            },  // b                 98
{'c', UTF16_UPSHOE            },  // c                 99
{'d', UTF16_DOWNSTILE         },  // d                100
{'e', UTF16_EPSILON           },  // e                101
{'f', UTF16_UNDERBAR          },  // f                102
{'g', UTF16_DEL               },  // g                103
{'h', UTF16_DELTA             },  // h                104
{'i', UTF16_IOTA              },  // i                105
{'j', UTF16_JOT               },  // j                106
{'k', UTF16_APOSTROPHE        },  // k                107
{'l', UTF16_QUAD              },  // l                108
{'m', UTF16_STILE             },  // m                109
{'n', UTF16_DOWNTACK          },  // n                110
{'o', UTF16_CIRCLE            },  // o                111
{'p', UTF16_STAR              },  // p                112
{'q', UTF16_QUERY             },  // q                113
{'r', UTF16_RHO               },  // r                114
{'s', UTF16_UPSTILE           },  // s                115
{'t', UTF16_TILDE             },  // t                116
{'u', UTF16_DOWNARROW         },  // u                117
{'v', UTF16_DOWNSHOE          },  // v                118
{'w', UTF16_OMEGA             },  // w                119
{'x', UTF16_RIGHTSHOE         },  // x                120
{'y', UTF16_UPARROW           },  // y                121
{'z', UTF16_LEFTSHOE          },  // z                122
{'{', UTF16_QUOTEQUAD         },  // Left brace       123
{'|', UTF16_LEFTTACK          },  // Stile            124
{'}', UTF16_ZILDE             },  // Right brace      125
{'~', UTF16_COMMABAR          },  // Tilde            126
}
#endif
;

typedef enum tagUNDOACTS
{
    undoNone = 0,       // No action
    undoIns,            // Insert a character
    undoRep,            // Replace a character
    undoDel,            // Delete one or more characters
    undoSel,            // Select one or more characters
    undoInsToggle,      // Toggle the insert mode
} UNDOACTS;

#define UNDO_NOGROUP    0

typedef struct tagUNDOBUF
{
    UINT  CharPosBeg,   // Beginning character position (from start of text),
                        //  -1 = current position
          CharPosEnd,   // Ending    ...
          Group;        // Group index identifies actions to be performed together,
                        //   0 = no grouping
    short Action;       // Action (see enum UNDOACTS)
    WCHAR Char;         // The character (if any),
                        //   0 = none
} UNDOBUF, *LPUNDOBUF;


#define ENUMS_DEFINED
#undef  EXTERN


//***************************************************************************
//  End of File: externs.h
//***************************************************************************
