//***************************************************************************
//  NARS2000 -- System Command:  )FNS, )NMS, )OPS, )VARS
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
//  $CmdFns_EM
//
//  Execute system command:  )FNS [first][-][last]
//***************************************************************************

BOOL CmdFns_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    return CmdFNOV_EM (lpwszTail, IzitFNS, FALSE);
} // End CmdFns_EM


//***************************************************************************
//  $IzitFNS
//
//  Return TRUE iff the object name type is that of a function
//***************************************************************************

BOOL IzitFNS
    (UINT stNameType)

{
    return IsNameTypeFn (stNameType);
} // End IzitFNS


//***************************************************************************
//  $CmdNms_EM
//
//  Execute system command:  )NMS [first][-][last]
//***************************************************************************

BOOL CmdNms_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    return CmdFNOV_EM (lpwszTail, IzitNMS, TRUE);
} // End CmdNms_EM


//***************************************************************************
//  $IzitNMS
//
//  Return TRUE iff the object type is that of a variable/function/operator
//***************************************************************************

BOOL IzitNMS
    (UINT stNameType)

{
    return IsNameTypeName (stNameType);
} // End IzitNMS


//***************************************************************************
//  $CmdOps_EM
//
//  Execute system command:  )OPS [first][-][last]
//***************************************************************************

BOOL CmdOps_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    return CmdFNOV_EM (lpwszTail, IzitOPS, FALSE);
} // End CmdOps_EM


//***************************************************************************
//  $IzitOPS
//
//  Return TRUE iff the object type is that of an operator
//***************************************************************************

BOOL IzitOPS
    (UINT stNameType)

{
    return IsNameTypeOp (stNameType);
} // End IzitOPS


//***************************************************************************
//  $CmdVars_EM
//
//  Execute system command:  )VARS [first][-][last]
//***************************************************************************

BOOL CmdVars_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    return CmdFNOV_EM (lpwszTail, IzitVARS, FALSE);
} // End CmdVars_EM


//***************************************************************************
//  $IzitVARS
//
//  Return TRUE iff the object type is that of a variable
//***************************************************************************

BOOL IzitVARS
    (UINT stNameType)

{
    return IsNameTypeVar (stNameType);
} // End IzitVARS


//***************************************************************************
//  $CmdFNOV_EM
//
//  Execute system command:  )FNS, )NMS, )OPS, or )VARS
//***************************************************************************

BOOL CmdFNOV_EM
    (LPWCHAR lpwszTail,             // Ptr to command line tail
     BOOL  (*IzitFVO) (UINT),       // Ptr to function to determine name type
     BOOL    bNMS)                  // TRUE iff the command is )NMS

{
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPWCHAR      lpwszFormat;       // Ptr to formatting save area
    LPSYMENTRY   lpSymEntry;        // Ptr to current SYMENTRY
    LPAPLCHAR    lpMemName;         // Ptr to name global memory
    LPSYMENTRY  *lpSymSort;         // Ptr to LPSYMENTRYs for sorting
    UINT         uSymCnt,           // Count of # matching STEs
                 uSymNum,           // Loop counter
                 uLineChar,         // Current char # in output line
                 uNameLen,          // Length of the current name
                 uQuadPW,           // Current value of []PW
                 uMaxNameLen = 0;   // Length of longest name
    BOOL         bLineCont;         // TRUE iff this line is a continuation

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Initialize the LPSYMENTRY sort array
    lpSymSort = (LPSYMENTRY *) lpMemPTD->lpwszTemp;

    // Trundle through the Symbol Table
    //   looking for functions (niladic or monadic/dyadic),
    //   operators (monadic or dyadic), and/or variables
    for (lpSymEntry = lpMemPTD->lpSymTab, uSymCnt = 0;
         lpSymEntry < lpMemPTD->lpSymTabNext;
         lpSymEntry++)
    if (lpSymEntry->stFlags.Inuse
     && lpSymEntry->stFlags.Value
     && lpSymEntry->stFlags.ObjName NE OBJNAME_SYS
     && (*IzitFVO) (lpSymEntry->stFlags.stNameType))
    {
        // ***FIXME*** -- Make sensitive to [first][-][last] in lpwszTail
        LPSYMENTRY lpSymLink = lpSymEntry;

        // Get a ptr to the global SYMENTRY (stSymLink EQ NULL)
        for (lpSymLink = lpSymEntry; lpSymLink->stSymLink; lpSymLink = lpSymLink->stSymLink)
            ;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (lpSymLink->stHshEntry->htGlbName);

        // Get the name length
        uNameLen = lstrlenW (lpMemName) + bNMS * 2;

        // Find the longest name
        uMaxNameLen = max (uMaxNameLen, uNameLen);

        // We no longer need this ptr
        MyGlobalUnlock (lpSymLink->stHshEntry->htGlbName); lpMemName = NULL;

        // Save the LPSYMENTRY ptr for later use
        lpSymSort[uSymCnt] = lpSymLink;

        // Count in another matching name
        uSymCnt++;
    } // End FOR/IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Sort the HGLOBALs
    ShellSort (lpSymSort, uSymCnt, CmpLPSYMENTRY);

    // Get the current value of []PW
    uQuadPW = (UINT) GetQuadPW ();

    // Re-initialize the output area
    for (uLineChar = 0; uLineChar < uQuadPW; uLineChar++)
        lpwszFormat[uLineChar] = L' ';

    // Display the names
    for (bLineCont = FALSE, uLineChar = uSymNum = 0;
         uSymNum < uSymCnt;
         uSymNum++)
    {
        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (lpSymSort[uSymNum]->stHshEntry->htGlbName);

        // Get the name length
        uNameLen = lstrlenW (lpMemName);

        // If the line is too long, skip to the next one
        if ((uLineChar + uNameLen + bNMS * 2) > uQuadPW
         && uLineChar NE 0)
        {
            // Ensure properly terminated
            lpwszFormat[uLineChar] = L'\0';

            // Output the current line
            AppendLine (lpwszFormat, bLineCont, TRUE);

            // Mark all lines from here on as continuations
            bLineCont = TRUE;

            // Re-initialize the output area
            for (uLineChar = 0; uLineChar < uQuadPW; uLineChar++)
                lpwszFormat[uLineChar] = L' ';

            // Re-initialize the char counter
            uLineChar = 0;
        } // End IF

        // Copy the name to the output area
        CopyMemory (&lpwszFormat[uLineChar], lpMemName, uNameLen * sizeof (APLCHAR));

        // If it's )NMS, append the name class
        if (bNMS)
        {
            lpwszFormat[uLineChar + uNameLen + 0] = L'.';
            lpwszFormat[uLineChar + uNameLen + 1] = L'0' + (UINT) CalcNameClass (lpSymSort[uSymNum]);
        } // End IF

        // Skip to the next name boundary
        uLineChar += uMaxNameLen + 1;

        // We no longer need this ptr
        MyGlobalUnlock (lpSymSort[uSymNum]->stHshEntry->htGlbName); lpMemName = NULL;
    } // End FOR

    // If there's still text in the buffer, output it
    if (uLineChar NE 0)
    {
        // Ensure properly terminated
        lpwszFormat[uLineChar] = L'\0';

        // Output the current line
        AppendLine (lpwszFormat, TRUE, TRUE);
    } // End IF

    return FALSE;
} // End CmdFNOV_EM


//***************************************************************************
//  End of File: sc_fnov.c
//***************************************************************************
