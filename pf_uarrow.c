//***************************************************************************
//  NARS2000 -- Primitive Function -- UpArrow
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
//  $PrimFnUpArrow_EM_YY
//
//  Primitive function for monadic and dyadic UpArrow ("first" and "take")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnUpArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnUpArrow_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_UPARROW);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonUpArrow_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydUpArrow_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnUpArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnUpArrow_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic UpArrow
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnUpArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnUpArrow_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnUpArrow_EM_YY,// Ptr to primitive function routine
                                    lptkLftArg,         // Ptr to left arg token
                                    lptkFunc,           // Ptr to function token
                                    lptkRhtArg,         // Ptr to right arg token
                                    lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnUpArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonUpArrow_EM_YY
//
//  Primitive function for monadic UpArrow (ERROR)
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonUpArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonUpArrow_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLLONGEST   aplLongest;
    HGLOBAL      lpSymGlb;
    UCHAR        immType;
    LPPL_YYSTYPE lpYYRes;

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

    // Extract the first item (or the prototype) from the right arg
    FirstValue (lptkRhtArg,         // Ptr to right arg token
                NULL,               // Ptr to integer result
                NULL,               // Ptr to float ...
                NULL,               // Ptr to WCHAR ...
               &aplLongest,         // Ptr to longest ...
               &lpSymGlb,           // Ptr to lpSym/Glb ...
               &immType,            // Ptr to ...immediate type ...
                NULL);              // Ptr to array type ...
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // If the first value is an array, ...
    if (lpSymGlb)
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDir (lpSymGlb);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
        lpYYRes->tkToken.tkFlags.ImmType   = immType;
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkLongest  = aplLongest;
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    return lpYYRes;
} // End PrimFnMonUpArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydUpArrowLftGlbValid_EM
//
//  Dyadic UpArrow left argument validation on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydUpArrowLftGlbValid_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnDydUpArrowLftGlbValid_EM
    (HGLOBAL   hGlbLft,                 // Left arg handle
     LPAPLNELM lpaplNELMRes,            // Ptr to result NELM
     LPTOKEN   lptkFunc)                // Ptr to function token

{
    LPVOID   lpMemLft,      // Ptr to left argument global memory
             lpDataLft;
    APLSTYPE aplTypeLft;
    APLNELM  aplNELMLft;
    APLRANK  aplRankLft;
    BOOL     bRet = TRUE;
    APLNELM  uNELM;
    APLUINT  u;
    UINT     uBits;
    APLINT   aplIntTmp;

    if (!hGlbLft)
        DbgBrk ();          // ***FIXME*** -- Could be immediate (hGlbLft EQ NULL)

    // Lock the memory to get a ptr to it
    lpMemLft = MyGlobalLock (hGlbLft);

#define lpHeaderLft     ((LPVARARRAY_HEADER) lpMemLft)
    aplTypeLft = lpHeaderLft->ArrType;
    aplNELMLft = lpHeaderLft->NELM;
    aplRankLft = lpHeaderLft->Rank;
#undef  lpHeaderLft

    // Point to the left arg's data
    lpDataLft = VarArrayBaseToData (lpMemLft, aplRankLft);

    // Check for LEFT DOMAIN ERROR and fill in *lpaplNELMRes
    // Split cases based upon the left arg's storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:
            // The result's NELM is either zero or one
            *lpaplNELMRes = 1;

            uNELM = aplNELMLft >> LOG2NBIB;
            for (u = 0; *lpaplNELMRes && u < uNELM; u++) // char at a time, except for the last byte
                *lpaplNELMRes = ((LPAPLBOOL) lpDataLft)[u] EQ 0xFF;

            // Check the last byte

            // # valid bits in last byte
            uBits = (UINT) (aplNELMLft & MASKLOG2NBIB);
            if (uBits)                                  // Last byte not full
                *lpaplNELMRes &= ((LPAPLBOOL) lpDataLft)[u] EQ ((1 << uBits) - 1);

            break;

        case ARRAY_INT:
            *lpaplNELMRes = 1;      // Initialize with identity element for multiply
            for (u = 0; bRet && u < aplNELMLft; u++)
            {
                // Multiply the two numbers as APLINTs so we can check for overflow
                aplIntTmp = imul64 (*lpaplNELMRes, ((LPAPLINT) lpDataLft)[u], &bRet);

                // Ensure the value fits into a dimension
                bRet = bRet && (((LPAPLINT) lpDataLft)[u] <= MAX_APLDIM);

                // Save back
                *lpaplNELMRes = aplIntTmp;
            } // End FOR

            break;

        case ARRAY_FLOAT:
            *lpaplNELMRes = 1;      // Initialize with identity element for multiply
            for (u = 0; bRet && u < aplNELMLft; u++)
            {
                aplIntTmp = FloatToAplint_SCT (((LPAPLFLOAT) lpDataLft)[u], &bRet);
                if (bRet)
                {
                    // Multiply the two numbers as APLINTs so we can check for overflow
                    aplIntTmp = imul64 (*lpaplNELMRes, ((LPAPLINT) lpDataLft)[u], &bRet);

                    // Ensure the value fits into a dimension
                    bRet = bRet && (aplIntTmp <= MAX_APLDIM);

                    // Save back
                    *lpaplNELMRes = aplIntTmp;
                } // End IF/ELSE
            } // End FOR

            break;

        case ARRAY_APA:
        {
            APLINT  apaOff,
                    apaMul,
                    apaVal;

            // Save the APA data
#define lpAPA       ((LPAPLAPA) lpDataLft)
            apaOff = lpAPA->Off;
            apaMul = lpAPA->Mul;
#undef  lpAPA
            *lpaplNELMRes = 1;      // Initialize with identity element for multiply
            for (u = 0; bRet && u < aplNELMLft; u++)
            {
                apaVal = apaOff + apaMul * u;
                if (apaVal > MAX_APLNELM)
                    bRet = FALSE;
                else
                {
                    // Multiply the two numbers as APLINTs so we can check for overflow
                    aplIntTmp = imul64 (((APLINT) *lpaplNELMRes), apaVal, &bRet);

                    // Ensure the value fits into a dimension
                    bRet = bRet && (aplIntTmp <= MAX_APLDIM);

                    // Save back
                    *lpaplNELMRes = aplIntTmp;
                } // End IF/ELSE
            } // End FOR

            break;
        } // End ARRAY_APA

        case ARRAY_CHAR:
        case ARRAY_NESTED:          // We could check for promotion to simple numeric
        case ARRAY_HETERO:          // ...
        case ARRAY_LIST:
            bRet = FALSE;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbLft); lpMemLft = NULL;

    // NELM is always non-negative
    *lpaplNELMRes = abs64 ((APLINT) *lpaplNELMRes);

    // If error, it's a DOMAIN ERROR
    if (!bRet)
        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                                   lptkFunc);
    return bRet;
} // End PrimFnDydUpArrowLftGlbValid_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydUpArrow_EM_YY
//
//  Primitive function for dyadic UpArrow ("take")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydUpArrow_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydUpArrow_EM_YY
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
                 aplNELMRes,        // Result   ...
                 aplNELMAxis;       // Axis     ...
    APLRANK      aplRankLft,        // Left arg rank
                 aplRankRht,        // Right ...
                 aplRankRes;        // Result   ...
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRht = NULL,    // Right ...
                 hGlbRes = NULL,    // Result   ...
                 hGlbAxis = NULL;   // Axis     ...
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL,   // Ptr to right ...
                 lpMemRes = NULL;   // Ptr to result   ...
    APLUINT      ByteRes;           // # bytes needed for the result
    LPAPLUINT    lpMemAxisHead,     // Ptr to axis values, fleshed out
                 lpMemAxisTail;     // Ptr to grade up of AxisHead
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result

    return PrimFnNonceError_EM (lptkFunc);

    // Get the attributes (Type, NELM, and Rank)
    //   of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // If the right arg is scalar, the result rank is aplNELMLft,
    //   otherwise the result rank is aplRankRht
    if (aplRankRht EQ 0)
        aplRankRes = aplNELMLft;
    else
        aplRankRes = aplRankRht;

    // The storage type of the result is the same as that of the right arg
    aplTypeRes = aplTypeRht;

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

    // Run through the left arg converting Booleans/floats
    //   to integers, and checking floats for near-integers
    if (PrimFnDydUpArrowLftGlbValid_EM (hGlbLft,
                                       &aplNELMRes,
                                        lptkFunc))
        goto ERROR_EXIT;

    // Calculate the space needed for the result
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
    lpHeader->NELM       = 0;
    lpHeader->Rank       = aplRankRes;

#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Skip over the header and dimensions to the data
    if (lpMemLft)
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);





    DbgBrk ();          // ***FINISHME*** -- PrimFnDydUpArrow_EM_YY


















    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

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

    return lpYYRes;
} // End PrimFnUpArrow_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_uarrow.c
//***************************************************************************
