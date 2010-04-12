//***************************************************************************
//  NARS2000 -- Common Processing Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
    return (c EQ ' ' || c EQ '\t') ? TRUE : FALSE;
} // End IsWhite


//****************************************************************************
//  $IsWhiteW
//
//  Determine if a char is white space
//****************************************************************************

UBOOL IsWhiteW
    (WCHAR wc)

{
    return (wc EQ L' ' || wc EQ WC_HT) ? TRUE : FALSE;
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
    // Skip over white space
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
          wszExt  [_MAX_EXT],
          wszComctl32FileVer[64];           // Comctl32.dll file version string

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
        swscanf (wszComctl32FileVer,
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


//***************************************************************************
//  $nop
//***************************************************************************

void nop
    (void)

{
} // End nop


//***************************************************************************
//  $EnumCallbackRemoveProp
//
//  Callback function to remove all window properties
//***************************************************************************

UBOOL CALLBACK EnumCallbackRemoveProp
    (HWND    hWnd,          // Handle of window with property
     LPCWSTR lpwszString,   // Property string or atom
     HANDLE  hData)         // Data handle
{
    // Remove the property
    RemovePropW (hWnd, lpwszString);

    // Continue enumerating
    return TRUE;
} // End EnumCallbackRemoveProp


//***************************************************************************
//  $IsValidHandle
//
//  Return TRUE iff the given handle is valid
//***************************************************************************

UBOOL IsValidHandle
    (HGLOBAL hGlbSrc)

{
    // Clear the ptr type bits
    hGlbSrc = ClrPtrTypeDir (hGlbSrc);

    __try
    {
        return ((hGlbSrc NE NULL) && (GlobalFlags (hGlbSrc) NE GMEM_INVALID_HANDLE));
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
    __try
    {
        return !IsBadReadPtr (lpMemSrc, uLen);
    } __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return FALSE;
    } // End __try/__except
} // End IsValidPtr


//***************************************************************************
//  End of File: common.c
//***************************************************************************
