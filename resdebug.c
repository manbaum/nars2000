//***************************************************************************
//  NARS2000 -- My Routines For Debugging
//***************************************************************************

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

#define OBJ_GLOBAL  15


// ************** DEBUGGING DATA ********************************************

UINT
    auLinNumPEN        [MAXOBJ],            //  1
    auLinNumBRUSH      [MAXOBJ],            //  2
    auLinNumDC         [MAXOBJ],            //  3
    auLinNumMETADC     [MAXOBJ],            //  4
    auLinNumPAL        [MAXOBJ],            //  5
    auLinNumFONT       [MAXOBJ],            //  6
    auLinNumBITMAP     [MAXOBJ],            //  7
    auLinNumREGION     [MAXOBJ],            //  8
    auLinNumMETAFILE   [MAXOBJ],            //  9
    auLinNumMEMDC      [MAXOBJ],            // 10
    auLinNumEXTPEN     [MAXOBJ],            // 11
    auLinNumENHMETADC  [MAXOBJ],            // 12
    auLinNumENHMETAFILE[MAXOBJ],            // 13
    auLinNumCOLORSPACE [MAXOBJ],            // 14
    auLinNumGLOBAL     [MAXOBJ];            // 15

UINT *lpaauLinNum[] = {&auLinNumPEN        [0],
                       &auLinNumBRUSH      [0],
                       &auLinNumDC         [0],
                       &auLinNumMETADC     [0],
                       &auLinNumPAL        [0],
                       &auLinNumFONT       [0],
                       &auLinNumBITMAP     [0],
                       &auLinNumREGION     [0],
                       &auLinNumMETAFILE   [0],
                       &auLinNumMEMDC      [0],
                       &auLinNumEXTPEN     [0],
                       &auLinNumENHMETADC  [0],
                       &auLinNumENHMETAFILE[0],
                       &auLinNumCOLORSPACE [0],
                       &auLinNumGLOBAL     [0]};

int iCountPEN         = 0,                  //  1
    iCountBRUSH       = 0,                  //  2
    iCountDC          = 0,                  //  3
    iCountMETADC      = 0,                  //  4
    iCountPAL         = 0,                  //  5
    iCountFONT        = 0,                  //  6
    iCountBITMAP      = 0,                  //  7
    iCountREGION      = 0,                  //  8
    iCountMETAFILE    = 0,                  //  9
    iCountMEMDC       = 0,                  // 10
    iCountEXTPEN      = 0,                  // 11
    iCountENHMETADC   = 0,                  // 12
    iCountENHMETAFILE = 0,                  // 13
    iCountCOLORSPACE  = 0,                  // 14
    iCountGLOBAL      = 0;                  // 15

int *lpiaCount[] = {&iCountPEN        ,     //  1
                    &iCountBRUSH      ,     //  2
                    &iCountDC         ,     //  3
                    &iCountMETADC     ,     //  4
                    &iCountPAL        ,     //  5
                    &iCountFONT       ,     //  6
                    &iCountBITMAP     ,     //  7
                    &iCountREGION     ,     //  8
                    &iCountMETAFILE   ,     //  9
                    &iCountMEMDC      ,     // 10
                    &iCountEXTPEN     ,     // 11
                    &iCountENHMETADC  ,     // 12
                    &iCountENHMETAFILE,     // 13
                    &iCountCOLORSPACE ,     // 14
                    &iCountGLOBAL     };    // 15

HANDLE ahPEN         [MAXOBJ],              //  1
       ahBRUSH       [MAXOBJ],              //  2
       ahDC          [MAXOBJ],              //  3
       ahMETADC      [MAXOBJ],              //  4
       ahPAL         [MAXOBJ],              //  5
       ahFONT        [MAXOBJ],              //  6
       ahBITMAP      [MAXOBJ],              //  7
       ahREGION      [MAXOBJ],              //  8
       ahMETAFILE    [MAXOBJ],              //  9
       ahMEMDC       [MAXOBJ],              // 10
       ahEXTPEN      [MAXOBJ],              // 11
       ahENHMETADC   [MAXOBJ],              // 12
       ahENHMETAFILE [MAXOBJ],              // 13
       ahCOLORSPACE  [MAXOBJ],              // 14
       ahGLOBAL      [MAXOBJ];              // 15

HANDLE *lpaah[] = {&ahPEN        [0],
                   &ahBRUSH      [0],
                   &ahDC         [0],
                   &ahMETADC     [0],
                   &ahPAL        [0],
                   &ahFONT       [0],
                   &ahBITMAP     [0],
                   &ahREGION     [0],
                   &ahMETAFILE   [0],
                   &ahMEMDC      [0],
                   &ahEXTPEN     [0],
                   &ahENHMETADC  [0],
                   &ahENHMETAFILE[0],
                   &ahCOLORSPACE [0],
                   &ahGLOBAL     [0]};



//***************************************************************************
//  $MyDbgBrk
//***************************************************************************

void MyDbgBrk
    (LPCHAR szTemp)

{
    DbgBrk ();
} // End MyDbgBrk


//***************************************************************************
//  $SaveObj
//
//  Save the allocation of an object
//***************************************************************************

void _SaveObj
    (DWORD   dwType,
     HGDIOBJ hObject,
     UINT    uLine)

{
    int *lpiCount;

    // Get a ptr to the counter
    lpiCount = lpiaCount[dwType - 1];

    if (*lpiCount >= MAXOBJ)
        return;

    // Save the line number
    lpaauLinNum[dwType - 1][*lpiCount] = uLine;

    // Count in another OBJECT
    lpaah[dwType - 1][*lpiCount] = hObject;

    // Increment the counter
    (*lpiCount)++;
} // _SaveObj


//***************************************************************************
//  $DeleObj
//
//  Delete the allocation of an object
//***************************************************************************

void _DeleObj
    (DWORD   dwType,
     HGDIOBJ hObject,
     UINT    uLine)

{
    int i, iLen;
    int *lpiCount;

    // Get a ptr to the counter
    lpiCount = lpiaCount[dwType - 1];

    // Find this object in the array
    iLen = *lpiCount;
    for (i = 0; i < iLen; i++)
    if (hObject EQ lpaah[dwType - 1][i])
        break;

    // If we didn't find it, ...
    if (i EQ iLen)
    {
        if (*lpiCount < MAXOBJ)
            MyDbgBrk ("DeleObj 2");
    } else
    {
        // Move down the saved values above this entry
        MoveMemory (&lpaauLinNum[dwType - 1][i],
                    &lpaauLinNum[dwType - 1][i + 1],
                    (iLen - i) * sizeof (lpaauLinNum[0][0]));
        MoveMemory (&lpaah[dwType - 1][i],
                    &lpaah[dwType - 1][i + 1],
                    (iLen - i) * sizeof (lpaah[0][0]));
    } // End IF/ELSE

    if (*lpiCount < MAXOBJ)
    {
        // Count out another OBJECT
        if (*lpiCount)
            (*lpiaCount[dwType - 1])--;
        else
            MyDbgBrk ("DeleObj 1");
    } // End IF
} // _DeleObj


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
        MyDbgBrk ("MyCreateCompatibleBitmap");
        GetLastError ();
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
        MyDbgBrk ("MyCreateCompatibleDC");
        GetLastError ();
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
        MyDbgBrk ("MyCreatePen");
        GetLastError ();
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
        MyDbgBrk ("MyCreateFontIndirect");
        GetLastError ();
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
        MyDbgBrk ("MyCreatePolygonRgn");
        GetLastError ();
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
        MyDbgBrk ("MyCreateRgnIndirect");
        GetLastError ();
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
        MyDbgBrk ("MyCreateSolidBrush");
        GetLastError ();
    } else
        _SaveObj (OBJ_BRUSH, hBrush, uLine);

    return hBrush;
} // _MyCreateSolidBrush


//***************************************************************************
//  $MyDeleteDC
//
//  Delete a memory DC
//***************************************************************************

BOOL _MyDeleteDC
    (HDC  hMEMDC,               // Handle to memory device context
     UINT uLine)                // Line #

{
    BOOL bRet;

    bRet = DeleteDC (hMEMDC);

    if (!bRet)
    {
        MyDbgBrk ("MyDeleteDC");
        GetLastError ();
    } else
        _DeleObj (OBJ_MEMDC, hMEMDC, uLine);

    return bRet;
} // _MyDeleteDC


//***************************************************************************
//  $MyDeleteObject
//
//  Delete an object
//***************************************************************************

BOOL _MyDeleteObject
    (HGDIOBJ hObject,       // Handle to graphic object
     UINT    uLine)         // Line #

{
    BOOL bRet;
    DWORD dwType;

    // See what kind of object we're deleting
    dwType = GetObjectType (hObject);

    // Validate the object type
    if (!(OBJ_PEN <= dwType && dwType <= OBJ_COLORSPACE))
        MyDbgBrk ("MyDeleteObject 1");

    // Delete the object
    bRet = DeleteObject (hObject);

    // If it failed, ...
    if (!bRet)
    {
        MyDbgBrk ("MyDeleteObject 2");
        GetLastError ();
    } else
        _DeleObj (dwType, hObject, uLine);

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
        MyDbgBrk ("MyGetDC");
        GetLastError ();
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
        MyDbgBrk ("MyGetWindowDC");
        GetLastError ();
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
        MyDbgBrk ("MyLoadBitmap");
        GetLastError ();
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
        MyDbgBrk ("MyLoadImage");
        GetLastError ();
    } else
        _SaveObj (OBJ_BITMAP, hBitmap, uLine);

    return hBitmap;
} // _MyLoadImage


//***************************************************************************
//  $MyReleaseDC
//
//  Release a DC
//***************************************************************************

BOOL _MyReleaseDC
    (HWND hWnd,                 // Handle of window
     HDC  hDC,                  // Handle of device context
     UINT uLine)                // Line #

{
    BOOL bRet;

    bRet = ReleaseDC (hWnd, hDC);

    if (!bRet)
    {
        MyDbgBrk ("MyReleaseDC");
        GetLastError ();
    } else
        _DeleObj (OBJ_DC, hDC, uLine);

    return bRet;
} // _MyReleaseDC


//***************************************************************************
//  $MyGlobalAlloc
//
//  Allocate a global variable
//***************************************************************************

LPVOID _MyGlobalAlloc
    (UINT  uFlags,  // object allocation attributes
     DWORD dwBytes, // number of bytes to allocate
     UINT  uLine)

{
    LPVOID lpVoid;

    lpVoid = GlobalAlloc (uFlags, dwBytes);

    if (!lpVoid)
    {
        MyDbgBrk ("MyGlobalAlloc");
        GetLastError ();
        CheckMemStat ();
    } else
        _SaveObj (OBJ_GLOBAL, lpVoid, uLine);

    return lpVoid;
} // End _MyGlobalAlloc


//***************************************************************************
//  $MyGlobalLock
//
//  Lock a global variable
//***************************************************************************

LPVOID _MyGlobalLock
    (HGLOBAL hMem,      // Address of the global memory object
     UINT    uLine)

{
    LPVOID lpVoid;

    lpVoid = GlobalLock (hMem);

    if (!lpVoid)
    {
        char szTemp[1024];

        MyDbgBrk ("MyGlobalLock");
        GetLastError ();
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
    } else
        _SaveObj (OBJ_GLOBAL, hMem, uLine);

    return lpVoid;
} // End _MyGlobalLock


//***************************************************************************
//  $MyGlobalUnlock
//
//  Unlock a global variable
//***************************************************************************

BOOL _MyGlobalUnlock
    (HGLOBAL hMem,      // Address of the global memory object
     UINT    uLine)

{
    BOOL bRet;

    // Ensure the lock count is non-zero
    if ((_MyGlobalFlags (hMem, uLine) & GMEM_LOCKCOUNT) EQ 0)
        MyDbgBrk ("MyGlobalUnlock");

    bRet = GlobalUnlock (hMem);
    _DeleObj (OBJ_GLOBAL, hMem, uLine);

    return bRet;
} // End _MyGlobalUnlock


//***************************************************************************
//  $MyGlobalSize
//
//  Get the size of a global variable
//***************************************************************************

DWORD _MyGlobalSize
    (HGLOBAL hMem,      // Address of the global memory object
     UINT    uLine)

{
    DWORD dwRet;

    dwRet = GlobalSize (hMem);
    if (dwRet EQ 0)
    {
        MyDbgBrk ("MyGlobalSize");
        GetLastError ();
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
     UINT    uLine)

{
    DWORD dwRet;

    dwRet = GlobalFlags (hMem);
    if (dwRet EQ GMEM_INVALID_HANDLE)
    {
        MyDbgBrk ("MyGlobalFlags");
        GetLastError ();
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
     DWORD   dwBytes,   // New size of block
     UINT    uFlags,    // How to reallocate
     UINT    uLine)

{
    DWORD dwRet;
    HGLOBAL hGlb;

    dwRet = GlobalSize (hMem);
    if (dwRet EQ 0)
    {
        MyDbgBrk ("MyGlobalReAlloc #1");
        GetLastError ();
    } // End IF

    if ((_MyGlobalFlags (hMem, uLine) & GMEM_LOCKCOUNT) NE 0)
        MyDbgBrk ("MyGlobalReAlloc #2");

    hGlb = GlobalReAlloc (hMem, dwBytes, uFlags);
    if (!hGlb)
    {
        MyDbgBrk ("MyGlobalReAlloc #3");
        GetLastError ();
    } // End IF

    return hGlb;
} // End _MyGlobalReAlloc


//***************************************************************************
//  $MyGlobalFree
//
//  My GlobalFree function which checks for unlocked memory
//***************************************************************************

HGLOBAL _MyGlobalFree
    (HGLOBAL hGlb,              // Handle
     UINT    uLine)             // Line #

{
    // GlobalFlags returns the lock count in the low-order byte
    if (_MyGlobalFlags (hGlb, uLine) & GMEM_LOCKCOUNT)
        MyDbgBrk ("MyGlobalFree"); // We should never get here
    else
        _DeleObj (OBJ_GLOBAL, hGlb, uLine);

    return GlobalFree (hGlb);
} // MyGlobalFree


//***************************************************************************
//  $MyQueryObject
//
//  Check to see how many of a given OBJECT we have outstanding
//***************************************************************************

HANDLE _MyQueryObject
    (DWORD dwType,
     int   iCount,
     UINT  uLine)

{
    int *lpiCount;

    // Get a ptr to the counter
    lpiCount = lpiaCount[dwType - 1];

    if (iCount NE *lpiCount)
        MyDbgBrk ("MyQueryObject");

    return &lpaah[dwType - 1][(*lpiCount) - 1];
} // End MyQueryObject


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


//***************************************************************************
//  End of File: resdebug.c
//***************************************************************************
