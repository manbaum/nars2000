//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftShoe
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
//  PrimFnLeftShoe_EM
//
//  Primitive function for monadic and dyadic LeftShoe ("enclose" and "partitioned enclose")
//***************************************************************************

LPYYSTYPE PrimFnLeftShoe_EM
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_LEFTSHOE);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonLeftShoe_EM             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydLeftShoe_EM (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
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
    (LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.lpSym->stData.stGlbData));

                return PrimFnMonLeftShoeGlb_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.lpSym->stData.stGlbData),
                                                lptkAxis,
                                                lptkFunc);
            } // End IF

            // Handle the immediate case
            return PrimFnMonLeftShoeCon_EM (lptkRhtArg->tkData.lpSym->stFlags.ImmType,
                                            lptkRhtArg->tkData.lpSym->stData.stLongest,
                                            lptkRhtArg,
                                            lptkAxis,
                                            lptkFunc);
        case TKT_VARIMMED:
            return PrimFnMonLeftShoeCon_EM (lptkRhtArg->tkFlags.ImmType,
                                            lptkRhtArg->tkData.tkLongest,
                                            lptkRhtArg,
                                            lptkAxis,
                                            lptkFunc);
        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnMonLeftShoeGlb_EM (ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),
                                            lptkAxis,
                                            lptkFunc);
        case TKT_LISTPAR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
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
    (UINT       ImmType,
     APLLONGEST aplLongest,
     LPTOKEN    lpTokenRht,
     LPTOKEN    lptkAxis,
     LPTOKEN    lptkFunc)

{
    UINT YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

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
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } // End IF

    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARIMMED;
    YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = ImmType;
////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;        // Already zero from ZeroMemory
    YYRes[YYLclIndex].tkToken.tkData.tkLongest  = aplLongest;
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lpTokenRht->tkCharIndex;

    return &YYRes[YYLclIndex];
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
    (HGLOBAL hGlbRht,
     LPTOKEN lptkAxis,
     LPTOKEN lptkFunc)

{
    HGLOBAL  hGlbRes = NULL,
             hGlbAxis = NULL,
             hGlbSub = NULL,
             hGlbProto = NULL,
             hGlbRhtProto = NULL,
             hGlbOdo = NULL,
             hGlbWVec = NULL;
    LPVOID   lpMemRes = NULL,
             lpMemRht = NULL,
             lpMemSub = NULL,
             lpMemProto = NULL;
    LPAPLDIM lpMemDimRht = NULL;
    LPAPLINT lpMemAxis = NULL,
             lpMemGrUp = NULL,
             lpMemOdo = NULL,
             lpMemWVec = NULL;
    APLUINT  ByteRes;           // # bytes needed in the result
    APLNELM  aplNELMAxis,
             aplNELMRes,
             aplNELMRht,
             aplNELMSub;
    APLRANK  aplRankRht,
             aplRankRes;
    BOOL     bRet = TRUE;
    APLNELM  uRes, uRht, uSub, uOdo, uRhtOff;
    APLSTYPE aplTypeRht;
    APLNELMSIGN iRht;
    APLINT   apaOff,
             apaMul,
             apaLen;
    UINT     YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

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
                           &aplNELMAxis,    // Return # elements in axis vector
                           &hGlbAxis))      // Return HGLOBAL with APLINT axis values
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
        // Fill in the result token
        YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;    // Already zero from ZeroMemory
////////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
        YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = CopySymGlbDirGlb (hGlbRht);
        YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

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

    // If it's an APA, get its parameters
    if (aplTypeRht EQ ARRAY_APA)
    {
#define lpaplAPA    ((LPAPLAPA) lpMemRht)

        // Get the APA data
        apaOff = lpaplAPA->Off;
        apaMul = lpaplAPA->Mul;
        apaLen = lpaplAPA->Len;

#undef  lpaplAPA
    } // End IF

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
                                   lptkFunc);
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
                                                      lptkFunc);
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
                                                      lptkFunc);
                    if (!bRet)
                        goto ERROR_EXIT;
                    break;

                case ARRAY_HETERO:
                case ARRAY_NESTED:
                    if (aplNELMSub EQ 0)
                        hGlbProto = MakePrototype_EM (ClrPtrTypeIndGlb (lpMemRht),
                                                      lptkFunc,
                                                      FALSE);   // Allow CHARs
                    else
                    {
                        // Calculate storage for <aplNELMSub>
                        //   HGLOBALs in an array of variables of
                        //   rank <aplNELMAxis> and NELM <aplNELMSub>.
                        ByteRes = CalcArraySize (ARRAY_NESTED, aplNELMSub, aplNELMAxis);

                        // N.B.:  Conversion from APLUINT to UINT.
                        Assert (ByteRes EQ (UINT) ByteRes);
                        hGlbProto = DbgGlobalAlloc (GHND, (UINT) ByteRes);
                        if (!hGlbProto)
                        {
                            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                                       lptkFunc);
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
                            // Fill in the result token
                            YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////////////////////////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;    // Already zero from ZeroMemory
////////////////////////////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
                            YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (hGlbProto);
                            YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

                            // Free the prototype storage
                            FreeResult (&YYRes[YYLclIndex].tkToken);

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
                                       lptkFunc);
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
                                       lptkFunc);
            bRet = FALSE;

            goto ERROR_EXIT;
        } // End IF

        // Lock the global memory to get a ptr to it
        lpMemOdo = MyGlobalLock (hGlbOdo);

        //***************************************************************
        // Calculate the storage needed for each subarray.
        // N.B.:  Conversion from APLUINT to UINT.
        //***************************************************************
        // Handle APAs as INTs
        if (aplTypeRht EQ ARRAY_APA)
            ByteRes = CalcArraySize (ARRAY_INT , aplNELMSub, aplNELMAxis);
        else
            ByteRes = CalcArraySize (aplTypeRht, aplNELMSub, aplNELMAxis);
////////// Split cases based upon the right arg's storage type
////////switch (aplTypeRht)
////////{
////////    case ARRAY_BOOL:        // One value per bit, rounded up to the byte
////////        ByteRes = sizeof (APLBOOL)   * RoundUpBits8 (aplNELMSub);
////////
////////        break;
////////
////////    case ARRAY_INT:         // One value per APLINT
////////    case ARRAY_APA:         // ...
////////        ByteRes = sizeof (APLINT)    * aplNELMSub;
////////
////////        break;
////////
////////    case ARRAY_FLOAT:       // One value per APLFLOAT
////////        ByteRes = sizeof (APLFLOAT)  * aplNELMSub;
////////
////////        break;
////////
////////    case ARRAY_CHAR:        // One value per APLCHAR
////////        ByteRes = sizeof (APLCHAR)   * aplNELMSub;
////////
////////        break;
////////
////////    case ARRAY_HETERO:      // One value per APLHETERO/APLNESTED
////////    case ARRAY_NESTED:
////////        ByteRes = sizeof (APLHETERO) * aplNELMSub;
////////
////////        break;
////////
////////    case ARRAY_LIST:        // Handled above
////////    defstop
////////        break;
////////} // End SWITCH
////////
////////// Add in the header and dimensions
////////ByteRes += sizeof (VARARRAY_HEADER)
////////         + sizeof (APLDIM) * aplNELMAxis;

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
                                                   lptkFunc);
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
                                                   lptkFunc);
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
                                                   lptkFunc);
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
                                                   lptkFunc);
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
                                                   lptkFunc);
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
                                                   lptkFunc);
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
                                                   lptkFunc);
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
    // Fill in the result token
    YYRes[YYLclIndex].tkToken.tkFlags.TknType   = TKT_VARARRAY;
////YYRes[YYLclIndex].tkToken.tkFlags.ImmType   = 0;    // Already zero from ZeroMemory
////YYRes[YYLclIndex].tkToken.tkFlags.NoDisplay = 0;    // Already zero from ZeroMemory
    YYRes[YYLclIndex].tkToken.tkData.tkGlbData  = MakeGlbTypeGlb (TypeDemote (hGlbRes));
    YYRes[YYLclIndex].tkToken.tkCharIndex       = lptkFunc->tkCharIndex;
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
        return &YYRes[YYLclIndex];
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
    (HGLOBAL *lphGlbProto,
     HGLOBAL  hGlbMT,
     APLSTYPE aplTypeProto,
     APLNELM  aplNELMSub,
     APLNELM  aplNELMAxis,
     APLSTYPE aplType,
     LPTOKEN  lptkFunc)

{
    APLUINT ByteRes,
            uRes;
    LPVOID lpMemProto;

    if (aplNELMSub EQ 0)
        *lphGlbProto = hGlbMT;
    else
    {
        // Calculate storage for <aplNELMSub>
        //   HGLOBALs in an array of variables of
        //   rank <aplNELMAxis> and NELM <aplNELMSub>.
        ByteRes = CalcArraySize (aplTypeProto, aplNELMSub, aplNELMAxis);
////////ByteRes = sizeof (VARARRAY_HEADER)
////////        + sizeof (APLDIM)  * aplNELMAxis
////////        + sizeofItem * aplNELMSub;
        // N.B.:  Conversion from APLUINT to UINT.
        Assert (ByteRes EQ (UINT) ByteRes);
        *lphGlbProto = DbgGlobalAlloc (GHND, (UINT) ByteRes);
        if (!*lphGlbProto)
        {
            ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                       lptkFunc);
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
    (APLUINT      ByteRes,
     APLSTYPE     aplType,
     APLNELM      aplNELMSub,
     APLNELM      aplNELMAxis,
     APLRANK      aplRankRes,
     APLRANK      aplRankRht,
     LPAPLNESTED  lpMemRes,
     LPAPLDIM     lpMemDimRht,
     LPAPLINT     lpMemAxis,
     HGLOBAL     *lphGlbSub,
     LPVOID      *lplpMemSub,
     LPTOKEN      lptkFunc)

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
                                   lptkFunc);
        return FALSE;
    } // End IF

    // Save the HGLOBAL in the result
    *lpMemRes = MakeGlbTypeGlb (*lphGlbSub);

    // Lock the subarray memory to get a ptr to it
    *lplpMemSub = MyGlobalLock (*lphGlbSub);

#define lpHeader    (*(LPVARARRAY_HEADER *) lplpMemSub)

    // Fill in the subarray header
    lpHeader->Sign.ature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplType;
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
    (LPTOKEN lptkLftArg,
     LPTOKEN lptkFunc,
     LPTOKEN lptkRhtArg,
     LPTOKEN lptkAxis)

{
    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.lpVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.lpSym->stFlags.Imm)
            {
                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.lpSym->stData.stGlbData));

                return PrimFnDydLeftShoeGlb_EM (lptkLftArg,
                                                ClrPtrTypeDirGlb (lptkRhtArg->tkData.lpSym->stData.stGlbData),
                                                lptkAxis,
                                                lptkFunc);
            } // End IF

            // Handle the immediate case

            // Fall through to TKT_VARIMMED case to signal a RANK ERROR

        case TKT_VARIMMED:
            ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                                       lptkFunc);
            return NULL;

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnDydLeftShoeGlb_EM (lptkLftArg,
                                            ClrPtrTypeDirGlb (lptkRhtArg->tkData.tkGlbData),
                                            lptkAxis,
                                            lptkFunc);
        case TKT_LISTPAR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                       lptkFunc);
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
    (LPTOKEN lptkLftArg,
     HGLOBAL hGlbRht,
     LPTOKEN lptkAxis,
     LPTOKEN lptkFunc)

{
    APLINT  aplAxis;        // The (one and only) axis value
    APLRANK aplRankRht;     // The rank of the right arg
    BOOL    bRet = TRUE;
    UINT    YYLclIndex;

    // Get new index into YYRes
    YYLclIndex = NewYYResIndex ();

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
                           &aplAxis,        // Return last axis value
                           NULL,            // Return # elements in axis vector
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } else
    {
        // No axis means partition on the last dimension
        aplAxis = max (0, (APLINT) aplRankRht - 1);
    } // End IF/ELSE

    DbgBrk ();              // ***FINISHME***

    // Split cases based upon the










////ERROR_EXIT:
////if (bCopyArray && (!bRet) && hGlbRht)
////{
////    // We no longer need this storage
////    FreeResultGlobalVar (hGlbRht); hGlbRht = NULL;
////} // End IF

    if (bRet)
        return &YYRes[YYLclIndex];
    else
        return NULL;
} // End PrimFnDydLeftShoeGlb_EM


//***************************************************************************
//  End of File: pf_lshoe.c
//***************************************************************************
