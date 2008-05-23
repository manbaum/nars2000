//***************************************************************************
//  NARS2000 -- User-Defined Function/Operator Execution Routines
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
#include "aplerrors.h"
#include "resdebug.h"
#include "sysvars.h"
#include "externs.h"
#include "pertab.h"
#include "dfnhdr.h"
#include "sis.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $ExecDfnGlbProto_EM_YY
//
//  Execute a user-defined function/operator on a prototype
//***************************************************************************

LPPL_YYSTYPE ExecDfnGlbProto_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFcnStr,            // Ptr to function strand
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)
{
    HGLOBAL hGlbProto;              // Prototype global memory handle

    // Get the user-defined function/operator global memory handle
    hGlbProto = lptkFcnStr->tkData.tkGlbData;

    Assert (GetSignatureGlb (ClrPtrTypeDirAsGlb (hGlbProto)) EQ DFN_HEADER_SIGNATURE);

    // Execute the user-defined function/operator on the arg using the []PROTOTYPE entry point
    return ExecDfnGlb_EM_YY (ClrPtrTypeDirAsGlb (hGlbProto),// User-defined function/operator global memory handle
                             lptkLftArg,                    // Ptr to left arg token (may be NULL if monadic)
              (LPPL_YYSTYPE) lptkFcnStr,                    // Ptr to function strand
                             NULL,                          // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                             lptkRhtArg,                    // Ptr to right arg token
                             LINENUM_PROTOTYPE);            // Starting line # (see LINE_NUMS)
} // End ExecDfnGlbProto_EM_YY


//***************************************************************************
//  $ExecDfnGlb_EM_YY
//
//  Execute a user-defined function/operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecDfnGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecDfnGlb_EM_YY
    (HGLOBAL      hGlbDfnHdr,       // User-defined function/operator global memory handle
     LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStr,       // Ptr to function strand (may be NULL if not an operator and no axis)
     LPTOKEN      lptkAxis,         // Ptr to axis token (may be NULL -- used only if function strand is NULL)
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LINE_NUMS    startLineNum)     // Starting line # (see LINE_NUMS)

{
    LPPL_YYSTYPE lpYYFcnStrLft,     // Ptr to left operand function strand (may be NULL if not an operator)
                 lpYYFcnStrRht;     // Ptr to right operand function strand (may be NULL if monadic operator or not an operator)

    // If there's a function strand, ...
    if (lpYYFcnStr NE NULL)
        // Check for axis operator
        lptkAxis = CheckAxisOper (lpYYFcnStr);

    // If there's room for a left operand, ...
    if (lpYYFcnStr
     && lpYYFcnStr->TknCount > (UINT) (1 + (lptkAxis NE NULL)))
    {
        // Set ptr to left operand
        lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

        // If there's room for a right operand, ...
        if (lpYYFcnStr->TknCount > (UINT) (lpYYFcnStrLft->TknCount + (lpYYFcnStrLft - lpYYFcnStr)))
            // Set ptr to right operand
            lpYYFcnStrRht = &lpYYFcnStrLft[lpYYFcnStrLft->TknCount];
        else
            lpYYFcnStrRht = NULL;
    } else
        lpYYFcnStrLft = lpYYFcnStrRht = NULL;

    // Call common routine
    return
      ExecDfnOprGlb_EM_YY (hGlbDfnHdr,      // User-defined function/operator global memory handle
                           lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
                           lpYYFcnStrLft,   // Ptr to left operand function strand (may be NULL if not an operator and no axis)
                           lpYYFcnStr,      // Ptr to function strand (may be NULL if not an operator and no axis)
                           lpYYFcnStrRht,   // Ptr to right operand function strand (may be NULL if not an operator and no axis)
                           lptkAxis,        // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                           lptkRhtArg,      // Ptr to right arg token
                           startLineNum);   // Starting line # (see LINE_NUMS)
} // End ExecDfnGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecDfnOprGlb_EM_YY
//
//  Execute a user-defined function/operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecDfnOprGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecDfnOprGlb_EM_YY
    (HGLOBAL      hGlbDfnHdr,       // User-defined function/operator global memory handle
     LPTOKEN      lptkLftArg,       // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrLft,    // Ptr to left operand function strand (may be NULL if not an operator and no axis)
     LPPL_YYSTYPE lpYYFcnStr,       // Ptr to function strand (may be NULL if not an operator and no axis)
     LPPL_YYSTYPE lpYYFcnStrRht,    // Ptr to right operand function strand (may be NULL if not an operator and no axis)
     LPTOKEN      lptkAxis,         // Ptr to axis token (may be NULL -- used only if function strand is NULL)
     LPTOKEN      lptkRhtArg,       // Ptr to right arg token
     LINE_NUMS    startLineNum)     // Starting line # (see LINE_NUMS)

{
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht;        // Right ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht;        // Right ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht;        // Right ...
    LPSYMENTRY   lpSymEntryBeg,     // Ptr to start of SYMENTRYs on the SIS
                 lpSymEntryNxt,     // Ptr to next     ...
                 lpSymLftFcn = NULL,// Ptr to original tkSym in the named left operand
                 lpSymRhtFcn = NULL;// ...                                right ...
    HGLOBAL      hGlbPTD = NULL;    // PerTabData global memory handle
    LPPERTABDATA lpMemPTD = NULL;   // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYFcnTmpLft,     // Ptr to temp left operand function strand (may be NULL if not an operator)
                 lpYYFcnTmpRht;     // Ptr to temp right operand function strand (may be NULL if monadic operator or not an operator)
    LPTOKEN      lptkLftTmp,        // Ptr to temp left arg token
                 lptkRhtTmp;        // ...         right
    TOKEN        tkLftTmp,          // Temp left arg token
                 tkRhtTmp;          // ...  right ...
    SYMENTRY     symLftArg,         // Temp SYMENTRY for left  arg
                 symLftFcn,         // ...               left  operand
                 symRhtFcn,         // ...               right operand
                 symRhtArg;         // ...               right arg
    BOOL         bNamedLftFcn,      // TRUE iff the left operand is a named function
                 bNamedRhtFcn;      // ...          right ...

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

    // If there's no left arg token and this function requires a left arg,
    //   signal a SYNTAX ERROR
    if (lptkLftArg EQ NULL
     && lpMemDfnHdr->FcnValence NE FCNVALENCE_AMB
     && lpMemDfnHdr->numLftArgSTE NE 0)
        goto SYNTAX_EXIT;

    // If there's a left arg token and this function has no left arg,
    //   signal a SYNTAX ERROR
    if (lptkLftArg NE NULL
     && lpMemDfnHdr->numLftArgSTE EQ 0)
        goto SYNTAX_EXIT;

    // If there is an axis token and this function does not accept
    //   an axis operator, signal an SYNTAX ERROR
    if (lptkAxis NE NULL
     && lpMemDfnHdr->steAxisOpr EQ NULL)
        goto SYNTAX_EXIT;

    // If there's no right arg token and this function requires a right arg,
    //   signal a SYNTAX ERROR
    if (lptkRhtArg EQ NULL
     && lpMemDfnHdr->numRhtArgSTE NE 0)
        goto SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    if (lptkLftArg)
        AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    if (lptkRhtArg)
        AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // If the DFN left arg is present and a list, make sure the left arg token
    //   is a scalar, or a vector of the proper length
    if (lptkLftArg NE NULL && lpMemDfnHdr->ListLft)
    {
        if (IsMultiRank (aplRankLft))
            goto LEFT_RANK_EXIT;
        if (IsVector (aplRankLft)
         && lpMemDfnHdr->numLftArgSTE NE aplNELMLft)
            goto LEFT_LENGTH_EXIT;
    } // End IF

    // If the DFN right arg is present and a list, make sure the right arg token
    //   is a scalar, or a vector of the proper length
    if (lptkRhtArg NE NULL && lpMemDfnHdr->ListRht)
    {
        if (IsMultiRank (aplRankRht))
            goto RIGHT_RANK_EXIT;
        if (IsVector (aplRankRht)
         && lpMemDfnHdr->numRhtArgSTE NE aplNELMRht)
            goto RIGHT_LENGTH_EXIT;
    } // End IF

    // Split cases based upon the starting line #
    switch (startLineNum)
    {
        case LINENUM_ONE:
            startLineNum = 1;

            break;

        case LINENUM_IDENTITY:
            startLineNum = lpMemDfnHdr->nIdentityLine;

            break;

        case LINENUM_INVERSE:
            startLineNum = lpMemDfnHdr->nInverseLine;

            break;

        case LINENUM_PROTOTYPE:
            // If not defined (=0), enter at line 1
            startLineNum = max (1, lpMemDfnHdr->nPrototypeLine);

            break;

        case LINENUM_SINGLETON:
            startLineNum = lpMemDfnHdr->nSingletonLine;

            break;

        defstop
            break;
    } // End SWITCH

    // Check for non-existant label
    if (startLineNum EQ 0)
        goto DOMAIN_EXIT;
RESTART_EXCEPTION_EXECDFNGLB:
    __try
    {
        // Fill in the SIS header for a User-Defined Function/Operator
        FillSISNxt (lpMemPTD,                   // Ptr to PerTabData global memory
                    NULL,                       // Semaphore handle (Filled in by ExecuteFunction_EM_YY)
                    lpMemDfnHdr->DfnType,       // DfnType
                    lpMemDfnHdr->FcnValence,    // FcnValence
                    FALSE,                      // Suspended
                    FALSE);                     // LinkIntoChain
        // Fill in the non-default SIS header entries
        lpMemPTD->lpSISNxt->hGlbDfnHdr   = hGlbDfnHdr;
        lpMemPTD->lpSISNxt->hGlbFcnName  = lpMemDfnHdr->steFcnName->stHshEntry->htGlbName;
        lpMemPTD->lpSISNxt->DfnAxis      = lpMemDfnHdr->DfnAxis;
        lpMemPTD->lpSISNxt->PermFn       = lpMemDfnHdr->PermFn;
        lpMemPTD->lpSISNxt->CurLineNum   = 1;
        lpMemPTD->lpSISNxt->NxtLineNum   = 2;
////////lpMemPTD->lpSISNxt->numLabels    =              // Filled in below
        lpMemPTD->lpSISNxt->numFcnLines  = lpMemDfnHdr->numFcnLines;
////////lpMemPTD->lpSISNxt->lpSISNxt     =              // Filled in below
#ifdef DEBUG
        dprintfW (L"~~Localize:    %08X (%s)", lpMemPTD->lpSISNxt, L"ExecDfnGlb_EM_YY");
#endif
    } __except (CheckException (GetExceptionInformation (), "ExecDfnGlb_EM_YY"))
    {
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_ACCESS_VIOLATION:
            {
                MEMORY_BASIC_INFORMATION mbi;

                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                // See how many pages are already allocated
                VirtualQuery (lpMemPTD->lpSISNxt,
                             &mbi,
                              sizeof (mbi));

                // Check for no allocation as yet
                if (mbi.State EQ MEM_RESERVE)
                    mbi.RegionSize = 0;

                // Allocate more memory to the YYRes buffer
                if (VirtualAlloc (lpMemPTD->lpSISNxt,
                                  mbi.RegionSize + DEF_SIS_INCRSIZE * sizeof (SYMENTRY),
                                  MEM_COMMIT,
                                  PAGE_READWRITE) NE NULL)
                    goto RESTART_EXCEPTION_EXECDFNGLB;

                // Fall through to never-never-land

            } // End EXCEPTION_ACCESS_VIOLATION

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    //***************************************************************
    // Errors beyond this point must call Unlocalize
    //***************************************************************

    lpMemPTD->lpSISCur               = lpMemPTD->lpSISNxt;

    // In case any of the operands/args is a named var/fcn and that same name
    //   is local to the function/operator, we need to copy the STE of the name
    //   or else it'll get wiped out by a call to LocalizeSymEntries before we
    //   get to use it in InitVarSTEs/InitFcnSTEs
    if (lptkLftArg)
    {
        // Copy the token
        tkLftTmp = *lptkLftArg;
        lptkLftTmp = &tkLftTmp;

        // If the token is named, ...
        if (IsTknNamed (lptkLftTmp->tkFlags.TknType))
        {
            // Copy the STE in case its name conflicts with a local
            symLftArg = *lptkLftTmp->tkData.tkSym;
            lptkLftTmp->tkData.tkSym = &symLftArg;
        } // End IF
    } else
        lptkLftTmp = NULL;

    if (lpYYFcnStrLft)
    {
        // Copy the PL_YYSTYPE
        lpYYFcnTmpLft = lpYYFcnStrLft;

        // If the token is named, ...
        if (bNamedLftFcn = IsTknNamed (lpYYFcnTmpLft->tkToken.tkFlags.TknType))
        {
            // Copy the STE in case its name conflicts with a local
            lpSymLftFcn = lpYYFcnTmpLft->tkToken.tkData.tkSym;
            symLftFcn = *lpSymLftFcn;
            lpYYFcnTmpLft->tkToken.tkData.tkSym = &symLftFcn;
        } // End IF
    } else
        lpYYFcnTmpLft = NULL;

    if (lpYYFcnStrRht)
    {
        // Copy the PL_YYSTYPE
        lpYYFcnTmpRht = lpYYFcnStrRht;

        // If the token is named, ...
        if (bNamedRhtFcn = IsTknNamed (lpYYFcnTmpRht->tkToken.tkFlags.TknType))
        {
            // Copy the STE in case its name conflicts with a local
            lpSymRhtFcn = lpYYFcnTmpRht->tkToken.tkData.tkSym;
            symRhtFcn = *lpSymRhtFcn;
            lpYYFcnTmpRht->tkToken.tkData.tkSym = &symRhtFcn;
        } // End IF
    } else
        lpYYFcnTmpRht = NULL;

    if (lptkRhtArg)
    {
        // Copy the token
        tkRhtTmp = *lptkRhtArg;
        lptkRhtTmp = &tkRhtTmp;

        // If the token is named, ...
        if (IsTknNamed (lptkRhtTmp->tkFlags.TknType))
        {
            // Copy the STE in case its name conflicts with a local
            symRhtArg = *lptkRhtTmp->tkData.tkSym;
            lptkRhtTmp->tkData.tkSym = &symRhtArg;
        } // End IF
    } else
        lptkRhtTmp = NULL;

    // Point to the destination SYMENTRYs
    lpSymEntryBeg =
    lpSymEntryNxt = (LPSYMENTRY) ByteAddr (lpMemPTD->lpSISNxt, sizeof (SIS_HEADER));

    // Copy onto the SIS the current STEs for each local
    //   and undefine all but the system vars

    // Localize and clear the result STEs
    lpSymEntryNxt =
      LocalizeSymEntries (lpSymEntryNxt,
                          lpMemDfnHdr->numResultSTE,
                          (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offResultSTE),
                          lpMemPTD);
    // Localize and clear the left arg STEs
    lpSymEntryNxt =
      LocalizeSymEntries (lpSymEntryNxt,
                          lpMemDfnHdr->numLftArgSTE,
                          (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offLftArgSTE),
                          lpMemPTD);
    // Localize and clear the left operand STE
    lpSymEntryNxt =
      LocalizeSymEntries (lpSymEntryNxt,
                          lpMemDfnHdr->steLftOpr NE NULL,
                         &lpMemDfnHdr->steLftOpr,
                          lpMemPTD);
    // Localize and clear the axis operand STE
    lpSymEntryNxt =
      LocalizeSymEntries (lpSymEntryNxt,
                          lpMemDfnHdr->steAxisOpr NE NULL,
                         &lpMemDfnHdr->steAxisOpr,
                          lpMemPTD);
    // Localize and clear the right operand STE
    lpSymEntryNxt =
      LocalizeSymEntries (lpSymEntryNxt,
                          lpMemDfnHdr->steRhtOpr NE NULL,
                         &lpMemDfnHdr->steRhtOpr,
                          lpMemPTD);
    // Localize and clear the right arg STEs
    lpSymEntryNxt =
      LocalizeSymEntries (lpSymEntryNxt,
                          lpMemDfnHdr->numRhtArgSTE,
                          (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offRhtArgSTE),
                          lpMemPTD);
    // Localize and clear the locals STEs
    lpSymEntryNxt =
      LocalizeSymEntries (lpSymEntryNxt,
                          lpMemDfnHdr->numLocalsSTE,
                          (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offLocalsSTE),
                          lpMemPTD);
    // Search for line labels, localize and initialize them
    lpSymEntryNxt =
      LocalizeLabels (lpSymEntryNxt,
                     &lpMemPTD->lpSISNxt->numLabels,
                      lpMemDfnHdr,
                      lpMemPTD);
    // Save the # LPSYMENTRYs localized
    lpMemPTD->lpSISNxt->numSymEntries = lpSymEntryNxt - lpSymEntryBeg;

    // Save as new SISNxt ptr
    lpMemPTD->lpSISNxt                = (LPSIS_HEADER) lpSymEntryNxt;

    // Setup the left arg STEs
    InitVarSTEs (lptkLftTmp,
                 lpMemDfnHdr->numLftArgSTE,
                 (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offLftArgSTE));
    // Setup the left operand STE
    if (lpYYFcnTmpLft)
    {
        // Split cases based upon fcn vs. var
        if (!IsTknFcnOpr (&lpYYFcnTmpLft->tkToken))
            InitVarSTEs (&lpYYFcnTmpLft->tkToken,
                          lpMemDfnHdr->steLftOpr NE NULL,
                         &lpMemDfnHdr->steLftOpr);
        else
        if (!InitFcnSTEs (lpYYFcnTmpLft,
                          lpMemDfnHdr->steLftOpr NE NULL,
                         &lpMemDfnHdr->steLftOpr))
            goto UNLOCALIZE_EXIT;
    } // End IF

    // Setup the axis operand STE
    InitVarSTEs (lptkAxis,
                 lpMemDfnHdr->steAxisOpr NE NULL,
                &lpMemDfnHdr->steAxisOpr);
    // Setup the right operand STE
    if (lpYYFcnTmpRht)
    {
        // Split cases based upon fcn vs. var
        if (!IsTknFcnOpr (&lpYYFcnTmpRht->tkToken))
            InitVarSTEs (&lpYYFcnTmpRht->tkToken,
                          lpMemDfnHdr->steRhtOpr NE NULL,
                         &lpMemDfnHdr->steRhtOpr);
        else
        if (!InitFcnSTEs (lpYYFcnTmpRht,
                          lpMemDfnHdr->steRhtOpr NE NULL,
                         &lpMemDfnHdr->steRhtOpr))
            goto UNLOCALIZE_EXIT;
    } // End IF

    // Setup the right arg STEs
    InitVarSTEs (lptkRhtTmp,
                 lpMemDfnHdr->numRhtArgSTE,
                 (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offRhtArgSTE));
    // We no longer need these ptrs
    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    lpYYRes =
      ExecuteFunction_EM_YY (startLineNum, &lpYYFcnStr->tkToken);

    goto NORMAL_EXIT;

UNLOCALIZE_EXIT:
    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    Unlocalize ();

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbDfnHdr && lpMemDfnHdr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF

    if (hGlbPTD && lpMemPTD)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF

    // Restore named left operand tkSym
    if (lpSymLftFcn && bNamedLftFcn)
        lpYYFcnTmpLft->tkToken.tkData.tkSym = lpSymLftFcn;

    // Restore named right operand tkSym
    if (lpSymRhtFcn && bNamedRhtFcn)
        lpYYFcnTmpRht->tkToken.tkData.tkSym = lpSymRhtFcn;

    return lpYYRes;
} // End ExecDfnOprGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $ExecuteFunction_EM_YY
//
//  Execute a function, line by line
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecuteFunction_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecuteFunction_EM_YY
    (UINT         uLineNum,         // Starting line # (origin-1)
     LPTOKEN      lptkFunc)         // Ptr to function token

{
    LPPL_YYSTYPE   lpYYRes = NULL;  // Ptr to the result
    LPDFN_HEADER   lpMemDfnHdr;     // Ptr to user-defined function/operator header
    HGLOBAL        hGlbDfnHdr,      // User-defined function/operator global memory handle
                   hGlbTxtLine,     // Line text global memory handle
                   hGlbTknLine,     // Tokenized line global memory handle
                   hGlbPTD;         // PerTabData global memory handle
    LPPERTABDATA   lpMemPTD = NULL; // Ptr to PerTabData global memory
    LPFCNLINE      lpFcnLines;      // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HWND           hWndSM;          // Session Manager window handle
    HANDLE         hSemaphore;      // Semaphore handle
    UINT           numResultSTE,    // # result STEs (may be zero if no result)
                   numRes;          // Loop counter
    LPSYMENTRY    *lplpSymEntry;    // Ptr to 1st result STE
    HGLOBAL        hGlbTknHdr;      // Tokenized header global memory handle
    BOOL           bRet;            // TRUE iff result is valid
    EXIT_TYPES     exitType;        // Return code from ParseLine

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the user-defined function/operator header global memory handle
    hGlbDfnHdr = lpMemPTD->lpSISCur->hGlbDfnHdr;

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

    // Get the tokenized header global memory handle
    hGlbTknHdr = lpMemDfnHdr->hGlbTknHdr;

    // Get the SM window handle
    hWndSM = lpMemPTD->hWndSM;

    // Save current line #
    lpMemPTD->lpSISCur->CurLineNum = uLineNum;
    lpMemPTD->lpSISCur->NxtLineNum = uLineNum + 1;

    // Create a semaphore
    hSemaphore =
    lpMemPTD->lpSISCur->hSemaphore =
      CreateSemaphore (NULL,            // No security attrs
                       0,               // Initial count (non-signalled)
                       64*1024,         // Maximum count
                       NULL);           // No name
#ifdef DEBUG
    DisplayFcnLine (lpMemDfnHdr->hGlbTxtHdr, lpMemPTD, 0);
#endif
    // Loop through the function lines
    while (0 < uLineNum && uLineNum <= lpMemPTD->lpSISCur->numFcnLines)
    {
        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Get the starting line's token & text global memory handles
        hGlbTxtLine = lpFcnLines[uLineNum - 1].hGlbTxtLine;
        hGlbTknLine = lpFcnLines[uLineNum - 1].hGlbTknLine;

#ifdef DEBUG
        DisplayFcnLine (hGlbTxtLine, lpMemPTD, uLineNum);
#endif
        // We no longer need these ptrs
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

        // Execute the function line
        exitType =
          ParseLine (hWndSM,                // Session Manager window handle
                     hGlbTxtLine,           // Line text global memory handle
                     hGlbTknLine,           // Tokenixed line global memory handle
                     NULL,                  // Ptr to line text global memory
                     hGlbPTD);              // PerTabData global memory handle
        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // If suspended, wait for the semaphore to trigger
        if (lpMemPTD->lpSISCur->Suspended)
        {
            HWND hWndEC;        // Edit Control window handle

            // Get the Edit Control window handle
            hWndEC = (HWND) GetWindowLongW (hWndSM, GWLSF_HWNDEC);

            // If we're at a stop, display the error message
            //   along with the function name/line #
            if (lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_STOP)
            {
                LPAPLCHAR    lpMemName;     // Ptr to function name global memory
                LPSIS_HEADER lpSISCur;      // Ptr to current SIS entry

                // Get a ptr to the current SIS
                lpSISCur = lpMemPTD->lpSISCur;

                // Display the error message
                AppendLine (ERRMSG_WS_FULL APPEND_NAME, FALSE, TRUE);

                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (lpSISCur->hGlbFcnName);

                // Format the name and line #
                wsprintfW (lpwszTemp,
                           L"%s[%d]",
                           lpMemName,
                           lpSISCur->CurLineNum);
                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

                // Display the function name and line #
                AppendLine (lpwszTemp, FALSE, TRUE);
            } // End IF

            // Display the default prompt
            DisplayPrompt (hWndEC, 2);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

#ifdef DEBUG
            dprintfW (L"~~WaitForSingleObject (ENTRY):  %s (%S#%d)", L"ExecuteFunction_EM_YY", FNLN);
#endif
            // Wait for the semaphore to trigger
            WaitForSingleObject (hSemaphore,            // Ptr to handle to wait for
                                 INFINITE);             // Timeout value in milliseconds
#ifdef DEBUG
            dprintfW (L"~~WaitForSingleObject (EXIT):  %s (%S#%d)", L"ExecuteFunction_EM_YY", FNLN);
#endif
            // Lock the memory to get a ptr to it
            lpMemPTD = MyGlobalLock (hGlbPTD);

            // Get the exit type from the semaphore restart
            exitType = lpMemPTD->ImmExecExitType;
        } // End IF

        // Get the user-defined function/operator header global memory handle
        //   in case it changed while waiting for the semaphore to trigger
        Assert (hGlbDfnHdr EQ lpMemPTD->lpSISCur->hGlbDfnHdr);
        hGlbDfnHdr = lpMemPTD->lpSISCur->hGlbDfnHdr;

        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

        // If we're suspended, resetting, or stopping:  break
        if (lpMemPTD->lpSISCur->Suspended
         || lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
            break;

        // Get next line #
        uLineNum = lpMemPTD->lpSISCur->NxtLineNum++;
        lpMemPTD->lpSISCur->CurLineNum = uLineNum;
    } // End WHILE
#ifdef DEBUG
    DisplayFcnLine (NULL, lpMemPTD, NEG1U);
#endif
    // Close the semaphore handle as it isn't used anymore
    CloseHandle (hSemaphore); hSemaphore = NULL;

    // If we're initially resetting through []ERROR/[]ES,
    //   convert to execute resetting
    if (exitType EQ EXITTYPE_QUADERROR_INIT)
    {
        // Convert to reset execute
        exitType = EXITTYPE_QUADERROR_EXEC;
        lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_QUADERROR_EXEC;

        // Set the caret to point to the user-defined function/operator
        ErrorMessageSetToken (lptkFunc);
    } // End IF

    // If we're resetting or stopping, Unlocalize
    if (lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
        goto UNLOCALIZE_EXIT;

    // If we're suspended, don't Unlocalize
    if (lpMemPTD->lpSISCur->Suspended)
        goto ERROR_EXIT;

    // Get the # STEs in the result
    numResultSTE = lpMemDfnHdr->numResultSTE;

    // Get ptr to 1st result STE
    lplpSymEntry = (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offResultSTE);

    // Assume all is OK
    bRet = TRUE;

    // Ensure that all STEs in the result have a value
    switch (numResultSTE)
    {
        case 0:         // No result
            // Make a PL_YYSTYPE NoValue entry
            lpYYRes = MakeNoValue_YY (lptkFunc);

            break;

        case 1:         // Single result name:  Copy it as the result from this function
            // Check for a value
            if (!(*lplpSymEntry)->stFlags.Value)
                // Make a PL_YYSTYPE NoValue entry
                lpYYRes = MakeNoValue_YY (lptkFunc);
            else
            {
                // Allocate a new YYRes
                lpYYRes = YYAlloc ();

                // If it's immediate, ...
                if ((*lplpSymEntry)->stFlags.Imm)
                {
                    // Fill in the result token
                    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                    lpYYRes->tkToken.tkFlags.ImmType   = (*lplpSymEntry)->stFlags.ImmType;
////////////////////lpYYRes->tkToken.tkFlags.NoDisplay =                // Set below
                    lpYYRes->tkToken.tkData.tkLongest  = (*lplpSymEntry)->stData.stLongest;
                    lpYYRes->tkToken.tkCharIndex       = NEG1U;
                } else
                {
                    // Fill in the result token
                    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;             // Already zero from YYAlloc
////////////////////lpYYRes->tkToken.tkFlags.NoDisplay =                // Set below
                    lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDir ((*lplpSymEntry)->stData.stGlbData);
                    lpYYRes->tkToken.tkCharIndex       = NEG1U;
                } // End IF/ELSE

                // Copy the non-displayable flag
                lpYYRes->tkToken.tkFlags.NoDisplay = lpMemDfnHdr->NoDispRes;
            } // End IF/ELSE

            break;

        default:        // Multiple result names:  Allocate storage to hold them
        {
            APLUINT ByteRes;        // # bytes in the result
            HGLOBAL hGlbRes;        // Result global memory handle
            LPVOID  lpMemRes;       // Ptr to result global memory

            // Calculate space needed for the result
            ByteRes = CalcArraySize (ARRAY_NESTED, numResultSTE, 1);

            // Allocate space for the result
            hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
            if (!hGlbRes)
            {
                // Set the error message
                ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

                // Set the error token
                ErrorMessageSetToken (NULL);

                goto ERROR_EXIT;
            } // End IF

            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = ARRAY_NESTED;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = numResultSTE;
            lpHeader->Rank       = 1;
#undef  lpHeader

            // Fill in the dimension
            *VarArrayBaseToDim (lpMemRes) = numResultSTE;

            // Skip over the header and dimension
            lpMemRes = VarArrayBaseToData (lpMemRes, 1);

            // Fill in the result
            for (numRes = 0; numRes < numResultSTE; numRes++)
            {
                // If it's immediate, copy the LPSYMENTRY
                if (lplpSymEntry[numRes]->stFlags.Imm)
                    *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lplpSymEntry[numRes]);
                else
                // Otherwise, copy the HGLOBAL
                    *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir (lplpSymEntry[numRes]->stData.stGlbData);
            } // End FOR

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;             // Already zero from YYAlloc
////////////lpYYRes->tkToken.tkFlags.NoDisplay =                // Set below
            lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
            lpYYRes->tkToken.tkCharIndex       = NEG1U;

            // Copy the non-displayable flag
            lpYYRes->tkToken.tkFlags.NoDisplay = lpMemDfnHdr->NoDispRes;

            // See if it fits into a lower (but not necessarily smaller) datatype
            TypeDemote (&lpYYRes->tkToken);

            break;
        } // End default
    } // End SWITCH
UNLOCALIZE_EXIT:
    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    Unlocalize ();
ERROR_EXIT:
    // We no longer need these ptrs
    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return lpYYRes;
} // End ExecuteFunction_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $DisplayFcnLine
//
//  Display a given function line
//***************************************************************************

void DisplayFcnLine
    (HGLOBAL      hGlbTxtLine,      // Function line text global memory handle (may be NULL if uLineNum EQ NEG1U)
     LPPERTABDATA lpMemPTD,         // Ptr to PerTabData global memory
     UINT         uLineNum)         // Function line # (NEG1U for terminating)

{
    LPMEMTXT_UNION lpMemTxtLine;
    LPAPLCHAR      lpMemFcnName;

    // If the handle is valid, ...
    if (hGlbTxtLine)
        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

    // Lock the memory to get a ptr to it
    lpMemFcnName = MyGlobalLock (lpMemPTD->lpSISCur->hGlbFcnName);

    if (uLineNum EQ NEG1U)
        wsprintfW (lpwszTemp,
                   L"Terminating <%s>",
                   lpMemFcnName);
    else
        wsprintfW (lpwszTemp,
                   L"Executing line %d of <%s>:  %s",
                   uLineNum,
                   lpMemFcnName,
                  &lpMemTxtLine->C);
    DbgMsgW (lpwszTemp);

    // We no longer need this ptr
    MyGlobalUnlock (lpMemPTD->lpSISCur->hGlbFcnName); lpMemFcnName = NULL;

    // If the handle is valid, ...
    if (hGlbTxtLine)
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
} // End DisplayFcnline


//***************************************************************************
//  $CheckDfnExitError_EM
//
//  Check on user-defined function/operator error on exit
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CheckDfnExitError_EM"
#else
#define APPEND_NAME
#endif

BOOL CheckDfnExitError_EM
    (LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    LPSIS_HEADER   lpSISCur;            // Ptr to current SIS header
    LPDFN_HEADER   lpMemDfnHdr = NULL;  // Ptr to user-defined function/operator header
    BOOL           bRet = FALSE;        // TRUE iff error on exit
    UINT           numResultSTE,        // # result STEs (may be zero if no result)
                   numRes;              // Loop counter
    LPSYMENTRY    *lplpSymEntry;        // Ptr to 1st result STE
    LPTOKEN_HEADER lptkHdr = NULL;      // Ptr to header of tokenized line
    LPTOKEN        lptkLine;            // Ptr to tokenized line

    // Get a ptr to the current SIS
    lpSISCur = lpMemPTD->lpSISCur;

    // If the current level isn't a user-defined function/operator, quit
    if (lpSISCur->DfnType NE DFNTYPE_OP1
     && lpSISCur->DfnType NE DFNTYPE_OP2
     && lpSISCur->DfnType NE DFNTYPE_FCN)
        goto NORMAL_EXIT;

    // If the function is already suspended, quit
    if (lpSISCur->Suspended)
        goto NORMAL_EXIT;

    // If we're resetting, quit
    if (lpSISCur->ResetFlag NE RESETFLAG_NONE)
        goto NORMAL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLock (lpSISCur->hGlbDfnHdr);

    // If the next line # would not exit the function, quit
    if (0 < lpSISCur->NxtLineNum
     &&     lpSISCur->NxtLineNum <= lpMemDfnHdr->numFcnLines)
        goto NORMAL_EXIT;

    // Get the # STEs in the result
    numResultSTE = lpMemDfnHdr->numResultSTE;

    // Get ptr to 1st result STE
    lplpSymEntry = (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offResultSTE);

    // Ensure that all STEs in the result have a value
    switch (numResultSTE)
    {
        case 0:         // No result:  Nothing to check
            break;

        case 1:         // Single result name:  If it has a value, ensure it's a var
            // Check for a value
            if ((*lplpSymEntry)->stFlags.Value)
            {
                // Lock the memory to get a ptr to it
                lptkHdr = MyGlobalLock (lpMemDfnHdr->hGlbTknHdr);

                // Get ptr to the tokens in the line
                lptkLine = (LPTOKEN) ByteAddr (lptkHdr, sizeof (TOKEN_HEADER));

                // Loop 'til we point to the first named token
                while (lptkLine->tkFlags.TknType NE TKT_VARNAMED)
                    lptkLine++;

                // If it's not a variable, ...
                if ((*lplpSymEntry)->stFlags.stNameType NE NAMETYPE_VAR)
                {
                    // Set the error message
                    ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

                    // Initialize for ERROR_EXIT code
                    numRes = 0;

                    goto ERROR_EXIT;
                } // End IF
            } // End IF/ELSE

            break;

        default:        // Multiple result names:  Ensure they all have a value and all are vars
            // Lock the memory to get a ptr to it
            lptkHdr = MyGlobalLock (lpMemDfnHdr->hGlbTknHdr);

            // Get ptr to the tokens in the line
            lptkLine = (LPTOKEN) ByteAddr (lptkHdr, sizeof (TOKEN_HEADER));

            // Loop 'til we point to the first named token
            while (lptkLine->tkFlags.TknType NE TKT_VARNAMED)
                lptkLine++;

            // Ensure all result names have a value and are vars
            for (bRet = TRUE, numRes = 0; bRet && numRes < numResultSTE; numRes++)
            {
                // If the name has no value, ...
                if (!lplpSymEntry[numRes]->stFlags.Value)
                {
                    // Set the error message
                    ErrorMessageIndirect (ERRMSG_VALUE_ERROR APPEND_NAME);

                    goto ERROR_EXIT;
                } else
                // If the name is not a variable, ...
                if (lplpSymEntry[numRes]->stFlags.stNameType NE NAMETYPE_VAR)
                {
                    // Set the error message
                    ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

                    goto ERROR_EXIT;
                } // End IF/ELSE/...
            } // End FOR

            break;
    } // End SWITCH

    // Mark as not error on exit
    bRet = FALSE;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Set the error token
    ErrorMessageSetToken (&lptkLine[numRes]);

    // Mark as an error on line 0
    lpSISCur->CurLineNum = 0;

    // Mark as suspended
    lpSISCur->Suspended = TRUE;

    // Mark as error on exit
    bRet = TRUE;
NORMAL_EXIT:
    if (lptkHdr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemDfnHdr->hGlbTknHdr); lptkHdr = NULL;
    } // End IF

    if (lpMemDfnHdr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpSISCur->hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF

    return bRet;
} // End CheckDfnExitError_EM
#undef  APPEND_NAME


//***************************************************************************
//  $Unlocalize
//
//  Unlocalize STEs on the innermost level
//    and strip off one level
//***************************************************************************

BOOL Unlocalize
    (void)

{
    HGLOBAL      hGlbPTD,           // PerTabData global memory handle
                 hGlbData;          // STE global memory handle
    LPPERTABDATA lpMemPTD = NULL;   // Ptr to PerTabData global memory
    UINT         numSymEntries,     // # LPSYMENTRYs localized
                 numSym;            // Loop counter
    LPSYMENTRY   lpSymEntryNxt;     // Ptr to next localized LPSYMENTRY on the SIS
    BOOL         bRet = TRUE;       // TRUE iff the result is valid
    RESET_FLAGS  resetFlag;         // Reset flag (see RESET_FLAGS)

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // If the SI is non-empty, ...
    if (lpMemPTD->SILevel)
    {
        Assert (lpMemPTD->lpSISCur NE NULL);
#ifdef DEBUG
        dprintfW (L"~~Unlocalize:  %08X to %08X", lpMemPTD->lpSISCur, lpMemPTD->lpSISCur->lpSISPrv);
#endif
        // Save the reset flag
        resetFlag = lpMemPTD->lpSISCur->ResetFlag;

        // Free the outgoing value of []EM
        FreeResultGlobalVar (lpMemPTD->lpSISCur->hGlbQuadEM); lpMemPTD->lpSISCur->hGlbQuadEM = NULL;

        // Get # LPSYMENTRYs on the stack
        numSymEntries = lpMemPTD->lpSISCur->numSymEntries;

        // Point to the localized LPSYMENTRYs
        lpSymEntryNxt = (LPSYMENTRY) ByteAddr (lpMemPTD->lpSISCur, sizeof (SIS_HEADER));

        // Loop through the # LPSYMENTRYs
        for (numSym = 0; numSym < numSymEntries; numSym++)
        // If the hash entry is valid, ...
        if (lpSymEntryNxt[numSym].stHshEntry)
        {
            LPSYMENTRY lpSymEntryCur;       // Ptr to SYMENTRY to release & unlocalize

            // Get the ptr to the corresponding SYMENTRY
            lpSymEntryCur = lpSymEntryNxt[numSym].stHshEntry->htSymEntry;

            // Release the current value of the STE
            //   if it's not immediate and has a value
            if (!lpSymEntryCur->stFlags.Imm && lpSymEntryCur->stFlags.Value)
            {
                // Get the global memory handle
                hGlbData = lpSymEntryCur->stData.stGlbData;

                // Split cases based upon the nametype
                switch (lpSymEntryCur->stFlags.stNameType)
                {
                    case NAMETYPE_VAR:
                        // stData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir (hGlbData));

                        // Free the global var
                        FreeResultGlobalVar (hGlbData); hGlbData = NULL;

                        break;

                    case NAMETYPE_FN0:
                        // stData is a user-defined function/operator
                        Assert (lpSymEntryCur->stFlags.UsrDfn);

                        // Free the global user-defined function/operator
                        FreeResultGlobalDfn (hGlbData);

                        break;

                    case NAMETYPE_FN12:
                    case NAMETYPE_OP1:
                    case NAMETYPE_OP2:
                        if (lpSymEntryCur->stFlags.UsrDfn)
                            // Free the global user-defined function/operator
                            FreeResultGlobalDfn (hGlbData);
                        else
                            // Free the global function array
                            FreeResultGlobalFcn (hGlbData);
                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End IF

            // Restore the previous STE
            *lpSymEntryCur = lpSymEntryNxt[numSym];
        } // End FOR

        // Strip the level from the stack
        lpMemPTD->lpSISNxt = lpMemPTD->lpSISCur;
        lpMemPTD->lpSISCur = lpMemPTD->lpSISCur->lpSISPrv;
        if (lpMemPTD->lpSISCur)
        {
            lpMemPTD->lpSISCur->lpSISNxt = NULL;

            // Transfer the reset flag up one level
            lpMemPTD->lpSISCur->ResetFlag = resetFlag;
        } // End IF
#ifdef DEBUG
        // Zero the old entry
        ZeroMemory (lpMemPTD->lpSISNxt,
                    sizeof (*lpMemPTD->lpSISNxt)
                  + numSymEntries * sizeof (lpSymEntryNxt));
#endif
        // Back off the SI Level
        lpMemPTD->SILevel--;
    } // End IF
///NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return bRet;
} // End Unlocalize


//***************************************************************************
//  $LocalizeLabels
//
//  Localize and initialize all line labels
//***************************************************************************

LPSYMENTRY LocalizeLabels
    (LPSYMENTRY   lpSymEntryNxt,    // Ptr to next SYMENTRY save area
     LPUINT       lpNumLabels,      // Ptr to # labels initialized so far
     LPDFN_HEADER lpMemDfnHdr,      // Ptr to user-defined function/operator header
     LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    UINT           numFcnLines,     // # lines in the function
                   uLineNum;        // Loop counter
    LPFCNLINE      lpFcnLines;      // Ptr to array of function line structs (FCNLINE[numFcnLines])
    LPTOKEN_HEADER lptkHdr;         // Ptr to header of tokenized line
    LPTOKEN        lptkLine;        // Ptr to tokenized line
    STFLAGS stFlagsClr = {0};       // Flags for clearing an STE

    // Set the Inuse flag
    stFlagsClr.Inuse = TRUE;

    // Get # lines in the function
    numFcnLines = lpMemDfnHdr->numFcnLines;

    // Get ptr to array of function line structs (FCNLINE[numFcnLines])
    lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

    // Loop through the function lines
    for (*lpNumLabels = uLineNum = 0; uLineNum < numFcnLines; uLineNum++)
    {
        UINT numTokens;     // # tokens in the line

        // Lock the memory to get a ptr to it
        lptkHdr = MyGlobalLock (lpFcnLines->hGlbTknLine);

        // Get the # tokens in the line
        numTokens = lptkHdr->TokenCnt;

        // Get ptr to the tokens in the line
        lptkLine = (LPTOKEN) ByteAddr (lptkHdr, sizeof (TOKEN_HEADER));

        Assert (lptkLine[0].tkFlags.TknType EQ TKT_EOL
             || lptkLine[0].tkFlags.TknType EQ TKT_EOS);

        // If there are at least three tokens, ...
        //   (TKT_EOL, TKT_VARNAMED, TKT_LABELSEP)
        if (numTokens >= 3)
        {
            if (lptkLine[2].tkFlags.TknType EQ TKT_LABELSEP
             && lptkLine[1].tkFlags.TknType EQ TKT_VARNAMED)
            {
                LPSYMENTRY lpSymEntrySrc;   // Ptr to source SYMENTRY

                // Get the source LPSYMENTRY
                lpSymEntrySrc = lptkLine[1].tkData.tkSym;

                // stData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lpSymEntrySrc) EQ PTRTYPE_STCONST);

                // Copy the old SYMENTRY to the SIS
                *lpSymEntryNxt++ = *lpSymEntrySrc;

                // Clear the STE flags & data
                *((PUINT_PTR) &lpSymEntrySrc->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////////lpSymEntrySrc->stData.stLongest = 0;        // stLongest set below

                // Initialize the SYMENTRY to an integer constant
                lpSymEntrySrc->stFlags.Imm        = TRUE;
                lpSymEntrySrc->stFlags.ImmType    = IMMTYPE_INT;
                lpSymEntrySrc->stFlags.Inuse      = TRUE;
                lpSymEntrySrc->stFlags.Value      = TRUE;
                lpSymEntrySrc->stFlags.ObjName    = OBJNAME_USR;
                lpSymEntrySrc->stFlags.stNameType = NAMETYPE_VAR;
                lpSymEntrySrc->stFlags.DfnLabel   = TRUE;
                lpSymEntrySrc->stData.stInteger   = uLineNum + 1;

                // Set the ptr to the previous entry to the STE on the stack
                lpSymEntrySrc->stPrvEntry       = &lpSymEntryNxt[-1];

                // Save the SI level for this SYMENTRY
                Assert (lpMemPTD->SILevel);
                lpSymEntrySrc->stSILevel        = lpMemPTD->SILevel - 1;

                // Count in another label
                (*lpNumLabels)++;
            } // End IF
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (lpFcnLines->hGlbTknLine); lptkHdr = NULL; lptkLine = NULL;

        // Skip to the next struct
        lpFcnLines++;
    } // End FOR

    return lpSymEntryNxt;
} // End LocalizeLabels


//***************************************************************************
//  $InitVarSTEs
//
//  Initialize variable arg STEs
//***************************************************************************

void InitVarSTEs
    (LPTOKEN      lptkArg,      // Ptr to variable token
     UINT         numArgSTE,    // # STEs to initialize
     LPSYMENTRY  *lplpSymEntry) // Ptr to LPSYMENTRYs

{
    UINT    uSym;               // Loop counter
    HGLOBAL hGlbArg;            // Arg global memory handle
    LPVOID  lpMemArg;           // Ptr to arg global memory
    STFLAGS stFlagsClr = {0};   // Flags for clearing an STE

    // Set the Inuse flag
    stFlagsClr.Inuse = TRUE;

    // If the token is defined, ...
    if (lptkArg && numArgSTE)
    {
        // Split cases based upon the variable token type
        switch (lptkArg->tkFlags.TknType)
        {
            case TKT_VARIMMED:
            case TKT_AXISIMMED:
                // Loop through the LPSYMENTRYs
                for (uSym = 0; uSym < numArgSTE; uSym++, lplpSymEntry++)
                {
                    // Clear the STE flags & data
                    *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////////////(*lplpSymEntry)->stData.stLongest = 0;      // stLongest set below

                    (*lplpSymEntry)->stFlags.Imm        = TRUE;
                    (*lplpSymEntry)->stFlags.ImmType    = lptkArg->tkFlags.ImmType;
                    (*lplpSymEntry)->stFlags.Value      = TRUE;
                    (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
                    (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
                    (*lplpSymEntry)->stData.stLongest   = lptkArg->tkData.tkLongest;
                } // End FOR

                return;

            case TKT_VARARRAY:
            case TKT_AXISARRAY:
                // Get the arg global memory handle
                hGlbArg = lptkArg->tkData.tkGlbData;

                break;

            case TKT_VARNAMED:
                // stData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lptkArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

                // If it's immediate, ...
                if (lptkArg->tkData.tkSym->stFlags.Imm)
                {
                    // Loop through the LPSYMENTRYs
                    for (uSym = 0; uSym < numArgSTE; uSym++, lplpSymEntry++)
                    {
                        // Clear the STE flags & data
                        *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////////////////(*lplpSymEntry)->stData.stLongest = 0;      // stLongest set below

                        (*lplpSymEntry)->stFlags.Imm        = TRUE;
                        (*lplpSymEntry)->stFlags.ImmType    = lptkArg->tkData.tkSym->stFlags.ImmType;
                        (*lplpSymEntry)->stFlags.Value      = TRUE;
                        (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stLongest   = lptkArg->tkData.tkSym->stData.stLongest;
                    } // End FOR

                    return;
                } else
                    // Get the arg global memory handle
                    hGlbArg = lptkArg->tkData.tkSym->stData.stGlbData;

                break;

            defstop
                break;
        } // End SWITCH

        // st/tkData is a valid HGLOBAL variable array
        Assert (IsGlbTypeVarDir (hGlbArg));

        // If there's only one STE, it gets the whole arg
        if (numArgSTE EQ 1)
        {
            // Clear the STE flags & data
            *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////(*lplpSymEntry)->stData.stLongest = 0;      // stLongest set below

////////////(*lplpSymEntry)->stFlags.Imm        = 0;    // Already zero from previous initialization
////////////(*lplpSymEntry)->stFlags.ImmType    = 0;    // Already zero from previous initialization
            (*lplpSymEntry)->stFlags.Value      = TRUE;
            (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
            (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
            (*lplpSymEntry)->stData.stGlbData   = CopySymGlbDir (hGlbArg);
        } else
        {
            APLSTYPE aplTypeArg;            // Arg storage type
            APLNELM  aplNELMArg;            // Arg NELM
            APLRANK  aplRankArg;            // Arg rank
            UINT     uBitIndex;             // Bit index for Booleans
            APLINT   apaOffArg,             // APA offset
                     apaMulArg;             // ... multiplier

            // Lock the memory to get a ptr to it
            lpMemArg = MyGlobalLock (ClrPtrTypeDirAsGlb (hGlbArg));

#define lpHeader        ((LPVARARRAY_HEADER) lpMemArg)
            aplTypeArg = lpHeader->ArrType;
            aplNELMArg = lpHeader->NELM;
            aplRankArg = lpHeader->Rank;
#undef  lpHeader

            // These were checked for above, but it never hurts to test again
            Assert (aplNELMArg EQ numArgSTE);
            Assert (IsVector (aplRankArg));

            // Skip over the header and dimensions to the data
            lpMemArg = VarArrayBaseToData (lpMemArg, aplRankArg);

            // In case the arg is Boolean
            uBitIndex = 0;

            // If the arg is APA, ...
            if (IsSimpleAPA (aplTypeArg))
            {
#define lpAPA   ((LPAPLAPA) lpMemArg)
                // Get the APA parameters
                apaOffArg = lpAPA->Off;
                apaMulArg = lpAPA->Mul;
#undef  lpAPA
            } // End IF

            // Loop through the LPSYMENTRYs
            for (uSym = 0; uSym < numArgSTE; uSym++, lplpSymEntry++)
            {
                // Clear the STE flags & data
                *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
                (*lplpSymEntry)->stData.stLongest = 0;

                // Split cases based upon the arg storage type
                switch (aplTypeArg)
                {
                    case ARRAY_BOOL:
                        (*lplpSymEntry)->stFlags.Imm        = TRUE;
                        (*lplpSymEntry)->stFlags.ImmType    = IMMTYPE_INT;
                        (*lplpSymEntry)->stFlags.Value      = TRUE;
                        (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stBoolean   = BIT0 & ((*(LPAPLBOOL) lpMemArg) >> uBitIndex);

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;                  // Start over
                            ((LPAPLBOOL) lpMemArg)++;       // Skip to next byte
                        } // End IF

                        break;

                    case ARRAY_INT:
                        (*lplpSymEntry)->stFlags.Imm        = TRUE;
                        (*lplpSymEntry)->stFlags.ImmType    = IMMTYPE_INT;
                        (*lplpSymEntry)->stFlags.Value      = TRUE;
                        (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stInteger   = *((LPAPLINT) lpMemArg)++;

                        break;

                    case ARRAY_FLOAT:
                        (*lplpSymEntry)->stFlags.Imm        = TRUE;
                        (*lplpSymEntry)->stFlags.ImmType    = IMMTYPE_FLOAT;
                        (*lplpSymEntry)->stFlags.Value      = TRUE;
                        (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stFloat     = *((LPAPLFLOAT) lpMemArg)++;

                        break;

                    case ARRAY_CHAR:
                        (*lplpSymEntry)->stFlags.Imm        = TRUE;
                        (*lplpSymEntry)->stFlags.ImmType    = IMMTYPE_CHAR;
                        (*lplpSymEntry)->stFlags.Value      = TRUE;
                        (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stChar      = *((LPAPLCHAR) lpMemArg)++;

                        break;

                    case ARRAY_APA:
                        (*lplpSymEntry)->stFlags.Imm        = TRUE;
                        (*lplpSymEntry)->stFlags.ImmType    = IMMTYPE_INT;
                        (*lplpSymEntry)->stFlags.Value      = TRUE;
                        (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
                        (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
                        (*lplpSymEntry)->stData.stInteger   = apaOffArg + apaMulArg * uSym;

                        break;

                    case ARRAY_HETERO:
                    case ARRAY_NESTED:
                        // Split cases based upon the arg ptr type
                        switch (GetPtrTypeInd (lpMemArg))
                        {
                            case PTRTYPE_STCONST:
                                (*lplpSymEntry)->stFlags.Imm        = TRUE;
                                (*lplpSymEntry)->stFlags.ImmType    = (*(LPAPLHETERO) lpMemArg)->stFlags.ImmType;
                                (*lplpSymEntry)->stFlags.Value      = TRUE;
                                (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
                                (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
                                (*lplpSymEntry)->stData             = (*(LPAPLHETERO) lpMemArg)->stData;

                                // Skip to next element in arg
                                ((LPAPLHETERO) lpMemArg)++;

                                break;

                            case PTRTYPE_HGLOBAL:
////////////////////////////////(*lplpSymEntry)->stFlags.Imm        = 0;    // Already zero from previous initialization
////////////////////////////////(*lplpSymEntry)->stFlags.ImmType    = 0;    // Already zero from previous initialization
                                (*lplpSymEntry)->stFlags.Value      = TRUE;
                                (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
                                (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
                                (*lplpSymEntry)->stData.stGlbData   = CopySymGlbInd (lpMemArg);

                                // Skip to next element in arg
                                ((LPAPLNESTED) lpMemArg)++;

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            // We no longer need this ptr
            MyGlobalUnlock (ClrPtrTypeDirAsGlb (hGlbArg)); lpMemArg = NULL;
        } // End IF/ELSE
    } // End IF
} // End InitVarSTEs


//***************************************************************************
//  $InitFcnSTEs
//
//  Initialize function arg STEs
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- InitFcnSTEs"
#else
#define APPEND_NAME
#endif

BOOL InitFcnSTEs
    (LPPL_YYSTYPE lpYYArg,      // Ptr to arg PL_YYSTYPE
     UINT         numArgSTE,    // # STEs to initialize
     LPSYMENTRY  *lplpSymEntry) // Ptr to LPSYMENTRYs

{
    STFLAGS stFlagsClr = {0};   // Flags for clearing an STE

    // Set the Inuse flag
    stFlagsClr.Inuse = TRUE;

    // If the token is defined, ...
    if (lpYYArg && numArgSTE)
    {
        Assert (numArgSTE EQ 1);

        // Split cases based upon the function count
        if (lpYYArg->TknCount EQ 1
         && lpYYArg->tkToken.tkFlags.TknType EQ TKT_FCNIMMED)
        {
            // Clear the STE flags & data
            *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////(*lplpSymEntry)->stData.stLongest = 0;      // stLongest set below

            (*lplpSymEntry)->stFlags.Imm        = TRUE;
            (*lplpSymEntry)->stFlags.ImmType    = lpYYArg->tkToken.tkFlags.ImmType;
            (*lplpSymEntry)->stFlags.Value      = TRUE;
            (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
            (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_FN12;
            (*lplpSymEntry)->stData.stLongest   = lpYYArg->tkToken.tkData.tkLongest;
        } else
        {
            APLUINT ByteRes;            // # bytes in the result
            HGLOBAL hGlbRes;            // Result global memory handle
            LPVOID  lpMemRes;           // Ptr to result global memory
            UINT    uFcn;               // Loop counter

            // Calculate space needed for the result
            ByteRes = CalcFcnSize (lpYYArg->TknCount);

            // Allocate global memory for the function array
            // N.B.: Conversion from APLUINT to UINT.
            Assert (ByteRes EQ (UINT) ByteRes);
            hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
            if (!hGlbRes)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPFCNARRAY_HEADER) lpMemRes)
            // Fill in the header
            lpHeader->Sig.nature  = FCNARRAY_HEADER_SIGNATURE;
            lpHeader->fnNameType  = NAMETYPE_FN12;
            lpHeader->RefCnt      = 1;
            lpHeader->tknNELM     = lpYYArg->TknCount;
////////////lpHeader->hGlbTxtLine = NULL;           // Already zero from GHND
#undef  lpHeader

            // Skip over the header to the data (PL_YYSTYPEs)
            lpMemRes = FcnArrayBaseToData (lpMemRes);

            // Copy the PL_YYSTYPEs to the global memory object
            // Test cases:  +/rank[2] a     (as defined operator)
            //              +/{rank}[2] a   (as primitive operator)
            CopyMemory (lpMemRes, lpYYArg, lpYYArg->TknCount * sizeof (PL_YYSTYPE));

            // Loop through the functions incrementing the RefCnt as appropriate
            for (uFcn = 0; uFcn < lpYYArg->TknCount; uFcn++)
            // Split cases based upon the token type
            switch (lpYYArg[uFcn].tkToken.tkFlags.TknType)
            {
                case TKT_FCNNAMED:
                    // If it's not an immediate, ...
                    if (!lpYYArg[uFcn].tkToken.tkData.tkSym->stFlags.Imm)
                        // Increment the RefCnt
                        IncrRefCntDir (lpYYArg[uFcn].tkToken.tkData.tkSym->stData.stGlbData);

                    break;

                case TKT_FCNARRAY:
                    // Increment the RefCnt
                    IncrRefCntDir (lpYYArg[uFcn].tkToken.tkData.tkGlbData);

                    break;

                default:
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // Clear the STE flags & data
            *((PUINT_PTR) &(*lplpSymEntry)->stFlags) &= *(PUINT_PTR) &stFlagsClr;
////////////(*lplpSymEntry)->stData.stLongest = 0;      // stLongest set below

////////////(*lplpSymEntry)->stFlags.Imm        = 0;    // Already zero from above
////////////(*lplpSymEntry)->stFlags.ImmType    = 0;    // Already zero from above
            (*lplpSymEntry)->stFlags.Value      = TRUE;
            (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
            (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_FN12;
            (*lplpSymEntry)->stData.stGlbData   = MakePtrTypeGlb (hGlbRes);
        } // End IF/ELSE
    } // End IF

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                              &lpYYArg->tkToken);
    return FALSE;
} // End InitFcnSTEs
#undef  APPEND_NAME


//***************************************************************************
//  $LocalizeSymEntries
//
//  Localize LPSYMENTRYs
//***************************************************************************

LPSYMENTRY LocalizeSymEntries
    (LPSYMENTRY   lpSymEntryNxt,        // Ptr to next SYMENTRY save area
     UINT         numSymEntries,        // # SYMENTRYs to localize
     LPSYMENTRY  *lplpSymEntrySrc,      // Ptr to ptr to source SYMENTRY
     LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    UINT uSym;                          // Loop counter

    // Loop through the SYMENTRYs
    for (uSym = 0; uSym < numSymEntries; uSym++)
    {
        // Copy the SYMENTRY to the SIS
        *lpSymEntryNxt++ = **lplpSymEntrySrc;

        // Erase the Symbol Table Entry
        EraseSTE (*lplpSymEntrySrc);

        // Set the ptr to the previous entry to the STE on the stack
        (*lplpSymEntrySrc)->stPrvEntry = &lpSymEntryNxt[-1];

        // Save the SI level for this SYMENTRY
        Assert (lpMemPTD->SILevel);
        (*lplpSymEntrySrc)->stSILevel  = lpMemPTD->SILevel - 1;

        // Skip to next source entry
        lplpSymEntrySrc++;
    } // End FOR

    return lpSymEntryNxt;
} // End LocalizeSymEntries


//***************************************************************************
//  End of File: execdfn.c
//***************************************************************************
