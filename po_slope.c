//***************************************************************************
//  NARS2000 -- Primitive Operator -- Slope
//***************************************************************************

#define STRICT
#include <windows.h>

#include "main.h"
#include "aplerrors.h"
#include "resdebug.h"
#include "externs.h"
#include "pertab.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "compro.h"
#endif

extern FASTBOOLFNS FastBoolFns[];   // ***FIXME*** -- move into externs.h


//***************************************************************************
//  $PrimOpSlope_EM_YY
//
//  Primitive operator for monadic and dyadic derived functions from
//    monadic operator Slope ("scan" and "ERROR")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpSlope_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    Assert (lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_OPSLOPE
         || lpYYFcnStr->tkToken.tkData.tkChar EQ INDEX_OPSLOPEBAR);

    // Split cases based upon monadic or dyadic derived function
    if (lptkLftArg EQ NULL)
        return PrimOpMonSlope_EM_YY (lpYYFcnStr,    // Ptr to operator function strand
                                     lptkRhtArg,    // Ptr to right arg
                                     lptkAxis);     // Ptr to axis token (may be NULL)
    else
        return PrimOpDydSlope_EM_YY (lptkLftArg,    // Ptr to left arg token
                                     lpYYFcnStr,    // Ptr to operator function strand
                                     lptkRhtArg,    // Ptr to right arg token
                                     lptkAxis);     // Ptr to axis token (may be NULL)
} // End PrimOpSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOpSlope_EM_YY
//
//  Generate a prototype for the derived functions from
//    monadic operator Slope ("scan" and "ERROR")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOpSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimProtoOpSlope_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token
     LPYYSTYPE lpYYFcnStrOpr,       // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    LPYYSTYPE lpYYFcnStrLft;        // Ptr to left operand function strand
    LPPRIMFNS lpPrimProtoLft;       // Ptr to left operand prototype function

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

    // Get a ptr to the prototype function for the first symbol (a function or operator)
    lpPrimProtoLft = PrimProtoFnsTab[SymTrans (&lpYYFcnStrLft->tkToken)];
    if (!lpPrimProtoLft)
    {
        ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                                  &lpYYFcnStrLft->tkToken);
        return NULL;
    } // End IF

    // If left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimOpMonSlopeCommon_EM_YY (lpYYFcnStrOpr,   // Ptr to operator function strand
                                           lptkRhtArg,      // Ptr to right arg token
                                           lptkAxis,        // Ptr to axis token (may be NULL)
                                           lpPrimProtoLft); // Ptr to left operand prototype function
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************
        return PrimFnSyntaxError_EM (&lpYYFcnStrOpr->tkToken);
} // End PrimProtoOpSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonSlope_EM_YY
//
//  Primitive operator for monadic derived function from Slope ("scan")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonSlope_EM_YY
    (LPYYSTYPE lpYYFcnStrOpr,       // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    return PrimOpMonSlopeCommon_EM_YY (lpYYFcnStrOpr,       // Ptr to operator function strand
                                       lptkRhtArg,          // Ptr to right arg token
                                       lptkAxis,            // Ptr to axis token (may be NULL)
                                       NULL);               // Ptr to left operand function strand
} // End PrimOpMonSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpMonSlopeCommon_EM_YY
//
//  Primitive operator for monadic derived function from Slope ("scan")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpMonSlopeCommon_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpMonSlopeCommon_EM_YY
    (LPYYSTYPE lpYYFcnStrOpr,       // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis,            // Ptr to axis token (may be NULL)
     LPPRIMFNS lpPrimProtoLft)      // Ptr to left operand prototype strand

{
    APLSTYPE    aplTypeRht,         // Right arg storage type
                aplTypeRes;         // Result    ...
    APLNELM     aplNELMRht,         // Right arg NELM
                aplNELMRes;         // Result    ...
    APLRANK     aplRankRht,         // Right arg rank
                aplRankRes;         // Result    ...
    APLUINT     aplAxis,            // The (one and only) axis value
                uLo,                // uDimLo loop counter
                uHi,                // uDimHi ...
                uDim,
                uDimLo,             // Product of dimensions below axis
                uDimHi,             // ...                   above ...
                uDimRht,            // Starting index in right arg of current vector
////            uDimRes,            // ...               result    ...
                uDimAxRht,          // Right arg axis dimension
                uRht,               // Right arg loop counter
                uRes,               // Result loop counter
                ByteRes;            // # bytes in the result
    APLINT  ////iDim,               // Integer dimension loop counter
                apaOffRht,          // Right arg APA offset
                apaMulRht;          // ...           multiplier
    HGLOBAL     hGlbRht = NULL,     // Right arg global memory handle
                hGlbRes = NULL;     // Result    ...
    LPVOID      lpMemRht = NULL,    // Ptr to right arg global memory
                lpMemRes = NULL;    // Ptr to result    ...
    LPAPLDIM    lpMemDimRht;        // Ptr to right arg dimensions
    BOOL        bRet = TRUE,        // TRUE iff result is valid
                bFastBool = FALSE;  // TRUE iff this is a Fast Boolean operation
////LPPRIMSPEC  lpPrimSpec;         // Ptr to local PRIMSPEC
    LPPRIMFLAGS lpPrimFlags;        // Ptr to corresponding PrimFlags entry
    LPYYSTYPE   lpYYRes = NULL;     // Ptr to the result
    LPYYSTYPE   lpYYFcnStrLft;      // Ptr to left operand function strand
    TOKEN       tkLftArg = {0},     // Left arg token
                tkRhtArg = {0};     // Right ...

    // Get the attributes (Type, NELM, and Rank) of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht);

    // Set ptr to left operand,
    //   skipping over the operator and axis token (if present)
    lpYYFcnStrLft = &lpYYFcnStrOpr[1 + (lptkAxis NE NULL)];

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
        // No axis specified:
        // if Slope, use last dimension
        if (lpYYFcnStrOpr->tkToken.tkData.tkChar EQ INDEX_OPSLOPE)
            aplAxis = aplRankRht - 1;
        else
            // Otherwise, it's SlopeBar on the first dimension
            aplAxis = 0;
    } // End IF/ELSE

    // Get right arg's global ptr
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is a scalar, return it
    if (aplRankRht EQ 0)
    {
        lpYYRes = PrimOpMonSlashScalar_EM_YY (lptkRhtArg,       // Ptr to right arg token
                                              hGlbRht,          // Right arg global memory handle
                                              lpMemRht,         // Ptr to right arg global memory
                                              lpYYFcnStrOpr,    // Ptr to operator function strand
                                              lpPrimProtoLft);  // Ptr to lefft operand prototype function
        goto NORMAL_EXIT;
    } // End IF

    //***************************************************************
    // From here on, the right arg is a vector or higher rank array
    //***************************************************************

    // The rank of the result is the same as that of the right arg
    aplRankRes = aplRankRht;

    // Skip over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // Get APA parameters
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        apaOffRht = lpAPA->Off;
        apaMulRht = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    //***************************************************************
    // Calculate product of dimensions before, at, and after the axis dimension
    //***************************************************************

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

    // Calculate the result NELM
    aplNELMRes = imul64 (uDimLo, uDimHi, &bRet);
    if (bRet || uDimAxRht EQ 0)
        aplNELMRes = imul64 (aplNELMRes, uDimAxRht, &bRet);
    if (!bRet)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
        goto ERROR_EXIT;
    } // End IF

    // If the right arg is empty, return it
    if (aplNELMRht EQ 0)
    {
        hGlbRes = CopySymGlbDirGlb (MakeGlbTypeGlb (hGlbRht));

        goto YYALLOC_EXIT;
    } // End IF

    // Calculate a ptr to the Primitive Function Flags
    lpPrimFlags = &PrimFlags[SymTrans (&lpYYFcnStrLft->tkToken)];

    // If the product of the dimensions above
    //   the axis dimension is one, and
    //   this is a primitive function, and
    //   the right arg is Boolean or APA, and
    //   we're not doing prototypes, then
    //   check for the possibility of doing a
    //   Fast Boolean Reduction
    if (uDimHi EQ 1
     && lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
     && (aplTypeRht EQ ARRAY_BOOL
      || (aplTypeRht EQ ARRAY_APA
       && (apaOffRht EQ 0 || apaOffRht EQ 1)
       && apaMulRht EQ 0 ))
     && lpPrimProtoLft EQ NULL
     && lpPrimFlags
     && lpPrimFlags->FastBool)
    {
        // Mark as a Fast Boolean operation
        bFastBool = TRUE;

        // The storage type is Boolean
        aplTypeRes = ARRAY_BOOL;
    } else
////// If the operand is a primitive scalar dyadic function,
//////   calculate the storage type of the result,
//////   otherwise, assume it's ARRAY_NESTED
////if (lpYYFcnStrLft->tkToken.tkFlags.TknType EQ TKT_FCNIMMED
//// && PrimFlags[SymTrans (&lpYYFcnStrLft->tkToken)].DydScalar)
////{
////    // Get the corresponding lpPrimSpec
////    lpPrimSpec = PrimSpecTab[SymTrans (&lpYYFcnStrLft->tkToken)];
////
////    // Calculate the storage type of the result
////    aplTypeRes = (*lpPrimSpec->StorageTypeDyd) (1,
////                                               &aplTypeRht,
////                                               &lpYYFcnStrLft->tkToken,
////                                                1,
////                                               &aplTypeRht);
////    if (aplTypeRes EQ ARRAY_ERROR)
////    {
////        ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
////                                  &lpYYFcnStrOpr->tkToken);
////        goto ERROR_EXIT;
////    } // End IF
////} else
        aplTypeRes = ARRAY_NESTED;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                  &lpYYFcnStrOpr->tkToken);
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

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Copy the dimensions from the right arg to the result
    for (uDim = 0; uDim < aplRankRht; uDim++)
        *((LPAPLDIM) lpMemRes)++ = lpMemDimRht[uDim];

    // lpMemRes now points to its data

    if (aplTypeRes EQ ARRAY_NESTED)
    {
        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        *((LPAPLNESTED) lpMemRes) = PTR_REUSED;
        for (uRes = 1; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;
    } // End IF

    // If this is a fast Boolean operation, ...
    if (bFastBool)
    {
        LPFASTBOOLFCN lpFastBool;           // Ptr to Fast Boolean reduction routine

        // Get a ptr to the appropriate Fast Boolean routine
        lpFastBool = FastBoolFns[lpPrimFlags->Index].lpScan;

        // Call it
        (*lpFastBool) (aplTypeRht,          // Right arg storage type
                       lpMemRht,            // Ptr to right arg memory
                       lpMemRes,            // Ptr to result    memory
                       uDimLo,              // Product of dimensions below axis
                       uDimAxRht,           // Length of right arg axis dimension
                       lpPrimFlags->Index,  // enum tagFBFNINDS value (e.g., index into FastBoolFns[])
                       lpYYFcnStrOpr);      // Ptr to operator function strand
    } else
    {
        // Fill in the right arg token
////////tkRhtArg.tkFlags.TknType   =            // To be filled in below
////////tkRhtArg.tkFlags.ImmType   =            // To be filled in below
////////tkRhtArg.tkFlags.NoDisplay = 0;         // Already zero from {0}
////////tkRhtArg.tkData.tkGlbData  =            // To be filled in below
        tkRhtArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Fill in the left arg token
////////tkLftArg.tkFlags.TknType   =            // To be filled in below
////////tkLftArg.tkFlags.ImmType   =            // To be filled in below
////////tkLftArg.tkFlags.NoDisplay = 0;         // Already zero from {0}
////////tkLftArg.tkData.tkGlbData  =            // To be filled in below
        tkLftArg.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

        // Loop through the right arg calling the
        //   function strand between data, storing in the
        //   result
        for (uLo = 0; uLo < uDimLo; uLo++)
        for (uHi = 0; uHi < uDimHi; uHi++)
        {
            APLUINT uAx;            // Loop counter for uDimAxRht
            APLINT  iDim;           // Loop counter for aplIntegerLftAbs

            // Calculate the starting index in the right arg of this vector
            uDimRht = uLo * uDimHi * uDimAxRht + uHi;

            // There are uDimAxRht elements in the result to be generated now
            //   each being successive reductions of leading elements in the
            //   vector under consideration.

            // If this function is associative, speed it up
            if (lpPrimFlags
             && (lpPrimFlags->AssocBool && aplTypeRht EQ ARRAY_BOOL
              || lpPrimFlags->AssocNumb && IsSimpleNum (aplTypeRht)))
            {
                // Calculate the first index in this vector
                uRht = uDimRht + 0 * uDimHi;

                // Get the first element as the left arg
                GetValueIntoToken (uRht,            // Index to use
                                   lpMemRht,        // Ptr to global memory object to index
                                   aplTypeRht,      // Storage type of the arg
                                   apaOffRht,       // APA offset (if needed)
                                   apaMulRht,       // APA multiplier (if needed)
                                  &tkLftArg);       // Ptr to token in which to place the value
                // Split cases based upon the token type of the left arg
                switch (tkLftArg.tkFlags.TknType)
                {
                    case TKT_VARIMMED:
                        // Save in the result as an LPSYMENTRY
                        ((LPAPLNESTED) lpMemRes)[uRht] = MakeSymEntry_EM (tkLftArg.tkFlags.ImmType,
                                                                         &tkLftArg.tkData.tkLongest,
                                                                         &lpYYFcnStrOpr->tkToken);
                        break;

                    case TKT_VARARRAY:
                        // Save in the result as an HGLOBAL
                        ((LPAPLNESTED) lpMemRes)[uRht] = tkLftArg.tkData.tkGlbData;

                        break;

                    defstop
                        break;
                } // End SWITCH

                // Loop forwards through the elements along the specified axis
                for (uAx = 1; uAx < uDimAxRht; uAx++)
                {
                    // Calculate the index of the next element in this vector
                    uRht = uDimRht + uAx * uDimHi;

                    // Get the next element as the right arg
                    GetValueIntoToken (uRht,        // Index to use
                                       lpMemRht,    // Ptr to global memory object to index
                                       aplTypeRht,  // Storage type of the arg
                                       apaOffRht,   // APA offset (if needed)
                                       apaMulRht,   // APA multiplier (if needed)
                                      &tkRhtArg);   // Ptr to token in which to place the value
                    // Execute the left operand between the left & right args
                    if (lpPrimProtoLft)
                        // Note that we cast the function strand to LPTOKEN
                        //   to bridge the two types of calls -- one to a primitive
                        //   function which takes a function token, and one to a
                        //   primitive operator which takes a function strand
                        lpYYRes = (*lpPrimProtoLft) (&tkLftArg,         // Ptr to left arg token
                                            (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                                     &tkRhtArg,         // Ptr to right arg token
                                                      lptkAxis);        // Ptr to axis token (may be NULL)
                    else
                        lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,         // Ptr to left arg token
                                                      lpYYFcnStrLft,    // Ptr to function strand
                                                     &tkRhtArg);        // Ptr to right arg token
                    // Free the left & right arg tokens
                    FreeResult (&tkRhtArg);
                    FreeResult (&tkLftArg);

                    // If it succeeded, ...
                    if (lpYYRes)
                    {
                        // Copy the result to the left arg token
                        tkLftArg = *CopyToken_EM (&lpYYRes->tkToken, FALSE);

                        // Free the YYRes
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } else
                        goto ERROR_EXIT;

                    // Split cases based upon the token type of the left arg
                    switch (tkLftArg.tkFlags.TknType)
                    {
                        case TKT_VARIMMED:
                            // Save in the result as an LPSYMENTRY
                            ((LPAPLNESTED) lpMemRes)[uRht] = MakeSymEntry_EM (tkLftArg.tkFlags.ImmType,
                                                                             &tkLftArg.tkData.tkLongest,
                                                                             &lpYYFcnStrOpr->tkToken);
                            break;

                        case TKT_VARARRAY:
                            // Save in the result as an HGLOBAL
                            ((LPAPLNESTED) lpMemRes)[uRht] = tkLftArg.tkData.tkGlbData;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR
            } else
            // Loop through the ending indices of the reduction, that is,
            //   each reduction starts at uRht and goes
            //   through the successive elements up to and including
            //   uRht + uAx * uDimHi.  If the primitive function is
            //   associative, we can use a shortcut.
            for (uAx = 0; uAx < uDimAxRht; uAx++)
            {
                // Reduce the uDimAxRht values starting at
                //   uRht = uDimRht +   0 * uDimHi;
                // through
                //   uRht = uDimRht + uAx * uDimHi;

                // Calculate the index of last element in this vector
                uRht = uDimRht + uAx * uDimHi;

                // Get the last element as the right arg
                GetValueIntoToken (uRht,            // Index to use
                                   lpMemRht,        // Ptr to global memory object to index
                                   aplTypeRht,      // Storage type of the arg
                                   apaOffRht,       // APA offset (if needed)
                                   apaMulRht,       // APA multiplier (if needed)
                                  &tkRhtArg);       // Ptr to token in which to place the value
                // Loop backwards through the elements along the specified axis
                for (iDim = uAx - 1; iDim >= 0; iDim--)
                {
                    // Calculate the index of the previous element in this vector
                    uRht = uDimRht + iDim * uDimHi;

                    // Get the previous element as the left arg
                    GetValueIntoToken (uRht,        // Index to use
                                       lpMemRht,    // Ptr to global memory object to index
                                       aplTypeRht,  // Storage type of the arg
                                       apaOffRht,   // APA offset (if needed)
                                       apaMulRht,   // APA multiplier (if needed)
                                      &tkLftArg);   // Ptr to token in which to place the value
                    // Execute the left operand between the left & right args
                    if (lpPrimProtoLft)
                        // Note that we cast the function strand to LPTOKEN
                        //   to bridge the two types of calls -- one to a primitive
                        //   function which takes a function token, and one to a
                        //   primitive operator which takes a function strand
                        lpYYRes = (*lpPrimProtoLft) (&tkLftArg,         // Ptr to left arg token
                                            (LPTOKEN) lpYYFcnStrLft,    // Ptr to left operand function strand
                                                     &tkRhtArg,         // Ptr to right arg token
                                                      lptkAxis);        // Ptr to axis token (may be NULL)
                    else
                        lpYYRes = ExecFuncStr_EM_YY (&tkLftArg,         // Ptr to left arg token
                                                      lpYYFcnStrLft,    // Ptr to function strand
                                                     &tkRhtArg);        // Ptr to right arg token
                    // Free the left & right arg tokens
                    FreeResult (&tkRhtArg);
                    FreeResult (&tkLftArg);

                    // If it succeeded, ...
                    if (lpYYRes)
                    {
                        // Copy the result to the right arg token
                        tkRhtArg = *CopyToken_EM (&lpYYRes->tkToken, FALSE);

                        // Free the YYRes
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } else
                        goto ERROR_EXIT;
                } // End FOR

                // Split cases based upon the token type of the right arg (result)
                switch (tkRhtArg.tkFlags.TknType)
                {
                    case TKT_VARIMMED:
                        // Save in the result as an LPSYMENTRY
                        ((LPAPLNESTED) lpMemRes)[uDimRht + uAx * uDimHi] = MakeSymEntry_EM (tkRhtArg.tkFlags.ImmType,
                                                                      &tkRhtArg.tkData.tkLongest,
                                                                      &lpYYFcnStrOpr->tkToken);
                        break;

                    case TKT_VARARRAY:
                        // Save in the result as an HGLOBAL
                        ((LPAPLNESTED) lpMemRes)[uDimRht + uAx * uDimHi] = tkRhtArg.tkData.tkGlbData;

                        break;

                    defstop
                        break;
                } // End SWITCH
            } // End FOR

            // Because we didn't CopyArray_EM on the tkGlbData,
            //   we don't need to free the right arg token
        } // End FOR/FOR
    } // End IF/ELSE
YYALLOC_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lpYYFcnStrOpr->tkToken.tkCharIndex;

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
} // End PrimOpMonSlopeCommon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimOpDydSlope_EM_YY
//
//  Primitive operator for dyadic derived function from Slope ("ERROR")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimOpDydSlope_EM_YY"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimOpDydSlope_EM_YY
    (LPTOKEN   lptkLftArg,          // Ptr to left arg token (may be NULL if monadic)
     LPYYSTYPE lpYYFcnStr,          // Ptr to operator function strand
     LPTOKEN   lptkRhtArg,          // Ptr to right arg token
     LPTOKEN   lptkAxis)            // Ptr to axis token (may be NULL)

{
    return PrimFnSyntaxError_EM (&lpYYFcnStr->tkToken);
} // End PrimOpDydSlope_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  End of File: po_slope.c
//***************************************************************************
