//***************************************************************************
//  NARS2000 -- Common Processing Routines
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
#include <direct.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


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
                        GetClassLongPtrW (hWnd, GCL_MENUNAME),
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
//  $CenterWindow
//
//  Center the window on the desktop
//************************************************************************

void CenterWindow
    (HWND hWnd)

{
    RECT  rcWnd;
    POINT PosCtr;

    GetWindowRect (GetDesktopWindow (), &rcWnd);    // Get the desktop's RECT
    PosCtr.x = (rcWnd.right  + rcWnd.left) / 2;     // Center horizontally
    PosCtr.y = (rcWnd.bottom + rcWnd.top ) / 2;     // Center vertically
    MoveWindowPos (hWnd, PosCtr);

} // End CenterWindow


//****************************************************************************
//  $IsWhite
//
//  Determine if a char is white space
//****************************************************************************

BOOL IsWhite
    (char c)

{
    return (c EQ ' ' || c EQ '\t') ? TRUE : FALSE;
} // End IsWhite


//****************************************************************************
//  $IsWhiteW
//
//  Determine if a char is white space
//****************************************************************************

BOOL IsWhiteW
    (WCHAR wc)

{
    return (wc EQ L' ' || wc EQ L'\t') ? TRUE : FALSE;
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
          wszExt  [_MAX_EXT];

    if (GetModuleFileNameW (hInstance, wszAppDPFE, sizeof (wszAppDPFE)))
    {
        // Split out the drive and path from the module filename
        _wsplitpath (wszAppDPFE, wszDrive, wszDir, wszFname, wszExt);

        // Create the .HLP file name
        _wmakepath  (wszHlpDPFE, wszDrive, wszDir, wszFname, L".HLP");
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
    (LPWCHAR lpMemDest,
     UINT    uLen,
     WCHAR   wc)

{
    while (uLen--)
        *lpMemDest++ = wc;
    return lpMemDest;
} // End FillMemoryW


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

BOOL CALLBACK EnumCallbackRemoveProp
    (HWND   hWnd,           // Handle of window with property
     LPCSTR lpszString,     // Property string or atom
     HANDLE hData)          // Data handle
{
    // Remove the property
    RemoveProp (hWnd, lpszString);

    // Continue enumerating
    return TRUE;
} // End EnumCallbackRemoveProp


//***************************************************************************
//  End of File: common.c
//***************************************************************************
