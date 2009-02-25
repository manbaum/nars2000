//***************************************************************************
//  NARS2000 -- Goto Functions
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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
#include <limits.h>
#include "headers.h"


//***************************************************************************
//  $GotoLine_EM
//
//  Handle {goto} LineNum
//
//  Return EXITTYPE_GOTO_ZILDE if we're going to {zilde}
//         EXITTYPE_GOTO_LINE  if we're going to a valid line num
//         EXITTYPE_ERROR if error
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- GotoLine_EM"
#else
#define APPEND_NAME
#endif

EXIT_TYPES GotoLine_EM
    (LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkFunc)              // Ptr to function token

{
    HGLOBAL        hGlbPTD,         // PerTabData global memory handle
                   hGlbTknLine;     // Tokenized line global memory handle
    LPPERTABDATA   lpMemPTD;        // Ptr to PerTabData global memory
    EXIT_TYPES     exitType;        // Return value
    APLSTYPE       aplTypeRht;      // Right arg storage type
    APLNELM        aplNELMRht;      // ...       NELM
    APLRANK        aplRankRht;      // ...       rank
    IMM_TYPES      immType;         // Right arg first value immediate type
    APLINT         aplIntegerRht;   // First value as integer
    APLFLOAT       aplFloatRht;     // ...            float
    LPSIS_HEADER   lpSISCur;        // Ptr to current SIS header
    TOKEN_TYPES    TknType;         // Target token type
    LPTOKEN_HEADER lpMemTknHdr;     // Ptr to tokenized line header
    LPTOKEN        lpMemTknLine;    // Ptr to tokenized line global memory
    LPPLLOCALVARS  lpplLocalVars;   // Ptr to PL local vars
    TOKEN          tkNxt;           // Token of next stmt
    UINT           uTknNum = 0;     // Starting token #

    // Get the thread's PerTabData global memory handle
    hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

    // Lock the memory to get a ptr to it
    lpMemPTD = MyGlobalLock (hGlbPTD);

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Get the first value
    GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                       &aplIntegerRht,  // Ptr to integer result
                       &aplFloatRht,    // Ptr to float ...
                        NULL,           // Ptr to WCHAR ...
                        NULL,           // Ptr to longest ...
                        NULL,           // Ptr to lpSym/Glb ...
                       &immType,        // Ptr to ...immediate type ...
                        NULL);          // Ptr to array type ...
    if (immType EQ IMMTYPE_ERROR
     || IsImmChr (immType))
        goto DOMAIN_EXIT;

    if (IsImmFlt (immType))
    {
        UBOOL bRet;

        // Attempt to convert the float to an integer using System CT
        aplIntegerRht = FloatToAplint_SCT (aplFloatRht, &bRet);
        if (!bRet)
            goto DOMAIN_EXIT;
    } // End IF

    // If the right arg is empty, ...
    if (IsEmpty (aplNELMRht))
    {
        exitType = EXITTYPE_GOTO_ZILDE;

        goto NORMAL_EXIT;
    } // End IF

    // Copy ptr to current SI level
    lpSISCur = lpMemPTD->lpSISCur;

    // Check for not restartable SI level
    if (lpSISCur->DfnType EQ DFNTYPE_IMM    // This level is Immediate
     && lpSISCur->lpSISPrv                  // There is a previous level
     && !lpSISCur->lpSISPrv->Restartable)   // and it's not restartable
        goto NORESTART_EXIT;

    // Ensure line # is within range
    if (aplIntegerRht < 0
     || aplIntegerRht > UINT_MAX)
        aplIntegerRht = -1;

    // Peel back to non-ImmExec, non-Execute, non-Quad layer
    while (lpSISCur
        && (lpSISCur->DfnType EQ DFNTYPE_IMM
         || lpSISCur->DfnType EQ DFNTYPE_QUAD
         || lpSISCur->DfnType EQ DFNTYPE_EXEC))
        lpSISCur = lpSISCur->lpSISPrv;

    if (lpSISCur EQ NULL)
        hGlbTknLine = NULL;
    else
        // Get the the corresponding tokenized line's global memory handle
        hGlbTknLine = GetTokenLineHandle (lpSISCur->hGlbDfnHdr, aplIntegerRht);
    if (hGlbTknLine)
    {
        UINT uStmtLen,
             TokenCnt;

        // Lock the memory to get a ptr to it
        lpMemTknHdr = MyGlobalLock (hGlbTknLine);

        // Get the token count
        TokenCnt = lpMemTknHdr->TokenCnt;

        // Skip over the header to the data
        lpMemTknLine = TokenBaseToStart (lpMemTknHdr);

        Assert (lpMemTknLine->tkFlags.TknType EQ TKT_EOS
             || lpMemTknLine->tkFlags.TknType EQ TKT_EOL);
        // Get the EOS/EOL token length and skip over it
        uStmtLen = lpMemTknLine->tkData.tkIndex;
        lpMemTknLine++; TokenCnt--;

        // Check for and skip over line label
        if (TokenCnt > 1
         && lpMemTknLine[1].tkFlags.TknType EQ TKT_LABELSEP)
        {
            lpMemTknLine += 2; TokenCnt -= 2;
        } // End IF

        // Get the next token and its type
        tkNxt = *lpMemTknLine;
        TknType = tkNxt.tkFlags.TknType;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbTknLine); lpMemTknLine = NULL;

        // Check for branch to non-restartable Control Structure tokens:
        //   CASE, CASELIST (both covered by SKIPCASE)
        if (TknType EQ TKT_CS_SKIPCASE)
            goto DESTIN_EXIT;

        // Get a ptr to the PL local vars
        lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

        // If the target is an ELSE stmt, skip to the next stmt
        //   and execute that one
        if (TknType EQ TKT_CS_ELSE)
            // Skip over the ELSE stmt
            uTknNum = uStmtLen;
        else
        // If the target is a FOR/FORLCL stmt, ensure that the identifier
        //  on the FORSTMT stack is the same as the one in the EndFor
        //  token.
        if (TknType EQ TKT_CS_ENDFOR
         || TknType EQ TKT_CS_ENDFORLCL)
        {
            // If there's no previous entry on the FORSTMT stack, ...
            if (lpSISCur EQ NULL
             || lpSISCur->lpForStmtBase EQ lpSISCur->lpForStmtNext)
                goto DESTIN_EXIT;

            // Compare the current FOR/FORLCL stmt identifier from the FORSTMT stack
            //   with the CLIndex in the ENDFOR/ENDFORLCL stmt -- if they are unequal,
            //   then this ENDFOR/ENDFORLCL stmt is not restartable (we're starting in
            //   the middle of a FOR/FORLCL stmt which has not been initialized
            //   or is out of nested sequence).
            if (lpSISCur->lpForStmtNext[-1].uForStmtID
             NE tkNxt.tkData.uCLIndex)
                goto DESTIN_EXIT;
        } // End IF/ELSE/IF
    } // End IF

    // Save the exit type
    exitType = EXITTYPE_GOTO_LINE;

    // Split cases based upon the function type
    switch (lpMemPTD->lpSISCur->DfnType)
    {
        case DFNTYPE_IMM:       // Restart execution in a suspended function
            // If there's a suspended function, ...
            if (lpMemPTD->lpSISCur->lpSISPrv)
            {
                // Save as the next line & token #s
                lpMemPTD->lpSISCur->lpSISPrv->NxtLineNum = (UINT) aplIntegerRht;
                lpMemPTD->lpSISCur->lpSISPrv->NxtTknNum  = uTknNum;

                // Mark as no longer suspended
                lpMemPTD->lpSISCur->lpSISPrv->Suspended = FALSE;
            } // End IF

            break;

        case DFNTYPE_OP1:
        case DFNTYPE_OP2:
        case DFNTYPE_FCN:       // Jump to a new line #
            // If there's a suspended function, ...
            if (lpMemPTD->lpSISCur)
            {
                // Save as the next line & token #s
                lpMemPTD->lpSISCur->NxtLineNum = (UINT) aplIntegerRht;
                lpMemPTD->lpSISCur->NxtTknNum  = uTknNum;

                // Mark as no longer suspended
                lpMemPTD->lpSISCur->Suspended = FALSE;
            } // End IF

            break;

        case DFNTYPE_EXEC:
        case DFNTYPE_QUAD:
            // Peel back to the first non-Imm/Exec layer
            //   starting with the previous SIS header
            lpSISCur = GetSISLayer (lpMemPTD->lpSISCur->lpSISPrv);

            // If there's a suspended user-defined function/operator, ...
            if (lpSISCur
             && (lpSISCur->DfnType EQ DFNTYPE_OP1
              || lpSISCur->DfnType EQ DFNTYPE_OP2
              || lpSISCur->DfnType EQ DFNTYPE_FCN))
            {
                // Save as the next line & token #s
                lpSISCur->NxtLineNum = (UINT) aplIntegerRht;
                lpSISCur->NxtTknNum  = uTknNum;

                // Mark as no longer suspended
                lpSISCur->Suspended = FALSE;

                // Save the suspended function's semaphore as the one to signal
                lpMemPTD->lpSISCur->hSigaphore = lpSISCur->hSemaphore;
            } // End IF

            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NORESTART_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NOTRESTARTABLE APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DESTIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DESTINATION_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    exitType = EXITTYPE_ERROR;
NORMAL_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;

    return exitType;
} // End GotoLine_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: goto.c
//***************************************************************************
