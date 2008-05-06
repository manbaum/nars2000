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
    // If the name doesn't begin with a drive letter and
    //   doesn't start at the root or a dot, prepend the
    //   default dir
    if (wszInp[0] NE L'\0'      // Non-empty,
     && wszInp[0] NE L'.'       // and not current dir,
     && wszInp[0] NE L'\\'      // and not root dir,
     && wszInp[1] NE L':')      // and no drive letter
    {
        lstrcpyW (wszOut, wszDefDir);
        lstrcatW (wszOut, wszInp);
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
                 hGlbCnt = NULL,        // Vars/Fcns/Oprs counters global memory handle
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
                 uGlbCnt=0;             // # entries in [Globals] section
    WCHAR        wszCount[8],           // Output save area for formatted uSymxxx counter
                 wszTimeStamp[16 + 1];  // ...                            time stamps
    FILE        *fStream;               // Ptr to file stream
    LPSYMENTRY   lpSymEntry,            // Ptr to STE
                 lpSymTabNext,          // ...
                 lpSymLink = NULL;      // Anchor of SYMENTRY links for [Globals] values
                                        //   so we may delete them easily
    STFLAGS      stFlags;               // STE flags
    SYSTEMTIME   systemTime;            // Current system (UTC) time
    FILETIME     ftCreation;            // Creation time

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
    MakeWorkspaceNameCanonical (wszWsidDPFE, wszTempDPFE, wszWorkDir);

    // If there is a specified WS Name,
    //   and the current []WSID is different from the specified WS Name,
    //   and a WS with the specified WS Name already exists,
    //   display an error
    //     "NOT SAVED, THIS WS IS ",[]WSID
    if (*lpwszTail)
    {
        // Convert the given workspace name into a canonical form (without WKSEXT)
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, wszWorkDir);

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
                ReplaceLastLineCRPmt (lpwszTemp);

                goto ERROR_EXIT;
            } // End IF

            // We no longer need this ptr
            MyGlobalUnlock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemOldWSID = NULL;

            // Set the value of the new []WSID as wszTailDPFE
            if (!SaveNewWsid_EM (wszTailDPFE))
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
    hGlbCnt = MyGlobalAlloc (GHND, 2 *
    sizeof (UINT) * (lpMemPTD->SILevel + 1));
    if (!hGlbCnt)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemCnt = MyGlobalLock (hGlbCnt);

    // The format of a saved ws is as an .ini file
    //   (named *.ws.nars) with the following sections:
    //   [General]    (General settings)
    //   [Globals]    (Ptrs to global memory variables/function/operators)
    //   [Vars.0]     (Global variables) -- targets of )COPY
    //   [Vars.1]     (Variables at level 1 in the SI stack)
    //   [Vars.sss]   (Variables at level nnn in the SI stack)
    //   [Fcns.0]     (Global functions) -- targets of )COPY
    //   [Fcns.1]     (Functions at level 1 in the SI stack)
    //   [Fcns.sss]   (Functions at level nnn in the SI stack)
    // The [Globals] section has content of
    //  FMTSTR_GLBCNT=V T N R S val for variables -- %c %d %d %*(d) ...
    //                              where V is V indicating a variable
    //                                    T is the variable type (BIFCHNA) -- %c
    //                                    N is the NELM -- %d
    //                                    R is the rank -- %d
    //                                    S is the shape -- Rank occurrences of %d
    //                                    value is the series of values of similar form to FMTSTR_GLBCNT
    //                                      without the leading V.
    //  FMTSTR_GLBCNT=F nnn.Name    for user-defined functions/operators
    //                              where F is F indicating a function
    //                                    nnn is a counter across functions & variables
    //                                    Name is the name of the function
    // The [Vars.sss] section has content of
    //  xxx=Name=FMTSTR_GLBCNT      for a variable in [Globals]
    //  xxx=Name=T 1 0 value        for an immediate scalar variable
    //                              where T is the variable immediate type (BIFC)
    // The [Fcns.sss] section has content of
    //  xxx=Name=y=FMTSTR_GLBCNT    for a function in [Globals] with NameType y
    //  xxx=Name=y={name} or char   for immediate functions
    // The [nnn.Name] section (used for user-defined functions/operators only)
    //   has content of
    //  xxx=<Function Header/Lines>

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
                              ConvertWideToNameLength (lpaplChar,                   // Ptr to output save buffer
                                                      &lpSymEntry->stData.stChar,   // Ptr to incoming chars
                                                       1);                          // # chars to convert
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
                                            &uGlbCnt,
                                             lpSymEntry,
                                            &lpSymLink);
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
                    // Format the function section name as F nnn.Name where nnn is the count
                    wsprintfW (lpwszFormat,
                               L"F %d.%s",
                               uGlbCnt,
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
                          ConvertWideToNameLength (lpaplChar,   // Ptr to output save buffer
                                                  &aplChar,     // Ptr to incoming chars
                                                   1);          // # chars to convert
                    } else
                        // Convert the function/operator in global memory to saved ws form
                        lpaplChar =
                          SavedWsFormGlbFcn (lpaplChar,
                                             lpwszFormat,
                                             lpSymEntry->stData.stGlbData,
                                             lpMemSaveWSID,
                                            &uGlbCnt,
                                             lpSymEntry,
                                            &lpSymLink);
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

    // Delete the FMTSTR_GLBOBJ=FMTSTR_GLBCNT references in the [Globals] section
    while (lpSymLink)
    {
        LPSYMENTRY lpSymLast;           // Temporary ptr to previous stSymLink
        WCHAR      wszGlbObj[16 + 1];   // Save area for formatted hGlbObj
                                        //   (room for 64-bit ptr plus terminating zero)

        // Format the global memory handle
        wsprintfW (wszGlbObj,
                   FMTSTR_GLBOBJ,
                   ClrPtrTypeDirAsGlb (lpSymLink->stData.stGlbData));
        // Delete the FMTSTR_GLBOBJ= entry in the [Globals] section
        WritePrivateProfileStringW (SECTNAME_GLOBALS,           // Ptr to the section name
                                   wszGlbObj,                   // Ptr to key name
                                   NULL,                        // Ptr to key value (NULL to delete)
                                   lpMemSaveWSID);              // Ptr to the file name
        // Point to the next entry and zap it
        lpSymLast = lpSymLink->stSymLink;
        lpSymLink->stSymLink = NULL;

        // Point to the next one
        lpSymLink = lpSymLast;
    } // End WHILE

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
    // Format the [Globals] count
    wsprintfW (wszCount,
               L"%d",
               uGlbCnt);
    // Write out the count to the [Globals] section
    WritePrivateProfileStringW (SECTNAME_GLOBALS,               // Ptr to the section name
                                KEYNAME_COUNT,                  // Ptr to the key name
                                wszCount,                       // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Write out the SI stack to the [SI] section
    CmdSiSinlCom_EM (L"",               // Ptr to command tail
                     TRUE,              // TRUE iff )SINL
                     lpMemSaveWSID);    // Ptr to the file name
    // Get the current system (UTC) time
    GetSystemTime (&systemTime);

    // Convert system time to file time and save as creation time
    SystemTimeToFileTime (&systemTime, &ftCreation);

    // Format the creation time
    wsprintfW (wszTimeStamp,
               L"%08X%08X",
               ftCreation.dwHighDateTime,
               ftCreation.dwLowDateTime);

    // Write out the creation time to the [General] section
    WritePrivateProfileStringW (SECTNAME_GENERAL,               // Ptr to the section name
                                KEYNAME_CREATIONTIME,           // Ptr to the key name
                                wszTimeStamp,                   // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Note if the SI is non-empty
    if (lpMemPTD->SILevel)
        ReplaceLastLineCRPmt (L"WARNING:  SI non-empty -- not restartable after )LOAD");

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
    ReplaceLastLineCRPmt (lpwszTemp);

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Display the error message
    ReplaceLastLineCRPmt (ERRMSG_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

NOTSAVED_CLEAR_EXIT:
    // Display the error message
    ReplaceLastLineCRPmt (ERRMSG_NOT_SAVED_CLEAR_WS);

    goto ERROR_EXIT;

NOTSAVED_FILE_EXIT:
    // Display the error message
    ReplaceLastLineCRPmt (ERRMSG_NOT_SAVED_FILE_ERROR APPEND_NAME);

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
    (LPAPLCHAR   lpaplChar,             // Ptr to output save area
     LPAPLCHAR   lpwszFcnTypeName,      // Ptr to the function section name as F nnn.Name where nnn is the count
     HGLOBAL     hGlbObj,               // WS object global memory handle
     LPAPLCHAR   lpMemSaveWSID,         // Ptr to saved WS file DPFE
     LPUINT      lpGlbCnt,              // Ptr to [Globals] count
     LPSYMENTRY  lpSymEntry,            // Ptr to this global's SYMENTRY
     LPSYMENTRY *lplpSymLink)           // Ptr tp ptr to SYMENTRY link

{
    LPVOID            lpMemObj = NULL;      // Ptr to WS object ...
    LPAPLCHAR         lpMemProKeyName,      // Ptr to profile keyname
                      lpaplCharStart,       // Ptr to start of buffer
                      lpaplCharStart2,      // Ptr to start of buffer
                      lpwszSectName;        // Ptr to the section name as nnn.Name where nnn is the count
    UINT              uLine;                // Function line loop counter
    FILETIME          ftCreation,           // Object creation time
                      ftLastMod;            // ...    last modification time
    WCHAR             wszGlbObj[16 + 1],    // Save area for formatted hGlbObj
                                            //   (room for 64-bit ptr plus terminating zero)
                      wszGlbCnt[8 + 1];     // Save area for formatted *lpGlbCnt
    LPSYMENTRY        lpSymLink;            // Ptr to SYMENTRY temp for *lplpSymLink
    SAVEDWSGLBVARPARM SavedWsGlbVarParm;    // Extra parms for SavedWsGlbVarConv

    Assert (IsGlbTypeFcnDir (hGlbObj)
         || IsGlbTypeDfnDir (hGlbObj));

    // Skip over the 'F ' to point to the section name
    lpwszSectName = &lpwszFcnTypeName[2];

    // Save ptr to start of buffer
    lpaplCharStart = lpaplChar;

    // Clear the ptr type bits
    hGlbObj = ClrPtrTypeDirAsGlb (hGlbObj);

    // Format the hGlbObj
    wsprintfW (wszGlbObj,
               FMTSTR_GLBOBJ,
               hGlbObj);
    // Save as ptr to the profile keyname
    lpMemProKeyName = lpaplChar;

    // Check to see if this global has already been saved
    //   in the [Globals] section
    if (GetPrivateProfileStringW (SECTNAME_GLOBALS,             // Ptr to the section name
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
            // Fill in the extra parms
            SavedWsGlbVarParm.lpMemSaveWSID = lpMemSaveWSID;
            SavedWsGlbVarParm.lpGlbCnt      = lpGlbCnt;
            SavedWsGlbVarParm.lpSymEntry    = lpSymEntry;
            SavedWsGlbVarParm.lplpSymLink   = lplpSymLink;

            // Call the common function display function
            lpaplChar =
              DisplayFcnGlb (lpaplCharStart,        // Ptr to output save area
                             hGlbObj,               // Function array global memory handle
                             FALSE,                 // TRUE iff we're to display the header
                            &SavedWsGlbVarConv,     // Ptr to function to convert an HGLOBAL to {{nnn}} (may be NULL)
                            &SavedWsGlbVarParm);    // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
            // Ensure properly terminated
            *lpaplChar++ = L'\0';

            // Save as new start of buffer
            lpaplCharStart2 = lpaplChar;

            // Format the text as an ASCII string with non-ASCII chars
            //   represented as either {symbol} or {\xXXXX} where XXXX is
            //   a four-digit hex number.
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,                   // Ptr to output save buffer
                                       lpaplCharStart,              // Ptr to incoming chars
                                       lstrlenW (lpaplCharStart));  // # chars to convert
            // Ensure properly terminated
            *lpaplChar++ = L'\0';

            // Copy creation time
            ftCreation = ((LPFCNARRAY_HEADER) lpMemObj)->ftCreation;

            // Copy last mod time
            ftLastMod  = ((LPFCNARRAY_HEADER) lpMemObj)->ftLastMod;

            // Write out the single line
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        L"0",               // Ptr to the key name
                                        lpaplCharStart2,    // Ptr to the key value
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

    // Write out the entry in the [Globals] section
    WritePrivateProfileStringW (SECTNAME_GLOBALS,               // Ptr to the section name
                                wszGlbCnt,                      // Ptr to the key value
                                lpwszFcnTypeName,               // Ptr to the key name
                                lpMemSaveWSID);                 // Ptr to the file name
    // Write out the saved marker
    WritePrivateProfileStringW (SECTNAME_GLOBALS,               // Ptr to the section name
                                wszGlbObj,                      // Ptr to the key name
                                wszGlbCnt,                      // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Move back to the start
    lpaplChar = lpaplCharStart;

    // Link this SYMENTRY into the chain
    lpSymLink = *lplpSymLink;
    *lplpSymLink = lpSymEntry;
    lpSymEntry->stSymLink = lpSymLink;

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
    ConvertWideToNameLength (lpwFormat,                     // Ptr to output save buffer
                            &lpMemTxtLine->C,               // Ptr to incoming chars
                             lpMemTxtLine->U);              // # chars to convert
    // We no longer need this ptr
    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

    // Write out the entry (nnn = FunctionLine)
    WritePrivateProfileStringW (lpwszSectName,              // Ptr to the section name
                                wszCount,                   // Ptr to the key name
                                lpwFormat,                  // Ptr to the key value
                                lpMemSaveWSID);             // Ptr to the file name
} // End WriteFunctionLine


//***************************************************************************
//  $SavedWsFormGlbVar
//
//  Convert a variable in global memory to saved ws form
//***************************************************************************

LPAPLCHAR SavedWsFormGlbVar
    (LPAPLCHAR   lpaplChar,             // Ptr to output save area
     HGLOBAL     hGlbObj,               // WS object global memory handle
     LPAPLCHAR   lpMemSaveWSID,         // Ptr to saved WS file DPFE
     LPUINT      lpGlbCnt,              // Ptr to [Globals] count
     LPSYMENTRY  lpSymEntry,            // Ptr to this global's SYMENTRY
     LPSYMENTRY *lplpSymLink)           // Ptr to ptr to SYMENTRY link

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
    LPSYMENTRY   lpSymLink;             // Ptr to SYMENTRY temp for *lplpSymLink

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
               FMTSTR_GLBOBJ,
               hGlbObj);
    // Check to see if this global has already been saved
    //   in the [Globals] section
    if (GetPrivateProfileStringW (SECTNAME_GLOBALS,             // Ptr to the section name
                                  wszGlbObj,                    // Ptr to the key name
                                  L"",                          // Default value if not found
                                  lpaplChar,                    // Ptr to the output buffer
                                  8 * sizeof (lpaplChar[0]),    // Byte size of the output buffer
                                  lpMemSaveWSID))               // Ptr to the file name
        goto NORMAL_EXIT;

    // Save as ptr to the profile keyname
    lpMemProKeyName = lpaplChar;

    // Save ptr to start of buffer
    lpaplCharStart = lpaplChar;

    // Append the marker as a variable
    *lpaplChar++ = L'V';
    *lpaplChar++ = L' ';

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
                  ConvertWideToNameLength (lpaplChar,           // Ptr to output save buffer
                                           (LPAPLCHAR) lpMemObj,// Ptr to incoming chars
                                           1);                  // # chars to convert
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
                      ConvertWideToNameLength (lpaplChar,                               // Ptr to output save buffer
                                              &(*(LPAPLHETERO) lpMemObj)->stData.stChar,// Ptr to incoming chars
                                               1);                                      // # chars to convert
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

                        // Convert the variable in global memory to saved ws form
                        lpaplChar =
                          SavedWsFormGlbVar (lpaplChar,
                                             hGlbSub,
                                             lpMemSaveWSID,
                                             lpGlbCnt,
                                             lpSymEntry,
                                             lplpSymLink);
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

    // Write out the entry in the [Globals] section
    WritePrivateProfileStringW (SECTNAME_GLOBALS,               // Ptr to the section name
                                wszGlbCnt,                      // Ptr to the key value
                                lpMemProKeyName,                // Ptr to the key name
                                lpMemSaveWSID);                 // Ptr to the file name
    // Write out the saved marker
    WritePrivateProfileStringW (SECTNAME_GLOBALS,               // Ptr to the section name
                                wszGlbObj,                      // Ptr to the key name
                                wszGlbCnt,                      // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Move back to the start
    lpaplChar = lpaplCharStart;

    // Link this SYMENTRY into the chain
    lpSymLink = *lplpSymLink;
    *lplpSymLink = lpSymEntry;
    lpSymEntry->stSymLink = lpSymLink;

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
//  $SaveNewWsid_EM
//
//  Save a new []WSID
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SaveNewWsid_EM"
#else
#define APPEND_NAME
#endif

BOOL SaveNewWsid_EM
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
    ReplaceLastLineCRPmt (ERRMSG_NOT_SAVED_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    if (iLen)
        // Restore zapped char
        lpMemSaveWSID[iLen2] = L'.';

    return bRet;
} // End SaveNewWsid_EM
#undef  APPEND_NAME


//***************************************************************************
//  $SavedWsGlbVarConv
//
//  Callback function for DisplayFcnGlb to format a global
//***************************************************************************

LPAPLCHAR SavedWsGlbVarConv
    (LPAPLCHAR           lpaplChar,                 // Ptr to output save area
     HGLOBAL             hGlbObj,                   // Object global memory handle
     LPSAVEDWSGLBVARPARM lpSavedWsGlbVarParm)       // Ptr to extra parameters for lpSavedWsGlbVarConv

{

    // Convert the variable in global memory to saved ws form
    lpaplChar =
      SavedWsFormGlbVar (lpaplChar,
                         hGlbObj,
                         lpSavedWsGlbVarParm->lpMemSaveWSID,
                         lpSavedWsGlbVarParm->lpGlbCnt,
                         lpSavedWsGlbVarParm->lpSymEntry,
                         lpSavedWsGlbVarParm->lplpSymLink);
    // Include a trailing blank
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End SavedWsGlbVarConv


//***************************************************************************
//  End of File: sc_save.c
//***************************************************************************
