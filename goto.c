//***************************************************************************
//  NARS2000 -- Goto Functions
//***************************************************************************

#define STRICT
#include <windows.h>

#include "aplerrors.h"
#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $GotoLine
//
//  Handle {goto} LineNum
//
//  Return GOTO_ZILDE if we're going to {zilde}
//         GOTO_LINE  if we're going to a valid line num
//         GOTO_ERROR if error
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GotoLine"
#else
#define APPEND_NAME
#endif

GOTO_TYPES GotoLine
    (LPTOKEN lptkRhtArg,        // Ptr to right arg token
     LPTOKEN lptkFunc)          // Ptr to function token

{
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    GOTO_TYPES   gotoType;          // Return value
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht;        // ...       NELM
    APLRANK      aplRankRht;        // ...       rank
    UCHAR        immType;           // Right arg first value immediate type
    APLINT       aplIntegerRht;     // First value as integer
    APLFLOAT     aplFloatRht;       // ...            float

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Check for RANK ERROR
    if (aplRankRht > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Get the first value
    FirstValue (lptkRhtArg,         // Ptr to right arg token
               &aplIntegerRht,      // Ptr to integer result
               &aplFloatRht,        // Ptr to float ...
                NULL,               // Ptr to WCHAR ...
                NULL,               // Ptr to longest ...
                NULL,               // Ptr to lpSym/Glb ...
               &immType,            // Ptr to ...immediate type ...
                NULL);              // Ptr to array type ...
    if (immType EQ IMMTYPE_ERROR
     || immType EQ IMMTYPE_CHAR)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    if (immType EQ IMMTYPE_FLOAT)
    {
        BOOL bRet;

        aplIntegerRht = FloatToAplint_SCT (aplFloatRht, &bRet);
        if (!bRet)
        {
            ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                       lptkFunc);
            goto ERROR_EXIT;
        } // End IF
    } // End IF

    // If the right arg is empty or out of range, ...
    if (aplNELMRht EQ 0
     || aplIntegerRht < 0
     || aplIntegerRht > 0x7FFFFFFF)
    {
        gotoType = GOTO_ZILDE;

        goto NORMAL_EXIT;
    } // End IF

    // Split cases based upon the execution mode
    switch (lpMemPTD->execState.exType)
    {
        case EX_IMM:
            // Restart execution in a suspended function
            DbgBrk ();      // ***FINISHME*** -- goto LineNum in EX_IMM

            // If there's a suspended function, ...
            if (lpMemPTD->lpSISCur)
            {
                // Save as the next line # ("- 1" because we pre-increment
                //   the value in <ExecuteFunction>).
                lpMemPTD->lpSISCur->CurLineNum = (UINT) aplIntegerRht - 1;

                // Mark as having been specified by {goto}
                lpMemPTD->lpSISCur->GotoLine = TRUE;

                // Mark as no longer suspended
                lpMemPTD->lpSISCur->Suspended = 0;

                gotoType = GOTO_LINE;
            } else
                gotoType = GOTO_ZILDE;





            break;

        case EX_DFN:
            // If there's a suspended function, ...
            if (lpMemPTD->lpSISCur)
            {
                // Save as the next line # ("- 1" because we pre-increment
                //   the value in <ExecuteFunction>).
                lpMemPTD->lpSISCur->CurLineNum = (UINT) aplIntegerRht - 1;

                // Mark as having been specified by {goto}
                lpMemPTD->lpSISCur->GotoLine = TRUE;

                // Mark as no longer suspended
                lpMemPTD->lpSISCur->Suspended = 0;

                gotoType = GOTO_LINE;
            } else
                gotoType = GOTO_ZILDE;
            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

ERROR_EXIT:
    gotoType = GOTO_ERROR;
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return gotoType;
} // End GotoLine
#undef  APPEND_NAME


//***************************************************************************
//  $GotoReset
//
//  Handle {goto}
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GotoReset"
#else
#define APPEND_NAME
#endif

void GotoReset
    (void)

{
    DbgBrk ();          // ***FINISHME*** -- GotoReset


} // End GotoReset
#undef  APPEND_NAME


//***************************************************************************
//  End of File: goto.c
//***************************************************************************
