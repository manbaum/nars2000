//***************************************************************************
//  NARS2000 -- Common Processing Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2015 Sudley Place Software

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
#include <stdio.h>
#include <direct.h>
#include "headers.h"


//************************************************************************
//  $ClientToWindowSize
//
//  Convert client area size to window size
//************************************************************************

SIZE ClientToWindowSize
    (HWND hWnd,
     SIZE Size)

{
    RECT rc;

    // Create a rectangle
    rc.left   = 0;
    rc.right  = rc.left + Size.cx;
    rc.top    = 0;
    rc.bottom = rc.top  + Size.cy;

    // Adjust the rectangle from client to window
    AdjustWindowRectEx (&rc,
                        GetWindowLongW   (hWnd, GWL_STYLE),
                        GetClassLongPtrW (hWnd, GCLP_MENUNAME) NE 0,
                        GetWindowLongW   (hWnd, GWL_EXSTYLE)
                       );
    Size.cx = rc.right  - rc.left;
    Size.cy = rc.bottom - rc.top;

    return Size;
} // End ClientToWindowSize


//************************************************************************
//  $MoveWindowPos
//
//  Move the window to a position based upon the previous center
//************************************************************************

void MoveWindowPos
    (HWND  hWnd,
     POINT PosCtr)

{
    RECT rcWnd;
    long lWidth, lHeight;

    GetWindowRect (hWnd, &rcWnd);           // Get the window's RECT
    lWidth = rcWnd.right  - rcWnd.left;     // Calc the width
    lHeight = rcWnd.bottom - rcWnd.top;     // Calc the height
    MoveWindow (hWnd,
                PosCtr.x - lWidth/2,        // Left coordinate
                PosCtr.y - lHeight/2,       // Top ...
                lWidth,                     // Width
                lHeight,                    // Height
                FALSE);                     // No re-paint
} // End MoveWindowPos


//************************************************************************
//  $MoveWindowPosSize
//
//  Move the window to a position based upon the previous center and size
//************************************************************************

void MoveWindowPosSize
    (HWND  hWnd,
     POINT PosCtr,
     SIZE  Size)

{
    MoveWindow (hWnd,
                PosCtr.x - Size.cx/2,       // Left coordinate
                PosCtr.y - Size.cy/2,       // Top ...
                Size.cx,                    // Width
                Size.cy,                    // Height
                FALSE);                     // No re-paint
} // End MoveWindowPosSize


//************************************************************************
//  $EnumCallbackMonitors
//
//  Callback routine for EnumDisplayMonitors
//************************************************************************

UBOOL CALLBACK EnumCallbackMonitors
    (HMONITOR hMonitor,             // Handle to display monitor
     HDC      hdcMonitor,           // Handle to monitor DC
     LPRECT   lprcMonitor,          // Monitor intersection rectangle
     LPARAM   dwData)               // Data

{
    // Save the monitor coordinates
    *((LPRECT) dwData) = *lprcMonitor;

    // Tell W to stop enumerating
    return FALSE;
} // End EnumCallbackMonitors


//************************************************************************
//  $CenterWindow
//
//  Center a window on the monitor that contains
//    the center of the Master Frame
//************************************************************************

void CenterWindow
    (HWND hWndCT)               // Window to center

{
    RECT        rcWndMF,        // Master Frame window rect
                rcWndCT;        // Window-to-center rect
    POINT       PosCtr;         // Position of the center of the window-to-center
    HMONITOR    hMonitor;       // Monitor handle
    MONITORINFO monInfo;        // Monitor info struct
    int         iOff;           // Non-negative offset to avoid top of window
                                //   being out-of-sight

    // Get the window rect of the Master Frame
    GetWindowRect (hWndMF, &rcWndMF);

    // Get the window rect of the window-to-center
    GetWindowRect (hWndCT, &rcWndCT);

    // Get the point in the center of the Master Frame.
    PosCtr.x = (rcWndMF.left + rcWndMF.right ) / 2;
    PosCtr.y = (rcWndMF.top  + rcWndMF.bottom) / 2;

    // Get the handle of the monitor that contains this point
    hMonitor = MonitorFromPoint (PosCtr, MONITOR_DEFAULTTONEAREST);

    // Get the monitor rectangle
    monInfo.cbSize = sizeof (monInfo);
    GetMonitorInfo (hMonitor, &monInfo);

    // Ensure that the top of the window-to-center
    //   isn't above the top of the selected monitor
    iOff = monInfo.rcMonitor.top - min (monInfo.rcMonitor.top, PosCtr.y - (rcWndCT.bottom - rcWndCT.top) / 2);

    // Move the monitor rectangle down as necessary
    PosCtr.y += iOff;

    // Move the window into position
    MoveWindowPos (hWndCT, PosCtr);
} // End CenterWindow


//****************************************************************************
//  $IsWhite
//
//  Determine if a char is white space
//****************************************************************************

UBOOL IsWhite
    (char c)

{
    return (c EQ ' ' || c EQ '\t');
} // End IsWhite


//****************************************************************************
//  $IsWhiteW
//
//  Determine if a wide char is white space
//****************************************************************************

UBOOL IsWhiteW
    (WCHAR wc)

{
    return (wc EQ L' ' || wc EQ WC_HT);
} // End IsWhiteW


//****************************************************************************
//  $SkipWhite
//
//  Skip over leading white space
//****************************************************************************

LPCHAR SkipWhite
    (LPCHAR lp)

{
    // Skip over white space
    while (IsWhite (*lp))
        lp++;

    return lp;
} // End SkipWhite


//****************************************************************************
//  $SkipWhiteW
//
//  Skip over leading white space
//****************************************************************************

LPWCHAR SkipWhiteW
    (LPWCHAR lpw)

{
    // Skip over white space
    while (IsWhiteW (*lpw))
        lpw++;

    return lpw;
} // End SkipWhiteW


//***************************************************************************
//  $SkipBlackW
//
//  Skip over leading black space
//***************************************************************************

LPWCHAR SkipBlackW
    (LPWCHAR lpw)

{
    // Skip over non-white space
    while (*lpw && !IsWhiteW (*lpw))
        lpw++;

    return lpw;
} // End SkipBlackW


//************************************************************************
//  $SkipToCharW
//
//  Skip to a given WCHAR taking into account EOL
//************************************************************************

LPWCHAR SkipToCharW
    (LPWCHAR lpwChar,
     WCHAR   wChar)

{
    LPWCHAR lpw;        // Temporary ptr

    // Find the char
    lpw = strchrW (lpwChar, wChar);

    // If it's not found (EOL instead), ...
    if (lpw EQ NULL)
        lpw = &lpwChar[lstrlenW (lpwChar)];

    return lpw;
} // End SkipToCharW


//************************************************************************
//  $SkipToStrW
//
//  Skip to the first in a given set of WCHARs taking into account EOL
//************************************************************************

LPWCHAR SkipToStrW
    (LPWCHAR lpwChar,
     LPWCHAR lpwSet)

{
    LPWCHAR lpw;        // Temporary ptr

    // Find the char
    lpw = strpbrkW (lpwChar, lpwSet);

    // If it's not found (EOL instead), ...
    if (lpw EQ NULL)
        lpw = &lpwChar[lstrlenW (lpwChar)];

    return lpw;
} // End SkipToStrW


//************************************************************************
//  $SkipToCharDQW
//
//  Skip to a given WCHAR taking into account Double Quotes and EOL
//************************************************************************

LPWCHAR SkipToCharDQW
    (LPWCHAR lpwChar,
     WCHAR   wChar)

{
    LPWCHAR lpw;            // Temporary ptr
    UBOOL   uState = FALSE; // DQ state:  FALSE = outside DQs,
                            //            TRUE  = inside DQs
    Assert (wChar NE WC_DQ);

    // Loop through the string
    for (lpw = lpwChar; *lpw; lpw++)
    // Split cases based upon the char
    switch (*lpw)
    {
        case WC_DQ:
            // Toggle the DQ state
            uState = !uState;

            break;

        default:
            if (!uState         // If we're outside DQs
             && *lpw EQ wChar)  // and it's a match, ...
                return lpw;     // Return a ptr to the match
            break;
    } // End FOR/SWITCH

    // If it's not found (EOL instead), ...
    if (lpw EQ NULL)
        lpw = &lpwChar[lstrlenW (lpwChar)];

    return lpw;
} // End SkipToCharDQW


//************************************************************************
//  $SkipPastCharW
//
//  Skip past a given WCHAR taking into account EOL
//************************************************************************

LPWCHAR SkipPastCharW
    (LPWCHAR lpwChar,
     WCHAR   wChar)

{
    LPWCHAR lpw;        // Temporary ptr

    // Find the char
    lpw = strchrW (lpwChar, wChar);

    // If it's not found (EOL instead), ...
    if (lpw EQ NULL)
        // Point to the trailing zero
        lpw = &lpwChar[lstrlenW (lpwChar)];
    else
        // Skip over the trailing char
        lpw++;

    return lpw;
} // End SkipPastCharW


//************************************************************************
//  $SkipPastStr
//
//  Skip past a given set of chars taking into account EOL
//************************************************************************

LPCHAR SkipPastStr
    (LPCHAR lpChar,
     LPCHAR lpStr)

{
    LPCHAR lp;          // Temporary ptr
    size_t iSpn;

    // Find the span of the set of chars
    iSpn = strspn (lpChar, lpStr);

    // If it's not found (EOL instead), ...
    if (iSpn EQ 0)
        // Point to the trailing zero
        lp = &lpChar[lstrlen (lpChar)];
    else
        // Skip over the trailing chars
        lp = &lpChar[iSpn];

    return lp;
} // End SkipPastChar


//************************************************************************
//  $SkipPastStrW
//
//  Skip past a given set of WCHARs taking into account EOL
//************************************************************************

LPWCHAR SkipPastStrW
    (LPWCHAR lpwChar,
     LPWCHAR lpwStr)

{
    LPWCHAR lpw;        // Temporary ptr
    size_t  iSpn;

    // Find the span of the set of WCHARs
    iSpn = strspnW (lpwChar, lpwStr);

    // If it's not found (EOL instead), ...
    if (iSpn EQ 0)
        // Point to the trailing zero
        lpw = &lpwChar[lstrlenW (lpwChar)];
    else
        // Skip over the trailing chars
        lpw = &lpwChar[iSpn];

    return lpw;
} // End SkipPastCharW


//***************************************************************************
//  $StripDQ
//
//  Strip off leading/trailing DQs
//***************************************************************************

LPWCHAR StripDQ
    (LPWCHAR lpwszTail)

{
    LPWCHAR lpwTemp;
    UINT    uLen;

    if (lpwszTail[0] EQ WC_DQ)
    {
        // Skip over it
        lpwszTail++;

        // Skip over leading blanks
        lpwszTail = SkipWhiteW (lpwszTail);

        // Find the matching DQ
        lpwTemp = strchrW (lpwszTail, WC_DQ);

        // If present, ...
        if (lpwTemp)
            // Zap it
           *lpwTemp = WC_EOS;

        // Get the string length
        uLen = lstrlenW (lpwszTail);

        // Skip back over trailing white space
        while (uLen && IsWhiteW (lpwszTail[uLen - 1]))
            uLen --;
        // Zap the trailing white space
        lpwszTail[uLen] = WC_EOS;
    } // End IF

    return lpwszTail;
} // End StripDQ


//***************************************************************************
//  $GetModuleFileNames
//
//  Construct file name(s) based upon where the module is on disk
//***************************************************************************

void GetModuleFileNames
    (HINSTANCE hInstance)

{
    WCHAR wszDir  [_MAX_DIR],
          wszDrive[_MAX_DRIVE],
          wszFname[_MAX_FNAME],
          wszExt  [_MAX_EXT];

    if (GetModuleFileNameW (hInstance, wszAppDPFE, sizeof (wszAppDPFE)))
    {
        // Split out the drive and path from the module filename
        _wsplitpath (wszAppDPFE, wszDrive, wszDir, wszFname, wszExt);

        // Create the .HLP file name
        _wmakepath  (wszHlpDPFE, wszDrive, wszDir, wszFname, L".HLP");

        // Create the APL Font file name
        _wmakepath  (wszFntDPFE, wszDrive, wszDir, DEF_APLFONT_EXTNAME, DEF_APLFONT_EXT);

        // Read in the application's File Version String
        LclFileVersionStrW (wszAppDPFE, wszFileVer);

        // Read in the COMCTL32.DLL File Version String
        LclFileVersionStrW (L"comctl32.dll", wszComctl32FileVer);

        // Get the Comctl32.dll major and minor file version #s
        sscanfW (wszComctl32FileVer,
                 L"%f",
                &fComctl32FileVer);
    } // End IF
} // End GetModuleFileNames


//***************************************************************************
//  $W2A
//
//  Convert and copy a wide char string to a multibyte char string
//***************************************************************************

void W2A
    (LPSTR  lpDest,
     LPWSTR lpSrc,
     DWORD  dwSize)

{
    int iLen;

    iLen = 1 + lstrlenW (lpSrc);    // Get source length
    WideCharToMultiByte (CP_ACP,    // ANSI code page
                         0,         // No flags
                         lpSrc,     // Source
                         iLen,      // Source length
                         lpDest,    // Destin
                         dwSize,    // Size of destin buffer
                         NULL,      // Use system default char
                         NULL);     // Skip notification
} // End W2A


//***************************************************************************
//  $A2W
//
//  Convert and copy a multibyte char string to a wide char string
//***************************************************************************

void A2W
    (LPWSTR lpDest,
     LPSTR  lpSrc,
     DWORD  dwSize)

{
    int iLen;

    iLen = 1 + lstrlenA (lpSrc);    // Get source length
    MultiByteToWideChar (CP_ACP,    // ANSI code page
                         0,         // No flags
                         lpSrc,     // Source
                         iLen,      // Source length
                         lpDest,    // Destin
                         dwSize);   // Size of destin buffer
} // End A2W


//***************************************************************************
//  $FillMemoryW
//
//  Fill memory with a wide char
//***************************************************************************

LPWCHAR FillMemoryW
    (LPWCHAR  lpMemDest,
     APLU3264 uLen,
     WCHAR    wc)

{
    while (uLen--)
        *lpMemDest++ = wc;
    return lpMemDest;
} // End FillMemoryW


//***************************************************************************
//  $FillBitMemory
//
//  Fill in memory with all 1s handling the last byte specially
//***************************************************************************

void FillBitMemory
    (LPAPLBOOL lpMemRes,                // Ptr to memory to fill
     APLNELM   aplNELMRes)              // # bits to fill

{
    // Fill the leading bytes with 0xFF
    FillMemory (lpMemRes, (APLU3264) aplNELMRes >> LOG2NBIB, 0xFF);

    // If there are extra bits at the end, ...
    if (MASKLOG2NBIB & (UINT) aplNELMRes)
        lpMemRes[aplNELMRes >> LOG2NBIB] =
          ((BIT0 << (MASKLOG2NBIB & (UINT) aplNELMRes)) - 1);
} // End FillBitMemory


#ifdef DEBUG
//***************************************************************************
//  $nop
//***************************************************************************

void nop
    (void)

{
} // End nop
#endif


#ifdef DEBUG
//***************************************************************************
//  $DbgNop
//***************************************************************************

void DbgNop
    (void)

{
    DbgBrk ();          // #ifdef DEBUG
} // End DbgNop
#endif


//***************************************************************************
//  $IsValidHandle
//
//  Return TRUE iff the given handle is valid
//***************************************************************************

UBOOL IsValidHandle
    (HGLOBAL hGlbSrc)

{
    __try
    {
        return ((hGlbSrc NE NULL) && (GlobalFlags (ClrPtrTypeDir (hGlbSrc)) NE GMEM_INVALID_HANDLE));
    } __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return FALSE;
    } // End __try/__except
} // End IsValidHandle


//***************************************************************************
//  $IsValidPtr
//
//  Return TRUE iff the given ptr is valid
//***************************************************************************

UBOOL IsValidPtr
    (LPVOID lpMemSrc,
     UINT   uLen)

{
    if (lpMemSrc EQ (LPVOID) -1
     || ((LPBYTE) lpMemSrc) < (LPBYTE) (64*1024))
        return FALSE;

    __try
    {
        return !IsBadReadPtr (lpMemSrc, uLen);
    } __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return FALSE;
    } // End __try/__except

    return FALSE;
} // End IsValidPtr


//***************************************************************************
//  $GetDlgUnitsInPixels
//
//  Get a dialog's base units in pixels
//***************************************************************************

SIZE GetDlgUnitsInPixels
    (HWND hWnd)

{
    RECT rcDlg;
    SIZE sDlg;

    // Set to the unit divisors
    rcDlg.left   =
    rcDlg.top    = 0;
    rcDlg.right  = 4;
    rcDlg.bottom = 8;

    MapDialogRect (hWnd, &rcDlg);

    sDlg.cx = rcDlg.right;
    sDlg.cy = rcDlg.bottom;

    return sDlg;
} // End GetDlgUnitsInPixels


//************************************************************************
//  DrawBitmap
//
//  Draw a bitmap in a DC
//************************************************************************

void DrawBitmap
    (HDC     hDC,           // Destination DC
     HBITMAP hBitMap,       // The bitmap to draw
     UINT    xDstOrg,       // Destin bit origin
     UINT    yDstOrg)       // ...

{
    BITMAP  bm;
    HDC     hDCMem;
    POINT   ptSize, ptOrg;
    HBITMAP hOldBitmap;

    // Get a Client Area DC for the bitmap
    hDCMem = MyCreateCompatibleDC (hDC);

    // Select the bitmap into the DC
    hOldBitmap = SelectObject (hDCMem, hBitMap);

    // Copy the mapping mode from the original DC
    SetMapMode (hDCMem, GetMapMode (hDC));

    // Get the size of the bitmap
    GetObjectA (hBitMap, sizeof (BITMAP), (LPSTR) &bm);

    // Save it as a POINT
    ptSize.x = bm.bmWidth;
    ptSize.y = bm.bmHeight;

    // Map the coords from Device to Logical
    DPtoLP (hDC, &ptSize, 1);

    // Initialize to the upper left corner
    ptOrg.x = ptOrg.y = 0;

    // Map the coords from Device to Logical
    DPtoLP (hDCMem, &ptOrg, 1);

    // Copy the bitmap from the memory DC to the destin DC
    BitBlt (hDC,
            xDstOrg, yDstOrg,
            ptSize.x, ptSize.y,
            hDCMem,
            ptOrg.x, ptOrg.y,
            SRCCOPY);
    // Restore the old object
    SelectObject (hDCMem, hOldBitmap);

    // We no longer need this resource
    MyDeleteDC (hDCMem); hDCMem = NULL;
} // End DrawBitmap


//***************************************************************************
//  $signum
//
//  Return -1, 0, +1 depending upon the sign of a given integer
//***************************************************************************

int signum
    (APLINT val)

{
    if (val < 0)
        return -1;
    else
       return (val > 0);
} // End signum


//***************************************************************************
//  $signumint
//
//  Return -1, 0, +1 depending upon the sign of a given integer
//***************************************************************************

int signumint
    (APLINT val)                // Source

{
    return (val < 0) ? -1 : (val > 0);
} // End signumint


//***************************************************************************
//  $signumflt
//
//  Return -1, 0, +1 depending upon the sign of a given float
//***************************************************************************

int signumflt
    (APLFLOAT val)

{
    return SIGN_APLFLOAT (val) ? -1 : (val > 0);
} // End signumflt


//***************************************************************************
//  $signumrat
//
//  Is an alias for mpq_sgn which already returns -1, 0, or 1.
//***************************************************************************


//***************************************************************************
//  $signumvfp
//
//  Return -1, 0, +1 depending upon the sign of a given VFP
//***************************************************************************

int signumvfp
    (LPAPLVFP val)

{
    return (SIGN_APLVFP (val) ? -1 : mpfr_sgn (val) > 0);
} // End signumvfp


//***************************************************************************
//  $GetSystemDate
//
//  Fill in the system date with the time set to zero
//***************************************************************************

void GetSystemDate
    (LPSYSTEMTIME lpst)

{
    // Fill in the date & time
    GetSystemTime (lpst);

    // Zero the time
    lpst->wHour         =
    lpst->wMinute       =
    lpst->wSecond       =
    lpst->wMilliseconds = 0;
} // End GetSystemDate


//***************************************************************************
//  $GetDlgItemInt64
//
//  64-bit version of GetDlgItemInt
//***************************************************************************

APLINT GetDlgItemInt64
    (HWND   hDlg,                       // Dialog box handle
     int    nIDDlgItem,                 // Identifier of the control
     UBOOL *lpTranslated,               // Return value of success (TRUE) or failure (FALSE) (may be NULL)
     UBOOL  bSigned)                    // TRUE iff the result may be signed

{
    WCHAR   wszTemp[32];                // Output save area
    LPWCHAR lpw;                        // Temporary ptr
    APLINT  aplInt;                     // The result

    // Get the value as text
    if (GetDlgItemTextW (hDlg, nIDDlgItem, wszTemp, strcountof (wszTemp)) NE 0)
    {
        // Skip over leading blanks
        lpw = SkipPastCharW (wszTemp, L' ');

        if (bSigned
         && lpw[0] EQ UTF16_OVERBAR)
            lpw[0] = L'-';
        // Convert to a number
        if (sscanfW (lpw, L"%I64u", &aplInt) EQ 1)
        {
            // Mark as successful
            if (lpTranslated)
                *lpTranslated = TRUE;

            // Return the result
            return aplInt;
        } // End IF
    } // End IF

    // Mark as in error
    if (lpTranslated)
        *lpTranslated = FALSE;

    // Return a result of zero
    return 0;
} // End GetDlgItemInt64


//***************************************************************************
//  $CharFromPos
//
//  Handle EM_CHARFROMPOS result, taking into account 16-bit integer overflow
//***************************************************************************

void CharFromPos
    (HWND    hWndEC,            // Edit Ctrl window handle
     LRESULT lResult,           // The result from SendMessage
     LPLONG  lplLineNum,        // Ptr to Line #
     LPLONG  lplCharPos)        // Ptr to Char Position

{
    LONG lLineNum,              // The line #
         lCharPos;              // The char position

    // Split out the values
    lLineNum = HIWORD (lResult);
    lCharPos = LOWORD (lResult);

    // If the result is valid, ...
    if (lResult != -1)
    {
        LONG lLineVis,          // The # of the first visible line
             lLinePos;          // The char position of the start of the line

        lLineVis = (LONG) SendMessageW (hWndEC, EM_GETFIRSTVISIBLELINE, 0, 0);
        while (lLineNum < lLineVis)
            lLineNum += 65536;

        lLinePos = (LONG) SendMessageW (hWndEC, EM_LINEINDEX, lLineNum, 0);
        while (lCharPos < lLinePos)
            lCharPos += 65536;
    } // End IF

    // Return the values
    *lplLineNum = lLineNum;
    *lplCharPos = lCharPos;
} // End CharFromPos


//***************************************************************************
//  $MySprintf
//
//  String safe version of <wsprintf>
//***************************************************************************

int MySprintf
    (LPCHAR lpBuffer,       // Ptr to buffer
     size_t iCount,         // Size of buffer (in bytes)
     LPCHAR lpszFmt,        // Ptr to format string
            ...)            // The variable list

{
    HRESULT  hResult;       // The result of <StringCbVPrintfW>
    va_list  vl;            // Ptr to variable list

    // Initialize the variable list
    va_start (vl, lpszFmt);

    // wsprintfW the list
    hResult = StringCbVPrintf (lpBuffer,
                               iCount,
                               lpszFmt,
                               vl);
    // End the variable list
    va_end (vl);

#ifdef DEBUG
    // If it failed, ...
    if (FAILED (hResult))
        DbgBrk ();          // #ifdef DEBUG
#endif

    return lstrlen (lpBuffer);
} // End MySprintf


//***************************************************************************
//  $MySprintfW
//
//  String safe version of <wsprintfW>
//***************************************************************************

int MySprintfW
    (LPWCHAR lpwBuffer,     // Ptr to buffer
     size_t  iCount,        // Size of buffer (in bytes)
     LPWCHAR lpwszFmt,      // Ptr to format string
             ...)           // The variable list

{
    HRESULT  hResult;       // The result of <StringCbVPrintfW>
    va_list  vl;            // Ptr to variable list

    // Initialize the variable list
    va_start (vl, lpwszFmt);

    // wsprintfW the list
    hResult = StringCbVPrintfW (lpwBuffer,
                                iCount,
                                lpwszFmt,
                                vl);
    // End the variable list
    va_end (vl);

#ifdef DEBUG
    // If it failed, ...
    if (FAILED (hResult))
        DbgBrk ();          // #ifdef DEBUG
#endif

    return lstrlenW (lpwBuffer);
} // End MySprintfW


//***************************************************************************
//  $strncmpWA
//
//  String compare at length between WCHAR and char
//***************************************************************************

int strncmpWA
    (LPWCHAR lpwLft,        // Left arg as WCHAR
     LPCHAR  lpRht,         // Right arg as CHAR
     int     iLenLft)       // Length of left arg in WCHARs

{
    int i;

    // Loop through all of the WCHARs
    for (i = 0; i < iLenLft; i++)
    if (lpwLft[i] NE lpRht[i])
        return signumint (lpwLft[i] - lpRht[i]);

    // The strings are equal
    return 0;
} // End strncmpWA


//***************************************************************************
//  $WriteFileWA
//
//  WriteFile of a WCHAR string to a char
//***************************************************************************

void WriteFileWA
    (HANDLE  hFile,             // File handle
     LPWCHAR lpwChar,           // Ptr to string to write out
     int     iLen,              // # WCHARs in the above string
     LPDWORD lpdwBytesOut,
     LPVOID  lpVoid)

{
    WCHAR wszTemp[1024];
    int   iChunkLen,            // Chunk length in WCHARs
          i;
    DWORD dwBytesOut = 0;

    // Initialize the dwBytesOut
    *lpdwBytesOut = 0;

    // Loop through the file chunks
    while (iLen)
    {
        // Calculate the chunk length in WCHARs
        iChunkLen = min (countof (wszTemp), iLen);

        // Move it down to strip out the high-order zeros
        for (i = 0; i < iChunkLen; i++)
            ((LPCHAR) wszTemp)[i] = ((LPCHAR) lpwChar)[2 * i];

        // Write out the char string
        WriteFile (hFile, wszTemp, iChunkLen, &dwBytesOut, lpVoid);

        // Account for it
        lpwChar += iChunkLen;
        iLen -= iChunkLen;
        *lpdwBytesOut += dwBytesOut;
    } // End WHILE
} // End WriteFileWA


//***************************************************************************
//  $strcpyW
//
//  Same as lstrcpyW but checks the return code and handles page faults
//    however, unlike lstrcpyW, it has no return value.
//***************************************************************************

void strcpyW
    (LPWSTR       lpwDst,       // Ptr to destination
     const WCHAR *lpwSrc)       // Ptr to source

{
    if (FAILED (StringCchCopyW (lpwDst,
                                lstrlenW (lpwSrc) + 1,
                                lpwSrc)))
        Assert (FALSE);
} // End strcpyW


//***************************************************************************
//  $strcatW
//
//  Same as lstrcatW but checks the return code and handles page faults
//    however, unlike lstrcatW, it has no return value.
//***************************************************************************

void strcatW
    (LPWSTR       lpwDst,       // Ptr to destination
     const WCHAR *lpwSrc)       // Ptr to source

{
    if (FAILED (StringCbCatW (lpwDst,
                              (lstrlenW (lpwDst)
                             + lstrlenW (lpwSrc)
                             + 1) * sizeof (WCHAR),
                              lpwSrc)))
        Assert (FALSE);
} // End strcatW


//***************************************************************************
//  $strcpyn
//
//  Same as lstrcpyn but checks the return code and handles page faults
//    however, unlike lstrcpyn, it has no return value.
//***************************************************************************

void strcpyn
    (LPSTR       lpDst,         // Ptr to destination
     const char *lpSrc,         // Ptr to source
     int         iMaxLength)    // Length in bytes including terminating EOS

{
    if (FAILED (StringCbCopyN (lpDst,
                               lstrlen (lpSrc) + 1,
                               lpSrc,
                               iMaxLength)))
        Assert (FALSE);
} // End strcpyn


//***************************************************************************
//  $strcpynW
//
//  Same as lstrcpynW but checks the return code and handles page faults
//    however, unlike lstrcpynW, it has no return value.
//***************************************************************************

void strcpynW
    (LPWSTR       lpwDst,       // Ptr to destination
     const WCHAR *lpwSrc,       // Ptr to source
     int          iMaxLength)   // Length in WCHARs including terminating EOS

{
    if (FAILED (StringCbCopyNW (lpwDst,
                                (lstrlenW (lpwSrc) + 1) * sizeof (WCHAR),
                                lpwSrc,
                                iMaxLength * sizeof (WCHAR))))
        Assert (FALSE);
} // End strcpynW


//***************************************************************************
//  End of File: common.c
//***************************************************************************
