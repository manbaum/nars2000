//***************************************************************************
//  NARS2000 -- My Routines For Debugging
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

#if RESDEBUG
#define WINVER       0x0501 // Needed for WINUSER.H definitions
#define _WIN32_WINNT 0x0501 // ...

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

#define OBJ_GLBLOCK     15
#define OBJ_GLBALLOC    16


// ************** DEBUGGING DATA ********************************************

UINT
 auLinNumPEN        [MAXOBJ],               //  1
 auLinNumBRUSH      [MAXOBJ],               //  2
 auLinNumDC         [MAXOBJ],               //  3
 auLinNumMETADC     [MAXOBJ],               //  4
 auLinNumPAL        [MAXOBJ],               //  5
 auLinNumFONT       [MAXOBJ],               //  6
 auLinNumBITMAP     [MAXOBJ],               //  7
 auLinNumREGION     [MAXOBJ],               //  8
 auLinNumMETAFILE   [MAXOBJ],               //  9
 auLinNumMEMDC      [MAXOBJ],               // 10
 auLinNumEXTPEN     [MAXOBJ],               // 11
 auLinNumENHMETADC  [MAXOBJ],               // 12
 auLinNumENHMETAFILE[MAXOBJ],               // 13
 auLinNumCOLORSPACE [MAXOBJ],               // 14
 auLinNumGLBLOCK    [MAXOBJ],               // 15
 auLinNumGLBALLOC   [MAXOBJ];               // 16

UINT *lpaauLinNum[] =
{&auLinNumPEN        [0],                   //  1
 &auLinNumBRUSH      [0],                   //  2
 &auLinNumDC         [0],                   //  3
 &auLinNumMETADC     [0],                   //  4
 &auLinNumPAL        [0],                   //  5
 &auLinNumFONT       [0],                   //  6
 &auLinNumBITMAP     [0],                   //  7
 &auLinNumREGION     [0],                   //  8
 &auLinNumMETAFILE   [0],                   //  9
 &auLinNumMEMDC      [0],                   // 10
 &auLinNumEXTPEN     [0],                   // 11
 &auLinNumENHMETADC  [0],                   // 12
 &auLinNumENHMETAFILE[0],                   // 13
 &auLinNumCOLORSPACE [0],                   // 14
 &auLinNumGLBLOCK    [0],                   // 15
 &auLinNumGLBALLOC   [0],                   // 16
};

int
 iCountPEN         = 0,                     //  1
 iCountBRUSH       = 0,                     //  2
 iCountDC          = 0,                     //  3
 iCountMETADC      = 0,                     //  4
 iCountPAL         = 0,                     //  5
 iCountFONT        = 0,                     //  6
 iCountBITMAP      = 0,                     //  7
 iCountREGION      = 0,                     //  8
 iCountMETAFILE    = 0,                     //  9
 iCountMEMDC       = 0,                     // 10
 iCountEXTPEN      = 0,                     // 11
 iCountENHMETADC   = 0,                     // 12
 iCountENHMETAFILE = 0,                     // 13
 iCountCOLORSPACE  = 0,                     // 14
 iCountGLBLOCK     = 0,                     // 15
 iCountGLBALLOC    = 0;                     // 16

int *lpiaCount[] =
{&iCountPEN        ,                        //  1
 &iCountBRUSH      ,                        //  2
 &iCountDC         ,                        //  3
 &iCountMETADC     ,                        //  4
 &iCountPAL        ,                        //  5
 &iCountFONT       ,                        //  6
 &iCountBITMAP     ,                        //  7
 &iCountREGION     ,                        //  8
 &iCountMETAFILE   ,                        //  9
 &iCountMEMDC      ,                        // 10
 &iCountEXTPEN     ,                        // 11
 &iCountENHMETADC  ,                        // 12
 &iCountENHMETAFILE,                        // 13
 &iCountCOLORSPACE ,                        // 14
 &iCountGLBLOCK    ,                        // 15
 &iCountGLBALLOC   ,                        // 16
};

HANDLE
 ahPEN         [MAXOBJ],                    //  1
 ahBRUSH       [MAXOBJ],                    //  2
 ahDC          [MAXOBJ],                    //  3
 ahMETADC      [MAXOBJ],                    //  4
 ahPAL         [MAXOBJ],                    //  5
 ahFONT        [MAXOBJ],                    //  6
 ahBITMAP      [MAXOBJ],                    //  7
 ahREGION      [MAXOBJ],                    //  8
 ahMETAFILE    [MAXOBJ],                    //  9
 ahMEMDC       [MAXOBJ],                    // 10
 ahEXTPEN      [MAXOBJ],                    // 11
 ahENHMETADC   [MAXOBJ],                    // 12
 ahENHMETAFILE [MAXOBJ],                    // 13
 ahCOLORSPACE  [MAXOBJ],                    // 14
 ahGLBLOCK     [MAXOBJ],                    // 15
 ahGLBALLOC    [MAXOBJ];                    // 16

HANDLE *lpaah[] =
{&ahPEN        [0],                         //  1
 &ahBRUSH      [0],                         //  2
 &ahDC         [0],                         //  3
 &ahMETADC     [0],                         //  4
 &ahPAL        [0],                         //  5
 &ahFONT       [0],                         //  6
 &ahBITMAP     [0],                         //  7
 &ahREGION     [0],                         //  8
 &ahMETAFILE   [0],                         //  9
 &ahMEMDC      [0],                         // 10
 &ahEXTPEN     [0],                         // 11
 &ahENHMETADC  [0],                         // 12
 &ahENHMETAFILE[0],                         // 13
 &ahCOLORSPACE [0],                         // 14
 &ahGLBLOCK    [0],                         // 15
 &ahGLBALLOC   [0],                         // 16
};

LPCHAR
 lpaFileNameGLBLOCK     [MAXOBJ],           // 15
 lpaFileNameGLBALLOC    [MAXOBJ];           // 16

LPCHAR *lpaaFileName[] =
{
 NULL,                                      //  1
 NULL,                                      //  2
 NULL,                                      //  3
 NULL,                                      //  4
 NULL,                                      //  5
 NULL,                                      //  6
 NULL,                                      //  7
 NULL,                                      //  8
 NULL,                                      //  9
 NULL,                                      // 10
 NULL,                                      // 11
 NULL,                                      // 12
 NULL,                                      // 13
 NULL,                                      // 14
 &lpaFileNameGLBLOCK[0],                    // 15
 &lpaFileNameGLBALLOC[0],                   // 16
};


//***************************************************************************
//  $SaveObj
//
//  Save the allocation of an object
//***************************************************************************

int _SaveObj
    (DWORD   dwType,
     HGDIOBJ hObject,
     UINT    uLine)         // Line #

{
    int *lpiCount;

    // Get a ptr to the counter
    lpiCount = lpiaCount[dwType - 1];

    if (*lpiCount >= (MAXOBJ - 1))
        return -1;

    // Save the line number
    lpaauLinNum[dwType - 1][*lpiCount] = uLine;

    // Count in another OBJECT
    lpaah[dwType - 1][*lpiCount] = hObject;

    // Increment the counter
    return (*lpiCount)++;
} // _SaveObj


//***************************************************************************
//  $DeleObj
//
//  Delete the allocation of an object
//***************************************************************************

void _DeleObj
    (DWORD   dwType,
     HGDIOBJ hObject)

{
    int i, iLen;
    int *lpiCount;

    // Get a ptr to the counter
    lpiCount = lpiaCount[dwType - 1];

    // Find this object in the array
    //   looking backwards
    iLen = *lpiCount;
    for (i = iLen - 1; i >= 0; i--)
    if (hObject EQ lpaah[dwType - 1][i])
        break;

    // If we didn't find it, ...
    if (i EQ iLen)
    {
        if (*lpiCount < MAXOBJ)
            DbgBrk ();
    } else
    {
        // Move down the saved values above this entry
        MoveMemory (&lpaauLinNum[dwType - 1][i],
                    &lpaauLinNum[dwType - 1][i + 1],
                    (iLen - i) * sizeof (lpaauLinNum[0][0]));
        MoveMemory (&lpaah[dwType - 1][i],
                    &lpaah[dwType - 1][i + 1],
                    (iLen - i) * sizeof (lpaah[0][0]));
        if (lpaaFileName[dwType - 1])
            MoveMemory (&lpaaFileName[dwType - 1][i],
                        &lpaaFileName[dwType - 1][i + 1],
                        (iLen - i) * sizeof (lpaaFileName[0][0]));
    } // End IF/ELSE

    if (*lpiCount < MAXOBJ)
    {
        // Count out another OBJECT
        if (*lpiCount)
            (*lpiaCount[dwType - 1])--;
        else
            DbgBrk ();
    } // End IF
} // _DeleObj


//***************************************************************************
//  $_LastLock
//
//  Format a message about the last place a global memory handle was locked
//    and allocated.
//***************************************************************************

void _LastLock
    (char   *szTemp,
     HGLOBAL hMem)

{
    int i, j,
        iLen;

    // Find this object in the array
    iLen = *lpiaCount[OBJ_GLBLOCK - 1];

    // Note we loop backwards to find the most recent entry
    for (i = iLen - 1; i >= 0; i--)
    if (hMem EQ lpaah[OBJ_GLBLOCK - 1][i])
        break;

    // Find this object in the array
    iLen = *lpiaCount[OBJ_GLBALLOC - 1];

    // Note we loop backwards to find the most recent entry
    for (j = iLen - 1; j >= 0; j--)
    if (hMem EQ lpaah[OBJ_GLBALLOC - 1][j])
        break;

    // If we didn't find it, ...
    if (i EQ iLen || j EQ iLen)
        DbgBrk ();
    else
        wsprintf (szTemp,
                  "The global (%p) was last locked in (%s#%d) and allocated in (%s#%d).",
                  hMem,
                  lpaaFileName[OBJ_GLBLOCK - 1][i],
                  lpaauLinNum[OBJ_GLBLOCK - 1][i],
                  lpaaFileName[OBJ_GLBALLOC - 1][j],
                  lpaauLinNum[OBJ_GLBALLOC - 1][j]);
} // End _LastLock


//***************************************************************************
//  $MyCreateCompatibleBitmap
//
//  Create a compatible bitmap
//***************************************************************************

HBITMAP _MyCreateCompatibleBitmap
    (HDC  hDC,      // Handle to device context
     int  nWidth,   // Width of bitmap, in pixels
     int  nHeight,  // Height of bitmap, in pixels
     UINT uLine)    // Line #

{
    HBITMAP hBitmap;

    hBitmap = CreateCompatibleBitmap (hDC, nWidth, nHeight);

    if (!hBitmap)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_BITMAP, hBitmap, uLine);

    return hBitmap;
} // _MyCreateCompatibleBitmap


//***************************************************************************
//  $MyCreateCompatibleDC
//
//  Create a compatible memory DC
//***************************************************************************

HDC _MyCreateCompatibleDC
    (HDC  hDC,                  // Handle to device context
     UINT uLine)                // Line #

{
    HDC hMEMDC;

    hMEMDC = CreateCompatibleDC (hDC);

    if (hMEMDC EQ NULL)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_MEMDC, hMEMDC, uLine);

    return hMEMDC;
} // _MyCreateCompatibleDC


//***************************************************************************
//  $MyCreatePen
//
//  Create a pen
//***************************************************************************

HPEN _MyCreatePen
    (int      fnPenStyle,   // Pen style
     int      nWidth,       // Pen width
     COLORREF crColor,      // Pen color
     UINT     uLine)        // Line #

{
    HPEN hPen;

    hPen = CreatePen (fnPenStyle, nWidth, crColor);

    if (!hPen)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_PEN, hPen, uLine);

    return hPen;
} // _MyCreatePen


//***************************************************************************
//  $MyCreateFontIndirect
//
//  Create a font indirectly
//***************************************************************************

HFONT _MyCreateFontIndirect
    (CONST LOGFONT *lplf,   // Pointer to logical font structure
     UINT          uLine)   // Line #

{
    HFONT hFont;

    hFont = CreateFontIndirect (lplf);

    if (!hFont)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_FONT, hFont, uLine);

    return hFont;
} // _MyCreateFontIndirect


//***************************************************************************
//  $MyCreatePolygonRgn
//
//  Create a polygon region
//***************************************************************************

HRGN _MyCreatePolygonRgn
    (CONST POINT *lppt,     // Pointer to array of points
     int  cPoints,          // Number of points in array
     int  fnPolyFillMode,   // Polygon-filling mode
     UINT uLine)            // Line #

{
    HRGN hRgn;

    hRgn = CreatePolygonRgn (lppt, cPoints, fnPolyFillMode);

    if (!hRgn)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_REGION, hRgn, uLine);

    return hRgn;
} // _MyCreatePolygonRgn


//***************************************************************************
//  $MyCreateRectRgnIndirect
//
//  Create a rectangular region indirect
//***************************************************************************

HRGN _MyCreateRectRgnIndirect
    (CONST RECT *lprc,      // Pointer to the rectangle
     UINT       uLine)      // Line #

{
    HRGN hRgn;

    hRgn = CreateRectRgnIndirect (lprc);

    if (!hRgn)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_REGION, hRgn, uLine);

    return hRgn;
} // _MyCreateRectRgnIndirect


//***************************************************************************
//  $MyCreateSolidBrush
//
//  Create a solid brush
//***************************************************************************

HBRUSH _MyCreateSolidBrush
    (COLORREF crColor,      // Brush color value
     UINT     uLine)        // Line #

{
    HBRUSH hBrush;

    hBrush = CreateSolidBrush (crColor);

    if (!hBrush)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_BRUSH, hBrush, uLine);

    return hBrush;
} // _MyCreateSolidBrush


//***************************************************************************
//  $MyDeleteDC
//
//  Delete a memory DC
//***************************************************************************

UBOOL _MyDeleteDC
    (HDC  hMEMDC,               // Handle to memory device context
     UINT uLine)                // Line #

{
    UBOOL bRet;

    bRet = DeleteDC (hMEMDC);

    if (!bRet)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _DeleObj (OBJ_MEMDC, hMEMDC);

    return bRet;
} // _MyDeleteDC


//***************************************************************************
//  $MyDeleteObject
//
//  Delete an object
//***************************************************************************

UBOOL _MyDeleteObject
    (HGDIOBJ hObject,       // Handle to graphic object
     UINT    uLine)         // Line #

{
    UBOOL bRet;
    DWORD dwType;

    // See what kind of object we're deleting
    dwType = GetObjectType (hObject);

    // Validate the object type
    if (!(OBJ_PEN <= dwType && dwType <= OBJ_COLORSPACE))
        DbgBrk ();

    // Delete the object
    bRet = DeleteObject (hObject);

    // If it failed, ...
    if (!bRet)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _DeleObj (dwType, hObject);

    return bRet;
} // _MyDeleteObject


//***************************************************************************
//  $MyGetDC
//
//  Get a DC
//***************************************************************************

HDC _MyGetDC
    (HWND hWnd,         // Handle of window
     UINT uLine)        // Line #

{
    HDC hDC;

    hDC = GetDC (hWnd);

    if (!hDC)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_DC, hDC, uLine);

    return hDC;
} // _MyGetDC


//***************************************************************************
//  $MyGetWindowDC
//
//  Get a window DC
//***************************************************************************

HDC _MyGetWindowDC
    (HWND hWnd,             // Handle of window
     UINT uLine)            // Line #

{
    HDC hDC;

    hDC = GetWindowDC (hWnd);

    if (!hDC)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_DC, hDC, uLine);

    return hDC;
} // _MyGetWindowDC


//***************************************************************************
//  $MyLoadBitmap
//
//  Load a bitmap
//***************************************************************************

HBITMAP _MyLoadBitmap
    (HINSTANCE hInstance,       // Handle of application instance
     LPCTSTR   lpBitmapName,    // Address of bitmap resource name
     UINT      uLine)           // Line #

{
    HBITMAP hBitmap;

    hBitmap = LoadBitmap (hInstance, lpBitmapName);

    if (!hBitmap)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_BITMAP, hBitmap, uLine);

    return hBitmap;
} // _MyLoadBitmap


//***************************************************************************
//  $MyLoadImage
//
//  Load an image
//***************************************************************************

HANDLE _MyLoadImage
    (HINSTANCE hinst,       // Handle of the instance that contains the image
     LPCTSTR   lpszName,    // Name or identifier of image
     UINT      uType,       // Type of image
     int       cxDesired,   // Desired width
     int       cyDesired,   // Desired height
     UINT      fuLoad,      // Load flags
     UINT      uLine)       // Line #

{
    HBITMAP hBitmap;

    hBitmap = LoadImage (hinst, lpszName, uType, cxDesired, cyDesired, fuLoad);

    if (!hBitmap)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_BITMAP, hBitmap, uLine);

    return hBitmap;
} // _MyLoadImage


//***************************************************************************
//  $MyReleaseDC
//
//  Release a DC
//***************************************************************************

UBOOL _MyReleaseDC
    (HWND hWnd,                 // Handle of window
     HDC  hDC,                  // Handle of device context
     UINT uLine)                // Line #

{
    UBOOL bRet;

    bRet = ReleaseDC (hWnd, hDC);

    if (!bRet)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _DeleObj (OBJ_DC, hDC);

    return bRet;
} // _MyReleaseDC


//***************************************************************************
//  $MyGlobalAlloc
//
//  Allocate a global variable
//***************************************************************************

LPVOID _MyGlobalAlloc
    (UINT   uFlags,         // Object allocation attributes
     SIZE_T dwBytes,        // Number of bytes to allocate
     char  *lpFileName,     // Ptr to filename
     UINT   uLine)          // Line #

{
    LPVOID lpVoid;
    int    iCount;

    CheckMemStat ();

    lpVoid = GlobalAlloc (uFlags, dwBytes);

    if (!lpVoid)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        CheckMemStat ();
    } else
    {
        iCount = _SaveObj (OBJ_GLBALLOC, lpVoid, uLine);
        lpaaFileName[OBJ_GLBALLOC - 1][iCount] = lpFileName;
    } // End IF/ELSE

    return lpVoid;
} // End _MyGlobalAlloc


//***************************************************************************
//  $MyGlobalLock
//
//  Lock a global variable
//***************************************************************************

LPVOID _MyGlobalLock
    (HGLOBAL hMem,          // Address of the global memory object
     char   *lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    return _MyGlobalLockSub (hMem, TRUE, lpFileName, uLine);
} // End _MyGlobalLock


//***************************************************************************
//  $MyGlobalLockNS
//
//  Lock a global variable, no save filename
//***************************************************************************

LPVOID _MyGlobalLockNS
    (HGLOBAL hMem,          // Address of the global memory object
     char   *lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    return _MyGlobalLockSub (hMem, FALSE, lpFileName, uLine);
} // End _MyGlobalLockNS


//***************************************************************************
//  $MyGlobalLockSub
//
//  Lock a global variable subroutine
//***************************************************************************

LPVOID _MyGlobalLockSub
    (HGLOBAL hMem,          // Address of the global memory object
     UBOOL   bSaveFileName, // TRUE iff we should save the filename
     char   *lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    LPVOID lpVoid;
    int    iCount;

    CheckMemStat ();

    lpVoid = GlobalLock (hMem);

    if (!lpVoid)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
    {
        iCount = _SaveObj (OBJ_GLBLOCK, hMem, uLine);
        if (bSaveFileName)
            lpaaFileName[OBJ_GLBLOCK - 1][iCount] = lpFileName;
    } // End IF/ELSE

    return lpVoid;
} // End _MyGlobalLockSub


//***************************************************************************
//  $MyGlobalUnlock
//
//  Unlock a global variable
//***************************************************************************

UBOOL _MyGlobalUnlock
    (HGLOBAL hMem,      // Address of the global memory object
     UINT    uLine)     // Line #

{
    UBOOL bRet;

    CheckMemStat ();

    // Ensure the lock count is non-zero
    if ((_MyGlobalFlags (hMem, uLine) & GMEM_LOCKCOUNT) EQ 0)
    {
        char szTemp[1024];

        DbgBrk ();

        // Format a message about the last lock
        _LastLock (szTemp, hMem);
    } // End IF

    bRet = GlobalUnlock (hMem);
    _DeleObj (OBJ_GLBLOCK, hMem);

    return bRet;
} // End _MyGlobalUnlock


//***************************************************************************
//  $MyGlobalSize
//
//  Get the size of a global variable
//***************************************************************************

SIZE_T _MyGlobalSize
    (HGLOBAL hMem,      // Address of the global memory object
     UINT    uLine)     // Line #

{
    SIZE_T dwRet;

    CheckMemStat ();

    dwRet = GlobalSize (hMem);
    if (dwRet EQ 0)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } // End IF

    return dwRet;
} // End _MyGlobalSize


//***************************************************************************
//  $MyGlobalFlags
//
//  Get the flags of a global variable
//***************************************************************************

DWORD _MyGlobalFlags
    (HGLOBAL hMem,      // Address of the global memory object
     UINT    uLine)     // Line #

{
    DWORD dwRet;

    CheckMemStat ();

    dwRet = GlobalFlags (hMem);
    if (dwRet EQ GMEM_INVALID_HANDLE)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } // End IF

    return dwRet;
} // End _MyGlobalFlags


//***************************************************************************
//  $MyGlobalReAlloc
//
//  Resize a global variable
//***************************************************************************

HGLOBAL _MyGlobalReAlloc
    (HGLOBAL hMem,      // Address of the global memory object
     SIZE_T  dwBytes,   // New size of block
     UINT    uFlags,    // How to reallocate
     UINT    uLine)     // Line #

{
    SIZE_T  dwRet;
    HGLOBAL hGlb;

    CheckMemStat ();

    dwRet = GlobalSize (hMem);
    if (dwRet EQ 0)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } // End IF

    if ((_MyGlobalFlags (hMem, uLine) & GMEM_LOCKCOUNT) NE 0)
    {
        char szTemp[1024];

        DbgBrk ();

        // Format a message about the last lock
        _LastLock (szTemp, hMem);
    } // End IF

    hGlb = GlobalReAlloc (hMem, dwBytes, uFlags);
    if (!hGlb)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } // End IF

    return hGlb;
} // End _MyGlobalReAlloc


//***************************************************************************
//  $MyGlobalFree
//
//  My GlobalFree function which checks for unlocked memory
//***************************************************************************

HGLOBAL _MyGlobalFree
    (HGLOBAL hMem,              // Handle
     UINT    uLine)             // Line #

{
    CheckMemStat ();

    // GlobalFlags returns the lock count in the low-order byte
    if (_MyGlobalFlags (hMem, uLine) & GMEM_LOCKCOUNT)
    {
        char szTemp[1024];

        DbgBrk ();

        // Format a message about the last lock
        _LastLock (szTemp, hMem);
    } else
        _DeleObj (OBJ_GLBALLOC, hMem);

    return GlobalFree (hMem);
} // _MyGlobalFree


//***************************************************************************
//  $MyVirtualAlloc
//
//  My VirtualAlloc function which checks for NULL results
//***************************************************************************

LPVOID _MyVirtualAlloc
    (LPVOID lpAddress,          // Address of region to reserve or commit
     DWORD  dwSize,             // Size of region
     DWORD  flAllocationType,   // Type of allocation
     DWORD  flProtect,          // Type of access protection
     UINT   uLine)              // Line #

{
    LPVOID lpRet;               // Ptr to result

    // Call the original function
    lpRet =
      VirtualAlloc (lpAddress,          // Address of region to reserve or commit
                    dwSize,             // Size of region
                    flAllocationType,   // Type of allocation
                    flProtect);         // Type of access protection
    if (lpRet EQ NULL)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } // End IF

    return lpRet;
} // End _MyVirtualAlloc


//***************************************************************************
//  $MyVirtualFree
//
//  My VirtualFree function which checks for FALSE results
//***************************************************************************

UBOOL _MyVirtualFree
    (LPVOID lpAddress,          // Address of region of committed pages
     DWORD  dwSize,             // Size of region
     DWORD  dwFreeType,         // Type of free operation
     UINT   uLine)              // Line #

{
    UBOOL bRet;                 // TRUE iff the result is valid

    // Call the original function
    bRet =
      VirtualFree (lpAddress,   // Address of region of committed pages
                   dwSize,      // Size of region
                   dwFreeType); // Type of free operation
    if (!bRet)
    {
        char szTemp[1024];

        DbgBrk ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } // End IF

    return bRet;
} // End _MyVirtualFree


//***************************************************************************
//  $MyQueryObject
//
//  Check to see how many of a given OBJECT we have outstanding
//***************************************************************************

HANDLE _MyQueryObject
    (DWORD dwType,
     int   iCount,
     UINT  uLine)           // Line #

{
    int *lpiCount;

    // Get a ptr to the counter
    lpiCount = lpiaCount[dwType - 1];

    if (iCount NE *lpiCount)
        DbgBrk ();

    return &lpaah[dwType - 1][(*lpiCount) - 1];
} // End _MyQueryObject


#ifdef DEBUG
//***************************************************************************
//  $_CheckMemStat
//
//  Check on memory status
//***************************************************************************

void _CheckMemStat
    (void)

{
////MEMORYSTATUS memStat;
////
////memStat.dwLength = sizeof (memStat);
////
////// Check the status
////GlobalMemoryStatus (&memStat);
////
////if (memStat.dwMemoryLoad EQ 100)
////    DbgBrk ();
////
    if (!HeapValidate (GetProcessHeap (), 0, NULL))
        DbgBrk ();
} // End _CheckMemStat
#endif
#endif


//***************************************************************************
//  End of File: resdebug.c
//***************************************************************************
