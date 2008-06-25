//***************************************************************************
//  NARS2000 -- Free Software APL Interpreter
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#define COMPILE_MULTIMON_STUBS
#define WINVER       0x0500 // Needed for WINUSER.H definitions
#define _WIN32_WINNT 0x0500 // ...

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <windowsx.h16>
//#include <multimon.h>   // Multiple monitor support
#include <limits.h>
#include <direct.h>
#include <float.h>
#include <wininet.h>

#include "uniscribe.h"
#include "main.h"
#include "resdebug.h"
#include "resource.h"
#include "editctrl.h"
#include "unitranshdr.h"

#define DEFINE_VARS
#define DEFINE_VALUES
#define DEFINE_ENUMS
#include "externs.h"
#undef  DEFINE_ENUMS
#undef  DEFINE_VALUES
#undef  DEFINE_VARS

#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//************************** Data Area **************************************

LRESULT WINAPI EditWndProcW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


typedef struct tagENUMSETFONTW
{
    LPWCHAR lpwClassName;
    HFONT   hFont;
} ENUMSETFONTW, *LPENUMSETFONTW;

typedef struct tagENUMPASSMSG
{
    UINT   message;
    WPARAM wParam;
    LPARAM lParam;
} ENUMPASSMSG, *LPENUMPASSMSG;

typedef struct tagUPDATESDLGSTR
{
    LPWCHAR lpFileVer,          // Ptr to file version string
            lpWebVer;           // Ptr to web  ...
} UPDATESDLGSTR, *LPUPDATESDLGSTR;

int  nMinState,                         // Minimized state as per WinMain
     iScrollSize;                       // Width of a vertical scrollbar
BOOL fHelp = FALSE,                     // TRUE iff we displayed help
     bCommandLine = FALSE;              // ...      there is a filename on the command line

HMODULE user32_module;                  // Needed by WineHQ\EDITCTRL.C

HICON hIconMF_Large, hIconMF_Small,     // Icon handles
      hIconSM_Large, hIconSM_Small,
#ifdef DEBUG
      hIconDB_Large, hIconDB_Small,
#endif
      hIconFE_Large, hIconFE_Small,
      hIconME_Large, hIconME_Small,
      hIconVE_Large, hIconVE_Small,
      hIconCC_Large, hIconCC_Small,
      hIconClose;

#define  MFWNDCLASS          "MFClass"                                      // Master Frame Window class
#define LMFWNDCLASS         L"MFClass"                                      // Master Frame Window class

WCHAR wszMFTitle[]          = WS_APPNAME WS_APPEND_DEBUG,                   // Master frame window title
      wszCCTitle[]          = WS_APPNAME L" Crash Control Window" WS_APPEND_DEBUG, // Crash Control window title
      wszTCTitle[]          = WS_APPNAME L" Tab Control Window" WS_APPEND_DEBUG;// Tab Control ... (for debugging purposes only)

char pszNoRegMFWndClass[]   = "Unable to register window class <" MFWNDCLASS ">.",
     pszNoRegSMWndClass[]   = "Unable to register window class <" SMWNDCLASS ">.",
#ifdef DEBUG
     pszNoRegDBWndClass[]   = "Unable to register window class <" DBWNDCLASS ">.",
#endif
     pszNoRegFEWndClass[]   = "Unable to register window class <" FEWNDCLASS ">.",
     pszNoRegMEWndClass[]   = "Unable to register window class <" MEWNDCLASS ">.",
     pszNoRegVEWndClass[]   = "Unable to register window class <" VEWNDCLASS ">.",
     pszNoRegECWndClass[]   = "Unable to register window class <" ECWNDCLASS ">.",
     pszNoRegCCWndClass[]   = "Unable to register window class <" CCWNDCLASS ">.";

char pszNoCreateMFWnd[]     = "Unable to create Master Frame window",
     pszNoCreateTCWnd[]     = "Unable to create Tab Control window",
     pszNoCreateTTWnd[]     = "Unable to create ToolTip window",
     pszNoCreateCCWnd[]     = "Unable to create Crash Control window";


//***************************************************************************
//  $EnumCallbackPassMsg
//
//  EnumChildWindows callback to pass a message to all child
//    windows of MDI Clients
//***************************************************************************

BOOL CALLBACK EnumCallbackPassMsg
    (HWND   hWndMC,         // Handle to child window (MDI Client)
     LPARAM lParam)         // Application-defined value

{
    HWND hWndChild;

    // Forward this to all child windows of all MDI Clients
    for (hWndChild = GetWindow (hWndMC, GW_CHILD);
         hWndChild;
         hWndChild = GetWindow (hWndChild, GW_HWNDNEXT))
    {
        // When an MDI child window is minimized, Windows creates two windows: an
        // icon and the icon title.  The parent of the icon title window is set to
        // the MDI client window, which confines the icon title to the MDI client
        // area.  The owner of the icon title is set to the MDI child window.
        if (GetWindow (hWndChild, GW_OWNER)) // If it's an icon title window, ...
            continue;                       // skip it, and continue enumerating

        PostMessageW (hWndChild, ((LPENUMPASSMSG) lParam)->message,
                                 ((LPENUMPASSMSG) lParam)->wParam,
                                 ((LPENUMPASSMSG) lParam)->lParam);
    } // End FOR

    return TRUE;        // Keep on truckin'
} // End EnumCallbackPassMsg


//***************************************************************************
//  $EnumCallbackSetFontW
//
//  EnumChildWindows callback to set a window's font
//***************************************************************************

BOOL CALLBACK EnumCallbackSetFontW
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    WCHAR wszTemp[32];

    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // Get the window's class name
    GetClassNameW (hWnd, wszTemp, sizeof (wszTemp) / sizeof (wszTemp[0]));

#define lpEnumSetFontW  ((LPENUMSETFONTW) lParam)

    // If this is the matching class name,
    //   set the new font in place and redraw.
    if (lstrcmpiW (lpEnumSetFontW->lpwClassName, wszTemp) EQ 0)
        SendMessageW (hWnd, WM_SETFONT, (WPARAM) lpEnumSetFontW->hFont, MAKELPARAM (TRUE, 0));

    return TRUE;        // Keep on truckin'
#undef  lpEnumSetFontW
} // End EnumCallbackSetFontW


//***************************************************************************
//  $InitChooseFont
//
//  Initialize CHOOSEFONT values
//***************************************************************************

void InitChooseFont
    (LPCHOOSEFONT lpcf,
     LPLOGFONT    lplf,
     int          iPtSize,
     BOOL         bPrinter)                 // TRUE iff this is a printer font
{
    HDC hDC;
    int iLogPixelsY;

    // Zero the struc
    ZeroMemory (lpcf, sizeof (CHOOSEFONT));

    if (bPrinter)
        // Create a printer device context
        hDC = GetPrinterDC ();
    else
        // Create a screen device context
        hDC = MyGetDC (HWND_DESKTOP);

    iLogPixelsY = GetDeviceCaps (hDC, LOGPIXELSY);
    lplf->lfHeight = -MulDiv (iPtSize, iLogPixelsY, 72);

    if (bPrinter)
        DeleteDC (hDC);
    else
        MyReleaseDC (HWND_DESKTOP, hDC); hDC = NULL;

    lpcf->lStructSize = sizeof (CHOOSEFONT);
////lpcf->hDC =                     // Only w/CF_PRINTERFONTS
    lpcf->lpLogFont  = lplf;
    lpcf->iPointSize = iPtSize * 10;// Font point size in 1/10ths
    lpcf->Flags = 0
                | CF_INITTOLOGFONTSTRUCT
                | CF_FORCEFONTEXIST
                | CF_SCREENFONTS
                  ;
////lpcf->rgbColors  =              // Only w/CF_EFFECTS
////lpcf->lCustData  =              // Only w/CF_ENABLEHOOK
////lpcf->lpfnHook   =              // Only w/CF_ENABLEHOOK
////lpcf->lpTemplateName =          // Only w/CF_ENABLETEMPLATE
////lpcf->hInstance  =              // Only w/CF_ENABLETEMPLATE
////lpcf->lpszStyle  =              // Only w/CF_USESTYLE
////lpcf->nFontType  =              // Output only
////lpcf->nSizeMin   =              // Only w/CF_LIMITSIZE
////lpcf->nSizeMax   =              // Only w/CF_LIMITSIZE
} // End InitChooseFont


//***************************************************************************
//  $MyChooseFont
//
//  Choose a font for the various windows.
//***************************************************************************

void MyChooseFont
    (LPCHOOSEFONT lpcf,
     void (*CreateNewFont) (void))

{
    if (ChooseFont (lpcf))      // Choose it
    {
        // Create a new font for the corresponding window
        (*CreateNewFont) ();
    } // End IF
} // End MyChooseFont


//***************************************************************************
//  $CreateNewFontCom
//
//  Subroutine to CreateNewFontxx to set various variables
//***************************************************************************

void CreateNewFontCom
    (HFONT       *lphFont,              // Ptr to in HFONT to create
     LPLOGFONT    lplf,                 // Ptr to in/out LOGFONT to set
     LPCHOOSEFONT lpcf,                 // Ptr to in CHOOSEFONT with iPointSize
     LPTEXTMETRIC lptm,                 // Ptr to in TEXTMETRICs
     long        *lpcxAveChar,          // Ptr to out avg width (may be NULL)
     long        *lpcyAveChar,          // Ptr to out avg height (may be NULL)
     HDC          hDC)                  // Handle to device context if not default (may be NULL)

{
    HDC   hDCTmp;
    HFONT hFontOld;
    long  cyAveChar;
    int   iLogPixelsY;

    // Delete the previous handle (if any)
    if (*lphFont)
    {
        // Delete the font handle
        MyDeleteObject (*lphFont); *lphFont = NULL;
    } // End IF

    // Create the font
    *lphFont = MyCreateFontIndirect (lplf);

    // Get a new device context or use the give one
    hDCTmp = hDC ? hDC : MyGetDC (HWND_DESKTOP);

    // Get the # pixels per vertical inch
    iLogPixelsY = GetDeviceCaps (hDCTmp, LOGPIXELSY);

    // Select the enwly created font
    hFontOld = SelectObject (hDCTmp, *lphFont);

    // Get the text metrics for this font
    GetTextMetrics (hDCTmp, lptm);

    // Restore the old font
    SelectObject (hDCTmp, hFontOld);

    // If no given device context, ...
    if (hDC EQ NULL)
    {
        // Release the one we created
        MyReleaseDC (HWND_DESKTOP, hDCTmp); hDCTmp = NULL;
    } // End IF

    // New height
    cyAveChar = MulDiv (lpcf->iPointSize / 10, iLogPixelsY, 72);

    // New width from TextMetrics
    lplf->lfWidth = lptm->tmAveCharWidth;

    // New width (same aspect ratio as old)
    if (lpcxAveChar)
        *lpcxAveChar = MulDiv (lplf->lfWidth, cyAveChar, -lplf->lfHeight);

    // New height
    lplf->lfHeight = -cyAveChar;

    if (lpcyAveChar)
        *lpcyAveChar = lptm->tmHeight;

    // Now that we've caclulated the correct height & width,
    //   delete the font and re-create it
    DeleteObject (*lphFont); *lphFont = NULL;

    // Re-create the font
    *lphFont = MyCreateFontIndirect (lplf);
} // End CreateNewFontCom


//***************************************************************************
//  $CreateNewFontTC
//
//  Create a new font for the TC window.
//***************************************************************************

void CreateNewFontTC
    (void)

{
    // Call common routine to set various variables
    CreateNewFontCom (&hFontTC,
                      &lfTC,
                      &cfTC,
                      &tmTC,
                      &cxAveCharTC,
                      &cyAveCharTC,
                       NULL);
    // Tell the TC about the new font
    SendMessageW (hWndTC, WM_SETFONT, (WPARAM) hFontTC, MAKELPARAM (TRUE, 0));

    // Repaint the TC labels
    InvalidateRect (hWndTC, NULL, TRUE);
} // End CreateNewFontTC


//***************************************************************************
//  $CreateNewFontCC
//
//  Create a new font for the CC window.
//***************************************************************************

void CreateNewFontCC
    (void)

{
    // Call common routine to set various variables
    CreateNewFontCom (&hFontCC,
                      &lfCC,
                      &cfCC,
                      &tmCC,
                       NULL,
                       NULL,
                       NULL);
    // Tell the CC about the new font
    SendMessageW (hWndCC, WM_SETFONT, (WPARAM) hFontCC, MAKELPARAM (TRUE, 0));

    // Repaint the CC text
    InvalidateRect (hWndCC, NULL, TRUE);
} // End CreateNewFontCC


//***************************************************************************
//  $CreateNewFontSM
//
//  Create a new font for the SM windows.
//***************************************************************************

void CreateNewFontSM
    (void)

{
    ENUMSETFONTW enumSetFontW;
#ifndef UNISCRIBE
    LOGFONT      lfAlt;
    TEXTMETRIC   tmAlt;
#endif

    // Call common routine to set various variables
    CreateNewFontCom (&hFontSM,
                      &lfSM,
                      &cfSM,
                      &tmSM,
                      &cxAveCharSM,
                      &cyAveCharSM,
                       NULL);
    // Initialize the struct
    enumSetFontW.lpwClassName = LSMWNDCLASS;
    enumSetFontW.hFont        = hFontSM;

    // Refont the SM windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFontW, (LPARAM) &enumSetFontW);

#ifndef UNISCRIBE
    // Copy the SM LOGFONT & TEXTMETRICS
    lfAlt = lfSM;
    tmAlt = tmSM;

    // Change the font name
    lstrcpy (lfAlt.lfFaceName, DEF_ASFONTNAME);

    // Re-specify the alternate SM font
    CreateNewFontCom (&hFontAlt,
                      &lfAlt,
                      &cfSM,
                      &tmAlt,
                       NULL,
                       NULL,
                       NULL);
#endif
#ifdef DEBUG
    // Initialize the struct
    enumSetFontW.lpwClassName = LDBWNDCLASS;
    enumSetFontW.hFont        = hFontSM;

    // Refont the DB windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFontW, (LPARAM) &enumSetFontW);
#endif
} // End CreateNewFontSM


//***************************************************************************
//  $CreateNewFontPR
//
//  Create a new font for the Printer
//***************************************************************************

void CreateNewFontPR
    (void)

{
    HDC hDC;                    // Handle to the printer device context

    // Create a printer device context
    hDC = GetPrinterDC ();

    // Call common routine to set various variables
    CreateNewFontCom (&hFontPR,
                      &lfPR,
                      &cfPR,
                      &tmPR,
                      &cxAveCharPR,
                      &cyAveCharPR,
                       hDC);
    // We no longer need this resource
    DeleteDC (hDC);
} // End CreateNewFontPR


//***************************************************************************
//  $GetPrinterDC
//
//  Create a printer Device Context
//***************************************************************************

HDC GetPrinterDC
    (void)

{
    char  szTemp[1024];         // Buffer for printer name
    DWORD dwLen = sizeof (szTemp);

    // Get info about the default printer
    GetDefaultPrinter (szTemp, &dwLen);

    // Create a printer device context
    return CreateDC ("WINSPOOL", szTemp, NULL, NULL);
} // End GetPrinterDC


//***************************************************************************
//  $CreateNewFontFE
//
//  Create a new font for the FE windows.
//***************************************************************************

void CreateNewFontFE
    (void)

{
    ENUMSETFONTW enumSetFontW;

    // Call common routine to set various variables
    CreateNewFontCom (&hFontFE,
                      &lfFE,
                      &cfFE,
                      &tmFE,
                      &cxAveCharFE,
                      &cyAveCharFE,
                       NULL);
    // Initialize the struct
    enumSetFontW.lpwClassName = LFEWNDCLASS;
    enumSetFontW.hFont        = hFontFE;

    // Refont the FE windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFontW, (LPARAM) &enumSetFontW);
} // End CreateNewFontFE


//***************************************************************************
//  $CreateNewFontME
//
//  Create a new font for the ME windows.
//***************************************************************************

void CreateNewFontME
    (void)

{
    ENUMSETFONTW enumSetFontW;

    // Call common routine to set various variables
    CreateNewFontCom (&hFontME,
                      &lfME,
                      &cfME,
                      &tmME,
                      &cxAveCharME,
                      &cyAveCharME,
                       NULL);
    // Initialize the struct
    enumSetFontW.lpwClassName = LMEWNDCLASS;
    enumSetFontW.hFont        = hFontME;

    // Refont the ME windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFontW, (LPARAM) &enumSetFontW);
} // End CreateNewFontME


//***************************************************************************
//  $CreateNewFontVE
//
//  Create a new font for the VE windows.
//***************************************************************************

void CreateNewFontVE
    (void)

{
    ENUMSETFONTW enumSetFontW;

    // Call common routine to set various variables
    CreateNewFontCom (&hFontVE,
                      &lfVE,
                      &cfVE,
                      &tmVE,
                      &cxAveCharVE,
                      &cyAveCharVE,
                       NULL);
    // Initialize the struct
    enumSetFontW.lpwClassName = LVEWNDCLASS;
    enumSetFontW.hFont        = hFontVE;

    // Refont the VE windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFontW, (LPARAM) &enumSetFontW);
} // End CreateNewFontVE


//***************************************************************************
//  $CreateToolTip
//
//  Creates the ToolTip window and initializes it.
//***************************************************************************

HWND CreateToolTip
    (HWND hWndParent)

{
    HWND hWnd;

    // Create the ToolTip window
    hWnd =
      CreateWindowExW (0L,              // Extended styles
#ifdef QTT
                       WC_TOOLTIPS,     // Class for Qcontrols
#else
                       TOOLTIPS_CLASSW, // Class for MS Controls
#endif
                       NULL,            // Window title
                       0
                     | TTS_NOANIMATE
                     | TTS_ALWAYSTIP
                       ,                // Styles
                       CW_USEDEFAULT,   // X-coord
                       CW_USEDEFAULT,   // Y-...
                       CW_USEDEFAULT,   // Width
                       CW_USEDEFAULT,   // Height
                       NULL,            // Parent window
                       NULL,            // Menu
                       _hInstance,      // Instance
                       NULL);           // No extra data
    if (hWnd EQ NULL)
    {
        MB (pszNoCreateTTWnd);
        return NULL;        // Stop the whole process
    } // End IF

    return hWnd;
} // End CreateToolTip


//***************************************************************************
//  $CreateChildWindows
//
//  Create the child windows under the parent window
//***************************************************************************

BOOL CreateChildWindows
    (HWND hWnd)

{
    RECT rc;                // Rectangle for setting size of window

    // Get the size and position of the parent window.
    GetClientRect (hWnd, &rc);

    // Create the ToolTip window first so that
    // the other windows can reference it.
    hWndTT = CreateToolTip (hWnd);

    if (hWndTT EQ NULL)
        return FALSE;       // Stop the whole process

    // Create the crash control window
    hWndCC =
      CreateWindowExW (0L,                  // Extended styles
                       LCCWNDCLASS,         // Class
                       wszCCTitle,          // Window title (for debugging purposes only)
                       0
                     | WS_OVERLAPPEDWINDOW
                       ,                    // Styles
                       CW_USEDEFAULT, CW_USEDEFAULT,    // X- and Y-coord
                       CW_USEDEFAULT, CW_USEDEFAULT,    // X- and Y-size
                       hWnd,                // Parent window
                       NULL,                // Menu
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWndCC EQ NULL)
    {
        MB (pszNoCreateCCWnd);
        return FALSE;
    } // End IF

    // Create the tab control window
    hWndTC =
      CreateWindowExW (0L,                  // Extended styles
                       WC_TABCONTROLW,      // Class
                       wszTCTitle,          // Window title (for debugging purposes only)
                       0
                     | TCS_OWNERDRAWFIXED
                     | WS_CHILD
                     | WS_CLIPSIBLINGS
                     | WS_VISIBLE
                       ,                    // Styles
                       rc.left,             // X-coord
                       rc.top,              // Y-coord
                       rc.right - rc.left,  // X-size
                       rc.bottom - rc.top,  // Y-size
                       hWnd,                // Parent window
                       NULL,                // Menu
                       _hInstance,          // Instance
                       NULL);               // No extra data
    if (hWndTC EQ NULL)
    {
        MB (pszNoCreateTCWnd);
        return FALSE;
    } // End IF

    // Subclass the Tab Control so we can handle some of its messages
    (HANDLE_PTR) lpfnOldTabCtrlWndProc =
      SetWindowLongPtrW (hWndTC,
                         GWL_WNDPROC,
                         (__int3264) (HANDLE_PTR) (WNDPROC) &LclTabCtrlWndProc);
    // Show and paint the window
    ShowWindow (hWndTC, SW_SHOWNORMAL);
    UpdateWindow (hWndTC);

    cfTC.hwndOwner = hWndTC;

    // Tell it about our tooltip control
    TabCtrl_SetToolTips (hWndTC, hWndTT);

    return TRUE;            // Tell 'em it worked
} // End CreateChildWindows


//***************************************************************************
//  $EnumCallbackRestoreAll
//
//  EnumChildWindows callback to restore all MDI Child windows
//
//  lParam = unused
//***************************************************************************

BOOL CALLBACK EnumCallbackRestoreAll
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    HWND hWndMC;            // Window handle to MDI Client

    // Get the window handle to the MDI Client
    hWndMC = GetParent (hWnd);

    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // Restore the window in case it's an icon
    SendMessageW (hWndMC, WM_MDIRESTORE, (WPARAM) hWnd, 0);

    return TRUE;                        // Continue enumerating
} // End EnumCallbackRestoreAll


//***************************************************************************
//  $MF_Create
//
//  Perform window-specific initialization
//***************************************************************************

void MF_Create
    (HWND hWnd)

{
    // Read the default scrollbar width
    iScrollSize = GetSystemMetrics (SM_CXVSCROLL);
} // End MF_Create


//***************************************************************************
//  $MF_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void MF_Delete
    (HWND hWnd)

{
} // End MF_Delete


//***************************************************************************
//  $MFWndProc
//
//  Message processing routine for the Master Frame window
//***************************************************************************

LRESULT APIENTRY MFWndProc
    (HWND hWnd,         // Window handle
     UINT message,      // Type of message
     UINT wParam,       // Additional information
     LONG lParam)       // ...

{
    RECT         rcDtop;        // Rectangle for desktop
    HWND         hWndActive,
                 hWndMC,
                 hWndSM;
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };
////LCLODSAPI ("MF: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_NCCREATE:       // lpcs = (LPCREATESTRUCTW) lParam
            hWndMF = hWnd;

            hMenuSM = LoadMenu (_hInstance, MAKEINTRESOURCE (IDR_SMMENU));
            hMenuFE = LoadMenu (_hInstance, MAKEINTRESOURCE (IDR_FEMENU));
////////////hMenuME = LoadMenu (_hInstance, MAKEINTRESOURCE (IDR_MEMENU));
////////////hMenuVE = LoadMenu (_hInstance, MAKEINTRESOURCE (IDR_VEMENU));

            hMenuSMWindow = GetSubMenu (hMenuSM, IDMPOS_SM_WINDOW);
            hMenuFEWindow = GetSubMenu (hMenuFE, IDMPOS_FE_WINDOW);
////////////hMenuMEWindow = GetSubMenu (hMenuME, IDMPOS_ME_WINDOW);
////////////hMenuVEWindow = GetSubMenu (hMenuVE, IDMPOS_VE_WINDOW);

            break;                  // Continue with next handler

        case WM_CREATE:
            // Create the child windows
            if (CreateChildWindows (hWnd) EQ FALSE)
                return -1;          // Stop the whole process

            // Read in window-specific .ini file entries
            ReadIniFileWnd (hWnd);

            // *************** Bitmaps *********************************
            hBitMapLineCont = MyLoadBitmap (_hInstance, MAKEINTRESOURCE (IDB_LINECONT));
            if (hBitMapLineCont)
            {
                GetObject (hBitMapLineCont, sizeof (BITMAP), (LPSTR) &bmLineCont);

                iLCWidth = 2 + bmLineCont.bmWidth + 2;  // Width of line continuation column
            } // End IF

            // *************** Image Lists *****************************
            hImageList =
              ImageList_Create (IMAGE_CX,       // Common width in pixels
                                IMAGE_CY,       // ...    height ...
                                0
                              | ILC_COLOR32
                              | ILC_MASK,       // Flags
                                1,              // Max # images
                                0);             // # images by which the list can grow
            if (!hImageList)
                return -1;          // Stop the whole process

            // Add the Close button icon to the image list
            ImageList_AddIcon (hImageList, hIconClose);

            // Assign the image list to the tab control
            SendMessageW (hWndTC, TCM_SETIMAGELIST, 0, (LPARAM) hImageList);

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
            UpdateWindow (hWnd);

            // Initialize window-specific resources
            MF_Create (hWnd);

////////////// Allocate per tab data as a dummy holder
//////////////   so that the first call to SaveWsData
//////////////   has something to save into.
////////////hGlbPTD = MyGlobalAlloc (GHND, sizeof (PERTABDATA));
////////////if (!hGlbPTD)
////////////    return -1;          // Stop the whole process

            // Load a CLEAR WS or the worksapce named on the command line
            if (!CreateNewTab (hWnd,
                               wszLoadFile,
                               TabCtrl_GetItemCount (hWndTC),
                               wszLoadFile[0] NE L'\0'))
                return -1;          // Stop the whole process

            // *************** Fonts ***********************************
            // Create a new font for various windows
            CreateNewFontTC ();
            CreateNewFontSM ();
            CreateNewFontPR ();
            CreateNewFontCC ();
            CreateNewFontFE ();
            CreateNewFontME ();
            CreateNewFontVE ();

            break;                  // Continue with next handler

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
        {
            ENUMPASSMSG enumPassMsg;

            // Save parameters in struc
            enumPassMsg.message = message;
            enumPassMsg.wParam  = wParam;
            enumPassMsg.lParam  = lParam;

            // Enumerate all child windows of the Tab Control
            //   which should give us all MDI Client windows
            EnumChildWindows (hWndTC, &EnumCallbackPassMsg, (LPARAM) &enumPassMsg);

            // Tell the Tooltip window about it
            PostMessageW (hWndTT, message, wParam, lParam);

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
                int  rcLeft, rcRight, rcBottom;

                // Calculate the display rectangle, assuming the
                // tab control is the size of the client area.
                SetRect (&rc,
                         0, 0,
                         LOWORD (lParam), HIWORD (lParam));

                hdwp = BeginDeferWindowPos (2);

                // Size the tab control to fit the client area.
                DeferWindowPos (hdwp,           // Handle to internal structure
                                hWndTC,         // Handle of window to position
                                NULL,           // Placement-order handle
                                rc.left,        // X-position
                                rc.top,         // Y-position
                                rc.right - rc.left, // X-size
                                rc.bottom - rc.top, // Y-size
                                SWP_NOMOVE | SWP_NOZORDER); // Window-positioning flags

                // Calculate the display rectangle, assuming the
                // tab control is the size of the client area.
                // Because I don't like the look of the tab control border,
                //   the following code saves and restores all but the
                //   top border (where the tabs are).
                rcLeft    = rc.left;
                rcRight   = rc.right;
                rcBottom  = rc.bottom;
                TabCtrl_AdjustRect (hWndTC, FALSE, &rc);
                rc.left   = rcLeft;
                rc.right  = rcRight;
                rc.bottom = rcBottom;

                // Get the window handle of the currently active MDI Client
                hWndMC = GetActiveMC (hWndTC);

                // Position and size the MDI Child window to fit the
                // tab control's display area
                DeferWindowPos (hdwp,           // Handle to internal structure
                                hWndMC,         // Handle of window to position
                                NULL,           // Placement-order handle
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
                PostMessageW (hWnd, MYWM_MOVE, 0, 0);

                // Normally we pass this message on to the MDI Child Window,
                //   so it can resize itself, however, in this case we're
                //   handling the hWndMC sizing.
                return FALSE;           // We handled the msg
            } // End IF

            break;                  // *MUST* pass on to DefFrameProcW

        case MYWM_MOVE:
            if (!IsIconic (hWnd))   // If we're not minimized, otherwise
            {                       // the positions are of the icon
                GetWindowRect (hWnd, &rcDtop);  // Get current window positions
                MFPosCtr.x = rcDtop.left + MFSize.cx / 2; // Convert to center coords
                MFPosCtr.y = rcDtop.top  + MFSize.cy / 2; // ...
            } // End IF

            return FALSE;           // We handled the msg

        case WM_MOVE:
            PostMessageW (hWnd, MYWM_MOVE, 0, 0);

            break;                  // Continue with next handler

        case WM_NOTIFY:             // idCtrl = (int) wParam;
                                    // pnmh = (LPNMHDR) lParam;
#define lpnmh   (*(LPNMHDR *) &lParam)

            // Split cases based upon the notification code
            switch (lpnmh->code)
            {
                case TTN_NEEDTEXTW:     // idTT = (int) wParam;
                                        // lpttt = (LPTOOLTIPTEXTW) lParam;
                {
                    static WCHAR TooltipText[_MAX_PATH];
                    LPAPLCHAR    lpMemWSID;     // Ptr to []WSID global memory
#define lpttt   (*(LPTOOLTIPTEXTW *) &lParam)

                    // Get a ptr to the ws name
                    lpMemWSID = PointToWsName ((__int3264) (HANDLE_PTR) lpttt->hdr.idFrom);
#ifndef DEBUG
                    // Return a ptr to the stored tooltip text
                    lstrcpyW (TooltipText, lpMemWSID);
#else
                    // Get the PerTabData global memory handle
                    hGlbPTD = GetPerTabHandle ((__int3264) (HANDLE_PTR) lpttt->hdr.idFrom); Assert (hGlbPTD NE NULL);

                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    // Return a ptr to the stored tooltip text
                    wsprintfW (TooltipText,
                               L"hWndMC=%p: %s",
                               lpMemPTD->hWndMC,
                               lpMemWSID);
                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
#endif
                    // Return the ptr to the caller
                    lpttt->lpszText = TooltipText;
#undef  lpttt
                    return FALSE;
                } // End TTN_NEEDTEXTW

                case TCN_SELCHANGING:   // idTabCtl = (int) LOWORD(wParam);
                                        // lpnmhdr = (LPNMHDR) lParam;
                    DestroyCaret ();    // 'cause we just lost the focus

                    // If the user clicked on the close button,
                    //   disallow this change so as to avoid
                    //   screen flicker
                    if (ClickOnClose ())
                        return TRUE;
                    // Save the index of the outgoing tab
                    gLstTabID = TranslateTabIndexToID (TabCtrl_GetCurSel (hWndTC));

                    return FALSE;

                case TCN_SELCHANGE:     // idTabCtl = (int) LOWORD(wParam);
                                        // lpnmhdr = (LPNMHDR) lParam;
                {
                    int iCurTabIndex,       // Index of the current tab
                        iLstTabIndex;       // Index of the previous tab

                    // Get the index of the incoming tab
                    iCurTabIndex = TabCtrl_GetCurSel (hWndTC);

                    // Get the index of the previous tab
                    iLstTabIndex = TranslateTabIDToIndex (gLstTabID);

                    // Save the index of the incoming tab
                    gCurTabID = TranslateTabIndexToID (iCurTabIndex);

                    // Hide the child windows of the outgoing tab
                    if (iLstTabIndex NE -1 && iLstTabIndex NE iCurTabIndex)
                        ShowHideChildWindows (GetWndMC (iLstTabIndex), FALSE);
                    // Show the child windows of the incoming tab
                    if (iCurTabIndex NE -1)
                        ShowHideChildWindows (GetWndMC (iCurTabIndex), TRUE);
                    return FALSE;       // We handled the msg
                } // End TCN_SELCHANGE

                default:
                    break;
            } // End SWITCH

            break;                  // Continue with next handler
#undef  lpnmh

        case WM_DRAWITEM:           // idCtl = (UINT) wParam;             // control identifier
                                    // lpdis = (LPDRAWITEMSTRUCT) lParam; // item-drawing information
        {
#define lpdis   (*(LPDRAWITEMSTRUCT *) &lParam)

            // Ensure this message is for our tab control
            if (lpdis->CtlType NE ODT_TAB)
                break;

            // Split cases based upon the item action
            switch (lpdis->itemAction)
            {
                case ODA_DRAWENTIRE:
                    // Draw the tab
                    DrawTab (lpdis->hDC,
                             lpdis->itemID,
                            &lpdis->rcItem);
                    break;

////////////////case ODA_FOCUS:     // These actions don't appear to occur with
////////////////                    //   an owner-drawn tab ctrl
////////////////case ODA_SELECT:    // ...
////////////////    break;
            } // End SWITCH

            break;
#undef  lpdis
        } // End WM_DRAWITEM

////////case WM_HELP:
////////{
////////    LPHELPINFO lpHI;
////////
////////    lpHI = (LPHELPINFO) lParam;
////////
////////    if (lpHI->iContextType EQ HELPINFO_WINDOW)  // Must be for control
////////    {
////////        WinHelpW (lpHI->hItemHandle,
////////                  wszHlpDPFE,
////////                  HELP_WM_HELP,
////////                  (DWORD) (LPVOID) aHelpIDs);
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
////////        WinHelpW ((HWND) wParam,
////////                  wszHlpDPFE,
////////                  HELP_CONTEXTMENU,
////////                  (DWORD) (LPVOID) aHelpIDs);
////////        fHelp = TRUE;
////////    } // End IF
////////    return FALSE;           // We handled the message

        case WM_COMMAND:            // wNotifyCode = HIWORD (wParam); // notification code
                                    // wID = LOWORD (wParam);         // item, control, or accelerator identifier
                                    // hwndCtrl = (HWND) lParam;      // handle of control
            // Get the window handle of the currently active MDI Client
            hWndMC = GetActiveMC (hWndTC);

            // Get the handle of the active MDI window
            hWndActive = (HWND) SendMessageW (hWndMC, WM_MDIGETACTIVE, 0, 0);

            switch (GET_WM_COMMAND_ID (wParam, lParam))
            {
                case IDM_EXIT:
                    PostMessageW (hWnd, WM_CLOSE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_UNDO:
                    SendMessageW (hWndActive, WM_UNDO, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_REDO:
                    SendMessageW (hWndActive, MYWM_REDO, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_CUT:
                    SendMessageW (hWndActive, WM_CUT, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_COPY:
                    SendMessageW (hWndActive, WM_COPY, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE:
                    SendMessageW (hWndActive, WM_PASTE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_APLWIN:
                    SendMessageW (hWndActive, MYWM_PASTE_APL, UNITRANS_APLWIN, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_APL2:
                    SendMessageW (hWndActive, MYWM_PASTE_APL, UNITRANS_APL2, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_ISO:
                    SendMessageW (hWndActive, MYWM_PASTE_APL, UNITRANS_ISO, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_PC3270:
                    SendMessageW (hWndActive, MYWM_PASTE_APL, UNITRANS_PC3270, 0);

                    return FALSE;       // We handled the msg

                case IDM_DELETE:
                    SendMessageW (hWndActive, WM_CLEAR, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_SELECTALL:
                    SendMessageW (hWndActive, MYWM_SELECTALL, 0, (LPARAM) -1);

                    return FALSE;       // We handled the msg

                case IDM_TILE_HORZ:
                    PostMessageW (hWndMC, WM_MDITILE, MDITILE_HORIZONTAL, 0);

                    return FALSE;       // We handled the msg

                case IDM_TILE_VERT:
                    PostMessageW (hWndMC, WM_MDITILE, MDITILE_VERTICAL, 0);

                    return FALSE;       // We handled the msg

                case IDM_CASCADE:
                    // In this case, we don't care whether or not there
                    // are any child windows as we're restoring them all anyway.
                    EnumChildWindows (hWndMC, EnumCallbackRestoreAll, 0);

                    PostMessageW (hWndMC, WM_MDICASCADE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_ARRANGE_ICONS:
                    PostMessageW (hWndMC, WM_MDIICONARRANGE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_TCFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Tab Control labels
                    MyChooseFont (&cfTC, &CreateNewFontTC);

                    return FALSE;       // We handled the msg

                case IDM_SMFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Session Manager window
                    MyChooseFont (&cfSM, &CreateNewFontSM);

                    return FALSE;       // We handled the msg

                case IDM_PRFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Printer
                    MyChooseFont (&cfPR, &CreateNewFontPR);

                    return FALSE;       // We handled the msg

                case IDM_FEFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Function Editor
                    MyChooseFont (&cfFE, &CreateNewFontFE);

                    return FALSE;       // We handled the msg

                case IDM_MEFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Matrix Editor
                    MyChooseFont (&cfME, &CreateNewFontME);

                    return FALSE;       // We handled the msg

                case IDM_VEFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Vector Editor
                    MyChooseFont (&cfVE, &CreateNewFontVE);

                    return FALSE;       // We handled the msg

                case IDM_HELP_CONTENTS:
                    WinHelpW (hWnd, wszHlpDPFE, HELP_INDEX, 0);

                    return FALSE;       // We handled the msg

                case IDM_ABOUT:
                    DialogBox (_hInstance,
                               MAKEINTRESOURCE (IDD_ABOUT),
                               hWnd,
                               (DLGPROC) &AboutDlgProc);
                    return FALSE;       // We handled the msg

                case IDM_UPDATES:
                    // See if we have the latest version
                    CheckForUpdates ();

                    return FALSE;       // We handled the msg

                // The following messages come from the Tab Control.
                // They are sent here because we need a window whose
                //   WM_COMMAND we control so as not to interfere with
                //   any other window's meaning for these menu items.
                case IDM_NEW_WS:
                    // Load a CLEAR WS
                    if (!CreateNewTab (hWnd,
                                       L"",
                                       (gOverTabIndex EQ -1) ? 999 : gOverTabIndex + 1,
                                       FALSE))
                        return -1;          // Stop the whole process

                    return FALSE;   // We handled the msg

                case IDM_LOAD_WS:
#ifdef DEBUG
                    DbgBrk ();      // ***FINISHME*** -- IDM_LOAD_WS
#endif


                    return FALSE;   // We handled the msg

                case IDM_XLOAD_WS:
#ifdef DEBUG
                    DbgBrk ();      // ***FINISHME*** -- IDM_XLOAD_WS
#endif


                    return FALSE;   // We handled the msg

                case IDM_COPY_WS:
#ifdef DEBUG
                    DbgBrk ();      // ***FINISHME*** -- IDM_COPY_WS
#endif


                    return FALSE;   // We handled the msg

                case IDM_PCOPY_WS:
#ifdef DEBUG
                    DbgBrk ();      // ***FINISHME*** -- IDM_PCOPY_WS
#endif



                    return FALSE;   // We handled the msg

                case IDM_SAVE_WS:
                    // Get the per tab global memory handle
                    hGlbPTD = GetPerTabHandle (gOverTabIndex); Assert (hGlbPTD NE NULL);

                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    // Get this tab's SM window handle
                    hWndSM = lpMemPTD->hWndSM;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                    // Tell the SM to save the ws
                    return SendMessageW (hWndSM, MYWM_SAVE_WS, 0, (LPARAM) L"");

                case IDM_SAVE_AS_WS:
#ifdef DEBUG
                    DbgBrk ();      // ***FINISHME*** -- IDM_SAVE_AS_WS
#endif


                    return FALSE;   // We handled the msg

                case IDM_DROP_WS:
#ifdef DEBUG
                    DbgBrk ();      // ***FINISHME*** -- IDM_DROP_WS
#endif



                    return FALSE;   // We handled the msg

                case IDM_DUP_WS:
#ifdef DEBUG
                    DbgBrk ();      // ***FINISHME*** -- IDM_DUP_WS
#endif



                    return FALSE;   // We handled the msg

                case IDM_PRINT_WS:
                {
                    PRINTDLGEX pdex;        // Struc used when printing
                    HWND       hWndMC,      // Active hWndMC
                               hWndAct,     // Active window handle
                               hWndEC;      // Edit Control window handle
                    DWORD      dwSelBeg,    // Selection beginning
                               dwSelEnd;    // ...       end
                    HRESULT    hResult;     // Result from PrintDlgEx

                    // Determine whether or not the Edit Control has a selection
                    hWndMC  = GetActiveMC (hWndTC);
                    hWndAct = (HWND) SendMessageW (hWndMC, WM_MDIGETACTIVE, 0, 0);
       (HANDLE_PTR) hWndEC = (GetWindowLongPtrW (hWndAct, GWLSF_HWNDEC));
                    SendMessageW (hWndEC, EM_GETSEL, (WPARAM) &dwSelBeg, (LPARAM) &dwSelEnd);

                    // Fill in the PRINTDLG struc
                    ZeroMemory (&pdex, sizeof (pdex));
                    pdex.lStructSize         = sizeof (pdex);
                    pdex.hwndOwner           = hWndEC;
////////////////////pdex.hDevMode            = NULL;    // Output only
////////////////////pdex.hDevNames           = NULL;    // Output only
////////////////////pdex.hDC                 = NULL;    // Output only
                    pdex.Flags               = 0
                                           | ((dwSelBeg EQ dwSelEnd) ? PD_CURRENTPAGE | PD_NOSELECTION
                                                                     : PD_SELECTION)
                                           | PD_RETURNDC
                                           | PD_NOPAGENUMS
                                           ;
////////////////////pdex.Flags2              = 0;       // Unused
////////////////////pdex.Exclusion Flags     = 0;       // Unused
////////////////////pdex.nPageRanges         = 0;       // Not with PD_NOPAGENUMS
////////////////////pdex.nMaxPageRanges      = 0;       // Not with PD_NOPAGENUMS
////////////////////pdex.lpPageRanges        = NULL;    // Not with PD_NOPAGENUMS
////////////////////pdex.nMinPage            =          // Not with PD_NOPAGENUMS
////////////////////pdex.nMaxPage            =          // Not with PD_NOPAGENUMS
                    pdex.nCopies             = 1;       // Input/output
////////////////////pdex.hInstance           = NULL;    // Used for PD_ENABLEPRINTTEMPLATE only
////////////////////pdex.lpPrintTemplateName = NULL;    // Unused
////////////////////pdex.lpCallback          = NULL;    // Unused
////////////////////pdex.nPropertyPages      = 0;       // Used for lphPropertyPages only
////////////////////pdex.lphPropertyPages    = NULL;    // Unused
                    pdex.nStartPage          = START_PAGE_GENERAL;
////////////////////pdex.dwResultAction      = 0;       // Output only

                    // Ask the user what to do
                    hResult = PrintDlgEx (&pdex);

                    // Split cases based upon the result code
                    if (hResult EQ S_OK)
                    switch (pdex.dwResultAction)
                    {
                        case PD_RESULT_APPLY:
#ifdef DEBUG
                            DbgBrk ();          // ***FINISHME*** -- PD_RESULT_APPLY
#endif

                            break;

                        case PD_RESULT_PRINT:
                        {
#ifdef DEBUG
                            DEVMODE  *lpDevMode;
                            DEVNAMES *lpDevNames;
#endif
                            DOCINFOW docInfo = {0};

////////////////////////////// If the user said print-to-file, get the filename
////////////////////////////if (pdex.Flags & PD_PRINTTOFILE)
////////////////////////////{
////////////////////////////    DbgBrk ();          // ***FINISHME*** -- Print-to-file
////////////////////////////
////////////////////////////
////////////////////////////
////////////////////////////} // End IF
#ifdef DEBUG
                            lpDevMode  = MyGlobalLock (pdex.hDevMode);
                            lpDevNames = MyGlobalLock (pdex.hDevNames);
#endif
                            // Create a new font for the printer
                            CreateNewFontCom (&hFontPR,
                                              &lfPR,
                                              &cfPR,
                                              &tmPR,
                                              &cxAveCharPR,
                                              &cyAveCharPR,
                                               pdex.hDC);
                            // Setup the DOCINFO struc for the print job
                            docInfo.cbSize       = sizeof (docInfo);
                            docInfo.lpszDocName  = WS_APPNAME;
                            docInfo.lpszOutput   = NULL;        // ***FIXME*** -- Insert filename
////////////////////////////docInfo.lpszDatatype = NULL;
////////////////////////////docInfo.fwType       = 0;

                            // Start the print job
                            StartDocW (pdex.hDC, &docInfo);

                            // Tell the Edit Control to print the client area
                            SendMessageW (hWndEC,
                                          WM_PRINTCLIENT,
                                          (WPARAM) pdex.hDC,
                                          PRF_PRINTCLIENT
                                        | ((pdex.Flags & PD_SELECTION) ? PRF_SELECTION
                                                                       : (pdex.Flags & PD_CURRENTPAGE) ? PRF_CURRENTPAGE
                                                                                                      : 0));
                            // End the print job
                            EndDoc (pdex.hDC);
#ifdef DEBUG
                            MyGlobalUnlock (pdex.hDevNames); lpDevNames = NULL;
                            MyGlobalUnlock (pdex.hDevMode);  lpDevMode  = NULL;
#endif
                            break;
                        } // End PD_RESULT_PRINT

                        case PD_RESULT_CANCEL:
                        case E_OUTOFMEMORY:
                        case E_INVALIDARG:
                        case E_POINTER:
                        case E_HANDLE:
                        case E_FAIL:
                            break;
                    } // End SWITCH

                    // Free allocated resources
                    if (pdex.hDC)
                        DeleteDC (pdex.hDC);
                    if (pdex.hDevMode)
                        GlobalFree (pdex.hDevMode);
                    if (pdex.hDevNames)
                        GlobalFree (pdex.hDevNames);

                    return FALSE;   // We handled the msg
                } // End IDM_PRINT_WS

                // The following messages are signalled from right-click on a Tab
                case IDM_SAVECLOSE_WS:
                    if (SendMessageW (hWnd, WM_COMMAND, GET_WM_COMMAND_MPS (IDM_SAVE_WS, NULL, 0)))
                        // Close the tab
                        CloseTab (gOverTabIndex);

                    return FALSE;   // We handled the msg

                case IDM_CLOSE_WS:
                    // Close the tab
                    CloseTab (gOverTabIndex);

                    return FALSE;   // We handled the msg

                case IDM_CLOSE_FN:
                    // Tell the active window to handle it
                    PostMessageW (hWndActive, MYWM_CLOSE_FN, wParam, lParam);

                    return FALSE;   // We handled the msg

                // The following messages are signalled from the Objects Menu
                //   when a function window is open
                case IDM_SAVE_FN:
                    // Tell the active window to handle it
                    PostMessageW (hWndActive, MYWM_SAVE_FN, wParam, lParam);

                    return FALSE;   // We handled the msg

                case IDM_SAVECLOSE_FN:
                    // Tell the active window to handle it
                    PostMessageW (hWndActive, MYWM_SAVECLOSE_FN, wParam, lParam);

                    return FALSE;   // We handled the msg

////////////////case IDM_SAVE_FN_AS:            // ***FIXME*** -- Make this work??
////////////////    SaveAsFunction (hWndActive);
////////////////
////////////////    return FALSE;   // We handled the msg
            } // End SWITCH

            break;                  // Continue with next handler ***MUST***

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, we handle erase background
            // in the WM_PAINT message for the child windows.
            return TRUE;            // We erased the background

        case WM_QUERYENDSESSION:
        case WM_CLOSE:
            if (fHelp && wszHlpDPFE[0])  // If we used help and there's a valid .HLP file, ...
            {
                WinHelpW (hWnd, wszHlpDPFE, HELP_QUIT, 0); // Quit it
                fHelp = FALSE;
            } // End IF

            // Save .ini file variables
            SaveIniFile ();

            // Ask the child windows if it's OK to close
            if (EnumChildWindows (hWnd, EnumCallbackQueryClose, 0))
            {
#ifdef DEBUG
////            // Tell the debugger windows to unhook its hooks
////            EnumChildWindows (hWnd, EnumCallbackUnhookDebugger, 0);
#endif
                // Delete all the tabs
                TabCtrl_DeleteAllItems (hWndTC);

                // This also tells the child windows to close
                DestroyWindow (hWnd);

                if (message EQ WM_QUERYENDSESSION)
                    return TRUE;        // OK to terminate
                else
                    return FALSE;       // We handled the msg
            } else
                return FALSE;           // Not OK to terminate/we handled the msg

        case WM_DESTROY:
            // Remove all saved window properties
            EnumProps (hWnd, EnumCallbackRemoveProp);

            // Uninitialize window-specific resources
            MF_Delete (hWnd);

            // Destroy the fonts
            if (hFontTC)
            {
                MyDeleteObject (hFontTC); hFontTC = NULL;
            } // End IF

            if (hFontSM)
            {
                MyDeleteObject (hFontSM); hFontSM = NULL;
            } // End IF

            if (hFontCC)
            {
                MyDeleteObject (hFontCC); hFontCC = NULL;
            } // End IF

            if (hFontFE)
            {
                MyDeleteObject (hFontFE); hFontFE = NULL;
            } // End IF

            if (hFontME)
            {
                MyDeleteObject (hFontME); hFontME = NULL;
            } // End IF

            if (hFontVE)
            {
                MyDeleteObject (hFontVE); hFontVE = NULL;
            } // End IF

            // Destroy the image list
            if (hImageList)
            {
                ImageList_Destroy (hImageList); hImageList = NULL;
            } // End IF

            // *************** Bitmaps *********************************
            if (hBitMapLineCont)
            {
                MyDeleteObject (hBitMapLineCont); hBitMapLineCont = NULL;
            } // End IF

            // Say goodbye
            PostQuitMessage (0);

            break;                  // Continue with default handler
    } // End SWITCH

    // Get the window handle of the currently active MDI Client
    if (message NE WM_NCDESTROY
     && message NE WM_CLOSE)
        hWndMC = GetActiveMC (hWndTC);
    else
        hWndMC = NULL;
////LCLODSAPI ("MFZ:", hWnd, message, wParam, lParam);
    return DefFrameProcW (hWnd, hWndMC, message, wParam, lParam);
} // End MFWndProc


//***************************************************************************
//  $GetActiveMC
//
//  Get the window handle of the currently active MDI Client
//***************************************************************************

HWND GetActiveMC
    (HWND hWndTC)           // Window handle of Tab Control

{
    int iCurTabIndex;       // Index of the current tab

    // If the Tab Control is not defined, quit
    if (hWndTC EQ NULL)
        return NULL;

    // Get the index of the currently selected tab
    iCurTabIndex = TabCtrl_GetCurSel (hWndTC);

    // If no tab selected (early in MFWndProc processing), quit
    if (iCurTabIndex EQ -1)
        return NULL;

    return GetWndMC (iCurTabIndex);
} // End GetActiveMC


//***************************************************************************
//  $GetWndMC
//
//  Get the window handle of the MDI Client of a given tab
//***************************************************************************

HWND GetWndMC
    (int iCurTab)           // Index of the tab of interest (-1 = none)

{
    HGLOBAL      hGlbPTD;
    LPPERTABDATA lpMemPTD;
    HWND         hWndMC;

    // If the Tab Control is not defined, quit
    if (hWndTC EQ NULL)
        return NULL;

    // If no tab selected (early in MFWndProc processing), quit
    if (iCurTab EQ -1)
        return NULL;

    // Get the per tab global memory handle
    hGlbPTD = GetPerTabHandle (iCurTab);

    // Ensure it's a valid ptr
    if (!IsGlbPtr (hGlbPTD))
        return NULL;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get window handle of MDI Client
    hWndMC = lpMemPTD->hWndMC;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return hWndMC;
} // End GetWndMC


//***************************************************************************
//  $EnumCallbackQueryClose
//
//  EnumChildWindows callback to query whether or not they can close
//***************************************************************************

BOOL CALLBACK EnumCallbackQueryClose
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    return (UINT) SendMessageW (hWnd, WM_QUERYENDSESSION, 0, 0);
} // End EnumCallbackQueryClose


//// #ifdef DEBUG
//// //***************************************************************************
//// //  $EnumCallbackCloseDebugger
//// //
//// //  EnumChildWindows callback to tell debugger windows to unhook its hooks
//// //***************************************************************************
////
//// BOOL CALLBACK EnumCallbackUnhookDebugger
////     (HWND   hWnd,           // Handle to child window
////      LPARAM lParam)         // Application-defined value
////
//// {
////     // When an MDI child window is minimized, Windows creates two windows: an
////     // icon and the icon title.  The parent of the icon title window is set to
////     // the MDI client window, which confines the icon title to the MDI client
////     // area.  The owner of the icon title is set to the MDI child window.
////     if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
////         return TRUE;                    // skip it, and continue enumerating
////
////     // If it's a debugger window, ...
////     if (IzitDB (hWnd))
////         // Tell it to unhook its hooks
////         SendMessageW (hWnd, MYWM_UNHOOK, 0, 0);
////
////     return TRUE;
//// } // End EnumCallbackUnhookDebugger
//// #endif


#ifdef DEBUG
//***************************************************************************
//  $IzitDB
//
//  Is the window DBWNDCLASS?
//***************************************************************************

BOOL IzitDB
    (HWND hWnd)

{
    WCHAR wszClassName[32];

    GetClassNameW (hWnd, wszClassName, (sizeof (wszClassName) / sizeof (wszClassName[0]))- 1);

    return (lstrcmpW (wszClassName, LDBWNDCLASS) EQ 0);
} // End IzitDB
#endif


//***************************************************************************
//  $CheckForUpdates
//
//  See if we have the latest version
//***************************************************************************

void CheckForUpdates
    (void)

{
    HINTERNET hNetOpen    = NULL,
              hNetConnect = NULL,
              hNetRequest = NULL;
    char      szTemp[64];
    WCHAR     wszFileVer[64],
              wszTemp[64];
    UINT      uNumBytes;

    // Read in the application's File Version String
    LclFileVersionStrW (wszAppDPFE, wszFileVer);

    // Get the file "nars2000.ver" from http://www.NARS2000.ORG/download/binaries/

    // Make the connection
    hNetOpen =
      InternetOpen ("NARS2000",                             // Ptr to User Agent
                    PRE_CONFIG_INTERNET_ACCESS,             // Access type
                    NULL,                                   // Ptr to proxy name (may be NULL)
                    NULL,                                   // Ptr to proxy bypass (may be NULL)
                    0);                                     // Flags
    if (!hNetOpen)
        goto ERROR_EXIT;

    // Connect to the server
    hNetConnect =
      InternetConnect (hNetOpen,                            // Open handle
                       "www.nars2000.org",                  // Ptr to server name
                       INTERNET_DEFAULT_HTTP_PORT,          // Server port
                       "guest",                             // Ptr to username
                       "guest",                             // Ptr to password
                       INTERNET_SERVICE_HTTP,               // Type of service access
                       0,                                   // Optional flags
                       0);                                  // Context value
    if (!hNetConnect)       // ***FIXME** -- Display error message
    {
        wsprintfW (lpwszGlbTemp,
                   L"InternetConnect failed with error code %d (%08X)",
                   GetLastError (),
                   GetLastError ());
        MessageBoxW (NULL, lpwszGlbTemp, lpwszAppName, MB_OK | MB_ICONERROR);

        goto ERROR_EXIT;
    } // End IF

    // Make a request
    hNetRequest =
      HttpOpenRequest (hNetConnect,
                       NULL,                                // "GET"
                       "/download/binaries/nars2000.ver",   // Ptr to /path/filename
                       NULL,                                // HTTP 1.1
                       NULL,                                // No referrer
                       NULL,                                // No ACCEPT types
                       0
                     | INTERNET_FLAG_NO_CACHE_WRITE         // Does not add the returned entity to the cache
                     | INTERNET_FLAG_PRAGMA_NOCACHE         // Forces the request to be resolved by the origin server,
                                                            //   even if a cached copy exists on the proxy.
                     | INTERNET_FLAG_RELOAD,                // Forces a download of the requested file, object, or directory
                                                            //   listing from the origin server, not from the cache.
                       0);                                  // Context value
    if (!hNetRequest)
    {
        wsprintfW (lpwszGlbTemp,
                   L"HttpOpenRequest failed with error code %d (%08X)",
                   GetLastError (),
                   GetLastError ());
        MessageBoxW (NULL, lpwszGlbTemp, lpwszAppName, MB_OK | MB_ICONERROR);

        goto ERROR_EXIT;
    } // End IF

    // Send the request
    if (HttpSendRequest (hNetRequest,                       // Request handle
                         NULL,                              // Ptr to additional headers (may be NULL)
                         0,                                 // Size of additional headers
                         NULL,                              // Ptr to optional data (may be NULL)
                         0))                                // Size of optional data
    {
        // Read the file
        if (InternetReadFile (hNetRequest,
                              szTemp,
                              sizeof (szTemp),
                             &uNumBytes))
        {
            UPDATESDLGSTR updatesDlgStr;

            // Ensure properly terminated
            szTemp[uNumBytes] = L'\0';

            // Convert the file contents to wide so we can compare it against the file version
            A2W (wszTemp, szTemp, sizeof (szTemp) - uNumBytes);

            // Save data in struc to pass to the dialog box
            updatesDlgStr.lpFileVer = wszFileVer;
            updatesDlgStr.lpWebVer  = wszTemp;

            // Show the result in a dialog
            DialogBoxParam (_hInstance,
                            MAKEINTRESOURCE (IDD_UPDATES),
                            hWndMF,
                 (DLGPROC) &UpdatesDlgProc,
                  (LPARAM) &updatesDlgStr);

            goto NORMAL_EXIT;
        } else
        {
            wsprintfW (lpwszGlbTemp,
                       L"InternetReadFile failed with error code %d (%08X)",
                       GetLastError (),
                       GetLastError ());
            MessageBoxW (NULL, lpwszGlbTemp, lpwszAppName, MB_OK | MB_ICONERROR);

            goto ERROR_EXIT;
        } // End IF/ELSE
    } else
    {
        wsprintfW (lpwszGlbTemp,
                   L"HttpSendRequest failed with error code %d (%08X)",
                   GetLastError (),
                   GetLastError ());
        MessageBoxW (NULL, lpwszGlbTemp, lpwszAppName, MB_OK | MB_ICONERROR);

        goto ERROR_EXIT;
    } // End IF/ELSE
ERROR_EXIT:
NORMAL_EXIT:
    if (hNetRequest)
    {
        InternetCloseHandle (hNetRequest); hNetRequest = NULL;
    } // End IF

    if (hNetConnect)
    {
        InternetCloseHandle (hNetConnect); hNetConnect = NULL;
    } // End IF

    if (hNetOpen)
    {
        InternetCloseHandle (hNetOpen); hNetOpen = NULL;
    } // End IF
} // End CheckForUpdates


//***************************************************************************
//  $UpdatesDlgProc
//
//  Dialog box to handle "Check For Updates"
//***************************************************************************

BOOL CALLBACK UpdatesDlgProc
    (HWND   hDlg,
     UINT   message,
     WPARAM wParam,
     LPARAM lParam)

{
    static HFONT hFont = NULL;

    // Split cases
    switch (message)
    {
        case WM_INITDIALOG:
        {
            WCHAR wszTemp[512];

#define lpUpdatesDlgStr     ((LPUPDATESDLGSTR) lParam)

            // Compare the two strings
            switch (lstrcmpW (lpUpdatesDlgStr->lpWebVer, lpUpdatesDlgStr->lpFileVer))
            {
                case -1:            // Actual version is later (must be running an unreleased version)
                    // Format the text
                    wsprintfW (wszTemp,
                               L"The version you are running is newer than the most current version on the website and there is no need to update your version.",
                               lpUpdatesDlgStr->lpFileVer,
                               lpUpdatesDlgStr->lpWebVer);
                    break;

                case 0:             // Same version (nothing to do)
                    wsprintfW (wszTemp,
                               L"The version you are running is the same as the most current version on the website and there is no need to update your version.",
                               lpUpdatesDlgStr->lpFileVer,
                               lpUpdatesDlgStr->lpWebVer);
                    break;

                case 1:             // New version is available for download
                    wsprintfW (wszTemp,
                               L"The version you are running is the older than the most current version on the website -- please download the latest version from ...",
                               lpUpdatesDlgStr->lpFileVer,
                               lpUpdatesDlgStr->lpWebVer);
                    break;

                defstop
                    break;
            } // End SWITCH

            // Write out the file & web version strings
            SetDlgItemTextW (hDlg, IDC_VERACTION, wszTemp);

            wsprintfW (wszTemp,
                       L"The version you are running is %s.",
                       lpUpdatesDlgStr->lpFileVer);
            SetDlgItemTextW (hDlg, IDC_FILEVER  , wszTemp);

            wsprintfW (wszTemp,
                       L"The latest released version is %s.",
                       lpUpdatesDlgStr->lpWebVer);
            SetDlgItemTextW (hDlg, IDC_WEBVER   , wszTemp);

#undef  lpUpdatesDlgStr

            CenterWindow (hDlg);    // Reposition the window to the center of the screen

////        // Get the IDC_LINK window handle
////        hWndStatic = GetDlgItem (hDlg, IDC_LINK);
////
////        // Subclass the IDC_LINK control
////        //   so we can handle WM_LBUTTONDOWN
////        (HANDLE_PTR) lpfnOldStaticWndProc =
////          SetWindowLongPtrW (hWndStatic,
////                             GWL_WNDPROC,
////                             (__int3264) (HANDLE_PTR) (WNDPROC) &LclStaticWndProc);
////        // Set the cursor for the static control to a hand



            return TRUE;            // Use the focus in wParam
        } // End WM_INITDIALOG

        case WM_CTLCOLORSTATIC:     // hdc = (HDC) wParam;   // Handle of display context
                                    // hwnd = (HWND) lParam; // Handle of static control
#define hDC     ((HDC)  wParam)
#define hWnd    ((HWND) lParam)

////        // We handle IDC_LINK static window only
////        if (hWnd == hWndStatic)
////        {
////            LOGFONT lf;
////
////            // Set the text color
////            SetTextColor (hDC, COLOR_BLUE);
////
////            // Set the background text color
////            SetBkColor (hDC, GetSysColor (COLOR_BTNFACE));
////
////            // Get the font handle
////            hFont = (HFONT) SendMessageW (hWnd, WM_GETFONT, 0, 0);
////
////            // Get the LOGFONT structure for the font
////            GetObject (hFont, sizeof (lf), &lf);
////
////            // Change to an underlined font
////            lf.lfUnderline = TRUE;
////
////            // Create a new font, now underlined
////            hFont = MyCreateFontIndirect (&lf);
////
////            // Select it into the DC
////            SelectObject (hDC, hFont);
////
////            // Return handle of brush for background
////            return (int) CreateSolidBrush (GetSysColor (COLOR_BTNFACE));
////        } else
                break;
#undef  hWnd
#undef  hDC

        case WM_CLOSE:
            // If it's still around, delete the font we created
            if (hFont)
            {
                MyDeleteObject (hFont); hFont = NULL;
            } // End IF

////        // Restore the old WndProc
////        SetWindowLongPtrW (hWndStatic,
////                           GWL_WNDPROC,
////                           (__int3264) (HANDLE_PTR) (WNDPROC) lpfnOldStaticWndProc);
////        lpfnOldStaticWndProc = NULL;

            EndDialog (hDlg, TRUE); // Quit this dialog

            return TRUE;            // We handled the msg

        case WM_COMMAND:
            // If the user pressed one of our buttons, ...
            switch (GET_WM_COMMAND_ID (wParam, lParam))
            {
                case IDOK:
                    PostMessageW (hDlg, WM_CLOSE, 0, 0);

                    return TRUE;    // We handled the msg
            } // End switch (wParam)

            break;
    } // End SWITCH

    return FALSE;           // We didn't handle the msg
} // End UpdatesDlgProc


//***************************************************************************
//  $InitApplication
//
//  Initializes window data and registers window class
//***************************************************************************

BOOL InitApplication
    (HANDLE hInstance)      // Current instance

{
    WNDCLASSEXW wcw = {sizeof (WNDCLASSEXW)};

    // Fill in Master Frame window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) MFWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = 0;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconMF_Large;
    wcw.hIconSm         = hIconMF_Small;
    wcw.hCursor         = LoadCursor (NULL, IDC_ARROW);
    wcw.hbrBackground   = (HBRUSH) (COLOR_WINDOW + 1);
    wcw.lpszMenuName    = MAKEINTRESOURCEW (IDR_SMMENU);
    wcw.lpszClassName   = LMFWNDCLASS;

    // Register the Master Frame window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegMFWndClass);
        return FALSE;
    } // End IF

    // Fill in Session Manager window class structure
    wcw.style           = CS_DBLCLKS | CS_NOCLOSE;
    wcw.lpfnWndProc     = (WNDPROC) SMWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLSM_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconSM_Large;
    wcw.hIconSm         = hIconSM_Small;
    wcw.hCursor         = LoadCursor (NULL, IDC_ARROW);
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
////wcw.lpszMenuName    = MAKEINTRESOURCEW (IDR_SMMENU);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LSMWNDCLASS;

    // Register the Session Manager window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegSMWndClass);
        return FALSE;
    } // End IF

    // Fill in Function Editor window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) FEWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLFE_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconFE_Large;
    wcw.hIconSm         = hIconFE_Small;
    wcw.hCursor         = LoadCursor (NULL, IDC_ARROW);
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
////wcw.lpszMenuName    = MAKEINTRESOURCE (IDR_FEMENU);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LFEWNDCLASS;

    // Register the Function Editor window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegFEWndClass);
        return FALSE;
    } // End IF

    // Fill in Matrix Editor window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) MEWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLME_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconME_Large;
    wcw.hIconSm         = hIconME_Small;
    wcw.hCursor         = LoadCursor (NULL, IDC_ARROW);
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
////wcw.lpszMenuName    = MAKEINTRESOURCE (IDR_MEMENU);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LMEWNDCLASS;

    // Register the Matrix Editor window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegMEWndClass);
        return FALSE;
    } // End IF

    // Fill in Vector Editor window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) VEWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLVE_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconVE_Large;
    wcw.hIconSm         = hIconVE_Small;
    wcw.hCursor         = LoadCursor (NULL, IDC_ARROW);
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
////wcw.lpszMenuName    = MAKEINTRESOURCE (IDR_VEMENU);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LVEWNDCLASS;

    // Register the Vector Editor window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegVEWndClass);
        return FALSE;
    } // End IF
#ifdef DEBUG
    // Fill in Debugger window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) DBWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLDB_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconDB_Large;
    wcw.hIconSm         = hIconDB_Small;
    wcw.hCursor         = LoadCursor (NULL, IDC_ARROW);
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
////wcw.lpszMenuName    = MAKEINTRESOURCE (IDR_SMMENU);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LDBWNDCLASS;

    // Register the Debugger window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegDBWndClass);
        return FALSE;
    } // End IF
#endif

    // Fill in Crash Control window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) CCWndProc;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLCC_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = hIconCC_Large;
    wcw.hIconSm         = hIconCC_Small;
    wcw.hCursor         = LoadCursor (NULL, IDC_ARROW);
    wcw.hbrBackground   = GetStockObject (WHITE_BRUSH);
////wcw.lpszMenuName    = MAKEINTRESOURCE (IDR_CCMENU);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LCCWNDCLASS;

    // Register the Crash Control window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegCCWndClass);
        return FALSE;
    } // End IF

    // Fill in Edit Control window class structure
    wcw.style           = CS_DBLCLKS;
    wcw.lpfnWndProc     = (WNDPROC) EditWndProcW;
    wcw.cbClsExtra      = 0;
    wcw.cbWndExtra      = GWLEC_EXTRA;
    wcw.hInstance       = hInstance;
    wcw.hIcon           = NULL;
    wcw.hIconSm         = NULL;
    wcw.hCursor         = LoadCursor (NULL, IDC_ARROW);
    wcw.hbrBackground   = (HBRUSH) (COLOR_WINDOW + 1);
    wcw.lpszMenuName    = NULL;
    wcw.lpszClassName   = LECWNDCLASS;

    // Register the Edit Control window class
    if (!RegisterClassExW (&wcw))
    {
        MB (pszNoRegECWndClass);
        return FALSE;
    } // End IF

    return TRUE;
} // End InitApplication


//***************************************************************************
//  $UninitApplication
//
//  Uninitialize application-specific resources
//***************************************************************************

void UninitApplication
    (HINSTANCE hInstance)

{
} // UninitApplication


//***************************************************************************
//  $InitInstance
//
//  Saves instance handle and creates main window
//***************************************************************************

BOOL InitInstance
    (HANDLE hInstance)

{
    int i;                          // Loop counter

    // Save in global variable
    _hInstance = hInstance;

    //***************************************************************
    // Allocate virtual memory for the WCHAR temporary storage
    //***************************************************************
#ifdef DEBUG
    memVirtStr[MEMVIRT_WSZGLBTEMP].lpText   = "lpwszGlbTemp in <InitInstance>";
#endif
    memVirtStr[MEMVIRT_WSZGLBTEMP].IncrSize = DEF_WGLBTEMP_INCRSIZE * sizeof (WCHAR);
    memVirtStr[MEMVIRT_WSZGLBTEMP].MaxSize  = DEF_WGLBTEMP_MAXSIZE  * sizeof (WCHAR);
    memVirtStr[MEMVIRT_WSZGLBTEMP].IniAddr  = (LPUCHAR)
    lpwszGlbTemp =
      GuardAlloc (NULL,             // Any address
                  memVirtStr[MEMVIRT_WSZGLBTEMP].MaxSize,
                  MEM_RESERVE,      // memVirtStr
                  PAGE_READWRITE);
    if (!memVirtStr[MEMVIRT_WSZGLBTEMP].IniAddr)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsgW (L"InitInstance:  GuardAlloc for <lpwszGlbTemp> failed");

        return FALSE;       // Mark as failed
    } // End IF

    // Commit the intial size
    MyVirtualAlloc (memVirtStr[MEMVIRT_WSZGLBTEMP].IniAddr,
                    DEF_WGLBTEMP_INITSIZE * sizeof (WCHAR),
                    MEM_COMMIT,
                    PAGE_READWRITE);

    //***************************************************************
    // Allocate virtual memory for the HshTab for {symbol} names & values
    //***************************************************************
#ifdef DEBUG
    memVirtStr[MEMVIRT_GLBHSHTAB].lpText   = "htsGLB.lpHshTab in <InitInstance>";
#endif
    memVirtStr[MEMVIRT_GLBHSHTAB].IncrSize = DEF_GLBHSHTAB_INCRSIZE * sizeof (htsGLB.lpHshTab[0]);
    memVirtStr[MEMVIRT_GLBHSHTAB].MaxSize  = DEF_GLBHSHTAB_MAXSIZE  * sizeof (htsGLB.lpHshTab[0]);
    memVirtStr[MEMVIRT_GLBHSHTAB].IniAddr  = (LPUCHAR)
    htsGLB.lpHshTab =
      GuardAlloc (NULL,             // Any address
                  memVirtStr[MEMVIRT_GLBHSHTAB].MaxSize,
                  MEM_RESERVE,      // memVirtStr
                  PAGE_READWRITE);
    if (!memVirtStr[MEMVIRT_GLBHSHTAB].IniAddr)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsgW (L"InitInstance:  GuardAlloc for <lpGlbHshTab> failed");

        return FALSE;       // Mark as failed
    } // End IF

    // Commit the intial size
    MyVirtualAlloc (memVirtStr[MEMVIRT_GLBHSHTAB].IniAddr,
                    DEF_GLBHSHTAB_INITSIZE * sizeof (htsGLB.lpHshTab[0]),
                    MEM_COMMIT,
                    PAGE_READWRITE);

    // Initialize the principal hash entry (1st one in each block).
    // This entry is never overwritten with an entry with a
    //   different hash value.
    for (i = 0; i < DEF_GLBHSHTAB_INITSIZE; i += DEF_GLBHSHTAB_EPB)
        htsGLB.lpHshTab[i].htFlags.PrinHash = TRUE;

    // Initialize the next & prev same HTE values
    for (i = 0; i < DEF_GLBHSHTAB_INITSIZE; i++)
    {
        htsGLB.lpHshTab[i].NextSameHash =
        htsGLB.lpHshTab[i].PrevSameHash = LPHSHENTRY_NONE;
    } // End FOR

    // Initialize the global HshTab values
    htsGLB.lpHshTabSplitNext = htsGLB.lpHshTab;
    htsGLB.iHshTabTotalSize  = DEF_GLBHSHTAB_INITSIZE;
    htsGLB.iHshTabBaseSize   = DEF_GLBHSHTAB_INITSIZE;
    htsGLB.iHshTabIncr       = DEF_GLBHSHTAB_INCR;
    htsGLB.iHshTabEPB        = DEF_GLBHSHTAB_EPB;
    htsGLB.iHshTabIncrSize   = DEF_GLBHSHTAB_INCRSIZE;
    htsGLB.uHashMask         = DEF_GLBHSHTAB_HASHMASK;

    // Read in the icons
    hIconMF_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_MF_LARGE));
    hIconMF_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_MF_SMALL));

    hIconSM_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_SM_LARGE));
    hIconSM_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_SM_SMALL));

#ifdef DEBUG
    hIconDB_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_DB_LARGE));
    hIconDB_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_DB_SMALL));
#endif

    hIconFE_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_FE_LARGE));
    hIconFE_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_FE_SMALL));

    hIconME_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ME_LARGE));
    hIconME_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ME_SMALL));

    hIconVE_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_VE_LARGE));
    hIconVE_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_VE_SMALL));

    hIconCC_Large = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_CC_LARGE));
    hIconCC_Small = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_CC_SMALL));

    hIconClose    = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_CLOSE   ));

    // Get keyboard accelerators
    hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE (IDR_ACCEL));

    return TRUE;
} // End InitInstance


//***************************************************************************
//  $UninitInstance
//
//  Uninitialize instance-specific resources
//***************************************************************************

void UninitInstance
    (HINSTANCE hInstance)

{
    UINT uCnt;              // Loop counter

    // *************** Temporary Storage ***********************
    for (uCnt = 0; uCnt < MEMVIRT_LENGTH; uCnt++)
    if (memVirtStr[uCnt].IniAddr)
    {
        // Free the virtual storage
        MyVirtualFree (memVirtStr[uCnt].IniAddr, 0, MEM_RELEASE);
    } // End IF

    // No need to destroy either the cursors or icons
} // UninitInstance


//***************************************************************************
//  $ParseCommandLine
//
//  Parse any command line
//***************************************************************************

BOOL ParseCommandLine
    (LPSTR lpCmdLine)

{
    LPCHAR p;
    WCHAR  wszTempDPFE[1024];

    // Skip over leading space
    p = SkipWhite (lpCmdLine);

    if (*p)
    {
        // Copy to temporary buffer
        A2W (wszTempDPFE, p, sizeof (wszTempDPFE));

        // Convert the []WSID workspace name into a canonical form (without WS_WKSEXT)
        MakeWorkspaceNameCanonical (wszLoadFile, wszTempDPFE, lpwszWorkDir);

        // Append the common workspace extension
        lstrcatW (wszLoadFile, WS_WKSEXT);

        // Mark as present
        bCommandLine = TRUE;
    } // End IF

    return TRUE;
} // ParseCommandLine


//***************************************************************************
//  $WinMain
//
//  Start the process
//***************************************************************************

int PASCAL WinMain
    (HINSTANCE   hInstance,
     HINSTANCE   hPrevInstance,     // Zero under Win32
     LPSTR       lpCmdLine,
     int         nCmdShow)

{
    MSG  Msg;           // Message for GetMessageW loop

    // This is needed by Wine's EDITCTRL.C
    user32_module = hInstance;

    // Ensure that the common control DLL is loaded.
    InitCommonControls ();

    // Construct file name(s) based upon where the module is on disk
    GetModuleFileNames (hInstance);

    // Ensure the Application Data and workspaces
    //   directories are present
    if (!CreateAppDataDirs ())
        goto EXIT1;

    // Save initial state
    nMinState = nCmdShow;

    // Allocate TLS indices
    dwTlsType        = TlsAlloc ();     // Thread type ('MF', 'TC', 'PL', etc.)
////dwTlsSemaphore   = TlsAlloc ();     // Thread semaphore (for 'PL' only)
    dwTlsPlLocalVars = TlsAlloc ();     // lpplLocalVars (for 'PL' only)
    dwTlsFhLocalVars = TlsAlloc ();     // lpfhLocalVars (for 'PL' or 'SM' only)
    dwTlsPerTabData  = TlsAlloc ();     // PerTabData    (for 'PL' or 'SM' only)

    // Save the thread type ('MF')
    TlsSetValue (dwTlsType, (LPVOID) 'MF');

    // Set rounding precision to 53-bits
    //   as per comments in top of <dtoa.c>
    //   but we need 64 bits because of 64-bit ints
    control87(PC_64, MCW_PC);

    // If there's a command line, parse it
    if (!ParseCommandLine (lpCmdLine))
        goto EXIT1;

    // Perform initializations that apply to a specific instance
    if (!InitInstance (hInstance))
        goto EXIT2;

    // Register the window class
    if (!InitApplication (hInstance))
        goto EXIT3;

    // Allocate two Critical Section objects
    //   for use in dtoa.c, and one
    //   for use in parseLine.
    InitializeCriticalSection (&CSO0);
    InitializeCriticalSection (&CSO1);
    InitializeCriticalSection (&CSOPL);

    // Create various permanent variables
    MakePermVars ();

    // Initialize all {symbol} names & values
    if (!InitSymbolNamesValues ())
        goto EXIT4;

    // Read in global .ini file values
    ReadIniFileGlb ();

    // Initialize ChooseFont argument here
    //   so its settings will be present
    //   for the second and subsequent time
    //   the common dialog is called.
    InitChooseFont (&cfSM, &lfSM, DEF_SMPTSIZE, FALSE);
    InitChooseFont (&cfPR, &lfPR, DEF_PRPTSIZE, TRUE );
    InitChooseFont (&cfTC, &lfTC, DEF_TCPTSIZE, FALSE);
    InitChooseFont (&cfFE, &lfFE, DEF_FEPTSIZE, FALSE);
    InitChooseFont (&cfME, &lfME, DEF_MEPTSIZE, FALSE);
    InitChooseFont (&cfVE, &lfVE, DEF_VEPTSIZE, FALSE);

    // Initialize tables for Primitive Fns, Operators, etc.
    InitPrimTabs ();

#ifdef DEBUG
    InitFsaTabs ();
#endif

    // Get and save the current Thread Id
    dwMainThreadId = GetCurrentThreadId ();

    // Create the Master Frame window
    hWndMF =
      CreateWindowExW (0L,                              // Extended styles
                       LMFWNDCLASS,                     // Class
                       wszMFTitle,                      // Title
                       0
                     | WS_OVERLAPPEDWINDOW
                       ,                                // Styles
                       CW_USEDEFAULT, CW_USEDEFAULT,    // X- and Y-coord
                       CW_USEDEFAULT, CW_USEDEFAULT,    // X- and Y-size
                       NULL,                            // Parent window
                       NULL,                            // Menu
                       _hInstance,                      // Instance
                       NULL);                           // No extra data
    if (hWndMF EQ NULL)
    {
        MB (pszNoCreateMFWnd);
        goto EXIT4;
    } // End IF

////// Make the window visible; update its client area
////ShowWindow (hWndMF, nCmdShow);
////UpdateWindow (hWndMF);

    __try
    {
        // Main message loop
        while (GetMessageW (&Msg, NULL, 0, 0))
        {
            HWND hWndMC;        // MDI Client window handle

            // Get the window handle of the currently active MDI Client
            hWndMC = GetActiveMC (hWndTC);

            // Handle MDI messages and accelerators
            if (!TranslateMDISysAccel (hWndMC, &Msg)
             && ((!hAccel) || !TranslateAccelerator (hWndMF, hAccel, &Msg)))
            {
                TranslateMessage (&Msg);
                DispatchMessageW (&Msg);
            } // End IF
        } // End WHILE
    } __except (CheckException (GetExceptionInformation (), L"WinMain"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except

    // GetMessageW returned FALSE for a Quit message
EXIT4:
    DeleteCriticalSection (&CSO1);
    DeleteCriticalSection (&CSO0);
EXIT3:
    UninitApplication (hInstance);
EXIT2:
    UninitInstance (hInstance);
EXIT1:
    return 0;
} // End WinMain


//***************************************************************************
//  End of File: main.c
//***************************************************************************
