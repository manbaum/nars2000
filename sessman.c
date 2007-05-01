//***************************************************************************
//  NARS2000 -- Session Manager
//***************************************************************************

#define STRICT
#define _WIN32_WINNT  0x0500
#include <windows.h>
#include <colors.h>

#include "main.h"
#include "resdebug.h"
#include "resource.h"
#include "externs.h"
#include "editctrl.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

/*

The Session Manager (SM) window consists of an Edit Control which
holds the APL statements of the session.

When the cursor moves to a line, the contents of the current line
are copied to <lpwszCurLine>.

If the user edits the line:
    * The edit changes are saved in the Edit Control.

In any case,
    * If the user hits Enter, the contents of the current
      line in the Edit Control are copied to the last line
      in the Edit Control, the contents of <lpwszCurLine>
      replace the current line in the Edit Control, and the
      last line in the Edit Control is executed.

 */

//// COLORREF crTextColor = DEF_TEXT_FG_COLOR,
////          crBkColor   = DEF_TEXT_BG_COLOR;

// Default six-space indent
WCHAR wszIndent[DEF_INDENT + 1] = {L' ',L' ',L' ',L' ',L' ',L' ',L'\0'};

////LPTOKEN lptkStackBase;          // Ptr to base of token stack used in parsing


//***************************************************************************
//  SetAttrs
//
//  Set attributes for a DC
//***************************************************************************

void SetAttrs
    (HDC      hDC,
     HFONT    hFont,
     COLORREF crfg,
     COLORREF crbk)

{
    // Set the
    SetMapMode (hDC, MM_TEXT);

    // Select the font into the DC
    if (hFont)
        SelectObject (hDC, hFont);

    // Set the color of the foreground text
    SetTextColor (hDC, crfg);

    // Set the color of the background text
    SetBkColor   (hDC, crbk);
} // End SetAttrs


//***************************************************************************
//  AppendLine
//
//  Append lpwszLine to the history buffer
//***************************************************************************

void AppendLine
    (LPWCHAR lpwszLine,
     BOOL    bLineCont,
     BOOL    bEndingCR)

{
    HWND hWndEC;

    // Get the handle to the edit control
    hWndEC = (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC);

    // Move the caret to the end of the buffer
    MoveCaretEOB (hWndEC);

    // Scroll the caret into view
    SendMessageW (hWndEC, EM_SCROLLCARET, 0, 0);

    // Replace the selection (none) with the new line
    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) lpwszLine);

    if (bEndingCR)
    {
        // Replace the selection (none) with "\r\n"
        SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) L"\r\n");
    } // End IF
} // End AppendLine


//***************************************************************************
//  ReplaceLine
//
//  Replace lpwszCurLine in the history buffer
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReplaceLine"
#else
#define APPEND_NAME
#endif

void ReplaceLine
    (LPWCHAR lpwszLine,
     UINT    uLineNum)

{
    HWND hWndEC;
    UINT uLinePos,
         uLineLen;

    // Get the handle to the edit control
    hWndEC = (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC);

    // Get the line position of the given line
    uLinePos = SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the length of the line
    uLineLen = SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Set the selection to this line
    SendMessageW (hWndEC, EM_SETSEL, uLinePos, uLinePos + uLineLen);

    // Replace the selection with the given line
    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) lpwszLine);
} // End ReplaceLine
#undef  APPEND_NAME


//***************************************************************************
//  IzitLastLine
//
//  Return TRUE iff the cursor is on the last line
//***************************************************************************

BOOL IzitLastLine
    (HWND hWndEC)           // Window handle of the Edit Control

{
    UINT uLineCnt,
         uLinePos,
         uLineLen,
         uCharPos;

    // Get the # lines in the text
    uLineCnt = SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

    // Get the line position of the last line
    uLinePos = SendMessageW (hWndEC, EM_LINEINDEX, (WPARAM) (uLineCnt - 1), 0);

    // Get the length of the line
    uLineLen = SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWndEC);

    return (uLinePos <= uCharPos) && (uCharPos <= (uLinePos + uLineLen));
} // End IzitlastLine


//// //***************************************************************************
//// //  DrawLineCont
//// //
//// //  Draw a line continuation char
//// //***************************************************************************
////
//// void DrawLineCont
////     (HDC hDC,
////      int iLineNum)
////
//// {
////     DrawBitmap (hDC,
////                 hBitMapLineCont,
////                 0,
////                 (iLineNum * cyAveCharSM)
////               + (cyAveCharSM - bmLineCont.bmHeight) / 2   // Vertically centered
////                );
//// } // End DrawLineCont


//// //***************************************************************************
//// //  DrawBitmap
//// //
//// //  Draw a bitmap
//// //***************************************************************************
////
//// void DrawBitmap
////     (HDC     hDC,
////      HBITMAP hBitmap,
////      UINT    xDstOrg,
////      UINT    yDstOrg) // Destin bit origin (upper left corner)
////
//// {
////     BITMAP  bm;
////     HDC     hDCMem;
////     POINT   ptSize, ptOrg;
////     HBITMAP hBitmapMem, hBitmapOld;
////
////     // Get the size of the bitmap
////     GetObject (hBitmap, sizeof (BITMAP), (LPSTR) &bm);
////
////     // Create a compatible DC and bitmap
////     hDCMem = MyCreateCompatibleDC (hDC);    // Get device context handle
//// #ifdef USE_COPYIMAGE
////     hBitmapMem = CopyImage (hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
////     hBitmapOld = SelectObject (hDCMem, hBitmapMem);
//// #else
////     // To avoid screen flicker, we use a temporary DC
////     hBitmapMem = MyCreateCompatibleBitmap (hDC,
////                                            bm.bmWidth,
////                                            bm.bmHeight);
////     hBitmapOld = SelectObject (hDCMem, hBitmapMem);
////
////     {
////         HDC hDCTmp;
////         HBITMAP hBitmapTmp;
////
////         // Create a temporary compatible DC
////         // and select our bitmap into it
////         hDCTmp = MyCreateCompatibleDC (hDC);
////         hBitmapTmp = SelectObject (hDCTmp, hBitmap);
////
////         // Copy the original bitmap from the temporary DC to the memory DC
////         BitBlt (hDCMem,
////                 0,
////                 0,
////                 bm.bmWidth,
////                 bm.bmHeight,
////                 hDCTmp,
////                 0,
////                 0,
////                 SRCCOPY);
////         SelectObject (hDCTmp, hBitmapTmp);
////         MyDeleteDC (hDCTmp); hDCTmp = NULL;
////     }
//// #endif
////     SetMapMode (hDCMem, GetMapMode (hDC));  // Set the mapping mode
////
////     // Convert the bitmap size from device units to logical units
////     ptSize.x = bm.bmWidth;
////     ptSize.y = bm.bmHeight;
////     DPtoLP (hDC, &ptSize, 1);
////
////     ptOrg.x = ptOrg.y = 0;
////     DPtoLP (hDCMem, &ptOrg, 1);
////
////     // Copy the memory DC to the screen DC
////     BitBlt (hDC,
////             xDstOrg, yDstOrg,
////             ptSize.x, ptSize.y,
////             hDCMem,
////             ptOrg.x, ptOrg.y,
////             SRCCOPY);
////     // Put the old one in place before we delete the DC
////     //   or we'll delete the new bitmap when we delete the DC.
////     SelectObject (hDCMem, hBitmapOld);
////
////     // Free resources
////     MyDeleteObject (hBitmapMem); hBitmapMem = NULL;
////     MyDeleteDC (hDCMem); hDCMem = NULL;
//// } // End DrawBitmap ()


//***************************************************************************
//  strchrW
//
//  The wide form of <strchr>
//***************************************************************************

LPWCHAR strchrW
    (LPWCHAR wp,
     WCHAR   wch)

{

    for (; *wp; wp++)
    if (wch EQ *wp)
        return wp;

    return NULL;
} // End strchrW


//***************************************************************************
//  MoveCaretEOB
//
//  Move the caret in an Edit Control to the end of the buffer
//***************************************************************************

void MoveCaretEOB
    (HWND hWnd)         // Window handle of Edit Control

{
    UINT uLineCnt,
         uLinePos,
         uLineLen,
         uCharPos;

    // Get the # lines in the text
    uLineCnt = SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

    // Get the initial char pos of the last line
    uLinePos = SendMessageW (hWnd, EM_LINEINDEX, uLineCnt - 1, 0);

    // Get the length of the last line
    uLineLen = SendMessageW (hWnd, EM_LINELENGTH, uLinePos, 0);

    // Add to get char pos
    uCharPos = uLinePos + uLineLen;

    // Set the caret to the End-of-Buffer
    SendMessageW (hWnd, EM_SETSEL, uCharPos, uCharPos);
} // End MoveCaretEOB


//// //***************************************************************************
//// //  MyCreateCaret
//// //
//// //  Create a caret
//// //***************************************************************************
////
//// void MyCreateCaret
////     (HWND      hWnd,
////      LPVKSTATE lpvkState,
////      long      cyAveChar)
//// {
////     PostMessageW (hWnd,
////                   MYWM_CREATECARET,
////                   lpvkState->Ins ? DEF_CURWID_INS : DEF_CURWID_REP,
////                   cyAveChar);
//// } // End MyCreateCaret


//***************************************************************************
//  DisplayPrompt
//
//  Display the usual six-space prompt
//***************************************************************************

void DisplayPrompt
    (HWND hWndEC)       // Window handle of the Edit Control

{
    UINT uCharPos,
         uLinePos;

    // Get the line position of the current line
    uLinePos = SendMessageW (hWndEC, EM_LINEINDEX, (WPARAM) -1, 0);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWndEC);

    // If the char position of the caret
    //   is not at the left, put it there
    if (uLinePos NE uCharPos)
        AppendLine (L"", FALSE, TRUE);
    AppendLine (wszIndent, FALSE, FALSE);
} // End DisplayPrompt


//***************************************************************************
//  SM_Create
//
//  Perform window-specific initialization
//***************************************************************************

void SM_Create
    (HWND hWnd)

{
} // End SM_Create


//***************************************************************************
//  SM_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void SM_Delete
    (HWND hWnd)

{
} // End SM_Delete


//***************************************************************************
//  SMWndProc
//
//  Message processing routine for the Session Manager window
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SMWndProc"
#else
#define APPEND_NAME
#endif

LRESULT APIENTRY SMWndProc
    (HWND hWnd,     // Window handle
     UINT message,  // Type of message
     UINT wParam,   // Additional information
     LONG lParam)   // ...

{
    HWND       hWndEC;      // Window handle to Edit Control
    int        iMaxLimit;   // Maximum # chars in edit control
    VKSTATE    vkState;
    long       lvkState;
    HGLOBAL    hGlbPTD;     // Handle to this window's PerTabData
    LPPERTABDATA lpMemPTD;  // Ptr to ...
////RECT       rcFmtEC;     // Formatting rectangle for the Edit Control
    LPUNDOBUF  lpUndoBeg,   // Ptr to start of Undo Buffer
               lpUndoNxt;   // ...    next available slot in the Undo Buffer
////HDC        hDC;
////HFONT      hFontOld;
////TEXTMETRIC tm;

    // Get the handle to the edit control
    hWndEC = (HWND) GetWindowLong (hWnd, GWLSF_HWNDEC);

////LCLODSAPI ("SM: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_NCCREATE:           // lpcs = (LPCREATESTRUCT) lParam
            hWndSM = hWnd;

            INIT_PERTABVARS

            break;                  // Continue with next handler

        case WM_CREATE:             // 0 = (int) wParam
                                    // lpcs = (LPCREATESTRUCT) lParam
#define lpMDIcs     ((LPMDICREATESTRUCT) (((LPCREATESTRUCT) lParam)->lpCreateParams))
        {
            int i;

            // Initialize variables
            cfSM.hwndOwner = hWnd;
            ZeroMemory (&vkState, sizeof (vkState));
            vkState.Ins = 1;        // Initially inserting ***FIXME*** Make it an option

            // Save in window extra bytes
            SetWindowLong (hWnd, GWLSF_VKSTATE, *(long *) &vkState);

            // Initialize window-specific resources
            SM_Create (hWnd);

            // *************** Undo Buffer *****************************
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
            SetWindowLong (hWnd, GWLSF_UNDO_BEG, (long) lpUndoBeg);
            SetWindowLong (hWnd, GWLSF_UNDO_NXT, (long) lpUndoBeg);
            SetWindowLong (hWnd, GWLSF_UNDO_LST, (long) lpUndoBeg);
////////////SetWindowLong (hWnd, GWLSF_UNDO_GRP, 0);    // Already zero

            // Start with an initial action of nothing
            AppendUndo (hWnd,                       // SM Window handle
                        GWLSF_UNDO_NXT,             // Offset in hWnd extra bytes of lpUndoNxt
                        undoNone,                   // Action
                        0,                          // Beginning char position
                        0,                          // Ending    ...
                        UNDO_NOGROUP,               // Group index
                        0);                         // Character
            // Save incremented starting ptr in window extra bytes
            SetWindowLong (hWnd, GWLSF_UNDO_BEG, (long) ++lpUndoBeg);

            // *************** lpwszCurLine ****************************

            // Allocate memory for the current line
            lpwszCurLine = VirtualAlloc (NULL,      // Any address
                                         DEF_CURLINE_MAXSIZE,
                                         MEM_RESERVE,
                                         PAGE_READWRITE);
            if (!lpwszCurLine)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpwszCurLine> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpwszCurLine,
                          DEF_CURLINE_INITSIZE,
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // *************** lpwszTmpLine ****************************

            // Allocate memory for the temporary line
            lpwszTmpLine = VirtualAlloc (NULL,      // Any address
                                         DEF_CURLINE_MAXSIZE,
                                         MEM_RESERVE,
                                         PAGE_READWRITE);
            if (!lpwszTmpLine)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpwszTmpLine> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpwszTmpLine,
                          DEF_CURLINE_INITSIZE,
                          MEM_COMMIT,
                          PAGE_READWRITE);

////////////// *************** lptkStackBase ***************************
////////////
////////////// Allocate virtual memory for the token stack used in parsing
////////////lptkStackBase = VirtualAlloc (NULL,      // Any address
////////////                              DEF_TOKENSTACK_MAXSIZE,
////////////                              MEM_RESERVE,
////////////                              PAGE_READWRITE);
////////////if (!lptkStackBase)
////////////{
////////////    // ***FIXME*** -- WS FULL before we got started???
////////////    DbgMsg ("WM_CREATE:  VirtualAlloc for <lptkStackBase> failed");
////////////
////////////    return -1;          // Mark as failed
////////////} // End IF
////////////
////////////// Commit the intial size
////////////VirtualAlloc (lptkStackBase,
////////////              DEF_TOKENSTACK_INITSIZE,
////////////              MEM_COMMIT,
////////////              PAGE_READWRITE);

            // *************** lpszNumAlp ******************************

            // Allocate virtual memory for the Name & Number accumulator
            lpszNumAlp = VirtualAlloc (NULL,      // Any address
                                       DEF_NUMALP_MAXSIZE,
                                       MEM_RESERVE,
                                       PAGE_READWRITE);
            if (!lpszNumAlp)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpszNumAlp> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpszNumAlp,
                          DEF_NUMALP_INITSIZE,
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // *************** lpwszString *****************************

            // Allocate virtual memory for the wide string accumulator
            lpwszString = VirtualAlloc (NULL,       // Any address
                                        DEF_STRING_MAXSIZE,
                                        MEM_RESERVE,
                                        PAGE_READWRITE);
            if (!lpwszString)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpwszString> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpwszString,
                          DEF_STRING_INITSIZE,
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // *************** lpHshTab ********************************

            // Allocate virtual memory for the hash table
            lpHshTab = VirtualAlloc (NULL,      // Any address
                                     DEF_HSHTAB_MAXSIZE,
                                     MEM_RESERVE,
                                     PAGE_READWRITE);
            if (!lpHshTab)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpHshTab> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpHshTab,
                          DEF_HSHTAB_INITSIZE,
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // Initialize the principal hash entry (1st one in each block).
            // This entry is never overwritten with an entry with a
            //   different hash value.
            for (i = 0; i < DEF_HSHTAB_INITSIZE; i += DEF_HSHTAB_EPB)
                lpHshTab[i].htFlags.PrinHash = 1;

            // Initialize the next & prev same HTE values
            for (i = 0; i < DEF_HSHTAB_INITSIZE; i++)
            {
                lpHshTab[i].NextSameHash =
                lpHshTab[i].PrevSameHash = LPHSHENTRY_NONE;
            } // End FOR

            // Initialize next split entry
            lpHshTabSplitNext = lpHshTab;

            // *************** lpSymTab ********************************

            // Allocate virtual memory for the symbol table
            lpSymTab = VirtualAlloc (NULL,      // Any address
                                     DEF_SYMTAB_MAXSIZE,
                                     MEM_RESERVE,
                                     PAGE_READWRITE);
            if (!lpSymTab)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpSymTab> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpSymTab,
                          DEF_SYMTAB_INITSIZE,
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // Initialize next available entry
            lpSymTabNext = lpSymTab;

            // Get the PerTabData handle
            hGlbPTD = (HGLOBAL) (lpMDIcs->lParam);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Initialize the Symbol table Entry for the constant zero
            lpMemPTD->steZero = SymTabAppendInteger_EM (0);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

////////////// *************** Fonts ***********************************
////////////
////////////// Get the text metrics for this font
////////////hDC = MyGetDC (hWnd);
////////////hFontOld = SelectObject (hDC, hFontTC);
////////////GetTextMetrics (hDC, &tm);
////////////SelectObject (hDC, hFontOld);
////////////MyReleaseDC (hWnd, hDC);
////////////
////////////// New height
////////////cyAveCharSM = MulDiv (cfSM.iPointSize / 10, iLogPixelsY, 72);
////////////cyAveCharSM = -lfSM.lfHeight;
////////////
////////////lfSM.lfWidth = (tm.tmAveCharWidth + tm.tmMaxCharWidth) / 2;
////////////
////////////// New width (same aspect ratio as old)
////////////cxAveCharSM = MulDiv (lfSM.lfWidth, cyAveCharSM, -lfSM.lfHeight);

            // *************** System Names ****************************

            // Append all system names (functions and variables) as reserved
            if (!AppendSystemNames_EM ())
            {
                DbgMsg ("WM_CREATE:  AppendSystemNames_EM failed");

                return -1;          // Mark as failed
            } // End IF

            // *************** System Vars *****************************

            // Initialize all system vars
            if (!InitSystemVars ())
            {
                DbgMsg ("WM_CREATE:  InitSystemVars failed");

                return -1;          // Mark as failed
            } // End IF

            // *************** Edit Control ****************************
            // Create an edit box within which we can edit
            hWndEC =
            CreateWindowExW (0L,                    // Extended styles
                             LECWNDCLASS,           // Class name
                             NULL,                  // Initial text
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
                             (HMENU) IDWC_SM_EC,    // ID
                             _hInstance,            // Instance
                             0);                    // lParam
            if (hWndEC EQ NULL)
            {
                MB (pszNoCreateSMEditCtrl);

                return -1;          // Stop the whole process
            } // End IF

            // Save in window extra bytes
            SetWindowLong (hWnd, GWLSF_HWNDEC, (long) hWndEC);

            // Subclass the Edit Control so we can handle some of its messages
            lpfnOldEditCtrlWndProc = (WNDPROC)
              SetWindowLongW (hWndEC,
                              GWL_WNDPROC,
                              (long) (WNDPROC) &LclEditCtrlWndProc);
            // Set the paint hook
            SendMessageW (hWndEC, EM_SETPAINTHOOK, 0, (LPARAM) &LclECPaintHook);

////////////// Set the soft-break flag
////////////SendMessageW (hWndEC, EM_FMTLINES, TRUE, 0);

            // Paint the window
            ShowWindow (hWndEC, SW_SHOWNORMAL);
            UpdateWindow (hWndEC);

////        // Use Post here as we need to wait for the EC window
////        //   to be drawn.
////        PostMessage (hWnd, MYWM_INIT_EC, 0, 0);

            return FALSE;           // We handled the msg
        } // End WM_CREATE
#undef  lpcs

        case WM_PARENTNOTIFY:       // fwEvent = LOWORD(wParam);  // Event flags
                                    // idChild = HIWORD(wParam);  // Identifier of child window
                                    // lValue = lParam;           // Child handle, or cursor coordinates
#define fwEvent     LOWORD (wParam)
#define idChild     HIWORD (wParam)

            // Check for WM_CREATE from the Edit Control
            if (fwEvent EQ WM_CREATE
             && idChild EQ IDWC_SM_EC)
                PostMessage (hWnd, MYWM_INIT_EC, 0, 0);

            return FALSE;           // We handled the msg

#undef  idChild
#undef  fwEvent

        case MYWM_INIT_EC:
            // Tell the Edit Control to redraw itself
            InvalidateRect (hWndEC, NULL, FALSE);

////        // Tell the Tab Control to redraw itself
////        InvalidateRect (hWndTC, NULL, FALSE);

            // Tell the Edit Control about its font
            SendMessageW (hWndEC, WM_SETFONT, (WPARAM) hFontSM, TRUE);

            // ***DEBUG***
            SendMessageW (hWnd, MYWM_KEYDOWN, VK_F9, 0);

////////////// Get the current vkState
////////////lvkState = GetWindowLong (hWnd, GWLSF_VKSTATE);
////////////vkState = *(LPVKSTATE) &lvkState;

////////////// Create a default sized system caret for display
////////////MyCreateCaret (hWndEC, &vkState, cyAveCharSM);

            // Make sure we can communicate between windows
            AttachThreadInput (GetCurrentThreadId (), dwMainThreadId, TRUE);

            // Display the default prompt
            DisplayPrompt (hWndEC);

            SetFocus (hWnd);

            return FALSE;           // We handled the msg

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
////////////////// Get the formatting rectangle
////////////////SendMessageW (hWndEC, EM_GETRECT, 0, (LPARAM) &rcFmtEC);
////////////////
////////////////// Move the rectangle over enough chars (FCN_INDENT) to provide room
//////////////////   for line numbers
////////////////rcFmtEC.left = FCN_INDENT * cxAveCharSM;
////////////////
////////////////// Tell the control about this change
////////////////SendMessageW (hWndEC, EM_SETRECT, 0, (LPARAM) &rcFmtEC);
            } // End IF

            break;                  // Continue with next handler ***MUST***
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_SETFONT:
            // Pass it on the the edit control
            SendMessageW (hWndEC, message, wParam, lParam);

            break;

        case WM_SETFOCUS:           // hwndLoseFocus = (HWND) wParam; // handle of window losing focus
            // Pass on to the edit ctrl
            SetFocus (hWndEC);

            break;                  // Continue with next handler ***MUST***

        case WM_UNDO:
        case MYWM_REDO:
        case WM_COPY:
        case WM_CUT:
        case WM_PASTE:
        case MYWM_PASTE_APLWIN:
        case MYWM_PASTE_APL2:
        case WM_CLEAR:
        case MYWM_SELECTALL:
            // Pass on to the Edit Control
            SendMessageW (hWndEC, message, wParam, lParam);

            return FALSE;           // We handled the msg

#define nVirtKey    ((int) wParam)
#define uLineNum    ((UINT) lParam)
        case MYWM_KEYDOWN:          // nVirtKey = (int) wParam;     // Virtual-key code
                                    // uLineNum = lParam;           // Line #   // lKeyData = lParam;           // Key data
        {
            EXECSTATE esState;
            UINT      uLineLen,
                      uLineCnt;

            // Special cases for SM windows:
            //   * Up/Dn arrows:
            //     * Cache original line before it's changed
            //
            //   * CR:
            //     * Restore original line from cached copy
            //     * Pass changed line to parent for execution
            //
            //   * Shift-CR
            //     * Insert a soft-break (line continuation) ***FIXME*** -- Not done as yet

            switch (nVirtKey)
            {
                case VK_RETURN:
                    // Check for Quad or Quote-quad input
////                if ()       // ***FIXME***
                    {


                    } // End IF

                    // If we're not on the last line,
                    //   copy it and append it to the buffer
                    if (!IzitLastLine (hWndEC))
                    {
                        UINT uLastNum;

                        // Get the current line
                        ((LPWORD) lpwszTmpLine)[0] = DEF_CURLINE_MAXLEN;
                        SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwszTmpLine);

                        // Append CRLF
                        lstrcatW (lpwszTmpLine, L"\r\n");

                        // Move the caret to the end of the buffer
                        MoveCaretEOB (hWndEC);

                        // Get the # of the last line
                        uLastNum = SendMessageW (hWndEC, EM_LINEFROMCHAR, (WPARAM) -1, 0);

                        // Replace the last line in the buffer
                        ReplaceLine (lpwszTmpLine, uLastNum);

                        // Restore the original of the current line
                        ReplaceLine (lpwszCurLine, uLineNum);

                        // Move the caret to the end of the buffer
                        MoveCaretEOB (hWndEC);

                        // Get the current line #
                        uLineNum = uLastNum;
                    } // End IF

                    // Mark as immediate execution
                    esState.exType = EX_IMMEX;
                    ExecuteLine (uLineNum, &esState, hWndEC);

                    // Display the default prompt
                    DisplayPrompt (hWndEC);

                    break;

                case VK_UP:
                    // If the next line is out of range, exit
                    if (uLineNum < 1)
                        break;

                    // Specify the maximum # chars for the buffer
                    ((LPWORD) lpwszCurLine)[0] = DEF_CURLINE_MAXLEN;

                    // Save the (new) current line
                    uLineLen =
                    SendMessageW (hWndEC, EM_GETLINE, max (uLineNum, 1) - 1, (LPARAM) lpwszCurLine);

                    // Ensure properly terminated
                    lpwszCurLine[uLineLen] = L'\0';

                    // Reset the changed line flag
                    SetWindowLong (hWnd, GWLSF_CHANGED, FALSE);

                    break;

                case VK_DOWN:
                    // Get the # lines in the Edit Control
                    uLineCnt = SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

                    // If the next line is out of range, exit
                    if (uLineCnt <= (uLineNum + 1))
                        break;

                    // Specify the maximum # chars for the buffer
                    ((LPWORD) lpwszCurLine)[0] = DEF_CURLINE_MAXLEN;

                    // Save the (new) current line
                    uLineLen =
                    SendMessageW (hWndEC, EM_GETLINE, uLineNum + 1, (LPARAM) lpwszCurLine);

                    // Ensure properly terminated
                    lpwszCurLine[uLineLen] = L'\0';

                    // Reset the changed line flag
                    SetWindowLong (hWnd, GWLSF_CHANGED, FALSE);

                    break;
#ifdef DEBUG
                case VK_F1:             // No action defined as yet
                case VK_F6:             // ...
                case VK_F7:             // ...
////////////////case VK_F8:             // Handled in EDITFCN.C
                case VK_F10:            // Not generated
                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F2:             // Display hash table entries
                    DisplayHshTab ();

                    return FALSE;
#endif
////#ifdef DEBUG
////                case VK_F3:             // Display current token entries
////
////                    DisplayTokens (ghGlbToken);
////
////                    return FALSE;
////#endif
#ifdef DEBUG
                case VK_F4:             // Display symbol table entries
                                        //   with non-zero reference counts
                    // If it's Shift-, then display all
                    if (GetKeyState (VK_SHIFT) & 0x80000000)
                        DisplaySymTab (TRUE);
                    else
                        DisplaySymTab (FALSE);

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F5:             // Display outstanding global memory objects
                    // If it's Shift-, then display all
                    if (GetKeyState (VK_SHIFT) & 0x80000000)
                        DisplayGlobals (TRUE);
                    else
                        DisplayGlobals (FALSE);

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F9:             // Resize Debugger and Session Manager windows
                {
                    RECT rc;
                    int  nWidthMC,  nHeightMC,
                         nHeightDB, nHeightSM;
                    HWND hWndMC;

                    // Get the window handle to the MDI Client (our parent)
                    hWndMC = GetParent (hWnd);

                    // Get its client rectangle
                    GetClientRect (hWndMC, &rc);

                    // Calculate its width & height
                    nWidthMC  = rc.right  - rc.left;
                    nHeightMC = rc.bottom - rc.top;

                    // Calculate the height of the DB & SM windows
                    nHeightSM = 350;
                    nHeightDB = nHeightMC - nHeightSM;

                    // Resize the Debugger window
                    //   to the top of the client area
                    SetWindowPos (hWndDB,           // Window handle to position
                                  0,                // SWP_NOZORDER
                                  0,                // X-position
                                  0,                // Y-...
                                  nWidthMC,         // Width
                                  nHeightDB,        // Height
                                  SWP_NOZORDER      // Flags
                                | SWP_SHOWWINDOW
                                 );
                    // Resize the Session Manager window
                    //   to the bottom of the client area
                    SetWindowPos (hWndSM,           // Window handle to position
                                  0,                // SWP_NOZORDER
                                  0,                // X-position
                                  nHeightDB,        // Y-...
                                  nWidthMC,         // Width
                                  nHeightSM,        // Height
                                  SWP_NOZORDER      // Flags
                                | SWP_SHOWWINDOW
                                 );
                    return FALSE;
                } // End VK_F9
#endif
#ifdef DEBUG
                case VK_F11:            // DbgBrk ()
                    DbgBrk ();

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F12:            // Clear the debugging display
                    // Clear the debugger listbox
                    DbgClr ();

                    return FALSE;
#endif
                defstop
                    break;
            } // End SWITCH

            return FALSE;           // We handled the msg
        } // End MYWM_KEYDOWN
#undef  uLineNum
#undef  nVirtKey

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            SM_Delete (hWnd);

            // Initialize window-specific resources
            SM_Create (hWnd);

            return FALSE;           // We handled the msg

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

#define wNotifyCode     (HIWORD (wParam))
#define wID             (LOWORD (wParam))
#define hWndCtrl        ((HWND) lParam)
        case WM_COMMAND:            // wNotifyCode = HIWORD (wParam); // Notification code
                                    // wID = LOWORD (wParam);         // Item, control, or accelerator identifier
                                    // hwndCtrl = (HWND) lParam;      // Handle of control
            // This message should be from the edit control
            Assert (wID      EQ IDWC_FE_EC
                 || wID      EQ IDWC_SM_EC);
            Assert (hWndCtrl EQ hWndEC || hWndEC EQ 0);

            // Split cases based upon the notify code
            switch (wNotifyCode)
            {
////            case EN_SETFOCUS:                   // idEditCtrl = (int) LOWORD(wParam); // Identifier of edit control
////                                                // wNotifyCode = HIWORD(wParam);      // Notification code
////                                                // hwndEditCtrl = (HWND) lParam;      // Handle of edit control
////                // Get the current vkState
////                lvkState = GetWindowLong (hWnd, GWLSF_VKSTATE);
////                vkState = *(LPVKSTATE) &lvkState;
////
////                // Create a default sized system caret for display
////                Assert (hWndEC NE 0);
////                MyCreateCaret (hWndEC, &vkState, cyAveCharSM);
////
////                // Paint the window
////                UpdateWindow (hWndEC);
////
////                break;

                case EN_CHANGE:                     // idEditCtrl = (int) LOWORD(wParam); // Identifier of edit control
                                                    // hwndEditCtrl = (HWND) lParam;      // Handle of edit control
                    // The contents of the edit control have changed,
                    // set the changed flag
                    SetWindowLong (hWnd, GWLSF_CHANGED, TRUE);

                    break;

                case EN_MAXTEXT:    // idEditCtrl = (int) LOWORD(wParam); // Identifier of edit control
                                    // hwndEditCtrl = (HWND) lParam;      // Handle of edit control
                    // The edit control has exceed its maximum # chars
                    DbgBrk ();      // ***TESTME***

                    // The default maximum is 32K, so we increase it by that amount
                    Assert (hWndEC NE 0);
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
            } // End IF

            break;
#undef  hWndCtrl
#undef  wID
#undef  wNotifyCode

        case WM_CLOSE:
            // Because the SM window doesn't close unless the
            //   MF window is closing, we ignore this message.
            // We close when the MF window calls DestroyWindow.

            return FALSE;           // We handled the msg

        case WM_DESTROY:
        {
            // *************** Undo Buffer *****************************
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

            // *************** lpSymTab ********************************
            if (lpSymTab)
            {
                VirtualFree (lpSymTab, 0, MEM_RELEASE); lpSymTab = NULL;
            } // End IF

            // *************** lpwszString *****************************
            if (lpwszString)
            {
                VirtualFree (lpwszString, 0, MEM_RELEASE); lpwszString = NULL;
            } // End IF

            // *************** lpszNumAlp ******************************
            if (lpszNumAlp)
            {
                VirtualFree (lpszNumAlp, 0, MEM_RELEASE); lpszNumAlp = NULL;
            } // End IF

////////////// *************** lptkStackBase ***************************
////////////if (lptkStackBase)
////////////{
////////////    VirtualFree (lptkStackBase, 0, MEM_RELEASE); lptkStackBase = NULL;
////////////} // End IF

            // *************** lpwszCurLine ****************************
            if (lpwszCurLine)
            {
                VirtualFree (lpwszCurLine, 0, MEM_RELEASE); lpwszCurLine = NULL;
            } // End IF

            // Uninitialize window-specific resources
            SM_Delete (hWnd);

            return FALSE;           // We handled the msg
        } // End WM_DESTROY
    } // End SWITCH

////LCLODSAPI ("SMZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProc (hWnd, message, wParam, lParam);
} // End SMWndProc
#undef  APPEND_NAME


//***************************************************************************
//  End of File: sessman.c
//***************************************************************************
