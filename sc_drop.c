//***************************************************************************
//  NARS2000 -- System Command:  )DROP
//***************************************************************************

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <time.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "aplerrors.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CmdDrop_EM
//
//  Execute the system command:  )DROP wsid
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdDrop_EM"
#else
#define APPEND_NAME
#endif

BOOL CmdDrop_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    WCHAR wszTailDPFE[_MAX_PATH];   // Save area for canonical form of given ws name
    BOOL  bRet = FALSE;             // TRUE iff result is valid
    WCHAR wszTemp[19 + 1];          // Output save area

    // If there is a specified WS Name, ...
    if (*lpwszTail)
    {
        // Convert the given workspace name into a canonical form
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, wszSaveDir);

        // Append the common workspace extension
        lstrcatW (wszTailDPFE, WS_WKSEXT);

        // Attempt to open the workspace
        if (!_wremove (wszTailDPFE))
        {
            SYSTEMTIME SystemTime;

            // Display the current timne & date of the drop

            // Get the current time
            if (bUseLocalTime)
                GetLocalTime  (&SystemTime);
            else
                GetSystemTime (&SystemTime);

            wsprintfW (wszTemp,
                       L"%02d/%02d/%04d %2d:%02d:%02d",
                       SystemTime.wMonth,
                       SystemTime.wDay,
                       SystemTime.wYear,
                       SystemTime.wHour,
                       SystemTime.wMinute,
                       SystemTime.wSecond);

            AppendLine (wszTemp, FALSE, TRUE);

            // Mark as successful
            bRet = TRUE;
        } else
            AppendLine (ERRMSG_WS_NOT_FOUND, FALSE, TRUE);
    } else
        IncorrectCommand ();

    return bRet;
} // End CmdDrop_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: sc_drop.c
//***************************************************************************
