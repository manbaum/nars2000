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
                        GetWindowLongW (hWnd, GWL_STYLE),
                        GetClassLongW  (hWnd, GCL_MENUNAME),
                        GetWindowLongW (hWnd, GWL_EXSTYLE)
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
//  Get a registry HGLOBAL value as an APLCHAR.
//***************************************************************************

HGLOBAL GetRegGlbChar
    (HKEY    hKey,                              // Handle of root key
     LPCHAR  pSubKey,                           // Ptr to key subordinate to root
     LPCHAR  pKeyStr,                           // Ptr to name of value to query
     HGLOBAL hDefVal,                           // Default value global memory handle
     LPWCHAR pDefVal)                           // Ptr to default values

{
    return GetRegGlbCom (hKey,                  // Handle of root key
                         pSubKey,               // Ptr to key subordinate to root
                         pKeyStr,               // Ptr to name of value to query
                         hDefVal,               // Default value global memory handle
                         pDefVal,               // Ptr to default values
                         ARRAY_CHAR,            // Array storage type
                         sizeof (APLCHAR),      // Size of each item in the array
                         lstrlenW (pDefVal));   // Length of the default char vector
} // End GetRegGlbChar


//***************************************************************************
//  $GetRegGlbInt
//
//  Get a registry HGLOBAL value as an APLINT.
//***************************************************************************

HGLOBAL GetRegGlbInt
    (HKEY     hKey,                             // Handle of root key
     LPCHAR   pSubKey,                          // Ptr to key subordinate to root
     LPCHAR   pKeyStr,                          // Ptr to name of value to query
     HGLOBAL  hDefVal,                          // Default value global memory handle
     LPAPLINT pDefVal,                          // Ptr to default values
     UINT     uVecLen)                          // Length of the default integer vector

{
    return GetRegGlbCom (hKey,                  // Handle of root key
                         pSubKey,               // Ptr to key subordinate to root
                         pKeyStr,               // Ptr to name of value to query
                         hDefVal,               // Default value global memory handle
                         pDefVal,               // Ptr to default values
                         ARRAY_INT,             // Array storage type
                         sizeof (APLINT),       // Size of each item in the array
                         uVecLen);              // Length of the default integer vector
} // End GetRegGlbInt


//***************************************************************************
//  $GetRegGlbCom
//
//  Get a registry HGLOBAL value.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GetRegGlbCom"
#else
#define APPEND_NAME
#endif

HGLOBAL GetRegGlbCom
    (HKEY     hKey,                             // Handle of root key
     LPCHAR   pSubKey,                          // Ptr to key subordinate to root
     LPCHAR   pKeyStr,                          // Ptr to name of value to query
     HGLOBAL  hDefVal,                          // Default value global memory handle
     LPVOID   pDefVal,                          // Ptr to default values
     APLSTYPE aplTypeCom,                       // Array storage type
     UINT     aplSizeCom,                       // Size of each item in the array
     UINT     uVecLen)                          // Length of the default vector

{
    HKEY    hKey2;                              // Ptr to created key
    HGLOBAL hGlbRes;                            // Result global memory handle
    LPVOID  lpMemRes;                           // Ptr to result global memory
    UINT    ByteRes,                            // # bytes in the result
            uLen,                               // Loop counter
            uActLen,                            // Actual length of incoming value
            uTstLen;                            // Smaller of actual length and vector length
    int     iActSize = 0;                       // Actual size in bytes

    if (RegCreateKey (hKey, pSubKey, &hKey2) EQ ERROR_SUCCESS)
    {
        // Request the data size
        switch (RegQueryValueEx (hKey2,     // Handle of key to query
                                 pKeyStr,   // Ptr to name of value to query
                                 NULL,      // Reserved
                                 NULL,      // Ptr to buffer for value type
                                 NULL,      // Ptr to data buffer
                                &iActSize)) // Ptr to data buffer size
        {
            case ERROR_FILE_NOT_FOUND:
                // The keyname was not found --
                //   use the default value
                break;

            case ERROR_SUCCESS:     // The stored value is of zero length
            case ERROR_MORE_DATA:
                // iActSize contains the # bytes needed
                uActLen = iActSize / aplSizeCom;

                // Calculate space needed for the result
                ByteRes = (UINT) CalcArraySize (aplTypeCom, uActLen, 1);

                // Allocate space for the data
                // Note, we can't use DbgGlobalAlloc here as the
                //   PTD has not been allocated as yet
                hGlbRes = MyGlobalAlloc (GHND, ByteRes);
                if (!hGlbRes)
                    return hGlbRes;

                // Lock the memory to get a ptr to it
                lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
                // Fill in the header values
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = aplTypeCom;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////lpHeader->SysVar     = 0;           // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = uActLen;
                lpHeader->Rank       = 1;
#undef  lpHeader

                // Save the dimension
                *VarArrayBaseToDim (lpMemRes) = uActLen;

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayBaseToData (lpMemRes, 1);

                RegQueryValueEx (hKey2,             // Handle of key to query
                                 pKeyStr,           // Ptr to name of value to query
                                 NULL,              // Reserved
                                 NULL,              // Ptr to buffer for value type
                                 (LPCHAR) lpMemRes, // Ptr to data buffer
                                &iActSize);         // Ptr to data buffer size
                RegCloseKey (hKey2); hKey2 = NULL;

                // Test to see if the incoming value is the same as the default value
                uTstLen = min (uActLen, uVecLen);

                // Split cases based upon the array storage type
                switch (aplTypeCom)
                {
                    case ARRAY_CHAR:
                        for (uLen = 0; uLen < uTstLen; uLen++)
                        if (((LPAPLCHAR) lpMemRes)[uLen] NE ((LPAPLCHAR) pDefVal)[uLen])
                            break;
                        break;

                    case ARRAY_INT:
                        for (uLen = 0; uLen < uTstLen; uLen++)
                        if (((LPAPLINT)  lpMemRes)[uLen] NE ((LPAPLINT)  pDefVal)[uLen])
                            break;
                        break;

                    defstop
                        break;
                } // End SWITCH

                // We no longer need this ptr
                MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

                // If it's the default value, ...
                if (uActLen EQ uVecLen
                 && uLen    EQ uVecLen)
                {
                    MyGlobalFree (hGlbRes); hGlbRes = NULL;
                } else
                    hDefVal = hGlbRes;
                break;

            defstop
                break;
        } // End SWITCH
    } else
        hDefVal = NULL;

    return hDefVal;
} // End GetRegGlbCom
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
