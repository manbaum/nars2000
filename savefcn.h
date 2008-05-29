//***************************************************************************
//  NARS2000 -- Save Function Header
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

typedef struct tagSF_FCNS
{
    BOOL       bRet;                // 00:  TRUE iff result is valid
    UINT       uErrLine;            // 04:  If (!bRet), the line in error (origin-0) (NEG1U = Error Message valid)
    LPSYMENTRY lpSymName;           // 08:  If (bRet), ptr to SYMENTRY of the function name
    LPTOKEN    lptkFunc;            // 0C:  Ptr to function token
    LPVOID     LclParams;           // 10:  Ptr to local parameters
    UINT (*SF_LineLen)  (HWND, LPVOID, UINT);                   // 14:  Ptr to get line length function
    void (*SF_ReadLine) (HWND, LPVOID, UINT, LPAPLCHAR);        // 18:  Ptr to read line function
    UINT (*SF_NumLines) (HWND, LPVOID);                         // 1C:  Ptr to get # lines function
    void (*SF_CreationTime) (LPVOID, SYSTEMTIME *, FILETIME *); // 20:  Ptr to get function creation time
    void (*SF_LastModTime)  (LPVOID, SYSTEMTIME *, FILETIME *); // 24:  Ptr to get function last mod time
    HGLOBAL (*SF_UndoBuffer) (HWND, LPVOID);                    // 28:  Ptr to get function Undo Buffer global memory handle
                                    // 2C:  Length of struc
} SF_FCNS, *LPSF_FCNS;


// Aliases for common functions
#define SF_NumLinesN        SF_NumLinesCom
#define SF_NumLinesSV       SF_NumLinesCom
#define SF_CreationTimeM    SF_CreationTimeCom
#define SF_CreationTimeN    SF_CreationTimeCom
#define SF_CreationTimeSV   SF_CreationTimeCom
#define SF_LastModTimeM     SF_LastModTimeCom
#define SF_LastModTimeN     SF_LastModTimeCom
#define SF_LastModTimeSV    SF_LastModTimeCom
#define SF_UndoBufferM      SF_UndoBufferCom
#define SF_UndoBufferN      SF_UndoBufferCom
#define SF_UndoBufferSV     SF_UndoBufferCom


typedef struct tagFX_PARAMS
{
    HGLOBAL    hGlbRht;             // 00:  Right arg global memory handle
    APLRANK    aplRankRht;          // 04:  Right arg rank
    APLLONGEST aplLongestRht;       // 0C:  Right arg immediate value
    APLDIM     aplRowsRht,          // 14:  Right arg # rows
               aplColsRht;          // 1C:  ...         cols
                                    // 24:  Length of struc
} FX_PARAMS, *LPFX_PARAMS;


typedef struct tagLW_PARAMS
{
    LPWCHAR    lpwSectName,         // 00:  Ptr to section name [nnn.Name]
               lpwszDPFE,           // 04:  Ptr to the workspace DPFE
               lpwBuffer,           // 08:  Ptr to temporary buffer
               lpMemUndoTxt;        // 0C:  Ptr to Undo Buffer in text format
    UINT       uMaxSize;            // 10:  Maximum size of lpwBuffer
    FILETIME   ftCreation,          // 14:  Function Creation Time
               ftLastMod;           // 18:  Function Last Modification Time
                                    // 1C:  Length
} LW_PARAMS, *LPLW_PARAMS;


//***************************************************************************
//  End of File: savefcn.h
//***************************************************************************
