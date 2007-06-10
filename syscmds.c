//***************************************************************************
//  NARS2000 -- System Command Routines
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

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
////{L"copy"  ,     &CmdCopyName_EM },
////{L"drop"  ,     &CmdDropWS_EM   },
    {L"edit"  ,     &CmdEdit_EM     },
////{L"erase" ,     &CmdEraseName_EM},
    {L"exit"  ,     &CmdExit_EM     },
////{L"fns"   ,     &CmdFns_EM      },
////{L"lib"   ,     &CmdLib_EM      },
////{L"load"  ,     &CmdLoadWS_EM   },
    {L"newtab",     &CmdNewTab_EM   },
////{L"nms"   ,     &CmdNms_EM      },
////{L"ops"   ,     &CmdOps_EM      },
    {L"save"  ,     &CmdSaveWS_EM   },
////{L"si"    ,     &CmdSi_EM       },
////{L"sic"   ,     &CmdSic_EM      },
////{L"sinl"  ,     &CmdSinl_EM     },
////{L"vars"  ,     &CmdVars_EM     },
////{L"xload" ,     &CmdXloadWS_EM  },
////{L"wsid"  ,     &CmdWsid_EM     },
}
#endif
;


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
    for (i = 0; i < (sizeof (SysCmdsTab) / sizeof (SysCmdsTab[0])); i++)
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
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet;          // TRUE iff result is valid

    // ***FIXME*** -- Make sensitive to user option to edit a
    //                function, vector, or matrix by default

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Open a Function Editor window
    bRet = CreateFcnWindow (lpwszTail, lpMemPTD->hWndMC);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
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
//  $CmdSaveWS_EM
//
//  Execute the system command:  )SAVE
//***************************************************************************

BOOL CmdSaveWS_EM
    (LPWCHAR lpwszTail)

{
    DbgBrk ();          // ***FINISHME***









    return FALSE;
} // End CmdSaveWS_EM


//***************************************************************************
//  End of File: syscmds.c
//***************************************************************************
