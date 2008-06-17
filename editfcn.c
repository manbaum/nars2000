//***************************************************************************
//  NARS2000 -- Function Editor
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

#define STRICT
#include <windows.h>
#include <windowsx.h>

#include "uniscribe.h"
#include "aplerrors.h"
#include "colors.h"
#include "main.h"
#include "resource.h"
#include "resdebug.h"
#include "sysvars.h"
#include "termcode.h"
#include "externs.h"
#include "editctrl.h"
#include "unitranshdr.h"
#include "unitranstab.h"
#include "dfnhdr.h"
#include "pertab.h"
#include "sis.h"


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
 * Line #s optional
 * Move Undo Code into EDITCTRL.C
 * Allow external editor
 *

 */

char szCloseMessage[] = "You have changed the body of this function;"
                        " save the changes?";

COLORREF crLineNum = RGB (143,188,143),   // Darkseagreen
         crLineTxt = RGB ( 65,105,225);   // Royalblue

// Define struct for passing parameters to WM_NCCREATE/WM_CREATE
//   for the Function Edit window
typedef struct tagFE_CREATESTRUCTW
{
    LPAPLCHAR lpwszLine;
    UINT      ErrCode;
} FE_CREATESTRUCTW, UNALIGNED *LPFE_CREATESTRUCTW;


typedef struct tagCLIPFMTS
{
    UINT    uFmtNum;                // Format #
    HGLOBAL hGlbFmt;                // Handle for this format #
} CLIPFMTS, *LPCLIPFMTS;


//***************************************************************************
//  $CreateFcnWindow
//
//  Create a function window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CreateFcnWindow"
#else
#define APPEND_NAME
#endif

BOOL CreateFcnWindow
    (LPWCHAR lpwszLine)             // Ptr to text after {del}

{
    HWND             hWnd;              // Handle for Function Editor window
    FE_CREATESTRUCTW feCreateStructW;   // CreateStruct for Function Editor window
    HGLOBAL          hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA     lpMemPTD;          // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Skip over the initial UTF16_DEL, if present
    if (lpwszLine[0] EQ UTF16_DEL)
        lpwszLine++;

    // Skip over leading blanks
    while (*lpwszLine EQ L' ')
        lpwszLine++;

    // Save a ptr to the line
    feCreateStructW.lpwszLine = lpwszLine;

    // Initialize the return error code
    feCreateStructW.ErrCode   = 0;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    hWnd =
      CreateMDIWindowW (LFEWNDCLASS,        // Class name
                        wszFETitle,         // Window title
                        0,                  // Styles
                        CW_USEDEFAULT,      // X-pos
                        CW_USEDEFAULT,      // Y-pos
                        CW_USEDEFAULT,      // Height
                        CW_USEDEFAULT,      // Width
                        lpMemPTD->hWndMC,   // Parent
                        _hInstance,         // Instance
              (LPARAM) &feCreateStructW);   // Extra data,
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // If it didn't succeed, ...
    if (hWnd EQ NULL)
    {
        switch (feCreateStructW.ErrCode)
        {
            case NAMETYPE_VAR:
                MB (pszNoEditVars);

                break;

            case NAMETYPE_FN12:
            case NAMETYPE_OP1:
            case NAMETYPE_OP2:
            case NAMETYPE_OP3:
                MB (pszNoEditPrimFns);

                break;

            case NAMETYPE_UNK:
            case NAMETYPE_FN0:
                MB (pszNoCreateFEWnd);

                break;

            defstop
                break;
        } // End SWITCH

        return FALSE;
    } // End IF

    // Make the window visible; update its client area
    ShowWindow (hWnd, SW_SHOWNORMAL);
    UpdateWindow (hWnd);

    // Set the appropriate font in place
    SendMessageW (hWnd, WM_SETFONT, (WPARAM) hFontFE, MAKELPARAM (TRUE, 0));

    return TRUE;
} // End CreateFcnWindow
#undef  APPEND_NAME


//***************************************************************************
//  $SetMarginsFE
//
//  Set the margins for a Function Editor window
//***************************************************************************

void SetMarginsFE
    (HWND hWndEC,           // Window handle to the Edit Control
     UINT uLeft)            // Left margin

{
    SendMessageW (hWndEC, EM_SETMARGINS, EC_LEFTMARGIN, uLeft);
} // End SetMarginsFE


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
    (HWND hWnd,                     // Window handle
     UINT message,                  // Type of message
     UINT wParam,                   // Additional information
     LONG lParam)                   // ...

{
    HWND         hWndEC;            // Handle of Edit Control window
    VKSTATE      vkState;           // Virtual key state (Shift, Alt, Ctrl)
    long         lvkState;          // Temporary var for vkState
    LPUNDO_BUF   lpUndoBeg,         // Ptr to start of Undo Buffer
                 lpUndoNxt;         // ...    next available slot in the Undo Buffer
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPMEMVIRTSTR lpLclMemVirtStr;   // Ptr to local MemVirtStr

////static DWORD aHelpIDs[] = {
////                           IDOK,             IDH_OK,
////                           IDCANCEL,         IDH_CANCEL,
////                           IDC_APPLY,        IDH_APPLY,
////                           IDC_HELP2,        IDH_HELP2,
////                           0,             0,
////                          };
////LCLODSAPI ("FE: ", hWnd, message, wParam, lParam);

    // Get the handle to the edit control
    (HANDLE_PTR) hWndEC = (GetWindowLongPtrW (hWnd, GWLSF_HWNDEC));

    switch (message)
    {
#define lpMDIcs     ((LPMDICREATESTRUCTW) ((*(LPCREATESTRUCTW *) &lParam)->lpCreateParams))
        case WM_CREATE:             // lpcs = (LPCREATESTRUCTW) lParam; // Structure with creation data
        {
            LPSYMENTRY   lpSymName;         // Ptr to function name STE
            HGLOBAL      hGlbDfnHdr = NULL; // User-defined function/operator header global memory handle
            LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header global memory

            // Initialize variables
            cfFE.hwndOwner = hWnd;
            ZeroMemory (&vkState, sizeof (vkState));

            // Save in window extra bytes
            vkState.Ins = TRUE;     // Initially inserting ***FIXME*** Make it an option
            SetWindowLongW (hWnd, GWLSF_VKSTATE, *(long *) &vkState);

            // See if there is an existing function
            lpSymName = ParseFunctionName (hWnd, (*(LPFE_CREATESTRUCTW *) &lpMDIcs->lParam)->lpwszLine);
            if (lpSymName)
            {
                // If it has a value and is not a user-defined function/operator, ...
                if (lpSymName->stFlags.Value
                 && !lpSymName->stFlags.UsrDfn)
                {
                    ((LPFE_CREATESTRUCTW) (lpMDIcs->lParam))->ErrCode = lpSymName->stFlags.stNameType;

                    return -1;
                } else
                if (lpSymName->stData.stGlbData)
                {
                    // Get the global memory handle
                    hGlbDfnHdr = lpSymName->stData.stGlbData;

                    // Clear the ptr type bits
                    hGlbDfnHdr = ClrPtrTypeDirAsGlb (hGlbDfnHdr);

                    // Lock the memory to get a ptr to it
                    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);
                } // End IF/ELSE
            } // End IF

            // At some point, we'll read in the undo buffer from
            //   the saved function and copy it to virtual memory
            //   also setting the _BEG, _NXT, and _LST ptrs.

            // _BEG is the (static) ptr to the beginning of the virtual memory.
            // _NXT is the (dynamic) ptr to the next available entry.
            //    Undo entries are between _NXT[-1] and _BEG, inclusive.
            // _LST is the (dynamic) ptr to the last available entry.
            //    Redo entries are between _NXT and _LST[-1], inclusive.

            // Allocate room for one MemVirtStr
            lpLclMemVirtStr =
              MyVirtualAlloc (NULL,                     // Any address (FIXED SIZE)
                              1 * sizeof (MEMVIRTSTR),
                              MEM_COMMIT | MEM_TOP_DOWN,
                              PAGE_READWRITE);
            if (!lpLclMemVirtStr)
            {
                // ***FIXME*** -- Display error msg

                return -1;
            } // End IF

            // Allocate virtual memory for the Undo Buffer
#ifdef DEBUG
            lpLclMemVirtStr[0].lpText   = "lpUndoBeg in <FEWndProc>";
#endif
            lpLclMemVirtStr[0].IncrSize = DEF_UNDOBUF_INCRSIZE * sizeof (UNDO_BUF);
            lpLclMemVirtStr[0].MaxSize  = DEF_UNDOBUF_MAXSIZE  * sizeof (UNDO_BUF);
            lpLclMemVirtStr[0].IniAddr  = (LPUCHAR)
            lpUndoBeg =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[0].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (!lpLclMemVirtStr[0].IniAddr)
            {
                // ***FIXME*** -- Display error msg

                return -1;
            } // End IF

            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_LPMVS, (__int3264) (LONG_PTR) lpLclMemVirtStr);

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[0]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[0].IniAddr,
                            DEF_UNDOBUF_INITSIZE * sizeof (UNDO_BUF),
                            MEM_COMMIT,
                            PAGE_READWRITE);
            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_INI, (__int3264) (LONG_PTR) lpUndoBeg);
////////////SetWindowLongW (hWnd, GWLSF_UNDO_GRP, 0);    // Already zero

            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_NXT, (__int3264) (LONG_PTR) lpUndoBeg);
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_LST, (__int3264) (LONG_PTR) lpUndoBeg);

            // Start with an initial action of nothing
            AppendUndo (hWnd,                       // FE Window handle
                        GWLSF_UNDO_NXT,             // Offset in hWnd extra bytes of lpUndoNxt
                        undoNone,                   // Action
                        0,                          // Beginning char position
                        0,                          // Ending    ...
                        UNDO_NOGROUP,               // Group index
                        0);                         // Character
            // If there's a pre-existing function,
            //   and there's an Undo Buffer
            if (hGlbDfnHdr
             && lpMemDfnHdr->hGlbUndoBuff)
            {
                LPUNDO_BUF lpMemUndo;       // Ptr to Undo Buffer global memory
                SIZE_T     uUndoSize;       // Size of Undo Buffer in bytes

                // Get the size in bytes
                uUndoSize = MyGlobalSize (lpMemDfnHdr->hGlbUndoBuff);

                // Lock the memory to get a ptr to it
                lpMemUndo = MyGlobalLock (lpMemDfnHdr->hGlbUndoBuff);

                // Copy the previous Undo Buffer contents
                CopyMemory (&lpUndoBeg[1], lpMemUndo, uUndoSize);

                // We no longer need this ptr
                MyGlobalUnlock (lpMemDfnHdr->hGlbUndoBuff); lpMemUndo = NULL;

                // Get the ptr to the next available entry
                lpUndoNxt = (LPUNDO_BUF) ByteAddr (&lpUndoBeg[1], uUndoSize);

                // Save in window extra bytes
                SetWindowLongPtrW (hWnd, GWLSF_UNDO_NXT, (__int3264) (LONG_PTR) lpUndoNxt);
                SetWindowLongPtrW (hWnd, GWLSF_UNDO_LST, (__int3264) (LONG_PTR) lpUndoNxt);
            } // End IF

            // Save incremented starting ptr in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_BEG, (__int3264) (LONG_PTR) ++lpUndoBeg);

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

            // Get the PerTabData global memory handle
            hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_HWNDEC, (__int3264) (LONG_PTR) hWndEC);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Subclass the Edit Control so we can handle some of its messages
            (HANDLE_PTR) lpMemPTD->lpfnOldEditCtrlWndProc =
              SetWindowLongPtrW (hWndEC,
                                 GWL_WNDPROC,
                                 (__int3264) (LONG_PTR) (WNDPROC) &LclEditCtrlWndProc);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // Set the paint hook
            SendMessageW (hWndEC, EM_SETPAINTHOOK, 0, (LPARAM) &LclECPaintHook);

            // If there's a pre-existing function,
            //   read in its lines as the initial text
            if (hGlbDfnHdr)
            {
                HGLOBAL        hGlbTxtLine;     // Line/header text global memory handle
                LPMEMTXT_UNION lpMemTxtLine;    // Ptr to header/line text global memory
                UINT           numFcnLines,     // # lines in the function
                               uLineNum;        // Line #
                LPFCNLINE      lpFcnLines;      // Ptr to array function line structs (FCNLINE[numFcnLines])

                // Get the function header text global memory handle
                hGlbTxtLine = lpMemDfnHdr->hGlbTxtHdr;

                // Lock the memory to get a ptr to it
                lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                // Append the text to the Edit Control
                SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) &lpMemTxtLine->C);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

                // Get the # lines in the function
                numFcnLines = lpMemDfnHdr->numFcnLines;

                // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

                // Loop through the lines, appending the text to the Edit Control
                for (uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
                {
                    // Append a CRLF to the Edit Control
                    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) L"\r\n");

                    // Get the line text global memory handle
                    hGlbTxtLine = lpFcnLines->hGlbTxtLine;

                    // Lock the memory to get a ptr to it
                    lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                    // Append the text to the Edit Control
                    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) &lpMemTxtLine->C);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

                    // Skip to the next struct
                    lpFcnLines++;
                } // End FOR
            } else
                // Set the initial text
                SendMessageW (hWndEC,
                              EM_REPLACESEL,
                              FALSE,
                              (LPARAM) (((LPFE_CREATESTRUCTW) (lpMDIcs->lParam))->lpwszLine));
            // Mark as no changes so far
            SetWindowLongW (hWnd, GWLSF_CHANGED, FALSE);

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
            PostMessageW (hWnd, MYWM_INIT_EC, 0, 0);

            break;
        } // End WM_CREATE
#undef  lpMDIcs

        case MYWM_INIT_EC:
            // Draw the line #s
            DrawLineNumsFE (hWndEC);

            return FALSE;           // We handled the msg

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
#define lpnmEC  (*(LPNMEDITCTRL *) &lParam)

            // Check for from Edit Ctrl
            if (lpnmEC->nmHdr.hwndFrom EQ hWndEC)
            {
                // Get the current vkState
                lvkState = GetWindowLongW (hWnd, GWLSF_VKSTATE);
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
                SetWindowPos (hWndEC,           // Window handle to position
                              0,                // SWP_NOZORDER
                              0,                // X-position
                              0,                // Y-...
                              nWidth,           // Width
                              nHeight,          // Height
                              SWP_NOZORDER      // Flags
                            | SWP_SHOWWINDOW
                             );
            break;                  // *MUST* pass on to DefMDIChildProcW
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_SETFONT:
            // Pass on to the edit control
            SendMessageW (hWndEC, message, wParam, lParam);

            // Changing the font also means changing the size
            //   of the margins as the character width might change
            SetMarginsFE (hWndEC, FCN_INDENT * cxAveCharFE);

            return FALSE;           // We handled the msg

        case MYWM_SETFOCUS:
            // Set the focus to the Function Editor so the cursor displays
            SetFocus (hWnd);

            return FALSE;           // We handled the msg

        case WM_SETFOCUS:
            // Pass on to the edit ctrl
            SetFocus (hWndEC);

            // Draw the line #s
            DrawLineNumsFE (hWndEC);

            break;                  // *MUST* pass on to DefMDIChildProcW

        case WM_MDIACTIVATE:        // Activate/de-activate a child window
            // If we're being activated, ...
            if (GET_WM_MDIACTIVATE_FACTIVATE (hWnd, wParam, lParam))
                ActivateMDIMenu (hMenuFE, hMenuFEWindow);
            break;                  // Continue with DefMDIChildProcW

        case WM_UNDO:
        case MYWM_REDO:
        case WM_COPY:
        case WM_CUT:
        case WM_PASTE:
        case MYWM_PASTE_APL:
        case WM_CLEAR:
        case MYWM_SELECTALL:
            // Pass on to the edit control
            SendMessageW (hWndEC, message, wParam, lParam);

            return FALSE;           // We handled the msg

#define wNotifyCode     (HIWORD (wParam))
#define wID             (LOWORD (wParam))
#define hWndCtrl        (*(HWND *) &lParam)
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
                    switch (GetWindowLongW (hWnd, GWLSF_LASTKEY))
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
                    SetWindowLongW (hWnd, GWLSF_CHANGED, TRUE);

                    break;

                case EN_MAXTEXT:    // idEditCtrl = (int) LOWORD(wParam); // Identifier of edit control
                                    // hwndEditCtrl = (HWND) lParam;      // Handle of edit control
                    // The edit control has exceed its maximum # chars

                    // The default maximum is 32K, so we increase it by that amount
                    SendMessageW (hWndEC,
                                  EM_SETLIMITTEXT,
                                  SendMessageW (hWndEC, EM_GETLIMITTEXT, 0, 0) + 32*1024, 0);
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
            // Remove all saved window properties
            EnumProps (hWnd, EnumCallbackRemoveProp);

            // Get the ptr to local MemVirtStr
            (HANDLE_PTR) lpLclMemVirtStr = GetWindowLongPtrW (hWnd, GWLSF_LPMVS);

            // If we allocated virtual storage, ...
            if (lpLclMemVirtStr)
            {
                // If we allocated virtual storage, ...
                if (lpLclMemVirtStr[0].IniAddr)
                {
                    // Free the virtual storage
                    MyVirtualFree (lpLclMemVirtStr[0].IniAddr, 0, MEM_RELEASE); lpLclMemVirtStr[0].IniAddr = NULL;

                    // Unlink this entry from the chain
                    UnlinkMVS (&lpLclMemVirtStr[0]);
                } // End IF

                // Free the virtual storage
                MyVirtualFree (lpLclMemVirtStr, 0, MEM_RELEASE); lpLclMemVirtStr = NULL;

                // Zap it in case we come through again
                SetWindowLongPtrW (hWnd, GWLSF_LPMVS, 0);
            } // End IF

            // Uninitialize window-specific resources
            FE_Delete (hWnd);

            break;
    } // End SWITCH

////LCLODSAPI ("FEZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProcW (hWnd, message, wParam, lParam);
} // End FEWndProc
#undef  APPEND_NAME


//***************************************************************************
//  $LclECPaintHook
//
//  Local Edit Control paint hook
//***************************************************************************

int LclECPaintHook
    (HWND    hWndEC,    // Window handle of Edit Control
     HDC     hDC,       // The Device Context
     int     x,         // The x-coordinate (Client Area)
     int     y,         // ... y- ...
     LPWCHAR lpwsz,     // Ptr to start of line
     UINT    uCol,      // Starting column in the line
     UINT    uLen)      // Length of text to display

{
    RECT             rc;                // Rect for actual width/height
#ifndef UNISCRIBE
    HGLOBAL          hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA     lpMemPTD;          // Ptr to PerTabData global memory
    int              iRes;              // The result
    long             cxAveChar;         // Width of average char in given screen

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);
#endif
    // Syntax Color the line

    // To do this, we use a FSA to parse the line from the start
    //   through the last char to display
    // ***FINISHME*** -- Syntax coloring

    rc.top    = y;
    rc.left   = x;
    rc.right  =
    rc.bottom = 0;

    // Skip to the starting col
    lpwsz += uCol;

    // Calculate the width & height of the line
    DrawTextW (hDC,
               lpwsz,
               uLen,
              &rc,
               0
             | DT_CALCRECT
             | DT_NOPREFIX
             | DT_NOCLIP);
#ifndef UNISCRIBE
    // On some systems when the alternate font isn't the same
    //   width as the SM font, the width calculated by DT_CALCRECT
    //   is too small, so we recalculate it here
    rc.right = rc.left + cxAveCharSM * uLen;

    // Calculate the result
    iRes = MAKELONG (rc.right - rc.left, rc.bottom - rc.top);

    // Get the appropriate cxAveChar
    cxAveChar = IzitSM (GetParent (hWndEC)) ? cxAveCharSM : cxAveCharFE;

    if (!lpMemPTD->lpFontLink
     || FAILED (DrawTextFL (lpMemPTD->lpFontLink, hDC, &rc, lpwsz, uLen, cxAveChar)))
        OneDrawTextW (hDC, &rc, &lpwsz, uLen, cxAveChar);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return iRes;
#else
    // Draw the line for real
    DrawTextW (hDC,
               lpwsz,
               uLen,
              &rc,
               0
             | DT_SINGLELINE
             | DT_NOPREFIX
             | DT_NOCLIP);
    return MAKELONG (rc.right - rc.left, rc.bottom - rc.top);
#endif
} // End LclECPaintHook


#ifndef UNISCRIBE
//***************************************************************************
//  $DrawTextFL
//
//  DrawText with font linking as seen in
//  http://blogs.msdn.com/oldnewthing/archive/2004/07/16/185261.aspx
//***************************************************************************

int DrawTextFL
    (IMLangFontLink *pfl,
     HDC             hdc,
     LPRECT          lprc,
     LPCWSTR         lpsz,
     long            cch,
     long            cxAveChar)

{
    HRESULT hr;
    HFONT   hfOrig = (HFONT) GetCurrentObject (hdc, OBJ_FONT);
    DWORD   dwFontCodePages = 0;

    hr = IMLangFontLink_GetFontCodePages (pfl, hdc, hfOrig, &dwFontCodePages);
    if (SUCCEEDED (hr))
    {
        while (cch > 0)
        if (FAILED (hr = DrawTextFLsub (pfl, hdc, dwFontCodePages, hfOrig, lprc, cxAveChar, &lpsz, &cch)))
            break;

        if (FAILED (hr))
        {
            //  We started outputting characters so we have to finish.
            //  Do the rest without font linking since we have no choice.
            OneDrawTextW (hdc, lprc, &lpsz, cch, cxAveChar);

            hr = S_FALSE;
        } // End IF
    } // End IF

    return hr;
} // End DrawTextFL
#endif


#ifndef UNISCRIBE
//***************************************************************************
//  $DrawTextFLsub
//
//  Subroutine to DrawTextFL
//***************************************************************************

HRESULT DrawTextFLsub
    (IMLangFontLink *pfl,
     HDC             hdc,
     DWORD           dwFontCodePages,
     HFONT           hfOrig,
     LPRECT          lprc,
     long            cxAveChar,
     LPCWSTR        *lplpsz,
     int            *lpcch)

{
    HRESULT hr;
    long    cchActual;
    HFONT   hfLinked;
    DWORD   dwActualCodePages;
    int     cchCnt,
            cch;

    hr = IMLangFontLink_GetStrCodePages (pfl, *lplpsz, *lpcch, dwFontCodePages, &dwActualCodePages, &cchActual);
    if (FAILED (hr))
        return hr;

    if (dwActualCodePages & dwFontCodePages)
        OneDrawTextW (hdc, lprc, lplpsz, cchActual, cxAveChar);
    else
    if (FAILED (hr = IMLangFontLink_MapFont (pfl, hdc, dwActualCodePages, 0, &hfLinked)))
    {
        // If the actual code pages are non-zero and cchActual > 1,
        //   run through the chars one-by-one
        if (dwActualCodePages && cchActual > 1)
        {
            for (cchCnt = 0; cchCnt < cchActual; cchCnt++, (*lpcch)--)
            {
                cch = 1;

                if (FAILED (hr = DrawTextFLsub (pfl, hdc, dwFontCodePages, hfOrig, lprc, cxAveChar, lplpsz, &cch)))
                    return hr;
            } // End FOR

            return S_OK;
        } else
        // If the char is {infinity} or A-Z underbar, ...
        if (**lplpsz EQ UTF16_INFINITY
         || (UTF16_A_ <= **lplpsz
          &&             **lplpsz <= UTF16_Z_))
            // Use the alternate font
            SelectObject (hdc, hFontAlt);

        // Output the char
        OneDrawTextW (hdc, lprc, lplpsz, cchActual, cxAveChar);

        // Restore the original font (even if we didn't change it out)
        SelectObject (hdc, hfOrig);

        // Set the return code to OK even though we failed to find a linked font
        hr = S_OK;
    } else
    {
        // Use the linked font
        SelectObject (hdc, hfLinked);
        OneDrawTextW (hdc, lprc, lplpsz, cchActual, cxAveChar);
        SelectObject (hdc, hfOrig);

        // Release the linked font
        IMLangFontLink_ReleaseFont (pfl, hfLinked);
    } // End IF/ELSE

    // Update the string count
    (*lpcch) -= cchActual;

    return hr;
} // End DrawTextFLsub
#endif


#ifndef UNISCRIBE
//***************************************************************************
//  $OneDrawTextW
//
//  Output one chunk all with the same font
//***************************************************************************

void OneDrawTextW
    (HDC      hdc,
     LPRECT   lprc,
     LPCWSTR *lplpsz,
     long     cchActual,
     long     cxAveChar)

{
    int  cch;

    for (cch = 0; cch < cchActual; cch++)
    {
        DrawTextW (hdc,
                  (*lplpsz)++,
                   1,
                   lprc,
                   0
                 | DT_SINGLELINE
                 | DT_NOPREFIX
                 | DT_NOCLIP);
        // Update the horizontal position
        lprc->left += cxAveChar;
    } // End FOR
} // End OneDrawTextW
#endif


//***************************************************************************
//  $IzitNameChar
//
//  Return TRUE iff the given APLCHAR is valid in a variable/function/operator name
//***************************************************************************

BOOL IzitNameChar
    (APLCHAR aplChar)

{
    return ((L'a' <= aplChar
          &&         aplChar <= L'z')
         || (L'A' <= aplChar
          &&         aplChar <= L'Z')
         || (L'0' <= aplChar
          &&         aplChar <= L'9')
         || aplChar EQ UTF16_DELTA
         || aplChar EQ UTF16_DELTAUNDERBAR
         || aplChar EQ UTF16_OVERBAR
         || aplChar EQ UTF16_ALPHA          // Only as one and only char
         || aplChar EQ UTF16_OMEGA          // ...
         || IsSysName (&aplChar));          // Valid as 1st char only
} // End IzitNameChar


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
////LRESULT      lResult;       // Result from calling original handler
    LPUNDO_BUF   lpUndoNxt,     // Ptr to next available slot in the Undo Buffer
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
    LRESULT      lResult;       // Temporary result
    BOOL         bSelection,    // TRUE iff there's a selection
                 bDrawLineNums = FALSE; // TRUE iff the ending code should draw the
                                //   line #s after calling the original handler
    HANDLE       hGlbClip;      // Handle to the clipboard
    LPWCHAR      lpMemClip;     // Memory ptr
    UINT         ksShft,        // TRUE iff VK_CONTROL is pressed
                 ksCtrl;        // ...      VK_SHIFT   ...
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPWCHAR      lpwszFormat;   // Ptr to formatting save area
    WNDPROC      lpfnOldEditCtrlWndProc; // Ptr to preceding Edit Control window procedure
    LPWCHAR      lpwszTemp;     // Ptr to temporary storage

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the address of the preceding Edit Control window proc
    lpfnOldEditCtrlWndProc = lpMemPTD->lpfnOldEditCtrlWndProc;

    // Get ptr to temporary storage
    lpwszTemp = lpMemPTD->lpwszTemp;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

////LCLODSAPI ("EC: ", hWnd, message, wParam, lParam);
    // Split cases
    switch (message)
    {
        // In the case of right double click, the sequence of messages is
        //   WM_RBUTTONDOWN
        //   WM_RBUTTONUP
        //   WM_CONTEXTMENU
        //   WM_RBUTTONDBLCLK
        //   WM_RBUTTONUP
        //
        // If the user double clicked, we set a timer in the WM_CONTEXTMENU
        //   message handler and wait for it to expire (real WM_CONTEXTMENU)
        //   or WM_RBUTTONDBLCLK occurs in which it's a double click.  Note
        //   the last WM_RBUTTONUP.  If we pass this message on, W will
        //   send another WM_CONTEXTMENU.  To avoid this, we save the current
        //   tick count at the end of WM_RBUTTONDBLCLK.  If WM_RBUTTONUP
        //   occurs too soon, we ignore that message.

        // ***FIXME*** -- If the user right double clicks on a name and the
        //                editor window for the function overlaps the name,
        //                then the last WM_RBUTTONUP message is sent to
        //                the editor window and appears as a WM_CONTEXTMENU.
        //                Maybe use SetCapture ??

#define ID_TIMER        1729
        case WM_CONTEXTMENU:                // hwnd = (HWND) wParam;
                                            // xPos = LOWORD(lParam);
                                            // yPos = HIWORD(lParam);
            // Set a timer waiting for WM_RBUTTONDBLCLK or expiration
            SetTimer (hWnd, ID_TIMER, GetDoubleClickTime () / 4, NULL);

            // Save the context menu's wParam & lParam so we
            //   can retrieve them in WM_TIMER.  The values
            //   in lParam are in screen coordinates and are
            //   translated for multiple monitor support which
            //   saves me the trouble.
            SetPropW (hWnd, L"TIMER.WPARAM", (LPVOID) wParam);
            SetPropW (hWnd, L"TIMER.LPARAM", (LPVOID) lParam);

            return FALSE;

        case WM_TIMER:              // wTimerID = wParam            // Timer identifier
                                    // tmpc = (TIMERPROC *) lParam  // Ptr to timer callback
            if (wParam EQ ID_TIMER)
            {
                // The timer expired, so it's a real WM_CONTEXTMENU
                KillTimer (hWnd, ID_TIMER);

                return CallWindowProcW (lpfnOldEditCtrlWndProc,
                                        hWnd,
                                        WM_CONTEXTMENU,
                                        (WPARAM) GetPropW (hWnd, L"TIMER.WPARAM"),
                                        (LPARAM) GetPropW (hWnd, L"TIMER.LPARAM"));
            } // End IF

            break;

        case WM_RBUTTONDBLCLK:
        {
            UINT       xPos, yPos;      // x- and y-screen coordinates
            LPSYMENTRY lpSymEntry;      // Ptr to the SYMENTRY under the name

            // It's a right double click, so cancel the timer
            KillTimer (hWnd, ID_TIMER);

            // Handle a right double click

            // Get the lParam from the original WM_CONTEXTMENU message
            yPos = HandleToULong (GetPropW (hWnd, L"TIMER.LPARAM"));

            xPos = (short) LOWORD (yPos);
            yPos = (short) HIWORD (yPos);

            // If there's an identifier underneath this double click,
            //   attempt to edit it as a function/variable
            lpSymEntry = (LPSYMENTRY) SendMessageW (hWnd, MYWM_IZITNAME, xPos, yPos);
            if (lpSymEntry
             && (lpSymEntry->stFlags.ObjName EQ OBJNAME_NONE
              || lpSymEntry->stFlags.ObjName EQ OBJNAME_USR)
             && (lpSymEntry->stFlags.UsrDfn
              || !lpSymEntry->stFlags.Value))
            {
                LPAPLCHAR lpMemName;

                // If the name is that of a user defined function/operator
                //   or it is undefined, edit it as a function
                switch (lpSymEntry->stFlags.stNameType)
                {
                    case NAMETYPE_UNK:
                    case NAMETYPE_FN0:
                    case NAMETYPE_FN12:
                    case NAMETYPE_OP1:
                    case NAMETYPE_OP2:
                    case NAMETYPE_OP3:
                        // Lock the memory to get a ptr to it
                        lpMemName = MyGlobalLock (lpSymEntry->stHshEntry->htGlbName);

                        // Lock the memory to get a ptr to it
                        lpMemPTD = MyGlobalLock (hGlbPTD);

                        // Get ptr to formatting save area
                        lpwszFormat = lpMemPTD->lpwszFormat;

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                        // Copy the name (and its trailing zero) to temporary storage
                        //   which won't go away when we unlock the name's global
                        //   memory handle
                        CopyMemory (lpwszFormat, lpMemName, (lstrlenW (lpMemName) + 1) * sizeof (APLCHAR));

                        // We no longer need this ptr
                        MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

                        // Open a Function Editor window
                        CreateFcnWindow (lpwszFormat);

                        break;

                    case NAMETYPE_LST:      // Ignore this name type
                    case NAMETYPE_VAR:      // ...
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End IF

            // In case we get a WM_RBUTTONUP message shortly after this one,
            //   we need a way to recognize it and discard it so W doesn't
            //   think to generate another WM_CONTEXTMENU message.
            SetPropW (hWnd, L"DBLCLK.TICKCOUNT", ULongToHandle (GetTickCount ()));

            break;
        } // End WM_RBUTTONDBLCLK
#undef  ID_TIMER

        case WM_RBUTTONUP:
            // If this message occurs closely on the heels of a WM_RBUTTONDBLCLK,
            //  discard it so W doesn't generate another WM_CONTEXTMENU message.
            if (GetDoubleClickTime () > (GetTickCount () - HandleToULong (GetPropW (hWnd, L"DBLCLK.TICKCOUNT"))))
                return FALSE;           // Discard this message
            break;

        case MYWM_IZITNAME:         // xPos = (UINT) wParam
                                    // yPos = (UINT) lParam
        {
            POINT pt;

            // This message is sent whenever the user right clicks
            //   inside a Function Edit window and the Edit Control
            //   wants to know whether or not to enable the
            //   Localize/Unlocalize menu items, or it occurs
            //   inside a Session Manager window and the user wants
            //   to edit the named object.

            // Translate the xPos & yPos mouse screen coordinates into
            //   client coordinates in the Edit Control window
            pt.x = (UINT) wParam;
            pt.y = (UINT) lParam;
            ScreenToClient (hWnd, &pt);

            // Translate the xPos & yPos client coordinates into
            //   char positions in the text
            uCharPos = (UINT) SendMessageW (hWnd, EM_CHARFROMPOS, 0, MAKELPARAM (pt.x, pt.y));

            // Split out the number of the line with this char
            //   and the char position
            uLineNum = HIWORD (uCharPos);
            uCharPos = LOWORD (uCharPos);

            // Get the length of the line with this char
            uLineLen = (UINT) SendMessageW (hWnd, EM_LINELENGTH, uCharPos, 0);

            // Get the index of the first character in the line
            uLinePos = (UINT) SendMessageW (hWnd, EM_LINEINDEX, uLineNum, 0);

            // Convert uCharPos from origin-0 to origin-Line
            uCharPos -= uLinePos;

            // Tell EM_GETLINE maximum # chars in the buffer
            // The output array is a temporary so we don't have to
            //   worry about overwriting outside the allocated buffer
            ((LPWORD) lpwszTemp)[0] = (WORD) uLineLen;

            // Get the contents of the line
            SendMessageW (hWnd, EM_GETLINE, uLineNum, (LPARAM) lpwszTemp);

            // Check the chars at and to the right of the specified char pos
            uCharPosEnd = uCharPos;
            while (IzitNameChar (lpwszTemp[uCharPosEnd]))
                uCharPosEnd++;

            // Ensure the name is properly terminated
            lpwszTemp[uCharPosEnd] = L'\0';

            // Start one char to the left
            uCharPosBeg = uCharPos - 1;

            // Check the chars to the left of the specified char pos
            if (uCharPos)
            {
                while (IzitNameChar (lpwszTemp[uCharPosBeg]))
                if (uCharPosBeg)
                    uCharPosBeg--;
                else
                    break;
            } // End IF

            // Back up to the next position
            //   as we started one back
            uCharPosBeg++;

            // The name spans [uCharPosBeg, uCharPosEnd)
            // Check the whole name now
            if (ValidName (&lpwszTemp[uCharPosBeg], uCharPosEnd - uCharPosBeg)
             && !IsSysName (&lpwszTemp[uCharPosBeg]))
            {
                STFLAGS    stFlags = {0};       // Symbol Table Flags used to limit the lookup
                LPSYMENTRY lpSymEntry;          // Ptr to SYMENTRY if found

                // Lookup the name in the symbol table
                lpSymEntry = SymTabLookupNameLength (&lpwszTemp[uCharPosBeg],
                                                      uCharPosEnd - uCharPosBeg,
                                                     &stFlags);
                if (lpSymEntry)
                    return (LRESULT) lpSymEntry;

                // Name not found -- append it
                // Lookup in or append to the symbol table
                return (LRESULT) SymTabAppendName_EM (&lpwszTemp[uCharPosBeg], NULL);
            } else
                return (LRESULT) NULL;
        } // End MYWM_IZITNAME

#define fwSizeType  wParam
#define nWidth      (LOWORD (lParam))
#define nHeight     (HIWORD (lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            // If requested to do so, change []PW to track the new width
            if (IzitSM (GetParent (hWnd))   // If Edit Control for SM,
             && OptionFlags.bAdjustPW       // and requested to do so,
             && nWidth NE 0)                // and width is non-zero
            {
                APLUINT aplInteger;

                // Calculate new width in chars
                aplInteger = nWidth / cxAveCharSM;

                // Validate the incoming value
                if (ValidateIntegerTest (&aplInteger,       // Ptr to the integer to test
                                          DEF_MIN_QUADPW,   // Low range value (inclusive)
                                          DEF_MAX_QUADPW,   // High ...
                                          bRangeLimit.PW))  // TRUE iff we're range limiting
                {
                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    // Save as new []PW
                    lpMemPTD->lpSymQuadPW->stData.stInteger = aplInteger;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
                } // End IF
            } // End IF

            break;
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

#define nVirtKey ((int) wParam)
        case WM_KEYDOWN:            // nVirtKey = (int) wParam;     // Virtual-key code
                                    // lKeyData = lParam;           // Key data
            // Skip this is the Menu key is pressed
            if (GetKeyState (VK_MENU) & 0x8000)
                break;

            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Save the key in the parent window's extra bytes
            SetWindowLongW (hWndParent, GWLSF_LASTKEY, nVirtKey);

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
                    PostMessageW (hWndParent, MYWM_KEYDOWN, wParam, lParam);

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
                        uLineNum = (UINT) SendMessageW (hWnd, EM_LINEFROMCHAR, (WPARAM) -1, 0);

                        PostMessageW (hWndParent, MYWM_KEYDOWN, wParam, uLineNum);
                    } // End IF

                    break;

                case 'C':
                    if (!ksCtrl)
                        break;

                    // Fall through to VK_CANCEL

                case VK_CANCEL:
                    PostMessageW (hWndParent, MYWM_KEYDOWN, VK_CANCEL, 0);

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
                    lvkState = GetWindowLongW (hWndParent, GWLSF_VKSTATE);
                    vkState = *(LPVKSTATE) &lvkState;

                    vkState.Ins = !vkState.Ins;

                    // Save in window extra bytes
                    SetWindowLongW (hWndParent, GWLSF_VKSTATE, *(long *) &vkState);

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
                        uLineNum = (UINT) SendMessageW (hWnd, EM_LINEFROMCHAR, (WPARAM) -1, 0);

                        PostMessageW (hWndParent, MYWM_KEYDOWN, wParam, uLineNum);

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
                    uLineNum = (UINT) SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPos, 0);

                    // Less the # of the topmost visible line
                    uLineNum -= (UINT) SendMessageW (hWnd, EM_GETFIRSTVISIBLELINE, 0, 0);

                    rcPaint.top = uLineNum * cyAveCharFE;
                    rcPaint.bottom = rcPaint.top + cyAveCharFE;

                    // Invalidate this line
                    InvalidateRect (hWnd, &rcPaint, FALSE);

                    break;
                } // End VK_RETURN

                case VK_TAB:
                    // Insert a tab -- convert into insert N spaces

                    // Get the indices of the selected text (if any)
                    SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

                    // Get the line # of this char
                    uLineNum = (UINT) SendMessageW (hWnd, EM_LINEFROMCHAR, uCharPosBeg, 0);

                    // Get the char position of the start of the current line
                    uLinePos = (UINT) SendMessageW (hWnd, EM_LINEINDEX, uLineNum, 0);

                    // Get the # spaces to insert
                    uCharPos = uCharPosBeg - uLinePos;
                    uSpaces = (((uCharPos + 1) / TABSTOP) * TABSTOP + TABSTOP) - uCharPos;

                    for (uChar = 0; uChar < uSpaces; uChar++)
                        wChar[uChar] = L' ';

                    wChar[uChar] = L'\0';

                    // Insert/replace the char string
                    InsRepCharStr (hWnd, GWLSF_VKSTATE, wChar);

                    return FALSE;       // We handled the msg
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
                PostMessageW (hWnd, WM_REDO, 0, 0);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-S (Save)
            if (chCharCode EQ 19)
            {
                // Post a request to ourselves to save the function
                PostMessageW (hWnd, MYWM_SAVE_FN, wParam, lParam);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-E (Save and End)
            if (chCharCode EQ 5)
            {
                // Post a request to ourselves to save & close the function
                PostMessageW (hWnd, MYWM_SAVECLOSE_FN, wParam, lParam);

                return FALSE;       // We handled the msg
            } // End IF

            // Check for Ctrl-Q (Close)
            if (chCharCode EQ 17)
            {
                // Post a request to ourselves to close the function
                PostMessageW (hWnd, MYWM_CLOSE_FN, wParam, lParam);

                return FALSE;       // We handled the msg
            } // End IF

////////////// Check for Ctrl-A (SaveAs)        // ***FIXME*** -- Make this work??
////////////if (chCharCode EQ 1)
////////////{
////////////    // Post a request to ourselves to save the function under a different name
////////////    PostMessageW (hWnd, MYWM_SAVE_AS_FN, wParam, lParam);
////////////
////////////    return FALSE;       // We handled the msg
////////////} // End IF

            iChar = chCharCode - ' ';
            if (0 <= iChar
             &&      iChar < ACHARCODES_NROWS)
            {
                // Get the Nrm- char code
                wChar[0] = aCharCodes[iChar].nrm;
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
                    WCHAR wszTemp[1024];    // Ptr to temporary output area

                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    wsprintfW (wszTemp,
                               L"CHAR:  chCharCode = %d, %c",
                               chCharCode,
                               chCharCode);
                    DbgMsgW (wszTemp);

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
             &&      iChar < ACHARCODES_NROWS)
            {
                // Get the Alt- char code
                wChar[0] = aCharCodes[iChar].alt;
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
                    WCHAR wszTemp[1024];    // Ptr to temporary output area

                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    wsprintfW (wszTemp,
                               L"SYSCHAR:  chCharCode = %d, %c",
                               chCharCode,
                               chCharCode);
                    DbgMsgW (wszTemp);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
#endif
                } // End IF/ELSE
            } else
            {
#ifdef DEBUG
                WCHAR wszTemp[1024];    // Ptr to temporary output area

                // Lock the memory to get a ptr to it
                lpMemPTD = MyGlobalLock (hGlbPTD);

                wsprintfW (wszTemp,
                           L"SYSCHAR:  chCharCode = %d, %c",
                           chCharCode,
                           chCharCode);
                DbgMsgW (wszTemp);

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
            (HANDLE_PTR) lpUndoNxt = GetWindowLongPtrW (hWndParent, GWLSF_UNDO_NXT);

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
                        lvkState = GetWindowLongW (hWndParent, GWLSF_VKSTATE);
                        vkState = *(LPVKSTATE) &lvkState;

                        vkState.Ins = !vkState.Ins;

                        // Save in window extra bytes
                        SetWindowLongW (hWndParent, GWLSF_VKSTATE, *(long *) &vkState);

                        break;

                    defstop
                        break;
                } // End SWITCH
            } while (lpUndoNxt->Group NE UNDO_NOGROUP
                  && lpUndoNxt->Group EQ lpUndoNxt[-1].Group);

            // Save the new Undo Next ptr
            SetWindowLongPtrW (hWndParent, GWLSF_UNDO_NXT, (__int3264) (LONG_PTR) lpUndoNxt);

            // Draw the line #s
            DrawLineNumsFE (hWnd);

            return TRUE;

        case EM_CANUNDO:

            // Get the ptrs to the next available slot in our Undo Buffer
            (HANDLE_PTR) lpUndoNxt = GetWindowLongPtrW (hWnd, GWLSF_UNDO_NXT);
            (HANDLE_PTR) lpUndoBeg = GetWindowLongPtrW (hWnd, GWLSF_UNDO_BEG);

            return (lpUndoBeg NE lpUndoNxt);

        case WM_REDO:
            break;

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
            uGroupIndex = 1 + GetWindowLongW (hWndParent, GWLSF_UNDO_GRP);
            SetWindowLongW (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

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
            // Tell the ending code to draw the line #s afterwards
            bDrawLineNums = TRUE;

            break;

        case WM_PASTE:              // bPasteAPL = (BOOL) wParam
                                    // 0 = lParam
            // If we've not already laundered the input, do so now
            if (wParam)
                // Run through the "Normal" processing
                PasteAPLChars (hWnd, UNITRANS_NORMAL);
            else
                // Insert text from the clipboard, deleting selected text (if any)
                PasteAPLChars (hWnd, OptionFlags.uDefaultPaste);

            // Undo deletes the inserted (pasted) chars
            //   and insert the deleted chars (if any)

            // Get the handle of the parent window
            hWndParent = GetParent (hWnd);

            // Get the indices of the selected text (if any)
            SendMessageW (hWnd, EM_GETSEL, (WPARAM) &uCharPosBeg, (LPARAM) &uCharPosEnd);

            // Note if there's a selection
            bSelection = uCharPosBeg NE uCharPosEnd;

            // Get the next group index, and save it back
            uGroupIndex = 1 + GetWindowLongW (hWndParent, GWLSF_UNDO_GRP);
            SetWindowLongW (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

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
            hGlbClip = GetClipboardData (CF_PRIVATEFIRST);
            if (!hGlbClip)
                hGlbClip = GetClipboardData (CF_UNICODETEXT);

            // If there's data in the clipboard, ...
            if (hGlbClip)
            {
                // Lock the memory to get a ptr to it
                // Note we can't use MyGlobalLock/Unlock as the lock count
                //   is not modified for a clipboard (non-owned) handle
                lpMemClip = GlobalLock (hGlbClip); Assert (lpMemClip NE NULL);

                // The ending position is the beginning position plus the string length
                uCharPosEnd = uCharPosBeg + lstrlenW (lpMemClip);

                // We no longer need this ptr
                GlobalUnlock (hGlbClip); lpMemClip = NULL;

                // Append the undo action (delete inserted (pasted) chars)
                AppendUndo (hWndParent,                     // SM/FE Window handle
                            GWLSF_UNDO_NXT,                 // Offset in hWnd extra bytes of lpUndoNxt
                            undoDel,                        // Action
                            uCharPosBeg,                    // Beginning char position
                            uCharPosEnd,                    // Ending    ...
                            uGroupIndex,                    // Group index
                            0);                             // Character
            } // End IF

            // We're done with the clipboard and its handle
            CloseClipboard (); hGlbClip = NULL;

            // Tell the ending code to draw the line #s afterwards
            bDrawLineNums = TRUE;

            break;

        case MYWM_PASTE_APL:
            PasteAPLChars (hWnd, (UINT) wParam);
            PostMessageW (hWnd, WM_PASTE, TRUE, 0);

            return FALSE;           // We handled the msg

        case MYWM_SAVE_FN:
        case MYWM_SAVECLOSE_FN:
        case MYWM_CLOSE_FN:
        case MYWM_SAVE_AS_FN:
            PostMessageW (GetParent (hWnd), message, wParam, lParam);

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
                    SendMessageW (hWnd, WM_PASTE, FALSE, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_APLWIN:
                    SendMessageW (hWnd, MYWM_PASTE_APL, UNITRANS_APLWIN, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_APL2:
                    SendMessageW (hWnd, MYWM_PASTE_APL, UNITRANS_APL2, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_ISO:
                    SendMessageW (hWnd, MYWM_PASTE_APL, UNITRANS_ISO, 0);

                    return FALSE;   // We handled the msg

                case IDM_PASTE_PC3270:
                    SendMessageW (hWnd, MYWM_PASTE_APL, UNITRANS_PC3270, 0);

                    return FALSE;   // We handled the msg

                case IDM_DELETE:
                    SendMessageW (hWnd, WM_CLEAR, 0, 0);

                    return FALSE;   // We handled the msg

                case IDM_SELECTALL:
                    SendMessageW (hWnd, MYWM_SELECTALL, 0, (LPARAM) -1);

                    return FALSE;   // We handled the msg
            } // End SWITCH

            break;

        case WM_PAINT:              // hdc = (HDC) wParam; // the device context to draw in
            // Tell the ending code to draw the line #s afterwards
            bDrawLineNums = TRUE;

            // If this tab isn't active, ignore the msg
            if (!IsCurTabActive ())
            {
                PAINTSTRUCT ps;

                // If there's no incoming HDC, ...
                if (((HDC) wParam) EQ NULL)
                {
                    // Tell 'em we're done
                    BeginPaint (hWnd, &ps);
                    EndPaint   (hWnd, &ps);
                } // End IF

                return FALSE;       // We ignored the msg
            } // End IF

            break;

        case WM_DESTROY:
            // Remove all saved window properties
            EnumProps (hWnd, EnumCallbackRemoveProp);

            break;
    } // End SWITCH

////LCLODSAPI ("FEC:", hWnd, message, wParam, lParam);
    lResult = CallWindowProcW (lpfnOldEditCtrlWndProc,
                               hWnd,
                               message,
                               wParam,
                               lParam);     // Pass on down the line
    if (bDrawLineNums)
        // Draw the line #s
        DrawLineNumsFE (hWnd);
    return lResult;
} // End LclEditCtrlWndProc


//***************************************************************************
//  $CopyGlbMemory
//
//  Copy global memory from one handle to another
//***************************************************************************

HGLOBAL CopyGlbMemory
    (HGLOBAL hGlbSrc)

{
    HGLOBAL hGlbDst;        // The result global memory handle
    SIZE_T  dwSize;         // The size of the source
    LPVOID  lpMemSrc,       // Ptr to source global memory
            lpMemDst;       // Ptr to result ...

    // Check for bad handle
    if (!IsGlbPtr (hGlbSrc))
        return NULL;

    // Get the size of the global memory object
    dwSize = MyGlobalSize (hGlbSrc);

    // Allocate space for the result
    // Note we do not use MyGlobalAlloc here as the global memory handle
    //   is to be placed onto the clipboard at which point the system
    //   will own the handle
    hGlbDst = GlobalAlloc (GHND | GMEM_DDESHARE, dwSize);
    if (hGlbDst)
    {
        // We don't use MyGlobalLock/Unlock on the source
        //   as we might not own the handle.

        // Lock both memory blocks
        lpMemDst = GlobalLock (hGlbDst);
        lpMemSrc = GlobalLock (hGlbSrc);

        // Copy source to destin
        CopyMemory (lpMemDst, lpMemSrc, dwSize);

        // We no longer need these ptrs
        GlobalUnlock (hGlbDst); lpMemDst = NULL;
        GlobalUnlock (hGlbSrc); lpMemSrc = NULL;
    } // End IF

    return hGlbDst;
} // End CopyGlbMemory


//***************************************************************************
//  $PasteAPLChars
//
//  Paste APL chars from another APL system
//***************************************************************************

void PasteAPLChars
    (HWND      hWndEC,              // Window handle of the Edit Control
     UNI_TRANS uIndex)              // UNI_TRANS index

{
    SIZE_T     dwSize;
    HGLOBAL    hGlbFmts = NULL,     // Clipboard formats global memory handle
               hGlbClip = NULL,
               hGlbText = NULL;
    LPCLIPFMTS lpMemFmts = NULL;    // Ptr to clipboard format global memory
    LPWCHAR    lpMemClip = NULL,    // Ptr to
               lpMemText = NULL;    // Ptr to
    UINT       uTran,               // Loop counter
               uText,               // Loop counter
               uFmt,                // Loop counter
               uFmtNum,             // Clipbaord format #
               uCount;              // # formats on the clipboard

    // Open the clipboard so we can read from it
    OpenClipboard (hWndEC);

    // Get the # formats on the clipboard
    uCount = CountClipboardFormats ();

    // If there are no formats, ...
    if (uCount EQ 0)
        goto NORMAL_EXIT;

    // Allocate memory to hold the format # and the matching handle
    hGlbFmts = MyGlobalAlloc (GHND, uCount * sizeof (CLIPFMTS));
    if (!hGlbFmts)
    {
        MessageBox (hWndEC,
                    "Unable to allocate memory for the clipboard formats",
                    lpszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemFmts = MyGlobalLock (hGlbFmts);

    // Enumerate the clipboard formats and save the format # and handle
    for (uFmtNum = uFmt = 0; uFmt < uCount; uFmt++)
    {
        uFmtNum = EnumClipboardFormats (uFmtNum);
        if (uFmtNum EQ CF_TEXT
         && uFmtNum EQ CF_OEMTEXT
         && uFmtNum EQ CF_UNICODETEXT
         && uFmtNum EQ CF_DSPTEXT)
        {
            // Save the format #
            lpMemFmts[uFmt].uFmtNum = uFmtNum;

            // Get a handle to the clipboard data
            //   and make a copy of the data
            lpMemFmts[uFmt].hGlbFmt =
              CopyGlbMemory (GetClipboardData (uFmtNum));
        } // End IF
    } // End FOR

    // Get a handle to the clipboard data for CF_PRIVATEFIRST
    hGlbClip = GetClipboardData (CF_PRIVATEFIRST);
    if (hGlbClip EQ NULL)
        // Get a handle to the clipboard data for CF_UNICODETEXT
        hGlbClip = GetClipboardData (CF_UNICODETEXT);

    if (hGlbClip)
    {
        // Get the memory size
        dwSize = GlobalSize (hGlbClip);

        // Allocate space for the new object
        // Note that we can't use MyGlobalAlloc or DbgGlobalAlloc
        //   because after we pass this handle to the clipboard
        //   we won't own it anymore.
        hGlbText = GlobalAlloc (GHND | GMEM_DDESHARE, dwSize);
        if (!hGlbText)
        {
            MessageBox (hWndEC,
                        "Unable to allocate memory for the copy of CF_UNICODETEXT/CF_PRIVATEFIRST format",
                        lpszAppName,
                        MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            goto ERROR_EXIT;
        } else
        {
            // Lock the memory to get a ptr to it
            // Note we can't use MyGlobalLock/Unlock as the lock count
            //   is not modified for a clipboard (non-owned) handle
            lpMemClip = GlobalLock (hGlbClip); Assert (lpMemClip NE NULL);
            lpMemText = GlobalLock (hGlbText); Assert (lpMemText NE NULL);

            // Make a copy of the clipboard data
            CopyMemory (lpMemText, lpMemClip, dwSize);

            // We no longer need this ptr
            GlobalUnlock (hGlbClip); lpMemClip = NULL;

            // Convert dwSize to # elements
            dwSize /= sizeof (WCHAR);

            // Translate the other APL charset to the NARS charset
            for (uText = 0; uText < dwSize; uText++, lpMemText++)
            if (*lpMemText)
            {
                for (uTran = 0; uTran < UNITRANS_NROWS; uTran++)
                if (*lpMemText EQ uniTransTab[uTran][uIndex])
                {
                    WCHAR wcTmp;

                    // Translate the external char to NARS
                    wcTmp = uniTransTab[uTran][UNITRANS_NARS];

                    // If the translation is not from NARS and
                    //   the char is UTF16_A_ through UTF16_Z_,
                    //   optionally translate it to 'a-z'
                    if (OptionFlags.bUnderbarToLowercase
                     && uTran NE UNITRANS_NORMAL
                     && UTF16_A_ <= wcTmp
                     &&             wcTmp <= UTF16_Z_)
                        wcTmp = L'a' + (wcTmp - UTF16_A_);
                    *lpMemText = wcTmp;

                    break;      // out of the innermost loop
                } // End FOR/IF/...
            } // End FOR/IF

            // We no longer need this ptr
            GlobalUnlock (hGlbText); lpMemText = NULL;

            // Empty the clipboard
            EmptyClipboard (); hGlbClip = NULL;

            // Place all the other formats on the clipboard first
            for (uFmt = 0; uFmt < uCount; uFmt++)
            if (lpMemFmts[uFmt].uFmtNum NE CF_PRIVATEFIRST
             && lpMemFmts[uFmt].hGlbFmt NE NULL)
                SetClipboardData (lpMemFmts[uFmt].uFmtNum, lpMemFmts[uFmt].hGlbFmt);

            // Place the changed data onto the clipboard
            SetClipboardData (CF_PRIVATEFIRST, hGlbText); hGlbText = NULL;
        } // End IF/ELSE
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbFmts); lpMemFmts = NULL;

    // We no longer need this storage
    MyGlobalFree (hGlbFmts); hGlbFmts = NULL;
NORMAL_EXIT:
    // We're done with the clipboard and its handle
    CloseClipboard (); hGlbClip = NULL;
ERROR_EXIT:
    if (hGlbText && lpMemText)
    {
        // We no longer need this ptr
        GlobalUnlock (hGlbText); lpMemText = NULL;
    } // End IF

    if (hGlbClip && lpMemClip)
    {
        // We no longer need this ptr
        GlobalUnlock (hGlbClip); lpMemClip = NULL;
    } // End IF

    // We're done with the clipboard
    CloseClipboard ();

    if (hGlbFmts)
    {
        // Loop through the formats freeing them as appropriate
        for (uFmt = 0; uFmt < uCount; uFmt++)
        switch (lpMemFmts[uFmt].uFmtNum)
        {
            case 0:             // Ignore empty slots
                break;

            case CF_DSPENHMETAFILE:
            case CF_DSPMETAFILEPICT:
            case CF_ENHMETAFILE:
            case CF_METAFILEPICT:
                DeleteMetaFile (lpMemFmts[uFmt].hGlbFmt);

                break;

            case CF_BITMAP:
            case CF_DSPBITMAP:
            case CF_PALETTE:
                DeleteObject (lpMemFmts[uFmt].hGlbFmt);

                break;

            case CF_DIB:
            case CF_DSPTEXT:
            case CF_OEMTEXT:
            case CF_TEXT:
            case CF_UNICODETEXT:
            default:
                GlobalFree (lpMemFmts[uFmt].hGlbFmt);

                break;
        } // End FOR/SWITCH

        // We no longer need this ptr
        MyGlobalUnlock (hGlbFmts); lpMemFmts = NULL;

        // We no longer need this storage
        MyGlobalFree (hGlbFmts); hGlbFmts = NULL;
    } // End IF
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
    uLineCnt = (UINT) SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

    // Get the initial char pos of the last line
    uLinePos = (UINT) SendMessageW (hWnd, EM_LINEINDEX, uLineCnt - 1, 0);

    // Get the length of the last line
    uLineLen = (UINT) SendMessageW (hWnd, EM_LINELENGTH, uLinePos, 0);

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
    (HWND hWndEC,               // Window handle of the Edit Control
     UINT uCharPos)             // Char position (-1 = under the caret)

{
    POINT        ptCaret;       // The caret position
    UINT         uLineNum,      // The line #
                 uLinePos,      // The line position (start of line)
                 uLineLen,      // The line length
                 uLineOff;      // The line offset
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPWCHAR      lpwszTemp;     // Ptr to temporary storage

    if (uCharPos EQ -1)
    {
        // Get the caret position in client coords
        GetCaretPos (&ptCaret);

        // Get the position of the char under the caret
        //   and the line #
        uCharPos = (UINT) SendMessageW (hWndEC, EM_CHARFROMPOS, 0, MAKELPARAM (ptCaret.x, ptCaret.y));
        uLineNum = HIWORD (uCharPos);
        uCharPos = LOWORD (uCharPos);
    } else
        // Get the line # of the char position
        uLineNum = (UINT) SendMessageW (hWndEC, EM_LINEFROMCHAR, uCharPos, 0);

    // Get the char position of the start of the current line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the length of the line
    uLineLen = (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // If the char position is at the end of the line, return '\r';
    //   if it's beyond the end of the line, return '\n'
    uLineOff = uCharPos - uLinePos;
    if (uLineOff >= uLineLen)
        return (uLineOff EQ uLineLen) ? L'\r' : L'\n';

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get ptr to temporary storage
    lpwszTemp = lpMemPTD->lpwszTemp;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Tell EM_GETLINE maximum # chars in the buffer
    // The output array is a temporary so we don't have to
    //   worry about overwriting outside the allocated buffer
    ((LPWORD) lpwszTemp)[0] = (WORD) uLineLen;

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
    LPUNDO_BUF lpUndoNxt;   // Ptr to next available slot in the Undo Buffer

    // Get the ptr to the next available slot in our Undo Buffer
    (HANDLE_PTR) lpUndoNxt = GetWindowLongPtrW (hWnd, GWLxx_UNDO_NXT);

    // Save the undo entry
    lpUndoNxt->Action     = Action;
    lpUndoNxt->CharPosBeg = CharPosBeg;
    lpUndoNxt->CharPosEnd = CharPosEnd;
    lpUndoNxt->Group      = Group;
    lpUndoNxt->Char       = Char;

    // Skip over this entry
    lpUndoNxt++;

    // Save the incremented ptr in window extra bytes
    SetWindowLongPtrW (hWnd, GWLSF_UNDO_NXT, (__int3264) (LONG_PTR) lpUndoNxt);
    SetWindowLongPtrW (hWnd, GWLSF_UNDO_LST, (__int3264) (LONG_PTR) lpUndoNxt);
} // End AppendUndo


//***************************************************************************
//  $InsRepCharStr
//
//  Insert or replace a char string in an edit control
//***************************************************************************

void InsRepCharStr
    (HWND    hWnd,          // EC window handle
     UINT    uGWL,          // The GetWindowLongPtrW offset
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
    lvkState = GetWindowLongW (hWndParent, uGWL);
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
        uLineLen = (UINT) SendMessageW (hWnd, EM_LINELENGTH, uCharPosBeg, 0);

        // Get the char position of the start of the current line
        uLinePos = (UINT) SendMessageW (hWnd, EM_LINEINDEX, (WPARAM) -1, 0);

        // Get the next group index, and save it back
        uGroupIndex = 1 + GetWindowLongW (hWndParent, GWLSF_UNDO_GRP);
        SetWindowLongW (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

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
        uGroupIndex = 1 + GetWindowLongW (hWndParent, GWLSF_UNDO_GRP);
        SetWindowLongW (hWndParent, GWLSF_UNDO_GRP, uGroupIndex);

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
    WCHAR wszClassName[32];

    GetClassNameW (hWnd, wszClassName, (sizeof (wszClassName) / sizeof (wszClassName[0])) - 1);

    return (lstrcmpW (wszClassName, LFEWNDCLASS) EQ 0);
} // End IzitFE


//***************************************************************************
//  $IzitSM
//
//  Is the window SMWNDCLASS?
//***************************************************************************

BOOL IzitSM
    (HWND hWnd)

{
    WCHAR wszClassName[32];

    GetClassNameW (hWnd, wszClassName, (sizeof (wszClassName) / sizeof (wszClassName[0])) - 1);

    return (lstrcmpW (wszClassName, LSMWNDCLASS) EQ 0);
} // End IzitSM


//***************************************************************************
//  $DrawLineNumsFE
//
//  Draw the line #s in an FE window
//***************************************************************************

void DrawLineNumsFE
    (HWND hWndEC)           // Edit Control window handle

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
    hWndParent = GetParent (hWndEC);

    // Ensure this is the function editor
    if (!IzitFE (hWndParent))
        return;

    // Get a device context
    hDC = MyGetDC (hWndEC);

    // Set our DC attributes
    SetAttrs (hDC, hFontFE, crLineNum, COLOR_WHITE);

    // Get the # lines in the text
    uLineCnt = (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

    // Get the # of the topmost visible line
    uLineTop = (UINT) SendMessageW (hWndEC, EM_GETFIRSTVISIBLELINE, 0, 0);

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
                 | DT_CALCRECT
                 | DT_NOPREFIX);
        // Move the rectangle down
        rcPaint.top    += uCnt * cyAveCharFE;
        rcPaint.bottom += uCnt * cyAveCharFE;

        // Draw the line #s
        DrawTextW (hDC,
                   wszLineNum,
                   uLen,
                   &rcPaint,
                   0
                 | DT_LEFT
                 | DT_NOPREFIX);
    } // End FOR

    // Because we might have deleted some lines whose
    //   #s we need to clear, and we have disabled the
    //   WM_ERASEBKGND message (so as to reduce screen flicker),
    //   we need to fill the bottom of the page with blanks

    // Get the client rectangle
    GetClientRect (hWndEC, &rcClient);

    // Set to the same top as the next line #
    rcClient.top = rcPaint.top + cyAveCharFE;

    // Pour on the white out
    FillRect (hDC, &rcClient, (HBRUSH) (HANDLE_PTR) GetClassLongPtrW (hWndEC, GCL_HBRBACKGROUND));

    // We no longer need this DC
    MyReleaseDC (hWndEC, hDC); hDC = NULL;
} // End DrawLineNumsFE


//***************************************************************************
//  $QueryCloseFE
//
//  Return a Boolean result on whether we can close
//***************************************************************************

BOOL QueryCloseFE
    (HWND hWndFE)           // Handle for FE window

{
    // If the text hasn't changed, continue with close
    if (!GetWindowLongW (hWndFE, GWLSF_CHANGED))
        return TRUE;

    // Ask the user what to do
    switch (MessageBox (hWndFE, szCloseMessage, lpszAppName, MB_YESNOCANCEL | MB_ICONQUESTION))
    {
        case IDYES:         // Save the function
            return SaveFunction (hWndFE);

        case IDNO:          // Do not save the function
            return TRUE;

        case IDCANCEL:      // Not OK to close
        default:            // Problem with MessageBox
            return FALSE;
    } // End SWITCH
} // End QueryClose


//***************************************************************************
//  $ErrorHandler
//
//  Error handler for calls to Tokenize_EM when parsing
//    a function header and its lines.
//***************************************************************************

void ErrorHandler
    (LPWCHAR lpwszMsg,          // Ptr to error message text
     LPWCHAR lpwszLine,         // Ptr to the line which generated the error
     UINT    uCaret,            // Position of caret (origin-0)
     HWND    hWndEC)            // Window handle to the Edit Control

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Save in global for later reference
    lpMemPTD->lpwszErrorMessage = lpwszMsg;
    lpMemPTD->uCaret = uCaret;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End ErrorHandler


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
    MEMVIRTSTR  lclMemVirtStr[1] = {0};// Room for one GuardAlloc

    Assert (IzitFE (hWndFE));

    // Get the handle to the edit control
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndFE, GWLSF_HWNDEC);

    // Tokenize the line
    hGlbTknHdr =
      Tokenize_EM (lpaplChar,               // The line to tokenize (not necessarily zero-terminated)
                   lstrlenW (lpaplChar),    // The length of the above line
                   NULL,                    // Window handle for Edit Control (may be NULL if lpErrHandFn is NULL)
                   NULL);                   // Ptr to error handling function (may be NULL)
    if (!hGlbTknHdr)
        goto ERROR_EXIT;

    // Allocate virtual memory for the Variable Strand accumulator
#ifdef DEBUG
    lclMemVirtStr[0].lpText   = "fhLocalvars.lpYYStrandStart in <ParseFunctionName>";
#endif
    lclMemVirtStr[0].IncrSize = DEF_STRAND_INCRSIZE * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].MaxSize  = DEF_STRAND_MAXSIZE  * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].IniAddr  = (LPUCHAR)
    fhLocalVars.lpYYStrandStart =
      GuardAlloc (NULL,       // Any address
                  lclMemVirtStr[0].MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lclMemVirtStr[0].IniAddr)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsgW (L"ParseFunctionName:  VirtualAlloc for <fhLocalVars.lpYYStrandStart> failed");

        goto ERROR_EXIT;        // Mark as failed
    } // End IF

    // Link this struc into the chain
    LinkMVS (&lclMemVirtStr[0]);

    // Commit the intial size
    MyVirtualAlloc (lclMemVirtStr[0].IniAddr,
                    DEF_STRAND_INITSIZE * sizeof (PL_YYSTYPE),
                    MEM_COMMIT,
                    PAGE_READWRITE);
    // Parse the header
    if (ParseHeader (hWndEC, hGlbTknHdr, &fhLocalVars, FALSE)
     && fhLocalVars.lpYYFcnName)
        // Get the Name's symbol table entry
        lpSymName = fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym;
    goto NORMAL_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // If we allocated virtual storage, ...
    if (lclMemVirtStr[0].IniAddr)
    {
        // Free the virtual storage
        MyVirtualFree (lclMemVirtStr[0].IniAddr, 0, MEM_RELEASE); lclMemVirtStr[0].IniAddr = NULL;
        fhLocalVars.lpYYStrandStart = NULL;

        // Unlink this entry from the chain
        UnlinkMVS (&lclMemVirtStr[0]);
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
////     DbgBrk ();          // ***FINISHME*** -- Save As Function
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
    BOOL bRet;                  // TRUE iff we're to close

    Assert (IzitFE (hWndFE));

    // If it's OK to close the window, do so, else ignore
    bRet = QueryCloseFE (hWndFE);
    if (bRet)
        // Close the Function Editor window
        SendMessageW (GetParent (hWndFE), WM_MDIDESTROY, (WPARAM) hWndFE, 0);
    return bRet;
} // End CloseFunction


//***************************************************************************
//  $ActivateMDIMenu
//
//  Activate an MDI window menu
//***************************************************************************

void ActivateMDIMenu
    (HMENU hMenuFrame,
     HMENU hMenuWindow)

{
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    SendMessageW (lpMemPTD->hWndMC,
                  WM_MDISETMENU,
                  GET_WM_MDISETMENU_MPS (hMenuFrame, hMenuWindow));
    DrawMenuBar (hWndMF);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End ActivateMDIMenu


//***************************************************************************
//  End of File: editfcn.c
//***************************************************************************
