//***************************************************************************
//  NARS2000 -- System Command:  )WSID
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
#include <time.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "aplerrors.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CmdWsid_EM
//
//  Execute the system command:  )WSID [wsid]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdWsid_EM"
#else
#define APPEND_NAME
#endif

BOOL CmdWsid_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    HGLOBAL      hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    APLNELM      aplNELMWSID;           // []WSID NELM
    APLRANK      aplRankWSID;           // ...    rank
    LPAPLCHAR    lpMemWSID;             // Ptr to old []WSID global memory
    WCHAR        wszTailDPFE[_MAX_PATH];// Save area for canonical form of given ws name
    APLUINT      ByteWSID;              // # bytes in the []WSID
    HGLOBAL      hGlbWSID;              // []WSID global memory handle
    UINT         uLen;                  // Length of given WSID
    BOOL         bRet = FALSE;          // TRUE iff result is valid

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Lock the memory to get a ptr to it
    lpMemWSID = MyGlobalLock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData));

#define lpHeader        ((LPVARARRAY_HEADER) lpMemWSID)
    // Get the NELM and Rank
    aplNELMWSID = lpHeader->NELM;
    aplRankWSID = lpHeader->Rank;
#undef  lpHeader

    // If there's a given WSID, set the WSID
    if (*lpwszTail)
    {
        // We no longer need this ptr
        MyGlobalUnlock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemWSID = NULL;

        // Convert the given workspace name into a canonical form
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, wszWorkDir);

        // Calculate space needed for the WSID
        uLen = lstrlenW (wszTailDPFE);

        // Include "+ 1" for a trailing zero
        ByteWSID = CalcArraySize (ARRAY_CHAR, uLen + 1, 1);

        // Allocate space for the WSID
        // N.B. Conversion from APLUINT to UINT.
        Assert (ByteWSID EQ (UINT) ByteWSID);
        hGlbWSID = DbgGlobalAlloc (GHND, (UINT) ByteWSID);
        if (!hGlbWSID)
        {
            ReplaceLastLineCRPmt (ERRMSG_WS_FULL APPEND_NAME);

            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemWSID = MyGlobalLock (hGlbWSID);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemWSID)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = 0;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = uLen;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemWSID) = uLen;

        // Skip over the header and dimensions to the data
        lpMemWSID = VarArrayBaseToData (lpMemWSID, 1);

        // Copy data to the []WSID
        CopyMemory (lpMemWSID, wszTailDPFE, uLen * sizeof (wszTailDPFE[0]));

        // We no longer need this ptr
        MyGlobalUnlock (hGlbWSID); lpMemWSID = NULL;

        // Lock the memory to get a ptr to it
        lpMemWSID = MyGlobalLock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData));

        // Skip over the header and dimensions to the data
        lpMemWSID = VarArrayBaseToData (lpMemWSID, aplRankWSID);

        // Display the old WSID
        AppendLine (L"WAS ", FALSE, FALSE);

        // If the old []WSID is empty, ...
        if (IsEmpty (aplNELMWSID))
            AppendLine (L"CLEAR WS", FALSE, TRUE);
        else
        {
            // Because the global memory doesn't have a zero terminator,
            //   we must copy the data to a temporary location and then
            //   append a zero terminator
            lstrcpynW (lpwszTemp, lpMemWSID, (UINT) aplNELMWSID + 1);
////////////lpwszTemp[aplNELMWSID] = L'\0';     // Already done via "+ 1" in lstrcpynW

            AppendLine (lpwszTemp, FALSE, TRUE);
        } // End IF/ELSE

        // We no longer need this ptr
        MyGlobalUnlock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemWSID = NULL;

        // Free the old []WSID
        FreeResultGlobalVar (lpMemPTD->lpSymQuadWSID->stData.stGlbData); lpMemPTD->lpSymQuadWSID->stData.stGlbData = NULL;

        // Save the new []WSID
        lpMemPTD->lpSymQuadWSID->stData.stGlbData = MakePtrTypeGlb (hGlbWSID);

        // Tell the Tab Ctrl about the new workspace name
        NewTabName ();
    } else
    // Display the current WSID
    {
        AppendLine (L"IS ", FALSE, FALSE);

        // If current []WSID is empty, ...
        if (IsEmpty (aplNELMWSID))
            AppendLine (L"CLEAR WS", FALSE, TRUE);
        else
        {
            // Skip over the header and dimensions to the data
            lpMemWSID = VarArrayBaseToData (lpMemWSID, aplRankWSID);

            // Because the global memory doesn't have a zero terminator,
            //   we must copy the data to a temporary location and then
            //   append a zero terminator
            lstrcpynW (lpwszTemp, lpMemWSID, (UINT) aplNELMWSID + 1);
////////////lpwszTemp[aplNELMWSID] = L'\0';     // Already done via "+ 1" in lstrcpynW

            AppendLine (lpwszTemp, FALSE, TRUE);
        } // End IF/ELSE

        // We no longer need this ptr
        MyGlobalUnlock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemWSID = NULL;
    } // End IF/ELSE

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End CmdWsid_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: sc_wsid.c
//***************************************************************************
