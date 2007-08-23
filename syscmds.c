//***************************************************************************
//  NARS2000 -- System Command Routines
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "dfnhdr.h"
#include "sis.h"

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
    {L"drop"  ,     &CmdDropWS_EM   },
    {L"edit"  ,     &CmdEdit_EM     },
////{L"erase" ,     &CmdEraseName_EM},
    {L"exit"  ,     &CmdExit_EM     },
    {L"fns"   ,     &CmdFns_EM      },
////{L"lib"   ,     &CmdLib_EM      },
////{L"load"  ,     &CmdLoadWS_EM   },
    {L"newtab",     &CmdNewTab_EM   },
////{L"nms"   ,     &CmdNms_EM      },
    {L"ops"   ,     &CmdOps_EM      },
    {L"reset" ,     &CmdReset_EM    },
    {L"save"  ,     &CmdSaveWS_EM   },
    {L"si"    ,     &CmdSi_EM       },
////{L"sic"   ,     &CmdSic_EM      },
////{L"sinl"  ,     &CmdSinl_EM     },
    {L"vars"  ,     &CmdVars_EM     },
////{L"xload" ,     &CmdXloadWS_EM  },
////{L"wsid"  ,     &CmdWsid_EM     },
}
#endif
;

// The # rows in the above table
#define SYSCMDSTAB_NROWS    (sizeof (SysCmdsTab) / sizeof (SysCmdsTab[0]))


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
//  $CmdFns_EM
//
//  Execute system command:  )FNS
//***************************************************************************

BOOL CmdFns_EM
    (LPWCHAR lpwszTail)

{
    return CmdFnsVarsOps_EM (lpwszTail, IzitFNS);
} // End CmdFns_EM


//***************************************************************************
//  $IzitFNS
//
//  Return TRUE iff the object type is that of a function
//***************************************************************************

BOOL IzitFNS
    (UINT ObjType)

{
    return IsNameTypeFn (ObjType);
} // End IzitFNS


//***************************************************************************
//  $CmdVars_EM
//
//  Execute system command:  )VARS
//***************************************************************************

BOOL CmdVars_EM
    (LPWCHAR lpwszTail)

{
    return CmdFnsVarsOps_EM (lpwszTail, IzitVARS);
} // End CmdVars_EM


//***************************************************************************
//  $IzitVARS
//
//  Return TRUE iff the object type is that of a variable
//***************************************************************************

BOOL IzitVARS
    (UINT ObjType)

{
    return IsNameTypeVar (ObjType);
} // End IzitVARS


//***************************************************************************
//  $CmdOps_EM
//
//  Execute system command:  )OPS
//***************************************************************************

BOOL CmdOps_EM
    (LPWCHAR lpwszTail)

{
    return CmdFnsVarsOps_EM (lpwszTail, IzitOPS);
} // End CmdOps_EM


//***************************************************************************
//  $IzitOPS
//
//  Return TRUE iff the object type is that of an operator
//***************************************************************************

BOOL IzitOPS
    (UINT ObjType)

{
    return IsNameTypeOp (ObjType);
} // End IzitOPS


//***************************************************************************
//  $CmdFnsVarsOps_EM
//
//  Execute system command:  )FNS or )VARS or )OPS
//***************************************************************************

BOOL CmdFnsVarsOps_EM
    (LPWCHAR lpwszTail,
     BOOL (*IzitFVO) (UINT))

{
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPSYMENTRY   lpSymEntry;        // Ptr to current SYMENTRY
    LPAPLCHAR    lpMemName;         // Ptr to name global memory
    HGLOBAL     *lphGlbSort;        // Ptr to HGLOBALs for sorting
    UINT         uSymCnt,           // Count of # matching STEs
                 uSymNum,           // Loop counter
                 uLineChar,         // Current char # in output line
                 uNameLen,          // Length of the current name
                 uQuadPW,           // Current value of []PW
                 uMaxNameLen = 0;   // Length of longest name
    BOOL         bLineCont;         // TRUE iff this line is a continuation

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Initialize the LPSYMENTRY sort array
    lphGlbSort = (HGLOBAL *) lpwszTemp;

    // Trundle through the Symbol Table
    //   looking for functions (niladic or monadic/dyadic)
    for (lpSymEntry = lpMemPTD->lpSymTab, uSymCnt = 0;
         lpSymEntry < lpMemPTD->lpSymTabNext;
         lpSymEntry++)
    if (lpSymEntry->stFlags.Inuse
     && lpSymEntry->stFlags.Value
     && lpSymEntry->stFlags.ObjName NE OBJNAME_SYS
     && (*IzitFVO) (lpSymEntry->stFlags.ObjType))
    {
        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (lpSymEntry->stHshEntry->htGlbName);

        // Find the longest name
        uMaxNameLen = max (uMaxNameLen, (UINT) lstrlenW (lpMemName));

        // We no longer need this ptr
        MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

        // Save the LPSYMENTRY ptr for later use
        lphGlbSort[uSymCnt] = lpSymEntry->stHshEntry->htGlbName;

        // Count in another matching name
        uSymCnt++;
    } // End FOR/IF

    // Sort the HGLOBALs
    ShellSort (lphGlbSort, uSymCnt, CmpHGLOBAL);

    // Get the current value of []PW
    uQuadPW = (UINT) (lpMemPTD->lpSymQuadPW->stData.stInteger);

    // Re-initialize the output area
    for (uLineChar = 0; uLineChar < uQuadPW; uLineChar++)
        lpwszFormat[uLineChar] = L' ';

    // Display the names
    for (bLineCont = FALSE, uLineChar = uSymNum = 0;
         uSymNum < uSymCnt;
         uSymNum++)
    {
        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (lphGlbSort[uSymNum]);

        uNameLen = lstrlenW (lpMemName);

        // If the line is too long, skip to the next one
        if ((uLineChar + uNameLen) > uQuadPW
         && uLineChar NE 0)
        {
            // Ensure properly terminated
            lpwszFormat[uLineChar] = L'\0';

            // Output the current line
            AppendLine (lpwszFormat, bLineCont, TRUE);

            // Mark all lines from here on as continuations
            bLineCont = TRUE;

            // Skip to the next line
            AppendLine (L"", TRUE, TRUE);

            // Re-initialize the output area
            for (uLineChar = 0; uLineChar < uQuadPW; uLineChar++)
                lpwszFormat[uLineChar] = L' ';

            // Re-initialize the char counter
            uLineChar = 0;
        } // End IF

        // Copy the name to the output area
        CopyMemory (&lpwszFormat[uLineChar], lpMemName, uNameLen * sizeof (APLCHAR));

        // Skip to the next name boundary
        uLineChar += uMaxNameLen + 1;

        // We no longer need this ptr
        MyGlobalUnlock (lphGlbSort[uSymNum]); lpMemName = NULL;
    } // End FOR

    // If there's still text in the buffer, output it
    if (uLineChar NE 0)
    {
        // Ensure properly terminated
        lpwszFormat[uLineChar] = L'\0';

        // Output the current line
        AppendLine (lpwszFormat, TRUE, TRUE);
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return FALSE;
} // End CmdFnsVarsOps_EM


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
//  $CmdReset_EM
//
//  Execute the system command:  )RESET
//***************************************************************************

BOOL CmdReset_EM
    (LPWCHAR lpwszTail)

{
    GotoReset ();

    return TRUE;
} // End CmdReset_EM


//***************************************************************************
//  $CmdSi_EM
//
//  Execute the system command:  )SI
//***************************************************************************

BOOL CmdSi_EM
    (LPWCHAR lpwszTail)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;      // Ptr to current SIS_HEADER srtuc

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Loop through the SI levels
    if (lpMemPTD->lpSISCur)
    for (lpSISCur = lpMemPTD->lpSISBeg;
         lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt;
         lpSISCur = lpSISCur->lpSISNxt)
    {
        LPAPLCHAR    lpMemName;         // Ptr to function name global memory

        // Split cases based upon the caller's function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_IMM:
                break;

            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (lpMemPTD->lpSISCur->hGlbFcnName);

                // Format the Name, Line #, and Suspension marker
                wsprintfW (lpwszTemp,
                           L"%s[%d] %c",
                           lpMemName,
                           lpSISCur->CurLineNum,
                           " *"[lpSISCur->Suspended]);
                // We no longer need this ptr
                MyGlobalUnlock (lpMemPTD->lpSISCur->hGlbFcnName); lpMemName = NULL;

                // Display the function name & line #
                AppendLine (lpwszTemp, FALSE, TRUE);

                break;

            case DFNTYPE_EXEC:
                AppendLine (WS_UTF16_UPTACKJOT, FALSE, TRUE);

                break;

            case DFNTYPE_QUAD:
                AppendLine (WS_UTF16_QUAD, FALSE, TRUE);

                break;

            case DFNTYPE_UNK:
            defstop
                break;
        } // End SWITCH
    } // End IF/FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return TRUE;
} // End CmdSi_EM


//***************************************************************************
//  End of File: syscmds.c
//***************************************************************************
