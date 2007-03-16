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
#define DEF_MAXLINELEN   1024           // Maximum line length
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


#ifdef DEFINE_VARS
#define EXTERN
#else
#define EXTERN extern
#endif

//***************************************************************************
//  Below this point, define variables which do not need to be saved
//    in the per tab structure
//***************************************************************************

// Default global values of system variables -- these values
//   are used to set the variables in a CLEAR WS.

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
APLBOOL  bQuadDF_CWS        ,           // []DF
         bQuadIF_CWS        ,           // []IF
         bQuadIO_CWS        ,           // []IO
         bQuadxSA_CWS       ;           // []SA (in its index form)

EXTERN
APLINT   uQuadPP_CWS        ,           // []PP
         uQuadPW_CWS        ,           // []PW
         uQuadRL_CWS        ;           // []RL

EXTERN
APLCHAR  cQuadPR_CWS        ;           // []PR     (' ') (When a char scalar)

EXTERN
LPSYMENTRY lpSymQuadRL;                 // Ptr to STE for QuadRL

EXTERN
LPCHAR lpszTemp;                        // Used for temporary storage

EXTERN
LPWCHAR lpwszTemp,                      // ...
        lpwszFormat;                    // Used for formatting

EXTERN
HINSTANCE _hInstance;                   // Global instance handle

EXTERN
HGLOBAL hGlbCurTab;                     // Global handle of current tab

EXTERN
HWND hWndTC,                            // Global Tab Control window handle
     hWndMF,                            // ...    Master Frame ...
     hWndTT;                            // ...    ToolTip      ...

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
// Define the local copy of various values which allows
//   ParseLine to be re-entrant.
PLLOCALVARS gplLocalVars
#ifdef DEFINE_VALUES
 = {0}
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

#ifndef ENUMS_DEFINED
typedef enum tagDISPLAY_FORMAT
{
    DISPLAY_SPACES = 0,                 // Use a space to surround nested arrays
    DISPLAY_PARENS,                     // ... parens  ...
} DISPLAY_FORMAT;

typedef enum tagAPA_FORMAT
{
    APA_OFF = 0,                        // Display the individual elements in an APA
    APA_ON,                             // ...     an APA as Off + Mul x {iota} Len
} APA_FORMAT;
#endif

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

EXTERN
HGLOBAL ghGlbToken;                     // Save area for current hGlbToken

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

EXTERN
CHARCODE aCharCode[1+126-32]    // This ordering follows the ASCII charset
                                //   from 32 to 126 inclusive
#ifdef DEFINE_VALUES
=
{
//Nrm Alt
{' ', 0                    },   // Space             32
{'!', UCS2_EQUALUNDERBAR   },   // Quote-dot         33
{'"', 0                    },   // Quotation mark    34
{'#', UCS2_DELSTILE        },   // Number sign       35
{'$', UCS2_DELTASTILE      },   // Dollar sign       36
{'%', UCS2_CIRCLESTILE     },   // Percent sign      37
{'&', UCS2_CIRCLEBAR       },   // Ampersand         38
{'\'',UCS2_THORN           },   // Apostrophe        39
{'(', UCS2_DOWNCARETTILDE  },   // Left paren        40
{')', UCS2_UPCARETTILDE    },   // Right paren       41
{'*', UCS2_CIRCLESTAR      },   // Star              42
{'+', UCS2_DOMINO          },   // Plus sign         43
{',', UCS2_LAMP            },   // Comma             44
{'-', UCS2_TIMES           },   // Bar               45
{'.', UCS2_SLOPEBAR        },   // Dot               46
{'/', UCS2_SLASHBAR        },   // Slash             47
{'0', UCS2_UPCARET         },   // 0                 48
{'1', UCS2_DIERESIS        },   // 1                 49
{'2', UCS2_OVERBAR         },   // 2                 50
{'3', UCS2_LEFTCARET       },   // 3                 51
{'4', UCS2_NOTMORE         },   // 4                 52
{'5', UCS2_EQUAL           },   // 5                 53
{'6', UCS2_NOTLESS         },   // 6                 54
{'7', UCS2_RIGHTCARET      },   // 7                 55
{'8', UCS2_NOTEQUAL        },   // 8                 56
{'9', UCS2_DOWNCARET       },   // 9                 57
{':', 0                    },   // Colon             58
{';', UCS2_HYDRANT         },   // Semicolon         59
{'<', 0                    },   // Less              60
{'=', UCS2_DIVIDE          },   // Equal             61
{'>', 0                    },   // More              62
{'?', 0                    },   // Query             63
{'@', UCS2_DELTILDE        },   // At sign           64
{'A', 0                    },   // A                 65
{'B', 0                    },   // B                 66
{'C', 0                    },   // C                 67
{'D', 0                    },   // D                 68
{'E', UCS2_EPSILONUNDERBAR },   // E                 69
{'F', 0                    },   // F                 70
{'G', UCS2_DIERESISDEL     },   // G                 71
{'H', UCS2_DELTAUNDERBAR   },   // H                 72
{'I', UCS2_IOTAUNDERBAR    },   // I                 73
{'J', UCS2_DIERESISJOT     },   // J                 74
{'K', 0                    },   // K                 75
{'L', UCS2_SQUAD           },   // L                 76
{'M', UCS2_STILETILDE      },   // M                 77
{'N', UCS2_DIERESISDOWNTACK},   // N                 78
{'O', UCS2_DIERESISCIRCLE  },   // O                 79
{'P', UCS2_DIERESISSTAR    },   // P                 80
{'Q', 0                    },   // Q                 81
{'R', 0                    },   // R                 82
{'S', 0                    },   // S                 83
{'T', UCS2_DIERESISTILDE   },   // T                 84
{'U', 0                    },   // U                 85
{'V', 0                    },   // V                 86
{'W', 0                    },   // W                 87
{'X', 0                    },   // X                 88
{'Y', 0                    },   // Y                 89
{'Z', 0                    },   // Z                 90
{'[', UCS2_LEFTARROW       },   // Left bracket      91
{'\\',UCS2_LEFTTACK        },   // Slope             92
{']', UCS2_RIGHTARROW      },   // Right bracket     93
{'^', UCS2_CIRCLESLOPE     },   // Up caret          94
{'_', UCS2_QUOTEDOT        },   // Underbar          95
{'`', UCS2_DIAMOND         },   // Grave accent      96
{'a', UCS2_ALPHA           },   // a                 97
{'b', UCS2_UPTACK          },   // b                 98
{'c', UCS2_UPSHOE          },   // c                 99
{'d', UCS2_DOWNSTILE       },   // d                100
{'e', UCS2_EPSILON         },   // e                101
{'f', UCS2_UNDERBAR        },   // f                102
{'g', UCS2_DEL             },   // g                103
{'h', UCS2_DELTA           },   // h                104
{'i', UCS2_IOTA            },   // i                105
{'j', UCS2_JOT             },   // j                106
{'k', UCS2_APOSTROPHE      },   // k                107
{'l', UCS2_QUAD            },   // l                108
{'m', UCS2_STILE           },   // m                109
{'n', UCS2_DOWNTACK        },   // n                110
{'o', UCS2_CIRCLE          },   // o                111
{'p', UCS2_STAR            },   // p                112
{'q', UCS2_QUERY           },   // q                113
{'r', UCS2_RHO             },   // r                114
{'s', UCS2_UPSTILE         },   // s                115
{'t', UCS2_TILDE           },   // t                116
{'u', UCS2_DNARROW         },   // u                117
{'v', UCS2_DOWNSHOE        },   // v                118
{'w', UCS2_OMEGA           },   // w                119
{'x', UCS2_RIGHTSHOE       },   // x                120
{'y', UCS2_UPARROW         },   // y                121
{'z', UCS2_LEFTSHOE        },   // z                122
{'{', UCS2_QUOTEQUAD       },   // Left brace       123
{'|', UCS2_RIGHTTACK       },   // Stile            124
{'}', UCS2_ZILDE           },   // Right brace      125
{'~', UCS2_COMMABAR        },   // Tilde            126
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
