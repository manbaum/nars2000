//***************************************************************************
//  NARS2000 -- Primitive Function -- DownArrow
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
//  $PrimFnDownArrow_EM_YY
//
//  Primitive function for monadic and dyadic DownArrow (ERROR and "drop")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDownArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDownArrow_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_DOWNARROW);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonDownArrow_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydDownArrow_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnDownArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnDownArrow_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic DownArrow
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnDownArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnDownArrow_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnDownArrow_EM_YY,  // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnDownArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDownArrow_EM_YY
//
//  Primitive function for dyadic DownArrow ("drop")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonDownArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonDownArrow_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (lptkFunc);
} // End PrimFnMonDownArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydDownArrow_EM_YY
//
//  Primitive function for dyadic DownArrow ("drop")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydDownArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydDownArrow_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,            // Left arg storage type
                 aplTypeRht,            // Right ...
                 aplTypeRes;            // Result   ...
    APLNELM      aplNELMLft,            // Left arg NELM
                 aplNELMRht,            // Right ...
                 aplNELMRes,            // Result   ...
                 aplNELMAxis;           // Axis     ...
    APLRANK      aplRankLft,            // Left arg rank
                 aplRankRht,            // Right ...
                 aplRankRes;            // Result   ...
    HGLOBAL      hGlbLft = NULL,        // Left arg global memory handle
                 hGlbRht = NULL,        // Right ...
                 hGlbRes = NULL,        // Result   ...
                 hGlbAxis = NULL;       // Axis     ...
    LPVOID       lpMemLft = NULL,       // Ptr to left arg global memory
                 lpMemRht = NULL,       // Ptr to right ...
                 lpMemRes = NULL;       // Ptr to result   ...
    HGLOBAL      hGlbTmpLft = NULL,     // Left arg temporary values global memory handle
                 hGlbOdoRht = NULL,     // Right arg odometer ...
                 hGlbWVecRht = NULL;    // Right arg weighting vector ...
    APLLONGEST   aplLongestLft,         // Left arg immediate value
                 aplLongestRht;         // Right arg ...
    APLINT       apaOffRht,             // Right arg APA offset
                 apaMulRht,             // Right arg APA multiplier
                 iRht,                  // Loop counter
                 aplIntLft;             // Temporary left arg integer
    APLUINT      ByteRes,               // # bytes in the result
                 uRes,                  // Loop counter
                 uRht,                  // Loop counter
                 uOdo;                  // Loop counter
    LPAPLDIM     lpMemDimRht = NULL;    // Ptr to right arg dimensions
    LPAPLUINT    lpMemAxisHead,         // Ptr to axis values, fleshed out
                 lpMemAxisTail = NULL,  // Ptr to grade up of AxisHead
                 lpMemOdoRht = NULL,    // Ptr to right arg odometer global memory
                 lpMemWVecRht = NULL;   // Ptr to right arg weighting vector global memory
    LPAPLINT     lpMemTmpLft,           // Ptr to left arg temporary values global memory
                 lpMemLoHiRht;          // Ptr to right arg low/high bounds
    UINT         uBitMask;              // Bit mask for looping through Booleans
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // If the right arg is scalar, the result rank is aplNELMLft,
    //   otherwise the result rank is aplRankRht
    if (aplRankRht EQ 0)
        aplRankRes = aplNELMLft;
    else
        aplRankRes = aplRankRht;

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRes,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            return NULL;
        // Lock the memory to get a ptr to it
        lpMemAxisHead = MyGlobalLock (hGlbAxis);

        // Get pointer to the axis tail (where the [X] values are)
        lpMemAxisTail = &lpMemAxisHead[aplRankRes - aplNELMAxis];
    } else
        // No axis is the same as all axes
        aplNELMAxis = aplRankRes;

    // Check for RANK error
    if (aplRankLft > 1)
    {
        ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Check for LENGTH error
    if (aplNELMLft NE aplNELMAxis)
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Check for DOMAIN error
    if (!IsSimpleNum (aplTypeLft))
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Get left & right arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Split off case of {zilde}{drop} SimpleScalar
    //               and      ''{drop} SimpleScalar
    if (IsSimple (aplTypeLft)
     && aplNELMLft EQ 0
     && IsSimple (aplTypeRht))
    {
        // Allocate a new YYRes;
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = TranslateArrayTypeToImmType (aplTypeRht);
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongestRht;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto NORMAL_EXIT;
    } // End IF

    // From here on, the result is a global

    // If the right arg is not immediate, ...
    if (lpMemRht)
    {
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

        // Skip over the header and dimensions to the data
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);
    } // End IF

    // lpMemRht now points to its data

    // Run through the left arg converting Booleans/floats
    //   to integers,  checking floats for near-integers
    //   calculating the result NELM as x/aplNELMLft, and
    //   saving the resulting normalized left arg in global
    //   memory as signed integers.
    hGlbTmpLft =
      PrimFnDydUpDownArrowLftGlbValid_EM (NULL,             // Ptr to common NELM
                                         &aplNELMRes,       // Ptr to result NELM
                                          aplRankRes,       // Result rank
                                          lpMemLft,         // Ptr to left arg global memory (may be NULL if immediate)
                                          aplTypeLft,       // Left arg storage type
                                          aplLongestLft,    // Left arg immediate value
                                          lpMemDimRht,      // Ptr to right arg global memory
                                          aplRankRht,       // Right arg rank
                                          lpMemAxisTail,    // Ptr to axis global memory (may be NULL if axis not present)
                                          TRUE,             // TRUE iff it's DownArrow
                                          lptkFunc);        // Ptr to function token
    if (!hGlbTmpLft)
        goto ERROR_EXIT;

    // The storage type of the result is the same as that of the right arg
    //   unless the right arg is hetero and the result is a singleton, or
    //   the right arg is APA
    if (aplTypeRht EQ ARRAY_HETERO
     && aplNELMRes EQ 1)
        aplTypeRes = TranslateImmTypeToArrayType ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType);
    else
    if (aplTypeRht EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result.
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
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->Perm       = 0;   // Already zero from GHND
////lpHeader->SysVar     = 0;   // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header and dimensions to the data
    if (lpMemLft)
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    else
        lpMemLft = &aplLongestLft;

    // Lock the memory to get a ptr to it
    lpMemTmpLft = MyGlobalLock (hGlbTmpLft);

    // Get ptr to low/high bounds
    lpMemLoHiRht = &lpMemTmpLft[aplRankRes];

    // Copy the left arg values to the result dimensions
    if (aplRankRht > 0)
    {
        for (uRes = 0; uRes < aplRankRes; uRes++)
        {
            aplIntLft = lpMemDimRht[uRes] - abs64 (lpMemTmpLft[uRes]);
            *((LPAPLDIM) lpMemRes)++ = max (0, aplIntLft);
        } // End FOR
    } else
    {
        for (uRes = 0; uRes < aplRankRes; uRes++)
        {
            aplIntLft = 1                 - abs64 (lpMemTmpLft[uRes]);
            *((LPAPLDIM) lpMemRes)++ = max (0, aplIntLft);
        } // End FOR
    } // End IF/ELSE

    // lpMemRes now points to its data

    // Handle empty array results
    if (aplNELMRes EQ 0)
    {
        // If the result is nested, copy the prototype from the right arg
        if (aplTypeRes EQ ARRAY_NESTED)
            *((LPAPLNESTED) lpMemRes) = *(LPAPLNESTED) lpMemRht;
        goto YYALLOC_EXIT;
    } // End IF

    // Copy the values from the right arg to the result
    //   taking into account
    //   1.  scalar right arg extension
    //   2.  negative values in the left arg
    //   3.  axis operator

    //***************************************************************
    // Allocate space for the right arg weighting vector which is
    //   {times}{backscan}1{drop}({rho}R),1
    // N.B.  Conversion from APLUINT to UINT.
    //***************************************************************
    ByteRes = aplRankRes * sizeof (APLUINT);
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbWVecRht = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbWVecRht)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemWVecRht = MyGlobalLock (hGlbWVecRht);

    // Loop through the dimensions of the right arg in reverse
    //   order {backscan} and compute the cumulative product
    //   (right arg weighting vector).
    // Note we use a signed index variable because we're
    //   walking backwards and the test against zero must be
    //   made as a signed variable.
    uRes = 1;
    if (lpMemDimRht)
    for (iRht = aplRankRes - 1; iRht >= 0; iRht--)
    {
        lpMemWVecRht[iRht] = uRes;
        uRes *= lpMemDimRht[iRht];
    } // End IF/FOR

    //***************************************************************
    // Allocate space for the right arg odometer array, one value per dimension
    //   in the right arg.
    // N.B.  Conversion from APLUINT to UINT.
    //***************************************************************
    ByteRes = aplRankRes * sizeof (APLUINT);
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbOdoRht = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbOdoRht)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        goto ERROR_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemOdoRht = MyGlobalLock (hGlbOdoRht);

    // Initialize the odometer array
    CopyMemory (lpMemOdoRht, lpMemLoHiRht, (UINT) aplRankRes * sizeof (APLUINT));

    // If the right arg is an APA, ...
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // If the right arg is an immediate, ...
    if (lpMemRht EQ NULL)
        lpMemRht = &aplLongestRht;

    // Loop through the result/right arg copying values
    //   common to the two from the right arg to the result
    for (uRes = 0; uRes < aplNELMRes; uRes++)
    {
        // Use the index in lpMemOdoRht to calculate the
        //   corresponding index in lpMemRht from which
        //   the next value comes.
        for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
            uRht += lpMemOdoRht[uOdo] * lpMemWVecRht[uOdo];

        // Increment the odometer in lpMemOdoRht subject to
        //   the values in lpMemLoHiRht
        IncrLoHiOdometer (lpMemOdoRht, lpMemLoHiRht, aplRankRes);

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                uBitMask = 1 << (MASKLOG2NBIB & (UINT) uRht);

                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLBOOL)   lpMemRes)[uRes >> LOG2NBIB] |=
                ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? 1 : 0) << (((UINT) uRes) & MASKLOG2NBIB);

                break;

            case ARRAY_INT:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLINT)    lpMemRes)[uRes] = ((LPAPLINT) lpMemRht)[uRht];

                break;

            case ARRAY_FLOAT:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLFLOAT)  lpMemRes)[uRes] = ((LPAPLFLOAT) lpMemRht)[uRht];

                break;

            case ARRAY_APA:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLINT)    lpMemRes)[uRes] = apaOffRht + apaMulRht * uRht;;

                break;

            case ARRAY_CHAR:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLCHAR)   lpMemRes)[uRes] = ((LPAPLCHAR) lpMemRht)[uRht];

                break;

            case ARRAY_NESTED:
            case ARRAY_HETERO:
                // Copy element # uRht from the right arg to lpMemRes[uRes]
                ((LPAPLNESTED) lpMemRes)[uRes] = CopySymGlbDir (((LPAPLNESTED) lpMemRht)[uRht]);

                break;

            defstop
                break;
        } // End SWITCH
    } // End FOR
YYALLOC_EXIT:
    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    lpYYRes->tkToken = *TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
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

    if (hGlbWVecRht)
    {
        if (lpMemWVecRht)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbWVecRht); lpMemWVecRht = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbWVecRht); hGlbWVecRht = NULL;
    } // End IF

    if (hGlbOdoRht)
    {
        if (lpMemOdoRht)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbOdoRht); lpMemOdoRht = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbOdoRht); hGlbOdoRht = NULL;
    } // End IF

    if (hGlbAxis)
    {
        if (lpMemAxisHead)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbAxis); lpMemAxisHead = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
    } // End IF

    if (hGlbTmpLft)
    {
        if (lpMemTmpLft)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTmpLft); lpMemTmpLft = NULL;
        } // End IF

        // We no longer need this storage
        DbgGlobalFree (hGlbTmpLft); hGlbTmpLft = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDownArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $IncrHiLoOdometer
//
//  Increment the odometer in lpMemOdo subject to the
//    limits in lpMemLoHi of length <aplRank>.
//***************************************************************************

void IncrLoHiOdometer
    (LPAPLUINT lpMemOdo,    // Ptr to the values to increment
     LPAPLDIM  lpMemLoHi,   // Ptr to the limits on each dimension
     APLRANK   aplRank)     // # elements to which each of the
                            //   ptrs point

{
    LPAPLDIM lpMemLo,       // Ptr to low bounds of LoHi
             lpMemHi;       // Ptr to high ...
    APLINT iOdo;            // Loop counter

    // Get the separate ptrs
    lpMemLo =  lpMemLoHi;
    lpMemHi = &lpMemLoHi[aplRank];

    // Loop through the odometer values from right to left
    // Note we use a signed index variable because we're
    //   walking backwards and the test against zero must be
    //   made as a signed variable.
    for (iOdo = aplRank - 1; iOdo >= 0; iOdo--)
    if (++lpMemOdo[iOdo] EQ lpMemHi[iOdo])
        lpMemOdo[iOdo] = lpMemLo[iOdo];
    else
        break;
} // End IncrLoHiOdometer


//***************************************************************************
//  $PrimFnDydUpDownArrowLftGlbValid_EM
//
//  Dyadic Up/DownArrow left argument validation on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydUpDownArrowLftGlbValid_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL PrimFnDydUpDownArrowLftGlbValid_EM
    (LPAPLNELM  lpaplNELMCom,           // Ptr to common NELM (may be NULL if DownArrow)
     LPAPLNELM  lpaplNELMRes,           // Ptr to result NELM
     APLRANK    aplRankRes,             // Result rank
     LPVOID     lpMemLft,               // Ptr to left arg global memory (may be NULL if immediate)
     APLSTYPE   aplTypeLft,             // Left arg storage type
     APLLONGEST aplLongestLft,          // Left arg immediate value
     LPAPLDIM   lpMemDimRht,            // Ptr to right arg dimensions global memory
     APLRANK    aplRankRht,             // Right arg rank
     LPAPLUINT  lpMemAxisTail,          // Ptr to axis global memory (may be NULL if axis not present)
     BOOL       bDownArrow,             // TRUE iff it's DownArrow
     LPTOKEN    lptkFunc)               // Ptr to function token

{
    LPVOID   lpDataLft;                 // Ptr to left arg data
    APLNELM  aplNELMLft;                // Left arg NELM
    APLRANK  aplRankLft;                // Left arg rank
    BOOL     bRet = TRUE;               // TRUE iff result is valid
    APLUINT  uDim,                      // Loop counter
             uRes;                      // Loop counter
    UINT     uBitMask;                  // Bit mask for looping through Booleans
    APLINT   aplIntTmp,                 // Temporary integer
             aplIntLft;                 // Temporary left arg integer
    HGLOBAL  hGlbTmpLft;                // Left arg temporary values global memory handle
    LPAPLINT lpMemTmpLft,               // Ptr to left arg temporary values global memory
             lpMemTmpLoRht,             // ...    right arg low bound
             lpMemTmpHiRht,             // ...              high bound
             lpMemTmpLoRes,             // ...    result    low bound
             lpMemTmpHiRes;             // ...              high bound

    // If the left arg is not immediate, ...
    if (lpMemLft)
    {
#define lpHeaderLft     ((LPVARARRAY_HEADER) lpMemLft)
////////aplTypeLft = lpHeaderLft->ArrType;      // Already passed as argument
        aplNELMLft = lpHeaderLft->NELM;
        aplRankLft = lpHeaderLft->Rank;
#undef  lpHeaderLft
        // Point to the left arg data
        lpDataLft = VarArrayBaseToData (lpMemLft, aplRankLft);
    } else
    {
        aplNELMLft = 1;
        aplRankLft = 0;

        // Point to the left arg data
        lpDataLft = &aplLongestLft;
    } // End IF/ELSE

    // Allocate space for a normalized left arg
    // Three copies for DownArrow (TmpLft, LoRht, HiRht),
    //   five for UpArrow (TmpLft, LoRht, HiRht, LoRes, HiRes)
    hGlbTmpLft = DbgGlobalAlloc (GHND, (UINT) ((5 - bDownArrow * 2) * aplRankRes * sizeof (APLINT)));
    if (!hGlbTmpLft)
        return NULL;

    // Lock the memory to get a ptr to it
    lpMemTmpLft  = MyGlobalLock (hGlbTmpLft);
    lpMemTmpLoRht = &lpMemTmpLft   [aplRankRes];
    lpMemTmpHiRht = &lpMemTmpLoRht [aplRankRes];
    if (!bDownArrow)
    {
        lpMemTmpLoRes = &lpMemTmpHiRht[aplRankRes];
        lpMemTmpHiRes = &lpMemTmpLoRes[aplRankRes];
    } // End IF

    // If there's an axis operator, and
    //   the right arg is not scalar, and
    //   it's UpArrow
    if (lpMemAxisTail
     && aplRankRht > 0
     && !bDownArrow)
        // Copy the right arg dimensions to the normalized left arg
        //   some of which will be overwritten by the actual left arg,
        //   the rest of which are the elided dimensions and hence
        //   are unchanged from the right arg to the result.
        CopyMemory (lpMemTmpLft, lpMemDimRht, (UINT) aplRankRht * sizeof (APLDIM));

    // Check for LEFT DOMAIN ERROR and fill in lpMemTmpLft
    // Split cases based upon the left arg's storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:
            uBitMask = 0x01;

            for (uDim = 0; bRet && uDim < aplNELMLft; uDim++)
            {
                // Get the left arg value
                aplIntLft = (uBitMask & *(LPAPLBOOL) lpDataLft) ? 1 : 0;

                // If there's an axis, ...
                if (lpMemAxisTail)
                    uRes = lpMemAxisTail[uDim];
                else
                    uRes = uDim;

                // Save in the normalized global memory
                lpMemTmpLft[uRes] = aplIntLft;

                // Shift over the left bit mask
                uBitMask <<= 1;

                // Check for end-of-byte
                if (uBitMask EQ END_OF_BYTE)
                {
                    uBitMask = 0x01;            // Start over
                    ((LPAPLBOOL) lpDataLft)++;  // Skip to next byte
                } // End IF
            } // End FOR

            break;

        case ARRAY_INT:
            for (uDim = 0; bRet && uDim < aplNELMLft; uDim++)
            {
                // Get the left arg value
                aplIntLft = ((LPAPLINT) lpDataLft)[uDim];

                // If there's an axis, ...
                if (lpMemAxisTail)
                    uRes = lpMemAxisTail[uDim];
                else
                    uRes = uDim;

                // Save in the normalized global memory
                lpMemTmpLft[uRes] = aplIntLft;
            } // End FOR

            break;

        case ARRAY_FLOAT:
            for (uDim = 0; bRet && uDim < aplNELMLft; uDim++)
            {
                aplIntLft = FloatToAplint_SCT (((LPAPLFLOAT) lpDataLft)[uDim], &bRet);
                if (bRet)
                {
                    // If there's an axis, ...
                    if (lpMemAxisTail)
                        uRes = lpMemAxisTail[uDim];
                    else
                        uRes = uDim;

                    // Save in the normalized global memory
                    lpMemTmpLft[uRes] = aplIntLft;
                } // End IF/ELSE
            } // End FOR

            break;

        case ARRAY_APA:
        {
            APLINT  apaOffLft,
                    apaMulLft;

#define lpAPA       ((LPAPLAPA) lpDataLft)
            // Get the APA parameters
            apaOffLft = lpAPA->Off;
            apaMulLft = lpAPA->Mul;
#undef  lpAPA
            for (uDim = 0; bRet && uDim < aplNELMLft; uDim++)
            {
                aplIntLft = apaOffLft + apaMulLft * uDim;
                if (aplIntLft > MAX_APLNELM)
                    bRet = FALSE;
                else
                {
                    // If there's an axis, ...
                    if (lpMemAxisTail)
                        uRes = lpMemAxisTail[uDim];
                    else
                        uRes = uDim;

                    // Save in the normalized global memory
                    lpMemTmpLft[uRes] = aplIntLft;
                } // End IF/ELSE
            } // End FOR

            break;
        } // End ARRAY_APA

        case ARRAY_CHAR:
        case ARRAY_NESTED:          // We could check for promotion to simple numeric
        case ARRAY_HETERO:          // ...
            bRet = FALSE;

            break;

        defstop
            break;
    } // End SWITCH

    if (bRet)
    {
        // Initialize with identity element for multiply
        *lpaplNELMRes = 1;
        if (lpaplNELMCom)
            *lpaplNELMCom = 1;

        // Loop through the normalized left arg
        //   and fill in *lpaplNELMRes, lpMemTmpLo, and lpMemTmpHi
        for (uDim = 0; bRet && uDim < aplRankRes; uDim++)
        {
            // Get the next value
            aplIntLft = abs64 (lpMemTmpLft[uDim]);
            if (aplRankRht > 0)
                aplIntTmp = lpMemDimRht[uDim];
            else
                aplIntTmp = 1;
            // If it's DownArrow, ...
            if (bDownArrow)
            {
                // If lpMemTmpLft[uDim] is negative, ...
                if (SIGN_APLNELM (lpMemTmpLft[uDim]))
                {
                    // The lower bound is ...
                    *lpMemTmpLoRht++ = 0;                               // Inclusive

                    // The upper bound is ...
                    *lpMemTmpHiRht++ = max (0, aplIntTmp - aplIntLft);  // Exclusive
                } else
                {
                    // The lower bound is ...
                    *lpMemTmpLoRht++ = aplIntLft;                       // Inclusive

                    // The upper bound is ...
                    *lpMemTmpHiRht++ = aplIntTmp;                       // Exclusive
                } // End IF/ELSE

                // Calculate the remaining dimension length
                if (aplRankRht > 0)
                    aplIntLft = aplIntTmp - aplIntLft;
                else
                    aplIntLft = 1 - aplIntLft;

                // The dimension is always >= 0 (handle overdrop)
                aplIntLft = max (0, aplIntLft);
            } else
            // It's UpArrow
            {
                // Calculate the common NELM
                *lpaplNELMCom *= min (aplIntLft, aplIntTmp);

                // If lpMemTmpLft[uDim] is negative, ...
                if (SIGN_APLNELM (lpMemTmpLft[uDim]))
                {
                    // The lower bound is ...
                    *lpMemTmpLoRht++ = max (0, aplIntTmp - aplIntLft);  // Inclusive

                    // The upper bound is ...
                    *lpMemTmpHiRht++ = aplIntTmp;                       // Exclusive

                    // The lower bound is ...
                    *lpMemTmpLoRes++ = max (0, aplIntLft - aplIntTmp);  // Inclusive

                    // The upper bound is ...
                    *lpMemTmpHiRes++ = aplIntLft;                       // Exclusive
                } else
                {
                    // The lower bound is ...
                    *lpMemTmpLoRht++ = 0;                               // Inclusive

                    // The upper bound is ...
                    *lpMemTmpHiRht++ = min (aplIntLft, aplIntTmp);      // Exclusive

                    // The lower bound is ...
                    *lpMemTmpLoRes++ = 0;

                    // The upper bound is ...
                    *lpMemTmpHiRes++ = min (aplIntTmp, aplIntLft);      // Exclusive
                } // End IF/ELSE
            } // End IF/ELSE

            // Multiply the two numbers as APLINTs so we can check for overflow
            aplIntTmp = imul64 (*lpaplNELMRes, aplIntLft, &bRet);

            // Ensure the value fits into a dimension
            bRet = bRet && (aplIntTmp <= MAX_APLDIM);

            // Save back
            *lpaplNELMRes = aplIntTmp;
        } // End FOR
    } // End IF

    // If error, it's a DOMAIN ERROR
    if (!bRet)
    {
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
        if (hGlbTmpLft)
        {
            if (lpMemTmpLft)
            {
                // We no longer need this ptr
                MyGlobalUnlock (hGlbTmpLft); lpMemTmpLft = NULL;
            } // End IF

            // We no longer need this storage
            DbgGlobalFree (hGlbTmpLft); hGlbTmpLft = NULL;
        } // End IF
    } else
    if (lpMemTmpLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTmpLft); lpMemTmpLft = NULL;
    } // End IF

    return hGlbTmpLft;
} // End PrimFnDydUpDownArrowLftGlbValid_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_darrow.c
//***************************************************************************
