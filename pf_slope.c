//***************************************************************************
//  NARS2000 -- Primitive Function -- Slope
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
        return PrimFnSyntaxError_EM (lptkFunc);

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
    return PrimFnValenceError_EM (lptkFunc);
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
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE     aplTypeLft,        // Left arg storage type
                 aplTypeRht,        // Right ...
                 aplTypeRes;        // Result   ...
    APLNELM      aplNELMLft,        // Left arg NELM
                 aplNELMRht,        // Right ...
                 aplNELMRes;        // Result ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht,        // Right ...
                 aplRankRes;        // Result   ...
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL,    // Result   ...
                 hGlbRep = NULL;    // Replicate ...
    LPAPLDIM     lpMemDimRht = NULL;// Ptr to right arg dimensions
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL,   // Ptr to right ...
                 lpMemRes = NULL;   // Ptr to result   ...
    LPAPLINT     lpMemRep = NULL;   // Ptr to replicate ...
    BOOL         bRet = TRUE;       // TRUE iff result is valid
    APLUINT      aplAxis,           // The (one and only) axis value
                 ByteRes,           //
                 uLo,               //
                 uDimLo,            //
                 uAx,               //
                 uDimAxRht,         //
                 uDimLftSum,        //
                 uHi,               //
                 uDimHi,            //
                 uDim,              //
                 uDimRht,           //
                 uDimRes,           //
                 uRes,              //
                 uRht,              //
                 uAcc,              //
                 uLen;              //
    APLINT       aplIntegerLft,     //
                 aplIntegerRht,     //
                 aplIntegerRep,     //
                 apaOff,            //
                 apaMul;            //
    APLFLOAT     aplFloatLft,       //
                 aplFloatRht,       //
                 aplFloatRep;       //
    APLCHAR      aplCharRht,        //
                 aplCharRep;        //
    APLNESTED    aplNestRht,        //
                 aplNestRep,        //
                 aplNestProto;      //
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    UINT         uBitMask,          //
                 uBitIndex;         //

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
            // Otherwise, it's SlopeBar on the first dimension
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

    // Check for DOMAIN ERROR
    if (!IsSimpleNum (aplTypeLft))
        goto DOMAIN_EXIT;

        // Skip over the header to the dimensions
    if (lpMemRht)
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Calculate product of dimensions before, at, and after the axis dimension
    if (aplRankRht EQ 0)
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
    if (aplNELMLft EQ 1)
    {
        // Get the integer or float value
        GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                           &aplIntegerLft,  // Ptr to integer result
                           &aplFloatLft,    // Ptr to float ...
                            NULL,           // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                            NULL,           // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
        // Attempt to convert FLOAT left arg
        if (aplTypeLft EQ ARRAY_FLOAT)
        {
            // Attempt to convert the float to an integer using System CT
            aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
            if (!bRet)
                goto DOMAIN_EXIT;
        } // End IF

        // Check the singleton arg
        if (!IsBooleanValue (aplIntegerLft))
            goto DOMAIN_EXIT;

        // Save as the sum of the values in the left arg
        uDimLftSum = aplIntegerLft;
    } else
    // Non-singleton left arg
    {
        // Copy left arg to temp storage (all APLINTs),
        //   check the left arg for valid values, and
        //   accumulate the sum (as uDimLftSum).

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
        uDimLftSum = 0;

        // Split cases based upon the left arg's storage type
        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
                uBitMask = 0x01;

                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    aplIntegerLft = (uBitMask & *(LPAPLBOOL) lpMemLft) ? 1 : 0;
                    uDimLftSum += aplIntegerLft;
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
                    if (!IsBooleanValue (aplIntegerLft))
                        goto DOMAIN_EXIT;
                    uDimLftSum += aplIntegerLft;

                    *lpMemRep++ = aplIntegerLft;
                } // End IF

                break;

            case ARRAY_FLOAT:
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    // Attempt to convert the float to an integer using System CT
                    aplIntegerLft = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemLft)++, &bRet);
                    if (!bRet || !IsBooleanValue (aplIntegerLft))
                        goto DOMAIN_EXIT;
                    uDimLftSum += aplIntegerLft;

                    *lpMemRep++ = aplIntegerLft;
                } // End FOR

                break;

            case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemLft)
                // Get the APA parameters
                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;
#undef  lpAPA
                // Check the first and last values
                if (!IsBooleanValue (apaOff)
                 || !IsBooleanValue (apaOff + apaMul * (aplNELMLft - 1)))
                    goto DOMAIN_EXIT;

                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    aplIntegerLft = apaOff + apaMul * uDim;
                    uDimLftSum += aplIntegerLft;
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
    } // End IF

    // Check for LENGTH ERROR
    // Scalar right arg matches everything
    if (aplRankRht NE 0
     && uDimLftSum NE lpMemDimRht[aplAxis])
    {
        ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                   lptkLftArg);
        goto ERROR_EXIT;
    } // End IF

    // Map APA right arg to INT result
    if (aplTypeRht EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Calculate the NELM of the result
    aplNELMRes = uDimLo * aplNELMLft * uDimHi;

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
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
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
    (VarArrayBaseToDim (lpMemRes))[aplAxis] = aplNELMLft;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayBaseToData (lpMemRes, aplRankRes);
    if (lpMemRht)
        lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Handle empty nested array results (prototypes)
    if (aplNELMRht EQ 0 && aplTypeRht EQ ARRAY_NESTED)
    {
        APLNELM aplNELM;

        // Empty nested results always have at least one element (the prototype)
        aplNELM = max (aplNELMRes, 1);

        for (uRes = 0; uRes < aplNELM; uRes++)
            *((LPAPLNESTED) lpMemRes)++ = CopySymGlbInd (lpMemRht);

        goto PROTO_EXIT;
    } // End IF

    // If the right arg is a singleton, get its value
    if (aplNELMRht EQ 1)
        GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                           &aplIntegerRht,  // Ptr to integer result
                           &aplFloatRht,    // Ptr to float ...
                           &aplCharRht,     // Ptr to WCHAR ...
                            NULL,           // Ptr to longest ...
                           &aplNestRht,     // Ptr to lpSym/Glb ...
                            NULL,           // Ptr to ...immediate type ...
                            NULL);          // Ptr to array type ...
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
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    uRes = uDimRes + uAx * uDimHi;
                    uBitIndex = (UINT) (uRes % NBIB);
                    if (uLen)
                    {
                        if (aplNELMRht EQ 1)
                            ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |= aplIntegerRht << uBitIndex;
                        else
                        {
                            if (lpMemRht)
                            {
                                uRht = uDimRht + uAcc++ * uDimHi;
                                uBitMask  = BIT0 << (UINT) (uRht % NBIB);
                                aplIntegerRep = (uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? 1 : 0;
                            } else
                                aplIntegerRep = aplIntegerRht;
                            ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |= aplIntegerRep << uBitIndex;
                        } // End IF/ELSE
////////////////////} else
////////////////////{
////////////////////    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |= 0;      // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (aplNELMRht EQ 1)
                            ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = aplIntegerRht;
                        else
                        {
                            if (lpMemRht)
                                aplIntegerRep = ((LPAPLINT) lpMemRht)[uDimRht + uAcc++ * uDimHi];
                            else
                                aplIntegerRep = aplIntegerRht;

                            ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = aplIntegerRep;
                        } // End IF/ELSE
////////////////////} else
////////////////////{
////////////////////    ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = 0;  // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < aplNELMLft; uAx++)
                {
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (aplNELMRht EQ 1)
                            ((LPAPLFLOAT) lpMemRes)[uDimRes + uAx * uDimHi] = aplFloatRht;
                        else
                        {
                            if (lpMemRht)
                                aplFloatRep = ((LPAPLFLOAT) lpMemRht)[uDimRht + uAcc++ * uDimHi];
                            else
                                aplFloatRep = aplFloatRht;

                            ((LPAPLFLOAT) lpMemRes)[uDimRes + uAx * uDimHi] = aplFloatRep;
                        } // End IF/ELSE
////////////////////} else
////////////////////{
////////////////////    ((LPAPLFLOAT) lpMemRes)[uDimRes + uAx * uDimHi] = 0;    // Already filled in by GHND
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
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (lpMemRht)
                            aplCharRep = ((LPAPLCHAR) lpMemRht)[uDimRht + uAcc++ * uDimHi];
                        else
                            aplCharRep = aplCharRht;

                        ((LPAPLCHAR) lpMemRes)[uDimRes + uAx * uDimHi] =
                        (aplNELMRht EQ 1) ? aplCharRht : aplCharRep;
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
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (aplNELMRht EQ 1)
                            ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = aplIntegerRht;
                        else
                            ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = apaOff + apaMul * (uDimRht + uAcc++ * uDimHi);
////////////////////} else
////////////////////{
////////////////////    ((LPAPLINT) lpMemRes)[uDimRes + uAx * uDimHi] = 0;  // Already filled in by GHND
                    } // End IF/ELSE
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Get the prototype of the right arg
            GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                               &aplIntegerRht,  // Ptr to integer result
                               &aplFloatRht,    // Ptr to float ...
                               &aplCharRht,     // Ptr to WCHAR ...
                                NULL,           // Ptr to longest ...
                               &aplNestRht,     // Ptr to lpSym/Glb ...
                                NULL,           // Ptr to ...immediate type ...
                                NULL);          // Ptr to array type ...
            aplNestProto =
              MakeMonPrototype_EM (aplNestRht,  // Proto arg handle
                                   lptkFunc,    // Ptr to function token
                                   MP_CHARS);   // CHARs allowed
            if (!aplNestProto)
                goto ERROR_EXIT;
            // Ensure its Ptr Type is marked as a global
            aplNestProto = MakePtrTypeGlb (aplNestProto);

            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDimRht = uLo * uDimHi * uDimAxRht  + uHi;
                uDimRes = uLo * uDimHi * aplNELMLft + uHi;
                for (uAcc = uAx = 0; uAx < uDimAxRht; uAx++)
                {
                    if (aplNELMLft EQ 1)
                        uLen = aplIntegerLft;
                    else
                        uLen = lpMemRep[uAx];
                    if (uLen)
                    {
                        if (aplNELMRht EQ 1)
                            ((LPAPLNESTED) lpMemRes)[uDimRes + uAx * uDimHi] = CopySymGlbDir (aplNestRht);
                        else
                        {
                            aplNestRep = ((LPAPLNESTED) lpMemRht)[uDimRht + uAcc++ * uDimHi];
                            ((LPAPLNESTED) lpMemRes)[uDimRes + uAx * uDimHi] = CopySymGlbDir (aplNestRep);
                        } // End IF/ELSE
                    } else
                        ((LPAPLNESTED) lpMemRes)[uDimRes + uAx * uDimHi] = CopySymGlbDir (aplNestProto);
                } // End FOR
            } // End FOR/FOR

            // We no longer need this storage
            FreeResultGlobalVar (ClrPtrTypeDirAsGlb (aplNestProto)); aplNestProto = NULL;

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
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
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
} // End PrimFnDydSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_slope.c
//***************************************************************************
