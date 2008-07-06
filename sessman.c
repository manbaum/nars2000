//***************************************************************************
//  NARS2000 -- Session Manager
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
#define _WIN32_WINNT  0x0500
#include <windows.h>
#include <windowsx.h>
#include <colors.h>

#include "uniscribe.h"
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
are copied to <lpwCurLine>.

If the user edits the line:
    * The edit changes are saved in the Edit Control.

In any case,
    * If the user hits Enter, the contents of the current
      line in the Edit Control are copied to the last line
      in the Edit Control, the contents of <lpwCurLine>
      replace the current line in the Edit Control, and the
      last line in the Edit Control is executed.

 */

//// COLORREF crTextColor = DEF_TEXT_FG_COLOR,
////          crBkColor   = DEF_TEXT_BG_COLOR;

////LPTOKEN lptkStackBase;          // Ptr to base of token stack used in parsing

// MDI WM_NCCREATE & WM_CREATE parameter passing convention
//
// When calling CreateMDIWindowW with an extra data parameter
//   of (say) &hGlbPTD, the window procedure receives the data
//   in the following struc:
//
//      typedef struct tagSM_CREATESTRUCTW
//      {
//          HGLOBAL hGlbPTD;
//      } SM_CREATESTRUCTW, UNALIGNED *LPSM_CREATESTRUCTW;
//
//   which is used as follows:
//
//      #define lpMDIcs     ((LPMDICREATESTRUCTW) (((LPCREATESTRUCTW) lParam)->lpCreateParams))
//      hGlbPTD = ((LPSM_CREATESTRUCTW) (lpMDIcs->lParam))->hGlbPTD;
//      #undef  lpMDIcs

APLCHAR wszQuadInput[] = WS_UTF16_QUAD L":";


typedef enum tagPTDMEMVIRTENUM
{
    PTDMEMVIRT_QUADERROR = 0,           // 00:  lpszQuadErrorMsg
    PTDMEMVIRT_UNDOBEG,                 // 01:  lpUndoBeg
    PTDMEMVIRT_HSHTAB,                  // 02:  htsPTD.lpHshTab
    PTDMEMVIRT_SYMTAB,                  // 03:  lpSymTab
    PTDMEMVIRT_SIS,                     // 04:  lpSISBeg
    PTDMEMVIRT_YYRES,                   // 05:  lpYYRes
    PTDMEMVIRT_STRAND_VAR,              // 06:  lpStrand[STRAND_VAR]
    PTDMEMVIRT_STRAND_FCN,              // 07:  lpStrand[STRAND_FCN]
    PTDMEMVIRT_WSZFORMAT,               // 08:  Temporary formatting
    PTDMEMVIRT_WSZTEMP,                 // 09:  Temporary save area
    PTDMEMVIRT_LENGTH                   // 0A:  # entries
} PTDMEMVIRTENUM;


//***************************************************************************
//  $SetAttrs
//
//  Set attributes for a DC
//***************************************************************************

void SetAttrs
    (HDC      hDC,              // Handle to the device context
     HFONT    hFont,            // Handle to the font (may be NULL)
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
//  $GetThreadSMEC
//
//  Get the hWndEC for the Session Manager from the current thread
//***************************************************************************

HWND GetThreadSMEC
    (void)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    HWND         hWndEC;        // Window handle to Edit Control

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the handle to the edit control
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (lpMemPTD->hWndSM, GWLSF_HWNDEC);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return hWndEC;
} // End GetThreadSMEC


//***************************************************************************
//  $AppendLine
//
//  Append lpwszLine to the history buffer
//***************************************************************************

void AppendLine
    (LPWCHAR lpwszLine,         // Ptr to the line to append
     UBOOL   bLineCont,         // TRUE iff this is a line continuation
     UBOOL   bEndingCRLF)       // TRUE iff this line should end with a CR/LF

{
    HWND hWndEC;                // Window handle to Edit Control

    // Get hWndEC for the Session Manager from the current thread
    hWndEC = GetThreadSMEC ();

    // Move the text caret to the end of the buffer
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
        // Replace the selection (none) with L"\r\n"
        SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) L"\r\n");
} // End AppendLine


//***************************************************************************
//  $ReplaceLine
//
//  Replace the current line in the history buffer
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReplaceLine"
#else
#define APPEND_NAME
#endif

void ReplaceLine
    (HWND    hWndEC,            // Edit Control window handle
     LPWCHAR lpwszLine,         // Ptr to incoming line text
     UINT    uLineNum)          // Line # to replace

{
    UINT uLinePos,
         uLineLen;

    // Get the line position of the given line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the length of the line
    uLineLen = (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Set the selection to this line
    SendMessageW (hWndEC, EM_SETSEL, uLinePos, uLinePos + uLineLen);
#ifdef DEBUG
    dprintfW (L"ReplaceLine: %d:<%s> (%S#%d)", uLineNum, lpwszLine, FNLN);
#endif
    // Replace the selection with the given line
    SendMessageW (hWndEC, EM_REPLACESEL, FALSE, (LPARAM) lpwszLine);
} // End ReplaceLine
#undef  APPEND_NAME


//***************************************************************************
//  $ReplacelastLineCR
//
//  Replace the last line in the history buffer
//    and move the text caret to the next line
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ReplacelastLineCR"
#else
#define APPEND_NAME
#endif

void ReplaceLastLineCR
    (LPWCHAR lpwszLine)         // Ptr to incoming line text

{
    HWND hWndEC;                // Window handle to Edit Control

    // Get hWndEC for the Session Manager from the current thread
    hWndEC = GetThreadSMEC ();

    // Move the text caret to the end of the buffer
    MoveCaretEOB (hWndEC);
#ifdef DEBUG
    dprintfW (L"ReplaceLastLineCR: <%s> (%S#%d)", lpwszLine, FNLN);
#endif
    // Replace the current (now last) line
    ReplaceLine (hWndEC, lpwszLine, NEG1U);

    // Move the text caret to the next line
    AppendLine (L"", FALSE, TRUE);
} // End ReplacelastLineCR
#undef  APPEND_NAME


//***************************************************************************
//  $ReplaceLastLineCRPmt
//
//  Replace the last line in the history buffer
//    move the text caret to the next line, and
//    display a prompt if this is not the active tab
//***************************************************************************

void ReplaceLastLineCRPmt
    (LPWCHAR lpwszLine)         // Ptr to incoming line text

{
#ifdef DEBUG
    dprintfW (L"ReplaceLastLineCRPmt: <%s> (%S#%d)", lpwszLine, FNLN);
#endif
    // Replace the last line
    ReplaceLastLineCR (lpwszLine);

    // If this is not the active tab, display a prompt
    if (!IsCurTabActive ())
        DisplayPrompt (GetThreadSMEC (), 8);
} // End ReplaceLastLineCRPmt


//***************************************************************************
//  $IzitLastLine
//
//  Return TRUE iff the cursor is on the last line
//***************************************************************************

UBOOL IzitLastLine
    (HWND hWndEC)           // Window handle of the Edit Control

{
    UINT uLineCnt,
         uLinePos,
         uLineLen,
         uCharPos;

    // Get the # lines in the text
    uLineCnt = (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

    // Get the line position of the last line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, (WPARAM) (uLineCnt - 1), 0);

    // Get the length of the line
    uLineLen = (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

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
//  $MoveCaretEOB
//
//  Move the text caret in an Edit Control to the end of the buffer
//***************************************************************************

void MoveCaretEOB
    (HWND hWndEC)           // Window handle of Edit Control

{
    UINT uLineCnt,
         uLinePos,
         uLineLen,
         uCharPos;

    // Get the # lines in the text
    uLineCnt = (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

    // Get the initial char pos of the last line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineCnt - 1, 0);

    // Get the length of the last line
    uLineLen = (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Add to get char pos
    uCharPos = uLinePos + uLineLen;

    // Set the caret to the End-of-Buffer
    SendMessageW (hWndEC, EM_SETSEL, uCharPos, uCharPos);
} // End MoveCaretEOB


//***************************************************************************
//  $DisplayPrompt
//
//  Display the usual six-space prompt
//***************************************************************************

void DisplayPrompt
    (HWND hWndEC,       // Window handle of the Edit Control
/////UBOOL bSetFocusSM, // TRUE iff we're to set the focus to the Session Manager
     UINT uCaller)      // ***DEBUG***

{
#ifdef DEBUG
    dprintfW (L"~~DisplayPrompt (%d)", uCaller);
#endif
    // Move the text caret to the end of the buffer
    MoveCaretEOB (hWndEC);

    // Display the indent
    AppendLine (wszIndent, FALSE, FALSE);

////if (bSetFocusSM)
////    // Set the focus to the Session Manager so the prompt displays
////    PostMessageW (GetParent (hWndEC), MYWM_SETFOCUS, 0, 0);
} // End DisplayPrompt


//***************************************************************************
//  $GetLineLength
//
//  Return the line length of a given line #
//***************************************************************************

UINT GetLineLength
    (HWND hWndEC,           // Edit Control window handle
     UINT uLineNum)         // The line #

{
    UINT uLinePos;          // Char position of start of line

    // Get the position of the start of the line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the line length
    return (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);
} // GetLineLength


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
    UINT         uLineLen;      // Line length
    APLUINT      ByteRes;       // # bytes in the result
    HGLOBAL      hGlbRes;       // Result global memory handle
    LPAPLCHAR    lpMemRes;      // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;       // Ptr to the result

    // Get the line length of a given line #
    uLineLen = GetLineLength (hWndEC, uLineNum);

    // Calculate space needed for the result
    // N.B.:  max is needed because, in order to get the line,
    //        we need to tell EM_GETLINE the buffer size which
    //        takes up one APLCHAR (WORD) at the start of the buffer.
    ByteRes = CalcArraySize (ARRAY_CHAR, max (uLineLen, 1), 1);

    // Allocate space for the result
    // N.B.:  Conversion from APLUINT to UINT
    Assert (ByteRes EQ (__int3264) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (__int3264) ByteRes);
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

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = uLineLen;
        lpHeader->Rank       = 1;
#undef  lpHeader

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
        if (lpMemPTD->cQuadPR NE CQUADPR_MT)
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
        lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;
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
        dprintfW (L"~~Releasing semaphore:  %p (%S#%d)", lpMemPTD->lpSISCur->hSemaphore, FNLN);
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
    HWND         hWndEC;                // Window handle to Edit Control
    VKSTATE      vkState;
    long         lvkState;
    HGLOBAL      hGlbPTD;               // Handle to this window's PerTabData
    LPPERTABDATA lpMemPTD;              // Ptr to ...
    LPWCHAR      lpwCurLine;            // Ptr to current line global memory
    UINT         uCnt;                  // Loop counter
////RECT         rcFmtEC;               // Formatting rectangle for the Edit Control
    LPUNDO_BUF   lpUndoBeg;             // Ptr to start of Undo Buffer
////HDC          hDC;
////HFONT        hFontOld;
////TEXTMETRIC   tm;
    static UBOOL bLoadMsgDisp = FALSE;  // TRUE iff )LOAD message has been displayed
    LPMEMVIRTSTR lpLclMemVirtStr;       // Ptr to local MemVirtStr

    // Get the handle to the edit control
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWnd, GWLSF_HWNDEC);

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

////LCLODSAPI ("SM: ", hWnd, message, wParam, lParam);
    switch (message)
    {
#define lpMDIcs     ((LPMDICREATESTRUCTW) ((*(LPCREATESTRUCTW *) &lParam)->lpCreateParams))
        case WM_NCCREATE:               // 0 = (int) wParam
                                        // lpcs = (LPCREATESTRUCTW) lParam
        {
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);
#ifndef UNISCRIBE
            // Initialize the OLE libraries
            CoInitialize (NULL);

            // Get FontLink ptr
            CoCreateInstance (&CLSID_CMultiLanguage,
                               NULL,
                               CLSCTX_ALL,
                              &IID_IMLangFontLink,
                               (void **) &lpMemPTD->lpFontLink);
#endif
            // Save the window handle
            lpMemPTD->hWndSM = hWnd;

            INIT_PERTABVARS

            // Allocate room for MVS_CNT MemVirtStrs
            //  (QUADERROR, UNDO_BUF, HSHTAB, SYMTAB, SIS, YYRES, STRAND_VAR, STRAND_FCN, WSZFORMAT)
            lpLclMemVirtStr =
              MyVirtualAlloc (NULL,                 // Any address (FIXED SIZE)
                              PTDMEMVIRT_LENGTH * sizeof (MEMVIRTSTR),
                              MEM_COMMIT | MEM_TOP_DOWN,
                              PAGE_READWRITE);
            if (!lpLclMemVirtStr)
            {
                // ***FIXME*** -- Display error msg

                return -1;
            } // End IF

            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_LPMVS, (__int3264) (LONG_PTR) lpLclMemVirtStr);

            // Allocate virtual memory for the []ERROR/[]ES buffer
#ifdef DEBUG
            lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].lpText   = "lpMemPTD->lpwszQuadErrorMsg in <SMWndProc>";
#endif
            lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].IncrSize = DEF_QUADERROR_INCRSIZE * sizeof (lpMemPTD->lpwszQuadErrorMsg[0]);
            lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].MaxSize  = DEF_QUADERROR_MAXSIZE  * sizeof (lpMemPTD->lpwszQuadErrorMsg[0]);
            lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].IniAddr  = (LPVOID)
            lpMemPTD->lpwszQuadErrorMsg =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"WM_NCCREATE:  VirtualAlloc for <lpwszQuadErrorMsg> failed");

                goto WM_NCCREATE_FAIL;  // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_QUADERROR]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_QUADERROR].IniAddr,
                            DEF_QUADERROR_INITSIZE * sizeof (lpMemPTD->lpwszQuadErrorMsg[0]),
                            MEM_COMMIT,
                            PAGE_READWRITE);
            break;                  // Continue with next handler
WM_NCCREATE_FAIL:
            // Send a constant message to the previous tab
            SendMessageLastTab (ERRMSG_TABS_FULL APPEND_NAME, hGlbPTD);

            return -1;              // Mark as failed
        } // End WM_NCCREATE
#undef  lpMDIcs

#define lpMDIcs     ((LPMDICREATESTRUCTW) ((*(LPCREATESTRUCTW *) &lParam)->lpCreateParams))
        case WM_CREATE:             // 0 = (int) wParam
                                    // lpcs = (LPCREATESTRUCTW) lParam
        {
            int     i;                  // Loop counter
            HGLOBAL hGlbTmp;            // Temporary hGlbNum/hGlbStr
            LRESULT lResult = -1;       // The result (assume we failed)

            // Initialize # threads
            SetPropW (hWnd, L"NTHREADS", 0);

            // Initialize variables
            cfSM.hwndOwner = hWnd;
            ZeroMemory (&vkState, sizeof (vkState));
            vkState.Ins = TRUE;     // Initially inserting ***FIXME*** Make it an option

            // Save in window extra bytes
            SetWindowLongW (hWnd, GWLSF_VKSTATE, *(long *) &vkState);

            // Initialize window-specific resources
            SM_Create (hWnd);

            // *************** Undo Buffer *****************************
            // _BEG is the (static) ptr to the beginning of the virtual memory.
            // _NXT is the (dynamic) ptr to the next available entry.
            //    Undo entries are between _NXT[-1] and _BEG, inclusive.
            // _LST is the (dynamic) ptr to the last available entry.
            //    Redo entries are between _NXT and _LST[-1], inclusive.

            (HANDLE_PTR) lpLclMemVirtStr = GetWindowLongPtrW (hWnd, GWLSF_LPMVS);

            // Allocate virtual memory for the Undo Buffer
#ifdef DEBUG
            lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].lpText   = "lpUndoBeg in <SMWndProc>";
#endif
            lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].IncrSize = DEF_UNDOBUF_INCRSIZE * sizeof (UNDO_BUF);
            lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].MaxSize  = DEF_UNDOBUF_MAXSIZE  * sizeof (UNDO_BUF);
            lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].IniAddr  = (LPVOID)
            lpUndoBeg =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            if (!lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"WM_CREATE:  VirtualAlloc for <lpUndoBeg> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_UNDOBEG].IniAddr,
                            DEF_UNDOBUF_INITSIZE * sizeof (lpUndoBeg[0]),
                            MEM_COMMIT,
                            PAGE_READWRITE);
            // Save in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_BEG, (__int3264) (LONG_PTR) lpUndoBeg);
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_NXT, (__int3264) (LONG_PTR) lpUndoBeg);
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_LST, (__int3264) (LONG_PTR) lpUndoBeg);
////////////SetWindowLongW (hWnd, GWLSF_UNDO_GRP, 0);    // Already zero

            // Start with an initial action of nothing
            AppendUndo (hWnd,                       // SM Window handle
                        GWLSF_UNDO_NXT,             // Offset in hWnd extra bytes of lpUndoNxt
                        undoNone,                   // Action
                        0,                          // Beginning char position
                        0,                          // Ending    ...
                        UNDO_NOGROUP,               // Group index
                        0);                         // Character
            // Save incremented starting ptr in window extra bytes
            SetWindowLongPtrW (hWnd, GWLSF_UNDO_BEG, (__int3264) (LONG_PTR) ++lpUndoBeg);

////////////// *************** lptkStackBase ***************************
////////////
////////////// Lock the memory to get a ptr to it
////////////lpMemPTD = MyGlobalLock (hGlbPTD);
////////////
////////////// Allocate virtual memory for the token stack used in parsing
////////////lpMemPTD->lptkStackBase =
////////////  GuardAlloc (NULL,         // Any address
////////////              DEF_TOKENSTACK_MAXSIZE * sizeof (TOKEN),
////////////              MEM_RESERVE,
////////////              PAGE_READWRITE);
////////////// We no longer need this ptr
////////////MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
////////////
////////////if (!???)
////////////{
////////////    // ***FIXME*** -- WS FULL before we got started???
////////////    DbgMsgW (L"WM_CREATE:  VirtualAlloc for <lptkStackBase> failed");
////////////
////////////    goto WM_CREATE_FAIL;    // Mark as failed
////////////} // End IF
////////////
////////////// Commit the intial size
////////////VirtualAlloc (p,
////////////              DEF_TOKENSTACK_INITSIZE * sizeof (TOKEN),
////////////              MEM_COMMIT,
////////////              PAGE_READWRITE);

            // *************** hGlbNum *********************************

            // Allocate storage for hGlbNum
            hGlbTmp = MyGlobalAlloc (GHND, DEF_NUM_INITSIZE * sizeof (char));
            if (!hGlbTmp)
                goto WM_CREATE_FAIL;

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Set initial limit for hGlbNum
            lpMemPTD->iNumLim = DEF_NUM_INITSIZE;

            // Save back into PTD var
            lpMemPTD->hGlbNum = hGlbTmp;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // *************** hGlbStr *********************************

            // Allocate storage for hGlbStr
            hGlbTmp = MyGlobalAlloc (GHND, DEF_STR_INITSIZE * sizeof (APLCHAR));
            if (!hGlbTmp)
                goto WM_CREATE_FAIL;

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Set initial limit for hGlbNum
            lpMemPTD->iStrLim = DEF_STR_INITSIZE;

            // Save back into PTD var
            lpMemPTD->hGlbStr = hGlbTmp;

////////////// We no longer need this ptr
////////////MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // *************** htsPTD.lpHshTab *************************

////////////// Lock the memory to get a ptr to it
////////////lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the hash table
#ifdef DEBUG
            lpLclMemVirtStr[PTDMEMVIRT_HSHTAB].lpText   = "lpMemPTD->htsPTD.lpHshTab in <SMWndProc>";
#endif
            lpLclMemVirtStr[PTDMEMVIRT_HSHTAB].IncrSize = DEF_HSHTAB_INCRSIZE * sizeof (lpMemPTD->htsPTD.lpHshTab[0]);
            lpLclMemVirtStr[PTDMEMVIRT_HSHTAB].MaxSize  = DEF_HSHTAB_MAXSIZE  * sizeof (lpMemPTD->htsPTD.lpHshTab[0]);
            lpLclMemVirtStr[PTDMEMVIRT_HSHTAB].IniAddr  = (LPVOID)
            lpMemPTD->htsPTD.lpHshTab =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_HSHTAB].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!lpLclMemVirtStr[PTDMEMVIRT_HSHTAB].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"WM_CREATE:  VirtualAlloc for <lpMemPTD->lpHshTab> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_HSHTAB]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_HSHTAB].IniAddr,
                            DEF_HSHTAB_INITSIZE * sizeof (lpMemPTD->htsPTD.lpHshTab[0]),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Initialize the principal hash entry (1st one in each block).
            // This entry is never overwritten with an entry with a
            //   different hash value.
            for (i = 0; i < DEF_HSHTAB_INITSIZE; i += DEF_HSHTAB_EPB)
                lpMemPTD->htsPTD.lpHshTab[i].htFlags.PrinHash = TRUE;

            // Initialize the next & prev same HTE values
            for (i = 0; i < DEF_HSHTAB_INITSIZE; i++)
            {
                lpMemPTD->htsPTD.lpHshTab[i].NextSameHash =
                lpMemPTD->htsPTD.lpHshTab[i].PrevSameHash = LPHSHENTRY_NONE;
            } // End FOR

            // Initialize next split entry
            lpMemPTD->htsPTD.lpHshTabSplitNext = lpMemPTD->htsPTD.lpHshTab;

////////////// We no longer need this ptr
////////////MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // *************** lpSymTab ********************************

////////////// Lock the memory to get a ptr to it
////////////lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the symbol table
#ifdef DEBUG
            lpLclMemVirtStr[PTDMEMVIRT_SYMTAB].lpText   = "lpMemPTD->lpSymTab in <SMWndProc>";
#endif
            lpLclMemVirtStr[PTDMEMVIRT_SYMTAB].IncrSize = DEF_SYMTAB_INCRSIZE * sizeof (lpMemPTD->lpSymTab[0]);
            lpLclMemVirtStr[PTDMEMVIRT_SYMTAB].MaxSize  = DEF_SYMTAB_MAXSIZE  * sizeof (lpMemPTD->lpSymTab[0]);
            lpLclMemVirtStr[PTDMEMVIRT_SYMTAB].IniAddr  = (LPVOID)
            lpMemPTD->lpSymTab =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_SYMTAB].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!lpLclMemVirtStr[PTDMEMVIRT_SYMTAB].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"WM_CREATE:  VirtualAlloc for <lpMemPTD->lpSymTab> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_SYMTAB]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_SYMTAB].IniAddr,
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

            if (lpMemPTD->steZero    EQ NULL
             || lpMemPTD->steOne     EQ NULL
             || lpMemPTD->steBlank   EQ NULL
             || lpMemPTD->steNoValue EQ NULL)
            {
                // ***FIXME*** -- SYMBOL TABLE FULL before we got started???
                DbgMsgW (L"WM_CREATE:  SymTabAppendPermXXX failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Set the flags for the NoValue entry
            lpMemPTD->steNoValue->stFlags.Perm       = TRUE;
            lpMemPTD->steNoValue->stFlags.ObjName    = OBJNAME_NONE;
            lpMemPTD->steNoValue->stFlags.stNameType = NAMETYPE_UNK;

            Assert (IsSymNoValue (lpMemPTD->steNoValue));

////////////// We no longer need this ptr
////////////MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // *************** State Indicator Stack *******************
////////////// Lock the memory to get a ptr to it
////////////lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the State Indicator Stack
#ifdef DEBUG
            lpLclMemVirtStr[PTDMEMVIRT_SIS].lpText   = "lpMemPTD->lpSISBeg in <SMWndProc>";
#endif
            lpLclMemVirtStr[PTDMEMVIRT_SIS].IncrSize = DEF_SIS_INCRSIZE * sizeof (SYMENTRY);
            lpLclMemVirtStr[PTDMEMVIRT_SIS].MaxSize  = DEF_SIS_MAXSIZE  * sizeof (SYMENTRY);
            lpLclMemVirtStr[PTDMEMVIRT_SIS].IniAddr  = (LPVOID)
            lpMemPTD->lpSISBeg = lpMemPTD->lpSISNxt =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_SIS].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!lpLclMemVirtStr[PTDMEMVIRT_SIS].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"WM_CREATE:  VirtualAlloc for <lpMemPTD->lpStateInd> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_SIS]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_SIS].IniAddr,
                            DEF_SIS_INITSIZE * sizeof (SYMENTRY),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** YYRes Buffer ****************************
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the YYRes buffer
#ifdef DEBUG
            lpLclMemVirtStr[PTDMEMVIRT_YYRES].lpText   = "lpMemPTD->lpYYRes in <SMWndProc>";
#endif
            lpLclMemVirtStr[PTDMEMVIRT_YYRES].IncrSize = DEF_YYRES_INCRSIZE * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_YYRES].MaxSize  = DEF_YYRES_MAXSIZE  * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_YYRES].IniAddr  = (LPVOID)
            lpMemPTD->lpYYRes =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_YYRES].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!lpLclMemVirtStr[PTDMEMVIRT_YYRES].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"WM_CREATE:  VirtualAlloc for <lpMemPTD->lpYYRes> failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_YYRES]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_YYRES].IniAddr,
                            DEF_YYRES_INITSIZE * sizeof (PL_YYSTYPE),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** Strand Accumulators *********************
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the VAR strand accumulator
#ifdef DEBUG
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].lpText   = "lpMemPTD->lpStrand[STRAND_VAR] in <SMWndProc>";
#endif
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].IncrSize = DEF_STRAND_INCRSIZE * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].MaxSize  = DEF_STRAND_MAXSIZE  * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].IniAddr  = (LPVOID)
            lpMemPTD->lpStrand[STRAND_VAR] =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"InitInstance:  VirtualAlloc for <lpMemPTD->lpStrandVar> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_STRAND_VAR].IniAddr,
                            DEF_STRAND_INITSIZE * sizeof (PL_YYSTYPE),
                            MEM_COMMIT,
                            PAGE_READWRITE);
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the FCN strand accumulator
#ifdef DEBUG
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].lpText   = "lpMemPTD->lpStrand[STRAND_FCN] in <SMWndProc>";
#endif
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].IncrSize = DEF_STRAND_INCRSIZE * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].MaxSize  = DEF_STRAND_MAXSIZE  * sizeof (PL_YYSTYPE);
            lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].IniAddr  = (LPVOID)
            lpMemPTD->lpStrand[STRAND_FCN] =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"InitInstance:  VirtualAlloc for <lpMemPTD->lpStrandFcn> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_STRAND_FCN].IniAddr,
                            DEF_STRAND_INITSIZE * sizeof (PL_YYSTYPE),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** lpwszFormat *****************************
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the WCHAR Formatting storage
#ifdef DEBUG
            lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].lpText   = "lpMemPTD->lpwszFormat in <SMWndProc>";
#endif
            lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].IncrSize = DEF_WFORMAT_INCRSIZE * sizeof (WCHAR);
            lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].MaxSize  = DEF_WFORMAT_MAXSIZE  * sizeof (WCHAR);
            lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].IniAddr  = (LPVOID)
            lpMemPTD->lpwszFormat =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"InitInstance:  GuardAlloc for <lpwszFormat> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_WSZFORMAT].IniAddr,
                            DEF_WFORMAT_INITSIZE * sizeof (WCHAR),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** lpwszTemp *******************************
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Allocate virtual memory for the WCHAR Formatting storage
#ifdef DEBUG
            lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].lpText   = "lpMemPTD->lpwszTemp in <SMWndProc>";
#endif
            lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].IncrSize = DEF_WPTDTEMP_INCRSIZE * sizeof (WCHAR);
            lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].MaxSize  = DEF_WPTDTEMP_MAXSIZE  * sizeof (WCHAR);
            lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].IniAddr  = (LPVOID)
            lpMemPTD->lpwszBaseTemp =
            lpMemPTD->lpwszTemp =
              GuardAlloc (NULL,             // Any address
                          lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].MaxSize,
                          MEM_RESERVE,
                          PAGE_READWRITE);
            // Save the maximum size
            lpMemPTD->uTempMaxSize = lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].MaxSize;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            if (!lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].IniAddr)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsgW (L"InitInstance:  GuardAlloc for <lpwszTemp> failed");

                return FALSE;           // Mark as failed
            } // End IF

            // Link this struc into the chain
            LinkMVS (&lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP]);

            // Commit the intial size
            MyVirtualAlloc (lpLclMemVirtStr[PTDMEMVIRT_WSZTEMP].IniAddr,
                            DEF_WPTDTEMP_INITSIZE * sizeof (WCHAR),
                            MEM_COMMIT,
                            PAGE_READWRITE);

            // *************** System Names ****************************

            // Initialize all system names (functions and variables) as reserved
            if (!InitSystemNames_EM ())
            {
                DbgMsgW (L"WM_CREATE:  InitSystemNames_EM failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // *************** System Vars *****************************

            // Initialize all system vars
            if (!InitSystemVars ())
            {
                DbgMsgW (L"WM_CREATE:  InitSystemVars failed");

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

            // *************** Edit Control ****************************
            // Create an Edit Control within which we can enter expressions
            hWndEC =
              CreateWindowExW (0L,                  // Extended styles
                               LECWNDCLASS,         // Class name
                               NULL,                // Initial text
                               0
                             | WS_CHILD
                             | WS_CLIPCHILDREN
                             | WS_VSCROLL
                             | ES_MULTILINE
                             | ES_WANTRETURN
                             | ES_NOHIDESEL         // ***TESTME***
                             | ES_AUTOHSCROLL
                             | ES_AUTOVSCROLL
                               ,                    // Styles
                               0,                   // X-position
                               0,                   // Y-...
                               CW_USEDEFAULT,       // Width
                               CW_USEDEFAULT,       // Height
                               hWnd,                // Parent window
                               (HMENU) IDWC_SM_EC,  // ID
                               _hInstance,          // Instance
                               0);                  // lParam
            if (hWndEC EQ NULL)
            {
                MB (pszNoCreateSMEditCtrl);

                goto WM_CREATE_FAIL;    // Mark as failed
            } // End IF

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

////////////// Set the soft-break flag (not supported by WineHQ edit ctrl)
////////////SendMessageW (hWndEC, EM_FMTLINES, TRUE, 0);

            // Paint the window
            ShowWindow (hWndEC, SW_SHOWNORMAL);
            UpdateWindow (hWndEC);

            // *************** Magic Functions *************************

            // Initialize all magic functions
            if (!InitMagicFunctions (hGlbPTD, hWndEC))
            {
                DbgMsgW (L"WM_CREATE:  InitMagicFunctions failed");

                goto WM_CREATE_FAIL_UNHOOK;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // ***FIXME*** -- For some reason, the
            //                background of the MC window
            //                doesn't get painted, so I'm
            //                using this kludge for now.
            InvalidateRect (lpMemPTD->hWndMC, NULL, TRUE);

            // Save the bExecLX flag
            lpMemPTD->bExecLX = (*(LPSM_CREATESTRUCTW *) &lpMDIcs->lParam)->bExecLX;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // Display the )LOAD message once and only once
            if (!bLoadMsgDisp)
            {
                // Display leading Copyright text and disclaimer
                AppendLine (L"NARS2000 Copyright (C) 2006-8 Sudley Place Software.",              FALSE, TRUE);
                AppendLine (L"This program comes with ABSOLUTELY NO WARRANTY; for details visit", TRUE , TRUE);
                AppendLine (L"  http://www.nars2000.org/License.html, or click on Help > About.", TRUE , TRUE);
                AppendLine (L"This is free software, and you are welcome to redistribute it",     TRUE , TRUE);
                AppendLine (L"  under certain conditions; visit the above link for details.",     TRUE , TRUE);

                // Mark as displayed so we don't do it again
                bLoadMsgDisp = TRUE;
            } // End IF

            // Attempt to load the workspace
            if (!LoadWorkspace_EM ((*(LPSM_CREATESTRUCTW *) &lpMDIcs->lParam)->hGlbDPFE, hWndEC))
            {
                // If we're loading a workspace from the command line, we can't afford to fail
                if (wszLoadFile[0]
                 && !LoadWorkspace_EM (NULL, hWndEC))   // Attempt to load a CLEAR WS
                    goto LOAD_WORKSPACE_FAIL;           // If that fails, give up
            } // End IF

            // Zap so the above test doesn't succeed again
            wszLoadFile[0] = L'\0';

            // Tell the window to finish initialization
            PostMessageW (hWnd, MYWM_INIT_EC, 0, 0);

            lResult = FALSE;        // We handled the msg

            goto NORMAL_EXIT;       // Join common code

WM_CREATE_FAIL_UNHOOK:
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Unhook the LclEditCtrlWndProc
            SetWindowLongPtrW (hWndEC,
                               GWL_WNDPROC,
                               (__int3264) (LONG_PTR) lpMemPTD->lpfnOldEditCtrlWndProc);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
WM_CREATE_FAIL:
            // Send a constant message to the previous tab
            SendMessageLastTab (ERRMSG_TABS_FULL APPEND_NAME, hGlbPTD);
LOAD_WORKSPACE_FAIL:
NORMAL_EXIT:
            // Free the workspace global
            MyGlobalFree ((*(LPSM_CREATESTRUCTW *) &lpMDIcs->lParam)->hGlbDPFE);

            return lResult;         // Mark as failed
        } // End WM_CREATE
#undef  lpMDIcs

        case WM_PARENTNOTIFY:       // fwEvent = LOWORD(wParam);  // Event flags
                                    // idChild = HIWORD(wParam);  // Identifier of child window
                                    // lValue = lParam;           // Child handle, or cursor coordinates
#define fwEvent     (LOWORD (wParam))
#define idChild     (HIWORD (wParam))

            // Check for WM_CREATE from the Edit Control/Debugger
            switch (fwEvent)
            {
                case WM_CREATE:
                    // Split cases based upon the child ID
                    switch (idChild)
                    {
                        case IDWC_SM_EC:
                            PostMessageW (hWnd, MYWM_INIT_EC, 0, 0);

                            break;
                    } // End SWITCH

                    break;

                case WM_LBUTTONDOWN:
                {
                    UINT uCharPos;              // Character position

                    // Ask the Edit Control what char is under the mouse cursor
                    uCharPos = (UINT) SendMessageW (hWndEC, EM_CHARFROMPOS, 0, lParam);

                    // If it's valid, ...
                    if (uCharPos NE NEG1U)
                        // Move to that line
                        MoveToLine (HIWORD (uCharPos),
                                    hGlbPTD,
                                    hWndEC);
                    break;
                } // End WM_LBUTTONDOWN

                default:
                    break;
            } // End SWITCH

            return FALSE;           // We handled the msg

#undef  idChild
#undef  fwEvent

#ifdef DEBUG
        case MYWM_INIT_SMDB:
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // If the Debugger window handle is active, ...
            if (lpMemPTD->hWndDB)
                PostMessageW (hWnd, MYWM_KEYDOWN, VK_F9, 0);
            else
                PostMessageW (hWnd, MYWM_INIT_SMDB, 0, 0);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            break;
#endif
        case MYWM_INIT_EC:
            // Wait for the third receipt of this message
            //   so we are sure everything is initialized
            switch ((__int3264) (LONG_PTR)GetPropW (hWnd, L"INIT_EC"))
            {
                case 0:
                    SetPropW (hWnd, L"INIT_EC", (HANDLE) 1);

                    return FALSE;

                case 1:
                    SetPropW (hWnd, L"INIT_EC", (HANDLE) 2);

                    return FALSE;

                case 2:
                    RemovePropW (hWnd, L"INIT_EC");

                    break;

                defstop
                    break;
            } // End SWITCH

            // Make sure we can communicate between windows
            AttachThreadInput (GetCurrentThreadId (), dwMainThreadId, TRUE);

            // Tell the Edit Control about its font
            SendMessageW (hWndEC, WM_SETFONT, (WPARAM) hFontSM, MAKELPARAM (TRUE, 0));
#ifdef DEBUG
            PostMessageW (hWnd, MYWM_INIT_SMDB, 0, 0);
#endif
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // If requested, execute []LX
            if (lpMemPTD->bExecLX)
            {
                // Reset the flag
                lpMemPTD->bExecLX = FALSE;

                // Execute the statement in immediate execution mode
                ImmExecStmt (WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"LX",    // Ptr to line to execute
                             4,                                         // NELM of line to execute
                             FALSE,                                     // TRUE iff free the line on completion
                             FALSE,                                     // TRUE iff wait until finished
                             hWndEC,                                    // Edit Control window handle
                             TRUE);                                     // TRUE iff errors are acted upon
            } else
            // If the SI level is for Quad Input
            if (lpMemPTD->lpSISCur && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                PostMessageW (hWnd, MYWM_QUOTEQUAD, FALSE, 15);
            else
                // Display the default prompt
                DisplayPrompt (hWndEC, 1);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // Ensure the SM has the focus
            SetFocus (hWnd);

            return FALSE;           // We handled the msg

        case MYWM_QUOTEQUAD:        // bQuoteQuad = (BOOL) wParam
                                    // TRUE iff Quote-Quad input, FALSE if Quad input
                                    // iPromptIndex = (int) lParam;
            if (!wParam)
            {
                AppendLine (wszQuadInput, FALSE, TRUE);
                DisplayPrompt (hWndEC, lParam);
            } // End IF

            return FALSE;           // We handled the msg

        case MYWM_ERRMSG:           // lpwErrMsg = (LPWCHAR) lParam;
            // Display the error message, replacing the current
            //   line as it might have a prompt on it
            ReplaceLastLineCRPmt (*(LPWCHAR *) &lParam);

            return FALSE;           // We handled the msg

        case MYWM_SAVE_WS:          // 0 = wParam;
                                    // lpwWSID = (LPWCHAR) lParam;
            // Save the workspace
            return CmdSave_EM (*(LPWCHAR *) &lParam);

#define fwSizeType  wParam
#define nWidth      (LOWORD (lParam))
#define nHeight     (HIWORD (lParam))
        case WM_SIZE:               // fwSizeType = wParam;      // Resizing flag
                                    // nWidth = LOWORD(lParam);  // Width of client area
                                    // nHeight = HIWORD(lParam); // Height of client area
            if (fwSizeType EQ SIZE_MAXIMIZED)
                InvalidateRect (hWndEC, NULL, TRUE);

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
            {
                ActivateMDIMenu (hMenuSM, hMenuSMWindow);
                SetFocus (hWnd);
            } // End IF

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
        case MYWM_PASTE_APL:
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
            UBOOL bRet;

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
                            UINT    uLastNum;
                            LPWCHAR lpwTmpLine;

                            // Get the line length of a given line #
                            uLineLen = GetLineLength (hWndEC, uLineNum);

                            // Allocate space for the line including a terminating CR/LF/zero
                            lpwTmpLine =
                              MyGlobalAlloc (GPTR, (uLineLen + 3) * sizeof (lpwTmpLine[0]));

                            // Tell EM_GETLINE maximum # chars in the buffer
                            ((LPWORD) lpwTmpLine)[0] = uLineLen;

                            // Get the current line
                            SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwTmpLine);

                            // Append CRLF
                            lstrcatW (lpwTmpLine, L"\r\n");

                            // Move the text caret to the end of the buffer
                            MoveCaretEOB (hWndEC);

                            // Get the # of the last line
                            uLastNum = (UINT) SendMessageW (hWndEC, EM_LINEFROMCHAR, (WPARAM) -1, 0);

                            // Replace the last line in the buffer
                            ReplaceLine (hWndEC, lpwTmpLine, uLastNum);

                            // We no longer need this storage
                            MyGlobalFree (lpwTmpLine); lpwTmpLine = NULL;

                            // Lock the memory to get a ptr to it
                            lpwCurLine = MyGlobalLock (lpMemPTD->hGlbCurLine);

                            // Restore the original of the current line
                            ReplaceLine (hWndEC, lpwCurLine, uLineNum);

                            // We no longer need this ptr
                            MyGlobalUnlock (lpMemPTD->hGlbCurLine); lpwCurLine = NULL;

                            // Move the text caret to the end of the buffer
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

                    // Set (new) current line
                    MoveToLine (--uLineNum, hGlbPTD, hWndEC);

                    break;

                case VK_DOWN:
                    // Get the # lines in the Edit Control
                    uLineCnt = (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);

                    // If the next line is out of range, exit
                    if (uLineCnt <= (uLineNum + 1))
                        break;

                    // Set (new) current line
                    MoveToLine (++uLineNum, hGlbPTD, hWndEC);

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
                    DisplayHshTab (FALSE);
                    DisplayHshTab (TRUE);

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

                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

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
#define lpnmEC  (*(LPNMEDITCTRL *) &lParam)

            // Ensure from Edit Ctrl
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

#define wNotifyCode     (HIWORD (wParam))
#define wID             (LOWORD (wParam))
#define hWndCtrl        (*(HWND *) &lParam)
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
                    SetWindowLongW (hWnd, GWLSF_CHANGED, TRUE);

                    break;

                case EN_MAXTEXT:    // idEditCtrl = (int) LOWORD(wParam); // Identifier of edit control
                                    // hwndEditCtrl = (HWND) lParam;      // Handle of edit control
                    // The edit control has exceed its maximum # chars

                    // The default maximum is 32K, so we increase it by that amount
                    Assert (hWndEC NE 0);
                    SendMessageW (hWndEC,
                                  EM_SETLIMITTEXT,
                                  (UINT) SendMessageW (hWndEC, EM_GETLIMITTEXT, 0, 0) + 32*1024, 0);
                    break;

////            case EN_SETFOCUS:   // 0x0100
////            case EN_KILLFOCUS:  // 0x0200
////            case EN_CHANGE:     // 0x0300
////            case EN_UPDATE:     // 0x0400
////            case EN_ERRSPACE:   // 0x0500
////            case EN_MAXTEXT:    // 0x0501
////            case EN_HSCROLL:    // 0x0601
////            case EN_VSCROLL:    // 0x0602
////                break;
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

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);
#ifdef DEBUG
            // If the debugger is still active, close it
            if (lpMemPTD->hWndDB)
                SendMessageW (lpMemPTD->hWndMC, WM_MDIDESTROY, (WPARAM) lpMemPTD->hWndDB, 0);
#endif
            // *************** hGlbStr *********************************
            if (lpMemPTD->hGlbStr)
            {
                MyGlobalFree (lpMemPTD->hGlbStr); lpMemPTD->hGlbStr = NULL;
            } // End IF

            // *************** hGlbNum *********************************
            if (lpMemPTD->hGlbNum)
            {
                MyGlobalFree (lpMemPTD->hGlbNum); lpMemPTD->hGlbNum = NULL;
            } // End IF

////////////// *************** lptkStackBase ***************************
////////////if (lpMemPTD->lptkStackBase)
////////////{
////////////    MyVirtualFree (lpMemPTD->lptkStackBase, 0, MEM_RELEASE); lpMemPTD->lptkStackBase = NULL;
////////////} // End IF

            // *************** hGlbCurLine *****************************
            if (lpMemPTD->hGlbCurLine)
            {
                MyGlobalFree (lpMemPTD->hGlbCurLine); lpMemPTD->hGlbCurLine = NULL;
            } // End IF

            // *************** []ERROR/[]ES ****************************
            // *************** Undo Buffer *****************************
            // *************** lpHshTab ********************************
            // *************** lpSymTab ********************************
            // *************** SIS *************************************
            // *************** YYRES ***********************************
            // *************** Strand Accumulator - VAR ****************
            // *************** Strand Accumulator - FCN ****************
            // *************** lpwszFormat *****************************

            // Get the MemVirtStr ptr
            (HANDLE_PTR) lpLclMemVirtStr = GetWindowLongPtrW (hWnd, GWLSF_LPMVS);
            if (lpLclMemVirtStr)
            {
                for (uCnt = 0; uCnt < PTDMEMVIRT_LENGTH; uCnt++)
                if (lpLclMemVirtStr[uCnt].IniAddr)
                {
                    // Free the virtual storage
                    MyVirtualFree (lpLclMemVirtStr[uCnt].IniAddr, 0, MEM_RELEASE); lpLclMemVirtStr[uCnt].IniAddr = NULL;

                    // Unlink this entry from the chain
                    UnlinkMVS (&lpLclMemVirtStr[uCnt]);
                } // End FOR/IF

                // Free the virtual storage
                MyVirtualFree (lpLclMemVirtStr, 0,MEM_RELEASE); lpLclMemVirtStr = NULL;

                // Zap the window extra in case we're called again???
                SetWindowLongPtrW (hWnd, GWLSF_LPMVS, 0);
            } // End IF

            // Uninitialize window-specific resources
            SM_Delete (hWnd);

#ifndef UNISCRIBE
            // Release the FontLink ptr
            if (lpMemPTD->lpFontLink)
            {
                IMLangFontLink_Release (lpMemPTD->lpFontLink); lpMemPTD->lpFontLink = NULL;
            } // End IF
#endif
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // Tell the thread to quit, too
            PostQuitMessage (0);

#ifndef UNISCRIBE
            // Uninitialize the OLE libraries
            CoUninitialize ();
#endif
            break;
    } // End SWITCH

////LCLODSAPI ("SMZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProcW (hWnd, message, wParam, lParam);
} // End SMWndProc
#undef  APPEND_NAME


//***************************************************************************
//  $MoveToLine
//
//  Common routine when moving the text cursor to a new line
//***************************************************************************

void MoveToLine
    (UINT    uLineNum,                  // The given line #
     HGLOBAL hGlbPTD,                   // PerTabData global memory handle
     HWND    hWndEC)                    // Edit Control window handle

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData
    UINT         uLineLen;              // Line length
    LPWCHAR      lpwCurLine;            // Ptr to current line global memory

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the length of the (new) current line
    uLineLen = GetLineLength (hWndEC, uLineNum);

    // If there's a previous current line global memory handle,
    //   free it
    if (lpMemPTD->hGlbCurLine)
    {
        MyGlobalFree (lpMemPTD->hGlbCurLine); lpMemPTD->hGlbCurLine = NULL;
    } // End IF

    // Allocate space for the line including a terminating zero
    lpMemPTD->hGlbCurLine =
      MyGlobalAlloc (GHND, (uLineLen + 1) * sizeof (lpwCurLine[0]));

    // Lock the memory to get a ptr to it
    lpwCurLine = MyGlobalLock (lpMemPTD->hGlbCurLine);

    // Tell EM_GETLINE maximum # chars in the buffer
    ((LPWORD) lpwCurLine)[0] = uLineLen;

    // Save the (new) current line
    SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwCurLine);

    // We no longer need this ptr
    MyGlobalUnlock (lpMemPTD->hGlbCurLine); lpwCurLine = NULL;

    // Reset the changed line flag
    SetWindowLongW (lpMemPTD->hWndSM, GWLSF_CHANGED, FALSE);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End MoveToLine


//***************************************************************************
//  End of File: sessman.c
//***************************************************************************
