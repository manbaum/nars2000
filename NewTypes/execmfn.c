//***************************************************************************
//  NARS2000 -- Magic Function/Operator Execution Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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
extern MAGIC_FCNOPR MFO_MonRank;
extern MAGIC_FCNOPR MFO_DydRank;
extern MAGIC_FCNOPR MFO_MonFMT;
extern MAGIC_FCNOPR MFO_BoxFMT;
extern MAGIC_FCNOPR MFO_MonVR;
extern MAGIC_FCNOPR MFO_IdnDot;
extern MAGIC_FCNOPR MFO_IdnJotDot;
extern MAGIC_FCNOPR MFO_MAD;
extern MAGIC_FCNOPR MFO_MSD;
extern MAGIC_FCNOPR MFO_MU;
extern MAGIC_FCNOPR MFO_MI;
extern MAGIC_FCNOPR MFO_MIO;
extern MAGIC_FCNOPR MFO_MEO;
extern MAGIC_FCNOPR MFO_MM;
extern MAGIC_FCNOPR MFO_MLRS;
extern MAGIC_FCNOPR MFO_MLRSU;
extern MAGIC_FCNOPR MFO_MMUL;
extern MAGIC_FCNOPR MFO_DydDnShoe;
extern MAGIC_FCNOPR MFO_DydUpShoe;
extern MAGIC_FCNOPR MFO_DydLRShoeUnd;
extern MAGIC_FCNOPR MFO_SD;
extern MAGIC_FCNOPR MFO_MonDomino;
extern MAGIC_FCNOPR MFO_DydDomino;
extern MAGIC_FCNOPR MFO_DydDotDot;
extern MAGIC_FCNOPR MFO_DydIotaUnderbar;
extern MAGIC_FCNOPR MFO_MonDot;
extern MAGIC_FCNOPR MFO_MonDotInit;
extern MAGIC_FCNOPR MFO_DydEpsUnderbar;
extern MAGIC_FCNOPR MFO_DydConv;
extern MAGIC_FCNOPR MFO_IdnConv;
extern MAGIC_FCNOPR MFO_DydScan;
extern MAGIC_FCNOPR MFO_DydScan1;
extern MAGIC_FCNOPR MFO_RoS1L;
extern MAGIC_FCNOPR MFO_RoS1R;
extern MAGIC_FCNOPR MFO_RoS2;
extern MAGIC_FCNOPR MFO_RoS3;
extern MAGIC_FCNOPR MFO_MDIU;
extern MAGIC_FCNOPR MFO_DetSing;
extern MAGIC_FCNOPR MFO_DydVOFact;
extern MAGIC_FCNOPR MFO_MonExecute;
extern MAGIC_FCNOPR MFO_DydSquad;
extern MAGIC_FCNOPR MFO_DydSlope;
extern MAGIC_FCNOPR MFO_MonShriek;
extern MAGIC_FCNOPR MFO_MatOpr;
extern MAGIC_FCNOPR MFO_DetPerm;
extern MAGIC_FCNOPR MFO_DydDot2;
extern MAGIC_FCNOPR MFO_DydMask;
extern MAGIC_FCNOPR MFO_DydMesh;
extern MAGIC_FCNOPR MFO_MonDomino2;
extern MAGIC_FCNOPR MFO_DydDomino2;
extern MAGIC_FCNOPR MFO_InvPJDRed;
extern MAGIC_FCNOPR MFO_InvBV;
extern MAGIC_FCNOPR MFO_IdnJot;
extern MAGIC_FCNOPR MFO_InvBS;
extern MAGIC_FCNOPR MFO_InvCBS;


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
    LPHSHTABSTR  lphtsOld;              // Ptr to old copy of HTS

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If this magic function/operator uses a separate HTS, ...
    if (lphtsPTD NE NULL)
    {
        // Save the old HTS
        lphtsOld = lpMemPTD->lphtsPTD;

        // Save address of previous struc
        lphtsPTD->lphtsPrvSrch =
        lphtsPTD->lphtsPrvMFO  = lphtsOld;

        // Put the HshTab and SymTab into effect
        lpMemPTD->lphtsPTD = lphtsPTD;

        // Make a copy of the current system vars so we have up-to-date values
        CopySysVars (lpMemPTD->lphtsPTD, lphtsOld);
    } // End IF

    lpYYRes =
      ExecDfnGlb_EM_YY (hGlbMFO,        // Magic function/operator global memory handle
                        lptkLftArg,     // Ptr to left arg token
                        lpYYFcnStrOpr,  // Ptr to operator function strand
                        lptkAxis,       // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                        lptkRhtArg,     // Ptr to right arg token
                        startLineType); // Starting line type (see LINE_NUMS)
    // If this magic function/operator uses a separate HTS, ...
    if (lphtsPTD NE NULL)
    {
        // Restore the old HTS
        lpMemPTD->lphtsPTD = lphtsOld;

        // Delete address of previous struc
        lphtsPTD->lphtsPrvSrch =
        lphtsPTD->lphtsPrvMFO  = NULL;
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
     LPPL_YYSTYPE lpYYFcnStrLftOpr,     // Ptr to left operand function strand (may be NULL if not an operator)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to operator function strand (may be NULL if not an operator)
     LPPL_YYSTYPE lpYYFcnStrRhtOpr,     // Ptr to right operand function strand (may be NULL if not a dyadic operator)
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis,             // Ptr to axis token (may be NULL)
     HGLOBAL      hGlbMFO,              // Magic function/operator global memory handle
     LPHSHTABSTR  lphtsPTD,             // Ptr to HSHTABSTR (may be NULL)
     LINE_NUMS    startLineType)        // Starting line type (see LINE_NUMS)

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes;               // Ptr to result
    LPHSHTABSTR  lphtsOld;              // Ptr to old copy of HTS

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If this magic function/operator uses a separate HTS, ...
    if (lphtsPTD NE NULL)
    {
        // Save the old HTS
        lphtsOld = lpMemPTD->lphtsPTD;

        // Save address of previous struc
        lphtsPTD->lphtsPrvSrch =
        lphtsPTD->lphtsPrvMFO  = lphtsOld;

        // Put the HshTab and SymTab into effect
        lpMemPTD->lphtsPTD = lphtsPTD;

        // Make a copy of the current system vars so we have up-to-date values
        CopySysVars (lpMemPTD->lphtsPTD, lphtsOld);
    } // End IF

    lpYYRes =
      ExecDfnOprGlb_EM_YY (hGlbMFO,             // Magic function/operator global memory handle
                           lptkLftArg,          // Ptr to left arg token
                           lpYYFcnStrLftOpr,    // Ptr to left operand function strand
                           lpYYFcnStrOpr,       // Ptr to operator function strand
                           lpYYFcnStrRhtOpr,    // Ptr to right operand function strand
                           lptkAxis,            // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                           lptkRhtArg,          // Ptr to right arg token
                           startLineType);      // Starting line type (see LINE_NUMS)
    // If this magic function/operator uses a separate HTS, ...
    if (lphtsPTD NE NULL)
    {
        // Restore the old HTS
        lpMemPTD->lphtsPTD = lphtsOld;

        // Delete address of previous struc
        lphtsPTD->lphtsPrvSrch =
        lphtsPTD->lphtsPrvMFO  = NULL;
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
    LPHSHTABSTR    lphtsOld;                // Ptr to old copy of HshTab struc

    // If lpInitMFO is valid, ...
    if (lpInitMFO NE NULL)
    {
        // If the HshTab has not been allocated as yet, ...
        if (lpInitMFO->lphtsMFO->lpHshTab EQ NULL)
        {
            // Count in another entry
            lpInitMFO->uCount++;

            // Ensure we have room in PTDMEMVIRT_ENUM
            if (lpInitMFO->uCount > numEntriesWithLocalSYMorHSH)
                DbgStop ();     // You forgot to define an additional entry in PTDMEMVIRT_ENUM

            // Check on Start < End
            Assert (lpInitMFO->uPtdMemVirtStart < lpInitMFO->uPtdMemVirtEnd);

            lpInitMFO->lpLclMemVirtStr[lpInitMFO->uPtdMemVirtStart].lpText = "lpInitMFO->htsMFO.lpHshTab in <Init1MagicFunction>";

            // Initialize entries
            lpInitMFO->lphtsMFO->bAFO = FALSE;
            lpInitMFO->lphtsMFO->bMFO = TRUE;

            // Allocate virtual memory for the hash table
            if (!AllocHshTab (&lpInitMFO->lpLclMemVirtStr[lpInitMFO->uPtdMemVirtStart++],   // Ptr to this PTDMEMVIRT entry
                               lpInitMFO->lphtsMFO,                                         // Ptr to this HSHTABSTR
                               DEF_MFO_HSHTAB_NBLKS,                                        // Initial # blocks in HshTab (@ EPB HTEs per block)
                               DEF_HSHTAB_EPB,                                              // # entries per block
                               DEF_MFO_HSHTAB_INCRNELM,                                     // # HTEs by which to resize when low
                               gMFOHshTabSize,                                              // Maximum # HTEs
                               TRUE))                                                       // TRUE iff we're to link this struc into the MVS
                DbgStop ();
        } // End IF

        // If the SymTab has not been allocated as yet, ...
        if (lpInitMFO->lphtsMFO->lpSymTab EQ NULL)
        {
            // Count in another entry
            lpInitMFO->uCount++;

            // Ensure we have room in PTDMEMVIRT_ENUM
            if (lpInitMFO->uCount > numEntriesWithLocalSYMorHSH)
                DbgStop ();     // You forgot to define an additional entry in PTDMEMVIRT_ENUM

            // Check on Start < End
            Assert (lpInitMFO->uPtdMemVirtStart < lpInitMFO->uPtdMemVirtEnd);

            lpInitMFO->lpLclMemVirtStr[lpInitMFO->uPtdMemVirtStart].lpText = "lpInitMFO->htsMFO.lpSymTab in <Init1MagicFunction>";
#ifdef DEBUG
            // Initialize entries
            lpInitMFO->lphtsMFO->bAFO = FALSE;
            lpInitMFO->lphtsMFO->bMFO = TRUE;
#endif
            // Allocate virtual memory for the symbol table
            if (!AllocSymTab (&lpInitMFO->lpLclMemVirtStr[lpInitMFO->uPtdMemVirtStart++],   // Ptr to this PTDMEMVIRT entry
                               lpInitMFO->lphtsMFO,                                         // Ptr to this HSHTABSTR
                               DEF_MFO_SYMTAB_INITNELM,                                     // Initial # STEs in SymTab
                               DEF_MFO_SYMTAB_INCRNELM,                                     // # STEs by which to resize when low
                               gMFOSymTabSize,                                              // Maximum # STEs
                               TRUE))                                                       // TRUE iff we're to link this struc into the MVS
                DbgStop ();
        } // End IF

        // Save the old HshTab struc
        lphtsOld = lpMemPTD->lphtsPTD;

        // Save address of previous struc
        lpInitMFO->lphtsMFO->lphtsPrvMFO = lphtsOld;

        // Put the HshTab and SymTab into effect
        lpMemPTD->lphtsPTD = lpInitMFO->lphtsMFO;

        // If the system names have not been appended as yet, ...
        if (!lpInitMFO->lphtsMFO->bSysNames)
        {
            // Append all system names to the local SymTab
            SymTabAppendAllSysNames_EM (lpInitMFO->lphtsMFO);

            // Assign default values to the system vars using a specific HTS
            AssignDefaultHTSSysVars (lpMemPTD, lpInitMFO->lphtsMFO);

            // Mark as appended so as to avoid doing this the next time
            lpInitMFO->lphtsMFO->bSysNames = TRUE;
        } // End IF
    } // End IF

    // Save the ptr to the next token on the CS stack
    //   as our beginning
    lptkCSBeg = lpMemPTD->lptkCSNxt;

    // Get the length of the header line
    uLineLen  = lstrlenW (lpMagicFcnOpr->Header);

    // Allocate space for the text
    //   (the "sizeof (lpMemTxtLine->U)" is for the leading line length
    //    and the "+ 1" is for the terminating zero)
    hGlbTxtHdr = DbgGlobalAlloc (GHND, sizeof (lpMemTxtLine->U) + (uLineLen + 1) * sizeof (lpMemTxtLine->C));
    if (hGlbTxtHdr EQ NULL)
    {
        MessageBoxW (hWndMF,
                    L"Insufficient memory to save the magic function/operator header text!!",
                     lpwszAppName,
                     MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        goto ERROR_EXIT;
    } // End IF

    // The following test isn't an optimzation, but avoids
    //   overwriting the allocation limits of the buffer
    //   when filling in the leading WORD with uLineLen
    //   on the call to EM_GETLINE.

    // If the line is non-empty, ...
    if (uLineLen NE 0)
    {
        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLock000 (hGlbTxtHdr);    // ->U not assigned as yet

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
                        0,                  // Logical function line # (0 = header)
                       &ErrorHandler,       // Ptr to error handling function (may be NULL)
                        NULL,               // Ptr to common struc (may be NULL if unused)
                        TRUE);              // TRUE iff we're tokenizing a Magic Function/Operator
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtHdr); lpMemTxtLine = NULL;
    } else
        // Tokenize the (empty) line
        hGlbTknHdr =
          Tokenize_EM (L"",                 // The line to tokenize (not necessarily zero-terminated)
                       0,                   // NELM of lpwszLine
                       NULL,                // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                       0,                   // Logical function line # (0 = header)
                       NULL,                // Ptr to error handling function (may be NULL)
                       NULL,                // Ptr to common struc (may be NULL if unused)
                       TRUE);               // TRUE iff we're tokenizing a Magic Function/Operator
    if (hGlbTknHdr EQ NULL)
    {
        WCHAR wszTemp[1024];

        // Format the error message
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                    ERRMSG_SYNTAX_ERROR_IN_FUNCTION_HEADER,
                    lpMemPTD->uCaret);
        // Display the error message
        MessageBoxW (hWndMF,
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
        MessageBoxW (hWndMF,
                    L"Insufficient memory to save the function header strand stack!!",
                     lpwszAppName,
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
        UINT         uLogLineNum,       // Current logical line # in the Edit Ctrl (0 = header)
                     uPhyLineNum,       // ...     physical ...
                     uOffset,           // Cumulative offset
                     numResultSTE,      // # result STEs (may be zero)
                     numLftArgSTE,      // # left arg ...
                     numRhtArgSTE,      // # right ...
                     numLocalsSTE,      // # locals ...
                     numPhyLines,       // # physical lines in the function
                     numLogLines,       // # logical  ...
                     numSTE;            // Loop counter
        LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header ...
        LPFCNLINE    lpFcnLines;        // Ptr to array of function line structs (FCNLINE[numFcnLines])
        LPSYMENTRY  *lplpSymDfnHdr;     // Ptr to LPSYMENTRYs at end of user-defined function/operator header
        CSLOCALVARS  csLocalVars = {0}; // CS local vars
        SYSTEMTIME   systemTime;        // Current system (UTC) time

        // Get # extra result STEs
        if (fhLocalVars.lpYYResult NE NULL)
            // Save in global memory
            numResultSTE = fhLocalVars.lpYYResult->uStrandLen;
        else
            numResultSTE = 0;

        // Get # extra left arg STEs
        if (fhLocalVars.lpYYLftArg NE NULL)
            // Save in global memory
            numLftArgSTE = fhLocalVars.lpYYLftArg->uStrandLen;
        else
            numLftArgSTE = 0;

        // Get # extra right arg STEs
        if (fhLocalVars.lpYYRhtArg NE NULL)
            // Save in global memory
            numRhtArgSTE = fhLocalVars.lpYYRhtArg->uStrandLen;
        else
            numRhtArgSTE = 0;

        // Get # locals STEs
        if (fhLocalVars.lpYYLocals NE NULL)
            // Save in global memory
            numLocalsSTE = fhLocalVars.lpYYLocals->uStrandLen;
        else
            numLocalsSTE = 0;

        // Get # lines in the function
        numPhyLines = lpMagicFcnOpr->numFcnLines;

        // The lines of MFOs do not come from an EC
        //   so Physical == Logical
        numLogLines = lpMagicFcnOpr->numFcnLines;

        // Get size of tokenization of all lines (excluding the header)
        for (uOffset = 0, uLogLineNum = uPhyLineNum = 1; uPhyLineNum <= numPhyLines; uPhyLineNum++)
            // If the preceding physical line is not continued to the current line, ...
            if (!SF_IsLineContMFO (hWndEC, lpMagicFcnOpr, uPhyLineNum - 1))
            {
                // Size a function line
                if (SaveFunctionLine (NULL, lpMagicFcnOpr, NULL, uLogLineNum, uPhyLineNum, NULL, hWndEC, NULL, &uOffset) EQ -1)
                    goto ERROR_EXIT;
                // Count in another logical line
                uLogLineNum++;
            } // End IF

        // Allocate global memory for the function header
        hGlbDfnHdr =
          DbgGlobalAlloc (GHND, sizeof (DFN_HEADER)
                              + sizeof (LPSYMENTRY) * (numResultSTE
                                                     + numLftArgSTE
                                                     + numRhtArgSTE
                                                     + numLocalsSTE)
                              + sizeof (FCNLINE) * numLogLines
                              + uOffset);
        if (hGlbDfnHdr EQ NULL)
        {
            MessageBoxW (hWndMF,
                        L"Insufficient memory to save the function header!!",
                         lpwszAppName,
                         MB_OK | MB_ICONWARNING | MB_APPLMODAL);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock000 (hGlbDfnHdr);

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
        lpMemDfnHdr->RefCnt       = 1;
        lpMemDfnHdr->numFcnLines  = numPhyLines;
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
        lpMemDfnHdr->bMFO         = TRUE;
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
        if (fhLocalVars.lpYYResult NE NULL)
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
        if (fhLocalVars.lpYYLftArg NE NULL)
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
        if (fhLocalVars.lpYYRhtArg NE NULL)
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
        if (fhLocalVars.lpYYLocals NE NULL)
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
        lpMemDfnHdr->offFcnLines = uOffset;

        // Save offset to tokenized lines
        lpMemDfnHdr->offTknLines = uOffset + numLogLines * sizeof (FCNLINE);

        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Initialize the offset of where to start saving the tokenized lines
        uOffset = lpMemDfnHdr->offTknLines;

        // Loop through the lines
        for (uLogLineNum = uPhyLineNum = 1; uPhyLineNum <= numPhyLines; uPhyLineNum++)
        {
            // If the preceding physical line is not continued to the current line, ...
            if (!SF_IsLineContMFO (hWndEC, lpMagicFcnOpr, uPhyLineNum - 1))
            {
                // Save a function line
                uLineLen =
                  SaveFunctionLine (NULL, lpMagicFcnOpr, lpMemDfnHdr, uLogLineNum, uPhyLineNum, lpFcnLines, hWndEC, hWndEC, &uOffset);
                if (uLineLen EQ -1)
                    goto ERROR_EXIT;
                // Count in another logical line
                uLogLineNum++;
            } // End IF

            // Transfer Stop & Trace info
            lpFcnLines->bStop  =
            lpFcnLines->bTrace = FALSE;

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
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"%s in %s -- Line %d statement %d position %d",
                        csLocalVars.lpwszErrMsg,
                        lpwszName,
                        csLocalVars.tkCSErr.tkData.Orig.c.uLineNum,
                        csLocalVars.tkCSErr.tkData.Orig.c.uStmtNum + 1,
                        csLocalVars.tkCSErr.tkCharIndex);
            MBW (wszTemp);

            goto ERROR_EXIT;
        } // End IF

        // Check for line labels ([]ID, etc.)
        if (!GetLabelNums (lpMemDfnHdr, NULL, TRUE, NULL))
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
    if (hGlbDfnHdr NE NULL)
    {
        FreeResultGlobalDfn (hGlbDfnHdr); hGlbDfnHdr = NULL;
    } // End IF

    if (hGlbTknHdr NE NULL)
    {
        LPTOKEN_HEADER lpMemTknHdr;

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);

        // Free the tokens
        Untokenize (lpMemTknHdr);

        // Unlock and free (and set to NULL) a global name and ptr
        UnlFreeGlbName (hGlbTknHdr, lpMemTknHdr);
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbTxtHdr, lpMemTxtLine);
NORMAL_EXIT:
    // Restore the ptr to the next token on the CS stack
    lpMemPTD->lptkCSNxt = lptkCSBeg;

    // If lpInitMFO is valid, ...
    if (lpInitMFO NE NULL)
    {
        // Restore the old HTS
        lpMemPTD->lphtsPTD = lphtsOld;

        // Delete address of previous struc
        lpInitMFO->lphtsMFO->lphtsPrvMFO = NULL;
    } // End IF

    // If we allocated virtual storage, ...
    if (lclMemVirtStr[0].IniAddr NE NULL)
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
    UBOOL    bRet = TRUE;               // TRUE iff the result is valid
    INIT_MFO initMFO;                   // Temporary struc for passing multiple args

    // Initialize the temp struc
////initMFO.lphtsMFO         = NULL;             // Set below
    initMFO.lpLclMemVirtStr  = lpLclMemVirtStr;
    initMFO.uPtdMemVirtStart = uPtdMemVirtStart;
    initMFO.uPtdMemVirtEnd   = uPtdMemVirtEnd;
    initMFO.uCount           = 0;

    // Define the magic functions/operators
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MonIota          ]  = Init1MagicFunction (MFON_MonIota          , &MFO_MonIota          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydIota          ]  = Init1MagicFunction (MFON_DydIota          , &MFO_DydIota          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MonRank          ]  = Init1MagicFunction (MFON_MonRank          , &MFO_MonRank          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydRank          ]  = Init1MagicFunction (MFON_DydRank          , &MFO_DydRank          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MonFMT           ]  = Init1MagicFunction (MFON_MonFMT           , &MFO_MonFMT           , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_BoxFMT           ]  = Init1MagicFunction (MFON_BoxFMT           , &MFO_BoxFMT           , lpMemPTD, hWndEC, NULL));
    initMFO.lphtsMFO = &lpMemPTD->ahtsMFO[HTS_MONVR];       // Set local HTS so []VR can use ##. to look up a name in the parent Sym & Hash Tables
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MonVR            ]  = Init1MagicFunction (MFON_MonVR            , &MFO_MonVR            , lpMemPTD, hWndEC, &initMFO));
////initMFO.lphtsMFO = NULL;
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_IdnDot           ]  = Init1MagicFunction (MFON_IdnDot           , &MFO_IdnDot           , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_IdnJotDot        ]  = Init1MagicFunction (MFON_IdnJotDot        , &MFO_IdnJotDot        , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MAD              ]  = Init1MagicFunction (MFON_MAD              , &MFO_MAD              , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MSD              ]  = Init1MagicFunction (MFON_MSD              , &MFO_MSD              , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MU               ]  = Init1MagicFunction (MFON_MU               , &MFO_MU               , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MI               ]  = Init1MagicFunction (MFON_MI               , &MFO_MI               , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MIO              ]  = Init1MagicFunction (MFON_MIO              , &MFO_MIO              , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MEO              ]  = Init1MagicFunction (MFON_MEO              , &MFO_MEO              , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MM               ]  = Init1MagicFunction (MFON_MM               , &MFO_MM               , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MLRS             ]  = Init1MagicFunction (MFON_MLRS             , &MFO_MLRS             , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MLRSU            ]  = Init1MagicFunction (MFON_MLRSU            , &MFO_MLRSU            , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MMUL             ]  = Init1MagicFunction (MFON_MMUL             , &MFO_MMUL             , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydDnShoe        ]  = Init1MagicFunction (MFON_DydDnShoe        , &MFO_DydDnShoe        , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydUpShoe        ]  = Init1MagicFunction (MFON_DydUpShoe        , &MFO_DydUpShoe        , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydLRShoeUnd     ]  = Init1MagicFunction (MFON_DydLRShoeUnd     , &MFO_DydLRShoeUnd     , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_SD               ]  = Init1MagicFunction (MFON_SD               , &MFO_SD               , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MonDomino        ]  = Init1MagicFunction (MFON_MonDomino        , &MFO_MonDomino        , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydDomino        ]  = Init1MagicFunction (MFON_DydDomino        , &MFO_DydDomino        , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydDotDot        ]  = Init1MagicFunction (MFON_DydDotDot        , &MFO_DydDotDot        , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydIotaUnderbar  ]  = Init1MagicFunction (MFON_DydIotaUnderbar  , &MFO_DydIotaUnderbar  , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MonDotInit       ]  = Init1MagicFunction (MFON_MonDotInit       , &MFO_MonDotInit       , lpMemPTD, hWndEC, NULL));

    // Run MFON_MonDotInit to initialize the Determinant Operator magic function subroutines
    // Note we must run this function AFTER MonDotInit is initialized and BEFORE MonDot is initialized
    //   so as to get the reference counts balanced.
    ExecNilMFO (lpMemPTD, hWndEC);

    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MonDot           ]  = Init1MagicFunction (MFON_MonDot           , &MFO_MonDot           , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydEpsUnderbar   ]  = Init1MagicFunction (MFON_DydEpsUnderbar   , &MFO_DydEpsUnderbar   , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydConv          ]  = Init1MagicFunction (MFON_DydConv          , &MFO_DydConv          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_IdnConv          ]  = Init1MagicFunction (MFON_IdnConv          , &MFO_IdnConv          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydScan          ]  = Init1MagicFunction (MFON_DydScan          , &MFO_DydScan          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydScan1         ]  = Init1MagicFunction (MFON_DydScan1         , &MFO_DydScan1         , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_RoS1L            ]  = Init1MagicFunction (MFON_RoS1L            , &MFO_RoS1L            , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_RoS1R            ]  = Init1MagicFunction (MFON_RoS1R            , &MFO_RoS1R            , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_RoS2             ]  = Init1MagicFunction (MFON_RoS2             , &MFO_RoS2             , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_RoS3             ]  = Init1MagicFunction (MFON_RoS3             , &MFO_RoS3             , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MDIU             ]  = Init1MagicFunction (MFON_MDIU             , &MFO_MDIU             , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DetSing          ]  = Init1MagicFunction (MFON_DetSing          , &MFO_DetSing          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydVOFact        ]  = Init1MagicFunction (MFON_DydVOFact        , &MFO_DydVOFact        , lpMemPTD, hWndEC, NULL));
    initMFO.lphtsMFO = &lpMemPTD->ahtsMFO[HTS_MONEXECUTE];  // Set local HTS to execute the expression using the outer HTS
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MonExecute       ]  = Init1MagicFunction (MFON_MonExecute       , &MFO_MonExecute       , lpMemPTD, hWndEC, &initMFO));
    initMFO.lphtsMFO = &lpMemPTD->ahtsMFO[HTS_DYDSQUAD];    // Set local HTS so Squad can store the hGlbDfnHdr for its local AFOs.
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydSquad         ]  = Init1MagicFunction (MFON_DydSquad         , &MFO_DydSquad         , lpMemPTD, hWndEC, &initMFO));
////initMFO.lphtsMFO = NULL;
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydSlope         ]  = Init1MagicFunction (MFON_DydSlope         , &MFO_DydSlope         , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MonShriek        ]  = Init1MagicFunction (MFON_MonShriek        , &MFO_MonShriek        , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MatOpr           ]  = Init1MagicFunction (MFON_MatOpr           , &MFO_MatOpr           , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DetPerm          ]  = Init1MagicFunction (MFON_DetPerm          , &MFO_DetPerm          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydDot2          ]  = Init1MagicFunction (MFON_DydDot2          , &MFO_DydDot2          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydMask          ]  = Init1MagicFunction (MFON_DydMask          , &MFO_DydMask          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydMesh          ]  = Init1MagicFunction (MFON_DydMesh          , &MFO_DydMesh          , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_MonDomino2       ]  = Init1MagicFunction (MFON_MonDomino2       , &MFO_MonDomino2       , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_DydDomino2       ]  = Init1MagicFunction (MFON_DydDomino2       , &MFO_DydDomino2       , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_InvPJDRed        ]  = Init1MagicFunction (MFON_InvPJDRed        , &MFO_InvPJDRed        , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_InvBV            ]  = Init1MagicFunction (MFON_InvBV            , &MFO_InvBV            , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_IdnJot           ]  = Init1MagicFunction (MFON_IdnJot           , &MFO_IdnJot           , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_InvBS            ]  = Init1MagicFunction (MFON_InvBS            , &MFO_InvBS            , lpMemPTD, hWndEC, NULL));
    bRet &= NULL NE (lpMemPTD->hGlbMFO[MFOE_InvCBS           ]  = Init1MagicFunction (MFON_InvCBS           , &MFO_InvCBS           , lpMemPTD, hWndEC, NULL));

    //***************************************************************
    //  N.B.:  If you define an additional MFO with a local
    //    HSHTAB & SYMTAB, you *MUST* define a corresponding entry
    //    in the enum <PTDMEMVIRT_ENUM>.
    //***************************************************************

    return bRet;
} // InitMagicFunctions


//***************************************************************************
//  $ExecNilMFO
//
//  Execute and erase a niladic MFO (MFON_MonDotInit)
//***************************************************************************

void ExecNilMFO
    (LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     HWND         hWndEC)               // Session Manager window handle

{
    HGLOBAL        hGlbTknHdr;          // Handle of tokenized line header
    LPSYMENTRY     lpSymEntry;          // Ptr to function STE
    LPTOKEN_HEADER lpMemTknHdr;         // Ptr to tokenized line header global memory
    STFLAGS        stFlags = {0};       // SymTab flags
    LPDFN_HEADER   lpMemDfnHdr;         // Ptr to user-defined function/operator header global memory

    // Tokenize the line
    hGlbTknHdr =
      Tokenize_EM (MFON_MonDotInit,             // The line to tokenize (not necessarily zero-terminated)
                   lstrlenW (MFON_MonDotInit),  // NELM of lpwszLine
                   hWndEC,                      // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                   1,                           // Logical function line # (0 = header)
                   NULL,                        // Ptr to error handling function (may be NULL)
                   NULL,                        // Ptr to common struc (may be NULL if unused)
                   TRUE);                       // TRUE iff we're tokenizing a Magic Function/Operator
    // Lock the memory to get a ptr to it
    lpMemTknHdr = MyGlobalLockTkn (hGlbTknHdr);

    // Execute the line
////exitType =
      ParseLine (GetParent (hWndEC),    // Session Manager window handle
                 lpMemTknHdr,           // Ptr to tokenized line header global memory
                 NULL,                  // Text of tokenized line global mamory handle
                 MFON_MonDotInit,       // Ptr to the complete line
                 lpMemPTD,              // Ptr to PerTabData global memory
                 1,                     // Function line # (1 for execute or immexec) (origin-1)
                 0,                     // Starting token # in the above function line (origin-0)
                 FALSE,                 // TRUE iff we're tracing this line
                 NULL,                  // User-defined function/operator global memory handle (NULL = execute/immexec)
                 NULL,                  // Ptr to function token used for AFO function name
                 FALSE,                 // TRUE iff errors are acted upon
                 FALSE,                 // TRUE iff executing only one stmt
                 FALSE);                // TRUE iff we're to skip the depth check
    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbTknHdr, lpMemTknHdr);

    // Tell 'em we're looking for MFO objects
////ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_MFO;

    // Lookup the function name
    lpSymEntry =
      SymTabLookupName (MFON_MonDotInit, &stFlags);

    Assert (lpSymEntry);

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLockDfn (lpSymEntry->stData.stGlbData);

    // Free the globals in the struc
    //   but don't Untokenize the function lines
    FreeResultGlobalDfnStruc (lpMemDfnHdr, FALSE);

    // Free the HshTab & SymTab
    FreeHshSymTabs (&lpMemDfnHdr->htsDFN, FALSE);

    // We no longer need this ptr
    MyGlobalUnlock (lpSymEntry->stData.stGlbData); lpMemDfnHdr = NULL;

    // We no longer need this storage
    DbgGlobalFree (lpSymEntry->stData.stGlbData); lpSymEntry->stData.stGlbData = NULL;

    // Erase the Symbol Table Entry
    EraseSTE (lpSymEntry, FALSE); lpSymEntry = NULL;
} // End ExecNilMFO


//***************************************************************************
//  End of File: execmfn.c
//***************************************************************************
