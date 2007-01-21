//***************************************************************************
//  NARS2000 -- Common Processing Routines
//***************************************************************************

#pragma pack (1)
#define STRICT
#include <windows.h>

#include "main.h"
#include "datatype.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//************************************************************************
//  ClientToWindowSize
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

void MoveWindowPos
    (HWND hWnd, POINT PosCtr)
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

void MoveWindowPosSize
    (HWND hWnd, POINT PosCtr, SIZE Size)
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

void CenterWindow
    (HWND hWnd)
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

BOOL IsWhite
    (char c)

{
    return (c EQ ' ' || c EQ '\t') ? TRUE : FALSE;

} // End IsWhite


//****************************************************************************
//  SkipWhite
//
//  Skip over leading white space
//****************************************************************************

LPCHAR SkipWhite
    (LPCHAR p)

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

DWORD GetRegDword
    (HKEY   hKey,
     LPCHAR pSubKey,
     LPCHAR pKeyStr,
     DWORD  dwDefVal)

{
    HKEY hKey2;
    DWORD dwActVal = 0, // Set to known value in case stored in shorter format
          dwSize = sizeof (dwActVal);

    if (RegOpenKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // handle of key to query
                             pKeyStr,       // address of name of value to query
                             NULL,          // reserved
                             NULL,          // address of buffer for value type
                             (LPCHAR) &dwActVal, // address of data buffer
                             &dwSize)       // address of data buffer size
            NE ERROR_SUCCESS)
            dwActVal = dwDefVal;

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        dwActVal = dwDefVal;

    return dwActVal;
} // End GetRegDword


//************************************************************************
//  GetRegQword
//
//  Get a registry Qword value.
//************************************************************************

QWORD GetRegQword
    (HKEY   hKey,
     LPCHAR pSubKey,
     LPCHAR pKeyStr,
     QWORD  qwDefVal)

{
    HKEY hKey2;
    QWORD qwActVal = 0; // Set to known value in case stored in shorter format
    DWORD dwSize = sizeof (qwActVal);

    if (RegOpenKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // handle of key to query
                             pKeyStr,       // address of name of value to query
                             NULL,          // reserved
                             NULL,          // address of buffer for value type
                             (LPCHAR) &qwActVal, // address of data buffer
                             &dwSize)       // address of data buffer size
            NE ERROR_SUCCESS)
            qwActVal = qwDefVal;

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        qwActVal = qwDefVal;

    return qwActVal;
} // End GetRegQword


//************************************************************************
//  GetRegWchar
//
//  Get a registry WCHAR value.
//************************************************************************

WCHAR GetRegWchar
    (HKEY   hKey,
     LPCHAR pSubKey,
     LPCHAR pKeyStr,
     WCHAR  wcDefVal)

{
    HKEY hKey2;
    WCHAR wcActVal = 0; // Set to known value in case stored in shorter format
    DWORD dwSize = sizeof (wcActVal);

    if (RegOpenKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // handle of key to query
                             pKeyStr,       // address of name of value to query
                             NULL,          // reserved
                             NULL,          // address of buffer for value type
                             (LPCHAR) &wcActVal, // address of data buffer
                             &dwSize)       // address of data buffer size
            NE ERROR_SUCCESS)
            wcActVal = wcDefVal;

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        wcActVal = wcDefVal;

    return wcActVal;
} // End GetRegWchar


//***************************************************************************
//  GetRegStr
//
//  Get a registry string value.
//***************************************************************************

void GetRegStr
    (HKEY   hKey,
     LPCHAR pSubKey,
     LPCHAR pKeyStr,
     LPCHAR pActStr,
     int    iActSize,
     LPCHAR pDefVal)

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
//  GetRegGlbChar
//
//  Get a registry HGLOBAL value.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GetRegGlbChar"
#else
#define APPEND_NAME
#endif
HGLOBAL GetRegGlbChar
    (HKEY    hKey,
     LPCHAR  pSubKey,
     LPCHAR  pKeyStr,
     LPWCHAR pDefVal)

{
    HKEY    hKey2;
    HGLOBAL hGlbRes;
    LPVOID  lpMem;
    BOOL    bDefVal;
    UINT    ByteRes;
////char    szBuf;
    int     iActSize = 0;
    UINT    uLen;

    uLen = lstrlenW (pDefVal);

    if (RegOpenKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        // Request the data size
        switch (RegQueryValueEx (hKey2,     // handle of key to query
                                 pKeyStr,   // address of name of value to query
                                 NULL,      // reserved
                                 NULL,      // address of buffer for value type
                                 NULL,      // address of data buffer
                                 &iActSize))// address of data buffer size
        {
            case ERROR_FILE_NOT_FOUND:
                // The keyname was not found --
                //   allocate space for the default value
                iActSize = uLen * sizeof (APLCHAR);
                bDefVal = TRUE;

                break;

            case ERROR_SUCCESS:     // The stored value is of zero length
            case ERROR_MORE_DATA:
                // iActSize contains the # bytes needed
                uLen = iActSize / sizeof (APLCHAR);
                bDefVal = FALSE;

                break;

            default:
                DbgBrk ();

                break;
        } // End SWITCH

        // Allocate space for the data
        ByteRes = sizeof (VARARRAY_HEADER)
                + sizeof (APLDIM) * 1
                + iActSize;
        hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
        if (!hGlbRes)
            return hGlbRes;

        // Lock the memory to get a ptr to it
        lpMem = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)

        // Fill in the header values
        lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->Perm       = 0;
////////lpHeader->SysVar     = 0;
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = uLen;
        lpHeader->Rank       = 1;

        // Save the dimension
        *VarArrayBaseToDim (lpHeader) = uLen;

        // Skip over the header and dimensions to the data
        lpHeader = VarArrayBaseToData (lpHeader, 1);

        // Split cases based upon whether the data is in the
        //   registry or the default value
        if (bDefVal)
            // Copy the data to memory
            CopyMemory (lpHeader, pDefVal, uLen * sizeof (APLCHAR));
        else
            RegQueryValueEx (hKey2,         // handle of key to query
                             pKeyStr,       // address of name of value to query
                             NULL,          // reserved
                             NULL,          // address of buffer for value type
                             (LPCHAR) lpHeader,// address of data buffer
                             &iActSize);    // address of data buffer size
#undef  lpHeader

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMem = NULL;

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        hGlbRes = NULL;

    return hGlbRes;
} // End GetRegGlbChar
#undef  APPEND_NAME


//***************************************************************************
//  GetRegBinary
//
//  Get a registry binary value.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GetRegBinary"
#else
#define APPEND_NAME
#endif
void GetRegBinary
    (HKEY    hKey,
     LPCHAR  pSubKey,
     LPCHAR  pKeyStr,
     UINT    uLen,
     LPVOID  pActVal,
     LPVOID  pDefVal)

{
    HKEY hKey2;
    DWORD dwSize = uLen;

    if (RegOpenKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // handle of key to query
                             pKeyStr,       // address of name of value to query
                             NULL,          // reserved
                             NULL,          // address of buffer for value type
                             (LPCHAR) &pActVal, // address of data buffer
                             &dwSize)       // address of data buffer size
            NE ERROR_SUCCESS)
            CopyMemory (pActVal, pDefVal, uLen);

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        CopyMemory (pActVal, pDefVal, uLen);
} // End GetRegBinary
#undef  APPEND_NAME


//***************************************************************************
//  GetFileNames
//
//  Construct some filenames
//***************************************************************************

void GetFileNames
    (HINSTANCE hInstance)

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
