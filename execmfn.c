//***************************************************************************
//  NARS2000 -- Magic Function Execution Routines
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

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "sysvars.h"
#include "externs.h"
#include "pertab.h"
#include "dfnhdr.h"
#include "sis.h"
#include "execmfn.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

extern MAGIC_FUNCTION MF_MonIota;
extern MAGIC_FUNCTION MF_DydIota;
extern MAGIC_FUNCTION MF_MonUpShoe;
extern MAGIC_FUNCTION MF_DydTilde;
extern MAGIC_FUNCTION MF_MonRank;
extern MAGIC_FUNCTION MF_DydRank;
////rn MAGIC_FUNCTION MF_MonRankAxis;
////rn MAGIC_FUNCTION MF_DydRankAxis;
extern MAGIC_FUNCTION MF_Conform;


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
     HGLOBAL      hGlbMF,               // Magic function global memory handle
     LINE_NUMS    lineNum)              // Starting line # type (see LINE_NUMS)

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to result

    lpYYRes =
      ExecDfnGlb_EM_YY (hGlbMF,         // Magic function global memory handle
                        lptkLftArg,     // Ptr to left arg token
                        lpYYFcnStrOpr,  // Ptr to operator function strand
                        lptkAxis,       // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                        lptkRhtArg,     // Ptr to right arg token
                        lineNum);       // Starting line # type (see LINE_NUMS)
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
     HGLOBAL      hGlbMF,               // Magic function global memory handle
     LINE_NUMS    lineNum)              // Starting line # type (see LINE_NUMS)

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to result

    lpYYRes =
      ExecDfnOprGlb_EM_YY (hGlbMF,          // Magic function global memory handle
                           lptkLftArg,      // Ptr to left arg token
                           lpYYFcnStrLft,   // Ptr to left operand function strand
                           lpYYFcnStrOpr,   // Ptr to operator function strand
                           lpYYFcnStrRht,   // Ptr to right operand function strand
                           lptkAxis,        // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                           lptkRhtArg,      // Ptr to right arg token
                           lineNum);        // Starting line # type (see LINE_NUMS)
    // If there was an error, set the caret to
    //   point to the primitive function
    if (lpYYRes EQ NULL)
        ErrorMessageSetToken (lptkFunc);

    return lpYYRes;
} // End ExecuteMagicOperator_EM_YY


//***************************************************************************
//  $Init1MagicFunction
//
//  Initialize a single magic function
//***************************************************************************

HGLOBAL Init1MagicFunction
    (LPWCHAR          lpwszName,        // Ptr to the external name
     LPMAGIC_FUNCTION lpMagicFunction,  // Ptr to magic function struc
     LPPERTABDATA     lpMemPTD,         // Ptr to PerTabData global memory
     HWND             hWndEC)           // Edit Control window handle

{
    UINT           uLineLen;            // Line length var
    HGLOBAL        hGlbTxtHdr = NULL,   // Header text global memory handle
                   hGlbTknHdr = NULL,   // Tokenized header text ...
                   hGlbDfnHdr = NULL;   // User-defined function/operator header ...
    LPMEMTXT_UNION lpMemTxtLine;        // Ptr to header/line text global memory
    FHLOCALVARS    fhLocalVars = {0};   // Re-entrant vars
    STFLAGS        stFlags = {0};       // STE flags for the MF
    LPSYMENTRY     lpSymEntry;          // Ptr to SYMENTRY for the MF
    MEMVIRTSTR     lclMemVirtStr[1] = {0}; // Room for one GuardAlloc

    // Throughout this function, we use GlobalAlloc
    //   instead of MyGlobalAlloc because we never intend to
    //   free the global memory

    // Get the length of the header line
    uLineLen  = lstrlenW (lpMagicFunction->Header);

    // Allocate space for the text
    //   (the "sizeof (uLineLen)" is for the leading line length
    //    and the " + 1" is for the terminating zero)
    hGlbTxtHdr = GlobalAlloc (GHND, sizeof (lpMemTxtLine->U) + (uLineLen + 1) * sizeof (lpMemTxtLine->C));
    if (!hGlbTxtHdr)
    {
        MessageBox (hWndEC,
                    "Insufficient memory to save the magic function header text!!",
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
        CopyMemory (&lpMemTxtLine->C, lpMagicFunction->Header, uLineLen * sizeof (APLCHAR));

        // Tokenize the line
        hGlbTknHdr =
          Tokenize_EM (&lpMemTxtLine->C,
                        uLineLen,
                        hWndEC,
                       &ErrorHandler);
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtHdr); lpMemTxtLine = NULL;
    } else
        // Tokenize the (empty) line
        hGlbTknHdr =
          Tokenize_EM (L"",
                       0,
                       NULL,
                       NULL);
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
#ifdef DEBUG
    lclMemVirtStr[0].lpText   = "fhLocalVars.lpYYStrandStart in <Init1MagicFunction>";
#endif
    lclMemVirtStr[0].IncrSize = DEF_STRAND_INCRSIZE * sizeof (PL_YYSTYPE);
    lclMemVirtStr[0].MaxSize  = DEF_STRAND_MAXSIZE  * sizeof (PL_YYSTYPE);
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
                    DEF_STRAND_INITSIZE * sizeof (PL_YYSTYPE),
                    MEM_COMMIT,
                    PAGE_READWRITE);
    // Parse the header
    if (ParseHeader (hWndEC, hGlbTknHdr, &fhLocalVars, TRUE))
    {
        UINT         uLineNum,          // Current line # in the Edit Control
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
        numFcnLines = lpMagicFunction->numFcnLines;

        // Allocate global memory for the function header
        hGlbDfnHdr = GlobalAlloc (GHND, sizeof (DFN_HEADER)
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
        lpMemDfnHdr->hGlbUndoBuff = NULL;

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
            uLineLen = lstrlenW (lpMagicFunction->Body[uLineNum]);

            // Allocate global memory to hold this text
            // The "sizeof (uLineLen) + " is for the leading length
            //   and the "+ 1" is for the terminating zero
            //   as well as to handle GlobalLock's aversion to locking
            //   zero-length arrays
            hGlbTxtLine = GlobalAlloc (GHND, sizeof (uLineLen) + (uLineLen + 1) * sizeof (APLCHAR));
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
                CopyMemory (&lpMemTxtLine->C, lpMagicFunction->Body[uLineNum], uLineLen * sizeof (APLCHAR));

                // Tokenize the line
                lpFcnLines->hGlbTknLine =
                  Tokenize_EM (&lpMemTxtLine->C,
                                uLineLen,
                                hWndEC,
                               &ErrorHandler);
            } else
                // Tokenize the (empty) line
                lpFcnLines->hGlbTknLine =
                  Tokenize_EM (L"",
                               0,
                               hWndEC,
                              &ErrorHandler);
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

            // No monitor info as yet
            lpFcnLines->hGlbMonInfo = NULL;

            // Transfer Stop & Trace info
            lpFcnLines->bStop  =
            lpFcnLines->bTrace = FALSE;     // ***FIXME*** -- transfer from orig fn

            // Skip to the next struct
            lpFcnLines++;
        } // End FOR

        // Check for special labels ([]IDENTITY, []INVERSE, []PROTOTYPE, and []SINGLETON)
        GetSpecialLabelNums (lpMemDfnHdr);

        // Set the flags for what we're appending
        stFlags.Perm    =
        stFlags.Inuse   =
        stFlags.Value   = TRUE;
        stFlags.ObjName = OBJNAME_MF;
        stFlags.UsrDfn  = TRUE;
        stFlags.DfnAxis = lpMemDfnHdr->DfnAxis;

        // Create a symbol table entry for the Magic Function
        lpSymEntry =
          SymTabAppendNewName_EM (lpwszName, &stFlags);

        if (!lpSymEntry)
            goto ERROR_EXIT;

        // Set the handle
        lpSymEntry->stData.stGlbData = MakePtrTypeGlb (hGlbDfnHdr);

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
//  Initialize all magic functions
//***************************************************************************

BOOL InitMagicFunctions
    (HGLOBAL hGlbPTD,           // PerTabData global memory handle
     HWND    hWndEC)            // Session Manager window handle

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Define the magic functions
    lpMemPTD->hGlbMF_MonIota   = Init1MagicFunction (L"#MonIota"  , &MF_MonIota  , lpMemPTD, hWndEC);
    lpMemPTD->hGlbMF_DydIota   = Init1MagicFunction (L"#DydIota"  , &MF_DydIota  , lpMemPTD, hWndEC);
    lpMemPTD->hGlbMF_MonUpShoe = Init1MagicFunction (L"#MonUpShoe", &MF_MonUpShoe, lpMemPTD, hWndEC);
    lpMemPTD->hGlbMF_DydTilde  = Init1MagicFunction (L"#DydTilde" , &MF_DydTilde , lpMemPTD, hWndEC);
    lpMemPTD->hGlbMF_MonRank   = Init1MagicFunction (L"#MonRank"  , &MF_MonRank  , lpMemPTD, hWndEC);
    lpMemPTD->hGlbMF_DydRank   = Init1MagicFunction (L"#DydRank"  , &MF_DydRank  , lpMemPTD, hWndEC);
    lpMemPTD->hGlbMF_Conform   = Init1MagicFunction (L"#Conform"  , &MF_Conform  , lpMemPTD, hWndEC);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return TRUE;
} // InitMagicFunctions


//***************************************************************************
//  End of File: execmfn.c
//***************************************************************************
