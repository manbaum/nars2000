//***************************************************************************
//  NARS2000 -- System Command:  )LOAD
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
//  $CmdLoad_EM
//
//  Execute the system command:  )LOAD wsid
//***************************************************************************

BOOL CmdLoad_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    WCHAR        wszTailDPFE[_MAX_PATH];// Save area for canonical form of given ws name
    FILE        *fStream;               // Ptr to file stream
    UINT         uSymVar,               // Var index counter
                 uSymFcn,               // Fcn index counter
                 uSymOpr;               // Opr index counter

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

    // Attempt to open the workspace
    fStream = _wfopen (wszTailDPFE, L"r");

    // If the workspace doesn't exist, ...
    if (fStream EQ NULL)
    {
        AppendLine (ERRMSG_WS_NOT_FOUND, FALSE, TRUE);

        goto ERROR_EXIT;
    } // End IF

    // We no longer need this handle
    fclose (fStream); fStream = NULL;

    // Get the Var, Fcn & Opr counts
    uSymVar =
      GetPrivateProfileIntW (L"General",        // Ptr to the section name
                             L"VarCount",       // Ptr to the key name
                             0,                 // Default value if not found
                             wszTailDPFE);      // Ptr to the file name
    uSymFcn =
      GetPrivateProfileIntW (L"General",        // Ptr to the section name
                             L"FcnCount",       // Ptr to the key name
                             0,                 // Default value if not found
                             wszTailDPFE);      // Ptr to the file name
    uSymOpr =
      GetPrivateProfileIntW (L"General",        // Ptr to the section name
                             L"OprCount",       // Ptr to the key name
                             0,                 // Default value if not found
                             wszTailDPFE);      // Ptr to the file name
    // Load the [Vars.0] section














    AppendLine (L"NONCE COMMAND", FALSE, TRUE);






ERROR_EXIT:
    return FALSE;
} // End CmdLoad_EM


//***************************************************************************
//  End of File: sc_load.c
//***************************************************************************
