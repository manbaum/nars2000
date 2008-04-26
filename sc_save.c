//***************************************************************************
//  NARS2000 -- System Command:  )SAVE
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
#include "pertab.h"
#include "aplerrors.h"
#include "sysvars.h"
#include "dfnhdr.h"
#include "workspace.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $MakeWorkspaceNameCanonical
//
// Convert a workspace name into a canonical form (without WSKEXT)
//***************************************************************************

void MakeWorkspaceNameCanonical
    (LPWCHAR wszOut,            // Output workspace name
     LPWCHAR wszInp,            // Input  ...
     LPWCHAR wszDefDir)         // Default drive and directory if no drive letter

{
    WCHAR wszDrive [_MAX_DRIVE],// Save area for drive letter
          wszDir   [_MAX_DIR],  // ...           directory
          wszFname [_MAX_FNAME],// ...           filename
          wszExt   [_MAX_EXT],  // ...           extension
          wszTmpDir[_MAX_DIR];  // ...           temporary directory

    // Split the name into its component parts
    _wsplitpath (wszInp, wszDrive, wszDir, wszFname, wszExt);

    // If there is no drive letter, use the default one
    if (wszDrive[0] EQ L'\0')
    {
        lstrcpyW (wszDrive, wszDefDrive);

        // If the directory is not at the root, prepend the default
        //   Save Workspace directory name
        if (wszDir[0] NE L'\\')
        {
            lstrcpyW (wszTmpDir, wszDefDir);
            lstrcatW (wszTmpDir, wszDir);
            wszDrive[0] = L'\0';        // Because wszSaveDir includes the drive letter
        } else
            lstrcpyW (wszTmpDir, wszDir);

        // Put it back together into a DPFE
        _wmakepath (wszOut, wszDrive, wszTmpDir, wszFname, wszExt);
    } else
        lstrcpyW (wszOut, wszInp);
} //  End MakeWorkspaceNameCanonical


//***************************************************************************
//  $CmdSave_EM
//
//  Execute the system command:  )SAVE [wsid]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdSave_EM"
#else
#define APPEND_NAME
#endif

BOOL CmdSave_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    HGLOBAL      hGlbPTD,               // PerTabData global memory handle
                 hGlbName,              // STE name global memory handle
                 hGlbCnt = NULL,        // Vars/Fcns counters global memory handle
                 stGlbData;             // SYMENTRY's stGlbData
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPVOID       lpMemOldWSID = NULL;   // Ptr to old []WSID global memory
    LPUINT       lpMemCnt = NULL;       // Ptr to Vars/Fcns counters
    WCHAR        wszTailDPFE[_MAX_PATH],// Save area for canonical form of given ws name
                 wszWsidDPFE[_MAX_PATH],// ...           ...               []WSID
                 wszTempDPFE[_MAX_PATH],// ...           temporary
                 wszSectName[15];       // ...           section name (e.g., [Vars.nnn])
    LPWCHAR      lpMemSaveWSID,         // Ptr to WSID to save to
                 lpMemName,             // Ptr to STE name
                 lpaplChar;             // Ptr to output save area
    APLNELM      aplNELMWSID;           // []WSID NELM
    APLRANK      aplRankWSID;           // ...    rank
    BOOL         bRet = FALSE;          // TRUE iff result is valid
    UINT         uNameLen;              // Length of the object name in WCHARs
    int          iCmp;                  // Comparison result
    UINT         uCnt,                  // Loop counter
                 uGlbCntVar = 0,        // # entries in [GlobalVars] section
                 uGlbCntFcn = 0;        // ..           [GlobalFcns] ...
    WCHAR        wszCount[8];           // Output save area for formatted uSymxxx counter
    FILE        *fStream;               // Ptr to file stream
    LPSYMENTRY   lpSymEntry,            // Ptr to STE
                 lpSymTabNext;          // ...
    STFLAGS      stFlags;               // STE flags

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Lock the memory to get a ptr to it
    lpMemOldWSID = MyGlobalLock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData));

#define lpHeader        ((LPVARARRAY_HEADER) lpMemOldWSID)
    // Get the NELM and Rank
    aplNELMWSID = lpHeader->NELM;
    aplRankWSID = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemSaveWSID = VarArrayBaseToData (lpMemOldWSID, aplRankWSID);

    // Because the global memory doesn't have a zero terminator,
    //   we must copy the data to a temporary location and then
    //   append a zero terminator
    lstrcpynW (wszTempDPFE, lpMemSaveWSID, (UINT) aplNELMWSID + 1);
////wszTempDPFE[aplNELMWSID] = L'\0';   // Already done via "+ 1" in lstrcpynW

    // Convert the []WSID workspace name into a canonical form (without WKSEXT)
    MakeWorkspaceNameCanonical (wszWsidDPFE, wszTempDPFE, wszSaveDir);

    // If there is a specified WS Name,
    //   and the current []WSID is different from the specified WS Name,
    //   and a WS with the specified WS Name already exists,
    //   display an error
    //     "NOT SAVED, THIS WS IS ",[]WSID
    if (*lpwszTail)
    {
        // Convert the given workspace name into a canonical form (without WKSEXT)
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, wszSaveDir);

        // Compare the workspace names (without their extensions)
        iCmp = lstrcmpiW (wszWsidDPFE, wszTailDPFE);

        // Append the common workspace extension
        lstrcatW (wszTailDPFE, WS_WKSEXT);

        // If unequal and the given named ws already exists,
        //   display an error
        if (iCmp NE 0)
        {
            // Attempt to open the workspace
            fStream = _wfopen (wszTailDPFE, L"r");

            // If it already exists, display an error
            if (fStream NE NULL)
            {
                // We no longer need this handle
                fclose (fStream); fStream = NULL;

                // If []WSID is empty, it's a CLEAR WS
                if (aplNELMWSID EQ 0)
                    lstrcpyW (lpwszTemp, ERRMSG_NOT_SAVED_CLEAR_WS);
                else
                {
                    // Copy the leading part of the message
                    lstrcpyW (lpwszTemp, ERRMSG_NOT_SAVED);

                    // Followed by the old []WSID
                    lstrcpynW (&lpwszTemp[lstrlenW (lpwszTemp)], lpMemSaveWSID, (UINT) aplNELMWSID + 1);
                } // End IF/ELSE

                // Display the error message
                AppendLine (lpwszTemp, FALSE, TRUE);

                goto ERROR_EXIT;
            } // End IF

            // We no longer need this ptr
            MyGlobalUnlock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemOldWSID = NULL;

            // Set the value of the new []WSID as wszTailDPFE
            if (!SaveNewWsid (wszTailDPFE))
                goto ERROR_EXIT;
        } // End IF

        // Save as the target WSID
        lpMemSaveWSID = wszTailDPFE;
    } else
    {
        // If []WSID is empty, it's a CLEAR WS
        if (aplNELMWSID EQ 0)
            goto NOTSAVED_CLEAR_EXIT;

        // Append the common workspace extension
        lstrcatW (wszWsidDPFE, WS_WKSEXT);

        // Save as the target WSID
        lpMemSaveWSID = wszWsidDPFE;
    } // End IF/ELSE

    if (lpMemOldWSID)
    {
        // We no longer need this ptr
        MyGlobalUnlock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemOldWSID = NULL;
    } // End IF

    // The full workspace name to save to is in lpMemSaveWSID

    // Allocate space for two counters (Vars & Fcns) per SI level
    hGlbCnt = MyGlobalAlloc (GHND, 2 * sizeof (UINT) * (lpMemPTD->SILevel + 1));
    if (!hGlbCnt)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemCnt = MyGlobalLock (hGlbCnt);

    // The format of a saved ws is as an .ini file
    //   (named *.ws.nars) with the following sections:
    //   [General]    (General settings)
    //   [GlobalVars] (Ptrs to global memory variables)
    //   [Vars.0]     (Global variables)
    //   [Vars.1]     (Variables at level 1 in the SI stack)
    //   [Vars.nnn]   (Variables at level nnn in the SI stack)
    //   [GlobalFcns] (Ptrs to global memory functions/operators)
    //   [Fns.0]      (Global functions)
    //   [Fns.1]      (Functions at level 1 in the SI stack)
    //   [Fns.nnn]    (Functions at level nnn in the SI stack)
    // Each of the above sections has content of
    //   1          = Name      (1st name)
    //   2          = Name      (2nd name)
    //   xxx        = Name      (xxxth name)
    // Corresponding to each of the above names, there is a
    //   separate section [nnn.Name], for example,
    //   [0.Name]     (The contents of the name "Name" at level 0)

    // Create (or truncate the file)
    fStream = _wfopen (lpMemSaveWSID, L"w");
    if (!fStream)
        goto NOTSAVED_FILE_EXIT;

    // Close it after creating the file
    fclose (fStream); fStream = NULL;

    // Write out the [General] section
    WritePrivateProfileStringW (SECTNAME_GENERAL,   // Ptr to the section name
                                KEYNAME_VERSION,    // Ptr to the key name
                                L"0.01",            // Ptr to the key value
                                lpMemSaveWSID);     // Ptr to the file name
    // Trundle through the Symbol Table
    for (lpSymTabNext = lpMemPTD->lpSymTab;
         lpSymTabNext < lpMemPTD->lpSymTabNext;
         lpSymTabNext++)
    if (lpSymTabNext->stFlags.Inuse)        // Must be Inuse
    // Handle different SI levels
    for (lpSymEntry = lpSymTabNext; lpSymEntry; lpSymEntry = lpSymEntry->stPrvEntry)
    if (lpSymEntry->stHshEntry->htGlbName           // Must have a name (not steZero, etc.)
     && lpSymEntry->stFlags.ObjName NE OBJNAME_MF   // and not be a Magic Function
     && lpSymEntry->stFlags.ObjName NE OBJNAME_LOD) // and not be a )LOAD HGLOBAL
    {
        // Get the symbol name's global memory handle
        hGlbName = lpSymEntry->stHshEntry->htGlbName;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (hGlbName);

        // Format the text as an ASCII string with non-ASCII chars
        //   represented as either {symbol} or {\xXXXX} where XXXX is
        //   a four-digit hex number.
        uNameLen =
          ConvertWideToName (lpwszTemp,
                             lpMemName);
        // We no longer need this ptr
        MyGlobalUnlock (hGlbName); lpMemName = NULL;

        // Point to the end of the name
        lpaplChar = &lpwszTemp[uNameLen];

        // Get the flags
        stFlags = lpSymEntry->stFlags;

        // Check for []DM
        if ((!stFlags.Value) && lstrcmpiW (lpwszTemp, L"{quad}dm") EQ 0)
        {
            // Mark as having a value
            stFlags.Value = TRUE;

            // Mark as a variable
            stFlags.stNameType = NAMETYPE_VAR;

            // Save the GlbData handle
            stGlbData = MakePtrTypeGlb (lpMemPTD->hGlbQuadDM);
        } else
            stGlbData = lpSymEntry->stData.stGlbData;

        if (stFlags.Value)                      // Must have a value
        {
            // Split cases based upon the object type
            switch (stFlags.stNameType)
            {
                case NAMETYPE_VAR:
                    // Append separator
                    *lpaplChar++ = L'=';

                    // If it's immediate, ...
                    if (stFlags.Imm)
                    {
                        lpaplChar =
                          AppendArrayHeader (lpaplChar,                         // Ptr to output save area
                                             TranslateImmTypeToChar (stFlags.ImmType),// Object storage type as WCHAR
                                             1,                                 // Object NELM
                                             0,                                 // Object rank
                                             NULL);                             // Ptr to object dimensions
                        // If it's char, ...
                        if (stFlags.ImmType EQ IMMTYPE_CHAR)
                        {
                            // Append a leading single quote
                            *lpaplChar++ = L'\'';

                            // Format the text as an ASCII string with non-ASCII chars
                            //   represented as either {symbol} or {\xXXXX} where XXXX is
                            //   a four-digit hex number.
                            lpaplChar +=
                              ConvertWideToNameLength (lpaplChar,
                                                      &lpSymEntry->stData.stChar,
                                                       1);
                            // Append a trailing single quote
                            *lpaplChar++ = L'\'';

                            // Ensure properly terminated
                            *lpaplChar = L'\0';
                        } else
                        {
                            // Format the value
                            lpaplChar =
                              FormatImmedFC (lpaplChar,
                                             stFlags.ImmType,
                                            &lpSymEntry->stData.stLongest,
                                             DEF_MAX_QUADPP,
                                             UTF16_DOT,
                                             UTF16_BAR,
                                             2);
                            // Delete the last blank in case it matters,
                            //   and ensure properly terminated
                            if (lpaplChar[-1] EQ L' ')
                                *--lpaplChar = L'\0';
                            else
                                *lpaplChar = L'\0';
                        } // End IF/ELSE
                    } else
                        // Convert the variable in global memory to saved ws form
                        lpaplChar =
                          SavedWsFormGlbVar (lpaplChar,
                                             stGlbData,
                                             lpMemSaveWSID,
                                            &uGlbCntVar);
                    // Format the counter
                    wsprintfW (wszCount, L"%d", lpMemCnt[0 + 2 * lpSymEntry->stSILevel]++);

                    // Format the section name as [Vars.nnn] where nnn is the SI level
                    wsprintfW (wszSectName, SECTNAME_VARS L".%d", lpSymEntry->stSILevel);

                    // Write out the entry (nnn = Name = value)
                    //   in the [Vars.nnn] section
                    WritePrivateProfileStringW (wszSectName,            // Ptr to the section name
                                                wszCount,               // Ptr to the key name
                                                lpwszTemp,              // Ptr to the key value
                                                lpMemSaveWSID);         // Ptr to the file name
                    break;

                case NAMETYPE_FN0:
                case NAMETYPE_FN12:
                case NAMETYPE_OP1:
                case NAMETYPE_OP2:
                case NAMETYPE_OP3:
                    // Format the section name as [nnn.Name] where nnn is the function count
                    wsprintfW (lpwszFormat,
                               L"%d.%s",
                               uGlbCntFcn,
                               lpwszTemp);
                    // Append separator
                    *lpaplChar++ = L'=';

                    // Append the name type
                    *lpaplChar++ = L'0' + stFlags.stNameType;

                    // Append separator
                    *lpaplChar++ = L'=';

                    // If it's immediate, ...
                    if (stFlags.Imm)
                    {
                        APLCHAR aplChar;

                        // Translate from INDEX_xxx to UTF16_xxx
                        aplChar = TranslateFcnOprToChar (lpSymEntry->stData.stChar);

                        // Format the text as an ASCII string with non-ASCII chars
                        //   represented as either {symbol} or {\xXXXX} where XXXX is
                        //   a four-digit hex number.
                        lpaplChar +=
                          ConvertWideToNameLength (lpaplChar,
                                                  &aplChar,
                                                   1);
                    } else
                        // Convert the function/operator in global memory to saved ws form
                        lpaplChar =
                          SavedWsFormGlbFcn (lpaplChar,
                                             lpwszFormat,
                                             lpSymEntry->stData.stGlbData,
                                             lpMemSaveWSID,
                                            &uGlbCntFcn);
                    // Format the counter
                    wsprintfW (wszCount, L"%d", lpMemCnt[1 + 2 * lpSymEntry->stSILevel]++);

                    // Format the section name as [Fcns.nnn] where nnn is the SI level
                    wsprintfW (wszSectName, SECTNAME_FCNS L".%d", lpSymEntry->stSILevel);

                    // Write out the entry (nnn=Name=Type=Value)
                    //   in the [Fcns.nnn] section
                    WritePrivateProfileStringW (wszSectName,            // Ptr to the section name
                                                wszCount,               // Ptr to the key name
                                                lpwszTemp,              // Ptr to the key value
                                                lpMemSaveWSID);         // Ptr to the file name
                    break;

                defstop
                    break;
            } // End SWITCH
        } else
        if (lpSymEntry->stSILevel)      // Must be suspended
        {
            // Format the section name
            wsprintfW (wszSectName, SECTNAME_VARS L".%d", lpSymEntry->stSILevel);

            // Write out the entry (nnn = Name = value)
            //   in the [Vars.nnn] section
            WritePrivateProfileStringW (wszSectName,            // Ptr to the section name
                                        lpwszTemp,              // Ptr to the key name
                                        L"",                    // Ptr to the key value
                                        lpMemSaveWSID);         // Ptr to the file name
        } // End IF/ELSE/...
    } // End FOR/IF/...

////DbgBrk ();

    // Delete the #XXXXXXXX=#n references in the [GlobalVars] section
    for (uCnt = 0; uCnt < uGlbCntVar; uCnt++)
    {
////    WritePrivateProfileStringW (SECTNAME_GLOBALVARS,        // Ptr to the section name
////                               ... ,                        // Ptr to key name
////                               NULL,                        // Ptr to key value (NULL to delete)
////                               lpMemSaveWSID);              // Ptr to the file name
    } // End FOR

    // Delete the #XXXXXXXX=#n references in the [GlobalFcns] section
    for (uCnt = 0; uCnt < uGlbCntFcn; uCnt++)
    {
////    WritePrivateProfileStringW (SECTNAME_GLOBALFCNS,        // Ptr to the section name
////                               ... ,                        // Ptr to key name
////                               NULL,                        // Ptr to key value (NULL to delete)
////                               lpMemSaveWSID);              // Ptr to the file name
    } // End FOR

    // Loop through the SI levels
    for (uCnt = 0; uCnt < (lpMemPTD->SILevel + 1); uCnt++)
    {
        // Format the section name as [Vars.nnn] where nnn is the SI level
        wsprintfW (wszSectName, SECTNAME_VARS L".%d", uCnt);

        // Format the Var count
        wsprintfW (wszCount,
                   L"%d",
                   lpMemCnt[0 + 2 * uCnt]);
        // Write out the Var count to the [Vars.nnn] section where nnn is the SI level
        WritePrivateProfileStringW (wszSectName,                    // Ptr to the section name
                                    KEYNAME_COUNT,                  // Ptr to the key name
                                    wszCount,                       // Ptr to the key value
                                    lpMemSaveWSID);                 // Ptr to the file name
        // Format the section name as [Fcns.nnn] where nnn is the SI level
        wsprintfW (wszSectName, SECTNAME_FCNS L".%d", uCnt);

        // Format the Fcn/Opr count
        wsprintfW (wszCount,
                   L"%d",
                   lpMemCnt[1 + 2 * uCnt]);
        // Write out the Fcn count to the [Fcns.nnn] section where nnn is the SI level
        WritePrivateProfileStringW (wszSectName,                    // Ptr to the section name
                                    KEYNAME_COUNT,                  // Ptr to the key name
                                    wszCount,                       // Ptr to the key value
                                    lpMemSaveWSID);                 // Ptr to the file name
    } // End FOR

    // Format the SI level
    wsprintfW (wszCount,
               L"%d",
               lpMemPTD->SILevel);
    // Write out the SI level to the [General] section
    WritePrivateProfileStringW (SECTNAME_GENERAL,               // Ptr to the section name
                                KEYNAME_SILEVEL,                // Ptr to the key name
                                wszCount,                       // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Format the [GlobalVars] count
    wsprintfW (wszCount,
               L"%d",
               uGlbCntVar);
    // Write out the count to the [GlobalVars] section
    WritePrivateProfileStringW (SECTNAME_GLOBALVARS,            // Ptr to the section name
                                KEYNAME_COUNT,                  // Ptr to the key name
                                wszCount,                       // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Format the [GlobalFcns] count
    wsprintfW (wszCount,
               L"%d",
               uGlbCntFcn);
    // Write out the count to the [GlobalFcns] section
    WritePrivateProfileStringW (SECTNAME_GLOBALFCNS,            // Ptr to the section name
                                KEYNAME_COUNT,                  // Ptr to the key name
                                wszCount,                       // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Write out the SI stack to the [SI] section
    CmdSiSinlCom_EM (L"",               // Ptr to command tail
                     TRUE,              // TRUE iff )SINL
                     lpMemSaveWSID);    // Ptr to the file name
    // Note if the SI is non-empty
    if (lpMemPTD->SILevel)
        AppendLine (L"WARNING:  SI non-empty -- not restartable after )LOAD", FALSE, TRUE);

    // Get the length of the []WSID excluding WKSEXT
    iCmp = lstrlenW (lpMemSaveWSID) - ((sizeof WKSEXT) - 1);

    // Omit the trailing WKSEXT
    Assert (lpMemSaveWSID[iCmp] EQ L'.');
    lpMemSaveWSID[iCmp] = L'\0';

    // Copy the WSID & " SAVED "
    lstrcpyW (lpwszTemp, lpMemSaveWSID),
    lstrcatW (lpwszTemp, L" SAVED ");

    // Format the current date & time
    FormatCurDateTime (&lpwszTemp[lstrlenW (lpwszTemp)]);

    // Write out the WSID and SAVED date & time
    AppendLine (lpwszTemp, FALSE, TRUE);

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Display the error message
    AppendLine (ERRMSG_WS_FULL APPEND_NAME, FALSE, TRUE);

    goto ERROR_EXIT;

NOTSAVED_CLEAR_EXIT:
    // Display the error message
    AppendLine (ERRMSG_NOT_SAVED_CLEAR_WS, FALSE, TRUE);

    goto ERROR_EXIT;

NOTSAVED_FILE_EXIT:
    // Display the error message
    AppendLine (ERRMSG_NOT_SAVED_FILE_ERROR APPEND_NAME, FALSE, TRUE);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemOldWSID)
    {
        // We no longer need this ptr
        MyGlobalUnlock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemOldWSID = NULL;
    } // End IF

    if (hGlbCnt)
    {
        if (lpMemCnt)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbCnt); lpMemCnt = NULL;
        } // End IF

        // We no longer need this storage
        MyGlobalFree (hGlbCnt); hGlbCnt = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End CmdSave_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SavedWsFormGlbFcn
//
//  Convert a function/operator in global memory to saved ws form
//***************************************************************************

LPAPLCHAR SavedWsFormGlbFcn
    (LPAPLCHAR lpaplChar,               // Ptr to output save area
     LPAPLCHAR lpwszSectName,           // Ptr to the section name as [nnn.Name] where nnn is the function count
     HGLOBAL   hGlbObj,                 // WS object global memory handle
     LPAPLCHAR lpMemSaveWSID,           // Ptr to saved WS file DPFE
     LPUINT    lpGlbCnt)                // Ptr to [GlobalFcns] count

{
    HGLOBAL        hGlbTxtLine;         // Text header/line global memory handle
    LPMEMTXT_UNION lpMemTxtLine;        // Ptr to header/textline global memory
    LPVOID         lpMemObj = NULL;     // Ptr to WS object ...
    LPAPLCHAR      lpMemProKeyName,     // Ptr to profile keyname
                   lpaplCharStart;      // Ptr to start of buffer
    UINT           uLine;               // Function line loop counter
    FILETIME       ftCreation,          // Object creation time
                   ftLastMod;           // ...    last modification time
    WCHAR          wszGlbObj[16 + 1],   // Save area for formatted hGlbObj (room for 64-bit ptr plus terminating zero)
                   wszGlbCnt[8 + 1];    // Save area for formatted *lpGlbCnt

    Assert (IsGlbTypeFcnDir (hGlbObj)
         || IsGlbTypeDfnDir (hGlbObj));

    // Save ptr to start of buffer
    lpaplCharStart = lpaplChar;

    // Clear the ptr type bits
    hGlbObj = ClrPtrTypeDirAsGlb (hGlbObj);

    // Format the hGlbObj
    wsprintfW (wszGlbObj,
               L"#%08X",
               hGlbObj);
    // Save as ptr to the profile keyname
    lpMemProKeyName = lpaplChar;

    // Check to see if this global has already been saved
    //   in the [GlobalFcns] section
    if (GetPrivateProfileStringW (SECTNAME_GLOBALFCNS,          // Ptr to the section name
                                  wszGlbObj,                    // Ptr to the key name
                                  L"",                          // Default value if not found
                                  lpaplChar,                    // Ptr to the output buffer
                                  8 * sizeof (lpaplChar[0]),    // Byte size of the output buffer
                                  lpMemSaveWSID))               // Ptr to the file name
        goto NORMAL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLock (hGlbObj);

    // Split cases based upon the object's signature
    switch (((LPHEADER_SIGNATURE) lpMemObj)->nature)
    {
        case FCNARRAY_HEADER_SIGNATURE:
            // Get the text line global memory handle
            hGlbTxtLine = ((LPFCNARRAY_HEADER) lpMemObj)->hGlbTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Format the text as an ASCII string with non-ASCII chars
            //   represented as either {symbol} or {\xXXXX} where XXXX is
            //   a four-digit hex number.
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,
                                      &lpMemTxtLine->C,
                                       lpMemTxtLine->U);
            // Ensure properly terminated
            *lpaplChar++ = L'\0';

            // Copy creation time
            ftCreation = ((LPFCNARRAY_HEADER) lpMemObj)->ftCreation;

            // Copy last mod time
            ftLastMod  = ((LPFCNARRAY_HEADER) lpMemObj)->ftLastMod;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

            // Write out the single line
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        L"0",               // Ptr to the key name
                                        lpaplCharStart,     // Ptr to the key value
                                        lpMemSaveWSID);     // Ptr to the file name
            // Write out the Count key value
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        KEYNAME_COUNT,      // Ptr to the key name
                                        L"1",               // Ptr to the key value
                                        lpMemSaveWSID);     // Ptr to the file name
            // Write out the UserDefined key value
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        KEYNAME_USERDEFINED,// Ptr to the key name
                                        L"0",               // Ptr to the key value
                                        lpMemSaveWSID);     // Ptr to the file name
            break;

        case DFN_HEADER_SIGNATURE:
        {
            UINT      numFcnLines;
            LPFCNLINE lpFcnLines;       // Ptr to array of function line structs (FCNLINE[numFcnLines])

            // Put the function definition in a separate
            //   section named [nnn.Name] with each line
            //   of the function on a separate line as in
            //   0 = <Header>
            //   1 = <First Line>, etc.
            //   CreationTime = xxxxxxxxxxxxxxxx (64-bit hex number)


#define lpMemDfnHdr     ((LPDFN_HEADER) lpMemObj)
            numFcnLines = lpMemDfnHdr->numFcnLines;

            // Get ptr to array of function line structs (FCNLINE[numFcnLines])
            lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

            // Write out the function header
            WriteFunctionLine (lpwszSectName,   // Ptr to the section name
                               lpaplChar,       // Ptr to save area for the formatted line
                               0,               // Line #
                               lpMemDfnHdr->hGlbTxtHdr,
                               lpMemSaveWSID);  // Ptr to the file name
            // Loop through the function lines
            for (uLine = 0; uLine < numFcnLines; uLine++, lpFcnLines++)
                WriteFunctionLine (lpwszSectName,   // Ptr to the section name
                                   lpaplChar,       // Ptr to save area for the formatted line
                                   uLine + 1,       // Line #
                                   lpFcnLines->hGlbTxtLine,
                                   lpMemSaveWSID);  // Ptr to the file name
            // Format the function line count
            wsprintfW (lpaplChar,
                       L"%d",
                       numFcnLines + 1);
            // Write out the Count key value
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        KEYNAME_COUNT,      // Ptr to the key name
                                        lpaplChar,          // Ptr to the key value
                                        lpMemSaveWSID);     // Ptr to the file name
            // Write out the UserDefined key value
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        KEYNAME_USERDEFINED,// Ptr to the key name
                                        L"1",               // Ptr to the key value
                                        lpMemSaveWSID);     // Ptr to the file name
#define lpUndoIni       lpaplChar       // Start of output save area

            // Write out the Undo buffer
            if (lpMemDfnHdr->hGlbUndoBuff)
            {
                LPUNDO_BUF lpMemUndo;               // Ptr to Undo Buffer global memory
                UINT       uUndoCount;              // # entries in the Undo Buffer
                LPAPLCHAR  lpUndoOut;               // Ptr to output save area
                WCHAR      wcTmp[2];                // Temporary char string

                // Ensure properly terminated
                wcTmp[1] = L'\0';

                // Get the # entries
                uUndoCount = (MyGlobalSize (lpMemDfnHdr->hGlbUndoBuff)) / sizeof (UNDO_BUF);

                // Start with the # entries
                lpUndoOut = lpUndoIni +
                  wsprintfW (lpUndoIni,
                             L"%d ",
                             uUndoCount);
                // Lock the memory to get a ptr to it
                lpMemUndo = MyGlobalLock (lpMemDfnHdr->hGlbUndoBuff);

                // Loop through the undo entries
                for (;
                     uUndoCount;
                     uUndoCount--, lpMemUndo++)
                {
                    LPAPLCHAR lpUndoChar;

                    // Split cases based upon the undo char
                    switch (lpMemUndo->Char)
                    {
                        case L'\0':
                            lpUndoChar = L"";

                            break;

                        case L'\n':
                            lpUndoChar = L"\\n";

                            break;

                        case L'\r':
                            lpUndoChar = L"\\r";

                            break;

                        case L'\'':
                            lpUndoChar = L"\\'";

                            break;

                        case L'\\':
                            lpUndoChar = L"\\";

                            break;

                        default:
                            // Copy to string
                            if (!(lpUndoChar = CharToName (lpMemUndo->Char)))
                            {
                                // Save as one-char string
                                wcTmp[0] = lpMemUndo->Char;
                                lpUndoChar = wcTmp;
                            } // End IF

                            break;
                    } // End SWITCH

                    // Format this element of the Undo Buffer
                    lpUndoOut +=
                      wsprintfW (lpUndoOut,
                                 (*lpUndoChar EQ L'\0') ? L"%c %d %d %d, "
                                                        : L"%c %d %d %d '%s', ",
                                 UndoActToChar[lpMemUndo->Action],
                                 lpMemUndo->CharPosBeg,
                                 lpMemUndo->CharPosEnd,
                                 lpMemUndo->Group,
                                 lpUndoChar);
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (lpMemDfnHdr->hGlbUndoBuff); lpMemUndo = NULL;

                // Ensure properly terminated (zap the trailing comma)
                if (lpUndoOut[-2] EQ L',')
                    lpUndoOut[-2] = L'\0';
                else
                    lpUndoOut[ 0] = L'\0';
            } else
                // Ensure properly terminated
                lstrcpyW (lpUndoIni, L"0");

            // Write out the Undo Buffer contents
            WritePrivateProfileStringW (lpwszSectName,          // Ptr to the section name
                                        KEYNAME_UNDO,           // Ptr to the key name
                                        lpUndoIni,              // Ptr to the key value
                                        lpMemSaveWSID);         // Ptr to the file name
            // ***FIXME*** -- Write out the stop/trace bits







            // Copy creation time
            ftCreation = lpMemDfnHdr->ftCreation;

            // Copy last modification time
            ftLastMod  = lpMemDfnHdr->ftLastMod;
#undef  lpMemDfnHdr
            break;
        } // End DFN_HEADER_SIGNATURE

        defstop
            break;
    } // End SWITCH

    // Save the ftCreation time
    wsprintfW (lpaplChar,
               L"%08X%08X",
               ftCreation.dwHighDateTime,
               ftCreation.dwLowDateTime);
    WritePrivateProfileStringW (lpwszSectName,          // Ptr to the section name
                                KEYNAME_CREATIONTIME,   // Ptr to the key name
                                lpaplChar,              // Ptr to the key value
                                lpMemSaveWSID);         // Ptr to the file name
    // Save the ftLastMod time
    wsprintfW (lpaplChar,
               L"%08X%08X",
               ftLastMod.dwHighDateTime,
               ftLastMod.dwLowDateTime);
    WritePrivateProfileStringW (lpwszSectName,          // Ptr to the section name
                                KEYNAME_LASTMODTIME,    // Ptr to the key name
                                lpaplChar,              // Ptr to the key value
                                lpMemSaveWSID);         // Ptr to the file name
    // Format the global count
    wsprintfW (wszGlbCnt,
               FMTSTR_GLBCNT,
              *lpGlbCnt);
    // Count in another entry
    (*lpGlbCnt)++;

    // Write out the entry in the [GlobalFcns] section
    WritePrivateProfileStringW (SECTNAME_GLOBALFCNS,            // Ptr to the section name
                                wszGlbCnt,                      // Ptr to the key value
                                lpwszSectName,                  // Ptr to the key name
                                lpMemSaveWSID);                 // Ptr to the file name
    // Write out the saved marker
    WritePrivateProfileStringW (SECTNAME_GLOBALFCNS,            // Ptr to the section name
                                wszGlbObj,                      // Ptr to the key name
                                wszGlbCnt,                      // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Move back to the start
    lpaplChar = lpaplCharStart;

    // Copy the formatted GlbCnt to the start of the buffer as the result
    lstrcpyW (lpaplChar, wszGlbCnt);
NORMAL_EXIT:
    if (hGlbObj && lpMemObj)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
    } // End IF

    // Return a ptr to the profile keyname's terminating zero
    return &lpaplChar[lstrlenW (lpaplChar)];
} // End SavedWsFormGlbFcn


//***************************************************************************
//  $WriteFunctionLine
//
//  Write out a function line to a the saved WS file
//***************************************************************************

void WriteFunctionLine
    (LPWCHAR lpwszSectName,     // Ptr to section name
     LPWCHAR lpwFormat,         // Ptr to output save area for the formatted function line
     UINT    uLineNum,          // The line #
     HGLOBAL hGlbTxtLine,       // Line text global memory handle
     LPWCHAR lpMemSaveWSID)     // Ptr to saved WS file DPFE

{
    LPMEMTXT_UNION lpMemTxtLine;
    WCHAR          wszCount[10];

    // Format the line number
    wsprintfW (wszCount,
               L"%d",
               uLineNum);
    // Lock the memory to get a ptr to it
    lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

    // Format the text as an ASCII string with non-ASCII chars
    //   represented as either {symbol} or {\xXXXX} where XXXX is
    //   a four-digit hex number.
    ConvertWideToNameLength (lpwFormat,
                            &lpMemTxtLine->C,
                             lpMemTxtLine->U);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

    // Write out the entry (nnn = FunctionLine)
    WritePrivateProfileStringW (lpwszSectName,                      // Ptr to the section name
                                wszCount,                           // Ptr to the key name
                                lpwFormat,                          // Ptr to the key value
                                lpMemSaveWSID);                     // Ptr to the file name
} // End WriteFunctionLine


//***************************************************************************
//  $SavedWsFormGlbVar
//
//  Convert a variable in global memory to saved ws form
//***************************************************************************

LPAPLCHAR SavedWsFormGlbVar
    (LPAPLCHAR lpaplChar,               // Ptr to output save area
     HGLOBAL   hGlbObj,                 // WS object global memory handle
     LPAPLCHAR lpMemSaveWSID,           // Ptr to saved WS file DPFE
     LPUINT    lpGlbCnt)                // Ptr to [GlobalVars] count

{
    APLSTYPE     aplTypeObj;            // WS object storage type
    APLNELM      aplNELMObj;            // ...       NELM
    APLRANK      aplRankObj;            // ...       rank
    APLINT       apaOff,                // APA offset
                 apaMul;                // ... multiplier
    APLUINT      uObj,                  // WS object loop counter
                 uQuadPP;               // []PP save area
    LPVOID       lpMemObj = NULL;       // Ptr to WS object global memory
    LPAPLCHAR    lpMemProKeyName,       // Ptr to profile keyname
                 lpaplCharStart;        // Ptr to start of buffer
    UINT         uBitIndex;             // Bit index when marching through Booleans
    HGLOBAL      hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    PERM_NDX     permNdx;               // Permanent object index
    STFLAGS      stFlags;               // Object SymTab flags
    WCHAR        wszGlbObj[16 + 1],     // Save area for formatted hGlbObj (room for 64-bit ptr plus terminating zero)
                 wszGlbCnt[8 + 1];      // Save area for formatted *lpGlbCnt

    // Save ptr to start of buffer
    lpaplCharStart = lpaplChar;

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbObj));

    // Clear the ptr type bits
    hGlbObj = ClrPtrTypeDirAsGlb (hGlbObj);

    // Format the hGlbObj
    wsprintfW (wszGlbObj,
               L"#%08X",
               hGlbObj);
    // Save as ptr to the profile keyname
    lpMemProKeyName = lpaplChar;

    // Check to see if this global has already been saved
    //   in the [GlobalVars] section
    if (GetPrivateProfileStringW (SECTNAME_GLOBALVARS,          // Ptr to the section name
                                  wszGlbObj,                    // Ptr to the key name
                                  L"",                          // Default value if not found
                                  lpaplChar,                    // Ptr to the output buffer
                                  8 * sizeof (lpaplChar[0]),    // Byte size of the output buffer
                                  lpMemSaveWSID))               // Ptr to the file name
        goto NORMAL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLock (hGlbObj);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemObj)
    // Get the array attributes
    aplTypeObj = lpHeader->ArrType;
    aplNELMObj = lpHeader->NELM;
    aplRankObj = lpHeader->Rank;
    permNdx    = lpHeader->PermNdx;
#undef  lpHeader

    // The array values are preceded by the array
    //   attributes in the form of
    //   StorageType   NELM   Rank  Shape
    //   "BIFCHNLA"     %d     %d   *(%d )
    //   %c %d %d %*(d )
    lpaplChar =
      AppendArrayHeader (lpaplChar,                         // Ptr to output save area
                         TranslateArrayTypeToChar (aplTypeObj),// Object storage type as WCHAR
                         aplNELMObj,                        // Object NELM
                         aplRankObj,                        // Object rank
                         VarArrayBaseToDim (lpMemObj));     // Ptr to object dimensions
    // Skip over the header and dimensions to the data
    lpMemObj = VarArrayBaseToData (lpMemObj, aplRankObj);

    // Split cases based the object storage type
    switch (aplTypeObj)
    {
        case ARRAY_BOOL:
            // Initialize
            uBitIndex = 0;

            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++)
            {
                // Format the value
                lpaplChar =
                  FormatAplintFC (lpaplChar,
                                  BIT0 & ((*(LPAPLBOOL) lpMemObj) >> uBitIndex),
                                  UTF16_BAR);
                // Check for end-of-byte
                if (++uBitIndex EQ NBIB)
                {
                    uBitIndex = 0;              // Start over
                    ((LPAPLBOOL) lpMemObj)++;   // Skip to next byte
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:
            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLINT) lpMemObj)++)
                // Format the value
                lpaplChar =
                  FormatAplintFC (lpaplChar,
                                  *(LPAPLINT) lpMemObj,
                                  UTF16_BAR);
            break;

        case ARRAY_FLOAT:
            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLFLOAT) lpMemObj)++)
                // Format the value
                lpaplChar =
                  FormatFloatFC (lpaplChar,
                                 *(LPAPLFLOAT) lpMemObj,
                                 DEF_MAX_QUADPP,
                                 UTF16_DOT,
                                 UTF16_BAR,
                                 2);
            break;

        case ARRAY_CHAR:
            // Append a leading single quote
            *lpaplChar++ = L'\'';

            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLCHAR) lpMemObj)++)
                // Format the text as an ASCII string with non-ASCII chars
                //   represented as either {symbol} or {\xXXXX} where XXXX is
                //   a four-digit hex number.
                lpaplChar +=
                  ConvertWideToNameLength (lpaplChar,
                                           (LPAPLCHAR) lpMemObj,
                                           1);
            // Append a trailing single quote
            *lpaplChar++ = L'\'';

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemObj)
            // Get the APA parameters
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
#undef  lpAPA
            // Format as an APA

            // Append the offset
            lpaplChar =
              FormatAplintFC (lpaplChar,
                              apaOff,
                              UTF16_BAR);
            // Append the multiplier
            lpaplChar =
              FormatAplintFC (lpaplChar,
                              apaMul,
                              UTF16_BAR);
            break;

        case ARRAY_HETERO:
            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLHETERO) lpMemObj)++)
            // Split cases based upon the element immediate type
            switch ((*(LPAPLHETERO) lpMemObj)->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpaplChar =
                      FormatAplintFC (lpaplChar,
                                      (*(LPAPLHETERO) lpMemObj)->stData.stBoolean,
                                      UTF16_BAR);
                    break;

                case IMMTYPE_INT:
                    lpaplChar =
                      FormatAplintFC (lpaplChar,
                                      (*(LPAPLHETERO) lpMemObj)->stData.stInteger,
                                      UTF16_BAR);
                    break;

                case IMMTYPE_CHAR:
                    // Append a leading single quote
                    *lpaplChar++ = L'\'';

                    // Format the text as an ASCII string with non-ASCII chars
                    //   represented as either {symbol} or {\xXXXX} where XXXX is
                    //   a four-digit hex number.
                    lpaplChar +=
                      ConvertWideToNameLength (lpaplChar,
                                              &(*(LPAPLHETERO) lpMemObj)->stData.stChar,
                                               1);
                    // Append a trailing single quote
                    *lpaplChar++ = L'\'';

                    // Append a trailing blank
                    *lpaplChar++ = L' ';

                    break;

                case IMMTYPE_FLOAT:
                    // Format the value
                    lpaplChar =
                      FormatFloatFC (lpaplChar,
                                     (*(LPAPLHETERO) lpMemObj)->stData.stFloat,
                                     DEF_MAX_QUADPP,
                                     UTF16_DOT,
                                     UTF16_BAR,
                                     2);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_NESTED:
            // Take into account nested prototypes
            aplNELMObj = max (aplNELMObj, 1);

            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLNESTED) lpMemObj)++)
            // Split case based upon the ptr type
            switch (GetPtrTypeInd (lpMemObj))
            {
                case PTRTYPE_STCONST:
#define lpSymEntry      (*(LPAPLHETERO) lpMemObj)

                    // Get the SymTab flags
                    stFlags = lpSymEntry->stFlags;

                    // Ensure we format with full precision in case it's floating point
                    uQuadPP = lpMemPTD->lpSymQuadPP->stData.stInteger;
                    if (IsImmFlt (stFlags.ImmType))
                        lpMemPTD->lpSymQuadPP->stData.stInteger = DEF_MAX_QUADPP;

                    // ***FIXME*** -- Handle non-printable chars (e.g. []TCxxx)

                    // Append the header for the simple scalar
                    lpaplChar =
                      AppendArrayHeader (lpaplChar,                         // Ptr to output save area
                                         TranslateImmTypeToChar (stFlags.ImmType),// Object storage type as WCHAR
                                         1,                                 // Object NELM
                                         0,                                 // Object rank
                                         NULL);                             // Ptr to object dimensions
                    // Format the value
                    lpaplChar =
                      FormatImmedFC (lpaplChar,
                                     stFlags.ImmType,
                                    &lpSymEntry->stData.stLongest,
                                     DEF_MAX_QUADPP,
                                     UTF16_DOT,
                                     UTF16_BAR,
                                     2);
#undef  lpSymEntry
                    // Restore user's precision
                    lpMemPTD->lpSymQuadPP->stData.stInteger = uQuadPP;

                    break;

                case PTRTYPE_HGLOBAL:
                {
                    APLSTYPE aplTypeSub;

#define hGlbSub     (*(LPAPLNESTED *) lpMemObj)

                    if (hGlbObj EQ PTR_REUSED)
                    {
                        // Append the HGLOBAL name
                        lpaplChar +=
                          wsprintfW (lpaplChar,
                                     L"REUSED ");
                    } else
                    {
                        // Get the global's Type
                        AttrsOfGlb (ClrPtrTypeDirAsGlb (hGlbSub), &aplTypeSub, NULL, NULL, NULL);

                        // Convert the variable in gloabl memory to saved ws form
                        lpaplChar =
                          SavedWsFormGlbVar (lpaplChar,
                                             hGlbSub,
                                             lpMemSaveWSID,
                                             lpGlbCnt);
                        // Ensure there's a trailing blank
                        if (lpaplChar[-1] NE L' ')
                        {
                            *lpaplChar++ = L' ';
                            *lpaplChar   = L'\0';
                        } // End IF
#undef  hGlbSub
                    } // End IF/ELSE

                    break;
                } // End PTRTYPE_HGLOBAL

                defstop
                    break;
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpaplChar[-1] EQ L' ')
        *--lpaplChar = L'\0';
    else
        *lpaplChar = L'\0';

    // Format the global count
    wsprintfW (wszGlbCnt,
               FMTSTR_GLBCNT,
              *lpGlbCnt);
    // Count in another entry
    (*lpGlbCnt)++;

    // Write out the entry in the [GlobalVars] section
    WritePrivateProfileStringW (SECTNAME_GLOBALVARS,            // Ptr to the section name
                                wszGlbCnt,                      // Ptr to the key value
                                lpMemProKeyName,                // Ptr to the key name
                                lpMemSaveWSID);                 // Ptr to the file name
    // Write out the saved marker
    WritePrivateProfileStringW (SECTNAME_GLOBALVARS,            // Ptr to the section name
                                wszGlbObj,                      // Ptr to the key name
                                wszGlbCnt,                      // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Move back to the start
    lpaplChar = lpaplCharStart;

    // Copy the formatted GlbCnt to the start of the buffer as the result
    lstrcpyW (lpaplChar, wszGlbCnt);
NORMAL_EXIT:
    if (hGlbObj && lpMemObj)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Return a ptr to the profile keyname's terminating zero
    return &lpaplChar[lstrlenW (lpaplChar)];
} // End SavedWsFormGlbVar


//***************************************************************************
//  $AppendArrayHeader
//
//  Append the array header
//***************************************************************************

LPAPLCHAR AppendArrayHeader
    (LPAPLCHAR lpaplChar,           // Ptr to output save area
     APLCHAR   aplCharObj,          // Object storage type as WCHAR
     APLNELM   aplNELMObj,          // Object NELM
     APLRANK   aplRankObj,          // Object rank
     LPAPLDIM  lpaplDimObj)         // Ptr to object dimensions

{
    APLRANK uObj;                   // Loop counter

    // Append the storage type as a WCHAR
    *lpaplChar++ = aplCharObj;

    // Append a separator
    *lpaplChar++ = L' ';

    // Append the NELM (with a trailing blank)
    lpaplChar =
      FormatAplintFC (lpaplChar,
                      aplNELMObj,
                      UTF16_BAR);
    // Append the rank (with a trailing blank)
    lpaplChar =
      FormatAplintFC (lpaplChar,
                      aplRankObj,
                      UTF16_BAR);

    // Append the shape (each with a trailing blank)
    for (uObj = 0; uObj < aplRankObj; uObj++)
        // Format the dimension
        lpaplChar =
          FormatAplintFC (lpaplChar,
                          lpaplDimObj[uObj],
                          UTF16_BAR);
    return lpaplChar;
} // End AppendArrayHeader


//***************************************************************************
//  $SaveNewWsid
//
//  Save a new []WSID
//***************************************************************************

BOOL SaveNewWsid
    (LPAPLCHAR lpMemSaveWSID)           // Ptr to []WSID to save (includes WKSEXT)

{
    HGLOBAL      hGlbPTD,               // PerTabData global memory handle
                 hGlbWSID;              // []WSID global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    int          iLen,                  // Length of []WSID (with WKSEXT)
                 iLen2;                 // ...              (w/o  ...)
    APLUINT      ByteWSID;              // # bytes in the []WSID
    LPAPLCHAR    lpMemNewWSID;          // Ptr to new []WSID global memory
    BOOL         bRet = FALSE;          // TRUE iff result is valid

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the length of the []WSID (including WKSEXT)
    iLen = lstrlenW (lpMemSaveWSID);

    // If the []WSID is non-empty, ...
    if (iLen)
    {
        // Omit the trailing WKSEXT
        iLen2 = iLen - ((sizeof WKSEXT) - 1);
        Assert (lpMemSaveWSID[iLen2] EQ L'.');
        lpMemSaveWSID[iLen2] = L'\0';

        // Calculate space needed for the new WSID
        // Include "+ 1" for a trailing zero
        ByteWSID = CalcArraySize (ARRAY_CHAR, iLen2 + 1, 1);

        // Allocate space for the new WSID
        // N.B. Conversion from APLUINT to UINT.
        Assert (ByteWSID EQ (UINT) ByteWSID);
        hGlbWSID = DbgGlobalAlloc (GHND, (UINT) ByteWSID);
        if (!hGlbWSID)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemNewWSID = MyGlobalLock (hGlbWSID);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemNewWSID)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = 0;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = iLen2;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemNewWSID) = iLen2;

        // Skip over the header and dimensions to the data
        lpMemNewWSID = VarArrayBaseToData (lpMemNewWSID, 1);

        // Copy data to the new []WSID
        CopyMemory (lpMemNewWSID, lpMemSaveWSID, iLen2 * sizeof (lpMemSaveWSID[0]));

        // We no longer need this ptr
        MyGlobalUnlock (hGlbWSID); lpMemNewWSID = NULL;
    } else
        hGlbWSID = hGlbV0Char;

    // Free the old []WSID
    FreeResultGlobalVar (lpMemPTD->lpSymQuadWSID->stData.stGlbData); lpMemPTD->lpSymQuadWSID->stData.stGlbData = NULL;

    // Save the new []WSID
    lpMemPTD->lpSymQuadWSID->stData.stGlbData = MakePtrTypeGlb (hGlbWSID);

    // Tell the Tab Ctrl about the new workspace name
    NewTabName ();

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Display the error message
    AppendLine (ERRMSG_NOT_SAVED_WS_FULL, FALSE, TRUE);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    if (iLen)
        // Restore zapped char
        lpMemSaveWSID[iLen2] = L'.';

    return bRet;
} // End SaveNewWsid


//***************************************************************************
//  End of File: sc_save.c
//***************************************************************************
