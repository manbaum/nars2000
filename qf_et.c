//***************************************************************************
//  NARS2000 -- System Function -- Quad ET
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"
#include "sis.h"
#include "dfnhdr.h"
#include "eventtypes.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnET_EM_YY
//
//  System function:  []ET -- Event Type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnET_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnET_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token  (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS header
    HGLOBAL      hGlbRes = NULL;    // Result ...
    LPAPLUINT    lpMemRes = NULL;   // Ptr to result global memory
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    EVENT_TYPES  EventType;         // Event type
    APLUINT      ByteRes;           // # bytes in the result

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    // Niladic functions cannot have axis operator
    Assert (lptkAxis EQ NULL);

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get ptr to current SIS header
    lpSISCur = lpMemPTD->lpSISCur;
        while (lpSISCur
        && lpSISCur->DfnType NE DFNTYPE_FCN
        && lpSISCur->DfnType NE DFNTYPE_OP1
        && lpSISCur->DfnType NE DFNTYPE_OP2)
        lpSISCur = lpSISCur->lpSISPrv;

    if (lpSISCur)
        EventType = lpSISCur->EventType;
    else
        EventType = EVENTTYPE_NOERROR;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, 2, 1);

    // Allocate space for the result
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->Perm       = 0;               // Already zero from GHND
////lpHeader->SysVar     = 0;               // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 2;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimension
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    *((LPAPLDIM) lpMemRes)++ = 2;

    // lpMemRes now points to the data

    *lpMemRes++ = ET_MAJOR (EventType);
    *lpMemRes++ = ET_MINOR (EventType);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
    return lpYYRes;
} // End SysFnET_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_et.c
//***************************************************************************
