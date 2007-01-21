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

#define DEF_CTEMP_MAXSIZE   1024        // Maximum size of char  temporary storage
#define DEF_CTEMP_INITSIZE  1024        // Initial ...
#define DEF_WTEMP_MAXSIZE   1024        // Maximum size of WCHAR ...
#define DEF_WTEMP_INITSIZE  1024        // Initial ...

WNDPROC lpfnOldTabCtrlWndProc;          // Save area for old Tab Control procedure
COLORREF crbk[2] = {RGB (249,215,216), RGB (212,223,245)}, // Background
         crfg[2] = {RGB (  0,  0,  0), RGB (  0,  0,  0)}, // Foreground, normal
         crhl[2] = {RGB (255,  0,  0), RGB (255,  0,  0)}; // Foreground, highlighted

int nMinState,                          // Minimized state as per WinMain
    iScrollSize;                        // Width of a vertical scrollbar
HANDLE hAccel;                          // Keyboard accelerators
BOOL fHelp = FALSE,                     // TRUE iff we displayed help
     bCommandLine;                      // ...      there is a filename on the command line

HICON hIconMF_Large, hIconMF_Small,     // Icon handles
      hIconSM_Large, hIconSM_Small,
      hIconDB_Large, hIconDB_Small;

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
     pszNoCreateTTWnd[]     = "Unable to create ToolTip window",
     pszNoInsertTCTab[]     = "Unable to create a new Tab";


//***************************************************************************
//  InitChooseFont
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

    hDC = GetDC (HWND_DESKTOP);
    iLogPixelsY = GetDeviceCaps (hDC, LOGPIXELSY);
    lplf->lfHeight = -MulDiv (iPtSize, iLogPixelsY, 72);
    ReleaseDC (HWND_DESKTOP, hDC);

    lpcf->lStructSize = sizeof (CHOOSEFONT);
////lpcf->hDC =                     // Only w/CF_PRINTERFONTS
    lpcf->lpLogFont  = lplf;
    lpcf->iPointSize = iPtSize * 10;// Font point size in 1/10ths
    lpcf->Flags = CF_INITTOLOGFONTSTRUCT
                | CF_FORCEFONTEXIST
                | CF_SCREENFONTS;
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
//  MyChooseFontSM
//
//  Choose a font for the SM window
//***************************************************************************

void MyChooseFontSM
    (void)

{
    if (ChooseFont (&cfSM))     // Choose it
    {
        // Create a new font for the SM
        CreateNewFontSM ();

        // Repaint the SM window
        InvalidateRect (hWndSM, NULL, TRUE);
    } // End IF
} // End MyChooseFontSM


//***************************************************************************
//  MyChooseFontTC
//
//  Choose a font for the TC labels
//***************************************************************************

void MyChooseFontTC
    (void)

{
    if (ChooseFont (&cfTC))     // Choose it
    {
        // Create a new font for the TC
        CreateNewFontTC ();

        // Repaint the TC labels
        InvalidateRect (hWndTC, NULL, TRUE);
    } // End IF
} // End MyChooseFontTC


//***************************************************************************
//  CreateNewFontTC
//
//  Create a new font for the TC.
//***************************************************************************

void CreateNewFontTC
    (void)

{
    // Delete the previous handle (if any)
    if (hFontTC)
    {
        // Delete the TC font handle
        MyDeleteObject (hFontTC); hFontTC = NULL;
    } // End IF

    // Create the font
    hFontTC = MyCreateFontIndirect (&lfTC);

    // Tell the TC about the new font
    SendMessage (hWndTC, WM_SETFONT, (WPARAM) hFontTC, 0);
} // End CreateNewFontTC


//***************************************************************************
//  CreateToolTip
//
//  Creates the ToolTip window and initializes it.
//***************************************************************************

HWND CreateToolTip
    (HWND hWndParent)

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
        CreateWindowEx (0L,                             // Extended styles
                        WC_TABCONTROL,                  // Class
                        szTCTitle,                      // Window title (for debugging purposes only)
                        TCS_OWNERDRAWFIXED
                      | WS_CHILD
                      | WS_CLIPSIBLINGS
                      | WS_VISIBLE,                     // Styles
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

    // Subclass the tab control so we can handle some of its messages
    lpfnOldTabCtrlWndProc = (WNDPROC)
      SetWindowLong (hWndTC,
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
//  LclTabCtrlWndProc
//
//  Local window procedure for the Tab Control
//***************************************************************************

LRESULT WINAPI LclTabCtrlWndProc
    (HWND   hWnd,
     UINT   message,
     WPARAM wParam,
     LPARAM lParam)

{
    TC_HITTESTINFO tcHit;
    static int     iCurTab;

    // Split cases
    switch (message)
    {
        case WM_MOUSEHOVER:         // fwKeys = wParam;        // key flags
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor in CA
                                    // yPos = HIWORD(lParam);  // vertical position of cursor  in CA
            // Save the client coordinates
            tcHit.pt.x = LOWORD (lParam);
            tcHit.pt.y = HIWORD (lParam);

            // Ask the Tab Control if we're over a tab
            iCurTab = TabCtrl_HitTest (hWndTC, &tcHit);

            DbgBrk ();

            // Ensure we're over a tab
            if (iCurTab EQ -1)
                break;

            // Draw the tab with the text highlighted
            DrawTabText (hWnd,
                         NULL,
                         iCurTab,
                         crhl[iCurTab],
                         crbk[iCurTab],
                         NULL,
                         NULL);
            return FALSE;       // We handled the message

        case WM_MOUSELEAVE:
            // Draw the tab with the text normal
            DrawTabText (hWnd,
                         NULL,
                         iCurTab,
                         crfg[iCurTab],
                         crbk[iCurTab],
                         NULL,
                         NULL);

            return FALSE;       // We handled the message
    } // End SWITCH

    return CallWindowProcW (lpfnOldTabCtrlWndProc,
                            hWnd,
                            message,
                            wParam,
                            lParam); // Pass on down the line
} // End LclTabCtrlWndProc


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

#define Assign(a)   lpMem->a = a;
    PERTABVARS
#undef  Assign

    // Get the handle of the active MDI window
    lpMem->hWndActive = (HWND) SendMessage (hWndMC, WM_MDIGETACTIVE, 0, 0);

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

#define Assign(a)   a = lpMem->a;
    PERTABVARS
#undef  Assign

    // Set the active MDI window
    SendMessage (hWndMC, WM_MDIACTIVATE, (WPARAM) lpMem->hWndActive, 0);

    // Give it the keyboard focus
    SetFocus (lpMem->hWndActive);

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
    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

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
    (HWND hWndMC,
     BOOL bShow)        // TRUE iff showing the window, FALSE if hiding it

{
    if (!hWndMC)
        return;

    // Put the MDI Client window at the top (SHOW) or bottom (HIDE)
    //   of the Z-order
    SetWindowPos (hWndMC,
                  bShow ? HWND_TOP : HWND_BOTTOM,
                  0, 0, 0, 0,
                  SWP_NOMOVE
                | SWP_NOSIZE);

    // Show/hide the MDI Client window
    ShowWindow (hWndMC, bShow ? SW_SHOW : SW_HIDE);

    // Loop through the child windows
    EnumChildWindows (hWndMC,
                     &ShowHideEnumChildProc,
                     bShow ? SW_SHOW : SW_HIDE);
} // End ShowHideChildWindows


//***************************************************************************
//  CreateNewTab
//
//  Create a new tab
//***************************************************************************

BOOL CreateNewTab
    (HWND  hWndParent,
     LPSTR lpszDPFE)        // Drive, Path, Filename, Ext of the workspace

{
    int          iCurTab;
    TC_ITEM      tcItem;
    HGLOBAL      hGlbData;
    LPPERTABDATA lpMem = NULL;
    BOOL         bRet = TRUE;
    RECT         rc;        // Rectangle for setting size of window
    int          rcLeft, rcRight, rcBottom;
    CLIENTCREATESTRUCT ccs; // For MDI Client window
    LPCHAR       p, q;
    int          iLabelText;

    // Get the size and position of the parent window.
    GetClientRect (hWndParent, &rc);

    // Save data from the current WS into global memory
    SaveWsData (hGlbCurTab);

    // Hide the child windows of the outgoing tab
    ShowHideChildWindows (hWndMC, FALSE);

    // Allocate per tab data
    hGlbData = MyGlobalAlloc (GHND, sizeof (PERTABDATA));
    if (!hGlbData)
        return FALSE;       // Stop the whole process

    // Calculate the offset to the label text
    q = lpszDPFE;
    while (p = strchr (q, '\\'))
        q = p + 1;
    iLabelText = q - lpszDPFE;

    tcItem.mask    = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
    tcItem.iImage  = -1;
    tcItem.pszText = &lpszDPFE[iLabelText];
    tcItem.lParam  = (LPARAM) hGlbData;

    iCurTab = TabCtrl_InsertItem (hWndTC, 0, &tcItem);

    if (iCurTab == -1)
    {
        MB (pszNoInsertTCTab);
        bRet = FALSE;       // Stop the whole process
        goto ERROR_EXIT;
    } // End IF

    // Lock the per-tab data to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Save the DPFE
    lstrcpy (lpMem->DPFE, lpszDPFE);

    // Save offset to label text
    lpMem->iLabelText = iLabelText;

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

    // Fill in the CLIENTCREATESTRUCT for the MDI Client
    ccs.hWindowMenu = GetSubMenu (GetMenu (hWndParent), IDMPOS_WINDOW);
    ccs.idFirstChild = IDM_CHILDWINDOW;

    // Create the MDI client window
    lpMem->hWndMC =
    CreateWindowEx (0,                      // Extended styles
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
        bRet = FALSE;       // Stop the whole process
        goto ERROR_EXIT;
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
////hWndSM = lpMem->hWndSM;
////hWndDB = lpMem->hWndDB;

    lpMem->hWndActive = hWndSM;

    // Save the handle
    hGlbCurTab = hGlbData;

    // Clear data in this WS to global default values
    ClearWsData ();

    // Show the child windows of the incoming tab
    ShowHideChildWindows (hWndMC, TRUE);

    // Activate this tab
    TabCtrl_SetCurSel (hWndTC, iCurTab);
ERROR_EXIT:
    // If we failed, ...
    if ((!bRet) && lpMem && lpMem->hWndMC)
    {
        if (lpMem->hWndDB)
            SendMessage (lpMem->hWndMC, WM_MDIDESTROY, (WPARAM) (lpMem->hWndDB), 0);
        if (lpMem->hWndSM)
            SendMessage (lpMem->hWndMC, WM_MDIDESTROY, (WPARAM) (lpMem->hWndSM), 0);
        DestroyWindow (lpMem->hWndMC);
    } // End IF

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
//  MF_Create
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
//  MF_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void MF_Delete
    (HWND hWnd)

{
} // End MF_Delete


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
    RECT         rcDtop;    // Rectangle for desktop
    int          iCurTab;
    TC_ITEM      tcItem;
    HBRUSH       hBrush;

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

            // Read in window-specific registry entries
            ReadRegWnd (hWnd);

            // *************** Bitmaps *********************************
            hBitMapLineCont = LoadBitmap (_hInstance, MAKEINTRESOURCE (IDB_LINECONT));
            if (hBitMapLineCont)
            {
                GetObject (hBitMapLineCont, sizeof (BITMAP), (LPSTR) &bmLineCont);

                iLCWidth = 2 + bmLineCont.bmWidth + 2;  // Width of line continuation column
            } // End IF

            // *************** Fonts ***********************************
            // Create a new font for the TC
            CreateNewFontTC ();

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
            if (!CreateNewTab (hWnd, "R:\\NARS2000\\trunk\\CLEAR WS-2"))
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
                case TTN_NEEDTEXT:      // idTT = (int) wParam;
                                        // lpttt = (LPTOOLTIPTEXT) lParam;
                {
                    TC_ITEM      tcItem;
                    HGLOBAL      hGlbData;
                    LPPERTABDATA lpMem;
                    static char  TooltipText[_MAX_PATH];

#define lpttt   ((LPTOOLTIPTEXT) lParam)

                    // The page # of the tab is in lpttt->hdr.idFrom
                    tcItem.mask = TCIF_PARAM;

                    // Get the lParam
                    TabCtrl_GetItem (hWndTC, lpttt->hdr.idFrom, &tcItem);

                    // Get the lParam (an HGLOBAL)
                    hGlbData = (HGLOBAL) tcItem.lParam;

                    // Lock the memory to get a ptr to it
                    lpMem = MyGlobalLock (hGlbData);

                    // Return a ptr to the stored tooltip text
                    lstrcpy (TooltipText, (LPCHAR) &lpMem->DPFE);

                    lpttt->lpszText = TooltipText;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbData); lpMem = NULL;
#undef  lpttt
                    return FALSE;
                } // End TTN_NEEDTEXT

                case TCN_SELCHANGING:   // idTabCtl = (int) LOWORD(wParam);
                                        // hwndTabCtl = (HWND) lParam;
                    // Save data from the current WS into global memory
                    SaveWsData (hGlbCurTab);

                    DestroyCaret ();    // 'cause we just lost the focus

                    return FALSE;

                case TCN_SELCHANGE:     // idTabCtl = (int) LOWORD(wParam);
                                        // hwndTabCtl = (HWND) lParam;
                    // Hide the child windows of the outgoing tab
                    ShowHideChildWindows (hWndMC, FALSE);

                    // Get the index of the currently selected tab
                    iCurTab = TabCtrl_GetCurSel (hWndTC);

                    // Get the lParam for this tab
                    tcItem.mask = TCIF_PARAM;
                    TabCtrl_GetItem (hWndTC, iCurTab, &tcItem);

                    // Save the handle
                    hGlbCurTab = (HGLOBAL) (tcItem.lParam);

                    // Restore data into the current WS from global memory
                    RestWsData (hGlbCurTab);

                    // Show the child windows of the incoming tab
                    ShowHideChildWindows (hWndMC, TRUE);

                    // Because we're activating a new tab,
                    //   the caret must be recreated
                    // Create a default sized system caret for display
                    MyCreateCaret ();

                    return FALSE;       // We handled the msg

                default:
                    break;
            } // End SWITCH
#undef  lpnmh

            break;                  // Continue with next handler

        case WM_DRAWITEM:           // idCtl = (UINT) wParam;             // control identifier
                                    // lpdis = (LPDRAWITEMSTRUCT) lParam; // item-drawing information
        {
#define lpdis   ((LPDRAWITEMSTRUCT) lParam)

            // Ensure this message is for our tab control
            if (lpdis->CtlType NE ODT_TAB)
                break;

            // Split cases absed upon the item action
            switch (lpdis->itemAction)
            {
                case ODA_DRAWENTIRE:
                    // Fill the background of the tab
                    hBrush = CreateSolidBrush (crbk[lpdis->itemID]);
                    FillRect (lpdis->hDC, &lpdis->rcItem, hBrush);
                    DeleteObject (hBrush); hBrush = NULL;

                    // Get the lParam for this tab
                    tcItem.mask = TCIF_PARAM;
                    TabCtrl_GetItem (hWndTC, lpdis->itemID, &tcItem);

                    // Write the text
                    DrawTabText (hWnd,
                                 lpdis->hDC,
                                 lpdis->itemID,
                                 crfg[lpdis->itemID],
                                 crbk[lpdis->itemID],
                                &lpdis->rcItem,
                                 (HGLOBAL) (tcItem.lParam));
                    break;

                case ODA_FOCUS:
                    DbgBrk ();


                    break;

                case ODA_SELECT:
                    DbgBrk ();


                    break;
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
                    //   a new font for the Session Manager window
                    MyChooseFontSM ();

                    return FALSE;       // We handled the msg

                case IDM_TABFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Tab Control labels
                    MyChooseFontTC ();

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

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, we handle erase background
            // in the WM_PAINT message for the child windows.
            return TRUE;            // We erased the background

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
//  DrawTabText
//
//  Draw the text on a tab
//***************************************************************************

void DrawTabText
    (HWND     hWnd,
     HDC      hDC,
     int      iCurTab,
     COLORREF crfg,
     COLORREF crbk,
     LPRECT   lpRect,
     HGLOBAL  hGlbData)

{
    LPPERTABDATA lpMem;
    HPEN         hPen;
    HDC          hDC2 = NULL;
    TC_ITEM      tcItem;
    RECT         rcLcl;

    if (hDC EQ NULL)
        hDC = hDC2 = GetDC (hWnd);

    if (hGlbData EQ NULL)
    {
        // The page # of the tab is in iCurTab
        tcItem.mask = TCIF_PARAM;

        // Get the lParam
        TabCtrl_GetItem (hWndTC, iCurTab, &tcItem);

        // Get the lParam (an HGLOBAL)
        hGlbData = (HGLOBAL) tcItem.lParam;
    } // End IF

    if (lpRect EQ NULL)
    {
        TabCtrl_GetItemRect (hWndTC, iCurTab, &rcLcl);
        lpRect = &rcLcl;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Write the text
    hPen = CreatePen (PS_SOLID, 1, crfg);
    SetBkColor (hDC, crbk);
    SetBkMode  (hDC, OPAQUE);
    DrawText (hDC,
             &lpMem->DPFE[lpMem->iLabelText],
              lstrlen (&lpMem->DPFE[lpMem->iLabelText]),
              lpRect,
              DT_SINGLELINE
            | DT_CENTER
            | DT_VCENTER);
    DeleteObject (hPen); hPen = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    if (hDC2 NE NULL)
        ReleaseDC (hWnd, hDC2);
} // End DrawTabText


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
    if (lfSM_CWS.lfHeight EQ 0)
        lfSM_CWS.lfHeight = lfSM.lfHeight;

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
