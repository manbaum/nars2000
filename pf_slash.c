//***************************************************************************
//  NARS2000 -- Primitive Function -- Slash
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif


//***************************************************************************
//  PrimFnSlash_EM
//
//  Primitive function for monadic and dyadic Slash (ERROR and "replicate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnSlash_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnSlash_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_SLASH
         || lptkFunc->tkData.tkChar EQ UTF16_SLASHBAR);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonSlash_EM    (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydSlash_EM    (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnSlash_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonSlash_EM
//
//  Primitive function for monadic Slash (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonSlash_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonSlash_EM
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End PrimFnMonSlash_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydSlash_EM
//
//  Primitive function for dyadic Slash ("replicate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydSlash_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydSlash_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE  aplTypeLft,
              aplTypeRht,
              aplTypeRes;
    APLNELM   aplNELMLft,
              aplNELMRht,
              aplNELMRes;
    APLRANK   aplRankLft,
              aplRankRht,
              aplRankRes;
    HGLOBAL   hGlbLft = NULL,
              hGlbRht = NULL,
              hGlbRes = NULL,
              hGlbRep = NULL;
    LPAPLDIM  lpMemDimRht = NULL;
    LPVOID    lpMemLft = NULL,
              lpMemRht = NULL,
              lpMemRes = NULL;
    LPAPLINT  lpMemRep = NULL;
    BOOL      bRet = TRUE;
    APLUINT   aplAxis,      // The (one and only) axis value
              ByteRes,
              uLo,
              uDimLo,
              uAx,
              uDimAxRht,
              uDimAxRes,
              uHi,
              uDimHi,
              uDim,
              uDimRht,
              uDimRes,
              uRes,
              uRht,
              uRep,
              uAcc,
              uLen;
    APLINT    aplIntegerLft,
              aplIntegerRht,
              aplIntegerRep,
              apaOff,
              apaMul;
    APLFLOAT  aplFloatLft,
              aplFloatRht,
              aplFloatRep;
    APLCHAR   aplCharRht,
              aplCharRep;
    APLNESTED aplNestRht,
              aplNestRep;
    LPYYSTYPE lpYYRes;
    UINT      uBitMask,
              uBitIndex;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // The rank of the result is the same as that of the right arg
    //   with scalar right arg promoted to a vector
    aplRankRes = max (aplRankRht, 1);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Replicate allows a single integer axis value only
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // Return TRUE iff fractional values present
                           &aplAxis,        // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } else
    {
        // No axis specified:
        //   if Slash, use last dimension
        if (lptkFunc->tkData.tkChar EQ UTF16_SLASH)
            aplAxis = aplRankRes - 1;
        else
            // Otherwise, it's SlashBar on the first dimension
            aplAxis = 0;
    } // End IF/ELSE

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for RANK ERROR
    if (aplRankLft > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkLftArg);
        goto ERROR_EXIT;
    } // End IF

        // Skip over the header to the dimensions
    if (lpMemRht)
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Singleton left arg or scalar right arg matches everything
    if (aplNELMLft NE 1
     && aplRankRht NE 0)
    {
        // Check for LENGTH ERROR
        if (aplNELMLft NE lpMemDimRht[aplAxis])
        {
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkLftArg);
            goto ERROR_EXIT;
        } // End IF
    } // End IF

    // Check for DOMAIN ERROR
    if (!IsSimpleNum (aplTypeLft))
        goto DOMAIN_EXIT;

    // Calculate product of dimensions before, at, and after the axis dimension
    if (aplRankRht EQ 0)
    {
        uDimLo = uDimHi = 1;
        uDimAxRes = uDimAxRht = aplNELMLft;
    } else
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
    if (aplNELMLft EQ 1)
    {
        // Get the integer or float value
        FirstValue (lptkLftArg,
                   &aplIntegerLft,
                   &aplFloatLft,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL);
        // Attempt to convert FLOAT left arg
        if (aplTypeLft EQ ARRAY_FLOAT)
        {
            aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
            if (!bRet)
                goto DOMAIN_EXIT;
        } // End IF

        // Check the singleton arg
        if (aplIntegerLft < 0)
            goto DOMAIN_EXIT;

        // Calculate the length of the axis dimension in the result
        if (lpMemDimRht && aplRankRht NE 0)
            uDimAxRes  = aplIntegerLft * lpMemDimRht[aplAxis];
        else
            uDimAxRes  = aplIntegerLft;
        aplNELMRes = uDimAxRes;
    } else
    // Non-singleton left arg
    {
        // Copy left arg to temp storage (all APLINTs),
        //   check the left arg for valid values, and
        //   accumulate the sum (as uDimAxRes).

        // Allocate temp storage for the normalized left arg
        // N.B.: Conversion from APLUINT to UINT
        ByteRes = aplNELMLft * sizeof (APLINT);
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbRep = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbRep)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkLftArg);
            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemRep = MyGlobalLock (hGlbRep);

        // Skip over the header to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

        // Initialize the result axis dimension accumulator
        uDimAxRes = 0;

        // Split cases based upon the left arg's storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
                uBitMask = 0x01;

                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    aplIntegerLft = (uBitMask & *(LPAPLBOOL) lpMemLft) ? 1 : 0;
                    uDimAxRes += aplIntegerLft;
                    *lpMemRep++ = aplIntegerLft;

                    // Shift over the left bit mask
                    uBitMask <<= 1;

                    // Check for end-of-byte
                    if (uBitMask EQ END_OF_BYTE)
                    {
                        uBitMask = 0x01;            // Start over
                        ((LPAPLBOOL) lpMemLft)++;   // Skip to next byte
                    } // End IF
                } // End FOR

                break;

            case ARRAY_INT:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    aplIntegerLft = *((LPAPLINT) lpMemLft)++;
                    if (aplIntegerLft < 0)
                        goto DOMAIN_EXIT;
                    uDimAxRes += aplIntegerLft;

                    *lpMemRep++ = aplIntegerLft;
                } // End IF

                break;

            case ARRAY_FLOAT:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    aplIntegerLft = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemLft)++, &bRet);
                    if (!bRet || aplIntegerLft < 0)
                        goto DOMAIN_EXIT;
                    uDimAxRes += aplIntegerLft;

                    *lpMemRep++ = aplIntegerLft;
                } // End FOR

                break;

            case ARRAY_APA:

#define lpAPA       ((LPAPLAPA) lpMemLft)

                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;

#undef  lpAPA

                // Check the first and last values
                if ((apaOff < 0)
                 || (apaOff + apaMul * (aplNELMLft - 1)) < 0)
                    goto DOMAIN_EXIT;

                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    aplIntegerLft = apaOff + apaMul * uDim;
                    uDimAxRes += aplIntegerLft;
                    *lpMemRep++ = aplIntegerLft;
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH

        // Restore lpMemRep to the start of the block
        MyGlobalUnlock (hGlbRep); lpMemRep = NULL;
        lpMemRep = MyGlobalLock (hGlbRep);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;

        // Calculate the NELM of the result
        aplNELMRes = uDimLo * uDimAxRes * uDimHi;
    } // End IF

    // Map APA right arg to INT result
    if (aplTypeRht EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header values
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;

#undef  lpHeader

    // Copy the dimensions from the right arg to the result
    if (lpMemRht)
        CopyMemory (VarArrayBaseToDim (lpMemRes),
                    VarArrayBaseToDim (lpMemRht),
                    (UINT) aplRankRht * sizeof (APLDIM));

    // Fill in the axis dimension
    (VarArrayBaseToDim (lpMemRes))[aplAxis] = uDimAxRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);
    if (lpMemRht)
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Handle empty nested array results (prototypes)
    if (aplNELMRht EQ 0 && aplTypeRht EQ ARRAY_NESTED)
    {
        *((LPAPLNESTED) lpMemRes) = CopySymGlbInd (lpMemRht);

        goto PROTO_EXIT;
    } // End IF

    // If the right arg is a singleton, get its value
    if (aplNELMRht EQ 1)
        FirstValue (lptkRhtArg,
                   &aplIntegerRht,
                   &aplFloatRht,
                   &aplCharRht,
                    NULL,
                   &aplNestRht,
                    NULL,
                    NULL);

    // Copy the data to the result

    // Split cases based upon the right arg's storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    if (lpMemRht)
                    {
                        uRht = uDimRht + uAx * uDimHi;
                        uBitMask = 1 << (UINT) (uRht % NBIB);
                        aplIntegerRep = (uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? 1 : 0;
                    } else
                        aplIntegerRep = aplIntegerRht;

                    for (uRep = 0; uRep < uLen; uRep++, uAcc++)
                    {
                        uRes = uDimRes + uAcc * uDimHi;
                        uBitIndex = (UINT) (uRes % NBIB);
                        ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                        ((aplNELMRht EQ 1) ? aplIntegerRht : aplIntegerRep) << uBitIndex;
                    } // End FOR
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    if (lpMemRht)
                        aplIntegerRep = ((LPAPLINT) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        aplIntegerRep = aplIntegerRht;

                    for (uRep = 0; uRep < uLen; uRep++, uAcc++)
                        ((LPAPLINT) lpMemRes)[uDimRes + uAcc * uDimHi] =
                        (aplNELMRht EQ 1) ? aplIntegerRht : aplIntegerRep;
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    if (lpMemRht)
                        aplFloatRep = ((LPAPLFLOAT) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        aplFloatRep = aplFloatRht;

                    for (uRep = 0; uRep < uLen; uRep++, uAcc++)
                        ((LPAPLFLOAT) lpMemRes)[uDimRes + uAcc * uDimHi] =
                        (aplNELMRht EQ 1) ? aplFloatRht : aplFloatRep;
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    if (lpMemRht)
                        aplCharRep = ((LPAPLCHAR) lpMemRht)[uDimRht + uAx * uDimHi];
                    else
                        aplCharRep = aplCharRht;

                    for (uRep = 0; uRep < uLen; uRep++, uAcc++)
                        ((LPAPLCHAR) lpMemRes)[uDimRes + uAcc * uDimHi] =
                        (aplNELMRht EQ 1) ? aplCharRht : aplCharRep;
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_APA:

#define lpAPA       ((LPAPLAPA) lpMemRht)

            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;

#undef  lpAPA

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    aplIntegerRep = apaOff + apaMul * (uDimRht + uAx * uDimHi);

                    for (uRep = 0; uRep < uLen; uRep++, uAcc++)
                        ((LPAPLINT) lpMemRes)[uDimRes + uAcc * uDimHi] =
                        (aplNELMRht EQ 1) ? aplIntegerRht : aplIntegerRep;
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht + uHi;
                uDimRes = uLo * uDimHi * uDimAxRes + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    aplNestRep = ((LPAPLNESTED) lpMemRht)[uDimRht + uAx * uDimHi];

                    for (uRep = 0; uRep < uLen; uRep++, uAcc++)
                        ((LPAPLNESTED) lpMemRes)[uDimRes + uAcc * uDimHi] = CopySymGlbDir
                       ((aplNELMRht EQ 1) ? aplNestRht : aplNestRep);
                } // End FOR
            } // End FOR/FOR

            break;

        defstop
            break;
    } // End SWITCH
PROTO_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
ERROR_EXIT:
    bRet = FALSE;
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
} // End PrimFnDydSlash_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_slash.c
//***************************************************************************
