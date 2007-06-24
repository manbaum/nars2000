//***************************************************************************
//  NARS2000 -- Primitive Function -- CircleStile
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
//  $PrimFnCircleStile_EM_YY
//
//  Primitive function for monadic and dyadic CircleStile ("reverse" and "rotate")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnCircleStile_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnCircleStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_CIRCLESTILE
         || lptkFunc->tkData.tkChar EQ UTF16_CIRCLEBAR);

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

LPYYSTYPE PrimProtoFnCircleStile_EM_YY
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

LPYYSTYPE PrimFnMonCircleStile_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE   aplTypeRht,
               aplTypeRes;
    APLNELM    aplNELMRht;
    APLRANK    aplRankRht;
    HGLOBAL    hGlbRht = NULL,
               hGlbRes = NULL;
    LPVOID     lpMemRht = NULL,
               lpMemRes = NULL;
    LPAPLDIM   lpMemDimRht;
    APLUINT    aplAxis,     // The (one and only) axis value
               uDim,
               uLo,
               uDimLo,
               uAx,
               uDimAx,
               uHi,
               uDimHi,
               uRes,
               uRht;
    APLINT     apaOff,
               apaMul;
    LPYYSTYPE  lpYYRes;         // Ptr to the result
    UINT       ByteRes,         // # bytes to allocate for the result
               uBitMask,
               uBitIndex;

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Check for list
    if (aplTypeRht EQ ARRAY_LIST)
    {
        ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                   lptkFunc);
        return NULL;
    } // End IF

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
                           &aplAxis,        // Return last axis value
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
    if (aplNELMRht EQ 1)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Copy the right arg token
        lpYYRes->tkToken = *CopyToken_EM (lptkRhtArg, FALSE);

        goto IMMED_EXIT;
    } // End IF

    // The type of the result is the same as the
    //   type of the right arg except APA -> INT
    if (aplTypeRht EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Calculate space needed for the result
    ByteRes = (UINT) CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    // Allocate storage for the result
    hGlbRes = DbgGlobalAlloc (GHND, ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc);
        return NULL;
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
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;

#undef  lpHeader

    // Get right arg's global ptrs
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);
    lpMemRht = VarArrayBaseToDim (lpMemRht);
    lpMemDimRht = (LPAPLDIM) lpMemRht;

    // Fill in the dimensions
    for (uDim = 0; uDim < aplRankRht; uDim++)
        *((LPAPLDIM) lpMemRes)++ = *((LPAPLDIM) lpMemRht)++;

    // Copy the data to the result

    // Handle prototypes
    if (aplNELMRht EQ 0)
    {
        if (aplTypeRht EQ ARRAY_NESTED)
            *((LPAPLNESTED) lpMemRes) = CopySymGlbInd (lpMemRht);
        goto NORMAL_EXIT;
    } // End IF

    if (aplRankRht EQ 0)
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
                    uRes = uDim +                 uAx  * uDimHi;
                    uRht = uDim + ((uDimAx - 1) - uAx) * uDimHi;
                    uBitMask  = 1 << (UINT) (uRht % NBIB);
                    uBitIndex = (UINT) (uRes % NBIB);
                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                    ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? 1 : 0) << uBitIndex;
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
                    ((LPAPLINT) lpMemRes)[uDim +                 uAx  * uDimHi] =
                    ((LPAPLINT) lpMemRht)[uDim + ((uDimAx - 1) - uAx) * uDimHi];
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                    ((LPAPLFLOAT) lpMemRes)[uDim +                 uAx  * uDimHi] =
                    ((LPAPLFLOAT) lpMemRht)[uDim + ((uDimAx - 1) - uAx) * uDimHi];
            } // End FOR/FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                    ((LPAPLCHAR) lpMemRes)[uDim +                 uAx  * uDimHi] =
                    ((LPAPLCHAR) lpMemRht)[uDim + ((uDimAx - 1) - uAx) * uDimHi];
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
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                    ((LPAPLINT) lpMemRes)[uDim +                 uAx  * uDimHi] =
                       apaOff + apaMul * (uDim + ((uDimAx - 1) - uAx) * uDimHi);
            } // End FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                    ((LPAPLNESTED) lpMemRes)[uDim +                 uAx  * uDimHi] = CopySymGlbDir
                   (((LPAPLNESTED) lpMemRht)[uDim + ((uDimAx - 1) - uAx) * uDimHi]);
            } // End FOR/FOR

            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

NORMAL_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
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

LPYYSTYPE PrimFnDydCircleStile_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE  aplTypeLft,
              aplTypeRht,
              aplTypeRes;
    APLNELM   aplNELMLft,
              aplNELMRht;
    APLRANK   aplRankLft,
              aplRankRht;
    HGLOBAL   hGlbLft = NULL,
              hGlbRht = NULL,
              hGlbRes = NULL,
              hGlbRot = NULL;
    LPAPLDIM  lpMemDimLft,
              lpMemDimRht;
    LPVOID    lpMemLft = NULL,
              lpMemRht = NULL,
              lpMemRes = NULL;
    LPAPLINT  lpMemRot = NULL;
    BOOL      bRet = TRUE;
    APLUINT   aplAxis,      // The (one and only) axis value
              ByteRes,
              uLo,
              uDimLo,
              uAx,
              uDimAx,
              uHi,
              uDimHi,
              uDim,
              uRes,
              uRht;
    APLINT    aplInteger,
              apaOff,
              apaMul,
              aplRot;
    APLFLOAT  aplFloat;
    LPYYSTYPE lpYYRes;
    UINT      uBitMask,
              uBitIndex;

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

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
                           &aplAxis,        // Return last axis value
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
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Scalar left arg matches everything
    if (aplRankLft NE 0)
    {
        // Check for RANK ERROR
        if (!(aplRankLft EQ 1 && aplNELMLft NE 1 && aplRankRht EQ 1)
         && (aplRankRht - aplRankLft) NE 1)
        {
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkLftArg);
            goto ERROR_EXIT;
        } // End IF

        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);
        lpMemDimLft = VarArrayBaseToDim (lpMemLft);

        // Check for LENGTH ERROR
        for (uDim = 0; uDim < aplRankRht; uDim++)
        if (uDim NE aplAxis
         && lpMemDimLft[uDim] NE lpMemDimRht[uDim + (uDim < aplAxis)])
        {
            ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                                       lptkLftArg);
            goto ERROR_EXIT;
        } // End IF
    } // End IF

    // Check for DOMAIN ERROR
    if (!IsSimpleNum (aplTypeLft))
        goto DOMAIN_EXIT;

    // If the left arg is singleton, get its value
    if (aplNELMLft EQ 1)
    {
        // Get the integer or float value
        FirstValue (lptkLftArg,         // Ptr to right arg token
                   &aplInteger,         // Ptr to integer result
                   &aplFloat,           // Ptr to float ...
                    NULL,               // Ptr to WCHAR ...
                    NULL,               // Ptr to longest ...
                    NULL,               // Ptr to lpSym/Glb ...
                    NULL,               // Ptr to ...immediate type ...
                    NULL);              // Ptr to array type ...
        // Attempt to convert FLOAT left arg
        if (aplTypeLft EQ ARRAY_FLOAT)
        {
            aplInteger = FloatToAplint_SCT (aplFloat, &bRet);
            if (!bRet)
                goto DOMAIN_EXIT;
        } // End IF
    } else
    // Split cases based upon the left arg's storage type
    {
        // Copy left arg to temp storage (all APLINTs), and
        //   check the left arg for valid values

        // Allocate temp storage for the normalized left arg
        // N.B.: Conversion from APLUINT to UINT
        ByteRes = aplNELMLft * sizeof (APLINT);
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbRot = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbRot)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkLftArg);
            goto ERROR_EXIT;
        } // End IF
        // Lock the memory to get a ptr to it
        lpMemRot = MyGlobalLock (hGlbRot);

        switch (aplTypeLft)
        {
            case ARRAY_BOOL:
                uBitMask = 0x01;

                for (uDim = 0; uDim < aplNELMLft; uDim++)
                {
                    *lpMemRot++ = (uBitMask & *(LPAPLBOOL) lpMemLft) ? 1 : 0;

                    // Shift over the left bit mask
                    uBitMask <<= 1;

                    // Check for end-of-byte
                    if (uBitMask EQ END_OF_BYTE)
                    {
                        uBitMask = 0x01;            // Start over
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
                    aplInteger = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemLft)++, &bRet);
                    if (!bRet)
                        goto DOMAIN_EXIT;

                    *lpMemRot++ = aplInteger;
                } // End FOR

                break;

            case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemLft)
                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;
#undef  lpAPA
                for (uDim = 0; uDim < aplNELMLft; uDim++)
                    *lpMemRot++ = apaOff + apaMul * uDim;
                break;

            defstop
                break;
        } // End SWITCH

        // Restore lpMemRot to the start of the block
        MyGlobalUnlock (hGlbRot); lpMemRot = NULL;
        lpMemRot = MyGlobalLock (hGlbRot);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    // Map APA right arg to INT result
    if (aplTypeRht EQ ARRAY_APA)
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = aplTypeRht;

    // Strip out the simple scalar right argument case
    if (aplRankRht EQ 0 && IsSimpleNH (aplTypeRes))
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
                    Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                    // If we ever get here, we must have missed a type demotion
                    DbgStop ();
                } // End IF

                // Handle the immediate case

                // Fill in the result token
                lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                lpYYRes->tkToken.tkFlags.ImmType   = lptkRhtArg->tkData.tkSym->stFlags.ImmType;
////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
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
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;

#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);
    lpMemDimRht = lpMemRht = VarArrayBaseToDim (lpMemRht);

    if (aplRankRht EQ 0)
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
    if (aplNELMRht EQ 0 && aplTypeRes EQ ARRAY_NESTED)
        *((LPAPLNESTED) lpMemRes) = CopySymGlbInd (lpMemRht);
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
                if (aplNELMLft EQ 1)
                    aplRot = aplInteger;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                {
                    uRes = uDim +                  uAx           * uDimHi;
                    uRht = uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi;
                    uBitMask  = 1 << (UINT) (uRht % NBIB);
                    uBitIndex = (UINT) (uRes % NBIB);
                    ((LPAPLBOOL) lpMemRes)[uRes >> LOG2NBIB] |=
                    ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? 1 : 0) << uBitIndex;
                } // End FOR
            } // End FOR/FOR

            break;

        case ARRAY_INT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (aplNELMLft EQ 1)
                    aplRot = aplInteger;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                    ((LPAPLINT) lpMemRes)[uDim +                  uAx           * uDimHi] =
                    ((LPAPLINT) lpMemRht)[uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi];
            } // End FOR/FOR

            break;

        case ARRAY_FLOAT:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (aplNELMLft EQ 1)
                    aplRot = aplInteger;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                    ((LPAPLFLOAT) lpMemRes)[uDim +                  uAx           * uDimHi] =
                    ((LPAPLFLOAT) lpMemRht)[uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi];
            } // End FOR/FOR

            break;

        case ARRAY_CHAR:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (aplNELMLft EQ 1)
                    aplRot = aplInteger;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                    ((LPAPLCHAR) lpMemRes)[uDim +                  uAx           * uDimHi] =
                    ((LPAPLCHAR) lpMemRht)[uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi];
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
                if (aplNELMLft EQ 1)
                    aplRot = aplInteger;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                    ((LPAPLINT) lpMemRes)[uDim +                  uAx           * uDimHi] =
                       apaOff + apaMul * (uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi);
            } // End FOR/FOR

            break;

        case ARRAY_HETERO:
        case ARRAY_NESTED:
            // Loop through the right arg copying the data to the result
            for (uLo = 0; uLo < uDimLo; uLo++)
            for (uHi = 0; uHi < uDimHi; uHi++)
            {
                if (aplNELMLft EQ 1)
                    aplRot = aplInteger;
                else
                    aplRot = lpMemRot[uLo * uDimHi + uHi];
                uDim = uLo * uDimHi * uDimAx + uHi;
                for (uAx = 0; uAx < uDimAx; uAx++)
                    ((LPAPLNESTED) lpMemRes)[uDim +                  uAx           * uDimHi] = CopySymGlbDir
                   (((LPAPLNESTED) lpMemRht)[uDim + AplModI (uDimAx, uAx + aplRot) * uDimHi]);
            } // End FOR/FOR

            break;

        defstop
            break;
    } // End SWITCH

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
