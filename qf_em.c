//***************************************************************************
//  NARS2000 -- System Function -- Quad EM
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

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnEM_EM_YY
//
//  System function:  []EM -- Event Message
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnEM_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnEM_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token  (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS header
////HGLOBAL      hGlbQDM = NULL;    // []DM   ...
    HGLOBAL      hGlbRes = NULL;    // Result ...
////LPAPLCHAR    lpMemQDM = NULL,   // Ptr to []DM global memory
////             lpMemRes = NULL;   // Ptr to result ...
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
////APLNELM      aplNELMQDM;        // []DM NELM
////APLRANK      aplRankQDM;        // []DM rank
////APLUINT      uQDM,              // Loop counter
////             uMaxLen,           // Maximum line length in []DM
////             uNumLines,         // # lines             ...
////             uTailLen,          // Tail line length
////             uLastCRLF,         // Index of char after the last CR/LF pair
////             ByteRes;           // # bytes in the result

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
        hGlbRes = CopySymGlbDirGlb (MakeGlbTypeGlb (lpSISCur->hGlbQuadEM));
    else
        hGlbRes = hGlbM3x0Char;

    // Allocate a YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnEM_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_em.c
//***************************************************************************
