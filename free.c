//***************************************************************************
//  NARS2000 -- Free Routines
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "resdebug.h"
#include "externs.h"
#include "dfnhdr.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $FreeResultName
//
//  Free named result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultName"
#else
#define APPEND_NAME
#endif

void FreeResultName
    (LPTOKEN lptkRes)

{
    DBGENTER;

    FreeResultSub (lptkRes, TRUE);

    DBGLEAVE;
} // End FreeResultName
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResult
//
//  Free the HGLOBALs and LPSYMENTRYs in a result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResult"
#else
#define APPEND_NAME
#endif

void FreeResult
    (LPTOKEN lptkRes)

{
    DBGENTER;

    FreeResultSub (lptkRes, FALSE);

    DBGLEAVE;
} // End FreeResult
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResultSub
//
//  Free the HGLOBALs and LPSYMENTRYs in a result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultSub"
#else
#define APPEND_NAME
#endif

void FreeResultSub
    (LPTOKEN lptkRes,
     BOOL    bFreeName)

{
    STFLAGS stFlags = {0};
    BOOL    bTmp;
    HGLOBAL hGlbData;

    // Split cases based upon the token type
    switch (lptkRes->tkFlags.TknType)
    {
        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If stData is immediate, exit
            if (lptkRes->tkData.tkSym->stFlags.Imm)
                break;

            // Check for internal functions,
            //   or no value
            if (lptkRes->tkData.tkSym->stFlags.FcnDir
             || !lptkRes->tkData.tkSym->stFlags.Value)
                break;

            // Get the global memory handle
            hGlbData = lptkRes->tkData.tkSym->stData.stGlbData;

            // Data is an valid HGLOBAL named primitive
            //   or user-defined function/operator
            Assert (IsGlbTypeFcnDir (hGlbData)
                 || IsGlbTypeDfnDir (hGlbData));

            // Is it time to free the name?
            if (bFreeName)
            {
                // If it's a user-defined function/operator, ...
                if (lptkRes->tkData.tkSym->stFlags.UsrDfn)
                {
                    // Free the user-defined function/operator
                    if (FreeResultGlobalDfn (hGlbData))
                    {
#ifdef DEBUG_ZAP
                        dprintfW (L"**Zapping in FreeResultSub: Token=%08X, Value=%08X (%S#%d)",
                                  lptkRes,
                                  ClrPtrTypeDir (lptkRes->tkData.tkSym->stData.stGlbData),
                                  FNLN);
#endif
                        lptkRes->tkData.tkSym->stData.stGlbData = NULL;
                    } // End IF
                } else
                {
                    // Free the function array
                    if (FreeResultGlobalFcn (hGlbData))
                    {
#ifdef DEBUG_ZAP
                        dprintfW (L"**Zapping in FreeResultSub: Token=%08X, Value=%08X (%S#%d)",
                                  lptkRes,
                                  ClrPtrTypeDir (lptkRes->tkData.tkSym->stData.stGlbData),
                                  FNLN);
#endif
                        lptkRes->tkData.tkSym->stData.stGlbData = NULL;
                    } // End IF
                } // End IF/ELSE
            } // End IF

            break;

        case TKT_VARNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Check for internal functions,
            //   or no value
            if (lptkRes->tkData.tkSym->stFlags.FcnDir
             || !lptkRes->tkData.tkSym->stFlags.Value)
                break;

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lptkRes->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory ptr
                hGlbData = lptkRes->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable or function array
                //   or user-defined function/operator
                Assert (IsGlbTypeVarDir (hGlbData)
                     || IsGlbTypeFcnDir (hGlbData)
                     || IsGlbTypeDfnDir (hGlbData));

                // The call to FreeResult after ArrayDisplay_EM needs the
                //   following if-statement.

                // Is it time to free the name?
                if (bFreeName)
                {
                    bTmp = FreeResultGlobalDFV (ClrPtrTypeDirGlb (hGlbData));
                    if (bTmp)
                    {
#ifdef DEBUG_ZAP
                        dprintfW (L"**Zapping in FreeResultSub: Token=%08X, Value=%08X (%S#%d)",
                                  lptkRes,
                                  ClrPtrTypeDir (hGlbData),
                                  FNLN);
#endif
                        lptkRes->tkData.tkSym->stData.stGlbData = NULL;
                    } // End IF
                } // End IF
            } // End IF

            // Is it time to free the name?
            if (bFreeName)
            {
                // Set the flags we'll leave alone
                stFlags.Inuse       =
                stFlags.NotCase     =
                stFlags.Perm        = 1;
                stFlags.ObjName     =
                stFlags.SysVarValid = NEG1U;

                // Clear the symbol table flags
                *(PUINT_PTR) &lptkRes->tkData.tkSym->stFlags &= *(PUINT_PTR) &stFlags;
            } // End IF

            return;

        case TKT_VARIMMED:  // tkData is an immediate constant
        case TKT_AXISIMMED: // ...
        case TKT_STRAND:    // Occurs if MakeStrand fails -- data freed by <FreeStrand>
        case TKT_LISTPAR:   // tkData is -1
        case TKT_LISTINT:   // tkData is -1
        case TKT_INPOUT:    // tkData is UTF16_xxx
            return;

        case TKT_VARARRAY:  // tkData contains an HGLOBAL of an array of LPSYMENTRYs and HGLOBALs
        case TKT_AXISARRAY: // ...
        case TKT_FCNARRAY:  // ...
        case TKT_LISTBR:    // ...
            // Get the global memory ptr
            hGlbData = lptkRes->tkData.tkGlbData;

            // Check for ptr reuse
            if (!PtrReusedDir (hGlbData))
            {
                // tkData is a valid HGLOBAL variable or function array
                Assert (IsGlbTypeVarDir (hGlbData)
                     || IsGlbTypeFcnDir (hGlbData)
                     || IsGlbTypeDfnDir (hGlbData));

                bTmp = FreeResultGlobalDFV (ClrPtrTypeDirGlb (hGlbData));
                if (bTmp)
                {
#ifdef DEBUG_ZAP
                    dprintfW (L"**Zapping in FreeResultSub: Token=%08X, Value=%08X (%S#%d)",
                              lptkRes,
                              ClrPtrTypeDir (lptkRes->tkData.tkGlbData),
                              FNLN);
#endif
                    lptkRes->tkData.tkGlbData = NULL;
                } // End IF
            } // End IF

            return;

        case TKT_FCNIMMED:  // tkData is an immediate function
        case TKT_OP1IMMED:  // ...                    monadic operator
        case TKT_OP2IMMED:  // ...                    dyadic  ...
        case TKT_OP3IMMED:  // ...                    ambiguous ...
        case TKT_OPJOTDOT:  // ...                    {jot}{dot}
            return;

        case TKT_STRNAMED:  // tkData contains an HGLOBAL of a strand of names
            DbgGlobalFree (ClrPtrTypeDirGlb (lptkRes->tkData.tkGlbData));

            return;

        defstop
            return;
    } // End SWITCH
} // End FreeResultSub
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResultGlobalDFV
//
//  Free a global defined function, variable, or function array
//***************************************************************************

BOOL FreeResultGlobalDFV
    (HGLOBAL hGlbData)

{
    // Split cases based upon the signature
    switch (GetSignatureGlb (hGlbData))
    {
        case DFN_HEADER_SIGNATURE:
            return FreeResultGlobalDfn (hGlbData);

        case FCNARRAY_HEADER_SIGNATURE:
            return FreeResultGlobalFcn (hGlbData);

        case VARARRAY_HEADER_SIGNATURE:
            return FreeResultGlobalVar (hGlbData);

        defstop
            return FALSE;
    } // End SWITCH
} // End FreeResultGlobalDFV


//***************************************************************************
//  $FreeResultGlobalVar
//
//  Free a global variable, recursively
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultGlobalVar"
#else
#define APPEND_NAME
#endif

BOOL FreeResultGlobalVar
    (HGLOBAL hGlbData)

{
    LPVOID    lpMem;
    APLRANK   aplRank;
    APLSTYPE  aplType;          // The array storage type (see ARRAY_TYPES)
    APLNELM   aplNELM;
    UINT      u,
              RefCnt;
    BOOL      bRet;

    DBGENTER;

    // Data is an valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir (MakeGlbTypeGlb (hGlbData)));

    // Lock the memory to get a ptr to it
    lpMem = MyGlobalLock (hGlbData);

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
    if (!lpHeader->Perm)
    {
        // Get the Type, RefCnt, NELM, and Rank
        aplType = lpHeader->ArrType;
        RefCnt  = lpHeader->RefCnt;
        aplNELM = lpHeader->NELM;
        aplRank = lpHeader->Rank;

        // Ensure non-zero
        Assert (RefCnt > 0);

        // Decrement
        RefCnt = --lpHeader->RefCnt;

#ifdef DEBUG_REFCNT
        dprintfW (L"##RefCnt-- in " APPEND_NAME L": %08X(res=%d) (%S#%d)",
                  ClrPtrTypeDir (hGlbData),
                  RefCnt,
                  FNLN);
#endif
#undef  lpHeader

        if (RefCnt EQ 0)
        // Split cases based upon the array type
        switch (aplType)
        {
            case ARRAY_BOOL:    // Nothing to do
            case ARRAY_INT:     // ...
            case ARRAY_CHAR:    // ...
            case ARRAY_FLOAT:   // ...
            case ARRAY_APA:     // ...
                break;

            case ARRAY_NESTED:  // Free the LPSYMENTRYs and/or HGLOBALs
                // Take into account nested prototypes
                aplNELM = max (aplNELM, 1);

                // Fall through to common handling

            case ARRAY_HETERO:  // Free the LPSYMENTRYs and/or HGLOBALs
            case ARRAY_LIST:    // ...
                // Point to the array data (LPSYMENTRYs and/or HGLOBALs)
                lpMem = VarArrayBaseToData (lpMem, aplRank);

                // Loop through the LPSYMENTRYs and/or HGLOBALs
                for (u = 0; u < aplNELM; u++, ((LPAPLNESTED) lpMem)++)
                {
                    // Check for reused ptrs
                    if (!PtrReusedInd (lpMem))
                    switch (GetPtrTypeInd (lpMem))
                    {
                        case PTRTYPE_STCONST:
                            break;

                        case PTRTYPE_HGLOBAL:
                            if (FreeResultGlobalVar (ClrPtrTypeIndGlb (lpMem)))
                            {
#ifdef DEBUG_ZAP
                                dprintfW (L"**Zapping in FreeResultGlobalVar: Global=%08X, Value=%08X (%S#%d)",
                                          hGlbData,
                                          ClrPtrTypeInd (lpMem),
                                          FNLN);
#endif
                                *((LPVOID *) lpMem) = NULL;
                            } // End IF

                            break;

                        defstop
                            break;
                    } // End IF/SWITCH
                } // End FOR

                break;

            defstop
                break;
        } // End IF/SWITCH
    } else
        RefCnt = 1;     // Any non-zero value to prevent erasure

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMem = NULL;

    // If the RefCnt is zero, free the global
    bRet = (RefCnt EQ 0);

    if (bRet)
    {
        DbgGlobalFree (hGlbData); hGlbData = NULL;
    } // End IF

    DBGLEAVE;

    return bRet;
} // End FreeResultGlobalVar
#undef  APPEND_NAME


//***************************************************************************
//  $FreeResultGlobalFcn
//
//  Free a global function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultGlobalFcn"
#else
#define APPEND_NAME
#endif

BOOL FreeResultGlobalFcn
    (HGLOBAL hGlbData)

{
    LPVOID       lpMemData;     // Ptr to the function array's global memory
    UINT         fcnNELM,       // The # elements in the function array
                 u,             // Loop counter
                 RefCnt;        // Reference count
    BOOL         bRet;          // TRUE iff result is valid
    LPPL_YYSTYPE lpYYToken;     // Ptr to function array token
    HGLOBAL      hGlbLcl,       // Global memory handle
                 hGlbTxtLine;   // Line text gobal memory handle

    DBGENTER;

    // Data is an valid HGLOBAL function array
    Assert (IsGlbTypeFcnDir (MakeGlbTypeGlb (hGlbData)));

    // Clear the ptr type bits
    hGlbData = ClrPtrTypeDirGlb (hGlbData);

    // Lock the memory to get a ptr to it
    lpMemData = MyGlobalLock (hGlbData);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemData)
    // Get the Type, RefCnt, NELM, and line text handle
    RefCnt      = lpHeader->RefCnt;
    fcnNELM     = lpHeader->fcnNELM;
    hGlbTxtLine = lpHeader->hGlbTxtLine;

    // Ensure non-zero
    Assert (RefCnt > 0);

    // Decrement
    RefCnt = --lpHeader->RefCnt;
#undef  lpHeader

#ifdef DEBUG_REFCNT
    dprintfW (L"##RefCnt-- in " APPEND_NAME L": %08X(res=%d) (%S#%d)",
              ClrPtrTypeDir (hGlbData),
              RefCnt,
              FNLN);
#endif

    if (RefCnt EQ 0)
    {
        // Free the line text
        if (hGlbTxtLine)
        {
            DbgGlobalFree (hGlbTxtLine); hGlbTxtLine = NULL;
        } // End IF

        // Point to the array data (PL_YYSTYPEs)
        lpYYToken = FcnArrayBaseToData (lpMemData);

        // Loop through the PL_YYSTYPEs
        for (u = 0; u < fcnNELM; u++, lpYYToken++)
        // Split cases based upon the token type
        switch (lpYYToken->tkToken.tkFlags.TknType)
        {
            case TKT_FCNIMMED:      // Nothing to do
            case TKT_VARIMMED:      // ...
            case TKT_OP1IMMED:      // ...
            case TKT_OP2IMMED:      // ...
            case TKT_OP3IMMED:      // ...
            case TKT_AXISIMMED:     // ...
            case TKT_OPJOTDOT:      // ...
                break;              // Ignore immediates

            case TKT_FCNNAMED:      // Free the named function array
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                // Check for internal functions
                if (lpYYToken->tkToken.tkData.tkSym->stFlags.FcnDir)
                    break;          // Ignore internal functions

                DbgBrk ();          // ***FIXME*** -- Can we ever get here??

                // Fall through to common code

            case TKT_FCNARRAY:      // Free the function array
                // Get the global memory handle
                hGlbLcl = lpYYToken->tkToken.tkData.tkGlbData;

                // tkData is a valid HGLOBAL function array
                //   or user-defined function/operator
                Assert (IsGlbTypeFcnDir (hGlbLcl)
                     || IsGlbTypeDfnDir (hGlbLcl));

                // Clear the ptr type bits
                hGlbLcl = ClrPtrTypeDirGlb (hGlbLcl);

                // Free the function array or user-defined function/operator
                if (FreeResultGlobalDFV (hGlbLcl))
                {
#ifdef DEBUG_ZAP
                    dprintfW (L"**Zapping in FreeResultGlobalFcn: Global=%08X, Value=%08X (%S#%d)",
                              hGlbData,
                              hGlbLcl,
                              FNLN);
#endif
                    lpYYToken->tkToken.tkData.tkGlbData = NULL;
                } // End IF

                break;

            case TKT_VARARRAY:      // Free the var array (strand arg to dyadic op)
            case TKT_AXISARRAY:     // Free the axis array
                // Get the global handle
                hGlbLcl = lpYYToken->tkToken.tkData.tkGlbData;

                // tkData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbLcl));

                // Clear the ptr type bits
                hGlbLcl = ClrPtrTypeDirGlb (hGlbLcl);

                // Free the global variable
                if (FreeResultGlobalVar (hGlbLcl))
                {
#ifdef DEBUG_ZAP
                    dprintfW (L"**Zapping in FreeResultGlobalFcn: Global=%08X, Value=%08X (%S#%d)",
                              hGlbData,
                              hGlbLcl,
                              FNLN);
#endif
                    lpYYToken->tkToken.tkData.tkGlbData = NULL;
                } // End IF

                break;

            case TKT_VARNAMED:
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

                // Get the global handle
                hGlbLcl = lpYYToken->tkToken.tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbLcl));

                // Clear the ptr type bits
                hGlbLcl = ClrPtrTypeDirGlb (hGlbLcl);

                // Free the global variable
                if (FreeResultGlobalVar (hGlbLcl))
                {
#ifdef DEBUG_ZAP
                    dprintfW (L"**Zapping in FreeResultGlobalFcn: Global=%08X, Value=%08X (%S#%d)",
                              hGlbData,
                              hGlbLcl,
                              FNLN);
#endif
                    lpYYToken->tkToken.tkData.tkSym->stData.stGlbData = NULL;
                } // End IF

                break;

            defstop
                break;
        } // End FOR/SWITCH
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMemData = NULL;

    // If the RefCnt is zero, free the global
    bRet = (RefCnt EQ 0);

    if (bRet)
    {
        DbgGlobalFree (hGlbData); hGlbData = NULL;
    } // End IF

    DBGLEAVE;

    return bRet;
} // End FreeResultGlobalFcn
#undef  APPEND_NAME


//***************************************************************************
//  $GetSignatureGlb
//
//  Get the signature of a gloabl object
//***************************************************************************

UINT GetSignatureGlb
    (HGLOBAL hGlbLcl)

{
    LPHEADER_SIGNATURE lpMemLcl;            // Ptr to signature global memory
    UINT               Sig;                 // The signature

    // Lock the memory to get a ptr to it
    lpMemLcl = MyGlobalLock (hGlbLcl);

    // Get the signature
    Sig = lpMemLcl->nature;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLcl); lpMemLcl = NULL;

    return Sig;
} // End GetSignatureGlb


//***************************************************************************
//  $FreeResultGlobalDfn
//
//  Free a global user-defined function/operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultGlobalDfn"
#else
#define APPEND_NAME
#endif

BOOL FreeResultGlobalDfn
    (HGLOBAL hGlbData)

{
    LPDFN_HEADER lpMemDfn;          // Ptr to user-defined function/operator header global memory
    UINT         numFcnLines,       // # lines in the function
                 RefCnt;            // Reference count
    LPFCNLINE    lpFcnLines;        // Ptr to the array of structs (FCNLINE[numFcnLine])

    // Data is an valid HGLOBAL user-defined function/operator
    Assert (IsGlbTypeDfnDir (MakeGlbTypeGlb (hGlbData)));

    // Clear the ptr type bits
    hGlbData = ClrPtrTypeDirGlb (hGlbData);

    // Lock the memory to get a ptr to it
    lpMemDfn = MyGlobalLock (hGlbData);

    // Get the reference count
    RefCnt = lpMemDfn->RefCnt;

    Assert (RefCnt > 0);

    // Decrement it
    // If the RefCnt is zero, free the globals
    if (--RefCnt EQ 0)
    {
        STFLAGS stFlags = {0};

        // Set the flags we'll leave alone
        stFlags.Inuse       =
        stFlags.NotCase     =
        stFlags.Perm        = 1;
        stFlags.ObjName     =
        stFlags.SysVarValid = NEG1U;

        // Clear the symbol table flags for the function name
        *(PUINT_PTR) &lpMemDfn->steFcnName->stFlags &= *(PUINT_PTR) &stFlags;

        // Check the static HGLOBALs
        if (lpMemDfn->hGlbTxtHdr)
        {
            // We no longer need this storage
            DbgGlobalFree (lpMemDfn->hGlbTxtHdr); lpMemDfn->hGlbTxtHdr = NULL;
        } // End IF

        if (lpMemDfn->hGlbTknHdr)
        {
            // Free the tokens
            Untokenize (lpMemDfn->hGlbTknHdr);

            // We no longer need this storage
            DbgGlobalFree (lpMemDfn->hGlbTknHdr); lpMemDfn->hGlbTknHdr = NULL;
        } // End IF

        if (lpMemDfn->hGlbUndoBuff)
        {
            // We no longer need this storage
            DbgGlobalFree (lpMemDfn->hGlbUndoBuff); lpMemDfn->hGlbUndoBuff = NULL;
        } // End IF

        // Get the # lines in the function
        numFcnLines = lpMemDfn->numFcnLines;

        // Get a ptr to the start of the function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfn, lpMemDfn->offFcnLines);

        // Loop through the lines
        while (numFcnLines--)
        {
            if (lpFcnLines->hGlbTxtLine)
            {
                // We no longer need this storage
                DbgGlobalFree (lpFcnLines->hGlbTxtLine); lpFcnLines->hGlbTxtLine = NULL;
            } // End IF

            if (lpFcnLines->hGlbTknLine)
            {
                // Free the tokens
                Untokenize (lpFcnLines->hGlbTknLine);

                // We no longer need this storage
                DbgGlobalFree (lpFcnLines->hGlbTknLine); lpFcnLines->hGlbTknLine = NULL;
            } // End IF

            if (lpFcnLines->hGlbMonInfo)
            {
                DbgBrk ();      // ***FINISHME*** -- Monitor Info in DFN






            } // End IF

            // Skip to the next struct
            lpFcnLines++;
        } // End WHILE
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbData); lpMemDfn = NULL;

    return (RefCnt EQ 0);
} // End FreeResultGlobalDfn
#undef  APPEND_NAME


//***************************************************************************
//  $FreeYYFcn
//
//  Free a strand of YYFcns
//***************************************************************************

void FreeYYFcn
    (LPPL_YYSTYPE lpYYFcn)

{
    UINT u, uCnt;

    // Get the token count
    uCnt = lpYYFcn->TknCount;

    Assert (uCnt NE 0);

    for (u = 0; u < uCnt; u++)
    {
        FreeResult (&lpYYFcn[u].tkToken); YYFree (&lpYYFcn[u]);
    } // End FOR
} // End FreeYYFcn


//***************************************************************************
//  $FreeYYFcn1
//
//  Free the first element of a strand of YYFcns
//***************************************************************************

void FreeYYFcn1
    (LPPL_YYSTYPE lpYYFcn)

{
    Assert (lpYYFcn->TknCount NE 0);

    FreeResult (&lpYYFcn->tkToken); YYFree (lpYYFcn);
} // End FreeYYFcn1


//***************************************************************************
//  End of File: free.c
//***************************************************************************
