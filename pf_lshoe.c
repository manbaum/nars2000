//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftShoe
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
//  PrimFnLeftShoe_EM
//
//  Primitive function for monadic and dyadic LeftShoe ("enclose" and "partitioned enclose")
//***************************************************************************

LPYYSTYPE PrimFnLeftShoe_EM
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_LEFTSHOE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonLeftShoe_EM             (lptkFunc, lptkRhtArg, lptkAxis, lpplLocalVars);
    else
        return PrimFnDydLeftShoe_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis, lpplLocalVars);
} // End PrimFnLeftShoe_EM


//***************************************************************************
//  PrimFnMonLeftShoe_EM
//
//  Primitive function for monadic LeftShoe ("enclose")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftShoe_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonLeftShoe_EM
    (LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                return PrimFnMonLeftShoeGlb_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkSym->stData.stGlbData),
                                                lptkAxis,
                                                lptkFunc,
                                                lpplLocalVars);
            } // End IF

            // Handle the immediate case
            return PrimFnMonLeftShoeCon_EM (lptkRhtArg->tkData.tkSym->stFlags.ImmType,
                                            lptkRhtArg->tkData.tkSym->stData.stLongest,
                                            lptkRhtArg,
                                            lptkAxis,
                                            lptkFunc,
                                            lpplLocalVars);
        case TKT_VARIMMED:
            return PrimFnMonLeftShoeCon_EM (lptkRhtArg->tkFlags.ImmType,
                                            lptkRhtArg->tkData.tkLongest,
                                            lptkRhtArg,
                                            lptkAxis,
                                            lptkFunc,
                                            lpplLocalVars);
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonLeftShoeGlb_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),
                                            lptkAxis,
                                            lptkFunc,
                                            lpplLocalVars);
        case TKT_LISTPAR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc,
                                       lpplLocalVars);
            return NULL;

        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnMonLeftShoe_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonLeftShoeCon_EM
//
//  Monadic LeftShoe ("enclose") on an immediate value.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftShoeCon_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonLeftShoeCon_EM
    (UINT          ImmType,         // The immediate type
     APLLONGEST    aplLongest,      // The immediate value
     LPTOKEN       lpTokenRht,      // Ptr to right arg token
     LPTOKEN       lptkAxis,        // Ptr to axis token
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    LPYYSTYPE lpYYRes;

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Enclose with axis on a simple scalar requires
        //   that the axis be an empty vector
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           0,               // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL,            // Return HGLOBAL with APLINT axis values
                           lpplLocalVars))  // Ptr to local plLocalVars
            return NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = ImmType;
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = aplLongest;
    lpYYRes->tkToken.tkCharIndex       = lpTokenRht->tkCharIndex;

    return lpYYRes;
} // End PrimFnMonLeftShoeCon_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonLeftShoeGlb_EM
//
//  Monadic LeftShoe ("enclose") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftShoeGlb_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnMonLeftShoeGlb_EM
    (HGLOBAL       hGlbRht,         // Handle to right arg
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    HGLOBAL     hGlbRes = NULL,
                hGlbAxis = NULL,
                hGlbSub = NULL,
                hGlbProto = NULL,
                hGlbRhtProto = NULL,
                hGlbOdo = NULL,
                hGlbWVec = NULL;
    LPVOID      lpMemRes = NULL,
                lpMemRht = NULL,
                lpMemSub = NULL,
                lpMemProto = NULL;
    LPAPLDIM    lpMemDimRht = NULL;
    LPAPLINT    lpMemAxis = NULL,
                lpMemGrUp = NULL,
                lpMemOdo = NULL,
                lpMemWVec = NULL;
    APLUINT     ByteRes;          // # bytes needed in the result
    APLNELM     aplNELMAxis,
                aplNELMRes,
                aplNELMRht,
                aplNELMSub;
    APLRANK     aplRankRht,
                aplRankRes;
    BOOL        bRet = TRUE;
    APLNELM     uRes, uRht, uSub, uOdo, uRhtOff;
    APLSTYPE    aplTypeRht;
    APLNELMSIGN iRht;
    APLINT      apaOff,
                apaMul;
    LPYYSTYPE   lpYYRes;

    // Get the rank of the right arg
    aplRankRht = RankOfGlb (hGlbRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           FALSE,           // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                           NULL,            // Return last axis value
                          &aplNELMAxis,     // Return # elements in axis vector
                          &hGlbAxis,        // Return HGLOBAL with APLINT axis values
                           lpplLocalVars))  // Ptr to local plLocalVars
            return NULL;
    } else
        // No axis means enclose all dimensions
        aplNELMAxis = aplRankRht;

    //***************************************************************
    // hGlbAxis is not-NULL iff there is an axis
    // aplNELMAxis contains the # elements in the axis.
    //***************************************************************

    // Calculate the rank of the result
    aplRankRes = aplRankRht - aplNELMAxis;

    // If the axis is present and it's empty, this
    //   is an identity function.  We could go through
    //   all of the work below and end up with the same
    //   result, but why bother??
    if (lptkAxis && aplNELMAxis EQ 0)
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDirGlb (hGlbRht);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto QUICK_EXIT;
    } // End IF

    // Lock the right arg to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)

    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;

#undef  lpHeader

    // Skp over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

#define lpAPA       ((LPAPLAPA) lpMemRht)

    // If the right arg is an APA, get its parameters
    if (aplTypeRht EQ ARRAY_APA)
    {
        apaOff = lpAPA->Off;
        apaMul = lpAPA->Mul;
    } // End IF

#undef  lpAPA

    // Calculate the NELM of the result taking into
    //   account the axis values.
////if (hGlbAxis)
    if (lptkAxis NE NULL)
    {
        // Lock the axis memory to get a ptr to it
        lpMemAxis = MyGlobalLock (hGlbAxis);

        // Point to the grade-up of the first
        //   <aplRankRht> values in lpMemAxis
        lpMemGrUp = &lpMemAxis[aplRankRht];

        aplNELMRes = 1;     // Initialize with identity element for multiplication
        aplNELMSub = 1;     // ...

        // Loop through the leading axis indices
        //   extracting the corresponding dimension
        //   from the right arg to calculate the NELM
        //   of the result.
        for (uRes = 0; uRes < aplRankRes; uRes++)
            aplNELMRes *= lpMemDimRht[lpMemAxis[uRes]];

        // Loop through the trailing axis indices
        //   extracting the corresponding dimension
        //   from the right arg to calulate the NELM
        //   of each subarray.
        for (uRes = aplRankRes; uRes < aplRankRht; uRes++)
            aplNELMSub *= lpMemDimRht[lpMemAxis[uRes]];
    } else
        aplNELMRes = aplNELMSub = 1;

    // Calculate space needed for the result
    // Add in the header, (aplRankRht - aplNELMAxis) dimensions,
    //   and <aplNELMRes> HGLOBALs (taking into account a prototype if empty).
    ByteRes = CalcArraySize (ARRAY_NESTED, aplNELMRes, aplRankRes);

    //***************************************************************
    // Now we can allocate the storage for the result.
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (UINT) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!hGlbRes)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc,
                                   lpplLocalVars);
        bRet = FALSE;

        goto ERROR_EXIT;
    } // End IF

    // Lock the global memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeaderRes     ((LPVARARRAY_HEADER) lpMemRes)

    // Fill in the header
    lpHeaderRes->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeaderRes->ArrType    = ARRAY_NESTED;
////lpHeaderRes->Perm       = 0;            // Already zero from GHND
////lpHeaderRes->SysVar     = 0;            // Already zero from GHND
    lpHeaderRes->RefCnt     = 1;
    lpHeaderRes->NELM       = aplNELMRes;
    lpHeaderRes->Rank       = aplRankRes;

#undef  lpHeaderRes

    // Point to the result's dimension
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    //***************************************************************
    // Copy the appropriate dimensions from the right arg
    //   to the result's dimension
    //***************************************************************

    // Loop through the leading axis indices
    //   extracting the corresponding dimension
    //   from the right arg
    for (uRes = 0; uRes < aplRankRes; uRes++)
        *((LPAPLDIM) lpMemRes)++ = lpMemDimRht[lpMemAxis[uRes]];

    // lpMemRes now points to the result's data

    //***************************************************************
    // Copy the data to the result
    //***************************************************************

    // If this is enclose without axis
    if (lptkAxis EQ NULL)
        // Save the HGLOBAL
        *((LPAPLNESTED) lpMemRes) = CopySymGlbDirGlb (hGlbRht);
    else
    {
        // Handle prototypes for the result
        if (aplNELMRes EQ 0)
        {
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_APA:
                case ARRAY_FLOAT:
                    bRet = PrimFnMonLeftShoeProto_EM (&hGlbProto,
                                                      hGlbZilde,
                                                      ARRAY_BOOL,
                                                      aplNELMSub,
                                                      aplNELMAxis,
                                                      ARRAY_BOOL,
                                                      lptkFunc,
                                                      lpplLocalVars);
                    if (!bRet)
                        goto ERROR_EXIT;
                    break;

                case ARRAY_CHAR:
                    bRet = PrimFnMonLeftShoeProto_EM (&hGlbProto,
                                                      hGlbMTChar,
                                                      ARRAY_CHAR,
                                                      aplNELMSub,
                                                      aplNELMAxis,
                                                      ARRAY_CHAR,
                                                      lptkFunc,
                                                      lpplLocalVars);
                    if (!bRet)
                        goto ERROR_EXIT;
                    break;

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                    if (aplNELMSub EQ 0)
                        hGlbProto = MakePrototype_EM (ClrPtrTypeIndGlb (lpMemRht),
                                                      lptkFunc,         // Ptr to function token
                                                      FALSE,            // Allow CHARs
                                                      lpplLocalVars);   // Ptr to local plLocalVars
                    else
                    {
                        // Calculate space needed for the result
                        ByteRes = CalcArraySize (ARRAY_NESTED, aplNELMSub, aplNELMAxis);

                        // N.B.:  Conversion from APLUINT to UINT.
                        Assert (ByteRes EQ (UINT) ByteRes);
                        hGlbProto = DbgGlobalAlloc (GHND, (UINT) ByteRes);
                        if (!hGlbProto)
                        {
                            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                                       lptkFunc,
                                                       lpplLocalVars);
                            bRet = FALSE;

                            goto ERROR_EXIT;
                        } // End IF

                        // Lock the memory to get a ptr to it
                        lpMemProto = MyGlobalLock (hGlbProto);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemProto)

                        // Fill in the header
                        lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
                        lpHeader->ArrType    = ARRAY_NESTED;
////////////////////////lpHeader->Perm       = 0;       // Already zero from GHND
////////////////////////lpHeader->SysVar     = 0;       // Already zero from GHND
                        lpHeader->RefCnt     = 1;
                        lpHeader->NELM       = aplNELMSub;
                        lpHeader->Rank       = aplNELMAxis;

#undef  lpHeader
                        // Fill in the dimension
                        *VarArrayBaseToDim (lpMemProto) = aplNELMSub;

                        // skip over the header and dimension to the data
                        lpMemProto = VarArrayBaseToData (lpMemProto, aplNELMAxis);

                        // Fill in the values
                        for (uRes = 0; uRes < aplNELMSub; uRes++)
                            // Save the value in the prototype
                            *((LPAPLNESTED) lpMemProto)++ = CopySymGlbInd (lpMemRht);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbProto); lpMemProto = NULL;

                        // If we failed, ...
                        if (!bRet)
                        {
                            // Allocate a new YYRes
                            lpYYRes = YYAlloc ();

                            // Fill in the result token
                            lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////////////////lpYYRes->tkToken.tkFlags.ImmType   = 0;    // Already zero from YYAlloc
////////////////////////////lpYYRes->tkToken.tkFlags.NoDisplay = 0;    // Already zero from YYAlloc
                            lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbProto);
                            lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                            // Free the prototype storage
                            FreeResult (&lpYYRes->tkToken); YYFree (lpYYRes); lpYYRes = NULL;

                            goto ERROR_EXIT;
                        } // End IF
                    } // End IF/ELSE

                    break;

                defstop
                    break;
            } // End SWITCH

            // Save the HGLOBAL in the result
            *((LPAPLNESTED) lpMemRes) = MakeGlbTypeGlb (hGlbProto);

            goto NORMAL_EXIT;
        } // End IF

        // Fill in the HGLOBALs with PTR_REUSED
        //   in case we fail part way through
        for (uRes = 0; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;

        //***************************************************************
        // Check for empty subarrays
        //***************************************************************

        if (aplNELMSub EQ 0)
        {
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_FLOAT:
                case ARRAY_APA:
                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        // Save the HGLOBAL in the result
                        *((LPAPLNESTED) lpMemRes)++ = MakeGlbTypeGlb (hGlbZilde);
                    break;

                case ARRAY_CHAR:
                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        // Save the HGLOBAL in the result
                        *((LPAPLNESTED) lpMemRes)++ = MakeGlbTypeGlb (hGlbMTChar);
                    break;

                case ARRAY_NESTED:
                case ARRAY_HETERO:
                    // Fill in the values
                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                        // Save the value in the prototype
                        *((LPAPLNESTED) lpMemRes)++ = CopySymGlbInd (lpMemRht);
                    break;

                case ARRAY_LIST:        // Handled above
                defstop
                    break;
            } // End SWITCH

            goto NORMAL_EXIT;
        } // End IF

        //***************************************************************
        // Allocate space for the weighting vector which is
        //   {times}{backscan}1{drop}({rho}R),1
        // N.B.:  Conversion from APLUINT to UINT.
        //***************************************************************
        ByteRes = aplRankRht * sizeof (APLINT);
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbWVec = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbWVec)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc,
                                       lpplLocalVars);
            bRet = FALSE;

            goto ERROR_EXIT;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemWVec = MyGlobalLock (hGlbWVec);

        // Loop through the dimensions of the right arg in reverse
        //   order {backscan} and compute the cumulative product
        //   (weighting vector).
        // Note we use a signed index variable because we're
        //   walking backwards and the test against zero must be
        //   made as a signed variable.
        for (uRes = 1, iRht = aplRankRht - 1; iRht >= 0; iRht--)
        {
            lpMemWVec[iRht] = uRes;
            uRes *= lpMemDimRht[iRht];
        } // End FOR

        //***************************************************************
        // Allocate space for the odometer array, one value per dimension
        //   in the right arg, with values initially all zero (thanks to GHND).
        // N.B.:  Conversion from APLUINT to UINT.
        //***************************************************************
        ByteRes = aplRankRht * sizeof (APLINT);
        Assert (ByteRes EQ (UINT) ByteRes);
        hGlbOdo = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!hGlbOdo)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc,
                                       lpplLocalVars);
            bRet = FALSE;

            goto ERROR_EXIT;
        } // End IF

        // Lock the global memory to get a ptr to it
        lpMemOdo = MyGlobalLock (hGlbOdo);

        //***************************************************************
        // Calculate space needed for each subarray.
        // N.B.:  Conversion from APLUINT to UINT.
        //***************************************************************
        // Handle APAs as INTs
        if (aplTypeRht EQ ARRAY_APA)
            ByteRes = CalcArraySize (ARRAY_INT , aplNELMSub, aplNELMAxis);
        else
            ByteRes = CalcArraySize (aplTypeRht, aplNELMSub, aplNELMAxis);

        //***************************************************************
        // Loop through the result NELM allocating storage
        //   for each subarray and extract the appropriate array from
        //   the right arg
        // The subarray is of rank <aplNELMAxis>.
        // The subarray's dimensions are the first <aplNELMAxis>
        //   values in lpMemDimRht[lpMemAxis[u]].
        // The subarray's NELM is <aplNELMSub>.
        // The subarray's values are the arrays indexed by all <aplNELMRes>
        //   values in the trailing indices in the right arg's dimensions.
        //***************************************************************

        for (uRhtOff = uRes = 0; uRes < aplNELMRes; uRes++)
        // Split cases based upon the right arg's array type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
                //***************************************************************
                // Allocate storage for this subarray,
                //   save the HGLOBAL in the result,
                //   lock the subarray to return a ptr to it,
                //   fill in the subarray header, and
                //   copy the dimensions from the right arg to the subarray.
                //***************************************************************
                bRet = PrimFnMonLeftShoeGlbSub_EM (ByteRes,
                                                   ARRAY_BOOL,
                                                   aplNELMSub,
                                                   aplNELMAxis,
                                                   aplRankRes,
                                                   aplRankRht,
                                                   ((LPAPLNESTED) lpMemRes)++,
                                                   lpMemDimRht,
                                                   lpMemAxis,
                                                   &hGlbSub,
                                                   &lpMemSub,
                                                   lptkFunc,
                                                   lpplLocalVars);
                if (!bRet)
                    goto ERROR_EXIT;

                //***************************************************************
                // Copy data from the right arg to the subarray
                //***************************************************************

                // Traverse the subarray
                for (uSub = 0; uSub < aplNELMSub; uSub++)
                {
                    UINT uBitMask;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    uBitMask = 1 << (((UINT) uRht) & MASKLOG2NBIB);

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    ((LPAPLBOOL) lpMemSub)[uSub >> LOG2NBIB] |=
                    ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? 1 : 0) << (((UINT) uSub) & MASKLOG2NBIB);

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                break;

            case ARRAY_INT:
                //***************************************************************
                // Allocate storage for this subarray,
                //   save the HGLOBAL in the result,
                //   lock the subarray to return a ptr to it,
                //   fill in the subarray header, and
                //   copy the dimensions from the right arg to the subarray.
                //***************************************************************
                bRet = PrimFnMonLeftShoeGlbSub_EM (ByteRes,
                                                   ARRAY_INT,
                                                   aplNELMSub,
                                                   aplNELMAxis,
                                                   aplRankRes,
                                                   aplRankRht,
                                                   ((LPAPLNESTED) lpMemRes)++,
                                                   lpMemDimRht,
                                                   lpMemAxis,
                                                   &hGlbSub,
                                                   &lpMemSub,
                                                   lptkFunc,
                                                   lpplLocalVars);
                if (!bRet)
                    goto ERROR_EXIT;

                //***************************************************************
                // Copy data from the right arg to the subarray
                //***************************************************************

                // Traverse the subarray
                for (uSub = 0; uSub < aplNELMSub; uSub++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    ((LPAPLINT) lpMemSub)[uSub] = ((LPAPLINT) lpMemRht)[uRht];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                break;

            case ARRAY_FLOAT:
                //***************************************************************
                // Allocate storage for this subarray,
                //   save the HGLOBAL in the result,
                //   lock the subarray to return a ptr to it,
                //   fill in the subarray header, and
                //   copy the dimensions from the right arg to the subarray.
                //***************************************************************
                bRet = PrimFnMonLeftShoeGlbSub_EM (ByteRes,
                                                   ARRAY_FLOAT,
                                                   aplNELMSub,
                                                   aplNELMAxis,
                                                   aplRankRes,
                                                   aplRankRht,
                                                   ((LPAPLNESTED) lpMemRes)++,
                                                   lpMemDimRht,
                                                   lpMemAxis,
                                                   &hGlbSub,
                                                   &lpMemSub,
                                                   lptkFunc,
                                                   lpplLocalVars);
                if (!bRet)
                    goto ERROR_EXIT;

                //***************************************************************
                // Copy data from the right arg to the subarray
                //***************************************************************

                // Traverse the subarray
                for (uSub = 0; uSub < aplNELMSub; uSub++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    ((LPAPLFLOAT) lpMemSub)[uSub] = ((LPAPLFLOAT) lpMemRht)[uRht];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                break;

            case ARRAY_CHAR:
                //***************************************************************
                // Allocate storage for this subarray,
                //   save the HGLOBAL in the result,
                //   lock the subarray to return a ptr to it,
                //   fill in the subarray header, and
                //   copy the dimensions from the right arg to the subarray.
                //***************************************************************
                bRet = PrimFnMonLeftShoeGlbSub_EM (ByteRes,
                                                   ARRAY_CHAR,
                                                   aplNELMSub,
                                                   aplNELMAxis,
                                                   aplRankRes,
                                                   aplRankRht,
                                                   ((LPAPLNESTED) lpMemRes)++,
                                                   lpMemDimRht,
                                                   lpMemAxis,
                                                   &hGlbSub,
                                                   &lpMemSub,
                                                   lptkFunc,
                                                   lpplLocalVars);
                if (!bRet)
                    goto ERROR_EXIT;

                //***************************************************************
                // Copy data from the right arg to the subarray
                //***************************************************************

                // Traverse the subarray
                for (uSub = 0; uSub < aplNELMSub; uSub++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    ((LPAPLCHAR) lpMemSub)[uSub] = ((LPAPLCHAR) lpMemRht)[uRht];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                break;

            case ARRAY_HETERO:
                //***************************************************************
                // Allocate storage for this subarray,
                //   save the HGLOBAL in the result,
                //   lock the subarray to return a ptr to it,
                //   fill in the subarray header, and
                //   copy the dimensions from the right arg to the subarray.
                //***************************************************************
                bRet = PrimFnMonLeftShoeGlbSub_EM (ByteRes,
                                                   ARRAY_HETERO,
                                                   aplNELMSub,
                                                   aplNELMAxis,
                                                   aplRankRes,
                                                   aplRankRht,
                                                   ((LPAPLNESTED) lpMemRes)++,
                                                   lpMemDimRht,
                                                   lpMemAxis,
                                                   &hGlbSub,
                                                   &lpMemSub,
                                                   lptkFunc,
                                                   lpplLocalVars);
                if (!bRet)
                    goto ERROR_EXIT;

                //***************************************************************
                // Copy data from the right arg to the subarray
                //***************************************************************

                // Traverse the subarray
                for (uSub = 0; uSub < aplNELMSub; uSub++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    // Note that APLHETERO elements are LPSYMENTRYs, so there's no
                    //   reference count to increment, or other special handling.
                    ((LPAPLHETERO) lpMemSub)[uSub] = ((LPAPLHETERO) lpMemRht)[uRht];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                break;

            case ARRAY_NESTED:
                //***************************************************************
                // Allocate storage for this subarray,
                //   save the HGLOBAL in the result,
                //   lock the subarray to return a ptr to it,
                //   fill in the subarray header, and
                //   copy the dimensions from the right arg to the subarray.
                //***************************************************************
                bRet = PrimFnMonLeftShoeGlbSub_EM (ByteRes,
                                                   ARRAY_NESTED,
                                                   aplNELMSub,
                                                   aplNELMAxis,
                                                   aplRankRes,
                                                   aplRankRht,
                                                   ((LPAPLNESTED) lpMemRes)++,
                                                   lpMemDimRht,
                                                   lpMemAxis,
                                                   &hGlbSub,
                                                   &lpMemSub,
                                                   lptkFunc,
                                                   lpplLocalVars);
                if (!bRet)
                    goto ERROR_EXIT;

                //***************************************************************
                // Copy data from the right arg to the subarray
                //***************************************************************

                // Traverse the subarray
                for (uSub = 0; uSub < aplNELMSub; uSub++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

#define lpMemData   ((LPAPLNESTED) lpMemRht)[uRht]

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    // Note that APLNESTED elements are a mixture of LPSYMENTRYs
                    //   and HGLOBALs, so we need to run the HGLOBALs through
                    //   CopySymGlbDir to increment the reference count.
                    ((LPAPLNESTED) lpMemSub)[uSub] = CopySymGlbDir (lpMemData);

#undef  lpMemData

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                break;

            case ARRAY_APA:
                //***************************************************************
                // Allocate storage for this subarray,
                //   save the HGLOBAL in the result,
                //   lock the subarray to return a ptr to it,
                //   fill in the subarray header, and
                //   copy the dimensions from the right arg to the subarray.
                //***************************************************************
                bRet = PrimFnMonLeftShoeGlbSub_EM (ByteRes,
                                                   ARRAY_INT,
                                                   aplNELMSub,
                                                   aplNELMAxis,
                                                   aplRankRes,
                                                   aplRankRht,
                                                   ((LPAPLNESTED) lpMemRes)++,
                                                   lpMemDimRht,
                                                   lpMemAxis,
                                                   &hGlbSub,
                                                   &lpMemSub,
                                                   lptkFunc,
                                                   lpplLocalVars);
                if (!bRet)
                    goto ERROR_EXIT;

                //***************************************************************
                // Copy data from the right arg to the subarray
                //***************************************************************

                // Traverse the subarray
                for (uSub = 0; uSub < aplNELMSub; uSub++)
                {
                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    ((LPAPLINT) lpMemSub)[uSub] = apaOff + apaMul * uRht;

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                break;

            case ARRAY_LIST:        // Handled above
            defstop
                break;
        } // End FOR/SWITCH
    } // End IF/ELSE
NORMAL_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = 0;     // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = 0;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (TypeDemote (hGlbRes));
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
ERROR_EXIT:
    if (lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (lpMemOdo)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbOdo); lpMemOdo = NULL;
    } // End IF

    if (lpMemWVec)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbWVec); lpMemWVec = NULL;
    } // End IF

    if (lpMemAxis)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbAxis); lpMemAxis = NULL;
    } // End IF

    if (lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht);  lpMemRht  = NULL;
    } // End IF
QUICK_EXIT:
    if (hGlbWVec)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbWVec); hGlbWVec = NULL;
    } // End IF

    if (hGlbOdo)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbOdo); hGlbOdo = NULL;
    } // End IF

    if (hGlbAxis)
    {
        // We no longer need this storage
        DbgGlobalFree (hGlbAxis); hGlbAxis = NULL;
    } // End IF

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimFnMonLeftShoeGlb_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonLeftShoeProto_EM
//
//  Subroutine to PrimFnMonLeftShoe_EM to handle prorotypes
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftShoeProto_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonLeftShoeProto_EM
    (HGLOBAL      *lphGlbProto,     // Ptr to handle to prototype result
     HGLOBAL       hGlbMT,          // Handle to empty (zilde or '')
     APLSTYPE      aplTypeProto,    // Prototype type
     APLNELM       aplNELMSub,      // Subarray NELM
     APLNELM       aplNELMAxis,     // Axis NELM
     APLSTYPE      aplType,         // Right arg type
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    APLUINT ByteRes,
            uRes;
    LPVOID lpMemProto;

    if (aplNELMSub EQ 0)
        *lphGlbProto = hGlbMT;
    else
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeProto, aplNELMSub, aplNELMAxis);

        // N.B.:  Conversion from APLUINT to UINT.
        Assert (ByteRes EQ (UINT) ByteRes);
        *lphGlbProto = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!*lphGlbProto)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc,
                                       lpplLocalVars);
            return FALSE;
        } // End IF

        // Lock the memory to get a ptr to it
        lpMemProto = MyGlobalLock (*lphGlbProto);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemProto)

        // Fill in the header
        lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplType;
////////lpHeader->Perm       = 0;   // ALready zero from GHND
////////lpHeader->SysVar     = 0;   // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMSub;
        lpHeader->Rank       = aplNELMAxis;

#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemProto) = aplNELMSub;

        // skip over the header and dimension to the data
        lpMemProto = VarArrayBaseToData (lpMemProto, aplNELMAxis);

        // Fill in the values if character
        if (aplType EQ ARRAY_CHAR)
        for (uRes = 0; uRes < aplNELMSub; uRes++)
            *((LPAPLCHAR) lpMemProto)++ = L' ';

        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbProto); lpMemProto = NULL;
    } // End IF/ELSE

    return TRUE;
} // End PrimFnMonLeftShoe_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnMonLeftShoeGlbSub_EM
//
//  Subroutine to PrimFnMonLeftShoeGlb_EM
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftShoeGlbSub_EM"
#else
#define APPEND_NAME
#endif

BOOL PrimFnMonLeftShoeGlbSub_EM
    (APLUINT       ByteRes,         // # bytes in result
     APLSTYPE      aplTypeSub,      // Subarray type
     APLNELM       aplNELMSub,      // Subarray NELM
     APLNELM       aplNELMAxis,     // Axis NELM
     APLRANK       aplRankRes,      // Result rank
     APLRANK       aplRankRht,      // Right arg rank
     LPAPLNESTED   lpMemRes,        // Ptr to result memory
     LPAPLDIM      lpMemDimRht,     // Ptr to right arg dimensions
     LPAPLINT      lpMemAxis,       // Ptr to axis memory
     HGLOBAL      *lphGlbSub,       // Ptr to subarray handle
     LPVOID       *lplpMemSub,      // Ptr to ptr to subarray memory
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    APLINT uRht;

    //***************************************************************
    // Allocate storage for this subarray.
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (UINT) ByteRes);
    *lphGlbSub = DbgGlobalAlloc (GHND, (UINT) ByteRes);
    if (!*lphGlbSub)
    {
        ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                   lptkFunc,
                                   lpplLocalVars);
        return FALSE;
    } // End IF

    // Save the HGLOBAL in the result
    *lpMemRes = MakeGlbTypeGlb (*lphGlbSub);

    // Lock the subarray memory to get a ptr to it
    *lplpMemSub = MyGlobalLock (*lphGlbSub);

#define lpHeader    (*(LPVARARRAY_HEADER *) lplpMemSub)

    // Fill in the subarray header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeSub;
////lpHeader->Perm       = 0;       // Already zero from GHND
////lpHeader->SysVar     = 0;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMSub;
    lpHeader->Rank       = aplNELMAxis;

#undef  lpHeader

    //***************************************************************
    // Copy the dimensions from the right arg into the
    //   subarray's dimensions using the trailing indices.
    // At the end, lpMemSub points to the data.
    //***************************************************************

    *lplpMemSub = VarArrayBaseToDim (*lplpMemSub);
    for (uRht = aplRankRes; uRht < (APLRANKSIGN) aplRankRht; uRht++)
        *((LPAPLDIM) *lplpMemSub)++ = lpMemDimRht[lpMemAxis[uRht]];

    return TRUE;
} // End PrimFnMonLeftShoeGlbSub_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydLeftShoe_EM
//
//  Primitive function for dyadic LeftShoe ("partitioned enclose")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydLeftShoe_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydLeftShoe_EM
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPTOKEN       lptkRhtArg,      // Ptr to right arg token
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkSym->stData.stGlbData));

                return PrimFnDydLeftShoeGlb_EM (lptkLftArg,
                                                ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkSym->stData.stGlbData),
                                                lptkAxis,
                                                lptkFunc,
                                                lpplLocalVars);
            } // End IF

            // Handle the immediate case

            // Fall through to TKT_VARIMMED case to signal a RANK ERROR

        case TKT_VARIMMED:
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc,
                                       lpplLocalVars);
            return NULL;

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnDydLeftShoeGlb_EM (lptkLftArg,
                                            ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),
                                            lptkAxis,
                                            lptkFunc,
                                            lpplLocalVars);
        case TKT_LISTPAR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc,
                                       lpplLocalVars);
            return NULL;

        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnDydLeftShoe_EM
#undef  APPEND_NAME


//***************************************************************************
//  PrimFnDydLeftShoeGlb_EM
//
//  Dyadic LeftShoe ("partitioned enclose") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydLeftShoeGlb_EM"
#else
#define APPEND_NAME
#endif

LPYYSTYPE PrimFnDydLeftShoeGlb_EM
    (LPTOKEN       lptkLftArg,      // Ptr to left arg token
     HGLOBAL       hGlbRht,         // Handle to right arg
     LPTOKEN       lptkAxis,        // Ptr to axis token (may be NULL)
     LPTOKEN       lptkFunc,        // Ptr to function token
     LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    APLINT  aplAxis;        // The (one and only) axis value
    APLRANK aplRankRht;     // The rank of the right arg
    BOOL    bRet = TRUE;
    LPYYSTYPE lpYYRes;

    // Get the rank of the right arg
    aplRankRht = RankOfGlb (hGlbRht);

    // Check for axis present
    if (lptkAxis NE NULL)
    {
        // Check the axis values, fill in # elements in axis
        if (!CheckAxis_EM (lptkAxis,        // The axis token
                           aplRankRht,      // All values less than this
                           TRUE,            // TRUE iff scalar or one-element vector only
                           FALSE,           // TRUE iff want sorted axes
                           FALSE,           // TRUE iff axes must be contiguous
                           FALSE,           // TRUE iff duplicate axes are allowed
                           NULL,            // TRUE iff fractional values allowed
                          &aplAxis,         // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL,            // Return HGLOBAL with APLINT axis values
                           lpplLocalVars))  // Ptr to local plLocalVars
            return NULL;
    } else
    {
        // No axis means partition on the last dimension
        aplAxis = max (0, (APLINT) aplRankRht - 1);
    } // End IF/ELSE

    DbgBrk ();              // ***FINISHME***

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Split cases based upon the










////ERROR_EXIT:
////if (bCopyArray && (!bRet) && hGlbRht)
////{
////    // We no longer need this storage
////    FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;
////} // End IF

    if (bRet)
        return lpYYRes;
    else
        return NULL;
} // End PrimFnDydLeftShoeGlb_EM


//***************************************************************************
//  End of File: pf_lshoe.c
//***************************************************************************
