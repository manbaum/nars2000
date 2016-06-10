//***************************************************************************
//  NARS2000 -- Primitive Function -- Slope
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

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
//  $PrimFnSlope_EM_YY
//
//  Primitive function for monadic and dyadic Slope (ERROR and "expand")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnSlope_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_SLOPE
         || lptkFunc->tkData.tkChar EQ UTF16_SLOPEBAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonSlope_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydSlope_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnSlope_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Slope
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnSlope_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnSlope_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnSlope_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonSlope_EM_YY
//
//  Primitive function for monadic Slope (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonSlope_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimFnMonSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydSlope_EM_YY
//
//  Primitive function for dyadic Slope ("expand")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydSlope_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft,           // Left arg storage type
                      aplTypeRht,           // Right ...
                      aplTypeRes;           // Result   ...
    APLNELM           aplNELMLft,           // Left arg NELM
                      aplNELMRht,           // Right ...
                      aplNELMRes;           // Result ...
    APLRANK           aplRankLft,           // Left arg rank
                      aplRankRht,           // Right ...
                      aplRankRes;           // Result   ...
    HGLOBAL           hGlbLft = NULL,       // Left arg global memory handle
                      hGlbRht = NULL,       // Right ...
                      hGlbRes = NULL,       // Result   ...
                      hGlbRep = NULL;       // Replicate ...
    LPAPLDIM          lpMemDimRht = NULL;   // Ptr to right arg dimensions
    LPVOID            lpMemLft = NULL,      // Ptr to left arg global memory
                      lpMemRht = NULL,      // Ptr to right ...
                      lpMemRes = NULL;      // Ptr to result   ...
    LPAPLINT          lpMemRep = NULL;      // Ptr to replicate ...
    APLLONGEST        aplLongestLft,        // Left arg immediate value
                      aplLongestRht;        // Right ...
    UBOOL             bRet;                 // TRUE iff result is valid
    APLUINT           aplAxis,              // The (one and only) axis value
                      ByteRes,              // # bytes in the result
                      uLo,                  //
                      uDimLo,               //
                      uAx,                  //
                      uDimAxRht,            //
                      uDimLftSum,           //
                      uHi,                  //
                      uDimHi,               //
                      uDim,                 //
                      uDimRht,              //
                      uDimRes,              //
                      uRes,                 //
                      uRht,                 //
                      uAcc,                 //
                      uLen;                 //
    APLINT            aplIntegerLft,        //
                      aplIntegerRep,        //
                      apaOff,               //
                      apaMul;               //
    APLCHAR           aplCharRep;           //
    LPVOID            lpSymGlbLft,          // Ptr to left arg global numeric
                      lpSymGlbRht,          //
                      aplNestProto = NULL;  //
    LPPL_YYSTYPE      lpYYRes = NULL;       // Ptr to the result
    UINT              uBitMask,             //
                      uBitIndex;            //
    LPPLLOCALVARS     lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    LPVARARRAY_HEADER lpMemHdrLft,          // Ptr to left arg header
                      lpMemHdrRht,          // ...    right ...
                      lpMemHdrRes;          // ...    result   ...
    ALLTYPES          atRht = {0};          // Right arg as ALLTYPES
    int               iHCDimRht,            // HC Dimension (1, 2, 4, 8)
                      i;                    // Loop counter

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // The rank of the result is the same as that of the right arg
    //   with scalar right arg promoted to a vector
    aplRankRes = max (aplRankRht, 1);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Expand allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                          &aplAxis,         // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } else
    {
        // No axis specified:
        //   if Slope, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_SLOPE)
            aplAxis = aplRankRes - 1;
        else
        {
            Assert (lptkFunc->tkData.tkChar EQ UTF16_SLOPEBAR);

            // Otherwise, it's SlopeBar on the first dimension
            aplAxis = 0;
        } // End IF/ELSE
    } // End IF/ELSE

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemHdrLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemHdrRht);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft)
     && !IsCharEmpty (aplTypeLft, aplNELMLft))
        goto LEFT_DOMAIN_EXIT;

    // If the right arg is a global, ...
    if (lpMemHdrRht)
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemHdrRht);

    // Calculate product of dimensions before, at, and after the axis dimension
    if (IsSingleton (aplNELMRht))
        uDimLo = uDimAxRht = uDimHi = 1;
    else
    {
        // Calculate the product of the right arg's dimensions below the axis dimension
        uDimLo = 1;
        for (uDim = 0; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        uDimAxRht = lpMemDimRht[uDim++];

        // Calculate the product of the right arg's dimensions above the axis dimension
        uDimHi = 1;
        for (; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];
    } // End IF/ELSE

    // If the left arg is singleton, get its value
    if (IsSingleton (aplNELMLft))
    {
        // Get the integer or float value
        GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                           &aplIntegerLft,  // Ptr to integer result
                            NULL,           // Ptr to float ...
                            NULL,           // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                           &lpSymGlbLft,     // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
        // If the left arg is an immediate, ...
        if (lpSymGlbLft EQ NULL)
            // Point to the data
            lpSymGlbLft = &aplLongestLft;

        // Attempt to convert the left arg to an INT
        aplIntegerLft = ConvertToInteger_SCT (aplTypeLft, lpSymGlbLft, 0, &bRet);
        if (!bRet)
            goto LEFT_DOMAIN_EXIT;

        // Check the singleton arg
        if (!IsBooleanValue (aplIntegerLft))
            goto LEFT_DOMAIN_EXIT;

        // Save as the sum of the values in the left arg
        uDimLftSum = aplIntegerLft;
    } else
    // Non-singleton left arg
    {
        // Copy left arg to temp storage (all APLINTs),
        //   check the left arg for valid values, and
        //   accumulate the sum (as uDimLftSum).

        // Calculate space needed for the normalized left arg
        ByteRes = aplNELMLft * sizeof (APLINT);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Allocate temp storage for the normalized left arg
        hGlbRep = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (hGlbRep EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        //   and check for empty (char) case
        if (!IsEmpty (aplNELMLft))
            lpMemRep = MyGlobalLock (hGlbRep);

        // Skip over the header to the data
        lpMemLft = VarArrayDataFmBase (lpMemHdrLft);

        // Initialize the result axis dimension accumulator
        uDimLftSum = 0;

        // Loop through the left arg
        for (uDim = 0; uDim < aplNELMLft; uDim++)
        {
            // Attempt to convert the float to an integer using System []CT
            aplIntegerLft = ConvertToInteger_SCT (aplTypeLft, lpMemLft, uDim, &bRet);

            if (!bRet || !IsBooleanValue (aplIntegerLft))
                goto LEFT_DOMAIN_EXIT;
            uDimLftSum += aplIntegerLft;

            *lpMemRep++ = aplIntegerLft;
        } // End FOR

        // Restore lpMemRep to the start of the block
        //   and check for empty (char) case
        if (!IsEmpty (aplNELMLft))
        {
            MyGlobalUnlock (hGlbRep); lpMemRep = NULL;
            lpMemRep = MyGlobalLock (hGlbRep);
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    // Check for LENGTH ERROR
    // Singleton right arg matches everything
    if (!IsSingleton (aplNELMRht)
     && uDimLftSum NE lpMemDimRht[aplAxis])
        goto LENGTH_EXIT;

    // Map APA right arg to INT result
    if (IsSimpleAPA (aplTypeRht))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Calculate the NELM of the result
    aplNELMRes = uDimLo * aplNELMLft * uDimHi;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemHdrRes = MyGlobalLock (hGlbRes);

#define lpHeader    lpMemHdrRes
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->bSelSpec   = lpMemHdrRht && lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Copy the dimensions from the right arg to the result
    if (lpMemHdrRht)
        CopyMemory (VarArrayBaseToDim (lpMemHdrRes),
                    VarArrayBaseToDim (lpMemHdrRht),
                    (APLU3264) aplRankRht * sizeof (APLDIM));

    // Fill in the axis dimension
    (VarArrayBaseToDim (lpMemHdrRes))[aplAxis] = aplNELMLft;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemHdrRes);
    if (lpMemHdrRht)
        lpMemRht = VarArrayDataFmBase (lpMemHdrRht);

    // Handle empty nested array results (prototypes)
    if (IsEmpty (aplNELMRht) && IsNested (aplTypeRht))
    {
        APLNELM aplNELM;

        // Empty nested results always have at least one element (the prototype)
        aplNELM = max (aplNELMRes, 1);

        for (uRes = 0; uRes < aplNELM; uRes++)
            *((LPAPLNESTED) lpMemRes)++ = CopySymGlbInd_PTB (lpMemRht);

        goto PROTO_EXIT;
    } // End IF

    if (IsSingleton (aplNELMLft))
        uLen = aplIntegerLft;
    // If the right arg is a singleton, get its value
    if (IsSingleton (aplNELMRht))
    {
        APLSTYPE aplTypeRep;

        GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                           &aplIntegerRep,  // Ptr to integer result
                            NULL,           // Ptr to float ...
                           &aplCharRep,     // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                           &lpSymGlbRht,    // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                           &aplTypeRep);    // Ptr to array type ...
        // If the right arg is an immediate, ...
        if (lpSymGlbRht EQ NULL)
            // Copy this value into atRht
            atRht.aplLongest = aplLongestRht;
        else
        // If the item is a ptr array, ...
        if (IsPtrArray (aplTypeRep))
            // Copy this value into atRht
            atRht.aplNested = CopySymGlbDir_PTB (lpSymGlbRht);
        else
            // Copy this value into atRht
            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpSymGlbRht, 0, &atRht);
    } // End IF

    // Calculate the HC Dimension (1, 2, 4, 8)
    iHCDimRht = TranslateArrayTypeToHCDim (aplTypeRht);

    // Copy the data to the result

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        uRes = uDimRes + uAx * uDimHi;
                        uBitIndex = (UINT) (uRes % NBIB);

                        if (!IsSingleton (aplNELMRht))
                        {
                            uRht = uDimRht + uAcc++ * uDimHi;
                            uBitMask  = BIT0 << (UINT) (uRht % NBIB);
                            aplIntegerRep = (uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE;
                        } // End IF/ELSE

                        ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |= aplIntegerRep << uBitIndex;
////////////////////} else
////////////////////{
////////////////////    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |= FALSE;  // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
        case ARRAY_HC2I:
        case ARRAY_HC4I:
        case ARRAY_HC8I:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                        {
                            // Clear the memory in case we use it and mistakenly free the copied memory
                            ZeroMemory (&atRht, sizeof (atRht));

                            // Get the next item from the right arg
                            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, uDimRht + uAcc++ * uDimHi, &atRht);
                        } // End IF

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            ((LPAPLINT  ) lpMemRes)[i + iHCDimRht * (uDimRes + uAx * uDimHi)] = atRht.aplHC8I.parts[i];
////////////////////} else
////////////////////{
////////////////////    // Loop through all of the parts
////////////////////    for (i = 0; i < iHCDimRht; i++)
////////////////////        ((LPAPLINT  ) lpMemRes)[i + iHCDimRht * (uDimRes + uAx * uDimHi)] = 0;   // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
        case ARRAY_HC2F:
        case ARRAY_HC4F:
        case ARRAY_HC8F:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                        {
                            // Clear the memory in case we use it and mistakenly free the copied memory
                            ZeroMemory (&atRht, sizeof (atRht));

                            // Get the next item from the right arg
                            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, uDimRht + uAcc++ * uDimHi, &atRht);
                        } // End IF/ELSE

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            ((LPAPLFLOAT) lpMemRes)[i + iHCDimRht * (uDimRes + uAx * uDimHi)] = atRht.aplHC8F.parts[i];
////////////////////} else
////////////////////{
////////////////////    // Loop through all of the parts
////////////////////    for (i = 0; i < iHCDimRht; i++)
////////////////////        ((LPAPLFLOAT) lpMemRes)[i + iHCDimRht * (uDimRes + uAx * uDimHi)] = 0; // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                            aplCharRep = ((LPAPLCHAR) lpMemRht)[uDimRht + uAcc++ * uDimHi];
                        ((LPAPLCHAR) lpMemRes)[uDimRes + uAx * uDimHi] = aplCharRep;
                    } else
                        ((LPAPLCHAR) lpMemRes)[uDimRes + uAx * uDimHi] = L' ';
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
#undef  lpAPA
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                            aplIntegerRep = apaOff + apaMul * (uDimRht + uAcc++ * uDimHi);
                        ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = aplIntegerRep;
////////////////////} else
////////////////////{
////////////////////    ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = 0;  // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            aplNestProto =
              MakeMonPrototype_EM_PTB (*(LPAPLNESTED) lpMemRht, // Proto arg handle
                                       ARRAY_NESTED,            // Array storage type
                                       lptkFunc,                // Ptr to function token
                                       MP_CHARS);               // CHARs allowed
            if (!aplNestProto)
                goto ERROR_EXIT;

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                            lpSymGlbRht = ((LPAPLNESTED) lpMemRht)[uDimRht + uAcc++ * uDimHi];

                        ((LPAPLNESTED) lpMemRes)[uDimRes + uAx * uDimHi] = CopySymGlbDir_PTB (lpSymGlbRht);
                    } else
                        ((LPAPLNESTED) lpMemRes)[uDimRes + uAx * uDimHi] = CopySymGlbDir_PTB (aplNestProto);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_RAT:
        case ARRAY_HC2R:
        case ARRAY_HC4R:
        case ARRAY_HC8R:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                        {
                            // Clear the memory in case we use it and mistakenly free the copied memory
                            ZeroMemory (&atRht, sizeof (atRht));

                            // Get the next item from the right arg
                            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, uDimRht + uAcc++ * uDimHi, &atRht);
                        } // End IF

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            mpq_init_set   (&((LPAPLRAT) lpMemRes)[i + iHCDimRht * (uDimRes + uAx * uDimHi)], &atRht.aplHC8R.parts[i]);
                    } else
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            mpq_init       (&((LPAPLRAT) lpMemRes)[i + iHCDimRht * (uDimRes + uAx * uDimHi)]);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_VFP:
        case ARRAY_HC2V:
        case ARRAY_HC4V:
        case ARRAY_HC8V:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    if (!IsSingleton (aplNELMLft))
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (!IsSingleton (aplNELMRht))
                        {
                            // Clear the memory in case we use it and mistakenly free the copied memory
                            ZeroMemory (&atRht, sizeof (atRht));

                            // Get the next item from the right arg
                            (*aTypeActPromote[aplTypeRht][aplTypeRht]) (lpMemRht, uDimRht + uAcc++ * uDimHi, &atRht);
                        } // End IF

                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            mpfr_init_copy (&((LPAPLVFP) lpMemRes)[i + iHCDimRht * (uDimRes + uAx * uDimHi)], &atRht.aplHC8V.parts[i]);
                    } else
                        // Loop through all of the parts
                        for (i = 0; i < iHCDimRht; i++)
                            mpfr_init0     (&((LPAPLVFP) lpMemRes)[i + iHCDimRht * (uDimRes + uAx * uDimHi)]);
                } // End FOR
            } // End FOR/FOR

            break;

        defstop
            break;
    } // End SWITCH

    // If the right arg is a singleton, get its value
    if (IsSingleton (aplNELMRht))
        // Free the old atRht
        (*aTypeFree[aplTypeRht]) (&atRht, 0);
PROTO_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
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
    // If the nested prototype is defined,
    //   and the prototype is a global memory handle, ...
    if (aplNestProto NE NULL
     && GetPtrTypeDir (aplNestProto) EQ PTRTYPE_HGLOBAL)
    {
        // We no longer need this storage
        FreeResultGlobalVar (aplNestProto); aplNestProto = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbRep, lpMemRep);

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
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
} // End PrimFnDydSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_slope.c
//***************************************************************************
