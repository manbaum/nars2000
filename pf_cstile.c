//***************************************************************************
//  NARS2000 -- Primitive Function -- CircleStile
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
#include "headers.h"


//***************************************************************************
//  $PrimFnCircleStile_EM_YY
//
//  Primitive function for monadic and dyadic CircleStile ("reverse" and "rotate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnCircleStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnCircleStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTILE
         || lptkFunc->tkData.tkChar EQ UTF16_CIRCLEBAR);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonCircleStile_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydCircleStile_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnCircleStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnCircleStile_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic CircleStile
//***************************************************************************

LPPL_YYSTYPE PrimProtoFnCircleStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnCircleStile_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnCircleStile_EM_YY


//***************************************************************************
//  $PrimFnMonCircleStile_EM_YY
//
//  Primitive function for monadic CircleStile ("reverse")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCircleStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonCircleStile_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeRht,       // Right arg storage type
                  aplTypeRes;       // Result    ...
    APLNELM       aplNELMRht;       // Right arg NELM
    APLRANK       aplRankRht;       // Right arg rank
    HGLOBAL       hGlbRht = NULL,   // Right arg global memory handle
                  hGlbRes = NULL;   // Result    ...
    LPVOID        lpMemRht = NULL,  // Ptr to right arg global memory
                  lpMemRes = NULL;  // Ptr to result    ...
    LPAPLDIM      lpMemDimRht;      // Ptr to right arg dimensions
    LPVARARRAY_HEADER lpMemHdrRht;  // Ptr to right arg header
    APLUINT       aplAxis,          // The (one and only) axis value
                  uDim,             // Loop counter
                  uLo,              // ...
                  uDimLo,           // ...
                  uAx,              // ...
                  uDimAx,           // ...
                  uHi,              // ...
                  uDimHi,           // ...
                  uRes,             // ...
                  uRht;             // ...
    APLINT        apaOffRht,        // Right arg APA offset
                  apaMulRht;        // Right arg APA multiplier
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    APLUINT       ByteRes;          // # bytes in the result
    UINT          uBitMask,         // Bit mask when looping through Booleans
                  uBitIndex;        // Bit index ...
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Reverse allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
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
        // If CircleStile, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTILE)
            aplAxis = aplRankRht - 1;
        else
            // Otherwise, it's CircleBar on the first dimension
            aplAxis = 0;
    } // End IF/ELSE

    // Check for singleton
    if (IsSingleton (aplNELMRht))
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Copy the right arg token
        lpYYRes->tkToken = *CopyToken_EM (lptkRhtArg, FALSE);

        goto IMMED_EXIT;
    } // End IF

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // In case we need it, save a ptr to the right arg hdr
    lpMemHdrRht = lpMemRht;

    // Skip over the header to the dimensions
    lpMemRht = VarArrayBaseToDim (lpMemRht);
    lpMemDimRht = (LPAPLDIM) lpMemRht;

    // The type of the result is the same as the
    //   type of the right arg except APA -> INT
    if (IsSimpleAPA (aplTypeRht))
    {
        // The reverse of an APV is an APV
        if (IsVector (aplRankRht))
            aplTypeRes = aplTypeRht;
        else
            aplTypeRes = ARRAY_INT;
    } else
        aplTypeRes = aplTypeRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Allocate storage for the result
    // N.B. Conversion from APLUINT to UINT.
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->PV0        = lpMemHdrRht->PV0;
    lpHeader->PV1        = lpMemHdrRht->PV1;
    lpHeader->bSelSpec   = lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the dimensions
    for (uDim = 0; uDim < aplRankRht; uDim++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

    // Copy the data to the result

    // Handle prototypes
    if (IsEmpty (aplNELMRht))
    {
        if (IsNested (aplTypeRht))
            *((LPAPLNESTED) lpMemRes) = CopySymGlbInd_PTB (lpMemRht);
        goto NORMAL_EXIT;
    } // End IF

    if (IsScalar (aplRankRht))
        uDimLo = uDimAx = uDimHi = 1;
    else
    {
        // Calculate the product of the dimensions below the axis dimension
        for (uDim = 0, uDimLo = 1; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        uDimAx = lpMemDimRht[uDim++];

        // Calculate the product of the dimensions below the axis dimension
        for (uDimHi = 1; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];
    } // End IF/ELSE

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    uRes = uDim +                 uAx  * uDimHi;
                    uRht = uDim + ((uDimAx - 1) - uAx) * uDimHi;
                    uBitMask  = BIT0 << (UINT) (uRht % NBIB);
                    uBitIndex = (UINT) (uRes % NBIB);
                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                    ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE) << uBitIndex;
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLINT) lpMemRes)[uDim +                 uAx  * uDimHi] =
                    ((LPAPLINT) lpMemRht)[uDim + ((uDimAx - 1) - uAx) * uDimHi];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLFLOAT) lpMemRes)[uDim +                 uAx  * uDimHi] =
                    ((LPAPLFLOAT) lpMemRht)[uDim + ((uDimAx - 1) - uAx) * uDimHi];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLCHAR) lpMemRes)[uDim +                 uAx  * uDimHi] =
                    ((LPAPLCHAR) lpMemRht)[uDim + ((uDimAx - 1) - uAx) * uDimHi];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
            // If the result is an APV, ...
            if (IsSimpleAPA (aplTypeRes))
            {
                Assert (uDimAx EQ aplNELMRht);

                // Calculate the new APA parameters
                ((LPAPLAPA) lpMemRes)->Off = apaOffRht + apaMulRht * (aplNELMRht - 1);
                ((LPAPLAPA) lpMemRes)->Mul = -apaMulRht;
            } else
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLINT) lpMemRes)[uDim +                 uAx  * uDimHi] =
                       apaOffRht + apaMulRht * (uDim + ((uDimAx - 1) - uAx) * uDimHi);
                } // End FOR
            } // End IF/ELSE/FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLNESTED) lpMemRes)[uDim +                 uAx  * uDimHi] = CopySymGlbDir_PTB
                   (((LPAPLNESTED) lpMemRht)[uDim + ((uDimAx - 1) - uAx) * uDimHi]);
                } // End FOR
            } // End FOR/FOR

            break;

        defstop
            break;
    } // End SWITCH
NORMAL_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto IMMED_EXIT;

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
IMMED_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnMonCircleStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydCircleStile_EM_YY
//
//  Primitive function for dyadic CircleStile ("rotate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydCircleStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydCircleStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeLft,       // Left arg storage type
                  aplTypeRht,       // Right ...
                  aplTypeRes;       // Result   ...
    APLNELM       aplNELMLft,       // Left arg NELM
                  aplNELMRht;       // Right ...
    APLRANK       aplRankLft,       // Left arg rank
                  aplRankRht;       // Right ...
    HGLOBAL       hGlbLft = NULL,   // Left arg global memory handle
                  hGlbRht = NULL,   // Right ...
                  hGlbRes = NULL,   // Result   ...
                  hGlbRot = NULL;   // Normalized left arg ...
    LPAPLDIM      lpMemDimLft,      // Ptr to left arg dimensions
                  lpMemDimRht;      // Ptr to right ...
    LPVOID        lpMemLft = NULL,  // Ptr to left arg global memory
                  lpMemRht = NULL,  // Ptr to right ...
                  lpMemRes = NULL;  // Ptr to result   ...
    LPVARARRAY_HEADER lpMemHdrRht;  // Ptr to right arg header
    LPAPLINT      lpMemRot = NULL;  // Ptr to normalized left arg ...
    UBOOL         bRet = TRUE;      // TRUE iff result is valid
    APLUINT       aplAxis,          // The (one and only) axis value
                  ByteRes,          // # bytes in the result
                  uLo,              // Loop counter
                  uDimLo,           // ...
                  uAx,              // ...
                  uDimAx,           // ...
                  uHi,              // ...
                  uDimHi,           // ...
                  uDim,             // ...
                  uRes,             // ...
                  uRht;             // ...
    APLINT        aplIntegerLft,    // Temporary left arg integer
                  apaOffLft,        // Left arg APA offset
                  apaMulLft,        // Left arg APA multiplier
                  apaOffRht,        // Right arg APA offset
                  apaMulRht,        // Right arg APA multiplier
                  aplRot;           //
    APLLONGEST    aplLongestLft;    // Left arg immediate value
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    UINT          uBitMask,         // Bit mask when looping through Booleans
                  uBitIndex;        // Bit index ...
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Rotate allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
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
        // If CircleStile, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTILE)
            aplAxis = aplRankRht - 1;
        else
            // Otherwise, it's CircleBar on the first dimension
            aplAxis = 0;
    } // End IF/ELSE

    // Get left and right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
                    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // In case we need access to the right arg header, save that ptr
    lpMemHdrRht = lpMemRht;

    // Scalar or one-element vector left arg matches everything
    if (!(IsScalar (aplRankLft) || IsVectorSing (aplNELMLft, aplRankLft)))
    {
        // Check for RANK ERROR
        if ((aplRankRht - aplRankLft) NE 1)
            goto RANK_EXIT;

        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);

        // Check for LENGTH ERROR
        for (uDim = 0; uDim < aplRankLft; uDim++)
        if (lpMemDimLft[uDim] NE lpMemDimRht[uDim + (uDim >= aplAxis)])
            goto LENGTH_EXIT;
    } // End IF

    // Check for DOMAIN ERROR
    if (!IsSimpleNum (aplTypeLft))
        goto DOMAIN_EXIT;

    // If the left arg is singleton, get its value
    if (IsSingleton (aplNELMLft))
    {
        // If the left arg is float, ...
        if (IsSimpleFlt (aplTypeLft))
        {
            // Attempt to convert the float to an integer using System CT
            aplIntegerLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);
            if (!bRet)
                goto DOMAIN_EXIT;
        } else
            aplIntegerLft = aplLongestLft;
    } else
    // Split cases based upon the left arg's storage type
    {
        LPAPLINT lpMemRotIni;

        // Copy left arg to temp storage (all APLINTs), and
        //   check the left arg for valid values

        // Allocate temp storage for the normalized left arg
        // N.B.: Conversion from APLUINT to UINT
        ByteRes = aplNELMLft * sizeof (APLINT);
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;
        hGlbRot = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbRot)
            goto WSFULL_EXIT;

        // If the left arg is non-empty, ...
        if (ByteRes)
            // Lock the memory to get a ptr to it
            lpMemRot = lpMemRotIni = MyGlobalLock (hGlbRot);
        else
            // The left arg is empty and W doesn't handle
            //   locking empty storage, so we just NULL the ptrs
            lpMemRot = lpMemRotIni = NULL;

        // Skip over the header and dimensions to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

        // Split cases based upon the left arg storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
                uBitMask = BIT0;

                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    *lpMemRot++ = (uBitMask & *(LPAPLBOOL) lpMemLft) ? TRUE : FALSE;

                    // Shift over the left bit mask
                    uBitMask <<= 1;

                    // Check for end-of-byte
                    if (uBitMask EQ END_OF_BYTE)
                    {
                        uBitMask = BIT0;            // Start over
                        ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                    } // End IF
                } // End IF

                break;

            case ARRAY_INT:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                    *lpMemRot++ = *((LPAPLINT) lpMemLft)++;
                break;

            case ARRAY_FLOAT:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Attempt to convert the float to an integer using System CT
                    aplIntegerLft = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemLft)++, &bRet);
                    if (!bRet)
                        goto DOMAIN_EXIT;

                    *lpMemRot++ = aplIntegerLft;
                } // End FOR

                break;

            case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemLft)
                // Get the APA parameters
                apaOffLft = lpAPA->Off;
                apaMulLft = lpAPA->Mul;
#undef  lpAPA
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                    *lpMemRot++ = apaOffLft + apaMulLft * uDim;
                break;

            defstop
                break;
        } // End SWITCH

        // Restore lpMemRot to the start of the block
        lpMemRot = lpMemRotIni;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    // Map APA right arg to INT result
    if (IsSimpleAPA (aplTypeRht))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Strip out the simple scalar right argument case
    if (IsScalar (aplRankRht) && IsSimpleNH (aplTypeRes))
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Split cases based upon the right arg's token type
        switch (lptkRhtArg->tkFlags.TknType)
        {
            case TKT_VARNAMED:
                // tkData is an LPSYMENTRY
                Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

                // If it's not immediate, we must look inside the array
                if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
                {
                    // stData is a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir_PTB (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                    // If we ever get here, we must have missed a type demotion
                    DbgStop ();
                } // End IF

                // Handle the immediate case

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = lptkRhtArg->tkData.tkSym->stFlags.ImmType;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE; // Already zero from YYAlloc
                lpYYRes->tkToken.tkData.tkLongest  = lptkRhtArg->tkData.tkSym->stData.stLongest;
                lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                break;

            case TKT_VARIMMED:
                // Fill in the result token
                lpYYRes->tkToken = *lptkRhtArg;

                break;

            defstop
                break;
        } // End SWITCH

        goto NORMAL_EXIT;
    } // End IF

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->PV0        = lpMemHdrRht->PV0;
    lpHeader->PV1        = lpMemHdrRht->PV1;
    lpHeader->bSelSpec   = lpMemHdrRht->bSelSpec;
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);
    lpMemDimRht = lpMemRht = VarArrayBaseToDim (lpMemRht);

    if (IsScalar (aplRankRht))
        uDimLo = uDimAx = uDimHi = 1;
    else
    {
        // Calculate the product of the right arg's dimensions below the axis dimension
        for (uDim = 0, uDimLo = 1; uDim < aplAxis; uDim++)
            uDimLo *= lpMemDimRht[uDim];

        // Get the length of the axis dimension
        uDimAx = lpMemDimRht[uDim++];

        // Calculate the product of the right arg's dimensions below the axis dimension
        for (uDimHi = 1; uDim < aplRankRht; uDim++)
            uDimHi *= lpMemDimRht[uDim];
    } // End IF/ELSE

    // Copy the dimensions from the right arg to the result
    for (uDim = 0; uDim < aplRankRht; uDim++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;;

    // lpMemRes now points to the result's data
    // lpMemRht now points to the right arg's data

    // Handle empty nested array results (prototypes)
    if (IsEmpty (aplNELMRht) && IsNested (aplTypeRes))
        *((LPAPLNESTED) lpMemRes) = CopySymGlbInd_PTB (lpMemRht);
    else
    // Copy the data to the result
    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    uRes = uDim +                  uAx           * uDimHi;
                    uRht = uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi;
                    uBitMask  = BIT0 << (UINT) (uRht % NBIB);
                    uBitIndex = (UINT) (uRes % NBIB);
                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                    ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE) << uBitIndex;
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLINT) lpMemRes)[uDim +                  uAx           * uDimHi] =
                    ((LPAPLINT) lpMemRht)[uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLFLOAT) lpMemRes)[uDim +                  uAx           * uDimHi] =
                    ((LPAPLFLOAT) lpMemRht)[uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLCHAR) lpMemRes)[uDim +                  uAx           * uDimHi] =
                    ((LPAPLCHAR) lpMemRht)[uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi];
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLINT) lpMemRes)[uDim +                  uAx           * uDimHi] =
                       apaOffRht + apaMulRht * (uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi);
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (IsSingleton (aplNELMLft))
                    aplRot = aplIntegerLft;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    ((LPAPLNESTED) lpMemRes)[uDim +                  uAx           * uDimHi] = CopySymGlbDir_PTB
                   (((LPAPLNESTED) lpMemRht)[uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi]);
                } // End FOR
            } // End FOR/FOR

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

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    bRet = FALSE;

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
    if (hGlbRot)
    {
        if (lpMemRot)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRot); lpMemRot = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbRot); hGlbRot = NULL;
    } // End IF

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

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimFnDydCircleStile_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_cstile.c
//***************************************************************************
