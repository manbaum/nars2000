//***************************************************************************
//  NARS2000 -- PL_YYSTYPE Functions
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2019 Sudley Place Software

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
#include "headers.h"


//***************************************************************************
//  $YYAllocTkn
//
//  Allocate a YYAlloc to hold a token
//***************************************************************************

LPPL_YYSTYPE YYAllocTkn
    (LPTOKEN lptkToken)         // Ptr to token

{
    LPPL_YYSTYPE lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken = *lptkToken;  // Maybe <*CopyToken (lptkToken)>

    return lpYYRes;
} // YYAllocTkn


//***************************************************************************
//  $YYAllocGlb
//
//  Allocate a global array via YYAlloc
//***************************************************************************

LPPL_YYSTYPE YYAllocGlb
    (HGLOBAL tkGlbData,         // Global array handle
     UINT    tkCharIndex)       // Character index

{
    LPPL_YYSTYPE lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (tkGlbData);
    lpYYRes->tkToken.tkCharIndex       = tkCharIndex;

    return lpYYRes;
} // YYAllocGlb


//***************************************************************************
//  $YYAllocImm
//
//  Allocate an immediate value via YYAlloc
//***************************************************************************

LPPL_YYSTYPE YYAllocImm
    (IMM_TYPES  immType,        // Immediate type
     APLLONGEST tkLongest,      // Immediate value
     UBOOL      bNoDisplay,     // NoDisplay value
     UINT       tkCharIndex)    // Character index

{
    LPPL_YYSTYPE lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = immType;
    lpYYRes->tkToken.tkFlags.NoDisplay = bNoDisplay;
    lpYYRes->tkToken.tkData.tkLongest  = tkLongest;
    lpYYRes->tkToken.tkCharIndex       = tkCharIndex;

    return lpYYRes;
} // YYAllocImm


//***************************************************************************
//  $YYAlloc
//
//  Allocate a new YYRes entry
//***************************************************************************

LPPL_YYSTYPE _YYAlloc
#ifdef DEBUG
    (LPCHAR lpFileName,         // Ptr to filename where allocated
     UINT   uLineNum)           // Line # where allocated
#else
    (void)
#endif

{
    UINT         u;                 // Loop counter
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

#ifdef DEBUG
    static UINT YYIndex = 0;
#endif

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Search for an empty YYRes slot,
    //   zero it,
    //   mark it as inuse,
    //   and return a ptr to it
    for (u = 0; u < lpMemPTD->numYYRes; u++)
    if (!lpMemPTD->lpYYRes[u].YYInuse)
    {
        // Set the return value
        lpYYRes = &lpMemPTD->lpYYRes[u];

        goto NORMAL_EXIT;
    } // End FOR/IF

    // If we get here, we ran out of indices
    lpYYRes = &lpMemPTD->lpYYRes[u++];
    lpMemPTD->numYYRes = u;
NORMAL_EXIT:
    // Zero the memory
    ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));

    // Mark as inuse
    lpYYRes->YYInuse = TRUE;
#ifdef DEBUG
    // Mark as unknown SynObj so as to catch cases where we miss setting it
    lpYYRes->tkToken.tkSynObj = soUNK;

    // Save the ptr to the filename where allocated
    lpYYRes->lpFileName = lpFileName;

    // Save the line # where allocated
    lpYYRes->uLineNum   = uLineNum;
#endif
    // Initialize the token count
    lpYYRes->TknCount = 1;
#ifdef DEBUG
    lpYYRes->SILevel = lpMemPTD->SILevel;   // Save the SI Level

    // Save unique number for debugging/tracking purposes
    lpYYRes->YYIndex = ++YYIndex;

    if (gYYAlloc NE 0)
        Assert (lpYYRes->YYIndex NE gYYAlloc);
#endif
    return lpYYRes;
} // End _YYAlloc


//***************************************************************************
//  $YYCopy
//
//  Copy one PL_YYSTYPE to another
//    retaining the destination Inuse and DEBUG values
//***************************************************************************

void YYCopy
    (LPPL_YYSTYPE lpYYDst,
     LPPL_YYSTYPE lpYYSrc)

{
#ifdef DEBUG
    UINT   YYIndex,
           SILevel,
           uLineNum;
    LPCHAR lpFileName;
#endif

    Assert (lpYYDst->YYInuse);

#ifdef DEBUG
    // Save the old values
    YYIndex    = lpYYDst->YYIndex;
    SILevel    = lpYYDst->SILevel;
    lpFileName = lpYYDst->lpFileName;
    uLineNum   = lpYYDst->uLineNum;
#endif
    *lpYYDst = *lpYYSrc;                // Copy the PL_YYSTYPE
    lpYYDst->YYInuse = TRUE;            // Retain YYInuse flag
#ifdef DEBUG
    lpYYDst->YYIndex    = YYIndex;      // Retain YYIndex
    lpYYDst->SILevel    = SILevel;      // ...    SILevel
    lpYYDst->lpFileName = lpFileName;   // ...    ptr to filename
    lpYYDst->uLineNum   = uLineNum;     // ...    line #
#endif
} // End YYCopy


//***************************************************************************
//  $YYCopy2
//
//  Copy one PL_YYSTYPE to another
//    retaining the destination Index
//***************************************************************************

void YYCopy2
    (LPPL_YYSTYPE lpYYDst,
     LPPL_YYSTYPE lpYYSrc)

{
#ifdef DEBUG
    UINT   YYIndex;
#endif

    Assert (lpYYDst->YYInuse);

#ifdef DEBUG
    // Save the old values
    YYIndex  = lpYYDst->YYIndex;
#endif
    *lpYYDst = *lpYYSrc;                // Copy the PL_YYSTYPE
    lpYYDst->YYInuse = TRUE;            // Retain YYInuse flag
#ifdef DEBUG
    lpYYDst->YYIndex    = YYIndex;      // Retain YYIndex
#endif
} // End YYCopy2


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
        lpYYDst->YYInuse = FALSE;   // Mark as no longer in use
    } // End IF/ELSE
} // End YYCopyFreeDst


//***************************************************************************
//  $YYCopyToMem
//
//  Copy one PL_YYSTYPE to memory erasing all YY flags
//    leaving the RefCnt unchanged, but zapping any curried values.
//***************************************************************************

void YYCopyToMem
    (LPPL_YYSTYPE lpYYDst,
     LPPL_YYSTYPE lpYYSrc)

{
    *lpYYDst = *lpYYSrc;                // Copy the PL_YYSTYPE
    lpYYDst->YYInuse    = FALSE;        // Zero YYInuse flag
#ifdef DEBUG
    lpYYDst->YYIndex    = 0;            // Zero YYIndex
    lpYYDst->SILevel    = 0;            // ...  SILevel
    lpYYDst->lpFileName = NULL;         // ...  ptr to filename
    lpYYDst->uLineNum   = 0;            // ...  line #
#endif
    // Zap any curried values
    lpYYDst->lpplYYIdxCurry =
    lpYYDst->lpplYYOpRCurry =
////lpYYDst->lpplYYArgCurry =           // Allowed as in (4{rho}){each}1 2 3
    lpYYDst->lpplYYOpLCurry = NULL;
} // End YYCopyToMem


//***************************************************************************
//  $YYFree
//
//  Free a YYRes entry
//***************************************************************************

void _YYFree
#ifdef DEBUG
    (LPPL_YYSTYPE lpYYRes,      // Ptr to the YYRes entry
     LPCHAR       lpFileName,   // Ptr to filename where allocated
     UINT         uLineNum)     // Line # where allocated
#else
    (LPPL_YYSTYPE lpYYRes)      // Ptr to the YYRes entry
#endif

{
#ifdef DEBUG
    UINT YYIndex;

    // If this entry is Inuse, ...
    if (lpYYRes->YYInuse)
    {
        if (!lpYYRes->YYPerm)
        {
            APLU3264     u;             // Index into lpMemPTD->YYRes
            LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

            // Get ptr to PerTabData global memory
            lpMemPTD = GetMemPTD ();

            u = (APLU3264) (lpYYRes - lpMemPTD->lpYYRes);
            Assert (u < lpMemPTD->numYYRes);
            Assert (_YYCheckInuse (lpYYRes, lpFileName, uLineNum));
        } else
        {
            WCHAR wszTemp[1024];

            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"YYFree:  YYRes at %p index %04X is Perm and about to be YYFree'd.  YYAlloc'ed at %S#%d (%S#%d)",
                        lpYYRes,
                        lpYYRes->YYIndex,
                        lpYYRes->lpFileName,
                        lpYYRes->uLineNum,
                        lpFileName,
                        uLineNum);
            DbgMsgW (wszTemp);

            DbgBrk ();      // #ifdef DEBUG
        } // End IF/ELSE

        if (gYYAlloc NE 0)
            Assert (lpYYRes->YYIndex NE gYYAlloc);

        // Save the old value
        YYIndex  = lpYYRes->YYIndex;
    } // End IF
#endif
    // If this entry is Inuse, ...
    if (lpYYRes->YYInuse)
        // Zap it
        ZeroMemory (lpYYRes, sizeof (lpYYRes[0]));
#ifdef DEBUG
    lpYYRes->lpFileName = lpFileName;
    lpYYRes->uLineNum   = uLineNum;
    lpYYRes->YYIndex    = YYIndex;
#endif
} // End _YYFree


#ifdef DEBUG
//***************************************************************************
//  $YYResIsEmpty
//
//  Ensure that YYRes has no Inuse entries
//***************************************************************************

UBOOL YYResIsEmpty
    (void)

{
    UINT         u;             // Loop counter
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    UBOOL        bRet = TRUE;   // TRUE iff result is valid

    CheckMemStat ();

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Loop through the YYRes entries looking for
    //   entries in use at this SI Level
    for (u = 0; u < lpMemPTD->numYYRes; u++)
    if (lpMemPTD->lpYYRes[u].YYInuse
     && lpMemPTD->SILevel EQ lpMemPTD->lpYYRes[u].SILevel)
    {
#ifdef DEBUG
        WCHAR wszTemp[1024];

        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"YYResIsEmpty:  The YYRes at %p TknType %S index %04X SI(%d) was allocated at %S#%d",
                   &lpMemPTD->lpYYRes[u],
                    GetTokenTypeName (lpMemPTD->lpYYRes[u].tkToken.tkFlags.TknType),
                    lpMemPTD->lpYYRes[u].YYIndex,
                    lpMemPTD->lpYYRes[u].SILevel,
                    lpMemPTD->lpYYRes[u].lpFileName,
                    lpMemPTD->lpYYRes[u].uLineNum);
        DbgMsgW (wszTemp);
#endif
        bRet = FALSE;
    } // End FOR/IF

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
    UINT TknCount;                  // Token count

    // Initialize the token count
    TknCount = lpYYArg->TknCount;

    // Overcome zero in single function tokens
    TknCount = max (TknCount, 1);

    // Count the # entries in the strand
    if (lpYYArg->lpplYYIdxCurry NE NULL)
        TknCount++;
    if (lpYYArg->lpplYYOpRCurry NE NULL)
        TknCount += YYCountFcnStr (lpYYArg->lpplYYOpRCurry);
    if (lpYYArg->lpplYYOpLCurry NE NULL)
        TknCount += YYCountFcnStr (lpYYArg->lpplYYOpLCurry);
    if (IsTknFcnArray (&lpYYArg->tkToken)
     || IsTknTrain    (&lpYYArg->tkToken))
    {
        HGLOBAL           hGlbFcn;      // Global memory handle
        LPFCNARRAY_HEADER lpMemFcn;     // Ptr to global memory

        // Get the global memory handle
        hGlbFcn = lpYYArg->tkToken.tkData.tkGlbData;

        // Lock the memory to get a ptr to it
        lpMemFcn = MyGlobalLockFcn (hGlbFcn);

        Assert (lpMemFcn->tknNELM NE 0);

        // Accumulate multiple tokens in the function array
        //   less the main token
        TknCount += lpMemFcn->tknNELM - 1;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbFcn); lpMemFcn = NULL;
    } // End IF

    // Note that lpplYYArgCurry does not count as an element in the strand

    return TknCount;
} // End YYCountFcnStr


//***************************************************************************
//  $YYCountFcnTrn
//
//  Count the # tokens in a function train
//***************************************************************************

UINT YYCountFcnTrn
    (LPPL_YYSTYPE lpYYArg)          // Ptr to function strand

{
    UINT TknCount = 0;              // Token count

    while (TRUE)
    {
        // Overcome zero in single function tokens
        TknCount += max (lpYYArg->TknCount, 1);

////////Assert (lpYYArg->lpplYYArgCurry EQ NULL);   // EXAMPLE:  (1 2/)3 4
        Assert (lpYYArg->lpplYYIdxCurry EQ NULL);
        Assert (lpYYArg->lpplYYOpRCurry EQ NULL);

        // Count the # entries in the strand
        if (lpYYArg->lpplYYOpLCurry NE NULL)
            lpYYArg = lpYYArg->lpplYYOpLCurry;
        else
            break;
    } // End WHILE

    return TknCount;
} // End YYCountFcnTrn


//***************************************************************************
//  $YYCopyFcnStr
//
//  Copy the tokens in a function strand
//    returning a ptr to the next entry
//***************************************************************************

LPPL_YYSTYPE YYCopyFcnStr
    (LPPL_YYSTYPE *lplpYYMemRoot,       // Ptr to ptr to root memory object
     LPPL_YYSTYPE  lpYYMem,             // Ptr to result memory object
     LPPL_YYSTYPE  lpYYArg,             // Ptr to function arg
     LPINT         lpTknCount)          // Ptr to resulting token count

{
    int           i,                    // Loop counter
                  iLen,                 // Token count in this function strand
                  TknCount,             // # tokens added for this element of the function strand
                  TmpCount,             // ... temp version
                  TotalTknCount = 0;    // Total token count
    UBOOL         bFcnArray;            // TRUE iff it's a function array (we should NOT copy the root item)
    LPPL_YYSTYPE  lpYYMem0,             // Ptr to temporary YYSTYPE
                  lpYYMemC,             // ...
                  lpYYArgI;             // ...

    // Get the token count in this function strand
    iLen = lpYYArg->TknCount;

    Assert (iLen > 0);

    // Overcome zero in single function tokens
    iLen = max (iLen, 1);

    // We need to modify the function count in the first token,
    //   so save its address in the array.
    lpYYMem0 = lpYYMem;

    // Izit a function array?
    bFcnArray = IsTknFcnArray (&lpYYArg->tkToken);

    // If the arg is a function array, ...
    if (bFcnArray)
    {
#ifdef DEBUG
        LPFCNARRAY_HEADER lpYYHdrArgI;

        lpYYHdrArgI = (LPVOID)
#endif
        // Lock the memory to get a ptr to it
        lpYYArgI = MyGlobalLockFcn (lpYYArg->tkToken.tkData.tkGlbData);

        // Skip over the header to the data (PL_YYSTYPEs)
        lpYYArgI = FcnArrayBaseToData (lpYYArgI);
#ifdef DEBUG
        // Loop through the items
        for (i = 0; i < (int) lpYYHdrArgI->tknNELM; i++)
        {
            Assert (lpYYArgI[i].lpplYYArgCurry EQ NULL);
            Assert (lpYYArgI[i].lpplYYIdxCurry EQ NULL);
            Assert (lpYYArgI[i].lpplYYOpRCurry EQ NULL);
            Assert (lpYYArgI[i].lpplYYOpLCurry EQ NULL);
        } // End FOR
#endif
        // Initialize the count
        TotalTknCount = 0;

        // Copy the PL_YYSTYPEs in the Function Strand to the global memory
        lpYYMem = YYCopyFcnStr (lplpYYMemRoot, lpYYMem, lpYYArgI, &TotalTknCount);

        // We no longer need this ptr
        MyGlobalUnlock (lpYYArg->tkToken.tkData.tkGlbData); lpYYArgI = NULL;

        Assert (iLen EQ 1);
    } // End IF

    // Loop through the tokens associated with this symbol
    for (i = 0; i < iLen; i++)
    {
#ifdef DEBUG
        lpYYArgI = &lpYYArg[i];
#endif
        Assert (YYCheckInuse (&lpYYArg[i]));

        Assert (lpYYArg[i].tkToken.tkFlags.TknType NE TKT_UNUSED);

        Assert (lpYYArg[i].lpplYYArgCurry EQ NULL);

        // If we should copy the root, ...
        if (!bFcnArray)
        {
            size_t uTmpCount;

            // Copy the root token
            YYCopyToMem (lpYYMem++, &lpYYArg[i]);

            // If the just copied arg is an operator, ...
            if (IsTknOp1 (&lpYYMem[-1].tkToken)
             || IsTknOp2 (&lpYYMem[-1].tkToken))
            {
                // Set the Backoff offset
                lpYYMem[-1].uBackOff = lpYYMem - *lplpYYMemRoot - 1;

                // If the previous operator is dyadic, this token is its right operand
                //   in which case we shouldn't mark this as a new root
                uTmpCount = lpYYMem[-1].uBackOff;

                if (!((uTmpCount >= 1 &&                                     IsTknOp2 (&lpYYMem[-2].tkToken))
                   || (uTmpCount >= 2 && IsTknAxis (&lpYYMem[-2].tkToken) && IsTknOp2 (&lpYYMem[-3].tkToken))))
                    // Mark as a new root
                    *lplpYYMemRoot = lpYYMem - 1;
            } // End IF

            // Initialize it
            TknCount = 1;
        } else
            // Initialize it
            TknCount = 0;

        // If the token has an axis operator, ...
        if (lpYYArg[i].lpplYYIdxCurry NE NULL)
        {
            // Copy the token
            YYCopyToMem (lpYYMem++, lpYYArg[i].lpplYYIdxCurry);

            // DO NOT FREE IT as vars are handled differently from fcns
            // FreeResult (lpYYArg->lpplYYIdxCurry);

            // YYFree it
            YYFree (lpYYArg->lpplYYIdxCurry); lpYYArg->lpplYYIdxCurry = NULL;

            // Count it in
            TknCount++;
        } // End IF

        // If the token has a right operand, ...
        if (lpYYArg->lpplYYOpRCurry NE NULL)
        {
            LPPL_YYSTYPE lpYYLclRoot;       // Ptr to temporary YYSTYPE

            // Initialize the count
            TmpCount = 0;

            // Copy the global root as a local root
            //   but do not update the global root
            lpYYLclRoot = *lplpYYMemRoot;

            // Copy the right operand
            lpYYMemC = YYCopyFcnStr (&lpYYLclRoot, lpYYMem, lpYYArg[i].lpplYYOpRCurry, &TmpCount);

            // Save as ptr to next entry
            lpYYMem = lpYYMemC;

            // YYFree it
            YYFree (lpYYArg[i].lpplYYOpRCurry); lpYYArg[i].lpplYYOpRCurry = NULL;

            // Include in the token count
            TknCount += TmpCount;
        } // End IF

        // If the token has a curried function/left operand, ...
        if (lpYYArg->lpplYYOpLCurry NE NULL)
        {
            LPPL_YYSTYPE lpYYLclRoot;       // Ptr to temporary YYSTYPE

            // Initialize the count
            TmpCount = 0;

            // Copy the global root as a local root
            lpYYLclRoot = *lplpYYMemRoot;

            // Copy the curried function/left operand
            lpYYMemC = YYCopyFcnStr (lplpYYMemRoot, lpYYMem, lpYYArg[i].lpplYYOpLCurry, &TmpCount);

            // If the copied token is an operator, ...
            if (IsTknOp1 (&lpYYMem->tkToken)
             || IsTknOp2 (&lpYYMem->tkToken))
            {
                // Initialize the BackOff offset
                lpYYMem->uBackOff = lpYYMem - lpYYLclRoot;

                // Mark as a new root
                *lplpYYMemRoot = lpYYMem;
            } else
            {
                // Loop through the linked list of previous operators
                for (lpYYMem = lpYYLclRoot;
                     lpYYMem->uBackOff NE 0;
                     lpYYMem -= lpYYMem->uBackOff)
                    // Count in the # tokens in the curried function
                    lpYYMem->TknCount += TmpCount;
            } // End IF/ELSE

            // Save as ptr to next entry
            lpYYMem = lpYYMemC;

            // YYFree it
            YYFree (lpYYArg[i].lpplYYOpLCurry); lpYYArg[i].lpplYYOpLCurry = NULL;

            // Include in the token count
            TknCount += TmpCount;
        } // End IF

        // Accumulate into the total token count
        TotalTknCount += TknCount;
    } // End FOR

    // If we're a function array, ...
    if (bFcnArray)
    {
        // Free it
        FreeTopResult (lpYYArg); lpYYArg = NULL;
    } // End IF

    // Save the total function count in the first token
    lpYYMem0->TknCount = TotalTknCount;

    // Return as the overall total
    *lpTknCount += TotalTknCount;

    Assert (YYCheckInuse (lpYYMem));

    return lpYYMem;
} // End YYCopyFcnStr


//***************************************************************************
//  $YYCopyFcnTrn
//
//  Copy the tokens in a function train
//***************************************************************************

LPPL_YYSTYPE YYCopyFcnTrn
    (LPPL_YYSTYPE  lpYYMem,             // Ptr to result memory object
     LPPL_YYSTYPE  lpYYArg,             // Ptr to function arg
     LPINT         lpTknCount)          // Ptr to resulting token count

{
    int          TknCount = 0;              // # tokens added for this element of the function strand
    LPPL_YYSTYPE lpYYMem0;                  // Ptr to temporary YYSTYPE
    LPPERTABDATA lpMemPTD = GetMemPTD ();   // Ptr to PerTabData global memory

    // We need to modify the function count in the first token,
    //   so save its address in the array.
    lpYYMem0 = lpYYMem;

    while (TRUE)
    {
        // Split cases based upon the token type
        switch (lpYYArg->tkToken.tkFlags.TknType)
        {
            case TKT_FCNNAMED:
                // Copy the root token
                YYCopyToMem (lpYYMem, lpYYArg);

                // Convert the named Fcn/Op1/Op2/Op3 to an unnamed form
                //   DO NOT INCREMENT THE RefCnt
                ConvertNamedFopToUnnamed (lpYYMem, soF, TKT_FCNIMMED, TKT_FCNAFO, TKT_FCNDFN, FALSE);

                // Skip to the next save area element
                lpYYMem++;

                break;

            case TKT_OP1NAMED:
                // Copy the root token
                YYCopyToMem (lpYYMem, lpYYArg);

                // Convert the named Fcn/Op1/Op2/Op3 to an unnamed form
                //   DO NOT INCREMENT THE RefCnt
                ConvertNamedFopToUnnamed (lpYYMem, soMOP, TKT_OP1IMMED, TKT_OP1AFO, TKT_OP1DFN, FALSE);

                // Skip to the next save area element
                lpYYMem++;

                break;

            case TKT_OP2NAMED:
                // Copy the root token
                YYCopyToMem (lpYYMem, lpYYArg);

                // Convert the named Fcn/Op1/Op2/Op3 to an unnamed form
                //   DO NOT INCREMENT THE RefCnt
                ConvertNamedFopToUnnamed (lpYYMem, soDOP, TKT_OP2IMMED, TKT_OP2AFO, TKT_OP2DFN, FALSE);

                // Skip to the next save area element
                lpYYMem++;

                break;

            case TKT_OP3NAMED:
                // Copy the root token
                YYCopyToMem (lpYYMem, lpYYArg);

                // Convert the named Fcn/Op1/Op2/Op3 to an unnamed form
                //   DO NOT INCREMENT THE RefCnt
                ConvertNamedFopToUnnamed (lpYYMem, soMOP, TKT_OP3IMMED, TKT_OP1AFO, TKT_OP1DFN, FALSE);

                // Skip to the next save area element
                lpYYMem++;

                break;

            case TKT_VARNAMED:
            case TKT_CHRSTRAND:
            case TKT_NUMSTRAND:
            case TKT_NUMSCALAR:
            case TKT_VARARRAY:
            case TKT_DELAFO:
            case TKT_AXISIMMED:
            case TKT_AXISARRAY:
            case TKT_STRNAMED:
            case TKT_FILLJOT:
                DbgBrk ();

            case TKT_VARIMMED:
            case TKT_FCNIMMED:
            case TKT_OP1IMMED:
            case TKT_OP2IMMED:
            case TKT_OP3IMMED:
            case TKT_FCNARRAY:
            case TKT_FCNAFO:
            case TKT_OP1AFO:
            case TKT_OP2AFO:
            case TKT_FCNDFN:
            case TKT_OP1DFN:
            case TKT_OP2DFN:
                // Copy the root token
                YYCopyToMem (lpYYMem++, lpYYArg);

                break;

            defstop
                break;
        } // End SWITCH

        // Count it in
        TknCount++;

        Assert (lpYYArg->lpplYYArgCurry EQ NULL);
        Assert (lpYYArg->lpplYYIdxCurry EQ NULL);
        Assert (lpYYArg->lpplYYOpRCurry EQ NULL);

        // If there's a curried function/left operand, ...
        if (lpYYArg->lpplYYOpLCurry NE NULL)
        {
            LPPL_YYSTYPE lpYYTmp;

            // Copy the curried function/left operand ptr
            lpYYTmp = lpYYArg->lpplYYOpLCurry;

            // If this is OPTRAIN, avoid duplicate YYFree
            if (lpYYArg->tkToken.tkFlags.TknType NE TKT_OP1IMMED
             || lpYYArg->tkToken.tkData.tkChar   NE UTF16_OPTRAIN)
            {
                // YYFree the old YYSTYPE
                YYFree (lpYYArg); lpYYArg = NULL;
            } // End IF

            // Go around again with the curried function
            lpYYArg = lpYYTmp;
        } else
            break;
    } // End WHILE

    // YYFree the old YYSTYPE
    YYFree (lpYYArg); lpYYArg = NULL;

    // Save the token count in the root
    lpYYMem0->TknCount = TknCount;

    // Return as the overall total
    *lpTknCount += TknCount;

    Assert (YYCheckInuse (lpYYMem));

    return lpYYMem;
} // End YYCopyFcnTrn


#ifdef DEBUG
//***************************************************************************
//  $YYCheckInuse
//
//  Debug routine to check YYInuse flag
//***************************************************************************

UBOOL _YYCheckInuse
    (LPPL_YYSTYPE lpYYRes,              // Ptr to YYSTYPE to check
     LPCHAR       lpFileName,           // Ptr to filename where allocated
     UINT         uLineNum)             // Line # where allocated

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If it's within the allocated range, ...
    if (lpMemPTD->lpYYRes <= lpYYRes
     &&                      lpYYRes < &lpMemPTD->lpYYRes[lpMemPTD->numYYRes])
    {
        WCHAR wszTemp[1024];

        // If it's inuse, ...
        if (lpYYRes->YYInuse)
            return TRUE;

        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"YYCheckInuse:  YYRes at %p index %04X not Inuse -- YYFree'd at %S#%d (%S#%d)",
                    lpYYRes,
                    lpYYRes->YYIndex,
                    lpYYRes->lpFileName,
                    lpYYRes->uLineNum,
                    lpFileName,
                    uLineNum);
        DbgMsgW (wszTemp);

        return FALSE;
    } else
        return TRUE;
} // End _YYCheckInuse
#endif


//***************************************************************************
//  End of File: yyfns.c
//***************************************************************************
