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
//  $ErrorMessage
//
//  Signal an error message
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ErrorMessage"
#else
#define APPEND_NAME
#endif

void ErrorMessage
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
    UINT          uNameLen;     // Length of function name[line #]
    LPMEMTXTUNION lpMemTxtLine; // Ptr to text header/line global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Mark as suspended
    DbgBrk ();
    if (lpMemPTD->lpSISCur)
        lpMemPTD->lpSISCur->Suspended = 1;

    // If we're to include the function name, ...
    if (lpMemPTD->execState.exType EQ EX_DFN)
    {
        LPAPLCHAR     lpMemName;        // Ptr to function name global memory
        LPDFN_HEADER  lpMemDfnHdr;      // Ptr to defined function header global memory
        LPFCNLINE     lpFcnLines;       // Ptr to array function line structs (FCNLINE[numFcnLines])

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLock (lpMemPTD->lpSISCur->hGlbFcnName);

        // Format the name and line #
        uNameLen =
        wsprintfW (lpwszTemp,
                   L"%s[%d] ",
                   lpMemName,
                   lpMemPTD->lpSISCur->CurLineNum);
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->lpSISCur->hGlbFcnName); lpMemName = NULL;

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (lpMemPTD->lpSISCur->hGlbDfnHdr);

        // Get a ptr to the line # in error
        if (lpMemPTD->lpSISCur->CurLineNum EQ 0)
            lpMemTxtLine = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);
        else
        {
            // Get ptr to array of function line structs (FCNLINE[numFcnLines])
            lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

            // Get a ptr to the function line, converting to origin-0 from origin-1
            lpMemTxtLine = MyGlobalLock (lpFcnLines[lpMemPTD->lpSISCur->CurLineNum - 1].hGlbTxtLine);
        } // End IF/ELSE

        // Ptr to the text
        lpwszLine = &lpMemTxtLine->C;

        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->lpSISCur->hGlbDfnHdr); lpMemDfnHdr = NULL;
    } else
        uNameLen = 0;

    // Calculate the length of the vector
    aplNELMRes = lstrlenW (lpwszMsg)
               + lstrlenW (L"\r\n")
               + uNameLen
               + lstrlenW (lpwszLine)
               + ((uCaret EQ NEG1U) ? 0
                                    : lstrlenW (L"\r\n")
                                    + uCaret + uNameLen + 1);
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
    lstrcpyW (lpMemRes, lpwszMsg); lpMemRes += lstrlenW (lpwszMsg);
    *lpMemRes++ = L'\r'; *lpMemRes++ = L'\n';
    if (uNameLen)
    {
        CopyMemory (lpMemRes, lpwszTemp, uNameLen * sizeof (APLCHAR));
        lpMemRes += uNameLen;
    } // End IF
    lstrcpyW (lpMemRes, lpwszLine); lpMemRes += lstrlenW (lpwszLine);

    // If the caret is not -1, display a caret
    if (uCaret NE NEG1U)
    {
        UINT    u;

        // Close the last line
        *lpMemRes++ = L'\r'; *lpMemRes++ = L'\n';

        // Append the caret
        for (u = 0; u < (uCaret + uNameLen); u++)
            *lpMemRes++ = L' ';
        *lpMemRes++ = UTF16_CIRCUMFLEX;     // UTF16_UPCARET;
////////*lpMemRes++ = L'\0';                // Already zero from (GHND)
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Free the old value
    FreeResultGlobalVar (lpMemPTD->hGlbQuadDM);

    // Save the new value in the STE
    lpMemPTD->hGlbQuadDM = hGlbRes;

////if (lpMemPTD->bDispFcnName)
    if (lpMemPTD->execState.exType EQ EX_DFN)
    {
        // We no longer need this ptr
        MyGlobalUnlock (GlobalHandle (lpMemTxtLine));
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    //***************************************************************
    // ***FIXME*** -- Signal an error which should execute []ELX
    //                which will then display this text
    //                (if []ELX is '[]DM').
    //***************************************************************

////#if (defined (DEBUG)) && (defined (EXEC_TRACE))
    { // ***DEBUG***
        DisplayGlbArr (hGlbRes, TRUE);
    } // ***DEBUG*** END
////#endif
} // End ErrorMessage
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
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

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
    (WCHAR   *lpwszMsg,
     LPTOKEN  lptkError)

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
    LPPLLOCALVARS lpplLocalVars;    // Ptr to local plLocalVars

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Set the error token
    if (lptkError)
        lpplLocalVars->tkErrorCharIndex = lptkError->tkCharIndex;
    else
        lpplLocalVars->tkErrorCharIndex = NEG1U;
} // End ErrorMessageSetToken


//***************************************************************************
//  End of File: errmsg.c
//***************************************************************************
