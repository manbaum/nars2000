//***************************************************************************
//  NARS2000 -- System Command Routines
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

typedef BOOL (SYSCMD) (LPWCHAR);
typedef SYSCMD *LPSYSCMD;

typedef struct tagSYSCMDSTAB
{
    LPWCHAR  lpszName;
    LPSYSCMD lpSysCmd_EM;
} SYSCMDSTAB, *LPSYSCMDSTAB;

// The lookup table for all system commands
SYSCMDSTAB SysCmdsTab[]
#ifndef PROTO
 =
{
    {L"clear" ,     &CmdClear_EM    },
    {L"close" ,     &CmdClose_EM    },
////{L"copy"  ,     &CmdCopy_EM     },
    {L"drop"  ,     &CmdDrop_EM     },
    {L"edit"  ,     &CmdEdit_EM     },
    {L"erase" ,     &CmdErase_EM    },
    {L"exit"  ,     &CmdExit_EM     },
    {L"fns"   ,     &CmdFns_EM      },
    {L"lib"   ,     &CmdLib_EM      },
////{L"load"  ,     &CmdLoadWS_EM   },
    {L"newtab",     &CmdNewTab_EM   },
    {L"nms"   ,     &CmdNms_EM      },
    {L"ops"   ,     &CmdOps_EM      },
    {L"reset" ,     &CmdReset_EM    },
    {L"save"  ,     &CmdSave_EM     },
    {L"si"    ,     &CmdSi_EM       },
    {L"sic"   ,     &CmdReset_EM    },
    {L"sinl"  ,     &CmdSinl_EM     },
    {L"vars"  ,     &CmdVars_EM     },
////{L"xload" ,     &CmdXload_EM    },
    {L"wsid"  ,     &CmdWsid_EM     },
}
#endif
;

// The # rows in the above table
#define SYSCMDSTAB_NROWS    (sizeof (SysCmdsTab) / sizeof (SysCmdsTab[0]))


//***************************************************************************
//  $IncorrectCommand
//
//  Signal an incorrect command
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- IncorrectCommand"
#else
#define APPEND_NAME
#endif

void IncorrectCommand
    (void)

{
    AppendLine (ERRMSG_INCORRECT_COMMAND APPEND_NAME, FALSE, TRUE);
} // End IncorrectCommand
#undef  APPEND_NAME


//***************************************************************************
//  $ExecSysCmd
//
//  Execute a system command
//***************************************************************************

BOOL ExecSysCmd
    (LPWCHAR lpwszLine)

{
    int     i;
    LPWCHAR wp;

    // Skip over the leading ')' and any following spaces
    while (*++lpwszLine EQ L' ');

    // Look for the next space as a separator
    wp = strchrW (lpwszLine, L' ');

    // If not found, goto the end of the line
    if (wp EQ NULL)
        wp = &lpwszLine[lstrlenW (lpwszLine)];
    else
    {
        // Make the command into a zero-terminated string
        *wp++ = L'\0';

        // Otherwise, skip over all leading spaces
        while (*wp EQ L' ')
            wp++;
    } // End IF/ELSE

    // Search for this command in the table
    for (i = 0; i < SYSCMDSTAB_NROWS; i++)
    if (lstrcmpiW (SysCmdsTab[i].lpszName, lpwszLine) EQ 0)
        return (*SysCmdsTab[i].lpSysCmd_EM) (wp);

    IncorrectCommand ();

    return FALSE;
} // End ExecSysCmd


//***************************************************************************
//  $CmdClear_EM
//
//  Execute the system command:  )CLEAR
//***************************************************************************

BOOL CmdClear_EM
    (LPWCHAR lpwszTail)

{
    if (!bNewTabOnClear)
        CloseTab (TabCtrl_GetCurSel (hWndTC));

    return CreateNewTab (hWndMF,
                         "CLEAR WS",
                         TabCtrl_GetItemCount (hWndTC));
} // End CmdClear_EM


//***************************************************************************
//  $CmdClose_EM
//
//  Execute the system command:  )CLOSE
//***************************************************************************

BOOL CmdClose_EM
    (LPWCHAR lpwszTail)

{
    if (TabCtrl_GetItemCount (hWndTC) EQ 1)
    {
        AppendLine (L"Can't close last workspace -- use )EXIT", FALSE, TRUE);

        return FALSE;
    } // End IF

    return CloseTab (TabCtrl_GetCurSel (hWndTC));
} // End CmdClose_EM


//***************************************************************************
//  $CmdEdit_EM
//
//  Execute the system command:  )EDIT
//***************************************************************************

BOOL CmdEdit_EM
    (LPWCHAR lpwszTail)

{
    // ***FIXME*** -- Make sensitive to user option to edit a
    //                function, vector, or matrix by default

    // Open a Function Editor window
    return CreateFcnWindow (lpwszTail);
} // End CmdEdit_EM


//***************************************************************************
//  $CmdExit_EM
//
//  Execute the system command:  )EXIT
//***************************************************************************

BOOL CmdExit_EM
    (LPWCHAR lpwszTail)

{
    PostMessage (hWndMF, WM_CLOSE, 0, 0);
    return TRUE;
} // End CmdExit_EM


//***************************************************************************
//  $CmdNewTab_EM
//
//  Execute the system command:  )NEWTAB
//***************************************************************************

BOOL CmdNewTab_EM
    (LPWCHAR lpwszTail)

{
    return CreateNewTab (hWndMF,
                         "CLEAR WS",
                         TabCtrl_GetItemCount (hWndTC));
} // End CmdNewTab_EM


//***************************************************************************
//  End of File: syscmds.c
//***************************************************************************
