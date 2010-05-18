//***************************************************************************
//  NARS2000 -- Magic Function/Operator Execution Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
#include "headers.h"


extern MAGIC_FCNOPR MFO_MonIota;
extern MAGIC_FCNOPR MFO_DydIota;
extern MAGIC_FCNOPR MFO_MonDnShoe;
extern MAGIC_FCNOPR MFO_DydTilde;
extern MAGIC_FCNOPR MFO_MonRank;
extern MAGIC_FCNOPR MFO_DydRank;
extern MAGIC_FCNOPR MFO_Conform;
extern MAGIC_FCNOPR MFO_MonFMT;
extern MAGIC_FCNOPR MFO_Box;
extern MAGIC_FCNOPR MFO_MonVR;


//***************************************************************************
//  $ExecuteMagicFunction_EM_YY
//
//  Execute a magic function/operator
//***************************************************************************

LPPL_YYSTYPE ExecuteMagicFunction_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN      lptkFunc,             // Ptr to function token
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand (may be NULL if not an operator)
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis,             // Ptr to axis token (may be NULL)
     HGLOBAL      hGlbMFO,              // Magic function/operator global memory handle
     LPHSHTABSTR  lphtsPTD,             // Ptr to HSHTABSTR (may be NULL)
     LINE_NUMS    startLineType)        // Starting line type (see LINE_NUMS)

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes;               // Ptr to result
    HSHTABSTR    htsPTD;                // Old copy of HshTab struc

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If this magic function/operator uses a separate HTS, ...
    if (lphtsPTD)
    {
        // Save the old HshTab struc
        htsPTD = lpMemPTD->htsPTD;

        // Put the HshTab and SymTab into effect
        lpMemPTD->htsPTD = *lphtsPTD;

        // Save address of previous struc
        lpMemPTD->htsPTD.lpHshTabPrvSrch =
        lpMemPTD->htsPTD.lpHshTabPrvMFO  = &htsPTD;
    } // End IF

    lpYYRes =
      ExecDfnGlb_EM_YY (hGlbMFO,        // Magic function/operator global memory handle
                        lptkLftArg,     // Ptr to left arg token
                        lpYYFcnStrOpr,  // Ptr to operator function strand
                        lptkAxis,       // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                        lptkRhtArg,     // Ptr to right arg token
                        startLineType); // Starting line type (see LINE_NUMS)
    // If this magic function/operator uses a separate HTS, ...
    if (lphtsPTD)
    {
        // Delete address of previous struc
        lpMemPTD->htsPTD.lpHshTabPrvSrch =
        lpMemPTD->htsPTD.lpHshTabPrvMFO  = NULL;

        // Copy back the contents of the current struc
        *lphtsPTD = lpMemPTD->htsPTD;

        // Restore the old HTS
        lpMemPTD->htsPTD = htsPTD;
    } // End IF

    // If there was an error, set the caret to
    //   point to the primitive function
    if (lpYYRes EQ NULL)
        ErrorMessageSetToken (lptkFunc);

    return lpYYRes;
} // End ExecuteMagicFunction_EM_YY


//***************************************************************************
//  $ExecuteMagicOperator_EM_YY
//
//  Execute a magic operator
//***************************************************************************

LPPL_YYSTYPE ExecuteMagicOperator_EM_YY
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN      lptkFunc,             // Ptr to function token
     LPPL_YYSTYPE lpYYFcnStrLft,        // Ptr to left operand function strand (may be NULL if not an operator)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand (may be NULL if not an operator)
     LPPL_YYSTYPE lpYYFcnStrRht,        // Ptr to right operand function strand (may be NULL if not an operator)
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis,             // Ptr to axis token (may be NULL)
     HGLOBAL      hGlbMFO,              // Magic function/operator global memory handle
     LPHSHTABSTR  lphtsPTD,             // Ptr to HSHTABSTR (may be NULL)
     LINE_NUMS    startLineType)        // Starting line type (see LINE_NUMS)

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes;               // Ptr to result
    HSHTABSTR    htsPTD;                // Old copy of HshTab struc

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If this magic function/operator uses a separate HTS, ...
    if (lphtsPTD)
    {
        // Save the old HshTab struc
        htsPTD = lpMemPTD->htsPTD;

        // Put the HshTab and SymTab into effect
        lpMemPTD->htsPTD = *lphtsPTD;

        // Save address of previous struc
        lpMemPTD->htsPTD.lpHshTabPrvSrch =
        lpMemPTD->htsPTD.lpHshTabPrvMFO  = &htsPTD;
    } // End IF

    lpYYRes =
      ExecDfnOprGlb_EM_YY (hGlbMFO,         // Magic function/operator global memory handle
                           lptkLftArg,      // Ptr to left arg token
                           lpYYFcnStrLft,   // Ptr to left operand function strand
                           lpYYFcnStrOpr,   // Ptr to operator function strand
                           lpYYFcnStrRht,   // Ptr to right operand function strand
                           lptkAxis,        // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                           lptkRhtArg,      // Ptr to right arg token
                           startLineType);  // Starting line type (see LINE_NUMS)
    // If this magic function/operator uses a separate HTS, ...
    if (lphtsPTD)
    {
        // Delete address of previous struc
        lpMemPTD->htsPTD.lpHshTabPrvSrch =
        lpMemPTD->htsPTD.lpHshTabPrvMFO  = NULL;

        // Copy back the contents of the current struc
        *lphtsPTD = lpMemPTD->htsPTD;

        // Restore the old HTS
        lpMemPTD->htsPTD = htsPTD;
    } // End IF

    // If there was an error, set the caret to
    //   point to the primitive function
    if (lpYYRes EQ NULL)
        ErrorMessageSetToken (lptkFunc);

    return lpYYRes;
} // End ExecuteMagicOperator_EM_YY


//***************************************************************************
//  $Init1MagicFunction
//
//  Initialize a single magic function/operator
//***************************************************************************

HGLOBAL Init1MagicFunction
    (LPWCHAR          lpwszName,            // Ptr to the external name
     LPMAGIC_FCNOPR   lpMagicFcnOpr,        // Ptr to magic function/operator struc
     LPPERTABDATA     lpMemPTD,             // Ptr to PerTabData global memory
     HWND             hWndEC,               // Edit Ctrl window handle
     LPINIT_MFO       lpInitMFO)            // Ptr to temporary struc

{
    UINT           uLineLen;                // Line length var
    HGLOBAL        hGlbTxtHdr = NULL,       // Header text global memory handle
                   hGlbTknHdr = NULL,       // Tokenized header text ...
                   hGlbDfnHdr = NULL;       // User-defined function/operator header ...
    LPMEMTXT_UNION lpMemTxtLine;            // Ptr to header/line text global memory
    FHLOCALVARS    fhLocalVars = {0};       // Function Header local vars
    LPSYMENTRY     lpSymEntry;              // Ptr to SYMENTRY for the MFO
    MEMVIRTSTR     lclMemVirtStr[1] = {0};  // Room for one GuardAlloc
    LPTOKEN        lptkCSBeg;               // Ptr to next token on the CS stack
    HSHTABSTR      htsPTD;                  // Old copy of HshTab struc

    // If lpInitMFO is valid, ...
    if (lpInitMFO)
    {
        // If the HshTab has not been allocated as yet, ...
        if (lpInitMFO->lpHTS->lpHshTab EQ NULL)
        {
            // Check on Start < End
            Assert (lpInitMFO->uPtdMemVirtStart < lpInitMFO->uPtdMemVirtEnd);

            lpInitMFO->lpLclMemVirtStr[lpInitMFO->uPtdMemVirtStart].lpText = "lpInitMFO->htsMFO.lpHshTab in <Init1MagicFunction>";

            // Allocate virtual memory for the hash table
            if (!AllocHshTab (&lpInitMFO->lpLclMemVirtStr[lpInitMFO->uPtdMemVirtStart++],   // Ptr to this PTDMEMVIRT entry
                               lpInitMFO->lpHTS,                                            // Ptr ot this HSHTABSTR
                               128,                                                         // Initial # blocks in HshTab (@ EPB HTEs per block)
                               16,                                                          // # HTEs by which to resize when low
                               1024))                                                       // Maximum # HTEs
                DbgStop ();
        } // End IF

        // If the SymTab has not been allocated as yet, ...
        if (lpInitMFO->lpHTS->lpSymTab EQ NULL)
        {
            // Check on Start < End
            Assert (lpInitMFO->uPtdMemVirtStart < lpInitMFO->uPtdMemVirtEnd);

            lpInitMFO->lpLclMemVirtStr[lpInitMFO->uPtdMemVirtStart].lpText = "lpInitMFO->htsMFO.lpSymTab in <Init1MagicFunction>";

            // Allocate virtual memory for the symbol table
            if (!AllocSymTab (&lpInitMFO->lpLclMemVirtStr[lpInitMFO->uPtdMemVirtStart++],   // Ptr to this PTDMEMVIRT entry
                               lpMemPTD,                                                    // PerTabData global memory handle
                               lpInitMFO->lpHTS,                                            // Ptr to this HSHTABSTR
                               FALSE,                                                       // TRUE iff we're to initialize the constant STEs
                               256,                                                         // Initial # STEs in SymTab
                               16,                                                          // # STEs by which to resize when low
                               1024))                                                       // Maximum # STEs
                DbgStop ();
        } // End IF

        // Save the old HshTab struc
        htsPTD = lpMemPTD->htsPTD;

        // Put the HshTab and SymTab into effect
        lpMemPTD->htsPTD = *lpInitMFO->lpHTS;

        // Save address of previous struc
        lpMemPTD->htsPTD.lpHshTabPrvMFO = &htsPTD;

        // If the system names have not been appended as yet, ...
        if (!lpMemPTD->htsPTD.bSysNames)
        {
            // Append all system names to the local SymTab
            SymTabAppendAllSysNames_EM (&lpMemPTD->htsPTD);

            // Assign default values to the system vars
            AssignDefaultSysVars (lpMemPTD->htsPTD.lpSymQuad);

            // Mark as appended so as to avoid doing this the next time
            lpMemPTD->htsPTD.bSysNames = TRUE;
        } // End IF
    } // End IF

    // Save the ptr to the next token on the CS stack
    //   as our beginning
    lptkCSBeg = lpMemPTD->lptkCSNxt;

    // Get the length of the header line
    uLineLen  = lstrlenW (lpMagicFcnOpr->Header);

    // Allocate space for the text
    //   (the "sizeof (uLineLen)" is for the leading line length
    //    and the "+ 1" is for the terminating zero)
    hGlbTxtHdr = MyGlobalAlloc (GHND, sizeof (lpMemTxtLine->U) + (uLineLen + 1) * sizeof (lpMemTxtLine->C));
    if (!hGlbTxtHdr)
    {
        MessageBox (hWndEC,
                    "Insufficient memory to save the magic function/operator header text!!",
                    lpszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        goto ERROR_EXIT;
    } // End IF

    // The following test isn't an optimzation, but avoids
    //   overwriting the allocation limits of the buffer
    //   when filling in the leading WORD with uLineLen
    //   on the call to EM_GETLINE.

    // If the line is non-empty, ...
    if (uLineLen)
    {
        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLock (hGlbTxtHdr);

        // Save the line length
        lpMemTxtLine->U = uLineLen;

        // Tell EM_GETLINE maximum # chars in the buffer
        lpMemTxtLine->W = (WORD) uLineLen;

        // Copy the line text to global memory
        CopyMemoryW (&lpMemTxtLine->C, lpMagicFcnOpr->Header, uLineLen);

        // Tokenize the line
        hGlbTknHdr =
          Tokenize_EM (&lpMemTxtLine->C,    // The line to tokenize (not necessarily zero-terminated)
                        uLineLen,           // NELM of lpwszLine
                        hWndEC,             // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                        0,                  // Function line # (0 = header)
                       &ErrorHandler,       // Ptr to error handling function (may be NULL)
                        TRUE);              // TRUE iff we're tokenizing a Magic Function/Operator
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtHdr); lpMemTxtLine = NULL;
    } else
        // Tokenize the (empty) line
        hGlbTknHdr =
          Tokenize_EM (L"",                 // The line to tokenize (not necessarily zero-terminated)
                       0,                   // NELM of lpwszLine
                       NULL,                // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                       0,                   // Function line # (0 = header)
                       NULL,                // Ptr to error handling function (may be NULL)
                       TRUE);               // TRUE iff we're tokenizing a Magic Function/Operator
    if (!hGlbTknHdr)
    {
        WCHAR wszTemp[1024];

        // Format the error message
        wsprintfW (wszTemp,
                   ERRMSG_SYNTAX_ERROR_IN_FUNCTION_HEADER,
                   lpMemPTD->uCaret);
        // Display the error message
        MessageBoxW (hWndEC,
                    wszTemp,
                    lpwszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        goto ERROR_EXIT;
    } // End IF

    // Allocate virtual memory for the Variable Strand accumulator
    lclMemVirtStr[0].lpText   = "fhLocalVars.lpYYStrandStart in <Init1MagicFunction>";
    lclMemVirtStr[0].IncrSize = DEF_STRAND_INCRNELM * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].MaxSize  = DEF_STRAND_MAXNELM  * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].IniAddr  = (LPUCHAR)
    fhLocalVars.lpYYStrandStart =
      GuardAlloc (NULL,             // Any address
                  lclMemVirtStr[0].MaxSize,
                  MEM_RESERVE,
                  PAGE_READWRITE);
    if (!lclMemVirtStr[0].IniAddr)
    {
        MessageBox (hWndEC,
                    "Insufficient memory to save the function header strand stack!!",
                    lpszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        goto ERROR_EXIT;    // Mark as failed
    } // End IF

    // Link this struc into the chain
    LinkMVS (&lclMemVirtStr[0]);

    // Commit the intial size
    MyVirtualAlloc (lclMemVirtStr[0].IniAddr,
                    DEF_STRAND_INITNELM * sizeof (PL_YYSTYPE),
                    MEM_COMMIT,
                    PAGE_READWRITE);
    // Parse the function header
    if (ParseFcnHeader (hWndEC, hGlbTknHdr, &fhLocalVars, TRUE))
    {
        UINT         uLineNum,          // Current line # in the Edit Ctrl
                     uOffset,           // Cumulative offset
                     numResultSTE,      // # result STEs (may be zero)
                     numLftArgSTE,      // # left arg ...
                     numRhtArgSTE,      // # right ...
                     numLocalsSTE,      // # locals ...
                     numFcnLines,       // # lines in the function
                     numSTE;            // Loop counter
        LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header ...
        LPFCNLINE    lpFcnLines;        // Ptr to array of function line structs (FCNLINE[numFcnLines])
        LPSYMENTRY  *lplpSymDfnHdr;     // Ptr to LPSYMENTRYs at end of user-defined function/operator header
        CSLOCALVARS  csLocalVars = {0}; // CS local vars
        SYSTEMTIME   systemTime;        // Current system (UTC) time

        // Get # extra result STEs
        if (fhLocalVars.lpYYResult)
            // Save in global memory
            numResultSTE = fhLocalVars.lpYYResult->uStrandLen;
        else
            numResultSTE = 0;

        // Get # extra left arg STEs
        if (fhLocalVars.lpYYLftArg)
            // Save in global memory
            numLftArgSTE = fhLocalVars.lpYYLftArg->uStrandLen;
        else
            numLftArgSTE = 0;

        // Get # extra right arg STEs
        if (fhLocalVars.lpYYRhtArg)
            // Save in global memory
            numRhtArgSTE = fhLocalVars.lpYYRhtArg->uStrandLen;
        else
            numRhtArgSTE = 0;

        // Get # locals STEs
        if (fhLocalVars.lpYYLocals)
            // Save in global memory
            numLocalsSTE = fhLocalVars.lpYYLocals->uStrandLen;
        else
            numLocalsSTE = 0;

        // Get # lines in the function
        numFcnLines = lpMagicFcnOpr->numFcnLines;

        // Allocate global memory for the function header
        hGlbDfnHdr =
          MyGlobalAlloc (GHND, sizeof (DFN_HEADER)
                             + sizeof (LPSYMENTRY) * (numResultSTE
                                                    + numLftArgSTE
                                                    + numRhtArgSTE
                                                    + numLocalsSTE)
                             + sizeof (FCNLINE) * numFcnLines);
        if (!hGlbDfnHdr)
        {
            MessageBox (hWndEC,
                        "Insufficient memory to save the function header!!",
                        lpszAppName,
                        MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

        // Save numbers in global memory
        lpMemDfnHdr->numResultSTE = numResultSTE;
        lpMemDfnHdr->numLftArgSTE = numLftArgSTE;
        lpMemDfnHdr->numRhtArgSTE = numRhtArgSTE;
        lpMemDfnHdr->numLocalsSTE = numLocalsSTE;

        // Save the static parts of the function into global memory
        lpMemDfnHdr->Sig.nature   = DFN_HEADER_SIGNATURE;
        lpMemDfnHdr->Version      = 0;
        lpMemDfnHdr->DfnType      = fhLocalVars.DfnType;
        lpMemDfnHdr->FcnValence   = fhLocalVars.FcnValence;
        lpMemDfnHdr->DfnAxis      = fhLocalVars.DfnAxis;
        lpMemDfnHdr->NoDispRes    = fhLocalVars.NoDispRes;
        lpMemDfnHdr->ListRes      = fhLocalVars.ListRes;
        lpMemDfnHdr->ListLft      = fhLocalVars.ListLft;
        lpMemDfnHdr->ListRht      = fhLocalVars.ListRht;
        lpMemDfnHdr->PermFn       = TRUE;
        lpMemDfnHdr->RefCnt       = 1;
        lpMemDfnHdr->numFcnLines  = numFcnLines;
        lpMemDfnHdr->steLftOpr    = fhLocalVars.lpYYLftOpr
                                  ? fhLocalVars.lpYYLftOpr ->tkToken.tkData.tkSym
                                  : NULL;
        lpMemDfnHdr->steFcnName   = fhLocalVars.lpYYFcnName->tkToken.tkData.tkSym;
        lpMemDfnHdr->steAxisOpr   = fhLocalVars.lpYYAxisOpr
                                  ? fhLocalVars.lpYYAxisOpr->tkToken.tkData.tkSym
                                  : NULL;
        lpMemDfnHdr->steRhtOpr    = fhLocalVars.lpYYRhtOpr
                                  ? fhLocalVars.lpYYRhtOpr ->tkToken.tkData.tkSym
                                  : NULL;
        lpMemDfnHdr->hGlbTxtHdr   = hGlbTxtHdr;
        lpMemDfnHdr->hGlbTknHdr   = hGlbTknHdr;
////////lpMemDfnHdr->hGlbUndoBuff = NULL;       // Already zero from GHND
////////lpMemDfnHdr->hGlbMonInfo  = NULL;       // Already zero from GHND

        // Get the current system (UTC) time
        GetSystemTime (&systemTime);

        // Convert system time to file time and save as creation time
        SystemTimeToFileTime (&systemTime, &lpMemDfnHdr->ftCreation);
        lpMemDfnHdr->ftLastMod    = lpMemDfnHdr->ftCreation;

        // Save the dynamic parts of the function into global memory

        // Initialize cumulative offset
        uOffset = sizeof (DFN_HEADER);

        // Initialize ptr to ptr to SYMENTRYs at end of header
        lplpSymDfnHdr = (LPAPLHETERO) ByteAddr (lpMemDfnHdr, uOffset);

        // If there's a result, ...
        if (fhLocalVars.lpYYResult)
        {
            lpMemDfnHdr->offResultSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numResultSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYResult[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offResultSTE = 0;

        // If there's a left arg, ...
        if (fhLocalVars.lpYYLftArg)
        {
            lpMemDfnHdr->offLftArgSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numLftArgSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYLftArg[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offLftArgSTE = 0;

        // If there's a right arg, ...
        if (fhLocalVars.lpYYRhtArg)
        {
            lpMemDfnHdr->offRhtArgSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numRhtArgSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYRhtArg[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offRhtArgSTE = 0;

        // If there are any locals, ...
        if (fhLocalVars.lpYYLocals)
        {
            lpMemDfnHdr->offLocalsSTE = uOffset;

            // Copy STEs to global memory
            for (numSTE = 0; numSTE < numLocalsSTE; numSTE++)
            {
                *lplpSymDfnHdr++ = fhLocalVars.lpYYLocals[numSTE].tkToken.tkData.tkSym;
                uOffset += sizeof (LPSYMENTRY);
            } // End FOR
        } else
            lpMemDfnHdr->offLocalsSTE = 0;

        // Save offset to start of function line structs
        if (lpMemDfnHdr->numFcnLines)
            lpMemDfnHdr->offFcnLines = uOffset;
        else
            lpMemDfnHdr->offFcnLines = 0;

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Loop through the lines
        for (uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
        {
            HGLOBAL hGlbTxtLine;    // Line text global memory handle

            // Get the line length of the line
            uLineLen = lstrlenW (lpMagicFcnOpr->Body[uLineNum]);

            // Allocate global memory to hold this text
            // The "sizeof (uLineLen) + " is for the leading length
            //   and the "+ 1" is for the terminating zero
            //   as well as to handle GlobalLock's aversion to locking
            //   zero-length arrays
            hGlbTxtLine = MyGlobalAlloc (GHND, sizeof (uLineLen) + (uLineLen + 1) * sizeof (APLCHAR));
            if (!hGlbTxtLine)
            {
                MessageBox (hWndEC,
                            "Insufficient memory to save a function line!!",
                            lpszAppName,
                            MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                goto ERROR_EXIT;
            } // End IF

            // Save the global memory handle
            lpFcnLines->hGlbTxtLine = hGlbTxtLine;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Save the line length
            lpMemTxtLine->U = uLineLen;

            // The following test isn't an optimzation, but avoids
            //   overwriting the allocation limits of the buffer
            //   when filling in the leading WORD with uLineLen
            //   on the call to EM_GETLINE.

            // If the line is non-empty, ...
            if (uLineLen)
            {
                // Tell EM_GETLINE maximum # chars in the buffer
                lpMemTxtLine->W = (WORD) uLineLen;

                // Copy the line text to global memory
                CopyMemoryW (&lpMemTxtLine->C, lpMagicFcnOpr->Body[uLineNum], uLineLen);

                // Tokenize the line
                lpFcnLines->hGlbTknLine =
                  Tokenize_EM (&lpMemTxtLine->C,    // The line to tokenize (not necessarily zero-terminated)
                                uLineLen,           // NELM of lpwszLine
                                hWndEC,             // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                                uLineNum + 1,       // Function line # (0 = header)
                               &ErrorHandler,       // Ptr to error handling function (may be NULL)
                                TRUE);              // TRUE iff we're tokenizing a Magic Function/Operator
            } else
                // Tokenize the (empty) line
                lpFcnLines->hGlbTknLine =
                  Tokenize_EM (L"",                 // The line to tokenize (not necessarily zero-terminated)
                               0,                   // NELM of lpwszLine
                               hWndEC,              // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                               uLineNum + 1,        // Function line # (0 = header)
                              &ErrorHandler,        // Ptr to error handling function (may be NULL)
                               TRUE);               // TRUE iff we're tokenizing a Magic Function/Operator
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

            // If tokenization failed, ...
            if (!lpFcnLines->hGlbTknLine)
            {
                DbgBrk ();      // ***FINISHME*** -- What to do if Tokenize_EM fails??

                // We can display a MessageBox with an error msg
                //   as we have the error message text in
                //   lpMemPTD->lpwszErrorMessage, the line in hGlbTxtLine,
                //   and the caret position in lpMemPTD->uCaret.




            } // End IF

            // Transfer Stop & Trace info
            lpFcnLines->bStop  =
            lpFcnLines->bTrace = FALSE;     // ***FIXME*** -- transfer from orig fn

            // Skip to the next struct
            lpFcnLines++;
        } // End FOR

        // Fill in the CS local vars struc
        csLocalVars.hWndEC      = hWndEC;
        csLocalVars.lpMemPTD    = lpMemPTD;
        csLocalVars.lptkCSBeg   =
        csLocalVars.lptkCSNxt   = lptkCSBeg;
        csLocalVars.lptkCSLink  = NULL;
        csLocalVars.hGlbDfnHdr  = hGlbDfnHdr;
        csLocalVars.hGlbImmExec = NULL;

        // Parse the tokens on the CS stack
        if (!ParseCtrlStruc_EM (&csLocalVars))
        {
            WCHAR wszTemp[1024];

            // Format the error message
            wsprintfW (wszTemp,
                       L"CS SYNTAX ERROR in %s -- Line %d statement %d position %d",
                       lpwszName,
                       csLocalVars.tkCSErr.tkData.Orig.c.uLineNum,
                       csLocalVars.tkCSErr.tkData.Orig.c.uStmtNum + 1,
                       csLocalVars.tkCSErr.tkCharIndex);
            MBW (wszTemp);

            goto ERROR_EXIT;
        } // End IF

        // Check for special labels ([]ID, etc.)
        if (!GetSpecialLabelNums (lpMemDfnHdr, NULL, TRUE))
            goto ERROR_EXIT;

        // Get the function/operator LPSYMENTRY
        lpSymEntry = lpMemDfnHdr->steFcnName;

        // Set the flags & handle
        lpSymEntry->stFlags.Inuse      =
        lpSymEntry->stFlags.Value      =
        lpSymEntry->stFlags.UsrDfn     = TRUE;
        lpSymEntry->stFlags.ObjName    = OBJNAME_MFO;
        lpSymEntry->stFlags.DfnAxis    = lpMemDfnHdr->DfnAxis;
        lpSymEntry->stFlags.stNameType = fhLocalVars.fhNameType;
        lpSymEntry->stData.stGlbData   = MakePtrTypeGlb (hGlbDfnHdr);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

        goto NORMAL_EXIT;
    } // End IF
ERROR_EXIT:
    if (hGlbDfnHdr)
    {
        FreeResultGlobalDfn (hGlbDfnHdr); hGlbDfnHdr = NULL;
    } // End IF

    if (hGlbTknHdr)
    {
        // Free the tokens
        Untokenize (hGlbTknHdr);

        // We no longer need this storage
        DbgGlobalFree (hGlbTknHdr); hGlbTknHdr = NULL;
    } // End IF

    if (hGlbTxtHdr)
    {
        // We no longer need this storage
        MyGlobalFree (hGlbTxtHdr); hGlbTxtHdr = NULL;
    } // End IF
NORMAL_EXIT:
    // Restore the ptr to the next token on the CS stack
    lpMemPTD->lptkCSNxt = lptkCSBeg;

    // If lpInitMFO is valid, ...
    if (lpInitMFO)
    {
        // Delete address of previous struc
        lpMemPTD->htsPTD.lpHshTabPrvMFO = NULL;

        // Copy back the contents of the current struc
        *lpInitMFO->lpHTS = lpMemPTD->htsPTD;

        // Restore the old HTS
        lpMemPTD->htsPTD = htsPTD;
    } // End IF

    // If we allocated virtual storage, ...
    if (lclMemVirtStr[0].IniAddr)
    {
        // Free the virtual storage
        MyVirtualFree (lclMemVirtStr[0].IniAddr, 0, MEM_RELEASE); lclMemVirtStr[0].IniAddr = NULL;

        // Unlink this entry from the chain
        UnlinkMVS (&lclMemVirtStr[0]);
    } // End IF

    return hGlbDfnHdr;
} // End Init1MagicFunction


//***************************************************************************
//  $InitMagicFunctions
//
//  Initialize all magic functions/operators
//***************************************************************************

UBOOL InitMagicFunctions
    (LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     HWND         hWndEC,               // Session Manager window handle
     LPMEMVIRTSTR lpLclMemVirtStr,      // Ptr to local MemVirtStr
     UINT         uPtdMemVirtStart,     // Starting offset into lpLclMemVirtStr
     UINT         uPtdMemVirtEnd)       // Ending   ...

{
    INIT_MFO initMFO;                   // Temporary struc for passing multiple args

    // Initialize the temp struc
////initMFO.lpHTS            = NULL;             // Set below
    initMFO.lpLclMemVirtStr  = lpLclMemVirtStr;
    initMFO.uPtdMemVirtStart = uPtdMemVirtStart;
    initMFO.uPtdMemVirtEnd   = uPtdMemVirtEnd;

    // Define the magic functions/operators
    lpMemPTD->hGlbMFO[MFOE_MonIota  ]  = Init1MagicFunction (MFON_MonIota  , &MFO_MonIota  , lpMemPTD, hWndEC, NULL);
    lpMemPTD->hGlbMFO[MFOE_DydIota  ]  = Init1MagicFunction (MFON_DydIota  , &MFO_DydIota  , lpMemPTD, hWndEC, NULL);
    lpMemPTD->hGlbMFO[MFOE_MonDnShoe]  = Init1MagicFunction (MFON_MonDnShoe, &MFO_MonDnShoe, lpMemPTD, hWndEC, NULL);
    lpMemPTD->hGlbMFO[MFOE_DydTilde ]  = Init1MagicFunction (MFON_DydTilde , &MFO_DydTilde , lpMemPTD, hWndEC, NULL);
    lpMemPTD->hGlbMFO[MFOE_MonRank  ]  = Init1MagicFunction (MFON_MonRank  , &MFO_MonRank  , lpMemPTD, hWndEC, NULL);
    lpMemPTD->hGlbMFO[MFOE_DydRank  ]  = Init1MagicFunction (MFON_DydRank  , &MFO_DydRank  , lpMemPTD, hWndEC, NULL);
    lpMemPTD->hGlbMFO[MFOE_Conform  ]  = Init1MagicFunction (MFON_Conform  , &MFO_Conform  , lpMemPTD, hWndEC, NULL);
    lpMemPTD->hGlbMFO[MFOE_MonFMT   ]  = Init1MagicFunction (MFON_MonFMT   , &MFO_MonFMT   , lpMemPTD, hWndEC, NULL);
    lpMemPTD->hGlbMFO[MFOE_Box      ]  = Init1MagicFunction (MFON_Box      , &MFO_Box      , lpMemPTD, hWndEC, NULL);
    initMFO.lpHTS = &lpMemPTD->htsPTD_MonVR;
    lpMemPTD->hGlbMFO[MFOE_MonVR    ]  = Init1MagicFunction (MFON_MonVR    , &MFO_MonVR    , lpMemPTD, hWndEC, &initMFO);

    // Set []IO to zero in the symbol table for []VR
    lpMemPTD->htsPTD_MonVR.lpSymQuad[SYSVAR_IO]->stData.stBoolean = 0;

    return TRUE;
} // InitMagicFunctions


//***************************************************************************
//  End of File: execmfn.c
//***************************************************************************
