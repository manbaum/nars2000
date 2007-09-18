//***************************************************************************
//  NARS2000 -- User-Defined Function/Operator Execution Routines
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

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $ExecDfnGlbProto_EM_YY
//
//  Execute a user-defined function/operator on a prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecDfnGlbProto_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecDfnGlbProto_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFcnStr,            // Ptr to function strand
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)
{
    HGLOBAL hGlbProto;              // Prototype global memory handle

    // Get the user-defined function/operator global memory handle
    hGlbProto = lptkFcnStr->tkData.tkGlbData;

    Assert (GetSignatureGlb (ClrPtrTypeDirGlb (hGlbProto)) EQ DFN_HEADER_SIGNATURE);

    // Execute the user-defined function/operator on the arg using the []PROTOTYPE entry point
    return ExecDfnGlb_EM_YY (ClrPtrTypeDirGlb (hGlbProto),  // User-defined function/operator global memory handle
                             lptkLftArg,                    // Ptr to left arg token (may be NULL if monadic)
              (LPPL_YYSTYPE) lptkFcnStr,                    // Ptr to function strand
                             lptkRhtArg,                    // Ptr to right arg token
                             LINENUM_PROTOTYPE);            // Starting line # (see LINE_NUMS)
} // End ExecDfnGlbProto_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecDfnGlb_EM_YY
//
//  Execute a user-defined function/operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecDfnGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecDfnGlb_EM_YY
    (HGLOBAL      hGlbDfnHdr,       // User-defined function/operator global memory handle
     LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,       // Ptr to function strand (may be NULL if not an operator and no axis)
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LINE_NUMS    startLineNum)     // Starting line # (see LINE_NUMS)

{
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht;        // Right ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht;        // Right ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht;        // Right ...
    LPSYMENTRY   lpSymEntryBeg,     // Ptr to start of SYMENTRYs on the SIS
                 lpSymEntryNxt;     // Ptr to next     ...
    STFLAGS      stFlagsMT = {0};   // STE flags for empty entry
    HGLOBAL      hGlbPTD = NULL;    // PerTabData global memory handle
    LPPERTABDATA lpMemPTD = NULL;   // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYFcnStrLft,     // Ptr to left operand function strand (may be NULL if not an operator)
                 lpYYFcnStrRht;     // Ptr to right operand function strand (may be NULL if monadic operaor or not an operator)
    LPTOKEN      lptkAxis;          // Ptr to axis token

    // Check for axis operator
    if (lpYYFcnStr
     && lpYYFcnStr->FcnCount > 1
     && (lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
      || lpYYFcnStr[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
        lptkAxis = &lpYYFcnStr[1].tkToken;
     else
        lptkAxis = NULL;

    // If there's room for a left operand, ...
    if (lpYYFcnStr
     && lpYYFcnStr->FcnCount > (UINT) (1 + (lptkAxis NE NULL)))
    {
        // Set ptr to left operand
        lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

        // If there's room for a right operand, ...
        if (lpYYFcnStr->FcnCount > (UINT) (lpYYFcnStrLft->FcnCount + (lpYYFcnStrLft - lpYYFcnStr)))
            // Set ptr to right operand
            lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->FcnCount];
        else
            lpYYFcnStrRht = NULL;
    } else
        lpYYFcnStrLft = lpYYFcnStrRht = NULL;

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

    // If there's no left arg token and this function requires a left arg,
    //   signal a SYNTAX ERROR
    if (lptkLftArg EQ NULL
     && lpMemDfnHdr->FcnValence NE FCNVALENCE_AMB
     && lpMemDfnHdr->numLftArgSTE NE 0)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkLftArg);
        goto ERROR_EXIT;
    } // End IF

    // If there is an axis token and this function does not accept
    //   an axis operator, signal an SYNTAX ERROR
    if (lptkAxis NE NULL
     && lpMemDfnHdr->steAxisOpr EQ NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        goto ERROR_EXIT;
    } // End IF

    // If there's no right arg token and this function requires a right arg,
    //   signal a SYNTAX ERROR
    if (lptkRhtArg EQ NULL
     && lpMemDfnHdr->numRhtArgSTE NE 0)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkRhtArg);
        goto ERROR_EXIT;
    } // End IF

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    if (lptkLftArg)
        AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    if (lptkRhtArg)
        AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // If the DFN left arg is a list, make sure the left arg token
    //   is a scalar, or a vector of the proper length
    if (lpMemDfnHdr->numLftArgSTE > 1
     && (aplRankLft > 1
      || (aplRankLft EQ 1
       && lpMemDfnHdr->numLftArgSTE NE aplNELMLft)))
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkLftArg);
        goto ERROR_EXIT;
    } // End IF

    // If the DFN right arg is a list, make sure the right arg token
    //   is a scalar, or a vector of the proper length
    if (lpMemDfnHdr->numRhtArgSTE > 1
     && (aplRankRht > 1
      || (aplRankRht EQ 1
       && lpMemDfnHdr->numRhtArgSTE NE aplNELMRht)))
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkRhtArg);
        goto ERROR_EXIT;
    } // End IF

    // Split cases based upon the starting line #
    switch (startLineNum)
    {
        case LINENUM_ONE:
            startLineNum = 1;

            break;

        case LINENUM_PROTOTYPE:
            // If not defined (=0), enter at line 1
            startLineNum = max (1, lpMemDfnHdr->nPrototypeLine);

            break;

        case LINENUM_INVERSE:
            startLineNum = lpMemDfnHdr->nInverseLine;

            break;

        case LINENUM_SINGLETON:
            startLineNum = lpMemDfnHdr->nSingletonLine;

            break;

        defstop
            break;
    } // End SWITCH

    // Check for non-existant label
    if (startLineNum EQ 0)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkRhtArg);
        goto ERROR_EXIT;
    } // End IF
RESTART_EXCEPTION_EXECDFNGLB:
    __try
    {
        // Fill in the SIS header for a User-Defined Function/Operator
        lpMemPTD->lpSISNxt->Sig.nature   = SIS_HEADER_SIGNATURE;
////////lpMemPTD->lpSISNxt->hThread      =          // Filled in by ExecuteFunction_EM_YY
////////lpMemPTD->lpSISNxt->hSemaphore   =          // ...
        lpMemPTD->lpSISNxt->hSigaphore   = NULL;
        lpMemPTD->lpSISNxt->hGlbDfnHdr   = hGlbDfnHdr;
        lpMemPTD->lpSISNxt->hGlbFcnName  = lpMemDfnHdr->steFcnName->stHshEntry->htGlbName;
        lpMemPTD->lpSISNxt->DfnType      = lpMemDfnHdr->DfnType;
        lpMemPTD->lpSISNxt->FcnValence   = lpMemDfnHdr->FcnValence;
        lpMemPTD->lpSISNxt->DfnAxis      = lpMemDfnHdr->DfnAxis;
        lpMemPTD->lpSISNxt->Suspended    = FALSE;
        lpMemPTD->lpSISNxt->Avail        = FALSE;
        lpMemPTD->lpSISNxt->CurLineNum   = 1;
        lpMemPTD->lpSISNxt->NxtLineNum   = 2;
////////lpMemPTD->lpSISNxt->numLabels    =              // Filled in below
        lpMemPTD->lpSISNxt->numResultSTE = lpMemDfnHdr->numResultSTE;
        lpMemPTD->lpSISNxt->numLftArgSTE = lpMemDfnHdr->numLftArgSTE;
        lpMemPTD->lpSISNxt->numRhtArgSTE = lpMemDfnHdr->numRhtArgSTE;
        lpMemPTD->lpSISNxt->numLocalsSTE = lpMemDfnHdr->numLocalsSTE;
        lpMemPTD->lpSISNxt->numFcnLines  = lpMemDfnHdr->numFcnLines;
        lpMemPTD->lpSISNxt->QQPromptLen  = 0;
        lpMemPTD->lpSISNxt->ErrorCode    = ERRORCODE_NONE;
        lpMemPTD->lpSISNxt->lpSISPrv     = lpMemPTD->lpSISCur;
////////lpMemPTD->lpSISNxt->lpSISNxt     =              // Filled in below
#ifdef DEBUG
        dprintfW (L"~~Localize:    %08X (%s)", lpMemPTD->lpSISNxt, L"ExecDfnGlb_EM_YY");
#endif
    } __except (CheckException (GetExceptionInformation ()))
    {
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_ACCESS_VIOLATION:
            {
                MEMORY_BASIC_INFORMATION mbi;

                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                // See how many pages are already allocated
                VirtualQuery (lpMemPTD->lpSISNxt,
                             &mbi,
                              sizeof (mbi));

                // Check for no allocation as yet
                if (mbi.State EQ MEM_RESERVE)
                    mbi.RegionSize = 0;

                // Allocate more memory to the YYRes buffer
                if (VirtualAlloc (lpMemPTD->lpSISNxt,
                                  mbi.RegionSize + DEF_SIS_INCRSIZE * sizeof (SYMENTRY),
                                  MEM_COMMIT,
                                  PAGE_READWRITE) NE NULL)
                    goto RESTART_EXCEPTION_EXECDFNGLB;

                // Fall through to never-never-land

            } // End EXCEPTION_ACCESS_VIOLATION

            defstop
                break;
        } // End SWITCH
    } // End __try/__except

    //***************************************************************
    // Errors beyond this point must call Unlocalize
    //***************************************************************

    lpMemPTD->lpSISCur               = lpMemPTD->lpSISNxt;

    // Increment to the next SI Level
    lpMemPTD->SILevel++;

    // Point to the destination SYMENTRYs
    lpSymEntryBeg =
    lpSymEntryNxt = (LPSYMENTRY) ByteAddr (lpMemPTD->lpSISNxt, sizeof (SIS_HEADER));

    // Fill in mask flag values for empty entry
    stFlagsMT.Inuse   = 1;          // Retain Inuse flag
    stFlagsMT.ObjName = NEG1U;      // ...    ObjName setting

    // Copy onto the SIS the current STEs for each local
    //   and undefine all but the system vars

    // Localize and clear the result STEs
    lpSymEntryNxt =
    LocalizeSymEntries (lpSymEntryNxt,
                        lpMemDfnHdr->numResultSTE,
                        (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offResultSTE),
                       &stFlagsMT);
    // Localize and clear the left arg STEs
    lpSymEntryNxt =
    LocalizeSymEntries (lpSymEntryNxt,
                        lpMemDfnHdr->numLftArgSTE,
                        (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offLftArgSTE),
                       &stFlagsMT);
    // Localize and clear the left operand STE
    lpSymEntryNxt =
    LocalizeSymEntries (lpSymEntryNxt,
                        lpMemDfnHdr->steLftOpr NE NULL,
                       &lpMemDfnHdr->steLftOpr,
                       &stFlagsMT);
    // Localize and clear the axis operand STE
    lpSymEntryNxt =
    LocalizeSymEntries (lpSymEntryNxt,
                        lpMemDfnHdr->steAxisOpr NE NULL,
                       &lpMemDfnHdr->steAxisOpr,
                       &stFlagsMT);
    // Localize and clear the right operand STE
    lpSymEntryNxt =
    LocalizeSymEntries (lpSymEntryNxt,
                        lpMemDfnHdr->steRhtOpr NE NULL,
                       &lpMemDfnHdr->steRhtOpr,
                       &stFlagsMT);
    // Localize and clear the right arg STEs
    lpSymEntryNxt =
    LocalizeSymEntries (lpSymEntryNxt,
                        lpMemDfnHdr->numRhtArgSTE,
                        (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offRhtArgSTE),
                       &stFlagsMT);
    // Localize and clear the locals STEs
    lpSymEntryNxt =
    LocalizeSymEntries (lpSymEntryNxt,
                        lpMemDfnHdr->numLocalsSTE,
                        (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offLocalsSTE),
                       &stFlagsMT);
    // Search for line labels, localize and initialize them
    lpSymEntryNxt =
    LocalizeLabels (lpSymEntryNxt,
                   &lpMemPTD->lpSISNxt->numLabels,
                    lpMemDfnHdr);

    // Save the # SYMENTRYs localized
    lpMemPTD->lpSISNxt->numSymEntries = lpSymEntryNxt - lpSymEntryBeg;

    // Save as new SISNxt ptr
    lpMemPTD->lpSISNxt               = (LPSIS_HEADER) lpSymEntryNxt;

    // Setup the left arg STEs
    InitVarSTEs (lptkLftArg,
                 lpMemDfnHdr->numLftArgSTE,
                 (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offLftArgSTE));
    // Setup the left operand STE
    if (!InitFcnSTEs (lpYYFcnStrLft,
                      lpMemDfnHdr->steLftOpr NE NULL,
                     &lpMemDfnHdr->steLftOpr))
        goto UNLOCALIZE_EXIT;
    // Setup the axis operand STE
    InitVarSTEs (lptkAxis,
                 lpMemDfnHdr->steAxisOpr NE NULL,
                &lpMemDfnHdr->steAxisOpr);
    // Setup the right operand STE
    if (!InitFcnSTEs (lpYYFcnStrRht,
                      lpMemDfnHdr->steRhtOpr NE NULL,
                     &lpMemDfnHdr->steRhtOpr))
        goto UNLOCALIZE_EXIT;
    // Setup the right arg STEs
    InitVarSTEs (lptkRhtArg,
                 lpMemDfnHdr->numRhtArgSTE,
                 (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offRhtArgSTE));

    // We no longer need these ptrs
    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return ExecuteFunction_EM_YY (startLineNum, &lpYYFcnStr->tkToken);

UNLOCALIZE_EXIT:
    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    Unlocalize ();
ERROR_EXIT:
    if (hGlbDfnHdr && lpMemDfnHdr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF

    if (hGlbPTD && lpMemPTD)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF

    return lpYYRes;
} // End ExecDfnGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecuteFunction_EM_YY
//
//  Execute a function, line by line
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecuteFunction_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecuteFunction_EM_YY
    (UINT         uLineNum,         // Starting line # (origin-1)
     LPTOKEN      lptkFunc)         // Ptr to function token

{
    LPPL_YYSTYPE   lpYYRes = NULL;  // Ptr to the result
    LPDFN_HEADER   lpMemDfnHdr;     // Ptr to user-defined function/operator header
    LPTOKEN_HEADER lptkHdr;         // Ptr to header of tokenized line
    LPTOKEN        lptkLine;        // Ptr to tokenized line
    HGLOBAL        hGlbDfnHdr,      // User-defined function/operator global memory handle
                   hGlbTxtLine,     // Line text global memory handle
                   hGlbTknLine,     // Tokenized line global memory handle
                   hGlbPTD;         // PerTabData global memory handle
    LPPERTABDATA   lpMemPTD = NULL; // Ptr to PerTabData global memory
    LPFCNLINE      lpFcnLines;      // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HWND           hWndSM;          // Session Manager window handle
    HANDLE         hSemaphore;      // Semaphore handle
    UINT           numResultSTE,    // # result STEs (may be zero if no result)
                   numRes;          // Loop counter
    LPSYMENTRY    *lplpSymEntry;    // Ptr to 1st result STE
    HGLOBAL        hGlbTknHdr;      // Tokenized header global memory handle
    BOOL           bRet;            // TRUE iff result is valid
#ifdef DEBUG
    EXIT_TYPES     exitType;        // Return code from ParseLine
#endif
    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the user-defined function/operator header global memory handle
    hGlbDfnHdr = lpMemPTD->lpSISCur->hGlbDfnHdr;

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

    // Get the tokenized header global memory handle
    hGlbTknHdr = lpMemDfnHdr->hGlbTknHdr;

    // Get the SM window handle
    hWndSM = lpMemPTD->hWndSM;

    // Save current line #
    lpMemPTD->lpSISCur->CurLineNum = uLineNum;
    lpMemPTD->lpSISCur->NxtLineNum = uLineNum + 1;

    // Create a semaphore
    hSemaphore =
    lpMemPTD->lpSISCur->hSemaphore =
    CreateSemaphore (NULL,              // No security attrs
                     0,                 // Initial count (non-signalled)
                     64*1024,           // Maximum count
                     NULL);             // No name
    // Loop through the function lines
    while (0 < uLineNum && uLineNum <= lpMemPTD->lpSISCur->numFcnLines)
    {
        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Get the starting line's token & text global memory handles
        hGlbTxtLine = lpFcnLines[uLineNum - 1].hGlbTxtLine;
        hGlbTknLine = lpFcnLines[uLineNum - 1].hGlbTknLine;

#ifdef DEBUG
        {
            LPMEMTXT_UNION lpMemTxtLine;
            LPAPLCHAR      lpMemFcnName;

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

            // Lock the memory to get a ptr to it
            lpMemFcnName = MyGlobalLock (lpMemPTD->lpSISCur->hGlbFcnName);

            wsprintfW (lpwszTemp,
                       L"Executing line %d of <%s>:  %s",
                       uLineNum,
                       lpMemFcnName,
                      &lpMemTxtLine->C);
            DbgMsgW (lpwszTemp);

            // We no longer need these ptrs
            MyGlobalUnlock (lpMemPTD->lpSISCur->hGlbFcnName); lpMemFcnName = NULL;
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        }
#endif
        // We no longer need these ptrs
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

        // Execute the function line
#ifdef DEBUG
        exitType =
#endif
        ParseLine (hWndSM,                  // Session Manager window handle
                   hGlbTxtLine,             // Line text global memory handle
                   hGlbTknLine,             // Tokenixed line global memory handle
                   NULL,                    // Ptr to line text global memory
                   hGlbPTD);                // PerTabData global memory handle
#ifdef DEBUG
        // Split cases based upon the exit type
        switch (exitType)
        {
            case EXITTYPE_GOTO_ZILDE:   // Nothing more to do with these types
            case EXITTYPE_DISPLAY:      // ...
            case EXITTYPE_NODISPLAY:    // ...
            case EXITTYPE_NOVALUE:      // ...
            case EXITTYPE_ERROR:        // ...
            case EXITTYPE_GOTO_LINE:    // ...
            case EXITTYPE_RESET_ONE:
            case EXITTYPE_RESET_ONE_INIT:
            case EXITTYPE_RESET_ALL:
                break;

            case EXITTYPE_NONE:
            defstop
                break;
        } // End SWITCH
#endif
        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // If suspended, wait for the semaphore to trigger
        if (lpMemPTD->lpSISCur->Suspended)
        {
            HWND hWndEC;        // Edit Control window handle

            // Get the Edit Control window handle
            hWndEC = (HWND) GetWindowLong (hWndSM, GWLSF_HWNDEC);

            // Display the default prompt
            DisplayPrompt (hWndEC, 2);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

#ifdef DEBUG
            dprintfW (L"~~WaitForSingleObject (ENTRY):  %s (%S#%d)", L"ExecuteFunction_EM_YY", FNLN);
#endif
            // Wait for the semaphore to trigger
            WaitForSingleObject (hSemaphore,            // Ptr to handle to wait for
                                 INFINITE);             // Timeout value in milliseconds
#ifdef DEBUG
            dprintfW (L"~~WaitForSingleObject (EXIT):  %s (%S#%d)", L"ExecuteFunction_EM_YY", FNLN);
#endif
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

        // If we're suspended or resetting, break
        if (lpMemPTD->lpSISCur->Suspended
         || lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
            break;

        // Get next line #
        uLineNum = lpMemPTD->lpSISCur->NxtLineNum++;
        lpMemPTD->lpSISCur->CurLineNum = uLineNum;
    } // End WHILE

    // Close the semaphore handle as it isn't used anymore
    CloseHandle (hSemaphore); hSemaphore = NULL;

    // If we're resetting, Unlocalize
    if (lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
        goto UNLOCALIZE_EXIT;

    // If we're suspended, don't Unlocalize
    if (lpMemPTD->lpSISCur->Suspended)
        goto ERROR_EXIT;

    // Get the # STEs in the result
    numResultSTE = lpMemDfnHdr->numResultSTE;

    // Get ptr to 1st result STE
    lplpSymEntry = (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offResultSTE);

    // Ensure that all STEs in the result have a value
    switch (numResultSTE)
    {
        case 0:         // No result
            // Make a PL_YYSTYPE NoValue entry
            lpYYRes = MakeNoValue_YY (lptkFunc);

            break;

        case 1:         // Single result name:  Copy it as the result from this function
            // Check for a value
            if (!(*lplpSymEntry)->stFlags.Value)
                // Make a PL_YYSTYPE NoValue entry
                lpYYRes = MakeNoValue_YY (lptkFunc);
            else
            {
                // Lock the memory to get a ptr to it
                lptkHdr = MyGlobalLock (hGlbTknHdr);

                // Get ptr to the tokens in the line
                lptkLine = (LPTOKEN) ByteAddr (lptkHdr, sizeof (TOKEN_HEADER));

                // Loop 'til we point to the first named token
                while (lptkLine->tkFlags.TknType NE TKT_VARNAMED)
                    lptkLine++;

                // If it's not a variable, ...
                if ((*lplpSymEntry)->stFlags.ObjType NE NAMETYPE_VAR)
                {
                    // Set the error message
                    ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

                    // Set the error token
                    ErrorMessageSetToken (lptkLine);
                } else
                {
                    // Allocate a new YYRes
                    lpYYRes = YYAlloc ();

                    // If it's immediate, ...
                    if ((*lplpSymEntry)->stFlags.Imm)
                    {
                        // Fill in the result token
                        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                        lpYYRes->tkToken.tkFlags.ImmType   = (*lplpSymEntry)->stFlags.ImmType;
////////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;             // Already zero from YYAlloc
                        lpYYRes->tkToken.tkData.tkLongest  = (*lplpSymEntry)->stData.stLongest;
                        lpYYRes->tkToken.tkCharIndex       = lptkLine->tkCharIndex;
                    } else
                    {
                        // Fill in the result token
                        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;             // Already zero from YYAlloc
////////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;             // Already zero from YYAlloc
                        lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDir ((*lplpSymEntry)->stData.stGlbData);
                        lpYYRes->tkToken.tkCharIndex       = lptkLine->tkCharIndex;
                    } // End IF/ELSE
                } // End IF/ELSE

                // We no longer need this ptr
                MyGlobalUnlock (hGlbTknHdr); lptkHdr = NULL; lptkLine = NULL;
            } // End IF/ELSE

            break;

        default:        // Multiple result names:  Allocate storage to hold them
        {
            APLUINT ByteRes;        // # bytes needed for the result
            HGLOBAL hGlbRes;        // Result global memory handle
            LPVOID  lpMemRes;       // Ptr to result global memory

            // Ensure all result names are vars and have a value
            for (bRet = TRUE, numRes = 0; numRes < numResultSTE; numRes++)
            {
                // Lock the memory to get a ptr to it
                lptkHdr = MyGlobalLock (hGlbTknHdr);

                // Get ptr to the tokens in the line
                lptkLine = (LPTOKEN) ByteAddr (lptkHdr, sizeof (TOKEN_HEADER));

                // Loop 'til we point to the first named token
                while (lptkLine->tkFlags.TknType NE TKT_VARNAMED)
                    lptkLine++;

                // If the name has no value, ...
                if (!lplpSymEntry[numRes]->stFlags.Value)
                {
                    // Set the error message
                    ErrorMessageIndirect (ERRMSG_VALUE_ERROR APPEND_NAME);

                    // Set the error token
                    ErrorMessageSetToken (&lptkLine[numRes]);

                    // Mark as in error
                    bRet = FALSE;
                } else
                // If the name is not a variable, ...
                if (lplpSymEntry[numRes]->stFlags.ObjType NE NAMETYPE_VAR)
                {
                    // Set the error message
                    ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

                    // Set the error token
                    ErrorMessageSetToken (&lptkLine[numRes]);

                    // Mark as in error
                    bRet = FALSE;
                } // End IF/ELSE/...

                // We no longer need this ptr
                MyGlobalUnlock (hGlbTknHdr); lptkHdr = NULL; lptkLine = NULL;

                if (!bRet)
                {
                    // Mark as an error on line 0
                    lpMemPTD->lpSISCur->CurLineNum = 0;

                    goto ERROR_EXIT;
                } // End IF
            } // End FOR

            // Calculate space needed for the result
            ByteRes = CalcArraySize (ARRAY_NESTED, numResultSTE, 1);

            // Allocate space for the result
            hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
            if (!hGlbRes)
            {
                // Set the error message
                ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

                // Set the error token
                ErrorMessageSetToken (NULL);

                goto ERROR_EXIT;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = ARRAY_NESTED;
////////////lpHeader->Perm       = 0;               // Already zero from GHND
////////////lpHeader->SysVar     = 0;               // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = numResultSTE;
            lpHeader->Rank       = 1;

#undef  lpHeader

            // Fill in the dimension
            *VarArrayBaseToDim (lpMemRes) = numResultSTE;

            // Skip over the header and dimension
            lpMemRes = VarArrayBaseToData (lpMemRes, 1);

            // Fill in the result
            for (numRes = 0; numRes < numResultSTE; numRes++)
            {
                // If it's immediate, copy the LPSYMENTRY
                if (lplpSymEntry[numRes]->stFlags.Imm)
                    *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lplpSymEntry[numRes]);
                else
                // Otherwise, copy the HGLOBAL
                    *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lplpSymEntry[numRes]->stData.stGlbData);
            } // End FOR

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;             // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;             // Already zero from YYAlloc
            lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = NEG1U;

            // See if it fits into a lower (but not necessarily smaller) datatype
            lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);

            break;
        } // End default
    } // End SWITCH
UNLOCALIZE_EXIT:
    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    Unlocalize ();
ERROR_EXIT:
    // We no longer need these ptrs
    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpYYRes;
} // End ExecuteFunction_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $Unlocalize
//
//  Unlocalize STEs on the innermost level
//    and strip off one level
//***************************************************************************

BOOL Unlocalize
    (void)

{
    HGLOBAL      hGlbPTD,           // PerTabData global memory handle
                 hGlbData;          // STE global memory handle
    LPPERTABDATA lpMemPTD = NULL;   // Ptr to PerTabData global memory
    UINT         numSymEntries,     // # SYMENTRYs localized
                 numSym;            // Loop counter
    LPSYMENTRY   lpSymEntryNxt;     // Ptr to next SYMENTRY on the SIS
    BOOL         bRet = TRUE;       // TRUE iff the result is valid
    RESET_FLAGS  resetFlag;         // Reset flag (see RESET_FLAGS)

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If the SI is non-empty, ...
    if (lpMemPTD->SILevel)
    {
        Assert (lpMemPTD->lpSISCur NE NULL);
#ifdef DEBUG
        dprintfW (L"~~Unlocalize:  %08X to %08X", lpMemPTD->lpSISCur, lpMemPTD->lpSISCur->lpSISPrv);
#endif
        // Save the reset flag
        resetFlag = lpMemPTD->lpSISCur->ResetFlag;

        // Get # SYMENTRYs on the stack
        numSymEntries = lpMemPTD->lpSISCur->numSymEntries;

        // Point to the destination SYMENTRYs
        lpSymEntryNxt = (LPSYMENTRY) ByteAddr (lpMemPTD->lpSISCur, sizeof (SIS_HEADER));

        // Loop through the # SYMENTRYs
        for (numSym = 0; numSym < numSymEntries; numSym++)
        // If the hash entry is valid, ...
        if (lpSymEntryNxt[numSym].stHshEntry)
        {
            LPSYMENTRY lpSymEntryCur;       // Ptr to SYMENTRY to release & unlocalize

            // Get the ptr to the corresponding SYMENTRY
            lpSymEntryCur = lpSymEntryNxt[numSym].stHshEntry->htSymEntry;

            // Release the current value of the STE
            //   if it's not immediate and has a value
            if (!lpSymEntryCur->stFlags.Imm && lpSymEntryCur->stFlags.Value)
            {
                // Get the global memory handle
                hGlbData = lpSymEntryCur->stData.stGlbData;

                // Split cases based upon the nametype
                switch (lpSymEntryCur->stFlags.ObjType)
                {
                    case NAMETYPE_VAR:
                        // stData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (hGlbData));

                        // Free the global var
                        FreeResultGlobalVar (ClrPtrTypeDirGlb (hGlbData));

                        break;

                    case NAMETYPE_FN0:
                        // stData is a user-defined function/operator
                        Assert (lpSymEntryCur->stFlags.UsrDfn);

                        // Free the global user-defined function/operator
                        FreeResultGlobalDfn (ClrPtrTypeDirGlb (hGlbData));

                        break;

                    case NAMETYPE_FN12:
                    case NAMETYPE_OP1:
                    case NAMETYPE_OP2:
                        if (lpSymEntryCur->stFlags.UsrDfn)
                            // Free the global user-defined function/operator
                            FreeResultGlobalDfn (ClrPtrTypeDirGlb (hGlbData));
                        else
                            // Free the global function array
                            FreeResultGlobalFcn (ClrPtrTypeDirGlb (hGlbData));
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End IF

            // Restore the previous STE
            *lpSymEntryCur = lpSymEntryNxt[numSym];
        } // End FOR

        // Strip the level from the stack
        lpMemPTD->lpSISNxt = lpMemPTD->lpSISCur;
        lpMemPTD->lpSISCur = lpMemPTD->lpSISCur->lpSISPrv;
        if (lpMemPTD->lpSISCur)
        {
            lpMemPTD->lpSISCur->lpSISNxt = NULL;

            // Transfer the reset flag up one level
            lpMemPTD->lpSISCur->ResetFlag = resetFlag;
        } // End IF
#ifdef DEBUG
        // Zero the old entry
        ZeroMemory (lpMemPTD->lpSISNxt,
                    sizeof (*lpMemPTD->lpSISNxt)
                  + numSymEntries * sizeof (lpSymEntryNxt));
#endif
        // Back off the SI Level
        lpMemPTD->SILevel--;
    } // End IF
///NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End Unlocalize


//***************************************************************************
//  $LocalizeLabels
//
//  Localize and initialize all line labels
//***************************************************************************

LPSYMENTRY LocalizeLabels
    (LPSYMENTRY   lpSymEntryNxt,
     LPUINT       lpNumLabels,
     LPDFN_HEADER lpMemDfnHdr)

{
    UINT           numFcnLines,  // # lines in the function
                   uLineNum;     // Loop counter
    LPFCNLINE      lpFcnLines;   // Ptr to array of function line structs (FCNLINE[numFcnLines])
    LPTOKEN_HEADER lptkHdr;      // Ptr to header of tokenized line
    LPTOKEN        lptkLine;     // Ptr to tokenized line
    STFLAGS stFlagsClr = {0};   // Flags for clearing an STE

    // Set the Inuse flag
    stFlagsClr.Inuse = 1;

    // Get # lines in the function
    numFcnLines = lpMemDfnHdr->numFcnLines;

    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

    // Loop through the function lines
    for (*lpNumLabels = uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
    {
        UINT numTokens;     // # tokens in the line

        // Lock the memory to get a ptr to it
        lptkHdr = MyGlobalLock (lpFcnLines->hGlbTknLine);

        // Get the # tokens in the line
        numTokens = lptkHdr->TokenCnt;

        // Get ptr to the tokens in the line
        lptkLine = (LPTOKEN) ByteAddr (lptkHdr, sizeof (TOKEN_HEADER));

        Assert (lptkLine[0].tkFlags.TknType EQ TKT_EOL
             || lptkLine[0].tkFlags.TknType EQ TKT_EOS);

        // If there are at least three tokens, ...
        //   (TKT_EOL, TKT_VARNAMED, TKT_COLON)
        if (numTokens >= 3)
        {
            if (lptkLine[2].tkFlags.TknType EQ TKT_COLON
             && lptkLine[1].tkFlags.TknType EQ TKT_VARNAMED)
            {
                LPSYMENTRY lpSymEntrySrc;   // Ptr to source SYMENTRY

                // Get the source LPSYMENTRY
                lpSymEntrySrc = lptkLine[1].tkData.tkSym;

                // stData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpSymEntrySrc) EQ PTRTYPE_STCONST);

                // Copy the old SYMENTRY to the SIS
                *lpSymEntryNxt++ = *lpSymEntrySrc;

                // Clear the STE flags & data
                *((PUINT_PTR) &lpSymEntrySrc->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////////lpSymEntrySrc->stData.stLongest = 0;        // stLongest set below

                // Initialize the SYMENTRY to an integer constant
                lpSymEntrySrc->stFlags.Imm      = 1;
                lpSymEntrySrc->stFlags.ImmType  = IMMTYPE_INT;
                lpSymEntrySrc->stFlags.Inuse    = 1;
                lpSymEntrySrc->stFlags.Value    = 1;
                lpSymEntrySrc->stFlags.ObjName  = OBJNAME_USR;
                lpSymEntrySrc->stFlags.ObjType  = NAMETYPE_VAR;
                lpSymEntrySrc->stFlags.DfnLabel = 1;
                lpSymEntrySrc->stData.stInteger = uLineNum + 1;

                // Count in another label
                (*lpNumLabels)++;
            } // End IF
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (lpFcnLines->hGlbTknLine); lptkHdr = NULL; lptkLine = NULL;

        // Skip to the next struct
        lpFcnLines++;
    } // End FOR

    return lpSymEntryNxt;
} // End LocalizeLabels


//***************************************************************************
//  $InitVarSTEs
//
//  Initialize variable arg STEs
//***************************************************************************

void InitVarSTEs
    (LPTOKEN      lptkArg,      // Ptr to variable token
     UINT         numArgSTE,    // # STEs to initialize
     LPSYMENTRY  *lplpSymEntry) // Ptr to LPSYMENTRYs

{
    UINT    uSym;               // Loop counter
    HGLOBAL hGlbArg;            // Arg global memory handle
    LPVOID  lpMemArg;           // Ptr to arg global memory
    STFLAGS stFlagsClr = {0};   // Flags for clearing an STE

    // Set the Inuse flag
    stFlagsClr.Inuse = 1;

    // If the token is defined, ...
    if (lptkArg && numArgSTE)
    {
        // Split cases based upon the variable token type
        switch (lptkArg->tkFlags.TknType)
        {
            case TKT_VARIMMED:
            case TKT_AXISIMMED:
                // Loop through the LPSYMENTRYs
                for (uSym = 0; uSym < numArgSTE; uSym++, lplpSymEntry++)
                {
                    // Clear the STE flags & data
                    *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////////////(*lplpSymEntry)->stData.stLongest = 0;      // stLongest set below

                    (*lplpSymEntry)->stFlags.Imm      = 1;
                    (*lplpSymEntry)->stFlags.ImmType  = lptkArg->tkFlags.ImmType;
                    (*lplpSymEntry)->stFlags.Value    = 1;
                    (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
                    (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_VAR;
                    (*lplpSymEntry)->stData.stLongest = lptkArg->tkData.tkLongest;
                } // End FOR

                return;

            case TKT_VARARRAY:
            case TKT_AXISARRAY:
                // Get the arg global memory handle
                hGlbArg = lptkArg->tkData.tkGlbData;

                break;

            case TKT_VARNAMED:
                // stData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lptkArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

                // If it's immediate, ...
                if (lptkArg->tkData.tkSym->stFlags.Imm)
                {
                    // Loop through the LPSYMENTRYs
                    for (uSym = 0; uSym < numArgSTE; uSym++, lplpSymEntry++)
                    {
                        // Clear the STE flags & data
                        *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////////////////(*lplpSymEntry)->stData.stLongest = 0;      // stLongest set below

                        (*lplpSymEntry)->stFlags.Imm      = 1;
                        (*lplpSymEntry)->stFlags.ImmType  = lptkArg->tkData.tkSym->stFlags.ImmType;
                        (*lplpSymEntry)->stFlags.Value    = 1;
                        (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stLongest = lptkArg->tkData.tkSym->stData.stLongest;

                        Assert (memcmp (*lplpSymEntry,
                                        lptkArg->tkData.tkSym,
                                        sizeof (**lplpSymEntry)) EQ 0);
                    } // End FOR

                    return;
                } else
                    // Get the arg global memory handle
                    hGlbArg = lptkArg->tkData.tkSym->stData.stGlbData;

                break;

            defstop
                break;
        } // End SWITCH

        // st/tkData is a valid HGLOBAL variable array
        Assert (IsGlbTypeVarDir (hGlbArg));

        // If there's only one STE, it gets the whole arg
        if (numArgSTE EQ 1)
        {
            // Clear the STE flags & data
            *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////(*lplpSymEntry)->stData.stLongest = 0;      // stLongest set below

////////////(*lplpSymEntry)->stFlags.Imm      = 0;      // Already zero from previous initialization
////////////(*lplpSymEntry)->stFlags.ImmType  = 0;      // Already zero from previous initialization
            (*lplpSymEntry)->stFlags.Value    = 1;
            (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
            (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_VAR;
            (*lplpSymEntry)->stData.stGlbData = CopySymGlbDir (hGlbArg);
        } else
        {
            APLSTYPE aplTypeArg;            // Arg storage type
            APLNELM  aplNELMArg;            // Arg NELM
            APLRANK  aplRankArg;            // Arg rank
            UINT     uBitIndex;             // Bit index for Booleans
            APLINT   apaOffArg,             // APA offset
                     apaMulArg;             // ... multiplier

            // Lock the memory to get a ptr to it
            lpMemArg = MyGlobalLock (ClrPtrTypeDirGlb (hGlbArg));

#define lpHeader        ((LPVARARRAY_HEADER) lpMemArg)

            aplTypeArg = lpHeader->ArrType;
            aplNELMArg = lpHeader->NELM;
            aplRankArg = lpHeader->Rank;

#undef  lpHeader

            // These were checked for above, but it never hurts to test again
            Assert (aplNELMArg EQ numArgSTE);
            Assert (aplRankArg EQ 1);

            // Skip over the header and dimensions to the data
            lpMemArg = VarArrayBaseToData (lpMemArg, aplRankArg);

            // In case the arg is Boolean
            uBitIndex = 0;

            // If the arg is APA, ...
            if (aplTypeArg EQ ARRAY_APA)
            {
#define lpAPA   ((LPAPLAPA) lpMemArg)
                // Get the APA parameters
                apaOffArg = lpAPA->Off;
                apaMulArg = lpAPA->Mul;
#undef  lpAPA
            } // End IF

            // Loop through the LPSYMENTRYs
            for (uSym = 0; uSym < numArgSTE; uSym++, lplpSymEntry++)
            {
                // Clear the STE flags & data
                *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
                (*lplpSymEntry)->stData.stLongest = 0;

                // Split cases based upon the arg storage type
                switch (aplTypeArg)
                {
                    case ARRAY_BOOL:
                        (*lplpSymEntry)->stFlags.Imm      = 1;
                        (*lplpSymEntry)->stFlags.ImmType  = IMMTYPE_INT;
                        (*lplpSymEntry)->stFlags.Value    = 1;
                        (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stBoolean = BIT0 & ((*(LPAPLBOOL) lpMemArg) >> uBitIndex);

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;                  // Start over
                            ((LPAPLBOOL) lpMemArg)++;       // Skip to next byte
                        } // End IF

                        break;

                    case ARRAY_INT:
                        (*lplpSymEntry)->stFlags.Imm      = 1;
                        (*lplpSymEntry)->stFlags.ImmType  = IMMTYPE_INT;
                        (*lplpSymEntry)->stFlags.Value    = 1;
                        (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stInteger = *((LPAPLINT) lpMemArg)++;

                        break;

                    case ARRAY_FLOAT:
                        (*lplpSymEntry)->stFlags.Imm      = 1;
                        (*lplpSymEntry)->stFlags.ImmType  = IMMTYPE_FLOAT;
                        (*lplpSymEntry)->stFlags.Value    = 1;
                        (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stFloat   = *((LPAPLFLOAT) lpMemArg)++;

                        break;

                    case ARRAY_CHAR:
                        (*lplpSymEntry)->stFlags.Imm      = 1;
                        (*lplpSymEntry)->stFlags.ImmType  = IMMTYPE_CHAR;
                        (*lplpSymEntry)->stFlags.Value    = 1;
                        (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stChar    = *((LPAPLCHAR) lpMemArg)++;

                        break;

                    case ARRAY_APA:
                        (*lplpSymEntry)->stFlags.Imm      = 1;
                        (*lplpSymEntry)->stFlags.ImmType  = IMMTYPE_INT;
                        (*lplpSymEntry)->stFlags.Value    = 1;
                        (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stInteger = apaOffArg + apaMulArg * uSym;

                        break;

                    case ARRAY_HETERO:
                    case ARRAY_NESTED:
                        // Split cases based upon the arg ptr type
                        switch (GetPtrTypeInd (lpMemArg))
                        {
                            case PTRTYPE_STCONST:
                                (*lplpSymEntry)->stFlags.Imm      = 1;
                                (*lplpSymEntry)->stFlags.ImmType  = (*(LPAPLHETERO) lpMemArg)->stFlags.ImmType;
                                (*lplpSymEntry)->stFlags.Value    = 1;
                                (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
                                (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_VAR;
                                (*lplpSymEntry)->stData           = (*(LPAPLHETERO) lpMemArg)->stData;

                                // Skip to next element in arg
                                ((LPAPLHETERO) lpMemArg)++;

                                break;

                            case PTRTYPE_HGLOBAL:
////////////////////////////////(*lplpSymEntry)->stFlags.Imm      = 0;      // Already zero from previous initialization
////////////////////////////////(*lplpSymEntry)->stFlags.ImmType  = 0;      // Already zero from previous initialization
                                (*lplpSymEntry)->stFlags.Value    = 1;
                                (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
                                (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_VAR;
                                (*lplpSymEntry)->stData.stGlbData = CopySymGlbInd (lpMemArg);

                                // Skip to next element in arg
                                ((LPAPLNESTED) lpMemArg)++;

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            // We no longer need this ptr
            MyGlobalUnlock (ClrPtrTypeDirGlb (hGlbArg)); lpMemArg = NULL;
        } // End IF/ELSE
    } // End IF
} // End InitVarSTEs


//***************************************************************************
//  $InitFcnSTEs
//
//  Initialize function arg STEs
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- InitFcnSTEs"
#else
#define APPEND_NAME
#endif

BOOL InitFcnSTEs
    (LPPL_YYSTYPE lpYYArg,      // Ptr to arg PL_YYSTYPE
     UINT         numArgSTE,    // # STEs to initialize
     LPSYMENTRY  *lplpSymEntry) // Ptr to LPSYMENTRYs

{
    STFLAGS stFlagsClr = {0};   // Flags for clearing an STE

    // Set the Inuse flag
    stFlagsClr.Inuse = 1;

    // If the token is defined, ...
    if (lpYYArg && numArgSTE)
    {
        Assert (numArgSTE EQ 1);

        // Split cases based upon the function count
        if (lpYYArg->FcnCount EQ 1)
        {
            Assert (lpYYArg->tkToken.tkFlags.TknType EQ TKT_FCNIMMED);

            // Clear the STE flags & data
            *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////(*lplpSymEntry)->stData.stLongest = 0;      // stLongest set below

            (*lplpSymEntry)->stFlags.Imm      = 1;
            (*lplpSymEntry)->stFlags.ImmType  = lpYYArg->tkToken.tkFlags.ImmType;
            (*lplpSymEntry)->stFlags.Value    = 1;
            (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
            (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_FN12;
            (*lplpSymEntry)->stData.stLongest = lpYYArg->tkToken.tkData.tkLongest;
        } else
        {
            APLUINT ByteRes;            // # bytes needed for the result
            HGLOBAL hGlbRes;            // Result global memory handle
            LPVOID  lpMemRes;           // Ptr to result global memory

            // Calculate space needed for the result
            ByteRes = sizeof (FCNARRAY_HEADER)
                    + lpYYArg->FcnCount * sizeof (PL_YYSTYPE);

            hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
            if (!hGlbRes)
            {
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                          &lpYYArg->tkToken);
                return FALSE;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemRes)

            // Fill in the header
            lpHeader->Sig.nature  = FCNARRAY_HEADER_SIGNATURE;
            lpHeader->NameType    = NAMETYPE_FN12;
            lpHeader->RefCnt      = 1;
            lpHeader->fcnNELM     = lpYYArg->FcnCount;
////////////lpHeader->hGlbTxtLine = NULL;           // Already zero from GHND

#undef  lpHeader

            // Skip over the header and dimensions to the data
            lpMemRes = FcnArrayBaseToData (lpMemRes);

            // Copy the PL_YYSTYPEs to the global memory object
            CopyMemory (lpMemRes, lpYYArg, lpYYArg->FcnCount * sizeof (PL_YYSTYPE));

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // Clear the STE flags & data
            *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////(*lplpSymEntry)->stData.stLongest = 0;      // stLongest set below

////////////(*lplpSymEntry)->stFlags.Imm      = 0;      // Already zero from above
////////////(*lplpSymEntry)->stFlags.ImmType  = 0;      // Already zero from above
            (*lplpSymEntry)->stFlags.Value    = 1;
            (*lplpSymEntry)->stFlags.ObjName  = OBJNAME_USR;
            (*lplpSymEntry)->stFlags.ObjType  = NAMETYPE_FN12;
            (*lplpSymEntry)->stData.stGlbData = MakeGlbTypeGlb (hGlbRes);
        } // End IF/ELSE
    } // End IF

    return TRUE;
} // End InitFcnSTEs
#undef  APPEND_NAME


//***************************************************************************
//  $LocalizeSymEntries
//
//  Localize SYMENTRYs
//***************************************************************************

LPSYMENTRY LocalizeSymEntries
    (LPSYMENTRY  lpSymEntryNxt,
     UINT        numSymEntries,
     LPSYMENTRY *lplpSymEntrySrc,
     LPSTFLAGS   lpstFlagsMT)

{
    UINT uSym;          // Loop counter

    // Loop through the SYMENTRYs
    for (uSym = 0; uSym < numSymEntries; uSym++)
    {
        // Copy the SYMENTRY to the SIS
        *lpSymEntryNxt++ = **lplpSymEntrySrc;

        // If the entry is not a system name, mark it as empty (e.g., VALUE ERROR)
        if ((*lplpSymEntrySrc)->stFlags.ObjName NE OBJNAME_SYS)
        {
            // Clear the STE flags & data
            *(PUINT_PTR) &(*lplpSymEntrySrc)->stFlags &= *(PUINT_PTR) lpstFlagsMT;
            (*lplpSymEntrySrc)->stData.stLongest = 0;
        } // End IF

        // Skip to next source entry
        lplpSymEntrySrc++;
    } // End FOR

    return lpSymEntryNxt;
} // End LocalizeSymEntries


//***************************************************************************
//  End of File: execdfn.c
//***************************************************************************
