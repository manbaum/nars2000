//***************************************************************************
//  NARS2000 -- System Function -- Quad UCS
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
#include <limits.h>
#include "headers.h"


//***************************************************************************
//  $SysFnUCS_EM_YY
//
//  System function:  []UCS -- Universal Character Set
//***************************************************************************

LPPL_YYSTYPE SysFnUCS_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token (should be NULL)
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
        return SysFnMonUCS_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydUCS_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnUCS_EM_YY


//***************************************************************************
//  $SysFnMonUCS_EM_YY
//
//  Monadic []UCS -- Universal Character Set
//***************************************************************************

LPPL_YYSTYPE SysFnMonUCS_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE       aplTypeRht,          // Right arg storage type
                   aplTypeRes,          // Result    ...
                   aplTypeHet;          // Hetero item ...
    APLNELM        aplNELMRht;          // Right arg NELM
    APLRANK        aplRankRht;          // Right arg rank
    HGLOBAL        hGlbRht = NULL,      // Right arg global memory handle
                   hGlbRes = NULL;      // Result    ...
    APLLONGEST     aplLongestRht;       // Right arg longest if immediate
    LPVOID         lpMemRht = NULL,     // Ptr to right arg global memory
                   lpMemRes = NULL;     // Ptr to result    ...
    LPPL_YYSTYPE   lpYYRes = NULL;      // Ptr to the result
    UBOOL          bRet;                // TRUE iff result if valid
    IMM_TYPES      immTypeRes;          // Result immediate type
    APLINT         apaOffRht,           // Right arg APA offset
                   apaMulRht;           // ...           multiplier
    APLUINT        ByteRes,             // # bytes in the result
                   uRht;                // Loop counter
    UINT           uBitMask;            // Bit mask for marching through Booleans
    APLHETERO      aplHeteroRht,        // Right arg value as APLHETERO
                   lpMemHeteroRht;      // Ptr to hetero item
    LPSYMENTRY     lpSymTmp;            // Ptr to temporary LPSYMENTRY

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for DOMAIN ERROR
    if (!IsSimpleGlbNum (aplTypeRht))
        goto DOMAIN_EXIT;

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg/result is a scalar, ...
    if (IsScalar (aplRankRht))
    {
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_FLOAT:
                // Attempt to convert the float to an integer using System []CT
                aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;

                // Fall through to common code

            case ARRAY_BOOL:
            case ARRAY_INT:
                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
////////////////////aplLongestRht = UTF16_REPLACEMENTCHAR;
                    goto NONCE_EXIT;

                // Set the result immediate type
                immTypeRes = IMMTYPE_CHAR;

                break;

            case ARRAY_CHAR:
                // Set the result immediate type
                immTypeRes = IMMTYPE_INT;

                break;

            case ARRAY_RAT:
                aplLongestRht = mpq_get_sctsx ((LPAPLRAT) VarArrayDataFmBase (lpMemRht), &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
////////////////////aplLongestRht = UTF16_REPLACEMENTCHAR;
                    goto NONCE_EXIT;

                // Set the result immediate type
                immTypeRes = IMMTYPE_CHAR;

                break;

            case ARRAY_VFP:
                aplLongestRht = mpfr_get_sctsx ((LPAPLVFP) VarArrayDataFmBase (lpMemRht), &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;
                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
////////////////////aplLongestRht = UTF16_REPLACEMENTCHAR;
                    goto NONCE_EXIT;

                // Set the result immediate type
                immTypeRes = IMMTYPE_CHAR;

                break;

            defstop
                break;
        } // End SWITCH

        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = immTypeRes;
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto NORMAL_EXIT;
    } // End IF

    // From here on, the result is a global memory handle

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
        case ARRAY_RAT:
        case ARRAY_VFP:
            aplTypeRes = ARRAY_CHAR;

            break;

        case ARRAY_CHAR:
            aplTypeRes = ARRAY_INT;

            break;

        case ARRAY_HETERO:
            aplTypeRes = ARRAY_HETERO;

            break;

        defstop
            break;
    } // End SWITCH

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Copy the dimensions to the result
    CopyMemory (VarArrayBaseToDim (lpMemRes),
                VarArrayBaseToDim (lpMemRht),
                (APLU3264) aplRankRht * sizeof (APLDIM));
    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemRht);
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            uBitMask = BIT0;

            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                *((LPAPLCHAR) lpMemRes)++ =
                  (APLCHAR) ((uBitMask & *(LPAPLBOOL) lpMemRht) ? TRUE : FALSE);

                // Shift over the right arg bit mask
                uBitMask <<= 1;

                // Check for end-of-byte
                if (uBitMask EQ END_OF_BYTE)
                {
                    uBitMask = BIT0;            // Start over
                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Get the next element
                aplLongestRht = *((LPAPLUINT) lpMemRht)++;

                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
////////////////////*((LPAPLCHAR) lpMemRes)++ = UTF16_REPLACEMENTCHAR;
                    goto NONCE_EXIT;
                else
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
            } // End IF

            break;

        case ARRAY_FLOAT:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Attempt to convert the float to an integer using System []CT
                aplLongestRht = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemRht)++, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;

                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
////////////////////*((LPAPLCHAR) lpMemRes)++ = UTF16_REPLACEMENTCHAR;
                    goto NONCE_EXIT;
                else
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
            } // End FOR

            break;

        case ARRAY_CHAR:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
                *((LPAPLINT) lpMemRes)++ = *((LPAPLCHAR) lpMemRht)++;
            break;

        case ARRAY_APA:
#define lpAPA           ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Get the next element
                aplLongestRht = apaOffRht + apaMulRht * uRht;

                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
////////////////////*((LPAPLCHAR) lpMemRes)++ = UTF16_REPLACEMENTCHAR;
                    goto NONCE_EXIT;
                else
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
            } // End IF

            break;

        case ARRAY_HETERO:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                aplHeteroRht  = *((LPAPLHETERO) lpMemRht)++;

                // Split cases based upon the ptr type bits
                switch (GetPtrTypeDir (aplHeteroRht))
                {
                    case PTRTYPE_STCONST:
                        aplLongestRht = aplHeteroRht->stData.stLongest;

                        // Split cases based upon the immediate type
                        switch (aplHeteroRht->stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:
                                *((LPAPLHETERO) lpMemRes)++ =
                                lpSymTmp =
                                  SymTabAppendChar_EM    ((APLBOOL) aplLongestRht, TRUE);
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                                break;

                            case IMMTYPE_FLOAT:
                                // Attempt to convert the float to an integer using System []CT
                                aplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
                                if (!bRet)
                                    goto DOMAIN_EXIT;

                                // Fall through to common code

                            case IMMTYPE_INT:
                                // Check for out of range for Unicode
                                //   as an APLUINT so we don't have to deal with negatives
                                if (MAX_UNICODE < aplLongestRht)
                                    goto DOMAIN_EXIT;

                                // Check for out of range for UCS-2
                                if (APLCHAR_SIZE <= aplLongestRht)
////////////////////////////////////aplLongestRht = UTF16_REPLACEMENTCHAR;
                                    goto NONCE_EXIT;

                                *((LPAPLHETERO) lpMemRes)++ =
                                lpSymTmp =
                                  SymTabAppendChar_EM    ((APLCHAR) aplLongestRht, TRUE);
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                                break;

                            case IMMTYPE_CHAR:
                                *((LPAPLHETERO) lpMemRes)++ =
                                lpSymTmp =
                                  SymTabAppendInteger_EM ((APLCHAR) aplLongestRht, TRUE);
                                if (lpSymTmp EQ NULL)
                                    goto ERROR_EXIT;
                                break;

                            defstop
                                break;
                        } // End SWITCH

                        break;

                    case PTRTYPE_HGLOBAL:
                        // Lock the memory to get a ptr to it
                        lpMemHeteroRht = MyGlobalLockVar (aplHeteroRht);

                        // Get the array type
                        aplTypeHet = ((LPVARARRAY_HEADER) lpMemHeteroRht)->ArrType;

                        // Skip over the header and dimensions to the data
                        lpMemHeteroRht = VarArrayDataFmBase (lpMemHeteroRht);

                        // Split cases based upon the array storage type
                        switch (aplTypeHet)
                        {
                            case ARRAY_RAT:
                                aplLongestRht = mpq_get_sctsx ((LPAPLRAT) lpMemHeteroRht, &bRet);

                                break;

                            case ARRAY_VFP:
                                aplLongestRht = mpfr_get_sctsx ((LPAPLVFP) lpMemHeteroRht, &bRet);

                                break;

                            defstop
                                break;
                        } // End SWITCH

                        if (!bRet)
                            goto DOMAIN_EXIT;

                        // Check for out of range for Unicode
                        //   as an APLUINT so we don't have to deal with negatives
                        if (MAX_UNICODE < aplLongestRht)
                            goto DOMAIN_EXIT;

                        // Check for out of range for UCS-2
                        if (APLCHAR_SIZE <= aplLongestRht)
////////////////////////////aplLongestRht = UTF16_REPLACEMENTCHAR;
                            goto NONCE_EXIT;

                        *((LPAPLHETERO) lpMemRes)++ =
                        lpSymTmp =
                          SymTabAppendChar_EM    ((APLCHAR) aplLongestRht, TRUE);
                        if (lpSymTmp EQ NULL)
                            goto ERROR_EXIT;

                        // We no longer need this ptr
                        MyGlobalUnlock (aplHeteroRht); lpMemHeteroRht = NULL;

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            break;

        case ARRAY_RAT:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Attempt to convert the RAT to an integer using System []CT
                aplLongestRht = mpq_get_sctsx (((LPAPLRAT) lpMemRht)++, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;

                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
////////////////////*((LPAPLCHAR) lpMemRes)++ = UTF16_REPLACEMENTCHAR;
                    goto NONCE_EXIT;
                else
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
            } // End FOR

            break;

        case ARRAY_VFP:
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Attempt to convert the VFP to an integer using System []CT
                aplLongestRht = mpfr_get_sctsx (((LPAPLVFP) lpMemRht)++, &bRet);
                if (!bRet)
                    goto DOMAIN_EXIT;

                // Check for out of range for Unicode
                //   as an APLUINT so we don't have to deal with negatives
                if (MAX_UNICODE < aplLongestRht)
                    goto DOMAIN_EXIT;

                // Check for out of range for UCS-2
                if (APLCHAR_SIZE <= aplLongestRht)
////////////////////*((LPAPLCHAR) lpMemRes)++ = UTF16_REPLACEMENTCHAR;
                    goto NONCE_EXIT;
                else
                    *((LPAPLCHAR) lpMemRes)++ = (APLCHAR) aplLongestRht;
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH

    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    // We no longer need this ptr
    if (hGlbRes NE NULL && lpMemRes NE NULL)
    {
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // We no longer need this ptr
    if (hGlbRht NE NULL && lpMemRht NE NULL)
    {
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysnMonUCS_EM_YY


//***************************************************************************
//  $SysFnDydUCS_EM_YY
//
//  Dyadic []UCS -- Convert between UCS-2 and UTF-8, -16, and -32.
//***************************************************************************

LPPL_YYSTYPE SysFnDydUCS_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht;           // Right ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht;           // Right ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht;           // Right ...
    LPVARARRAY_HEADER lpMemHdrLft = NULL,   // Ptr to left arg global memory header
                      lpMemHdrRht = NULL;   // ...    right ...
    APLLONGEST        aplLongestRht;        // Right arg as an immediate
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL;       // Right ...
    LPAPLCHAR         lpMemLft;             // Ptr to left arg global memory data
    LPVOID            lpMemRht;             // ...    right ...
    UCHAR             uUTF = 0;             // UTF width:  8, 16, or 32
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for LEFT RANK ERROR
    if (!IsVector (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleChar (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Get the left & right args global memory handles
                    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

    // Check the left arg
    if (lstrcmpW (lpMemLft, L"UTF-8" ) EQ 0)
        uUTF = 8;
    else
    if (lstrcmpW (lpMemLft, L"UTF-16") EQ 0)
        uUTF = 16;
    else
    if (lstrcmpW (lpMemLft, L"UTF-32") EQ 0)
        uUTF = 32;

    // Check for LEFT DOMAIN ERROR
    if (uUTF EQ 0)
        goto LEFT_DOMAIN_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsRank2P (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // If the right arg is a global, ...
    if (hGlbRht NE NULL)
        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);
    else
        // Point to the data
        lpMemRht = &aplLongestRht;

    // Split cases based upon Numeric v. Char
    if (IsNumeric (aplTypeRht))
        lpYYRes = UTFConvertNumToChr_EM (uUTF, aplTypeRht, aplNELMRht, lpMemRht, lptkFunc);
    else
    if (IsSimpleChar (aplTypeRht))
        lpYYRes = UTFConvertChrToNum_EM (uUTF, aplTypeRht, aplNELMRht, lpMemRht, lptkFunc);
    else
        goto RIGHT_DOMAIN_EXIT;

    if (lpYYRes NE NULL)
        goto NORMAL_EXIT;
    else
        goto ERROR_EXIT;
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

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
    if (hGlbLft NE NULL && lpMemHdrLft EQ NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemHdrLft = NULL;
    } // End IF

    if (hGlbRht NE NULL && lpMemHdrRht EQ NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemHdrRht = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydUCS_EM_YY


//***************************************************************************
//  $UTFConvertNumToChr_EM
//
//  Convert UTF Num to Chr
//***************************************************************************

LPPL_YYSTYPE UTFConvertNumToChr_EM
    (UCHAR    uUTF,             // UTF width:  8, 16, or 32
     APLSTYPE aplTypeRht,       // Right arg storage type
     APLNELM  aplNELMRht,       // Right ...
     LPVOID   lpMemRht,         // Ptr to right arg global memory data
     LPTOKEN  lptkFunc)         // Ptr to function token

{
    LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to result global memory header
    APLSTYPE          aplTypeRes = ARRAY_CHAR;  // Result storage type
    APLNELM           aplNELMRes,               // Result NELM
                      uRht;                     // Loop counter
    HGLOBAL           hGlbRes = NULL;           // Result global memory handle
    UBOOL             bRet = TRUE;              // True iff the result is valid
    APLINT            aplInt;                   // Temp
    LPPL_YYSTYPE      lpYYRes = NULL;           // Ptr to the result
    APLINT            maxItem,                  // Maximum value for an item
                      maxSize;                  // ...     size (NONCE check on UTF-32)
                                                //   Dependent on uUTF
    LPAPLCHAR         lpMemRes,                 // Ptr to result global memory data
                      lpMemTmp = NULL;          // Ptr to temp

    // Calculate the maximum item value:            8     16      32 (21/16)
    maxItem = (BIT0 << min (uUTF, 21)) - 1;     // 0xFF, 0xFFFF, 0x001FFFFF
    maxSize = (BIT0 << min (uUTF, 16)) - 1;     // 0xFF, 0xFFFF, 0x0000FFFF

    // The result NELM for all UTF conversions is no larger than the right arg NELM
    aplNELMRes = aplNELMRht;

    // Allocate global memory for the result
    hGlbRes =
      AllocateGlobalArray (aplTypeRes, aplNELMRes, 1, &aplNELMRes);

    // Check for error
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;
    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Allocate a temp array for a copy of the right arg
    //   accounting for a trailing zero of size 1-, 2-, or 4-bytes
    lpMemTmp = MyGlobalAlloc (GPTR, (size_t) (aplNELMRht + 1) * (uUTF / 8));

    // Check for error
    if (lpMemTmp EQ NULL)
        goto WSFULL_EXIT;

    // Copy the right arg to the result
    //   converting to an integer in the process
    for (uRht = 0; uRht < aplNELMRht; uRht++)
    {
        // Attempt to convert to an integer
        aplInt = ConvertToInteger_SCT (aplTypeRht, lpMemRht, uRht, &bRet);

        // Check for error
        if (!bRet
         || (0 > aplInt || aplInt > maxItem))
            goto DOMAIN_EXIT;

        if (aplInt > maxSize)
            goto NONCE_EXIT;            // ***FIXME*** -- UTF-32

        // Split cases based upon the UTF width
        switch (uUTF)
        {
            case  8:
                // Save in the temp as BYTEs
                ((LPBYTE ) lpMemTmp)[uRht] = (BYTE ) aplInt;

                break;

            case 16:
                // Save in the temp as WORDs
                ((LPWORD ) lpMemTmp)[uRht] = (WORD ) aplInt;

                break;

            case 32:
                // Save in the temp as DWORDs
                ((LPDWORD) lpMemTmp)[uRht] = (DWORD) aplInt;

                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR

    // Split cases based upon the UTF width
    switch (uUTF)
    {
        case  8:
            // If the right arg is NOT empty, ... (W doesn't handle empty args)
            if (!IsEmpty (aplNELMRht))
            {
                // Convert from UTF-8 to WCHAR
                aplNELMRes =
                  MultiByteToWideChar (CP_UTF8,         // CodePage
                                       0,               // dwFlags
                               (LPSTR) lpMemTmp,        // lpMultiByteStr
                                 (int) aplNELMRht,      // cbMultiByte
                                       lpMemRes,        // lpWideCharStr
                                 (int) aplNELMRes);     // cchWideChar
#ifdef DEBUG
                if (aplNELMRes EQ 0)
                {
                    DWORD dwErr = GetLastError ();
                    DbgBrk ();          // #ifdef DEBUG
                } // End IF
#endif
            } // End IF

            // Save back the corrected result NELM
            lpMemHdrRes->NELM                    =
            (VarArrayBaseToDim (lpMemHdrRes))[0] = aplNELMRes;

            break;

        case 16:
            // Copy the temp to the result
            CopyMemoryW (lpMemRes, lpMemTmp, (size_t) aplNELMRht);

            break;

        case 32:
            //Loop through the right arg and result
            for (uRht = 0; uRht < aplNELMRht; uRht++)
                // Save in the result
                lpMemRes[uRht] = (WCHAR) ((LPDWORD) lpMemTmp)[uRht];
////////////////lpMemRes[uRht] =         ((LPDWORD) lpMemTmp)[uRht];    // ***FIXME*** -- UTF-32

            break;

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    if (lpMemTmp NE NULL)
    {
        // We no longer need this storage
        MyGlobalFree (lpMemTmp); lpMemTmp = NULL;
    } // End IF

    return lpYYRes;
} // End UTFConvertNumToChr_EM


//***************************************************************************
//  $UTFConvertChrToNum_EM
//
//  Convert UTF Chr to Num
//***************************************************************************

LPPL_YYSTYPE UTFConvertChrToNum_EM
    (UCHAR     uUTF,            // UTF width:  8, 16, or 32
     APLSTYPE  aplTypeRht,      // Right arg storage type
     APLNELM   aplNELMRht,      // Right ...
     LPAPLCHAR lpMemRht,        // Ptr to right arg global memory data
     LPTOKEN   lptkFunc)        // Ptr to function token

{
    LPVARARRAY_HEADER lpMemHdrRes = NULL;       // Ptr to result global memory header
    APLSTYPE          aplTypeRes = ARRAY_INT;   // Result storage type
    APLNELM           aplNELMRes,               // Result NELM
                      uRht;                     // Loop counter
    APLRANK           aplRankRes = 1;           // Result rank
    HGLOBAL           hGlbRes = NULL;           // Result global memory handle
    LPAPLINT          lpMemRes,                 // Ptr to result global memory data
                      lpMemDst;                 // Ptr to destination
    LPUCHAR           lpMemSrc;                 // Ptr to source
    LPPL_YYSTYPE      lpYYRes = NULL;           // Ptr to the result

    // Split cases based upon the UTF conversion width
    switch (uUTF)
    {
        case  8:
            // The result NELM for UTF-8 conversion can't be more than 4 times the right arg NELM
            aplNELMRes = 4 * aplNELMRht;

            break;

        case 16:
            // The result NELM for UTF-16 conversion is the same as the right arg NELM
            aplNELMRes = aplNELMRht;

            break;

        case 32:
            // The result NELM for UTF-32 conversion is the same as the right arg NELM
            aplNELMRes = aplNELMRht;

            break;

        defstop
            break;
    } // End SWITCH

    // Allocate global memory for the result
    hGlbRes =
      AllocateGlobalArray (aplTypeRes, aplNELMRes, 1, &aplNELMRes);

    // Check for error
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;
    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLockVar (hGlbRes);

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);

    // Split cases based upon the UTF width
    switch (uUTF)
    {
        case  8:
            // If the right arg is NOT empty, ... (W doesn't handle empty args)
            if (!IsEmpty (aplNELMRht))
            {
                // Convert from WCHAR to UTF-8
                aplNELMRes =
                  WideCharToMultiByte (CP_UTF8,         // CodePage
                                       0,               // dwFlags
                                       lpMemRht,        // lpWideCharStr
                                 (int) aplNELMRht,      // cchWideChar
                               (LPSTR) lpMemRes,        // lpMultiByteStr
                                 (int) aplNELMRes,      // cbMultiByte
                                       NULL,            // lpDefaultChar
                                       NULL);           // lpUsedDefaultChar
#ifdef DEBUG
                if (aplNELMRes EQ 0)
                {
                    DWORD dwErr = GetLastError ();
                    DbgBrk ();          // #ifdef DEBUG
                } // End IF
#endif
            } // End IF

            // Save back the corrected result NELM
            lpMemHdrRes->NELM                    =
            (VarArrayBaseToDim (lpMemHdrRes))[0] = aplNELMRes;

            // Point to past the end of the destination
            lpMemDst = &lpMemRes[aplNELMRes];

            // Point to past the end of the source
            lpMemSrc = &((LPSTR) lpMemRes)[aplNELMRes];

            // Translate the bytes in MemStr to APLINTs
            while (aplNELMRes-- NE 0)
                // Save in the result
                *--lpMemDst = *--lpMemSrc;

            break;

        case 16:
        case 32:
            //Loop through the right arg and result
            for (uRht = 0; uRht < aplNELMRht; uRht++)
                // Save in the result
                lpMemRes[uRht] = lpMemRht[uRht];

            break;

        defstop
            break;
    } // End SWITCH

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes NE NULL)
    {
        if (lpMemHdrRes NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes NE NULL && lpMemHdrRes NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemHdrRes = NULL;
    } // End IF

    return lpYYRes;
} // End UTFConvertChrToNum_EM


//***************************************************************************
//  End of File: qf_ucs.c
//***************************************************************************
