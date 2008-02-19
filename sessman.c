//***************************************************************************
//  NARS2000 -- Session Manager
//***************************************************************************

#define STRICT
#define _WIN32_WINNT  0x0500
#include <windows.h>
#include <windowsx.h>
#include <colors.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "resource.h"
#include "externs.h"
#include "editctrl.h"
#include "pertab.h"
#include "dfnhdr.h"
#include "sis.h"

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

////LPTOKEN lptkStackBase;          // Ptr to base of token stack used in parsing

// MDI WM_NCCREATE & WM_CREATE parameter passing convention
//
// When calling CreateMDIWindow with an extra data parameter
//   of (say) &hGlbPTD, the window procedure receives the data
//   in the following struc:
//
//      typedef struct tagSM_CREATEPARAMS
//      {
//          HGLOBAL hGlbPTD;
//      } SM_CREATEPARAMS, UNALIGNED *LPSM_CREATEPARAMS;
//
//   which is used as follows:
//
//      #define lpMDIcs     ((LPMDICREATESTRUCT) (((LPCREATESTRUCT) lParam)->lpCreateParams))
//      hGlbPTD = ((LPSM_CREATEPARAMS) (lpMDIcs->lParam))->hGlbPTD;
//      #undef  lpMDIcs

APLCHAR wszQuadInput[] =
WS_UTF16_QUAD L":";


//***************************************************************************
//  $SetAttrs
//
//  Set attributes for a DC
//***************************************************************************

void SetAttrs
    (HDC      hDC,              // Handle to the device context
     HFONT    hFont,            // Handle to the font
     COLORREF crfg,             // Foreground text color
     COLORREF crbk)             // Background ...

{
    // Set the mapping mode
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
//  $AppendLine
//
//  Append lpwszLine to the history buffer
//***************************************************************************

void AppendLine
    (LPWCHAR lpwszLine,         // Ptr to the line to append
     BOOL    bLineCont,         // TRUE iff this is a line continuation
     BOOL    bEndingCRLF)       // TRUE iff this line should end with a CR/LF

{
    HWND         hWndEC;        // Window handle to Edit Control
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the handle to the edit control
    hWndEC = (HWND) GetWindowLong (lpMemPTD->hWndSM, GWLSF_HWNDEC);

    // Move the caret to the end of the buffer
    MoveCaretEOB (hWndEC);

    // Scroll the caret into view
    SendMessageW (hWndEC, EM_SCROLLCARET, 0, 0);
#ifdef DEBUG
    dprintfW (L"AppendLine: <%s> (%S#%d)", lpwszLine, FNLN);
#endif
    // Replace the selection (none) with the new line
    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) lpwszLine);

    // If requested, end the line
    if (bEndingCRLF)
        // Replace the selection (none) with "\r\n"
        SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) L"\r\n");
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End AppendLine


//***************************************************************************
//  $ReplaceLine
//
//  Replace lpwszCurLine in the history buffer
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReplaceLine"
#else
#define APPEND_NAME
#endif

void ReplaceLine
    (HWND    hWndSM,
     LPWCHAR lpwszLine,
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
//  $IzitLastLine
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
//// //  $DrawLineCont
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
//// //  $DrawBitmap
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
//  $strchrW
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
//  $MoveCaretEOB
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


//***************************************************************************
//  $DisplayPrompt
//
//  Display the usual six-space prompt
//***************************************************************************

void DisplayPrompt
    (HWND hWndEC,       // Window handle of the Edit Control
/////BOOL bSetFocusSM,  // TRUE iff we're to set the focus to the Session Manager
     UINT uCaller)      // ***DEBUG***

{
#ifdef DEBUG
    dprintfW (L"~~DisplayPrompt (%d)", uCaller);
#endif
    // Move the caret to the End-of-the-buffer
    MoveCaretEOB (hWndEC);

    // Display the indent
    AppendLine (wszIndent, FALSE, FALSE);

////if (bSetFocusSM)
////    // Set the focus to the Session Manager so the prompt displays
////    PostMessage (GetParent (hWndEC), MYWM_SETFOCUS, 0, 0);
} // End DisplayPrompt


//***************************************************************************
//  $FormatQQuadInput
//
//  Format QQ input and save in global memory
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FormatQQuadInput"
#else
#define APPEND_NAME
#endif

void FormatQQuadInput
    (UINT          uLineNum,        // Line #
     HWND          hWndEC,          // Handle of Edit Control window
     LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    UINT         uLinePos,      // Char position of start of line
                 uLineLen;      // Line length
    APLUINT      ByteRes;       // # bytes in the result
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPAPLCHAR    lpMemRes;      // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Get the position of the start of the line
    uLinePos = SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the line length
    uLineLen = SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Calculate space needed for the result
    // N.B.:  max is needed because, in order to get the line,
    //        we need to tell EM_GETLINE the buffer size which
    //        takes up one APLCHAR (WORD) at the start of the buffer.
    ByteRes = CalcArraySize (ARRAY_CHAR, max (uLineLen, 1), 1);

    // Allocate space for the result
    // N.B.:  Conversion from APLUINT to UINT
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lpMemPTD->lpSISCur->lptkFunc);
        // Make a PL_YYSTYPE NoValue entry
        lpYYRes = MakeNoValue_YY (lpMemPTD->lpSISCur->lptkFunc);
    } else
    {
        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeaderRes->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeaderRes->ArrType    = ARRAY_CHAR;
////////lpHeaderRes->Perm       = 0;        // Already zero from GHND
////////lpHeaderRes->SysVar     = 0;        // Already zero from GHND
        lpHeaderRes->RefCnt     = 1;
        lpHeaderRes->NELM       = uLineLen;
        lpHeaderRes->Rank       = 1;
#undef  lpHeaderRes

        // Save the dimension in the result
        *VarArrayBaseToDim (lpMemRes) = uLineLen;

        // Skip over the header and dimensions to the data
        lpMemRes = VarArrayBaseToData (lpMemRes, 1);

        // Tell EM_GETLINE maximum # chars in the buffer
        // Because we allocated space for max (uLineLen, 1)
        //   chars, we don't have to worry about overwriting
        //   the allocation limits of the buffer
        ((LPWORD) lpMemRes)[0] = (WORD) uLineLen;

        // Get the contents of the line
        SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpMemRes);

        // Replace leading Prompt Replacement chars
        if (lpMemPTD->cQuadPR NE L'\0')
        {
            UINT QQPromptLen,   // Length of QQ prompt
                 u;             // Loop counter

            // Get the length of the QQ prompt
            QQPromptLen = lpMemPTD->lpSISCur->QQPromptLen;

            // ***FIXME*** -- we're supposed to save the actual prompt
            //                and compare it with the chars after the
            //                user has responded to the request for
            //                Quote-Quad input.

            // Replace all prompt chars
            for (u = 0; u < QQPromptLen; u++)
                lpMemRes[u] = lpMemPTD->cQuadPR;
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;             // Already zero from YYAlloc
        lpYYRes->tkToken.tkFlags.NoDisplay = 1;
        lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
        lpYYRes->tkToken.tkCharIndex       = lpMemPTD->lpSISCur->lptkFunc->tkCharIndex;
    } // End IF/ELSE

    // Save the result in PerTabData
    lpMemPTD->YYResExec = *lpYYRes;

    // Free the YYRes we allocated
    YYFree (lpYYRes); lpYYRes = NULL;

    Assert (lpMemPTD->lpSISCur->hSemaphore NE NULL);

    if (lpMemPTD->lpSISCur->hSemaphore)
    {
#ifdef DEBUG
        dprintfW (L"~~Releasing semaphore:  %08X (%S#%d)", lpMemPTD->lpSISCur->hSemaphore, FNLN);
#endif
        // Signal WaitForInput that we have a result
        ReleaseSemaphore (lpMemPTD->lpSISCur->hSemaphore, 1, NULL);

        // Release our time slice so the released thread can act
        Sleep (0);
    } // End IF
} // End FormatQQuadInput
#undef  APPEND_NAME


//***************************************************************************
//  $SM_Create
//
//  Perform window-specific initialization
//***************************************************************************

void SM_Create
    (HWND hWnd)

{
} // End SM_Create


//***************************************************************************
//  $SM_Delete
//
//  Perform window-specific uninitialization
//***************************************************************************

void SM_Delete
    (HWND hWnd)

{
} // End SM_Delete


//***************************************************************************
//  $SMWndProc
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
    HWND         hWndEC;    // Window handle to Edit Control
    int          iMaxLimit; // Maximum # chars in edit control
    VKSTATE      vkState;
    long         lvkState;
    HGLOBAL      hGlbPTD;   // Handle to this window's PerTabData
    LPPERTABDATA lpMemPTD;  // Ptr to ...
////RECT         rcFmtEC;   // Formatting rectangle for the Edit Control
    LPUNDO_BUF   lpUndoBeg, // Ptr to start of Undo Buffer
                 lpUndoNxt; // ...    next available slot in the Undo Buffer
////HDC          hDC;
////HFONT        hFontOld;
////TEXTMETRIC   tm;

    // Get the handle to the edit control
    hWndEC = (HWND) GetWindowLong (hWnd, GWLSF_HWNDEC);

////LCLODSAPI ("SM: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_NCCREATE:               // lpcs = (LPCREATESTRUCT) lParam
        {
            LPVOID p;

            // Get the thread's PerTabData global memory handle
            hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Save the window handle
            lpMemPTD->hWndSM = hWnd;

            INIT_PERTABVARS

            // Allocate virtual memory for the []ERROR/[]ES buffer
            p = lpMemPTD->lpwszQuadErrorMsg =
            VirtualAlloc (NULL,          // Any address
                          DEF_QUADERROR_MAXSIZE * sizeof (lpMemPTD->lpwszQuadErrorMsg[0]),
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!p)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_NCCREATE:  VirtualAlloc for <lpwszQuadErrorMsg> failed");

                return -1;          // Mark as failed
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Commit the intial size
            VirtualAlloc (lpMemPTD->lpwszQuadErrorMsg,
                          DEF_QUADERROR_INITSIZE * sizeof (lpMemPTD->lpwszQuadErrorMsg[0]),
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            break;                  // Continue with next handler
        } // End WM_NCCREATE

        case WM_CREATE:             // 0 = (int) wParam
                                    // lpcs = (LPCREATESTRUCT) lParam
        {
            int    i;
            LPVOID p;

            // Get the thread's PerTabData global memory handle
            hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

            // Initialize # threads
            SetProp (hWnd, "NTHREADS", 0);

            // Initialize variables
            cfSM.hwndOwner = hWnd;
            ZeroMemory (&vkState, sizeof (vkState));
            vkState.Ins = 1;        // Initially inserting ***FIXME*** Make it an option

            // Save in window extra bytes
            SetWindowLong (hWnd, GWLSF_VKSTATE, *(long *) &vkState);

            // Initialize window-specific resources
            SM_Create (hWnd);

            // *************** Undo Buffer *****************************
            // _BEG is the (static) ptr to the beginning of the virtual memory.
            // _NXT is the (dynamic) ptr to the next available entry.
            //    Undo entries are between _NXT[-1] and _BEG, inclusive.
            // _LST is the (dynamic) ptr to the last available entry.
            //    Redo entries are between _NXT and _LST[-1], inclusive.

            // Allocate virtual memory for the Undo Buffer
            p = lpUndoBeg =
            VirtualAlloc (NULL,          // Any address
                          DEF_UNDOBUF_MAXSIZE * sizeof (lpUndoBeg[0]),
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (!p)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpUndoBeg> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (lpUndoBeg,
                          DEF_UNDOBUF_INITSIZE * sizeof (lpUndoBeg[0]),
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

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate memory for the current line
            p = lpMemPTD->lpwszCurLine =
              VirtualAlloc (NULL,       // Any address
                            DEF_CURLINE_MAXSIZE * sizeof (lpMemPTD->lpwszCurLine[0]),
                            MEM_RESERVE,
                            PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!p)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpMemPTD->lpwszCurLine> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (p,
                          DEF_CURLINE_INITSIZE * sizeof (lpMemPTD->lpwszCurLine[0]),
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // *************** lpwszTmpLine ****************************

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate memory for the temporary line
            p = lpMemPTD->lpwszTmpLine =
              VirtualAlloc (NULL,       // Any address
                            DEF_CURLINE_MAXSIZE * sizeof (lpMemPTD->lpwszTmpLine[0]),
                            MEM_RESERVE,
                            PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!p)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpMemPTD->lpwszTmpLine> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (p,
                          DEF_CURLINE_INITSIZE * sizeof (lpMemPTD->lpwszTmpLine[0]),
                          MEM_COMMIT,
                          PAGE_READWRITE);

////////////// *************** lptkStackBase ***************************
////////////
////////////// Lock the memory to get a ptr to it
////////////lpMemPTD = MyGlobalLock (hGlbPTD);
////////////
////////////// Allocate virtual memory for the token stack used in parsing
////////////p = lpMemPTD->lptkStackBase =
////////////  VirtualAlloc (NULL,       // Any address
////////////                DEF_TOKENSTACK_MAXSIZE * sizeof (TOKEN),
////////////                MEM_RESERVE,
////////////                PAGE_READWRITE);
////////////// We no longer need this ptr
////////////MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
////////////
////////////if (!p)
////////////{
////////////    // ***FIXME*** -- WS FULL before we got started???
////////////    DbgMsg ("WM_CREATE:  VirtualAlloc for <lptkStackBase> failed");
////////////
////////////    return -1;          // Mark as failed
////////////} // End IF
////////////
////////////// Commit the intial size
////////////VirtualAlloc (p,
////////////              DEF_TOKENSTACK_INITSIZE * sizeof (TOKEN),
////////////              MEM_COMMIT,
////////////              PAGE_READWRITE);

            // *************** lpszNumAlp ******************************

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the Name & Number accumulator
            p = lpMemPTD->lpszNumAlp =
              VirtualAlloc (NULL,       // Any address
                            DEF_NUMALP_MAXSIZE * sizeof (lpMemPTD->lpszNumAlp[0]),
                            MEM_RESERVE,
                            PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!p)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpszNumAlp> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (p,
                          DEF_NUMALP_INITSIZE * sizeof (lpMemPTD->lpszNumAlp[0]),
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // *************** lpwszString *****************************

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the wide string accumulator
            p = lpMemPTD->lpwszString =
              VirtualAlloc (NULL,       // Any address
                            DEF_STRING_MAXSIZE * sizeof (lpMemPTD->lpwszString[0]),
                            MEM_RESERVE,
                            PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!p)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpMemPTD->lpwszString> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (p,
                          DEF_STRING_INITSIZE * sizeof (lpMemPTD->lpwszString[0]),
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // *************** lpHshTab ********************************

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the hash table
            p = lpMemPTD->lpHshTab =
              VirtualAlloc (NULL,       // Any address
                            DEF_HSHTAB_MAXSIZE * sizeof (lpMemPTD->lpHshTab[0]),
                            MEM_RESERVE,
                            PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!p)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpMemPTD->lpHshTab> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (p,
                          DEF_HSHTAB_INITSIZE * sizeof (lpMemPTD->lpHshTab[0]),
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Initialize the principal hash entry (1st one in each block).
            // This entry is never overwritten with an entry with a
            //   different hash value.
            for (i = 0; i < DEF_HSHTAB_INITSIZE; i += DEF_HSHTAB_EPB)
                lpMemPTD->lpHshTab[i].htFlags.PrinHash = 1;

            // Initialize the next & prev same HTE values
            for (i = 0; i < DEF_HSHTAB_INITSIZE; i++)
            {
                lpMemPTD->lpHshTab[i].NextSameHash =
                lpMemPTD->lpHshTab[i].PrevSameHash = LPHSHENTRY_NONE;
            } // End FOR

            // Initialize next split entry
            lpMemPTD->lpHshTabSplitNext = lpMemPTD->lpHshTab;

////////////// We no longer need this ptr
////////////MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // *************** lpSymTab ********************************

////////////// Lock the memory to get a ptr to it
////////////lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the symbol table
            p = lpMemPTD->lpSymTab =
              VirtualAlloc (NULL,       // Any address
                            DEF_SYMTAB_MAXSIZE * sizeof (lpMemPTD->lpSymTab[0]),
                            MEM_RESERVE,
                            PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!p)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpMemPTD->lpSymTab> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (p,
                          DEF_SYMTAB_INITSIZE * sizeof (lpMemPTD->lpSymTab[0]),
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Initialize next available entry
            lpMemPTD->lpSymTabNext = lpMemPTD->lpSymTab;

            // Initialize the Symbol table Entry for the constants zero, one, blank, and No Value
            lpMemPTD->steZero    = SymTabAppendPermInteger_EM (0);
            lpMemPTD->steOne     = SymTabAppendPermInteger_EM (1);
            lpMemPTD->steBlank   = SymTabAppendPermChar_EM    (L' ');
            lpMemPTD->steNoValue = lpMemPTD->lpSymTabNext++;

            // Set the flags for the NoValue entry
            lpMemPTD->steNoValue->stFlags.Perm = 1;
            lpMemPTD->steNoValue->stFlags.ObjName = OBJNAME_NONE;
            lpMemPTD->steNoValue->stFlags.ObjType = NAMETYPE_UNK;

////////////// We no longer need this ptr
////////////MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // *************** State Indicator Stack *******************
////////////// Lock the memory to get a ptr to it
////////////lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the symbol table
            p = lpMemPTD->lpSISBeg = lpMemPTD->lpSISNxt =
              VirtualAlloc (NULL,       // Any address
                            DEF_SIS_MAXSIZE * sizeof (SYMENTRY),
                            MEM_RESERVE,
                            PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!p)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpMemPTD->lpStateInd> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (p,
                          DEF_SIS_INITSIZE * sizeof (SYMENTRY),
                          MEM_COMMIT,
                          PAGE_READWRITE);

            // *************** YYRes Buffer ****************************
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the YYRes buffer
            p = lpMemPTD->lpYYRes =
              VirtualAlloc (NULL,       // Any address
                            DEF_YYRES_MAXSIZE * sizeof (PL_YYSTYPE),
                            MEM_RESERVE,
                            PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!p)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  VirtualAlloc for <lpMemPTD->lpYYRes> failed");

                return -1;          // Mark as failed
            } // End IF

            // Commit the intial size
            VirtualAlloc (p,
                          DEF_YYRES_INITSIZE * sizeof (PL_YYSTYPE),
                          MEM_COMMIT,
                          PAGE_READWRITE);

////////////// *************** Fonts ***********************************
////////////
////////////// Get the text metrics for this font
////////////hDC = MyGetDC (hWnd);
////////////hFontOld = SelectObject (hDC, hFontTC);
////////////GetTextMetrics (hDC, &tm);
////////////SelectObject (hDC, hFontOld);
////////////MyReleaseDC (hWnd, hDC); hDC = NULL;
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

            // Initialize all system names (functions and variables) as reserved
            if (!InitSystemNames_EM ())
            {
                DbgMsg ("WM_CREATE:  InitSystemNames_EM failed");

                return -1;          // Mark as failed
            } // End IF

            // *************** System Vars *****************************

            // Initialize all system vars
            if (!InitSystemVars ())
            {
                DbgMsg ("WM_CREATE:  InitSystemVars failed");

                return -1;          // Mark as failed
            } // End IF

            // *************** Symbol Names ****************************

            // Initialize all symbol names
            if (!InitSymbolNames ())
            {
                DbgMsg ("WM_CREATE:  InitSymbolNames failed");

                return -1;          // Mark as failed
            } // End IF

            // *************** Edit Control ****************************
            // Create an Edit Control within which we can enter expressions
            hWndEC =
            CreateWindowExW (0L,                    // Extended styles
                             LECWNDCLASS,           // Class name
                             NULL,                  // Initial text
                             0
                           | WS_CHILD
                           | WS_CLIPCHILDREN
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

////////////// Set the soft-break flag (not supported by WineHQ edit ctrl)
////////////SendMessageW (hWndEC, EM_FMTLINES, TRUE, 0);

            // Paint the window
            ShowWindow (hWndEC, SW_SHOWNORMAL);
            UpdateWindow (hWndEC);

            // *************** Magic Functions *************************

            // Initialize all magic functions
            if (!InitMagicFunctions (hGlbPTD, hWndEC))
            {
                DbgMsg ("WM_CREATE:  InitMagicFunctions failed");

                return -1;          // Mark as failed
            } // End IF

            // ***FIXME*** -- For some reason, the
            //                background of the MC window
            //                doesn't get painted, so I'm
            //                using this kludge for now.

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            InvalidateRect (lpMemPTD->hWndMC, NULL, TRUE);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            return FALSE;           // We handled the msg
        } // End WM_CREATE

        case WM_PARENTNOTIFY:       // fwEvent = LOWORD(wParam);  // Event flags
                                    // idChild = HIWORD(wParam);  // Identifier of child window
                                    // lValue = lParam;           // Child handle, or cursor coordinates
#define fwEvent     (LOWORD (wParam))
#define idChild     (HIWORD (wParam))

            // Check for WM_CREATE from the Edit Control/Debugger
            if (fwEvent EQ WM_CREATE)
            // Split cases based upon the child ID
            switch (idChild)
            {
                case IDWC_SM_EC:
                    PostMessage (hWnd, MYWM_INIT_EC, 0, 0);

                    break;
            } // End IF

            return FALSE;           // We handled the msg

#undef  idChild
#undef  fwEvent

#ifdef DEBUG
        case MYWM_INIT_SMDB:
            // Get the thread's PerTabData global memory handle
            hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // If the Debugger window handle is active, ...
            if (lpMemPTD->hWndDB)
                PostMessage (hWnd, MYWM_KEYDOWN, VK_F9, 0);
            else
                PostMessage (hWnd, MYWM_INIT_SMDB, 0, 0);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            break;
#endif
        case MYWM_INIT_EC:
            // ***TESTME*** -- Correct to comment out this call to InvalidateRect?
////        // Tell the Edit Control to redraw itself
////        InvalidateRect (hWndEC, NULL, FALSE);

            // Tell the Edit Control about its font
            SendMessageW (hWndEC, WM_SETFONT, (WPARAM) hFontSM, MAKELPARAM (TRUE, 0));
#ifdef DEBUG
            PostMessage (hWnd, MYWM_INIT_SMDB, 0, 0);
#endif
            // Make sure we can communicate between windows
            AttachThreadInput (GetCurrentThreadId (), dwMainThreadId, TRUE);

            // Display the default prompt
            DisplayPrompt (hWndEC, 1);

            // Ensure the SM has the focus
            SetFocus (hWnd);

            return FALSE;           // We handled the msg

        case MYWM_QUOTEQUAD:        // bQuoteQuad = (BOOL) wParam
                                    // TRUE iff Quote-Quad input, FALSE if Quad input
            if (!wParam)
            {
                AppendLine (wszQuadInput, FALSE, TRUE);
                DisplayPrompt (hWndEC, lParam);
            } // End IF

            return FALSE;           // We handled the msg

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
            break;                  // *MUST* pass on to DefMDIChildProc
#undef  nHeight
#undef  nWidth
#undef  fwSizeType

        case WM_SETFONT:            // hFont = (HFONT) wParam;
                                    // fRedraw = LOWORD (lParam);
        case WM_KILLFOCUS:          // hwndGainFocus = (HWND) wParam; // handle of window gaining focus
            // Pass these messages through to the EditCtrl
            SendMessageW (hWndEC, message, wParam, lParam);

            break;

        case WM_SETFOCUS:           // hwndLoseFocus = (HWND) wParam; // handle of window losing focus
            // Pass on to the edit ctrl
            SetFocus (hWndEC);

            break;                  // *MUST* pass on to DefMDIChildProc

        case WM_MDIACTIVATE:        // Activate/de-activate a child window
            // If we're being activated, ...
            if (GET_WM_MDIACTIVATE_FACTIVATE (hWnd, wParam, lParam))
                ActivateMDIMenu (hMenuSM, hMenuSMWindow);
            break;                  // Continue with DefMDIChildProc

        case MYWM_SETFOCUS:
            // Set the focus to the Session Manager so the cursor displays
            SetFocus (hWnd);

            return FALSE;           // We handled the msg

        case WM_UNDO:
        case MYWM_REDO:
        case WM_COPY:
        case WM_CUT:
        case WM_PASTE:
        case MYWM_PASTE_APLWIN:
        case MYWM_PASTE_APL2:
        case MYWM_PASTE_ISO:
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
            UINT uLineLen,
                 uLineCnt;
            BOOL bRet;

            // Get the thread's PerTabData global memory handle
            hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

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
                case VK_CANCEL:
                {
                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    EnterCriticalSection (&CSOPL);

                    // Mark as Ctrl-Break
                    if (lpMemPTD->lpPLCur)
                    {
                        // If there's a delay active, signal it
                        if (lpMemPTD->hSemaDelay)
                            ReleaseSemaphore (lpMemPTD->hSemaDelay, 1, NULL);
                        else
                            lpMemPTD->lpPLCur->bCtrlBreak = TRUE;
                    } // End IF

                    LeaveCriticalSection (&CSOPL);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                    break;
                } // End VK_CANCEL

                case VK_RETURN:
                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    // If there's an active program, ignore this key
                    bRet = (lpMemPTD->lpSISCur && !lpMemPTD->lpSISCur->Suspended);
                    if (!bRet)
                    {
                        // If we're not on the last line,
                        //   copy it and append it to the buffer
                        if (!IzitLastLine (hWndEC))
                        {
                            UINT uLastNum;

                            // Tell EM_GETLINE maximum # chars in the buffer
                            // The output array is a temporary so we don't have to
                            //   worry about overwriting outside the allocated buffer
                            ((LPWORD) lpMemPTD->lpwszTmpLine)[0] = DEF_CURLINE_MAXLEN;

                            // Get the current line
                            SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpMemPTD->lpwszTmpLine);

                            // Append CRLF
                            lstrcatW (lpMemPTD->lpwszTmpLine, L"\r\n");

                            // Move the caret to the end of the buffer
                            MoveCaretEOB (hWndEC);

                            // Get the # of the last line
                            uLastNum = SendMessageW (hWndEC, EM_LINEFROMCHAR, (WPARAM) -1, 0);

                            // Replace the last line in the buffer
                            ReplaceLine (hWnd, lpMemPTD->lpwszTmpLine, uLastNum);

                            // Restore the original of the current line
                            ReplaceLine (hWnd, lpMemPTD->lpwszCurLine, uLineNum);

                            // Move the caret to the end of the buffer
                            MoveCaretEOB (hWndEC);

                            // Get the current line #
                            uLineNum = uLastNum;
                        } // End IF
                    } // End IF

                    // If we're in Quote-Quad input, ...
                    if (lpMemPTD->lpSISCur
                     && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QQUAD)
                    {
                        // Format QQ input and save in global memory
                        FormatQQuadInput (uLineNum, hWndEC, lpMemPTD);

                        bRet = TRUE;        // Mark as not ImmExecLine material
                    } // End IF

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                    // Execute the line if no other program is active
                    if (!bRet)
                        ImmExecLine (uLineNum, hWndEC);
                    break;

                case VK_UP:
                    // If the next line is out of range, exit
                    if (uLineNum < 1)
                        break;

                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    // Tell EM_GETLINE maximum # chars in the buffer
                    // The output array is a temporary so we don't have to
                    //   worry about overwriting outside the allocated buffer
                    ((LPWORD) lpMemPTD->lpwszCurLine)[0] = DEF_CURLINE_MAXLEN;

                    // Save the (new) current line
                    uLineLen =
                    SendMessageW (hWndEC, EM_GETLINE, max (uLineNum, 1) - 1, (LPARAM) lpMemPTD->lpwszCurLine);

                    // Ensure properly terminated
                    lpMemPTD->lpwszCurLine[uLineLen] = L'\0';

                    // Reset the changed line flag
                    SetWindowLong (hWnd, GWLSF_CHANGED, FALSE);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                    break;

                case VK_DOWN:
                    // Get the # lines in the Edit Control
                    uLineCnt = SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

                    // If the next line is out of range, exit
                    if (uLineCnt <= (uLineNum + 1))
                        break;

                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    // Tell EM_GETLINE maximum # chars in the buffer
                    // The output array is a temporary so we don't have to
                    //   worry about overwriting outside the allocated buffer
                    ((LPWORD) lpMemPTD->lpwszCurLine)[0] = DEF_CURLINE_MAXLEN;

                    // Save the (new) current line
                    uLineLen =
                    SendMessageW (hWndEC, EM_GETLINE, uLineNum + 1, (LPARAM) lpMemPTD->lpwszCurLine);

                    // Ensure properly terminated
                    lpMemPTD->lpwszCurLine[uLineLen] = L'\0';

                    // Reset the changed line flag
                    SetWindowLong (hWnd, GWLSF_CHANGED, FALSE);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                    break;
#ifdef DEBUG
                case VK_F1:             // No action defined as yet
                case VK_F6:             // ...
                case VK_F7:             // ...
////////////////case VK_F8:             // Handled in EDITFCN.C as <DisplayUndo>
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
////                    DisplayTokens (ghGlbToken);
////
////                    return FALSE;
////#endif
#ifdef DEBUG
                case VK_F4:             // Display symbol table entries
                                        //   with non-zero reference counts
                    // If it's Shift-, then display all
                    if (GetKeyState (VK_SHIFT) & 0x8000)
                        DisplaySymTab (TRUE);
                    else
                        DisplaySymTab (FALSE);

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F5:             // Display outstanding global memory objects
                    // If it's Shift-, then display all
                    if (GetKeyState (VK_SHIFT) & 0x8000)
                        DisplayGlobals (2);
                    else
                    // If it's Ctl-, then display non-sysvars
                    if (GetKeyState (VK_CONTROL) & 0x8000)
                        DisplayGlobals (1);
                    else
                    // Otherwise, then display non-permanent non-sysvars
                        DisplayGlobals (0);

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F9:             // Resize Debugger and Session Manager windows
                {
                    RECT         rc;
                    int          nWidthMC,  nHeightMC,
                                 nHeightDB, nHeightSM;
                    HWND         hWndMC;

                    // Get the thread's PerTabData global memory handle
                    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

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
                    SetWindowPos (lpMemPTD->hWndDB, // Window handle to position
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
                    SetWindowPos (lpMemPTD->hWndSM, // Window handle to position
                                  0,                // SWP_NOZORDER
                                  0,                // X-position
                                  nHeightDB,        // Y-...
                                  nWidthMC,         // Width
                                  nHeightSM,        // Height
                                  SWP_NOZORDER      // Flags
                                | SWP_SHOWWINDOW
                                 );
                    // Tell the debugger window to scroll the last line into view
                    SendMessageW (lpMemPTD->hWndDB, MYWM_DBGMSG_SCROLL, (WPARAM) -1, 0);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                    return FALSE;
                } // End VK_F9
#endif
#ifdef DEBUG
                case VK_F11:            // DbgBrk ()
                {
                    LPSYMENTRY lpSym = NULL;

                    DbgBrk ();      // ***FIXME*** -- Check on lock count of hGlbPTD

                    return FALSE;
                } // End VK_F11
#endif
#ifdef DEBUG
                case VK_F12:            // Clear the debugging display
                    // If it's Shift-, then set the gDbgLvl to 9
                    if (GetKeyState (VK_SHIFT) & 0x8000)
                        gDbgLvl = 9;
                    else
                        // Clear the debugger listbox
                        DbgClr ();

                    return FALSE;
#endif
#ifdef DEBUG
                defstop
#else
                default:
                    DbgBrk ();          // ***FIXME***
                    Beep (1000,        // Frequency in Hz (37 to 32,767)
                          1000);         // Duration in milliseconds
#endif
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

            // Ensure from Edit Ctrl
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
            // Remove all saved window properties
            EnumProps (hWnd, EnumCallbackRemoveProp);

            // Get the thread's PerTabData global memory handle
            hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

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
            if (lpMemPTD->lpSymTab)
            {
                VirtualFree (lpMemPTD->lpSymTab, 0, MEM_RELEASE); lpMemPTD->lpSymTab = NULL;
            } // End IF

            // *************** lpwszString *****************************
            if (lpMemPTD->lpwszString)
            {
                VirtualFree (lpMemPTD->lpwszString, 0, MEM_RELEASE); lpMemPTD->lpwszString = NULL;
            } // End IF

            // *************** lpszNumAlp ******************************
            if (lpMemPTD->lpszNumAlp)
            {
                VirtualFree (lpMemPTD->lpszNumAlp, 0, MEM_RELEASE); lpMemPTD->lpszNumAlp = NULL;
            } // End IF

////////////// *************** lptkStackBase ***************************
////////////if (lpMemPTD->lptkStackBase)
////////////{
////////////    VirtualFree (lpMemPTD->lptkStackBase, 0, MEM_RELEASE); lpMemPTD->lptkStackBase = NULL;
////////////} // End IF

            // *************** lpwszTmpLine ****************************
            if (lpMemPTD->lpwszTmpLine)
            {
                VirtualFree (lpMemPTD->lpwszTmpLine, 0, MEM_RELEASE); lpMemPTD->lpwszTmpLine = NULL;
            } // End IF

            // *************** lpwszCurLine ****************************
            if (lpMemPTD->lpwszCurLine)
            {
                VirtualFree (lpMemPTD->lpwszCurLine, 0, MEM_RELEASE); lpMemPTD->lpwszCurLine = NULL;
            } // End IF

            // Uninitialize window-specific resources
            SM_Delete (hWnd);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // We no longer need this storage
            MyGlobalFree (hGlbPTD); hGlbPTD = NULL;

            // Tell the thread to quit, too
            PostQuitMessage (0);

            break;
    } // End SWITCH

////LCLODSAPI ("SMZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProc (hWnd, message, wParam, lParam);
} // End SMWndProc
#undef  APPEND_NAME


//***************************************************************************
//  End of File: sessman.c
//***************************************************************************
