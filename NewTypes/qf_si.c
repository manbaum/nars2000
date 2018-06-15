//***************************************************************************
//  NARS2000 -- System Function -- Quad SI & Quad SINL
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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
#include "headers.h"
#include "debug.h"              // For xxx_TEMP_OPEN macros


//***************************************************************************
//  $SysFnSI_EM_YY
//
//  System function:  []SI -- State Indicator
//***************************************************************************

LPPL_YYSTYPE SysFnSI_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return SysFnSICom_EM_YY (lptkLftArg,    // Ptr to left arg token (should be NULL)
                             lptkFunc,      // Ptr to function token
                             lptkRhtArg,    // Ptr to right arg token (should be NULL)
                             lptkAxis,      // Ptr to axis token (may be NULL)
                             FALSE);        // TRUE iff this is []SINL
} // End SysFnSI_EM_YY


//***************************************************************************
//  $SysFnSINL_EM_YY
//
//  System function:  []SINL -- State Indicator with Namelist
//***************************************************************************

LPPL_YYSTYPE SysFnSINL_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return SysFnSICom_EM_YY (lptkLftArg,    // Ptr to left arg token (should be NULL)
                             lptkFunc,      // Ptr to function token
                             lptkRhtArg,    // Ptr to right arg token (should be NULL)
                             lptkAxis,      // Ptr to axis token (may be NULL)
                             TRUE);         // TRUE iff this is []SINL
} // End SysFnSINL_EM_YY


//***************************************************************************
//  $SysFnSICom_EM_YY
//
//  System function:  Common routine for []SI/[]SINL -- State Indicator
//***************************************************************************

LPPL_YYSTYPE SysFnSICom_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     UBOOL   bSINL)                 // TRUE iff []SINL

{
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    LPSIS_HEADER      lpSISCur;             // Ptr to current SIS_HEADER srtuc
    LPWCHAR           lpwszTemp,            // Ptr to temporary storage
                      lpwszCur,             // ...
                      lpwszEnd;             // ...
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    size_t            uLen,                 // Length of the current line
                      uNumCols,             // # cols in the result
                      uNumRows;             // # rows ...
    APLNELM           aplNELMRes;           // Result NELM
    APLDIM            uDimRes[2];           // Dimensions of the result
    HGLOBAL           hGlbRes = NULL;       // Result global memory handle
    LPVARARRAY_HEADER lpMemHdrRes = NULL;   // Ptr to result header
    LPWCHAR           lpMemRes;             // Ptr to result global memory
    VARS_TEMP_OPEN

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary storage
    lpwszCur    =
    lpwszTemp   = lpMemPTD->lpwszTemp;
    CHECK_TEMP_OPEN

    // Loop backwards through the SI levels
    for (lpSISCur = lpMemPTD->lpSISCur, uNumRows = uNumCols = 0;
         lpSISCur NE NULL;
         lpSISCur = lpSISCur->lpSISPrv)
    {
        LPWCHAR lpMemName;              // Ptr to function name global memory

        // Split cases based upon the caller's function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_IMM:
                break;

            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
                // Count in another row
                uNumRows++;

                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLockWsz (lpSISCur->hGlbFcnName);

                // Format the Name, Line #, and Suspension marker
                wsprintfW (lpwszCur,
                           L"%s[%d] %c",
                           lpMemName,
                           lpSISCur->CurLineNum,
                           " *"[lpSISCur->bSuspended]);
                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

                // If it's []SINL, display the namelist
                if (bSINL)
                {
                    LPWCHAR    lpwszNL;         // Temp ptr to the namelist
                    UINT       numSymEntries;   // # LPSYMENTRYs localized on the stack
                    LPSYMENTRY lpSymEntryNxt;   // Ptr to next localized LPSYMENTRY on the SIS

                    // Skip over the line text to point to the terminating zero
                    lpwszNL = &lpwszCur[lstrlenW (lpwszCur)];

                    // Get # LPSYMENTRYs on the stack
                    numSymEntries = lpSISCur->numSymEntries;

                    // Point to the localized LPSYMENTRYs
                    lpSymEntryNxt = (LPSYMENTRY) ByteAddr (lpSISCur, sizeof (SIS_HEADER));

                    // Loop through the # LPSYMENTRYs
                    while (numSymEntries-- NE 0)
                    {
                        // Set and skip over two blanks
                        *lpwszNL++ = L' ';
                        *lpwszNL++ = L' ';

                        // Copy the STE name to local storage
                        lpwszNL = CopySteName (lpwszNL, lpSymEntryNxt++, NULL);
                    } // End WHILE

                    // End the last line
                    *lpwszNL = WC_EOS;
                } // End IF

                // Calculate the length of this line
                uLen = lstrlenW (lpwszCur);

                // Calculate the maximum # cols
                uNumCols = max (uNumCols, uLen);

                // Skip over the formatted text
                //   and the terminating zero
                lpwszCur += uLen + 1;

                break;

            case DFNTYPE_EXEC:
                // Count in another row
                uNumRows++;

                // Format the line
                strcpyW (lpwszCur, WS_UTF16_UPTACKJOT);

                // Calculate the length of this line
                uLen = lstrlenW (lpwszCur);

                // Calculate the maximum # cols
                uNumCols = max (uNumCols, uLen);

                // Skip over the formatted text
                //   and the terminating zero
                lpwszCur += uLen + 1;

                break;

            case DFNTYPE_QUAD:
                // Count in another row
                uNumRows++;

                // Format the line
                strcpyW (lpwszCur, WS_UTF16_QUAD);

                // Calculate the length of this line
                uLen = lstrlenW (lpwszCur);

                // Calculate the maximum # cols
                uNumCols = max (uNumCols, uLen);

                // Skip over the formatted text
                //   and the terminating zero
                lpwszCur += uLen + 1;

                break;

            case DFNTYPE_ERRCTRL:
                // Count in another row
                uNumRows++;

                // Format the line
                strcpyW (lpwszCur, $QUAD_EA L"/" $QUAD_EC);

                // Calculate the length of this line
                uLen = lstrlenW (lpwszCur);

                // Calculate the maximum # cols
                uNumCols = max (uNumCols, uLen);

                // Skip over the formatted text
                //   and the terminating zero
                lpwszCur += uLen + 1;

                break;

            case DFNTYPE_UNK:
            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Mark as the end of the temp result
    lpwszEnd = lpwszCur;

    // Calculate the dimensions of the result
    uDimRes[0] = uNumRows;
    uDimRes[1] = uNumCols;

    // Calculate the NELM of the result
    aplNELMRes = uNumRows * uNumCols;

    // Allocate a character array for the result
    hGlbRes = AllocateGlobalArray (ARRAY_CHAR, aplNELMRes, 2, uDimRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Fill the result with blanks
    FillMemoryW (lpMemRes, (APLU3264) aplNELMRes, L' ');

    // Loop through the temp result
    for (lpwszCur = lpwszTemp; lpwszCur < lpwszEnd; lpwszCur += uLen + 1)
    {
        // Calculate the length of this line
        uLen = lstrlenW (lpwszCur);

        // Copy the text to the result
        CopyMemoryW (lpMemRes, lpwszCur, uLen);

        // Skip to the next line in the result
        lpMemRes += uNumCols;
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;

    EXIT_TEMP_OPEN

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // ...
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End SysFnSICom_EM_YY


//***************************************************************************
//  End of File: qf_si.c
//***************************************************************************
