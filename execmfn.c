//***************************************************************************
//  NARS2000 -- Magic Function Execution Routines
//***************************************************************************

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


//***************************************************************************
//  $ExecuteMagicFunction_EM_YY
//
//  Execute a magic function
//***************************************************************************

LPPL_YYSTYPE ExecuteMagicFunction_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis,                  // Ptr to axis token (may be NULL)
     HGLOBAL hGlbMagicFcn)              // Magic function global memory handle

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to result

    // ***FIXME*** -- Handle incoming axis

    lpYYRes =
      ExecDfnGlb_EM_YY (hGlbMagicFcn,   // Magic function global memory handle
                        lptkLftArg,     // Ptr to left arg token
                        NULL,           // Ptr to function strand
                        lptkRhtArg,     // Ptr to right arg token
                        LINENUM_ONE);   // Starting line # (see LINE_NUMS)
    // If there was an error, set the caret to
    //   point to the primitive function
    if (lpYYRes EQ NULL)
        ErrorMessageSetToken (lptkFunc);

    return lpYYRes;
} // End ExecuteMagicFunction_EM_YY


//***************************************************************************
//  $InitMagicFunction
//
//  Initialize a single magic function
//***************************************************************************

HGLOBAL InitMagicFunction
    (LPMAGIC_FUNCTION lpMagicFunction,  // Ptr to magic function struc
     LPPERTABDATA     lpMemPTD,         // Ptr to PerTabData global memory
     HWND             hWndEC)           // Edit Control window handle

{
    UINT           uLineLen;            // Line length var
    HGLOBAL        hGlbTxtHdr = NULL,   // Header text global memory handle
                   hGlbTknHdr = NULL,   // Tokenized header text ...
                   hGlbDfnHdr = NULL;   // User-defined function/operator header ...
    LPMEMTXT_UNION lpMemTxtLine;        // Ptr to header/line text global memory
    FHLOCALVARS    fhLocalVars = {0};   // Re-entrant vars

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
        hGlbTknHdr = Tokenize_EM (&lpMemTxtLine->C,
                                   uLineLen,
                                   hWndEC,
                                  &ErrorHandler);
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtHdr); lpMemTxtLine = NULL;
    } else
        // Tokenize the (empty) line
        hGlbTknHdr = Tokenize_EM (L"",
                                  0,
                                  hWndEC,
                                  NULL);
    if (!hGlbTknHdr)
    {
        WCHAR wszTemp[1024];

        // Format the error message
        wsprintfW (wszTemp,
                   L"SYNTAX ERROR in function header, position %d -- function not saved",
                   lpMemPTD->uCaret);
        // Display the error message
        MessageBoxW (hWndEC,
                    wszTemp,
                    lpwszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        goto ERROR_EXIT;
    } // End IF

    // Allocate virtual memory for the Variable Strand accumulator
    fhLocalVars.lpYYStrandStart =
        VirtualAlloc (NULL,      // Any address
                      DEF_STRAND_MAXSIZE * sizeof (PL_YYSTYPE),
                      MEM_RESERVE,
                      PAGE_READWRITE);
    if (!fhLocalVars.lpYYStrandStart)
    {
        MessageBox (hWndEC,
                    "Insufficient memory to save the function header strand stack!!",
                    lpszAppName,
                    MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        goto ERROR_EXIT;    // Mark as failed
    } // End IF

    // Commit the intial size
    VirtualAlloc (fhLocalVars.lpYYStrandStart,
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
        lpMemDfnHdr->Perm         = TRUE;
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
                lpFcnLines->hGlbTknLine = Tokenize_EM (&lpMemTxtLine->C,
                                                        uLineLen,
                                                        hWndEC,
                                                       &ErrorHandler);
            } else
                // Tokenize the (empty) line
                lpFcnLines->hGlbTknLine = Tokenize_EM (L"",
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

        // Check for special labels ([]PROTOTYPE, []INVERSE, and []SINGLETON)
        GetSpecialLabelNums (lpMemDfnHdr);

        // ***FIXME*** -- Do we need a Symbol Table Entry??






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
    // Free the virtual memory we allocated above
    if (fhLocalVars.lpYYStrandStart)
    {
        VirtualFree (fhLocalVars.lpYYStrandStart, 0, MEM_RELEASE); fhLocalVars.lpYYStrandStart = NULL;
    } // End IF

    return hGlbDfnHdr;
} // End InitMagicFunction


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
    lpMemPTD->hGlbMF_MonIota   = InitMagicFunction (&MF_MonIota,   lpMemPTD, hWndEC);
    lpMemPTD->hGlbMF_DydIota   = InitMagicFunction (&MF_DydIota,   lpMemPTD, hWndEC);
    lpMemPTD->hGlbMF_MonUpShoe = InitMagicFunction (&MF_MonUpShoe, lpMemPTD, hWndEC);
    lpMemPTD->hGlbMF_DydTilde  = InitMagicFunction (&MF_DydTilde,  lpMemPTD, hWndEC);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return TRUE;
} // InitMagicFunctions


//***************************************************************************
//  End of File: execmfn.c
//***************************************************************************
