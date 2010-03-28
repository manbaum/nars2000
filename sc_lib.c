//***************************************************************************
//  NARS2000 -- System Command:  )LIB
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
//  $CmdLib_EM
//
//  Execute the system command:  )LIB [dir] [[first][-][last]]
//***************************************************************************

UBOOL CmdLib_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    LPPERTABDATA     lpMemPTD;          // Ptr to PerTabData global memory
    HANDLE           hFind;             // Handle to FindData
    WIN32_FIND_DATAW FindData;          // FindFirstFile return data struc
    UINT             uExtLen;           // Length of workspace extension
    LPWCHAR          lpw,               // Temporary ptr
                     lpwszTemp,         // Ptr to temporary storage
                     lpwszFormat;       // ...
    UBOOL            bEndDQ = FALSE;    // TRUE iff the string ends with a Double Quote

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary storage
    lpwszTemp   = lpMemPTD->lpwszTemp;
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Skip to the next blank
    lpw = SkipToCharDQW (lpwszTail, L' ');

    // Zap it in case there are trailing blanks
    *lpw = WC_EOS;

    // If there's a command line tail, use it
    if (lpwszTail[0] NE WC_EOS)
    {
        lstrcpyW (lpwszFormat, lpwszTail);

        // Get the length to test for trailing backslash
        uExtLen = lstrlenW (lpwszFormat);

        // If it ends in a double quote, ...
        bEndDQ = (lpwszFormat[uExtLen - 1] EQ WC_DQ);

        // Ensure there's a trailing backslash
        if (lpwszFormat[uExtLen - 1 - bEndDQ] NE WC_SLOPE)
        {
            lpwszFormat[uExtLen + 0 - bEndDQ] = WC_SLOPE;
            lpwszFormat[uExtLen + 1 - bEndDQ] = WC_EOS;

            // If it ended with a double quote, ...
            if (bEndDQ)
            {
                // Append it again
                lpwszFormat[uExtLen + 0] = WC_DQ;
                lpwszFormat[uExtLen + 1] = WC_EOS;
            } // End IF
        } // End IF

        // Convert the given workspace name into a canonical form (without WS_WKSEXT)
        MakeWorkspaceNameCanonical (lpwszTemp, lpwszFormat, lpwszWorkDir);
    } else
        lstrcpyW (lpwszTemp, lpwszWorkDir);

    // Get the length to append trailing *.ws.nars
    uExtLen = lstrlenW (lpwszTemp);

    // If it ends in a double quote, ...
    bEndDQ = (lpwszTemp[uExtLen - 1] EQ WC_DQ);

    // Create the wildcard string to search for workspaces
    lstrcpyW (&lpwszTemp[uExtLen - bEndDQ], L"*");
    uExtLen++;
    lstrcpyW (&lpwszTemp[uExtLen - bEndDQ], WS_WKSEXT);
    uExtLen += strcountof (WS_WKSEXT);

    // If it ended with a double quote, ...
    if (bEndDQ)
    {
        // Append it again
        lpwszTemp[uExtLen + 0] = WC_DQ;
        lpwszTemp[uExtLen + 1] = WC_EOS;
    } // End IF

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
