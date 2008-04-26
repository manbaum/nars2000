//***************************************************************************
//  NARS2000 -- Tab Control Routines
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

#define WINVER       0x0501 // Needed for WINUSER.H definitions
#define _WIN32_WINNT 0x0501 // ...

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "resource.h"
#include "externs.h"
#include "pertab.h"
#include "threads.h"
#include "colors.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


// The width & height of an image as drawn on the tab
#define IMAGE_WIDTH     (IMAGE_CX + 3)
#define IMAGE_HEIGHT    (IMAGE_CY + 3)

// The left and right margins for a tab
#define TABMARGIN_LEFT      8
#define TABMARGIN_RIGHT     8

CNT_THREAD cntThread;           // Temporary storage for CreateNewTabInThread

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
//  $EnumCallbackShowHide
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

    return TRUE;                        // Continue enumerating
} // End EnumCallbackShowHide


//***************************************************************************
//  $ShowHideChildWindows
//
//  Show or hide the given window and all its children
//***************************************************************************

void ShowHideChildWindows
    (HWND hWndMC,       // Window handle to MDI Client
     BOOL bShow)        // TRUE iff showing the window, FALSE if hiding it

{
    if (!hWndMC)
        return;

    if (bShow)
    {
        HWND hWndAct;

        // Put the MDI Client window at the top (SHOW) or bottom (HIDE)
        //   of the Z-order
        SetWindowPos (hWndMC,
                      HWND_TOP,
                      0, 0, 0, 0,
                      SWP_NOMOVE
                    | SWP_NOSIZE);
        // Show/hide the MDI Client window
        ShowWindow (hWndMC, SW_SHOWNORMAL);

        // Loop through the child windows
        EnumChildWindows (hWndMC,
                          EnumCallbackShowHide,
                          SW_SHOWNORMAL);
        // Set the focus to the active window
        hWndAct = (HWND) SendMessageW (hWndMC, WM_MDIGETACTIVE, 0, 0);
        if (hWndAct)
            SetFocus (hWndAct);
        else
            DbgStop ();
    } else
    {
        // Loop through the child windows
        EnumChildWindows (hWndMC,
                          EnumCallbackShowHide,
                          SW_HIDE);
        // Put the MDI Client window at the top (SHOW) or bottom (HIDE)
        //   of the Z-order
        SetWindowPos (hWndMC,
                      HWND_BOTTOM,
                      0, 0, 0, 0,
                      SWP_NOMOVE
                    | SWP_NOSIZE);

        // Show/hide the MDI Client window
        ShowWindow (hWndMC, SW_HIDE);
    } // End IF/ELSE

////// Loop through the child windows
////EnumChildWindows (hWndMC,
////                  EnumCallbackShowHide,
////                  bShow ? SW_SHOWNORMAL : SW_HIDE);
////// Put the MDI Client window at the top (SHOW) or bottom (HIDE)
//////   of the Z-order
////SetWindowPos (hWndMC,
////              bShow ? HWND_TOP : HWND_BOTTOM,
////              0, 0, 0, 0,
////              SWP_NOMOVE
////            | SWP_NOSIZE);
////// Show/hide the MDI Client window
////ShowWindow (hWndMC, bShow ? SW_SHOWNORMAL : SW_HIDE);
} // End ShowHideChildWindows


//***************************************************************************
//  $CreateNewTab
//
//  Create a new tab
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CreateNewTab"
#else
#define APPEND_NAME
#endif

BOOL CreateNewTab
    (HWND    hWndParent,        // Window handle of the parent
     LPWCHAR lpwsz,             // Drive, Path, Filename, Ext of the workspace
     int     iTab)              // Insert the new tab to the left of this one

{
    DWORD   dwThreadId;         // Thread ID
    HANDLE  hThread;            // Handle to the thread
    HGLOBAL hGlbDPFE = NULL;    // Workspace DPFE global memory handle
    LPWCHAR lpwszDPFE;          // Ptr to workspace DPFE global memory
    UINT    uLen;               // Length of workspace DPFE

    // Save the incoming Workspace DPFE in a global memory object
    //   and pass that handle to the Session Manager
    uLen = lstrlenW (lpwsz);

    // Allocate space for the workspace name ("+ 1" for trailing zero)
    // The storage for this handle is freed in <CreateNewTabInThread>.
    hGlbDPFE = MyGlobalAlloc (GHND, (uLen + 1) * sizeof (lpwsz[0]));
    if (!hGlbDPFE)
        return FALSE;

    // Lock the memory to get a ptr to it
    lpwszDPFE = MyGlobalLock (hGlbDPFE);

    // Copy the workspace name to global memory
    CopyMemory (lpwszDPFE, lpwsz, uLen * sizeof (lpwsz[0]));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbDPFE); lpwszDPFE = NULL;

    // Save args in struc to pass to thread func
    cntThread.hWndParent = hWndParent;
    cntThread.hGlbDPFE   = hGlbDPFE;
    cntThread.iTab       = iTab;

#ifdef DEBUG
    // Can't call Debugger as it hasn't been created as yet.
////dprintfW (L"--Starting thread in <CreateNewTab>.");
#endif

    // Create a new thread
    hThread = CreateThread (NULL,                   // No security attrs
                            0,                      // Use default stack size
                           &CreateNewTabInThread,   // Starting routine
                           &cntThread,              // Param to thread func
                            CREATE_SUSPENDED,       // Creation flag
                           &dwThreadId);            // Returns thread id
    // Save the thread handle
    cntThread.hThread = hThread;

    if (hThread)
        ResumeThread (hThread);

    return (hThread NE NULL);
} // End CreateNewTab
#undef  APPEND_NAME


//***************************************************************************
//  $CreateNewTabInThread
//
//  Create a new tab within a thread
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CreateNewTabInThread"
#else
#define APPEND_NAME
#endif

BOOL WINAPI CreateNewTabInThread
    (LPCNT_THREAD lpcntThread)

{
    int          iCurTab = -1;      // Index of the current tab
    TC_ITEMW     tcItem = {0};      // TabCtrl item struc
    HGLOBAL      hGlbPTD,           // PerTabData global memory handle
                 hGlbDPFE = NULL;   // Workspace DPFE global memory handle
    LPPERTABDATA lpMemPTD = NULL;   // Ptr to PerTabData global memory
    BOOL         bRet = FALSE;      // TR$UE iff the result is valid
    RECT         rc;                // Rectangle for setting size of window
    int          rcLeft, rcRight, rcBottom;
    CLIENTCREATESTRUCT ccs;         // For MDI Client window
    SM_CREATESTRUCTW csSM;          // For Session Manager window
    HANDLE       hThread;           // Handle to this thread
    HWND         hWndMC,            // Window handle of MDI Client
                 hWndParent;        // Window handle of the parent
    int          iTab;              // Insert the new tab to the left of this one
    MSG          Msg;               // Message for GetMessage loop
    int          nThreads;
    WCHAR        wszTemp[32];       // Temporary storage

    // Store the thread type ('TC')
    TlsSetValue (dwTlsType, (LPVOID) 'TC');

    // Extract values from the arg struc
    hWndParent = lpcntThread->hWndParent;
    hGlbDPFE   = lpcntThread->hGlbDPFE;
    iTab       = lpcntThread->iTab;
    hThread    = lpcntThread->hThread;

    // Get the size and position of the parent window.
    GetClientRect (hWndParent, &rc);

    if (gCurTab NE -1)
    {
        // Get the per tab global memory handle
        hGlbPTD = GetPerTabHandle (gCurTab);

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Hide the child windows of the outgoing tab
        ShowHideChildWindows (lpMemPTD->hWndMC, FALSE);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF

    // Allocate per tab data
    hGlbPTD = MyGlobalAlloc (GHND, sizeof (PERTABDATA));
    if (!hGlbPTD)
        goto ERROR_EXIT;    // Stop the whole process

    // Save the thread's PerTabData global memory handle
    TlsSetValue (dwTlsPerTabData, (LPVOID) hGlbPTD);

    // Initialize the item struc
    tcItem.mask       = TCIF_IMAGE | TCIF_PARAM;
////tcItem.pszText    =
////tcItem.cchTextMax =
    tcItem.iImage     = 0;
    tcItem.lParam     = (LPARAM) hGlbPTD;

    // Insert a new tab
    // The new tab is inserted to the left of the index value (iTab)
    iCurTab = SendMessageW (hWndTC, TCM_INSERTITEMW, iTab, (LPARAM) &tcItem);
    if (iCurTab EQ -1)
    {
        MB (pszNoInsertTCTab);

        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Save the current and previous tab index
    lpMemPTD->CurTabIndex = iCurTab;
    lpMemPTD->PrvTabIndex = iTab - 1;

    // Save the next available color index
    lpMemPTD->crIndex = GetNextTabColorIndex ();

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

    // Fill in the CLIENTCREATESTRUCT for the MDI Client window
    ccs.hWindowMenu = GetSubMenu (GetMenu (hWndParent), IDMPOS_SM_WINDOW);
    ccs.idFirstChild = IDM_CHILDWINDOW;

    // Create the MDI client window
    lpMemPTD->hWndMC =
      CreateWindowExW (0,                   // Extended styles
                       LMCWNDCLASS,         // Class name
                       wszMCTitle,          // Window title (for debugging purposes only)
                       0
//////               | MDIS_ALLCHILDSTYLES
                     | WS_CHILD
                     | WS_BORDER
                     | WS_CLIPCHILDREN
                       ,                    // Styles
                       rc.left,             // X-coordinate
                       rc.top,              // Y-...
                       rc.right  - rc.left, // Width
                       rc.bottom - rc.top,  // Height
                       hWndParent,          // Parent
                       (HMENU) IDWC_TC_MC,  // ID
                       _hInstance,          // hInstance
                      &ccs);                // lParam
    if (lpMemPTD->hWndMC EQ NULL)
    {
        MB (pszNoCreateMCWnd);

        goto ERROR_EXIT;
    } // End IF

////// Save the PTD handle with the window
////SetPropW (lpMemPTD->hWndMC, L"PTD", hGlbPTD);
////
    // Show and paint the window
    ShowWindow (lpMemPTD->hWndMC, SW_SHOWNORMAL);
    UpdateWindow (lpMemPTD->hWndMC);

#ifdef DEBUG
    // Create the Debugger window first
    //   so it can be used by subsequent windows
    CreateDebuggerWindow (hGlbPTD);
#endif

    // Fill in the SM WM_CREATE data struct
    csSM.hGlbDPFE = hGlbDPFE;

    // Create the Session Manager window
    lpMemPTD->hWndSM =
      CreateMDIWindowW (LSMWNDCLASS,        // Class name
                        wszSMTitle,         // Window title
                        0
                      | WS_CLIPCHILDREN,    // Styles
                        CW_USEDEFAULT,      // X-pos
                        CW_USEDEFAULT,      // Y-pos
                        CW_USEDEFAULT,      // Height
                        CW_USEDEFAULT,      // Width
                        lpMemPTD->hWndMC,   // Parent
                        _hInstance,         // Instance
              (LPARAM) &csSM);              // Extra data
    // If there's an error, don't display a message
    //   because that is a normal occurrence if we
    //   fail )LOAD.
    if (lpMemPTD->hWndSM EQ NULL)
        goto ERROR_EXIT;

    // Get # current threads under the SM
    nThreads = 1 + (int) GetPropW (lpMemPTD->hWndSM, L"NTHREADS");

    // Format the next property name
    wsprintfW (wszTemp, L"Thread%d", hThread);

    // Set the property to the current thread handle
    SetPropW (hWndParent, wszTemp, hThread);

    // Set the handle of the active window
    lpMemPTD->hWndActive = lpMemPTD->hWndSM;

    // Save the handle
    SetWindowLongW (lpMemPTD->hWndSM, GWLSF_PERTAB, (long) hGlbPTD);

    // Show the child windows of the incoming tab
    ShowHideChildWindows (lpMemPTD->hWndMC, TRUE);

    // Activate this tab
    TabCtrl_SetCurSel (hWndTC, iCurTab);

    // Draw the tab with the text normal
    InvalidateRect (hWndTC, NULL, FALSE);

    // Save hWndMC for use inside message loop
    //   so we can unlock the per-tab data memory
    hWndMC = lpMemPTD->hWndMC;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Free the storage for the workspace DPFE global memory
    if (hGlbDPFE)
    {
        MyGlobalFree (hGlbDPFE); hGlbDPFE = NULL;
    } // End IF

    __try
    {
        // Main message loop
        while (GetMessage (&Msg, NULL, 0, 0))
        {
            // Handle MDI messages and accelerators
            if (!TranslateMDISysAccel (hWndMC, &Msg)
             && ((!hAccel) || !TranslateAccelerator (hWndMF, hAccel, &Msg)))
            {
                TranslateMessage (&Msg);
                DispatchMessage  (&Msg);
            } // End IF
        } // End WHILE
    } __except (CheckException (GetExceptionInformation (), "CreateNewTabInThread"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except
    // GetMessage returned FALSE for a Quit message

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    if (hGlbPTD && lpMemPTD)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF

    // If there's a current tab index, delete it
    if (iCurTab NE -1)
        TabCtrl_DeleteItem (hWndTC, iCurTab);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    if (lpMemPTD->hWndSM)
        // Unhook the LclEditCtrlWndProc
        SetWindowLongW ((HWND) GetWindowLongW (lpMemPTD->hWndSM, GWLSF_HWNDEC),
                        GWL_WNDPROC,
                        (long) lpMemPTD->lpfnOldEditCtrlWndProc);
    // Destroy any windows we might have created
    if (lpMemPTD && lpMemPTD->hWndMC)
    {
        if (lpMemPTD->hWndDB)
        {
            SendMessageW (lpMemPTD->hWndMC, WM_MDIDESTROY, (WPARAM) (lpMemPTD->hWndDB), 0); lpMemPTD->hWndDB = NULL;
        } // End IF

        if (lpMemPTD->hWndSM)
        {
            SendMessageW (lpMemPTD->hWndMC, WM_MDIDESTROY, (WPARAM) (lpMemPTD->hWndSM), 0); lpMemPTD->hWndSM = NULL;
        } // End IF

        DestroyWindow (lpMemPTD->hWndMC); lpMemPTD->hWndMC = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    if (hGlbDPFE)
    {
        // Free the storage for the workspace DPFE global memory
        MyGlobalFree (hGlbDPFE); hGlbDPFE = NULL;
    } // End IF

    if (hGlbPTD)
    {
        // We no longer need this storage
        MyGlobalFree (hGlbPTD); hGlbPTD = NULL;
    } // End IF

    // Get the per tab global memory handle
    hGlbPTD = GetPerTabHandle (TabCtrl_GetCurSel (hWndTC));
    if (hGlbPTD)
    {
        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Show the child windows of the incoming tab
        ShowHideChildWindows (lpMemPTD->hWndMC, TRUE);

        // Ensure the SM has the focus
        PostMessageW (lpMemPTD->hWndSM, MYWM_SETFOCUS, 0, 0);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF

    return bRet;
} // End CreateNewTabInThread
#undef  APPEND_NAME


//***************************************************************************
//  $GetPerTabHandle
//
//  Return the per tab global memory handle
//***************************************************************************

HGLOBAL GetPerTabHandle
    (int iTab)

{
    TC_ITEMW tcItem = {0};      // TabCtrl item struc

    // We want lParam only
    tcItem.mask       = TCIF_PARAM;
////tcItem.pszText    =
////tcItem.cchTextMax =
////tcItem.iImage     =
////tcItem.lParam     =

    // Return the lParam value (hGlbPTD)
    SendMessageW (hWndTC, TCM_GETITEMW, iTab, (LPARAM) &tcItem);

    // Return the handle
    return (HGLOBAL) tcItem.lParam;
} // End GetPerTabHandle


//***************************************************************************
//  $LclTabCtrlWndProc
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
    static BOOL    bCaptured = FALSE;
    POINT          ptScr;
    HMENU          hMenu;
    UINT           uCloseState,
                   uOverTabState;
    ////int            iTmpTab;
    HGLOBAL        hGlbPTD;         // PerTabData global memory handle
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory

////LCLODSAPI ("TC: ", hWnd, message, wParam, lParam);
    // Split cases
    switch (message)
    {
////    case WM_ERASEBKGND:
////        // In order to reduce screen flicker, we handle erase background
////        // in the WM_PAINT message for the individual tabs
////        return TRUE;            // We erased the background
#if 0
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
                SendMessageW (hWnd, WM_MOUSELEAVE, wParam, lParam);
                ReleaseCapture ();
                bCaptured = FALSE;

                break;
            } // End IF

            // If it's not the same tab, restore the state of the previous one
            if (iTmpTab NE gOverTab)
                SendMessageW (hWnd, WM_MOUSELEAVE, wParam, lParam);

            gOverTab = iTmpTab;

            // Draw the tab with the text highlighted
            SetTabTextState (gOverTab, TRUE);
            InvalidateRect (hWnd, NULL, FALSE);

            break;

        case WM_MOUSELEAVE:
            // If the tab index is invalid, ignore this message
            if (gOverTab EQ -1)
                break;

            // Draw the tab with the text normal
            SetTabTextState (gOverTab, FALSE);
            InvalidateRect (hWnd, NULL, FALSE);

            break;
#endif
        case WM_RBUTTONDOWN:                // fwKeys = wParam;        // Key flags
                                            // xPos = LOWORD(lParam);  // Horizontal position of cursor
                                            // yPos = HIWORD(lParam);  // Vertical position of cursor
            if (bCaptured)
            {
                SendMessageW (hWnd, WM_MOUSELEAVE, wParam, lParam);
                ReleaseCapture ();
                bCaptured = FALSE;
            } // End IF

            // Save the client coordinates
            tcHit.pt.x = LOWORD (lParam);
            tcHit.pt.y = HIWORD (lParam);

            // Ask the Tab Control if we're over a tab
            gOverTab = TabCtrl_HitTest (hWnd, &tcHit);

            // Get the mouse position in screen coordinates
            GetCursorPos (&ptScr);

            // Create a popup menu
            hMenu = CreatePopupMenu ();

            // If we're not over a tab, disallow any actions
            //   which require a valid tab indsex
            uOverTabState = (gOverTab EQ -1)
                          ? MF_GRAYED
                          : MF_ENABLED;

            // If we're not over a tab or this is the last open tab,
            //   disallow closing the WS
            uCloseState = (gOverTab EQ -1 || TabCtrl_GetItemCount (hWnd) EQ 1)
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
                CloseTab (gOverTab);
            break;

        case TCM_DELETEITEM:                    // itemID = (int) wParam;
                                                // 0 = lParam;
        {
            int  iNewTab;                       // Index of new tab (after deleting this one)
            HWND hWndEC;                        // Edit Control window handle

#define iTab    ((int) wParam)

            // If gOverTab is this tab or to the right of it,
            //    decrement gOverTab.
            if (gOverTab >= iTab)
                gOverTab--;

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

            // Get the outgoing per tab global memory handle
            hGlbPTD = GetPerTabHandle (iTab);

            // If it's invalid, we're closing out the last tab, so just quit
            if (hGlbPTD EQ NULL)
                break;

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Hide the child windows of the outgoing tab
            ShowHideChildWindows (lpMemPTD->hWndMC, FALSE);

            // Reset this tab's color index bit
            ResetTabColorIndex (lpMemPTD->crIndex);

#ifdef DEBUG
#define APPEND_NAME     L" -- LclTabCtrlWndProc/TCM_DELETEITEM"
#else
#define APPEND_NAME
#endif

            // Free global storage if the SymTab is valid
            if (lpMemPTD->lpSymTab)
            {
                FreeResultGlobalVar (lpMemPTD->lpSymQuadALX ->stData.stGlbData); lpMemPTD->lpSymQuadALX ->stData.stGlbData = NULL;
                FreeResultGlobalVar (lpMemPTD->lpSymQuadELX ->stData.stGlbData); lpMemPTD->lpSymQuadELX ->stData.stGlbData = NULL;
                FreeResultGlobalVar (lpMemPTD->lpSymQuadFC  ->stData.stGlbData); lpMemPTD->lpSymQuadFC  ->stData.stGlbData = NULL;
                FreeResultGlobalVar (lpMemPTD->lpSymQuadIC  ->stData.stGlbData); lpMemPTD->lpSymQuadIC  ->stData.stGlbData = NULL;
                FreeResultGlobalVar (lpMemPTD->lpSymQuadLX  ->stData.stGlbData); lpMemPTD->lpSymQuadLX  ->stData.stGlbData = NULL;
                FreeResultGlobalVar (lpMemPTD->lpSymQuadSA  ->stData.stGlbData); lpMemPTD->lpSymQuadSA  ->stData.stGlbData = NULL;
                FreeResultGlobalVar (lpMemPTD->lpSymQuadWSID->stData.stGlbData); lpMemPTD->lpSymQuadWSID->stData.stGlbData = NULL;
                if (lpMemPTD->cQuadPR)
                    FreeResultGlobalVar (lpMemPTD->lpSymQuadPR  ->stData.stGlbData); lpMemPTD->lpSymQuadPR  ->stData.stGlbData = NULL;
            } // End IF

#undef  APPEND_NAME

            // If the SM handle is valid, ...
            if (lpMemPTD->hWndSM)
            {
                // Get the Edit Ctrl window handle
                hWndEC = (HWND) GetWindowLongW (lpMemPTD->hWndSM, GWLSF_HWNDEC);

                // Unhook the LclEditCtrlWndProc
                SetWindowLongW (hWndEC,
                                GWL_WNDPROC,
                                (long) lpMemPTD->lpfnOldEditCtrlWndProc);
            } // End IF

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // The storage for hGlbPTD is freed in CreateTabInThread

            // Save as new tab index
            gCurTab = TabCtrl_GetCurSel (hWndTC);

            break;
#undef  iTab
        } // End TCM_DELETEITEM
    } // End SWITCH

    return CallWindowProcW (lpfnOldTabCtrlWndProc,
                            hWnd,
                            message,
                            wParam,
                            lParam); // Pass on down the line
} // End LclTabCtrlWndProc


//***************************************************************************
//  $ClickOnClose
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
    gOverTab = TabCtrl_HitTest (hWndTC, &tcHit);

    // If we're not over a tab, quit
    if (gOverTab EQ -1)
        return FALSE;

    // If there's only one tab, don't check the close button
    if (TabCtrl_GetItemCount (hWndTC) EQ 1)
        return FALSE;

    // Get the item's bounding rectangle
    TabCtrl_GetItemRect (hWndTC, gOverTab, &rcTab);

    // Remove the border from the rectangle
    AdjustTabRect (&rcTab, gOverTab, FALSE);

    // Transform the bounding rectangle into an image rectangle
    GetImageRect (&rcTab);

    // If the point is on the button, return so
    return PtInRect (&rcTab, tcHit.pt);
} // End ClickOnClose


//***************************************************************************
//  $CloseTab
//
//  Close a given tab
//***************************************************************************

BOOL CloseTab
    (int iTabIndex)             // Tab index

{
#ifdef DEBUG
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD = NULL; // Ptr to PerTabData global memory

    // Get the per tab global memory handle
    hGlbPTD = GetPerTabHandle (iTabIndex);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Tell the debugger window to close
    SendMessageW (lpMemPTD->hWndDB, WM_CLOSE, 0, 0);

    // We no longe need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
#endif
    // Close the tab
    return (TabCtrl_DeleteItem (hWndTC, iTabIndex) NE -1);
} // End CloseTab


//***************************************************************************
//  $SetTabTextState
//
//  Set a given tab's text state to highlight or normal
//***************************************************************************

void SetTabTextState
    (int  iCurTab,      // Index # of current tab
     BOOL bHighlight)   // TRUE iff to be drawn as highlighted

{
    HGLOBAL      hGlbPTD;
    LPPERTABDATA lpMemPTD;

    // Get the per tab global memory handle
    hGlbPTD = GetPerTabHandle (iCurTab);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Save the text state
    lpMemPTD->bTabTextState = bHighlight;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End SetTabTextState


//***************************************************************************
//  $FillTabBackground
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
    COLORREF rgbStart = RGB (255, 255, 255);    // White at the Bottom
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
                  (nBands - i) + lpRect->top,
                  lpRect->right + 1,
                  (nBands - i) + lpRect->top + 1);
        crBand = RGB ((int) (((float) GetRValue (rgbStart)) - fRStep * i),
                      (int) (((float) GetGValue (rgbStart)) - fGStep * i),
                      (int) (((float) GetBValue (rgbStart)) - fBStep * i));
        hBrush = CreateSolidBrush (crBand);
        FillRect (hDC, &rcBand, hBrush);
        DeleteObject (hBrush); hBrush = NULL;
    } // End FOR
} // End FillTabBackground


//***************************************************************************
//  $GetNextTabColorIndex
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
                if (!(crIndices[i] & (BIT0 << j)))
                {
                    crIndices[i] |= BIT0 << j;  // Set the bit
                    return j + i * NBIB;        // Return its index
                } // End IF
    return -1;
} // End GetNextTabColorIndex


//***************************************************************************
//  $GetTabColorIndex
//
//  Get a given tab's color index
//***************************************************************************

int GetTabColorIndex
    (int iTab)

{
    HGLOBAL      hGlbPTD;
    LPPERTABDATA lpMemPTD;
    int          crIndex;

    // Get the per tab global memory handle
    hGlbPTD = GetPerTabHandle (iTab);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the color index
    crIndex = lpMemPTD->crIndex;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return crIndex;
} // End GetTabColorIndex


//***************************************************************************
//  $ResetTabColorIndex
//
//  Reset a given tab's color index bit
//***************************************************************************

void ResetTabColorIndex
    (int crIndex)

{
    // Reset this tab's color index bit
    crIndices[crIndex >> LOG2NBIB] &= ~(BIT0 << (crIndex % NBIB));
} // End ResetTabColorIndex


//***************************************************************************
//  $AdjustTabRect
//
//  Adjust a tab's bounding rectangle to avoid writing
//    over (or under) the border.
//***************************************************************************

void AdjustTabRect
    (LPRECT lpRect,         // Ptr to rectangle to adjust
     int    iTabIndex,      // Tab index
     BOOL   bForce)         // TRUE iff we're to force adjustment

{
    // If this is the current tab, adjust by one pixel
    if (bForce || iTabIndex EQ TabCtrl_GetCurSel (hWndTC))
    {
        lpRect->left   += 1;
        lpRect->top    += 1;
        lpRect->right  -= 1;
        lpRect->bottom -= 1;
    } // End IF
} // End AdjustTabRect


//***************************************************************************
//  $DrawTab
//
//  Draw the tab
//***************************************************************************

void DrawTab
    (HDC      hDC,                  // Handle to device context
     int      iCurTab,              // Index of current tab
     LPRECT   lpRect)               // Ptr to surrounding rectangle

{
    HGLOBAL      hGlbPTD;           // Handle of PerTabData
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPAPLCHAR    lpMemWSID;         // Ptr to []WSID global memory
    int          crIndex;           // Index into crTab for this tab
    COLORREF     crfg,              // Foreground color
                 crbk;              // Background color
#if 0
    HDC          hDCMem;            // Handle to memory device context
    HBITMAP      hBitmap,           // Handle to bitmap
                 hBitmapOld;        // Handle to old bitmap
#endif

    // Get the per tab global memory handle
    hGlbPTD = GetPerTabHandle (iCurTab);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the tab's color index
    crIndex = lpMemPTD->crIndex;

    // Get the tab's background color
    crbk = crTab[crIndex].bk;

    // Get the tab text's foreground color
    if (lpMemPTD->bTabTextState)
        crfg = crTab[crIndex].hl;
    else
        crfg = crTab[crIndex].fg;

    SetAttrs (hDC, hFontTC, crfg, crbk);

#if 0
    // Create a compatible DC and bitmap
    hDCMem = CreateCompatibleDC (hDC);
    hBitmap = CreateCompatibleBitmap (hDC,
                                      lpRect->right,
                                      lpRect->bottom);
    hBitmapOld = SelectObject (hDCMem, hBitmap);
    SetAttrs (hDCMem, hFontTC, crfg, crbk);
#endif

#define hDCMem  hDC

    // Remove the border from the rectangle
    AdjustTabRect (lpRect, iCurTab, FALSE);

    // Fill the background of the tab
    FillTabBackground (hDCMem, lpRect, crbk);

    // Draw transparently so the background shows through
    SetBkMode (hDCMem, TRANSPARENT);

    // Get a ptr to the ws name
    lpMemWSID = PointToWsName (iCurTab);

    // Account for the tab left margin
    lpRect->left  += TABMARGIN_LEFT;
    lpRect->right += TABMARGIN_LEFT;

    // Draw the text
    DrawTextW (hDCMem,
               lpMemWSID,
               lstrlenW (lpMemWSID),
               lpRect,
               0
             | DT_SINGLELINE
             | DT_VCENTER
             | DT_NOPREFIX
              );
    // Restore the original values
    lpRect->right -= TABMARGIN_LEFT;
    lpRect->left  -= TABMARGIN_LEFT;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // If there's only one tab, don't draw the close button
    if (TabCtrl_GetItemCount (hWndTC) NE 1)
    {
        RECT rcImage;

        // Copy the original rectangle
        rcImage = *lpRect;

        // Get the image rectangle
        GetImageRect (&rcImage);

        // Draw transparently
        ImageList_SetBkColor (hImageList, CLR_NONE);

        // Draw the image
        ImageList_Draw (hImageList,         // Handle to the image list
                        0,                  // Index of the image to draw
                        hDCMem,             // Handle of the DC
                        rcImage.left,       // X-coordinate
                        rcImage.top,        // Y-...
                        0                   // Styles
                      | ILD_TRANSPARENT
                        );
    } // End IF

#if 0
    // Copy the memory DC to the screen DC
    BitBlt (hDC,
            lpRect->left,
            lpRect->top,
            lpRect->right  - lpRect->left,
            lpRect->bottom - lpRect->top,
            hDCMem,
            lpRect->left,
            lpRect->top,
            SRCCOPY);
    // Restore the old resources
    SelectObject (hDCMem, hBitmapOld);

    // We no longer need these resources
    DeleteObject (hBitmap); hBitmap = NULL;
    DeleteDC (hDCMem); hDCMem = NULL;
#endif
} // End DrawTab


//***************************************************************************
//  $PointToWsName
//
//  Skip over the path name to the workspace name
//***************************************************************************

LPAPLCHAR PointToWsName
    (int iTabIndex)             // Tab index

{
    HGLOBAL      hGlbPTD,       // PerTabData global memory handle
                 hGlbWSID;      // []WSID global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPAPLCHAR    lpMemWSID,     // Ptr to []WSID global memory
                 lpwTemp;       // Temporary ptr

    // Get the PerTabData global memory handle
    hGlbPTD = GetPerTabHandle (iTabIndex); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If the []WSID STE has been setup, ...
    if (lpMemPTD->lpSymQuadWSID)
    {
        APLNELM aplNELMWSID;    // []WSID NELM
        APLRANK aplRankWSID;    // []WSID rank

        // Get the []WSID global memory handle
        hGlbWSID = ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData);

        // If the []WSID STE has been setup, ...
        if (hGlbWSID)
        {
            // Lock the memory to get a ptr to it
            lpMemWSID = MyGlobalLock (hGlbWSID);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemWSID)
            // Get the NELM and rank
            aplNELMWSID = lpHeader->NELM;
            aplRankWSID = lpHeader->Rank;
#undef  lpHeader

            // If []WSID is non-empty, ...
            if (aplNELMWSID)
            {
                LPAPLCHAR p, q;             // Temporary ptrs

                // Skip over the header to the data
                lpMemWSID = VarArrayBaseToData (lpMemWSID, aplRankWSID);

                // Skip over the path
                q = lpMemWSID;
                while (p = strchrW (q, '\\'))
                    q = p + 1;

                // Copy to temporary storage
                lstrcpynW (lpwszTemp, q, (lpMemWSID + aplNELMWSID + 1) - q);

                // Copy the ptr
                lpwTemp = lpwszTemp;
            } else
                // Point to the ws name
                lpwTemp = L"CLEAR WS";
        } else
            // Point to the ws name
            lpwTemp = L"CLEAR WS";
    } else
    {
        // Mark as invalid
        hGlbWSID = NULL;

        // Point to the ws name
        lpwTemp = L"CLEAR WS";
    } // End IF/ELSE

    if (hGlbWSID)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbWSID); lpMemWSID = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpwTemp;
} // End PointToWsName


//***************************************************************************
//  $GetImageRect
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
//  $NewTabName
//
//  Tell the tab ctrl about a new workspace name
//***************************************************************************

void NewTabName
    (void)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    int          iCurTab;       // Index of the current tab
    TC_ITEMW     tcItem = {0};  // TabCtrl item struc

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the tab index
    iCurTab = lpMemPTD->CurTabIndex;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Initialize the item struc
    tcItem.mask       = TCIF_TEXT;
    tcItem.pszText    = PointToWsName (iCurTab);
////tcItem.cchTextMax =
////tcItem.iImage     =
////tcItem.lParam     =

    // Tell the Tab Ctrl about the new name
    SendMessageW (hWndTC, TCM_SETITEMW, iCurTab, (LPARAM) &tcItem);
} // End NewTabName


//***************************************************************************
//  End of File: tabctrl.c
//***************************************************************************
