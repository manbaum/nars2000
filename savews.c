//***************************************************************************
//  NARS2000 -- Save Workspace
//***************************************************************************

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <direct.h>
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

// State Indicator level
UINT SILevel = 0;


//***************************************************************************
//  $MakeWorkspaceNameCanonical
//
// Convert a workspace name into a canonical form
//***************************************************************************

void MakeWorkspaceNameCanonical
    (LPWCHAR wszOut,            // Output workspace name
     LPWCHAR wszInp,            // Input  ...
     LPWCHAR wszDefDir)         // Default drive and directory if no drive letter

{
    WCHAR wszDrive[_MAX_DRIVE], // Save area for drive letter
          wszDir[_MAX_DIR],     // ...           directory
          wszFname[_MAX_FNAME], // ...           filename
          wszExt[_MAX_EXT],     // ...           extension
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
//  $CmdSaveWS_EM
//
//  Execute the system command:  )SAVE
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdSaveWS_EM"
#else
#define APPEND_NAME
#endif

BOOL CmdSaveWS_EM
    (LPWCHAR lpwszTail)     // Command line tail with NO leading blanks

{
    HGLOBAL      hGlbPTD,               // PerTabData global memory handle
                 hGlbName,              // STE name global memory handle
                 hGlbObj;               // WS object (fn, op, var) globalmemory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    APLUINT      ByteWSID;              // # bytes in the []WSID
    LPVOID       lpMemOldWSID = NULL,   // Ptr to old []WSID global memory
                 lpMemNewWSID,          // Ptr to new ...
                 lpMemObj;              // Ptr to WS object global memory
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
                 uSymFcn,               // Fcn index counter
                 uSymOpr,               // Opr index counter
                 uLine;                 // Function line loop counter
    WCHAR        wszCount[10],          // Output save area for formatted uSymxxx counter
                 wch[2];                // Save area to create one-char string
    FILE        *fStream;               // Ptr to file stream
    LPSYMENTRY   lpSymEntry;            // Ptr to STE
    BOOL         bNameTypeFn;           // TRUE iff the name type is a function
/////////////////bNameTypeOp;           // ...                         operator
    HGLOBAL      hGlbWSID;              // []WSID global memory handle

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Lock the memory to get a ptr to it
    lpMemOldWSID = MyGlobalLock (ClrPtrTypeDirGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData));

#define lpHeader        ((LPVARARRAY_HEADER) lpMemOldWSID)

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
////wszTempDPFE[aplNELMWSID] = L'\0';   // Already done via "+ 1" in lpstrcpynW

    // Convert the []WSID workspace name into a canonical form
    MakeWorkspaceNameCanonical (wszWsidDPFE, wszTempDPFE, wszSaveDir);

    // If there is a specified WS Name,
    //   and the current []WSID is different from the specified WS Name,
    //   and a WS with the specified WS Name already exists,
    //   display an error
    //     "NOT SAVED, THIS WS IS ",[]WSID
    if (*lpwszTail)
    {
        // Convert the given workspace name into a canonical form
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

                // Copy the leading part of the message
                lstrcpyW (lpwszTemp, ERRMSG_NOT_SAVED);

                // Followed by the actual []WSID
                if (aplNELMWSID EQ 0)
                    lstrcpyW  (&lpwszTemp[lstrlenW (lpwszTemp)], L"CLEAR WS");
                else
                    lstrcpynW (&lpwszTemp[lstrlenW (lpwszTemp)], lpMemSaveWSID, (UINT) aplNELMWSID + 1);

                AppendLine (lpwszTemp, FALSE, TRUE);

                goto ERROR_EXIT;
            } // End IF
        } // End IF

        // Save as the WSID to save to
        lpMemSaveWSID = wszTailDPFE;
    } else
    // If the current []WSID is empty, display an error
    //     "NOT SAVED, THIS WS IS CLEAR WS"
    {
        // If []WSID is empty
        if (aplNELMWSID EQ 0)
        {
            AppendLine (ERRMSG_NOT_SAVED_CLEAR_WS, FALSE, TRUE);

            goto ERROR_EXIT;
        } // End IF

        // Append the common workspace extension
        lstrcatW (wszWsidDPFE, WS_WKSEXT);

        // Save as the WSID to save to
        lpMemSaveWSID = wszWsidDPFE;
    } // End IF/ELSE

    // We no longer need this ptr
    MyGlobalUnlock (ClrPtrTypeDirGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemOldWSID = NULL;

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
    WritePrivateProfileStringW (L"General",     // Section name
                                L"Version",     // Key name
                                L"0.01",        // Key value
                                lpMemSaveWSID); // File name
    // Trundle through the Symbol Table
    for (lpSymEntry = lpMemPTD->lpSymTab, uSymVar = uSymFcn = uSymOpr = 0;
         lpSymEntry < lpMemPTD->lpSymTabNext;
         lpSymEntry++)
    if (lpSymEntry->stFlags.Inuse
     && lpSymEntry->stFlags.Value)
    {
        STFLAGS stFlags;        // STE flags

        // Save the flags
        stFlags = lpSymEntry->stFlags;

        // Get the symbol name's global memory handle
        hGlbName = lpSymEntry->stHshEntry->htGlbName;

        // If it has no name (such as steZero or steBlank), skip it
        if (!hGlbName)
            continue;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (hGlbName);

        // Format the text as an ASCII string with non-ASCII chars
        //   represented as either {symbol} or \xXXXX where XXXX is
        //   a four-digit hex number.
        uNameLen = ConvertWideToName (lpwszTemp, lpMemName);

        // Point to the end of the name
        lpaplChar = &lpwszTemp[uNameLen];

        // Terminate the name
        *lpaplChar++ = L'\0';

        // Split cases based upon the object type
        switch (stFlags.ObjType)
        {
            case NAMETYPE_VAR:
            {
                // stData is a variable
                Assert (IsNameTypeVar (stFlags.ObjType));

                // Format the counter
                wsprintfW (wszCount, L"%d", uSymVar++);

                // Format the section name
                wsprintfW (wszSectName, L"Vars.%d", SILevel);

                // Write out the entry (nnn = Name)
                //   in the [Vars.nnn] section
                WritePrivateProfileStringW (wszSectName,
                                            wszCount,
                                            lpwszTemp,
                                            lpMemSaveWSID);
                // If it's immediate, ...
                if (stFlags.Imm)
                {
                    APLUINT uQuadPP;

                    // Ensure we format with full precision in case it's floating point
                    uQuadPP = lpMemPTD->lpSymQuadPP->stData.stInteger;
                    if (stFlags.ImmType EQ IMMTYPE_FLOAT)
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

                    // Write out the entry (Name = Value)
                    //   in the [Fns.nnn], [Ops.nnn], or [Vars.nnn] section
                    WritePrivateProfileStringW (wszSectName,
                                                lpwszTemp,
                                               &lpwszTemp[uNameLen + 1],
                                                lpMemSaveWSID);
               } else
               {
                    // Get the global memory handle
                    hGlbObj = lpSymEntry->stData.stGlbData;

                    // Convert the gloabl memory to transfer form
                    lpaplChar =
                    TransferFormGlb (lpaplChar,
                                     lpSymEntry->stData.stGlbData,
                                     lpMemSaveWSID);
                    // Write out the entry (Name = Value)
                    //   in the [Vars.nnn] section
                    WritePrivateProfileStringW (wszSectName,
                                                lpwszTemp,
                                               &lpwszTemp[uNameLen + 1],
                                                lpMemSaveWSID);
               } // End IF/ELSE

               // We no longer need this ptr
               MyGlobalUnlock (hGlbName); lpMemName = NULL;

                break;
            } // End NAMETYPE_VAR

            case NAMETYPE_FN0:
            case NAMETYPE_FN12:
            case NAMETYPE_OP1:
            case NAMETYPE_OP2:
                bNameTypeFn = IsNameTypeFn (stFlags.ObjType);
////////////////bNameTypeOp = IsNameTypeOp (stFlags.ObjType);

                // Format the counter
                if (bNameTypeFn)
                    wsprintfW (wszCount, L"%d", uSymFcn++);
                else
                    wsprintfW (wszCount, L"%d", uSymOpr++);

                // Format the section name
                if (bNameTypeFn)
                    wsprintfW (wszSectName, L"Fns.%d", SILevel);
                else
                    wsprintfW (wszSectName, L"Ops.%d", SILevel);

                // Write out the entry (nnn = Name)
                //   in the [Fns.nnn] or [Ops.nnn] section
                WritePrivateProfileStringW (wszSectName,
                                            wszCount,
                                            lpwszTemp,
                                            lpMemSaveWSID);
                // Check for immediate
                if (stFlags.Imm)
                {
                    // Make it a one-char string
                    wch[0] = lpSymEntry->stData.stChar;

                    // Format the text as an ASCII string with non-ASCII chars
                    //   represented as either {symbol} or \xXXXX where XXXX is
                    //   a four-digit hex number.
                    lpaplChar += ConvertWideToName (lpaplChar, wch);
                } else
                {
                    HGLOBAL       hGlbTxtLine;
                    LPMEMTXTUNION lpMemTxtLine;

                    // Get the global memory handle
                    hGlbObj = lpSymEntry->stData.stGlbData;

                    Assert (IsGlbTypeFcnDir (hGlbObj)
                         || IsGlbTypeDfnDir (hGlbObj));

                    // Clear the ptr type bits
                    hGlbObj = ClrPtrTypeDirGlb (hGlbObj);

                    // Lock the memory to get a ptr to it
                    lpMemObj = MyGlobalLock (hGlbObj);

                    // Split cases based upon the object's signature
                    switch (((LPHEADER_SIGNATURE) lpMemObj)->nature)
                    {
                        case FCNARRAY_HEADER_SIGNATURE:
                            // Get the text line global memory handle
                            hGlbTxtLine = ((LPFCNARRAY_HEADER) lpMemObj)->hGlbTxtLine;

                            // Lock the memory to get a ptr to it
                            lpMemTxtLine.V = MyGlobalLock (hGlbTxtLine);

                            // Format the text as an ASCII string with non-ASCII chars
                            //   represented as either {symbol} or \xXXXX where XXXX is
                            //   a four-digit hex number.
                            lpaplChar += ConvertWideToName (lpaplChar, (LPAPLCHAR) &lpMemTxtLine.U[1]);

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine.V = NULL;

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

                            // Format the section name
                            lpw = lpwszFormat;
                            lpw +=
                            wsprintfW (lpwszFormat,
                                       L"%d.%s",
                                       uSymFcn - 1,
                                       lpwszTemp);

#define lpMemDfnHdr     ((LPDFN_HEADER) lpMemObj)
                            numFcnLines = lpMemDfnHdr->numFcnLines;

                            // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                            lpFcnLines = (LPFCNLINE) &((LPBYTE) lpMemDfnHdr)[lpMemDfnHdr->offFcnLines];

                            // Write out the function header
                            WriteFunctionLine (lpwszFormat,     // Ptr to the section name
                                               lpw + 1,         // Ptr to save area for the formatted line
                                                                // (" + 1" to skip over the terminating zero
                                                                //  of the section name)
                                               0,
                                               lpMemDfnHdr->hGlbTxtHdr,
                                               lpMemSaveWSID);
                            // Loop through the function lines
                            for (uLine = 0; uLine < numFcnLines; uLine++, lpFcnLines++)
                                WriteFunctionLine (lpwszFormat,     // Ptr to the section name
                                                   lpw + 1,         // Ptr to save area for the formatted line
                                                                    // (" + 1" to skip over the terminating zero
                                                                    //  of the section name)
                                                   uLine + 1,
                                                   lpFcnLines->hGlbTxtLine,
                                                   lpMemSaveWSID);


#undef  lpMemDfnHdr
                            break;
                        } // End DFN_HEADER_SIGNATURE

                        defstop
                            break;
                    } // End SWITCH

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
                } // End IF/ELSE

                // Ensure properly terminated
                *lpaplChar = L'\0';

                // Write out the entry (Name = Value)
                //   in the [Fns.nnn] or [Ops.nnn] section
                WritePrivateProfileStringW (wszSectName,
                                            lpwszTemp,
                                           &lpwszTemp[uNameLen + 1],
                                            lpMemSaveWSID);
                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR/IF

    // Format the Var count
    wsprintfW (wszCount,
               L"%d",
               uSymVar);
    // Write out the Var count to the [General] section
    WritePrivateProfileStringW (L"General",     // Section name
                                L"VarCount",    // Key name
                                wszCount,       // Key value
                                lpMemSaveWSID); // File name
    // Format the Fcn count
    wsprintfW (wszCount,
               L"%d",
               uSymFcn);
    // Write out the Fcn count to the [General] section
    WritePrivateProfileStringW (L"General",     // Section name
                                L"FcnCount",    // Key name
                                wszCount,       // Key value
                                lpMemSaveWSID); // File name
    // Format the Opr count
    wsprintfW (wszCount,
               L"%d",
               uSymOpr);
    // Write out the Opr count to the [General] section
    WritePrivateProfileStringW (L"General",     // Section name
                                L"OprCount",    // Key name
                                wszCount,       // Key value
                                lpMemSaveWSID); // File name
    // Format the SI depth
    wsprintfW (wszCount,
               L"%d",
               SILevel);
    // Write out the SI depth to the [General] section
    WritePrivateProfileStringW (L"General",     // Section name
                                L"SIDepth",     // Key name
                                wszCount,       // Key value
                                lpMemSaveWSID); // File name
    // Calculate space needed for the WSID
    iCmp = lstrlenW (lpMemSaveWSID);
    ByteWSID = CalcArraySize (ARRAY_CHAR, iCmp, 1);

    // Allocate space for the WSID
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
////lpHeader->Perm       = 0;   // Already zero from GHND
////lpHeader->SysVar     = 0;   // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = iCmp;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemNewWSID) = iCmp;

    // Skip over the header and dimensions to the data
    lpMemNewWSID = VarArrayBaseToData (lpMemNewWSID, 1);

    // Copy data to the []WSID
    CopyMemory (lpMemNewWSID, lpMemSaveWSID, iCmp * sizeof (lpMemSaveWSID[0]));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbWSID); lpMemNewWSID = NULL;

    // Free the old []WSID
    FreeResultGlobalVar (ClrPtrTypeDirGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData));

    // Save the new []WSID
    lpMemPTD->lpSymQuadWSID->stData.stGlbData = MakeGlbTypeGlb (hGlbWSID);

    // Mark as successful
    bRet = TRUE;
ERROR_EXIT:
    if (lpMemOldWSID)
    {
        // We no longer need this ptr
        MyGlobalUnlock (ClrPtrTypeDirGlb (lpMemPTD->lpSymQuadWSID->stData.stGlbData)); lpMemOldWSID = NULL;
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End CmdSaveWS_EM
#undef  APPEND_NAME


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
    LPMEMTXTUNION lpMemTxtLine;
    WCHAR         wszCount[10];

    // Format the line number
    wsprintfW (wszCount,
               L"%d",
               uLineNum);
    // Lock the memory to get a ptr to it
    lpMemTxtLine.V = MyGlobalLock (hGlbTxtLine);

    // Format the text as an ASCII string with non-ASCII chars
    //   represented as either {symbol} or \xXXXX where XXXX is
    //   a four-digit hex number.
    ConvertWideToName (lpwFormat, (LPWCHAR) &lpMemTxtLine.U[1]);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine.V = NULL;

    // Write out the entry (nnn = FunctionLine)
    WritePrivateProfileStringW (lpwszSectName,
                                wszCount,
                                lpwFormat,
                                lpMemSaveWSID);
} // End WriteFunctionLine


//***************************************************************************
//  $TransferFormGlb
//
//  Convert global memory to transfer form
//***************************************************************************

LPAPLCHAR TransferFormGlb
    (LPAPLCHAR lpaplChar,               // Ptr to output save area
     HGLOBAL   hGlbObj,                 // WS object global memory handle
     LPAPLCHAR lpMemSaveWSID)           // Ptr to saved WS file DPFE

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
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // stData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (hGlbObj));

    // Clear the ptr type bits
    hGlbObj = ClrPtrTypeDirGlb (hGlbObj);

    // Save as ptr to the profile keyname
    lpMemProKey = lpaplChar;

    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLock (hGlbObj);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemObj)

    aplTypeObj = lpHeader->ArrType;
    aplNELMObj = lpHeader->NELM;
    aplRankObj = lpHeader->Rank;

    // Append the HGLOBAL name
    lpaplChar +=
    wsprintfW (lpaplChar,
               L"%c%08X ",
               L"GP"[lpHeader->Perm],
               hGlbObj);
#undef  lpHeader

    // Terminate the keyname
    *lpaplChar++ = L'\0';

    // Save as ptr to the profile value
    lpMemProVal = lpaplChar;

    // Append the shape if non-scalar
    if (aplRankObj > 0)
    {
        for (uObj = 0; uObj < aplRankObj; uObj++)
            // Format the dimension
            lpaplChar =
            FormatAplint (lpaplChar,
                         (VarArrayBaseToDim (lpMemObj))[uObj]);
        lpaplChar--;        // Back up to the trailing blank

        // Append a {rho}
#define APPEND_TEXT     L"{rho}"
        lstrcpyW (lpaplChar, APPEND_TEXT);
        lpaplChar += (sizeof (APPEND_TEXT) / sizeof (WCHAR)) - 1;
#undef  APPEND_TEXT
    } // End FOR

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

                // Format the value
                lpaplChar += ConvertWideToName (lpaplChar, wch);
            } // End FOR

            // Append a trailing single quote
            *lpaplChar++ = L'\'';

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemObj)
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
            FormatAplint (lpaplChar,
                          apaOff);
            lpaplChar--;        // Back up to the trailing blank

            // Append a comma
            *lpaplChar++ = L',';

            // Append the multiplier
            lpaplChar =
            FormatAplint (lpaplChar,
                          apaMul);
            lpaplChar--;        // Back up to the trailing blank

            // Append a comma
            *lpaplChar++ = L',';

            // Append the length
            lpaplChar =
            FormatAplint (lpaplChar,
                          aplNELMObj);
            lpaplChar--;        // Back up to the trailing blank

            // Append the trailing marker
            *lpaplChar++ = L')';

            break;

        case ARRAY_HETERO:
            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPSYMENTRY *) lpMemObj)++)
            // Split cases based upon the element immediate type
            switch ((*(LPSYMENTRY *) lpMemObj)->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    lpaplChar =
                    FormatAplint (lpaplChar,
                                  (*(LPSYMENTRY *) lpMemObj)->stData.stBoolean);
                    break;

                case IMMTYPE_INT:
                    lpaplChar =
                    FormatAplint (lpaplChar,
                                  (*(LPSYMENTRY *) lpMemObj)->stData.stInteger);
                    break;

                case IMMTYPE_CHAR:
                    // Append a leading single quote
                    *lpaplChar++ = L'\'';

                    // Save the char in a string
                    wch[0] = (*(LPSYMENTRY *) lpMemObj)->stData.stChar;

                    // Format the value
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
                                 (*(LPSYMENTRY *) lpMemObj)->stData.stFloat,
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

            // Take into account empty arrays
            aplNELMObj = max (aplNELMObj, 1);

            // Loop through the array elements
            for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLNESTED) lpMemObj)++)
            // Split case based upon the ptr type
            switch (GetPtrTypeInd (lpMemObj))
            {
                case PTRTYPE_STCONST:
#define lpSymEntry      (*(LPSYMENTRY *) lpMemObj)

                    // Ensure we format with full precision in case it's floating point
                    uQuadPP = lpMemPTD->lpSymQuadPP->stData.stInteger;
                    if (lpSymEntry->stFlags.ImmType EQ IMMTYPE_FLOAT)
                        lpMemPTD->lpSymQuadPP->stData.stInteger = DEF_MAX_QUADPP;

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

                    // Get the global's Type
                    AttrsOfGlb (ClrPtrTypeDirGlb (hGlbSub), &aplTypeSub, NULL, NULL, NULL);

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

                                    lpMemSaveWSID);
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

    // Write out the entry in the [Globals section
    WritePrivateProfileStringW (L"Globals",
                                lpMemProKey,
                                lpMemProVal,
                                lpMemSaveWSID);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbObj); lpMemObj = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Return a ptr to the profile keyname's terminating zero
    return &lpMemProKey[lstrlenW (lpMemProKey)];
} // End TransferFormGlb


//***************************************************************************
//  $CmdDropWS_EM
//
//  Execute the system command:  )DROP
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdDropWS_EM"
#else
#define APPEND_NAME
#endif

BOOL CmdDropWS_EM
    (LPWCHAR lpwszTail)     // Command line tail with NO leading blanks

{
    WCHAR wszTailDPFE[_MAX_PATH];   // Save area for canonical form of given ws name
    BOOL  bRet = FALSE;             // TRUE iff result is valid
    WCHAR wszTemp[19 + 1];          // Output save area

    // If there is a specified WS Name, ...
    if (*lpwszTail)
    {
        // Convert the given workspace name into a canonical form
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, wszSaveDir);

        // Append the common workspace extension
        lstrcatW (wszTailDPFE, WS_WKSEXT);

        // Attempt to open the workspace
        if (!_wremove (wszTailDPFE))
        {
            SYSTEMTIME SystemTime;

            // Display the current timne & date of the drop

            // Get the current time
            if (bUseLocalTime)
                GetLocalTime  (&SystemTime);
            else
                GetSystemTime (&SystemTime);

            wsprintfW (wszTemp,
                       L"%02d/%02d/%04d %2d:%02d:%02d",
                       SystemTime.wMonth,
                       SystemTime.wDay,
                       SystemTime.wYear,
                       SystemTime.wHour,
                       SystemTime.wMinute,
                       SystemTime.wSecond);

            AppendLine (wszTemp, FALSE, TRUE);

            // Mark as successful
            bRet = TRUE;
        } else
            AppendLine (ERRMSG_WS_NOT_FOUND, FALSE, TRUE);
    } else
        IncorrectCommand ();

    return bRet;
} // End CmdSaveWS_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: savews.c
//***************************************************************************
