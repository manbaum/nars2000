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

            // stData is an HGLOBAL
            Assert (!lptkRes->tkData.tkSym->stFlags.Imm);

            // Check for system functions
            if (lptkRes->tkData.tkSym->stFlags.SysFn0
             || lptkRes->tkData.tkSym->stFlags.SysFn12)
                break;

            // Get the global memory handle
            hGlbData = lptkRes->tkData.tkSym->stData.stGlbData;

            // Data is an valid HGLOBAL named primitive or defined function
            Assert (IsGlbTypeFcnDir (hGlbData)
                 || IsGlbTypeDfnDir (hGlbData));

            // Is it time to free the name?
            if (bFreeName)
            {
                // If it's a defined function, ...
                if (lptkRes->tkData.tkSym->stFlags.UsrDfn)
                {
                    // Free the defined function,
                    if (FreeResultGlobalDfn (hGlbData))
                    {
#ifdef DEBUG
                        dprintf ("**Zapping in FreeResultSub: Token=%08X, Value=%08X (%s#%d)",
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
#ifdef DEBUG
                        dprintf ("**Zapping in FreeResultSub: Token=%08X, Value=%08X (%s#%d)",
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
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRes->tkData.tkVoid) EQ PTRTYPE_STCONST);

            if (!lptkRes->tkData.tkSym->stFlags.Value)
                return;

            // If the LPSYMENTRY is not immediate, it must be an HGLOBAL
            if (!lptkRes->tkData.tkSym->stFlags.Imm)
            {
                // Get the global memory ptr
                hGlbData = lptkRes->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable or function array
                Assert (IsGlbTypeVarDir (hGlbData)
                     || IsGlbTypeFcnDir (hGlbData));

                // The call to FreeResult after ArrayDisplay_EM needs the
                //   following if-statement.

                // Is it time to free the name?
                if (bFreeName)
                {
                    // Get the global memory ptr
                    hGlbData = ClrPtrTypeDirGlb (hGlbData);

                    if (lptkRes->tkFlags.TknType EQ TKT_FCNNAMED
                     || lptkRes->tkFlags.TknType EQ TKT_OP1NAMED
                     || lptkRes->tkFlags.TknType EQ TKT_OP2NAMED)
                        bTmp = FreeResultGlobalFcn (hGlbData);
                    else
                        bTmp = FreeResultGlobalVar (hGlbData);
                    if (bTmp)
                    {
#ifdef DEBUG
                        dprintf ("**Zapping in FreeResultSub: Token=%08X, Value=%08X (%s#%d)",
                                 lptkRes,
                                 ClrPtrTypeDir (lptkRes->tkData.tkSym->stData.stGlbData),
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
                stFlags.SysName     =
                stFlags.UsrName     =
                stFlags.Inuse       =
                stFlags.NotCase     =
                stFlags.Perm        = 1;
                stFlags.SysVarValid = NEG1U;

                // Clear the symbol table flags
                *(UINT *) &lptkRes->tkData.tkSym->stFlags &= *(UINT *) &stFlags;
            } // End IF

            return;

        case TKT_VARIMMED:  // tkData is an immediate constant
        case TKT_AXISIMMED: // ...
        case TKT_STRAND:    // Occurs if MakeStrand fails -- data freed by <FreeStrand>
        case TKT_LISTPAR:   // tkData is -1
        case TKT_LISTINT:   // tkData is -1
            return;

        case TKT_VARARRAY:  // tkData contains an HGLOBAL of an array of LPSYMENTRYs and HGLOBALs
        case TKT_AXISARRAY: // ...
        case TKT_FCNARRAY:  // ...
            // Check for ptr reuse
            if (!PtrReusedDir (lptkRes->tkData.tkGlbData))
            {
                // tkData is a valid HGLOBAL variable or function array
                Assert (IsGlbTypeVarDir (lptkRes->tkData.tkGlbData)
                     || IsGlbTypeFcnDir (lptkRes->tkData.tkGlbData));

                if (lptkRes->tkFlags.TknType EQ TKT_FCNARRAY)
                    bTmp = FreeResultGlobalFcn (ClrPtrTypeDirGlb (lptkRes->tkData.tkGlbData));
                else
                    bTmp = FreeResultGlobalVar (ClrPtrTypeDirGlb (lptkRes->tkData.tkGlbData));
                if (bTmp)
                {
#ifdef DEBUG
                    dprintf ("**Zapping in FreeResultSub: Token=%08X, Value=%08X (%s#%d)",
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
    APLSTYPE  aplType;          // The array storage type (see enum ARRAY_TYPES)
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

#ifdef DEBUG
        dprintfW (L"##RefCnt-- in " APPEND_NAME L": %08X(res=%d) (%S#%d)", ClrPtrTypeDir (hGlbData), RefCnt, FNLN);
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
                // Take into account nested prototype
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
#ifdef DEBUG
                                dprintf ("**Zapping in FreeResultGlobalVar: Global=%08X, Value=%08X (%s#%d)",
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
    LPVOID    lpMemData,        // Ptr to the function array's global memory
              lpMemLcl;         // Ptr to
////APLSTYPE  cFcnType;         // The function array storage type (see FCNTYPES enum)
    APLNELM   aplNELM;          // The # elements in the function array
    UINT      u,                // Loop counter
              RefCnt,           // Reference count
              Sig;              // Signature
    BOOL      bRet;             // TRUE iff result is valid
    LPYYSTYPE lpYYToken;        // Ptr to function array token
    HGLOBAL   hGlbLcl,          // Global memory handle
              hGlbTxtLine;      // Line text gobal memory handle

    DBGENTER;

    // Data is an valid HGLOBAL function array
    Assert (IsGlbTypeFcnDir (MakeGlbTypeGlb (hGlbData)));

    // Lock the memory to get a ptr to it
    lpMemData = MyGlobalLock (hGlbData);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemData)

    // Get the Type, RefCnt, NELM, and line text handle
////cFcnType    = lpHeader->FcnType;
    RefCnt      = lpHeader->RefCnt;
    aplNELM     = lpHeader->NELM;
    hGlbTxtLine = lpHeader->hGlbTxtLine;

    // Ensure non-zero
    Assert (RefCnt > 0);

    // Decrement
    RefCnt = --lpHeader->RefCnt;

#undef  lpHeader

#ifdef DEBUG
    dprintfW (L"##RefCnt-- in " APPEND_NAME L": %08X(res=%d) (%S#%d)", ClrPtrTypeDir (hGlbData), RefCnt, FNLN);
#endif

    if (RefCnt EQ 0)
    {
        // Free the line text
        if (hGlbTxtLine)
        {
            DbgGlobalFree (hGlbTxtLine); hGlbTxtLine = NULL;
        } // End IF

        // Point to the array data (YYSTYPEs)
        lpYYToken = FcnArrayBaseToData (lpMemData);

        // Loop through the YYSTYPEs
        for (u = 0; u < aplNELM; u++, lpYYToken++)
        // Split cases based upon the token type
        switch (lpYYToken->tkToken.tkFlags.TknType)
        {
            case TKT_FCNIMMED:      // Nothing to do
            case TKT_VARIMMED:      // ...
            case TKT_OP1IMMED:      // ...
            case TKT_OP2IMMED:      // ...
            case TKT_AXISIMMED:     // ...
            case TKT_OPJOTDOT:      // ...
                break;              // Ignore immediates

            case TKT_FCNARRAY:      // Free the function array
                // Check for internal functions
                if (lpYYToken->tkToken.tkFlags.FcnDir)
                    break;          // Ignore internal functions

                // Get the global memory handle
                hGlbLcl = lpYYToken->tkToken.tkData.tkGlbData;

                // tkData is a valid HGLOBAL function array or defined function
                Assert (IsGlbTypeFcnDir (hGlbLcl)
                     || IsGlbTypeDfnDir (hGlbLcl));

                // Clear the ptr type bits
                hGlbLcl = ClrPtrTypeDirGlb (hGlbLcl);

                // Lock the memory to get a ptr to it
                lpMemLcl = MyGlobalLock (hGlbLcl);

                // Get the signature
                Sig = ((LPHEADER_SIGNATURE) lpMemLcl)->nature;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbLcl); lpMemLcl = NULL;

                // Split cases based upon the array signature
                switch (Sig)
                {
                    case FCNARRAY_HEADER_SIGNATURE:
                        // Free the global function
                        if (FreeResultGlobalFcn (hGlbLcl))
                        {
#ifdef DEBUG
                            dprintf ("**Zapping in FreeResultGlobalFcn: Global=%08X, Value=%08X (%s#%d)",
                                     hGlbData,
                                     hGlbLcl,
                                     FNLN);
#endif
                            lpYYToken->tkToken.tkData.tkGlbData = NULL;
                        } // End IF

                        break;

                    case DFN_HEADER_SIGNATURE:
                        DbgBrk ();

                        if (FreeResultGlobalDfn (hGlbData))
                        {
#ifdef DEBUG
                            dprintf ("**Zapping in FreeResultGlobalFcn: Global=%08X, Value=%08X (%s#%d)",
                                     hGlbData,
                                     hGlbLcl,
                                     FNLN);
#endif
                            lpYYToken->tkToken.tkData.tkGlbData = NULL;
                        } // End IF

                        break;

                    defstop
                        break;
                } // End SWITCH

                break;

////////////case TKT_VARARRAY:      // Free the var array
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
#ifdef DEBUG
                    dprintf ("**Zapping in FreeResultGlobalFcn: Global=%08X, Value=%08X (%s#%d)",
                             hGlbData,
                             hGlbLcl,
                             FNLN);
#endif
                    lpYYToken->tkToken.tkData.tkGlbData = NULL;
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
//  $FreeResultGlobalDfn
//
//  Free a global defined function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- FreeResultGlobalDfn"
#else
#define APPEND_NAME
#endif

BOOL FreeResultGlobalDfn
    (HGLOBAL hGlbData)

{
    LPDFN_HEADER lpMemDfn;          // Ptr to defined function header global memory
    UINT         numFcnLines,       // # lines in the function
                 RefCnt;            // Reference count
    LPFCNLINE    lpFcnLines;        // Ptr to the array of structs (FCNLINE[numFcnLine])

    // Data is an valid HGLOBAL defined function
    Assert (IsGlbTypeDfnDir (hGlbData));

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
        stFlags.SysName     =
        stFlags.UsrName     =
        stFlags.Inuse       =
        stFlags.NotCase     =
        stFlags.Perm        = 1;
        stFlags.SysVarValid = NEG1U;

        DbgBrk ();

        // Clear the symbol table flags
////    *(UINT *) &lptkRes->tkData.tkSym->stFlags &= *(UINT *) &stFlags;

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
        lpFcnLines = (LPFCNLINE) &((LPBYTE) lpMemDfn)[lpMemDfn->offFcnLines];

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
                DbgBrk ();      // ***FINISHME***






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
    (LPYYSTYPE lpYYFcn)

{
    int i;

    Assert (lpYYFcn->TknCount NE 0);

    for (i = 0; i < lpYYFcn[i].TknCount; i++)
    {
        FreeResult (&lpYYFcn[i].tkToken); YYFree (&lpYYFcn[i]);
    } // End FOR
} // End FreeYYFcn


//***************************************************************************
//  $FreeYYFcn1
//
//  Free the first element of a strand of YYFcns
//***************************************************************************

void FreeYYFcn1
    (LPYYSTYPE lpYYFcn)

{
    Assert (lpYYFcn->TknCount NE 0);

    FreeResult (&lpYYFcn->tkToken); YYFree (lpYYFcn);
} // End FreeYYFcn1


//***************************************************************************
//  End of File: free.c
//***************************************************************************
