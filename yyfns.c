//***************************************************************************
//  NARS2000 -- YYSTYPE Functions
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "dfnhdr.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $YYAlloc
//
//  Allocate a new YYRes entry
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- YYAlloc"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE YYAlloc
    (void)

{
    UINT         u;             // Loop counter
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;// Ptr to the result

#ifdef DEBUG
    static UINT YYIndex = 0;
#endif

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Search for an empty YYRes slot,
    //   zero it,
    //   mark it as inuse,
    //   and return a ptr to it
    for (u = 0; u < lpMemPTD->numYYRes; u++)
    if (!lpMemPTD->lpYYRes[u].YYInuse)
    {
        // Set the return value
        lpYYRes = &lpMemPTD->lpYYRes[u];

#ifdef DEBUG
        if (YYIndex EQ 0x168F)
            DbgBrk ();
#endif
        // Zero the memory
        ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));

        // Mark as inuse
        lpYYRes->YYInuse = 1;
#ifdef DEBUG
        lpYYRes->SILevel = lpMemPTD->SILevel;   // Save the SI Level
        lpYYRes->YYFlag = 0;  // Mark as a YYAlloc Index

        // Save unique number for debugging/tracking purposes
        lpYYRes->YYIndex = ++YYIndex;
#endif
        goto NORMAL_EXIT;
    } // End FOR/IF

    // If we get here, we ran out of indices
    lpYYRes = &lpMemPTD->lpYYRes[u++];
    lpMemPTD->numYYRes = u;
RESTART_EXCEPTION_YYALLOC:
    __try
    {
#ifdef DEBUG
        if (YYIndex EQ 0x168F)
            DbgBrk ();
#endif
        // Zero the memory
        ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));

        // Mark as inuse
        lpYYRes->YYInuse = 1;
#ifdef DEBUG
        lpYYRes->SILevel = lpMemPTD->SILevel;   // Save the SI Level
        lpYYRes->YYFlag = 0;  // Mark as a YYAlloc Index

        // Save unique number for debugging/tracking purposes
        lpYYRes->YYIndex = ++YYIndex;
#endif
    } __except (CheckException (GetExceptionInformation ()))
    {
#ifdef DEBUG
        dprintfW (L"!!Initiating Exception in " APPEND_NAME L": %08X (%S#%d)", MyGetExceptionCode (), FNLN);
#endif
        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_ACCESS_VIOLATION:
            {
                MEMORY_BASIC_INFORMATION mbi;

                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                // See how many pages are already allocated
                VirtualQuery (lpMemPTD->lpYYRes,
                             &mbi,
                              sizeof (mbi));

                // Check for no allocation as yet
                if (mbi.State EQ MEM_RESERVE)
                    mbi.RegionSize = 0;

                // Allocate more memory to the YYRes buffer
                if (VirtualAlloc (lpMemPTD->lpYYRes,
                                  mbi.RegionSize + DEF_YYRES_INCRSIZE * sizeof (PL_YYSTYPE),
                                  MEM_COMMIT,
                                  PAGE_READWRITE) NE NULL)
                    goto RESTART_EXCEPTION_YYALLOC;

                // Fall through to never-never-land

            } // End EXCEPTION_ACCESS_VIOLATION

            defstop
                break;
        } // End SWITCH
    } // End __try/__except
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpYYRes;
} // End YYAlloc
#undef  APPEND_NAME


//***************************************************************************
//  $YYCopy
//
//  Copy one PL_YYSTYPE to another
//    retaining the destination Inuse, Flag, and Index
//***************************************************************************

void YYCopy
    (LPPL_YYSTYPE lpYYDst,
     LPPL_YYSTYPE lpYYSrc)

{
#ifdef DEBUG
    UINT YYIndex,
         YYFlag;
#endif

    Assert (lpYYDst->YYInuse);

#ifdef DEBUG
    // Save the old index & flag
    YYIndex = lpYYDst->YYIndex;
    YYFlag  = lpYYDst->YYFlag;
#endif
    *lpYYDst = *lpYYSrc;            // Copy the PL_YYSTYPE
    lpYYDst->YYInuse = 1;           // Retain YYInuse flag
#ifdef DEBUG
    lpYYDst->YYIndex = YYIndex;     // Retain YYIndex
    lpYYDst->YYFlag  = YYFlag;      // ...    YYFlag
#endif
} // End YYCopy


//***************************************************************************
//  $YYCopyFreeDst
//
//  Copy one PL_YYSTYPE to another
//    retaining the destination Inuse, Flag, and Index
//    and free the destination copy if it's not inuse.
//***************************************************************************

void YYCopyFreeDst
    (LPPL_YYSTYPE lpYYDst,
     LPPL_YYSTYPE lpYYSrc)

{
    if (lpYYDst->YYInuse)
        YYCopy (lpYYDst, lpYYSrc);
    else
    {
        lpYYDst->YYInuse = 1;       // Mark as in use for YYCopy
        YYCopy (lpYYDst, lpYYSrc);
        lpYYDst->YYInuse = 0;       // Mark as no longer in use
    } // End IF/ELSE
} // End YYCopyFreeDst


//***************************************************************************
//  $YYFree
//
//  Free a YYRes entry
//***************************************************************************

void YYFree
    (LPPL_YYSTYPE lpYYRes)      // Ptr to the YYRes entry

{
#ifdef DEBUG                    // ***DEBUG***
    UINT         u;             // Index into lpMemPTD->YYRes
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    u = lpYYRes - lpMemPTD->lpYYRes;
    Assert (u < lpMemPTD->numYYRes);
    Assert (lpYYRes->YYInuse);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
#endif
////lpYYRes->YYInuse = 0;       // Free it
    ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));
} // End YYFree


#ifdef DEBUG
//***************************************************************************
//  $YYResIsEmpty
//
//  Ensure that YYRes has no Inuse entries
//***************************************************************************

BOOL YYResIsEmpty
    (void)

{
    UINT         u;             // Loop counter
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet = TRUE;   // TRUE iff result is valid

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Loop through the YYRes entries looking for
    //   entries in use at this SI Level
    for (u = 0; u < lpMemPTD->numYYRes; u++)
    if (lpMemPTD->lpYYRes[u].YYInuse
     && lpMemPTD->SILevel EQ lpMemPTD->lpYYRes[u].SILevel)
    {
        bRet = FALSE;

        break;
    } // End FOR/IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End YYResIsEmpty
#endif


//***************************************************************************
//  $YYCopyFcn
//
//  Copy one or more PL_YYSTYPE functions to a memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- YYCopyFcn"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE YYCopyFcn
    (LPPL_YYSTYPE  lpYYMem,             // Ptr to result memory object
     LPPL_YYSTYPE  lpYYArg,             // Ptr to function arg
     LPPL_YYSTYPE *lpYYBase,            // Ptr to ptr to YY base address
     LPINT         lpFcnCount,          // Ptr to resulting function count
     BOOL          bResUsed)            // TRUE iff the result is used

{
    int          i,
                 iLen,
                 FcnCount,
                 TotalFcnCount = 0;
    PL_YYSTYPE   YYFcn = {0};
    HGLOBAL      hGlbData;
    LPTOKEN      lpToken;
    LPPL_YYSTYPE lpYYMem0,
                 lpYYCopy;

    // Get the token count in this function strand
    iLen = lpYYArg->TknCount;

    // We need to modify the function count in the first token,
    //   so save its address in the array.
    lpYYMem0 = lpYYMem;

    // Loop through the tokens associated with this symbol
    for (i = 0; i < iLen; i++)
    {
#ifdef DEBUG
        LPPL_YYSTYPE lpYYArgI;

        lpYYArgI = &lpYYArg[i];
#endif
        Assert (YYCheckInuse (&lpYYArg[i]));        // ***DEBUG***

        // Calculate the earlier function base
        *lpYYBase = min (*lpYYBase, lpYYArg[i].lpYYFcn);

        // If the function is indirect, recurse
        if (lpYYArg[i].YYIndirect)
        {
            FcnCount = 0;   // Initialize as it is incremented in YYCopyFcn
            lpYYMem = YYCopyFcn (lpYYMem, lpYYArg[i].lpYYFcn, lpYYBase, &FcnCount, TRUE);

            Assert (YYCheckInuse (&lpYYArg[i]));    // ***DEBUG***
        } else
        {
            // Get the function arg token
            lpToken = &lpYYArg[i].tkToken;

            // Special case for named functions/operators
            if (lpToken->tkFlags.TknType EQ TKT_FCNNAMED
             || lpToken->tkFlags.TknType EQ TKT_OP1NAMED
             || lpToken->tkFlags.TknType EQ TKT_OP2NAMED)
            {
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

                // If it's an immediate function/operator, copy it directly
                if (lpToken->tkData.tkSym->stFlags.Imm)
                {
                    YYFcn.tkToken.tkFlags.TknType   = TranslateImmTypeToTknType (lpToken->tkData.tkSym->stFlags.ImmType);
                    YYFcn.tkToken.tkFlags.ImmType   = lpToken->tkData.tkSym->stFlags.ImmType;
////////////////////YYFcn.tkToken.tkFlags.NoDisplay = 0;        // Already zero from = {0}
                    YYFcn.tkToken.tkData.tkLongest  = 0;        // Keep the extraneous data clear
                    YYFcn.tkToken.tkData.tkChar     = lpToken->tkData.tkSym->stData.stChar;
                    YYFcn.tkToken.tkCharIndex       = lpToken->tkCharIndex;
////////////////////YYFcn.tkToken.lptkOrig          = NULL;     // ALready zero from = {0}
////////////////////YYFcn.TknCount                  = lpYYArg[i].TknCount; // (Factored out below)
////////////////////YYFcn.FcnCount                  = FcnCount;            // (Factored out below)
////////////////////YYFcn.YYInuse                   = 0;        // (Factored out below)
////////////////////YYFcn.YYIndirect                = 0;        // Already zero from = {0}
////////////////////YYFcn.YYAvail                   = 0;        // Already zero from = {0}
////////////////////YYFcn.YYIndex                   = 0;        // (Factored out below)
////////////////////YYFcn.YYFlag                    = 0;        // Already zero from = {0}
////////////////////YYFcn.lpYYFcn                   = NULL;     // Already zero from = {0}
                    YYFcn.lpYYStrandBase            = lpYYArg[i].lpYYStrandBase;
                } else
                {
                    // If it's an internal function, ...
                    if (lpToken->tkData.tkSym->stFlags.FcnDir)
                        // Copy the argument
                        YYFcn = lpYYArg[i];
                    else
                    {
                        // Get the global memory handle or function address if direct
                        hGlbData = lpToken->tkData.tkSym->stData.stGlbData;

                        //stData is a valid HGLOBAL function array
                        //   or user-defined function/operator
                        Assert (IsGlbTypeFcnDir (hGlbData)
                             || IsGlbTypeDfnDir (hGlbData));

                        // Increment the reference count in global memory
                        DbgIncrRefCntDir (hGlbData);

                        // Fill in the token
                        YYFcn.tkToken.tkFlags.TknType   = TKT_FCNARRAY;
////////////////////////YYFcn.tkToken.tkFlags.ImmType   = 0;        // Already zero from = {0}
////////////////////////YYFcn.tkToken.tkFlags.NoDisplay = 0;        // Already zero from = {0}
                        YYFcn.tkToken.tkData.tkGlbData  = hGlbData;
                        YYFcn.tkToken.tkCharIndex       = lpToken->tkCharIndex;
////////////////////////YYFcn.tkToken.lptkOrig          = NULL;     // ALready zero from = {0}
////////////////////////YYFcn.TknCount                  = lpYYArg[i].TknCount; // (Factored out below)
////////////////////////YYFcn.FcnCount                  = FcnCount;            // (Factored out below)
////////////////////////YYFcn.YYInuse                   = 0;        // (Factored out below)
////////////////////////YYFcn.YYIndirect                = 0;        // Already zero from = {0}
////////////////////////YYFcn.YYAvail                   = 0;        // Already zero from = {0}
////////////////////////YYFcn.YYIndex                   = 0;        // (Factored out below)
////////////////////////YYFcn.YYFlag                    = 0;        // Already zero from = {0}
////////////////////////YYFcn.lpYYFcn                   = NULL;     // Already zero from = {0}
                        YYFcn.lpYYStrandBase            = lpYYArg[i].lpYYStrandBase;
                    } // End IF
                } // End IF/ELSE

                YYFcn.TknCount = lpYYArg[i].TknCount;
                YYFcn.YYInuse  = 0;
#ifdef DEBUG
                YYFcn.YYIndex  = NEG1U;
#endif
                // Copy the destination YYInuse flag in case it's
                //   in the YYRes allocated region
                YYFcn.YYInuse = lpYYMem->YYInuse;

                // Copy to the destination
                *lpYYMem++ = YYFcn;
                Assert (YYCheckInuse (&lpYYMem[-1]));   // ***DEBUG***
            } else
            {
                lpYYCopy = CopyPL_YYSTYPE_EM_YY (&lpYYArg[i], FALSE);
                if (lpYYMem->YYInuse)
                    YYCopy (lpYYMem++, lpYYCopy);
                else
                    YYCopyFreeDst (lpYYMem++, lpYYCopy);
                Assert (YYCheckInuse (&lpYYMem[-1]));   // ***DEBUG***
                YYFree (lpYYCopy); lpYYCopy = NULL;
            } // End IF/ELSE

            // Save the function count
            FcnCount = 1;
            lpYYMem[-1].FcnCount = FcnCount;
        } // End IF/ELSE

        // Accumulate into the total function count
        TotalFcnCount += FcnCount;
    } // End FOR

    // Save the total function count in the first token
    lpYYMem0->FcnCount = TotalFcnCount;

    // Return as the overall total
    *lpFcnCount = TotalFcnCount;

#ifdef DEBUG
    lpYYArg[0].FcnCount = TotalFcnCount;

    if (bResUsed)
        Assert (YYCheckInuse (lpYYMem));            // ***DEBUG***
#endif

    return lpYYMem;
} // End YYCopyFcn
#undef  APPEND_NAME


#ifdef DEBUG
//***************************************************************************
//  $YYCheckInuse
//
//  Debug routine to check YYInuse flag
//***************************************************************************

BOOL YYCheckInuse
    (LPPL_YYSTYPE lpYYRes)

{
    HGLOBAL      hGlbPTD;       // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    BOOL         bRet;          // Return value

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If it's within the allocated range, ...
    if (lpMemPTD->lpYYRes <= lpYYRes
     &&                      lpYYRes < &lpMemPTD->lpYYRes[lpMemPTD->numYYRes])
        bRet = lpYYRes->YYInuse;
    else
        bRet = TRUE;
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End YYCheckInuse
#endif


//***************************************************************************
//  End of File: yyfns.c
//***************************************************************************
