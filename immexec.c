//***************************************************************************
//  NARS2000 -- Immediate Execution
//***************************************************************************

#define STRICT
#define _WIN32_WINNT   0x0500   // Needed for RegisterWaitForSingleObject
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "dfnhdr.h"
#include "sis.h"
#include "threads.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


typedef struct tagWFSO      // Struct for WaitForSingleObject
{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    HANDLE       WaitHandle,    // WaitHandle from RegisterWaitForSingleObject
                 hSigaphore,    // Handle to signal next (may be NULL)
                 hThread;       // Thread handle
} WFSO, *LPWFSO;

IE_THREAD ieThread;             // Temporary global


//***************************************************************************
//  $WaitForImmExecStmt
//
//  Wait callback for ImmExecStmt
//***************************************************************************

VOID CALLBACK WaitForImmExecStmt
    (LPVOID  lpParameter,           // Thread data
     BOOLEAN TimerOrWaitFired)      // Reason

{
    LPWFSO       lpMemWFSO;         // Ptr to WFSO global memory
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    EXIT_TYPES   exitType;          // Exit type (see EXIT_TYPES)
#ifdef DEBUG
    DWORD        dwRet;             // Return code from UnregisterWait
#endif

#define hGlbWFSO    ((HGLOBAL) lpParameter)

    // Lock the memory to get a ptr to it
    lpMemWFSO = MyGlobalLock (hGlbWFSO);

    // Save the thread's PerTabData global memory handle
    TlsSetValue (dwTlsPerTabData, lpMemWFSO->hGlbPTD);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (lpMemWFSO->hGlbPTD);

    // Get the thread's exit code
    GetExitCodeThread (lpMemWFSO->hThread, &exitType);

    // Save in global memory
    lpMemPTD->ImmExecExitType = exitType;

    // We no longer need this ptr
    MyGlobalUnlock (lpMemWFSO->hGlbPTD); lpMemPTD = NULL;

#ifdef DEBUG
    dprintfW (L"~~WaitForImmExecStmt (%08X)", lpMemWFSO->hThread);
#endif
    // Cancel the wait operation
#ifdef DEBUG
    dwRet =
#endif
      UnregisterWait (lpMemWFSO->WaitHandle); lpMemWFSO->WaitHandle = NULL;
    Assert (dwRet EQ 0 || dwRet EQ ERROR_IO_PENDING);

    // Signal the next level (if appropriate)
    if (lpMemWFSO->hSigaphore)
        ReleaseSemaphore (lpMemWFSO->hSigaphore, 1, NULL);
    // We no longer need this ptr
    MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;

    // Free the allocated memory
    DbgGlobalFree (hGlbWFSO); hGlbWFSO = NULL;
#undef  hGlbWFSO
} // End WaitForImmExecStmt


//***************************************************************************
//  $ImmExecLine
//
//  Execute a line (sys/user command, fn defn, etc.)
//    in immediate execution mode
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ImmExecLine"
#else
#define APPEND_NAME
#endif

void ImmExecLine
    (UINT uLineNum,             // Line #
     HWND hWndEC)               // Handle of Edit Control window

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPWCHAR      lpwszCompLine, // Ptr to complete line
                 lpwszLine;     // Ptr to line following leading blanks
    UINT         uLinePos,      // Char position of start of line
                 uLineLen;      // Line length

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Get the position of the start of the line
    uLinePos = SendMessageW (hWndEC, EM_LINEINDEX, uLineNum, 0);

    // Get the line length
    uLineLen = SendMessageW (hWndEC, EM_LINELENGTH, uLinePos, 0);

    // Allocate virtual memory for the line (along with its continuations)
    lpwszCompLine = VirtualAlloc (NULL,             // Any address
                                  (uLineLen + 1) * sizeof (WCHAR),  // "+ 1" for the terminating zero
                                  MEM_COMMIT,
                                  PAGE_READWRITE);
    if (!lpwszCompLine)
    {
        // ***FIXME*** -- WS FULL before we got started???
        DbgMsg ("ImmExecLine:  VirtualAlloc for <lpwszCompLine> failed");

        return;                 // Mark as failed
    } // End IF

    // Tell EM_GETLINE maximum # chars in the buffer
    // Because we allocated space for the terminating zero,
    //   we don't have to worry about overwriting the
    //   allocation limits of the buffer
    ((LPWORD) lpwszCompLine)[0] = (WORD) uLineLen;

    // Get the contents of the line
    SendMessageW (hWndEC, EM_GETLINE, uLineNum, (LPARAM) lpwszCompLine);

    // Ensure properly terminated
    lpwszCompLine[uLineLen] = L'\0';

    // Strip off leading blanks
    for (lpwszLine = lpwszCompLine;
         lpwszLine[0] && lpwszLine[0] EQ L' ';
         lpwszLine++)
    {};

    // Split cases based upon the first non-blank char
    switch (lpwszLine[0])
    {
        case L')':          // System commands
            // Execute the command (ignore the result)
            ExecSysCmd (lpwszLine);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // If it's Quad input, and we're not resetting, ...
            if (lpMemPTD->lpSISCur
             && lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_NONE
             && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                // Tell the SM to display the Quad Input Prompt
                PostMessage (lpMemPTD->hWndSM, MYWM_QUOTEQUAD, FALSE, 10);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            break;

        case L']':          // User commands
#if (defined (DEBUG)) && (defined (EXEC_TRACE))
            // ***FIXME***
            DbgMsg ("User command");
#endif
            AppendLine (ERRMSG_NONCE_ERROR, FALSE, TRUE);

            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // If it's Quad input, ...
            if (lpMemPTD->lpSISCur
             && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                // Tell the SM to display the Quad Input Prompt
                PostMessage (lpMemPTD->hWndSM, MYWM_QUOTEQUAD, FALSE, 11);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            break;

        case UTF16_DEL:     // Function definition
            // Create the Function Editor Window (ignore the result)
            CreateFcnWindow (lpwszLine);

            // Fall through to empty line case

        case L'\0':         // Empty line
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // If it's Quad input, ...
            if (lpMemPTD->lpSISCur
             && lpMemPTD->lpSISCur->DfnType EQ DFNTYPE_QUAD)
                // Tell the SM to display the Quad Input Prompt
                PostMessage (lpMemPTD->hWndSM, MYWM_QUOTEQUAD, FALSE, 12);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

            break;

        default:
            // Execute the statement
            ImmExecStmt (lpwszCompLine,     // Ptr to line to execute
                         TRUE,              // Free lpwszCompLine on completion
                         FALSE,             // TRUE iff wait until finished
                         hWndEC);           // Edit Control window handle
            return;
    } // End SWITCH

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If no SIS layer or not Quad input and not reset all, ...
    if (lpMemPTD->lpSISCur EQ NULL
     || (lpMemPTD->lpSISCur->DfnType NE DFNTYPE_QUAD
      && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_ALL))
        // Display the default prompt
        DisplayPrompt (hWndEC, 4);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Free the virtual memory for the complete line
    VirtualFree (lpwszCompLine, 0, MEM_RELEASE); lpwszCompLine = NULL;
} // End ImmExecLine
#undef  APPEND_NAME


//***************************************************************************
//  $ImmExecStmt
//
//  Execute a statement
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ImmExecStmt"
#else
#define APPEND_NAME
#endif

EXIT_TYPES ImmExecStmt
    (LPWCHAR lpwszCompLine, // Ptr to line to execute
     BOOL    bFreeLine,     // TRUE iff free lpwszCompLine on completion
     BOOL    bWaitUntilFini,// TRUE iff wait until finished
     HWND    hWndEC)        // Edit Control window handle

{
    HANDLE     hThread;     // Thread handle
    DWORD      dwThreadId;  // The thread ID #
    LPWFSO     lpMemWFSO;   // Ptr to WFSO global memory
    EXIT_TYPES exitType;    // Exit code from thread

    // Save args in struc to pass to thread func
    ieThread.hWndEC         = hWndEC;
    ieThread.hGlbPTD        = TlsGetValue (dwTlsPerTabData); Assert (ieThread.hGlbPTD NE NULL);
    ieThread.lpwszCompLine  = lpwszCompLine;
    ieThread.hGlbWFSO       = DbgGlobalAlloc (GHND, sizeof (WFSO));
    ieThread.bFreeLine      = bFreeLine;
    ieThread.bWaitUntilFini = bWaitUntilFini;

    // Lock the memory to get a ptr to it
    lpMemWFSO = MyGlobalLock (ieThread.hGlbWFSO);

    // Fill in the struct
    lpMemWFSO->hGlbPTD = ieThread.hGlbPTD;

    // Create a new thread
    hThread = CreateThread (NULL,                   // No security attrs
                            0,                      // Use default stack size
                           &ImmExecStmtInThread,    // Starting routine
                           &ieThread,               // Param to thread func
                            CREATE_SUSPENDED,       // Creation flag
                           &dwThreadId);            // Returns thread id
#ifdef DEBUG
    // Save the thread handle
    lpMemWFSO->hThread = hThread;
#endif
    // Should we wait until finished?
    if (bWaitUntilFini)
    {
        // Start 'er up
        ResumeThread (hThread);
#ifdef DEBUG
        dprintfW (L"~~WaitForSingleObject (ENTRY):  %08X -- %s (%S#%d)", hThread, L"ImmExecStmt", FNLN);
#endif
        // Wait until this thread terminates
        WaitForSingleObject (hThread,              // Handle to wait on
                             INFINITE);            // Wait time in milliseconds
#ifdef DEBUG
        dprintfW (L"~~WaitForSingleObject (EXIT):   %08X -- %s (%S#%d)", hThread, L"ImmExecStmt", FNLN);
#endif
        // Get the exit code
        GetExitCodeThread (hThread, &exitType);
    } else
    {
#ifdef DEBUG
        dprintfW (L"~~RegisterWaitForSingleObject (%08X) (%S#%d)", hThread, FNLN);
#endif
        // Tell W to callback when this thread terminates
        RegisterWaitForSingleObject (&lpMemWFSO->WaitHandle,// Return wait handle
                                      hThread,              // Handle to wait on
                                     &WaitForImmExecStmt,   // Callback function
                                      ieThread.hGlbWFSO,    // Callback function parameter
                                      INFINITE,             // Wait time in milliseconds
                                      WT_EXECUTEONLYONCE);  // Options
        // We no longer need this ptr
        MyGlobalUnlock (ieThread.hGlbWFSO); lpMemWFSO = NULL;

        // Start 'er up
        ResumeThread (hThread);

        exitType = EXITTYPE_NONE;
    } // End IF/ELSE

    return exitType;
} // End ImmExecStmt
#undef  APPEND_NAME


//***************************************************************************
//  $ImmExecStmtInThread
//
//  Execute a line (sys/user command, fn defn, etc.)
//    in immediate execution mode
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ImmExecStmtInThread"
#else
#define APPEND_NAME
#endif

DWORD WINAPI ImmExecStmtInThread
    (LPIE_THREAD lpieThread)            // Ptr to IE_THREAD struc

{
    HANDLE        hSigaphore = NULL;    // Semaphore handle to signal (NULL if none)
    LPWCHAR       lpwszCompLine;        // Ptr to complete line
    HGLOBAL       hGlbToken,            // Handle of tokenized line
                  hGlbWFSO;             // WaitForSingleObject callback global memory handle
    HWND          hWndEC,               // Handle of Edit Control window
                  hWndSM;               // ...       Session Manager ...
    HGLOBAL       hGlbPTD;              // Handle to this window's PerTabData
    LPPERTABDATA  lpMemPTD;             // Ptr to ...
    RESET_FLAGS   resetFlag;            // Reset flag (see RESET_FLAGS)
    BOOL          bFreeLine,            // TRUE iff we should free lpszCompLine on completion
                  bWaitUntilFini;       // TRUE iff wait until finished
    EXIT_TYPES    exitType;             // Return code from ParseLine
    LPWFSO        lpMemWFSO;            // Ptr to WFSO global memory
    LPSIS_HEADER  lpSISPrv;             // Ptr to previous SIS header

#ifndef DEBUG
    __try
    {
#endif
        // Save the thread type ('IE')
        TlsSetValue (dwTlsType, (LPVOID) 'IE');

        // Extract values from the arg struc
        hWndEC         = lpieThread->hWndEC;
        hGlbPTD        = lpieThread->hGlbPTD;
        lpwszCompLine  = lpieThread->lpwszCompLine;
        hGlbWFSO       = lpieThread->hGlbWFSO;
        bFreeLine      = lpieThread->bFreeLine;
        bWaitUntilFini = lpieThread->bWaitUntilFini;

        // Save the thread's PerTabData global memory handle
        TlsSetValue (dwTlsPerTabData, hGlbPTD);
#ifdef DEBUG
        dprintfW (L"--Starting thread in <ImmExecStmtInThread>.");
#endif
        // Get the window handle of the Session Manager
        hWndSM = GetParent (hWndEC);

        // Initialize the Reset Flag
        resetFlag = RESETFLAG_NONE;

        // Tokenize, parse, and untokenize the line

        // Tokenize the line
        hGlbToken = Tokenize_EM (lpwszCompLine,
                                 lstrlenW (lpwszCompLine),
                                 hWndEC,
                                &ErrorMessageDirect);
        // If it's invalid, ...
        if (hGlbToken EQ NULL)
        {
            // Execute the statement in immediate execution mode
            exitType =
              ImmExecStmt (WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"ELX", // Ptr to line to execute
                           FALSE,               // TRUE iff free the lpwszLine on completion
                           TRUE,                // TRUE iff wait until finished
                           (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC)); // Edit Control window handle
            // Split cases based upon the exit type
            switch (exitType)
            {
                case EXITTYPE_GOTO_LINE:        // Pass on to caller
                case EXITTYPE_RESET_ALL:        // ...
                case EXITTYPE_RESET_ONE:        // ...
                case EXITTYPE_RESET_ONE_INIT:   // ...
                    break;

                case EXITTYPE_ERROR:            // Mark as in error (from the error in Tokenize_EM)
                case EXITTYPE_DISPLAY:          // ...
                case EXITTYPE_NODISPLAY:        // ...
                case EXITTYPE_NOVALUE:          // ...
                case EXITTYPE_GOTO_ZILDE:       // ...
                    exitType = EXITTYPE_ERROR;

                    break;

                case EXITTYPE_NONE:
                defstop
                    break;
            } // End SWITCH
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Fill in the SIS header for Immediate Execution Mode
        FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                    NULL,                   // Semaphore handle
                    DFNTYPE_IMM,            // DfnType
                    FCNVALENCE_IMM,         // FcnValence
                    FALSE,                  // Suspended
                    TRUE);                  // LinkIntoChain
        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

        // Run the parser in a separate thread
        exitType =
          ParseLine (hWndSM,                // Session Manager window handle
                     NULL,                  // Line text global memory handle
                     hGlbToken,             // Tokenized line global memory handle
                     lpwszCompLine,         // Ptr to the complete line
                     hGlbPTD);              // PerTabData global memory handle
        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Start with the preceding layer (if any)
        lpSISPrv = lpMemPTD->lpSISCur->lpSISPrv;

        // Split cases based upon the exit type
        switch (exitType)
        {
            case EXITTYPE_QUADERROR_INIT:
                // If there are no more SI layers, ...
                if (lpSISPrv EQ NULL)
                {
                    // ***FIXME*** -- Set the tkCharIndex for the error???

                    // Set the error message
                    ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                                        lpwszCompLine,                  // Ptr to the line which generated the error
                                        lpMemPTD->tkErrorCharIndex,     // Position of caret (origin-0)
                                        hWndSM);                        // Window handle to the Session Manager
                    // Execute []ELX in immediate execution mode
////////////////////exitType =
                      ImmExecStmt (WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"ELX", // Ptr to line to execute
                                   FALSE,           // TRUE iff free the lpwszLine on completion
                                   TRUE,            // TRUE iff wait until finished
                                   (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC));// Edit Control window handle
                    // Set the reset flag
                    lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                    exitType = EXITTYPE_NONE;

                    break;
                } // End IF

                // If the previous layer in the SI stack is a
                //   User-Defined Function/Operator or Quad Input,
                //   signal it to handle this action
                if (lpSISPrv
                 && (lpSISPrv->DfnType EQ DFNTYPE_OP1
                  || lpSISPrv->DfnType EQ DFNTYPE_OP2
                  || lpSISPrv->DfnType EQ DFNTYPE_FCN
                  || lpSISPrv->DfnType EQ DFNTYPE_QUAD))
                {
                    // Lock the memory to get a ptr to it
                    lpMemWFSO = MyGlobalLock (hGlbWFSO);

                    Assert (lpSISPrv->hSemaphore NE NULL);

                    // Tell the wait handler to signal this layer
                    hSigaphore = lpMemWFSO->hSigaphore = lpSISPrv->hSemaphore;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;
                } // End IF

                break;

            case EXITTYPE_QUADERROR_EXEC:
                // Execute []ELX in immediate execution mode
////////////////exitType =
                  ImmExecStmt (WS_UTF16_UPTACKJOT WS_UTF16_QUAD L"ELX", // Ptr to line to execute
                               FALSE,           // TRUE iff free the lpwszLine on completion
                               TRUE,            // TRUE iff wait until finished
                               (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC));// Edit Control window handle
                // Set the reset flag
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                exitType = EXITTYPE_NONE;

                break;

            case EXITTYPE_ERROR:
                // If from Quad Input, tell SM to redisplay the prompt
                if (lpSISPrv
                 &&lpSISPrv->DfnType EQ DFNTYPE_QUAD)
                    PostMessage (hWndSM, MYWM_QUOTEQUAD, FALSE, 13);
                break;

            case EXITTYPE_GOTO_ZILDE:   // Nothing more to do with these types
            case EXITTYPE_DISPLAY:      // ...
            case EXITTYPE_NOVALUE:      // ...
                break;

            case EXITTYPE_NODISPLAY:    // Signal previous SI layer's semaphore if it's Quad input
                // If the previous layer in the SI stack is Quad input,
                //   signal it to receive this value
                if (lpSISPrv
                 && lpSISPrv->DfnType EQ DFNTYPE_QUAD)
                {
                    // Lock the memory to get a ptr to it
                    lpMemWFSO = MyGlobalLock (hGlbWFSO);

                    Assert (lpSISPrv->hSemaphore NE NULL);

                    // Tell the wait handler to signal this layer
                    hSigaphore = lpMemWFSO->hSigaphore = lpSISPrv->hSemaphore;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;
                } // End IF

                break;

            case EXITTYPE_RESET_ONE:        // Stop at this level
                exitType = EXITTYPE_NOVALUE;
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

                break;

            case EXITTYPE_RESET_ONE_INIT:   // Change to EXITTYPE_RESET_ONE
                exitType = EXITTYPE_RESET_ONE;
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE;

                // Fall through to common code

            case EXITTYPE_RESET_ALL:        // Continue resetting if more layers
                // If there are no more SI layers, ...
                if (lpSISPrv EQ NULL)
                {
                    // Display the default prompt
                    DisplayPrompt (hWndEC, 6);

                    break;
                } // End IF

                // Fall through to common code

            case EXITTYPE_GOTO_LINE:        // Signal previous SI layer's semaphore if it's user-defined function/operator
                // If the previous layer in the SI stack is a
                //   User-Defined Function/Operator or Quad Input,
                //   signal it to handle this action
                if (lpSISPrv
                 && (lpSISPrv->DfnType EQ DFNTYPE_OP1
                  || lpSISPrv->DfnType EQ DFNTYPE_OP2
                  || lpSISPrv->DfnType EQ DFNTYPE_FCN
                  || lpSISPrv->DfnType EQ DFNTYPE_QUAD))
                {
                    // Lock the memory to get a ptr to it
                    lpMemWFSO = MyGlobalLock (hGlbWFSO);

                    Assert (lpSISPrv->hSemaphore NE NULL);

                    // Tell the wait handler to signal this layer
                    hSigaphore = lpMemWFSO->hSigaphore = lpSISPrv->hSemaphore;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbWFSO); lpMemWFSO = NULL;
                } // End IF

                break;

            case EXITTYPE_NONE:
            defstop
                break;
        } // End SWITCH

        // Get the reset flag
        resetFlag = lpMemPTD->lpSISCur->ResetFlag;

        // Unlocalize the STEs on the innermost level
        //   and strip off one level
        Unlocalize ();

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

        // Untokenize the temporary line and free its memory
        Untokenize (hGlbToken);
        MyGlobalFree (hGlbToken); hGlbToken = NULL;
ERROR_EXIT:
#ifdef DEBUG
        dprintfW (L"--Ending   thread in <ImmExecStmtInThread>.");
#endif
        // Free the virtual memory for the complete line
        if (bFreeLine)
        {
            VirtualFree (lpwszCompLine, 0, MEM_RELEASE); lpwszCompLine = NULL;
        } // End IF

        // Display the default prompt
        //   if we're not resetting, and
        //   the caller isn't waiting for us to finish, and
//////////   the exit type isn't error, and
        //   there's no semaphore to signal
#ifdef DEBUG
        dprintfW (L"--Before DisplayPrompt (3):  resetFlag = %d, bWaitUntilFini = %d, exitType = %d, hSigaphore = %08X",
                  resetFlag,
                  bWaitUntilFini,
                  exitType,
                  hSigaphore);
#endif
        if (resetFlag EQ RESETFLAG_NONE
         && !bWaitUntilFini
/////////&& exitType NE EXITTYPE_ERROR
         && hSigaphore EQ NULL)
            DisplayPrompt (hWndEC, 3);
        return exitType;
#ifndef DEBUG
    } __except (CheckException (GetExceptionInformation (), "ImmExecStmtInThread"))
    {
        // Display message for unhandled exception
        DisplayException ();

        return EXITTYPE_ERROR;      // To keep the compiler happy
    } // End __try/__except
#endif
} // End ImmExecStmtInThread
#undef  APPEND_NAME


//***************************************************************************
//  End of File: immexec.c
//***************************************************************************
