//***************************************************************************
//  NARS2000 -- Error Message Functions
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "dfnhdr.h"
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $BreakMessage
//
//  Display a message from Ctrl-Break
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- BreakMessage"
#else
#define APPEND_NAME
#endif

void BreakMessage
    (HWND         hWndSM,
     LPSIS_HEADER lpSISCur)

{
    LPAPLCHAR    lpMemName;     // Ptr to function name global memory
    APLNELM      aplNELMRes;    // Length of function name[line #]
    APLUINT      ByteRes;       // # bytes in the result
    LPAPLCHAR    lpMemRes;      // Ptr to result global memory
    HGLOBAL      hGlbRes,       // Result global memory handle
                 hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Mark as suspended
    lpMemPTD->lpSISCur->Suspended = TRUE;

    // Lock the memory to get a ptr to it
    lpMemName = MyGlobalLock (lpSISCur->hGlbFcnName);

    // Format the name and line #
    aplNELMRes =
      wsprintfW (lpwszTemp,
                 L"%s[%d]",
                 lpMemName,
                 lpSISCur->CurLineNum);
    // We no longer need this ptr
    MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

    // Calculate space needed for te result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

    // Allocate space for the result
    // N.B.  Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        MessageBoxW (hWndSM,
                     L"Unable to allocate space for " WS_UTF16_QUAD L"DM",
                     lpwszAppName,
                     MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        return;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;   // Already zero from GHND
////lpHeader->SysVar     = 0;   // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Copy the function name[line #] to the result
    CopyMemory (lpMemRes, lpwszTemp, (UINT) aplNELMRes * sizeof (APLCHAR));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Free the old value
    FreeResultGlobalVar (lpMemPTD->hGlbQuadDM);

    // Save the new value in the STE
    lpMemPTD->hGlbQuadDM = hGlbRes;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End BreakMessage
#undef  APPEND_NAME


//***************************************************************************
//  $ErrorMessageDirect
//
//  Signal an error message, directly
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ErrorMessageDirect"
#else
#define APPEND_NAME
#endif

void ErrorMessageDirect
    (LPWCHAR lpwszMsg,          // Ptr to error message text
     LPWCHAR lpwszLine,         // Ptr to the line which generated the error
     UINT    uCaret,            // Position of caret (origin-0)
     HWND    hWndSM)            // Window handle to the Session Manager

{
    APLNELM       aplNELMRes;   // Result NELM
    APLUINT       ByteRes;      // # bytes in the result
    HGLOBAL       hGlbRes;      // Result global memory handle
    LPAPLCHAR     lpMemRes;     // Ptr to result global memory
    HGLOBAL       hGlbPTD;      // PerTabData global memory handle
    LPPERTABDATA  lpMemPTD;     // Ptr to PerTabData global memory
    LPSIS_HEADER  lpSISCur;     // Ptr to current SIS header
    UINT          uErrMsgLen,   // Error message length
                  uNameLen,     // Length of function name[line #]
                  uErrLinLen,   // Error line length
                  uCaretLen,    // Caret line length
                  uTailLen,     // Length of line tail
                  uMaxLen;      // Maximum length
    LPMEMTXT_UNION lpMemTxtLine = NULL; // Ptr to text header/line global memory

#define ERROR_CARET     UTF16_UPCARET   // UTF16_CIRCUMFLEX

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Clear any semaphore to signal
    lpSISCur = lpMemPTD->lpSISCur;
    if (lpSISCur)
        lpSISCur->hSigaphore = NULL;

    // Split cases based upon the DfnType
    if (lpSISCur)
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
            {
                LPAPLCHAR    lpMemName;         // Ptr to function name global memory
                LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header global memory
                LPFCNLINE    lpFcnLines;        // Ptr to array function line structs (FCNLINE[numFcnLines])

                // Include the function name

                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (lpSISCur->hGlbFcnName);

                // Format the name and line #
                uNameLen =
                  wsprintfW (lpwszTemp,
                             L"%s[%d] ",
                             lpMemName,
                             lpSISCur->CurLineNum);
                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLock (lpSISCur->hGlbDfnHdr);

                // Get a ptr to the line # in error
                if (lpSISCur->CurLineNum EQ 0)
                    lpMemTxtLine = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);
                else
                {
                    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

                    // Get a ptr to the function line, converting to origin-0 from origin-1
                    lpMemTxtLine = MyGlobalLock (lpFcnLines[lpSISCur->CurLineNum - 1].hGlbTxtLine);
                } // End IF/ELSE

                // Ptr to the text
                lpwszLine = &lpMemTxtLine->C;

                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbDfnHdr); lpMemDfnHdr = NULL;

                break;
            } // End DFNTYPE_OP1/OP2/FCN

            case DFNTYPE_EXEC:
                // Include a leading marker

                lstrcpyW (lpwszTemp, WS_UTF16_UPTACKJOT L"     ");
                uNameLen = 6;

                break;

            case DFNTYPE_IMM:       // No action
            case DFNTYPE_QUAD:      // ...
                uNameLen = 0;

                break;

            case DFNTYPE_QQUAD:
            defstop
                break;
        } // End SWITCH
    else
        uNameLen = 0;

    // Calculate the various lengths
    uErrMsgLen = lstrlenW (lpwszMsg);
    uErrLinLen = lstrlenW (lpwszLine);
    if (uCaret EQ NEG1U)
        uCaretLen = 0;
    else
        uCaretLen  = uNameLen + uCaret;

    // Calculate the length of the []DM vector
    aplNELMRes = uErrMsgLen
               + lstrlenW (L"\r\n")
               + uNameLen
               + uErrLinLen
               + ((uCaret EQ NEG1U) ? 0
                                    : lstrlenW (L"\r\n")
                                    + uCaretLen + 1);
    // Calculate space needed for te result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

    // Allocate space for the result
    // N.B.  Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        MessageBoxW (hWndSM,
                     L"Unable to allocate space for " WS_UTF16_QUAD L"DM",
                     lpwszAppName,
                     MB_OK | MB_ICONWARNING | MB_APPLMODAL);
        return;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;   // Already zero from GHND
////lpHeader->SysVar     = 0;   // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Copy the error message to the result
    CopyMemory (lpMemRes, lpwszMsg, uErrMsgLen * sizeof (APLCHAR));
    lpMemRes += uErrMsgLen;

    // Copy a line terminator to the result
    *lpMemRes++ = L'\r'; *lpMemRes++ = L'\n';

    // Copy the function name[line #] to the result
    CopyMemory (lpMemRes, lpwszTemp, uNameLen * sizeof (APLCHAR));
    lpMemRes += uNameLen;

    // Copy the function line to the result
    CopyMemory (lpMemRes, lpwszLine, uErrLinLen * sizeof (APLCHAR));
    lpMemRes += uErrLinLen;

    // If the caret is not -1, display a caret
    if (uCaret NE NEG1U)
    {
        UINT uLen;  // Length

        // Close the last line
        *lpMemRes++ = L'\r'; *lpMemRes++ = L'\n';

        // Get the # leading blanks
        uLen = uCaret + uNameLen;

        // Append the caret
        lpMemRes = FillMemoryW (lpMemRes, uLen, L' ');
       *lpMemRes++ = ERROR_CARET;
///////*lpMemRes++ = L'\0';                 // Already zero from (GHND)
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Free the old value
    FreeResultGlobalVar (lpMemPTD->hGlbQuadDM);

    // Save the new value in the PTD
    lpMemPTD->hGlbQuadDM = hGlbRes; hGlbRes = NULL;

    // Also create the corresponding value for []EM

    // If there's no user-defined function/operator at or
    //   above the current level, the default
    //   value for []EM is already in place
    while (lpSISCur
        && lpSISCur->DfnType NE DFNTYPE_FCN
        && lpSISCur->DfnType NE DFNTYPE_OP1
        && lpSISCur->DfnType NE DFNTYPE_OP2)
        lpSISCur = lpSISCur->lpSISPrv;

    if (lpSISCur)
    {
        // Calculate the maximum length
        uMaxLen = max (uErrMsgLen, uNameLen + uErrLinLen);
        uMaxLen = max (uMaxLen, uCaretLen);

        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_CHAR, 3 * uMaxLen, 2);

        // Allocate space for the result
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (hGlbRes)
        {
            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = ARRAY_CHAR;
////////////lpHeader->Perm       = 0;               // Already zero from GHND
////////////lpHeader->SysVar     = 0;               // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = 3 * uMaxLen;
            lpHeader->Rank       = 2;
#undef  lpHeader

            // Skip over the header to the dimensions
            lpMemRes = (LPAPLCHAR) VarArrayBaseToDim (lpMemRes);

            // Fill in the result's dimensions
            *((LPAPLDIM) lpMemRes)++ = 3;
            *((LPAPLDIM) lpMemRes)++ = uMaxLen;

            // lpMemRes now points to the data

            // Copy the error message text to the result
            CopyMemory (lpMemRes, lpwszMsg, uErrMsgLen * sizeof (APLCHAR));
            lpMemRes += uErrMsgLen;
            uTailLen = uMaxLen - uErrMsgLen;
            lpMemRes = FillMemoryW (lpMemRes, uTailLen, L' ');

            // Copy the function name[line #] to the result
            CopyMemory (lpMemRes, lpwszTemp, uNameLen * sizeof (APLCHAR));
            lpMemRes += uNameLen;

            // Copy the function line to the result
            CopyMemory (lpMemRes, lpwszLine, uErrLinLen * sizeof (APLCHAR));
            lpMemRes += uErrLinLen;
            uTailLen = uMaxLen - (uNameLen + uErrLinLen);
            lpMemRes = FillMemoryW (lpMemRes, uTailLen, L' ');

            // Copy the caret line to the result
            if (uCaret NE NEG1U)
            {
                lpMemRes = FillMemoryW (lpMemRes, uCaretLen, L' ');
               *lpMemRes++ = ERROR_CARET;
                uTailLen = uMaxLen - (uCaretLen + 1);
                lpMemRes = FillMemoryW (lpMemRes, uTailLen, L' ');
            } else
                lpMemRes = FillMemoryW (lpMemRes, uMaxLen, L' ');

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // Free the old value of []EM
            FreeResultGlobalVar (lpSISCur->hGlbQuadEM);

            // Save the global in the current SIS header
            lpSISCur->hGlbQuadEM = hGlbRes;
        } else
            lpSISCur->hGlbQuadEM = hGlbM3x0Char;
    } // End IF

    if (lpMemTxtLine)
    {
        // We no longer need this ptr
        MyGlobalUnlock (GlobalHandle (lpMemTxtLine));
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End ErrorMessageDirect
#undef  ERROR_CARET
#undef  APPEND_NAME


//***************************************************************************
//  $ErrorMessageIndirect
//
//  Signal an error message, indirectly
//***************************************************************************

void ErrorMessageIndirect
    (LPWCHAR lpwszMsg)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Save in global for later reference
    lpMemPTD->lpwszErrorMessage = lpwszMsg;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
} // End ErrorMessageIndirect


//***************************************************************************
//  $ErrorMessageIndirectToken
//
//  Signal an error message, indirectly
//    and set the error token
//***************************************************************************

void ErrorMessageIndirectToken
    (LPAPLCHAR lpwszMsg,            // Ptr to error message
     LPTOKEN   lptkError)           // Ptr to function token

{
    // Set the error message
    ErrorMessageIndirect (lpwszMsg);

    // Set the error token
    ErrorMessageSetToken (lptkError);
} // End ErrorMessageIndirectToken


//***************************************************************************
//  $ErrorMessageSetToken
//
//  Set the error token for an error message
//***************************************************************************

void ErrorMessageSetToken
    (LPTOKEN lptkError)             // Ptr to error token (may be NULL)

{
    // Set the error char index
    ErrorMessageSetCharIndex ((lptkError NE NULL) ? lptkError->tkCharIndex
                                                  : NEG1U);
} // End ErrorMessageSetToken


//***************************************************************************
//  $ErrorMessageSetCharIndex
//
//  Set the error char index for an error message
//***************************************************************************

void ErrorMessageSetCharIndex
    (int tkCharIndex)               // Error char index

{
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Set the error char index
    lpplLocalVars->tkErrorCharIndex = tkCharIndex;
} // End ErrorMessageSetCharIndex


//***************************************************************************
//  End of File: errmsg.c
//***************************************************************************
