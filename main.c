//***************************************************************************
//  NARS2000 -- Open Source APL Interpreter
//***************************************************************************

#define COMPILE_MULTIMON_STUBS
//#define WINVER       0x04FF // Needed for WINUSER.H definitions
//#define _WIN32_WINNT 0x04FF // ...

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <windowsx.h16>
//#include <multimon.h>   // Multiple monitor support
//#include <QControl.h>
#include <limits.h>
#include <direct.h>

#include "main.h"
#include "resdebug.h"
#include "resource.h"

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

typedef struct tagENUMSETFONT
{
    LPCHAR  lpClassName;
    HFONT   hFont;
} ENUMSETFONT, *LPENUMSETFONT;


#define DEF_CTEMP_MAXSIZE   1024        // Maximum size of char  temporary storage
#define DEF_CTEMP_INITSIZE  1024        // Initial ...
#define DEF_WTEMP_MAXSIZE   1024        // Maximum size of WCHAR ...
#define DEF_WTEMP_INITSIZE  1024        // Initial ...

int nMinState,                          // Minimized state as per WinMain
    iScrollSize;                        // Width of a vertical scrollbar
HANDLE hAccel;                          // Keyboard accelerators
BOOL fHelp = FALSE,                     // TRUE iff we displayed help
     bCommandLine;                      // ...      there is a filename on the command line

HICON hIconMF_Large, hIconMF_Small,     // Icon handles
      hIconSM_Large, hIconSM_Small,
#ifdef DEBUG
      hIconDB_Large, hIconDB_Small,
#endif
      hIconFE_Large, hIconFE_Small,
      hIconME_Large, hIconME_Small,
      hIconVE_Large, hIconVE_Small,
      hIconClose;

char szMFTitle[]        = "NARS2000" APPEND_DEBUG,                  // Master frame window title
     szTCTitle[]        = "NARS2000 Tab Control Window" APPEND_DEBUG;// Tab Control ... (for debugging purposes only)

#define MFWNDCLASS      "MFClass"       // Master Frame Window class

char szMFClass[]        = MFWNDCLASS;   // Master Frame Window class

char pszNoRegMFWndClass[]   = "Unable to register window class <" MFWNDCLASS ">.",
     pszNoRegSMWndClass[]   = "Unable to register window class <" SMWNDCLASS ">.",
#ifdef DEBUG
     pszNoRegDBWndClass[]   = "Unable to register window class <" DBWNDCLASS ">.",
#endif
     pszNoRegFEWndClass[]   = "Unable to register window class <" FEWNDCLASS ">.",
     pszNoRegMEWndClass[]   = "Unable to register window class <" MEWNDCLASS ">.",
     pszNoRegVEWndClass[]   = "Unable to register window class <" VEWNDCLASS ">.";

char pszNoCreateMFWnd[]     = "Unable to create Master Frame window",
     pszNoCreateTCWnd[]     = "Unable to create Tab Control window",
     pszNoCreateTTWnd[]     = "Unable to create ToolTip window";


//***************************************************************************
//  EnumCallbackSetFont
//
//  EnumChildWindows callback to set a window's font
//***************************************************************************

BOOL CALLBACK EnumCallbackSetFont
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

{
    char szTemp[32];

    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    // Get the window's class name
    GetClassName (hWnd, szTemp, sizeof (szTemp));

#define lpEnumSetFont   ((LPENUMSETFONT) lParam)

    // If this is the matching class name,
    //   set the new font in place and redraw.
    if (lstrcmpi (lpEnumSetFont->lpClassName, szTemp) EQ 0)
    {
        SendMessage (hWnd, WM_SETFONT, (WPARAM) lpEnumSetFont->hFont, TRUE);
        InvalidateRect (hWnd, NULL, TRUE);
    } // End IF

    return TRUE;        // Keep on truckin'
#undef  lpEnumSetFont
} // End EnumCallbackSetFont


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
//  MyChooseFontTC
//
//  Choose a font for the TC labels
//***************************************************************************

void MyChooseFontTC
    (void)

{
    if (ChooseFont (&cfTC))     // Choose it
    {
        // Create a new font for the TC window
        CreateNewFontTC ();

        // Repaint the TC labels
        InvalidateRect (hWndTC, NULL, TRUE);
    } // End IF
} // End MyChooseFontTC


//***************************************************************************
//  MyChooseFontSM
//
//  Choose a font for the SM windows
//***************************************************************************

void MyChooseFontSM
    (void)

{
    if (ChooseFont (&cfSM))     // Choose it
    {
        // Create a new font for the SM windows
        CreateNewFontSM ();
    } // End IF
} // End MyChooseFontSM


//***************************************************************************
//  MyChooseFontFE
//
//  Choose a font for the FE windows
//***************************************************************************

void MyChooseFontFE
    (void)

{
    if (ChooseFont (&cfFE))     // Choose it
    {
        // Create a new font for the FE windows
        CreateNewFontFE ();
    } // End IF
} // End MyChooseFontFE


//***************************************************************************
//  MyChooseFontME
//
//  Choose a font for the ME windows
//***************************************************************************

void MyChooseFontME
    (void)

{
    if (ChooseFont (&cfME))     // Choose it
    {
        // Create a new font for the ME windows
        CreateNewFontME ();
    } // End IF
} // End MyChooseFontME


//***************************************************************************
//  MyChooseFontVE
//
//  Choose a font for the VE windows
//***************************************************************************

void MyChooseFontVE
    (void)

{
    if (ChooseFont (&cfVE))     // Choose it
    {
        // Create a new font for the VE windows
        CreateNewFontVE ();
    } // End IF
} // End MyChooseFontVE


//***************************************************************************
//  CreateNewFontTC
//
//  Create a new font for the TC window.
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
//  CreateNewFontSM
//
//  Create a new font for the SM windows.
//***************************************************************************

void CreateNewFontSM
    (void)

{
    ENUMSETFONT enumSetFont;

    // Delete the previous handle (if any)
    if (hFontSM)
    {
        // Delete the SM font handle
        MyDeleteObject (hFontSM); hFontSM = NULL;
    } // End IF

    // Create the font
    hFontSM = MyCreateFontIndirect (&lfSM);

    // Initialize the struct
    enumSetFont.lpClassName = szSMClass;
    enumSetFont.hFont       = hFontSM;

    // Refont the SM windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFont, (LPARAM) &enumSetFont);

#ifdef DEBUG
    // Initialize the struct
    enumSetFont.lpClassName = szDBClass;
    enumSetFont.hFont       = hFontSM;

    // Refont the DB windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFont, (LPARAM) &enumSetFont);
#endif
} // End CreateNewFontSM


//***************************************************************************
//  CreateNewFontFE
//
//  Create a new font for the FE windows.
//***************************************************************************

void CreateNewFontFE
    (void)

{
    ENUMSETFONT enumSetFont;

    // Delete the previous handle (if any)
    if (hFontFE)
    {
        // Delete the FE font handle
        MyDeleteObject (hFontFE); hFontFE = NULL;
    } // End IF

    // Create the font
    hFontFE = MyCreateFontIndirect (&lfFE);

    // Initialize the struct
    enumSetFont.lpClassName = szFEClass;
    enumSetFont.hFont       = hFontFE;

    // Refont the FE windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFont, (LPARAM) &enumSetFont);
} // End CreateNewFontFE


//***************************************************************************
//  CreateNewFontME
//
//  Create a new font for the ME windows.
//***************************************************************************

void CreateNewFontME
    (void)

{
    ENUMSETFONT enumSetFont;

    // Delete the previous handle (if any)
    if (hFontME)
    {
        // Delete the ME font handle
        MyDeleteObject (hFontME); hFontME = NULL;
    } // End IF

    // Create the font
    hFontME = MyCreateFontIndirect (&lfME);

    // Initialize the struct
    enumSetFont.lpClassName = szMEClass;
    enumSetFont.hFont       = hFontME;

    // Refont the ME windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFont, (LPARAM) &enumSetFont);
} // End CreateNewFontME


//***************************************************************************
//  CreateNewFontVE
//
//  Create a new font for the VE windows.
//***************************************************************************

void CreateNewFontVE
    (void)

{
    ENUMSETFONT enumSetFont;

    // Delete the previous handle (if any)
    if (hFontVE)
    {
        // Delete the VE font handle
        MyDeleteObject (hFontVE); hFontVE = NULL;
    } // End IF

    // Create the font
    hFontVE = MyCreateFontIndirect (&lfVE);

    // Initialize the struct
    enumSetFont.lpClassName = szVEClass;
    enumSetFont.hFont       = hFontVE;

    // Refont the VE windows
    EnumChildWindows (hWndMF, &EnumCallbackSetFont, (LPARAM) &enumSetFont);
} // End CreateNewFontVE


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
//  EnumCallbackRestoreAll
//
//  EnumChildWindows callback to restore all MDI Child windows
//
//  lParam = unused
//***************************************************************************

BOOL CALLBACK EnumCallbackRestoreAll
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

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
} // End EnumCallbackRestoreAll


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

LRESULT APIENTRY MFWndProc
    (HWND hWnd,         // Window handle
     UINT message,      // Type of message
     UINT wParam,       // Additional information
     LONG lParam)       // ...

{
    RECT         rcDtop;    // Rectangle for desktop
    int          iCurTab;

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

            // *************** Image Lists *****************************
            hImageList =
            ImageList_Create (IMAGE_CX,         // Common width in pixels
                              IMAGE_CY,         // ...    height ...
                              ILC_COLOR32
                            | ILC_MASK,         // Flags
                              1,                // Max # images
                              0);               // # images by which the list can grow
            if (!hImageList)
                return -1;          // Stop the whole process

            // Add the Close button icon to the image list
            ImageList_AddIcon (hImageList, hIconClose);

            // Assign the image list to the tab control
            TabCtrl_SetImageList (hWndTC, hImageList);

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

            // Allocate per tab data as a dummy holder
            //   so that the first call to SaveWsData
            //   has something to save into.
            hGlbCurTab = MyGlobalAlloc (GHND, sizeof (PERTABDATA));
            if (!hGlbCurTab)
                return -1;          // Stop the whole process

            // Load a CLEAR WS
            if (!CreateNewTab (hWnd,
                               "CLEAR WS",
                               TabCtrl_GetItemCount (hWndTC)))
                return -1;          // Stop the whole process

            // *************** Fonts ***********************************
            // Create a new font for various windows
            CreateNewFontTC ();
            CreateNewFontSM ();
            CreateNewFontFE ();
            CreateNewFontME ();
            CreateNewFontVE ();

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
                    HGLOBAL      hGlbData;
                    LPPERTABDATA lpMem;
                    static char  TooltipText[_MAX_PATH];

#define lpttt   ((LPTOOLTIPTEXT) lParam)

                    // Get the per tab global memory handle
                    hGlbData = GetPerTabHandle (lpttt->hdr.idFrom);

                    // Lock the memory to get a ptr to it
                    lpMem = MyGlobalLock (hGlbData);

                    // Return a ptr to the stored tooltip text
                    lstrcpy (TooltipText, (LPCHAR) &lpMem->DPFE);

                    // ***DEBUG***
                    wsprintf (TooltipText,
                              "hWndMC=%08X, hGlbHist=%08X",
                              lpMem->hWndMC,
                              lpMem->hGlbHist);

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

                    // If the user clicked on the close button,
                    //   disallow this change so as to avoid
                    //   screen flicker
                    return ClickOnClose ();

                case TCN_SELCHANGE:     // idTabCtl = (int) LOWORD(wParam);
                                        // hwndTabCtl = (HWND) lParam;
                    // Hide the child windows of the outgoing tab
                    ShowHideChildWindows (hWndMC, FALSE);

                    // Get the index of the currently selected tab
                    iCurTab = TabCtrl_GetCurSel (hWndTC);

                    // Get the per tab global memory handle
                    hGlbCurTab = GetPerTabHandle (iCurTab);

                    // Restore data into the current WS from global memory
                    RestWsData (hGlbCurTab);

                    // Show the child windows of the incoming tab
                    ShowHideChildWindows (hWndMC, TRUE);

                    // Because we're activating a new tab,
                    //   the caret must be recreated
                    // Create a default sized system caret for display
                    MyCreateCaret (hWndSM);

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

            // Split cases based upon the item action
            switch (lpdis->itemAction)
            {
                case ODA_DRAWENTIRE:
                    // Draw the tab
                    DrawTab (hWndTC,
                             lpdis->hDC,
                             lpdis->itemID,
                            &lpdis->rcItem);
                    break;

////////////////case ODA_FOCUS:     // These actions don't appear to occur with a tab ctrl
////////////////case ODA_SELECT:
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
                    EnumChildWindows (hWndMC, EnumCallbackRestoreAll, 0);

                    PostMessage (hWndMC, WM_MDICASCADE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_ARRANGE_ICONS:
                    PostMessage (hWndMC, WM_MDIICONARRANGE, 0, 0);

                    return FALSE;       // We handled the msg

                case IDM_TCFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Tab Control labels
                    MyChooseFontTC ();

                    return FALSE;       // We handled the msg

                case IDM_SMFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Session Manager window
                    MyChooseFontSM ();

                    return FALSE;       // We handled the msg

                case IDM_FEFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Function Editor
                    MyChooseFontFE ();

                    return FALSE;       // We handled the msg

                case IDM_MEFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Matrix Editor
                    MyChooseFontME ();

                    return FALSE;       // We handled the msg

                case IDM_VEFONT:
                    // Display a Font Dialog so the user can choose
                    //   a new font for the Vector Editor
                    MyChooseFontVE ();

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
                                       "CLEAR WS",
                                       (iOverTab EQ -1) ? 999 : iOverTab + 1))
                        return -1;          // Stop the whole process

                    return FALSE;   // We handled the msg

                case IDM_DUP_WS:
                    // ***FINISHME***


                    return FALSE;   // We handled the msg

                case IDM_SAVECLOSE_WS:
                    // ***FINISHME***


                    return FALSE;   // We handled the msg

                case IDM_CLOSE_WS:
                    // Close the tab
                    CloseTab (iOverTab);

                    return FALSE;   // We handled the msg
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

            // Ask the child windows if it's OK to close
            if (EnumChildWindows (hWnd, EnumCallbackQueryClose, 0))
                // This also tells the child windows to close
                DestroyWindow (hWnd);
            break;                  // Continue with default handler
        } // End WM_CLOSE

        case WM_DESTROY:
            // Uninitialize window-specific resources
            MF_Delete (hWnd);

            // Destroy the fonts
            if (hFontTC)
            {
                DeleteObject (hFontTC); hFontTC = NULL;
            } // End IF

            if (hFontSM)
            {
                DeleteObject (hFontSM); hFontSM = NULL;
            } // End IF

            if (hFontFE)
            {
                DeleteObject (hFontFE); hFontFE = NULL;
            } // End IF

            if (hFontME)
            {
                DeleteObject (hFontME); hFontME = NULL;
            } // End IF

            if (hFontVE)
            {
                DeleteObject (hFontVE); hFontVE = NULL;
            } // End IF

            // Destroy the image list
            if (hImageList)
                ImageList_Destroy (hImageList);

            // Say goodbye
            PostQuitMessage (0);

            break;                  // Continue with default handler
    } // End SWITCH

////ODSAPI ("MFZ:", hWnd, message, wParam, lParam);
    return DefFrameProc (hWnd, hWndMC, message, wParam, lParam);
} // End MFWndProc


//***************************************************************************
//  EnumCallbackQueryClose
//
//  EnumChildWindows callback to qeury whether or not they can close
//***************************************************************************

BOOL CALLBACK EnumCallbackQueryClose
    (HWND  hWnd,            // Handle to child window
     LPARAM lParam)         // Application-defined value

{
////BOOL bClose;

    // When an MDI child window is minimized, Windows creates two windows: an
    // icon and the icon title.  The parent of the icon title window is set to
    // the MDI client window, which confines the icon title to the MDI client
    // area.  The owner of the icon title is set to the MDI child window.
    if (GetWindow (hWnd, GW_OWNER))     // If it's an icon title window, ...
        return TRUE;                    // skip it, and continue enumerating

    return SendMessage (hWnd, WM_QUERYENDSESSION, 0, 0);
////DbgBrk ();
////bClose = SendMessage (hWnd, WM_QUERYENDSESSION, 0, 0);
////
////if (!bClose)
////    DbgBrk ();
////*((LPBOOL) lParam) &= bClose;
////
////return bClose;
} // End EnumCallbackQueryClose


//***************************************************************************
//  InitApplication
//
//  Initializes window data and registers window class
//***************************************************************************

BOOL InitApplication
    (HANDLE hInstance)      // Current instance

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

    // Fill in Function Editor window class structure
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC) FEWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = GWLFE_EXTRA;
    wc.hInstance = hInstance;
    wc.hIcon = hIconFE_Large;
    wc.hIconSm = hIconFE_Small;
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject (WHITE_BRUSH);
    wc.lpszMenuName = MAKEINTRESOURCE (IDR_MENU);
    wc.lpszClassName = szFEClass;

    // Register the Session Manager window class
    if (!RegisterClassEx (&wc))
    {
        MB (pszNoRegFEWndClass);
        return FALSE;
    } // End IF

    // Fill in Matrix Editor window class structure
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC) MEWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = GWLME_EXTRA;
    wc.hInstance = hInstance;
    wc.hIcon = hIconME_Large;
    wc.hIconSm = hIconME_Small;
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject (WHITE_BRUSH);
    wc.lpszMenuName = MAKEINTRESOURCE (IDR_MENU);
    wc.lpszClassName = szMEClass;

    // Register the Session Manager window class
    if (!RegisterClassEx (&wc))
    {
        MB (pszNoRegMEWndClass);
        return FALSE;
    } // End IF

    // Fill in Vector Editor window class structure
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC) VEWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = GWLVE_EXTRA;
    wc.hInstance = hInstance;
    wc.hIcon = hIconVE_Large;
    wc.hIconSm = hIconVE_Small;
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject (WHITE_BRUSH);
    wc.lpszMenuName = MAKEINTRESOURCE (IDR_MENU);
    wc.lpszClassName = szVEClass;

    // Register the Session Manager window class
    if (!RegisterClassEx (&wc))
    {
        MB (pszNoRegVEWndClass);
        return FALSE;
    } // End IF
#ifdef DEBUG
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
#endif
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
    InitChooseFont (&cfFE, &lfFE, DEF_FEPTSIZE);
    InitChooseFont (&cfME, &lfME, DEF_MEPTSIZE);
    InitChooseFont (&cfVE, &lfVE, DEF_VEPTSIZE);

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
