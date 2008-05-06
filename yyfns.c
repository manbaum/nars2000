//***************************************************************************
//  NARS2000 -- PL_YYSTYPE Functions
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
//  $_YYAlloc
//
//  Allocate a new YYRes entry
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- _YYAlloc"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE _YYAlloc
#ifdef DEBUG
    (LPCHAR lpFileName,         // Ptr to filename where allocated
     UINT   uLineNum)           // Line # where allocated
#else
    (void)
#endif

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

        // Zero the memory
        ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));

        // Mark as inuse
        lpYYRes->YYInuse = TRUE;
#ifdef DEBUG
        lpYYRes->SILevel = lpMemPTD->SILevel;   // Save the SI Level
        lpYYRes->YYFlag = 0;  // Mark as a YYAlloc Index

        // Save unique number for debugging/tracking purposes
        lpYYRes->YYIndex = ++YYIndex;

        // Save the ptr to the filename where allocated
        lpYYRes->lpFileName = lpFileName;

        // Save the line # where allocated
        lpYYRes->uLineNum   = uLineNum;
#endif
        goto NORMAL_EXIT;
    } // End FOR/IF

    // If we get here, we ran out of indices
    lpYYRes = &lpMemPTD->lpYYRes[u++];
    lpMemPTD->numYYRes = u;
RESTART_EXCEPTION_YYALLOC:
    __try
    {
        // Zero the memory
        ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));

        // Mark as inuse
        lpYYRes->YYInuse = TRUE;
#ifdef DEBUG
        lpYYRes->SILevel = lpMemPTD->SILevel;   // Save the SI Level
        lpYYRes->YYFlag = 0;  // Mark as a YYAlloc Index

        // Save unique number for debugging/tracking purposes
        lpYYRes->YYIndex = ++YYIndex;
#endif
    } __except (CheckException (GetExceptionInformation (), "YYAlloc"))
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

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpYYRes;
} // End _YYAlloc
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
    lpYYDst->YYInuse = TRUE;        // Retain YYInuse flag
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
        lpYYDst->YYInuse = TRUE;    // Mark as in use for YYCopy
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

    CheckMemStat ();

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
#ifdef DEBUG
        char szTemp[1024];

        wsprintf (szTemp,
                  "The YYRes at %08X TknType %s index %04X SI(%d) was allocated at %s#%d",
                 &lpMemPTD->lpYYRes[u],
                  GetTokenTypeName (lpMemPTD->lpYYRes[u].tkToken.tkFlags.TknType),
                  lpMemPTD->lpYYRes[u].YYIndex,
                  lpMemPTD->lpYYRes[u].SILevel,
                  lpMemPTD->lpYYRes[u].lpFileName,
                  lpMemPTD->lpYYRes[u].uLineNum);
        DbgMsg (szTemp);
#endif
        bRet = FALSE;

        break;
    } // End FOR/IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End YYResIsEmpty
#endif


//***************************************************************************
//  $YYCountFcnStr
//
//  Count the # tokens in a function strand
//***************************************************************************

UINT YYCountFcnStr
    (LPPL_YYSTYPE lpYYArg)          // Ptr to function strand

{
    UINT              uCnt,         // Loop counter
                      uLen,         // # tokens at top level
                      TknCount;     // The result
    LPTOKEN           lpToken;      // Ptr to token
    TOKEN_TYPES       tknType;      // Token type
    HGLOBAL           hGlbFcn;      // Function array global memory handle
    LPFCNARRAY_HEADER lpMemFcn;     // Ptr to function array global memory

    // Get the token count at the top level in this function strand
    uLen = lpYYArg->TknCount;

    // Loop through the tokens associated with this symbol
    for (uCnt = TknCount = 0; uCnt < uLen; uCnt++)
    {
        // If the function is indirect, recurse
        if (lpYYArg[uCnt].YYIndirect)
            TknCount += YYCountFcnStr (lpYYArg[uCnt].lpYYFcnBase);
        else
        {
            // Get the function arg token
            lpToken = &lpYYArg[uCnt].tkToken;

            // Get the token type
            tknType = lpToken->tkFlags.TknType;

            // If it's named and not immediate and not an internal function, ...
            if ((tknType EQ TKT_FCNNAMED
              || tknType EQ TKT_OP1NAMED
              || tknType EQ TKT_OP2NAMED
              || tknType EQ TKT_OP3NAMED)
             && !lpToken->tkData.tkSym->stFlags.Imm     // not an immediate, and
             && !lpToken->tkData.tkSym->stFlags.UsrDfn  // not a user-defined function/operator, and
             && !lpToken->tkData.tkSym->stFlags.FcnDir) // not an internal function
            {
                // Get the global memory handle
                hGlbFcn = ClrPtrTypeDirAsGlb (lpToken->tkData.tkSym->stData.stGlbData);

                // Lock the memory to get a ptr to it
                lpMemFcn = MyGlobalLock (hGlbFcn);

                // Accumulate tokens
                TknCount += lpMemFcn->tknNELM;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;
            } else
                TknCount++;
        } // End IF/ELSE
    } // End FOR

    return TknCount;
} // End YYCountFcnStr


//***************************************************************************
//  $YYIsFcnStrAxis
//
//  Check for an axis operator in a function strand
//***************************************************************************

BOOL YYIsFcnStrAxis
    (LPPL_YYSTYPE lpYYArg)          // Ptr to function strand

{
    BOOL              bRet;         // TRUE iff the result is valid
    HGLOBAL           hGlbFcn;      // Function array global memory handle
    LPFCNARRAY_HEADER lpMemFcn;     // Ptr to function array global memory

    /*
        The only cases we need to consider are as follows:

            Token           Type        Imm/Glb     TknCount

        1.  TKT_FCNIMMED    /           Imm             2
            TKT_AXISIMMED   [2]         Imm             1

        2.  TKT_FCNNAMED    /           Imm             2
            TKT_AXISIMMED   [2]         Imm             1

        3.  TKT_FCNARRAY    /[2]        Glb             1

        4.  TKT_FCNNAMED    /[2]        Glb             1

     */

    // SPlit cases based upon the token type
    switch (lpYYArg->tkToken.tkFlags.TknType)
    {
        case TKT_FCNIMMED:
            // Is there another token?
            if (lpYYArg->TknCount > 1)
                // Is the next token an axis value?
                return (lpYYArg[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
                     || lpYYArg[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY);
            else
                return FALSE;

        case TKT_FCNNAMED:
            // If the named op is immediate, ...
            if (lpYYArg->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Is there another token?
                if (lpYYArg->TknCount > 1)
                    // Is the next token an axis value?
                    return (lpYYArg[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
                         || lpYYArg[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY);
                else
                    return FALSE;
            } // End IF

            // Fall through to common global case

        case TKT_FCNARRAY:
            // Get the global memory handle
            hGlbFcn = ClrPtrTypeDirAsGlb (lpYYArg->tkToken.tkData.tkSym->stData.stGlbData);

            // Lock the memory to get a ptr to it
            lpMemFcn = MyGlobalLock (hGlbFcn);

            Assert (lpMemFcn->tknNELM > 1);

            // Skip over the function array header
            lpMemFcn = FcnArrayBaseToData (lpMemFcn);

            // Is the next token an axis value?
            bRet = (((LPPL_YYSTYPE) lpMemFcn)[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
                 || ((LPPL_YYSTYPE) lpMemFcn)[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;

            return bRet;

        defstop
            return FALSE;
    } // End SWITCH
} // End YYIsFcnStrAxis


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
     LPINT         lpTknCount,          // Ptr to resulting token count
     BOOL          bResUsed)            // TRUE iff the result is used

{
    int          i,
                 iLen,
                 TknCount,
                 TotalTknCount = 0;
    PL_YYSTYPE   YYFcn = {0};
    HGLOBAL      hGlbFcn;               // Function array global memory handle
    LPVOID       lpMemFcn;              // Ptr to function array global memory
    LPTOKEN      lpToken;
    LPPL_YYSTYPE lpYYMem0,
                 lpYYMem1,
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
        *lpYYBase = min (*lpYYBase, lpYYArg[i].lpYYFcnBase);

        // If the function is indirect, recurse
        if (lpYYArg[i].YYIndirect)
        {
            // Save the starting point so we can save the token count
            lpYYMem1 = lpYYMem;

            TknCount = 0;   // Initialize as it is incremented in YYCopyFcn
            lpYYMem = YYCopyFcn (lpYYMem, lpYYArg[i].lpYYFcnBase, lpYYBase, &TknCount, TRUE);

            Assert (TknCount EQ (lpYYMem - lpYYMem1));

            // Set the proper token count
            lpYYMem1->TknCount = TknCount;

            Assert (YYCheckInuse (&lpYYArg[i]));    // ***DEBUG***
        } else
        {
            // Get the function arg token
            lpToken = &lpYYArg[i].tkToken;

            // Assume it's a single token
            TknCount = 1;

            // Special case for named functions/operators
            if (lpToken->tkFlags.TknType EQ TKT_FCNNAMED
             || lpToken->tkFlags.TknType EQ TKT_OP1NAMED
             || lpToken->tkFlags.TknType EQ TKT_OP2NAMED
             || lpToken->tkFlags.TknType EQ TKT_OP3NAMED)
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
                    YYFcn.TknCount                  = TknCount;
////////////////////YYFcn.YYInuse                   = 0;        // (Factored out below)
////////////////////YYFcn.YYIndirect                = 0;        // Already zero from = {0}
////////////////////YYFcn.YYAvail                   = 0;        // Already zero from = {0}
////////////////////YYFcn.YYIndex                   = 0;        // (Factored out below)
////////////////////YYFcn.YYFlag                    = 0;        // Already zero from = {0}
////////////////////YYFcn.lpYYFcnBase               = NULL;     // Already zero from = {0}
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
                        hGlbFcn = lpToken->tkData.tkSym->stData.stGlbData;

                        // stData is a valid HGLOBAL function array
                        //   or user-defined function/operator
                        Assert (IsGlbTypeFcnDir (hGlbFcn)
                             || IsGlbTypeDfnDir (hGlbFcn));

                        // Split cases based upon the function signature
                        switch (GetSignatureGlb (hGlbFcn))
                        {
                            case DFN_HEADER_SIGNATURE:
                                // Increment the reference count in global memory
                                DbgIncrRefCntDir (hGlbFcn);

                                // Fill in the token
                                YYFcn.tkToken.tkFlags.TknType   = TKT_FCNARRAY;
////////////////////////////////YYFcn.tkToken.tkFlags.ImmType   = 0;        // Already zero from = {0}
////////////////////////////////YYFcn.tkToken.tkFlags.NoDisplay = 0;        // Already zero from = {0}
                                YYFcn.tkToken.tkData.tkGlbData  = hGlbFcn;
                                YYFcn.tkToken.tkCharIndex       = lpToken->tkCharIndex;
////////////////////////////////YYFcn.tkToken.lptkOrig          = NULL;     // ALready zero from = {0}
                                YYFcn.TknCount                  = TknCount;
////////////////////////////////YYFcn.YYInuse                   = 0;        // (Factored out below)
////////////////////////////////YYFcn.YYIndirect                = 0;        // Already zero from = {0}
////////////////////////////////YYFcn.YYAvail                   = 0;        // Already zero from = {0}
////////////////////////////////YYFcn.YYIndex                   = 0;        // (Factored out below)
////////////////////////////////YYFcn.YYFlag                    = 0;        // Already zero from = {0}
////////////////////////////////YYFcn.lpYYFcnBase               = NULL;     // Already zero from = {0}
                                YYFcn.lpYYStrandBase            = lpYYArg[i].lpYYStrandBase;

                                break;

                            case FCNARRAY_HEADER_SIGNATURE:
                                // Clear the ptr type bits
                                hGlbFcn = ClrPtrTypeDirAsGlb (hGlbFcn);

                                // Lock the memory to get a ptr to it
                                lpMemFcn = MyGlobalLock (hGlbFcn);

                                // Get the token count
                                TknCount = ((LPFCNARRAY_HEADER) lpMemFcn)->tknNELM;

                                Assert (TknCount > 1);

                                // Skip over the header to the data
                                lpMemFcn = FcnArrayBaseToData (lpMemFcn);

                                // Copy the contents of the function array to global memory
                                CopyMemory (lpYYMem, lpMemFcn, TknCount * sizeof (lpYYMem[0]));

                                // Skip over the copied function array
                                lpYYMem += TknCount;

                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End IF
                } // End IF/ELSE

                // If we didn't expand a function array, ...
                if (TknCount EQ 1)
                {
                    YYFcn.YYInuse  = 0;
#ifdef DEBUG
                    YYFcn.YYIndex  = NEG1U;
#endif
                    // Copy the destination YYInuse flag in case it's
                    //   in the YYRes allocated region
                    YYFcn.YYInuse  = lpYYMem->YYInuse;
                    YYFcn.TknCount = TknCount;

                    // Copy to the destination
                    *lpYYMem++ = YYFcn;
                    Assert (YYCheckInuse (&lpYYMem[-1]));   // ***DEBUG***
                } // End IF
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
        } // End IF/ELSE

        // Accumulate into the total token count
        TotalTknCount += TknCount;
    } // End FOR

    // Save the total function count in the first token
    lpYYMem0->TknCount = TotalTknCount;

    // Return as the overall total
    *lpTknCount = TotalTknCount;

#ifdef DEBUG
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
