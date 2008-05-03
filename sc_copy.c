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
#include "workspace.h"

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
    WCHAR        wszTailDPFE[_MAX_PATH],    // Save area for canonical form of given ws name
                 wcTmp;                     // Temporary char
    LPWCHAR      lpwCmd,                    // Ptr to command line
                 lpwErrMsg,                 // Ptr to (constant) error message text
                 lpwNotFound;               // Ptr to list of names not found
    int          iCurTabID,                 // Tab ID
                 iSrcTabID,                 // Source tab ID
                 iSrcTabIndex,              // Source tab index
                 iCnt;                      // # fields scanned
    BOOL         bRet = FALSE;              // TRUE iff result is valid
    FILE        *fStream;                   // Ptr to file stream for the plain text workspace file
    HWND         hWndEC;                    // Edit Control window handle
    LPSYMENTRY   lpSymLink = NULL;          // Anchor of SYMENTRY links for [Globals] values
                                            //   so we may delete them easily

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Get the Session Manager's hWndEC
    hWndEC = GetThreadSMEC ();

    // If there's no WSID, that's an error
    if (lpwszTail EQ L'\0')
        goto INCORRECTCOMMAND_EXIT;

    // Save not found ptr
    lpwNotFound = lpwszTail;

    // Split cases based upon the first char
    // If we're copying from another tab, ...
    if (*lpwszTail EQ L'#')
    {
        // Scan the command line for the source tab ID
        iCnt = swscanf (lpwszTail, L"#%u", &iSrcTabID);

        // Ensure we got a value
        if (iCnt NE 1)
            goto INCORRECTCOMMAND_EXIT;

        // Skip over the field
        lpwCmd = SkipToCharW (lpwszTail, L' ');
        wcTmp = *lpwCmd; *lpwCmd++ = L'\0';
        if (wcTmp)
            lpwCmd = SkipWhiteW (lpwCmd);

        // Get the matching source tab index
        iSrcTabIndex = TranslateTabIDToIndex (iSrcTabID);
        if (iSrcTabIndex EQ -1)
            goto TABNOTFOUND_EXIT;

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Get the tab ID from which this command was issued
        iCurTabID = lpMemPTD->CurTabID;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

        AppendLine (L"NONCE COMMAND", FALSE, TRUE);

////    DbgBrk ();












    } else
    // Copy from a workspace file
    {
        // Skip over the workspace name
        // ***FIXME*** -- Handle workspace names enclosed in double quotes
        //                  because they have embedded blanks.
        lpwCmd = SkipToCharW (lpwszTail, L' ');
        wcTmp = *lpwCmd; *lpwCmd++ = L'\0';
        if (wcTmp)
            lpwCmd = SkipWhiteW (lpwCmd);

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

        // If the replaced char is EOS, copy all names
        if (wcTmp EQ L'\0')
        {
            // Loop through the [Vars.0] section copying
            //   all the names
            if (CopyWsVars (NULL,                   // Ptr to name in command line (may be NULL if bAllNames)
                            lpwCmd,                 // Ptr to command line
                            hWndEC,                 // Edit Control for SM window handle
                           &lpwErrMsg,              // Ptr to ptr to (constant) error message text
                            TRUE,                   // TRUE if we should process all names
                           &lpSymLink,              // Ptr to ptr to SYMENTRY link
                            wszTailDPFE) EQ -1)     // Save area for canonical form of given ws name
                goto ERRMSG_EXIT;

            // Loop through the [Fcns.0] section copying
            //   all the names
            if (CopyWsFcns (NULL,                   // Ptr to name in command line (may be NULL if bAllNames)
                            hWndEC,                 // Edit Control for SM window handle
                           &lpwErrMsg,              // Ptr to ptr to (constant) error message text
                            TRUE,                   // TRUE if we should process all names
                           &lpSymLink,              // Ptr to ptr to SYMENTRY link
                            wszTailDPFE))           // Save area for canonical form of given ws name
                goto ERRMSG_EXIT;
        } else
        {
            // Loop through the names starting at <lpwCmd>
            while (*lpwCmd)
            {
                LPWCHAR lpwNameInCmd;           // Ptr to the name in the command line

                // Find the next name
                lpwNameInCmd = lpwCmd;
                lpwCmd = SkipToCharW (lpwNameInCmd, L' ');
                if (*lpwCmd NE L'\0')
                {
                    *lpwCmd++ = L'\0';
                    lpwCmd = SkipWhiteW (lpwCmd);
                } // End IF

                // Loop through the [Vars.0] section looking for each given name of the form
                //  xxx=Name=#ggg               for a variable in [Globals]
                //  xxx=Name=T 1 0 value        for an immediate scalar variable
                //                              where T is the variable immediate type (BIFC)
                switch (CopyWsVars (lpwNameInCmd,   // Ptr to name in command line (may be NULL if bAllNames)
                                    lpwCmd,         // Ptr to command line
                                    hWndEC,         // Edit Control for SM window handle
                                   &lpwErrMsg,      // Ptr to ptr to (constant) error message text
                                    FALSE,          // TRUE if we should process all names
                                   &lpSymLink,      // Ptr to ptr to SYMENTRY link
                                    wszTailDPFE))   // Save area for canonical form of given ws name
                {
                    case -1:
                        goto ERRMSG_EXIT;

                    case 0:
                        // We found a match, so look for the next name
                        continue;

                    case 1:
                        // No match, keep processing
                        break;

                    defstop
                        break;
                } // End SWITCH

                // Loop through the [Fcns.0] section looking for each given name of the form
                //  xxx=Name=y=#ggg             for a function in [Globals] with NameType y
                //  xxx=Name=y={name} or char   for immediate functions
                switch (CopyWsFcns (lpwNameInCmd,   // Ptr to name in command line (may be NULL if bAllNames)
                                    hWndEC,         // Edit Control for SM window handle
                                   &lpwErrMsg,      // Ptr to ptr to (constant) error message text
                                    FALSE,          // TRUE if we should process all names
                                   &lpSymLink,      // Ptr to ptr to SYMENTRY link
                                    wszTailDPFE))   // Save area for canonical form of given ws name
                {
                    case -1:
                        goto ERRMSG_EXIT;

                    case 0:
                        // We found a match, so look for the next name
                        continue;

                    case 1:
                        // No match, keep processing
                        break;

                    defstop
                        break;
                } // End SWITCH

                // We didn't find the name, so add it to the NOT FOUND: list
                lstrcpyW (lpwNotFound, lpwNameInCmd);

                // Skip to the trailing zero
                lpwNotFound += lstrlenW (lpwNotFound);

                // Insert a blank separator and skip over it for the next entry
                *lpwNotFound++ = L' ';
            } // End WHILE
        } // End IF/ELSE

        // Delete the symbol table entries for vars/fcns we allocated of the form "#%d"
        DeleteGlobalLinks (lpSymLink);
    } // End IF/ELSE

    // Display the workspace timestamp
    DisplayWorkspaceStamp (wszTailDPFE);

    // Check for names not found
    if (lpwNotFound NE lpwszTail)
    {
        // Ensure properly terminated
        lpwNotFound[-1] = L'\0';

        // Write out the leading text followed by the names
        AppendLine (ERRMSG_NOT_FOUND, FALSE, FALSE);
        AppendLine (lpwszTail, FALSE, TRUE);
    } // End IF

    // Mark as successfull
    bRet = TRUE;

    goto NORMAL_EXIT;

INCORRECTCOMMAND_EXIT:
    IncorrectCommand ();

    goto ERROR_EXIT;

TABNOTFOUND_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_TAB_NOT_FOUND APPEND_NAME);

    goto ERROR_EXIT;

WSNOTFOUND_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_WS_NOT_FOUND APPEND_NAME);

    goto ERROR_EXIT;

ERRMSG_EXIT:
    ReplaceLastLineCRPmt (lpwErrMsg);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return bRet;
} // End CmdCopy_EM
#undef  APPEND_NAME


//***************************************************************************
//  $DeleteGlobalLinks
//
//  Delete SymLink entries
//***************************************************************************

void DeleteGlobalLinks
    (LPSYMENTRY lpSymLink)

{
    LPSYMENTRY lpSymLast;       // Temporary ptr to previous stSymLink

    while (lpSymLink)
    {
        // Free the global var/fcn/opr
        FreeResultGlobalDFV (lpSymLink->stData.stGlbData);

        // Point to the next entry and zap it
        lpSymLast = lpSymLink->stSymLink;
        lpSymLink->stSymLink = NULL;

        // Erase the entry
        EraseSTE (lpSymLink);

        // ***FIXME*** -- What's involved in deleting the STE??

        // Point to the next one
        lpSymLink = lpSymLast;
    } // End WHILE
} // End DeleteGlobalLinks


//***************************************************************************
//  $CopyWsVars
//
//  Copy all vars from a workspace or just those which match a given name
//***************************************************************************

int CopyWsVars
    (LPWCHAR     lpwNameInCmd,              // Ptr to name in command line (may be NULL if bAllNames)
     LPWCHAR     lpwCmd,                    // Ptr to command line
     HWND        hWndEC,                    // Edit Control for SM window handle
     LPWCHAR    *lplpwErrMsg,               // Ptr to ptr to (constant) error message text
     BOOL        bAllNames,                 // TRUE if we should process all names
     LPSYMENTRY *lplpSymLink,               // Ptr to ptr to SYMENTRY link
     WCHAR       wszTailDPFE[])             // Save area for canonical form of given ws name

{
    WCHAR        wszCount[8];               // Save area for formatted uSymVar/Fcn counter
    LPWCHAR      lpwNameInWrk,              // Ptr to name in workspace file
                 lpwDataInWrk;              // Ptr to key data from the saved workspace file
    UINT         uSymVar,                   // Var section counter
                 uCnt;                      // Loop counter
    LPSYMENTRY   lpSymEntry;                // Ptr to name in cmd SYMENTRY
    STFLAGS      stFlags = {0};             // STE flags

    // Get the [Vars.0] count
    uSymVar =
      GetPrivateProfileIntW (SECTNAME_VARS L".0",       // Ptr to the section name
                             KEYNAME_COUNT,             // Ptr to the key name
                             0,                         // Default value if not found
                             wszTailDPFE);              // Ptr to the file name
    // Loop through the [Vars.0] section
    for (uCnt = 0; uCnt < uSymVar; uCnt++)
    {
        // Format the counter
        wsprintfW (wszCount, L"%d", uCnt);

        // Point to the name in the workspace
        lpwNameInWrk = lpwszTemp;

        // Read the next string
        GetPrivateProfileStringW (SECTNAME_VARS L".0",  // Ptr to the section name
                                  wszCount,             // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  lpwNameInWrk,         // Ptr to the output buffer
                                  memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                  wszTailDPFE);         // Ptr to the file name
        // Find the separator after the name and zap it
        lpwDataInWrk = strchrW (lpwNameInWrk, L'=');
        *lpwDataInWrk++ = L'\0';

        // Convert the {name}s and other chars to UTF16_xxx
        ConvertNameInPlace (lpwNameInWrk);

        // Compare the names to see if we have a match
        if (bAllNames || lstrcmpW (lpwNameInWrk, lpwNameInCmd) EQ 0)
        {
            APLSTYPE     aplTypeObj;            // Object storage type
            APLLONGEST   aplLongestObj;         // Object immediate value
            LPAPLLONGEST lpaplLongestObj;       // Ptr to ...
            BOOL         bImmed;                // TRUE iff the result of ParseSavedWsVar_EM is immediate

            // Set the flags for what we're looking up/appending
            stFlags.Inuse   = TRUE;

            if (IsSysName (lpwNameInWrk))
            {
                // If we're copying all names, skip the system vars
                if (bAllNames)
                    continue;

                // Tell 'em we're looking for system names
                stFlags.ObjName = OBJNAME_SYS;
            } else
                // Tell 'em we're looking for user names
                stFlags.ObjName = OBJNAME_USR;

            // Lookup the name in the symbol table
            lpSymEntry =
              SymTabLookupName (lpwNameInWrk, &stFlags);

            // If the name is not found, it must be a user name
            Assert ((stFlags.ObjName EQ OBJNAME_USR) || lpSymEntry NE NULL);

            // If the name is not found, append it as a user name
            if (lpSymEntry EQ NULL)
            {
                // Append the name to get a new LPSYMENTRY
                lpSymEntry = SymTabAppendName_EM (lpwNameInWrk, &stFlags);

                // Mark the SYMENTRY as immediate so we don't free the
                //   (non-existant) stGlbData
                // Set other flags as appropriate
                lpSymEntry->stFlags.Imm        = TRUE;
                lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
                lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
            } // End IF

            // Clear so we save a clean value
            aplLongestObj = 0;

            // Set this value as lpaplLongestObj is incremented by ParseSavedWsVar_EM
            lpaplLongestObj = &aplLongestObj;

            // Parse the value into aplLongestObj and aplTypeObj
            lpwDataInWrk =
              ParseSavedWsVar_EM (lpwDataInWrk,     // Ptr to input buffer
                                 &lpaplLongestObj,  // Ptr to ptr to output element
                                 &aplTypeObj,       // Ptr to storage type (may be NULL)
                                 &bImmed,           // Ptr to immediate flag (TRUE iff result is immediate) (may be NULL)
                                  FALSE,            // TRUE iff to save SymTabAppend values, FALSE to save values directly
                                  hWndEC,           // Edit Control window handle
                                  lplpSymLink,      // Ptr to ptr to SYMENTRY link
                                  wszTailDPFE,      // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
                                  lplpwErrMsg);     // Ptr to ptr to (constant error message text
            if (lpwDataInWrk EQ NULL)
                goto ERRMSG_EXIT;

            // If it's []DM, handle it specially
            if (lstrcmpiW (lpwNameInWrk, WS_UTF16_QUAD L"dm") EQ 0)
            {
                HGLOBAL      hGlbPTD;                   // PerTabData global memory handle
                LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory

                Assert (!bImmed);
                Assert (IsSimpleChar (aplTypeObj));

                // Get the PerTabData global memory handle
                hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

                // Lock the memory to get a ptr to it
                lpMemPTD = MyGlobalLock (hGlbPTD);

                // Out with the old
                FreeResultGlobalVar (lpMemPTD->hGlbQuadDM); lpMemPTD->hGlbQuadDM = NULL;

                // In with the new
                lpMemPTD->hGlbQuadDM = ClrPtrTypeDirAsGlb ((HGLOBAL) aplLongestObj);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
            } else
            {
                // Out with the old
                if (!lpSymEntry->stFlags.Imm)
                {
                    FreeResultGlobalVar (lpSymEntry->stData.stGlbData); lpSymEntry->stData.stGlbData = NULL;
                } // End IF

                // In with the new
                if (bImmed)
                {
                    // Set the stFlags & stData
                    lpSymEntry->stFlags.Imm      = TRUE;
                    lpSymEntry->stFlags.Value    = TRUE;
                    lpSymEntry->stFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeObj);
                    lpSymEntry->stData.stLongest = aplLongestObj;
                } else
                {
                    // Set the stFlags & stData
                    lpSymEntry->stFlags.Imm      = FALSE;
                    lpSymEntry->stFlags.Value    = TRUE;
                    lpSymEntry->stFlags.ImmType  = 0;
                    lpSymEntry->stData.stLongest = aplLongestObj;
                } // End IF/ELSE
            } // End IF

            // If we're copying a singla name, ...
            if (!bAllNames)
                // Mark as successful (we found a match)
                return 0;
        } // End IF
    } // End FOR

    if (!bAllNames)
        // Mark as no match, keep processing
        return 1;
    else
        // Mark as successful (we copied all the vars)
        return 0;
ERRMSG_EXIT:
    // Mark as in error
    return -1;
} // End CopyWsVars


//***************************************************************************
//  $CopyWsFcns
//
//  Copy all fcns/oprs from a workspace or just those which match a given name
//***************************************************************************

int CopyWsFcns
    (LPWCHAR     lpwNameInCmd,              // Ptr to name in command line (may be NULL if bAllNames)
     HWND        hWndEC,                    // Edit Control for SM window handle
     LPWCHAR    *lplpwErrMsg,               // Ptr to ptr to (constant) error message text
     BOOL        bAllNames,                 // TRUE if we should process all names
     LPSYMENTRY *lplpSymLink,               // Ptr to ptr to SYMENTRY link
     WCHAR       wszTailDPFE[])             // Save area for canonical form of given ws name

{
    WCHAR        wszCount[8];               // Save area for formatted uSymVar/Fcn counter
    LPWCHAR      lpwNameInWrk,              // Ptr to name in workspace file
                 lpwDataInWrk;              // Ptr to key data from the saved workspace file
    UINT         uSymFcn,                   // FcnOpr section counter
                 uCnt;                      // Loop counter
    LPSYMENTRY   lpSymEntry;                // Ptr to name in command's SYMENTRY
    STFLAGS      stFlags = {0};             // STE flags

    // Get the [Fcns.0] count
    uSymFcn =
      GetPrivateProfileIntW (SECTNAME_FCNS L".0",       // Ptr to the section name
                             KEYNAME_COUNT,             // Ptr to the key name
                             0,                         // Default value if not found
                             wszTailDPFE);              // Ptr to the file name
    // Loop through the [Fcns.0] section
    for (uCnt = 0; uCnt < uSymFcn; uCnt++)
    {
        NAME_TYPES nameType;

        // Format the counter
        wsprintfW (wszCount, L"%d", uCnt);

        // Point to the name in the workspace
        lpwNameInWrk = lpwszTemp;

        // Read the next string
        GetPrivateProfileStringW (SECTNAME_FCNS L".0",  // Ptr to the section name
                                  wszCount,             // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  lpwNameInWrk,         // Ptr to the output buffer
                                  memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                  wszTailDPFE);         // Ptr to the file name
        // Find the separator after the name and zap it
        lpwDataInWrk = strchrW (lpwNameInWrk, L'=');
        *lpwDataInWrk++ = L'\0';

        // Convert the {name}s and other chars to UTF16_xxx
        ConvertNameInPlace (lpwNameInWrk);

        // Compare the names to see if we have a match
        if (bAllNames || lstrcmpW (lpwNameInWrk, lpwNameInCmd) EQ 0)
        {
            // Parse the name type (2 = FN0, 3 = FN12, 4 = OP1, 5 = OP2, 6 = OP3)
            nameType = *lpwDataInWrk++ - '0';

            Assert (*lpwDataInWrk EQ L'='); lpwDataInWrk++;

            // Set the flags for what we're looking up
            stFlags.Inuse   = TRUE;
            stFlags.ObjName = OBJNAME_USR;

            // Lookup the name in the symbol table
            lpSymEntry =
              SymTabLookupName (lpwNameInWrk, &stFlags);

            // If the name is not found, it must be a user name
            Assert ((stFlags.ObjName EQ OBJNAME_USR) || lpSymEntry NE NULL);

            // If the name is not found, append it as a user name
            if (lpSymEntry EQ NULL)
            {
                // Append the name to get a new LPSYMENTRY
                lpSymEntry = SymTabAppendName_EM (lpwNameInWrk, &stFlags);

                // Set stFlags as appropriate
                lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
////////////////lpSymEntry->stFlags.stNameType = nameType;          // Set in ParseSavedWsFcn_EM
            } // End IF

            // Parse the line into lpSymEntry->stData
            if (!ParseSavedWsFcn_EM (lpwDataInWrk,      // Ptr to input buffer
                                     lpSymEntry,        // Ptr to STE for the object
                                     nameType,          // Function name type (see NAME_TYPES)
                                     hWndEC,            // Edit Control window handle
                                     lplpSymLink,       // Ptr to ptr to SYMENTRY link
                                     wszTailDPFE,       // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
                                     lplpwErrMsg))      // Ptr to ptr to (constant) error message text
                goto ERRMSG_EXIT;
            // If we're copying a singla name, ...
            if (!bAllNames)
                // Mark as successful (we found a match)
                return 0;
        } // End IF
    } // End FOR

    if (!bAllNames)
        // Mark as no match, keep processing
        return 1;
    else
        // Mark as successful (we copied all the vars)
        return 0;
ERRMSG_EXIT:
    // Mark as in error
    return -1;
} // End CopyWsFcns


//***************************************************************************
//  End of File: sc_copy.c
//***************************************************************************
