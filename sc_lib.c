//***************************************************************************
//  NARS2000 -- System Command:  )LIB
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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
//  $CmdLib_EM
//
//  Execute the system command:  )LIB [dir] [[first][-][last]]
//***************************************************************************

UBOOL CmdLib_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    LPPERTABDATA     lpMemPTD;  // Ptr to PerTabData global memory
    HANDLE           hFind;     // Handle to FindData
    WIN32_FIND_DATAW FindData;  // FindFirstFile return data struc
    UINT             uExtLen;   // Length of workspace extension
    LPWCHAR          lpw,       // Temporary ptr
                     lpwszTemp; // Ptr to temporary storage

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Get ptr to temporary storage
    lpwszTemp = lpMemPTD->lpwszTemp;

    // Skip to the next blank
    lpw = SkipToCharW (lpwszTail, L' ');

    // Zap it in case there are trailing blanks
    *lpw = WC_EOS;

    // If there's a command line tail, use it
    if (lpwszTail[0] NE WC_EOS)
    {
        lstrcpyW (lpwszTemp, lpwszTail);

        // Ensure there's a trailing backslash
        uExtLen = lstrlenW (lpwszTemp);
        if (lpwszTemp[uExtLen - 1] NE WC_SLOPE)
        {
            lpwszTemp[uExtLen + 0] = WC_SLOPE;
            lpwszTemp[uExtLen + 1] = WC_EOS;
        } // End IF
    } else
        lstrcpyW (lpwszTemp, lpwszWorkDir);

    // Create the wildcard string to search for workspaces
    lstrcatW (lpwszTemp, L"*");
    lstrcatW (lpwszTemp, WS_WKSEXT);

    // ***FIXME*** -- Make sensitive to [first][-][last] in lpwszTail
    // ***FIXME*** -- Display in columns

    // Get length of workspace extension
    uExtLen = strcountof (WS_WKSEXT);

    // Find the first (if any) workspace
    hFind = FindFirstFileW (lpwszTemp, &FindData);

    if (hFind NE INVALID_HANDLE_VALUE)
    {
        // Delete the workspace extension
        FindData.cFileName[lstrlenW (FindData.cFileName) - uExtLen] = WC_EOS;

        // Display the workspace name
        AppendLine (FindData.cFileName, FALSE, TRUE);

        // Continue looking
        while (FindNextFileW (hFind, &FindData))
        {
            // Delete the workspace extension
            FindData.cFileName[lstrlenW (FindData.cFileName) - uExtLen] = WC_EOS;

            // Display the workspace name
            AppendLine (FindData.cFileName, FALSE, TRUE);
        } // End WHILE

        // Close the find handle
        FindClose (hFind); hFind = NULL;

        return TRUE;
    } else
        return FALSE;
} // End CmdLib_EM


//***************************************************************************
//  End of File: sc_lib.c
//***************************************************************************
