//***************************************************************************
//  NARS2000 -- Tab Control Routines
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "resource.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


// The width & height of an image as drawn on the tab
#define IMAGE_WIDTH     (IMAGE_CX + 3)
#define IMAGE_HEIGHT    (IMAGE_CY + 3)


// 1s mark the indices in use
UCHAR crIndices[32] = {0};      // This supports 256 (=32x8) open WSs

typedef struct tagTABCOLORS
{
    COLORREF bk,                        // Background
             fg,                        // Foreground
             hl;                        // Highlight
} TABCOLORS;

TABCOLORS crTab[] =
//                           Text              Text
//     Background         Foreground         Highlight
{
 {RGB (255,228,181), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Moccasin              1
 {RGB (220, 20, 60), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Crimson               2
 {RGB (255,  0,255), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Fuchsia               3
 {RGB (123,104,238), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Medium slate blue     4
 {RGB ( 30,144,255), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Dodger blue           5
 {RGB ( 50,205, 50), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Line green            6
 {RGB (255,215,  0), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Gold                  7
 {RGB (189,183,107), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Dark khaki            8
 {RGB (222,184,135), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Burly wood            9
 {RGB (193,208,165), RGB (  0,  0,  0), RGB (255,  0,  0)}, //                      10
 {RGB (244,164, 96), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Sandy brown          11
 {RGB (184,134, 11), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Dark goldenrod       12
 {RGB (  0,250,154), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Medium spring green  13
 {RGB (173,255, 47), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Green yellow         14
 {RGB (148,  0,211), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Dark violet          15
 {RGB (230,230,250), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Lavender             16
 {RGB (255,105,180), RGB (  0,  0,  0), RGB (255,  0,  0)}, // Hot pink             17
};

#define NUM_CRTABS  (sizeof (crTab) / sizeof (crTab[0]))


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
//  EnumCallbackShowHide
//
//  Enum child proc for ShowChildWindows
//***************************************************************************

BOOL CALLBACK EnumCallbackShowHide
    (HWND   hWnd,           // Handle to child window
     LPARAM lParam)         // Application-defined value

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
} // End EnumCallbackShowHide


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
                      EnumCallbackShowHide,
                      bShow ? SW_SHOW : SW_HIDE);
} // End ShowHideChildWindows


//***************************************************************************
//  CreateNewTab
//
//  Create a new tab
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CreateNewTab"
#else
#define APPEND_NAME
#endif

BOOL CreateNewTab
    (HWND  hWndParent,
     LPSTR lpszDPFE,        // Drive, Path, Filename, Ext of the workspace
     int   iTab)

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
    tcItem.iImage  = 0;
    tcItem.pszText = &lpszDPFE[iLabelText];
    tcItem.lParam  = (LPARAM) hGlbData;

    // Insert a new tab
    // The new tab is inserted to the left of the index value (iTab)
    iCurTab = TabCtrl_InsertItem (hWndTC, iTab, &tcItem);

    if (iCurTab EQ -1)
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

    // Save the next available color index
    lpMem->crIndex = GetNextTabColorIndex ();

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
                    0
////              | MDIS_ALLCHILDSTYLES,    // Styles
                  | WS_CHILD
                  | WS_BORDER
                  | WS_CLIPCHILDREN
                    ,                       // Styles
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

#ifdef DEBUG
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
#endif

    // Create the Session Manager window
    lpMem->hWndSM =
    CreateMDIWindow (szSMClass,             // Class name
                     szSMTitle,             // Window title
                     0
                   | WS_VSCROLL
                   | WS_HSCROLL
                     ,                      // Styles
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
////hWndSM = lpMem->hWndSM;     // Already set by the window's WM_NCCREATE action
////hWndDB = lpMem->hWndDB;     // ...

    // Set the handle of the active window
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
        DbgGlobalFree (hGlbData); hGlbData = NULL;
    } // End IF

    return bRet;
} // End CreateNewTab
#undef  APPEND_NAME


//***************************************************************************
//  GetPerTabHandle
//
//  Return the per tab global memory handle
//***************************************************************************

HGLOBAL GetPerTabHandle
    (int iTab)

{
    TC_ITEM tcItem;

    tcItem.mask = TCIF_PARAM;
    TabCtrl_GetItem (hWndTC, iTab, &tcItem);

    // Save the handle
    return (HGLOBAL) tcItem.lParam;
} // End GetPerTabHandle


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
    TC_HITTESTINFO  tcHit;
    static BOOL     bCaptured = FALSE;
    POINT           ptScr;
    HMENU           hMenu;
    UINT            uCloseState,
                    uOverTabState;
    int             iTmpTab;
    HGLOBAL         hGlbData;
    LPPERTABDATA    lpMem;

    // Split cases
    switch (message)
    {
        case WM_MOUSEMOVE:          // fwKeys = wParam;        // Key flags
                                    // xPos = LOWORD(lParam);  // Horizontal position of cursor in CA
                                    // yPos = HIWORD(lParam);  // Vertical position of cursor  in CA
            if (!bCaptured)
            {
                SetCapture (hWnd);
                bCaptured = TRUE;
            } // End IF

            // Save the client coordinates
            tcHit.pt.x = LOWORD (lParam);
            tcHit.pt.y = HIWORD (lParam);

            // Ask the Tab Control if we're over a tab
            iTmpTab = TabCtrl_HitTest (hWnd, &tcHit);

            // Ensure we're over a tab
            if (iTmpTab EQ -1)
            {
                SendMessage (hWnd, WM_MOUSELEAVE, wParam, lParam);
                ReleaseCapture ();
                bCaptured = FALSE;

                break;
            } // End IF

            // If it's not the same tab, restore the state of the previous one
            if (iTmpTab NE iOverTab)
                SendMessage (hWnd, WM_MOUSELEAVE, wParam, lParam);

            iOverTab = iTmpTab;

            // Draw the tab with the text highlighted
            SetTabText (iOverTab, TRUE);
            InvalidateRect (hWnd, NULL, FALSE);

            break;

        case WM_MOUSELEAVE:
            // If the tab index is invalid, ignore this message
            if (iOverTab EQ -1)
                break;

            // Draw the tab with the text normal
            SetTabText (iOverTab, FALSE);
            InvalidateRect (hWnd, NULL, FALSE);

            break;

        case WM_RBUTTONDOWN:                // fwKeys = wParam;        // Key flags
                                            // xPos = LOWORD(lParam);  // Horizontal position of cursor
                                            // yPos = HIWORD(lParam);  // Vertical position of cursor
            if (bCaptured)
            {
                SendMessage (hWnd, WM_MOUSELEAVE, wParam, lParam);
                ReleaseCapture ();
                bCaptured = FALSE;
            } // End IF

            // Save the client coordinates
            tcHit.pt.x = LOWORD (lParam);
            tcHit.pt.y = HIWORD (lParam);

            // Ask the Tab Control if we're over a tab
            iOverTab = TabCtrl_HitTest (hWnd, &tcHit);

            // Get the mouse position in screen coordinates
            GetCursorPos (&ptScr);

            // Create a popup menu
            hMenu = CreatePopupMenu ();

            // If we're not over a tab, disallow any actions
            //   which require a valid tab indsex
            uOverTabState = (iOverTab EQ -1)
                          ? MF_GRAYED
                          : MF_ENABLED;

            // If we're not over a tab or this is the last open tab,
            //   disallow closing the WS
            uCloseState = (iOverTab EQ -1 || TabCtrl_GetItemCount (hWnd) EQ 1)
                        ? MF_GRAYED
                        : MF_ENABLED;

            AppendMenu (hMenu,                  // Handle
                        uOverTabState
                      | MF_STRING,              // Flags
                        IDM_NEW_WS,
                        "&New WS");
            AppendMenu (hMenu,                  // Handle
                        MF_ENABLED
                      | MF_STRING,              // Flags
                        IDM_DUP_WS,
                        "&Duplicate WS");
            AppendMenu (hMenu,                  // Handle
                        uCloseState
                      | MF_STRING,              // Flags
                        IDM_SAVECLOSE_WS,
                        "&Save and Close WS");
            AppendMenu (hMenu,                  // Handle
                        uCloseState
                      | MF_STRING,              // Flags
                        IDM_CLOSE_WS,
                        "&Close WS");

            TrackPopupMenu (hMenu,              // Handle
                            TPM_CENTERALIGN
                          | TPM_LEFTBUTTON
                          | TPM_RIGHTBUTTON,    // Flags
                            ptScr.x,            // x-position
                            ptScr.y,            // y-position
                            0,                  // Reserved (must be zero)
                            hWndMF,             // Handle of owner window
                            NULL);              // Dismissal area outside rectangle (none)
            // Free the menu resources
            DestroyMenu (hMenu);

            break;

        case WM_LBUTTONUP:                  // fwKeys = wParam;        // Key flags
                                            // xPos = LOWORD(lParam);  // Horizontal position of cursor
                                            // yPos = HIWORD(lParam);  // vertical position of cursor
            // If the user clicked on the icon, close the tab
            if (ClickOnClose ())
                CloseTab (iOverTab);
            break;

        case TCM_DELETEITEM:                    // itemID = (int) wParam;
                                                // 0 = lParam;
        {
            int  iNewTab;
            HWND hWndClose;

#define iTab    ((int) wParam)

            // If iOverTab is this tab or to the right ot it,
            //    decrement iOverTab.
            if (iOverTab >= iTab)
                iOverTab--;

            // If the current tab is the one being deleted,
            //   the index of the new tab is one to the right
            //   if there is a tab there, or one to the left.
            // Note that within this message, we haven't deleted
            //   the tab as yet, so the indexing doesn't need
            //   to take that into account.
            iNewTab = TabCtrl_GetCurSel (hWnd);
            if (iNewTab EQ iTab)
            {
                // Izit the rightmost tab?
                if (iNewTab EQ (TabCtrl_GetItemCount (hWnd) - 1))
                    iNewTab--;
                else
                    iNewTab++;
            } // End IF

            // Save data from the current WS into global memory
            SaveWsData (hGlbCurTab);

            // Get the per tab global memory handle
            hGlbData = GetPerTabHandle (iTab);

            // Restore data into the current WS from global memory
            RestWsData (hGlbData);

            // Hide the child windows of the outgoing tab
            ShowHideChildWindows (hWndMC, FALSE);

            // Lock the memory to get a ptr to it
            lpMem = MyGlobalLock (hGlbData);

            // Reset this tab's color index bit
            ResetTabColorIndex (lpMem->crIndex);

#ifdef DEBUG
#define APPEND_NAME     L" -- LclTabCtrlWndProc/TCM_DELETEITEM"
#else
#define APPEND_NAME
#endif

            // Free global storage
////////////DbgGlobalFree (hGlbHist    ); hGlbHist     = NULL;  // Freed in SMWndProc/WM_DESTROY
            FreeResultGlobalVar (hGlbQuadALX ); hGlbQuadALX  = NULL;
            FreeResultGlobalVar (hGlbQuadELX ); hGlbQuadELX  = NULL;
            FreeResultGlobalVar (hGlbQuadLX  ); hGlbQuadLX   = NULL;
            FreeResultGlobalVar (hGlbQuadSA  ); hGlbQuadSA   = NULL;
            FreeResultGlobalVar (hGlbQuadWSID); hGlbQuadWSID = NULL;
            FreeResultGlobalVar (hGlbQuadPR  ); hGlbQuadPR   = NULL;

#undef  APPEND_NAME

            // We no longer need this ptr
            MyGlobalUnlock (hGlbData); lpMem = NULL;

            // We no longer need this storage
            MyGlobalFree (hGlbData); hGlbData = NULL;

            hWndClose = hWndMC;

            // In order to reduce screen flicker, we
            //   postpone removing the tab until the new
            //   tab has been displayed.

            // Close the matching MDI client window
            DestroyWindow (hWndClose);

            DestroyCaret ();    // 'cause we just lost the focus

            // Get the per tab global memory handle
            hGlbCurTab = GetPerTabHandle (iNewTab);

            // Restore data into the current WS from global memory
            RestWsData (hGlbCurTab);

            // Show the child windows of the incoming tab
            ShowHideChildWindows (hWndMC, TRUE);

            break;
#undef  iTab
        } // End TCM_DELETETEM
    } // End SWITCH

    return CallWindowProcW (lpfnOldTabCtrlWndProc,
                            hWnd,
                            message,
                            wParam,
                            lParam); // Pass on down the line
} // End LclTabCtrlWndProc


//***************************************************************************
//  ClickOnClose
//
//  Did the user click on a close button?
//***************************************************************************

BOOL ClickOnClose
    (void)

{
    TC_HITTESTINFO  tcHit;
    RECT            rcTab;

    // Get the mouse position in screen coordinates
    GetCursorPos (&tcHit.pt);

    // Convert from screen to window coordinates
    ScreenToClient (hWndTC, &tcHit.pt);

    // Ask the Tab Control if we're over a tab
    iOverTab = TabCtrl_HitTest (hWndTC, &tcHit);

    // If we're not over a tab, quit
    if (iOverTab EQ -1)
        return FALSE;

    // If there's only one tab, don't check the close button
    if (TabCtrl_GetItemCount (hWndTC) EQ 1)
        return FALSE;

    // Get the item's bounding rectangle
    TabCtrl_GetItemRect (hWndTC, iOverTab, &rcTab);

    // Remove the border from the rectangle
    AdjustTabRect (&rcTab, iOverTab);

    // Transform the bounding rectangle into an image rectangle
    GetImageRect (&rcTab);

    // If the point is on the button, return so
    return PtInRect (&rcTab, tcHit.pt);
} // End ClickOnClose


//***************************************************************************
//  CloseTab
//
//  Close a tab
//***************************************************************************

BOOL CloseTab
    (int iTab)

{
    // Close the tab
    return (TabCtrl_DeleteItem (hWndTC, iTab) NE -1);
} // End CloseTab


//***************************************************************************
//  SetTabText
//
//  Set a given tab's text to highlight or normal
//***************************************************************************

void SetTabText
    (int  iCurTab,
     BOOL bHighlight)

{
    HGLOBAL      hGlbData;
    LPPERTABDATA lpMem;

    // Get the per tab global memory handle
    hGlbData = GetPerTabHandle (iOverTab);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Save the text state
    lpMem->bTabTextState = bHighlight;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;
} // End SetTabText


//***************************************************************************
//  FillTabBackground
//
//  Fill in a tab's background
//***************************************************************************

void FillTabBackground
    (HDC      hDC,
     LPRECT   lpRect,
     COLORREF crbk)

{
    HBRUSH   hBrush;
    int      i, nBands;
    RECT     rcBand;
    COLORREF crBand;
    COLORREF rgbStart = RGB (255, 255, 255);    // White at the top
    float    fRStep, fGStep, fBStep;

    // Calculate the # bands
    nBands = lpRect->bottom - lpRect->top;

    fRStep = ((float) (GetRValue (rgbStart) - GetRValue (crbk))) / (float) nBands;
    fGStep = ((float) (GetGValue (rgbStart) - GetGValue (crbk))) / (float) nBands;
    fBStep = ((float) (GetBValue (rgbStart) - GetBValue (crbk))) / (float) nBands;

    // Loop through all bands
    for (i = 0; i < nBands; i++)
    {
        // Fill the rectangle with one band
        SetRect (&rcBand,
                 lpRect->left,
                 i + lpRect->top,
                 lpRect->right + 1,
                 i + lpRect->top + 1);
        crBand = RGB ((int) (((float) GetRValue (rgbStart)) - fRStep * i),
                      (int) (((float) GetGValue (rgbStart)) - fGStep * i),
                      (int) (((float) GetBValue (rgbStart)) - fBStep * i));
        hBrush = CreateSolidBrush (crBand);
        FillRect (hDC, &rcBand, hBrush);
        DeleteObject (hBrush); hBrush = NULL;
    } // End FOR
} // End FillTabBackground


//***************************************************************************
//  GetNextTabColorIndex
//
//  Get the next available tab color index.
//***************************************************************************

int GetNextTabColorIndex
    (void)

{
    int i, j;

    // Search for the next available color index
    for (i = 0; i < sizeof (crIndices); i++)
        // If not all in use
        if (crIndices[i] NE 0xFF)
            // Search for the available bit within this byte
            for (j = 0; j < NBIB; j++)
                // If it's this bit, ...
                if (!(crIndices[i] & (1 << j)))
                {
                    crIndices[i] |= 1 << j;     // Set the bit
                    return j + i * NBIB;        // Return its index
                } // End IF
    return -1;
} // End GetNextTabColorIndex


//***************************************************************************
//  GetTabColorIndex
//
//  Get a given tab's color index
//***************************************************************************

int GetTabColorIndex
    (int iOverTab)

{
    HGLOBAL      hGlbData;
    LPPERTABDATA lpMem;
    int          crIndex;

    // Get the per tab global memory handle
    hGlbData = GetPerTabHandle (iOverTab);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    crIndex = lpMem->crIndex;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    return crIndex;
} // End GetTabColorIndex


//***************************************************************************
//  ResetTabColorIndex
//
//  Reset a given tab's color index bit
//***************************************************************************

void ResetTabColorIndex
    (int crIndex)

{
    // Reset this tab's color index bit
    crIndices[crIndex >> LOG2NBIB] &= ~(1 << (crIndex % NBIB));
} // End ResetTabColorIndex


//***************************************************************************
//  AdjustTabRect
//
//  Adjust a tab's bounding rectangle to avoid writing
//    over (or under) the border.
//***************************************************************************

void AdjustTabRect
    (LPRECT lpRect,
     int    iTab)

{
    // If this is the current tab, adjust by one pixel
    if (iTab EQ TabCtrl_GetCurSel (hWndTC))
    {
        lpRect->left   += 1;
        lpRect->top    += 1;
        lpRect->right  -= 1;
        lpRect->bottom -= 1;
    } // End IF
} // End AdjustTabRect


//***************************************************************************
//  DrawTab
//
//  Draw the tab
//***************************************************************************

void DrawTab
    (HWND     hWnd,
     HDC      hDC,
     int      iCurTab,
     LPRECT   lpRect)

{
    HGLOBAL      hGlbData;
    LPPERTABDATA lpMem;
    int          crIndex;
    COLORREF     crfg,
                 crbk;

    // Get the per tab global memory handle
    hGlbData = GetPerTabHandle (iCurTab);

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

    // Get the tab's color index
    crIndex = lpMem->crIndex;

    // Get the tab's background color
    crbk = crTab[crIndex].bk;

    // Get the tab text's foreground color
    if (lpMem->bTabTextState)
        crfg = crTab[crIndex].hl;
    else
        crfg = crTab[crIndex].fg;

    hDC = MyGetDC (hWndTC);
    SetAttrs (hDC, hFontTC, crfg, crbk);

    // Remove the border from the rectangle
    AdjustTabRect (lpRect, iCurTab);

    // Fill the background of the tab
    FillTabBackground (hDC, lpRect, crbk);

    // Draw transparently so the background shows through
    SetBkMode  (hDC, TRANSPARENT);

    // Reduce the right side of the rectangle to make room for the image
    lpRect->right -= IMAGE_WIDTH;

    // Draw the text
    DrawText (hDC,
             &lpMem->DPFE[lpMem->iLabelText],
              lstrlen (&lpMem->DPFE[lpMem->iLabelText]),
              lpRect,
              0
            | DT_SINGLELINE
            | DT_CENTER
            | DT_VCENTER
             );
    // Restore the original value
    lpRect->right += IMAGE_WIDTH;

    // Get the image rectangle
    GetImageRect (lpRect);

    // Draw transparently
    ImageList_SetBkColor (hImageList, CLR_NONE);

    // If there's only one tab, don't draw the close button
    if (TabCtrl_GetItemCount (hWnd) NE 1)
        // Draw the image
        ImageList_Draw (hImageList,         // Handle to the image list
                        0,                  // Index of the image to draw
                        hDC,                // Handle of the DC
                        lpRect->left,       // X-coordinate
                        lpRect->top,        // Y-...
                        0
                      | ILD_TRANSPARENT
                        );                  // Styles
    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    // We no longer need this DC
    MyReleaseDC (hWndTC, hDC);
} // End DrawTab


//***************************************************************************
//  GetImageRect
//
//  Get the bounding rectangle for an image right-adjusted horizontally
//    and centered vertically inside the given rectangle.
//***************************************************************************

void GetImageRect
    (LPRECT lpRect)

{
    lpRect->left   = lpRect->right - IMAGE_WIDTH;
    lpRect->top    = lpRect->top
                   + (lpRect->bottom - lpRect->top - IMAGE_HEIGHT) / 2,
    lpRect->bottom = lpRect->top + IMAGE_HEIGHT;
} // End GetImageRect


//***************************************************************************
//  End of File: tabctrl.c
//***************************************************************************
