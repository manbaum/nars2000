//***************************************************************************
//  NARS2000 -- System Command:  )COPY
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
//  $CmdCopy_EM
//
//  Execute the system command:  )COPY wsid [obj1 [obj2 ...]]
//                               )COPY #nnn [obj1 [obj2 ...]]
//    where wsid is a workspace identifier and #nnn is a tab identifier.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdCopy_EM"
#else
#define APPEND_NAME
#endif

BOOL CmdCopy_EM
    (LPWCHAR lpwszTail)                     // Ptr to command line tail

{
    HGLOBAL      hGlbPTD;                   // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory
    WCHAR        wszTailDPFE[_MAX_PATH];    // Save area for canonical form of given ws name
    int          iTabIndex;             // Tab index
    BOOL         bRet = FALSE;              // TRUE iff result is valid
    FILE        *fStream;                   // Ptr to file stream for the plain text workspace file

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // If there's no WSID, that's an error
    if (lpwszTail EQ L'\0')
    {
        IncorrectCommand ();

        return FALSE;
    } // End IF

    // Split cases based upon the first char
    // If we're copying from another tab, ...
    if (*lpwszTail EQ L'#')
    {
        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Get the tab index from which this command was issued
        iTabIndex = lpMemPTD->CurTabIndex;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;














        AppendLine (L"NONCE COMMAND", FALSE, TRUE);
    } else
    // Copy from a workspace file
    {
        // Convert the given workspace name into a canonical form (without WKSEXT)
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, wszWorkDir);

        // Append the common workspace extension
        lstrcatW (wszTailDPFE, WS_WKSEXT);

        // Attempt to open the workspace
        fStream = _wfopen (wszTailDPFE, L"r");

        // If the workspace doesn't exist, ...
        if (fStream EQ NULL)
            goto WSNOTFOUND_EXIT;

        // We no longer need this handle
        fclose (fStream); fStream = NULL;









        AppendLine (L"NONCE COMMAND", FALSE, TRUE);
    } // End IF/ELSE

    // Mark as successfull
    bRet = TRUE;

    goto NORMAL_EXIT;

WSNOTFOUND_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_WS_NOT_FOUND APPEND_NAME);

NORMAL_EXIT:
    return bRet;
} // End CmdCopy_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: sc_copy.c
//***************************************************************************
