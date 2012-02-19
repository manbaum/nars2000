//***************************************************************************
//  NARS2000 -- Primitive Function -- Iota Underbar
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2012 Sudley Place Software

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
//  $PrimFnIotaUnderbar_EM_YY
//
//  Primitive function for monadic and dyadic IotaUnderbar ("indices" and ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnIotaUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnIotaUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_IOTAUNDERBAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonIotaUnderbar_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydIotaUnderbar_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnIotaUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnIotaUnderbar_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic IotaUnderbar
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnIotaUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnIotaUnderbar_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return
      PrimProtoFnMixed_EM_YY (&PrimFnIotaUnderbar_EM_YY,// Ptr to primitive function routine
                               lptkLftArg,              // Ptr to left arg token
                               lptkFunc,                // Ptr to function token
                               lptkRhtArg,              // Ptr to right arg token
                               lptkAxis);               // Ptr to axis token (may be NULL)
} // End PrimProtoFnIotaUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonIotaUnderbar_EM_YY
//
//  Primitive function for monadic IotaUnderbar ("indices")
//
//  This monadic function implements R/{iota}>R
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonIotaUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonIotaUnderbar_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeRht;       // Right arg storage type
    APLNELM       aplNELMRht,       // Right arg NELM
                  aplNELMRes;       // Result    ...
    APLRANK       aplRankRht;       // Right arg rank
    APLINT        aplIntegerRht,    // Right arg temp integer value
                  aplIntegerRep,    // Repetition integer
                  apaOff,           // APA offset
                  apaMul;           // ... multiplier
    APLUINT       ByteRes,          // # bytes in the result
                  uLen,             // Length of repetition
                  uRht,             // ...
                  uRep;             // ...
    UBOOL         bRet;             // TRUE iff the result is valid
    UINT          uBitMask;         // Boolean bit mask
    HGLOBAL       hGlbRht = NULL,   // Right arg global memory handle
                  hGlbRes = NULL,   // Result    ...
                  hGlbRep = NULL;   // Right arg temp ...
    LPVOID        lpMemRht = NULL;  // Ptr to right arg global memory
    LPAPLINT      lpMemRes = NULL,  // Ptr to result    ...
                  lpMemRep = NULL,  // ...    right arg temp ...
                  lpMemIniRep;      // ...    ...
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    APLBOOL       bQuadIO;          // []IO

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check the rank
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for DOMAIN ERROR
    if (!IsSimpleNum (aplTypeRht))
        goto DOMAIN_EXIT;

    // Get right arg's global ptr
    aplIntegerRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is empty, ...
    if (IsEmpty (aplNELMRht))
        // We now have the result NELM
        apaOff = aplNELMRes = 0;
    else
    // If the right arg is singleton, ...
    if (IsSingleton (aplNELMRht))
    {
        // Attempt to convert FLOAT right arg
        if (IsSimpleFlt (aplTypeRht))
        {
            // Attempt to convert the float to an integer using System CT
            aplIntegerRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplIntegerRht, &bRet);
            if (!bRet)
                goto DOMAIN_EXIT;
        } // End IF

        // Check the singleton arg
        if (aplIntegerRht < 0)
            goto DOMAIN_EXIT;

        // Point to the value
        lpMemRep = lpMemRht = &aplIntegerRht;

        // We now have the result NELM
        apaOff = aplNELMRes = aplIntegerRht;
    } else
    // The right arg has two or more elements
    {
        // If the right arg is float, copy it to
        //   temp storage (all APLINTs),
        //   check the right arg for valid values, and
        //   accumulate the sum (as aplNELMRes).

        // Skip over the header to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

        // If the right arg is a float, ...
        if (IsSimpleFlt (aplTypeRht))
        {
            // Calculate space needed for the normalized right arg
            ByteRes = _imul64 (aplNELMRht , sizeof (APLINT), &bRet);

            // Check for overflow
            if (!bRet || ByteRes NE (APLU3264) ByteRes)
                goto WSFULL_EXIT;

            // Allocate temp storage for the normalized right arg
            hGlbRep = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (!hGlbRep)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemRep = MyGlobalLock (hGlbRep);
        } // End IF

        // Initialize the result NELM accumulator
        aplNELMRes = 0;

        // Split cases based upon the right arg's storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                // Count the # bits in the right arg
                FastBoolRedPlus (ARRAY_BOOL,        // Right arg storage type
                                 aplNELMRht,        // Right arg NELM
                                 lpMemRht,          // Ptr to right arg memory
                                &aplNELMRes,        // Ptr to result    memory
                                 1,                 // Product of dimensions below axis
                                 aplNELMRht,        // Length of right arg axis dimension
                                 PF_INDEX_PLUS,     // FBFN_INDS value (e.g., index into FastBoolFns[])
                                 NULL);             // Ptr to operator function strand
                break;

            case ARRAY_INT:
                // Save the right arg ptr
                lpMemRep = lpMemRht;

                // Loop through the right arg validating the data
                //   and copying it to the repetition vector
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Get the next value
                    aplIntegerRht = *((LPAPLINT) lpMemRht)++;

                    // Validate it
                    if (aplIntegerRht < 0)
                        goto DOMAIN_EXIT;

                    // Accumulate in the result NELM
                    aplNELMRes += aplIntegerRht;
                } // End IF

                break;

            case ARRAY_FLOAT:
                // Save the initial repetition vector ptr
                lpMemIniRep = lpMemRep;

                // Loop through the right arg validating the data
                //   and copying it to the repetition vector
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                {
                    // Attempt to convert the float to an integer using System CT
                    aplIntegerRht = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemRht)++, &bRet);

                    // Validate it
                    if (!bRet || aplIntegerRht < 0)
                        goto DOMAIN_EXIT;

                    // Accumulate in the result NELM
                    aplNELMRes += aplIntegerRht;

                    // Save in the repetition vector
                    *lpMemRep++ = aplIntegerRht;
                } // End FOR

                // Restore lpMemRep to the start of the vector
                lpMemRep = lpMemIniRep;

                break;

            case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
                // Get the APA parameters
                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;
#undef  lpAPA
                __try
                {
                    // Check the first and last values to validate the entire APA
                    if ((apaOff < 0)
/////////////////////|| (apaOff + apaMul * ((APLINT) aplNELMRht - 1)) < 0)
                     || iadd64 (apaOff, imul64 (apaMul, isub64 (aplNELMRht, 1))) < 0)
                        goto DOMAIN_EXIT;

                    // Calculate the sum of the APA as
                    //  (apaOff * aplNELMRht) + (apaMul * aplNELMRht * (aplNELMRht - 1)) / 2
                    aplNELMRes = imul64 (apaOff,         aplNELMRht)
                               + imul64 (apaMul, imul64 (aplNELMRht,  aplNELMRht - 1) / 2);
                } __except (CheckException (GetExceptionInformation (), L"PrimFnMonIotaUnderbar_EM_YY"))
                {
                    dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);

                    // Split cases based upon the ExceptionCode
                    switch (MyGetExceptionCode ())
                    {
                        case EXCEPTION_RESULT_FLOAT:
                            MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                            goto DOMAIN_EXIT;

                        default:
                            // Display message for unhandled exception
                            DisplayException ();

                            break;
                    } // End SWITCH
                } // End __try/__except

                break;

            case ARRAY_CHAR:
            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

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
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Fill in the axis dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, 1);

    // If the right arg is Boolean, ...
    if (IsSimpleBool (aplTypeRht))
    {
        uBitMask = BIT0;

        // Loop through the right arg copying the data to the result
        for (aplIntegerRep = bQuadIO, uRht = 0;
             uRht < aplNELMRht;
             uRht++, aplIntegerRep++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // If it's a one, ...
            if (uBitMask & *(LPAPLBOOL) lpMemRht)
                // Save the value in the result
                *lpMemRes++ = aplIntegerRep;

            // Shift over the right bit mask
            uBitMask <<= 1;

            // Check for end-of-byte
            if (uBitMask EQ END_OF_BYTE)
            {
                uBitMask = BIT0;            // Start over
                ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
            } // End IF
        } // End FOR
    } else
    // Loop through the right arg copying the data to the result
    for (aplIntegerRep = bQuadIO, uRht = 0;
         uRht < aplNELMRht;
         uRht++, aplIntegerRep++)
    {
        // Get the next repetition value
        uLen = (IsSimpleAPA (aplTypeRht)) ? apaOff + apaMul * uRht : *lpMemRep++;

        // Loop through the repetition value
        for (uRep = 0; uRep < uLen; uRep++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Save the value in the result
            *lpMemRes++ = aplIntegerRep;
        } // End FOR
    } // End IF/ELSE/FOR

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRep)
    {
        if (lpMemRep)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRep); lpMemRep = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbRep); hGlbRep = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnMonIotaUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydIotaUnderbar_EM_YY
//
//  Primitive function for dyadic IotaUnderbar (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydIotaUnderbar_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydIotaUnderbar_EM_YY
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis)                      // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnDydIotaUnderbar_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_iotaund.c
//***************************************************************************
