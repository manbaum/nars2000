//***************************************************************************
//  NARS2000 -- System Function -- Quad ES
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
#include "externs.h"
#include "pertab.h"
#include "sis.h"
#include "eventtypes.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  $SysFnES_EM_YY
//
//  System function:  []ES -- Event Simulate
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnES_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnES_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

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

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonES_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydES_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End SysFnES_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonES_EM_YY
//
//  Monadic []ES -- Event Simulate w/ Message or Type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonES_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonES_EM_YY
    (LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    return SysFnDydES_EM_YY (NULL,          // Ptr to left arg token
                             lptkFunc,      // Ptr to function token
                             lptkRhtArg,    // Ptr to right arg token
                             lptkAxis);     // Ptr to axis token (may be NULL)
} // End SysFnMonES_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydES_EM_YY
//
//  Dyadic []ES -- Event Simulate w/ Message and Type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydES_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydES_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if called monadically)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht;        // Right ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht;        // Right ...
    APLRANK      aplRankLft,        // Left arg Rank
                 aplRankRht;        // Right ...
    APLLONGEST   aplLongestRht1,    // Right arg longest if immediate, 1st
                 aplLongestRht2;    // ...                             2nd
    HGLOBAL      hGlbRht = NULL;    // Right arg global memory handle
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    HGLOBAL      hGlbPTD;           // PerTabData global memory handle
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to result

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);
    if (lptkLftArg)
        AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Check for LEFT RANK ERROR
    if (lptkLftArg && IsMultiRank (aplRankLft))
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Check for RIGHT DOMAIN ERROR
    if ((!IsSimpleNH (aplTypeRht))
     || (lptkLftArg && IsSimpleChar (aplTypeRht)))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Check for LEFT DOMAIN ERROR
    if (lptkLftArg && !IsSimpleChar (aplTypeLft))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // If the right arg is empty, return NoValue
    if (IsEmpty (aplNELMRht))
        lpYYRes = MakeNoValue_YY (lptkFunc);
    else
    {
        // Get the thread's PerTabData global memory handle
        hGlbPTD = TlsGetValue (dwTlsPerTabData); Assert (hGlbPTD NE NULL);

        // Lock the memory to get a ptr to it
        lpMemPTD = MyGlobalLock (hGlbPTD);

        // Get right arg's global ptrs
        aplLongestRht1 = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

        // Split cases based upon the numeric vs. char storage type
        if (IsSimpleChar (aplTypeRht))
        {
            // If the message is a global, ...
            if (hGlbRht)
            {
                // Skip over the header and dimensions to the data
                lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

                // Copy the error message to temporary storage
                CopyMemory (lpMemPTD->lpwszQuadErrorMsg, lpMemRht, (UINT) aplNELMRht * sizeof (APLCHAR));
            } else
                lpMemPTD->lpwszQuadErrorMsg[0] = (APLCHAR) aplLongestRht1;

            // Ensure properly terminated
            lpMemPTD->lpwszQuadErrorMsg[aplNELMRht] = L'\0';
        } else
        {
            UBOOL       bRet;               // TRUE iff result is valid
            EVENT_TYPES EventType;          // Event type

            // Check for RIGHT LENGTH ERROR
            if (aplNELMRht NE 2)
            {
                ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

            // Get the first value
            GetNextValueMem (lpMemRht,
                             aplTypeRht,
                             0,
                             NULL,
                            &aplLongestRht1,
                             NULL);
            // Get the second value
            GetNextValueMem (lpMemRht,
                             aplTypeRht,
                             1,
                             NULL,
                            &aplLongestRht2,
                             NULL);
            // If the right arg is float, attempt to convert it to integer
            if (IsSimpleFlt (aplTypeRht))
            {
                // Attempt to convert the float to an integer using System CT
                aplLongestRht1 = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht1, &bRet);
                if (bRet)
                    // Attempt to convert the float to an integer using System CT
                    aplLongestRht2 = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht2, &bRet);
                if (!bRet)
                {
                    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                               lptkFunc);
                    goto ERROR_EXIT;
                } // End IF
            } // End IF

            // Check for RIGHT DOMAIN ERROR
            if (aplLongestRht1 >= (BIT0 << 16)
             || aplLongestRht2 >= (BIT0 << 16))
            {
                ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                           lptkFunc);
                goto ERROR_EXIT;
            } // End IF

            // Combine the two elements into the Event Type
            EventType = MAKE_ET (aplLongestRht1, aplLongestRht2);

            // Save the event type
            SetEventTypeMessage (EventType, NULL, NULL);

            // If there's a left arg, ...
            if (lptkLftArg)
            {
                HGLOBAL    hGlbLft;             // Left arg global memory handle
                LPAPLCHAR  lpMemLft;            // Ptr to left arg global memory
                APLLONGEST aplLongestLft;       // Left arg immediate value

                // Get left arg's global ptrs
                aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);

                // If the message is a global, ...
                if (hGlbLft)
                {
                    // Skip over the header and dimensions to the data
                    lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

                    // Copy the error message to temporary storage
                    CopyMemory (lpMemPTD->lpwszQuadErrorMsg, lpMemLft, (UINT) aplNELMLft * sizeof (APLCHAR));

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
                } else
                    lpMemPTD->lpwszQuadErrorMsg[0] = (APLCHAR) aplLongestLft;

                // Ensure properly terminated
                lpMemPTD->lpwszQuadErrorMsg[aplNELMLft] = L'\0';
            } else
            {
                LPAPLCHAR lpwErrMsg;

                // Otherwise, use the default error text for the Event Type

                // Split cases based upon the Event Type
                switch (EventType)
                {
                    case EVENTTYPE_NOERROR:
                        lpwErrMsg = L"";

                        break;

                    case EVENTTYPE_UNK:
                        lpwErrMsg = L"UNKNOWN ERROR";

                        break;

                    case EVENTTYPE_BREAK:
                        lpwErrMsg = L"INTERRUPT";

                        break;

                    case EVENTTYPE_SYSTEM_ERROR:
                        lpwErrMsg = L"SYSTEM ERROR";

                        break;

                    case EVENTTYPE_WS_FULL:
                        lpwErrMsg = ERRMSG_WS_FULL;

                        break;

                    case EVENTTYPE_LIMIT_ST:
                    case EVENTTYPE_LIMIT_RANK:
                    case EVENTTYPE_LIMIT_IMPL:
                        lpwErrMsg = L"SYSTEM LIMIT";

                        break;

                    case EVENTTYPE_SYNTAX_ERROR_ARGOMITTED:
                    case EVENTTYPE_SYNTAX_ERROR_ILLFORMED:
                    case EVENTTYPE_SYNTAX_ERROR_NAMECLASS:
                    case EVENTTYPE_SYNTAX_ERROR_CONTEXT:
                        lpwErrMsg = ERRMSG_SYNTAX_ERROR;

                        break;

                    case EVENTTYPE_VALUE_ERROR_NAME:
                    case EVENTTYPE_VALUE_ERROR_FCN:
                        lpwErrMsg = ERRMSG_VALUE_ERROR;

                        break;

                    case EVENTTYPE_VALENCE_ERROR:
                        lpwErrMsg = ERRMSG_VALENCE_ERROR;

                        break;

                    case EVENTTYPE_RANK_ERROR:
                        lpwErrMsg = ERRMSG_RANK_ERROR;

                        break;

                    case EVENTTYPE_LENGTH_ERROR:
                        lpwErrMsg = ERRMSG_LENGTH_ERROR;

                        break;

                    case EVENTTYPE_DOMAIN_ERROR:
                        lpwErrMsg = ERRMSG_DOMAIN_ERROR;

                        break;

                    case EVENTTYPE_INDEX_ERROR:
                        lpwErrMsg = ERRMSG_INDEX_ERROR;

                        break;

                    case EVENTTYPE_AXIS_ERROR:
                        lpwErrMsg = ERRMSG_AXIS_ERROR;

                        break;

                    default:
                        lpwErrMsg = L"";

                        break;
                } // End SWITCH

                // Copy the error message to temporary storage
                CopyMemory (lpMemPTD->lpwszQuadErrorMsg, lpwErrMsg, (lstrlenW (lpwErrMsg) + 1) * sizeof (APLCHAR));
            } // End IF/ELSE
        } // End IF/ELSE

        // Save in PTD -- note that the tkCharIndex in the
        //   function token passed here isn't used unless this is
        //   immediate execution mode; normally, the tkCharIndex of the
        //   caller's is used.
        ErrorMessageIndirectToken (lpMemPTD->lpwszQuadErrorMsg, lptkFunc);
        lpMemPTD->tkErrorCharIndex = lptkFunc->tkCharIndex;

        // Set the reset flag
        lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_QUADERROR_INIT;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbPTD); lpMemPTD = NULL;
    } // End IF/ELSE
ERROR_EXIT:
    // We no longer need this ptr
    if (hGlbRht && lpMemRht)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydES_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: qf_es.c
//***************************************************************************
