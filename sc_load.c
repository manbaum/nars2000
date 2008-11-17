//***************************************************************************
//  NARS2000 -- System Command:  )LOAD
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
#include "savefcn.h"
#include "fh_parse.h"
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CmdLoad_EM
//
//  Execute the system command:  )LOAD wsid
//***************************************************************************

UBOOL CmdLoad_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    return CmdLoadCom_EM (lpwszTail, TRUE);
} // End CmdLoad_EM


//***************************************************************************
//  $CmdXload_EM
//
//  Execute the system command:  )XLOAD wsid
//***************************************************************************

UBOOL CmdXload_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    return CmdLoadCom_EM (lpwszTail, FALSE);
} // End CmdXload_EM


//***************************************************************************
//  $CmdLoadCom_EM
//
//  Execute the system command:  )LOAD or )XLOAD wsid
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdLoadCom_EM"
#else
#define APPEND_NAME
#endif

UBOOL CmdLoadCom_EM
    (LPWCHAR lpwszTail,                 // Ptr to command line tail
     UBOOL   bExecLX)                   // TRUE iff execute []LX after successful load

{
    HGLOBAL      hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    WCHAR        wszTailDPFE[_MAX_PATH];// Save area for canonical form of given ws name
    LPWCHAR      lpw;                   // Temporary ptr
    int          iTabIndex;             // Tab index
    FILE        *fStream;               // Ptr to file stream for the plain text workspace file

    // Skip to the next blank
    lpw = SkipToCharW (lpwszTail, L' ');

    // Zap it in case there are trailing blanks
    *lpw = L'\0';

    // If there's no WSID, that's an error
    if (lpwszTail[0] EQ L'\0')
    {
        IncorrectCommand ();

        return FALSE;
    } // End IF

    // Convert the given workspace name into a canonical form (without WS_WKSEXT)
    MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, lpwszWorkDir);

    // Append the common workspace extension
    lstrcatW (wszTailDPFE, WS_WKSEXT);

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the tab index from which this command was issued
    iTabIndex = TranslateTabIDToIndex (lpMemPTD->CurTabID);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Handle WS NOT FOUND messages here
    // Attempt to open the workspace
    fStream = _wfopen (wszTailDPFE, L"r");

    // If the workspace doesn't exist, ...
    if (fStream EQ NULL)
        goto WSNOTFOUND_EXIT;

    // We no longer need this handle
    fclose (fStream); fStream = NULL;

    return
      CreateNewTab (hWndMF,             // Parent window handle
                    wszTailDPFE,        // Drive, Path, Filename, Ext of the workspace
                    iTabIndex + 1,      // Insert new tab to the left of this one
                    bExecLX);           // TRUE iff execute []LX after successful load
WSNOTFOUND_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_WS_NOT_FOUND APPEND_NAME);

    return FALSE;
} // End CmdLoadCom_EM
#undef  APPEND_NAME


//***************************************************************************
//  $LoadWorkspace_EM
//
//  Load a workspace
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- LoadWorkspace_EM"
#else
#define APPEND_NAME
#endif

UBOOL LoadWorkspace_EM
    (HGLOBAL hGlbDPFE,                  // Workspace DPFE global memory handle (NULL = CLEAR WS)
     HWND    hWndEC)                    // Edit Ctrl window handle

{
    LPWCHAR      lpwszDPFE,             // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
                 lpwErrMsg;             // Ptr to (constant) error message text
    FILE        *fStream;               // Ptr to file stream for the plain text workspace file
    UINT         uSymVar,               // Var section counter
                 uSymFcn,               // Fcn/Opr ...
                 uGlbCnt,               // [Globals] count
                 uSILevel,              // [General] SILevel value
                 uSID,                  // Loop counter
                 uCnt;                  // Loop counter
    WCHAR        wszCount[8],           // Save area for formatted uSymVar/Fcn counter
////             wszVersion[8],         // ...                     version info
                 wszSectName[15];       // ...                     section name (e.g., [Vars.nnn])
    UBOOL        bRet = FALSE,          // TRUE iff the result is valid
                 bImmed,                // TRUE iff the result of ParseSavedWsVar_EM is immediate
                 bSuspended;            // TRUE iff the function is suspended
    HGLOBAL      hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    APLSTYPE     aplTypeObj;            // Object storage type
    STFLAGS      stFlags = {0};         // SymTab flags
    LPSYMENTRY   lpSymEntry;            // Ptr to STE for HGLOBAL
    LPWCHAR      lpwCharEnd;            // Temporary ptr
    APLLONGEST   aplLongestObj;         // Object immediate value
    LPAPLLONGEST lpaplLongestObj;       // Ptr to ...
    LPSYMENTRY   lpSymLink = NULL;      // Ptr to anchor of SYMENTRY links for [Globals] values
                                        //   so we may delete them easily

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Check for CLEAR WS
    if (hGlbDPFE EQ NULL)
    {
        // Set to an empty vector which also looks like a CLEAR WS
        lpwszDPFE = L"";

        goto WSID_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpwszDPFE = MyGlobalLock (hGlbDPFE);

    // Check for CLEAR WS
    if (lpwszDPFE[0] EQ L'\0')
        goto WSID_EXIT;

    // Attempt to open the workspace
    fStream = _wfopen (lpwszDPFE, L"r");

    // If the workspace doesn't exist, ...
    if (fStream EQ NULL)
        goto WSNOTFOUND_EXIT;

    // We no longer need this handle
    fclose (fStream); fStream = NULL;

    // Make a backup copy of the workspace
    if (OptionFlags.bBackupOnLoad)
        MakeWorkspaceBackup (lpwszDPFE, LOADBAK_EXT);

////// Get the version #
////GetPrivateProfileStringW (SECTNAME_GENERAL,         // Ptr to the section name
////                          KEYNAME_VERSION,          // Ptr to the key name
////                          L"",                      // Ptr to the default value
////                          wszVersion,               // Ptr to the output buffer
////                          itemsizeof (wszVersion),  // Byte size of the output buffer
////                          lpwszDPFE);               // Ptr to the file name
    // Get the SI Level
    uSILevel =
      GetPrivateProfileIntW (SECTNAME_GENERAL,          // Ptr to the section name
                             KEYNAME_SILEVEL,           // Ptr to the key name
                             0,                         // Default value if not found
                             lpwszDPFE);                // Ptr to the file name
    __try
    {
        // Mark as suspended to handle the initial case
        bSuspended = TRUE;

        // Loop through the SI levels
        for (uSID = 0; uSID <= uSILevel; uSID++)
        {
            //***************************************************************
            // If this isn't the first SI level, create a new SIS struc
            //***************************************************************
            if (0 < uSID)
            {
                LPWCHAR      lpwSrc,                // Ptr to incoming data
                             lpwSrcStart,           // Ptr to starting point
                             lpwFcnName,            // Ptr to function name
                             lpwFcnLine;            // Ptr to function line #
                UINT         uMaxSize,              // Maximum size of lpwSrc
                             uLineNum;              // Function line #
                UBOOL        bSuspended;            // TRUE iff the function is suspended
                LPSYMENTRY   lpSymEntry;            // Ptr to function SYMENTRY
                HGLOBAL      hGlbDfnHdr;            // Defined function global memory handle
                LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator header

                // Lock the memory to get a ptr to it
                lpMemPTD = MyGlobalLock (hGlbPTD);

////////////////__try
////////////////{
////////////////    uMaxSize = 1 / (uSID - 1);
////////////////} __except (CheckException (GetExceptionInformation (), L"LoadWorkspace_EM"))
////////////////{
////////////////    // Display message for unhandled exception
////////////////    DisplayException ();
////////////////} // End __try/__Except

                // If there are more SI levels and the previous level was suspended,
                //   create a new SIS struc for immediate execution mode
                if (uSID < uSILevel
                 && bSuspended)
                    // Fill in the SIS header for Immediate Execution Mode
                    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                                NULL,                   // Semaphore handle
                                DFNTYPE_IMM,            // DfnType
                                FCNVALENCE_IMM,         // FcnValence
                                FALSE,                  // Suspended
                                FALSE,                  // Restartable
                                TRUE);                  // LinkIntoChain
                // Save ptr & maximum size
                lpwSrc   = lpMemPTD->lpwszTemp;
                uMaxSize = lpMemPTD->uTempMaxSize;
                lpwSrcStart = lpwSrc;

                // Format the counter
                wsprintfW (wszCount, L"%d", uSID - 1);

                // Read in the SI line
                GetPrivateProfileStringW (SECTNAME_SI,          // Ptr to the section name
                                          wszCount,             // Ptr to the key name
                                          L"",                  // Ptr to the default value
                                          lpwSrc,               // Ptr to the output buffer
                                          uMaxSize,             // Byte size of the output buffer
                                          lpwszDPFE);           // Ptr to the file name
                // Copy as ptr to function name or Quad
                lpwFcnName = lpwSrc;

                // Convert the {name}s and other chars to UTF16_xxx
                (void) ConvertNameInPlace (lpwFcnName);

                // Find the trailing marker of the function name
                lpwFcnLine = strchrW (lpwFcnName, L'[');

                // Check for Quad suspension first
                if ((lpwFcnName[0] NE L'\0')
                 && (lstrcmpW (lpwFcnName, WS_UTF16_QUAD) EQ 0))
                {
                    // Fill in the SIS header for Quad Input Mode
                    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                                NULL,                   // Semaphore handle
                                DFNTYPE_QUAD,           // DfnType
                                FCNVALENCE_NIL,         // FcnValence
                                TRUE,                   // Suspended
                                FALSE,                  // Restartable
                                TRUE);                  // LinkIntoChain
                    // If this is the last SI level, mark as not executing []LX
                    if (uSID EQ (uSILevel - 1))
                        lpMemPTD->bExecLX = FALSE;
                } else
                // If there's no left bracket, this SI level is uninteresting
                if (lpwFcnLine NE NULL)
                {
                    // Zap the trailing marker
                    *lpwFcnLine++ = L'\0';

                    // Get the function line #
                    swscanf (lpwFcnLine,
                             L"%u",
                            &uLineNum);
                    // Set the flags for what we're looking up
                    ZeroMemory (&stFlags, sizeof (stFlags));
                    stFlags.Inuse   = TRUE;
                    stFlags.ObjName = OBJNAME_USR;

                    // Look up the function name
                    lpSymEntry =
                      SymTabLookupName (lpwFcnName, &stFlags);

////////////////////if (!lpSymEntry)            // ***FIXME***

                    // Get a ptr to the function header
                    hGlbDfnHdr = ClrPtrTypeDirAsGlb (lpSymEntry->stData.stGlbData);

                    // Lock the memory to get a ptr to it
                    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

                    // Get suspended state
                    bSuspended = strchrW (lpwFcnLine, L'*') NE NULL;

                    // Fill in the SIS header for a User-Defined Function/Operator
                    FillSISNxt (lpMemPTD,                   // Ptr to PerTabData global memory
                                NULL,                       // Semaphore handle (Filled in by ExecuteFunction_EM_YY)
                                lpMemDfnHdr->DfnType,       // DfnType
                                lpMemDfnHdr->FcnValence,    // FcnValence
                                bSuspended,                 // TRUE iff suspended
                                FALSE,                      // Restartable
                                FALSE);                     // LinkIntoChain
                    // Fill in the non-default SIS header entries
                    lpMemPTD->lpSISNxt->hGlbDfnHdr   = hGlbDfnHdr;
                    lpMemPTD->lpSISNxt->hGlbFcnName  = lpMemDfnHdr->steFcnName->stHshEntry->htGlbName;
                    lpMemPTD->lpSISNxt->DfnAxis      = lpMemDfnHdr->DfnAxis;
                    lpMemPTD->lpSISNxt->PermFn       = lpMemDfnHdr->PermFn;
                    lpMemPTD->lpSISNxt->CurLineNum   = uLineNum;
                    lpMemPTD->lpSISNxt->NxtLineNum   = uLineNum + 1;
////////////////////lpMemPTD->lpSISNxt->numLabels    =              // Filled in by LocalizeAll
                    lpMemPTD->lpSISNxt->numFcnLines  = lpMemDfnHdr->numFcnLines;
////////////////////lpMemPTD->lpSISNxt->lpSISNxt     =              // Filled in by LocalizeAll
#ifdef DEBUG
                    dprintfW (L"~~Localize:    %p (%s)", lpMemPTD->lpSISNxt, L"LoadWorkspace_EM");
#endif
                    lpMemPTD->lpSISCur               = lpMemPTD->lpSISNxt;

                    // Localize all arguments, results, and locals
                    LocalizeAll (lpMemPTD, lpMemDfnHdr);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
                } // End IF

                // We no longer need this ptr
                MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
            } // End IF

            //***************************************************************
            // Load the variables at this SI level
            //***************************************************************

            // Format the section name
            wsprintfW (wszSectName, SECTNAME_VARS L".%d", uSID);

            // Get the [Vars.nnn] count
            uSymVar =
              GetPrivateProfileIntW (wszSectName,               // Ptr to the section name
                                     KEYNAME_COUNT,             // Ptr to the key name
                                     0,                         // Default value if not found
                                     lpwszDPFE);                // Ptr to the file name
            // Loop through the [Vars.sss] section where sss is the SI level
            for (uCnt = 0; uCnt < uSymVar; uCnt++)
            {
                LPWCHAR lpwSrc,             // Ptr to incoming data
                        lpwSrcStart;        // Ptr to starting point
                UINT    uMaxSize;           // Maximum size of lpwSrc

                // Lock the memory to get a ptr to it
                lpMemPTD = MyGlobalLock (hGlbPTD);

                // Save ptr & maximum size
                lpwSrc   = lpMemPTD->lpwszTemp;
                uMaxSize = lpMemPTD->uTempMaxSize;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                // Save the starting point
                lpwSrcStart = lpwSrc;

                // Format the counter
                wsprintfW (wszCount, L"%d", uCnt);

                // Read the next string
                GetPrivateProfileStringW (wszSectName,          // Ptr to the section name
                                          wszCount,             // Ptr to the key name
                                          L"",                  // Ptr to the default value
                                          lpwSrc,               // Ptr to the output buffer
                                          uMaxSize,             // Byte size of the output buffer
                                          lpwszDPFE);           // Ptr to the file name
                // Look for the name separator (L'=')
                lpwCharEnd = strchrW (lpwSrc, L'=');
                Assert (lpwCharEnd NE NULL);

                // Zap to form zero-terminated name
                *lpwCharEnd = L'\0';

                // Convert the {name}s and other chars to UTF16_xxx
                lpwSrc = ConvertNameInPlace (lpwSrc);

                // Skip over the zapped L'='
                lpwSrc++;

                // Set the flags for what we're looking up/appending
                ZeroMemory (&stFlags, sizeof (stFlags));
                stFlags.Inuse   = TRUE;

                if (IsSysName (lpwSrcStart))
                    // Tell 'em we're looking for system names
                    stFlags.ObjName = OBJNAME_SYS;
                else
                    // Tell 'em we're looking for user names
                    stFlags.ObjName = OBJNAME_USR;

                // Lookup the name in the symbol table
                lpSymEntry =
                  SymTabLookupName (lpwSrcStart, &stFlags);

                // If the name is not found, it must be a user name
                Assert ((stFlags.ObjName EQ OBJNAME_USR) || lpSymEntry NE NULL);

                // If the name is not found, append it as a user name
                if (lpSymEntry EQ NULL)
                {
                    // Append the name to get a new LPSYMENTRY
                    lpSymEntry = SymTabAppendName_EM (lpwSrcStart, &stFlags);
                    if (!lpSymEntry)
                        goto ERROR_EXIT;

                    // Mark the SYMENTRY as immediate so we don't free the
                    //   (non-existant) stGlbData
                    // Set other flags as appropriate
                    lpSymEntry->stFlags.Imm        = TRUE;
                } // End IF

                // Clear so we save a clean value
                aplLongestObj = 0;

                // Set this value as lpaplLongestObj is incremented by ParseSavedWsVar_EM
                lpaplLongestObj = &aplLongestObj;

                // Parse the value into aplLongestObj and aplTypeObj
                lpwSrc =
                  ParseSavedWsVar_EM (lpwSrc,           // Ptr to input buffer
                                      uMaxSize,         // Maximum size of lpwSrc
                                     &lpaplLongestObj,  // Ptr to ptr to output element
                                     &aplTypeObj,       // Ptr to storage type (may be NULL)
                                     &bImmed,           // Ptr to immediate flag (TRUE iff result is immediate) (may be NULL)
                                      FALSE,            // TRUE iff to save SymTabAppend values, FALSE to save values directly
                                      hWndEC,           // Edit Ctrl window handle
                                     &lpSymLink,        // Ptr to ptr to SYMENTRY link
                                      lpwszDPFE,        // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
                                     &lpwErrMsg);       // Ptr to ptr to (constant error message text
                if (lpwSrc EQ NULL)
                    goto ERRMSG_EXIT;

                // If it's []DM, handle it specially
                if (lstrcmpiW (lpwSrcStart, WS_UTF16_QUAD L"dm") EQ 0)
                {
                    Assert (!bImmed);
                    Assert (IsSimpleChar (aplTypeObj));

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
                    if (lpSymEntry->stFlags.Value && !lpSymEntry->stFlags.Imm)
                    {
                        FreeResultGlobalVar (lpSymEntry->stData.stGlbData); lpSymEntry->stData.stGlbData = NULL;
                    } // End IF

                    // In with the new
                    if (bImmed)
                        lpSymEntry->stFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeObj);
                    else
                        lpSymEntry->stFlags.ImmType  = IMMTYPE_ERROR;
                    // Set the common values
                    lpSymEntry->stFlags.Imm        = bImmed;
                    lpSymEntry->stFlags.Value      = TRUE;
                    lpSymEntry->stFlags.ObjName    = (IsSysName (lpwSrcStart) ? OBJNAME_SYS : OBJNAME_USR);
                    lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
                    lpSymEntry->stData.stLongest   = aplLongestObj;
                } // End IF

                // Restore the original value
                *lpwCharEnd = L'=';
            } // End FOR

            //***************************************************************
            // Load the functions at this SI level
            //***************************************************************

            // Format the section name
            wsprintfW (wszSectName, SECTNAME_FCNS L".%d", uSID);

            // Get the [Fcns.nnn] count
            uSymFcn =
              GetPrivateProfileIntW (wszSectName,               // Ptr to the section name
                                     KEYNAME_COUNT,             // Ptr to the key name
                                     0,                         // Default value if not found
                                     lpwszDPFE);                // Ptr to the file name
            // Loop through the [Fcns.sss] section where sss is the SI level
            for (uCnt = 0; uCnt < uSymFcn; uCnt++)
            {
                LPWCHAR lpwSrc,             // Ptr to incoming data
                        lpwSrcStart;        // Ptr to starting point
                UINT    uMaxSize;           // Maximum size of lpwSrc
                NAME_TYPES nameType;

                // Lock the memory to get a ptr to it
                lpMemPTD = MyGlobalLock (hGlbPTD);

                // Save ptr & maximum size
                lpwSrc   = lpMemPTD->lpwszTemp;
                uMaxSize = lpMemPTD->uTempMaxSize;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                // Save the starting point
                lpwSrcStart = lpwSrc;

                // Format the counter
                wsprintfW (wszCount, L"%d", uCnt);

                // Read the next string
                GetPrivateProfileStringW (wszSectName,          // Ptr to the section name
                                          wszCount,             // Ptr to the key name
                                          L"",                  // Ptr to the default value
                                          lpwSrc,               // Ptr to the output buffer
                                          uMaxSize,             // Byte size of the output buffer
                                          lpwszDPFE);           // Ptr to the file name
                // Look for the name separator (L'=')
                lpwCharEnd = strchrW (lpwSrc, L'=');
                Assert (lpwCharEnd NE NULL);

                // Zap to form zero-terminated name
                *lpwCharEnd = L'\0';

                // Convert the {name}s and other chars to UTF16_xxx
                lpwSrc = ConvertNameInPlace (lpwSrc);

                // Skip over the zapped L'='
                lpwSrc++;

                // Parse the name type (2 = FN0, 3 = FN12, 4 = OP1, 5 = OP2, 6 = OP3)
                nameType = *lpwSrc++ - '0';

                Assert (*lpwSrc EQ L'='); lpwSrc++;

                // Set the flags for what we're looking up
                ZeroMemory (&stFlags, sizeof (stFlags));
                stFlags.Inuse   = TRUE;
                stFlags.ObjName = OBJNAME_USR;

                // Lookup the name in the symbol table
                lpSymEntry =
                  SymTabLookupName (lpwSrcStart, &stFlags);

                // If the name is not found, it must be a user name
                Assert ((stFlags.ObjName EQ OBJNAME_USR) || lpSymEntry NE NULL);

                // If the name is not found, append it as a user name
                if (lpSymEntry EQ NULL)
                {
                    // Append the name to get a new LPSYMENTRY
                    lpSymEntry = SymTabAppendName_EM (lpwSrcStart, &stFlags);
                    if (!lpSymEntry)
                        goto ERROR_EXIT;

                    // Set stFlags as appropriate
                    lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
////////////////////lpSymEntry->stFlags.stNameType = nameType;          // Set in ParseSavedWsFcn_EM
                } // End IF

                // Parse the line into lpSymEntry->stData
                if (!ParseSavedWsFcn_EM (lpwSrc,        // Ptr to input buffer
                                         uMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart), // Maximum size of lpwSrc
                                         lpSymEntry,    // Ptr to STE for the object
                                         nameType,      // Function name type (see NAME_TYPES)
                                         hWndEC,        // Edit Ctrl window handle
                                        &lpSymLink,     // Ptr to ptr to SYMENTRY link
                                         lpwszDPFE,     // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
                                        &lpwErrMsg))    // Ptr to ptr to (constant) error message text
                    goto ERRMSG_EXIT;
            } // End FOR
        } // End FOR
    } __except (CheckVirtAlloc (GetExceptionInformation (),
                                L"LoadWorkspace_EM"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except

    // Get [Globals] count
    uGlbCnt =
      GetPrivateProfileIntW (SECTNAME_GLOBALS,          // Ptr to the section name
                             KEYNAME_COUNT,             // Ptr to the key name
                             0,                         // Default value if not found
                             lpwszDPFE);                // Ptr to the file name
    // Delete the symbol table entries for vars/fcns we allocated of the form FMTSTR_GLBCNT
    DeleteGlobalLinks (lpSymLink);

    // Display the workspace timestamp
    DisplayWorkspaceStamp (lpwszDPFE);
WSID_EXIT:
    // Set the value of the new []WSID as lpwszDPFE
    bRet = SaveNewWsid_EM (lpwszDPFE);

    goto NORMAL_EXIT;

WSNOTFOUND_EXIT:
    // Send this (constant) error message to the previously outgoing tab
    SendMessageLastTab (ERRMSG_WS_NOT_FOUND APPEND_NAME, hGlbPTD);

    goto ERROR_EXIT;

ERRMSG_EXIT:
    // Send this (constant) error message to the previously outgoing tab
    SendMessageLastTab (lpwErrMsg, hGlbPTD);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbDPFE)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbDPFE); lpwszDPFE = NULL;
    } // End IF

    return bRet;
} // End LoadWorkspace_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ParseSavedWsFcn_EM
//
//  Parse a value of the form FMTSTR_GLBCNT or {name}...
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseSavedWsFcn_EM"
#else
#define APPEND_NAME
#endif

UBOOL ParseSavedWsFcn_EM
    (LPWCHAR     lpwSrc,                // Ptr to input buffer
     UINT        uMaxSize,              // Maximum size of lpwSrc
     LPSYMENTRY  lpSymObj,              // Ptr to STE for the object
     NAME_TYPES  nameType,              // Function name type (see NAME_TYPES)
     HWND        hWndEC,                // Edit Ctrl window handle
     LPSYMENTRY *lplpSymLink,           // Ptr to ptr to SYMENTRY link
     LPWCHAR     lpwszDPFE,             // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
     LPWCHAR    *lplpwErrMsg)           // Ptr to ptr to (constant) error message text

{
    WCHAR      wcTmp;                   // Temporary char
    LPWCHAR    lpwCharEnd,              // Temporary ptr
               lpwDataEnd;              // ...
    STFLAGS    stFlags = {0};           // SymTab flags
    LPSYMENTRY lpSymEntry;              // Ptr to STE for HGLOBAL
    HGLOBAL    hGlbObj,                 // Object global memory handle
               hGlbOld;                 // Old ...
    UBOOL      bRet = FALSE;            // TRUE iff result is valid

    // Tell 'em we're looking for )LOAD objects
////ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;

    // Mark as a function/operator
    lpSymObj->stFlags.stNameType = nameType;

    // Copy the old value
    hGlbOld = lpSymObj->stData.stGlbData;

    if (*lpwSrc EQ FMTCHR_LEAD)
    {
        // Find the trailing L' '
        lpwCharEnd = SkipToCharW (lpwSrc, L' ');
        lpwDataEnd = &lpwSrc[lstrlenW (lpwSrc)] + 1;

        // Save old next char, zap to form zero-terminated name
        wcTmp = *lpwCharEnd; *lpwCharEnd = L'\0';

        // Get the matching HGLOBAL
        lpSymEntry =
          SymTabLookupName (lpwSrc, &stFlags);

        // If it's not found or has no value, load it from the [Globals] section
        if (lpSymEntry EQ NULL || !lpSymEntry->stFlags.Value)
            hGlbObj =
              LoadWorkspaceGlobal_EM (lpwSrc,       // Ptr to keyname (FMTSTR_GLBCNT)
                                      lpwDataEnd,   // Ptr to next available byte
                                      uMaxSize - (APLU3264) ((LPBYTE) lpwDataEnd - (LPBYTE) lpwSrc), // Maximum size of lpwDataEnd
                                      hWndEC,       // Edit Ctrl window handle
                                      lplpSymLink,  // Ptr to ptr to SYMENTRY link
                                      lpwszDPFE,    // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
                                      lplpwErrMsg); // Ptr to ptr to (constant) error message text
        else
        {
            hGlbObj = lpSymEntry->stData.stGlbData;
            lpSymEntry = NULL;
        } // End IF/ELSE

        if (hGlbObj EQ NULL)
            goto CORRUPTWS_EXIT;

        // Restore the original value
        *lpwCharEnd = wcTmp;

        // Save in the result
        lpSymObj->stData.stGlbData = hGlbObj;

        // Increment the reference count
        DbgIncrRefCntDir (hGlbObj);
    } else
    {
        // Convert the single {name} or other char to UTF16_xxx
        if (L'{' EQ  *lpwSrc)
        {
            // Get the next char
            // Because we created this name, we can expect it to be found
            //   and so don't need to check for zero result
            wcTmp = SymbolNameToChar (lpwSrc);

            // Skip to the next field
            lpwSrc = SkipPastCharW (lpwSrc, L'}');
        } else
            wcTmp = *lpwSrc++;

        // Mark as immediate
        lpSymObj->stFlags.Imm = TRUE;

        // Mark as immediate type
        lpSymObj->stFlags.ImmType = TranslateNameTypeToImmType (nameType);

        // Save in the result
        lpSymObj->stData.stChar = wcTmp;

        // Ensure it has a value
        hGlbObj = NULL;
    } // End IF

    // If there's an old value and it's different, ...
    if (hGlbOld && hGlbOld NE hGlbObj)
    {
        // Free the old value
        FreeResultGlobalDFLV (hGlbOld); hGlbOld = NULL;
    } // End IF

    return TRUE;
CORRUPTWS_EXIT:
    *lplpwErrMsg = ERRMSG_CORRUPT_WS APPEND_NAME;

    return FALSE;
} // End ParseSavedWsFcn_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ParseSavedWsVar_EM
//
//  Parse a value of the form V FMTSTR_GLBCNT or V T N R S V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseSavedWsVar_EM"
#else
#define APPEND_NAME
#endif

LPWCHAR ParseSavedWsVar_EM
    (LPWCHAR     lpwSrc,                // Ptr to input buffer
     UINT        uMaxSize,              // Maximum size of lpwSrc
     LPVOID     *lplpMemObj,            // Ptr to ptr to output element
     LPAPLSTYPE  lpaplTypeObj,          // Ptr to storage type (may be NULL)
     LPUBOOL     lpbImmed,              // Ptr to immediate flag (TRUE iff result is immediate) (may be NULL)
     UBOOL       bSymTab,               // TRUE iff to save SymTabAppend values, FALSE to save values directly
     HWND        hWndEC,                // Edit Ctrl window handle
     LPSYMENTRY *lplpSymLink,           // Ptr to ptr to SYMENTRY link
     LPWCHAR     lpwszDPFE,             // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
     LPWCHAR    *lplpwErrMsg)           // Ptr to ptr to (constant error message text

{
    WCHAR        wcTmp;                 // Temporary char
    LPWCHAR      lpwCharEnd,            // Temporary ptr
                 lpwDataEnd;            // ...
    STFLAGS      stFlags = {0};         // SymTab flags
    LPSYMENTRY   lpSymEntry,            // Ptr to STE for HGLOBAL
                 lpSymTmp;              // Ptr to temporary LPSYMENTRY
    APLINT       aplInteger;            // Temporary integer
    APLSTYPE     aplTypeObj;            // Object storage type
    HGLOBAL      hGlbObj;               // Object global memory handle
    HGLOBAL      hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPWCHAR      lpwszFormat;       // Ptr to formatting save area

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Tell 'em we're looking for )LOAD objects
////ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;

    // If it's in the [Globals] section, ...
    if (*lpwSrc EQ FMTCHR_LEAD)
    {
        // Find the trailing L' '
        lpwCharEnd = SkipToCharW (lpwSrc, L' ');
        lpwDataEnd = &lpwSrc[lstrlenW (lpwSrc)] + 1;

        // Save old next char, zap to form zero-terminated name
        wcTmp = *lpwCharEnd; *lpwCharEnd = L'\0';

        // Get the matching HGLOBAL
        lpSymEntry =
          SymTabLookupName (lpwSrc, &stFlags);

        // If it's not found or has no value, load it from the [Globals] section
        if (lpSymEntry EQ NULL || !lpSymEntry->stFlags.Value)
        {
            hGlbObj =
              LoadWorkspaceGlobal_EM (lpwSrc,       // Ptr to keyname (FMTSTR_GLBCNT)
                                      lpwDataEnd,   // Ptr to next available byte
                                      uMaxSize - (APLU3264) ((LPBYTE) lpwDataEnd - (LPBYTE) lpwSrc), // Maximum size of lpwDataEnd
                                      hWndEC,       // Edit Ctrl window handle
                                      lplpSymLink,  // Ptr to ptr to SYMENTRY link
                                      lpwszDPFE,    // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
                                      lplpwErrMsg); // Ptr to ptr to (constant) error message text
            if (hGlbObj EQ NULL)
                goto ERROR_EXIT;
        } else
        {
            hGlbObj = lpSymEntry->stData.stGlbData; lpSymEntry = NULL;
            if (hGlbObj EQ NULL)
                goto CORRUPTWS_EXIT;
        } // End IF/ELSE

        // Restore the original value
        *lpwCharEnd = wcTmp;

        // Save in the result and skip over it
        *((LPAPLNESTED) *lplpMemObj)++ = hGlbObj;

        // Increment the reference count
        DbgIncrRefCntDir (hGlbObj);

        // Skip to the next field
        lpwSrc = &lpwCharEnd[1];

        // If the caller wants the storage type, ...
        if (lpaplTypeObj)
        {
            LPVOID  lpMemObj;           // Ptr to object global memory

            // stData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbObj));

            // Clear the type bits
            hGlbObj = ClrPtrTypeDirAsGlb (hGlbObj);

            // Lock the memory to get a ptr to it
            lpMemObj = MyGlobalLock (hGlbObj);

            // Save the storage type
#define lpHeader        ((LPVARARRAY_HEADER) lpMemObj)
            *lpaplTypeObj = lpHeader->ArrType;
#undef  lpHeader

            // We no longer need this ptr
            MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
        } // End IF

        // If the caller wants the immediate flag, ...
        if (lpbImmed)
            // Save as not immediate
            *lpbImmed = FALSE;
    } else
    {
        // Get the storage type from the immediate char
        aplTypeObj = TranslateCharToArrayType (*lpwSrc);

        // If the caller wants the storage type, ...
        if (lpaplTypeObj)
            // Save the storage type
            *lpaplTypeObj = aplTypeObj;

        // If the caller wants the immediate flag, ...
        if (lpbImmed)
            // Save as immediate
            *lpbImmed = TRUE;

        // Skip to the NELM
        lpwSrc = SkipPastCharW (lpwSrc, L' ');

        Assert (*lpwSrc EQ L'1');

        // Skip to the rank
        lpwSrc = SkipPastCharW (lpwSrc, L' ');

        Assert (*lpwSrc EQ L'0');

        // Skip to the value
        lpwSrc = SkipPastCharW (lpwSrc, L' ');

        // Split cases based upon the storage type
        switch (aplTypeObj)
        {
            case ARRAY_BOOL:        // Boolean
            case ARRAY_INT:         // Integer
                // Scan in the value
                swscanf (lpwSrc, SCANFSTR_APLINT, &aplInteger);

                // Skip to the next field
                lpwSrc = SkipPastCharW (lpwSrc, L' ');

                // If we're to save the SymTab, ...
                if (bSymTab)
                {
                    // Save in the result and skip over it
                    *((LPAPLHETERO) *lplpMemObj)++ =
                    lpSymTmp =
                      SymTabAppendInteger_EM (aplInteger);
                    if (!lpSymTmp)
                        goto ERROR_EXIT;
                } else
                    // Save the result directly
                    *((LPAPLINT) *lplpMemObj) = aplInteger;
                break;

            case ARRAY_CHAR:        // Character
                Assert (L'\'' EQ *lpwSrc); lpwSrc++;

                // Convert the single {name) or other char to UTF16_xxx
                if (L'{' EQ  *lpwSrc)
                {
                    // Get the next char
                    // Because we created this name, we can expect it to be found
                    //   and so don't need to check for zero result
                    wcTmp = SymbolNameToChar (lpwSrc);

                    // Skip to the next field
                    lpwSrc = SkipPastCharW (lpwSrc, L'}');
                } else
                    wcTmp = *lpwSrc++;

                Assert (L'\'' EQ *lpwSrc); lpwSrc++;

                // If we're to save the SymTab, ...
                if (bSymTab)
                {
                    // Save in the result and skip over it
                    *((LPAPLHETERO) *lplpMemObj)++ =
                    lpSymTmp =
                      SymTabAppendChar_EM (wcTmp);
                    if (!lpSymTmp)
                        goto ERROR_EXIT;
                } else
                    // Save the result directly
                    *((LPAPLCHAR) *lplpMemObj) = wcTmp;
                break;

            case ARRAY_FLOAT:       // Float
                // Find the trailing L' '
                lpwCharEnd = SkipToCharW (lpwSrc, L' ');

                // Save old next char, zap to form zero-terminated name
                wcTmp = *lpwCharEnd; *lpwCharEnd = L'\0';

                // Convert the format string to ASCII
                W2A ((LPCHAR) lpwszFormat, lpwSrc, DEF_WFORMAT_MAXSIZE - 1);

                // Restore the original value
                *lpwCharEnd = wcTmp;

                // If we're to save the SymTab, ...
                if (bSymTab)
                {
                    // Save in the result and skip over it
                    *((LPAPLHETERO) *lplpMemObj)++ =
                    lpSymTmp =
                      SymTabAppendFloat_EM (strtod ((LPCHAR) lpwszFormat, NULL));
                    if (!lpSymTmp)
                        goto ERROR_EXIT;
                } else
                    // Save the result directly
                    *((LPAPLFLOAT) *lplpMemObj) = strtod ((LPCHAR) lpwszFormat, NULL);

                // Skip to the next field
                lpwSrc = &lpwCharEnd[1];

                break;

            defstop
                break;
        } // End FOR/SWITCH
    } // End IF

    return lpwSrc;

CORRUPTWS_EXIT:
    *lplpwErrMsg = ERRMSG_CORRUPT_WS APPEND_NAME;

    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End ParseSavedWsVar_EM
#undef  APPEND_NAME


//***************************************************************************
//  $LoadWorkspaceGlobal_EM
//
//  Return the global memory handle corresponding
//    to an entry in the [Globals] section
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- LoadWorkspaceGlobal_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL LoadWorkspaceGlobal_EM
    (LPWCHAR     lpwGlbName,                // Ptr to keyname (FMTSTR_GLBCNT)
     LPWCHAR     lpwSrc,                    // Ptr to next available byte
     UINT        uMaxSize,                  // Maximum size of
     HWND        hWndEC,                    // Edit Ctrl window handle
     LPSYMENTRY *lplpSymLink,               // Ptr to ptr to SYMENTRY link
     LPWCHAR     lpwszDPFE,                 // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
     LPWCHAR    *lplpwErrMsg)               // Ptr to ptr to (constant) error message text

{
    APLSTYPE     aplTypeObj;                // Object storage type
    APLNELM      aplNELMObj;                // Object NELM
    APLRANK      aplRankObj;                // Object rank
    HGLOBAL      hGlbObj;                   // Object global memory handle
    APLUINT      ByteObj,                   // # bytes needed for the object
                 uObj;                      // Loop counter
    STFLAGS      stFlags = {0};             // SymTab flags
    LPSYMENTRY   lpSymEntry,                // Ptr to STE for HGLOBAL
                 lpSymLink;                 // Ptr to SYMENTRY temp for *lplpSymLink
    WCHAR        wcTmp,                     // Temporary char
                 wszTimeStamp[16 + 1];      // Output save area for time stamp
    LPWCHAR      lpwFcnName,                // Ptr to function name
                 lpwSectName,               // Ptr to section name
                 lpwSrcStart,               // Ptr to starting point
                 lpwCharEnd;                // Temporary ptr
    UINT         uBitIndex,                 // Bit index for looping through Boolean result
                 uLineCnt,                  // # lines in the current function including the header
                 uCnt,                      // Loop counter
                 Count;                     // Temporary count for monitor info
    FILETIME     ftCreation,                // Function creation time
                 ftLastMod;                 // ...      last modification time
    SYSTEMTIME   systemTime;                // Current system (UTC) time
    UBOOL        bUserDefined = FALSE;      // TRUE iff the durrent function is User-Defined
    LPVOID       lpMemObj;                  // Ptr to object global memory
    APLINT       aplInteger;                // Temporary integer
    HGLOBAL      hGlbPTD;                   // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory
    LPWCHAR      lpwszFormat;               // Ptr to formatting save area

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Save the starting ptr
    lpwSrcStart = lpwSrc;

    // Read the corresponding string from [Globals]
    GetPrivateProfileStringW (SECTNAME_GLOBALS,     // Ptr to the section name
                              lpwGlbName,           // Ptr to the key name
                              L"",                  // Ptr to the default value
                              lpwSrc,               // Ptr to the output buffer
                              uMaxSize,             // Byte size of the output buffer
                              lpwszDPFE);           // Ptr to the file name
    // Parse the array attributes
    // The result in lpwSrc is
    //   V T N R S value    for variables
    //   F nnn.Name         for functions

    // Ensure it's non-empty
    if  (lpwSrc[0] EQ L'\0')
        goto CORRUPTWS_EXIT;

    // Split cases based upon Variable vs. Function/Operator
    switch (*lpwSrc++)
    {
        case L'V':
            // ***FIXME*** -- Do we need to restore the object PermNdx?

            Assert (*lpwSrc EQ L' '); lpwSrc++;

            // Get the object storage type
            aplTypeObj = TranslateCharToArrayType (*lpwSrc++);

            // Ensure there's a valid separator
            if (*lpwSrc NE L' ')
                goto CORRUPTWS_EXIT;

            // Skip over it
            lpwSrc++;

            // Get the object NELM
            swscanf (lpwSrc, SCANFSTR_APLUINT, &aplNELMObj); lpwSrc = SkipBlackW (lpwSrc);

            // Ensure there's a valid separator
            if (*lpwSrc NE L' ')
                goto CORRUPTWS_EXIT;

            // Skip over it
            lpwSrc++;

            // Get the object rank
            swscanf (lpwSrc, SCANFSTR_APLUINT, &aplRankObj); lpwSrc = SkipBlackW (lpwSrc);

            // Ensure there's a valid separator
            if (*lpwSrc NE L' ')
                goto CORRUPTWS_EXIT;

            // Skip over it
            lpwSrc++;

            //***************************************************************
            // Calculate space needed for the object
            //***************************************************************
            ByteObj = CalcArraySize (aplTypeObj, aplNELMObj, aplRankObj);

            //***************************************************************
            // Now we can allocate the storage for the result
            // N.B.:  Conversion from APLUINT to UINT.
            //***************************************************************
            Assert (ByteObj EQ (APLU3264) ByteObj);
            hGlbObj = MyGlobalAlloc (GHND, (APLU3264) ByteObj);
            if (!hGlbObj)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemObj = MyGlobalLock (hGlbObj);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemObj)
            // Fill in the header
////////////// Note that the RefCnt is initialized to zero
////////////// It will be incremented upon each reference
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = aplTypeObj;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND ***FIXME*** -- Set correct PERMNDX_xxx
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMObj;
            lpHeader->Rank       = aplRankObj;
#undef  lpHeader

            // Skip over the header to the dimensions
            lpMemObj = VarArrayBaseToDim (lpMemObj);

            // Fill in the result's dimension
            for (uObj = 0; uObj < aplRankObj; uObj++)
            {
                // Scan in the next dimension
                swscanf (lpwSrc, SCANFSTR_APLUINT, lpMemObj);

                // Skip to the next field
                lpwSrc = SkipPastCharW (lpwSrc, L' ');

                // Skip over the dimension
                ((LPAPLDIM) lpMemObj)++;
            } // End FOR

            // lpwSrc now points to the data

            // Split cases based upon the object storage type
            switch (aplTypeObj)
            {
                case ARRAY_BOOL:
                    // Initialize the bit index
                    uBitIndex = 0;

                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                    {
                        // Scan in the next value
                        swscanf (lpwSrc, SCANFSTR_APLUINT, &aplInteger);

                        // Skip to the next field
                        lpwSrc = SkipPastCharW (lpwSrc, L' ');

                        // Save in the result
                        *((LPAPLBOOL) lpMemObj) |= aplInteger << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemObj)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:
                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                    {
                        // Scan in the next value and skip over it
                        swscanf (lpwSrc, SCANFSTR_APLINT, ((LPAPLINT) lpMemObj)++);

                        // Skip to the next field
                        lpwSrc = SkipPastCharW (lpwSrc, L' ');
                    } // End FOR

                    break;

                case ARRAY_FLOAT:
                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                    {
                        // Find the trailing L' '
                        lpwCharEnd = SkipToCharW (lpwSrc, L' ');

                        // Save old next char, zap to form zero-terminated name
                        wcTmp = *lpwCharEnd; *lpwCharEnd = L'\0';

                        // Convert the format string to ASCII
                        W2A ((LPCHAR) lpwszFormat, lpwSrc, DEF_WFORMAT_MAXSIZE - 1);

                        // Restore the original value
                        *lpwCharEnd = wcTmp;

                        // Use David Gay's routines
                        // Save in the result and skip over it
                        *((LPAPLFLOAT) lpMemObj)++ = strtod ((LPCHAR) lpwszFormat, NULL);

                        // Skip to the next field
                        lpwSrc = &lpwCharEnd[1];
                    } // End FOR

                    break;

                case ARRAY_CHAR:
                    Assert (L'\'' EQ *lpwSrc); lpwSrc++;

                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                    {
                        // Convert the single {name} or other char to UTF16_xxx
                        if (L'{' EQ  *lpwSrc)
                        {
                            // Get the next char
                            // Because we created this name, we can expect it to be found
                            //   and so don't need to check for zero result
                            wcTmp = SymbolNameToChar (lpwSrc);

                            // Skip to the next field
                            lpwSrc = SkipPastCharW (lpwSrc, L'}');
                        } else
                            wcTmp = *lpwSrc++;

                        // Save in the result and skip over it
                        *((LPAPLCHAR) lpMemObj)++ = wcTmp;
                    } // End FOR

                    Assert (L'\'' EQ *lpwSrc); lpwSrc++;

                    break;

                case ARRAY_APA:
                    // The next two values are the APA offset and multiplier

                    // Scan in the next value and skip over it
                    swscanf (lpwSrc, SCANFSTR_APLINT, ((LPAPLINT) lpMemObj)++);

                    // Skip to the next field
                    lpwSrc = SkipPastCharW (lpwSrc, L' ');

                    // Scan in the next value and skip over it
                    swscanf (lpwSrc, SCANFSTR_APLINT, ((LPAPLINT) lpMemObj)++);

                    // Skip to the next field
                    lpwSrc = SkipPastCharW (lpwSrc, L' ');

                    break;

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                    // The elements consist of either a simple scalar
                    //   (starts with one of the storage types), or a
                    //   global (starts with a FMTCHR_LEAD)

                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                        lpwSrc =
                          ParseSavedWsVar_EM (lpwSrc,       // Ptr to input buffer
                                              uMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart), // Maximum size of lpwSrc
                                             &lpMemObj,     // Ptr to ptr to output element
                                              NULL,         // Ptr to storage type (may be NULL)
                                              NULL,         // Ptr to immediate flag (TRUE iff result is immediate) (may be NULL)
                                              TRUE,         // TRUE iff to save SymTabAppend values, FALSE to save values directly
                                              hWndEC,       // Edit Ctrl window handle
                                              lplpSymLink,  // Ptr to ptr to SYMENTRY link
                                              lpwszDPFE,    // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
                                              lplpwErrMsg); // Ptr to ptr to (constant error message text
                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbObj); lpMemObj = NULL;

            // Set the ptr type bits
            hGlbObj = MakePtrTypeGlb (hGlbObj);

            break;

        case L'F':
            Assert (*lpwSrc EQ L' '); lpwSrc++;

            // Point to the section name of the form nnn.Name where nnn is the count
            lpwSectName = lpwSrc;

            // Get a ptr to the function name
            lpwFcnName = strchrW (lpwSectName, L'.') + 1;

            // Skip past the section name and its terminating zero
            lpwSrc = &lpwSectName[lstrlenW (lpwSectName) + 1];

            // Get the count for the section name
            uLineCnt =
              GetPrivateProfileIntW (lpwSectName,               // Ptr to the section name
                                     KEYNAME_COUNT,             // Ptr to the key name
                                     0,                         // Default value if not found
                                     lpwszDPFE);                // Ptr to the file name
            Assert (uLineCnt > 0);

            // Get the UserDefined flag
            bUserDefined =
              GetPrivateProfileIntW (lpwSectName,               // Ptr to the section name
                                     KEYNAME_USERDEFINED,       // Ptr to the key name
                                     0,                         // Default value if not found
                                     lpwszDPFE);                // Ptr to the file name
            // Get the current system (UTC) time
            GetSystemTime (&systemTime);

            // Convert system time to file time and save as creation time
            SystemTimeToFileTime (&systemTime, &ftCreation);

            // Format the CreationTime/LastModTime
            wsprintfW (wszTimeStamp,
                       FMTSTR_DATETIME,
                       ftCreation.dwHighDateTime,
                       ftCreation.dwLowDateTime);
            // Get the CreationTime string
            GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                                      KEYNAME_CREATIONTIME, // Ptr to the key name
                                      wszTimeStamp,         // Ptr to the default value
                                      lpwSrc,               // Ptr to the output buffer
                                      uMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart), // Maximum size of lpwSrc
                                      lpwszDPFE);           // Ptr to the file name
            // Convert the CreationTime string to time
            swscanf (lpwSrc, SCANFSTR_TIMESTAMP, &ftCreation);

            // Get the LastModTime string
            GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                                      KEYNAME_LASTMODTIME,  // Ptr to the key name
                                      wszTimeStamp,         // Ptr to the default value
                                      lpwSrc,               // Ptr to the output buffer
                                      uMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart), // Maximum size of lpwSrc
                                      lpwszDPFE);           // Ptr to the file name
            // Convert the LastModTime string to time
            swscanf (lpwSrc, SCANFSTR_TIMESTAMP, &ftLastMod);

            // If it's a user-defined function/operator, ...
            if (bUserDefined)
            {
                SF_FCNS    SF_Fcns = {0};       // Common struc for SaveFunctionCom
                LW_PARAMS  LW_Params = {0};     // Local  ...
                LPWCHAR    lpMemUndoTxt;        // Ptr to Undo Buffer in text format

                // Save ptr to undo buffer in text format
                lpMemUndoTxt = lpwSrc;

                // Get the Undo buffer string, and
                //   skip over the buffer (including the trailing zero)
                lpwSrc += 1 +
                  GetPrivateProfileStringW (lpwSectName,        // Ptr to the section name
                                            KEYNAME_UNDO,       // Ptr to the key name
                                            L"",                // Ptr to the default value
                                  (LPWCHAR) lpMemUndoTxt,       // Ptr to the output buffer
                                            uMaxSize - (APLU3264) ((LPBYTE) lpMemUndoTxt - (LPBYTE) lpwSrcStart), // Maximum size of lpMemUndoTxt
                                            lpwszDPFE);         // Ptr to the file name
                // Fill in common values
                SF_Fcns.bDisplayErr     = TRUE;             // Display Errors
////////////////SF_Fcns.bRet            =                   // Filled in by SaveFunctionCom
////////////////SF_Fcns.uErrLine        =                   // ...
////////////////SF_Fcns.lpSymName       =                   // ...
                SF_Fcns.lptkFunc        = NULL;             // Ptr to function token ***FIXME*** -- Used on error
                SF_Fcns.SF_LineLen      = SF_LineLenLW;     // Ptr to line length function
                SF_Fcns.SF_ReadLine     = SF_ReadLineLW;    // Ptr to read line function
                SF_Fcns.SF_NumLines     = SF_NumLinesLW;    // Ptr to get # lines function
                SF_Fcns.SF_CreationTime = SF_CreationTimeLW;// Ptr to get function creation time
                SF_Fcns.SF_LastModTime  = SF_LastModTimeLW; // Ptr to get function last modification time
                SF_Fcns.SF_UndoBuffer   = SF_UndoBufferLW;  // Ptr to get function last modification time

                // Fill in local values
                LW_Params.lpwSectName  = lpwSectName;    // Ptr to section name
                LW_Params.lpwszDPFE    = lpwszDPFE;      // Ptr to workspace DPFE
                LW_Params.lpwBuffer    = lpwSrc;         // Ptr to buffer
                LW_Params.lpMemUndoTxt = lpMemUndoTxt;   // Ptr to Undo Buffer in text format
                LW_Params.uMaxSize     = uMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart); // Maximum size of lpwSrc
                LW_Params.ftCreation   = ftCreation;     // Function Creation Time
                LW_Params.ftLastMod    = ftLastMod;      // Function Last Modification Time

                // Save ptr to local parameters
                SF_Fcns.LclParams   = &LW_Params;

                // Call common routine
                if (!SaveFunctionCom (NULL, &SF_Fcns))
                {
                    // Format the error message
                    wsprintfW (lpwszFormat,
                               L"Error loading function <%s> line <%d> in workspace <%s> -- WORKSPACE MAY BE CORRUPT:  PROCEED WITH CAUTION",
                               lpwFcnName,
                               SF_Fcns.uErrLine,
                               lpwszDPFE);
                    MBW (lpwszFormat);

                    goto CORRUPTWS_EXIT;
                } // End IF/ELSE

                // Read in and process the monitor info
                GetPrivateProfileStringW (lpwSectName,        // Ptr to the section name
                                          KEYNAME_MONINFO,    // Ptr to the key name
                                          L"",                // Ptr to the default value
                                (LPWCHAR) lpMemUndoTxt,       // Ptr to the output buffer
                                          uMaxSize - (APLU3264) ((LPBYTE) lpMemUndoTxt - (LPBYTE) lpwSrcStart), // Maximum size of lpMemUndoTxt
                                          lpwszDPFE);         // Ptr to the file name
                // If there's monitor info, ...
                if (lpMemUndoTxt[0])
                {
                    LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator header
                    LPINTMONINFO lpMemMonInfo;          // Ptr to function line monitoring info

                    // Lock the memory to get a ptr to it
                    lpMemDfnHdr = MyGlobalLock (SF_Fcns.hGlbDfnHdr);

                    // Allocate space for the monitor info
                    lpMemDfnHdr->hGlbMonInfo =
                      MyGlobalAlloc (GHND, (lpMemDfnHdr->numFcnLines + 1) * sizeof (INTMONINFO));
                    if (lpMemDfnHdr->hGlbMonInfo)
                    {
                        // Lock the memory to get a ptr to it
                        lpMemMonInfo = MyGlobalLock (lpMemDfnHdr->hGlbMonInfo);

                        // Loop through the function header & lines
                        for (uCnt = 0; uCnt < uLineCnt; uCnt++, lpMemMonInfo++)
                        {
                            // Scan in the first field
                            swscanf (lpMemUndoTxt,
                                     L"%I64u %I64u %u",
                                    &lpMemMonInfo->IncSubFns,
                                    &lpMemMonInfo->ExcSubFns,
                                    &Count);
                            // Save the count
                            lpMemMonInfo->Count = Count;

                            // Increment past the input fields
                            lpMemUndoTxt = SkipPastCharW (lpMemUndoTxt, L',');
                        } // End FOR

                        // We no longer need this ptr
                        MyGlobalUnlock (lpMemDfnHdr->hGlbMonInfo); lpMemMonInfo = NULL;

                        // Mark as monitor info present
                        lpMemDfnHdr->MonOn = TRUE;
                    } // End IF

                    // We no longer need this ptr
                    MyGlobalUnlock (SF_Fcns.hGlbDfnHdr); lpMemDfnHdr = NULL;
                } // End IF
            } else
            // It's a function array
            {
                LPWCHAR          lpwLine;           // Ptr to line to execute
                HGLOBAL          hGlbPTD;           // PerTabData global memory handle
                LPPERTABDATA     lpMemPTD;          // Ptr to PerTabData global memory
                LOADWSGLBVARPARM LoadWsGlbVarParm;  // Extra parms for LoadWsGlbVarConv
#ifdef DEBUG
                EXIT_TYPES exitType;
#endif
                // Append the variable name and an assignment arrow to the output buffer
                lpwLine = lpwSrc;
                lstrcpyW (lpwLine, lpwFcnName);
                lpwSrc += lstrlenW (lpwSrc);
                *lpwSrc++ = UTF16_LEFTARROW;

                // Get the one (and only) line
                GetPrivateProfileStringW (lpwSectName,        // Ptr to the section name
                                          L"0",               // Ptr to the key name
                                          L"",                // Ptr to the default value
                                          lpwSrc,             // Ptr to the output buffer
                                          uMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart),// Maximum size of lpwSrc
                                          lpwszDPFE);         // Ptr to the file name
                // Convert in place
                lpwSrcStart = lpwSrc;

                // Convert the {name}s and other chars to UTF16_xxx
                (void) ConvertNameInPlace (lpwSrc);

                // Fill in the extra parms
                LoadWsGlbVarParm.lpwSrc        = lpwSrc;
                LoadWsGlbVarParm.uMaxSize      = uMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart); // Maximum size of lpwSrc
                LoadWsGlbVarParm.hWndEC        = hWndEC;
                LoadWsGlbVarParm.lplpSymLink   = lplpSymLink;
                LoadWsGlbVarParm.lpwszDPFE     = lpwszDPFE;
                LoadWsGlbVarParm.lplpwErrMsg   = lplpwErrMsg;

                // Get the PerTabData global memory handle
                hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

                // Lock the memory to get a ptr to it
                lpMemPTD = MyGlobalLock (hGlbPTD);

                // Save in PerTabData struc
                lpMemPTD->lpLoadWsGlbVarParm = &LoadWsGlbVarParm;
                lpMemPTD->lpLoadWsGlbVarConv = &LoadWsGlbVarConv;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

                // Execute the statement
#ifdef DEBUG
                exitType =
#endif
                  ImmExecStmt (lpwLine,             // Ptr to line to execute
                               lstrlenW (lpwLine),  // NELM of lpwLine
                               FALSE,               // Free lpwLine on completion
                               TRUE,                // TRUE iff wait until finished
                               hWndEC,              // Edit Ctrl window handle
                               FALSE);              // TRUE iff errors are acted upon
                Assert (exitType EQ EXITTYPE_NOVALUE);

                // Lock the memory to get a ptr to it
                lpMemPTD = MyGlobalLock (hGlbPTD);

                // Save in PerTabData struc
                lpMemPTD->lpLoadWsGlbVarParm = NULL;
                lpMemPTD->lpLoadWsGlbVarConv = NULL;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
            } // End IF/ELSE

            // Lookup the STE and get its HGLOBAL
            //   so we can save it in the STE for FMTSTR_GLBCNT

            // Set the flags for what we're looking up
            ZeroMemory (&stFlags, sizeof (stFlags));
            stFlags.Inuse   = TRUE;
            stFlags.ObjName = OBJNAME_USR;

            // Lookup the name in the symbol table
            lpSymEntry =
              SymTabLookupName (lpwFcnName, &stFlags);
            Assert (lpSymEntry NE NULL);

            // Copy the HGLOBAL
            hGlbObj = lpSymEntry->stData.stGlbData;

            // If it's not a user-defined function, ...
            if (!bUserDefined)
                // Increment the reference count
                DbgIncrRefCntDir (hGlbObj);
            break;

        defstop
            break;
    } // End SWITCH

    // Set the flags for what we're appending
    ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;

    // Create a symbol table entry for the )LOAD HGLOBAL
    lpSymEntry =
      SymTabAppendName_EM (lpwGlbName, &stFlags);
    if (!lpSymEntry)
        goto ERROR_EXIT;

    // Set the handle
    lpSymEntry->stData.stGlbData = hGlbObj;

    // Link this SYMENTRY into the chain
    //   unless it's already linked
    if (lpSymEntry->stSymLink EQ NULL)
    {
        lpSymLink             = *lplpSymLink;
       *lplpSymLink           =  lpSymEntry;
        lpSymEntry->stSymLink =  lpSymLink;
    } // End IF

    return hGlbObj;

CORRUPTWS_EXIT:
    *lplpwErrMsg = ERRMSG_CORRUPT_WS APPEND_NAME;

    goto ERROR_EXIT;

WSFULL_EXIT:
    *lplpwErrMsg = ERRMSG_WS_FULL APPEND_NAME;

    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End LoadWorkspaceGlobal_EM
#undef  APPEND_NAME


//***************************************************************************
//  $LoadWsGlbVarConv
//
//  Callback function for ParseLine to convert a FMTSTR_GLBCNT to an HGLOBAL
//***************************************************************************

HGLOBAL LoadWsGlbVarConv
    (UINT               uGlbCnt,
     LPLOADWSGLBVARPARM lpLoadWsGlbVarParm)

{
    WCHAR      wszGlbCnt[8 + 1];        // Save area for formatted uGlbCnt
    STFLAGS    stFlags = {0};           // SymTab flags
    LPSYMENTRY lpSymEntry;              // Ptr to STE for HGLOBAL

    // Tell 'em we're looking for )LOAD objects
////ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;

    // Format the global count
    wsprintfW (wszGlbCnt,
               FMTSTR_GLBCNT,
               uGlbCnt);
    // Get the matching HGLOBAL
    lpSymEntry =
      SymTabLookupName (wszGlbCnt, &stFlags);

    // If it's not found or has no value, load it from the [Globals] section
    if (lpSymEntry EQ NULL || !lpSymEntry->stFlags.Value)
        return
          LoadWorkspaceGlobal_EM (wszGlbCnt,                        // Ptr to keyname (FMTSTR_GLBCNT)
                                  lpLoadWsGlbVarParm->lpwSrc,       // Ptr to next available byte
                                  lpLoadWsGlbVarParm->uMaxSize,     // Maximum size of lpwSrc
                                  lpLoadWsGlbVarParm->hWndEC,       // Edit Ctrl window handle
                                  lpLoadWsGlbVarParm->lplpSymLink,  // Ptr to ptr to SYMENTRY link
                                  lpLoadWsGlbVarParm->lpwszDPFE,    // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
                                  lpLoadWsGlbVarParm->lplpwErrMsg); // Ptr to ptr to (constant) error message text
    else
        return lpSymEntry->stData.stGlbData;
} // End LoadWsGlbVarConv


//***************************************************************************
//  End of File: sc_load.c
//***************************************************************************
