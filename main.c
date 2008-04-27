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

#include "main.h"
#include "resdebug.h"
#include "resource.h"
#include "editctrl.h"

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


int nMinState,                          // Minimized state as per WinMain
    iScrollSize;                        // Width of a vertical scrollbar
BOOL fHelp = FALSE,                     // TRUE iff we displayed help
     bCommandLine;                      // ...      there is a filename on the command line

HMODULE user32_module;

HICON hIconMF_Large, hIconMF_Small,     // Icon handles
      hIconSM_Large, hIconSM_Small,
#ifdef DEBUG
      hIconDB_Large, hIconDB_Small,
#endif
      hIconFE_Large, hIconFE_Small,
      hIconME_Large, hIconME_Small,
      hIconVE_Large, hIconVE_Small,
      hIconClose;

#define  MFWNDCLASS          "MFClass"                                      // Master Frame Window class
#define LMFWNDCLASS         L"MFClass"                                      // Master Frame Window class

WCHAR wszMFTitle[]          = L"NARS2000" LAPPEND_DEBUG,                    // Master frame window title
      wszTCTitle[]          = L"NARS2000 Tab Control Window" LAPPEND_DEBUG, // Tab Control ... (for debugging purposes only)
      wszMFClass[]          = LMFWNDCLASS;                                  // Master Frame Window class

char pszNoRegMFWndClass[]   = "Unable to register window class <" MFWNDCLASS ">.",
     pszNoRegSMWndClass[]   = "Unable to register window class <" SMWNDCLASS ">.",
#ifdef DEBUG
     pszNoRegDBWndClass[]   = "Unable to register window class <" DBWNDCLASS ">.",
#endif
     pszNoRegFEWndClass[]   = "Unable to register window class <" FEWNDCLASS ">.",
     pszNoRegMEWndClass[]   = "Unable to register window class <" MEWNDCLASS ">.",
     pszNoRegVEWndClass[]   = "Unable to register window class <" VEWNDCLASS ">.",
     pszNoRegECWndClass[]   = "Unable to register window class <" ECWNDCLASS ">.";

char pszNoCreateMFWnd[]     = "Unable to create Master Frame window",
     pszNoCreateTCWnd[]     = "Unable to create Tab Control window",
     pszNoCreateTTWnd[]     = "Unable to create ToolTip window";


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
     int          iPtSize)
{
    HDC hDC;

    // Zero the struc
    memset (lpcf, 0, sizeof (CHOOSEFONT));

    hDC = MyGetDC (HWND_DESKTOP);
    iLogPixelsY = GetDeviceCaps (hDC, LOGPIXELSY);
    lplf->lfHeight = -MulDiv (iPtSize, iLogPixelsY, 72);
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
    (HFONT       *lphFont,
     LPLOGFONT    lplf,
     LPCHOOSEFONT lpcf,
     LPTEXTMETRIC lptm,
     long        *lpcxAveChar,
     long        *lpcyAveChar)

{
    HDC   hDC;
    HFONT hFontOld;

    // Delete the previous handle (if any)
    if (*lphFont)
    {
        // Delete the SM font handle
        MyDeleteObject (*lphFont); *lphFont = NULL;
    } // End IF

    // Create the font
    *lphFont = MyCreateFontIndirect (lplf);

    // Get the text metrics for this font
    hDC = MyGetDC (HWND_DESKTOP);
    hFontOld = SelectObject (hDC, *lphFont);
    GetTextMetrics (hDC, lptm);
    SelectObject (hDC, hFontOld);
    MyReleaseDC (HWND_DESKTOP, hDC); hDC = NULL;

    // New height
    *lpcyAveChar = MulDiv (lpcf->iPointSize / 10, iLogPixelsY, 72);

    // New width from TextMetrics
    lplf->lfWidth = lptm->tmAveCharWidth;

    // New width (same aspect ratio as old)
    *lpcxAveChar = MulDiv (lplf->lfWidth, *lpcyAveChar, -lplf->lfHeight);

    lplf->lfHeight = -*lpcyAveChar;
    *lpcyAveChar = lptm->tmHeight;

    // Now that we've caclulated the correct height & width,
    //   delete the font and re-create it
    DeleteObject (*lphFont);

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
                      &cyAveCharTC);
    // Tell the TC about the new font
    SendMessageW (hWndTC, WM_SETFONT, (WPARAM) hFontTC, MAKELPARAM (TRUE, 0));

    // Repaint the TC labels
    InvalidateRect (hWndTC, NULL, TRUE);
} // End CreateNewFontTC


//***************************************************************************
//  $CreateNewFontSM
//
//  Create a new font for the SM windows.
//***************************************************************************

void CreateNewFontSM
    (void)

{
    ENUMSETFONTW enumSetFontW;

    // Call common routine to set various variables
    CreateNewFontCom (&hFontSM,
                      &lfSM,
                      &cfSM,
                      &tmSM,
                      &cxAveCharSM,
                      &cyAveCharSM);
    // Initialize the struct
    enumSetFontW.lpwClassName = LSMWNDCLASS;
    enumSetFontW.hFont        = hFontSM;

    // Refont the SM windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFontW, (LPARAM) &enumSetFontW);

#ifdef DEBUG
    // Initialize the struct
    enumSetFontW.lpwClassName = LDBWNDCLASS;
    enumSetFontW.hFont        = hFontSM;

    // Refont the DB windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFontW, (LPARAM) &enumSetFontW);
#endif
} // End CreateNewFontSM


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
                      &cyAveCharFE);
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
                      &cyAveCharME);
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
                      &cyAveCharVE);
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
    lpfnOldTabCtrlWndProc = (WNDPROC)
      SetWindowLongW (hWndTC,
                      GWL_WNDPROC,
                      (long) (WNDPROC) &LclTabCtrlWndProc);
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
    RECT         rcDtop;    // Rectangle for desktop
    HWND         hWndActive,
                 hWndMC;

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

            // Read in window-specific registry entries
            ReadRegWnd (hWnd);

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

            // Load a CLEAR WS
            if (!CreateNewTab (hWnd,
                               L"",
                               TabCtrl_GetItemCount (hWndTC),
                               FALSE))
                return -1;          // Stop the whole process

            // *************** Fonts ***********************************
            // Create a new font for various windows
            CreateNewFontTC ();
            CreateNewFontSM ();
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
#define lpnmh   ((LPNMHDR) lParam)

            // Split cases based upon the notification code
            switch (lpnmh->code)
            {
                case TTN_NEEDTEXTW:     // idTT = (int) wParam;
                                        // lpttt = (LPTOOLTIPTEXTW) lParam;
                {
                    static WCHAR TooltipText[_MAX_PATH];
                    LPAPLCHAR    lpMemWSID;     // Ptr to []WSID global memory
#define lpttt   ((LPTOOLTIPTEXTW) lParam)

                    // Get a ptr to the ws name
                    lpMemWSID = PointToWsName (lpttt->hdr.idFrom);
#ifndef DEBUG
                    // Return a ptr to the stored tooltip text
                    lstrcpyW (TooltipText, lpMemWSID);
#else
                    {
                        HGLOBAL      hGlbPTD;       // PerTabData global memory handle
                        LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

                        // Get the PerTabData global memory handle
                        hGlbPTD = GetPerTabHandle (lpttt->hdr.idFrom);

                        // Lock the memory to get a ptr to it
                        lpMemPTD = MyGlobalLock (hGlbPTD);

                        // Return a ptr to the stored tooltip text
                        wsprintfW (TooltipText,
                                   L"hWndMC=%08X: %s",
                                   lpMemPTD->hWndMC,
                                   lpMemWSID);
                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
                    }
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
                    gLstTab = TabCtrl_GetCurSel (hWndTC);

                    return FALSE;

                case TCN_SELCHANGE:     // idTabCtl = (int) LOWORD(wParam);
                                        // lpnmhdr = (LPNMHDR) lParam;
                    // Save the index of the incoming tab
                    gCurTab = TabCtrl_GetCurSel (hWndTC);

                    // Hide the child windows of the outgoing tab
                    if (gLstTab NE -1)
                        ShowHideChildWindows (GetWndMC (gLstTab), FALSE);
                    // Show the child windows of the incoming tab
                    if (gCurTab NE -1)
                        ShowHideChildWindows (GetWndMC (gCurTab), TRUE);
                    return FALSE;       // We handled the msg

                default:
                    break;
            } // End SWITCH

            break;                  // Continue with next handler
#undef  lpnmh

        case WM_DRAWITEM:           // idCtl = (UINT) wParam;             // control identifier
                                    // lpdis = (LPDRAWITEMSTRUCT) lParam; // item-drawing information
        {
#define lpdis   ((LPDRAWITEMSTRUCT) lParam)

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
                    SendMessageW (hWndActive, MYWM_PASTE_APLWIN, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_APL2:
                    SendMessageW (hWndActive, MYWM_PASTE_APL2, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_PASTE_ISO:
                    SendMessageW (hWndActive, MYWM_PASTE_ISO, 0, 0);

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
                    WinHelp (hWnd, szHlpDPFE, HELP_INDEX, 0);

                    return FALSE;       // We handled the msg

                case IDM_ABOUT:
                    DialogBox (_hInstance,
                               MAKEINTRESOURCE (IDD_ABOUT),
                               hWnd,
                               &AboutDlgProc);
                    return FALSE;       // We handled the msg

                // The following messages come from the Tab Control.
                // They are sent here because we need a window whose
                //   WM_COMMAND we control so as not to interfere with
                //   any other window's meaning for these menu items.
                case IDM_NEW_WS:
                    // Load a CLEAR WS
                    if (!CreateNewTab (hWnd,
                                       L"",
                                       (gOverTab EQ -1) ? 999 : gOverTab + 1,
                                       FALSE))
                        return -1;          // Stop the whole process

                    return FALSE;   // We handled the msg

                case IDM_LOAD_WS:
                    DbgBrk ();      // ***FINISHME*** -- IDM_LOAD_WS



                    return FALSE;   // We handled the msg

                case IDM_XLOAD_WS:
                    DbgBrk ();      // ***FINISHME*** -- IDM_XLOAD_WS



                    return FALSE;   // We handled the msg

                case IDM_COPY_WS:
                    DbgBrk ();      // ***FINISHME*** -- IDM_COPY_WS



                    return FALSE;   // We handled the msg

                case IDM_PCOPY_WS:
                    DbgBrk ();      // ***FINISHME*** -- IDM_PCOPY_WS



                    return FALSE;   // We handled the msg

                case IDM_SAVE_WS:
                    CmdSave_EM (L"");   // Handle the same as )SAVE

                    return FALSE;   // We handled the msg

                case IDM_SAVE_AS_WS:
                    DbgBrk ();      // ***FINISHME*** -- IDM_SAVE_AS_WS



                    return FALSE;   // We handled the msg

                case IDM_DROP_WS:
                    DbgBrk ();      // ***FINISHME*** -- IDM_DROP_WS



                    return FALSE;   // We handled the msg

                case IDM_DUP_WS:
                    DbgBrk ();      // ***FINISHME*** -- IDM_DUP_WS



                    return FALSE;   // We handled the msg

                case IDM_SAVECLOSE_WS:
                    if (CmdSave_EM (L""))   // Handle the same as )SAVE
                        // Close the tab
                        CloseTab (gOverTab);

                    return FALSE;   // We handled the msg

                case IDM_CLOSE_WS:
                    // Close the tab
                    CloseTab (gOverTab);

                    return FALSE;   // We handled the msg

                case IDM_CLOSE_FN:
                    // Tell the active window to handle it
                    PostMessageW (hWndActive, MYWM_CLOSE_FN, wParam, lParam);

                    return FALSE;   // We handled the msg

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
            if (fHelp && szHlpDPFE[0])   // If we used help and there's a valid .HLP file, ...
            {
                WinHelp (hWnd, szHlpDPFE, HELP_QUIT, 0); // Quit it
                fHelp = FALSE;
            } // End IF

            // Save environment variables
            SaveEnvironment ();

            // Ask the child windows if it's OK to close
            if (EnumChildWindows (hWnd, EnumCallbackQueryClose, 0))
            {
#ifdef DEBUG
                // Tell the debugger windows to unhook its hooks
                EnumChildWindows (hWnd, EnumCallbackUnhookDebugger, 0);
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
    int iCurTab;            // Index of the current tab

    // If the Tab Control is not defined, quit
    if (hWndTC EQ NULL)
        return NULL;

    // Get the index of the currently selected tab
    iCurTab = TabCtrl_GetCurSel (hWndTC);

    // If no tab selected (early in MFWndProc processing), quit
    if (iCurTab EQ -1)
        return NULL;

    return GetWndMC (iCurTab);
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

    return SendMessageW (hWnd, WM_QUERYENDSESSION, 0, 0);
} // End EnumCallbackQueryClose


#ifdef DEBUG
//***************************************************************************
//  $EnumCallbackCloseDebugger
//
//  EnumChildWindows callback to tell debugger windows to unhook its hooks
//***************************************************************************

BOOL CALLBACK EnumCallbackUnhookDebugger
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // If it's a debugger window, ...
    if (IzitDB (hWnd))
        // Tell it to unhook its hooks
        SendMessageW (hWnd, MYWM_UNHOOK, 0, 0);

    return TRUE;
} // End EnumCallbackUnhookDebugger
#endif


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
    // Save in global variable
    _hInstance = hInstance;

    // Allocate virtual memory for the char temporary storage
    memVirtStr[MEMVIRT_SZTEMP].MaxSize = DEF_CTEMP_MAXSIZE * sizeof (char);
    memVirtStr[MEMVIRT_SZTEMP].IniAddr = (LPUCHAR)
    lpszTemp =
      VirtualAlloc (NULL,       // Any address
                    memVirtStr[MEMVIRT_SZTEMP].MaxSize,
                    MEM_RESERVE,        // memVirtStr
                    PAGE_READWRITE);
    if (!lpszTemp)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("InitInstance:  VirtualAlloc for <lpszTemp> failed");

        return FALSE;           // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (lpszTemp,
                  DEF_CTEMP_INITSIZE * sizeof (char),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Allocate virtual memory for the WCHAR temporary storage
    memVirtStr[MEMVIRT_WSZTEMP].MaxSize = DEF_WTEMP_MAXSIZE * sizeof (WCHAR);
    memVirtStr[MEMVIRT_WSZTEMP].IniAddr = (LPUCHAR)
    lpwszTemp =
      VirtualAlloc (NULL,       // Any address
                    memVirtStr[MEMVIRT_WSZTEMP].MaxSize,
                    MEM_RESERVE,        // memVirtStr
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

    // Allocate virtual memory for the WCHAR Formatting storage
    memVirtStr[MEMVIRT_WSZFORMAT].MaxSize = DEF_WFORMAT_MAXSIZE * sizeof (WCHAR);
    memVirtStr[MEMVIRT_WSZFORMAT].IniAddr = (LPUCHAR)
    lpwszFormat =
      VirtualAlloc (NULL,       // Any address
                    memVirtStr[MEMVIRT_WSZFORMAT].MaxSize,
                    MEM_RESERVE,        // memVirtStr
                    PAGE_READWRITE);
    if (!lpwszFormat)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("InitInstance:  VirtualAlloc for <lpwszFormat> failed");

        return FALSE;           // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (lpwszFormat,
                  DEF_WFORMAT_INITSIZE * sizeof (WCHAR),
                  MEM_COMMIT,
                  PAGE_READWRITE);
#ifdef DEBUG
    // Allocate virtual memory for the char debug storage
    memVirtStr[MEMVIRT_SZDEBUG].MaxSize = DEF_DEBUG_MAXSIZE * sizeof (char);
    memVirtStr[MEMVIRT_SZDEBUG].IniAddr = (LPUCHAR)
    lpszDebug =
      VirtualAlloc (NULL,       // Any address
                    memVirtStr[MEMVIRT_SZDEBUG].MaxSize,
                    MEM_RESERVE,        // memVirtStr
                    PAGE_READWRITE);
    if (!lpszDebug)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("InitInstance:  VirtualAlloc for <lpszDebug> failed");

        return FALSE;           // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (lpszDebug,
                  DEF_DEBUG_INITSIZE * sizeof (char),
                  MEM_COMMIT,
                  PAGE_READWRITE);

    // Allocate virtual memory for the WCHAR debug storage
    memVirtStr[MEMVIRT_WSZDEBUG].MaxSize = DEF_DEBUG_MAXSIZE * sizeof (WCHAR);
    memVirtStr[MEMVIRT_WSZDEBUG].IniAddr = (LPUCHAR)
    lpwszDebug =
      VirtualAlloc (NULL,       // Any address
                    memVirtStr[MEMVIRT_WSZDEBUG].MaxSize,
                    MEM_RESERVE,        // memVirtStr
                    PAGE_READWRITE);
    if (!lpwszDebug)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("InitInstance:  VirtualAlloc for <lpwszDebug> failed");

        return FALSE;           // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (lpwszDebug,
                  DEF_DEBUG_INITSIZE * sizeof (WCHAR),
                  MEM_COMMIT,
                  PAGE_READWRITE);
#endif
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
    // *************** Temporary Storage ***********************
#ifdef DEBUG
    if (lpszDebug)
    {
        VirtualFree (lpszDebug, 0, MEM_RELEASE); lpszDebug = NULL;
    } // End IF

    if (lpwszDebug)
    {
        VirtualFree (lpwszDebug, 0, MEM_RELEASE); lpwszDebug = NULL;
    } // End IF
#endif
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
//  $ParseCommandLine
//
//  Parse any command line
//***************************************************************************

BOOL ParseCommandLine
    (LPSTR lpCmdLine)

{
    LPCHAR p;

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
    MSG  Msg;           // Message for GetMessage loop

    // This is needed by Wine's EDITCTRL.C
    user32_module = hInstance;

    // Ensure that the common control DLL is loaded.
    InitCommonControls ();

    // Get the DPFE for our files
    GetFileNames (hInstance);

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
    control87(PC_53, MCW_PC);

    // If there's a command line, parse it
    if (!ParseCommandLine (lpCmdLine))
        return -1;                  // Exit

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

    // Read in global registry values
    ReadRegGlb ();

    // Initialize ChooseFont argument here
    //   so its settings will be present
    //   for the second and subsequent time
    //   the common dialog is called.
    InitChooseFont (&cfSM, &lfSM, DEF_SMPTSIZE);
    InitChooseFont (&cfTC, &lfTC, DEF_TCPTSIZE);
    InitChooseFont (&cfFE, &lfFE, DEF_FEPTSIZE);
    InitChooseFont (&cfME, &lfME, DEF_MEPTSIZE);
    InitChooseFont (&cfVE, &lfVE, DEF_VEPTSIZE);

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
                       wszMFClass,                      // Class
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
        while (GetMessage (&Msg, NULL, 0, 0))
        {
            HWND hWndMC;        // MDI Client window handle

            // Get the window handle of the currently active MDI Client
            hWndMC = GetActiveMC (hWndTC);

            // Handle MDI messages and accelerators
            if (!TranslateMDISysAccel (hWndMC, &Msg)
             && ((!hAccel) || !TranslateAccelerator (hWndMF, hAccel, &Msg)))
            {
                TranslateMessage (&Msg);
                DispatchMessage  (&Msg);
            } // End IF
        } // End WHILE
    } __except (CheckException (GetExceptionInformation (), "WinMain"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except

    // GetMessage returned FALSE for a Quit message
EXIT4:
    DeleteCriticalSection (&CSO1);
    DeleteCriticalSection (&CSO0);
EXIT3:
    UninitApplication (hInstance);
EXIT2:
    UninitInstance (hInstance);

    return 0;
} // End WinMain


//***************************************************************************
//  End of File: main.c
//***************************************************************************
