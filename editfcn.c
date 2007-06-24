//***************************************************************************
//  NARS2000 -- Function Editor
//***************************************************************************

#define STRICT
#include <windows.h>
#include <windowsx.h>

#include "colors.h"
#include "main.h"
#include "termcode.h"
#include "resource.h"
#include "resdebug.h"
#include "externs.h"
#include "editctrl.h"
#include "Unitrans.h"
#include "dfnhdr.h"
#include "pertab.h"

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


COLORREF crLineNum = RGB (143,188,143),   // Darkseagreen
         crLineTxt = RGB ( 65,105,225);   // Royalblue

typedef struct tagFE_CREATESTRUCT
{
    LPAPLCHAR lpwszLine;
} FE_CREATESTRUCT, *LPFE_CREATESTRUCT;


//***************************************************************************
//  $CreateFcnWindow
//
//  Create a function window
//***************************************************************************

BOOL CreateFcnWindow
    (LPWCHAR lpwszLine,
     HWND    hWndMC)

{
    HWND hWnd;
    FE_CREATESTRUCT feCreateStruct;

    // Skip over the initial UTF16_DEL, if present
    if (lpwszLine[0] EQ UTF16_DEL)
        lpwszLine++;

    // Skip over leading blanks
    while (*lpwszLine EQ L' ')
        lpwszLine++;

    // Save a ptr to the line
    feCreateStruct.lpwszLine = lpwszLine;

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
                      (LPARAM) &feCreateStruct);// Extra data,
    if (hWnd EQ NULL)
    {
        MB (pszNoCreateFEWnd);
        return FALSE;
    } // End IF

    // Save the PTD handle with the window
    SetProp (hWnd, "PTD", (HGLOBAL) GetProp (hWndMC, "PTD"));

    // Make the window visible; update its client area
    ShowWindow (hWnd, SW_SHOWNORMAL);
    UpdateWindow (hWnd);

    // Set the appropriate font in place
    SendMessageW (hWnd, WM_SETFONT, (WPARAM) hFontFE, TRUE);

    return TRUE;
} // End CreateFcnWindow


//***************************************************************************
//  $SetFmtRect
//
//  Set the formatting rectangle
//***************************************************************************

void SetFmtRect
    (HWND hWndEC,           // Window handle to the Edit Control
     UINT uLeft)            // Left margin

{
    RECT rcFmtEC;           // Formatting rectangle for the Edit Control

    // Get the formatting rectangle
    SendMessageW (hWndEC, EM_GETRECT, 0, (LPARAM) &rcFmtEC);

    // Set the left margin
    rcFmtEC.left = uLeft;

    // Tell the control about this change
    SendMessageW (hWndEC, EM_SETRECT, 0, (LPARAM) &rcFmtEC);
} // End SetFmtRect


//***************************************************************************
//  $FE_Create
//
//  Perform window-specific initialization
//***************************************************************************

void FE_Create
    (HWND hWnd)

{
} // End FE_Create


//***************************************************************************
//  $FE_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void FE_Delete
    (HWND hWnd)

{
} // End FE_Delete


//***************************************************************************
//  $FEWndProc
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
    HWND      hWndEC;       // Handle of Edit Control window
    int       iMaxLimit;    // Maximum # chars in edit control
    VKSTATE   vkState;      // Virtual key state (Shift, Alt, Ctrl)
    long      lvkState;     // Temporary var for vkState
    LPUNDOBUF lpUndoBeg,    // Ptr to start of Undo Buffer
              lpUndoNxt;    // ...    next available slot in the Undo Buffer

////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };
////LCLODSAPI ("FE: ", hWnd, message, wParam, lParam);

    // Get the handle to the edit control
    hWndEC = (HWND) GetWindowLong (hWnd, GWLSF_HWNDEC);

    switch (message)
    {
#define lpMDIcs     ((LPMDICREATESTRUCT) (((LPCREATESTRUCT) lParam)->lpCreateParams))
        case WM_CREATE:             // lpcs = (LPCREATESTRUCT) lParam; // Structure with creation data
        {
            LPSYMENTRY   lpSymName;         // Ptr to function name STE
            HGLOBAL      hGlbDfnHdr = NULL; // Defined function header global memory handle
            LPDFN_HEADER lpMemDfnHdr;       // Ptr to defined function header global memory
            HGLOBAL      hGlbPTD;           // PerTabData global memory handle
            LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

            // Initialize variables
            cfFE.hwndOwner = hWnd;
            ZeroMemory (&vkState, sizeof (vkState));

            // Save in window extra bytes
            vkState.Ins = 1;        // Initially inserting ***FIXME*** Make it an option
            SetWindowLong (hWnd, GWLSF_VKSTATE, *(long *) &vkState);

            // See if there is an existing function
            lpSymName = ParseFunctionName (hWnd, ((LPFE_CREATESTRUCT) (lpMDIcs->lParam))->lpwszLine);
            if (lpSymName
             && lpSymName->stData.stGlbData)
            {
                // Get the global memory handle
                hGlbDfnHdr = lpSymName->stData.stGlbData;

                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);
            } // End IF

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
            SetWindowLong (hWnd, GWLSF_UNDO_INI, (long) lpUndoBeg);
////////////SetWindowLong (hWnd, GWLSF_UNDO_GRP, 0);    // Already zero

            // If there's a pre-existing function,
            //   and there's an Undo Buffer
            if (hGlbDfnHdr
             && lpMemDfnHdr->hGlbUndoBuff)
            {
                LPUNDOBUF lpMemUndo;        // Ptr to Undo Buffer global memory
                UINT      uUndoSize;        // Size of Undo Buffer in bytes

                // Get the size in bytes
                uUndoSize = MyGlobalSize (lpMemDfnHdr->hGlbUndoBuff);

                // Lock the memory to get a ptr to it
                lpMemUndo = MyGlobalLock (lpMemDfnHdr->hGlbUndoBuff);

                // Copy the previous Undo Buffer contents
                CopyMemory (lpUndoBeg, lpMemUndo, uUndoSize);

                // We no longer need this ptr
                MyGlobalUnlock (lpMemDfnHdr->hGlbUndoBuff); lpMemUndo = NULL;

                // Get the ptr to the next available entry
                lpUndoNxt = (LPUNDOBUF) &((LPBYTE) lpUndoBeg)[uUndoSize];

                // Save in window extra bytes
                SetWindowLong (hWnd, GWLSF_UNDO_NXT, (long) lpUndoNxt);
                SetWindowLong (hWnd, GWLSF_UNDO_LST, (long) lpUndoNxt);
            } else
            {
                // Save in window extra bytes
                SetWindowLong (hWnd, GWLSF_UNDO_NXT, (long) lpUndoBeg);
                SetWindowLong (hWnd, GWLSF_UNDO_LST, (long) lpUndoBeg);

                // Start with an initial action of nothing
                AppendUndo (hWnd,                       // FE Window handle
                            GWLSF_UNDO_NXT,             // Offset in hWnd extra bytes of lpUndoNxt
                            undoNone,                   // Action
                            0,                          // Beginning char position
                            0,                          // Ending    ...
                            UNDO_NOGROUP,               // Group index
                            0);                         // Character
            } // End IF/ELSE

            // Save incremented starting ptr in window extra bytes
            SetWindowLong (hWnd, GWLSF_UNDO_BEG, (long) ++lpUndoBeg);

            // Create an edit control within which we can edit
            hWndEC =
            CreateWindowExW (0L,                    // Extended styles
                             LECWNDCLASS,           // Class name
                             L"",                   // Initial text
                             0
                           | WS_CHILD
                           | WS_VSCROLL
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
                             (HMENU) IDWC_FE_EC,    // ID
                             _hInstance,            // Instance
                             0);                    // lParam
            if (hWndEC EQ NULL)
            {
                MB (pszNoCreateFEEditCtrl);

                return -1;          // Stop the whole process
            } // End IF

            // Save the PTD handle with the window
            hGlbPTD = GetProp (GetParent (hWnd), "PTD");
            SetProp (hWnd, "PTD", (HGLOBAL) hGlbPTD);

            // Save in window extra bytes
            SetWindowLong (hWnd, GWLSF_HWNDEC, (long) hWndEC);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Subclass the Edit Control so we can handle some of its messages
            lpMemPTD->lpfnOldEditCtrlWndProc = (WNDPROC)
              SetWindowLongW (hWndEC,
                              GWL_WNDPROC,
                              (long) (WNDPROC) &LclEditCtrlWndProc);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // Set the paint hook
            SendMessageW (hWndEC, EM_SETPAINTHOOK, 0, (LPARAM) &LclECPaintHook);

            // If there's a pre-existing function,
            //   read in its lines as the initial text
            if (hGlbDfnHdr)
            {
                HGLOBAL   hGlbTxt;          // Line/header text global memory handle
                LPVOID    lpMem;            // Ptr to global memory
                UINT      numFcnLines,      // # lines in the function
                          uLineNum;         // Line #
                LPFCNLINE lpFcnLines;       // Ptr to array function line structs (FCNLINE[numFcnLines])

                // Get the function header text global memory handle
                hGlbTxt = lpMemDfnHdr->hGlbTxtHdr;

                // Lock the memory to get a ptr to it
                lpMem = MyGlobalLock (hGlbTxt);

                // Append the text to the Edit Control
                SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) lpMem);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbTxt); lpMem = NULL;

                // Get the # lines in the function
                numFcnLines = lpMemDfnHdr->numFcnLines;

                // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                lpFcnLines = (LPFCNLINE) &((LPBYTE) lpMemDfnHdr)[lpMemDfnHdr->offFcnLines];

                // Loop through the lines, appending the text to the Edit Control
                for (uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
                {
                    // Append a CRLF to the Edit Control
                    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) L"\r\n");

                    // Get the line text global memory handle
                    hGlbTxt = lpFcnLines->hGlbTxtLine;

                    // Lock the memory to get a ptr to it
                    lpMem = MyGlobalLock (hGlbTxt);

                    // Append the text to the Edit Control
                    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) lpMem);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTxt); lpMem = NULL;

                    // Skip to the next struct
                    lpFcnLines++;
                } // End FOR
            } else
                // Set the initial text
                SendMessageW (hWndEC,
                              EM_REPLACESEL,
                              FALSE,
                              (LPARAM) (((LPFE_CREATESTRUCT) (lpMDIcs->lParam))->lpwszLine));

            // Set the selection to the start of the buffer
            SendMessageW (hWndEC, EM_SETSEL, 0, 0);

            // Mark as no changes so far
            SetWindowLong (hWnd, GWLSF_CHANGED, FALSE);

            // If there's a pre-existing function, ...
            if (hGlbDfnHdr)
            {
                // We no longer need this ptr
                MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
            } // End IF

            // Paint the window
            ShowWindow (hWndEC, SW_SHOWNORMAL);
            UpdateWindow (hWndEC);

            // Use Post here as we need to wait for the EC window
            //   to be drawn.
            PostMessage (hWnd, MYWM_INIT_EC, 0, 0);

            break;
        } // End WM_CREATE
#undef  lpMDIcs

        case MYWM_INIT_EC:
            // Draw the line #s
            DrawLineNumsFE (hWndEC);

            return FALSE;           // We handled the msg

        case MYWM_IZITNAME:         // start = (UINT) wParam
                                    // end   = (UINT) lParam
            // This message is sent whenever the user right clicks
            //   inside a Function Edit window and the Edit Control
            //   wants to know whether or not to enable the
            //   Localize/Unlocalize menu items.
////        DbgBrk ();      /// ***FINISHME***





            return FALSE;           // Its not a name

        case MYWM_SAVE_FN:
            // Save the function (if well-formed)
            SaveFunction (hWnd);

            return FALSE;           // We handled the msg

        case MYWM_SAVECLOSE_FN:
            // Save the function (if well-formed)
            if (SaveFunction (hWnd))
                // Close the function (if allowed)
                CloseFunction (hWnd);

            return FALSE;           // We handled the msg

        case MYWM_CLOSE_FN:
            // Close the function (if allowed)
            CloseFunction (hWnd);

            return FALSE;           // We handled the msg

////////case MYWM_SAVE_AS_FN:
////////    // Save the function (if well-formed) under a different name
////////    SaveAsFunction (hWnd);
////////
////////    return FALSE;           // We handled the msg

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            FE_Delete (hWnd);

            // Initialize window-specific resources
            FE_Create (hWnd);

            break;                  // Continue with next handler

        case WM_NOTIFY:             // idCtrl = (int) wParam;
                                    // pnmh = (LPNMHDR) lParam;
#define lpnmEC  ((LPNMEDITCTRL) lParam)

            // Check for from Edit Ctrl
            if (lpnmEC->nmHdr.hwndFrom EQ hWndEC)
            {
                // Get the current vkState
                lvkState = GetWindowLong (hWnd, GWLSF_VKSTATE);
                vkState = *(LPVKSTATE) &lvkState;

                *lpnmEC->lpCaretWidth =
                  vkState.Ins ? DEF_CURWID_INS : DEF_CURWID_REP;
            } // End IF

            return FALSE;           // We handled the msg
#undef  lpnmEC


#define fwSizeType  wParam
#define nWidth      (LOWORD (lParam))
#define nHeight     (HIWORD (lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            if (fwSizeType NE SIZE_MINIMIZED)
            {
                SetWindowPos (hWndEC,           // Window handle to position
                              0,                // SWP_NOZORDER
                              0,                // X-position
                              0,                // Y-...
                              nWidth,           // Width
                              nHeight,          // Height
                              SWP_NOZORDER      // Flags
                            | SWP_SHOWWINDOW
                             );
                // Set the formatting rectangle -- make room
                //   for FCN_INDENT chars for the [line numbers]
                SetFmtRect (hWndEC, FCN_INDENT * cxAveCharFE);
            } // End IF

            break;                  // Continue with next handler ***MUST***
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_SETFONT:
            // Pass on the the edit control
            SendMessageW (hWndEC, message, wParam, lParam);

            // Setting the font also resets the formatting rectangle,
            //   so respecify it here
            // Set the formatting rectangle -- make room
            //   for FCN_INDENT chars for the [line numbers]
            SetFmtRect (hWndEC, FCN_INDENT * cxAveCharFE);

            return FALSE;           // We handled the msg

        case WM_SETFOCUS:
            // Pass on to the edit ctrl
            SetFocus (hWndEC);

            break;

        case WM_MDIACTIVATE:        // Activate/de-activate a child window
            // If we're being activated, ...
            if (GET_WM_MDIACTIVATE_FACTIVATE (hWnd, wParam, lParam))
            {
                SendMessage (GetParent (hWndEC),
                             WM_MDISETMENU,
                             GET_WM_MDISETMENU_MPS (hMenuFE, hMenuFEWindow));
                SetMenu (hWndMF, hMenuFE);
                DrawMenuBar (hWndMF);
            } // End IF

            break;                  // Continue with WM_MDIACTIVATE

        case WM_UNDO:
        case MYWM_REDO:
        case WM_COPY:
        case WM_CUT:
        case WM_PASTE:
        case MYWM_PASTE_APLWIN:
        case MYWM_PASTE_APL2:
        case WM_CLEAR:
        case MYWM_SELECTALL:
            // Pass on the the edit control
            SendMessageW (hWndEC, message, wParam, lParam);

            return FALSE;           // We handled the msg

#define wNotifyCode     (HIWORD (wParam))
#define wID             (LOWORD (wParam))
#define hWndCtrl        ((HWND) lParam)
        case WM_COMMAND:            // wNotifyCode = HIWORD (wParam); // Notification code
                                    // wID = LOWORD (wParam);         // Item, control, or accelerator identifier
                                    // hwndCtrl = (HWND) lParam;      // Handle of control
            // This message should be from the edit control
            Assert (wID      EQ IDWC_FE_EC);
            Assert (hWndCtrl EQ hWndEC);

            // Split cases based upon the notify code
            switch (wNotifyCode)
            {
                case EN_CHANGE:                     // idEditCtrl = (int) LOWORD(wParam); // identifier of edit control
                                                    // hwndEditCtrl = (HWND) lParam;      // handle of edit control
                    // Split cases based upon the last key
                    switch (GetWindowLong (hWnd, GWLSF_LASTKEY))
                    {
                        case VK_RETURN:
                        case VK_BACK:
                        case VK_DELETE:
                            // Draw the line #s
                            DrawLineNumsFE (hWndEC);

                            break;
                    } // End SWITCH

                    // The contents of the edit control have changed,
                    // set the changed flag
                    SetWindowLong (hWnd, GWLSF_CHANGED, TRUE);

                    break;

                case EN_MAXTEXT:    // idEditCtrl = (int) LOWORD(wParam); // Identifier of edit control
                                    // hwndEditCtrl = (HWND) lParam;      // Handle of edit control
                    // The edit control has exceed its maximum # chars
                    DbgBrk ();      // ***TESTME***

                    // The default maximum is 32K, so we increase it by that amount
                    iMaxLimit = SendMessageW (hWndEC, EM_GETLIMITTEXT, 0, 0);
                    SendMessageW (hWndEC, EM_SETLIMITTEXT, iMaxLimit + 32*1024, 0);

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
            } // End SWITCH

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
            (long) lpUndoBeg = GetWindowLong (hWnd, GWLSF_UNDO_BEG);
            if (lpUndoBeg)
            {
                // Free the virtual storage, first backing up to the start
                VirtualFree (--lpUndoBeg, 0, MEM_RELEASE);
                lpUndoBeg = lpUndoNxt = NULL;
                SetWindowLong (hWnd, GWLSF_UNDO_BEG, (long) lpUndoBeg);
                SetWindowLong (hWnd, GWLSF_UNDO_NXT, (long) lpUndoNxt);
            } // End IF

            // Uninitialize window-specific resources
            FE_Delete (hWnd);

            break;
    } // End SWITCH

////LCLODSAPI ("FEZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProc (hWnd, message, wParam, lParam);
} // End FEWndProc
#undef  APPEND_NAME


//***************************************************************************
//  $LclECPaintHook
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
    // ***FINISHME***




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
//  $LclEditCtrlWndProc
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

    VKSTATE      vkState;       // Virtual key state (Shift, Alt, Ctrl)
    long         lvkState;      // Temporary var for vkState
    HWND         hWndParent;    // Handle of parent (SM/FE) window
    LRESULT      lResult;       // Result from calling original handler
    LPUNDOBUF    lpUndoNxt,     // Ptr to next available slot in the Undo Buffer
                 lpUndoBeg;     // ...    first          ...
    UINT         uCharPosBeg,   // Pos of the beginning char
                 uCharPosEnd,   // ...        ending    ...
                 uCharPos,      // ...    a character position
                 uLinePos,      // Char position of start of line
                 uLineNum,      // Line #
                 uLineLen,      // Line length
                 uSpaces,       // # spaces to insert
                 uGroupIndex;   // Group index
    WCHAR        wChar[TABSTOP + 1],
                 uChar;
    BOOL         bSelection;    // TRUE iff there's a selection
    HANDLE       hGlbClip;      // Handle to the clipboard
    LPWCHAR      lpMemClip;     // Memory ptr
    UINT         ksShft,        // TRUE iff VK_CONTROL is pressed
                 ksCtrl;        // ...      VK_SHIFT   ...
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

////LCLODSAPI ("EC: ", hWnd, message, wParam, lParam);
    // Split cases
    switch (message)
    {
#define nVirtKey ((int) wParam)
        case WM_KEYDOWN:            // nVirtKey = (int) wParam;     // Virtual-key code
                                    // lKeyData = lParam;           // Key data
            // Skip this is the Menu key is pressed
            if (GetKeyState (VK_MENU) & 0x8000)
                break;

            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Save the key in the parent window's extra bytes
            SetWindowLong (hWndParent, GWLSF_LASTKEY, nVirtKey);

            // This message handles special keys that don't
            //   produce a WM_CHAR, i.e. non-printable keys,
            //   Backspace, and Delete.

            ksCtrl = (GetKeyState (VK_CONTROL) & 0x8000) ? TRUE : FALSE;
            ksShft = (GetKeyState (VK_SHIFT)   & 0x8000) ? TRUE : FALSE;

            // Process the virtual key
            switch (nVirtKey)
            {
#ifdef DEBUG
                case VK_F1:
                case VK_F2:
                case VK_F3:
                case VK_F4:
                case VK_F5:
                case VK_F6:
                case VK_F7:
////////////////case VK_F8:
                case VK_F9:
                case VK_F10:
                case VK_F11:
                case VK_F12:
                    PostMessage (hWndParent, MYWM_KEYDOWN, wParam, lParam);

                    return FALSE;

                case VK_F8:             // Display the Undo Buffer
                    DisplayUndo (hWnd); // Display the Undo Buffer

                    return FALSE;
#endif
                case VK_UP:
                case VK_DOWN:
                    if (IzitSM (hWndParent))
                    {
                        // Get the current line #
                        uLineNum = SendMessageW (hWnd, EM_LINEFROMCHAR, (WPARAM) -1, 0);

                        PostMessage (hWndParent, MYWM_KEYDOWN, wParam, uLineNum);
                    } // End IF

                    break;

                case VK_INSERT:         // Insert
                {
                    long cyAveChar;

                    // Ins      toggles the key state
                    // Shft-Ins WM_PASTEs
                    // Ctrl-Ins WM_COPYs

                    // If either VK_SHIFT or VK_CONTROL is pressed,
                    //   ignore this keystroke
                    if (ksCtrl || ksShft)
                        break;

                    // Set the cursor height
                    if (IzitSM (hWndParent))
                        cyAveChar = cyAveCharSM;
                    else
////////////////////if (IzitFE (hWndParent))    // ***FIXME*** -- Handle other Edit Control parents
                        cyAveChar = cyAveCharFE;

                    // Get the current vkState
                    lvkState = GetWindowLong (hWndParent, GWLSF_VKSTATE);
                    vkState = *(LPVKSTATE) &lvkState;

                    vkState.Ins = !vkState.Ins;

                    // Save in window extra bytes
                    SetWindowLong (hWndParent, GWLSF_VKSTATE, *(long *) &vkState);

                    // Get the char position of the caret
                    uCharPos = GetCurCharPos (hWnd);

                    // Save Undo entry
                    AppendUndo (hWndParent,                 // SM/FE Window handle
                                GWLSF_UNDO_NXT,             // Offset in hWnd extra bytes of lpUndoNxt
                                undoInsToggle,              // Action
                                uCharPos,                   // Beginning char position
                                uCharPos + 1,               // Ending    ...
                                UNDO_NOGROUP,               // Group index
                                0);                         // Character
                    break;
                } // End VK_INSERT

                case VK_RETURN:
                {
                    RECT rcFmtEC,       // Formatting rectangle for the Edit Control
                         rcPaint;

                    // If the parent is SM,
                    // pass on to caller and return
                    if (IzitSM (hWndParent))
                    {
                        // Get the current line #
                        uLineNum = SendMessageW (hWnd, EM_LINEFROMCHAR, (WPARAM) -1, 0);

                        PostMessage (hWndParent, MYWM_KEYDOWN, wParam, uLineNum);

                        return FALSE;
                    } // End IF

                    // Insert L"\r\n"

                    // Get the char position of the caret
                    uCharPos = GetCurCharPos (hWnd);

                    // Undo deletes it
                    AppendUndo (hWndParent,                 // FE Window handle
                                GWLSF_UNDO_NXT,             // Offset in hWnd extra bytes of lpUndoNxt
                                undoDel,                    // Action
                                uCharPos,                   // Beginning char position
                                uCharPos + 2,               // Ending    ...
                                UNDO_NOGROUP,               // Group index
                                0);                         // Character
                    // Get the formatting rectangle
                    SendMessageW (hWnd, EM_GETRECT, 0, (LPARAM) &rcFmtEC);

                    // Get the client rectangle
                    GetClientRect (hWnd, &rcPaint);

                    // Remove the line #s column
                    rcPaint.left = rcFmtEC.left;

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
                    // If VK_CONTROL is pressed, ignore this keystroke
                    if (ksCtrl)
                        break;

                    // Delete the preceding char or selection

                    // Undo inserts the deleted char(s)

                    // Get the indices of the selected text (if any)
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
                    uGroupIndex = 1 + GetWindowLong (hWndParent, GWLSF_UNDO_GRP);
                    SetWindowLong (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

                    // Append an Undo action to set the selection if there was one before
                    if (bSelection)
                        AppendUndo (hWndParent,                     // SM/FE Window handle
                                    GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                                    undoSel,                        // Action
                                    uCharPosBeg,                    // Beginning char position
                                    uCharPosEnd,                    // Ending    ...
                                    uGroupIndex,                    // Group index
                                    0);                             // Character
                    // Loop through the selected chars (if any)
                    for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                        AppendUndo (hWndParent,                     // SM/FE Window handle
                                    GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                                    undoIns,                        // Action
                                    uCharPosBeg,                    // Beginning char position
                                    0,                              // Ending    ...
                                    uGroupIndex,                    // Group index
                                    GetCharValue (hWnd, uCharPos)); // Character
                    break;

                case VK_TAB:
                    // Insert a tab -- convert into insert N spaces

                    // Get the indices of the selected text (if any)
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
                    InsRepCharStr (hWnd, GWLSF_VKSTATE, wChar);

                    return FALSE;       // We handled the msg

                case VK_DELETE:
                    // Delete a char or chars or a selection

                    // If both VK_CONTROL and VK_SHIFT are pressed,
                    //   ignore this keystroke
                    if (ksCtrl && ksShft)
                        break;

                    // If there's a selection,
                    //   Del       WM_CLEARs the selection
                    //   Shift-Del WM_CUTs   the selection.

                    // If there's no selection,
                    //   Del       deletes the char to the right of the cursor
                    //   Shift-Del deletes the char to the left of the cursor
                    //   Ctrl-Del  deletes to the end of the line.

                    // Undo inserts the deleted char(s)

                    // Get the indices of the selected text (if any)
                    SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

                    // Note if there's a selection
                    bSelection = uCharPosBeg NE uCharPosEnd;

                    // If there's no selection, modify the
                    //   starting and ending ptrs as per the
                    //   key state
                    if (!bSelection)
                    {
                        if (ksCtrl)     // If VK_CONTROL is pressed, ...
                        {
                            // Get the line # of the starting char
                            uLineNum = SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPosBeg, 0);

                            // Get the char position of the start of the current line
                            uLinePos = SendMessageW (hWnd, EM_LINEINDEX, uLineNum, 0);

                            // Get the length of the line
                            uLineLen = SendMessageW (hWnd, EM_LINELENGTH, uCharPosBeg, 0);

                            // Set the ending ptr to the end of the line
                            //   to delete to that point
                            uCharPosEnd = uLinePos + uLineLen;
                        } else
                        if (ksShft)     // If VK_SHIFT is pressed, ...
                            // Decrement the starting ptr to delete the lefthand char
                            uCharPosBeg--;
                        else            // Neither VK_CONTROL nor VK_SHIFT is pressed, ...
                            // Increment the ending ptr to delete the righthand char
                            uCharPosEnd++;
                    } // End IF

                    // Get the next group index, and save it back
                    uGroupIndex = 1 + GetWindowLong (hWndParent, GWLSF_UNDO_GRP);
                    SetWindowLong (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

                    // Append an Undo action to set the selection if there was one before
                    if (bSelection)
                        AppendUndo (hWndParent,                     // SM/FE Window handle
                                    GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                                    undoSel,                        // Action
                                    uCharPosBeg,                    // Beginning char position
                                    uCharPosEnd,                    // Ending    ...
                                    uGroupIndex,                    // Group index
                                    0);                             // Character
                    // Loop through the selected chars (if any)
                    for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                        AppendUndo (hWndParent,                     // SM/FE Window handle
                                    GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
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
            int iChar;

            // Handle Shifted & unshifted chars
            //  e.g., 'a' = 97, 'z' = 122

            // Ignore Tab as we handled it in WM_KEYDOWN
            if (chCharCode EQ L'\t')
                return FALSE;       // We handled the msg

            // Check for Return
            if (chCharCode EQ L'\r'         // It's CR
             && IzitSM (GetParent (hWnd)))  // Parent is SM
            {
                // If it's on the last line, move the caret to the EOL (EOB)
                if (IzitLastLine (hWnd))
                    MoveCaretEOB (hWnd);        // Move to the End-of-Buffer
                else
                // Otherwise, let the SM handle copying and restoring the lines
                    return FALSE;
            } // End IF

            // Check for Ctrl-Y (Redo)
            if (chCharCode EQ 25)
            {
                // Post to ourselves a request to Redo
                PostMessage (hWnd, WM_REDO, 0, 0);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-S (Save)
            if (chCharCode EQ 19)
            {
                // Post a request to ourselves to save the function
                PostMessage (hWnd, MYWM_SAVE_FN, wParam, lParam);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-E (Save and End)
            if (chCharCode EQ 5)
            {
                // Post a request to ourselves to save & close the function
                PostMessage (hWnd, MYWM_SAVECLOSE_FN, wParam, lParam);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-Q (Close)
            if (chCharCode EQ 17)
            {
                // Post a request to ourselves to close the function
                PostMessage (hWnd, MYWM_CLOSE_FN, wParam, lParam);

                return FALSE;       // We handled the msg
            } // End IF

////////////// Check for Ctrl-A (SaveAs)        // ***FIXME*** -- Make this work??
////////////if (chCharCode EQ 1)
////////////{
////////////    // Post a request to ourselves to save the function under a different name
////////////    PostMessage (hWnd, MYWM_SAVE_AS_FN, wParam, lParam);
////////////
////////////    return FALSE;       // We handled the msg
////////////} // End IF

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
                    InsRepCharStr (hWnd, GWLSF_VKSTATE, wChar);

                    return FALSE;       // We handled the msg
                } else
                // Otherwise, DbgMsg it
                {
#ifdef DEBUG
                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    wsprintfW (lpwszDebug,
                               L"CHAR:  chCharCode = %d, %c",
                               chCharCode,
                               chCharCode);
                    DbgMsgW (lpwszDebug);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
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
                    InsRepCharStr (hWnd, GWLSF_VKSTATE, wChar);

                    return FALSE;       // We handled the msg
                } else
                // Otherwise, DbgMsg it
                {
#ifdef DEBUG
                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    wsprintfW (lpwszDebug,
                               L"SYSCHAR:  chCharCode = %d, %c",
                               chCharCode,
                               chCharCode);
                    DbgMsgW (lpwszDebug);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
#endif
                } // End IF/ELSE
            } else
            {
#ifdef DEBUG
                // Lock the memory to get a ptr to it
                lpMemPTD = MyGlobalLock (hGlbPTD);

                wsprintfW (lpwszDebug,
                           L"SYSCHAR:  chCharCode = %d, %c",
                           chCharCode,
                           chCharCode);
                DbgMsgW (lpwszDebug);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
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
            (long) lpUndoNxt = GetWindowLong (hWndParent, GWLSF_UNDO_NXT);

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
                        lvkState = GetWindowLong (hWndParent, GWLSF_VKSTATE);
                        vkState = *(LPVKSTATE) &lvkState;

                        vkState.Ins = !vkState.Ins;

                        // Save in window extra bytes
                        SetWindowLong (hWndParent, GWLSF_VKSTATE, *(long *) &vkState);

                        break;

                    defstop
                        break;
                } // End SWITCH
            } while (lpUndoNxt->Group NE UNDO_NOGROUP
                  && lpUndoNxt->Group EQ lpUndoNxt[-1].Group);

            // Save the new Undo Next ptr
            SetWindowLong (hWndParent, GWLSF_UNDO_NXT, (long) lpUndoNxt);

            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return TRUE;

        case EM_CANUNDO:

            // Get the ptrs to the next available slot in our Undo Buffer
            (long) lpUndoNxt = GetWindowLong (hWnd, GWLSF_UNDO_NXT);
            (long) lpUndoBeg = GetWindowLong (hWnd, GWLSF_UNDO_BEG);

            return (lpUndoBeg NE lpUndoNxt);

        case WM_REDO:
            DbgBrk ();              // ***FINISHME*** -- Make Redo work??














            break;

        case WM_CUT:                // 0 = wParam
                                    // 0 = lParam
        case WM_CLEAR:              // 0 = wParam
                                    // 0 = lParam
            // Delete selected chars and (if WM_CUT) copy to clipboard

            // Get the indices of the selected text (if any)
            SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

            // If there's no selected text, ignore this msg
            if (uCharPosBeg EQ uCharPosEnd)
                break;

            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Undo inserts the selected chars

            // Get the next group index, and save it back
            uGroupIndex = 1 + GetWindowLong (hWndParent, GWLSF_UNDO_GRP);
            SetWindowLong (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

            // Append an Undo action to set the selection
            AppendUndo (hWndParent,                     // SM/FE Window handle
                        GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                        undoSel,                        // Action
                        uCharPosBeg,                    // Beginning char position
                        uCharPosEnd,                    // Ending    ...
                        uGroupIndex,                    // Group index
                        0);                             // Character
            // Loop through the selected chars (if any)
            for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                            undoIns,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            0,                              // Ending    ...
                            uGroupIndex,                    // Group index
                            GetCharValue (hWnd, uCharPos)); // Character
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // We need to call the original handler first before
            //   drawing the line #s as this operation might change
            //   the count.
            lResult = CallWindowProcW (lpMemPTD->lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       message,
                                       wParam,
                                       lParam); // Pass on down the line
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return lResult;

        case WM_PASTE:              // 0 = wParam
                                    // 0 = lParam
            // Insert text from the clipboard, deleting selected text (if any)

            // Undo deletes the inserted (pasted) chars
            //   and insert the deleted chars (if any)

            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Get the indices of the selected text (if any)
            SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

            // Note if there's a selection
            bSelection = uCharPosBeg NE uCharPosEnd;

            // Get the next group index, and save it back
            uGroupIndex = 1 + GetWindowLong (hWndParent, GWLSF_UNDO_GRP);
            SetWindowLong (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

            // Append an Undo action to set the selection if there was one before
            if (bSelection)
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                            undoSel,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            uCharPosEnd,                    // Ending    ...
                            uGroupIndex,                    // Group index
                            0);                             // Character
            // Append the undo action (insert deleted (selected) chars)
            // Loop through the selected chars (if any)
            for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
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
            lpMemClip = GlobalLock (hGlbClip); Assert (lpMemClip NE NULL);

            // The ending position is the beginning position plus the string length
            uCharPosEnd = uCharPosBeg + lstrlenW (lpMemClip);

            // We no longer need this ptr
            GlobalUnlock (hGlbClip); lpMemClip = NULL;

            // We're done with the clipboard
            CloseClipboard ();

            // Append the undo action (delete inserted (pasted) chars)
            AppendUndo (hWndParent,                     // SM/FE Window handle
                        GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                        undoDel,                        // Action
                        uCharPosBeg,                    // Beginning char position
                        uCharPosEnd,                    // Ending    ...
                        uGroupIndex,                    // Group index
                        0);                             // Character
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // We need to call the original handler first before
            //   drawing the line #s as this operation might change
            //   the count.
            lResult = CallWindowProcW (lpMemPTD->lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       message,
                                       wParam,
                                       lParam); // Pass on down the line
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return lResult;

        case MYWM_PASTE_APLWIN:
            PasteAPLChars (hWnd, UNITRANS_APLWIN);

            return FALSE;           // We handled the msg

        case MYWM_PASTE_APL2:
            PasteAPLChars (hWnd, UNITRANS_APL2);

            return FALSE;           // We handled the msg

        case MYWM_SAVE_FN:
        case MYWM_SAVECLOSE_FN:
        case MYWM_CLOSE_FN:
        case MYWM_SAVE_AS_FN:
            PostMessage (GetParent (hWnd), message, wParam, lParam);

            return FALSE;           // We handled the msg


#define wNotifyCode     (HIWORD (wParam))
#define wID             (LOWORD (wParam))
#define hWndCtrl        ((HWND) lParam)
        case WM_COMMAND:            // wNotifyCode = HIWORD (wParam); // Notification code
                                    // wID = LOWORD (wParam);         // Item, control, or accelerator identifier
                                    // hwndCtrl = (HWND) lParam;      // Handle of control
            // Split cases based upon the ID
            switch (wID)
            {
                case IDM_UNDO:
                    SendMessageW (hWnd, WM_UNDO, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_REDO:
                    SendMessageW (hWnd, MYWM_REDO, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_CUT:
                    SendMessageW (hWnd, WM_CUT, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_COPY:
                    SendMessageW (hWnd, WM_COPY, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE:
                    SendMessageW (hWnd, WM_PASTE, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_APLWIN:
                    SendMessageW (hWnd, MYWM_PASTE_APLWIN, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_APL2:
                    SendMessageW (hWnd, MYWM_PASTE_APL2, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_DELETE:
                    SendMessageW (hWnd, WM_CLEAR, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_SELECTALL:
                    SendMessageW (hWnd, MYWM_SELECTALL, 0, (LPARAM) -1);

                    return FALSE;   // We handled the msg
            } // End SWITCH

            break;

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, ...
            return TRUE;            // We erased the background

        case WM_PAINT:              // hdc = (HDC) wParam; // The device context to draw in
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            lResult = CallWindowProcW (lpMemPTD->lpfnOldEditCtrlWndProc,
                                       hWnd,
                                       message,
                                       wParam,
                                       lParam); // Pass on down the line
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return lResult;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    lResult = CallWindowProcW (lpMemPTD->lpfnOldEditCtrlWndProc,
                               hWnd,
                               message,
                               wParam,
                               lParam); // Pass on down the line
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lResult;
} // End LclEditCtrlWndProc


//***************************************************************************
//  $PasteAPLChars
//
//  Paste APL chars from another APL system
//***************************************************************************

void PasteAPLChars
    (HWND     hWndEC,       // Window handle of the Edit Control
     UNITRANS uIndex)       // ENUM tagUNITRANS index

{
    DWORD   dwSize;
    HGLOBAL hGlbClip,
            hGlbCopy;
    LPWCHAR lpMemClip,
            lpMemCopy;
    UINT    uCopy,
            uTran;

    // Open the clipboard so we can read from it
    OpenClipboard (hWndEC);

    // Get a handle to the clipboard data
    hGlbClip = GetClipboardData (CF_UNICODETEXT);

    // Get the memory size
    dwSize = GlobalSize (hGlbClip);

    // Allocate space for the new object
    // Note that we can't use MyGlobalAlloc or DbgGlobalAlloc
    //   because after we pass this handle to the clipboard
    //   we won't own it anymore.
    hGlbCopy = GlobalAlloc (GHND | GMEM_DDESHARE, dwSize);
    if (!hGlbCopy)
    {
        DbgMsg ("Unable to allocate memory for clipboard in <PasteAPLChars>");
    } else
    {
        // Lock the memory to get a ptr to it
        // Note we can't use MyGlobalLock/Unlock as the lock count
        //   is not modified for a clipboard (non-owned) handle
        lpMemClip = GlobalLock (hGlbClip); Assert (lpMemClip NE NULL);
        lpMemCopy = GlobalLock (hGlbCopy); Assert (lpMemCopy NE NULL);

        // Make a copy of the clipboard data
        CopyMemory (lpMemCopy, lpMemClip, dwSize);

        // We no longer need this ptr
        GlobalUnlock (hGlbClip); lpMemClip = NULL;

        // Convert dwSize to # elements
        dwSize /= sizeof (WCHAR);

        // Translate the APL charset to the NARS charset
        for (uCopy = 0; uCopy < dwSize; uCopy++, lpMemCopy++)
        for (uTran = 0; uTran < (sizeof (uniTrans) / sizeof (uniTrans[0])); uTran++)
        if (*lpMemCopy EQ uniTrans[uTran][uIndex])
        {
            *lpMemCopy  = uniTrans[uTran][UNITRANS_NARS];

            break;      // out of the innermost loop
        } // End FOR/FOR/IF

        // We no longer need this ptr
        GlobalUnlock (hGlbCopy); lpMemCopy = NULL;

        // Empty the clipboard
        EmptyClipboard (); hGlbClip = NULL;

        // Place the changed data onto the clipboard
        SetClipboardData (CF_UNICODETEXT, hGlbCopy); hGlbCopy = NULL;

        // We're done with the clipboard
        CloseClipboard ();

        // Pass on to the Edit Control
        SendMessageW (hWndEC, WM_PASTE, 0, 0);
    } // End IF/ELSE
} // End PasteAPLChars


//***************************************************************************
//  $IzitEOB
//
//  Return TRUE iff the caret is at the End-of-Buffer
//***************************************************************************

BOOL IzitEOB
    (HWND hWnd)         // Window handle of the Edit Control

{
    UINT uCharPos,
         uLineCnt,
         uLinePos,
         uLineLen;

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWnd);

    // Get the # lines in the text
    uLineCnt = SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

    // Get the initial char pos of the last line
    uLinePos = SendMessageW (hWnd, EM_LINEINDEX, uLineCnt - 1, 0);

    // Get the length of the last line
    uLineLen = SendMessageW (hWnd, EM_LINELENGTH, uLinePos, 0);

    return (uCharPos >= (uLinePos + uLineLen));
} // End IzitEOB


//***************************************************************************
//  $GetCurCharPos
//
//  Return the character position of the caret
//***************************************************************************

UINT GetCurCharPos
    (HWND hWndEC)       // Window handle of the Edit Control

{
    UINT uCharPosBeg,
         uCharPosEnd;

    // Get the indices of the selected text (if any)
    SendMessageW (hWndEC, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

    return uCharPosEnd;
} // End GetCurCharPos


//***************************************************************************
//  $GetCharValue
//
//  Return the character under the caret or a given position
//***************************************************************************

WCHAR GetCharValue
    (HWND hWndEC,       // Window handle of the Edit Control
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
        uCharPos = SendMessageW (hWndEC, EM_CHARFROMPOS, 0, MAKELPARAM (ptCaret.x, ptCaret.y));
        uLineNum = HIWORD (uCharPos);
        uCharPos = LOWORD (uCharPos);
    } else
        // Get the line # of the char position
        uLineNum = SendMessageW (hWndEC, EM_LINEFROMCHAR, uCharPos, 0);

    // Get the char position of the start of the current line
    uLinePos = SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the length of the line
    uLineLen = SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // If the char position is at the end of the line, return '\r';
    //   if it's beyond the end of the line, return '\n'
    uLineOff = uCharPos - uLinePos;
    if (uLineOff >= uLineLen)
        return (uLineOff EQ uLineLen) ? L'\r' : L'\n';

    // Tell EM_GETLINE maximum # chars in the buffer
    ((LPWORD) lpwszTemp)[0] = uLineLen;

    // Get the current line
    SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwszTemp);

    // Finally, return the char under the caret
    return lpwszTemp[uCharPos - uLinePos];
} // End GetCharValue


//***************************************************************************
//  $AppendUndo
//
//  Append a new Undo entry
//***************************************************************************

void AppendUndo
    (HWND  hWnd,            // SM/FE Window handle
     UINT  GWLxx_UNDO_NXT,  // Offset in hWnd extra bytes of lpUndoNxt
     UINT  Action,          // Action to take
     UINT  CharPosBeg,      // Beginning character position, -1 = caret
     UINT  CharPosEnd,      // Ending    ...
     UINT  Group,           // Group index, 0 = no group
     WCHAR Char)            // Character, 0 = none

{
    LPUNDOBUF lpUndoNxt;    // Ptr to next available slot in the Undo Buffer

    // Get the ptr to the next available slot in our Undo Buffer
    (long) lpUndoNxt = GetWindowLong (hWnd, GWLxx_UNDO_NXT);

    // Save the undo entry
    lpUndoNxt->Action     = Action;
    lpUndoNxt->CharPosBeg = CharPosBeg;
    lpUndoNxt->CharPosEnd = CharPosEnd;
    lpUndoNxt->Group      = Group;
    lpUndoNxt->Char       = Char;

    // Skip over this entry
    lpUndoNxt++;

    // Save the incremented ptr in window extra bytes
    SetWindowLong (hWnd, GWLSF_UNDO_NXT, (long) lpUndoNxt);
    SetWindowLong (hWnd, GWLSF_UNDO_LST, (long) lpUndoNxt);
} // End AppendUndo


//***************************************************************************
//  $InsRepCharStr
//
//  Insert or replace a char string in an edit control
//***************************************************************************

void InsRepCharStr
    (HWND    hWnd,          // EC window handle
     UINT    uGWL,          // The GetWindowLong offset
     LPWCHAR lpwch)         // The incoming char string

{
    VKSTATE vkState;        // Virtual key state (Shift, Alt, Ctrl)
    long    lvkState;       // Temporary var for vkState
    HWND    hWndParent;     // Handle of parent (SM/FE) window
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

    // Get the indices of the selected text (if any)
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
        uGroupIndex = 1 + GetWindowLong (hWndParent, GWLSF_UNDO_GRP);
        SetWindowLong (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

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

            // Undo deletes the inserted char
            AppendUndo (hWndParent,                         // SM/FE Window handle
                        GWLSF_UNDO_NXT,                     // Offset in hWnd extra bytes of lpUndoNxt
                        undoDel,                            // Action
                        uCharPosBeg,                        // Beginning char position
                        uCharPosBeg + 1,                    // Ending    ...
                        uGroupIndex,                        // Group index
                        0);                                 // Character
        } else
        {
            // Replace a char string

            // Undo replaces it with the existing char
            AppendUndo (hWndParent,                         // SM/FE Window handle
                        GWLSF_UNDO_NXT,                     // Offset in hWnd extra bytes of lpUndoNxt
                        undoRep,                            // Action
                        uCharPosBeg,                        // Beginning char position
                        0,                                  // Ending    ...
                        uGroupIndex,                        // Group index
                        GetCharValue (hWnd, uCharPosBeg));  // Character
        } // End IF
    } else
    {
        // Insert a char string, deleting selected chars (if any)

        // Undo inserts the deleted (selected) chars (if any)

        // Get the next group index, and save it back
        uGroupIndex = 1 + GetWindowLong (hWndParent, GWLSF_UNDO_GRP);
        SetWindowLong (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

        // Append an Undo action to set the selection if there was one before
        if (bSelection)
            AppendUndo (hWndParent,                     // SM/FE Window handle
                        GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                        undoSel,                        // Action
                        uCharPosBeg,                    // Beginning char position
                        uCharPosEnd,                    // Ending    ...
                        uGroupIndex,                    // Group index
                        0);                             // Character
        // Loop through the selected chars (if any)
        for (uCharPos = uCharPosBeg; uCharPos < uCharPosEnd; uCharPos++)
            AppendUndo (hWndParent,                     // SM/FE Window handle
                        GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                        undoIns,                        // Action
                        uCharPosBeg,                    // Beginning char position
                        0,                              // Ending    ...
                        uGroupIndex,                    // Group index
                        GetCharValue (hWnd, uCharPos)); // Character
        // Undo deletes the inserted char string
        AppendUndo (hWndParent,                     // SM/FE Window handle
                    GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                    undoDel,                        // Action
                    uCharPosBeg,                    // Beginning char position
                    uCharPosBeg + uStrLen,          // Ending    ...
                    uGroupIndex,                    // Group index
                    0);                             // Character
    } // End IF/ELSE

    // Insert/replace the char string into the text
    SendMessageW (hWnd, EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) lpwch);
} // End InsRepCharStr


//***************************************************************************
//  $IzitFE
//
//  Is the window FEWNDCLASS?
//***************************************************************************

BOOL IzitFE
    (HWND hWnd)

{
    char szClassName[32];

    GetClassName (hWnd, szClassName, sizeof (szClassName) - 1);

    return (lstrcmp (szClassName, FEWNDCLASS) EQ 0);
} // End IzitFE


//***************************************************************************
//  $IzitSM
//
//  Is the window SMWNDCLASS?
//***************************************************************************

BOOL IzitSM
    (HWND hWnd)

{
    char szClassName[32];

    GetClassName (hWnd, szClassName, sizeof (szClassName) - 1);

    return (lstrcmp (szClassName, SMWNDCLASS) EQ 0);
} // End IzitSM


//***************************************************************************
//  $DrawLineNumsFE
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
            uLineCnt,       // # lines in the edit control
            uLineTop,       // # of topmost visible line
            uCnt;           // Counter
    WCHAR   wszLineNum[FCN_INDENT + 1];  // Line # (e.g. L"[0000]\0"
    HWND    hWndParent;     // Window handle of the parent (i.e. hWndFE)

    // Get the handle to the parent window (hWndFE)
    hWndParent = GetParent (hWnd);

    // Ensure this is the function editor
    if (!IzitFE (hWndParent))
        return;

    // Get a device context
    hDC = MyGetDC (hWnd);

    // Set our DC attributes
    SetAttrs (hDC, hFontFE, crLineNum, COLOR_WHITE);

    // Get the # lines in the text
    uLineCnt = SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

    // Get the # of the topmost visible line
    uLineTop = SendMessageW (hWnd, EM_GETFIRSTVISIBLELINE, 0, 0);

    // Less the top index
    uLineCnt -= uLineTop;

    // Loop through the line #s
    for (uCnt = 0; uCnt < uLineCnt; uCnt++)
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

    // We no longer need this DC
    MyReleaseDC (hWnd, hDC);
} // End DrawLineNumsFE


//***************************************************************************
//  $QueryCloseFE
//
//  Return a Boolean result on whether we can close
//***************************************************************************

BOOL QueryCloseFE
    (HWND hWnd)

{
    // If the text hasn't changed, continue with close
    if (!GetWindowLong (hWnd, GWLSF_CHANGED))
        return TRUE;

    // Ask the user what to do
    switch (MessageBox (hWnd, szCloseMessage, pszAppName, MB_YESNOCANCEL | MB_ICONQUESTION))
    {
        case IDYES:         // Save the function
            return SaveFunction (hWnd);

        case IDNO:          // Do not save the function
            return TRUE;

        case IDCANCEL:      // Not OK to close
        default:            // Problem with MessageBox
            return FALSE;
    } // End SWITCH
} // End QueryClose


//***************************************************************************
//  $ParseFunctionName
//
//  Parse a function header and return the LPSYMENTRY
//    of the function name (if any)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseFunctionName"
#else
#define APPEND_NAME
#endif

LPSYMENTRY ParseFunctionName
    (HWND      hWndFE,          // Window handle to Function Editor
     LPAPLCHAR lpaplChar)       // Ptr to header text

{
    HWND        hWndEC;             // Window handle to Edit Control
    HGLOBAL     hGlbTknHdr = NULL;  // Tokenized header global memory handle
    FHLOCALVARS fhLocalVars = {0};  // Re-entrant vars
    LPSYMENTRY  lpSymName = NULL;   // Ptr to SYMENTRY for the function name

    Assert (IzitFE (hWndFE));

    // Get the handle to the edit control
    hWndEC = (HWND) GetWindowLong (hWndFE, GWLSF_HWNDEC);

    // Tokenize the line
    hGlbTknHdr = Tokenize_EM (lpaplChar);
    if (!hGlbTknHdr)
        return NULL;

    // Allocate virtual memory for the Variable Strand accumulator
    fhLocalVars.lpYYStrandStart =
        VirtualAlloc (NULL,      // Any address
                      DEF_STRAND_MAXSIZE * sizeof (YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!fhLocalVars.lpYYStrandStart)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ParseFunctionName:  VirtualAlloc for <fhLocalVars.lpYYStrandStart> failed");

        goto ERROR_EXIT;    // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (fhLocalVars.lpYYStrandStart,
                  DEF_STRAND_INITSIZE * sizeof (YYSTYPE),
                  MEM_COMMIT,
                  PAGE_READWRITE);
    // Parse the header
    if (ParseHeader (hWndEC, hGlbTknHdr, &fhLocalVars))
        // Get the Name's symbol table entry
        lpSymName = fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym;
    goto NORMAL_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // Free the virtual memory we allocated above
    if (fhLocalVars.lpYYStrandStart)
    {
        VirtualFree (fhLocalVars.lpYYStrandStart, 0, MEM_RELEASE); fhLocalVars.lpYYStrandStart = NULL;
    } // End IF

    if (hGlbTknHdr)
    {
        // Free the tokens
        Untokenize (hGlbTknHdr);

        // We no longer need this storage
        DbgGlobalFree (hGlbTknHdr); hGlbTknHdr = NULL;
    } // End IF

    return lpSymName;
} // End ParseFunctionName
#undef  APPEND_NAME


//***************************************************************************
//  $SaveFunction
//
//  Attempt to save a function to the WS
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SaveFunction"
#else
#define APPEND_NAME
#endif

BOOL SaveFunction
    (HWND hWndFE)

{
    HWND         hWndEC;            // Window handle to Edit Control
    UINT         uLineLen;          // Line length
    HGLOBAL      hGlbTxtHdr = NULL, // Header text global memory handle
                 hGlbTknHdr = NULL, // Tokenized header text ...
                 hGlbDfnHdr = NULL; // Defined function header ...
    LPAPLCHAR    lpMemTxtHdr;       // Ptr to header text global memory
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to defined function header ...
    BOOL         bRet = FALSE;      // TRUE iff result is valid
    FHLOCALVARS  fhLocalVars = {0}; // Re-entrant vars

    Assert (IzitFE (hWndFE));

    // Get the handle to the edit control
    hWndEC = (HWND) GetWindowLong (hWndFE, GWLSF_HWNDEC);

    // Get the length of the header line
    uLineLen = SendMessageW (hWndEC, EM_LINELENGTH, 0, 0);

    // Allocate space for the text
    //   (the "+ 1" is for a trailing zero)
    // Note, we can't use DbgGlobalAlloc here as we
    //   might have been called from the Master Frame
    //   via a system command, in which case there is
    //   no PTD for that thread.
    hGlbTxtHdr = MyGlobalAlloc (GHND, (uLineLen + 1) * sizeof (APLCHAR));
    if (!hGlbTxtHdr)
    {
        MessageBox (hWndEC,
                    "Insufficient memory to save the function header text!!",
                    pszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        return bRet;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemTxtHdr = MyGlobalLock (hGlbTxtHdr);

    // Tell EM_GETLINE maximum # chars in the buffer
    ((LPWORD) lpMemTxtHdr)[0] = uLineLen;

    // Save the text in global memory
    SendMessageW (hWndEC, EM_GETLINE, 0, (LPARAM) lpMemTxtHdr);

    // Tokenize the line
    hGlbTknHdr = Tokenize_EM ((LPAPLCHAR) lpMemTxtHdr);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbTxtHdr); lpMemTxtHdr = NULL;

    if (!hGlbTknHdr)
        goto ERROR_EXIT;

    // Allocate virtual memory for the Variable Strand accumulator
    fhLocalVars.lpYYStrandStart =
        VirtualAlloc (NULL,      // Any address
                      DEF_STRAND_MAXSIZE * sizeof (YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!fhLocalVars.lpYYStrandStart)
    {
        MessageBox (hWndEC,
                    "Insufficient memory to save the function header strand stack!!",
                    pszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        goto ERROR_EXIT;    // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (fhLocalVars.lpYYStrandStart,
                  DEF_STRAND_INITSIZE * sizeof (YYSTYPE),
                  MEM_COMMIT,
                  PAGE_READWRITE);
    // Parse the header
    if (ParseHeader (hWndEC, hGlbTknHdr, &fhLocalVars))
    {
        UINT        uLineNum,           // Current line # in the Edit Control
                    uOffset,            // Cumulative offset
                    numResultSTE,       // # result STEs (may be zero)
                    numLftArgSTE,       // # left arg ...
                    numRhtArgSTE,       // # right ...
                    numLocalsSTE,       // # locals ...
                    numFcnLines,        // # lines in the function
                    numSTE;             // Loop counter
        LPFCNLINE   lpFcnLines;         // Ptr to array of function line structs (FCNLINE[numFcnLines])
        LPUNDOBUF   lpUndoBeg,          // Ptr to start of Undo buffer
                    lpUndoLst;          // Ptr to end ...
        LPSYMENTRY  lpSymName = NULL,   // Ptr to SYMENTRY for the function name
                   *lplpSymDfnHdr;      // Ptr to LPSYMENTRYs at end of defined function header

        // Check to see if this function is already in global memory
        lpSymName = fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym;

        // If it's already in memory, free it
        if (lpSymName->stData.stGlbData)
            FreeResultGlobalDfn (lpSymName->stData.stGlbData);

        // Get # extra result STEs
        if (fhLocalVars.lpYYResult)
            // Save in global memory
            numResultSTE = fhLocalVars.lpYYResult->uStrandLen;
        else
            numResultSTE = 0;

        // Get # extra left arg STEs
        if (fhLocalVars.lpYYLftArg)
            // Save in global memory
            numLftArgSTE = fhLocalVars.lpYYLftArg->uStrandLen;
        else
            numLftArgSTE = 0;

        // Get # extra right arg STEs
        if (fhLocalVars.lpYYRhtArg)
            // Save in global memory
            numRhtArgSTE = fhLocalVars.lpYYRhtArg->uStrandLen;
        else
            numRhtArgSTE = 0;

        // Get # locals STEs
        if (fhLocalVars.lpYYLocals)
            // Save in global memory
            numLocalsSTE = fhLocalVars.lpYYLocals->uStrandLen;
        else
            numLocalsSTE = 0;

        // Get # lines in the function
        numFcnLines = SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0) - 1;

        // Allocate global memory for the function
        hGlbDfnHdr = DbgGlobalAlloc (GHND, sizeof (DFN_HEADER)
                                         + sizeof (LPSYMENTRY) * (numResultSTE
                                                                + numLftArgSTE
                                                                + numRhtArgSTE
                                                                + numLocalsSTE)
                                         + sizeof (FCNLINE) * numFcnLines);
        if (!hGlbDfnHdr)
        {
            MessageBox (hWndEC,
                        "Insufficient memory to save the function header!!",
                        pszAppName,
                        MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

        // Save numbers in global memory
        lpMemDfnHdr->numResultSTE = numResultSTE;
        lpMemDfnHdr->numLftArgSTE = numLftArgSTE;
        lpMemDfnHdr->numRhtArgSTE = numRhtArgSTE;

        // Save the static parts of the function into global memory
        lpMemDfnHdr->Sig.nature  = DFN_HEADER_SIGNATURE;
        lpMemDfnHdr->Version     = 0;

        // If it's an operator, ...
        if (fhLocalVars.OprValence)
        {
            lpMemDfnHdr->Type       = fhLocalVars.OprValence;
            lpMemDfnHdr->FcnValence = 0;
        } else
        // It's a function
        {
            lpMemDfnHdr->Type       = 3;
            lpMemDfnHdr->FcnValence = fhLocalVars.FcnValence;
        } // End IF/ELSE

        lpMemDfnHdr->RefCnt         = 1;
        lpMemDfnHdr->numFcnLines    = numFcnLines;
        lpMemDfnHdr->hGlbTxtHdr     = hGlbTxtHdr;
        lpMemDfnHdr->hGlbTknHdr     = hGlbTknHdr;

        // Get the ptr to the start of the Undo Buffer
        (long) lpUndoBeg = GetWindowLong (hWndFE, GWLSF_UNDO_BEG);
        if (lpUndoBeg)
        {
            LPUNDOBUF lpMemUndo;        // Ptr to Undo Buffer global memory

            // Get the ptr to the last entry in the Undo Buffer
            (long) lpUndoLst = GetWindowLong (hWndFE, GWLSF_UNDO_LST);

            // Check for empty Undo buffer
            if (lpUndoLst EQ lpUndoBeg)
                lpMemDfnHdr->hGlbUndoBuff = NULL;
            else
            {
                // Allocate storage for the Undo buffer
                lpMemDfnHdr->hGlbUndoBuff = DbgGlobalAlloc (GHND, (lpUndoLst - lpUndoBeg) * sizeof (UNDOBUF));
                if (!lpMemDfnHdr->hGlbUndoBuff)
                {
                    MessageBox (hWndEC,
                                "Insufficient memory to save Undo buffer!!",
                                pszAppName,
                                MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                    goto ERROR_EXIT;
                } // End IF

                // Lock the memory to get a ptr to it
                lpMemUndo = MyGlobalLock (lpMemDfnHdr->hGlbUndoBuff);

                // Copy the Undo Buffer to global memory
                CopyMemory (lpMemUndo, lpUndoBeg, (lpUndoLst - lpUndoBeg) * sizeof (UNDOBUF));

                // We no longer need this ptr
                MyGlobalUnlock (lpMemDfnHdr->hGlbUndoBuff); lpMemUndo = NULL;
            } // End IF/ELSE
        } // End IF

        // Save the dynamic parts of the function into global memory

        // Initialize cumulative offset
        uOffset = sizeof (DFN_HEADER);

        // Initialize ptr to ptr to SYMENTRYs at end of header
        lplpSymDfnHdr = (LPSYMENTRY *) &((LPBYTE) lpMemDfnHdr)[uOffset];

        // If there's a result, ...
        if (fhLocalVars.lpYYResult)
        {
            lpMemDfnHdr->offResultSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numResultSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYResult[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offResultSTE = 0;

        // If there's a left arg, ...
        if (fhLocalVars.lpYYLftArg)
        {
            lpMemDfnHdr->offLftArgSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numLftArgSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYLftArg[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offLftArgSTE = 0;

        // If there's a right arg, ...
        if (fhLocalVars.lpYYRhtArg)
        {
            lpMemDfnHdr->offRhtArgSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numRhtArgSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYRhtArg[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offRhtArgSTE = 0;

        // If there are any locals, ...
        if (fhLocalVars.lpYYLocals)
        {
            lpMemDfnHdr->offLocalsSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numLocalsSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYLocals[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offLocalsSTE = 0;

        // Save offset to start of function line structs
        if (lpMemDfnHdr->numFcnLines)
            lpMemDfnHdr->offFcnLines = uOffset;
        else
            lpMemDfnHdr->offFcnLines = 0;

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) &((LPBYTE) lpMemDfnHdr)[lpMemDfnHdr->offFcnLines];

        // Loop through the lines
        for (uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
        {
            HGLOBAL hGlbTxtLine;    // Line text global memory handle
            LPVOID  lpMem;          // Ptr to global memory
            UINT    uLineLen,       // Line length
                    uLineNdx;       // Line index

            // Get the char pos at the start of this line
            uLineNdx = SendMessageW (hWndEC, EM_LINEINDEX, uLineNum + 1, 0);

            // Get the line length of the line
            uLineLen = SendMessageW (hWndEC, EM_LINELENGTH, uLineNdx, 0);

            // Allocate global memory to hold this text
            // The "+ 1" is to include a terminating zero
            //   as well as handle GlobalLock's aversion to locking
            //   zero-length arrays
            hGlbTxtLine = DbgGlobalAlloc (GHND, (uLineLen + 1) * sizeof (APLCHAR));
            if (!hGlbTxtLine)
            {
                MessageBox (hWndEC,
                            "Insufficient memory to save a function line!!",
                            pszAppName,
                            MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                goto ERROR_EXIT;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMem = MyGlobalLock (hGlbTxtLine);

            // Tell EM_GETLINE maximum # chars in the buffer
            ((LPWORD) lpMem)[0] = uLineLen;

            // Read in the line text
            SendMessageW (hWndEC, EM_GETLINE, (WPARAM) (uLineNum + 1), (LPARAM) lpMem);

            // Save the global memory handle
            lpFcnLines->hGlbTxtLine = hGlbTxtLine;

            // Tokenize the line
            lpFcnLines->hGlbTknLine = Tokenize_EM (lpMem);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMem = NULL;

            // If tokenization failed, ...
            if (!lpFcnLines->hGlbTknLine)
            {
                DbgBrk ();      // ***FINISHME*** -- What to do here??






            } // End IF

            // No monitor info as yet
            lpFcnLines->hGlbMonInfo = NULL;

            // Skip to the next struct
            lpFcnLines++;
        } // End WHILE

        // Check for special labels ([]PROTOTYPE, []INVERSE, and []SINGLETON)
        GetSpecialLabelNums (lpMemDfnHdr);

        // Save the global memory handle in the STE
        lpSymName->stData.stGlbData = hGlbDfnHdr;

        // Mark as valued
        lpSymName->stFlags.Value = 1;

        // Mark as with the proper valence

        // Split cases based upon the function type
        switch (lpMemDfnHdr->Type)
        {
            case 1:         // Monadic operator
                lpSymName->stFlags.UsrOp1 = 1;

                break;

            case 2:         // Dyadic operator
                lpSymName->stFlags.UsrOp2 = 1;

                break;

            case 3:         // Function
                // Split cases based upon the function valence
                switch (lpMemDfnHdr->FcnValence)
                {
                    case 0:         // Niladic function
                        lpSymName->stFlags.UsrFn0 = 1;

                        break;

                    case 1:         // Monadic function
                    case 2:         // Dyadic function
                    case 3:         // Ambivalent function
                        lpSymName->stFlags.UsrFn12 = 1;

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            case 0:         // Unknown
            defstop
                break;
        } // End SWITCH

        // Mark as unchanged since the last save
        SetWindowLong (hWndFE, GWLSF_CHANGED, FALSE);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

        bRet = TRUE;

        goto NORMAL_EXIT;
    } // End IF
ERROR_EXIT:
    if (hGlbDfnHdr)
    {
        FreeResultGlobalDfn (hGlbDfnHdr); hGlbDfnHdr = NULL;
    } // End IF

    // Free the virtual memory we allocated above
    if (fhLocalVars.lpYYStrandStart)
    {
        VirtualFree (fhLocalVars.lpYYStrandStart, 0, MEM_RELEASE); fhLocalVars.lpYYStrandStart = NULL;
    } // End IF

    if (hGlbTknHdr)
    {
        // Free the tokens
        Untokenize (hGlbTknHdr);

        // We no longer need this storage
        DbgGlobalFree (hGlbTknHdr); hGlbTknHdr = NULL;
    } // End IF

    if (hGlbTxtHdr)
    {
        // We no longer need this storage
        MyGlobalFree (hGlbTxtHdr); hGlbTxtHdr = NULL;
    } // End IF
NORMAL_EXIT:
    return bRet;
} // End SaveFunction
#undef  APPEND_NAME


//***************************************************************************
//  GetSpecialLabelNums
//
//  Return the line #s (or zero if not found) of the special line labels
//***************************************************************************

void GetSpecialLabelNums
    (LPDFN_HEADER  lpMemDfnHdr)     // Ptr to defined function header

{
    UINT           numFcnLines,  // # lines in the function
                   uLineNum;     // Loop counter
    LPFCNLINE      lpFcnLines;   // Ptr to array of function line structs (FCNLINE[numFcnLines])
    LPTOKEN_HEADER lptkHdr;      // Ptr to header of tokenized line
    LPTOKEN        lptkLine;     // Ptr to tokenized line

    // Get # lines in the function
    numFcnLines = lpMemDfnHdr->numFcnLines;

    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
    lpFcnLines = (LPFCNLINE) &((LPBYTE) lpMemDfnHdr)[lpMemDfnHdr->offFcnLines];

    // Loop through the function lines
    for (uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
    {
        UINT numTokens;     // # tokens in the line

        // Lock the memory to get a ptr to it
        lptkHdr = MyGlobalLock (lpFcnLines->hGlbTknLine);

        // Get the # tokens in the line
        numTokens = lptkHdr->TokenCnt;

        // Get ptr to the tokens in the line
        lptkLine = (LPTOKEN) &((LPBYTE) lptkHdr)[sizeof (TOKEN_HEADER)];

        Assert (lptkLine[0].tkFlags.TknType EQ TKT_EOL);

        // If there are at least three tokens
        //   (TKT_EOL, TKT_VARNAMED, TKT_COLON)
        if (numTokens < 3)
            continue;

        if (lptkLine[2].tkFlags.TknType EQ TKT_COLON
         && lptkLine[1].tkFlags.TknType EQ TKT_VARNAMED)
        {
            HGLOBAL   hGlbName;     // Name's global memory handle
            LPAPLCHAR lpMemName;    // Ptr to the name

            // stData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkLine[1].tkData.tkSym) EQ PTRTYPE_STCONST);

            // Get the Name's global memory handle
            hGlbName = lptkLine[1].tkData.tkSym->stHshEntry->hGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            if (lstrcmpiW (lpMemName, WS_UTF16_QUAD L"prototype") EQ 0)
                lpMemDfnHdr->nPrototypeLine = uLineNum + 1;

            if (lstrcmpiW (lpMemName, WS_UTF16_QUAD L"inverse") EQ 0)
                lpMemDfnHdr->nInverseLine   = uLineNum + 1;

            if (lstrcmpiW (lpMemName, WS_UTF16_QUAD L"singleton") EQ 0)
                lpMemDfnHdr->nSingletonLine = uLineNum + 1;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (lpFcnLines->hGlbTknLine); lptkHdr = NULL; lptkLine = NULL;

        // Skip to the next struct
        lpFcnLines++;
    } // End FOR
} // End GetSpecialLabelNums


//// //***************************************************************************
//// //  $SaveAsFunction
//// //
//// //  Attempt to save a function to the WS using a different name
//// //***************************************************************************
////
//// BOOL SaveAsFunction
////     (HWND hWndFE)
////
//// {
////     DbgBrk ();          // ***FINISHME***
////
////     Assert (IzitFE (hWndFE));
////
////
////
////
////
////
////
////     return FALSE;
//// } // End SaveAsFunction


//***************************************************************************
//  $CloseFunction
//
//  Attempt to close a function
//***************************************************************************

BOOL CloseFunction
    (HWND hWndFE)

{
    Assert (IzitFE (hWndFE));

    // If it's OK to close the window, do so, else ignore
    if (QueryCloseFE (hWndFE))
    {
        // Close the Function Editor window
        SendMessage (GetParent (hWndFE), WM_MDIDESTROY, (WPARAM) hWndFE, 0);

        return TRUE;
    } // End IF

    return FALSE;
} // End CloseFunction


//***************************************************************************
//  End of File: editfcn.c
//***************************************************************************
