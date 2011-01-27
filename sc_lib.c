//***************************************************************************
//  NARS2000 -- System Command:  )LIB
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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
#include "headers.h"


//***************************************************************************
//  $CmdLib_EM
//
//  Execute the system command:  )LIB  [dir] [[first][-][last]]
//***************************************************************************

UBOOL CmdLib_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    return CmdLibCom_EM (lpwszTail, TRUE);
} // End CmdLib_EM


//***************************************************************************
//  $CmdUlib_EM
//
//  Execute the system command:  )ULIB [dir] [[first][-][last]]
//***************************************************************************

UBOOL CmdUlib_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    return CmdLibCom_EM (lpwszTail, FALSE);
} // End CmdUlib_EM


//***************************************************************************
//  $CmdLibCom_EM
//
//  Execute the system command:  )LIB  [dir] [[first][-][last]]
//                               )ULIB [dir] [[first][-][last]]
//***************************************************************************

UBOOL CmdLibCom_EM
    (LPWCHAR lpwszTail,                         // Ptr to command line tail
     UBOOL   bUniqWs)                           // TRUE iff listing unique workspaces only

{
    LPPERTABDATA  lpMemPTD;                     // Ptr to PerTabData global memory
    LPWCHAR       lpwszTemp,                    // Ptr to temporary storage
                  lpwszFormat,                  // ...
                  lpwszSaveName,                // Ptr to next save area for a filename
                  lpwszLeadRange,               // Ptr to the leading range
                  lpwszTailRange,               // ...        trailing ... (after the separator)
                                                //                         (may be NULL if no separator)
                 *lplpwszArgs,                  // Ptr to ptr to args
                 *lplpwszPtr;                   // Ptr to ptr to saved names
    UINT          uArgCnt;                      // # args after the system command
    WCHAR         wszFileName[_MAX_PATH];       // Save area for longest DPFE
    UINT          uExtLen,                      // Length of workspace extension
                  uNameCnt,                     // Name counter
                  uCnt;                         // Loop counter
    UBOOL         bEndDQ = FALSE;               // TRUE iff the string ends with a Double Quote
    LPWSZLIBDIRS  lpwszLibDirs;                 // Ptr to LibDirs

    // Check for command line switches
    if (!CheckCommandLine (lpwszTail,           // Ptr to command line (after the command itself)
                          &uArgCnt,             // Ptr to # args
                          &lplpwszArgs,         // Ptr to ptr to ptr to args
                          &lpwszLeadRange,      // Ptr to ptr to leading range
                          &lpwszTailRange,      // ...           trailing range
                           TRUE))               // TRUE iff )LIB
        return FALSE;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary storage
    lpwszTemp   = lpMemPTD->lpwszTemp;
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Lock the memory to get a ptr to it
    lpwszLibDirs = MyGlobalLock (hGlbLibDirs);

    // Loop through the search dirs
    for (uCnt = 0; uCnt < uNumLibDirs; uCnt++)
    {
        // If there's a command line tail, use it
        if (uArgCnt > 0
         && lplpwszArgs[0][0] NE LIBCMD_SWITCH)
        {
            lstrcpyW (lpwszFormat, lplpwszArgs[0]);

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
            MakeWorkspaceNameCanonical (wszFileName, lpwszFormat, lpwszLibDirs[uCnt]);
        } else
            lstrcpyW (wszFileName, lpwszLibDirs[uCnt]);

        // Get the length of the filename so far
        uExtLen = lstrlenW (wszFileName);

        // Initialize the name counter and name ptr
        uNameCnt = 0;
        lplpwszPtr = (LPWCHAR *) lpwszFormat;
        lpwszSaveName = lpwszTemp;

        // Accumulate the workspaces (*.ws.nars)
        lpwszSaveName =
          CmdLibFiles (wszFileName,             // Ptr to file name root
                       WS_WKSEXT,               // Extension to use
                       FALSE,                   // TRUE iff we should show the extension
                       lpwszLeadRange,          // Ptr to leading range
                       lpwszTailRange,          // ...    trailing ...  (may be NULL if no separator)
                       lpwszSaveName,           // Ptr to save area for the names
                      &lplpwszPtr[uNameCnt],    // Ptr to save area for the ptrs
                      &uNameCnt,                // Ptr to name counter
                       bUniqWs);                // TRUE iff listing unique workspaces only
        // Zap so we start off at the same place
        wszFileName[uExtLen] = WC_EOS;

        // Accumulate the atf files (*.atf)
        lpwszSaveName =
          CmdLibFiles (wszFileName,             // Ptr to file name root
                       WS_ATFEXT,               // Extension to use
                       TRUE,                    // TRUE iff we should show the extension
                       lpwszLeadRange,          // Ptr to leading range
                       lpwszTailRange,          // ...    trailing ...  (may be NULL if no separator)
                       lpwszSaveName,           // Ptr to save area for the names
                      &lplpwszPtr[uNameCnt],    // Ptr to save area for the ptrs
                      &uNameCnt,                // Ptr to name counter
                       bUniqWs);                // TRUE iff listing unique workspaces only
        // Zap so we start off at the same place
        wszFileName[uExtLen] = WC_EOS;

        // If there are any files, ...
        if (uNameCnt)
        {
            // Display the current search dir
            AppendLine (L"\"", FALSE, FALSE);
            AppendLine (wszFileName, FALSE, FALSE);
            AppendLine (L"\"", FALSE, TRUE);

            // Sort the names
            ShellSort (lplpwszPtr, uNameCnt, CmpLPLPWCHAR);

            // Display the names
            DisplayLibNames (lplpwszPtr, uNameCnt);
        } // End IF
    } // End FOR

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLibDirs); lpwszLibDirs = NULL;

    return TRUE;
} // End CmdLibCom_EM


//***************************************************************************
//  $CmdLibFiles
//
//  Display library files (*.ws.nars or *.atf)
//***************************************************************************

LPWCHAR CmdLibFiles
    (LPWCHAR  wszFileName,              // Ptr to file name root
     LPWCHAR  wszExt,                   // Extension to use
     UBOOL    bShowExt,                 // TRUE iff we should show the extension
     LPWCHAR  lpwszLeadRange,           // Ptr to leading range
     LPWCHAR  lpwszTailRange,           // ...    trailing ...  (may be NULL if no separator)
     LPWCHAR  lpwszSaveName,            // Ptr to save area for the names
     LPWCHAR *lplpwszSavePtr,           // Ptr to save area for the ptrs
     LPUINT   lpuCntNames,              // Ptr to name counter
     UBOOL    bUniqWs)                  // TRUE iff listing unique workspaces only

{
    HANDLE           hFind;             // Handle to FindData
    WIN32_FIND_DATAW FindData;          // FindFirstFile return data struc
    UINT             uFileLen,          // Length of workspace filename
                     uExtLen;           // Length of workspace extension
    UBOOL            bEndDQ;            // TRUE iff the string ends with a Double Quote

    // Append a backslash to the icoming filename if not already there
    AppendBackslash (wszFileName);

    // Get the length to append trailing extension (wszExt)
    uFileLen = lstrlenW (wszFileName);

    // If it ends in a double quote, ...
    bEndDQ = (wszFileName[uFileLen - 1] EQ WC_DQ);

    // Create the wildcard string to search for workspaces
    lstrcpyW (&wszFileName[uFileLen - bEndDQ], L"*");
    uFileLen++;
    lstrcpyW (&wszFileName[uFileLen - bEndDQ], wszExt);
    uFileLen += lstrlenW (wszExt);

    // If it ended with a double quote, ...
    if (bEndDQ)
    {
        // Append it again
        wszFileName[uFileLen + 0] = WC_DQ;
        wszFileName[uFileLen + 1] = WC_EOS;
    } // End IF

    // Get length of workspace extension
    uExtLen = lstrlenW (wszExt);

    // Find the first (if any) workspace
    hFind = FindFirstFileW (wszFileName, &FindData);

    if (hFind NE INVALID_HANDLE_VALUE)
    {
        // Check for a match
        lpwszSaveName =
          CmdLibCheckMatch (FindData.cFileName,     // Ptr to the found filename.ext
                            bShowExt,               // TRUE iff we should show the extension
                            bUniqWs,                // TRUE iff listing unique workspaces only
                            uExtLen,                // Length of workspace extension
                            lpwszLeadRange,         // Ptr to leading range
                            lpwszTailRange,         // ...    trailing ...  (may be NULL if no separator)
                            lpwszSaveName,          // Ptr to save area for the names
                           &lplpwszSavePtr,         // Ptr to save area for the ptrs
                            lpuCntNames);           // Ptr to name counter
        // Continue looking
        while (FindNextFileW (hFind, &FindData))
            // Check for a match
            lpwszSaveName =
              CmdLibCheckMatch (FindData.cFileName,     // Ptr to the found filename.ext
                                bShowExt,               // TRUE iff we should show the extension
                                bUniqWs,                // TRUE iff listing unique workspaces only
                                uExtLen,                // Length of workspace extension
                                lpwszLeadRange,         // Ptr to leading range
                                lpwszTailRange,         // ...    trailing ...  (may be NULL if no separator)
                                lpwszSaveName,          // Ptr to save area for the names
                               &lplpwszSavePtr,         // Ptr to save area for the ptrs
                                lpuCntNames);           // Ptr to name counter
        // Close the find handle
        FindClose (hFind); hFind = NULL;
    } // End IF

    return lpwszSaveName;
} // End CmdLibFiles


//***************************************************************************
//  $CmdLibCheckMatch
//
//  Subroutine to CmdLibFiles to check a filename.ext for a match
//***************************************************************************

LPWCHAR CmdLibCheckMatch
    (LPWCHAR   cFileName,                   // Ptr to the found filename.ext
     UBOOL     bShowExt,                    // TRUE iff we should show the extension
     UBOOL     bUniqWs,                     // TRUE iff listing unique workspaces only
     UINT      uExtLen,                     // Length of workspace extension
     LPWCHAR   lpwszLeadRange,              // Ptr to leading range
     LPWCHAR   lpwszTailRange,              // ...    trailing ...  (may be NULL if no separator)
     LPWCHAR   lpwszSaveName,               // Ptr to save area for the names
     LPWCHAR **lplplpwszSavePtr,            // Ptr to ptr to save area for the ptrs
     LPUINT    lpuCntNames)                 // Ptr to name counter
{
    UINT uWsLen;                            // Length of the workspace name

    // Get the length of the workspace name plus extension
    uWsLen = lstrlenW (cFileName);

    // If we're not showing the extension, ...
    if (!bShowExt)
    {
        // Delete the workspace extension
        cFileName[uWsLen - uExtLen] = WC_EOS;

        // Account for it
        uWsLen -= uExtLen;
    } // End IF

    // Check against leading and trailing ranges
    if ((lpwszLeadRange[0] EQ WC_EOS
      || strncmpiW (cFileName, lpwszLeadRange, lstrlenW (lpwszLeadRange)) >= 0)
     && (lpwszTailRange NE NULL
      || strncmpiW (cFileName, lpwszLeadRange, lstrlenW (lpwszLeadRange)) EQ 0)
     && (lpwszTailRange EQ NULL
      || lpwszTailRange[0] EQ WC_EOS
      || strncmpiW (cFileName, lpwszTailRange, lstrlenW (lpwszTailRange)) <= 0))
    {
        // Omit ".bak" workspace names
        if (!(bUniqWs
           && uWsLen > WS_BAKEXT_LEN
           && lstrcmpiW (WS_BAKEXT, &cFileName[uWsLen - WS_BAKEXT_LEN]) EQ 0))
        {
            // Accumulate the workspace name
            lstrcpyW (lpwszSaveName, cFileName);

            // Add in another name
            (*lpuCntNames)++;

            // Save a ptr to the name and skip over it
            *(*lplplpwszSavePtr)++ = lpwszSaveName;

            // Skip over the name and trailing zero
            lpwszSaveName += lstrlenW (lpwszSaveName) + 1;
        } // End IF
    } // End IF

    return lpwszSaveName;
} // End CmdLibCheckMatch


//***************************************************************************
//  $DisplayLibNames
//
//  Display the names in columns
//***************************************************************************

void DisplayLibNames
    (LPWCHAR *lplpwszPtr,                   // Ptr to ptrs to names
     UINT     uNameCnt)                     // # names

{
    WCHAR   wszLine[DEF_MAX_QUADPW];        // Save area for output line
    UINT    uCnt,                           // Loop counter
            uLineChar,                      // Current char # in output line
            uNameLen,                       // Length of the current name
            uQuadPW;                        // Current value of []PW
    UBOOL   bLineCont;                      // TRUE iff this line is a continuation
    LPWCHAR lpMemName;                      // Ptr to current name

#define LINE_INDENT     4
#define NAME_WIDTH      8

    // Check for no names
    if (uNameCnt EQ 0)
        AppendLine (L"    <none>", TRUE, TRUE);
    else
    {
        // Get the current value of []PW
        uQuadPW = (UINT) GetQuadPW ();

        // Initialize the output area
        FillMemoryW (wszLine, uQuadPW, L' ');

        // Display the names
        for (bLineCont = FALSE, uLineChar = LINE_INDENT, uCnt = 0;
             uCnt < uNameCnt;
             uCnt++)
        {
            // Point to the name
            lpMemName = lplpwszPtr[uCnt];

            // Get the name length
            uNameLen = lstrlenW (lpMemName);

            // If the line is too long, skip to the next one
            if ((uLineChar + uNameLen) > uQuadPW
             && uLineChar > LINE_INDENT)
            {
                // Ensure properly terminated
                wszLine[min (uLineChar, uQuadPW)] = WC_EOS;

                // Output the current line
                AppendLine (wszLine, bLineCont, TRUE);

                // Mark all lines from here on as continuations
                bLineCont = TRUE;

                // Re-initialize the output area
                FillMemoryW (wszLine, uQuadPW, L' ');

                // Re-initialize the char counter
                uLineChar = LINE_INDENT;
            } // End IF

            // Copy the name to the output area
            CopyMemoryW (&wszLine[uLineChar], lpMemName, uNameLen);

            // Skip to the next name boundary
            uLineChar += NAME_WIDTH * (1 + uNameLen / NAME_WIDTH);
        } // End FOR

        // If there's still text in the buffer, output it
        if (uLineChar > LINE_INDENT)
        {
            // Ensure properly terminated
            wszLine[uLineChar] = WC_EOS;

            // Output the current line
            AppendLine (wszLine, TRUE, TRUE);
        } // End IF
    } // End IF
} // End DisplayLibNames


//***************************************************************************
//  End of File: sc_lib.c
//***************************************************************************
