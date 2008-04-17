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
    iTabIndex = lpMemPTD->TabIndex;

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
    (HGLOBAL hGlbDPFE)                  // Workspace DPFE global memory handle

{
    LPWCHAR    lpwszDPFE;               // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
    FILE      *fStream;                 // Ptr to file stream for the plain text workspace file
    UINT       uSymVar,                 // Var index counter
               uSymFcn,                 // Fcn/Opr index counter
               uGlbCnt,                 // [GlobalVars] count
               uSIDepth,                // [General] SIDepth value
               uSID,                    // Loop counter
               uLen,                    // Temporary length
               uStr;                    // Loop counter
    LPWCHAR    lpwSrc,                  // Ptr to incoming data
               lpwDst,                  // Ptr to destination data in name
               lpwSrcStart;             // Ptr to starting point
    WCHAR      wszCount[8],             // Save area for formatted uSymVar/Fcn counter
               wszSectName[15],         // ...                     section name (e.g., [Vars.nnn])
               wcTmp;                   // Temporary char
    BOOL       bRet = FALSE;            // TRUE iff the result is valid
    APLSTYPE   aplTypeObj;              // Object storage type
    APLNELM    aplNELMObj;              // Object NELM
    APLRANK    aplRankObj;              // Object rank
    APLINT     aplInteger;              // Temporary integer
    APLUINT    ByteObj,                 // # bytes needed for the object
               uObj;                    // Loop counter
    HGLOBAL    hGlbObj;                 // Object global memory handle
    LPVOID     lpMemObj;                // Ptr to object global memory
    STFLAGS    stFlags = {0};           // SymTab flags
    LPSYMENTRY lpSymEntry;              // Ptr to STE for HGLOBAL
    UINT       uBitIndex;               // Bit index for looping through Boolean result
    LPWCHAR    lpwCharEnd;              // Temporary ptr
    char       szFmt[128];              // Temporary save area

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

    // Set the flags for what we're appending or looking up
////stFlags.Perm    =
    stFlags.Inuse   = TRUE;
////stFlags.Value   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;
////stFlags.UsrDfn  = TRUE;
////stFlags.DfnAxis = FALSE;        // Already zero from = {0}

    // Get the Var & Fcn/Opr counts
    uSymVar =
      GetPrivateProfileIntW (SECTNAME_GENERAL,          // Ptr to the section name
                             KEYNAME_VARCOUNT,          // Ptr to the key name
                             0,                         // Default value if not found
                             lpwszDPFE);                // Ptr to the file name
    uSymFcn =
      GetPrivateProfileIntW (SECTNAME_GENERAL,          // Ptr to the section name
                             KEYNAME_FCNCOUNT,          // Ptr to the key name
                             0,                         // Default value if not found
                             lpwszDPFE);                // Ptr to the file name
    uSIDepth =
      GetPrivateProfileIntW (SECTNAME_GENERAL,          // Ptr to the section name
                             KEYNAME_SIDEPTH,           // Ptr to the key name
                             0,                         // Default value if not found
                             lpwszDPFE);                // Ptr to the file name
    // Get [GlobalVars] count
    uGlbCnt =
      GetPrivateProfileIntW (SECTNAME_GLOBALVARS,       // Ptr to the section name
                             KEYNAME_GLBCOUNT,          // Ptr to the key name
                             0,                         // Default value if not found
                             lpwszDPFE);                // Ptr to the file name
    // Loop through the [GlobalVars] section
    for (uStr = 0; uStr < uGlbCnt; uStr++)
    {
        // Save ptr
        lpwSrc = lpwszTemp;

        // Format the counter
        uLen = wsprintfW (wszCount, FMTSTR_GLBCNT, uStr);

        // Read the next string
        GetPrivateProfileStringW (SECTNAME_GLOBALVARS,  // Ptr to the section name
                                  wszCount,             // Ptr to the key name
                                  L"",                  // Ptr to the default value
                                  lpwSrc,               // Ptr to the output buffer
                                  memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                  lpwszDPFE);           // Ptr to the file name
        // Parse the array attributes
        Assert (lpwSrc[0] NE L'\0');

        // ***FIXME*** -- Do we need to restore the object PermNdx?

        // Get the object storage type
        aplTypeObj = TranslateCharToArrayType (*lpwSrc++);

        Assert (*lpwSrc EQ L' '); lpwSrc++;

        // Get the object NELM
        swscanf (lpwSrc, L"%I64d", &aplNELMObj); lpwSrc = SkipBlackW (lpwSrc);

        Assert (*lpwSrc EQ L' '); lpwSrc++;

        // Get the object rank
        swscanf (lpwSrc, L"%I64d", &aplRankObj); lpwSrc = SkipBlackW (lpwSrc);

        Assert (*lpwSrc EQ L' '); lpwSrc++;

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

        // Create a symbol table entry for the )LOAD HGLOBAL
        lpSymEntry =
          SymTabAppendNewName_EM (wszCount, &stFlags);

        // Set the handle
        lpSymEntry->stData.stGlbData = MakePtrTypeGlb (hGlbObj);

        // Lock the memory to get a ptr to it
        lpMemObj = MyGlobalLock (hGlbObj);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemObj)
        // Fill in the header
        // Note that the RefCnt is initialized to zero
        // It will be incremented upon each reference
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeObj;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND ***FIXME*** -- Set correct PERMNDX_xxx
////////lpHeader->SysVar     = 0;               // Already zero from GHND
////////lpHeader->RefCnt     = 0;               // Already zero from GHND
        lpHeader->NELM       = 1;
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
                    wcTmp = *lpwCharEnd;
                    *lpwCharEnd = L'\0';

                    // Convert the format string to ASCII
                    W2A (szFmt, lpwSrc, sizeof (szFmt) - 1);

                    // Restore the original value
                    *lpwCharEnd = wcTmp;

                    // Use David Gay's routines
                    // Save in the result and skip over it
                    *((LPAPLFLOAT) lpMemObj)++ = strtod (szFmt, NULL);

                    // Skip to the next field
                    lpwSrc = &lpwCharEnd[1];
                } // End FOR

                break;

            case ARRAY_CHAR:
                Assert (L'\'' EQ *lpwSrc); lpwSrc++;

                // Loop through the elements
                for (uObj = 0; uObj < aplNELMObj; uObj++)
                {
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
                if (*lpwSrc EQ L'#')
                {
                    // Find the trailing L' '
                    lpwCharEnd = SkipToCharW (lpwSrc, L' ');

                    // Save old next char, zap to form zero-terminated name
                    wcTmp = *lpwCharEnd;
                    *lpwCharEnd = L'\0';

                    // Get the matching HGLOBAL
                    lpSymEntry = SymTabLookupName (lpwSrc, &stFlags);

                    Assert (lpSymEntry NE NULL);

                    // Restore the original value
                    *lpwCharEnd = wcTmp;

                    // Save in the result and skip over it
                    *((LPAPLNESTED) lpMemObj)++ = lpSymEntry->stData.stGlbData;

                    // Increment the reference count
                    DbgIncrRefCntDir (lpSymEntry->stData.stGlbData);

                    // Skip to the next field
                    lpwSrc = &lpwCharEnd[1];
                } else
                {
                    // Skip to the NELM
                    lpwSrc = SkipPastCharW (lpwSrc, L' ');

                    Assert (*lpwSrc EQ L'1');

                    // Skip to the rank
                    lpwSrc = SkipPastCharW (lpwSrc, L' ');

                    Assert (*lpwSrc EQ L'0');

                    // Skip to the value
                    lpwSrc = SkipPastCharW (lpwSrc, L' ');

                    // Split cases based upon the immediate type char
                    switch (*lpwSrc)
                    {
                        case L'B':  // Boolean
                        case L'I':  // Integer
                            // Scan in the value
                            swscanf (lpwSrc, L"%I64d", &aplInteger);

                            // Skip to the next field
                            lpwSrc = SkipPastCharW (lpwSrc, L' ');

                            // Save in the result and skip over it
                            *((LPAPLHETERO) lpMemObj)++ = SymTabAppendInteger_EM (aplInteger);

                            break;

                        case L'C':  // Character
                            Assert (L'\'' EQ *lpwSrc); lpwSrc++;

                            if (L'{' EQ  *lpwSrc)
                            {
                                // Get the next char
                                wcTmp = NameToChar (lpwSrc);

                                // Skip to the next field
                                lpwSrc = SkipPastCharW (lpwSrc, L'}');
                            } else
                                wcTmp = *lpwSrc++;

                            Assert (L'\'' EQ *lpwSrc); lpwSrc++;

                            // Save in the result and skip over it
                            *((LPAPLHETERO) lpMemObj)++ = SymTabAppendChar_EM (wcTmp);

                            break;

                        case L'F':  // Float
                            // Find the trailing L' '
                            lpwCharEnd = SkipToCharW (lpwSrc, L' ');

                            // Save old next char, zap to form zero-terminated name
                            wcTmp = *lpwCharEnd;
                            *lpwCharEnd = L'\0';

                            // Convert the format string to ASCII
                            W2A (szFmt, lpwSrc, sizeof (szFmt) - 1);

                            // Restore the original value
                            *lpwCharEnd = wcTmp;

                            // Save in the result and skip over it
                            *((LPAPLHETERO) lpMemObj)++ = SymTabAppendFloat_EM (strtod (szFmt, NULL));

                            // Skip to the next field
                            lpwSrc = &lpwCharEnd[1];

                            break;

                        defstop
                            break;
                    } // End FOR/SWITCH
                } // End FOR/SWITCH

                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this ptr
        MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
    } // End FOR

    // Loop through the SI levels
    for (uSID = 0; uSID <= uSIDepth; uSID++)
    {
        // Format the section name
        wsprintfW (wszSectName, SECTNAME_VARS L".%d", uSID);

        // Loop through the [Vars.0] section
        for (uStr = 0; uStr < uSymVar; uStr++)
        {
            // Save ptr
            lpwSrc = lpwszTemp;

            // Format the counter
            uLen = wsprintfW (wszCount, L"%d", uStr);

            // Read the next string
            GetPrivateProfileStringW (wszSectName,          // Ptr to the section name
                                      wszCount,             // Ptr to the key name
                                      L"",                  // Ptr to the default value
                                      lpwSrc,               // Ptr to the output buffer
                                      memVirtStr[MEMVIRT_WSZTEMP].MaxSize,  // Byte size of the output buffer
                                      lpwszDPFE);           // Ptr to the file name
            DbgBrk ();

            // Look for the name separator (L'=')
            lpwCharEnd = strchrW (lpwSrc, L'=');
            Assert (lpwCharEnd NE NULL);

            // Zap to form zero-terminated name
            *lpwCharEnd = L'\0';

            // Save the starting point
            lpwSrcStart = lpwSrc;
            lpwDst = lpwSrc;

            // Convert the name from {...} form
            while (*lpwSrc)
            if (*lpwSrc EQ L'{')
            {
                // Get the next char
                *lpwDst++ = NameToChar (lpwSrc);

                // Find the matching L'}'
                lpwSrc = SkipPastCharW (lpwSrc, L'}');
            } else
                *lpwDst++ = *lpwSrc++;

            // Ensure the name is zero-terminated
            *lpwDst = L'\0';

            // Lookup the name in the symbol table
            lpSymEntry = SymTabLookupName (lpwSrcStart, &stFlags);










            // Restore the original value
            *lpwCharEnd = L'=';
        } // End FOR

        // Format the section name
        wsprintfW (wszSectName, SECTNAME_FCNS L".%d", uSID);

        // Loop through the [Fcns.0] section
        for (uStr = 0; uStr < uSymFcn; uStr++)
        {
            DbgBrk ();



        } // End FOR
    } // End FOR










    AppendLine (L"NONCE COMMAND", FALSE, TRUE);

    goto ERROR_EXIT;







WSID_EXIT:
    // Set the value of the new []WSID as lpwszDPFE
    if (SaveNewWsid (lpwszDPFE))
        bRet = TRUE;

    goto NORMAL_EXIT;

WSNOTFOUND_EXIT:
    AppendLine (ERRMSG_WS_NOT_FOUND, FALSE, TRUE);

    goto ERROR_EXIT;

WSFULL_EXIT:
    AppendLine (ERRMSG_WS_FULL APPEND_NAME, FALSE, TRUE);
ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbDPFE); lpwszDPFE = NULL;

    return bRet;
} // End LoadWorkspace_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: sc_load.c
//***************************************************************************
