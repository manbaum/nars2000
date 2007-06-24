//***************************************************************************
//  NARS2000 -- Save Workspace
//***************************************************************************

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <direct.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "aplerrors.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

// State Indicator level
int SILevel = 0;


//***************************************************************************
//  MakeWorkspaceNameCanonical
//
// Convert a workspace name into a canonical form
//***************************************************************************

void MakeWorkspaceNameCanonical
    (LPWCHAR wszOut,            // Output workspace name
     LPWCHAR wszInp,            // Input  ...
     LPWCHAR wszDefDir)         // Default drive and directory if no drive letter

{
    WCHAR wszDrive[_MAX_DRIVE], // Save area for drive letter
          wszDir[_MAX_DIR],     // ...           directory
          wszFname[_MAX_FNAME], // ...           filename
          wszExt[_MAX_EXT],     // ...           extension
          wszTmpDir[_MAX_DIR];  // ...           temporary directory

    // Split the name into its component parts
    _wsplitpath (wszInp, wszDrive, wszDir, wszFname, wszExt);

    // If there is no drive letter, use the default one
    if (wszDrive[0] EQ L'\0')
    {
        lstrcpyW (wszDrive, wszDefDrive);

        // If the directory is not at the root, prepend the default
        //   Save Workspace directory name
        if (wszDir[0] NE L'\\')
        {
            lstrcpyW (wszTmpDir, wszDefDir);
            lstrcatW (wszTmpDir, wszDir);
            wszDrive[0] = L'\0';        // Because wszSaveDir includes the drive letter
        } else
            lstrcpyW (wszTmpDir, wszDir);
    } // End IF

    // Put it back together into a DPFE
    _wmakepath (wszOut, wszDrive, wszTmpDir, wszFname, wszExt);
} //  End MakeWorkspaceNameCanonical


//***************************************************************************
//  $CmdSaveWS_EM
//
//  Execute the system command:  )SAVE
//***************************************************************************

BOOL CmdSaveWS_EM
    (LPWCHAR lpwszTail)     // Command line tail with NO leading blanks

{
    HGLOBAL      hGlbPTD,               // PerTabData global memory handle
                 hGlbName;              // STE name global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPVOID       lpMemWSID;             // Ptr to []WSID global memory
    WCHAR        wszTailDPFE[_MAX_PATH],// ...           canonical form of given ws name
                 wszWsidDPFE[_MAX_PATH],// ...           ...               []WSID
                 wszTempDPFE[_MAX_PATH];// ...           temporary
    LPWCHAR      lpSaveWSID,            // WSID to save to
                 lpMemName;             // Ptr to STE name
    APLNELM      aplNELMWSID;           // []WSID NELM
    APLRANK      aplRankWSID;           // ...    rank
    BOOL         bRet = FALSE;          // TRUE iff result is valid
    int          iCmp,                  // Comparison result
                 iSym;                  // Symbol table loop counter
    FILE        *fStream;               // Ptr to file stream

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Lock the memory to get a ptr to it
    lpMemWSID = MyGlobalLock (ClrPtrTypeDirGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData));

#define lpHeader        ((LPVARARRAY_HEADER) lpMemWSID)

    aplNELMWSID = lpHeader->NELM;
    aplRankWSID = lpHeader->Rank;

#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpSaveWSID = VarArrayBaseToData (lpMemWSID, aplRankWSID);

    // Because the global memory doesn't have a zero terminator,
    //   we must copy the data to a temporary location and then
    //   append a zero terminator
    lstrcpynW (wszTempDPFE, lpSaveWSID, (UINT) aplNELMWSID + 1);
////wszTempDPFE[aplNELMWSID] = L'\0';   // Already done via "+ 1" in lpstrcpynW

    // Convert the []WSID workspace name into a canonical form
    MakeWorkspaceNameCanonical (wszWsidDPFE, wszTempDPFE, wszSaveDir);

    // If there is a specified WS Name,
    //   and the current []WSID is different from the specified WS Name,
    //   and a WS with the specified WS Name already exists,
    //   display an error
    //     "NOT SAVED, THIS WS IS ",[]WSID
    if (*lpwszTail)
    {
        // Convert the given workspace name into a canonical form
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, wszSaveDir);

        // Compare the workspace names (without their extensions)
        iCmp = lstrcmpiW (wszWsidDPFE, wszTailDPFE);

        // Append the common workspace extension
        lstrcatW (wszTailDPFE, WS_WKSEXT);

        // If unequal and the given named ws already exists,
        //   display an error
        if (iCmp NE 0)
        {
            // Attempt to open the workspace
            fStream = _wfopen (wszTailDPFE, L"r");

            // If it already exists, display an error
            if (fStream NE NULL)
            {
                // We no longer need this handle
                fclose (fStream); fStream = NULL;

                // Copy the leading part of the message
                lstrcpyW (lpwszTemp, ERRMSG_NOT_SAVED);

                // Followed by the actual []WSID
                if (aplNELMWSID EQ 0)
                    lstrcpyW  (&lpwszTemp[lstrlenW (lpwszTemp)], L"CLEAR WS");
                else
                    lstrcpynW (&lpwszTemp[lstrlenW (lpwszTemp)], lpSaveWSID, (UINT) aplNELMWSID + 1);

                AppendLine (lpwszTemp, FALSE, TRUE);

                goto ERROR_EXIT;
            } // End IF
        } // End IF

        // Save as the WSID to save to
        lpSaveWSID = wszTailDPFE;
    } else
    // If the current []WSID is empty, display an error
    //     "NOT SAVED, THIS WS IS CLEAR WS"
    {
        // If []WSID is empty
        if (aplNELMWSID EQ 0)
        {
            AppendLine (ERRMSG_NOT_SAVED_CLEAR_WS, FALSE, TRUE);

            goto ERROR_EXIT;
        } // End IF

        // Append the common workspace extension
        lstrcatW (wszWsidDPFE, WS_WKSEXT);

        // Save as the WSID to save to
        lpSaveWSID = wszWsidDPFE;
    } // End IF/ELSE

    // The full workspace name to save to is in lpSaveWSID

    // The format of a saved ws is as an .ini file
    //   (named *.ws.nars) with the following sections:
    //   [General]    (General settings)
    //   [Fns.0]      (Global functions)
    //   [Fns.1]      (Functions at level 1 in the SI stack)
    //   [Fns.nnn]    (Functions at level nnn in the SI stack)
    //   [Vars.0]     (Global variables)
    //   [Vars.1]     (Variables at level 1 in the SI stack)
    //   [Vars.nnn]   (Variables at level nnn in the SI stack)
    // Each of the above sections has content of
    //   1          = Name      (1st name)
    //   2          = Name      (2nd name)
    //   xxx        = Name      (xxxth name)
    // Corresponding to each of the above names, there is a
    //   separate section [nnn.Name], for example,
    //   [0.Name]     (The contents of the name "Name" at level 0)

    // Create (or truncate the file)
    fStream = _wfopen (wszTailDPFE, L"w");
    fclose (fStream); fStream = NULL;

    // Write out the [General] section
    WritePrivateProfileStringW (L"General",     // Section name
                                L"Version",     // Key name
                                L"0.01",        // Key value
                                wszTailDPFE);   // File name

    DbgBrk ();          // ***FINISHME***

    // Trundle through the Symbol Table
    for (iSym = 0; iSym < lpMemPTD->iSymTabTotalSize; iSym++)
    {
        LPSYMENTRY lpSymEntry;

        // Save the LPSYMENTRY
        lpSymEntry = &lpMemPTD->lpSymTab[iSym];

        if (lpSymEntry->stFlags.Inuse)
        {
            if (lpSymEntry->stFlags.SysVar          // Save the current value
             || lpSymEntry->stFlags.UsrVar)
            {
                DbgBrk ();

                // Get the symbol name's global memory handle
                hGlbName = lpSymEntry->stHshEntry->hGlbName;

                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (hGlbName);

                // Format the name as an ASCII string with non-ASCII chars
                //   represented as \XXXX where XXX is a four-digit hex number





                // We no longer need this ptr
                MyGlobalUnlock (hGlbName); lpMemName = NULL;
            } else
            if (lpSymEntry->stFlags.UsrFn0)
            {



            } else
            if (lpSymEntry->stFlags.UsrFn12)
            {



            } else
            if (lpSymEntry->stFlags.UsrOp1)
            {



            } else
            if (lpSymEntry->stFlags.UsrOp2)
            {



            } // End IF/ELSE/...




        } // End FOR/IF
    } // End FOR/IF



















    bRet = TRUE;
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (ClrPtrTypeDirGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemWSID = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End CmdSaveWS_EM


//***************************************************************************
//  End of File: savews.c
//***************************************************************************
