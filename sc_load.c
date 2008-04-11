//***************************************************************************
//  NARS2000 -- System Command:  )LOAD
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
//  $CmdLoad_EM
//
//  Execute the system command:  )LOAD wsid
//***************************************************************************

BOOL CmdLoad_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    HGLOBAL      hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    WCHAR        wszTailDPFE[_MAX_PATH];// Save area for canonical form of given ws name
    int          iTabIndex;             // Tab index

    // If there's no WSID, that's an error
    if (lpwszTail[0] EQ L'\0')
    {
        IncorrectCommand ();

        return FALSE;
    } // End IF

    // Convert the given workspace name into a canonical form (without WKSEXT)
    MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, wszSaveDir);

    // Append the common workspace extension
    lstrcatW (wszTailDPFE, WS_WKSEXT);

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the tab index from which this command was issued
    iTabIndex = lpMemPTD->TabIndex;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return
      CreateNewTab (hWndMF,             // Parent window handle
                    wszTailDPFE,        // Drive, Path, Filename, Ext of the workspace
                    iTabIndex + 1);     // Insert new tab to the left of this one
} // End CmdLoad_EM


//***************************************************************************
//  $LoadWorkspace
//
//  Load a workspace
//***************************************************************************

BOOL LoadWorkspace
    (LPWCHAR lpwszDPFE)                 // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)

{
    FILE *fStream;                      // Ptr to file stream for the plain text workspace file
    UINT  uSymVar,                      // Var index counter
          uSymFcn,                      // Fcn/Opr index counter
          uStr;                         // Loop counter
    WCHAR wszCount[8];                  // Output save area for formatted uSymVar/Fcn counter

    // Check for CLEAR WS
    if (lpwszDPFE[0] EQ L'\0')
        goto WSID_EXIT;

    // Attempt to open the workspace
    fStream = _wfopen (lpwszDPFE, L"r");

    // If the workspace doesn't exist, ...
    if (fStream EQ NULL)
    {
        AppendLine (ERRMSG_WS_NOT_FOUND, FALSE, TRUE);

        goto ERROR_EXIT;
    } // End IF

    // We no longer need this handle
    fclose (fStream); fStream = NULL;

    // Get the Var & Fcn/Opr counts
    uSymVar =
      GetPrivateProfileIntW (L"General",        // Ptr to the section name
                             L"VarCount",       // Ptr to the key name
                             0,                 // Default value if not found
                             lpwszDPFE);        // Ptr to the file name
    uSymFcn =
      GetPrivateProfileIntW (L"General",        // Ptr to the section name
                             L"FcnCount",       // Ptr to the key name
                             0,                 // Default value if not found
                             lpwszDPFE);        // Ptr to the file name
    // Load the [Globals] section
    for (uStr = 0; ; uStr++)
    {
        // Format the counter
        wsprintfW (wszCount, L"%d", uStr);

        // Read the next string
        GetPrivateProfileStringW (L"Globals",
                                  wszCount,         // Ptr to the key name
                                  L"",              // Ptr to the default value
                                  lpwszTemp,        // Ptr to the output buffer
                                  memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                  lpwszDPFE);       // Ptr to the file name
        // Parse the array attributes





    } // End FOR
















    AppendLine (L"NONCE COMMAND", FALSE, TRUE);





WSID_EXIT:
    // Set the value of the new []WSID as lpwszDPFE
    if (SaveNewWsid (lpwszDPFE))
        return TRUE;
ERROR_EXIT:
    return FALSE;
} // End LoadWorkspace


//***************************************************************************
//  End of File: sc_load.c
//***************************************************************************
