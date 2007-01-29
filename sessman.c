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

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

/*

The Session Manager (SM) window consists of <iNumLines> lines of
APL statements, some block of which are displayed in the window
at any one time.

When the cursor moves to a line, the contents of the lpGlbHist
value are copied to <lpwszCurLine>.

1.  Assume that the user edits the line:
    a.  The edit changes are made to <lpwszCurLine>.
    b.  If the user hits Enter, the contents of
        <lpwszCurLine> are copied to lpGlbHist[iLastLine].
    c.  If the user moves the cursor off the current line,
        the contents of <lpwszCurLine> are copied to
        lpGlbHist[iCurLine].hGlb.
 */

#define SCROLL_FIRST        0           // Constants for ScrollHorz & ScrollVert
#define SCROLL_LAST         1           // ...

////LPEXCEPTION_POINTERS lpExp;

COLORREF crTextColor = DEF_TEXT_FG_COLOR,
         crBkColor   = DEF_TEXT_BG_COLOR;

WCHAR wszEmpty[] = {L'\0'};     // Empty wide string

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
//  SetVScrollRange
//
//  Set the vertical scroll range
//***************************************************************************

void SetVScrollRange
    (void)

{
    SetScrollRange (hWndSM, SB_VERT, iFirstBufferLine, iLastValidLine + 1, TRUE);
} // End SetVScrollRange


//***************************************************************************
//  SetHScrollRange
//
//  Set the horizontal scroll range
//***************************************************************************

void SetHScrollRange
    (void)

{
    SetScrollRange (hWndSM, SB_HORZ, iFirstBufferChar, iLastBufferChar, TRUE);
} // End SetHScrollRange


//***************************************************************************
//  SetVScrollPos
//
//  Set the vertical scroll position
//***************************************************************************

void SetVScrollPos
    (void)

{
    SetScrollPos (hWndSM, SB_VERT, iFirstWindowLine, TRUE);
} // End SetVScrollPos


//***************************************************************************
//  SetHScrollPos
//
//  Set the horizontal scroll position
//***************************************************************************

void SetHScrollPos
    (void)

{
    SetScrollPos (hWndSM, SB_HORZ, iFirstWindowChar, TRUE);
} // End SetHScrollPos


//***************************************************************************
//  ScrollVert
//
//  Scroll the window vertically
//***************************************************************************

void ScrollVert
    (BOOL bScroll,
     int iLineCnt)

{
    int iNewWindowLine;

    // Set new first & last window lines
    switch (bScroll)
    {
        case SCROLL_FIRST:
////////////wsprintf (lpszTemp,
////////////          "VSCROLL_FIRSTa:  %d",
////////////          iLineCnt);
////////////DbgMsg (lpszTemp);

            // Calculate the new position
            iNewWindowLine = iFirstWindowLine + iLineCnt;

            if (iNewWindowLine < iFirstBufferLine)
                iLineCnt += iFirstBufferLine - iNewWindowLine;

            if ((iNewWindowLine + nWindowLines) > iLastBufferLine)
                iLineCnt += iLastBufferLine - (iNewWindowLine + nWindowLines);

            iFirstWindowLine += iLineCnt;
            iLastWindowLine   = iFirstWindowLine + nWindowLines;

////        wsprintf (lpszTemp,
////                  "VSCROLL_FIRSTz:  %d",
////                  iLineCnt);
////        DbgMsg (lpszTemp);

            break;

        case SCROLL_LAST:
////////////wsprintf (lpszTemp,
////////////          "VSCROLL_LASTa:  %d",
////////////          iLineCnt);
////////////DbgMsg (lpszTemp);

            // Calculate the new position
            iNewWindowLine = iLastWindowLine + iLineCnt;

            if (iNewWindowLine < iFirstBufferLine)
                iLineCnt += iFirstBufferLine - iNewWindowLine;

            if ((iNewWindowLine + nWindowLines) > iLastBufferLine)
                iLineCnt += iLastBufferLine - (iNewWindowLine + nWindowLines);

            iLastWindowLine += iLineCnt;
            iFirstWindowLine = iLastWindowLine - nWindowLines;

////        wsprintf (lpszTemp,
////                  "VSCROLL_LASTz:  %d",
////                  iLineCnt);
////        DbgMsg (lpszTemp);

            break;
    } // End SWITCH

    // For vertical scrolling only, don't let
    //   the first line get beyond iLastValidLine + 1
    if (iFirstWindowLine > (iLastValidLine + 1))
    {
        iFirstWindowLine = iLastValidLine + 1;
        iLastWindowLine  = iFirstWindowLine + nWindowLines;
    } // End IF

    // Ensure first & last are within buffer limits
    iFirstWindowLine  = max (iFirstWindowLine, iFirstBufferLine);
    iLastWindowLine   = min (iLastWindowLine,  iLastBufferLine);

    // If the current line has changed,
    //   save it into the history buffer
    if (bCurLineChanged)
        ReplaceLine (iCurLine);

    // Ensure the caret is always visible
    if (iCurLine < iFirstWindowLine)
        iCurLine = iFirstWindowLine;
    else
    if (iCurLine > iLastWindowLine)
        iCurLine = iLastWindowLine;

    // Copy the current line from the
    //   history buffer into lpwszCurLine;
    CopyLine (iCurLine);

    // In case we changed iCurLine or iFirstWindowLine,
    //   move the caret
    MoveCaretSM ();

    // As we changed iFirstWindowLine,
    //   move the thumb on the scrollbar
    SetVScrollPos ();

    // Redraw the entire client area
    InvalidateRect (hWndSM, NULL, TRUE);
} // End ScrollVert


//***************************************************************************
//  ScrollHorz
//
//  Scroll the window horizontally
//***************************************************************************

void ScrollHorz
    (BOOL bScroll,
     int iCharCnt)

{
    int iNewWindowChar;

    // Horizontal scrolling should be relative to the longest line in the history buffer
    // ***FIXME***


    // Set new first & last window chars
    switch (bScroll)
    {
        case SCROLL_FIRST:
////        wsprintf (lpszTemp,
////                  "HSCROLL_FIRST:  %d",
////                  iCharCnt);
////        DbgMsg (lpszTemp);

            // Calculate the new position
            iNewWindowChar = iFirstWindowChar + iCharCnt;

            if (iNewWindowChar < iFirstBufferChar)
                iCharCnt += iFirstBufferChar - iNewWindowChar;

            if ((iNewWindowChar + nWindowChars) > iLastBufferChar)
                iCharCnt += iLastBufferChar - (iNewWindowChar + nWindowChars);

            iFirstWindowChar += iCharCnt;
            iLastWindowChar   = iFirstWindowChar + nWindowChars;

            break;

        case SCROLL_LAST:
////        wsprintf (lpszTemp,
////                  "HSCROLL_LAST:  %d",
////                  iCharCnt);
////        DbgMsg (lpszTemp);

            // Calculate the new position
            iNewWindowChar = iLastWindowChar + iCharCnt;

            if (iNewWindowChar < iFirstBufferChar)
                iCharCnt += iFirstBufferChar - iNewWindowChar;

            if ((iNewWindowChar + nWindowChars) > iLastBufferChar)
                iCharCnt += iLastBufferChar - (iNewWindowChar + nWindowChars);

            iLastWindowChar += iCharCnt;
            iFirstWindowChar = iLastWindowChar - nWindowChars;

            break;
    } // End SWITCH

    // Ensure first & last are within buffer limits
    iFirstWindowChar  = max (iFirstWindowChar, iFirstBufferChar);
    iLastWindowChar   = min (iLastWindowChar,  iLastBufferChar);

    // In case we changed iFirstWindowChar,
    //   move the caret
    MoveCaretSM ();

    // In case we changed iFirstWindowChar,
    //   move the thumb on the scrollbar
    SetHScrollPos ();

    // Redraw the entire client area
    InvalidateRect (hWndSM, NULL, TRUE);
} // End ScrollHorz


//***************************************************************************
//  AppendLine
//
//  Append lpwszCurLine to the history buffer
//***************************************************************************

void AppendLine
    (LPWCHAR lpwszLine,
     BOOL    bLineCont)

{
    // Ensure there are enough lines or we need to wrap the buffer
    // ***FIXME*** -- implement a circular history buffer

    lstrcpyW (lpwszCurLine, lpwszLine);
    iCurLineLen = lstrlenW (lpwszCurLine);
    bCurLineChanged = TRUE;

////wsprintf (lpszTemp,
////          "lstrlenW (lpwszCurLine) = %d",
////          lstrlenW (lpwszCurLine));
////DbgMsg (lpszTemp);

    ReplaceLine (iLastValidLine + 1);   // iLastValidLine changed

    // Account for a new line
    iCurLine = iLastValidLine + 1;

    // As we changed iLastValidLine,
    //   reset the scrollbar range.
    SetVScrollRange ();

    // Initialize the Current Line
    InitCurLine (bLineCont);

    // As we changed iCurLine, move the caret
    MoveCaretSM ();

    // Scroll the window as necessary
    // The "+1" is to cover the line the cursor is on
    if ((iCurLine + 1) > iLastWindowLine)
        ScrollVert (SCROLL_LAST, (iCurLine + 1) - iLastWindowLine);
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
    (int iLine)

{
    LPGLBHIST lpGlbHist;
    HGLOBAL   hGlb;
    LPWCHAR   wszLine;

    // Get the global handle to the history buffer
    lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

////wsprintf (lpszTemp,
////          "iLine = %d, iLastValidLine = %d",
////          iLine,
////          iLastValidLine);
////DbgMsg (lpszTemp);

    // Free the old entry (if any)
    if (iLine <= iLastBufferLine
     && lpGlbHist[iLine].hGlb)
    {
        DbgGlobalFree (lpGlbHist[iLine].hGlb); lpGlbHist[iLine].hGlb = NULL;
    } else
    {
        // Increase iLastValidLine if necessary
        iLastValidLine = max (iLastValidLine, iLine);
    } // End IF/ELSE

    // Allocate memory for the last line
    hGlb = MyGlobalAlloc (GHND, (iCurLineLen + 1) * sizeof (WCHAR));

    if (!hGlb)
    {
        // ***FIXME*** -- WS FULL
        DbgMsg ("ASSERTION FAILED:  ReplaceLine:  MyGlobalAlloc failed");
    } else
    {
////////wsprintf (lpszTemp,
////////          "iLastValidLine = %d",
////////          iLastValidLine);
////////DbgMsg (lpszTemp);

        // Save the handle for later use
        lpGlbHist[iLine].hGlb = hGlb;

        // Get a ptr to the memory
        wszLine = (LPWCHAR) MyGlobalLock (hGlb);

        // Copy the contents of lpwszCurLine to global memory
        memmove (wszLine,
                 lpwszCurLine,
                 iCurLineLen * sizeof (WCHAR));

        bCurLineChanged = FALSE;

        // Unlock the handle
        MyGlobalUnlock (hGlb); wszLine = NULL;
    } // End IF/ELSE

    // Unlock the handle
    MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;

    // Redraw the line
    InvalidateLine (iLine);
} // End ReplaceLine
#undef  APPEND_NAME


//***************************************************************************
//  DeleteLine
//
//  Delete a line from the history buffer
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- DeleteLine"
#else
#define APPEND_NAME
#endif

void DeleteLine
    (int iLine)

{
    LPGLBHIST lpGlbHist;

    // Get the global handle to the history buffer
    lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

    // Free the old entry (if any)
    if (lpGlbHist[iLine].hGlb)
    {
        DbgGlobalFree (lpGlbHist[iLine].hGlb); lpGlbHist[iLine].hGlb = NULL;
    } // End IF

////wsprintf (lpszTemp,
////          "DeleteLine:  iLine = %d, iLastValidLine = %d",
////          iLine,
////          iLastValidLine);
////DbgMsg (lpszTemp);

    // Move the entries down over the deleted one
    if (iLine <= iLastValidLine)
    {
        memmove (&lpGlbHist[iLine],
                 &lpGlbHist[iLine + 1],
                 (iLastValidLine - iLine) * sizeof (WCHAR));
        lpGlbHist[iLastValidLine].hGlb = NULL;
        lpGlbHist[iLastValidLine].ContPrev =
        lpGlbHist[iLastValidLine].ContNext = FALSE;
    } // End IF

    if (iLine <= iLastValidLine)
    {
        // Set last valid line
        iLastValidLine--;
////////DbgMsg ("Decrementing iLastValidLine");

        // As we changed iLastValidLine,
        //   reset the scrollbar range.
        SetVScrollRange ();
    } // End IF

    // Unlock the handle
    MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;
} // End DeleteLine
#undef  APPEND_NAME


//***************************************************************************
//  CopyLine
//
//  Copy a line from the history buffer into lpwszCurLine.
//***************************************************************************

void CopyLine
    (int iLine)

{
    LPGLBHIST lpGlbHist;
    HGLOBAL   hGlb;
    LPWCHAR   wszLine;

    // Get the global handle to the history buffer
    lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

    // If the memory exists, copy the contents to the local buffer
    if (lpGlbHist[iLine].hGlb)
    {
        // Lock the memory
        hGlb = lpGlbHist[iLine].hGlb;
        wszLine = (LPWCHAR) MyGlobalLock (hGlb);

        // Set the line length
        iCurLineLen = lstrlenW (wszLine);

        // Copy the contents of global memory to lpwszCurLine
        memmove (lpwszCurLine,
                 wszLine,
                 (iCurLineLen + 1) * sizeof (WCHAR));
        // Unlock the handle
        MyGlobalUnlock (hGlb); wszLine = NULL;

        bCurLineChanged = FALSE;
    } else
        // Otherwise, initialize the local buffer
        InitCurLine (lpGlbHist->ContPrev);

    // Unlock the handle
    MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;
} // End CopyLine


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
//  DrawLineCont
//
//  Draw a line continuation char
//***************************************************************************

void DrawLineCont
    (HDC hDC,
     int iLineNum)

{
    DrawBitmap (hDC,
                hBitMapLineCont,
                0,
                (iLineNum * cyAveCharSM)
              + (cyAveCharSM - bmLineCont.bmHeight) / 2   // Vertically centered
               );
} // End DrawLineCont


//***************************************************************************
//  DrawLine
//
//  Draw a line in the window from within a WM_PAINT message.
//***************************************************************************

void DrawLine
     (LPWCHAR lpwszLine,
      int     iLine,
      HDC     hDC,
      BOOL    bLineCont)
{
    RECT rc;
    int iLen;

    iLen = lstrlenW (lpwszLine);

    rc.left   = iLCWidth;
    rc.right  = rc.left + cxAveCharSM * iLen;
    rc.top    = iLine   * cyAveCharSM;
    rc.bottom = rc.top  + cyAveCharSM;

    // Draw the text
    DrawTextW (hDC,
               lpwszLine,
               iLen,
               &rc,
               DT_NOPREFIX
             | DT_SINGLELINE
             | DT_NOCLIP
          );
    if (bLineCont)
        // Draw a continuation char
        DrawLineCont (hDC, iLine);
} // End DrawLine


//***************************************************************************
//  DrawBitmap
//
//  Draw a bitmap
//***************************************************************************

void DrawBitmap
    (HDC     hDC,
     HBITMAP hBitmap,
     UINT    xDstOrg,
     UINT    yDstOrg) // Destin bit origin (upper left corner)

{
    BITMAP  bm;
    HDC     hDCMem;
    POINT   ptSize, ptOrg;
    HBITMAP hBitmapMem, hBitmapOld;

    // Get the size of the bitmap
    GetObject (hBitmap, sizeof (BITMAP), (LPSTR) &bm);

    // Create a compatible DC and bitmap
    hDCMem = MyCreateCompatibleDC (hDC);    // Get device context handle
#ifdef USE_COPYIMAGE
    hBitmapMem = CopyImage (hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
    hBitmapOld = SelectObject (hDCMem, hBitmapMem);
#else
    // To avoid screen flicker, we use a temporary DC
    hBitmapMem = MyCreateCompatibleBitmap (hDC,
                                           bm.bmWidth,
                                           bm.bmHeight);
    hBitmapOld = SelectObject (hDCMem, hBitmapMem);

    {
        HDC hDCTmp;
        HBITMAP hBitmapTmp;

        // Create a temporary compatible DC
        // and select our bitmap into it
        hDCTmp = MyCreateCompatibleDC (hDC);
        hBitmapTmp = SelectObject (hDCTmp, hBitmap);

        // Copy the original bitmap from the temporary DC to the memory DC
        BitBlt (hDCMem,
                0,
                0,
                bm.bmWidth,
                bm.bmHeight,
                hDCTmp,
                0,
                0,
                SRCCOPY);
        SelectObject (hDCTmp, hBitmapTmp);
        MyDeleteDC (hDCTmp); hDCTmp = NULL;
    }
#endif
    SetMapMode (hDCMem, GetMapMode (hDC));  // Set the mapping mode

    // Convert the bitmap size from device units to logical units
    ptSize.x = bm.bmWidth;
    ptSize.y = bm.bmHeight;
    DPtoLP (hDC, &ptSize, 1);

    ptOrg.x = ptOrg.y = 0;
    DPtoLP (hDCMem, &ptOrg, 1);

    // Copy the memory DC to the screen DC
    BitBlt (hDC,
            xDstOrg, yDstOrg,
            ptSize.x, ptSize.y,
            hDCMem,
            ptOrg.x, ptOrg.y,
            SRCCOPY);
    // Put the old one in place before we delete the DC
    //   or we'll delete the new bitmap when we delete the DC.
    SelectObject (hDCMem, hBitmapOld);

    // Free resources
    MyDeleteObject (hBitmapMem); hBitmapMem = NULL;
    MyDeleteDC (hDCMem); hDCMem = NULL;
} // End DrawBitmap ()


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
//  InvalidateLine
//
//  Invalidate a single line so W calls WM_PAINT
//    to redraw the line.
//***************************************************************************

void InvalidateLine
    (int iLine)

{
    RECT rc;

    rc.left = 0;
    rc.right  = 65535;      // Use any non-zero value as the
                            //   code in WM_PAINT looks at the
                            //   rc.top and rc.bottom only
    rc.top    = (iLine - iFirstWindowLine) * cyAveCharSM;
    rc.bottom = rc.top                     + cyAveCharSM;

////wsprintf (lpszTemp,
////          "InvalidateLine:  %d, L = %d, T = %d, R = %d, B = %d",
////          iLine,
////          rc.left,
////          rc.top,
////          rc.right,
////          rc.bottom);
////DbgMsg (lpszTemp);

#if (defined (DEBUG)) && 0
    { // ***DEBUG***
        wsprintf (lpszTemp,
                  "InvalidateLine:  %d",
                  iLine);
        DbgMsg (lpszTemp);
    } // ***DEBUG*** END
#endif

    InvalidateRect (hWndSM, &rc, TRUE);
} // End InvalidateLine


//***************************************************************************
//  InvalidateRange
//
//  Invalidate a range of lines so W calls WM_PAINT
//    to redraw the line.
//***************************************************************************

void InvalidateRange
    (int iTopLine,
     int iBotLine)

{
    RECT rc;

    rc.left = 0;
    rc.right  = 65535;      // Use any non-zero value as the
                            //   code in WM_PAINT looks at the
                            //   rc.top and rc.bottom only
    rc.top    = (    iTopLine - iFirstWindowLine) * cyAveCharSM;
    rc.bottom = (1 + iBotLine - iFirstWindowLine) * cyAveCharSM;

////wsprintf (lpszTemp,
////          "InvalidateRange:  %d - %d, L = %d, T = %d, R = %d, B = %d",
////          iTopLine, iBotLine,
////          rc.left,
////          rc.top,
////          rc.right,
////          rc.bottom);
////DbgMsg (lpszTemp);

#if (defined (DEBUG)) & 0
    { // ***DEBUG***
        wsprintf (lpszTemp,
                  "InvalidateRange:  %d-%d",
                  iTopLine,
                  iBotLine);
        DbgMsg (lpszTemp);
    } // ***DEBUG*** END
#endif
    InvalidateRect (hWndSM, &rc, TRUE);
} // End InvalidateRange


//***************************************************************************
//  MoveCaretSM
//
//  Move the caret into position
//***************************************************************************

void MoveCaretSM
    (void)

{
    SetCaretPos ((iCurChar - iFirstWindowChar) * cxAveCharSM + iLCWidth,
                 (iCurLine - iFirstWindowLine) * cyAveCharSM);
} // End MoveCaretSM


//***************************************************************************
//  DisplayChar
//
//  Display a single character by inserting it into the current line
//  and redrawing that line.
//***************************************************************************

void DisplayChar
    (WCHAR chCharCode)

{
    int  i;
    long lvkState;

// Line Continuation ToDo
// Handle in the middle of a line (split in two)
// Handle beyond the end of the line (append trailing blanks
//   and create a second line)



    // Split case depending upon whether we're
    //   before or beyond the end of the line
    if (iCurChar < iCurLineLen)
    {
        // Get the current vkState
        lvkState = GetWindowLong (hWndSM, GWLSM_VKSTATE);

        // Handle Insert vs. Replace
        if (((LPVKSTATE) &lvkState)->Ins)
        {
            // Ensure the line doesn't get too long
            if ((iCurLineLen + 1) > iLastBufferChar)
            {
                // ***FIXME*** -- LINE TOO LONG
                DbgMsg ("DisplayChar:  LINE TOO LONG");

                return;
            } // End IF

            // Make room at iCurChar for a new character
            memmove (&lpwszCurLine[iCurChar + 1],
                     &lpwszCurLine[iCurChar],
                     (iCurLineLen - iCurChar) * sizeof (WCHAR));
            iCurLineLen++;
        } // End IF
    } else
    if (iCurChar > iCurLineLen)
    {
        // Insert blanks from iCurLineLen to iCurChar
        for (i = iCurLineLen; i < iCurChar; i++)
            lpwszCurLine[i] = L' ';
        iCurLineLen = iCurChar;
    } // End IF/ELSE/IF

    // Insert the character
    lpwszCurLine[iCurChar] = chCharCode;
    bCurLineChanged = TRUE;

    // Set new caret position
    iCurChar++;

    // As we changed iCurChar, move the caret
    MoveCaretSM ();

    // Set new line length
    iCurLineLen = max (iCurLineLen, iCurChar);

    // Ensure properly terminated
    lpwszCurLine[iCurLineLen] = L'\0';

    // Scroll the window as necessary
    if (iCurChar > iLastWindowChar)
        ScrollHorz (SCROLL_LAST, iCurChar - iLastWindowChar);
    else
        // Invalidate the line so it gets repainted
        InvalidateLine (iCurLine);
} // End DisplayChar


//***************************************************************************
//  DeleteChar
//
//  Delete the character at iCurChar
//***************************************************************************

void DeleteChar
    (void)
{
    LPGLBHIST lpGlbHist;
    HGLOBAL   hGlb;
    LPWCHAR   wszLine;
    int iLen;

    // In case the user moved the cursor beyond
    //   the end of the line (i.e. lstrlen (lpwszCurLine)),
    //   we need to ensure we don't fill in a
    //   non-existant blank.
    if (iCurChar < iCurLineLen)
    {
        // Erase the char in that position
        memmove (&lpwszCurLine[iCurChar],
                 &lpwszCurLine[iCurChar + 1],
                 (iCurLineLen - iCurChar - 1) * sizeof (WCHAR));
        iCurLineLen--;
        lpwszCurLine[iCurLineLen] = L'\0';
        bCurLineChanged = TRUE;

        // Invalidate the line so it gets repainted
        InvalidateLine (iCurLine);
    } else
    if (iCurChar EQ iCurLineLen
     && iCurLine NE (iLastValidLine + 1))
    {
        // We're at the end of a line
        //   with one following line:
        //   merge it into this current line.

        // Get the global handle to the history buffer
        lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

        // Get a ptr to the contents of the current line
        hGlb = lpGlbHist[iCurLine + 1].hGlb;
        if (hGlb)
        {
            wszLine = (LPWCHAR) MyGlobalLock (hGlb);
        } else
            wszLine = wszEmpty;

        // Ensure the resulting line isn't too long
        iLen = lstrlenW (wszLine);
        if ((iCurLineLen + iLen) > iLastBufferChar)
        {
            // ***FIXME*** -- LINE TOO LONG
            DbgMsg ("DeleteChar:  LINE TOO LONG");
        } else
        {
            // Set new current line length
            iCurLineLen += iLen;

            // Merge iCurLine and iCurLine+1
            lstrcatW (lpwszCurLine, wszLine);
            ReplaceLine (iCurLine);
            bCurLineChanged = TRUE;

            // Unlock the memory before deleting it
            if (hGlb)
            {
                MyGlobalUnlock (hGlb); wszLine = NULL;
            } // End IF

            // Delete iCurLine + 1
            DeleteLine (iCurLine + 1); hGlb = NULL;

            // Redraw this line and the ones below it
            InvalidateRange (iCurLine, iLastValidLine + 1);
        } // End IF/ELSE

        // Unlock the handles
        if (hGlb)
        {
            MyGlobalUnlock (hGlb); wszLine = NULL;
        } // End IF
        MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;
    } // End IF/ELSE/IF
} // End DeleteChar


//***************************************************************************
//  MyCreateCaret
//
//  Create a caret
//***************************************************************************

void MyCreateCaret
    (HWND      hWnd,
     LPVKSTATE lpvkState,
     long      cyAveChar,
     void      (*lpMoveCaret) (void))
{
    // Create a default sized system caret for display
    CreateCaret (hWnd,
                 NULL,
                 lpvkState->Ins ? DEF_CURWID_INS : DEF_CURWID_REP,
                 cyAveChar);
    // Position it
    if (lpMoveCaret)
        (*lpMoveCaret) ();

    // Show it
    ShowCaret (hWnd);
} // End MyCreateCaret


//***************************************************************************
//  InitCurLine
//
//  Initialize the current line
//***************************************************************************

void InitCurLine
    (BOOL bLineCont)
{
    int i;

    for (i = 0; (!bLineCont) && i < DEF_INDENT; i++)
        lpwszCurLine[i] = L' ';
    lpwszCurLine[i] = L'\0';
    iCurChar = iCurLineLen = i;

    bCurLineChanged = FALSE;
} // End InitCurLine


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
    VKSTATE    vkState;
    long       lvkState;
    LPGLBHIST  lpGlbHist;
    HGLOBAL    hGlb;
    LPWCHAR    wszLine;
    RECT       rc;
    HDC        hDC;
    HFONT      hFontOld;
    TEXTMETRIC tm;

////ODSAPI ("SM: ", hWnd, message, wParam, lParam);
    switch (message)
    {
        case WM_NCCREATE:           // lpcs = (LPCREATESTRUCT) lParam
            hWndSM = hWnd;

            INIT_PERTABVARS

            break;                  // Continue with next handler

        case WM_CREATE:
        {
            int i;

            // Initialize variables
            cfSM.hwndOwner = hWnd;
            ZeroMemory (&vkState, sizeof (vkState));
            vkState.Ins = 1;        // Initially inserting ***FIXME*** Make it an option

            // Save in window extra bytes
            SetWindowLong (hWnd, GWLSM_VKSTATE, *(long *) &vkState);

            // Initialize window-specific resources
            SM_Create (hWnd);

            // *************** lpwszCurLine ****************************

            // Allocate memory for the current line
            // Note that this memory is allocated as fixed
            //   because we use it so often
            lpwszCurLine = MyGlobalAlloc (GPTR, (DEF_MAXLINELEN + 1) * sizeof (WCHAR));
            if (!lpwszCurLine)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  MyGlobalAlloc for <lpwszCurLine> failed");

                return -1;          // Mark as failed
            } // End IF

            // Initialize the current line
            InitCurLine (FALSE);

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

            // *************** History *********************************

            // Allocate memory for the array of ptrs to the
            //   Session Manager window lines
            // The "+1" is for converting limit to length.
            hGlbHist = MyGlobalAlloc (GHND, (iLastBufferLine + 1) * sizeof (GLBHIST));
            if (!hGlbHist)
            {
                // ***FIXME*** -- WS FULL before we got started???
                DbgMsg ("WM_CREATE:  MyGlobalAlloc for <hGlbHist> failed");

                return -1;          // Mark as failed
            } // End IF

            // Mark the first entry as such
            lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);
            lpGlbHist[0].First = 1;
            MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;

            // *************** Fonts ***********************************

            // Get the text metrics for this font
            hDC = MyGetDC (hWnd);
            hFontOld = SelectObject (hDC, hFontTC);
            GetTextMetrics (hDC, &tm);
            SelectObject (hDC, hFontOld);
            MyReleaseDC (hWnd, hDC);

            // New height
            cyAveCharSM = MulDiv (cfSM.iPointSize / 10, iLogPixelsY, 72);
            cyAveCharSM = -lfSM.lfHeight;

            lfSM.lfWidth = (tm.tmAveCharWidth + tm.tmMaxCharWidth) / 2;

            // New width (same aspect ratio as old)
            cxAveCharSM = MulDiv (lfSM.lfWidth, cyAveCharSM, -lfSM.lfHeight);

            // Because cxAveCharSM & cyAveCharSM changed, we need to reposition
            //   the caret as it depends upon those two vars.
            MoveCaretSM ();

            // Recalculate the # horizontal characters
            //   and vertical lines.
            nWindowChars = cxWindowPixels / cxAveCharSM;
            nWindowLines = cyWindowPixels / cyAveCharSM;

            // *************** ScrollBars ******************************

            // Set scrollbar ranges
            SetVScrollRange ();
            SetHScrollRange ();

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

            return FALSE;           // We handled the msg
        } // End WM_CREATE

#define chCharCode ((char) wParam)
        case WM_CHAR:               // chCharCode = (TCHAR) wParam; // character code
                                    // lKeyData = lParam;           // Key data
        {
            // Handle Unshifted and Shifted chars
            //  e.g., 'a' = 97, 'z' = 122
            //        'A' = 65, 'Z' =  90

            // Process the character code
            switch (chCharCode)
            {
                case VK_BACK:           // Backspace
                    if (iCurChar > iFirstBufferChar)
                    {
                        // We're past the first char in the buffer

                        // Delete the preceding char

                        // Set new caret position
                        iCurChar--;

                        // As we changed iCurChar, move the caret
                        MoveCaretSM ();   // Move the caret

                        // Delete the char at iCurChar
                        DeleteChar ();

                        // Scroll the window as necessary
                        if (iCurChar < iFirstWindowChar)
                            ScrollHorz (SCROLL_FIRST, iCurChar - iFirstWindowChar);
                    } else
                    {
                        // We're at the first char in the buffer.

                        // If there's a preceding line, merge with it
                        if (iCurLine > iFirstBufferLine)
                        {
                            int iLen;

                            // If the current line has changed,
                            //   save it into the history buffer
                            if (bCurLineChanged)
                                ReplaceLine (iCurLine);

                            // Copy the preceding line from the
                            //   history buffer into lpwszCurLine;
                            CopyLine (iCurLine - 1);

                            // Get the global handle to the history buffer
                            lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

                            // Get a ptr to the contents of the current line
                            hGlb = lpGlbHist[iCurLine].hGlb;
                            if (hGlb)
                            {
                                wszLine = (LPWCHAR) MyGlobalLock (hGlb);
                            } else
                                wszLine = wszEmpty;

                            // Ensure the resulting line isn't too long
                            iLen = lstrlenW (wszLine);
                            if ((iCurLineLen + iLen) > iLastBufferChar)
                            {
                                // ***FIXME*** -- LINE TOO LONG
                                DbgMsg ("VK_BACK:  LINE TOO LONG");

                                // Copy the current line from the
                                //   history buffer into lpwszCurLine;
                                CopyLine (iCurLine);
                            } else
                            {
                                // Set new current line
                                iCurLine--;

                                // Set new caret position
                                iCurChar = iCurLineLen;

                                // Set new current line length
                                iCurLineLen += iLen;

                                // As we changed iCurLine & iCurChar, move the caret
                                MoveCaretSM ();

                                // Scroll the window as necessary
                                if (iCurLine < iFirstWindowLine)
                                    ScrollVert (SCROLL_FIRST, iCurLine - iFirstWindowLine);

                                // Merge iCurLine and iCurLine+1
                                lstrcatW (lpwszCurLine, wszLine);
                                bCurLineChanged = TRUE;

                                // Unlock the memory before deleting it
                                if (hGlb)
                                {
                                    MyGlobalUnlock (hGlb); wszLine = NULL;
                                } // End IF

                                // Delete iCurLine + 1
                                DeleteLine (iCurLine + 1); hGlb = NULL;

                                // Redraw the this line and the ones below it
                                InvalidateRange (iCurLine, iLastValidLine + 1);
                            } // End IF/ELSE

                            // Unlock the handles
                            if (hGlb)
                            {
                                // We no longer need this ptr
                                MyGlobalUnlock (hGlb); wszLine = NULL;
                            } // End IF

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;
                        } // End IF
                    } // End IF/ELSE

                    return FALSE;

                case VK_TAB:            // Tab
                {
                    int iNewCurChar, i;

                    iNewCurChar = DEF_TABS * (1 + iCurChar / DEF_TABS);

                    // Ensure line not too long
                    if ((iCurLineLen + iNewCurChar - iCurChar) > iLastBufferChar)
                    {
                        // ***FIXME*** -- LINE TOO LONG
                        DbgMsg ("DisplayChar:  LINE TOO LONG");




                    } // End IF

                    // If we're within the current line,
                    //   insert some spaces
                    if (iCurChar < iCurLineLen)
                    {
                        memmove (&lpwszCurLine[iNewCurChar],
                                 &lpwszCurLine[iCurChar],
                                 (iCurLineLen - iCurChar) * sizeof (WCHAR));
                        // Calculate the new line length
                        iCurLineLen += (iNewCurChar - iCurChar);

                        // Fill with blanks
                        for (i = iCurChar; i < iNewCurChar; i++)
                            lpwszCurLine[i] = L' ';
                    } // End IF

                    // Position the caret
                    iCurChar = iNewCurChar;

                    // As we changed iCurChar, move the caret
                    MoveCaretSM ();

                    // Ensure properly terminated
                    lpwszCurLine[iCurLineLen] = L'\0';

                    // Invalidate the line so it gets repainted
                    InvalidateLine (iCurLine);

                    return FALSE;
                } // End VK_TAB

                case '\n':              // Line continuation char
                    DisplayChar ('\n');

                    // Restore the original value of lpGlbHist[iCurLine]
                    InvalidateLine (iCurLine);

                    // Append lpwszCurLine to the end of the history
                    AppendLine (lpwszCurLine, TRUE);  // iLastValidLine changes

                    // Get the global handle to the history buffer
                    lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

                    // Mark the lines as continuations of each other
                    lpGlbHist[iLastValidLine    ].ContNext =
                    lpGlbHist[iLastValidLine + 1].ContPrev = TRUE;

                    // Unlock the handle
                    MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;

                    return FALSE;

                case VK_RETURN:         // Enter
                {
                    EXECSTATE esState;
                    int       iExecLine;

                    // Restore the original value of lpGlbHist[iCurLine]
                    InvalidateLine (iCurLine);

                    // Append lpwszCurLine to the end of the history
                    AppendLine (lpwszCurLine, FALSE);  // iLastValidLine changes

                    // Get the global handle to the history buffer
                    lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

                    // Find the first line in the continuation sequence
                    for (iExecLine = iLastValidLine;
                         lpGlbHist[iExecLine].ContPrev;
                         iExecLine--)
                    {};

                    // Unlock the handle
                    MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;

                    // Mark as immediate execution
                    esState.exType = EX_IMMEX;
                    ExecuteLine (iExecLine, &esState);

                    return FALSE;
                } // End VK_RETURN

                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':

                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':

                case '`':
                case '~':
                case '!':
                case '@':
                case '#':
                case '$':
                case '%':
                case '^':
                case '&':
                case '*':
                case '(':
                case ')':
                case '{':
                case '}':
                case '[':
                case ']':
                case '-':
                case '_':
                case '=':
                case '+':
                case '|':
                case '\\':
                case ':':
                case ';':
                case '"':
                case '\'':
                case ',':
                case '<':
                case '.':
                case '>':
                case '/':
                case '?':
                case VK_SPACE:          // Space
                    DisplayChar (chCharCode);

                    return FALSE;
                default:
#ifdef DEBUG
                    wsprintf (lpszTemp,
                              "CHAR:  chCharCode = %d, %c",
                              chCharCode,
                              chCharCode);
                    DbgMsg (lpszTemp);
#endif
                    return FALSE;
            } // End SWITCH

            break;
        } // End WM_CHAR
#undef  chCharCode

#define chCharCode ((char) wParam)
        case WM_SYSCHAR:            // chCharCode = (TCHAR) wParam; // character code
                                    // lKeyData = lParam;           // Key data
        {
            int iChar;
            WCHAR wch;
            // Handle Shifted & unshifted Alt chars
            //  e.g., 'a' = 97, 'z' = 122

            iChar = chCharCode - ' ';
            if (iChar < (sizeof (aCharCode) / sizeof (CHARCODE)))
            {
                // Get the Alt- char code
                wch = aCharCode[iChar].alt;

                // If it's valid, display it
                if (wch)
                    DisplayChar (wch);
                else
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

            return FALSE;
        } // End WM_SYSCHAR
#undef  chCharCode

#ifdef DEBUG
#define chCharCode ((char) wParam)
        case WM_DEADCHAR:
            wsprintf (lpszTemp,
                      "DEADCHAR:  chCharCode = %d, %c",
                      chCharCode,
                      chCharCode);
            DbgMsg (lpszTemp);

            return FALSE;
#undef  chCharCode
#endif
#ifdef DEBUG
#define chCharCode ((char) wParam)
        case WM_SYSDEADCHAR:
            wsprintf (lpszTemp,
                      "SYSDEADCHAR:  chCharCode = %d, %c",
                      chCharCode,
                      chCharCode);
            DbgMsg (lpszTemp);

            return FALSE;
#undef  chCharCode
#endif

#define nVirtKey ((int) wParam)
        case WM_KEYDOWN:            // nVirtKey = (int) wParam;     // virtual-key code
                                    // lKeyData = lParam;           // Key data
        {
            // Process the virtual key
            switch (nVirtKey)
            {
                case VK_HOME:           // Home
                    // Set new caret position
                    iCurChar = 0;

                    // As we changed iCurChar, move the caret
                    MoveCaretSM ();       // Move the caret

                    // Scroll the window as necessary
                    if (iCurChar < iFirstWindowChar)
                        ScrollHorz (SCROLL_FIRST, iCurChar - iLastWindowChar);

                    return FALSE;

                case VK_END:            // End
                    // Set new caret position
                    iCurChar = iCurLineLen;

                    // As we changed iCurChar, move the caret
                    MoveCaretSM ();       // Move the caret

                    // Scroll the window as necessary
                    if (iCurChar > iLastWindowChar)
                        ScrollHorz (SCROLL_LAST, iCurChar - iLastWindowChar);
                    else
                    if (iCurChar < iFirstWindowChar)
                        ScrollHorz (SCROLL_FIRST, iCurChar - iFirstWindowChar);

                    return FALSE;

                case VK_LEFT:           // Left arrow
                    // If the caret is not at the start of the buffer, ...
                    if (iCurChar > iFirstBufferChar)
                    {
                        // Set new caret position
                        iCurChar--;

                        // As we changed iCurChar, move the caret
                        MoveCaretSM ();   // Move the caret
                    } // End IF

                    // Scroll the window as necessary
                    if (iCurChar < iFirstWindowChar)
                        ScrollHorz (SCROLL_FIRST, iCurChar - iFirstWindowChar);

                    return FALSE;

                case VK_RIGHT:          // Right arrow
                    // If the caret is not at the end of the buffer, ...
                    if (iCurChar < iLastBufferChar)
                    {
                        // Set new caret position
                        iCurChar++;

                        // As we changed iCurChar, move the caret
                        MoveCaretSM ();       // Move the caret
                    } // End IF

                    // Scroll the window as necessary
                    if (iCurChar > iLastWindowChar)
                        ScrollHorz (SCROLL_LAST, iCurChar - iLastWindowChar);

                    return FALSE;

                case VK_UP:             // Up arrow
                    // Move the cursor up one line
                    //   unless we're at the start
                    if (iCurLine > iFirstBufferLine)
                    {
                        // If the current line has changed,
                        //   save it into the history buffer
                        if (bCurLineChanged)
                            ReplaceLine (iCurLine);

                        // Set new current line
                        iCurLine--;

                        // As we changed iCurLine, move the caret
                        MoveCaretSM ();

                        // Copy the current line from the
                        //   history buffer into lpwszCurLine;
                        CopyLine (iCurLine);

                        // Scroll the window as necessary
                        if (iCurLine < iFirstWindowLine)
                            ScrollVert (SCROLL_FIRST, iCurLine - iFirstWindowLine);
                    } // End IF

                    return FALSE;

                case VK_DOWN:           // Down arrow
                    // Move the cursor down one line
                    //   unless we're at the last valid line
                    if (iCurLine < (iLastValidLine + 1))
                    {
                        // If the current line has changed,
                        //   save it into the history buffer
                        if (bCurLineChanged)
                            ReplaceLine (iCurLine);

                        // Set new current line
                        iCurLine++;

                        // As we changed iCurLine, move the caret
                        MoveCaretSM ();

                        // Copy the current line from the
                        //   history buffer into lpwszCurLine;
                        CopyLine (iCurLine);

                        // Scroll the window as necessary
                        if (iCurLine > iLastWindowLine)
                            ScrollVert (SCROLL_LAST, iCurLine - iLastWindowLine);
                    } // End IF

                    return FALSE;

                case VK_INSERT:         // Insert
                    // Get the current vkState
                    lvkState = GetWindowLong (hWndSM, GWLSM_VKSTATE);
                    vkState = *(LPVKSTATE) &lvkState;

                    vkState.Ins = !vkState.Ins;

                    // Save in window extra bytes
                    SetWindowLong (hWnd, GWLSM_VKSTATE, *(long *) &vkState);

                    // Create a default sized system caret for display
                    DestroyCaret ();        // 'cause we're changing the cursor width
                    MyCreateCaret (hWndSM, &vkState, cyAveCharSM, &MoveCaretSM);

                    return FALSE;

                case VK_DELETE:         // Delete
                    // Delete the char at iCurChar
                    DeleteChar ();

                    return FALSE;

                case VK_PRIOR:          // Page Up
                    // View the previous page
                    ScrollVert (SCROLL_FIRST, -nWindowLines);

                    return FALSE;

                case VK_NEXT:           // Page Down
                    // View the next page
                    ScrollVert (SCROLL_FIRST,  nWindowLines);

                    return FALSE;
#ifdef DEBUG
                case VK_F1:             // Display session manager history
                    DisplayHistory ();

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F2:             // Display hash table entries
                    DisplayHshTab ();

                    return FALSE;
#endif
#ifdef DEBUG
                case VK_F3:             // Display current token entries
                    DisplayTokens (ghGlbToken);

                    return FALSE;
#endif
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

                    GetClientRect (hWndMC, &rc);
                    nWidthMC  = rc.right  - rc.left;
                    nHeightMC = rc.bottom - rc.top;

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
////////////////case VK_F10:            // Not generated
////////////////    return FALSE;
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
#ifdef DEBUG
////////////////default:
////////////////    wsprintf (lpszTemp,
////////////////              "WM_KEYDOWN:  nVirtKey = %d",
////////////////              nVirtKey);
////////////////    DbgMsg (lpszTemp);
#endif
            } // End SWITCH

            // We need to pass this message on to the next handler
            //   so WM_CHAR & WM_SYSCHAR can process it.
            break;
        } // End WM_KEYDOWN
#undef  nVirtKey

#define nPos ((short int) HIWORD(wParam))
        case WM_HSCROLL:        // nScrollCode = (int) LOWORD(wParam);  // scroll bar value
                                // nPos = (short int) HIWORD(wParam);   // scroll box position
                                // hwndScrollBar = (HWND) lParam;       // handle of scroll bar
            // Split cases
            switch (LOWORD (wParam))
            {
////////////////case SB_BOTTOM:
////////////////    break;

////////////////case SB_ENDSCROLL:
////////////////    break;

////////////////case SB_TOP:
////////////////    break;

////////////////case SB_THUMBPOSITION:      // Look in nPos
////////////////    break;

                case SB_LINELEFT:
                    ScrollHorz (SCROLL_FIRST, -1);
                    break;

                case SB_LINERIGHT:
                    ScrollHorz (SCROLL_FIRST,  1);
                    break;

                case SB_PAGELEFT:
                    ScrollHorz (SCROLL_FIRST, -nWindowChars);
                    break;

                case SB_PAGERIGHT:
                    ScrollHorz (SCROLL_FIRST,  nWindowChars);
                    break;

                case SB_THUMBTRACK:         // Look in nPos
                    // nPos contains the new iFirstWindowChar
                    ScrollHorz (SCROLL_FIRST,  nPos - iFirstWindowChar);

                    break;
            } // End SWITCH

            return FALSE;           // We handled the msg
#undef  nPos

#define nPos ((short int) HIWORD(wParam))
        case WM_VSCROLL:        // nScrollCode = (int) LOWORD(wParam);  // scroll bar value
                                // nPos = (short int) HIWORD(wParam);   // scroll box position
                                // hwndScrollBar = (HWND) lParam;       // handle of scroll bar
            // Split cases
            switch (LOWORD (wParam))
            {
////////////////case SB_BOTTOM:
////////////////    break;

////////////////case SB_ENDSCROLL:
////////////////    break;

////////////////case SB_TOP:
////////////////    break;

////////////////case SB_THUMBPOSITION:      // Look in nPos
////////////////    break;

                case SB_LINEUP:
                    ScrollVert (SCROLL_FIRST, -1);
                    break;

                case SB_LINEDOWN:
                    ScrollVert (SCROLL_FIRST,  1);
                    break;

                case SB_PAGEDOWN:
                    ScrollVert (SCROLL_FIRST,  nWindowLines);
                    break;

                case SB_PAGEUP:
                    ScrollVert (SCROLL_FIRST, -nWindowLines);
                    break;

                case SB_THUMBTRACK:         // Look in nPos
                    // nPos contains the new iFirstWindowLine
                    ScrollVert (SCROLL_FIRST,  nPos - iFirstWindowLine);

                    break;
            } // End SWITCH

            return FALSE;           // We handled the msg
#undef  nPos

#define fwKeys  (LOWORD(wParam))
#define xPos    (LOWORD(lParam))
#define yPos    (HIWORD(lParam))
        case WM_MOUSEWHEEL:         // fwKeys = LOWORD(wParam); // key flags
                                    // iDist  = (int) HIWORD(wParam); // Scroll distance in units od WHEEL_DELTA
                                    // xPos = LOWORD(lParam);  // horizontal position of cursor
                                    // yPos = HIWORD(lParam);  // vertical position of cursor
            if (GET_WHEEL_DELTA_WPARAM (wParam) > 0)
                PostMessage (hWnd, WM_VSCROLL, SB_LINEUP, 0);
            else
                PostMessage (hWnd, WM_VSCROLL, SB_LINEDOWN, 0);
            return FALSE;           // We handled the msg
#undef  yPos
#undef  xPos
#undef  fwKeys

#define fwSizeType wParam
        case WM_SIZE:               // fwSizeType = wParam;      // resizing flag
                                    // nWidth = LOWORD(lParam);  // width of client area
                                    // nHeight = HIWORD(lParam); // height of client area
            if (fwSizeType NE SIZE_MINIMIZED)
            {
                cxWindowPixels = LOWORD (lParam);   // Save for later use
                cyWindowPixels = HIWORD (lParam);   // ...

                // Recalculate the # characters up/down and across
                // The test is in case we get called before cxAveCharSM is calculated
                if (cxAveCharSM)
                {
                    nWindowChars  = cxWindowPixels / cxAveCharSM;
                    nWindowLines  = cyWindowPixels / cyAveCharSM;

                    // Set last window chars & lines
                    iLastWindowChar = iFirstWindowChar + nWindowChars;
                    iLastWindowLine = iFirstWindowLine + nWindowLines;
                } // End IF
            } // End IF

            break;                  // Continue with next handler ***MUST***
#undef  fwSizeType

        case WM_SETFOCUS:           // hwndLoseFocus = (HWND) wParam; // handle of window losing focus
            // Get the current vkState
            lvkState = GetWindowLong (hWnd, GWLSM_VKSTATE);
            vkState = *(LPVKSTATE) &lvkState;

            // Create a default sized system caret for display
            MyCreateCaret (hWndSM, &vkState, cyAveCharSM, &MoveCaretSM);

            break;                  // Continue with next handler ***MUST***

        case WM_KILLFOCUS:          // hwndGetFocus = (HWND) wParam; // handle of window receiving focus
            DestroyCaret ();        // 'cause we just lost the focus

            break;                  // Continue with next handler

        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            // Uninitialize window-specific resources
            SM_Delete (hWnd);

            // Initialize window-specific resources
            SM_Create (hWnd);

            return FALSE;           // We handled the msg

#define fwKeys  wParam
#define xPos    (LOWORD(lParam))
#define yPos    (HIWORD(lParam))
        case WM_LBUTTONDOWN:        // fwKeys = wParam;        // Key flags
                                    // xPos = LOWORD(lParam);  // Horizontal position of cursor
                                    // yPos = HIWORD(lParam);  // Vertical position of cursor
        {
            int xPosRel;

            // If the current line has changed,
            //   save it into the history buffer
            if (bCurLineChanged)
                ReplaceLine (iCurLine);

            // Ensure xPos is relative to the non-iLCWidth client area
            xPosRel = max (0, xPos - iLCWidth);

            // Identify the selected char
            iCurChar = iFirstWindowChar + (xPosRel / cxAveCharSM);
            iCurLine = iFirstWindowLine + (yPos    / cyAveCharSM);

            // Move the cursor to this line
            //   unless it's below the last valid line
            iCurLine = min (iCurLine, iLastValidLine + 1);

            // ***FIXME** -- Should we squeak if the cursor is too low??

            // Move the caret there
            MoveCaretSM ();

            // Copy the current line from the
            //   history buffer into lpwszCurLine;
            CopyLine (iCurLine);

            return FALSE;           // We handled the msg
        } // End WM_LBUTTONDOWN
#undef  yPos
#undef  xPos
#undef  fwKeys

        case WM_ERASEBKGND:
            // In order to reduce screen flicker, we handle erase background
            // in the WM_PAINT message.
            return TRUE;            // We erased the background

        case WM_PAINT:
            // Validate the update region
            if (GetUpdateRect (hWnd, &rc, FALSE))
            {
                PAINTSTRUCT ps;
                int         iTop, iBot;
                LPWCHAR     lpwszLine;
                HDC         hDC, hDCMem;
                HBITMAP     hBitmap, hBitmapOld;
                RECT        rcLineCont;

                // Tell W to lay out the dropcloths
                BeginPaint (hWnd, &ps);

                // Get the size of the client area
                // so we know how big a bitmap to define
                GetClientRect (hWnd, &rc);

                rcLineCont.left   = 0;
                rcLineCont.top    = 0;
                rcLineCont.right  = iLCWidth;
                rcLineCont.bottom = rc.bottom;

                // To avoid flicker, we draw in a memory DC
                hDCMem = MyCreateCompatibleDC (ps.hdc);
                hBitmap = MyCreateCompatibleBitmap (ps.hdc,
                                                    rc.right,
                                                    rc.bottom);
                hBitmapOld = SelectObject (hDCMem, hBitmap);

                // Handle WM_ERASEBKGND here by filling in the client area
                FillRect (hDCMem, &rc, (HBRUSH) GetClassLong (hWnd, GCL_HBRBACKGROUND));

                // Fill in the background for the line continuation column
                FillRect (hDCMem, &rcLineCont, GetStockObject (LTGRAY_BRUSH));

                // Set our DC attributes
                SetAttrs (hDCMem, hFontSM, crTextColor, crBkColor);

                // Get the global handle to the history buffer
                lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

                // Calculate the range of changed line(s)
////////////////iTop = iFirstWindowLine + (ps.rcPaint.top    / cyAveCharSM);
////////////////iBot = iFirstWindowLine + (ps.rcPaint.bottom / cyAveCharSM);
                iTop = iFirstWindowLine + (rc.top    / cyAveCharSM);
                iBot = iFirstWindowLine + (rc.bottom / cyAveCharSM);
#if (defined (DEBUG)) && 0
                { // ***DEBUG***
                    wsprintf (lpszTemp,
                              "WM_PAINT:  iTop = %d, iBot = %d",
                              iTop,
                              iBot);
                    DbgMsg (lpszTemp);
                } // ***DEBUG*** END
#endif
                // Redraw the changed line(s)
                for (; iTop < iBot; iTop++)
                if (iTop EQ iCurLine)
                {
                    // Ensure properly terminated
                    lpwszCurLine[iCurLineLen] = L'\0';
                    wszLine = lpwszCurLine;

                    // Ensure iFirstWindowChar not beyond string end
                    if (iFirstWindowChar < lstrlenW (wszLine))
                        lpwszLine = &wszLine[iFirstWindowChar];
                    else
                        lpwszLine = wszEmpty;
                    DrawLine (lpwszLine,
                              iTop - iFirstWindowLine,
                              hDCMem,
                              lpGlbHist[iTop].ContPrev);
                } else
                if (iTop <= iLastBufferLine)
                {
                    if (lpGlbHist[iTop].hGlb)
                    {
                        // Get ptr to the global memory
                        hGlb = lpGlbHist[iTop].hGlb;
                        wszLine = (LPWCHAR) MyGlobalLock (hGlb);

                        // Ensure iFirstWindowChar not beyond string end
                        if (iFirstWindowChar < lstrlenW (wszLine))
                            lpwszLine = &wszLine[iFirstWindowChar];
                        else
                            lpwszLine = wszEmpty;
                        DrawLine (lpwszLine,
                                  iTop - iFirstWindowLine,
                                  hDCMem,
                                  lpGlbHist[iTop].ContPrev);

                        // Unlock the handle
                        MyGlobalUnlock (hGlb); wszLine = NULL;
                    }
////////////////////else
////////////////////    DrawLine (wszEmpty,
////////////////////              iTop - iFirstWindowLine,
////////////////////              hDCMem,
////////////////////              lpGlbHist[iTop].ContPrev);
                } // End FOR/IF/ELSE/IF

                // Obtain a DC without a clipping region
                //   so we may draw anywhere we like
                //   (such as for line continuations)
                hDC = MyGetDC (hWnd);

                // Set our DC attributes
                SetAttrs (hDCMem, hFontSM, crTextColor, crBkColor);

                // Copy the memory DC to the screen DC
                BitBlt (hDC,
                        0,
                        0,
                        rc.right,
                        rc.bottom,
                        hDCMem,
                        0,
                        0,
                        SRCCOPY);
                // Free resources
                SelectObject (hDCMem, hBitmapOld);
                MyDeleteObject (hBitmap); hBitmap = NULL;
                MyDeleteDC (hDCMem); hDCMem = NULL;
                MyReleaseDC (hWnd, hDC); hDC = NULL;

                // Unlock the handle
                MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;

                // Pass the turpentine
                EndPaint (hWnd, &ps);
            } // End IF

            return FALSE;           // We handled the msg

        case WM_CLOSE:
            // Because the SM window doesn't close unless the
            //   MF window is closing, we ignore this message.
            // We close when the MF window calls DestroyWindow.

            return FALSE;           // We handled the msg

        case WM_DESTROY:
        {
            // *************** ScrollBars ******************************
            // Nothing to undo

            // *************** History *********************************
            if (hGlbHist)
            {
                int i;

                // Get the global handle to the history buffer
                lpGlbHist = (LPGLBHIST) MyGlobalLock (hGlbHist);

                // Free the lines
                for (i = 0; i <= iLastValidLine; i++)
                if (lpGlbHist[i].hGlb)
                {
                    DbgGlobalFree (lpGlbHist[i].hGlb); lpGlbHist[i].hGlb = NULL;
                } // End IF

                // Unlock and free the global handle
                MyGlobalUnlock (hGlbHist); lpGlbHist = NULL;
                DbgGlobalFree (hGlbHist); hGlbHist = NULL;
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
                DbgGlobalFree (lpwszCurLine); lpwszCurLine = NULL;
            } // End IF

            // Uninitialize window-specific resources
            SM_Delete (hWnd);

            return FALSE;           // We handled the msg
        } // End WM_DESTROY
    } // End SWITCH

////ODSAPI ("SMZ:", hWnd, message, wParam, lParam);
    return DefMDIChildProc (hWnd, message, wParam, lParam);
} // End SMWndProc
#undef  APPEND_NAME


//***************************************************************************
//  End of File: sessman.c
//***************************************************************************
