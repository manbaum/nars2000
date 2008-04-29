//***************************************************************************
//  NARS2000 -- System Function -- Quad SYSVER
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
#include <time.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnSYSVER_EM_YY
//
//  System function:  []SYSVER -- System Version
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnSYSVER_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnSYSVER_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    UINT         ByteRes;           // # bytes in the result
    APLNELM      aplNELMRes;        // Result NELM
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPAPLCHAR    lpMemRes,          // Ptr to result global memory
                 lpMemData;         // Ptr to result data
    LPAPLCHAR    lpw;               // Temporary ptr
    HANDLE       hFile;             // File handle from CreateFileW
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************

    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Define maximum length of []SYSVER
#define SYSVER  L"000.000.0000.00799  Tue Jan 16 17:43:45 2007  Win/32"
#define SYSVER_NELM    ((sizeof (SYSVER) / sizeof (APLCHAR)) - 1)

    // Calculate space needed for the result
    ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, SYSVER_NELM, 1);

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
////lpHeader->NELM       = SYSVER_NELM;     // Filled in below
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
////*VarArrayBaseToDim (lpMemRes) = SYSVER_NELM;    // Filled in below

    // Skip over the header and dimensions to the data
    lpw = lpMemData = VarArrayBaseToData (lpMemRes, 1);

    // Read in the application's File Version String
    LclFileVersionStrW (wszAppDPFE, lpw);

    // Skip to the trailing zero
    lpw += lstrlenW (lpw);
    *lpw++ = L' ';      // Blank separators
    *lpw++ = L' ';

    // Open the executable file so we can read its internal timestamp
    hFile = CreateFileW (wszAppDPFE,                // lpwFileName
                         GENERIC_READ,              // dwDesiredAccess
                         FILE_SHARE_READ,           // dwShareMode
                         NULL,                      // lpSecurityAttributes
                         OPEN_EXISTING,             // dwCreationDistribution
                         FILE_ATTRIBUTE_NORMAL,     // dwFlagsAndAttributes
                         NULL);                     // hTemplateFile
    if (hFile)
    {
        DWORD              dwTemp,
                           dwCount;
        IMAGE_DOS_HEADER   idh;
        IMAGE_NT_HEADERS32 inth;

        // Set the file pointer to read the e_lfanew value
        SetFilePointer (hFile, ((LPBYTE) &idh.e_lfanew) - (LPBYTE) &idh, NULL, FILE_BEGIN);

        // Read in the e_lfanew value
        ReadFile (hFile, &dwTemp, sizeof (dwTemp), &dwCount, NULL);

        // Add in the distance to the file timestamp
        dwTemp += ((LPBYTE) (&inth.FileHeader.TimeDateStamp)) - (LPBYTE) &inth;

        // Set file pointer to the file timestamp
        SetFilePointer (hFile, dwTemp, NULL, FILE_BEGIN);

        // Read in the file timestamp
        ReadFile (hFile, &dwTemp, sizeof (dwTemp), &dwCount, NULL);

        // Get the time in ASCII
        lstrcpyW (lpw, ConvTimeW (dwTemp));

        // Skip to the trailing zero
        lpw += lstrlenW (lpw);
        *lpw++ = L' ';    // Blank separators
        *lpw++ = L' ';

#define SYSTYPE     L"Win/32"

        CopyMemory (lpw, SYSTYPE, sizeof (SYSTYPE) - 1 * sizeof (APLCHAR));

        // We no longer need this handle
        CloseHandle (hFile); hFile = NULL;
    } // End IF

    // Calculate the actual NELM
    aplNELMRes = lstrlenW (lpMemData);

    // Fill in actual length of []SYSVER
#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    lpHeader->NELM = aplNELMRes;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Make sure we didn't overwrite
    if (aplNELMRes < SYSVER_NELM)
    {
        // Calculate space needed for the result
        ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

        // Re-allocate the global downwards
        hGlbRes = MyGlobalReAlloc (hGlbRes, ByteRes, GMEM_MOVEABLE);
    } else
    if (aplNELMRes > SYSVER_NELM)
        // We should never get here
        DbgStop ();

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnSYSVER_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ConvTimeW
//
//  Convert to ASCII string in the form of
//    Wed Jan 02 02:03:55 1980
//***************************************************************************

LPWSTR ConvTimeW
    (DWORD time)
{
    LPWSTR lpw;

    lpw = _wasctime (gmtime ((const time_t *) &time));

    // Zap the "\n" so it prints on one line
    lpw[lstrlenW (lpw) - 1] = L'\0';

    return lpw;
} // End ConvTimeW


//***************************************************************************
//  End of File: qf_sysver.c
//***************************************************************************
