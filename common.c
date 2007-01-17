//***************************************************************************
//  NARS2000 -- Common Processing Routines
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>
#include "main.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


extern char szInitDir[];

char szAppDPFE  [_MAX_PATH];        // .EXE drive, path, filename.ext
char szHlpDPFE  [_MAX_PATH];        // .HLP ...

//************************************************************************
//  ClientToWindowSize
//
//  Convert client area size to window size
//************************************************************************

SIZE ClientToWindowSize (HWND hWnd, SIZE Size)

{
    RECT rc;

    // Create a rectangle
    rc.left   = 0;
    rc.right  = rc.left + Size.cx;
    rc.top    = 0;
    rc.bottom = rc.top  + Size.cy;

    // Adjust the rectangle from client to window
    AdjustWindowRectEx (&rc,
                        GetWindowLong (hWnd, GWL_STYLE),
                        GetClassLong  (hWnd, GCL_MENUNAME),
                        GetWindowLong (hWnd, GWL_EXSTYLE)
                       );
    Size.cx = rc.right  - rc.left;
    Size.cy = rc.bottom - rc.top;

    return Size;
} // End ClientToWindowSize


//************************************************************************
//  MoveWindowPos
//
//  Move the window to a position based upon the previous center
//************************************************************************

void MoveWindowPos (HWND hWnd, POINT PosCtr)
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
//  MoveWindowPosSize
//
//  Move the window to a position based upon the previous center and size
//************************************************************************

void MoveWindowPosSize (HWND hWnd, POINT PosCtr, SIZE Size)
{
    MoveWindow (hWnd,
                PosCtr.x - Size.cx/2,       // Left coordinate
                PosCtr.y - Size.cy/2,       // Top ...
                Size.cx,                    // Width
                Size.cy,                    // Height
                FALSE);                     // No re-paint
} // End MoveWindowPosSize


//************************************************************************
//  CenterWindow
//
//  Center the window on the desktop
//************************************************************************

void CenterWindow (HWND hWnd)
{
    RECT rcWnd;
    POINT PosCtr;

    GetWindowRect (GetDesktopWindow (), &rcWnd);    // Get the desktop's RECT
    PosCtr.x = (rcWnd.right  + rcWnd.left) / 2;     // Center horizontally
    PosCtr.y = (rcWnd.bottom + rcWnd.top ) / 2;     // Center vertically
    MoveWindowPos (hWnd, PosCtr);

} // End CenterWindow


//****************************************************************************
//  IsWhite
//
//  Determine if a char is white space
//****************************************************************************

BOOL IsWhite (char c)

{
    return (c EQ ' ' || c EQ '\t') ? TRUE : FALSE;

} // End IsWhite


//****************************************************************************
//  SkipWhite
//
//  Skip over leading white space
//****************************************************************************

char *SkipWhite (char *p)

{
    // Skip over white space
    while (IsWhite (*p))
        p++;

    return p;

} // End SkipWhite


//************************************************************************
//  GetRegDword
//
//  Get a registry dword value.
//************************************************************************

DWORD GetRegDword (HKEY hKey,
                   char *pSubKey,
                   char *pKeyStr,
                   DWORD dwDefVal)

{
    HKEY hKey2;
    DWORD dwActVal = 0, dwSize = sizeof (dwActVal);

    if (RegOpenKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // handle of key to query
                             pKeyStr,       // address of name of value to query
                             NULL,          // reserved
                             NULL,          // address of buffer for value type
                             (char *) &dwActVal, // address of data buffer
                             &dwSize)       // address of data buffer size
            NE ERROR_SUCCESS)
            dwActVal = dwDefVal;

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        dwActVal = dwDefVal;

    return dwActVal;
} // End GetRegDword


//***************************************************************************
//  GetRegStr
//
//  Get a registry string value.
//***************************************************************************

void GetRegStr (HKEY hKey,
                char *pSubKey,
                char *pKeyStr,
                char *pActStr,
                int  iActSize,
                char *pDefVal)

{
    HKEY hKey2;

    if (RegOpenKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // handle of key to query
                             pKeyStr,       // address of name of value to query
                             NULL,          // reserved
                             NULL,          // address of buffer for value type
                             pActStr,       // address of data buffer
                             &iActSize)     // address of data buffer size
            NE ERROR_SUCCESS)
            lstrcpy (pActStr, pDefVal);

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        lstrcpy (pActStr, pDefVal);
} // End GetRegStr


//***************************************************************************
//  GetFileNames
//
//  Construct some filenames
//***************************************************************************

void GetFileNames (HINSTANCE hInstance)

{
    char szDrive[_MAX_DRIVE],
         szDir[_MAX_DIR],
         szFname[_MAX_FNAME],
         szExt[_MAX_EXT];

    if (GetModuleFileName (hInstance, szAppDPFE, sizeof (szAppDPFE)))
    {
        // Split out the drive and path from the module filename
        _splitpath (szAppDPFE, szDrive, szDir, szFname, szExt);

        // Create the .HLP file name
        _makepath  (szHlpDPFE, szDrive, szDir, szFname, ".HLP");

        // Create the initial directory for File Open & Save
        _makepath  (szInitDir, szDrive, szDir, NULL,    NULL);
    } // End IF
} // End GetFileNames


//***************************************************************************
//  End of File: common.c
//***************************************************************************
