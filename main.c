//***************************************************************************
//  NARS2000 -- Open Source APL Interpreter
//***************************************************************************

#define COMPILE_MULTIMON_STUBS
//#define WINVER       0x04FF // Needed for WINUSER.H definitions
//#define _WIN32_WINNT 0x04FF // ...

#pragma pack (1)
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <windowsx.h16>
#include <commctrl.h>   // Includes the common control header
//#include <multimon.h>   // Multiple monitor support
//#include <QControl.h>
#include <limits.h>
#include <direct.h>
#include "main.h"
#include "resdebug.h"
#include "resource.h"
#include "datatype.h"
#include "symtab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


extern LPWCHAR    lpwszCurLine;
////extern LPTOKEN lptkStackBase;
extern LPCHAR     lpszNumAlp;
extern LPWCHAR    lpwszString;
extern LPHSHENTRY lpHshTab;
extern LPSYMENTRY lpSymTab;
extern HGLOBAL    hGlbHist;
extern HGLOBAL    hGlbQuadALX   , hGlbQuadALX_CWS,
                  hGlbQuadELX   , hGlbQuadELX_CWS,
                  hGlbQuadLX    , hGlbQuadLX_CWS,
                  hGlbQuadPR    , hGlbQuadPR_CWS,
                  hGlbQuadSA    , hGlbQuadSA_CWS,
                  hGlbQuadWSID  , hGlbQuadWSID_CWS;
extern APLFLOAT   fQuadCT       , fQuadCT_CWS;
extern APLBOOL    bQuadDF       , bQuadDF_CWS,
                  bQuadIF       , bQuadIF_CWS,
                  bQuadIO       , bQuadIO_CWS,
                  bQuadxSA      , bQuadxSA_CWS;
extern APLINT     uQuadPP       , uQuadPP_CWS,
                  uQuadPW       , uQuadPW_CWS,
                  uQuadRL       , uQuadRL_CWS;
extern APLCHAR    cQuadPR       , cQuadPR_CWS;

extern
int cxWindowPixels,             // Size of SM client area in pixels
    cyWindowPixels,             // ...
    nWindowChars,               // ...                    in chars
    nWindowLines;               // ...                       lines
extern
BOOL bSelText;                  // Are we selecting text?
extern
int xSelText, ySelText,         // Starting x- & y-positions of selected text
    xSelChar, ySelChar;         // Starting x- & y- offsets into history buffer (in chars)
extern
RECT rcSelInit, rcSelText;      // Selection rectangles
extern
BOOL bCurLineChanged;           // Has the current line changed?
extern
HGLOBAL ghGlbToken;             // Save area for current token memory handle
extern
int iFirstBufferLine,           // Index into lpGlbHist of the first line
                                //   in the history buffer (ALWAYS 0).
    iLastBufferLine,            // Index into lpGlbHist of the last line
                                //   in the history buffer (ALWAYS DEF_HISTLINES-1)
    iFirstWindowLine,           // Index into lpGlbHist of the top line
                                //   in the window -- changed by vertical
                                //   scrolling.
    iCurLine,                   // Index into lpGlbHist of the line with
                                //   the cursor on it -- changed by up and
                                //   down cursor movements.
    iLastWindowLine,            // Index into lpGlbHist of the bottom line
                                //   in the window -- changed by vertical
                                //   scrolling or window resizing.
    iLastValidLine,             // Index into lpGlbHist of the last line
                                //   which is defined (contents are not NULL)
                                //   -- changed by the user pressing Enter
                                //   (and thus executing a statement), or by
                                //   normal program or system command
                                //   output or by error messages.
    iFirstBufferChar,           // Index into lpwszCurLine of the first char
                                //   in the buffer (ALWAYS 0)
    iFirstWindowChar,           // Index into lpwszCurLine of the first char
                                //   in the window -- changed by horizontal
                                //   scrolling.
    iCurChar,                   // Horizontal position of the caret (in chars)
                                // The vertical position is iCurLine - iFirstWindowLine (in chars).
    iLastWindowChar,            // Index into lpwszCurLine of the last char
                                //   in the window -- changed by horizontal
                                //   scrolling or window resizing.
    iLastBufferChar,            // Index into lpwszCurLine of the last char
                                //   in the buffer (always (DEF_MAXLINELEN - 1))
    iCurLineLen,                // Length of lpwszCurLine (not including the trailing zero)
    iLogPixelsX, iLogPixelsY;   // # logical pixels/inch in screen X- and Y-dimensions

// Structure for Per Tab Control Data
typedef struct tagPERTABDATA
{
    HWND hWndMC,        // Window handle for MDI Client window
         hWndSM,        // ...               session manager
         hWndDB;        // ...               debugger window

    LPWCHAR    lpwszCurLine;
////LPTOKEN    lptkStackBase;
    LPCHAR     lpszNumAlp;
    LPWCHAR    lpwszString;
    LPHSHENTRY lpHshTab;
    LPSYMENTRY lpSymTab;
    HGLOBAL    hGlbHist;

    // Miscellaneous values
    int         cxWindowPixels,
                cyWindowPixels,
                nWindowChars,
                nWindowLines;
    BOOL        bSelText;
    int         xSelText,
                ySelText,
                xSelChar,
                ySelChar;
    RECT        rcSelInit,
                rcSelText;
    BOOL        bCurLineChanged;
    HGLOBAL     ghGlbToken;
    int         iFirstBufferLine,
                iLastBufferLine,
                iFirstWindowLine,
                iCurLine,
                iLastWindowLine,
                iLastValidLine,
                iFirstBufferChar,
                iFirstWindowChar,
                iCurChar,
                iLastWindowChar,
                iLastBufferChar,
                iCurLineLen,
                iLogPixelsX,
                iLogPixelsY;

    // Save system variable values
    HGLOBAL    hGlbQuadALX,
               hGlbQuadELX,
               hGlbQuadLX,
               hGlbQuadSA,
               hGlbQuadWSID,
               hGlbQuadPR;
    APLFLOAT   fQuadCT;
    APLBOOL    bQuadDF,
               bQuadIF,
               bQuadIO,
               bQuadxSA;
    APLINT     uQuadPP,
               uQuadPW,
               uQuadRL;
    APLCHAR    cQuadPR;
} PERTABDATA, *LPPERTABDATA;


//************************** Data Area **************************************

#define DEF_CTEMP_MAXSIZE   1024        // Maximum size of char  temporary storage
#define DEF_CTEMP_INITSIZE  1024        // Initial ...
#define DEF_WTEMP_MAXSIZE   1024        // Maximum size of WCHAR ...
#define DEF_WTEMP_INITSIZE  1024        // Initial ...


#ifdef DEBUG_ODS
extern char DebugFile[];
extern HANDLE DebugHandle;
#endif

char *lpszTemp;                         // Used for temporary storage
WCHAR *lpwszTemp;                       // ...

HINSTANCE _hInstance;                   // Global instance handle
HICON hIconMF_Large, hIconMF_Small,     // Icon handles
      hIconSM_Large, hIconSM_Small,
      hIconDB_Large, hIconDB_Small;
HGLOBAL hGlbCurTab;                     // Global handle of current tab
HWND hWndTC,                            // Global Tab Control window handle
     hWndMF,                            // ...    Master Frame ...
     hWndMC,                            // ...    MDI Client   ...
     hWndSM,                            // ...    Session Manager ...
     hWndDB,                            // ...    Debugger     ...
     hWndTT;                            // ...    ToolTip      ...
int nMinState,                          // Minimized state as per WinMain
    iScrollSize,                        // Width of a vertical scrollbar
    MFSizeState = SIZE_RESTORED;        // Size state for MF (SIZE_xxx)
HANDLE hAccel;                          // Keyboard accelerators
POINT MFPosCtr;                         // X- and Y- center of Master Frame Window position
SIZE  MFSize;                           // Size of Master Frame Window window rectangle
BOOL fHelp = FALSE,                     // TRUE iff we displayed help
     bCommandLine;                      // ...      there is a filename on the command line
char szAppDPFE  [_MAX_PATH],            // .EXE drive, path, filename.ext
     szHlpDPFE  [_MAX_PATH],            // .HLP ...
     szInitDir  [_MAX_PATH],            // Initial directory for File Open & Save
     szOpenFile [_MAX_PATH];            // Save area for multiple files to open

#ifdef DEBUG
#define APPEND_DEBUG " (DEBUG)"
#else
#define APPEND_DEBUG
#endif

char pszAppName[]       = "NARS2000" APPEND_DEBUG;                  // Application name for MessageBox

char szMFTitle[]        = "NARS2000" APPEND_DEBUG,                  // Master frame window title
     szSMTitle[]        = "NARS2000 Session Manager" APPEND_DEBUG,  // Session Manager ...
     szTCTitle[]        = "NARS2000 Tab Control Window" APPEND_DEBUG,// Tab Control ... (for debugging purposes only)
     szMCTitle[]        = "NARS2000 MDI Client Window" APPEND_DEBUG,// MDI Client ... (for debugging purposes only)
     szDBTitle[]        = "NARS2000 Debugger Window" APPEND_DEBUG;  // Debugger ...

#define MFWNDCLASS      "MFClass"       // Master Frame Window class
#define MCWNDCLASS      "MDIClient"     // MDI Client   ...
#define SMWNDCLASS      "SMClass"       // Session Manager ...
#define DBWNDCLASS      "DBClass"       // Debugger     ...

char szMFClass[]        = MFWNDCLASS,   // Master Frame Window class
     szMCClass[]        = MCWNDCLASS,   // MDI Client   ...
     szSMClass[]        = SMWNDCLASS,   // Session Manager ...
     szDBClass[]        = DBWNDCLASS;   // Debugger     ...

char pszNoRegMFWndClass[]   = "Unable to register window class <" MFWNDCLASS ">.",
     pszNoRegSMWndClass[]   = "Unable to register window class <" SMWNDCLASS ">.",
     pszNoRegDBWndClass[]   = "Unable to register window class <" DBWNDCLASS ">.";

char pszNoCreateMFWnd[]     = "Unable to create Master Frame window",
     pszNoCreateTCWnd[]     = "Unable to create Tab Control window",
     pszNoCreateMCWnd[]     = "Unable to create MDI Client window",
     pszNoCreateSMWnd[]     = "Unable to create Session Manager window",
     pszNoCreateDBWnd[]     = "Unable to create Debugger window",
     pszNoCreateTTWnd[]     = "Unable to create ToolTip window";


//***************************************************************************
//  MF_Create
//
//  Perform window-specific initialization
//***************************************************************************

void MF_Create (HWND hWnd)

{
    // Read the default scrollbar width
    iScrollSize = GetSystemMetrics (SM_CXVSCROLL);
} // End MF_Create


//***************************************************************************
//  MF_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void MF_Delete (HWND hWnd)

{
} // End MF_Delete


//***************************************************************************
//  CreateToolTip
//
//  Creates the ToolTip window and initializes it.
//***************************************************************************

HWND CreateToolTip (HWND hWndParent)

{
    HWND hWnd;

    // Create the ToolTip window
    hWnd =
    CreateWindowEx (0L,             // Extended styles
#if QTT
                    WC_TOOLTIPS,    // Class for Qcontrols
#else
                    TOOLTIPS_CLASS, // Class for MS Controls
#endif
                    NULL,           // Window title
                    TTS_NOANIMATE
                  | TTS_ALWAYSTIP,  // Styles
                    CW_USEDEFAULT,  // X-coord
                    CW_USEDEFAULT,  // Y-...
                    CW_USEDEFAULT,  // Width
                    CW_USEDEFAULT,  // Height
                    NULL,           // Parent window
                    NULL,           // Menu
                    _hInstance,     // Instance
                    NULL);          // No extra data

    if (hWnd EQ NULL)
    {
        MB (pszNoCreateTTWnd);
        return NULL;        // Stop the whole process
    } // End IF

    return hWnd;
} // End CreateToolTip


//***************************************************************************
//  CreateChildWindows
//
//  Create the child windows under the parent window
//***************************************************************************

BOOL CreateChildWindows (HWND hWnd)

{
    RECT rc;                // Rectangle for setting size of window

    // Get the size and position of the parent window.
    GetClientRect (hWnd, &rc);

    // Create the ToolTip window first so that
    // the other windows can reference it.
    hWndTT = CreateToolTip (hWnd);

    if (hWndTT EQ NULL)
        return FALSE;       // Stop the whole process

    // Create the tab control window
    hWndTC =
        CreateWindowEx (0L,                             // Extended styles
                        WC_TABCONTROL,                  // Class
                        szTCTitle,                      // Window title (for debugging purposes only)
                        WS_CHILD
                      | WS_CLIPSIBLINGS
                      | WS_VISIBLE
                      | TCS_TOOLTIPS,                   // Styles
                        rc.left,                        // X-coord
                        rc.top,                         // Y-coord
                        rc.right - rc.left,             // X-size
                        rc.bottom - rc.top,             // Y-size
                        hWnd,                           // Parent window
                        NULL,                           // Menu
                        _hInstance,                     // Instance
                        NULL);                          // No extra data

    if (hWndTC EQ NULL)
    {
        MB (pszNoCreateTCWnd);
        return FALSE;
    } // End IF

    // Show and paint the window
    ShowWindow (hWndTC, SW_SHOWNORMAL);
    UpdateWindow (hWndTC);

    return TRUE;            // Tell 'em it worked
} // End CreateChildWindows


//***************************************************************************
//  ClearWsData
//
//  Clear data in this WS to global default values
//***************************************************************************

void ClearWsData
    (void)

{
    hGlbQuadALX     = hGlbQuadALX_CWS;
    hGlbQuadELX     = hGlbQuadELX_CWS;
    hGlbQuadLX      = hGlbQuadLX_CWS;
    hGlbQuadSA      = hGlbQuadSA_CWS;
    hGlbQuadWSID    = hGlbQuadWSID_CWS;
    hGlbQuadPR      = hGlbQuadPR_CWS;
    fQuadCT         = fQuadCT_CWS;
    bQuadDF         = bQuadDF_CWS;
    bQuadIF         = bQuadIF_CWS;
    bQuadIO         = bQuadIO_CWS;
    uQuadPP         = uQuadPP_CWS;
    uQuadPW         = uQuadPW_CWS;
    uQuadRL         = uQuadRL_CWS;
    cQuadPR         = cQuadPR_CWS;
    bQuadxSA        = bQuadxSA_CWS;
} // End ClearWsData


//***************************************************************************
//  SaveWsData
//
//  Save data from the current WS into global memory
//***************************************************************************

void SaveWsData
    (HGLOBAL hGlbData)

{
    LPPERTABDATA lpMem;

    // Check to see if none saved as yet
    if (!hGlbData)
        return;

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Save the various handles and ptrs
    lpMem->hWndMC           = hWndMC          ;
    lpMem->hWndSM           = hWndSM          ;
    lpMem->hWndDB           = hWndDB          ;
    lpMem->lpwszCurLine     = lpwszCurLine    ;
////lpMem->lptkStackBase    = lptkStackBase   ;
    lpMem->lpszNumAlp       = lpszNumAlp      ;
    lpMem->lpwszString      = lpwszString     ;
    lpMem->lpHshTab         = lpHshTab        ;
    lpMem->lpSymTab         = lpSymTab        ;
    lpMem->hGlbHist         = hGlbHist        ;

    // Save miscellaneous values
    lpMem->cxWindowPixels   = cxWindowPixels  ;
    lpMem->cyWindowPixels   = cyWindowPixels  ;
    lpMem->nWindowChars     = nWindowChars    ;
    lpMem->nWindowLines     = nWindowLines    ;
    lpMem->bSelText         = bSelText        ;
    lpMem->xSelText         = xSelText        ;
    lpMem->ySelText         = ySelText        ;
    lpMem->xSelChar         = xSelChar        ;
    lpMem->ySelChar         = ySelChar        ;
    lpMem->rcSelInit        = rcSelInit       ;
    lpMem->rcSelText        = rcSelText       ;
    lpMem->bCurLineChanged  = bCurLineChanged ;
    lpMem->ghGlbToken       = ghGlbToken      ;
    lpMem->iFirstBufferLine = iFirstBufferLine;
    lpMem->iLastBufferLine  = iLastBufferLine ;
    lpMem->iFirstWindowLine = iFirstWindowLine;
    lpMem->iCurLine         = iCurLine        ;
    lpMem->iLastWindowLine  = iLastWindowLine ;
    lpMem->iLastValidLine   = iLastValidLine  ;
    lpMem->iFirstBufferChar = iFirstBufferChar;
    lpMem->iFirstWindowChar = iFirstWindowChar;
    lpMem->iCurChar         = iCurChar        ;
    lpMem->iLastWindowChar  = iLastWindowChar ;
    lpMem->iLastBufferChar  = iLastBufferChar ;
    lpMem->iCurLineLen      = iCurLineLen     ;
    lpMem->iLogPixelsX      = iLogPixelsX     ;
    lpMem->iLogPixelsY      = iLogPixelsY     ;

    // Save system variable values
    lpMem->hGlbQuadALX      = hGlbQuadALX     ;
    lpMem->hGlbQuadELX      = hGlbQuadELX     ;
    lpMem->hGlbQuadLX       = hGlbQuadLX      ;
    lpMem->hGlbQuadSA       = hGlbQuadSA      ;
    lpMem->hGlbQuadWSID     = hGlbQuadWSID    ;
    lpMem->hGlbQuadPR       = hGlbQuadPR      ;
    lpMem->fQuadCT          = fQuadCT         ;
    lpMem->bQuadDF          = bQuadDF         ;
    lpMem->bQuadIF          = bQuadIF         ;
    lpMem->bQuadIO          = bQuadIO         ;
    lpMem->uQuadPP          = uQuadPP         ;
    lpMem->uQuadPW          = uQuadPW         ;
    lpMem->uQuadRL          = uQuadRL         ;
    lpMem->cQuadPR          = cQuadPR         ;
    lpMem->bQuadxSA         = bQuadxSA        ;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;
} // End SaveWsData


//***************************************************************************
//  RestWsData
//
//  Restore data into the current WS from global memory
//***************************************************************************

void RestWsData
    (HGLOBAL hGlbData)

{
    LPPERTABDATA lpMem;

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Restore the various handles and ptrs
    hWndMC          =  lpMem->hWndMC          ;
    hWndSM          =  lpMem->hWndSM          ;
    hWndDB          =  lpMem->hWndDB          ;
    lpwszCurLine    =  lpMem->lpwszCurLine    ;
////lptkStackBase   =  lpMem->lptkStackBase   ;
    lpszNumAlp      =  lpMem->lpszNumAlp      ;
    lpwszString     =  lpMem->lpwszString     ;
    lpHshTab        =  lpMem->lpHshTab        ;
    lpSymTab        =  lpMem->lpSymTab        ;
    hGlbHist        =  lpMem->hGlbHist        ;

    // Restore miscellaneous values
    cxWindowPixels  = lpMem->cxWindowPixels   ;
    cyWindowPixels  = lpMem->cyWindowPixels   ;
    nWindowChars    = lpMem->nWindowChars     ;
    nWindowLines    = lpMem->nWindowLines     ;
    bSelText        = lpMem->bSelText         ;
    xSelText        = lpMem->xSelText         ;
    ySelText        = lpMem->ySelText         ;
    xSelChar        = lpMem->xSelChar         ;
    ySelChar        = lpMem->ySelChar         ;
    rcSelInit       = lpMem->rcSelInit        ;
    rcSelText       = lpMem->rcSelText        ;
    bCurLineChanged = lpMem->bCurLineChanged  ;
    ghGlbToken      = lpMem->ghGlbToken       ;
    iFirstBufferLine= lpMem->iFirstBufferLine ;
    iLastBufferLine = lpMem->iLastBufferLine  ;
    iFirstWindowLine= lpMem->iFirstWindowLine ;
    iCurLine        = lpMem->iCurLine         ;
    iLastWindowLine = lpMem->iLastWindowLine  ;
    iLastValidLine  = lpMem->iLastValidLine   ;
    iFirstBufferChar= lpMem->iFirstBufferChar ;
    iFirstWindowChar= lpMem->iFirstWindowChar ;
    iCurChar        = lpMem->iCurChar         ;
    iLastWindowChar = lpMem->iLastWindowChar  ;
    iLastBufferChar = lpMem->iLastBufferChar  ;
    iCurLineLen     = lpMem->iCurLineLen      ;
    iLogPixelsX     = lpMem->iLogPixelsX      ;
    iLogPixelsY     = lpMem->iLogPixelsY      ;

    // Restore system variable values
    hGlbQuadALX     = lpMem->hGlbQuadALX      ;
    hGlbQuadELX     = lpMem->hGlbQuadELX      ;
    hGlbQuadLX      = lpMem->hGlbQuadLX       ;
    hGlbQuadSA      = lpMem->hGlbQuadSA       ;
    hGlbQuadWSID    = lpMem->hGlbQuadWSID     ;
    hGlbQuadPR      = lpMem->hGlbQuadPR       ;
    fQuadCT         = lpMem->fQuadCT          ;
    bQuadDF         = lpMem->bQuadDF          ;
    bQuadIF         = lpMem->bQuadIF          ;
    bQuadIO         = lpMem->bQuadIO          ;
    uQuadPP         = lpMem->uQuadPP          ;
    uQuadPW         = lpMem->uQuadPW          ;
    uQuadRL         = lpMem->uQuadRL          ;
    cQuadPR         = lpMem->cQuadPR          ;
    bQuadxSA        = lpMem->bQuadxSA         ;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;
} // End RestWsData


//***************************************************************************
//  ShowHideEnumChildProc
//
//  Enum child proc for ShowChildWindows
//***************************************************************************

BOOL CALLBACK ShowHideEnumChildProc
    (HWND   hWnd,
     LPARAM lParam)

{
    // Show or hide the window
    ShowWindow (hWnd, lParam);

    return TRUE;
} // End ShowHideEnumChildProc


//***************************************************************************
//  ShowHideChildWindows
//
//  Show or hide the given window and all its children
//***************************************************************************

void ShowHideChildWindows
    (HWND   hWnd,
     LPARAM lParam)

{
    if (!hWnd)
        return;

    // Show/hide the parent window
    ShowWindow (hWnd, lParam);

    // Loop through the child windows
    EnumChildWindows (hWnd, &ShowHideEnumChildProc, lParam);
} // End ShowHideChildWindows


//***************************************************************************
//  CreateNewTab
//
//  Create a new tab
//***************************************************************************

BOOL CreateNewTab
    (HWND hWndParent,
     LPSTR lpszWSID)

{
    TC_ITEM      tcItem;
    HGLOBAL      hGlbData;
    LPPERTABDATA lpMem = NULL;
    BOOL         bRet = TRUE;
    RECT         rc;        // Rectangle for setting size of window
    CLIENTCREATESTRUCT ccs; // For MDI Client window

    // Get the size and position of the parent window.
    GetClientRect (hWndParent, &rc);

    // Save data from the current WS into global memory
    SaveWsData (hGlbCurTab);

    // Hide the child windows of the outgoing tab
    ShowHideChildWindows (hWndMC, SW_HIDE);

    // Allocate per tab data
    hGlbData = MyGlobalAlloc (GHND, sizeof (PERTABDATA));
    if (!hGlbData)
        return FALSE;       // Stop the whole process

    // Save the handle
    hGlbCurTab = hGlbData;

    tcItem.mask    = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
    tcItem.iImage  = -1;
    tcItem.pszText = lpszWSID;
    tcItem.lParam  = (LPARAM) hGlbData;

    if (TabCtrl_InsertItem (hWndTC, 0, &tcItem) == -1)
    {
        bRet = FALSE;       // Stop the whole process
        goto ERROR_EXIT;
    } // End IF

    // Lock the per-tab data to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Calculate the display rectangle, assuming the
    // tab control is the size of the client area.
    TabCtrl_AdjustRect (hWndTC, FALSE, &rc);

    // Fill in the CLIENTCREATESTRUCT for the MDI Client
    ccs.hWindowMenu = GetSubMenu (GetMenu (hWndParent), IDMPOS_WINDOW);
    ccs.idFirstChild = IDM_CHILDWINDOW;

    // Create the MDI client window
    lpMem->hWndMC =
    CreateWindowEx (WS_EX_CLIENTEDGE,       // Extended styles
                    szMCClass,              // Class name
                    szMCTitle,              // Window title (for debugging purposes only)
////                MDIS_ALLCHILDSTYLES,    // Styles
                    WS_CHILD
                  | WS_BORDER
                  | WS_CLIPCHILDREN,        // Styles
                    rc.left,                // X-coordinate
                    rc.top,                 // Y-...
                    rc.right  - rc.left,    // Width
                    rc.bottom - rc.top,     // Height
                    hWndParent,             // Parent
                    (HMENU) 2,              // Child identifier
                    _hInstance,             // hInstance
                    &ccs);                  // lParam
    if (lpMem->hWndMC EQ NULL)
    {
        MB (pszNoCreateMCWnd);
        return FALSE;       // Stop the whole process
    } // End IF

    // Show and paint the window
    ShowWindow (lpMem->hWndMC, SW_SHOWNORMAL);
    UpdateWindow (lpMem->hWndMC);

    // Create the Debugger window first
    //   so it can be used by subsequent windows
    lpMem->hWndDB =
    CreateMDIWindow (szDBClass,             // Class name
                     szDBTitle,             // Window title
                     0,                     // Styles
                     CW_USEDEFAULT,         // X-pos
                     CW_USEDEFAULT,         // Y-pos
                     CW_USEDEFAULT,         // Height
                     CW_USEDEFAULT,         // Width
                     lpMem->hWndMC,         // Parent
                     _hInstance,            // Instance
                     0);                    // No extra data
    if (lpMem->hWndDB EQ NULL)
    {
        MB (pszNoCreateDBWnd);
        bRet = FALSE;       // Stop the whole process
        goto ERROR_EXIT;
    } // End IF

    // Create the Session Manager window
    lpMem->hWndSM =
    CreateMDIWindow (szSMClass,             // Class name
                     szSMTitle,             // Window title
                     WS_VSCROLL             // Styles
                   | WS_HSCROLL,
                     CW_USEDEFAULT,         // X-pos
                     CW_USEDEFAULT,         // Y-pos
                     CW_USEDEFAULT,         // Height
                     CW_USEDEFAULT,         // Width
                     lpMem->hWndMC,         // Parent
                     _hInstance,            // Instance
                     0);                    // No extra data
    if (lpMem->hWndSM EQ NULL)
    {
        MB (pszNoCreateSMWnd);
        bRet = FALSE;       // Stop the whole process
        goto ERROR_EXIT;
    } // End IF

    // Save the window handles in global variables
    hWndMC = lpMem->hWndMC;
    hWndSM = lpMem->hWndSM;
    hWndDB = lpMem->hWndDB;

    // Clear data in this WS to global default values
    ClearWsData ();
ERROR_EXIT:
    if (lpMem)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbData); lpMem = NULL;
    } // End IF

    // If we failed,
    if ((!bRet) && hGlbData)
    {
        // We no longer need this storage
        MyGlobalFree (hGlbData); hGlbData = NULL;
    } // End IF

    return bRet;
} // End CreateNewTab


//***************************************************************************
//  RestoreAllEnumProc
//
//  EnumChildWindows callback to restore all MDI Child windows
//
//  lParam = unused
//***************************************************************************

BOOL CALLBACK RestoreAllEnumProc (HWND hWnd, LPARAM lParam)

{
    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // Restore the window in case it's an icon
    SendMessage (hWndMC, WM_MDIRESTORE, (WPARAM) hWnd, 0);

    return TRUE;                        // Continue enumerating
} // End RestoreAllEnumProc


//***************************************************************************
//  MFWndProc
//
//  Message processing routine for the Master Frame window
//***************************************************************************

LRESULT APIENTRY
MFWndProc (HWND hWnd,       // Window handle
           UINT message,    // Type of message
           UINT wParam,     // Additional information
           LONG lParam)     // ...

{
    RECT rcDtop;            // Rectangle for desktop
////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };
////ODSAPI ("MF: ", hWnd, message, wParam, lParam);
    switch (message)
    {
////////case WM_KEYDOWN:
////////    ODSAPI ("MF: ", hWnd, message, wParam, lParam);
////////
////////    break;
////////
        case WM_NCCREATE:       // lpcs = (LPCREATESTRUCT) lParam
            hWndMF = hWnd;

            break;                  // Continue with next handler

        case WM_CREATE:
        {
            // Create the child windows
            if (CreateChildWindows (hWnd) EQ FALSE)
                return -1;          // Stop the whole process

            // Read in the registry entries
            ReadReg (hWnd);

////        // Ensure the position is valid
////        if (MFPosCtr.x > rcDtop.right)  // If center is right of right, ...
////            MFPosCtr.x = rcDtop.right;
////        if (MFPosCtr.y > rcDtop.bottom) // If center is below the bottom, ...
////            MFPosCtr.y = rcDtop.bottom;
////
            // Check the last SizeState
            if (MFSizeState EQ SIZE_MAXIMIZED)
                nMinState = SW_MAXIMIZE;

            // Reposition the window to previous center & size
            MoveWindowPosSize (hWnd, MFPosCtr, MFSize);

            ShowWindow (hWnd, nMinState);   // Show as per request

            // Initialize window-specific resources
            MF_Create (hWnd);

            // Load a CLEAR WS
            if (!CreateNewTab (hWnd, "CLEAR WS"))
                return -1;          // Stop the whole process

            // Load a CLEAR WS
            if (!CreateNewTab (hWnd, "CLEAR WS-2"))
                return -1;          // Stop the whole process

            break;                  // Continue with next handler
        } // WM_CREATE

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
        {
            HWND hWndMD;

            // Forward this to all child windows
            for (hWndMD = GetWindow (hWndMC, GW_CHILD);
                 hWndMD;
                 hWndMD = GetWindow (hWndMD, GW_HWNDNEXT))
                PostMessage (hWndMD, message, wParam, lParam);

            PostMessage (hWndTT, message, wParam, lParam);

            // Uninitialize window-specific resources
            MF_Delete (hWnd);

            // Initialize window-specific resources
            MF_Create (hWnd);

            break;                  // Continue with next handler
        } // End WM_SYSCOLORCHANGE/WM_SETTINGCHANGE

        case WM_SIZE:                       // uFlags = (UINT) wParam
                                            // cx = LOWORD (lParam)
                                            // cy = HIWORD (lParam)
            if (wParam EQ SIZE_MAXIMIZED
             || wParam EQ SIZE_RESTORED)
            {
                SIZE S;
                HDWP hdwp;
                RECT rc;

                // Calculate the display rectangle, assuming the
                // tab control is the size of the client area.
                SetRect (&rc,
                         0, 0,
                         LOWORD (lParam), HIWORD (lParam));

                hdwp = BeginDeferWindowPos (2);

                // Size the tab control to fit the client area.
                DeferWindowPos (hdwp,           // Handle to internal structure
                                hWndTC,         // Handle of window to position
                                HWND_TOP,       // Placement-order handle
                                rc.left,        // X-position
                                rc.top,         // Y-position
                                rc.right - rc.left, // X-size
                                rc.bottom - rc.top, // Y-size
                                SWP_NOMOVE | SWP_NOZORDER); // Window-positioning flags

                // Calculate the display rectangle, assuming the
                // tab control is the size of the client area.
                TabCtrl_AdjustRect (hWndTC, FALSE, &rc);

                // Position and size the MDI Child window to fit the
                // tab control's display area
                DeferWindowPos (hdwp,           // Handle to internal structure
                                hWndMC,         // Handle of window to position
                                HWND_TOP,       // Placement-order handle
                                rc.left,        // X-position
                                rc.top,         // Y-position
                                rc.right - rc.left, // X-size
                                rc.bottom - rc.top, // Y-size
                                0);             // Window-positioning flags
                EndDeferWindowPos (hdwp);

                // Save the current Maximized or Normal state
                MFSizeState = wParam;

                S.cx = LOWORD (lParam);
                S.cy = HIWORD (lParam);

                // Convert client area size to window size
                MFSize = ClientToWindowSize (hWnd, S);

                // Because we track the center position of the window,
                // we need to modify that as well.  Note that we needn't
                // specify lParam as our MYWM_MOVE code doesn't use it.
                PostMessage (hWnd, MYWM_MOVE, 0, 0);

                // Normally we pass this message on to the MDI Child Window,
                //   so it can resize itself, however, in this case we're
                //   handling the hWndMC sizing.
                return FALSE;           // We handled the msg
            } // End IF

            break;                  // Continue with next handler

        case MYWM_MOVE:
            if (!IsIconic (hWnd))   // If we're not minimized, otherwise
            {                       // the positions are of the icon
                GetWindowRect (hWnd, &rcDtop);  // Get current window positions
                MFPosCtr.x = rcDtop.left + MFSize.cx / 2; // Convert to center coords
                MFPosCtr.y = rcDtop.top  + MFSize.cy / 2; // ...
            } // End IF

            return FALSE;           // We handled the msg

        case WM_MOVE:
            PostMessage (hWnd, MYWM_MOVE, 0, 0);

            break;                  // Continue with next handler

        case WM_NOTIFY:             // idCtrl = (int) wParam;
                                    // pnmh = (LPNMHDR) lParam;
#define lpnmh   ((LPNMHDR) lParam)

            // Split cases based upon the notification code
            switch (lpnmh->code)
            {
                case TCN_SELCHANGE:
                {
                    int          iCurTab;
                    TC_ITEM      tcItem;
                    LPPERTABDATA lpMem;

                    // Save data from the current WS into global memory
                    SaveWsData (hGlbCurTab);

                    // Hide the child windows of the outgoing tab
                    ShowHideChildWindows (hWndMC, SW_HIDE);

                    // Get the index of the currently selected tab
                    iCurTab = TabCtrl_GetCurSel (hWndTC);

                    // Get the lParam for this tab
                    tcItem.mask = TCIF_PARAM;
                    TabCtrl_GetItem (hWndTC, iCurTab, &tcItem);

                    // Save the handle
                    hGlbCurTab = (HGLOBAL) (tcItem.lParam);

                    // Lock the memory to get a ptr to it
                    lpMem = MyGlobalLock (hGlbCurTab);

                    hWndMC = lpMem->hWndMC;
                    hWndSM = lpMem->hWndSM;
                    hWndDB = lpMem->hWndDB;

                    // Restore data into the current WS from global memory
                    RestWsData (hGlbCurTab);

                    // Show the child windows of the incoming tab
                    ShowHideChildWindows (hWndMC, SW_SHOW);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbCurTab); lpMem = NULL;

                    return FALSE;       // We handled the msg
                } // End TCN_SELCHANGE

                break;
            } // End SWITCH
#undef  lpnmh

            break;                  // Continue with next handler

////////case WM_HELP:
////////{
////////    LPHELPINFO lpHI;
////////
////////    lpHI = (LPHELPINFO) lParam;
////////
////////    if (lpHI->iContextType EQ HELPINFO_WINDOW)  // Must be for control
////////    {
////////        WinHelp (lpHI->hItemHandle,
////////                 szHlpDPFE,
////////                 HELP_WM_HELP,
////////                 (DWORD) (LPVOID) aHelpIDs);
////////        fHelp = TRUE;
////////    } // End IF
////////
////////    return FALSE;           // We handled the message
////////
////////} // End WM_HELP
////////
////////case WM_CONTEXTMENU:
////////    if (hWndTreeView NE (HWND) wParam)
////////    {
////////        WinHelp ((HWND) wParam,
////////                 szHlpDPFE,
////////                 HELP_CONTEXTMENU,
////////                 (DWORD) (LPVOID) aHelpIDs);
////////        fHelp = TRUE;
////////    } // End IF
////////    return FALSE;           // We handled the message

        case WM_COMMAND:            // wNotifyCode = HIWORD(wParam); // notification code
                                    // wID = LOWORD(wParam);         // item, control, or accelerator identifier
                                    // hwndCtl = (HWND) lParam;      // handle of control
            switch (GET_WM_COMMAND_ID (wParam, lParam))
            {
                case IDM_EXIT:
                    PostMessage (hWnd, WM_CLOSE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_TILE_HORZ:
                    PostMessage (hWndMC, WM_MDITILE, MDITILE_HORIZONTAL, 0);

                    return FALSE;       // We handled the msg

                case IDM_TILE_VERT:
                    PostMessage (hWndMC, WM_MDITILE, MDITILE_VERTICAL, 0);

                    return FALSE;       // We handled the msg

                case IDM_CASCADE:
                    // In this case, we don't care whether or not there
                    // are any child windows as we're restoring them all anyway.
                    EnumChildWindows (hWndMC, RestoreAllEnumProc, 0);

                    PostMessage (hWndMC, WM_MDICASCADE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_ARRANGE_ICONS:
                    PostMessage (hWndMC, WM_MDIICONARRANGE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_SCREENFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the screen
                    MyChooseFont ();

                    return FALSE;       // We handled the msg

                case IDM_HELP_CONTENTS:
                    WinHelp (hWnd, szHlpDPFE, HELP_INDEX, 0);

                    return FALSE;       // We handled the msg

                case IDM_ABOUT:
                    DialogBox (_hInstance,
                               MAKEINTRESOURCE (IDD_ABOUT),
                               hWnd,
                               &AboutDlgProc);
                    return FALSE;       // We handled the msg
            } // End SWITCH

            break;                  // Continue with next handler ***MUST***

////    case WM_ERASEBKGND:
////        // In order to reduce screen flicker, we handle erase background
////        // in the WM_PAINT message for the child windows.
////        return TRUE;            // We erased the background

        case WM_QUERYENDSESSION:
        case WM_CLOSE:
        {
            if (fHelp && szHlpDPFE[0])   // If we used help and there's a valid .HLP file, ...
            {
                WinHelp (hWnd, szHlpDPFE, HELP_QUIT, 0); // Quit it
                fHelp = FALSE;
            } // End IF

            // Save environment variables
            SaveEnvironment ();

            // This also tells the child windows to close
            DestroyWindow (hWnd);

            break;                  // Continue with default handler
        } // End WM_CLOSE

        case WM_DESTROY:
            // Uninitialize window-specific resources
            MF_Delete (hWnd);

            // Say goodbye
            PostQuitMessage (0);

            break;                  // Continue with default handler
    } // End SWITCH

////ODSAPI ("MFZ:", hWnd, message, wParam, lParam);
    return DefFrameProc (hWnd, hWndMC, message, wParam, lParam);
} // End MFWndProc


//***************************************************************************
//  InitApplication
//
//  Initializes window data and registers window class
//***************************************************************************

BOOL InitApplication (HANDLE hInstance)     // Current instance

{
    WNDCLASSEX wc = {sizeof (WNDCLASSEX)};

    // Fill in Master Frame window class structure
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC) MFWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = hIconMF_Large;
    wc.hIconSm = hIconMF_Small;
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCE (IDR_MENU);
    wc.lpszClassName = szMFClass;

    // Register the Master Frame window class
    if (!RegisterClassEx (&wc))
    {
        MB (pszNoRegMFWndClass);
        return FALSE;
    } // End IF

    // Fill in Session Manager window class structure
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC) SMWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = GWLSM_EXTRA;
    wc.hInstance = hInstance;
    wc.hIcon = hIconSM_Large;
    wc.hIconSm = hIconSM_Small;
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject (WHITE_BRUSH);
    wc.lpszMenuName = MAKEINTRESOURCE (IDR_MENU);
    wc.lpszClassName = szSMClass;

    // Register the Session Manager window class
    if (!RegisterClassEx (&wc))
    {
        MB (pszNoRegSMWndClass);
        return FALSE;
    } // End IF

    // Fill in Debugger window class structure
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC) DBWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = GWLDB_EXTRA;
    wc.hInstance = hInstance;
    wc.hIcon = hIconDB_Large;
    wc.hIconSm = hIconDB_Small;
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject (WHITE_BRUSH);
    wc.lpszMenuName = MAKEINTRESOURCE (IDR_MENU);
    wc.lpszClassName = szDBClass;

    // Register the Debugger window class
    if (!RegisterClassEx (&wc))
    {
        MB (pszNoRegDBWndClass);
        return FALSE;
    } // End IF

    return TRUE;
} // End InitApplication


//***************************************************************************
//  UninitApplication
//
//  Uninitialize application-specific resources
//***************************************************************************

void UninitApplication (HINSTANCE hInstance)

{
} // UninitApplication


//***************************************************************************
//  InitInstance
//
//  Saves instance handle and creates main window
//***************************************************************************

BOOL InitInstance (HANDLE hInstance)

{
    // Save in global variable
    _hInstance = hInstance;

    // Allocate virtual memory for the char temporary storage
    lpszTemp = VirtualAlloc (NULL,          // Any address
                             DEF_CTEMP_MAXSIZE * sizeof (char),
                             MEM_RESERVE,
                             PAGE_READWRITE);
    if (!lpszTemp)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("InitInstance:  VirtualAlloc for <lpszTemp> failed");

        return FALSE;       // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (lpszTemp,
                  DEF_CTEMP_INITSIZE * sizeof (char),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Allocate virtual memory for the WCHAR temporary storage
    lpwszTemp = VirtualAlloc (NULL,         // Any address
                              DEF_WTEMP_MAXSIZE * sizeof (WCHAR),
                              MEM_RESERVE,
                              PAGE_READWRITE);
    if (!lpwszTemp)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("InitInstance:  VirtualAlloc for <lpwszTemp> failed");

        return FALSE;       // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (lpwszTemp,
                  DEF_WTEMP_INITSIZE * sizeof (WCHAR),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Read in the icons
    hIconMF_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_MF_LARGE));
    hIconMF_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_MF_SMALL));
    hIconSM_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_SM_LARGE));
    hIconSM_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_SM_SMALL));
    hIconDB_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_DB_LARGE));
    hIconDB_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_DB_SMALL));

    // Get keyboard accelerators
    hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE (IDR_ACCEL));

    return TRUE;
} // End InitInstance


//***************************************************************************
//  UninitInstance
//
//  Uninitialize instance-specific resources
//***************************************************************************

void UninitInstance (HINSTANCE hInstance)

{
    // Free the temporary storage
    if (lpszTemp)
    {
        VirtualFree (lpszTemp, 0, MEM_RELEASE); lpszTemp = NULL;
    } // End IF

    if (lpwszTemp)
    {
        VirtualFree (lpwszTemp, 0, MEM_RELEASE); lpwszTemp = NULL;
    } // End IF

    // No need to destroy either the cursors or icons
} // UninitInstance


//***************************************************************************
//  ParseCommandLine
//
//  Parse any command line
//***************************************************************************

BOOL ParseCommandLine (LPSTR lpCmdLine)

{
    char *p;

    // Skip over leading space
    p = SkipWhite (lpCmdLine);

    if (*p)
    {
        // Copy to buffer
        lstrcpy (szOpenFile, p);

        // Mark as present
        bCommandLine = TRUE;
    } // End IF

    return TRUE;
} // ParseCommandLine


//***************************************************************************
//  WinMain
//
//  Start the process
//***************************************************************************

int PASCAL WinMain (HINSTANCE   hInstance,
                    HINSTANCE   hPrevInstance,  // Zero under Win32
                    LPSTR       lpCmdLine,
                    int         nCmdShow)
{
    MSG  Msg;

    // Ensure that the common control DLL is loaded.
    InitCommonControls ();

    // Get the DPFE for our files
    GetFileNames (hInstance);

    // Save initial state
    nMinState = nCmdShow;

    // If there's a command line, parse it
    if (!ParseCommandLine (lpCmdLine))
        return -1;                  // Exit

    // Perform initializations that apply to a specific instance
    if (!InitInstance (hInstance))
        goto EXIT2;

    // Register the window class
    if (!InitApplication (hInstance))
        goto EXIT3;

    // Initialize ChooseFont argument here
    //   so its settings will be present
    //   for the second and subsequent time
    //   the common dialog is called.
    InitChooseFont ();

    // Create various permanent variables
    MakePermVars ();

    // Initialize Primitive Fns
    InitPrimFns ();

    // Create the Master Frame window
    hWndMF =
        CreateWindowEx (0L,                             // Extended styles
                        szMFClass,                      // Class
                        szMFTitle,                      // Title
                        WS_OVERLAPPEDWINDOW,            // Style
                        CW_USEDEFAULT, CW_USEDEFAULT,   // X- and Y-coord
                        CW_USEDEFAULT, CW_USEDEFAULT,   // X- and Y-size
                        hWndTC,                         // Parent window
                        NULL,                           // Menu
                        _hInstance,                     // Instance
                        NULL);                          // No extra data
    if (hWndMF EQ NULL)
    {
        MB (pszNoCreateMFWnd);
        return FALSE;
    } // End IF

    // Make the window visible; update its client area
    ShowWindow (hWndMF, nCmdShow);
    UpdateWindow (hWndMF);

    // Main message loop
    while (GetMessage(&Msg, NULL, 0, 0))
    {
        // Handle MDI messages and accelerators
        if (!TranslateMDISysAccel (hWndMC, &Msg)
         && ((!hAccel) || !TranslateAccelerator (hWndMF, hAccel, &Msg)))
        {
            TranslateMessage (&Msg);
            DispatchMessage (&Msg);
        } // End IF
    } // End WHILE
    // GetMessage returned FALSE for a Quit message
EXIT3:
    UninitApplication (hInstance);
EXIT2:
    UninitInstance (hInstance);

    return 0;
} // End WinMain


//***************************************************************************
//  End of File: main.c
//***************************************************************************
