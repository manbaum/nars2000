//***************************************************************************
//  NARS2000 -- Common Processing Routines
//***************************************************************************

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
                        GetWindowLong (hWnd, GWL_STYLE),
                        GetClassLong  (hWnd, GCL_MENUNAME),
                        GetWindowLong (hWnd, GWL_EXSTYLE)
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
//  $SkipWhite
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
//  $GetRegDword
//
//  Get a registry dword value.
//************************************************************************

DWORD GetRegDword
    (HKEY   hKey,
     LPCHAR pSubKey,
     LPCHAR pKeyStr,
     DWORD  dwDefVal)

{
    HKEY  hKey2;
    DWORD dwActVal = 0, // Set to known value in case stored in shorter format
          dwSize = sizeof (dwActVal);

    if (RegCreateKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // Handle of key to query
                             pKeyStr,       // Address of name of value to query
                             NULL,          // Reserved
                             NULL,          // Address of buffer for value type
                             (LPCHAR) &dwActVal, // Address of data buffer
                             &dwSize)       // Address of data buffer size
            NE ERROR_SUCCESS)
            dwActVal = dwDefVal;

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        dwActVal = dwDefVal;

    return dwActVal;
} // End GetRegDword


//************************************************************************
//  $GetRegQword
//
//  Get a registry Qword value.
//************************************************************************

QWORD GetRegQword
    (HKEY   hKey,
     LPCHAR pSubKey,
     LPCHAR pKeyStr,
     QWORD  qwDefVal)

{
    HKEY  hKey2;
    QWORD qwActVal = 0; // Set to known value in case stored in shorter format
    DWORD dwSize = sizeof (qwActVal);

    if (RegCreateKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // Handle of key to query
                             pKeyStr,       // Address of name of value to query
                             NULL,          // Reserved
                             NULL,          // Address of buffer for value type
                             (LPCHAR) &qwActVal, // Address of data buffer
                             &dwSize)       // Address of data buffer size
            NE ERROR_SUCCESS)
            qwActVal = qwDefVal;

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        qwActVal = qwDefVal;

    return qwActVal;
} // End GetRegQword


//************************************************************************
//  $GetRegWchar
//
//  Get a registry WCHAR value.
//************************************************************************

WCHAR GetRegWchar
    (HKEY   hKey,
     LPCHAR pSubKey,
     LPCHAR pKeyStr,
     WCHAR  wcDefVal)

{
    HKEY  hKey2;
    WCHAR wcActVal = 0; // Set to known value in case stored in shorter format
    DWORD dwSize = sizeof (wcActVal);

    if (RegCreateKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // Handle of key to query
                             pKeyStr,       // Address of name of value to query
                             NULL,          // Reserved
                             NULL,          // Address of buffer for value type
                             (LPCHAR) &wcActVal, // Address of data buffer
                             &dwSize)       // Address of data buffer size
            NE ERROR_SUCCESS)
            wcActVal = wcDefVal;

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        wcActVal = wcDefVal;

    return wcActVal;
} // End GetRegWchar


//***************************************************************************
//  $GetRegStr
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

    if (RegCreateKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // Handle of key to query
                             pKeyStr,       // Address of name of value to query
                             NULL,          // Reserved
                             NULL,          // Address of buffer for value type
                             pActStr,       // Address of data buffer
                             &iActSize)     // Address of data buffer size
            NE ERROR_SUCCESS)
            lstrcpy (pActStr, pDefVal);

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        lstrcpy (pActStr, pDefVal);
} // End GetRegStr


//***************************************************************************
//  $GetRegGlbChar
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
     HGLOBAL hDefVal,
     LPWCHAR pDefVal)

{
    HKEY    hKey2;
    HGLOBAL hGlbVal;
    LPVOID  lpMem;
    UINT    ByteRes;
    int     iActSize = 0;
    UINT    uLen;

    if (RegCreateKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        // Request the data size
        switch (RegQueryValueEx (hKey2,     // Handle of key to query
                                 pKeyStr,   // Address of name of value to query
                                 NULL,      // Reserved
                                 NULL,      // Address of buffer for value type
                                 NULL,      // Address of data buffer
                                 &iActSize))// Address of data buffer size
        {
            case ERROR_FILE_NOT_FOUND:
                // The keyname was not found --
                //   use the default value
                break;

            case ERROR_SUCCESS:     // The stored value is of zero length
            case ERROR_MORE_DATA:
                // iActSize contains the # bytes needed
                uLen = iActSize / sizeof (APLCHAR);

                // Calculate space needed for the result
                ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, uLen, 1);

                // Allocate space for the data
                // Note, we can't use DbgGlobalAlloc here as the
                //   PTD has not been allocated as yet
                hGlbVal = MyGlobalAlloc (GHND, ByteRes);
                if (!hGlbVal)
                    return hGlbVal;

                // Lock the memory to get a ptr to it
                lpMem = MyGlobalLock (hGlbVal);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
                // Fill in the header values
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = ARRAY_CHAR;
////////////////lpHeader->Perm       = 0;       // Already zero from GHND
////////////////lpHeader->SysVar     = 0;       // ...
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = uLen;
                lpHeader->Rank       = 1;
#undef  lpHeader

                // Save the dimension
                *VarArrayBaseToDim (lpMem) = uLen;

                // Skip over the header and dimensions to the data
                lpMem = VarArrayBaseToData (lpMem, 1);

                RegQueryValueEx (hKey2,         // Handle of key to query
                                 pKeyStr,       // Address of name of value to query
                                 NULL,          // Reserved
                                 NULL,          // Address of buffer for value type
                                 (LPCHAR) lpMem,// Address of data buffer
                                 &iActSize);    // Address of data buffer size
                RegCloseKey (hKey2); hKey2 = NULL;

                // See if this is the default value
                iActSize = lstrlenW (pDefVal);
                for (uLen = 0; uLen < (UINT) iActSize; uLen++)
                if (((LPWCHAR) lpMem)[uLen] NE pDefVal[uLen])
                    break;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbVal); lpMem = NULL;

                // If it's the default value, ...
                if (uLen EQ (UINT) iActSize)
                {
                    MyGlobalFree (hGlbVal); hGlbVal = NULL;
                } else
                    hDefVal = hGlbVal;
                break;

            defstop
                break;
        } // End SWITCH
    } else
        hDefVal = NULL;

    return hDefVal;
} // End GetRegGlbChar
#undef  APPEND_NAME


//***************************************************************************
//  $GetRegBinary
//
//  Get a registry binary value.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GetRegBinary"
#else
#define APPEND_NAME
#endif
void GetRegBinary
    (HKEY    hKey,          // Main key
     LPCHAR  pSubKey,       // Ptr to sub key
     LPCHAR  pKeyStr,       // Ptr to key name
     LPVOID  pActVal,       // Ptr to actual value
     UINT    uLen,          // Length of ...
     LPVOID  pDefVal)       // Ptr to default value

{
    HKEY  hKey2;
    DWORD dwSize = uLen;

    if (RegCreateKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        if (RegQueryValueEx (hKey2,         // Handle of key to query
                             pKeyStr,       // Address of name of value to query
                             NULL,          // Reserved
                             NULL,          // Address of buffer for value type
                             (LPCHAR) pActVal, // Address of data buffer
                             &dwSize)       // Address of data buffer size
            NE ERROR_SUCCESS)
            CopyMemory (pActVal, pDefVal, uLen);

        RegCloseKey (hKey2); hKey2 = NULL;
    } else
        CopyMemory (pActVal, pDefVal, uLen);
} // End GetRegBinary
#undef  APPEND_NAME


//***************************************************************************
//  $GetFileNames
//
//  Construct some file and directory names
//***************************************************************************

void GetFileNames
    (HINSTANCE hInstance)

{
    char szDir  [_MAX_DIR],
         szDrive[_MAX_DRIVE],
         szFname[_MAX_FNAME],
         szExt  [_MAX_EXT],
         szTemp [_MAX_PATH];

    if (GetModuleFileName (hInstance, szAppDPFE, sizeof (szAppDPFE)))
    {
        // Split out the drive and path from the module filename
        _splitpath (szAppDPFE, szDrive, szDir, szFname, szExt);

        // Create the .HLP file name
        _makepath  (szHlpDPFE, szDrive, szDir, szFname, ".HLP");

        // Create the Load and the Save Workspace directory names
        _makepath  (szTemp, szDrive, szDir, NULL, NULL);
        lstrcat    (szTemp, WKSNAME "\\");      // Must end with a backslash

        // Ensure the directory exists
        _mkdir (szTemp);

        // Convert to wide
        A2W (wszLoadDir, szTemp, _MAX_PATH);
        lstrcpyW (wszSaveDir, wszLoadDir);

        // Save the default drive letter for later use
        A2W (wszDefDrive, szDrive, _MAX_DRIVE);

////////// Create the initial directory for File Open & Save
////////_makepath  (szInitDir, szDrive, szDir, NULL,    NULL);
    } // End IF
} // End GetFileNames


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
//  End of File: common.c
//***************************************************************************
