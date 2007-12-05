//***************************************************************************
//  NARS2000 -- System Command:  )LIB
//***************************************************************************

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
//  $CmdLib_EM
//
//  Execute the system command:  )LIB [first][-][last]
//***************************************************************************

BOOL CmdLib_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    HANDLE           hFind;     // Handle to FindData
    WIN32_FIND_DATAW FindData;  // FindFirstFile return data struc
    UINT             uExtLen;   // Length of workspace extension

    // Create the wildcard string to search for workspaces
    lstrcpyW (lpwszTemp, wszSaveDir);
    lstrcatW (lpwszTemp, L"*");
    lstrcatW (lpwszTemp, WS_WKSEXT);

    // ***FIXME*** -- Make sensitive to [first][-][last] in lpwszTail
    // ***FIXME*** -- Display in columns

    // Get length of workspace extension
    uExtLen = lstrlenW (WS_WKSEXT);

    // Find the first (if any) workspace
    hFind = FindFirstFileW (lpwszTemp, &FindData);

    if (hFind NE INVALID_HANDLE_VALUE)
    {
        // Delete the workspace extension
        FindData.cFileName[lstrlenW (FindData.cFileName) - uExtLen] = L'\0';

        // Display the workspace name
        AppendLine (FindData.cFileName, FALSE, TRUE);

        // Continue looking
        while (FindNextFileW (hFind, &FindData))
        {
            // Delete the workspace extension
            FindData.cFileName[lstrlenW (FindData.cFileName) - uExtLen] = L'\0';

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
