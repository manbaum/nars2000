//***************************************************************************
//  NARS2000 -- System Function -- Quad EA
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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
//  $SysFnEA_EM_YY
//
//  System function:  []EA -- Execute Alternate
//***************************************************************************

LPPL_YYSTYPE SysFnEA_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token  (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonEA_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydEA_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnEA_EM_YY


//***************************************************************************
//  $SysFnMonEA_EM_YY
//
//  Monadic []EA -- error
//***************************************************************************

LPPL_YYSTYPE SysFnMonEA_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonEA_EM_YY


//***************************************************************************
//  $SysFnDydEA_EM_YY
//
//  Dyadic []EA -- Execute Alternate
//***************************************************************************

LPPL_YYSTYPE SysFnDydEA_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht;           // right ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht;           // right ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht;           // Right ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL;       // Right ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg header
                      lpMemHdrRht = NULL;   // ...    right ...
    LPAPLCHAR         lpMemLft,             // Ptr to left arg global memory
                      lpMemRht;             // ...    right ...
    APLLONGEST        aplLongestLft,        // Left arg immediate value
                      aplLongestRht;        // Right ...
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to result
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Create an SIS layer to stop unwinding through this level
    // Fill in the SIS header for Quad-EA
    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                NULL,                   // Semaphore handle
                DFNTYPE_ERRCTRL,        // DfnType
                FCNVALENCE_MON,         // FcnValence
                FALSE,                  // Suspended
                TRUE,                   // Restartable
                TRUE);                  // LinkIntoChain
    // Fill in the non-default SIS header entries
    lpMemPTD->lpSISCur->bItsEC = TRUE;  // So we don't suspend our caller

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT & RIGHT RANK ERRORs
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for LEFT & RIGHT DOMAIN ERRORs
    if (!IsSimpleChar (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;
    if (!IsSimpleChar (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get the right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // If it's global, ...
    if (hGlbRht NE NULL)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
     else
        // Point to the immediate value
        lpMemRht = (LPAPLCHAR) &aplLongestRht;

    // Execute the right arg
    lpYYRes =
      PrimFnMonUpTackJotCommon_EM_YY (lpMemRht,     // Ptr to text of line to execute
                                      aplNELMRht,   // Length of the line to execute
                                      FALSE,        // TRUE iff we should free lpwszCompLine
                                      TRUE,         // TRUE iff we should return a NoValue YYRes
                                      FALSE,        // TRUE iff we should act on errors
                                      NULL,         // Ptr to return EXITTYPE_xxx (may be NULL)
                                      lptkFunc);    // Ptr to function token
    // If it succeeded, ...
    if (lpYYRes NE NULL)
        goto NORMAL_EXIT;

    // There was an error, so execute the left arg

    // Get the left arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);

    // If it's global, ...
    if (hGlbLft NE NULL)
        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);
    else
        // Point to the immediate value
        lpMemLft = (LPAPLCHAR) &aplLongestLft;

    // Execute the left arg
    lpYYRes =
      PrimFnMonUpTackJotCommon_EM_YY (lpMemLft,     // Ptr to text of line to execute
                                      aplNELMLft,   // Length of the line to execute
                                      FALSE,        // TRUE iff we should free lpwszCompLine
                                      TRUE,         // TRUE iff we should return a NoValue YYRes
                                      TRUE,         // TRUE iff we should act on errors
                                      NULL,         // Ptr to return EXITTYPE_xxx (may be NULL)
                                      lptkFunc);    // Ptr to function token
    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbRht NE NULL && lpMemHdrRht NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    if (hGlbLft NE NULL && lpMemHdrLft NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    // Unlocalize the STEs on the innermost level
    //   and strip off one level
    UnlocalizeSTEs (NULL);

    return lpYYRes;
} // End SysFnDydEA_EM_YY


//***************************************************************************
//  End of File: qf_ea.c
//***************************************************************************
