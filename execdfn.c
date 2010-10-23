//***************************************************************************
//  NARS2000 -- User-Defined Function/Operator Execution Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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
//  $ExecDfnGlbProto_EM_YY
//
//  Execute a user-defined function/operator on a prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecDfnGlbProto_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecDfnGlbProto_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFcnStr,            // Ptr to function strand
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)
{
    HGLOBAL      hGlbProto;         // Prototype global memory handle
    LPPL_YYSTYPE lpYYRes,           // Ptr to the result
                 lpYYRes2;          // Ptr to secondary result

    // Get the user-defined function/operator global memory handle
    hGlbProto = lptkFcnStr->tkData.tkGlbData;

    Assert (GetSignatureGlb_PTB (hGlbProto) EQ DFN_HEADER_SIGNATURE);

    // Execute the user-defined function/operator on the arg using the []PRO entry point
    lpYYRes2 =
      ExecDfnGlb_EM_YY (hGlbProto,              // User-defined function/operator global memory handle
                        lptkLftArg,             // Ptr to left arg token (may be NULL if monadic)
         (LPPL_YYSTYPE) lptkFcnStr,             // Ptr to function strand
                        NULL,                   // Ptr to axis token (may be NULL -- used only if function strand is NULL)
                        lptkRhtArg,             // Ptr to right arg token
                        LINENUM_PRO);           // Starting line # (see LINE_NUMS)
    // If the result is valid, ...
    if (lpYYRes2)
    {
        // Check for NoValue
        if (IsTokenNoValue (&lpYYRes2->tkToken))
        {
            // Free the YYRes (but not the storage)
            YYFree (lpYYRes2); lpYYRes2 = NULL;

            goto VALUE_EXIT;
        } // End IF

        // Convert the result into a prototype
        lpYYRes =
          PrimFnMonDownTack_EM_YY (lptkFcnStr,          // Ptr to function token
                                  &lpYYRes2->tkToken,   // Ptr to right arg token
                                   NULL);               // Ptr to axis token (may be NULL)
        FreeResult (lpYYRes2); YYFree (lpYYRes2); lpYYRes2 = NULL;
    } else
        lpYYRes = lpYYRes2;

    return lpYYRes;

VALUE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                               lptkFcnStr);
    return lpYYRes2;
} // End ExecDfnGlbProto_EM_YY
#undef  APPEND_NAME


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
     LINE_NUMS    startLineType)    // Starting line type (see LINE_NUMS)

{
    LPPL_YYSTYPE lpYYFcnStrLft,     // Ptr to left operand function strand (may be NULL if not an operator)
                 lpYYFcnStrRht;     // Ptr to right operand function strand (may be NULL if monadic operator or not an operator)

    // Clear the ptr type bits
    hGlbDfnHdr = ClrPtrTypeDir (hGlbDfnHdr);

    // If there's a function strand, ...
    if (lpYYFcnStr NE NULL)
        // Check for axis operator
        lptkAxis = CheckAxisOper (lpYYFcnStr);

    // If there's room for a left operand, ...
    if (lpYYFcnStr
     && lpYYFcnStr->TknCount > (APLU3264) (1 + (lptkAxis NE NULL)))
    {
        // Set ptr to left operand
        lpYYFcnStrLft = &lpYYFcnStr[1 + (lptkAxis NE NULL)];

        // If there's room for a right operand, ...
        if (lpYYFcnStr->TknCount > (APLU3264) (lpYYFcnStrLft->TknCount + (lpYYFcnStrLft - lpYYFcnStr)))
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
                           startLineType);  // Starting line type (see LINE_NUMS)
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
     LINE_NUMS    startLineType)    // Starting line type (see LINE_NUMS)

{
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht;        // Right ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht;        // Right ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht;        // Right ...
    LPSYMENTRY   lpSymLftFcn = NULL,// Ptr to original tkSym in the named left operand
                 lpSymRhtFcn = NULL;// ...                                right ...
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
    UBOOL        bNamedLftFcn,      // TRUE iff the left operand is a named function
                 bNamedRhtFcn;      // ...          right ...
    UBOOL        bOldExecuting;     // Old value of bExecuting
    HWND         hWndEC;            // Edit Ctrl window handle
    UINT         startLineNum;      // Starting line #

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the Edit Ctrl window handle
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (lpMemPTD->hWndSM, GWLSF_HWNDEC);

    // Indicate that we're executing
    bOldExecuting = lpMemPTD->bExecuting; lpMemPTD->bExecuting = TRUE;

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

    // If there's no left arg token and this function requires a left arg,
    //   and we're not entering at LINENUM_ID,
    //   signal a VALENCE ERROR
    if (lptkLftArg EQ NULL
     && lpMemDfnHdr->FcnValence NE FCNVALENCE_AMB
     && lpMemDfnHdr->numLftArgSTE NE 0
     && startLineType NE LINENUM_ID)
        goto VALENCE_EXIT;

    // If there's a left arg token and this function has no left arg,
    //   signal a VALENCE ERROR
    if (lptkLftArg NE NULL
     && lpMemDfnHdr->numLftArgSTE EQ 0)
        goto VALENCE_EXIT;

    // If there is an axis token and this function does not accept
    //   an axis operator, signal an SYNTAX ERROR
    if (lptkAxis NE NULL
     && lpMemDfnHdr->steAxisOpr EQ NULL)
        goto AXIS_SYNTAX_EXIT;

    // If there's no right arg token and this function requires a right arg,
    //   signal a SYNTAX ERROR
    if (lptkRhtArg EQ NULL
     && lpMemDfnHdr->numRhtArgSTE NE 0)
        goto RIGHT_SYNTAX_EXIT;

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
    switch (startLineType)
    {
        case LINENUM_ONE:
            startLineNum = 1;

            break;

        case LINENUM_ID:
            startLineNum = lpMemDfnHdr->nSysLblId;

            break;

        case LINENUM_INV:
            startLineNum = lpMemDfnHdr->nSysLblInv;

            break;

        case LINENUM_PRO:
            startLineNum = lpMemDfnHdr->nSysLblPro;

            break;

        case LINENUM_SGL:
            startLineNum = lpMemDfnHdr->nSysLblSgl;

            break;

        defstop
            break;
    } // End SWITCH

    // Check for non-existant label
    if (startLineNum EQ 0)
        goto DOMAIN_EXIT;

    // Fill in the SIS header for a User-Defined Function/Operator
    FillSISNxt (lpMemPTD,                   // Ptr to PerTabData global memory
                NULL,                       // Semaphore handle (Filled in by ExecuteFunction_EM_YY)
                lpMemDfnHdr->DfnType,       // DfnType
                lpMemDfnHdr->FcnValence,    // FcnValence
                FALSE,                      // Suspended
                TRUE,                       // Restartable
                FALSE);                     // LinkIntoChain
    // Fill in the non-default SIS header entries
    lpMemPTD->lpSISNxt->hGlbDfnHdr   = hGlbDfnHdr;
    lpMemPTD->lpSISNxt->hGlbFcnName  = lpMemDfnHdr->steFcnName->stHshEntry->htGlbName;
    lpMemPTD->lpSISNxt->DfnAxis      = lpMemDfnHdr->DfnAxis;
    lpMemPTD->lpSISNxt->PermFn       = lpMemDfnHdr->PermFn;
    lpMemPTD->lpSISNxt->CurLineNum   = 1;
    lpMemPTD->lpSISNxt->NxtLineNum   = 2;
////lpMemPTD->lpSISNxt->numLabels    =              // Filled in by LocalizeAll
    lpMemPTD->lpSISNxt->numFcnLines  = lpMemDfnHdr->numFcnLines;
////lpMemPTD->lpSISNxt->lpSISNxt     =              // Filled in by LocalizeAll
#ifdef DEBUG
    dprintfWL9 (L"~~Localize:    %p (%s)", lpMemPTD->lpSISNxt, L"ExecDfnGlb_EM_YY");
#endif

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

        // If the token type is named, ...
        if (IsTknTypeNamed (lptkLftTmp->tkFlags.TknType))
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

        // If the token type is named, ...
        if (bNamedLftFcn = IsTknTypeNamed (lpYYFcnTmpLft->tkToken.tkFlags.TknType))
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

        // If the token type is named, ...
        if (bNamedRhtFcn = IsTknTypeNamed (lpYYFcnTmpRht->tkToken.tkFlags.TknType))
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

        // If the token type is named, ...
        if (IsTknTypeNamed (lptkRhtTmp->tkFlags.TknType))
        {
            // Copy the STE in case its name conflicts with a local
            symRhtArg = *lptkRhtTmp->tkData.tkSym;
            lptkRhtTmp->tkData.tkSym = &symRhtArg;
        } // End IF
    } else
        lptkRhtTmp = NULL;

    // Localize all arguments, results, and locals
    LocalizeAll (lpMemPTD, lpMemDfnHdr);

    // Setup the left arg STEs
    InitVarSTEs (lptkLftTmp,
                 lpMemDfnHdr->numLftArgSTE,
                 (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offLftArgSTE));
    // Setup the left operand STE
    if (lpYYFcnTmpLft
     && lpYYFcnTmpLft->tkToken.tkFlags.TknType NE TKT_FILLJOT)
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
    if (lpYYFcnTmpRht
     && lpYYFcnTmpRht->tkToken.tkFlags.TknType NE TKT_FILLJOT)
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
            goto LEFT_UNLOCALIZE_EXIT;
    } // End IF

    // Setup the right arg STEs
    InitVarSTEs (lptkRhtTmp,
                 lpMemDfnHdr->numRhtArgSTE,
                 (LPAPLHETERO) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offRhtArgSTE));
    // We no longer need this ptr
    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

    // Execute the user-defined function/operator
    lpYYRes =
      ExecuteFunction_EM_YY (startLineNum, &lpYYFcnStr->tkToken);

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

LEFT_UNLOCALIZE_EXIT:
UNLOCALIZE_EXIT:
    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    UnlocalizeSTEs ();

    goto NORMAL_EXIT;

// The RANK, LENGTH, and SYNTAX errors don't point the caret to the
//   specific arg because that might confuse the user into thinking
//   that the error is with the construction of that arg rather than
//   its interaction with this user-defined function/operator.
LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    goto ERROR_EXIT;

VALENCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALENCE_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    goto ERROR_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

RIGHT_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpYYFcnStr->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // Restore the previous executing state
    lpMemPTD->bExecuting = bOldExecuting;

    if (hGlbDfnHdr && lpMemDfnHdr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
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
//  $LocalizeAll
//
//  Localize all args of a given function
//***************************************************************************

void LocalizeAll
    (LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     LPDFN_HEADER lpMemDfnHdr)          // Ptr to user-defined function/operator header

{
    LPSYMENTRY lpSymEntryBeg,           // Ptr to start of SYMENTRYs on the SIS
               lpSymEntryNxt;           // Ptr to next     ...

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
    lpMemPTD->lpSISNxt->numSymEntries = (UINT) (lpSymEntryNxt - lpSymEntryBeg);

    Assert (lpMemPTD->lpSISNxt->numSymEntries EQ
            (lpMemDfnHdr->numResultSTE
           + lpMemDfnHdr->numLftArgSTE
           + (lpMemDfnHdr->steLftOpr NE NULL)
           + (lpMemDfnHdr->steAxisOpr NE NULL)
           + (lpMemDfnHdr->steRhtOpr NE NULL)
           + lpMemDfnHdr->numRhtArgSTE
           + lpMemDfnHdr->numLocalsSTE
           + lpMemPTD->lpSISNxt->numLabels));
    // Save as new SISNxt ptr
    lpMemPTD->lpSISNxt                = (LPSIS_HEADER) lpSymEntryNxt;
} // End LocalizeAll


//***************************************************************************
//  $_CheckSymEntries
//
//  Debug routine to check on localized SYMENTRYs
//***************************************************************************

void _CheckSymEntries
    (LPSTR   lpFileName,            // Ptr to our caller's filename
     UINT    uLineNum)              // Caller's line #

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS header
    LPSYMENTRY   lpSymEntryNxt;     // Ptr to next localized LPSYMENTRY on the SIS
    UINT         numSymEntries,     // # LPSYMENTRYs localized
                 numSym;            // Loop counter

    // Get ptr to PerTabData global memory
    lpMemPTD = TlsGetValue (dwTlsPerTabData); // Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // If lpMemPTD isn't set, just exit
    if (lpMemPTD EQ NULL)
        return;

    // Get a ptr to the current SIS header
    lpSISCur = lpMemPTD->lpSISCur;

    while (lpSISCur)
    {
        // Get # LPSYMENTRYs on the stack
        numSymEntries = lpSISCur->numSymEntries;

        // Point to the start of the localized LPSYMENTRYs
        lpSymEntryNxt = (LPSYMENTRY) ByteAddr (lpSISCur, sizeof (SIS_HEADER));

        // Point to the end of the localize LPSYMENTRYs
        lpSymEntryNxt = &lpSymEntryNxt[numSymEntries - 1];

        // Loop through the LPSYMENTRYs on the stack
        for (numSym = 0; numSym < numSymEntries; numSym++, lpSymEntryNxt--)
        if (lpSymEntryNxt->stFlags.ObjName NE OBJNAME_LOD)
        {
            __try
            {
                if (lpSymEntryNxt->stHshEntry EQ NULL)
                    DbgBrk ();
                if (lpSymEntryNxt->stHshEntry->htSymEntry EQ NULL)
                    DbgBrk ();
            } __except (CheckException (GetExceptionInformation (), L"CheckSymEntries"))
            {
                wsprintfW (lpMemPTD->lpwszTemp,
                           L"CheckSymEntries was called from <%S> line #%d",
                           lpFileName,
                           uLineNum);
                AppendLine (lpMemPTD->lpwszTemp, FALSE, TRUE);

                FormatSTE (lpSymEntryNxt, lpMemPTD->lpwszTemp);
                AppendLine (lpMemPTD->lpwszTemp, FALSE, TRUE);

                FormatHTE (lpSymEntryNxt->stHshEntry, lpMemPTD->lpwszTemp, 0);
                AppendLine (lpMemPTD->lpwszTemp, FALSE, TRUE);

                // Display message for unhandled exception
                DisplayException ();
            } // End __try/__except
        } // End FOR/IF

        // Get a ptr to the previous (if any) entry
        lpSISCur = lpSISCur->lpSISPrv;
    } // End WHILE
} // End _CheckSymEntries


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
                   hGlbTknLine;     // Tokenized line global memory handle
    LPPERTABDATA   lpMemPTD = NULL; // Ptr to PerTabData global memory
    LPFCNLINE      lpFcnLines;      // Ptr to array of function line structs (FCNLINE[numFcnLines])
    HWND           hWndSM;          // Session Manager window handle
    HANDLE         hSemaphore;      // Semaphore handle
    UINT           numResultSTE,    // # result STEs (may be zero if no result)
                   numRes,          // Loop counter
                   uTokenCnt,       // # tokens in the function line
                   uTknNum;         // Next token # in the line
    LPSYMENTRY    *lplpSymEntry;    // Ptr to 1st result STE
    HGLOBAL        hGlbTknHdr;      // Tokenized header global memory handle
    UBOOL          bRet;            // TRUE iff result is valid
    EXIT_TYPES     exitType;        // Return code from ParseLine
    LPTOKEN_HEADER lpMemTknLine;    // Ptr to tokenized line global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the user-defined function/operator header global memory handle
    hGlbDfnHdr = lpMemPTD->lpSISCur->hGlbDfnHdr;

    // Lock the memory to get a ptr to it
    lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

    // If monitoring is on, increment the function counter
    if (lpMemDfnHdr->MonOn)
    {
        LPINTMONINFO lpMemMonInfo;          // Ptr to function line monitoring info

        // Lock the memory to get a ptr to it
        lpMemMonInfo = MyGlobalLock (lpMemDfnHdr->hGlbMonInfo);

        // Increment the function counter
        lpMemMonInfo[0].Count++;

        // We no longer need this ptr
        MyGlobalUnlock (lpMemDfnHdr->hGlbMonInfo); lpMemMonInfo = NULL;
    } // End IF

    // Get the tokenized header global memory handle
    hGlbTknHdr = lpMemDfnHdr->hGlbTknHdr;

    // Get the SM window handle
    hWndSM = lpMemPTD->hWndSM;

    // Initialize the exit type in case we fall through
    exitType = EXITTYPE_NONE;

    // Save current line & token #
    lpMemPTD->lpSISCur->CurLineNum = uLineNum;
    lpMemPTD->lpSISCur->NxtLineNum = uLineNum + 1;
    lpMemPTD->lpSISCur->NxtTknNum  = uTknNum = 0;

    // Create a semaphore
    hSemaphore =
    lpMemPTD->lpSISCur->hSemaphore =
      CreateSemaphoreW (NULL,           // No security attrs
                        0,              // Initial count (non-signalled)
                        64*1024,        // Maximum count
                        NULL);          // No name
#ifdef DEBUG
    DisplayFcnLine (lpMemDfnHdr->hGlbTxtHdr, lpMemPTD, 0);
#endif
    // Loop through the function lines
    while (0 < uLineNum && uLineNum <= lpMemPTD->lpSISCur->numFcnLines)
    {
        // Get ptr to array of function line structs (FCNLINE[numFcnLines])
        lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

        // Check for empty line
        if (lpFcnLines[uLineNum - 1].bEmpty)
            goto NEXTLINE;

        // Get the starting line's token & text global memory handles
        hGlbTxtLine = lpFcnLines[uLineNum - 1].hGlbTxtLine;
        hGlbTknLine = lpFcnLines[uLineNum - 1].hGlbTknLine;

        // Lock the memory to get a ptr to it
        lpMemTknLine = MyGlobalLock (hGlbTknLine);

        // Get the token count of this line
        uTokenCnt = lpMemTknLine->TokenCnt;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTknLine); lpMemTknLine = NULL;

        // If the starting token # is outside the token count, ...
        if (uTknNum >= uTokenCnt)
            goto NEXTLINE;
#ifdef DEBUG
        DisplayFcnLine (hGlbTxtLine, lpMemPTD, uLineNum);
#endif
        // If line monitoring is enabled, start it for this line
        if (lpMemDfnHdr->MonOn)
            StartStopMonInfo (lpMemDfnHdr, uLineNum, TRUE);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

        // Execute the line
        exitType =
          ParseLine (hWndSM,                // Session Manager window handle
                     hGlbTknLine,           // Tokenized line global memory handle
                     hGlbTxtLine,           // Text      ...
                     NULL,                  // Ptr to line text global memory
                     lpMemPTD,              // Ptr to PerTabData global memory
                     uLineNum,              // Function line # (1 for execute or immexec)
                     uTknNum,               // Starting token # in the above function line
                     hGlbDfnHdr,            // User-defined function/operator global memory handle (NULL = execute/immexec)
                     TRUE,                  // TRUE iff errors are acted upon
                     FALSE);                // TRUE iff executing only one stmt
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

        // If line monitoring is enabled, stop it for this line
        if (lpMemDfnHdr->MonOn)
            StartStopMonInfo (lpMemDfnHdr, uLineNum, FALSE);
RESTART_AFTER_ERROR:
        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

        // If suspended, wait for the semaphore to trigger
        if (lpMemPTD->lpSISCur->Suspended)
        {
            HWND hWndEC;        // Edit Ctrl window handle

            // Get the Edit Ctrl window handle
            (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

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
                wsprintfW (lpMemPTD->lpwszTemp,
                           L"%s[%d]",
                           lpMemName,
                           lpSISCur->CurLineNum);
                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

                // Display the function name and line #
                AppendLine (lpMemPTD->lpwszTemp, FALSE, TRUE);
            } // End IF

            // Display the default prompt
            DisplayPrompt (hWndEC, 2);
#ifdef DEBUG
            dprintfWL9 (L"~~WaitForSingleObject (ENTRY):  %s (%S#%d)", L"ExecuteFunction_EM_YY", FNLN);
#endif
            // Wait for the semaphore to trigger
            WaitForSingleObject (hSemaphore,            // Ptr to handle to wait for
                                 INFINITE);             // Timeout value in milliseconds
#ifdef DEBUG
            dprintfWL9 (L"~~WaitForSingleObject (EXIT):  %s (%S#%d)", L"ExecuteFunction_EM_YY", FNLN);
#endif
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
NEXTLINE:
        // Get next line #
        uLineNum = lpMemPTD->lpSISCur->NxtLineNum++;
        uTknNum  = lpMemPTD->lpSISCur->NxtTknNum;
        lpMemPTD->lpSISCur->CurLineNum = uLineNum;
        lpMemPTD->lpSISCur->NxtTknNum = 0;
    } // End WHILE

    // Split cases based upon the exit type
    switch (exitType)
    {
        case EXITTYPE_RESET_ONE:
        case EXITTYPE_RESET_ONE_INIT:
            // Change the reset flag as we're done with it
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

            // Make a PL_YYSTYPE NoValue entry
            lpYYRes = MakeNoValue_YY (lptkFunc);

            break;

        case EXITTYPE_RESET_ALL:
        case EXITTYPE_GOTO_ZILDE:
        case EXITTYPE_GOTO_LINE:
        case EXITTYPE_QUADERROR_INIT:
        case EXITTYPE_QUADERROR_EXEC:
        case EXITTYPE_ERROR:
        case EXITTYPE_STOP:
        case EXITTYPE_DISPLAY:
        case EXITTYPE_NODISPLAY:
        case EXITTYPE_NOVALUE:
            break;

        case EXITTYPE_NONE:
            // In case we don't execute any lines and one or more of
            //   the multiple results have no value, ...
            if (CheckDfnExitError_EM (lpMemPTD))
            {
                LPWCHAR lpwszLine = L":return";
                HGLOBAL hGlbTkn;

                // Tokenize the line
                hGlbTkn =
                  Tokenize_EM (lpwszLine,               // The line to tokenize (not necessarily zero-terminated)
                               lstrlenW (lpwszLine),    // NELM of lpwszLine
                               NULL,                    // Window handle for Edit Ctrl (may be NULL if lpErrHandFn is NULL)
                               1,                       // Function line # (0 = header)
                               NULL,                    // Ptr to error handling function (may be NULL)
                               FALSE);                  // TRUE iff we're tokenizing a Magic Function/Operator
                // Execute the line
                exitType =
                  ParseLine (hWndSM,                    // Session Manager window handle
                             hGlbTkn,                   // Tokenized line global memory handle
                             lpMemDfnHdr->hGlbTxtHdr,   // Text      ...
                             NULL,                      // Ptr to line text global memory
                             lpMemPTD,                  // Ptr to PerTabData global memory
                             1,                         // Function line # (1 for execute or immexec)
                             0,                         // Starting token # in the above function line
                             hGlbDfnHdr,                // User-defined function/operator global memory handle (NULL = execute/immexec)
                             TRUE,                      // TRUE iff errors are acted upon
                             FALSE);                    // TRUE iff executing only one stmt
                // We no longer need this resource
                MyGlobalFree (hGlbTkn); hGlbTkn = NULL;

                goto RESTART_AFTER_ERROR;
            } // End IF

            break;

        defstop
            break;
    } // End SWITCH
#ifdef DEBUG
    DisplayFcnLine (NULL, lpMemPTD, NEG1U);
#endif
    // Close the semaphore handle as it isn't used anymore
    CloseHandle (hSemaphore); hSemaphore = lpMemPTD->lpSISCur->hSemaphore = NULL;

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

    // If we're resetting or stopping, exit normally
    if (lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
        goto NORMAL_EXIT;

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
////////////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////////////////lpYYRes->tkToken.tkFlags.NoDisplay =                // Set below
                    lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDir_PTB ((*lplpSymEntry)->stData.stGlbData);
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

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            // Allocate space for the result
            hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (!hGlbRes)
                goto WSFULL_EXIT;

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
                    *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lplpSymEntry[numRes]);
                else
                // Otherwise, copy the HGLOBAL
                    *((LPAPLNESTED) lpMemRes)++ = CopySymGlbDir_PTB (lplpSymEntry[numRes]->stData.stGlbData);
            } // End FOR

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // Allocate a new YYRes
            lpYYRes = YYAlloc ();

            // Fill in the result token
            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
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

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Set the error message
    ErrorMessageIndirect (ERRMSG_WS_FULL APPEND_NAME);

    // Set the error token
    ErrorMessageSetToken (NULL);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

    return lpYYRes;
} // End ExecuteFunction_EM_YY
#undef  APPEND_NAME


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

UBOOL CheckDfnExitError_EM
    (LPPERTABDATA lpMemPTD)         // Ptr to PerTabData global memory

{
    LPSIS_HEADER   lpSISCur;            // Ptr to current SIS header
    LPDFN_HEADER   lpMemDfnHdr = NULL;  // Ptr to user-defined function/operator header
    UBOOL          bRet = FALSE;        // TRUE iff error on exit
    UINT           numResultSTE,        // # result STEs (may be zero if no result)
                   numRes;              // Loop counter
    LPSYMENTRY    *lplpSymEntry;        // Ptr to 1st result STE
    LPTOKEN_HEADER lptkHdr = NULL;      // Ptr to header of tokenized line
    LPTOKEN        lptkLine;            // Ptr to tokenized line

    // Get a ptr to the current SIS
    lpSISCur = lpMemPTD->lpSISCur;

    // If the current level is Immediate Execution Mode, Execute, or Quad Input
    //   back off until it isn't
    while (lpSISCur
        && (lpSISCur->DfnType EQ DFNTYPE_IMM
         || lpSISCur->DfnType EQ DFNTYPE_EXEC
         || lpSISCur->DfnType EQ DFNTYPE_QUAD))
        // Back off to the previous SI level
        lpSISCur = lpSISCur->lpSISPrv;

    // If we're back to the start, quit
    if (lpSISCur EQ NULL)
        goto NORMAL_EXIT;

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
                    // Initialize for ERROR_EXIT code
                    numRes = 0;

                    goto SYNTAX_EXIT;
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
                    goto VALUE_EXIT;
                else
                // If the name is not a variable, ...
                if (lplpSymEntry[numRes]->stFlags.stNameType NE NAMETYPE_VAR)
                    goto SYNTAX_EXIT;
            } // End FOR

            break;
    } // End SWITCH

    // Mark as not error on exit
    bRet = FALSE;

    goto NORMAL_EXIT;

SYNTAX_EXIT:
    // Set the error message
    ErrorMessageIndirect (ERRMSG_SYNTAX_ERROR APPEND_NAME);

    goto ERROR_EXIT;

VALUE_EXIT:
    // Set the error message
    ErrorMessageIndirect (ERRMSG_VALUE_ERROR APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
    // Set the error token
    ErrorMessageSetToken (&lptkLine[numRes]);

    // Mark as an error on line 0
    lpSISCur->CurLineNum = 0;

    // Mark as suspended
    lpSISCur->Suspended = TRUE;

    // Get a ptr to the current SIS
    lpSISCur = lpMemPTD->lpSISCur;

    // If the current level is Immediate Execution Mode, Execute, or Quad Input
    //   back off until it isn't
    while (lpSISCur
        && (lpSISCur->DfnType EQ DFNTYPE_IMM
         || lpSISCur->DfnType EQ DFNTYPE_EXEC
         || lpSISCur->DfnType EQ DFNTYPE_QUAD))
    {
        // Mark as unwinding
        lpSISCur->Unwind = TRUE;

        // Back off to the previous SI level
        lpSISCur = lpSISCur->lpSISPrv;
    } // End WHILE

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
//  $UnlocalizeSTEs
//
//  Unlocalize STEs on the innermost level
//    and strip off one level
//***************************************************************************

void UnlocalizeSTEs
    (void)

{
    HGLOBAL      hGlbData;          // STE global memory handle
    LPPERTABDATA lpMemPTD = NULL;   // Ptr to PerTabData global memory
    UINT         numSymEntries,     // # LPSYMENTRYs localized
                 numSym;            // Loop counter
    LPSYMENTRY   lpSymEntryNxt,     // Ptr to next localized LPSYMENTRY on the SIS
                 lpSymEntryCur;     // Ptr to SYMENTRY to release & unlocalize
    RESET_FLAGS  resetFlag;         // Reset flag (see RESET_FLAGS)
    LPFORSTMT    lpForStmtNext;     // Ptr to next entry on the FOR/FORLCL stmt stack
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS level

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // If the SI is non-empty, ...
    if (lpMemPTD->SILevel)
    {
        // Copy current SIS ptr
        lpSISCur = lpMemPTD->lpSISCur;

        Assert (lpSISCur NE NULL);
#ifdef DEBUG
        dprintfWL9 (L"~~Unlocalize:  %p to %p", lpSISCur, lpSISCur->lpSISPrv);
#endif
        // Save the reset flag
        resetFlag = lpSISCur->ResetFlag;

        // Free the outgoing value of []EM
        FreeResultGlobalVar (lpSISCur->hGlbQuadEM); lpMemPTD->lpSISCur->hGlbQuadEM = NULL;

        // Get # LPSYMENTRYs on the stack
        numSymEntries = lpSISCur->numSymEntries;

        // Point to the start of the localized LPSYMENTRYs
        lpSymEntryNxt = (LPSYMENTRY) ByteAddr (lpSISCur, sizeof (SIS_HEADER));

        // Point to the end of the localize LPSYMENTRYs
        lpSymEntryNxt = &lpSymEntryNxt[numSymEntries - 1];

        // Loop through the LPSYMENTRYs on the stack
        for (numSym = 0; numSym < numSymEntries; numSym++, lpSymEntryNxt--)
        // If the hash entry is valid, ...
        if (lpSymEntryNxt->stHshEntry)
        {
            // Get the ptr to the corresponding SYMENTRY
            lpSymEntryCur = lpSymEntryNxt->stHshEntry->htSymEntry;

            // Release the current value of the STE
            //   if it's not immediate and has a value
            if (!lpSymEntryCur->stFlags.Imm
             &&  lpSymEntryCur->stFlags.Value)
            {
                // Get the global memory handle
                hGlbData = lpSymEntryCur->stData.stGlbData;

                // Split cases based upon the nametype
                switch (lpSymEntryCur->stFlags.stNameType)
                {
                    case NAMETYPE_VAR:
                        // stData is a valid HGLOBAL variable array
                        Assert (IsGlbTypeVarDir_PTB (hGlbData));

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
                    case NAMETYPE_OP3:
                    case NAMETYPE_TRN:
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
            *lpSymEntryCur = *lpSymEntryNxt;
        } // End FOR/IF

        // Loop through the entries on the FOR/FORLCL stmt stack
        for (lpForStmtNext = lpSISCur->lpForStmtBase;
             lpForStmtNext && lpForStmtNext < lpSISCur->lpForStmtNext;
             lpForStmtNext++)
        {
            // We're done -- free the global in the token (if any)
            FreeResultTkn (&lpForStmtNext->tkForArr);

            // If this is :FORLCL, ...
            if (lpForStmtNext->symForI.stFlags.Inuse)
            {
                // Free the value of the :IN var
                FreeResultTkn (&lpForStmtNext->tkForI);

                // Restore the original :IN var
                *lpForStmtNext->tkForI.tkData.tkSym = lpForStmtNext->symForI;
            } // End IF
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
        // Zero the old entries
        ZeroMemory (lpMemPTD->lpSISNxt,
                    sizeof (*lpMemPTD->lpSISNxt)
                  + numSymEntries * sizeof (lpSymEntryNxt));
#endif
        // Back off the SI Level
        lpMemPTD->SILevel--;
    } // End IF
} // End UnlocalizeSTEs


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
                *lpSymEntryNxt = *lpSymEntrySrc;

                // Clear the STE flags & data
                *((UINT *) &lpSymEntrySrc->stFlags) &= *(UINT *) &stFlagsClr;
////////////////lpSymEntrySrc->stData.stLongest = 0;        // stLongest set below via stInteger

                // Initialize the SYMENTRY to an integer constant
                lpSymEntrySrc->stFlags.Imm        = TRUE;
                lpSymEntrySrc->stFlags.ImmType    = IMMTYPE_INT;
                lpSymEntrySrc->stFlags.Inuse      = TRUE;
                lpSymEntrySrc->stFlags.Value      = TRUE;
                lpSymEntrySrc->stFlags.ObjName    = (lpSymEntryNxt->stFlags.ObjName EQ OBJNAME_SYS)
                                                  ? OBJNAME_SYS
                                                  : OBJNAME_USR;
                lpSymEntrySrc->stFlags.stNameType = NAMETYPE_VAR;
                lpSymEntrySrc->stFlags.DfnLabel   = TRUE;
                lpSymEntrySrc->stData.stInteger   = uLineNum + 1;

                // Set the ptr to the previous entry to the STE in its shadow chain
                lpSymEntrySrc->stPrvEntry         = lpSymEntryNxt;

                // Save the SI level for this SYMENTRY
                Assert (lpMemPTD->SILevel);
                lpSymEntrySrc->stSILevel          = lpMemPTD->SILevel - 1;

                // Count in another label
                (*lpNumLabels)++;

                // Skip to the next SYMENTRY
                lpSymEntryNxt++;
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
                    *((UINT *) &(*lplpSymEntry)->stFlags) &= *(UINT *) &stFlagsClr;
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
            case TKT_CHRSTRAND:
            case TKT_NUMSTRAND:
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
                        *((UINT *) &(*lplpSymEntry)->stFlags) &= *(UINT *) &stFlagsClr;
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
        Assert (IsGlbTypeVarDir_PTB (hGlbArg));

        // If there's only one STE, it gets the whole arg
        if (numArgSTE EQ 1)
        {
            // Clear the STE flags & data
            *((UINT *) &(*lplpSymEntry)->stFlags) &= *(UINT *) &stFlagsClr;
////////////(*lplpSymEntry)->stData.stLongest = 0;              // stLongest set below

////////////(*lplpSymEntry)->stFlags.Imm        = FALSE;        // Already zero from previous initialization
////////////(*lplpSymEntry)->stFlags.ImmType    = IMMTYPE_ERROR;// Already zero from previous initialization
            (*lplpSymEntry)->stFlags.Value      = TRUE;
            (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
            (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
            (*lplpSymEntry)->stData.stGlbData   = CopySymGlbDir_PTB (hGlbArg);
        } else
        {
            APLSTYPE aplTypeArg;            // Arg storage type
            APLNELM  aplNELMArg;            // Arg NELM
            APLRANK  aplRankArg;            // Arg rank
            UINT     uBitIndex;             // Bit index for Booleans
            APLINT   apaOffArg,             // APA offset
                     apaMulArg;             // ... multiplier

            // Lock the memory to get a ptr to it
            lpMemArg = MyGlobalLock (ClrPtrTypeDir (hGlbArg));

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
                *((UINT *) &(*lplpSymEntry)->stFlags) &= *(UINT *) &stFlagsClr;
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
////////////////////////////////(*lplpSymEntry)->stFlags.Imm        = FALSE;        // Already zero from previous initialization
////////////////////////////////(*lplpSymEntry)->stFlags.ImmType    = IMMTYPE_ERROR;// Already zero from previous initialization
                                (*lplpSymEntry)->stFlags.Value      = TRUE;
                                (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
                                (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_VAR;
                                (*lplpSymEntry)->stData.stGlbData   = CopySymGlbInd_PTB (lpMemArg);

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
            MyGlobalUnlock (ClrPtrTypeDir (hGlbArg)); lpMemArg = NULL;
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

UBOOL InitFcnSTEs
    (LPPL_YYSTYPE lpYYArg,      // Ptr to arg PL_YYSTYPE
     UINT         numArgSTE,    // # STEs to initialize
     LPSYMENTRY  *lplpSymEntry) // Ptr to LPSYMENTRYs

{
    STFLAGS stFlagsClr = {0};   // Flags for clearing an STE
    UINT    TknCount;           // Token count

    // Set the Inuse flag
    stFlagsClr.Inuse = TRUE;

    // If the token is defined, ...
    if (lpYYArg && numArgSTE)
    {
        Assert (numArgSTE EQ 1);

        // Get the token count
        TknCount = lpYYArg->TknCount;

        // Split cases based upon the function count
        if (TknCount EQ 1)
        {
            // Clear the STE flags & data
            *((UINT *) &(*lplpSymEntry)->stFlags) &= *(UINT *) &stFlagsClr;

            // Set common values
            (*lplpSymEntry)->stFlags.Value      = TRUE;
            (*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;
            (*lplpSymEntry)->stFlags.stNameType = NAMETYPE_FN12;

            // If the function is immediate, ...
            if (lpYYArg->tkToken.tkFlags.TknType EQ TKT_FCNIMMED)
            {
                (*lplpSymEntry)->stFlags.Imm        = TRUE;
                (*lplpSymEntry)->stFlags.ImmType    = lpYYArg->tkToken.tkFlags.ImmType;
////////////////(*lplpSymEntry)->stFlags.Value      = TRUE;             // Set above
////////////////(*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;      // ...
////////////////(*lplpSymEntry)->stFlags.stNameType = NAMETYPE_FN12;    // ...
////////////////(*lplpSymEntry)->stFlags.UsrDfn     = FALSE;            // Already zero from above
                (*lplpSymEntry)->stData.stLongest   = lpYYArg->tkToken.tkData.tkLongest;
            } else
            {
////////////////(*lplpSymEntry)->stFlags.Imm        = FALSE;            // Already zero from above
////////////////(*lplpSymEntry)->stFlags.ImmType    = IMMTYPE_ERROR;    // Already zero from above
////////////////(*lplpSymEntry)->stFlags.Value      = TRUE;             // Set above
////////////////(*lplpSymEntry)->stFlags.ObjName    = OBJNAME_USR;      // ...
////////////////(*lplpSymEntry)->stFlags.stNameType = NAMETYPE_FN12;    // ...
                (*lplpSymEntry)->stFlags.UsrDfn     = (GetSignatureGlb_PTB (lpYYArg->tkToken.tkData.tkSym) EQ DFN_HEADER_SIGNATURE);
                (*lplpSymEntry)->stData.stGlbData   = CopySymGlbDir_PTB (lpYYArg->tkToken.tkData.tkGlbData);
            } // End IF/ELSE
        } else
        {
            APLUINT           ByteRes;          // # bytes in the result
            HGLOBAL           hGlbRes;          // Result global memory handle
            LPVOID            lpMemRes;         // Ptr to result global memory
            UINT              uFcn;             // Loop counter
            LPFCNARRAY_HEADER lpHeader;         // Ptr to function array header
#ifdef DEBUG
            // If the token is a global, ...
            if (!IsTknImmed (&lpYYArg->tkToken))
            // Split cases based upon the signature
            switch (GetSignatureGlb_PTB (lpYYArg->tkToken.tkData.tkSym))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                case DFN_HEADER_SIGNATURE:
                    break;

                defstop
                    break;
            } // End IF/SWITCH
#endif
            // Calculate space needed for the result
            ByteRes = CalcFcnSize (TknCount);

            // Check for overflow
            if (ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            // Allocate global memory for the function array
            hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (!hGlbRes)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemRes = MyGlobalLock (hGlbRes);

            // Save a ptr to the function array header
            lpHeader = (LPFCNARRAY_HEADER) lpMemRes;

            // Fill in the header
            lpHeader->Sig.nature  = FCNARRAY_HEADER_SIGNATURE;
            lpHeader->fnNameType  = NAMETYPE_FN12;
            lpHeader->RefCnt      = 1;
            lpHeader->tknNELM     = TknCount;

            // Skip over the header to the data (PL_YYSTYPEs)
            lpMemRes = FcnArrayBaseToData (lpMemRes);

            // Copy the PL_YYSTYPEs to the global memory object
            // Test cases:  +/rank[2] a     (as defined operator)
            //              +/{rank}[2] a   (as primitive operator)
            CopyMemory (lpMemRes, lpYYArg, TknCount * sizeof (PL_YYSTYPE));

            // Make a function array text line
            MakeTxtLine (lpHeader);

            // Loop through the functions incrementing the RefCnt as appropriate
            for (uFcn = 0; uFcn < TknCount; uFcn++, lpYYArg++)
            // Split cases based upon the token type
            switch (lpYYArg->tkToken.tkFlags.TknType)
            {
                case TKT_FCNARRAY:
                case TKT_VARARRAY:
                case TKT_NUMSTRAND:
                case TKT_CHRSTRAND:
                case TKT_AXISARRAY:
                    // Increment the RefCnt
                    DbgIncrRefCntDir_PTB (lpYYArg->tkToken.tkData.tkGlbData);

                    break;

                case TKT_AXISIMMED:
                case TKT_VARIMMED:
                case TKT_FCNIMMED:
                case TKT_OP1IMMED:
                case TKT_OP2IMMED:
                case TKT_OP3IMMED:
                case TKT_OPJOTDOT:
                case TKT_FILLJOT:
                    break;

                case TKT_VARNAMED:
                case TKT_FCNNAMED:
                case TKT_OP1NAMED:
                case TKT_OP2NAMED:
                case TKT_OP3NAMED:
                    // If it's not an immediate, ...
                    if (!lpYYArg->tkToken.tkData.tkSym->stFlags.Imm)
                        // Increment the RefCnt
                        DbgIncrRefCntDir_PTB (lpYYArg->tkToken.tkData.tkSym->stData.stGlbData);

                    break;

                defstop
                    break;
            } // End FOR/SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

            // Clear the STE flags & data
            *((UINT *) &(*lplpSymEntry)->stFlags) &= *(UINT *) &stFlagsClr;
////////////(*lplpSymEntry)->stFlags.Imm        = FALSE;        // Already zero from above
////////////(*lplpSymEntry)->stFlags.ImmType    = IMMTYPE_ERROR;// Already zero from above
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
        *lpSymEntryNxt = **lplpSymEntrySrc;

        // If the SYMENTRY is a system var, ...
        if (lpSymEntryNxt->stFlags.ObjName EQ OBJNAME_SYS)
        {
            // If the system var is a global, ...
            if (!lpSymEntryNxt->stFlags.Imm)
                // Increment the reference count in global memory
                //   as we retain the value over localization
                DbgIncrRefCntDir_PTB (lpSymEntryNxt->stData.stGlbData);
        } else
            // Erase the Symbol Table Entry
            //   unless it's a []var
            EraseSTE (*lplpSymEntrySrc, FALSE);
        // Set the ptr to the previous entry to the STE in the shadow chain
        (*lplpSymEntrySrc)->stPrvEntry = lpSymEntryNxt;

        // Save the SI level for this SYMENTRY
        Assert (lpMemPTD->SILevel);
        (*lplpSymEntrySrc)->stSILevel = lpMemPTD->SILevel - 1;

        // Skip to next entries
        lplpSymEntrySrc++;
          lpSymEntryNxt++;
    } // End FOR

    return lpSymEntryNxt;
} // End LocalizeSymEntries


//***************************************************************************
//  End of File: execdfn.c
//***************************************************************************
