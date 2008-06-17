//***************************************************************************
//  NARS2000 -- Primitive Function -- UpTackJot
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
#include <math.h>

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


UTJ_THREAD utjThread;           // Temporary global


//***************************************************************************
//  $PrimFnUpTackJot_EM_YY
//
//  Primitive function for monadic and dyadic UpTackJot ("execute" and ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPTACKJOT);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkAxis);
        return NULL;
    } // End IF

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonUpTackJot_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydUpTackJot_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnUpTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnUpTackJot_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic UpTackJot
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnUpTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnUpTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnUpTackJot_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnUpTackJot_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTackJot_EM_YY
//
//  Primitive function for monadic UpTackJot ("execute")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonUpTackJot_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLNELM  aplNELMRht;            // Right arg NELM
    APLRANK  aplRankRht;            // Right arg rank
    HGLOBAL  hGlbRht;               // Right arg global memory handle

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for RIGHT DOMAIN ERROR
    if ((!IsSimpleChar (aplTypeRht)) && !IsEmpty (aplNELMRht))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Check for empty case
    if (IsEmpty (aplNELMRht))
        // Return PL_YYSTYPE NoValue entry
        return MakeNoValue_YY (lptkFunc);

    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory handle
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbRht));

                break;          // Join common global code
            } // End IF

            // Handle the immediate case
            return PrimFnMonUpTackJotImm_EM_YY
                   (lptkRhtArg->tkData.tkSym->stData.stLongest, // Immediate value
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARIMMED:
            return PrimFnMonUpTackJotImm_EM_YY
                   (lptkRhtArg->tkData.tkLongest,               // Immediate value
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            // Get the global memory handle
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            break;          // Join common global code

        defstop
            return NULL;
    } // End SWITCH

    return PrimFnMonUpTackJotGlb_EM_YY (ClrPtrTypeDirAsGlb (hGlbRht),   // HGLOBAL
                                        lptkAxis,                       // Ptr to axis token (may be NULL)
                                        lptkFunc);                      // Ptr to function token
} // End PrimFnMonUpTackJot_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTackJotImm_EM_YY
//
//  Monadic UpTackJot ("execute") on an immediate value
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpTackJotImm_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonUpTackJotImm_EM_YY
    (APLLONGEST    aplLongest,      // Ptr to right arg value
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPTOKEN       lptkFunc)        // Ptr to function token

{
    LPAPLCHAR lpwszCompLine;        // Ptr to the line to execute

    // Allocate space for the immediate value
    lpwszCompLine =
      MyVirtualAlloc (NULL,         // Any address (FIXED SIZE)
                      (1 + 1) * sizeof (WCHAR), // "+ 1" for the terminating zero
                      MEM_COMMIT | MEM_TOP_DOWN,
                      PAGE_READWRITE);
    if (!lpwszCompLine)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;                // Mark as failed
    } // End IF

    // Save the char in the line
    lpwszCompLine[0] = (APLCHAR) aplLongest;
    lpwszCompLine[1] = L'\0';

    return PrimFnMonUpTackJotCommon_EM_YY (lpwszCompLine, TRUE, lptkFunc);
} // End PrimFnMonUpTackJotImm_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTackGlb_EM_YY
//
//  Monadic UpTackJot ("execute") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpTackJotGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonUpTackJotGlb_EM_YY
    (HGLOBAL hGlbRht,               // Handle to right arg
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    LPAPLCHAR lpwszCompLine;        // Ptr to the line to execute
    LPAPLCHAR lpMemRht;             // Ptr to right arg global memory
    APLNELM   aplNELMRht;           // Right arg NELM
    APLRANK   aplRankRht;           // Right arg rank

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the NELM and Rank
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimension
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Allocate space for the global value
    Assert (aplNELMRht EQ (UINT) aplNELMRht);
    lpwszCompLine =
      MyVirtualAlloc (NULL,         // Any address (FIXED SIZE)
                      ((UINT) aplNELMRht + 1) * sizeof (APLCHAR),   // "+ 1" for the terminating zero
                      MEM_COMMIT | MEM_TOP_DOWN,
                      PAGE_READWRITE);
    if (!lpwszCompLine)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;                // Mark as failed
    } // End IF

    // Copy the chars into the line
    Assert (aplNELMRht EQ (UINT) aplNELMRht);
    CopyMemory (lpwszCompLine, lpMemRht, (UINT) aplNELMRht * sizeof (APLCHAR));
    lpwszCompLine[aplNELMRht] = L'\0';

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

    return PrimFnMonUpTackJotCommon_EM_YY (lpwszCompLine, TRUE, lptkFunc);
} // End PrimFnMonUpTackJotGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpTackJotCommon_EM_YY
//
//  Common subroutine to the Imm and Glb cases
//***************************************************************************

LPPL_YYSTYPE PrimFnMonUpTackJotCommon_EM_YY
    (LPAPLCHAR lpwszCompLine,
     BOOL      bFreeCompLine,
     LPTOKEN   lptkFunc)

{
    LPPL_YYSTYPE   lpYYRes;         // Ptr to the result
    HGLOBAL        hGlbPTD;         // PerTabData global memory handle
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Save args in struc to pass to thread func
    // Get hWndEC for the Session Manager from the current thread
    utjThread.hWndEC        = GetThreadSMEC ();
    utjThread.hGlbPTD       = hGlbPTD;
    utjThread.lpwszCompLine = lpwszCompLine;

    PrimFnMonUpTackJotInThread (&utjThread);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If there's no error, ...
    if (lpMemPTD->YYResExec.YYInuse)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Copy the result
        *lpYYRes = lpMemPTD->YYResExec;
        lpYYRes->tkToken.tkCharIndex = lptkFunc->tkCharIndex;
        ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
    } else
        lpYYRes = NULL;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Free the virtual memory for the complete line
    if (bFreeCompLine)
    {
        MyVirtualFree (lpwszCompLine, 0, MEM_RELEASE); lpwszCompLine = NULL;
    } // End IF

    return lpYYRes;
} // PrimFnMonUpTackJotCommon_EM_YY


//***************************************************************************
//  $PrimFnMonUpTackJotInThread
//
//  Monadic UpTackJot ("execute") in a thread
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpTackJotInThread"
#else
#define APPEND_NAME
#endif

DWORD WINAPI PrimFnMonUpTackJotInThread
    (LPUTJ_THREAD lputjThread)          // Ptr to UpTackJotThread struct

{
    HGLOBAL       hGlbPTD;              // PerTabData global memory handle
    LPPERTABDATA  lpMemPTD;             // Ptr to PerTabData global memory
    HWND          hWndSM,               // Session Manager window handle
                  hWndEC;               // Edit Control    ...
    LPAPLCHAR     lpwszCompLine;        // Ptr to the complete line to execute
    DWORD         dwRet = 0;            // Return code from this function
    HGLOBAL       hGlbToken = NULL;     // Tokenized line global memory handle
    HANDLE        hSigaphore = NULL;    // Semaphore handle to signal (NULL if none)
    EXIT_TYPES    exitType;             // Return code from ParseLine
    LPPLLOCALVARS lpplLocalVars;        // Ptr to local plLocalVars

    // Extract values from the arg struc
    hWndEC        = lputjThread->hWndEC;
    hGlbPTD       = lputjThread->hGlbPTD;
    lpwszCompLine = lputjThread->lpwszCompLine;

    // Save the thread's PerTabData global memory handle
    TlsSetValue (dwTlsPerTabData, hGlbPTD);

#ifdef DEBUG
    dprintfW (L"--Starting thread in <PrimFnMonUpTackJotInThread>.");
#endif

    // Get the window handle of the Session Manager
    hWndSM = GetParent (hWndEC);

    // Tokenize, parse, and untokenize the line

    // Tokenize the line
    hGlbToken =
      Tokenize_EM (lpwszCompLine,
                   lstrlenW (lpwszCompLine),
                   hWndEC,
                  &ErrorMessageDirect);
    // If it's invalid, ...
    if (hGlbToken EQ NULL)
    {
        dwRet = 1;          // Mark as failed Tokenize_EM

        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Fill in the SIS header for the Execute primitive
    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                NULL,                   // Semaphore handle
                DFNTYPE_EXEC,           // DfnType
                FCNVALENCE_MON,         // FcnValence
                FALSE,                  // Suspended
                TRUE,                   // Restartable
                TRUE);                  // LinkIntoChain
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Run the parser in a separate thread
    exitType =
      ParseLine (hWndSM,            // Session Manager window handle
                 NULL,              // Line text global memory handle
                 hGlbToken,         // Tokenized line global memory handle
                 lpwszCompLine,     // Ptr to the complete line
                 hGlbPTD,           // PerTabData global memory handle
                 TRUE);             // TRUE iff errors are acted upon
    // Split cases based upon the exit type
    switch (exitType)
    {
        case EXITTYPE_DISPLAY:      // Nothing more to do with these types
        case EXITTYPE_NOVALUE:      // ...
        case EXITTYPE_GOTO_ZILDE:   // ...
        case EXITTYPE_GOTO_LINE:    // ...
        case EXITTYPE_NODISPLAY:    // ...
        case EXITTYPE_ERROR:        // ...
        case EXITTYPE_STOP:         // ...
            break;

        case EXITTYPE_RESET_ONE:
            DbgBrk ();


            break;

        case EXITTYPE_RESET_ONE_INIT:
            DbgBrk ();


            break;

        case EXITTYPE_RESET_ALL:
            DbgBrk ();


            break;

        case EXITTYPE_NONE:
        defstop
            break;
    } // End SWITCH

    // Get this thread's LocalVars ptr
    lpplLocalVars = (LPPLLOCALVARS) TlsGetValue (dwTlsPlLocalVars);

    // Save the exit type in the plLocalVars
    lpplLocalVars->ExitType = exitType;

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Save the semaphore handle to signal after Unlocalize (may be NULL if none)
    hSigaphore = lpMemPTD->lpSISCur->hSigaphore;
    lpMemPTD->lpSISCur->hSigaphore = NULL;

    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    Unlocalize ();

    // If this hSigaphore is not for this level, pass it on up the line
    hSigaphore = PassSigaphore (lpMemPTD, hSigaphore);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // Untokenize the temporary line and free its memory
    Untokenize (hGlbToken);
    MyGlobalFree (hGlbToken); hGlbToken = NULL;
ERROR_EXIT:
#ifdef DEBUG
    dprintfW (L"--Ending   thread in <PrimFnMonUpTackJotInThread>.");
#endif
    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Display the default prompt if there's a suspension
    if (hSigaphore EQ NULL
     && (lpMemPTD->lpSISCur EQ NULL
      || lpMemPTD->lpSISCur->Suspended))
        DisplayPrompt (hWndEC, 5);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    // If there's a semaphore to signal, ...
    if (hSigaphore)
    {
#ifdef DEBUG
        dprintfW (L"~~Releasing semaphore:  %p (%S#%d)", hSigaphore, FNLN);
#endif
        ReleaseSemaphore (hSigaphore, 1, NULL);

        // Release our time slice so the released thread can act
        Sleep (0);
    } // End IF

    return dwRet;
} // End PrimFnMonUpTackJotInThread
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydUpTackJot_EM_YY
//
//  Primitive function for dyadic UpTackJot ("ERROR")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydUpTackJot_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydUpTackJot_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc);
} // End PrimFnDydUpTackJot_EM_YY


//***************************************************************************
//  End of File: pf_utackjot.c
//***************************************************************************
