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
#include "workspace.h"
#include "savefcn.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $CmdLoad_EM
//
//  Execute the system command:  )LOAD wsid
//***************************************************************************

BOOL CmdLoad_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    HGLOBAL      hGlbPTD;               // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    WCHAR        wszTailDPFE[_MAX_PATH];// Save area for canonical form of given ws name
    int          iTabIndex;             // Tab index

    // If there's no WSID, that's an error
    if (lpwszTail[0] EQ L'\0')
    {
        IncorrectCommand ();

        return FALSE;
    } // End IF

    // Convert the given workspace name into a canonical form (without WKSEXT)
    MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, wszSaveDir);

    // Append the common workspace extension
    lstrcatW (wszTailDPFE, WS_WKSEXT);

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the tab index from which this command was issued
    iTabIndex = lpMemPTD->CurTabIndex;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return
      CreateNewTab (hWndMF,             // Parent window handle
                    wszTailDPFE,        // Drive, Path, Filename, Ext of the workspace
                    iTabIndex + 1);     // Insert new tab to the left of this one
} // End CmdLoad_EM


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

BOOL LoadWorkspace_EM
    (HGLOBAL hGlbDPFE,                  // Workspace DPFE global memory handle
     HWND    hWndEC)                    // Edit control window handle

{
    LPWCHAR      lpwszDPFE;             // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
    FILE        *fStream;               // Ptr to file stream for the plain text workspace file
    UINT         uSymVar,               // Var index counter
                 uSymFcn,               // Fcn/Opr index counter
                 uGlbCnt,               // [GlobalVars] count
                 uLineCnt,              // # lines in the current function
                 uSILevel,              // [General] SILevel value
                 uSID,                  // Loop counter
                 uStr;                  // Loop counter
    LPWCHAR      lpwSrc,                // Ptr to incoming data
                 lpwDst,                // Ptr to destination data in name
                 lpwSrcStart,           // Ptr to starting point
                 lpwSectName;           // Ptr to section name
    WCHAR        wszCount[8],           // Save area for formatted uSymVar/Fcn counter
                 wszSectName[15],       // ...                     section name (e.g., [Vars.nnn])
                 wcTmp;                 // Temporary char
    BOOL         bRet = FALSE,          // TRUE iff the result is valid
                 bImmed,                // TRUE iff the result of ParseSavedWsVar is immediate
                 bUserDefined;          // TRUE iff the durrent function is User-Defined
    APLSTYPE     aplTypeObj;            // Object storage type
    APLNELM      aplNELMObj;            // Object NELM
    APLRANK      aplRankObj;            // Object rank
    APLINT       aplInteger;            // Temporary integer
    APLUINT      ByteObj,               // # bytes needed for the object
                 uObj;                  // Loop counter
    HGLOBAL      hGlbPTD,               // PerTabData global memory handle
                 hGlbObj;               // Object global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPVOID       lpMemObj;              // Ptr to object global memory
    STFLAGS      stFlags = {0};         // SymTab flags
    LPSYMENTRY   lpSymEntry;            // Ptr to STE for HGLOBAL
    UINT         uBitIndex;             // Bit index for looping through Boolean result
    LPWCHAR      lpwCharEnd;            // Temporary ptr
    APLLONGEST   aplLongestObj;         // Object immediate value
    LPAPLLONGEST lpaplLongestObj;       // Ptr to ...
    FILETIME     ftCreation,            // Function creation time
                 ftLastMod;             // ...      last modification time

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

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

    //***************************************************************
    // Load the variables
    //***************************************************************

    // Get [GlobalVars] count
    uGlbCnt =
      GetPrivateProfileIntW (SECTNAME_GLOBALVARS,       // Ptr to the section name
                             KEYNAME_COUNT,             // Ptr to the key name
                             0,                         // Default value if not found
                             lpwszDPFE);                // Ptr to the file name
    // Loop through the [GlobalVars] section
    for (uStr = 0; uStr < uGlbCnt; uStr++)
    {
        // Save ptr
        lpwSrc = lpwszTemp;

        // Format the counter
        wsprintfW (wszCount, FMTSTR_GLBCNT, uStr);

        // Read the next string
        GetPrivateProfileStringW (SECTNAME_GLOBALVARS,  // Ptr to the section name
                                  wszCount,             // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  lpwSrc,               // Ptr to the output buffer
                                  memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                  lpwszDPFE);           // Ptr to the file name
        // Parse the array attributes

        // Ensure it's non-empty
        if  (lpwSrc[0] EQ L'\0')
            goto CORRUPTWS_EXIT;

        // ***FIXME*** -- Do we need to restore the object PermNdx?

        // Get the object storage type
        aplTypeObj = TranslateCharToArrayType (*lpwSrc++);

        // Ensure there's a valid separator
        if (*lpwSrc NE L' ')
            goto CORRUPTWS_EXIT;

        // Skip over it
        lpwSrc++;

        // Get the object NELM
        swscanf (lpwSrc, L"%I64d", &aplNELMObj); lpwSrc = SkipBlackW (lpwSrc);

        // Ensure there's a valid separator
        if (*lpwSrc NE L' ')
            goto CORRUPTWS_EXIT;

        // Skip over it
        lpwSrc++;

        // Get the object rank
        swscanf (lpwSrc, L"%I64d", &aplRankObj); lpwSrc = SkipBlackW (lpwSrc);

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
        Assert (ByteObj EQ (UINT) ByteObj);
        hGlbObj = MyGlobalAlloc (GHND, (UINT) ByteObj);
        if (!hGlbObj)
            goto WSFULL_EXIT;

        // Set the flags for what we're appending
        stFlags.Inuse   = TRUE;
        stFlags.ObjName = OBJNAME_LOD;

        // Create a symbol table entry for the )LOAD HGLOBAL
        lpSymEntry =
          SymTabAppendName_EM (wszCount, &stFlags);

        // Set the handle
        lpSymEntry->stData.stGlbData = MakePtrTypeGlb (hGlbObj);

        // Lock the memory to get a ptr to it
        lpMemObj = MyGlobalLock (hGlbObj);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemObj)
        // Fill in the header
////////// Note that the RefCnt is initialized to zero
////////// It will be incremented upon each reference
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeObj;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND ***FIXME*** -- Set correct PERMNDX_xxx
////////lpHeader->SysVar     = 0;               // Already zero from GHND
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
            swscanf (lpwSrc, L"%I64d", lpMemObj);

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
                    swscanf (lpwSrc, L"%I64d", &aplInteger);

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
                    swscanf (lpwSrc, L"%I64d", ((LPAPLINT) lpMemObj)++);

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
                        wcTmp = NameToChar (lpwSrc);

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
                swscanf (lpwSrc, L"%I64d", ((LPAPLINT) lpMemObj)++);

                // Skip to the next field
                lpwSrc = SkipPastCharW (lpwSrc, L' ');

                // Scan in the next value and skip over it
                swscanf (lpwSrc, L"%I64d", ((LPAPLINT) lpMemObj)++);

                // Skip to the next field
                lpwSrc = SkipPastCharW (lpwSrc, L' ');

                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
                // The elements consist of either a simple scalar
                //   (starts with one of the storage types), or a
                //   global (starts with a #)

                // Loop through the elements
                for (uObj = 0; uObj < aplNELMObj; uObj++)
                    lpwSrc =
                      ParseSavedWsVar (lpwSrc, &lpMemObj, NULL, NULL, TRUE);
                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this ptr
        MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
    } // End FOR

    // Get the SI Level
    uSILevel =
      GetPrivateProfileIntW (SECTNAME_GENERAL,          // Ptr to the section name
                             KEYNAME_SILEVEL,           // Ptr to the key name
                             0,                         // Default value if not found
                             lpwszDPFE);                // Ptr to the file name
    // Loop through the SI levels
    for (uSID = 0; uSID <= uSILevel; uSID++)
    {
        // Format the section name
        wsprintfW (wszSectName, SECTNAME_VARS L".%d", uSID);

        // Get the [Vars.nnn] count
        uSymVar =
          GetPrivateProfileIntW (wszSectName,               // Ptr to the section name
                                 KEYNAME_COUNT,             // Ptr to the key name
                                 0,                         // Default value if not found
                                 lpwszDPFE);                // Ptr to the file name
        // Loop through the [Vars.nnn] section where nnn is the SI level
        for (uStr = 0; uStr < uSymVar; uStr++)
        {
            // Save ptr
            lpwSrc = lpwszTemp;

            // Format the counter
            wsprintfW (wszCount, L"%d", uStr);

            // Read the next string
            GetPrivateProfileStringW (wszSectName,          // Ptr to the section name
                                      wszCount,             // Ptr to the key name
                                      L"",                  // Ptr to the default value
                                      lpwSrc,               // Ptr to the output buffer
                                      memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                      lpwszDPFE);           // Ptr to the file name
            // Look for the name separator (L'=')
            lpwCharEnd = strchrW (lpwSrc, L'=');
            Assert (lpwCharEnd NE NULL);

            // Zap to form zero-terminated name
            *lpwCharEnd = L'\0';

            // Save the starting point
            lpwSrcStart = lpwSrc;
            lpwDst = lpwSrc;

            // Convert the {name}s and other chars to UTF16_xxx
            lpwSrc = ConvertNameInPlace (lpwSrc);

            // Skip over the zapped L'='
            lpwSrc++;

            // Set the flags for what we're looking up/appending
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

                // Mark the SYMENTRY as immediate so we don't free the
                //   (non-existant) stGlbData
                // Set other flags as appropriate
                lpSymEntry->stFlags.Imm        =
                lpSymEntry->stFlags.Value      = TRUE;
                lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
                lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
            } // End IF

            // Clear so we save a clean value
            aplLongestObj = 0;

            // Set this value as lpaplLongestObj is incremented by ParseSavedWsVar
            lpaplLongestObj = &aplLongestObj;

            // Parse the value into aplLongestObj and aplTypeObj
            lpwSrc =
              ParseSavedWsVar (lpwSrc, &lpaplLongestObj, &aplTypeObj, &bImmed, FALSE);

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
                if (!lpSymEntry->stFlags.Imm)
                {
                    FreeResultGlobalVar (lpSymEntry->stData.stGlbData); lpSymEntry->stData.stGlbData = NULL;
                } // End IF

                // In with the new
                if (bImmed)
                {
                    // Set the stFlags & stData
                    lpSymEntry->stFlags.Imm      = TRUE;
                    lpSymEntry->stFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeObj);
                    lpSymEntry->stData.stLongest = aplLongestObj;
                } else
                {
                    // Set the stFlags & stData
                    lpSymEntry->stFlags.Imm      = FALSE;
                    lpSymEntry->stFlags.ImmType  = 0;
                    lpSymEntry->stData.stLongest = aplLongestObj;
                } // End IF/ELSE
            } // End IF

            // Restore the original value
            *lpwCharEnd = L'=';
        } // End FOR
    } // End FOR

    // Delete the symbol table entries for vars we allocated of the form "#%d"
    DeleteLoadSTEs (uGlbCnt);

    //***************************************************************
    // Load the functions
    //***************************************************************

    // Get [GlobalFcns] count
    uGlbCnt =
      GetPrivateProfileIntW (SECTNAME_GLOBALFCNS,       // Ptr to the section name
                             KEYNAME_COUNT,             // Ptr to the key name
                             0,                         // Default value if not found
                             lpwszDPFE);                // Ptr to the file name
    // Loop through the [GlobalFcns] section
    for (uStr = 0; uStr < uGlbCnt; uStr++)
    {
        LPWCHAR lpwFcnName;             // Ptr to function name

        // Save ptr
        lpwSrc = lpwszTemp;

        // Format the counter
        wsprintfW (wszCount, FMTSTR_GLBCNT, uStr);

        // Prepare to read in the section name
        lpwSectName = lpwSrc;

        // Read the next string
        // The output is a section name of the form nnn.Name where nnn is the function count
        GetPrivateProfileStringW (SECTNAME_GLOBALFCNS,  // Ptr to the section name
                                  wszCount,             // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  lpwSectName,          // Ptr to the output buffer
                                  memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                  lpwszDPFE);           // Ptr to the file name
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
        // Get the CreationTime string
        GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                                  KEYNAME_CREATIONTIME, // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  lpwSrc,               // Ptr to the output buffer
                                  memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                  lpwszDPFE);           // Ptr to the file name
        // Convert the CreationTime string to time
        swscanf (lpwSrc, L"%16I64X", &ftCreation);

        // Get the LastModTime string
        GetPrivateProfileStringW (lpwSectName,          // Ptr to the section name
                                  KEYNAME_LASTMODTIME,  // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  lpwSrc,               // Ptr to the output buffer
                                  memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                  lpwszDPFE);           // Ptr to the file name
        // Convert the LastModTime string to time
        swscanf (lpwSrc, L"%16I64X", &ftLastMod);

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
                                        memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                        lpwszDPFE);         // Ptr to the file name
            // Fill in common values
////////////SF_Fcns.bRet            =                   // Filled in by SaveFunctionCom
////////////SF_Fcns.uErrLine        =                   // ...
////////////SF_Fcns.lpSymName       =                   // ...
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
        } else
        // It's a function array
        {
#ifdef DEBUG
            EXIT_TYPES exitType;
#endif
            // Append the variable name and an assignment arrow to the output buffer
            lstrcpyW (lpwSrc, lpwFcnName);
            lpwFcnName = lpwSrc;
            lpwSrc += lstrlenW (lpwSrc);
            *lpwSrc++ = UTF16_LEFTARROW;

            // Get the one (and only) line
            GetPrivateProfileStringW (lpwSectName,        // Ptr to the section name
                                      L"0",               // Ptr to the key name
                                      L"",                // Ptr to the default value
                                      lpwSrc,             // Ptr to the output buffer
                                      memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                      lpwszDPFE);         // Ptr to the file name
            // Convert in place
            lpwSrcStart = lpwDst = lpwSrc;

            // Convert the {name}s and other chars to UTF16_xxx
            (void) ConvertNameInPlace (lpwSrc);

            // Execute the statement
#ifdef DEBUG
            exitType =
#endif
              ImmExecStmt (lpwFcnName,      // Ptr to line to execute
                           FALSE,           // Free lpwszCompLine on completion
                           TRUE,            // TRUE iff wait until finished
                           hWndEC);         // Edit Control window handle
            Assert (exitType EQ EXITTYPE_NOVALUE);
        } // End IF/ELSE

        // Lookup the STE and save its HGLOBAL in the STE for #nnn

        // Set the flags for what we're looking up/appending
        stFlags.Inuse   = TRUE;

        if (IsSysName (lpwFcnName))
            // Tell 'em we're looking for system names
            stFlags.ObjName = OBJNAME_SYS;
        else
            // Tell 'em we're looking for user names
            stFlags.ObjName = OBJNAME_USR;

        // Lookup the name in the symbol table
        lpSymEntry =
          SymTabLookupName (lpwFcnName, &stFlags);
        Assert (lpSymEntry NE NULL);

        // Copy the HGLOBAL
        hGlbObj = CopySymGlbDir (lpSymEntry->stData.stGlbData);

        // Set the flags for what we're appending
        stFlags.Inuse   = TRUE;
        stFlags.ObjName = OBJNAME_LOD;

        // Create a symbol table entry for the )LOAD HGLOBAL
        lpSymEntry =
          SymTabAppendName_EM (wszCount, &stFlags);

        // Set the handle
        lpSymEntry->stData.stGlbData = MakePtrTypeGlb (hGlbObj);
    } // End FOR

    // Get the SI Level
    uSILevel =
      GetPrivateProfileIntW (SECTNAME_GENERAL,          // Ptr to the section name
                             KEYNAME_SILEVEL,           // Ptr to the key name
                             0,                         // Default value if not found
                             lpwszDPFE);                // Ptr to the file name
    // Loop through the SI levels
    for (uSID = 0; uSID <= uSILevel; uSID++)
    {
        // Format the section name
        wsprintfW (wszSectName, SECTNAME_FCNS L".%d", uSID);

        // Get the [Fcns.nnn] count
        uSymFcn =
          GetPrivateProfileIntW (wszSectName,               // Ptr to the section name
                                 KEYNAME_COUNT,             // Ptr to the key name
                                 0,                         // Default value if not found
                                 lpwszDPFE);                // Ptr to the file name
        // Loop through the [Fcns.nnn] section where nnn is the SI level
        for (uStr = 0; uStr < uSymFcn; uStr++)
        {
            NAME_TYPES nameType;

            // Save ptr
            lpwSrc = lpwszTemp;

            // Format the counter
            wsprintfW (wszCount, L"%d", uStr);

            // Read the next string
            GetPrivateProfileStringW (wszSectName,          // Ptr to the section name
                                      wszCount,             // Ptr to the key name
                                      L"",                  // Ptr to the default value
                                      lpwSrc,               // Ptr to the output buffer
                                      memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                      lpwszDPFE);           // Ptr to the file name
            // Look for the name separator (L'=')
            lpwCharEnd = strchrW (lpwSrc, L'=');
            Assert (lpwCharEnd NE NULL);

            // Zap to form zero-terminated name
            *lpwCharEnd = L'\0';

            // Save the starting point
            lpwSrcStart = lpwSrc;
            lpwDst = lpwSrc;

            // Convert the {name}s and other chars to UTF16_xxx
            lpwSrc = ConvertNameInPlace (lpwSrc);

            // Skip over the zapped L'='
            lpwSrc++;

            // Parse the name type (2 = FN0, 3 = FN12, 4 = OP1, 5 = OP2, 6 = OP3)
            nameType = *lpwSrc++ - '0';

            Assert (*lpwSrc EQ L'='); lpwSrc++;

            // Set the flags for what we're looking up/appending
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

                // Set stFlags as appropriate
                lpSymEntry->stFlags.Value      = TRUE;
                lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
////////////////lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
            } // End IF

            // Parse the line into lpSymEntry->stData
            ParseSavedWsFcn (lpwSrc, lpSymEntry, nameType);
        } // End FOR
    } // End FOR

    // Delete the symbol table entries for functions we allocated of the form "#%d"
    DeleteLoadSTEs (uGlbCnt);
WSID_EXIT:
    // Set the value of the new []WSID as lpwszDPFE
    bRet = SaveNewWsid (lpwszDPFE);

    goto NORMAL_EXIT;

CORRUPTWS_EXIT:
    AppendLine (ERRMSG_CORRUPT_WS APPEND_NAME, FALSE, TRUE);

    goto ERROR_EXIT;

WSNOTFOUND_EXIT:
{
    int iPrvTabIndex;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the index of the tab from which we were )LOADed
    iPrvTabIndex = lpMemPTD->PrvTabIndex;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    if (iPrvTabIndex NE -1)
    {
        // Get the PerTabData global memory handle for the preceding tab
        hGlbPTD = GetPerTabHandle (iPrvTabIndex);

        // Lock the memory to get a pre to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Send this error message to the previous tab's SM
        SendMessageW (lpMemPTD->hWndSM, MYWM_ERRMSG, 0, (LPARAM) ERRMSG_WS_NOT_FOUND APPEND_NAME);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF

    goto ERROR_EXIT;
}

WSFULL_EXIT:
    AppendLine (ERRMSG_WS_FULL APPEND_NAME, FALSE, TRUE);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbDPFE); lpwszDPFE = NULL;

    return bRet;
} // End LoadWorkspace_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ParseSavedWsFcn
//
//  Parse a value of the form #nnn or {name}...
//***************************************************************************

void ParseSavedWsFcn
    (LPWCHAR    lpwSrc,                 // Ptr to input buffer
     LPSYMENTRY lpSymObj,               // Ptr to STE for the object
     NAME_TYPES nameType)               // Function name type (see NAME_TYPES)

{
    WCHAR      wcTmp;                   // Temporary char
    LPWCHAR    lpwCharEnd;              // Temporary ptr
    STFLAGS    stFlags = {0};           // SymTab flags
    LPSYMENTRY lpSymEntry;              // Ptr to STE for HGLOBAL
    HGLOBAL    hGlbObj;                 // Object global memory handle

    // Tell 'em we're looking for )LOAD objects
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;

    // Mark as a function/operator
    lpSymObj->stFlags.stNameType = nameType;

    // Copy the old value
    hGlbObj = lpSymObj->stData.stGlbData;

    if (*lpwSrc EQ L'#')
    {
        // Find the trailing L' '
        lpwCharEnd = SkipToCharW (lpwSrc, L' ');

        // Save old next char, zap to form zero-terminated name
        wcTmp = *lpwCharEnd; *lpwCharEnd = L'\0';

        // Get the matching HGLOBAL
        lpSymEntry =
          SymTabLookupName (lpwSrc, &stFlags);

        Assert (lpSymEntry NE NULL);

        // Restore the original value
        *lpwCharEnd = wcTmp;

        // Save in the result
        lpSymObj->stData.stGlbData = CopySymGlbDir (lpSymEntry->stData.stGlbData);
    } else
    {
        // Convert the single {name} or other char to UTF16_xxx
        if (L'{' EQ  *lpwSrc)
        {
            // Get the next char
            wcTmp = NameToChar (lpwSrc);

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
    } // End IF

    // If there's an old value, ...
    if (hGlbObj)
    {
        // Free the old value
        FreeResultGlobalDFV (hGlbObj); hGlbObj = NULL;
    } // End IF
} // End ParseSavedWsFcn


//***************************************************************************
//  $ParseSavedWsVar
//
//  Parse a value of the form #nnn or X N R S V
//***************************************************************************

LPWCHAR ParseSavedWsVar
    (LPWCHAR    lpwSrc,                 // Ptr to input buffer
     LPVOID    *lplpMemObj,             // Ptr to ptr to output element
     LPAPLSTYPE lpaplTypeObj,           // Ptr to storage type (may be NULL)
     LPBOOL     lpbImmed,               // Ptr to immediate flag (TRUE iff result is immediate) (may be NULL)
     BOOL       bSymTab)                // TRUE iff to save SymTabAppend values, FALSE to save values directly

{
    WCHAR      wcTmp;                   // Temporary char
    LPWCHAR    lpwCharEnd;              // Temporary ptr
    STFLAGS    stFlags = {0};           // SymTab flags
    LPSYMENTRY lpSymEntry;              // Ptr to STE for HGLOBAL
    APLINT     aplInteger;              // Temporary integer
    APLSTYPE   aplTypeObj;              // Object storage type

    // Tell 'em we're looking for )LOAD objects
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;

    if (*lpwSrc EQ L'#')
    {
        // Find the trailing L' '
        lpwCharEnd = SkipToCharW (lpwSrc, L' ');

        // Save old next char, zap to form zero-terminated name
        wcTmp = *lpwCharEnd; *lpwCharEnd = L'\0';

        // Get the matching HGLOBAL
        lpSymEntry =
          SymTabLookupName (lpwSrc, &stFlags);

        Assert (lpSymEntry NE NULL);

        // Restore the original value
        *lpwCharEnd = wcTmp;

        // Save in the result and skip over it
        *((LPAPLNESTED) *lplpMemObj)++ =
          lpSymEntry->stData.stGlbData;

        // Increment the reference count
        DbgIncrRefCntDir (lpSymEntry->stData.stGlbData);

        // Skip to the next field
        lpwSrc = &lpwCharEnd[1];

        // If the caller wants the storage type, ...
        if (lpaplTypeObj)
        {
            HGLOBAL hGlbObj;            // Object global memory handle
            LPVOID  lpMemObj;           // Ptr to object global memory

            // stData is not immediate
            Assert (!lpSymEntry->stFlags.Imm);

            // Get the global memory handle
            hGlbObj = lpSymEntry->stData.stGlbData;

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
                swscanf (lpwSrc, L"%I64d", &aplInteger);

                // Skip to the next field
                lpwSrc = SkipPastCharW (lpwSrc, L' ');

                // If we're to save the SymTab, ...
                if (bSymTab)
                    // Save in the result and skip over it
                    *((LPAPLHETERO) *lplpMemObj)++ =
                      SymTabAppendInteger_EM (aplInteger);
                else
                    // Save the result directly
                    *((LPAPLINT) *lplpMemObj) = aplInteger;
                break;

            case ARRAY_CHAR:        // Character
                Assert (L'\'' EQ *lpwSrc); lpwSrc++;

                // Convert the single {name) or other char to UTF16_xxx
                if (L'{' EQ  *lpwSrc)
                {
                    // Get the next char
                    wcTmp = NameToChar (lpwSrc);

                    // Skip to the next field
                    lpwSrc = SkipPastCharW (lpwSrc, L'}');
                } else
                    wcTmp = *lpwSrc++;

                Assert (L'\'' EQ *lpwSrc); lpwSrc++;

                // If we're to save the SymTab, ...
                if (bSymTab)
                    // Save in the result and skip over it
                    *((LPAPLHETERO) *lplpMemObj)++ =
                      SymTabAppendChar_EM (wcTmp);
                else
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
                    // Save in the result and skip over it
                    *((LPAPLHETERO) *lplpMemObj)++ =
                      SymTabAppendFloat_EM (strtod ((LPCHAR) lpwszFormat, NULL));
                else
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
} // End ParseSavedWsVar


//***************************************************************************
//  $DeleteLoadSTEs
//
//  Delete OBJNAME_LOD STEs
//***************************************************************************

void DeleteLoadSTEs
    (UINT uGlbCnt)

{
    UINT       uStr;                    // Loop counter
    WCHAR      wszCount[8];             // Save area for formatted uSymVar/Fcn counter
    STFLAGS    stFlags = {0};           // SymTab flags
    LPSYMENTRY lpSymEntry;              // Ptr to STE for HGLOBAL

    // Tell 'em we're looking for )LOAD objects
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;

    // Loop through the [GlobalVars] or [GlobalFcns] section deleting #nnn STEs
    for (uStr = 0; uStr < uGlbCnt; uStr++)
    {
        // Format the counter
        wsprintfW (wszCount, FMTSTR_GLBCNT, uStr);

        // Get the matching HGLOBAL
        lpSymEntry =
          SymTabLookupName (wszCount, &stFlags);

        Assert (lpSymEntry NE NULL);

        // If the SYMENTRY is not immediate, free its global
        if (!lpSymEntry->stFlags.Imm)
        {
            FreeResultGlobalDFV (lpSymEntry->stData.stGlbData); lpSymEntry->stData.stGlbData = NULL;
        } // End IF

        // Erase the STE
        EraseSTE (lpSymEntry);
    } // End FOR
} // End DeleteLoadSTEs


//***************************************************************************
//  End of File: sc_load.c
//***************************************************************************
