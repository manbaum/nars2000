//***************************************************************************
//  NARS2000 -- Function Editor
//***************************************************************************

#define STRICT
#include <windows.h>

#include "colors.h"
#include "main.h"
#include "termcode.h"
#include "resource.h"
#include "resdebug.h"
#include "externs.h"
#include "editctrl.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

// ToDo
/*
 * Syntax Coloring:
   * Global Name
   * Local Name
   * Label
   * Primitive Function
   * Quad Function
   * Quad Variable
   * Control Structure
   * Number
   * Character
   * Comment
   * Unmatched Grouping Symbols
 * Redo
 * Toolbar
 * Load
 * Save
 * Parse header
 * Line #s optional
 * Move Undo Code into EDITCTRL.C
 * Allow external editor
 *

 */

////WCHAR debugText[] =
////L"(Z1 Z2)" WS_UTF16_LEFTARROW L"[(L1 L2 L3)] (LO FOO RO) (R1 R2 R3 R4)\r\n"
////L"Z1" WS_UTF16_LEFTARROW WS_UTF16_DELTASTILE WS_UTF16_DELTASTILE L"R1"
////;

char szCloseMessage[] = "You have changed the body of this function;"
                        " save the changes?";

WNDPROC lpfnOldEditCtrlWndProc;         // Save area for old Edit Control procedure

COLORREF crLineNum = RGB (143,188,143),   // Darkseagreen
         crLineTxt = RGB ( 65,105,225);   // Royalblue

#define FCN_INDENT  6

#define WM_REDO     (WM_USER + 0)

#define DEF_UNDOBUF_INITSIZE     4*1024
#define DEF_UNDOBUF_MAXSIZE     64*1024


//***************************************************************************
//  CreateFcnWindow
//
//  Create a function window
//***************************************************************************

BOOL CreateFcnWindow
    (LPWCHAR lpwszLine)

{
    HWND hWnd;

    // ***FIXME*** -- What do we do with this handle??

    hWnd =
    CreateMDIWindowW (LFEWNDCLASS,          // Class name
                      wszFETitle,           // Window title
                      0,                    // Styles
                      CW_USEDEFAULT,        // X-pos
                      CW_USEDEFAULT,        // Y-pos
                      CW_USEDEFAULT,        // Height
                      CW_USEDEFAULT,        // Width
                      hWndMC,               // Parent
                      _hInstance,           // Instance
                      (LPARAM) &lpwszLine[1]);  // Extra data, skip over the initial UTF16_DEL
    if (hWnd EQ NULL)
    {
        MB (pszNoCreateFEWnd);
        return FALSE;
    } // End IF

    // Make the window visible; update its client area
    ShowWindow (hWnd, SW_SHOWNORMAL);
    UpdateWindow (hWnd);

    return TRUE;
} // End CreateFcnWindow


//***************************************************************************
//  EB_Create
//
//  Perform window-specific initialization
//***************************************************************************

void EB_Create
    (HWND hWnd)

{
} // End EB_Create


//***************************************************************************
//  EB_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void EB_Delete
    (HWND hWnd)

{
} // End EB_Delete


//***************************************************************************
//  FEWndProc
//
//  Message processing routine for the Function Editor window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FEWndProc"
#else
#define APPEND_NAME
#endif

LRESULT APIENTRY FEWndProc
    (HWND hWnd,         // Window handle
     UINT message,      // Type of message
     UINT wParam,       // Additional information
     LONG lParam)       // ...

{
    HWND      hWndEB;       // Handle of Edit box window
    int       iMaxLimit;    // Maximum # chars in edit control
    VKSTATE   vkState;      // Virtual key state (Shift, Alt, Ctrl)
    long      lvkState;     // Temporary var for vkState
    RECT      rcFmtEB;      // Formatting rectangle for the Edit Control
    LPUNDOBUF lpUndoBeg,    // Ptr to start of Undo Buffer
              lpUndoNxt;    // ...    next available slot in the Undo Buffer

////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };
////ODSAPI ("FE: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_CREATE:             // lpcs = (LPCREATESTRUCT) lParam; // Structure with creation data

#define lpMDIcs     ((LPMDICREATESTRUCT) (((LPCREATESTRUCT) lParam)->lpCreateParams))

            // Initialize variables
            cfFE.hwndOwner = hWnd;
            ZeroMemory (&vkState, sizeof (vkState));
            SetWindowLong (hWnd, GWLFE_LINECNT,  1);

            // Save in window extra bytes
            vkState.Ins = 1;        // Initially inserting ***FIXME*** Make it an option
            SetWindowLong (hWnd, GWLFE_VKSTATE, *(long *) &vkState);

            // At some point, we'll read in the undo buffer from
            //   the saved function and copy it to virtual memory
            //   also setting the _BEG, _NXT, and _LST ptrs.

            // _BEG is the (static) ptr to the beginning of the virtual memory.
            // _NXT is the (dynamic) ptr to the next available entry.
            //    Undo entries are between _NXT[-1] and _BEG, inclusive.
            // _LST is the (dynamic) ptr to the last available entry.
            //    Redo entries are between _NXT and _LST[-1], inclusive.

            // Allocate virtual memory for the Undo Buffer
            lpUndoBeg =
            VirtualAlloc (NULL,          // Any address
                          DEF_UNDOBUF_MAXSIZE * sizeof (UNDOBUF),
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // Commit the intial size
            VirtualAlloc (lpUndoBeg,
                          DEF_UNDOBUF_INITSIZE * sizeof (UNDOBUF),
                          MEM_COMMIT,
                          PAGE_READWRITE);
            // Save in window extra bytes
            SetWindowLong (hWnd, GWLFE_UNDO_BEG, (long) lpUndoBeg);
            SetWindowLong (hWnd, GWLFE_UNDO_NXT, (long) lpUndoBeg);
            SetWindowLong (hWnd, GWLFE_UNDO_LST, (long) lpUndoBeg);
////////////SetWindowLong (hWnd, GWLFE_GROUPIND, 0);        // Already zero

            // Start with an initial action of nothing
            AppendUndo (hWnd,                       // FE Window handle
                        undoNone,                   // Action
                        0,                          // Beginning char position
                        0,                          // Ending    ...
                        UNDO_NOGROUP,               // Group index
                        0);                         // Character
            // Save incremented starting ptr in window extra bytes
            SetWindowLong (hWnd, GWLFE_UNDO_BEG, (long) ++lpUndoBeg);

            // Create an edit box within which we can edit
            hWndEB =
            CreateWindowExW (0L,                    // Extended styles
                             LECWNDCLASS,           // Class name
                             L"",                   // Initial text
                             0
                           | WS_CHILD
                           | WS_VSCROLL
///////////////////////////| WS_VISIBLE
                           | ES_MULTILINE
                           | ES_WANTRETURN
                           | ES_NOHIDESEL           // ***TESTME***
                           | ES_AUTOHSCROLL
                           | ES_AUTOVSCROLL
                             ,                      // Styles
                             0,                     // X-position
                             0,                     // Y-...
                             CW_USEDEFAULT,         // Width
                             CW_USEDEFAULT,         // Height
                             hWnd,                  // Parent window
                             (HMENU) IDWC_FE_EB,    // ID
                             _hInstance,            // Instance
                             0);                    // lParam
            if (hWndEB EQ NULL)
            {
                MB (pszNoCreateFEEditCtrl);

                return -1;          // Stop the whole process
            } // End IF

            // Save in window extra bytes
            SetWindowLong (hWnd, GWLFE_HWNDEB, (long) hWndEB);

            // Subclass the edit control so we can handle some of its messages
            lpfnOldEditCtrlWndProc = (WNDPROC)
              SetWindowLongW (hWndEB,
                              GWL_WNDPROC,
                              (long) (WNDPROC) &LclEditCtrlWndProc);
            // Set the paint hook
            SendMessage (hWndEB, EM_SETPAINTHOOK, 0, (LPARAM) &LclECPaintHook);

            // Tell the edit box about its font
            SendMessageW (hWndEB, WM_SETFONT, (WPARAM) hFontFE, 0);

            // Set the initial text
            SendMessageW (hWndEB, WM_SETTEXT, 0, (LPARAM) lpMDIcs->lParam);

            // Paint the window
            ShowWindow (hWndEB, SW_SHOWNORMAL);
            UpdateWindow (hWndEB);

            // Use Post here as we need to wait for the EB window
            //   to be drawn.
            PostMessage (hWnd, MYWM_DRAWLINENUMS, 0, 0);

            break;
#undef  lpMDIcs

        case WM_SETFONT:
            // Get the handle to the edit control
            hWndEB = (HWND) GetWindowLong (hWnd, GWLFE_HWNDEB);

            // Pass it on the the edit control
            SendMessageW (hWndEB, message, wParam, lParam);

            break;

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            EB_Delete (hWnd);

            // Initialize window-specific resources
            EB_Create (hWnd);

            break;                  // Continue with next handler

#define fwSizeType  wParam
#define nWidth      (LOWORD (lParam))
#define nHeight     (HIWORD (lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            // Get the handle to the edit control
            hWndEB = (HWND) GetWindowLong (hWnd, GWLFE_HWNDEB);

            if (fwSizeType NE SIZE_MINIMIZED)
            {
                SetWindowPos (hWndEB,           // Window handle to position
                              0,                // SWP_NOZORDER
                              0,                // X-position
                              0,                // Y-...
                              nWidth,           // Width
                              nHeight,          // Height
                              SWP_NOZORDER      // Flags
                            | SWP_SHOWWINDOW
                             );
                // Get the formatting rectangle
                SendMessageW (hWndEB, EM_GETRECT, 0, (LPARAM) &rcFmtEB);

                // Move the rectangle over enough chars (FCN_INDENT) to provide room
                //   for line numbers
                rcFmtEB.left = FCN_INDENT * cxAveCharFE;

                // Tell the control about this change
                SendMessageW (hWndEB, EM_SETRECT, 0, (LPARAM) &rcFmtEB);
            } // End IF

            break;                  // Continue with next handler ***MUST***
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_SETFOCUS:
            // Get the handle to the edit control
            hWndEB = (HWND) GetWindowLong (hWnd, GWLFE_HWNDEB);

            // Pass on to the edit ctrl
            SetFocus (hWndEB);

            break;

        case MYWM_DRAWLINENUMS:
            // Get the handle to the edit control
            hWndEB = (HWND) GetWindowLong (hWnd, GWLFE_HWNDEB);

            // Draw the line #s
            DrawLineNumsFE (hWndEB);

            return FALSE;           // We handled the msg

        case WM_UNDO:
        case MYWM_REDO:
        case WM_COPY:
        case WM_CUT:
        case WM_PASTE:
        case MYWM_PASTE_APLWIN:
        case WM_CLEAR:
        case MYWM_SELECTALL:
            // Get the handle to the edit control
            hWndEB = (HWND) GetWindowLong (hWnd, GWLFE_HWNDEB);

            SendMessageW (hWndEB, message, wParam, lParam);

            return FALSE;           // We handled the msg

#define wNotifyCode     (HIWORD (wParam))
#define wID             (LOWORD (wParam))
#define hWndCtrl        ((HWND) lParam)
        case WM_COMMAND:            // wNotifyCode = HIWORD(wParam); // Notification code
                                    // wID = LOWORD(wParam);         // Item, control, or accelerator identifier
                                    // hwndCtl = (HWND) lParam;      // Handle of control
            // Get the handle to the edit control
            hWndEB = (HWND) GetWindowLong (hWnd, GWLFE_HWNDEB);

            // This message should be from the edit control
            Assert (wID      EQ IDWC_FE_EB);
            Assert (hWndCtrl EQ hWndEB);

            // Split cases based upon the notify code
            switch (wNotifyCode)
            {
                case EN_SETFOCUS:                   // idEditCtrl = (int) LOWORD(wParam); // identifier of edit control
                                                    // wNotifyCode = HIWORD(wParam);      // notification code
                                                    // hwndEditCtrl = (HWND) lParam;      // handle of edit control
                    // Get the current vkState
                    lvkState = GetWindowLong (hWnd, GWLFE_VKSTATE);
                    vkState = *(LPVKSTATE) &lvkState;

                    // Create a default sized system caret for display
                    DestroyCaret ();        // 'cause we're changing the cursor width
                    MyCreateCaret (hWndEB, &vkState, cyAveCharFE, NULL);

                    // Paint the window
                    UpdateWindow (hWndEB);

                    break;

                case EN_CHANGE:                     // idEditCtrl = (int) LOWORD(wParam); // identifier of edit control
                                                    // hwndEditCtrl = (HWND) lParam;      // handle of edit control
                    // Split cases based upon the last key
                    switch (GetWindowLong (hWnd, GWLFE_LASTKEY))
                    {
                        case VK_RETURN:
                        case VK_BACK:
                        case VK_DELETE:
                            // Draw the line #s
                            DrawLineNumsFE (hWndEB);

                            break;
                    } // End SWITCH

                    // The contents of the edit control have changed,
                    // set the changed flag
                    SetWindowLong (hWnd, GWLFE_CHANGED, TRUE);

                    break;

                case EN_MAXTEXT:    // idEditCtrl = (int) LOWORD(wParam); // Identifier of edit control
                                    // hwndEditCtrl = (HWND) lParam;      // Handle of edit control
                    // The edit control has exceed its maximum # chars
                    DbgBrk ();      // ***TESTME***

                    // The default maximum is 32K, so we increase it by that amount
                    iMaxLimit = SendMessageW (hWndEB, EM_GETLIMITTEXT, 0, 0);
                    SendMessageW (hWndEB, EM_SETLIMITTEXT, iMaxLimit + 32*1024, 0);

                    break;

////            case EN_SETFOCUS:   // 0x0100
////            case EN_KILLFOCUS:  // 0x0200
////            case EN_CHANGE:     // 0x0300
////            case EN_UPDATE:     // 0x0400
////            case EN_ERRSPACE:   // 0x0500
////            case EN_MAXTEXT:    // 0x0501
////            case EN_HSCROLL:    // 0x0601
////            case EN_VSCROLL:    // 0x0602
                    break;
            } // End IF

            break;
#undef  hWndCtrl
#undef  wID
#undef  wNotifyCode

        case WM_QUERYENDSESSION:
        case WM_CLOSE:
            // If it's OK to close the window, do so, else ignore
            if (!QueryCloseFE (hWnd))
                return FALSE;       // Do not close
            break;                  // Default action is to close

        case WM_DESTROY:
            // Free allocated storage

            // Get the ptr to the start of the Undo Buffer
            (long) lpUndoBeg = GetWindowLong (hWnd, GWLFE_UNDO_BEG);
            if (lpUndoBeg)
            {
                // Free the virtual storage, first backing up to the start
                VirtualFree (--lpUndoBeg, 0, MEM_RELEASE);
                lpUndoBeg = lpUndoNxt = NULL;
                SetWindowLong (hWnd, GWLFE_UNDO_BEG, (long) lpUndoBeg);
                SetWindowLong (hWnd, GWLFE_UNDO_NXT, (long) lpUndoNxt);
            } // End IF

            // Uninitialize window-specific resources
            EB_Delete (hWnd);

            break;
    } // End SWITCH

////ODSAPI ("FEZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProc (hWnd, message, wParam, lParam);
} // End FEWndProc
#undef  APPEND_NAME


//***************************************************************************
//  LclECPaintHook
//
//  Local Edit Control paint hook
//***************************************************************************

int LclECPaintHook
    (HDC     hDC,       // The Device Context
     int     x,         // The x-coordinate (Client Area)
     int     y,         // ... y- ...
     LPWCHAR lpwsz,     // Ptr to start of line
     UINT    uCol,      // Starting column in the line
     UINT    uLen)      // Length of text to display

{
    // Syntax Color the line

    // To do this, we use a FSA to parse the line from the start
    //   through the last char to display





    // Draw the line
    return (int) LOWORD (TabbedTextOutW (hDC,
                                         x, y,
                                         lpwsz + uCol,
                                         uLen,
                                         0,
                                         NULL,
                                         0));
} // End LclECPaintHook


//***************************************************************************
//  LclEditCtrlWndProc
//
//  Local window procedure for the Edit Control
//***************************************************************************

LRESULT WINAPI LclEditCtrlWndProc
    (HWND   hWnd,
     UINT   message,
     WPARAM wParam,
     LPARAM lParam)

{
#define TABSTOP     8

    VKSTATE   vkState;      // Virtual key state (Shift, Alt, Ctrl)
    long      lvkState;     // Temporary var for vkState
    HWND      hWndParent;   // Handle of parent (FE) window
    LRESULT   lResult;      // Result from calling original handler
    LPUNDOBUF lpUndoNxt,    // Ptr to next available slot in the Undo Buffer
              lpUndoBeg;    // ...    first          ...
    UINT      uCharPosBeg,  // Pos of the beginning char
              uCharPosEnd,  // ...        ending    ...
              uCharPos,     // ...    a character position
              uLinePos,     // Char position of start of lnie
              uLineNum,     // Line #
              uSpaces,      // # spaces to insert
              uGroupIndex;  // Group index
    WCHAR     wChar[TABSTOP + 1],
              uChar;
    BOOL      bSelection;   // TRUE iff there's a selection

    // Split cases
    switch (message)
    {
#define nVirtKey ((int) wParam)
        case WM_KEYDOWN:            // nVirtKey = (int) wParam;     // Virtual-key code
                                    // lKeyData = lParam;           // Key data
            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Save the key in the parent window's extra bytes
            SetWindowLong (hWndParent, GWLFE_LASTKEY, nVirtKey);

            // Process the virtual key
            switch (nVirtKey)
            {
#ifdef DEBUG
                case VK_F8:             // Display the Undo Buffer
                    DisplayUndo (hWnd); // Display the Undo Buffer

                    return FALSE;
#endif
                case VK_INSERT:         // Insert
                    // Get the current vkState
                    lvkState = GetWindowLong (hWndParent, GWLFE_VKSTATE);
                    vkState = *(LPVKSTATE) &lvkState;

                    vkState.Ins = !vkState.Ins;

                    // Save in window extra bytes
                    SetWindowLong (hWndParent, GWLFE_VKSTATE, *(long *) &vkState);

                    // Get the char position of the caret
                    uCharPos = GetCurCharPos (hWnd);

                    // Save Undo entry
                    AppendUndo (hWndParent,                 // FE Window handle
                                undoInsToggle,              // Action
                                uCharPos,                   // Beginning char position
                                uCharPos + 1,               // Ending    ...
                                UNDO_NOGROUP,               // Group index
                                0);                         // Character
                    // Create a default sized system caret for display
                    DestroyCaret ();        // 'cause we're changing the cursor width
                    MyCreateCaret (hWnd, &vkState, cyAveCharFE, NULL);

                    break;

                case VK_RETURN:
                {
                    RECT rcFmt,
                         rcPaint;

                    // Insert L"\r\n"

                    // Get the char position of the caret
                    uCharPos = GetCurCharPos (hWnd);

                    // Undo is to delete it
                    AppendUndo (hWndParent,                 // FE Window handle
                                undoDel,                    // Action
                                uCharPos,                   // Beginning char position
                                uCharPos + 2,               // Ending    ...
                                UNDO_NOGROUP,               // Group index
                                0);                         // Character
                    // Get the formatting rectangle
                    SendMessageW (hWnd, EM_GETRECT, 0, (LPARAM) &rcFmt);

                    // Get the client rectangle
                    GetClientRect (hWnd, &rcPaint);

                    // Remove the line #s column
                    rcPaint.left = rcFmt.left;

                    // Get the line # of this char
                    uLineNum = SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPos, 0);

                    // Less the # of the topmost visible line
                    uLineNum -= SendMessageW (hWnd, EM_GETFIRSTVISIBLELINE, 0, 0);

                    rcPaint.top = uLineNum * cyAveCharFE;
                    rcPaint.bottom = rcPaint.top + cyAveCharFE;

                    // Invalidate this line
                    InvalidateRect (hWnd, &rcPaint, FALSE);

                    break;
                } // End VK_RETURN

                case VK_BACK:
                    // Delete the preceding char or selection

                    // Undo is to insert the deleted char(s)

                    // Get the indices of the selected text
                    SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

                    // Note if there's a selection
                    bSelection = uCharPosBeg NE uCharPosEnd;

                    // If there's no selection, adjust the beginning ptr
                    //   to select the preceding char
                    if (!bSelection)
                    {
                        // Get the preceding char
                        uChar = GetCharValue (hWnd, uCharPosBeg - 1);

                        // If the preceding char is L'\n' (meaning, we're at the
                        //   beginning of the line), then we need to AppendUndo
                        //   the matching L'\r', too, as they go together.
                        uCharPosBeg -= 1 + (uChar EQ L'\n');

                        // Tell the Edit Control about the selection
                        SendMessageW (hWnd, EM_SETSEL, (WPARAM) uCharPosBeg, uCharPosEnd);
                    } // End IF

                    // Get the next group index, and save it back
                    uGroupIndex = 1 + GetWindowLong (hWndParent, GWLFE_UNDO_GRP);
                    SetWindowLong (hWndParent, GWLFE_UNDO_GRP, uGroupIndex);

                    // Append an Undo action to set the selection if there was one before
                    if (bSelection)
                        AppendUndo (hWndParent,                     // FE Window handle
                                    undoSel,                        // Action
                                    uCharPosBeg,                    // Beginning char position
                                    uCharPosEnd,                    // Ending    ...
                                    uGroupIndex,                    // Group index
                                    0);                             // Character
                    // Loop through the selected chars (if any)
                    for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                        AppendUndo (hWndParent,                     // FE Window handle
                                    undoIns,                        // Action
                                    uCharPosBeg,                    // Beginning char position
                                    0,                              // Ending    ...
                                    uGroupIndex,                    // Group index
                                    GetCharValue (hWnd, uCharPos)); // Character
                    break;

                case VK_TAB:
                    // Insert a tab -- convert into insert N spaces

                    // Get the indices of the selected text
                    SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

                    // Get the line # of this char
                    uLineNum = SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPosBeg, 0);

                    // Get the char position of the start of the current line
                    uLinePos = SendMessageW (hWnd, EM_LINEINDEX, uLineNum, 0);

                    // Get the # spaces to insert
                    uCharPos = uCharPosBeg - uLinePos;
                    uSpaces = (((uCharPos + 1) / TABSTOP) * TABSTOP + TABSTOP) - uCharPos;

                    for (uChar = 0; uChar < uSpaces; uChar++)
                        wChar[uChar] = L' ';

                    wChar[uChar] = L'\0';

                    // Insert/replace the char string
                    InsRepCharStr (hWnd, GWLFE_VKSTATE, wChar);

                    return FALSE;       // We handled the msg

                case VK_DELETE:
                    // Delete a char or a selection

                    // Undo is to insert the deleted char(s)

                    // Get the indices of the selected text
                    SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

                    // Note if there's a selection
                    bSelection = uCharPosBeg NE uCharPosEnd;

                    // If there's no selection, increment the ending
                    //   ptr to account for it
                    uCharPosEnd += !bSelection;

                    // Get the next group index, and save it back
                    uGroupIndex = 1 + GetWindowLong (hWndParent, GWLFE_UNDO_GRP);
                    SetWindowLong (hWndParent, GWLFE_UNDO_GRP, uGroupIndex);

                    // Append an Undo action to set the selection if there was one before
                    if (bSelection)
                        AppendUndo (hWndParent,                     // FE Window handle
                                    undoSel,                        // Action
                                    uCharPosBeg,                    // Beginning char position
                                    uCharPosEnd,                    // Ending    ...
                                    uGroupIndex,                    // Group index
                                    0);                             // Character
                    // Loop through the selected chars (if any)
                    for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                        AppendUndo (hWndParent,                     // FE Window handle
                                    undoIns,                        // Action
                                    uCharPosBeg,                    // Beginning char position
                                    0,                              // Ending    ...
                                    uGroupIndex,                    // Group index
                                    GetCharValue (hWnd, uCharPos)); // Character
                    break;
            } // End SWITCH

            // We need to pass this message on to the next handler
            //   so WM_CHAR & WM_SYSCHAR can process it.
            break;
#undef  nVirtKey

#define chCharCode ((char) wParam)
        case WM_CHAR:               // chCharCode = (TCHAR) wParam; // character code
                                    // lKeyData = lParam;           // Key data
        {
            int   iChar;

            // Handle Shifted & unshifted chars
            //  e.g., 'a' = 97, 'z' = 122

            // Ignore Tab as we handled it in WM_KEYDOWN
            if (chCharCode EQ L'\t')
                return FALSE;       // We handled the msg

            // Check for Ctrl-Y (Redo)
            if (chCharCode EQ 25)
            {
                DbgBrk ();

                // Post to ourselves a request to Redo
                PostMessage (hWnd, WM_REDO, 0, 0);

                return FALSE;       // We handled the msg
            } // End IF

            iChar = chCharCode - ' ';
            if (0 <= iChar
             &&      iChar < (sizeof (aCharCode) / sizeof (aCharCode[0])))
            {
                // Get the Nrm- char code
                wChar[0] = aCharCode[iChar].nrm;
                wChar[1] = L'\0';

                // If it's valid, insert/replace it
                if (wChar[0])
                {
                    // Insert/replace the char string
                    InsRepCharStr (hWnd, GWLFE_VKSTATE, wChar);

                    return FALSE;       // We handled the msg
                } else
                // Otherwise, DbgMsg it
                {
#ifdef DEBUG
                    wsprintfW (lpwszTemp,
                               L"CHAR:  chCharCode = %d, %c",
                               chCharCode,
                               chCharCode);
                    DbgMsgW (lpwszTemp);
#endif
                } // End IF/ELSE
            } // End IF/ELSE

            break;
        } // End WM_CHAR
#undef  chCharCode

#define chCharCode ((char) wParam)
        case WM_SYSCHAR:            // chCharCode = (TCHAR) wParam; // character code
                                    // lKeyData = lParam;           // Key data
        {
            int   iChar;

            // Handle Shifted & unshifted Alt chars
            //  e.g., 'a' = 97, 'z' = 122

            iChar = chCharCode - ' ';
            if (0 <= iChar
             &&      iChar < (sizeof (aCharCode) / sizeof (aCharCode[0])))
            {
                // Get the Alt- char code
                wChar[0] = aCharCode[iChar].alt;
                wChar[1] = L'\0';

                // If it's valid, insert/replace it
                if (wChar[0])
                {
                    // Insert/replace the char string
                    InsRepCharStr (hWnd, GWLFE_VKSTATE, wChar);

                    return FALSE;       // We handled the msg
                } else
                // Otherwise, DbgMsg it
                {
#ifdef DEBUG
                    wsprintfW (lpwszTemp,
                               L"SYSCHAR:  chCharCode = %d, %c",
                               chCharCode,
                               chCharCode);
                    DbgMsgW (lpwszTemp);
#endif
                } // End IF/ELSE
            } else
            {
#ifdef DEBUG
                wsprintfW (lpwszTemp,
                           L"SYSCHAR:  chCharCode = %d, %c",
                           chCharCode,
                           chCharCode);
                DbgMsgW (lpwszTemp);
#endif
            } // End IF/ELSE

            break;
        } // End WM_SYSCHAR
#undef  chCharCode

        case WM_UNDO:               // 0 = wParam
                                    // 0 = lParam
            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Get the ptr to our Undo Buffer
            (long) lpUndoNxt = GetWindowLong (hWndParent, GWLFE_UNDO_NXT);

            do
            {
                // Split cases based upon the previous Undo Action
                switch ((--lpUndoNxt)->Action)
                {
                    case undoNone:
                        lpUndoNxt++;

                        break;

                    case undoIns:
                        // Get the character position in the Undo Entry
                        uCharPosBeg = lpUndoNxt->CharPosBeg;

                        // Unset the selection to this char position
                        SendMessageW (hWnd, EM_SETSEL, uCharPosBeg, uCharPosBeg);

                        // Put the character into a string
                        wChar[0] = lpUndoNxt->Char;
                        wChar[1] = L'\0';

                        // Insert the selection
                        SendMessageW (hWnd, EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) &wChar);

                        break;

                    case undoRep:
                        // Get the character position in the Undo Entry
                        uCharPosBeg = lpUndoNxt->CharPosBeg;

                        // Set the selection to this char position
                        SendMessageW (hWnd, EM_SETSEL, uCharPosBeg, uCharPosBeg + 1);

                        // Put the character into a string
                        wChar[0] = lpUndoNxt->Char;
                        wChar[1] = L'\0';

                        // Replace the selection
                        SendMessageW (hWnd, EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) &wChar);

                        break;

                    case undoDel:
                        // Get the character indices in the Undo Entry
                        uCharPosBeg = lpUndoNxt->CharPosBeg;
                        uCharPosEnd = lpUndoNxt->CharPosEnd;

                        // Set the selection to these chars
                        SendMessageW (hWnd, EM_SETSEL, uCharPosBeg, uCharPosEnd);

                        // Delete the selection
                        SendMessageW (hWnd, EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) L"");

                        break;

                    case undoSel:
                        // Get the character indices in the Undo Entry
                        uCharPosBeg = lpUndoNxt->CharPosBeg;
                        uCharPosEnd = lpUndoNxt->CharPosEnd;

                        // Set the selection to these chars
                        SendMessageW (hWnd, EM_SETSEL, uCharPosBeg, uCharPosEnd);

                        break;

                    case undoInsToggle:
                        // Get the current vkState
                        lvkState = GetWindowLong (hWndParent, GWLFE_VKSTATE);
                        vkState = *(LPVKSTATE) &lvkState;

                        vkState.Ins = !vkState.Ins;

                        // Save in window extra bytes
                        SetWindowLong (hWndParent, GWLFE_VKSTATE, *(long *) &vkState);

                        break;

                    defstop
                        break;
                } // End SWITCH
            } while (lpUndoNxt->Group NE UNDO_NOGROUP
                  && lpUndoNxt->Group EQ lpUndoNxt[-1].Group);

            // Save the new Undo Next ptr
            SetWindowLong (hWndParent, GWLFE_UNDO_NXT, (long) lpUndoNxt);

            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return TRUE;

        case EM_CANUNDO:

            // Get the ptrs to the next available slot in our Undo Buffer
            (long) lpUndoNxt = GetWindowLong (hWnd, GWLFE_UNDO_NXT);
            (long) lpUndoBeg = GetWindowLong (hWnd, GWLFE_UNDO_BEG);

            return (lpUndoBeg NE lpUndoNxt);

        case WM_REDO:
            DbgBrk ();              // ***FINISHME***


            break;

        case WM_CUT:                // 0 = wParam
                                    // 0 = lParam
        case WM_CLEAR:              // 0 = wParam
                                    // 0 = lParam
            // Delete selected chars and (if WM_CUT) copy to clipboard

            // Get the indices of the selected text
            SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

            // If there's no selected text, ignore this msg
            if (uCharPosBeg EQ uCharPosEnd)
                break;

            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Undo is to insert the selected chars

            // Get the next group index, and save it back
            uGroupIndex = 1 + GetWindowLong (hWndParent, GWLFE_UNDO_GRP);
            SetWindowLong (hWndParent, GWLFE_UNDO_GRP, uGroupIndex);

            // Append an Undo action to set the selection
            AppendUndo (hWndParent,                     // FE Window handle
                        undoSel,                        // Action
                        uCharPosBeg,                    // Beginning char position
                        uCharPosEnd,                    // Ending    ...
                        uGroupIndex,                    // Group index
                        0);                             // Character
            // Loop through the selected chars (if any)
            for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                AppendUndo (hWndParent,                     // FE Window handle
                            undoIns,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            0,                              // Ending    ...
                            uGroupIndex,                    // Group index
                            GetCharValue (hWnd, uCharPos)); // Character
            // We need to call the original handler first before
            //   drawing the line #s as this operation might change
            //   the count.
            lResult = CallWindowProcW (lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       message,
                                       wParam,
                                       lParam); // Pass on down the line
            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return lResult;

        case WM_PASTE:              // 0 = wParam
                                    // 0 = lParam
        {
            HANDLE  hGlbClip;
            LPWCHAR lpMem;
            BOOL    bSelection;     // TRUE iff there's a selection

            // Insert text from the clipboard, deleting selected text (if any)

            // Undo is to delete the inserted (pasted) chars
            //   and insert the deleted chars (if any)

            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Get the indices of the selected text (if any)
            SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

            // Note if there's a selection
            bSelection = uCharPosBeg NE uCharPosEnd;

            // Get the next group index, and save it back
            uGroupIndex = 1 + GetWindowLong (hWndParent, GWLFE_UNDO_GRP);
            SetWindowLong (hWndParent, GWLFE_UNDO_GRP, uGroupIndex);

            // Append an Undo action to set the selection if there was one before
            if (bSelection)
                AppendUndo (hWndParent,                     // FE Window handle
                            undoSel,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            uCharPosEnd,                    // Ending    ...
                            uGroupIndex,                    // Group index
                            0);                             // Character
            // Append the undo action (insert deleted (selected) chars)
            // Loop through the selected chars (if any)
            for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                AppendUndo (hWndParent,                     // FE Window handle
                            undoIns,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            0,                              // Ending    ...
                            uGroupIndex,                    // Group index
                            GetCharValue (hWnd, uCharPos)); // Character
            // Open the clipboard so we can read from it
            OpenClipboard (hWnd);

            // Get a handle to the clipboard data
            hGlbClip = GetClipboardData (CF_UNICODETEXT);

            // Lock the memory to get a ptr to it
            // Note we can't use MyGlobalLock/Unlock as the lock count
            //   is not modified for a clipboard (non-owned) handle
            lpMem = GlobalLock (hGlbClip); Assert (lpMem NE NULL);

            // The ending position is the beginning position plus the string length
            uCharPosEnd = uCharPosBeg + lstrlenW (lpMem);

            // We no longer need this ptr
            GlobalUnlock (hGlbClip); lpMem = NULL;

            // We're done with the clipboard
            CloseClipboard ();

            // Append the undo action (delete inserted (pasted) chars)
            AppendUndo (hWndParent,                     // FE Window handle
                        undoDel,                        // Action
                        uCharPosBeg,                    // Beginning char position
                        uCharPosEnd,                    // Ending    ...
                        uGroupIndex,                    // Group index
                        0);                             // Character
            // We need to call the original handler first before
            //   drawing the line #s as this operation might change
            //   the count.
            lResult = CallWindowProcW (lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       message,
                                       wParam,
                                       lParam); // Pass on down the line
            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return lResult;
        } // End WM_PASTE

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, ...
            return TRUE;            // We erased the background

        case WM_PAINT:              // hdc = (HDC) wParam; // The device context to draw in
            lResult = CallWindowProcW (lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       message,
                                       wParam,
                                       lParam); // Pass on down the line
            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return lResult;
    } // End SWITCH

    return CallWindowProcW (lpfnOldEditCtrlWndProc,
                            hWnd,
                            message,
                            wParam,
                            lParam); // Pass on down the line
} // End LclEditCtrlWndProc


//***************************************************************************
//  GetCurCharPos
//
//  Return the character position of the caret
//***************************************************************************

UINT GetCurCharPos
    (HWND hWnd)         // EB window handle

{
    POINT ptCaret;

    // Get the caret position in client coords
    GetCaretPos (&ptCaret);

    // Get the Position of the char under the caret
    return LOWORD (SendMessageW (hWnd, EM_CHARFROMPOS, 0, MAKELPARAM (ptCaret.x, ptCaret.y)));
} // End GetCurCharPos


//***************************************************************************
//  GetCharValue
//
//  Return the character under the caret or a given position
//***************************************************************************

WCHAR GetCharValue
    (HWND hWnd,         // EB window handle
     UINT uCharPos)     // Char position (-1 = under the caret)

{
    POINT ptCaret;
    UINT  uLineNum,
          uLinePos,
          uLineLen,
          uLineOff;

    if (uCharPos EQ -1)
    {
        // Get the caret position in client coords
        GetCaretPos (&ptCaret);

        // Get the position of the char under the caret
        //   and the line #
        uCharPos = SendMessageW (hWnd, EM_CHARFROMPOS, 0, MAKELPARAM (ptCaret.x, ptCaret.y));
        uLineNum = HIWORD (uCharPos);
        uCharPos = LOWORD (uCharPos);
    } else
        // Get the line # of the char position
        uLineNum = SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPos, 0);

    // Get the char position of the start of the current line
    uLinePos = SendMessageW (hWnd, EM_LINEINDEX, uLineNum, 0);

    // Get the length of the line
    uLineLen = SendMessageW (hWnd, EM_LINELENGTH, uLinePos, 0);

    // If the char position is at the end of the line, return '\r';
    //   if it's beyond the end of the line, return '\n'
    uLineOff = uCharPos - uLinePos;
    if (uLineOff >= uLineLen)
        return (uLineOff EQ uLineLen) ? L'\r' : L'\n';

    // Specify the maximum # chars for the buffer
    lpwszTemp[0] = (WCHAR) uLineLen;

    // Get the current line
    SendMessageW (hWnd, EM_GETLINE, uLineNum, (LPARAM) lpwszTemp);

    // Finally, return the char under the caret
    return lpwszTemp[uCharPos - uLinePos];
} // End GetCharValue


//***************************************************************************
//  AppendUndo
//
//  Append a new Undo entry
//***************************************************************************

void AppendUndo
    (HWND  hWnd,            // FE Window handle
     UINT  Action,          // Action to take
     UINT  CharPosBeg,      // Beginning character position, -1 = caret
     UINT  CharPosEnd,      // Ending    ...
     UINT  Group,           // Group index, 0 = no group
     WCHAR Char)            // Character, 0 = none

{
    LPUNDOBUF lpUndoNxt;    // Ptr to next available slot in the Undo Buffer

    // Get the ptr to the next available slot in our Undo Buffer
    (long) lpUndoNxt = GetWindowLong (hWnd, GWLFE_UNDO_NXT);

    // Save the undo entry
    lpUndoNxt->Action     = Action;
    lpUndoNxt->CharPosBeg = CharPosBeg;
    lpUndoNxt->CharPosEnd = CharPosEnd;
    lpUndoNxt->Group      = Group;
    lpUndoNxt->Char       = Char;

    // Skip over this entry
    lpUndoNxt++;

    // Save the incremented ptr in window extra bytes
    SetWindowLong (hWnd, GWLFE_UNDO_NXT, (long) lpUndoNxt);
    SetWindowLong (hWnd, GWLFE_UNDO_LST, (long) lpUndoNxt);
} // End AppendUndo


//***************************************************************************
//  InsRepCharStr
//
//  Insert or replace a char string in an edit control
//***************************************************************************

void InsRepCharStr
    (HWND    hWnd,          // EB window handle
     UINT    uGWL,          // The GetWindowLong offset
     LPWCHAR lpwch)         // The incoming char string

{
    VKSTATE vkState;        // Virtual key state (Shift, Alt, Ctrl)
    long    lvkState;       // Temporary var for vkState
    HWND    hWndParent;     // Handle of parent (FE) window
    UINT    uCharPosBeg,
            uCharPosEnd,
            uCharPos,
            uLineLen,
            uLinePos,
            uGroupIndex,    // Group index
            uStrLen;        // Incoming string length
    BOOL    bSelection;     // TRUE iff there is a selection

    // Get the handle of the parent window
    hWndParent = GetParent (hWnd);

    // Get the current vkState
    lvkState = GetWindowLong (hWndParent, uGWL);
    vkState = *(LPVKSTATE) &lvkState;

    // Get the selection indices
    SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

    // Note if there's a selection
    bSelection = uCharPosBeg NE uCharPosEnd;

    // Get the incoming string length
    uStrLen = lstrlenW (lpwch);

    // If there's no selection, and we're in Replace mode,
    //   set the selection to the current character
    if (!bSelection && !vkState.Ins)
    {
        // Get the length of the line with the given char position
        uLineLen = SendMessageW (hWnd, EM_LINELENGTH, uCharPosBeg, 0);

        // Get the char position of the start of the current line
        uLinePos = SendMessageW (hWnd, EM_LINEINDEX, (WPARAM) -1, 0);

        // Get the next group index, and save it back
        uGroupIndex = 1 + GetWindowLong (hWndParent, GWLFE_UNDO_GRP);
        SetWindowLong (hWndParent, GWLFE_UNDO_GRP, uGroupIndex);

        // Calculate the ending position of the replacement
        uCharPosEnd = uCharPosBeg + uStrLen;

        // Set the selection to a corresponding length char string so we replace
        SendMessageW (hWnd, EM_SETSEL, uCharPosBeg, min (uCharPosEnd, uLineLen + uLinePos));

        // Loop through the incoming char string
        for (; uCharPosBeg < uCharPosEnd; uCharPosBeg++)
        // If we're at the end of the line, insert rather than replace
        if (uCharPosBeg >= (uLineLen + uLinePos))
        {
            // Insert a char

            // Undo is to delete the inserted char
            AppendUndo (hWndParent,                         // FE Window handle
                        undoDel,                            // Action
                        uCharPosBeg,                        // Beginning char position
                        uCharPosBeg + 1,                    // Ending    ...
                        uGroupIndex,                        // Group index
                        0);                                 // Character
        } else
        {
            // Replace a char string

            // Undo is to replace it with the existing char
            AppendUndo (hWndParent,                         // FE Window handle
                        undoRep,                            // Action
                        uCharPosBeg,                        // Beginning char position
                        0,                                  // Ending    ...
                        uGroupIndex,                        // Group index
                        GetCharValue (hWnd, uCharPosBeg));  // Character
        } // End IF
    } else
    {
        // Insert a char string, deleting selected chars (if any)

        // Undo is to insert the deleted (selected) chars (if any)

        // Get the next group index, and save it back
        uGroupIndex = 1 + GetWindowLong (hWndParent, GWLFE_UNDO_GRP);
        SetWindowLong (hWndParent, GWLFE_UNDO_GRP, uGroupIndex);

        // Append an Undo action to set the selection if there was one before
        if (bSelection)
            AppendUndo (hWndParent,                     // FE Window handle
                        undoSel,                        // Action
                        uCharPosBeg,                    // Beginning char position
                        uCharPosEnd,                    // Ending    ...
                        uGroupIndex,                    // Group index
                        0);                             // Character
        // Loop through the selected chars (if any)
        for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
            AppendUndo (hWndParent,                     // FE Window handle
                        undoIns,                        // Action
                        uCharPosBeg,                    // Beginning char position
                        0,                              // Ending    ...
                        uGroupIndex,                    // Group index
                        GetCharValue (hWnd, uCharPos)); // Character
        // Undo is to delete the inserted char string
        AppendUndo (hWndParent,                             // FE Window handle
                    undoDel,                                // Action
                    uCharPosBeg,                            // Beginning char position
                    uCharPosBeg + uStrLen,                  // Ending    ...
                    uGroupIndex,                            // Group index
                    0);                                     // Character
    } // End IF/ELSE

    // Insert/replace the char string into the text
    SendMessageW (hWnd, EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) lpwch);
} // End InsRepCharStr


//***************************************************************************
//  DrawLineNumsFE
//
//  Draw the line #s in an FE window
//***************************************************************************

void DrawLineNumsFE
    (HWND hWnd)

{
    HDC     hDC;            // Device context
    RECT    rcPaint,        // Paint rectangle
            rcClient;       // Client area
    UINT    uLen,           // Length of string
            uLineCount,     // # lines in the edit control
            uLineTop,       // # of topmost visible line
            uCnt;           // Counter
    WCHAR   wszLineNum[FCN_INDENT + 1];  // Line # (e.g. L"[0000]\0"
    HWND    hWndParent;     // Window handle of the parent (i.e. hWndFE)

    // Get the handle to the parent window (hWndFE)
    hWndParent = GetParent (hWnd);

    // Get a device context
    hDC = MyGetDC (hWnd);

    // Set our DC attributes
    SetAttrs (hDC, hFontFE, crLineNum, COLOR_WHITE);

    // Get the # lines in the text
    uLineCount = SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

    // Get the # of the topmost visible line
    uLineTop = SendMessageW (hWnd, EM_GETFIRSTVISIBLELINE, 0, 0);

    // Less the top index
    uLineCount -= uLineTop;

    // Loop through the line #s
    for (uCnt = 0; uCnt < uLineCount; uCnt++)
    {
        // Format the line #
        wsprintfW (wszLineNum,
                   L"[%d]",
                   uCnt + uLineTop);
        uLen = lstrlenW (wszLineNum);

        // Pad out to FCN_INDENT chars
        for (; uLen < FCN_INDENT; uLen++)
            wszLineNum[uLen] = L' ';
        wszLineNum[uLen] = L'\0';

        // Calculate the rectangle size of the line #s
        SetRectEmpty (&rcPaint);
        DrawTextW (hDC,
                   wszLineNum,
                   uLen,
                   &rcPaint,
                   0
                 | DT_CALCRECT);
        // Move the rectangle down
        rcPaint.top    += uCnt * cyAveCharFE;
        rcPaint.bottom += uCnt * cyAveCharFE;

        // Draw the line #s
        DrawTextW (hDC,
                   wszLineNum,
                   uLen,
                   &rcPaint,
                   0
                 | DT_LEFT);
    } // End FOR

    // Because we might have deleted some lines whose
    //   #s we need to clear, and we have disabled the
    //   WM_ERASEBKGND message (so as to reduce screen flicker),
    //   we need to fill the bottom of the page with blanks

    // Get the client rectangle
    GetClientRect (hWnd, &rcClient);

    // Set to the same top as the next line #
    rcClient.top = rcPaint.top + cyAveCharFE;

    // Pour on the white out
    FillRect (hDC, &rcClient, (HBRUSH) GetClassLong (hWnd, GCL_HBRBACKGROUND));

    // Save the current line count for the next time
    SetWindowLong (hWndParent, GWLFE_LINECNT, uLineCount);

    // We no longer need this DC
    MyReleaseDC (hWnd, hDC);
} // End DrawLineNumsFE


//***************************************************************************
//  QueryCloseFE
//
//  Return a Boolean result on whether we can close
//***************************************************************************

BOOL QueryCloseFE
    (HWND hWnd)

{
    // If the text hasn't changed, continue with close
    if (!GetWindowLong (hWnd, GWLFE_CHANGED))
        return TRUE;

    // Ask the user what to do
    switch (MessageBox (hWnd, szCloseMessage, pszAppName, MB_YESNOCANCEL | MB_ICONQUESTION))
    {
        case IDYES:         // Save the function
            DbgBrk ();      // ***FINISHME***



            return TRUE;    // Now it's OK to close

        case IDNO:          // Do not save the function
            return TRUE;

        case IDCANCEL:      // Not OK to close
        default:            // Problem with MessageBox
            return FALSE;
    } // End SWITCH
} // End QueryClose


//***************************************************************************
//  End of File: editfcn.c
//***************************************************************************
