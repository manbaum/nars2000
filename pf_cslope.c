//***************************************************************************
//  NARS2000 -- Primitive Function -- CircleSlope
//***************************************************************************

#pragma pack (1)
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
//  PrimFnCircleSlope_EM
//
//  Primitive function for monadic and dyadic CircleSlope ("transpose")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnCircleSlope_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnCircleSlope_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UCS2_CIRCLESLOPE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonCircleSlope_EM (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydCircleSlope_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnPlus_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonCircleSlope_EM
//
//  Primitive function for monadic CircleSlope (index generator)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonCircleSlope_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonCircleSlope_EM
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    APLSTYPE  aplTypeRht;
    APLNELM   aplNELMRht;
    APLRANK   aplRankRht;
    HGLOBAL   hGlbLft;
    LPVOID    lpMemLft;
    UINT      YYLclIndex,
              ByteRes;
    LPYYSTYPE lpYYRes;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

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

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Calculate space needed for the left arg
    ByteRes = (UINT) CalcArraySize (ARRAY_APA, aplRankRht, 1);

    // Allocate storage for the left argument
    hGlbLft = DbgGlobalAlloc (GHND, ByteRes);
    if (!hGlbLft)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemLft = MyGlobalLock (hGlbLft);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemLft)

    // Fill in the header values
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_APA;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplRankRht;
    lpHeader->Rank       = 1;

#undef  lpHeader

    // Fill in the dimension
    *VarArrayBaseToDim (lpMemLft) = aplRankRht;

    // Skip over the header and dimensions to the data
    lpMemLft = VarArrayBaseToData (lpMemLft, 1);

#define lpAPA       ((LPAPLAPA) lpMemLft)

    // Fill in the APA parameters
    lpAPA->Off = (aplRankRht - 1) + bQuadIO;
    lpAPA->Mul = -1;
    lpAPA->Len = aplRankRht;

#undef  lpAPA

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLft); lpMemLft = NULL;

    // Fill in the left arg token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbLft);
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Call the dyadic function
    lpYYRes = PrimFnDydCircleSlope_EM (&YYRes[YYLclIndex].tkToken, lptkFunc, lptkRhtArg, lptkAxis);

    // We no longer need this storage
    DbgGlobalFree (hGlbLft); hGlbLft = NULL;

    return lpYYRes;
} // End PrimFnMonCircleSlope_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydCircleSlope_EM
//
//  Primitive function for dyadic CircleSlope (index of)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydCircleSlope_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydCircleSlope_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

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
              hGlbAxis = NULL,
              hGlbWVec = NULL,
              hGlbOdo = NULL;
    LPAPLDIM  lpMemRhtDim,
              lpMemResDim;
    APLDIM    uMinDim;
    LPVOID    lpMemLft = NULL,
              lpMemRht = NULL,
              lpMemRes = NULL;
    LPAPLUINT lpMemAxis = NULL,
              lpMemGrUp,
              lpMemWVec = NULL,
              lpMemOdo = NULL;
    BOOL      bRet = TRUE;
    APLUINT   ByteRes, uRes, uRht, uOdo;
    UINT      YYLclIndex,
              uBitIndex,
              uBitMask;
    APLINT    iDim,
              apaOffRht,
              apaMulRht;

    // Get new index into YYRes
    YYLclIndex = YYResIndex = (YYResIndex + 1) % NUMYYRES;

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

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

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

    // Check for LENGTH ERROR
    if (aplNELMLft NE aplRankRht)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkLftArg);
        goto ERROR_EXIT;
    } // End IF

    // Treat the left arg as an axis
    if (!CheckAxis_EM (lptkLftArg,      // The "axis" token
                       aplRankRht,      // All values less than this
                       FALSE,           // TRUE iff scalar or one-element vector only
                       FALSE,           // TRUE iff want sorted axes
                       TRUE,            // TRUE iff axes must be contiguous
                       TRUE,            // TRUE iff duplicate axes are allowed
                       NULL,            // TRUE iff fractional values allowed
                      &aplRankRes,      // Return last axis value
                       NULL,            // Return # elements in axis vector
                       &hGlbAxis))      // Return HGLOBAL with APLUINT axis values
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkLftArg);
        goto ERROR_EXIT;
    } // End IF

    // Map APA right arg to INT result
    if (aplTypeRht EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Strip out the simple scalar right argument case
    if (aplRankRht EQ 0 && IsSimpleNH (aplTypeRes))
    {
        // Fill in the result token
        YYRes[YYLclIndex].tkToken = *lptkRhtArg;
        YYRes[YYLclIndex].tkToken.tkFlags.TknType = TKT_VARIMMED;

        goto NORMAL_EXIT;
    } // End IF

    // The rank of the result is one more than the highest axis value
    aplRankRes++;

    // Lock the memory to get a ptr to it
    lpMemAxis = MyGlobalLock (hGlbAxis);

    // Point to the grade-up of the first
    //   <aplRankRht> values in lpMemAxis
    lpMemGrUp = &lpMemAxis[aplRankRht];

    // Skip over the header to the dimensions
    lpMemRhtDim = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Calculate the NELM of the result
    aplNELMRes = 1;
    if (aplRankRes NE 0)
    {
        uMinDim = lpMemRhtDim[lpMemGrUp[0]];
        for (uRht = 1; uRht < aplRankRht; uRht++)
        {
            // Compare the values corresponding to the indices
            //   to see if they are the same (and thus coalescing)
            if (lpMemAxis[lpMemGrUp[uRht - 1]] EQ lpMemAxis[lpMemGrUp[uRht]])
                uMinDim = min (uMinDim, lpMemRhtDim[lpMemGrUp[uRht]]);
            else
            {
                aplNELMRes *= uMinDim;
                uMinDim = lpMemRhtDim[lpMemGrUp[uRht]];
            } // End IF/ELSE
        } // End FOR

        // Count in the last dimension
        aplNELMRes *= uMinDim;
    } // End IF

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

    // Skip over the header to the dimensions
    lpMemRes = lpMemResDim = VarArrayBaseToDim (lpMemRes);

    // Fill in the dimensions
    if (aplRankRes NE 0)
    {
        uMinDim = lpMemRhtDim[lpMemGrUp[0]];
        for (uRht = 1; uRht < aplRankRht; uRht++)
        {
            // Compare the values corresponding to the indices
            //   to see if they are the same (and thus coalescing)
            if (lpMemAxis[lpMemGrUp[uRht - 1]] EQ lpMemAxis[lpMemGrUp[uRht]])
                uMinDim = min (uMinDim, lpMemRhtDim[lpMemGrUp[uRht]]);
            else
            {
                *((LPAPLDIM) lpMemRes)++ = uMinDim;
                uMinDim = lpMemRhtDim[lpMemGrUp[uRht]];
            } // End IF/ELSE
        } // End FOR

        // Save the last dimension
        *((LPAPLDIM) lpMemRes)++ = uMinDim;
    } // End IF

    // lpMemRes now points to the data

    // Handle empty nested array results (prototypes)
    if (aplNELMRes EQ 0 && aplTypeRes EQ ARRAY_NESTED)
    {
        *((LPAPLNESTED) lpMemRes) = CopySymGlb (*(LPAPLNESTED) lpMemRht);

        goto PROTO_EXIT;
    } // End IF

    //***************************************************************
    // Allocate space for the weighting vector which is
    //   {times}{backscan}1{drop}({rho}R),1
    // N.B.  Conversion from APLUINT to UINT.
    //***************************************************************
    ByteRes = aplRankRht * sizeof (APLINT);

    // In case the result is a scalar, allocate at least
    //   one byte so the GlobalLock doesn't fail -- Windows
    //   doesn't handle the empty case well.
    ByteRes = max (ByteRes, 1);

    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbWVec = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbWVec)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemWVec = MyGlobalLock (hGlbWVec);

    // Loop through the dimensions of the result in reverse
    //   order {backscan} and compute the cumulative product
    //   (weighting vector).
    // Note we use a signed index variable because we're
    //   walking backwards and the test against zero must be
    //   made as a signed variable.
    for (uRes = 1, iDim = aplRankRht - 1; iDim >= 0; iDim--)
    {
        lpMemWVec[iDim] = uRes;
        uRes *= lpMemRhtDim[iDim];
    } // End FOR

    //***************************************************************
    // Allocate space for the odometer array, one value per dimension
    //   in the result, with values initially all zero (thanks to GHND).
    // N.B.  Conversion from APLUINT to UINT.
    //***************************************************************
    ByteRes = aplRankRes * sizeof (APLINT);

    // In case the result is a scalar, allocate at least
    //   one byte so the GlobalLock doesn't fail -- Windows
    //   doesn't handle the empty case well.
    ByteRes = max (ByteRes, 1);

    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbOdo)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the global memory to get a ptr to it
    lpMemOdo = MyGlobalLock (hGlbOdo);

    // Copy the data to the result

    // Split cases based upon the result/right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            uBitIndex = 0;

            // Loop through the elements in the result
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Use the index in lpMemOdo to calculate the
                //   corresponding index in lpMemRes where the
                //   next value from lpMemRht goes.
                for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                    uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

                uBitMask = 1 << ((int) (uRht % NBIB));

                // Copy element # uRht from the right arg to lpMemRes[uRes]
                *((LPAPLBOOL) lpMemRes) |= ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht / NBIB]) ? 1 : 0) << uBitIndex;

                // Check for end-of-byte
                if (++uBitIndex EQ NBIB)
                {
                    uBitIndex = 0;              // Start over
                    ((LPAPLBOOL) lpMemRes)++;   // Skip to next byte
                } // End IF

                // Increment the odometer in lpMemOdo subject to
                //   the values in lpMemResDim
                IncrOdometer (lpMemOdo, lpMemResDim, NULL, aplRankRes);
            } // End FOR

            break;

        case ARRAY_INT:
            // Loop through the elements in the result
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Use the index in lpMemOdo to calculate the
                //   corresponding index in lpMemRes where the
                //   next value from lpMemRht goes.
                for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                    uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLINT) lpMemRes)[uRes] = ((LPAPLINT) lpMemRht)[uRht];

                // Increment the odometer in lpMemOdo subject to
                //   the values in lpMemResDim
                IncrOdometer (lpMemOdo, lpMemResDim, NULL, aplRankRes);
            } // End FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the elements in the result
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Use the index in lpMemOdo to calculate the
                //   corresponding index in lpMemRes where the
                //   next value from lpMemRht goes.
                for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                    uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLFLOAT) lpMemRes)[uRes] = ((LPAPLFLOAT) lpMemRht)[uRht];

                // Increment the odometer in lpMemOdo subject to
                //   the values in lpMemResDim
                IncrOdometer (lpMemOdo, lpMemResDim, NULL, aplRankRes);
            } // End FOR

            break;

        case ARRAY_APA:
            // Save the APA parameters
            apaOffRht = ((LPAPLAPA) lpMemRht)->Off;
            apaMulRht = ((LPAPLAPA) lpMemRht)->Mul;

            // Loop through the elements in the result
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Use the index in lpMemOdo to calculate the
                //   corresponding index in lpMemRes where the
                //   next value from lpMemRht goes.
                for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                    uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLINT) lpMemRes)[uRes] = apaOffRht + apaMulRht * uRht;

                // Increment the odometer in lpMemOdo subject to
                //   the values in lpMemResDim
                IncrOdometer (lpMemOdo, lpMemResDim, NULL, aplRankRes);
            } // End FOR

            break;

        case ARRAY_CHAR:
            // Loop through the elements in the result
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Use the index in lpMemOdo to calculate the
                //   corresponding index in lpMemRes where the
                //   next value from lpMemRht goes.
                for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                    uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLCHAR) lpMemRes)[uRes] = ((LPAPLCHAR) lpMemRht)[uRht];

                // Increment the odometer in lpMemOdo subject to
                //   the values in lpMemResDim
                IncrOdometer (lpMemOdo, lpMemResDim, NULL, aplRankRes);
            } // End FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the elements in the result
            for (uRes = 0; uRes < aplNELMRes; uRes++)
            {
                // Use the index in lpMemOdo to calculate the
                //   corresponding index in lpMemRes where the
                //   next value from lpMemRht goes.
                for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                    uRht += lpMemOdo[lpMemAxis[uOdo]] * lpMemWVec[uOdo];

                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLHETERO) lpMemRes)[uRes] = CopySymGlb (((LPAPLHETERO) lpMemRht)[uRht]);

                // Increment the odometer in lpMemOdo subject to
                //   the values in lpMemResDim
                IncrOdometer (lpMemOdo, lpMemResDim, NULL, aplRankRes);
            } // End FOR

            break;

        defstop
            break;
    } // End SWITCH
PROTO_EXIT:
    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;
    YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;
////YYRes[YYLclIndex].tkToken.tkFlags.Color     =
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (TypeDemote (hGlbRes));
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

ERROR_EXIT:
    bRet = FALSE;
NORMAL_EXIT:
    if (hGlbWVec)
    {
        if (lpMemWVec)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    if (hGlbOdo)
    {
        if (lpMemOdo)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    if (hGlbAxis)
    {
        if (lpMemAxis)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAxis); lpMemAxis = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
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
        return &YYRes[YYLclIndex];
    else
        return NULL;
} // End PrimFnDydCircleSlope_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_cslope.c
//***************************************************************************
