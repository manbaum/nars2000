//***************************************************************************
//  NARS2000 -- Save Function Routines
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
#include <stdio.h>

#include "aplerrors.h"
#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "dfnhdr.h"
#include "pertab.h"
#include "sis.h"
#include "savefcn.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SaveFunction
//
//  Attempt to save a function to the WS
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SaveFunction"
#else
#define APPEND_NAME
#endif

BOOL SaveFunction
    (HWND hWndFE)                   // Function Editor window handle

{
    SF_FCNS SF_Fcns = {0};

    // Fill in common values
    SF_Fcns.SF_LineLen      = SF_LineLenFE;     // Ptr to line length function
    SF_Fcns.SF_ReadLine     = SF_ReadLineFE;    // Ptr to read line function
    SF_Fcns.SF_NumLines     = SF_NumLinesFE;    // Ptr to get # lines function
    SF_Fcns.SF_CreationTime = SF_CreationTimeCom;// Ptr to get function creation time
    SF_Fcns.SF_LastModTime  = SF_LastModTimeCom;// Ptr to get function creation time
    SF_Fcns.SF_UndoBuffer   = SF_UndoBufferFE;  // Ptr to get function Undo Buffer global memory handle
////SF_Fcns.LclParams       = NULL;             // Already zero from = {0}

    // Call common routine
    return SaveFunctionCom (hWndFE,             // Function Edit window handle (not-[]FX only)
                           &SF_Fcns);           // Ptr to common values
} // End SaveFunction
#undef  APPEND_NAME


//***************************************************************************
//  $SF_LineLenFE
//
//  Return the length of a function text header/line
//    when called from function editor
//***************************************************************************

UINT SF_LineLenFE
    (HWND      hWndEC,              // Edit Control window handle (FE only)
     LPVOID    lpVoid,              // Ptr to common struc
     UINT      uLineNum)            // Function line #

{
    UINT uLinePos;

    // Get the char pos at the start of this line
    uLinePos = (UINT) SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    return (UINT) SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);
} // End SF_LineLenFE


//***************************************************************************
//  $SF_LineLenLW
//
//  Return the length of a function text header/line
//    when called from LoadWorkspace
//***************************************************************************

UINT SF_LineLenLW
    (HWND        hWndEC,            // Edit Control window handle (FE only)
     LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     UINT        uLineNum)          // Function line #

{
    // The caller requests the line length before it reads
    //   the line so it can allocate memory.

    // This means we must read the line first so we can get its length
    SF_ReadLineLW (hWndEC,                  // Edit Control window handle (FE only)
                   lpLW_Params,             // Ptr to common struc
                   uLineNum,                // Function line #
                   lpLW_Params->lpwBuffer); // Ptr to header/line global memory
    // Return the length
    return lstrlenW (lpLW_Params->lpwBuffer);
} // End SF_LineLenLW


//***************************************************************************
//  $SF_LineLenM
//
//  Return the length of a function text header/line
//    when called from []FX with a simple char matrix arg
//***************************************************************************

UINT SF_LineLenM
    (HWND        hWndEC,            // Edit Control window handle (FE only)
     LPFX_PARAMS lpFX_Params,       // Ptr to common struc
     UINT        uLineNum)          // Function line #

{
    LPAPLCHAR lpMemRht;             // Ptr to right arg global memory
    UINT      uRowOff,              // Offset to start of row
              uLineLen;             // Line length

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (lpFX_Params->hGlbRht);

    // Get the # cols in the matrix
    uLineLen = (UINT) lpFX_Params->aplColsRht;

    // Calc the offset to the appropriate row
    uRowOff = uLineNum * uLineLen;

    // Skip over the header to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, lpFX_Params->aplRankRht);

    // As this is a matrix and the header/function line might have
    //   been padded out beyond its normal length, delete trailing blanks
    for (; uLineLen; uLineLen--)
    if (lpMemRht[uRowOff + uLineLen - 1] NE L' ')
        break;

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemRht = NULL;

    return uLineLen;
} // End SF_LineLenM


//***************************************************************************
//  $SF_LineLenN
//
//  Return the length of a function text header/line
//    when called from []FX with a nested arg
//***************************************************************************

UINT SF_LineLenN
    (HWND        hWndEC,            // Edit Control window handle (FE only)
     LPFX_PARAMS lpFX_Params,       // Ptr to common struc
     UINT        uLineNum)          // Function line #

{
    LPAPLNESTED lpMemRht;           // Ptr to right arg global memory
    HGLOBAL     hGlbItmRht;         // Right arg item global memory handle
    APLNELM     aplNELMItmRht;      // Right arg item NELM

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, lpFX_Params->aplRankRht);

    // Split cases based upon the right arg item ptr type
    switch (GetPtrTypeDir (lpMemRht[uLineNum]))
    {
        case PTRTYPE_STCONST:
            aplNELMItmRht = 1;      // Scalar is of length 1

            break;

        case PTRTYPE_HGLOBAL:
            // Get the global memory handle w/o ptr bits
            hGlbItmRht = ClrPtrTypeDirAsGlb (lpMemRht[uLineNum]);

            // Get the right arg item global attrs
            AttrsOfGlb (hGlbItmRht, NULL, &aplNELMItmRht, NULL, NULL);

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemRht = NULL;

    return (UINT) aplNELMItmRht;
} // End SF_LineLenN


//***************************************************************************
//  $SF_LineLenSV
//
//  Return the length of a function text header/line
//    when called from []FX with a simple char scalar or vector arg
//***************************************************************************

UINT SF_LineLenSV
    (HWND      hWndEC,              // Edit Control window handle (FE only)
     LPVOID    lpVoid,              // Ptr to common struc
     UINT      uLineNum)            // Function line #

{
    // The function line is a single char
    return 1;
} // End SF_LineLenSV


//***************************************************************************
//  $SF_LineLenTF1
//
//  Return the length of a function text header/line
//    when called from []TF with a simple char matrix arg
//***************************************************************************

UINT SF_LineLenTF1
    (HWND         hWndEC,           // Edit Control window handle (FE only)
     LPTF1_PARAMS lpTF1_Params,     // Ptr to common struc
     UINT         uLineNum)         // Function line #

{
    return (UINT) lpTF1_Params->aplColsRht;
} // End SF_LineLenTF1


//***************************************************************************
//  $SF_ReadLineFE
//
//  Read in a header/function line
//    when called from function editor
//***************************************************************************

void SF_ReadLineFE
    (HWND      hWndEC,              // Edit Control window handle (FE only)
     LPVOID    lpVoid,              // Ptr to common struc
     UINT      uLineNum,            // Function line #
     LPAPLCHAR lpMemLine)           // Ptr to header/line global memory

{
    SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpMemLine);
} // End SF_ReadLineFE


//***************************************************************************
//  $SF_ReadLineLW
//
//  Read in a header/function line
//    when called from LoadWorkspace
//***************************************************************************

void SF_ReadLineLW
    (HWND        hWndEC,            // Edit Control window handle (FE only)
     LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     UINT        uLineNum,          // Function line #
     LPAPLCHAR   lpMemLine)         // Ptr to header/line global memory

{
    WCHAR wszLineNum[8];            // Save area for formatted line #

    // Format the line #
    wsprintfW (wszLineNum, L"%d", uLineNum);

    // Get the one (and only) line
    GetPrivateProfileStringW (lpLW_Params->lpwSectName,             // Ptr to the section name
                              wszLineNum,                           // Ptr to the key name
                              L"",                                  // Ptr to the default value
                              lpLW_Params->lpwBuffer,               // Ptr to the output buffer
                              lpLW_Params->uMaxSize,                // Byte size of the output buffer
                              lpLW_Params->lpwszDPFE);              // Ptr to the file name
    // Convert the {name}s and other chars to UTF16_xxx
    (void) ConvertNameInPlace (lpLW_Params->lpwBuffer);

    // Copy the buffer to the caller's save area
    CopyMemory (lpMemLine,
                lpLW_Params->lpwBuffer,
                lstrlenW (lpLW_Params->lpwBuffer) * sizeof (lpLW_Params->lpwBuffer[0]));
} // End SF_ReadLineLW


//***************************************************************************
//  $SF_ReadLineM
//
//  Read in a header/function line
//    when called from []FX with a simple char matrix arg
//***************************************************************************

void SF_ReadLineM
    (HWND        hWndEC,            // Edit Control window handle (FE only)
     LPFX_PARAMS lpFX_Params,       // Ptr to common struc
     UINT        uLineNum,          // Function line #
     LPAPLCHAR   lpMemLine)         // Ptr to header/line global memory

{
    LPAPLCHAR lpMemRht;             // Ptr to right arg global memory
    UINT      uRowOff,              // Offset to start of row
              uLineLen;             // Line length

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, lpFX_Params->aplRankRht);

    // Get the # cols in the matrix
    uLineLen = (UINT) lpFX_Params->aplColsRht;

    // Calc the offset to the appropriate row
    uRowOff = uLineNum * uLineLen;

    // As this is a matrix and the header/function line might have
    //   been padded out beyond its normal length, delete trailing blanks
    for (; uLineLen; uLineLen--)
    if (lpMemRht[uRowOff + uLineLen - 1] NE L' ')
        break;

    // Copy the line to the result
    for (; uLineLen; uLineLen--, uRowOff++)
        *lpMemLine++ = lpMemRht[uRowOff];

    // Ensure properly terminated
    *lpMemLine++ = L'\0';

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemRht = NULL;
} // End SF_ReadLineM


//***************************************************************************
//  $SF_ReadLineN
//
//  Read in a header/function line
//    when called from []FX with a nested arg
//***************************************************************************

void SF_ReadLineN
    (HWND        hWndEC,            // Edit Control window handle (FE only)
     LPFX_PARAMS lpFX_Params,       // Ptr to common struc
     UINT        uLineNum,          // Function line #
     LPAPLCHAR   lpMemLine)         // Ptr to header/line global memory

{
    LPAPLNESTED lpMemRht;           // Ptr to right arg global memory
    LPAPLCHAR   lpMemItmRht;        // Ptr to right arg item global memory
    HGLOBAL     hGlbItmRht;         // Right arg item global memory handle
    UINT        uLineLen;           // Line length

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (lpFX_Params->hGlbRht);

    // Skip over the header to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, lpFX_Params->aplRankRht);

    // Split cases based upon the right arg item ptr type
    switch (GetPtrTypeDir (lpMemRht[uLineNum]))
    {
        case PTRTYPE_STCONST:
            // Copy the single char to the result
            *lpMemLine++ = ((LPSYMENTRY*) lpMemRht)[uLineNum]->stData.stChar;
            *lpMemLine++ = L'\0';

            break;

        case PTRTYPE_HGLOBAL:
            // Get the global memory handle w/o ptr bits
            hGlbItmRht = ClrPtrTypeDirAsGlb (lpMemRht[uLineNum]);

            // Lock the memory to get a ptr to it
            lpMemItmRht = MyGlobalLock (hGlbItmRht);

            // Get the NELM (line length)
#define lpHeader        ((LPVARARRAY_HEADER) lpMemItmRht)
            uLineLen = (UINT) lpHeader->NELM;

            // Skip over the header to the data
            lpMemItmRht = VarArrayBaseToData (lpMemItmRht, lpHeader->Rank);
#undef  lpHeader

            // Copy the data to the result
            CopyMemory (lpMemLine, lpMemItmRht, uLineLen * sizeof (APLCHAR));

            // We no longer need this ptr
            MyGlobalUnlock (hGlbItmRht); lpMemItmRht = NULL;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemRht = NULL;
} // End SF_ReadLineN


//***************************************************************************
//  $SF_ReadLineSV
//
//  Read in a header/function line
//    when called from []FX with a simple char scalar or vector arg
//***************************************************************************

void SF_ReadLineSV
    (HWND        hWndEC,            // Edit Control window handle (FE only)
     LPFX_PARAMS lpFX_Params,       // Ptr to common struc
     UINT        uLineNum,          // Function line #
     LPAPLCHAR   lpMemLine)         // Ptr to header/line global memory

{
    LPAPLCHAR lpMemRht;             // Ptr to right arg global memory

    // Check for immediate right arg
    if (uLineNum EQ 0 && lpFX_Params->hGlbRht EQ NULL)
    {
        // Copy the simple char to the result
        *lpMemLine++ = (APLCHAR) lpFX_Params->aplLongestRht;
        *lpMemLine++ = L'\0';
    } else
    {
        // Lock the memory to get a ptr to it
        lpMemRht = MyGlobalLock (lpFX_Params->hGlbRht);

        // Skip over the header to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, lpFX_Params->aplRankRht);

        // Copy the simple char to the result
        *lpMemLine++ = lpMemRht[uLineNum];
        *lpMemLine++ = L'\0';

        // We no longer need this ptr
        MyGlobalUnlock (lpFX_Params->hGlbRht); lpMemRht = NULL;
    } // End IF/ELSE
} // End SF_ReadLineSV


//***************************************************************************
//  $SF_ReadLineTF1
//
//  Read in a header/function line
//    when called from []TF with a simple char matrix arg
//***************************************************************************

void SF_ReadLineTF1
    (HWND         hWndEC,           // Edit Control window handle (FE only)
     LPTF1_PARAMS lpTF1_Params,     // Ptr to common struc
     UINT         uLineNum,         // Function line #
     LPAPLCHAR    lpMemLine)        // Ptr to header/line global memory

{
    CopyMemory (lpMemLine,
                lpTF1_Params->lpMemRht + uLineNum * lpTF1_Params->aplColsRht,
                (__int3264) lpTF1_Params->aplColsRht * sizeof (APLCHAR));
} // End SF_ReadLineTF1


//***************************************************************************
//  $SF_NumLinesCom
//
//  Return the # lines in the function (excluding the header line)
//    when called from []FX with a simple char scalar or vector arg, or
//    when called from []FX with a nested arg
//***************************************************************************

UINT SF_NumLinesCom
    (HWND        hWndEC,            // Edit Control window handle (FE only)
     LPFX_PARAMS lpFX_Params)       // Ptr to common struc

{
    // The # function lines
    return -1 +
      (UINT) lpFX_Params->aplColsRht;
} // End SF_NumLinesCom


//***************************************************************************
//  $SF_NumLinesFE
//
//  Return the # lines in the function (excluding the header line)
//    when called from function editor
//***************************************************************************

UINT SF_NumLinesFE
    (HWND      hWndEC,              // Edit Control window handle (FE only)
     LPVOID    lpVoid)              // Ptr to common struc

{
    return -1 +
      (UINT) SendMessageW (hWndEC, EM_GETLINECOUNT, 0, 0);
} // End SF_NumLinesFE


//***************************************************************************
//  $SF_NumLinesLW
//
//  Return the # lines in the function (excluding the header line)
//    when called from LoadWorkspace
//***************************************************************************

UINT SF_NumLinesLW
    (HWND        hWndEC,            // Edit Control window handle (FE only)
     LPLW_PARAMS lpLW_Params)       // Ptr to common struc

{
    return -1 +
      GetPrivateProfileIntW (lpLW_Params->lpwSectName,      // Ptr to the section name
                             KEYNAME_COUNT,                 // Ptr to the key name
                             0,                             // Default value if not found
                             lpLW_Params->lpwszDPFE);       // Ptr to the file name
} // End SF_NumLinesLW


//***************************************************************************
//  $SF_NumLinesM
//
//  Return the # lines in the function (excluding the header line)
//    when called from []FX with a simple char matrix arg
//***************************************************************************

UINT SF_NumLinesM
    (HWND        hWndEC,            // Edit Control window handle (FE only)
     LPFX_PARAMS lpFX_Params)       // Ptr to common struc

{
    // The # function lines
    return -1 +
      (UINT) lpFX_Params->aplRowsRht;
} // End SF_NumLinesM


//***************************************************************************
//  $SF_NumLinesTF1
//
//  Return the # lines in the function (excluding the header line)
//    when called from []TF with a simple char matrix arg
//***************************************************************************

UINT SF_NumLinesTF1
    (HWND         hWndEC,           // Edit Control window handle (FE only)
     LPTF1_PARAMS lpTF1_Params)     // Ptr to common struc

{
    // The # function lines
    return -1 +
      (UINT) lpTF1_Params->aplRowsRht;
} // End SF_NumLinesTF1


//***************************************************************************
//  $SF_CreationTimeCom
//
//  Return the creation time
//    when called from function editor, or
//    when called from []FX with a simple char scalar or vector arg, or
//    when called from []FX with a simple char matrix arg, or
//    when called from []FX with a nested arg
//    when called from []TF with a simple char matrix arg
//***************************************************************************

void SF_CreationTimeCom
    (LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     SYSTEMTIME *lpSystemTime,      // Ptr to current system (UTC) time
     FILETIME   *lpftCreation)      // Ptr to output save area

{
    // Convert system time to file time and save as creation time
    SystemTimeToFileTime (lpSystemTime, lpftCreation);
} // End SF_CreationTimeCom


//***************************************************************************
//  $SF_CreationTimeLW
//
//  Return the creation time
//    when called from LoadWorkspace
//***************************************************************************

void SF_CreationTimeLW
    (LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     SYSTEMTIME *lpSystemTime,      // Ptr to current system (UTC) time
     FILETIME   *lpftCreation)      // Ptr to output save area

{
    // Copy from parameters
    *lpftCreation = lpLW_Params->ftCreation;
} // End SF_CreationTimeLW


//***************************************************************************
//  $SF_LastModTimeCom
//
//  Return the last modification time
//    when called from function editor
//    when called from []FX with a simple char scalar or vector arg
//    when called from []FX with a simple char matrix arg
//    when called from []FX with a nested arg
//    when called from []TF with a simple char matrix arg
//***************************************************************************

void SF_LastModTimeCom
    (LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     SYSTEMTIME *lpSystemTime,      // Ptr to current system (UTC) time
     FILETIME   *lpftLastMod)       // Ptr to output save area

{
    // Convert system time to file time and save as last modification time
    SystemTimeToFileTime (lpSystemTime, lpftLastMod);
} // End SF_LastModTimeCom


//***************************************************************************
//  $SF_LastModTimeLW
//
//  Return the last modification time
//    when called from LoadWorkspace
//***************************************************************************

void SF_LastModTimeLW
    (LPLW_PARAMS lpLW_Params,       // Ptr to common struc
     SYSTEMTIME *lpSystemTime,      // Ptr to current system (UTC) time
     FILETIME   *lpftLastMod)       // Ptr to output save area

{
    // Copy from parameters
    *lpftLastMod = lpLW_Params->ftLastMod;
} // End SF_LastModTimeLW


//***************************************************************************
//  $SF_UndoBufferCom
//
//  Return a ptr to the Undo buffer
//    when called from []FX with a simple char scalar or vector arg
//    when called from []FX with a simple char matrix arg
//    when called from []FX with a nested arg
//    when called from []TF with a simple char matrix arg
//***************************************************************************

HGLOBAL SF_UndoBufferCom
    (HWND        hWndEC,            // Edit Cnotrol window handle
     LPVOID      lpVoid)            // Ptr to common struc

{
    // No Undo Buffer for []FX created functions
    return NULL;
} // End SF_UndoBufferCom


//***************************************************************************
//  $SF_UndoBufferFE
//
//  Return a ptr to the Undo buffer
//    when called from function editor
//***************************************************************************

HGLOBAL SF_UndoBufferFE
    (HWND        hWndEC,            // Edit Cnotrol window handle
     LPVOID      lpVoid)            // Ptr to common struc

{
    HWND         hWndFE;            // Function Editor window handle
    LPUNDO_BUF   lpUndoBeg,         // Ptr to start of Undo buffer
                 lpUndoLst;         // Ptr to end ...
    HGLOBAL      hGlbUndoBuff;      // Undo Buffer global memory handle

    // Get the Function Editor window handle
    hWndFE = GetParent (hWndEC);

    (HANDLE_PTR) lpUndoBeg = GetWindowLongPtrW (hWndFE, GWLSF_UNDO_BEG);
    if (lpUndoBeg)
    {
        LPUNDO_BUF lpMemUndo;       // Ptr to Undo Buffer global memory

        // Get the ptr to the last entry in the Undo Buffer
        (HANDLE_PTR) lpUndoLst = GetWindowLongPtrW (hWndFE, GWLSF_UNDO_LST);

        // Check for empty Undo buffer
        if (lpUndoLst EQ lpUndoBeg)
            hGlbUndoBuff = NULL;
        else
        {
            // Allocate storage for the Undo buffer
            hGlbUndoBuff = DbgGlobalAlloc (GHND, (lpUndoLst - lpUndoBeg) * sizeof (UNDO_BUF));
            if (!hGlbUndoBuff)
            {
                MessageBox (hWndEC,
                            "Insufficient memory to save Undo buffer!!",
                            lpszAppName,
                            MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                return NULL;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemUndo = MyGlobalLock (hGlbUndoBuff);

            // Copy the Undo Buffer to global memory
            CopyMemory (lpMemUndo, lpUndoBeg, (lpUndoLst - lpUndoBeg) * sizeof (UNDO_BUF));

            // We no longer need this ptr
            MyGlobalUnlock (hGlbUndoBuff); lpMemUndo = NULL;
        } // End IF/ELSE
    } // End IF

    return hGlbUndoBuff;
} // End SF_UndoBufferFE


//***************************************************************************
//  $SF_UndoBufferLW
//
//  Return a ptr to the Undo buffer
//    when called from LoadWorkspace
//***************************************************************************

HGLOBAL SF_UndoBufferLW
    (HWND        hWndEC,            // Edit Cnotrol window handle
     LPLW_PARAMS lpLW_Params)       // Ptr to common struc

{
    LPWCHAR    lpMemUndoTxt;        // Ptr to Undo Buffer in text format
    WCHAR      wcAction;            // Temporary character
    LPUNDO_BUF lpMemUndoBin;        // Ptr to Undo Buffer in binary format
    UINT       uUndoCount,          // # entries in the Undo Buffer
               uFields;             // # fields parsed by swscanf
    HGLOBAL    hGlbUndoBuff;        // Undo Buffer global memory handle

    // Get parameters
    lpMemUndoTxt = lpLW_Params->lpMemUndoTxt;

    // Parse the # entries, and skip over it
    swscanf (lpMemUndoTxt, L"%d", &uUndoCount);
    lpMemUndoTxt = SkipBlackW (lpMemUndoTxt);

    // Check for empty buffer
    if (*lpMemUndoTxt EQ 0)
        return NULL;

    // Skip over the separating blank
    Assert (*lpMemUndoTxt EQ L' '); lpMemUndoTxt++;

    // Allocate storage for the Undo buffer
    hGlbUndoBuff = DbgGlobalAlloc (GHND, uUndoCount * sizeof (UNDO_BUF));
    if (!hGlbUndoBuff)
    {
        MessageBox (hWndEC,
                    "Insufficient memory to save Undo buffer!!",
                    lpszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemUndoBin = MyGlobalLock (hGlbUndoBuff);

    // Parse the Undo Buffer entries
    while (*lpMemUndoTxt)
    {
        // Convert the Undo Buffer text to Undo Buffer format

        // Parse the Action, CharPosBeg, CharPosEnd, Group
        uFields =
          swscanf (lpMemUndoTxt,
                   L"%c %d %d %d '%c'",
                  &wcAction,
                  &lpMemUndoBin->CharPosBeg,
                  &lpMemUndoBin->CharPosEnd,
                  &lpMemUndoBin->Group,
                  &lpMemUndoBin->Char);
        // Convert the Action code to binary
        switch (wcAction)
        {
            case L'N':
                // Save the Action
                lpMemUndoBin->Action = undoNone;

                break;

            case L'I':
                // Save the Action
                lpMemUndoBin->Action = undoIns;

                break;

            case L'R':
                // Save the Action
                lpMemUndoBin->Action = undoRep;

                break;

            case L'D':
                // Save the Action
                lpMemUndoBin->Action = undoDel;

                break;

            case L'S':
                // Save the Action
                lpMemUndoBin->Action = undoSel;

                break;

            case L'T':
                // Save the Action
                lpMemUndoBin->Action = undoInsToggle;

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the # fields scanned
        switch (uFields)
        {
            case 4:
                Assert (lpMemUndoBin->Action NE undoIns
                     && lpMemUndoBin->Action NE undoRep);
                break;

            case 5:
                // Parse the Char if Ins or Rep
                Assert (lpMemUndoBin->Action EQ undoIns
                     || lpMemUndoBin->Action EQ undoRep);
                if (lpMemUndoBin->Char EQ L'{')
                {
                    // Skip to the left brace char
                    lpMemUndoTxt = SkipToCharW (lpMemUndoTxt, L'{');

                    // Parse the {name}
                    lpMemUndoBin->Char = SymbolNameToChar (lpMemUndoTxt);
                } else
                if (lpMemUndoBin->Char EQ L'\\')
                {
                    // Skip over the slope char
                    lpMemUndoTxt = SkipPastCharW (lpMemUndoTxt, L'\\');

                    // Split cases based upon the next char
                    switch (*lpMemUndoTxt)
                    {
                        case L'n':
                            lpMemUndoBin->Char = L'\n';

                            break;

                        case L'r':
                            lpMemUndoBin->Char = L'\r';

                            break;

                        case L'\'':
                            lpMemUndoBin->Char = L'\'';

                            break;

                        case L'\\':
                            lpMemUndoBin->Char = L'\\';

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End IF/ELSE/...

                break;

            defstop
                break;
        } // End SWITCH

        // Skip over the separating comma
        lpMemUndoTxt = SkipPastCharW (lpMemUndoTxt, L',');

        if (*lpMemUndoTxt)
        {
            // Skip over the separating blank
            Assert (*lpMemUndoTxt EQ L' '); lpMemUndoTxt++;
        } // End IF

        // Skip to next entry
        lpMemUndoBin++;
    } // End WHILE

    // We no longer need this ptr
    MyGlobalUnlock (hGlbUndoBuff); lpMemUndoBin = NULL;

    return hGlbUndoBuff;
} // End SF_UndoBufferLW


//***************************************************************************
//  $SaveFunctionCom
//
//  Attempt to save a function to the WS.
//  Subroutine to SaveFunction; also called directly from []FX.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SaveFunctionCom"
#else
#define APPEND_NAME
#endif

BOOL SaveFunctionCom
    (HWND      hWndFE,                  // Function Editor window handle (FE only, NULL otherwise)
     LPSF_FCNS lpSF_Fcns)               // Ptr to common struc

{
    HWND           hWndEC = NULL;       // Window handle to Edit Control
    UINT           uLineLen;            // Line length
    HGLOBAL        hGlbTxtHdr = NULL,   // Header text global memory handle
                   hGlbTknHdr = NULL,   // Tokenized header text ...
                   hGlbDfnHdr = NULL;   // User-defined function/operator header ...
    LPMEMTXT_UNION lpMemTxtLine;        // Ptr to header/line text global memory
    FHLOCALVARS    fhLocalVars = {0};   // Re-entrant vars
    HGLOBAL        hGlbPTD;             // PerTabData global memory handle
    LPPERTABDATA   lpMemPTD;            // Ptr to PerTabData global memory
    WCHAR          wszTemp[1024];       // Save area for error message text
    MEMVIRTSTR     lclMemVirtStr[1] = {0};// Room for one GuardAlloc

    // Fill in common values
    lpSF_Fcns->bRet = FALSE;
    lpSF_Fcns->uErrLine = NEG1U;

    if (hWndFE)
    {
        Assert (IzitFE (hWndFE));
    } // End IF

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Get the handle to the edit control
    if (hWndFE)
        (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndFE, GWLSF_HWNDEC);

    // Get the length of the header line
    uLineLen = (*lpSF_Fcns->SF_LineLen) (hWndEC, lpSF_Fcns->LclParams, 0);

    // Allocate space for the text
    //   (the "sizeof (uLineLen)" is for the leading line length
    //    and the " + 1" is for the terminating zero)
    // Note, we can't use DbgGlobalAlloc here as we
    //   might have been called from the Master Frame
    //   via a system command, in which case there is
    //   no PTD for that thread.
    hGlbTxtHdr = MyGlobalAlloc (GHND, sizeof (lpMemTxtLine->U) + (uLineLen + 1) * sizeof (lpMemTxtLine->C));
    if (!hGlbTxtHdr)
    {
        MessageBox (hWndEC,
                    "Insufficient memory to save the function header text!!",
                    lpszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        goto ERROR_EXIT;
    } // End IF

    // The following test isn't an optimzation, but avoids
    //   overwriting the allocation limits of the buffer
    //   when filling in the leading WORD with uLineLen
    //   on the call to EM_GETLINE.

    // If the header is non-empty, ...
    if (uLineLen)
    {
        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLock (hGlbTxtHdr);

        // Save the line length
        lpMemTxtLine->U = uLineLen;

        // Tell EM_GETLINE maximum # chars in the buffer
        lpMemTxtLine->W = (WORD) uLineLen;

        // Read in the line text
        (*lpSF_Fcns->SF_ReadLine) (hWndEC, lpSF_Fcns->LclParams, 0, &lpMemTxtLine->C);

        // Tokenize the line
        hGlbTknHdr =
          Tokenize_EM (&lpMemTxtLine->C,
                        uLineLen,
                        hWndEC,
                       &ErrorHandler);
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtHdr); lpMemTxtLine = NULL;
    } // End IF

    if (!hGlbTknHdr)
    {
        // Mark the line in error
        lpSF_Fcns->uErrLine = 0;

        if (hWndFE)
        {
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Format the error message
            wsprintfW (wszTemp,
                       ERRMSG_SYNTAX_ERROR_IN_FUNCTION_HEADER,
                       lpMemPTD->uCaret);
            // Display the error message
            MessageBoxW (hWndEC,
                        wszTemp,
                        lpwszAppName,
                        MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
        } // End IF

        goto ERROR_EXIT;
    } // End IF

    // Allocate virtual memory for the Variable Strand accumulator
#ifdef DEBUG
    lclMemVirtStr[0].lpText   = "fhLocalvars.lpYYStrandStart in <SaveFunctionCom>";
#endif
    lclMemVirtStr[0].IncrSize = DEF_STRAND_INCRSIZE * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].MaxSize  = DEF_STRAND_MAXSIZE  * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].IniAddr  = (LPUCHAR)
    fhLocalVars.lpYYStrandStart =
      GuardAlloc (NULL,             // Any address
                  lclMemVirtStr[0].MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lclMemVirtStr[0].IniAddr)
    {
        if (hWndFE)
            MessageBox (hWndEC,
                        "Insufficient memory to save the function header strand stack!!",
                        lpszAppName,
                        MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        else
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lpSF_Fcns->lptkFunc);
        goto ERROR_EXIT;        // Mark as failed
    } // End IF

    // Link this struc into the chain
    LinkMVS (&lclMemVirtStr[0]);

    // Commit the intial size
    MyVirtualAlloc (lclMemVirtStr[0].IniAddr,
                    DEF_STRAND_INITSIZE * sizeof (PL_YYSTYPE),
                    MEM_COMMIT,
                    PAGE_READWRITE);
    // Parse the header
    if (ParseHeader (hWndEC, hGlbTknHdr, &fhLocalVars, TRUE))
    {
        UINT         uLineNum,          // Current line # in the Edit Control
                     uOffset,           // Cumulative offset
                     numResultSTE,      // # result STEs (may be zero)
                     numLftArgSTE,      // # left arg ...
                     numRhtArgSTE,      // # right ...
                     numLocalsSTE,      // # locals ...
                     numFcnLines,       // # lines in the function
                     numSTE;            // Loop counter
        LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header ...
        LPFCNLINE    lpFcnLines;        // Ptr to array of function line structs (FCNLINE[numFcnLines])
        LPSYMENTRY   lpSymName = NULL,  // Ptr to SYMENTRY for the function name
                    *lplpSymDfnHdr;     // Ptr to LPSYMENTRYs at end of user-defined function/operator header
        SYSTEMTIME   systemTime;        // Current system (UTC) time
        FILETIME     ftCreation;        // Creation time
        HGLOBAL      hGlbOldDfn;        // Old Dfn global memory handle

        // Check on invalid function name (e.g. empty function header/body)
        if (!fhLocalVars.lpYYFcnName)
        {
            // Mark the line in error
            lpSF_Fcns->uErrLine = 0;

            goto ERROR_EXIT;
        } // End IF

        // Get the current system (UTC) time
        GetSystemTime (&systemTime);

        // Check to see if this function is already in global memory
        lpSymName = fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym;

        // Get the old Dfn global memory handle
        hGlbOldDfn = lpSymName->stData.stGlbData;

        // If it's already in memory, get its creation time
        //   and then free it
        if (hGlbOldDfn)
        {
            LPSIS_HEADER lpSISCur;

            // Clear the ptr type bits
            hGlbOldDfn = ClrPtrTypeDirAsGlb (hGlbOldDfn);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Get a ptr to the current SI stack
            lpSISCur = lpMemPTD->lpSISCur;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            // Check for already on the SI stack
            for (;
                 lpSISCur;
                 lpSISCur = lpSISCur->lpSISPrv)
            if (lpSISCur->hGlbDfnHdr EQ hGlbOldDfn)
            {
                // Signal SI Damage
                MessageBox (hWndEC,
                            "SI Damage in pending function:  changes to this function NOT saved",
                            lpszAppName,
                           MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                SetFocus (GetParent (hWndEC));

                goto ERROR_EXIT;
            } // End FOR

            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLock (hGlbOldDfn);

            // Get the creation time
            ftCreation = lpMemDfnHdr->ftCreation;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbOldDfn); lpMemDfnHdr = NULL;

            // Free it
            FreeResultGlobalDfn (hGlbOldDfn); hGlbOldDfn = NULL;
        } else
            (*lpSF_Fcns->SF_CreationTime) (lpSF_Fcns->LclParams, &systemTime, &ftCreation);

        // Get # extra result STEs
        if (fhLocalVars.lpYYResult)
            // Save in global memory
            numResultSTE = fhLocalVars.lpYYResult->uStrandLen;
        else
            numResultSTE = 0;

        // Get # extra left arg STEs
        if (fhLocalVars.lpYYLftArg)
            // Save in global memory
            numLftArgSTE = fhLocalVars.lpYYLftArg->uStrandLen;
        else
            numLftArgSTE = 0;

        // Get # extra right arg STEs
        if (fhLocalVars.lpYYRhtArg)
            // Save in global memory
            numRhtArgSTE = fhLocalVars.lpYYRhtArg->uStrandLen;
        else
            numRhtArgSTE = 0;

        // Get # locals STEs
        if (fhLocalVars.lpYYLocals)
            // Save in global memory
            numLocalsSTE = fhLocalVars.lpYYLocals->uStrandLen;
        else
            numLocalsSTE = 0;

        // Get # lines in the function (excluding the header)
        numFcnLines = (*lpSF_Fcns->SF_NumLines) (hWndEC, lpSF_Fcns->LclParams);

        // Allocate global memory for the function header
        hGlbDfnHdr = DbgGlobalAlloc (GHND, sizeof (DFN_HEADER)
                                         + sizeof (LPSYMENTRY) * (numResultSTE
                                                                + numLftArgSTE
                                                                + numRhtArgSTE
                                                                + numLocalsSTE)
                                         + sizeof (FCNLINE) * numFcnLines);
        if (!hGlbDfnHdr)
        {
            if (hWndFE)
                MessageBox (hWndEC,
                            "Insufficient memory to save the function header!!",
                            lpszAppName,
                            MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            else
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                           lpSF_Fcns->lptkFunc);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

        // Save numbers in global memory
        lpMemDfnHdr->numResultSTE = numResultSTE;
        lpMemDfnHdr->numLftArgSTE = numLftArgSTE;
        lpMemDfnHdr->numRhtArgSTE = numRhtArgSTE;
        lpMemDfnHdr->numLocalsSTE = numLocalsSTE;

        // Save the static parts of the function into global memory
        lpMemDfnHdr->Sig.nature   = DFN_HEADER_SIGNATURE;
        lpMemDfnHdr->Version      = 0;
        lpMemDfnHdr->DfnType      = fhLocalVars.DfnType;
        lpMemDfnHdr->FcnValence   = fhLocalVars.FcnValence;
        lpMemDfnHdr->DfnAxis      = fhLocalVars.DfnAxis;
        lpMemDfnHdr->NoDispRes    = fhLocalVars.NoDispRes;
        lpMemDfnHdr->ListRes      = fhLocalVars.ListRes;
        lpMemDfnHdr->ListLft      = fhLocalVars.ListLft;
        lpMemDfnHdr->ListRht      = fhLocalVars.ListRht;
        lpMemDfnHdr->RefCnt       = 1;
        lpMemDfnHdr->numFcnLines  = numFcnLines;
        lpMemDfnHdr->steLftOpr    = fhLocalVars.lpYYLftOpr
                                  ? fhLocalVars.lpYYLftOpr ->tkToken.tkData.tkSym
                                  : NULL;
        lpMemDfnHdr->steFcnName   = fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym;
        lpMemDfnHdr->steAxisOpr   = fhLocalVars.lpYYAxisOpr
                                  ? fhLocalVars.lpYYAxisOpr->tkToken.tkData.tkSym
                                  : NULL;
        lpMemDfnHdr->steRhtOpr    = fhLocalVars.lpYYRhtOpr
                                  ? fhLocalVars.lpYYRhtOpr ->tkToken.tkData.tkSym
                                  : NULL;
        lpMemDfnHdr->hGlbTxtHdr   = hGlbTxtHdr;
        lpMemDfnHdr->hGlbTknHdr   = hGlbTknHdr;

        // Save creation time
        lpMemDfnHdr->ftCreation = ftCreation;

        // Get the function's last modification time
        (*lpSF_Fcns->SF_LastModTime) (lpSF_Fcns->LclParams, &systemTime, &lpMemDfnHdr->ftLastMod);

        // Get the ptr to the start of the Undo Buffer
        lpMemDfnHdr->hGlbUndoBuff = (*lpSF_Fcns->SF_UndoBuffer) (hWndEC, lpSF_Fcns->LclParams);

        // Save the dynamic parts of the function into global memory

        // Initialize cumulative offset
        uOffset = sizeof (DFN_HEADER);

        // Initialize ptr to ptr to SYMENTRYs at end of header
        lplpSymDfnHdr = (LPAPLHETERO) ByteAddr (lpMemDfnHdr, uOffset);

        // If there's a result, ...
        if (fhLocalVars.lpYYResult)
        {
            lpMemDfnHdr->offResultSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numResultSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYResult[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offResultSTE = 0;

        // If there's a left arg, ...
        if (fhLocalVars.lpYYLftArg)
        {
            lpMemDfnHdr->offLftArgSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numLftArgSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYLftArg[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offLftArgSTE = 0;

        // If there's a right arg, ...
        if (fhLocalVars.lpYYRhtArg)
        {
            lpMemDfnHdr->offRhtArgSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numRhtArgSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYRhtArg[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offRhtArgSTE = 0;

        // If there are any locals, ...
        if (fhLocalVars.lpYYLocals)
        {
            lpMemDfnHdr->offLocalsSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numLocalsSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYLocals[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offLocalsSTE = 0;

        // Save offset to start of function line structs
        if (lpMemDfnHdr->numFcnLines)
            lpMemDfnHdr->offFcnLines = uOffset;
        else
            lpMemDfnHdr->offFcnLines = 0;

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Loop through the lines
        for (uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
        {
            HGLOBAL hGlbTxtLine;    // Line text global memory handle

            // Get the length of the function line
            uLineLen = (*lpSF_Fcns->SF_LineLen) (hWndEC, lpSF_Fcns->LclParams, uLineNum + 1);

            // Allocate global memory to hold this text
            // The "sizeof (uLineLen) + " is for the leading length
            //   and the "+ 1" is for the terminating zero
            //   as well as to handle GlobalLock's aversion to locking
            //   zero-length arrays
            hGlbTxtLine = DbgGlobalAlloc (GHND, sizeof (lpMemTxtLine->U) + (uLineLen + 1) * sizeof (APLCHAR));
            if (!hGlbTxtLine)
            {
                if (hWndFE)
                    MessageBox (hWndEC,
                                "Insufficient memory to save a function line!!",
                                lpszAppName,
                                MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                else
                    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                               lpSF_Fcns->lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Save the global memory handle
            lpFcnLines->hGlbTxtLine = hGlbTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Save the line length
            lpMemTxtLine->U = uLineLen;

            // The following test isn't an optimzation, but avoids
            //   overwriting the allocation limits of the buffer
            //   when filling in the leading WORD with uLineLen
            //   on the call to EM_GETLINE.

            // If the line is non-empty, ...
            if (uLineLen)
            {
                // Tell EM_GETLINE maximum # chars in the buffer
                lpMemTxtLine->W = (WORD) uLineLen;

                // Read in the line text
                (*lpSF_Fcns->SF_ReadLine) (hWndEC, lpSF_Fcns->LclParams, uLineNum + 1, &lpMemTxtLine->C);

                // Tokenize the line
                lpFcnLines->hGlbTknLine =
                  Tokenize_EM (&lpMemTxtLine->C,
                                uLineLen,
                                hWndEC,
                               &ErrorHandler);
            } else
                // Tokenize the (empty) line
                lpFcnLines->hGlbTknLine =
                  Tokenize_EM (L"",
                               0,
                               hWndEC,
                              &ErrorHandler);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

            // If tokenization failed, ...
            if (!lpFcnLines->hGlbTknLine)
            {
                if (hWndFE)
                {
                    // Lock the memory to get a ptr to it
                    lpMemPTD = MyGlobalLock (hGlbPTD);

                    // Format the error message
                    wsprintfW (wszTemp,
                               L"SYNTAX ERROR on line # %d, position %d -- function not saved",
                               uLineNum + 1,
                               lpMemPTD->uCaret);
                    // Display the error message
                    MessageBoxW (hWndEC,
                                wszTemp,
                                lpwszAppName,
                                MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
                } else
                    // Save the line # in error (origin-0)
                    lpSF_Fcns->uErrLine = uLineNum + 1;

                goto ERROR_EXIT;
            } // End IF

            // No monitor info as yet
            lpFcnLines->hGlbMonInfo = NULL;

            // Transfer Stop & Trace info
            lpFcnLines->bStop  =
            lpFcnLines->bTrace = FALSE;     // ***FIXME*** -- transfer from orig fn

            // Skip to the next struct
            lpFcnLines++;
        } // End FOR

        // Check for special labels ([]IDENTITY, []INVERSE, []PROTOTYPE, and []SINGLETON)
        GetSpecialLabelNums (lpMemDfnHdr);

        // Save the global memory handle in the STE
        lpSymName->stData.stGlbData = MakePtrTypeGlb (hGlbDfnHdr);

        // Mark as valued and user-defined function/operator
        lpSymName->stFlags.Value  =
        lpSymName->stFlags.UsrDfn = TRUE;

        // Copy the "Accepts Axis Operator" flag
        lpSymName->stFlags.DfnAxis = lpMemDfnHdr->DfnAxis;

        // Set the object name
        lpSymName->stFlags.ObjName = OBJNAME_USR;

        // Mark as with the proper type and valence

        // Split cases based upon the function type
        switch (lpMemDfnHdr->DfnType)
        {
            case DFNTYPE_OP1:   // Monadic operator
                lpSymName->stFlags.stNameType = NAMETYPE_OP1;

                break;

            case DFNTYPE_OP2:   // Dyadic operator
                lpSymName->stFlags.stNameType = NAMETYPE_OP2;

                break;

            case DFNTYPE_FCN:   // Function
                // Split cases based upon the function valence
                switch (lpMemDfnHdr->FcnValence)
                {
                    case FCNVALENCE_NIL:    // Niladic function
                        lpSymName->stFlags.stNameType = NAMETYPE_FN0;

                        break;

                    case FCNVALENCE_MON:    // Monadic function
                    case FCNVALENCE_DYD:    // Dyadic function
                    case FCNVALENCE_AMB:    // Ambivalent function
                        lpSymName->stFlags.stNameType = NAMETYPE_FN12;

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

            case DFNTYPE_UNK:   // Unknown
            defstop
                break;
        } // End SWITCH

        // Mark as unchanged since the last save
        if (hWndFE)
            SetWindowLongW (hWndFE, GWLSF_CHANGED, FALSE);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

        lpSF_Fcns->bRet = TRUE;
        lpSF_Fcns->lpSymName = lpSymName;

        goto NORMAL_EXIT;
    } else
    if (hWndFE)
        // Ensure the FE window redraws the caret
        SetFocus (hWndFE);
ERROR_EXIT:
    if (hGlbDfnHdr)
    {
        FreeResultGlobalDfn (hGlbDfnHdr); hGlbDfnHdr = NULL;
        hGlbTknHdr = hGlbTxtHdr = NULL;
    } // End IF

    if (hGlbTknHdr)
    {
        // Free the tokens
        Untokenize (hGlbTknHdr);

        // We no longer need this storage
        DbgGlobalFree (hGlbTknHdr); hGlbTknHdr = NULL;
    } // End IF

    if (hGlbTxtHdr)
    {
        // We no longer need this storage
        MyGlobalFree (hGlbTxtHdr); hGlbTxtHdr = NULL;
    } // End IF
NORMAL_EXIT:
    // If we allocated virtual storage, ...
    if (lclMemVirtStr[0].IniAddr)
    {
        // Free the virtual storage
        MyVirtualFree (lclMemVirtStr[0].IniAddr, 0, MEM_RELEASE); lclMemVirtStr[0].IniAddr = NULL;
        fhLocalVars.lpYYStrandStart = NULL;

        // Unlink this entry from the chain
        UnlinkMVS (&lclMemVirtStr[0]);
    } // End IF

    return lpSF_Fcns->bRet;
} // End SaveFunctionCom
#undef  APPEND_NAME


//***************************************************************************
//  $GetSpecialLabelNums
//
//  Return the line #s (or zero if not found) of the special line labels
//***************************************************************************

void GetSpecialLabelNums
    (LPDFN_HEADER  lpMemDfnHdr)     // Ptr to user-defined function/operator header

{
    UINT           numFcnLines,  // # lines in the function
                   uLineNum;     // Loop counter
    LPFCNLINE      lpFcnLines;   // Ptr to array of function line structs (FCNLINE[numFcnLines])
    LPTOKEN_HEADER lptkHdr;      // Ptr to header of tokenized line
    LPTOKEN        lptkLine;     // Ptr to tokenized line

    // Get # lines in the function
    numFcnLines = lpMemDfnHdr->numFcnLines;

    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

    // Loop through the function lines
    for (uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
    if (lpFcnLines->hGlbTknLine)
    {
        UINT numTokens;     // # tokens in the line

        // Lock the memory to get a ptr to it
        lptkHdr = MyGlobalLock (lpFcnLines->hGlbTknLine);

        // Get the # tokens in the line
        numTokens = lptkHdr->TokenCnt;

        // Get ptr to the tokens in the line
        lptkLine = (LPTOKEN) ByteAddr (lptkHdr, sizeof (TOKEN_HEADER));

        Assert (lptkLine[0].tkFlags.TknType EQ TKT_EOL
             || lptkLine[0].tkFlags.TknType EQ TKT_EOS);

        // If there are at least three tokens, ...
        //   (TKT_EOL, TKT_VARNAMED, TKT_LABELSEP)
        if (numTokens >= 3)
        {
            if (lptkLine[2].tkFlags.TknType EQ TKT_LABELSEP
             && lptkLine[1].tkFlags.TknType EQ TKT_VARNAMED)
            {
                HGLOBAL   hGlbName;     // Name's global memory handle
                LPAPLCHAR lpMemName;    // Ptr to the name

                // stData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lptkLine[1].tkData.tkSym) EQ PTRTYPE_STCONST);

                // Get the Name's global memory handle
                hGlbName = lptkLine[1].tkData.tkSym->stHshEntry->htGlbName;

                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (hGlbName);

                if (lstrcmpiW (lpMemName, WS_UTF16_QUAD L"identity") EQ 0)
                    lpMemDfnHdr->nIdentityLine  = uLineNum + 1;

                if (lstrcmpiW (lpMemName, WS_UTF16_QUAD L"inverse") EQ 0)
                    lpMemDfnHdr->nInverseLine   = uLineNum + 1;

                if (lstrcmpiW (lpMemName, WS_UTF16_QUAD L"prototype") EQ 0)
                    lpMemDfnHdr->nPrototypeLine = uLineNum + 1;

                if (lstrcmpiW (lpMemName, WS_UTF16_QUAD L"singleton") EQ 0)
                    lpMemDfnHdr->nSingletonLine = uLineNum + 1;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbName); lpMemName = NULL;
            } // End IF
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (lpFcnLines->hGlbTknLine); lptkHdr = NULL; lptkLine = NULL;

        // Skip to the next struct
        lpFcnLines++;
    } // End FOR/IF
} // End GetSpecialLabelNums


//***************************************************************************
//  End of File: savefcn.c
//***************************************************************************
