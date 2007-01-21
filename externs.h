//***************************************************************************
//  NARS2000 -- Extern Variables
//***************************************************************************

#ifdef DEFINE_VARS
#define EXTERN
#else
#define EXTERN extern
#endif

// Default definitions
#define DEF_SMFONTNAME      "APL385 Unicode"
//efine DEF_SMFONTNAME      "SImPL"
#define DEF_TCLOGFONT       0,0,0,0,FW_BOLD  ,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,VARIABLE_PITCH | FF_ROMAN ,"Georgia"
#define DEF_SMLOGFONT       0,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,FIXED_PITCH    | FF_MODERN,"APL385 Unicode"
#define DEF_TCFONTNAME      "Georgia"
#define DEF_INDENT          6           // Prompt indent
#define DEF_SMPTSIZE       13           // Point size for SM font
#define DEF_TCPTSIZE        8           // ...            TC ...
//efine DEF_FONTWEIGHT     FW_THIN      // Font weight
#define DEF_TABS            4           // Tab insertion
#define DEF_CURWID_INS      2           // Cursor width for insert mode
#define DEF_CURWID_REP      5           // ...              replace ...
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

#include "symtab.h"
#include "tokens.h"
#include "parse.h"


// Define variables which are also used in the per tab structure
#include "pertabdefs.h"

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
LPCHAR lpszTemp;                        // Used for temporary storage

EXTERN
WCHAR *lpwszTemp;                       // ...

EXTERN
HINSTANCE _hInstance;                   // Global instance handle

EXTERN
HGLOBAL hGlbCurTab;                     // Global handle of current tab

EXTERN
HWND hWndTC,                            // Global Tab Control window handle
     hWndMF,                            // ...    Master Frame ...
     hWndTT;                            // ...    ToolTip      ...

#ifdef DEBUG
#define APPEND_DEBUG " (DEBUG)"
#else
#define APPEND_DEBUG
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
// Use as in:  (*aSysVarValid[SYSVAR_VALID_IO]) (lpYYName, &lpYYExpr->tkToken);
BOOL (*aSysVarValid[SYSVAR_VALID_LENGTH]) (LPYYSTYPE, LPTOKEN);

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

typedef struct tagVKSTATE
{
    UINT  Shift:1,                      // Left- or right-shift key up(0) or down(1)
////     lShift:1,
////     rShift:1,
          Alt:1,                        // Left- or right-Alt key up(0) or down(1)
////     lAlt:1,
////     rAlt:1,
          Ctl:1,                        // Left or -right Ctl key up(0) or down(1)
////     lCtl:1,
////     rCtl:1,
          Ins:1;                        // Replace(0) or insert(1)
} VKSTATE;

EXTERN
VKSTATE vkState;

EXTERN
HBITMAP hBitMapLineCont;                // Bitmap for the line continuation char

EXTERN
int     iLCWidth;                       // Width of the line continuation column

EXTERN
BITMAP  bmLineCont;                     // Bitmap metrics for the line continuation char

EXTERN
LOGFONT lfSM_CWS                        // LOGFONT for SM in CLEAR WS
#ifdef DEFINE_VALUES
 = {DEF_SMLOGFONT}
#endif
;

EXTERN
HFONT   hFontTC;                        // Handle to font for the TC

EXTERN
CHOOSEFONT cfTC;                        // Global for ChooseFont for the TC

EXTERN
LOGFONT lfTC                            // LOGFONT struc for the font for the TC
#ifdef DEFINE_VALUES
 = {DEF_TCLOGFONT}
#endif
;

////EXTERN
////NEWTEXTMETRIC ntmTC;                    // NEWTEXTMETRIC ...


#ifdef DEBUG_ODS
EXTERN
char DebugFile[];

EXTERN
HANDLE DebugHandle;
#endif


#define ENUMS_DEFINED


//***************************************************************************
//  End of File: externs.h
//***************************************************************************
