//***************************************************************************
//  NARS2000 -- System Command Common Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
#include "headers.h"


//***************************************************************************
//  $MakeWorkspaceNameCanonical
//
// Convert a workspace name into a canonical form (without WS_WKSEXT)
//***************************************************************************

void MakeWorkspaceNameCanonical
    (LPWCHAR wszOut,            // Output workspace name
     LPWCHAR wszInp,            // Input  ...
     LPWCHAR wszDefDir)         // Default drive and directory if no drive letter

{
    UINT uLen;

    // If the incoming workspace name begins with a double-quote, skip over it
    if (wszInp[0] EQ WC_DQ)
        wszInp++;

    // Get the incoming workspace name string length
    uLen = lstrlenW (wszInp);

    // If the incoming workspace name ends with a double-quote, delete it
    if (uLen && wszInp[uLen - 1] EQ WC_DQ)
        wszInp[uLen - 1] = WC_EOS;

    // If the name doesn't begin with a drive letter and
    //   doesn't start at the root or a dot, prepend the
    //   default dir
    if (wszInp[0] NE WC_EOS     // Non-empty,
     && wszInp[0] NE L'.'       // and not current dir,
     && wszInp[0] NE WC_SLOPE   // and not root dir,
     && wszInp[1] NE L':')      // and no drive letter
    {
        lstrcpyW (wszOut, wszDefDir);
        lstrcatW (wszOut, wszInp);
    } else
        lstrcpyW (wszOut, wszInp);

    // Get the outgoing workspace name string length
    uLen = lstrlenW (wszOut);

    // If the workspace name is long enough and
    //   ends with WSKEXT
    if (uLen >= WS_WKSEXT_LEN
     && lstrcmpiW (&wszOut[uLen - WS_WKSEXT_LEN], WS_WKSEXT) EQ 0)
        wszOut[uLen - WS_WKSEXT_LEN] = WC_EOS;
} //  End MakeWorkspaceNameCanonical


//***************************************************************************
//  $DisplayWorkspaceStamp
//
//  Display the workspace timestamp
//***************************************************************************

void DisplayWorkspaceStamp
    (LPWCHAR lpwszDPFE)                 // Workspace filenam

{
#define TIMESTAMP_FMT L"SAVED MM/DD/YYYY hh:mm:ss"

    WCHAR      wszTimeStamp[16 + 1],    // Output save area for time stamp
               wszTimeStamp2[16 + 1],   // Output save area for time stamp
               wszTemp[1 + countof (TIMESTAMP_FMT)];
    FILETIME   ftCreation,              // Function creation time in UTC
               ftLocalTime;             // ...                       localtime
    SYSTEMTIME systemTime;              // Current system (UTC) time

    // Get the current system (UTC) time
    GetSystemTime (&systemTime);

    // Convert system time to file time and save as creation time
    SystemTimeToFileTime (&systemTime, &ftCreation);

    // Format the creation time
    wsprintfW (wszTimeStamp2,
               FMTSTR_DATETIME,
               ftCreation.dwHighDateTime,
               ftCreation.dwLowDateTime);
    // Read the creation time
    GetPrivateProfileStringW (SECTNAME_GENERAL,     // Ptr to the section name
                              KEYNAME_CREATIONTIME, // Ptr to the key name
                              wszTimeStamp2,        // Ptr to the default value
                              wszTimeStamp,         // Ptr to the output buffer
                              sizeof (wszTimeStamp),// Byte size of the output buffer
                              lpwszDPFE);           // Ptr to the file name
    // Convert the CreationTime string to time
    swscanf (wszTimeStamp, SCANFSTR_TIMESTAMP, &ftCreation);

    if (OptionFlags.bUseLocalTime)
        // Convert to local filetime
        FileTimeToLocalFileTime (&ftCreation, &ftLocalTime);
    else
        ftLocalTime = ftCreation;

    // Convert the creation time to system time so we can display it
    FileTimeToSystemTime (&ftLocalTime, &systemTime);

    lstrcpyW (wszTemp, L"SAVED ");

    // Format it
    wsprintfW (wszTemp + lstrlenW (wszTemp),
               DATETIME_FMT L"%s",
               systemTime.wMonth,
               systemTime.wDay,
               systemTime.wYear,
               systemTime.wHour,
               systemTime.wMinute,
               systemTime.wSecond,
               OptionFlags.bUseLocalTime ? L"" : L" (GMT)");
    // Display it
    AppendLine (wszTemp, FALSE, TRUE);
} // End DisplayWorkspaceStamp


//***************************************************************************
//  $SendMessageLastTab
//
//  Send a (constant) message to the previously outgoing tab
//***************************************************************************

void SendMessageLastTab
    (const LPWCHAR lpwErrMsg,           // Ptr to constant error message text
     LPPERTABDATA  lpMemPTD)            // Ptr to PerTabData global memory

{
    int iPrvTabIndex;                   // Index of previous tab

    // Get the index of the tab from which we were )LOADed
    iPrvTabIndex = TranslateTabIDToIndex (lpMemPTD->PrvTabID);

    if (iPrvTabIndex NE -1)
    {
        // Get the PerTabData global memory ptr for the preceding tab
        lpMemPTD = GetPerTabPtr (iPrvTabIndex); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

        // Send this error message to the previous tab's SM
        SendMessageW (lpMemPTD->hWndSM, MYWM_ERRMSG, 0, (LPARAM) lpwErrMsg);
    } // End IF
} // End SendMessageLastTab


//***************************************************************************
//  $MakeWorkspaceBackup
//
//  Save a backup copy of the workspace we're about to load
//***************************************************************************

void MakeWorkspaceBackup
    (LPWCHAR lpwszDPFE,                 // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
     LPWCHAR lpwExtType)                // LOADBAK_EXT or SAVEBAK_EXT

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPWCHAR      lpwszTemp;             // Ptr to temporary storage
    APLUINT      uLen;                  // Temporary length
    FILE        *fStream = NULL,        // Ptr to file stream for the plain text workspace file
                *fBackup;               // Ptr to file stream for the backup file

    // Attempt to open the workspace
    fStream = _wfopen (lpwszDPFE, L"r");

    // If the workspace doesn't exist, ...
    if (fStream EQ NULL)
        goto WSNOTFOUND_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get a ptr to a temporary save area
    lpwszTemp = lpMemPTD->lpwszTemp;

    // The workspace name is of the form d:\path\to\workspace\filename.ws.nars
    // The backup    ...                 d:\path\to\workspace\filename.bak.ws.nars

    // Copy the original name
    lstrcpyW (lpwszTemp, lpwszDPFE);

    // Get the entire length less WS_WKSEXT
    uLen = lstrlenW (lpwszTemp) - WS_WKSEXT_LEN;

    // Append new extensions
    lstrcpyW (&lpwszTemp[uLen], lpwExtType);

    fBackup = _wfopen (lpwszTemp, L"wb");
    if (fBackup EQ NULL)
        goto NOT_OPENED_EXIT;

    // Copy the workspace to its backup
    while (feof (fStream) EQ 0
        && ferror (fBackup) EQ 0)
    {
        uLen = (APLU3264) fread  (lpwszTemp, 1, DEF_WPTDTEMP_INITNELM * sizeof (WCHAR), fStream);
                          fwrite (lpwszTemp, 1, (APLU3264) uLen                       , fBackup);
    } // End WHILE

    // We no longer need this handle
    fclose (fStream); fStream = NULL;

    uLen = ferror (fBackup);

    // We no longer need this handle
    fclose (fBackup); fBackup = NULL;

    if (uLen NE 0)
        AppendLine (L"WORKSPACE BACKUP NOT COMPLETE", FALSE, TRUE);

    goto NORMAL_EXIT;

NOT_OPENED_EXIT:
    AppendLine (L"WORKSPACE BACKUP NOT OPENED", FALSE, TRUE);

    goto ERROR_EXIT;

WSNOTFOUND_EXIT:
ERROR_EXIT:
NORMAL_EXIT:
    if (fStream)
    {
        // We no longer need this handle
        fclose (fStream); fStream = NULL;
    } // End IF
} // End MakeWorkspaceBackup


//***************************************************************************
//  $SaveNewWsid_EM
//
//  Save a new []WSID
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SaveNewWsid_EM"
#else
#define APPEND_NAME
#endif

UBOOL SaveNewWsid_EM
    (LPAPLCHAR lpMemSaveWSID)           // Ptr to []WSID to save (includes WS_WKSEXT)

{
    HGLOBAL      hGlbWSID;              // []WSID global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    int          iLen,                  // Length of []WSID (with WS_WKSEXT)
                 iLen2;                 // ...              (w/o  ...)
    APLUINT      ByteWSID;              // # bytes in the []WSID
    LPAPLCHAR    lpMemNewWSID;          // Ptr to new []WSID global memory
    UBOOL        bRet = FALSE;          // TRUE iff result is valid

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the length of the []WSID (including WS_WKSEXT)
    iLen = lstrlenW (lpMemSaveWSID);

    // If the []WSID is non-empty, ...
    if (iLen)
    {
        // Omit the trailing WS_WKSEXT
        iLen2 = iLen - WS_WKSEXT_LEN;
        Assert (lpMemSaveWSID[iLen2] EQ L'.');
        lpMemSaveWSID[iLen2] = WC_EOS;

        // Calculate space needed for the new WSID
        ByteWSID = CalcArraySize (ARRAY_CHAR, iLen2, 1);

        // Allocate space for the new WSID
        // N.B. Conversion from APLUINT to UINT.
        if (ByteWSID NE (APLU3264) ByteWSID)
            goto WSFULL_EXIT;
        hGlbWSID = DbgGlobalAlloc (GHND, (APLU3264) ByteWSID);
        if (!hGlbWSID)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemNewWSID = MyGlobalLock (hGlbWSID);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemNewWSID)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = iLen2;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemNewWSID) = iLen2;

        // Skip over the header and dimensions to the data
        lpMemNewWSID = VarArrayBaseToData (lpMemNewWSID, 1);

        // Copy data to the new []WSID
        CopyMemoryW (lpMemNewWSID, lpMemSaveWSID, iLen2);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbWSID); lpMemNewWSID = NULL;
    } else
        hGlbWSID = hGlbV0Char;

    // Free the old []WSID
    FreeResultGlobalVar (lpMemPTD->htsPTD.lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemPTD->htsPTD.lpSymQuad[SYSVAR_WSID]->stData.stGlbData = NULL;

    // Save the new []WSID
    lpMemPTD->htsPTD.lpSymQuad[SYSVAR_WSID]->stData.stGlbData = MakePtrTypeGlb (hGlbWSID);

    // Tell the Tab Ctrl about the new workspace name
    NewTabName ();

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Display the error message
    ReplaceLastLineCRPmt (ERRMSG_NOT_SAVED_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (iLen)
        // Restore zapped char
        lpMemSaveWSID[iLen2] = L'.';

    return bRet;
} // End SaveNewWsid_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ShortenWSID
//
//  Shorten a WSID if it's in the default workspace dir
//***************************************************************************

LPWCHAR ShortenWSID
    (LPWCHAR lpMemWSID)

{
    UINT uLen;

    // Get the length of the default workspace dir
    uLen = lstrlenW (lpwszWorkDir);

    // If the workspace is saved into the default location,
    //   omit the leading portion of the text
    if (wcsncmp (lpwszWorkDir, lpMemWSID, uLen) EQ 0)
        return &lpMemWSID[uLen];
    else
        return lpMemWSID;
} // End ShortenWSID


//***************************************************************************
//  End of File: sc_common.c
//***************************************************************************
