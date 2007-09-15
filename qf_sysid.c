//***************************************************************************
//  NARS2000 -- System Function -- Quad SYSID
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnSYSID_EM
//
//  System function:  []SYSID -- System Identifier
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnSYSID_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnSYSID_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    UINT         ByteRes;           // # bytes in the result
    HGLOBAL      hGlbRes;           // Result global memory handle
    LPVOID       lpMemRes;          // Ptr to result global memory
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

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

#define SYSID   L"NARS2000"
#define SYSID_NELM    (sizeof (SYSID) / sizeof (APLCHAR) - 1)

    // Calculate space needed for the result
    ByteRes = (UINT) CalcArraySize (ARRAY_CHAR, SYSID_NELM, 1);

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->Perm       = 0;           // Already zero from GHND
////lpHeader->SysVar     = 0;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = SYSID_NELM;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemRes) = SYSID_NELM;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // Copy the SYSID to the result
    CopyMemory (lpMemRes, SYSID, SYSID_NELM * sizeof (APLCHAR));

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End SysFnSYSID_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_sysid.c
//***************************************************************************
