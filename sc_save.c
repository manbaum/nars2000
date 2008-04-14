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
                 hGlbObj;               // WS object (fn, op, var) globalmemory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPVOID       lpMemOldWSID = NULL,   // Ptr to old []WSID global memory
                 lpMemObj;              // Ptr to WS object ...
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
    UINT         uSymVar,               // Var index counter
                 uSymFcn,               // Fcn/Opr index counter
                 uGlbCnt = 0,           // # entries in [Globals] section
                 uLine;                 // Function line loop counter
    WCHAR        wszCount[8],           // Output save area for formatted uSymxxx counter
                 wch[2];                // Save area to create one-char string
    FILE        *fStream;               // Ptr to file stream
    LPSYMENTRY   lpSymEntry,            // Ptr to STE
                 lpSymTabNext;          // ...
    FILETIME     ftCreation;            // Object creation time

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

    // Terminate the one-char string
    wch[1] = L'\0';

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
        {
            // Display the error message
            AppendLine (ERRMSG_NOT_SAVED_CLEAR_WS, FALSE, TRUE);

            goto ERROR_EXIT;
        } // End IF

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

    // The format of a saved ws is as an .ini file
    //   (named *.ws.nars) with the following sections:
    //   [General]    (General settings)
    //   [Fns.0]      (Global functions)
    //   [Fns.1]      (Functions at level 1 in the SI stack)
    //   [Fns.nnn]    (Functions at level nnn in the SI stack)
    //   [Vars.0]     (Global variables)
    //   [Vars.1]     (Variables at level 1 in the SI stack)
    //   [Vars.nnn]   (Variables at level nnn in the SI stack)
    // Each of the above sections has content of
    //   1          = Name      (1st name)
    //   2          = Name      (2nd name)
    //   xxx        = Name      (xxxth name)
    // Corresponding to each of the above names, there is a
    //   separate section [nnn.Name], for example,
    //   [0.Name]     (The contents of the name "Name" at level 0)

    // Create (or truncate the file)
    fStream = _wfopen (lpMemSaveWSID, L"w");
    fclose (fStream); fStream = NULL;

    // Write out the [General] section
    WritePrivateProfileStringW (L"General",     // Ptr to the section name
                                L"Version",     // Ptr to the key name
                                L"0.01",        // Ptr to the key value
                                lpMemSaveWSID); // Ptr to the file name
    // Trundle through the Symbol Table
    for (lpSymTabNext = lpMemPTD->lpSymTab, uSymVar = uSymFcn = 0;
         lpSymTabNext < lpMemPTD->lpSymTabNext;
         lpSymTabNext++)
    if (lpSymTabNext->stFlags.Inuse)        // Must be Inuse
    // Handle different SI levels
    for (lpSymEntry = lpSymTabNext; lpSymEntry; lpSymEntry = lpSymEntry->stPrvEntry)
    if (lpSymEntry->stHshEntry->htGlbName)  // Must have a name (not steZero, etc.)
    {
        // Get the symbol name's global memory handle
        hGlbName = lpSymEntry->stHshEntry->htGlbName;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (hGlbName);

        // Format the text as an ASCII string with non-ASCII chars
        //   represented as either {symbol} or {\xXXXX} where XXXX is
        //   a four-digit hex number.
        uNameLen = ConvertWideToName (lpwszTemp, lpMemName);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbName); lpMemName = NULL;

        // Point to the end of the name
        lpaplChar = &lpwszTemp[uNameLen];

        if (lpSymEntry->stFlags.Value)      // Must have a value
        {
            STFLAGS stFlags;        // STE flags

            // Save the flags
            stFlags = lpSymEntry->stFlags;

            // Split cases based upon the object type
            switch (stFlags.ObjType)
            {
                case NAMETYPE_VAR:
                {
                    // stData is a variable
                    Assert (IsNameTypeVar (stFlags.ObjType));

                    // Append separator
                    *lpaplChar++ = L'=';

                    // If it's immediate, ...
                    if (stFlags.Imm)
                    {
                        // Append the storage type
                        *lpaplChar++ = TranslateImmTypeToChar (stFlags.ImmType);

                        // Append the rank
                        *lpaplChar++ = L'0';
                        *lpaplChar++ = L' ';

                        // If it's char, ...
                        if (stFlags.ImmType EQ IMMTYPE_CHAR)
                        {
                            // Append a leading single quote
                            *lpaplChar++ = L'\'';

                            // Save the char in a string
                            wch[0] = lpSymEntry->stData.stChar;

                            // Format the text as an ASCII string with non-ASCII chars
                            //   represented as either {symbol} or {\xXXXX} where XXXX is
                            //   a four-digit hex number.
                            lpaplChar += ConvertWideToName (lpaplChar, wch);

                            // Append a trailing single quote
                            *lpaplChar++ = L'\'';

                            // Ensure properly terminated
                            *lpaplChar = L'\0';
                        } else
                        {
                            APLUINT uQuadPP;

                            // Ensure we format with full precision in case it's floating point
                            uQuadPP = lpMemPTD->lpSymQuadPP->stData.stInteger;
                            if (IsImmFlt (stFlags.ImmType))
                                lpMemPTD->lpSymQuadPP->stData.stInteger = DEF_MAX_QUADPP;

                            // Format the value
                            lpaplChar =
                              FormatImmed (lpaplChar,
                                           stFlags.ImmType,
                                          &lpSymEntry->stData.stLongest);
                            // Restore user's precision
                            lpMemPTD->lpSymQuadPP->stData.stInteger = uQuadPP;

                            // Delete the last blank in case it matters,
                            //   and ensure properly terminated
                            if (lpaplChar[-1] EQ L' ')
                                *--lpaplChar = L'\0';
                            else
                                *lpaplChar = L'\0';
                        } // End IF/ELSE
                    } else
                    {
                        // Get the global memory handle
                        hGlbObj = lpSymEntry->stData.stGlbData;

                        // Convert the gloabl memory to transfer form
                        lpaplChar =
                          TransferFormGlb (lpaplChar,
                                           lpSymEntry->stData.stGlbData,
                                           lpMemSaveWSID,
                                          &uGlbCnt);
                    } // End IF/ELSE

                    // Format the counter
                    wsprintfW (wszCount, L"%d", uSymVar++);

                    // Format the section name
                    wsprintfW (wszSectName, L"Vars.%d", lpSymEntry->stSILevel);

                    // Write out the entry (nnn = Name = value)
                    //   in the [Vars.nnn] section
                    WritePrivateProfileStringW (wszSectName,            // Ptr to the section name
                                                wszCount,               // Ptr to the key name
                                                lpwszTemp,              // Ptr to the key value
                                                lpMemSaveWSID);         // Ptr to the file name
                    break;
                } // End NAMETYPE_VAR

                case NAMETYPE_FN0:
                case NAMETYPE_FN12:
                case NAMETYPE_OP1:
                case NAMETYPE_OP2:
                case NAMETYPE_OP3:
                    // Append terminator
                    *lpaplChar++ = L'\0';

                    // Format the counter
                    wsprintfW (wszCount, L"%d", uSymFcn++);

                    // Format the section name
                    wsprintfW (wszSectName, L"Fns.%d", lpSymEntry->stSILevel);

                    // Write out the entry (nnn = Name)
                    //   in the [Fns.nnn] or [Ops.nnn] section
                    WritePrivateProfileStringW (wszSectName,            // Ptr to the section name
                                                wszCount,               // Ptr to the key name
                                                lpwszTemp,              // Ptr to the key value
                                                lpMemSaveWSID);         // Ptr to the file name
                    // Check for immediate
                    if (stFlags.Imm)
                    {
                        LPAPLCHAR lpwszStart = lpaplChar;

                        DbgBrk ();

                        // Make it a one-char string
                        wch[0] = lpSymEntry->stData.stChar;

                        // Format the text as an ASCII string with non-ASCII chars
                        //   represented as either {symbol} or {\xXXXX} where XXXX is
                        //   a four-digit hex number.
                        lpaplChar += ConvertWideToName (lpaplChar, wch);

                        // Ensure properly terminated
                        *lpaplChar = L'\0';

                        // Format the section name as [nnn.Name]
                        wsprintfW (lpwszFormat,
                                   L"%s.%s",
                                   wszCount,
                                   lpwszTemp);
                        // Write out the single line
                        WritePrivateProfileStringW (lpwszFormat,        // Ptr to the section name
                                                    L"0",               // Ptr to the key name
                                                    lpwszStart,         // Ptr to the key value
                                                    lpMemSaveWSID);     // Ptr to the file name
                    } else
                    {
                        HGLOBAL       hGlbTxtLine;
                        LPMEMTXT_UNION lpMemTxtLine;

                        // Get the global memory handle
                        hGlbObj = lpSymEntry->stData.stGlbData;

                        Assert (IsGlbTypeFcnDir (hGlbObj)
                             || IsGlbTypeDfnDir (hGlbObj));

                        // Clear the ptr type bits
                        hGlbObj = ClrPtrTypeDirAsGlb (hGlbObj);

                        // Lock the memory to get a ptr to it
                        lpMemObj = MyGlobalLock (hGlbObj);

                        // Split cases based upon the object's signature
                        switch (((LPHEADER_SIGNATURE) lpMemObj)->nature)
                        {
                            case FCNARRAY_HEADER_SIGNATURE:
                                DbgBrk ();

                                // Get the text line global memory handle
                                hGlbTxtLine = ((LPFCNARRAY_HEADER) lpMemObj)->hGlbTxtLine;

                                // Lock the memory to get a ptr to it
                                lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                                // Format the text as an ASCII string with non-ASCII chars
                                //   represented as either {symbol} or {\xXXXX} where XXXX is
                                //   a four-digit hex number.
                                lpaplChar += ConvertWideToName (lpaplChar, &lpMemTxtLine->C);

                                // Copy creation time
                                ftCreation = ((LPFCNARRAY_HEADER) lpMemObj)->ftCreation;

                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

                                break;

                            case DFN_HEADER_SIGNATURE:
                            {
                                LPWCHAR   lpw;
                                UINT      numFcnLines;
                                LPFCNLINE lpFcnLines;       // Ptr to array of function line structs (FCNLINE[numFcnLines])

                                // Put the function definition in a separate
                                //   section named [nnn.Name] with each line
                                //   of the function on a separate line as in
                                //   0 = <Header>
                                //   1 = <First Line>, etc.
                                //   CreationTime = xxxxxxxxxxxxxxxx (64-bit hex number)

                                // Format the section name
                                lpw = lpwszFormat;
                                lpw +=
                                wsprintfW (lpwszFormat,
                                           L"%s.%s",
                                           wszCount,
                                           lpwszTemp);

#define lpMemDfnHdr     ((LPDFN_HEADER) lpMemObj)
                                numFcnLines = lpMemDfnHdr->numFcnLines;

                                // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                                lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

                                // Write out the function header
                                WriteFunctionLine (lpwszFormat,     // Ptr to the section name
                                                   lpw + 1,         // Ptr to save area for the formatted line
                                                                    // (" + 1" to skip over the terminating zero
                                                                    //  of the section name)
                                                   0,               // Line #
                                                   lpMemDfnHdr->hGlbTxtHdr,
                                                   lpMemSaveWSID);  // Ptr to the file name
                                // Loop through the function lines
                                for (uLine = 0; uLine < numFcnLines; uLine++, lpFcnLines++)
                                    WriteFunctionLine (lpwszFormat,     // Ptr to the section name
                                                       lpw + 1,         // Ptr to save area for the formatted line
                                                                        // (" + 1" to skip over the terminating zero
                                                                        //  of the section name)
                                                       uLine + 1,       // Line #
                                                       lpFcnLines->hGlbTxtLine,
                                                       lpMemSaveWSID);  // Ptr to the file name

#define lpUndoIni       lpwszTemp       // Start of output save area

                                // Write out the Undo buffer
                                if (lpMemDfnHdr->hGlbUndoBuff)
                                {
                                    LPUNDO_BUF lpMemUndo;               // Ptr to Undo Buffer global memory
                                    UINT       uUndoCount;              // # entries in the Undo Buffer
                                    LPAPLCHAR  lpUndoOut;               // Ptr to output save area
                                    APLCHAR    undoChar[] = L" '?'";    // Undo character string

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
                                                lpUndoChar = L" '\\n'";

                                                break;

                                            case L'\r':
                                                lpUndoChar = L" '\\r'";

                                                break;

                                            default:
                                                // Copy to string
                                                undoChar[2] = lpMemUndo->Char;
                                                lpUndoChar = undoChar;

                                                break;
                                        } // End SWITCH

                                        // Format this element of the Undo Buffer
                                        lpUndoOut +=
                                          wsprintfW (lpUndoOut,
                                                     L"%c %d %d %d%s, ",
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
                                WritePrivateProfileStringW (lpwszFormat,            // Ptr to the section name
                                                            L"Undo",                // Ptr to the key name
                                                            lpUndoIni,              // Ptr to the key value
                                                            lpMemSaveWSID);         // Ptr to the file name
                                // ***FIXME*** -- Write out the stop/trace bits







                                // Copy creation time
                                ftCreation = lpMemDfnHdr->ftCreation;
#undef  lpMemDfnHdr
                                break;
                            } // End DFN_HEADER_SIGNATURE

                            defstop
                                break;
                        } // End SWITCH

                        // Save the ftCreation time
                        wsprintfW (lpwszTemp,
                                   L"%08X%08X",
                                   ftCreation.dwHighDateTime,
                                   ftCreation.dwLowDateTime);
                        WritePrivateProfileStringW (lpwszFormat,        // Ptr to the section name
                                                    L"CreationTime",    // Ptr to the key name
                                                    lpwszTemp,          // Ptr to the key value
                                                    lpMemSaveWSID);     // Ptr to the file name
                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
                    } // End IF/ELSE

                    // Ensure properly terminated
                    *lpaplChar = L'\0';

                    break;

                defstop
                    break;
            } // End SWITCH
        } else
        if (lpSymEntry->stSILevel)      // Must be suspended
        {
            // Format the section name
            wsprintfW (wszSectName, L"Vars.%d", lpSymEntry->stSILevel);

            // Write out the entry (nnn = Name = value)
            //   in the [Vars.nnn] section
            WritePrivateProfileStringW (wszSectName,            // Ptr to the section name
                                        lpwszTemp,              // Ptr to the key name
                                        L"",                    // Ptr to the key value
                                        lpMemSaveWSID);         // Ptr to the file name
        } // End IF/ELSE/...
    } // End FOR/IF/...

    // Format the Var count
    wsprintfW (wszCount,
               L"%d",
               uSymVar);
    // Write out the Var count to the [General] section
    WritePrivateProfileStringW (L"General",                         // Ptr to the section name
                                L"VarCount",                        // Ptr to the key name
                                wszCount,                           // Ptr to the key value
                                lpMemSaveWSID);                     // Ptr to the file name
    // Format the Fcn/Opr count
    wsprintfW (wszCount,
               L"%d",
               uSymFcn);
    // Write out the Fcn count to the [General] section
    WritePrivateProfileStringW (L"General",                         // Ptr to the section name
                                L"FcnCount",                        // Ptr to the key name
                                wszCount,                           // Ptr to the key value
                                lpMemSaveWSID);                     // Ptr to the file name
    // Format the SI depth
    wsprintfW (wszCount,
               L"%d",
               lpMemPTD->SILevel);
    // Write out the SI depth to the [General] section
    WritePrivateProfileStringW (L"General",                         // Ptr to the section name
                                L"SIDepth",                         // Ptr to the key name
                                wszCount,                           // Ptr to the key value
                                lpMemSaveWSID);                     // Ptr to the file name
    // Format the [Globals] count
    wsprintfW (wszCount,
               L"%d",
               uGlbCnt);
    // Write out the [Globals] count to the [Globals] section
    WritePrivateProfileStringW (L"Globals",                         // Ptr to the section name
                                L"Count",                           // Ptr to the key name
                                wszCount,                           // Ptr to the key value
                                lpMemSaveWSID);                     // Ptr to the file name
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
ERROR_EXIT:
    if (lpMemOldWSID)
    {
        // We no longer need this ptr
        MyGlobalUnlock (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemOldWSID = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End CmdSave_EM
#undef  APPEND_NAME


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
        {
            AppendLine (ERRMSG_NOT_SAVED_WS_FULL, FALSE, TRUE);

            goto ERROR_EXIT;
        } // End IF

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
    FreeResultGlobalVar (ClrPtrTypeDirAsGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData));

    // Save the new []WSID
    lpMemPTD->lpSymQuadWSID->stData.stGlbData = MakePtrTypeGlb (hGlbWSID);

    // Tell the Tab Ctrl about the new workspace name
    NewTabName ();

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    if (iLen)
        // Restore zapped char
        lpMemSaveWSID[iLen2] = L'.';

    return bRet;
} // End SaveNewWsid


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
    ConvertWideToName (lpwFormat, &lpMemTxtLine->C);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

    // Write out the entry (nnn = FunctionLine)
    WritePrivateProfileStringW (lpwszSectName,                      // Ptr to the section name
                                wszCount,                           // Ptr to the key name
                                lpwFormat,                          // Ptr to the key value
                                lpMemSaveWSID);                     // Ptr to the file name
} // End WriteFunctionLine


//***************************************************************************
//  $TransferFormGlb
//
//  Convert global memory to transfer form
//***************************************************************************

LPAPLCHAR TransferFormGlb
    (LPAPLCHAR lpaplChar,               // Ptr to output save area
     HGLOBAL   hGlbObj,                 // WS object global memory handle
     LPAPLCHAR lpMemSaveWSID,           // Ptr to saved WS file DPFE
     LPUINT    lpGlbCnt)                // Ptr to [Globals] count

{
    APLSTYPE     aplTypeObj;            // WS object storage type
    APLNELM      aplNELMObj;            // ...       NELM
    APLRANK      aplRankObj;            // ...       rank
    APLINT       apaOff,                // APA offset
                 apaMul;                // ... multiplier
    WCHAR        wch[2];                // Save area to create one-char string
    APLUINT      uObj,                  // WS object loop counter
                 uQuadPP;               // []PP save area
    LPVOID       lpMemObj;              // Ptr to WS object global memory
    LPAPLCHAR    lpMemProKey,           // Ptr to profile keyname
                 lpMemProVal;           // Ptr to profile value
    UINT         uBitIndex;             // Bit index when marching through Booleans
    HGLOBAL      hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbObj));

    // Clear the ptr type bits
    hGlbObj = ClrPtrTypeDirAsGlb (hGlbObj);

    // Save as ptr to the profile keyname
    lpMemProKey = lpaplChar;

    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLock (hGlbObj);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemObj)
    // Get the Array Type, NELM, and Rank
    aplTypeObj = lpHeader->ArrType;
    aplNELMObj = lpHeader->NELM;
    aplRankObj = lpHeader->Rank;

    // Append the HGLOBAL name
    lpaplChar +=
    wsprintfW (lpaplChar,
               L"%c%08X ",
               L"GP"[lpHeader->PermNdx NE PERMNDX_NONE],
               hGlbObj);
#undef  lpHeader

    // Terminate the keyname and one-char string
    *lpaplChar++ = wch[1] = L'\0';

    // Save as ptr to the profile value
    lpMemProVal = lpaplChar;

    // Check to see if it's already in the [Globals] section
    GetPrivateProfileStringW (L"Globals",                       // Ptr to the section name
                              lpMemProKey,                      // Ptr to the key name
                              L"",                              // Ptr to the default value
                              lpMemProVal,                      // Ptr to the output buffer
                              2,                                // Byte size of the output buffer
                              lpMemSaveWSID);                   // Ptr to the file name
    if (lpMemProVal[0] NE L'\0')
        goto NORMAL_EXIT;

    // The array values are preceded by the array
    //   attributes in the form of
    //   StorageType   NELM   Rank  Shape
    //   "BIFCHNLA"     %d     %d   *(%d )
    //   %c %d %d %*(d )

    // Append the storage type as a WCHAR
    *lpaplChar++ = TranslateArrayTypeToChar (aplTypeObj);

    // Append a separator
    *lpaplChar++ = L' ';

    // Append the NELM (with a trailing blank)
    lpaplChar =
      FormatAplint (lpaplChar, aplNELMObj);

    // Append the rank (with a trailing blank)
    lpaplChar =
      FormatAplint (lpaplChar, aplRankObj);

    // Append the shape (each with a trailing blank)
    for (uObj = 0; uObj < aplRankObj; uObj++)
        // Format the dimension
        lpaplChar =
          FormatAplint (lpaplChar,
                        (VarArrayBaseToDim (lpMemObj))[uObj]);

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
                  FormatAplint (lpaplChar,
                                BIT0 & ((*(LPAPLBOOL) lpMemObj) >> uBitIndex));
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
                  FormatAplint (lpaplChar,
                                *(LPAPLINT) lpMemObj);
            break;

        case ARRAY_FLOAT:
            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLFLOAT) lpMemObj)++)
                // Format the value
                lpaplChar =
                  FormatFloat (lpaplChar,
                               *(LPAPLFLOAT) lpMemObj,
                               DEF_MAX_QUADPP);
            break;

        case ARRAY_CHAR:
            // Append a leading single quote
            *lpaplChar++ = L'\'';

            // Terminate the one-char string
            wch[1] = L'\0';

            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLCHAR) lpMemObj)++)
            {
                // Save the char in a string
                wch[0] = *(LPAPLCHAR) lpMemObj;

                // Format the text as an ASCII string with non-ASCII chars
                //   represented as either {symbol} or {\xXXXX} where XXXX is
                //   a four-digit hex number.
                lpaplChar += ConvertWideToName (lpaplChar, wch);
            } // End FOR

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

            // Append the leading "APA("
#define APPEND_TEXT     L"APA("
            lstrcpyW (lpaplChar, APPEND_TEXT);
            lpaplChar += (sizeof (APPEND_TEXT) / sizeof (WCHAR)) - 1;
#undef  APPEND_TEXT

            // Append the offset
            lpaplChar =
              FormatAplintFC (lpaplChar,
                              apaOff,
                              L'-');
            // Append a comma, overwriting the trailing blank
            lpaplChar[-1] = L',';

            // Append the multiplier
            lpaplChar =
              FormatAplintFC (lpaplChar,
                              apaMul,
                              L'-');
            // Append a comma, overwriting the trailing blank
            lpaplChar[-1] = L',';

            // Append the length
            lpaplChar =
              FormatAplint (lpaplChar,
                            aplNELMObj);
            // Append the trailing marker, overwriting the trailing blank
            lpaplChar[-1] = L')';

            break;

        case ARRAY_HETERO:
            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLHETERO) lpMemObj)++)
            // Split cases based upon the element immediate type
            switch ((*(LPAPLHETERO) lpMemObj)->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpaplChar =
                      FormatAplint (lpaplChar,
                                    (*(LPAPLHETERO) lpMemObj)->stData.stBoolean);
                    break;

                case IMMTYPE_INT:
                    lpaplChar =
                      FormatAplint (lpaplChar,
                                    (*(LPAPLHETERO) lpMemObj)->stData.stInteger);
                    break;

                case IMMTYPE_CHAR:
                    // Append a leading single quote
                    *lpaplChar++ = L'\'';

                    // Save the char in a string
                    wch[0] = (*(LPAPLHETERO) lpMemObj)->stData.stChar;

                    // Format the text as an ASCII string with non-ASCII chars
                    //   represented as either {symbol} or {\xXXXX} where XXXX is
                    //   a four-digit hex number.
                    lpaplChar += ConvertWideToName (lpaplChar, wch);

                    // Append a trailing single quote
                    *lpaplChar++ = L'\'';

                    // Append a trailing blank
                    *lpaplChar++ = L' ';

                    break;

                case IMMTYPE_FLOAT:
                    // Format the value
                    lpaplChar =
                      FormatFloat (lpaplChar,
                                   (*(LPAPLHETERO) lpMemObj)->stData.stFloat,
                                   DEF_MAX_QUADPP);
                    break;

                defstop
                    break;
            } // End SWITCH

            break;

        case ARRAY_NESTED:
            // Append a leading '('
            *lpaplChar++ = L'(';

            // Append a leading "{enclose}"
#define APPEND_TEXT     L"{enclose}"
////////////lstrcpyW (lpaplChar, APPEND_TEXT);
////////////lpaplChar += (sizeof (APPEND_TEXT) / sizeof (WCHAR)) - 1;
#undef  APPEND_TEXT

            // Take into account nested prototypes
            aplNELMObj = max (aplNELMObj, 1);

            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLNESTED) lpMemObj)++)
            // Split case based upon the ptr type
            switch (GetPtrTypeInd (lpMemObj))
            {
                case PTRTYPE_STCONST:
#define lpSymEntry      (*(LPAPLHETERO) lpMemObj)

                    // Ensure we format with full precision in case it's floating point
                    uQuadPP = lpMemPTD->lpSymQuadPP->stData.stInteger;
                    if (IsImmFlt (lpSymEntry->stFlags.ImmType))
                        lpMemPTD->lpSymQuadPP->stData.stInteger = DEF_MAX_QUADPP;

                    // ***FIXME*** -- Handle non-printable chars (e.g. []TCxxx)

                    // Format the value
                    lpaplChar =
                      FormatImmed (lpaplChar,
                                   lpSymEntry->stFlags.ImmType,
                                  &lpSymEntry->stData.stLongest);
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

                        // Append a leading "({enclose}"
#define APPEND_TEXT     L"({enclose}"
                        if (!IsSimple (aplTypeSub))
                        {
                            lstrcpyW (lpaplChar, APPEND_TEXT);
                            lpaplChar += (sizeof (APPEND_TEXT) / sizeof (WCHAR)) - 1;
                        } // End IF
#undef  APPEND_TEXT
                        lpaplChar =
                          TransferFormGlb (lpaplChar,
                                          hGlbSub,
                                          lpMemSaveWSID,
                                          lpGlbCnt);
#undef  hGlbSub
                        if (!IsSimple (aplTypeSub))
                        {
                            // Delete the last blank in case it matters,
                            //   and append a trailing ')'
                            if (lpaplChar[-1] EQ L' ')
                                *--lpaplChar = L')';
                            else
                                *  lpaplChar = L')';
                            // Skip over the paren
                            lpaplChar++;
                        } // End IF
                    } // End IF/ELSE

                    break;
                } // End PTRTYPE_HGLOBAL

                defstop
                    break;
            } // End FOR

            // Delete the last blank in case it matters,
            //   and append a trailing ')'
            if (lpaplChar[-1] EQ L' ')
                *--lpaplChar = L')';
            else
                *  lpaplChar = L')';

            // Skip over the paren
            lpaplChar++;

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

    // Write out the entry in the [Globals] section
    WritePrivateProfileStringW (L"Globals",                     // Ptr to the section name
                                lpMemProKey,                    // Ptr to the key name
                                lpMemProVal,                    // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Count in another entry
    (*lpGlbCnt)++;
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbObj); lpMemObj = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Return a ptr to the profile keyname's terminating zero
    return &lpMemProKey[lstrlenW (lpMemProKey)];
} // End TransferFormGlb


//***************************************************************************
//  End of File: sc_save.c
//***************************************************************************
