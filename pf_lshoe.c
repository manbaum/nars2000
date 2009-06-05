//***************************************************************************
//  NARS2000 -- Primitive Function -- LeftShoe
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
//  $PrimFnLeftShoe_EM_YY
//
//  Primitive function for monadic and dyadic LeftShoe ("enclose" and "partitioned enclose")
//***************************************************************************

LPPL_YYSTYPE PrimFnLeftShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_LEFTSHOE);

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonLeftShoe_EM_YY             (lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydLeftShoe_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnLeftShoe_EM


//***************************************************************************
//  $PrimProtoFnLeftShoe_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic LeftShoe
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnLeftShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnLeftShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // Called monadically or dyadically
    //***************************************************************

    // Convert to a prototype
    return PrimProtoFnMixed_EM_YY (&PrimFnLeftShoe_EM_YY,   // Ptr to primitive function routine
                                    lptkLftArg,             // Ptr to left arg token
                                    lptkFunc,               // Ptr to function token
                                    lptkRhtArg,             // Ptr to right arg token
                                    lptkAxis);              // Ptr to axis token (may be NULL)
} // End PrimProtoFnLeftShoe_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonLeftShoe_EM_YY
//
//  Primitive function for monadic LeftShoe ("enclose")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonLeftShoe_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Split cases based upon the right arg's token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        HGLOBAL hGlbRht;

        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, we must look inside the array
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the right arg global memory handle
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                // stData is a valid HGLOBAL variable array
                Assert (IsGlbTypeVarDir (hGlbRht));

                return PrimFnMonLeftShoeGlb_EM_YY
                       (ClrPtrTypeDir (hGlbRht),    // Right arg global memory handle
                        lptkAxis,                   // Ptr to axis token (may be NULL)
                        lptkFunc);                  // Ptr to function token
            } // End IF

            // Handle the immediate case
            return PrimFnMonLeftShoeCon_EM_YY
                   (lptkRhtArg->tkData.tkSym->stFlags.ImmType,  // Immediate type
                    lptkRhtArg->tkData.tkSym->stData.stLongest, // Immediate value
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARIMMED:
            return PrimFnMonLeftShoeCon_EM_YY
                   (lptkRhtArg->tkFlags.ImmType,                // Immediate type
                    lptkRhtArg->tkData.tkLongest,               // Immediate value
                    lptkAxis,                                   // Ptr to axis token (may be NULL)
                    lptkFunc);                                  // Ptr to function token
        case TKT_VARARRAY:
            // Get the right arg global memory handle
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (hGlbRht));

            return PrimFnMonLeftShoeGlb_EM_YY
                   (ClrPtrTypeDir (hGlbRht),    // Right arg global memory handle
                    lptkAxis,                   // Ptr to axis token (may be NULL)
                    lptkFunc);                  // Ptr to function token
        defstop
            return NULL;
    } // End SWITCH
} // End PrimFnMonLeftShoe_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonLeftShoeCon_EM_YY
//
//  Monadic LeftShoe ("enclose") on an immediate value.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftShoeCon_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonLeftShoeCon_EM_YY
    (UINT       ImmType,            // The immediate type
     APLLONGEST aplLongest,         // The immediate value
     LPTOKEN    lptkAxis,           // Ptr to axis token
     LPTOKEN    lptkFunc)           // Ptr to function token

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result

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

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARIMMED;
    lpYYRes->tkToken.tkFlags.ImmType   = ImmType;
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;     // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkLongest  = aplLongest;
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;
} // End PrimFnMonLeftShoeCon_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonLeftShoeGlb_EM_YY
//
//  Monadic LeftShoe ("enclose") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftShoeGlb_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonLeftShoeGlb_EM_YY
    (HGLOBAL hGlbRht,               // Handle to right arg
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    HGLOBAL       hGlbRes = NULL,
                  hGlbAxis = NULL,
                  hGlbSub = NULL,
                  hSymGlbProto = NULL,
                  hGlbRhtProto = NULL,
                  hGlbOdo = NULL,
                  hGlbWVec = NULL;
    LPVOID        lpMemRes = NULL,
                  lpMemRht = NULL,
                  lpMemSub = NULL,
                  lpMemProto = NULL;
    LPAPLDIM      lpMemDimRht = NULL;
    LPAPLINT      lpMemAxis = NULL,
                  lpMemGrUp = NULL,
                  lpMemOdo = NULL,
                  lpMemWVec = NULL;
    APLUINT       ByteRes;          // # bytes in the result
    APLNELM       aplNELMAxis,
                  aplNELMRes,
                  aplNELMRht,
                  aplNELMSub;
    APLRANK       aplRankRht,
                  aplRankRes;
    UBOOL         bRet = TRUE;
    APLNELM       uRes, uRht, uSub, uOdo, uRhtOff;
    APLSTYPE      aplTypeRht;
    APLNELMSIGN   iRht;
    APLINT        apaOff,
                  apaMul;
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

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
                          &hGlbAxis))       // Return HGLOBAL with APLINT axis values
            goto ERROR_EXIT;
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
    if (lptkAxis && IsEmpty (aplNELMAxis))
    {
        // Allocate a new YYRes
        lpYYRes = YYAlloc ();

        // Fill in the result token
        lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
        lpYYRes->tkToken.tkData.tkGlbData  = CopySymGlbDirAsGlb (hGlbRht);
        lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

        goto QUICK_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type and NELM
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
#undef  lpHeader

    // Skp over the header to the dimensions
    lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayBaseToData (lpMemRht, aplRankRht);

    // If the right arg is an APA, ...
    if (IsSimpleAPA (aplTypeRht))
    {
#define lpAPA       ((LPAPLAPA) lpMemRht)
        // Get the APA parameters
        apaOff = lpAPA->Off;
        apaMul = lpAPA->Mul;
#undef  lpAPA
    } // End IF

    // Calculate the NELM of the result taking into
    //   account the axis values.
    if (lptkAxis NE NULL)
    {
        // Lock the memory to get a ptr to it
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
    ByteRes = CalcArraySize (ARRAY_NESTED, aplNELMRes, aplRankRes);

    //***************************************************************
    // Now we can allocate the storage for the result.
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (APLU3264) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_NESTED;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

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
        *((LPAPLNESTED) lpMemRes) = CopySymGlbDirAsGlb (hGlbRht);
    else
    {
        // Handle prototypes for the result
        if (IsEmpty (aplNELMRes))
        {
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_APA:
                case ARRAY_FLOAT:
                    bRet = PrimFnMonLeftShoeProto_EM (&hSymGlbProto,
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
                    bRet = PrimFnMonLeftShoeProto_EM (&hSymGlbProto,
                                                       hGlbV0Char,
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
                    if (IsEmpty (aplNELMSub))
                    {
                        hSymGlbProto =
                          MakeMonPrototype_EM (*(LPAPLNESTED) lpMemRht, // Proto arg handle
                                               lptkFunc,                // Ptr to function token
                                               MP_CHARS);               // CHARs allowed
                        if (!hSymGlbProto)
                            goto WSFULL_EXIT;
                    } else
                    {
                        // Calculate space needed for the result
                        ByteRes = CalcArraySize (ARRAY_NESTED, aplNELMSub, aplNELMAxis);

                        // N.B.:  Conversion from APLUINT to UINT.
                        Assert (ByteRes EQ (APLU3264) ByteRes);
                        hSymGlbProto = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
                        if (!hSymGlbProto)
                            goto WSFULL_EXIT;

                        // Lock the memory to get a ptr to it
                        lpMemProto = MyGlobalLock (hSymGlbProto);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemProto)
                        // Fill in the header
                        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                        lpHeader->ArrType    = ARRAY_NESTED;
////////////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
                        lpHeader->RefCnt     = 1;
                        lpHeader->NELM       = aplNELMSub;
                        lpHeader->Rank       = aplNELMAxis;
#undef  lpHeader
                        // Fill in the dimension
                        *VarArrayBaseToDim (lpMemProto) = aplNELMSub;

                        // Skip over the header and dimension to the data
                        lpMemProto = VarArrayBaseToData (lpMemProto, aplNELMAxis);

                        // Fill in the values
                        for (uRes = 0; uRes < aplNELMSub; uRes++)
                            // Save the value in the prototype
                            *((LPAPLNESTED) lpMemProto)++ = CopySymGlbInd (lpMemRht);

                        // We no longer need this ptr
                        MyGlobalUnlock (hSymGlbProto); lpMemProto = NULL;

                        // Set the ptr type bits
                        hSymGlbProto = MakePtrTypeGlb (hSymGlbProto);
                    } // End IF/ELSE

                    break;

                defstop
                    break;
            } // End SWITCH

            // Save the HGLOBAL in the result
            *((LPAPLNESTED) lpMemRes) = hSymGlbProto;

            goto NORMAL_EXIT;
        } // End IF

        // Fill nested result with PTR_REUSED
        //   in case we fail part way through
        for (uRes = 0; uRes < aplNELMRes; uRes++)
            ((LPAPLNESTED) lpMemRes)[uRes] = PTR_REUSED;

        //***************************************************************
        // Check for empty subarrays
        //***************************************************************

        if (IsEmpty (aplNELMSub))
        {
            // Split cases based upon the right arg's storage type
            switch (aplTypeRht)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_FLOAT:
                case ARRAY_APA:
                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Save the HGLOBAL in the result
                        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbZilde);
                    } // End FOR

                    break;

                case ARRAY_CHAR:
                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Save the HGLOBAL in the result
                        *((LPAPLNESTED) lpMemRes)++ = MakePtrTypeGlb (hGlbV0Char);
                    } // End FOR

                    break;

                case ARRAY_NESTED:
                case ARRAY_HETERO:
                    // Fill in the values
                    for (uRes = 0; uRes < aplNELMRes; uRes++)
                    {
                        // Check for Ctrl-Break
                        if (CheckCtrlBreak (*lpbCtrlBreak))
                            goto ERROR_EXIT;

                        // Save the value in the prototype
                        *((LPAPLNESTED) lpMemRes)++ = CopySymGlbInd (lpMemRht);
                    } // End FOR

                    break;

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
        ByteRes = aplRankRht * sizeof (APLUINT);
        Assert (ByteRes EQ (APLU3264) ByteRes);
        hGlbWVec = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbWVec)
            goto WSFULL_EXIT;

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
        ByteRes = aplRankRht * sizeof (APLUINT);
        Assert (ByteRes EQ (APLU3264) ByteRes);
        hGlbOdo = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbOdo)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemOdo = MyGlobalLock (hGlbOdo);

        //***************************************************************
        // Calculate space needed for each subarray.
        //***************************************************************
        // Handle APAs as INTs
        if (IsSimpleAPA (aplTypeRht))
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

                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    uBitMask = BIT0 << (MASKLOG2NBIB & (UINT) uRht);

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    ((LPAPLBOOL) lpMemSub)[uSub >> LOG2NBIB] |=
                    ((uBitMask & ((LPAPLBOOL) lpMemRht)[uRht >> LOG2NBIB]) ? TRUE : FALSE) << (MASKLOG2NBIB & (UINT) uSub);
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
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    ((LPAPLINT) lpMemSub)[uSub] = ((LPAPLINT) lpMemRht)[uRht];
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
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    ((LPAPLFLOAT) lpMemSub)[uSub] = ((LPAPLFLOAT) lpMemRht)[uRht];
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
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    ((LPAPLCHAR) lpMemSub)[uSub] = ((LPAPLCHAR) lpMemRht)[uRht];
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
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    // Note that APLHETERO elements are LPSYMENTRYs, so there's no
                    //   reference count to increment, or other special handling.
                    ((LPAPLHETERO) lpMemSub)[uSub] = ((LPAPLHETERO) lpMemRht)[uRht];
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
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    // Note that APLNESTED elements are a mixture of LPSYMENTRYs
                    //   and HGLOBALs, so we need to run the HGLOBALs through
                    //   CopySymGlbDir to increment the reference count.
                    ((LPAPLNESTED) lpMemSub)[uSub] = CopySymGlbDir (((LPAPLNESTED) lpMemRht)[uRht]);
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
                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpbCtrlBreak))
                        goto ERROR_EXIT;

                    // Use the index in lpMemOdo to calculate the
                    //   corresponding index in lpMemRes where the
                    //   next value from lpMemRht goes.
                    for (uRht = uOdo = 0; uOdo < aplRankRht; uOdo++)
                        uRht += lpMemOdo[lpMemGrUp[uOdo]] * lpMemWVec[uOdo];

                    // Increment the odometer in lpMemOdo subject to
                    //   the values in lpMemDimRht[lpMemAxis]
                    IncrOdometer (lpMemOdo, lpMemDimRht, lpMemAxis, aplRankRht);

                    // Copy element # uRht from the right arg to lpMemSub[uSub]
                    ((LPAPLINT) lpMemSub)[uSub] = apaOff + apaMul * uRht;
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (hGlbSub); lpMemSub = NULL;

                break;

            defstop
                break;
        } // End FOR/SWITCH
    } // End IF/ELSE
NORMAL_EXIT:
    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto UNLOCK_EXIT;

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
UNLOCK_EXIT:
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

    return lpYYRes;
} // End PrimFnMonLeftShoeGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonLeftShoeProto_EM
//
//  Subroutine to PrimFnMonLeftShoe_EM to handle prototypes
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftShoeProto_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnMonLeftShoeProto_EM
    (HGLOBAL *lphGlbProto,          // Ptr to handle to prototype result
     HGLOBAL  hGlbMT,               // Handle to empty (zilde or '')
     APLSTYPE aplTypeProto,         // Prototype type
     APLNELM  aplNELMSub,           // Subarray NELM
     APLNELM  aplNELMAxis,          // Axis NELM
     APLSTYPE aplType,              // Right arg type
     LPTOKEN  lptkFunc)             // Ptr to function token

{
    APLUINT ByteRes;                // # bytes in the result
////APLUINT uRes;                   // Result loop counter
    LPVOID lpMemProto;              // Ptr to prototype global memory

    // If the subarray is empty, the result is zilde
    if (IsEmpty (aplNELMSub))
        *lphGlbProto = hGlbMT;
    else
    {
        // Calculate space needed for the result
        ByteRes = CalcArraySize (aplTypeProto, aplNELMSub, aplNELMAxis);

        // N.B.:  Conversion from APLUINT to UINT.
        Assert (ByteRes EQ (APLU3264) ByteRes);
        *lphGlbProto = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!*lphGlbProto)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemProto = MyGlobalLock (*lphGlbProto);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemProto)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplType;
////////lpHeader->PermNdx    = PERMNDX_NONE;// ALready zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMSub;
        lpHeader->Rank       = aplNELMAxis;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemProto) = aplNELMSub;

        // skip over the header and dimension to the data
        lpMemProto = VarArrayBaseToData (lpMemProto, aplNELMAxis);

        // Fill in the values if character
        // N.B.:  Conversion from APLUINT to UINT
        Assert (aplNELMSub EQ (APLU3264) aplNELMSub);
        if (IsSimpleChar (aplType))
            FillMemoryW (lpMemProto, (APLU3264) aplNELMSub, L' ');
        // We no longer need this ptr
        MyGlobalUnlock (*lphGlbProto); lpMemProto = NULL;
    } // End IF/ELSE

    return TRUE;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End PrimFnMonLeftShoe_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonLeftShoeGlbSub_EM
//
//  Subroutine to PrimFnMonLeftShoeGlb_EM
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonLeftShoeGlbSub_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnMonLeftShoeGlbSub_EM
    (APLUINT     ByteRes,           // # bytes in the result
     APLSTYPE    aplTypeSub,        // Subarray type
     APLNELM     aplNELMSub,        // Subarray NELM
     APLNELM     aplNELMAxis,       // Axis NELM
     APLRANK     aplRankRes,        // Result rank
     APLRANK     aplRankRht,        // Right arg rank
     LPAPLNESTED lpMemRes,          // Ptr to result memory
     LPAPLDIM    lpMemDimRht,       // Ptr to right arg dimensions
     LPAPLINT    lpMemAxis,         // Ptr to axis memory
     HGLOBAL    *lphGlbSub,         // Ptr to subarray handle
     LPVOID     *lplpMemSub,        // Ptr to ptr to subarray memory
     LPTOKEN     lptkFunc)          // Ptr to function token

{
    APLINT uRht;

    //***************************************************************
    // Allocate storage for this subarray.
    // N.B.:  Conversion from APLUINT to UINT.
    //***************************************************************
    Assert (ByteRes EQ (APLU3264) ByteRes);
    *lphGlbSub = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!*lphGlbSub)
        goto WSFULL_EXIT;

    // Save the HGLOBAL in the result
    *lpMemRes = MakePtrTypeGlb (*lphGlbSub);

    // Lock the memory to get a ptr to it
    *lplpMemSub = MyGlobalLock (*lphGlbSub);

#define lpHeader    (*(LPVARARRAY_HEADER *) lplpMemSub)
    // Fill in the subarray header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeSub;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
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

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End PrimFnMonLeftShoeGlbSub_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydLeftShoe_EM_YY
//
//  Primitive function for dyadic LeftShoe ("partitioned enclose")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydLeftShoe_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydLeftShoe_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
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

                return PrimFnDydLeftShoeGlb_EM (lptkLftArg,
                                                ClrPtrTypeDir (lptkRhtArg->tkData.tkSym->stData.stGlbData),
                                                lptkAxis,
                                                lptkFunc);
            } // End IF

            // Handle the immediate case

            // Fall through to TKT_VARIMMED case to signal a RANK ERROR

        case TKT_VARIMMED:
            goto RANK_EXIT;

        case TKT_VARARRAY:
            // tkData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir (lptkRhtArg->tkData.tkGlbData));

            return PrimFnDydLeftShoeGlb_EM (lptkLftArg,
                                            ClrPtrTypeDir (lptkRhtArg->tkData.tkGlbData),
                                            lptkAxis,
                                            lptkFunc);
        defstop
            return NULL;
    } // End SWITCH

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFnDydLeftShoe_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydLeftShoeGlb_EM_YY
//
//  Dyadic LeftShoe ("partitioned enclose") on a global memory object
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydLeftShoeGlb_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydLeftShoeGlb_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     HGLOBAL hGlbRht,               // Handle to right arg
     LPTOKEN lptkAxis,              // Ptr to axis token (may be NULL)
     LPTOKEN lptkFunc)              // Ptr to function token

{
    APLUINT      aplAxis;           // The (one and only) axis value
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
    APLDIM       uDimAxRht;         // Right arg axis length
    HGLOBAL      hGlbLft = NULL,    // Left arg global memory handle
                 hGlbRes = NULL;    // Result   ...
    LPVOID       lpMemLft = NULL,   // Ptr to left arg global memory
                 lpMemRht = NULL,   // Ptr to right ...
                 lpMemRes = NULL;   // Ptr to result   ...
    APLLONGEST   aplLongestLft,     // Left arg immediate value
                 aplLongestLast;    // Previous left arg immediate value
    LPPL_YYSTYPE lpYYRes = NULL;    // Ptr to the result
    UBOOL        bRet = FALSE,      // TRUE iff the result is valid
                 bLastValid;        // TRUE iff aplLongestLast is valid
    APLUINT      ByteRes;           // # bytes in the result
    UINT         uCnt,              // Loop counter
                 uStartCnt,         // Starting count for an item
                 uAxis;             // Loop counter
    APLUINT      uDimLo,            // Product of dimensions below axis
                 uDimHi,            // ...                   above ...
                 uDim;              // Loop counter

    //***************************************************************
    // Get the attributes (Type, NELM, and Rank) of the left & right args
    //***************************************************************
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfGlb   (hGlbRht,    &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

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
                           NULL))           // Return HGLOBAL with APLINT axis values
            return NULL;
    } else
        // No axis means partition on the last dimension
        aplAxis = max (0, (APLINT) aplRankRht - 1);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplTypeLft))
        goto LEFT_RANK_EXIT;

    // Check for RIGHT RANK ERROR
    if (IsScalar (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

    // Skip over the header to the dimension
    lpMemRht = VarArrayBaseToDim (lpMemRht);

    // Check for LENGTH ERROR
    if (!IsScalar (aplRankLft)
     && aplNELMLft NE ((LPAPLDIM) lpMemRht)[aplAxis])
        goto LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsSimpleNum (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Calculate the product of the right arg's dimensions below the axis dimension
    uDimLo = 1;
    for (uDim = 0; uDim < aplAxis; uDim++)
        uDimLo *= ((LPAPLDIM) lpMemRht)[uDim];

    // Calculate the product of the right arg's dimensions above the axis dimension
    uDimHi = 1;
    for (uDim++; uDim < aplRankRht; uDim++)
        uDimHi *= ((LPAPLDIM) lpMemRht)[uDim];

    // Get left arg's global ptrs
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);

    if (IsSimpleFlt (aplTypeLft))
    {
        aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);
        if (!bRet)
            goto LEFT_DOMAIN_EXIT;
    } // End IF

    // If the left arg is global
    if (hGlbLft)
    {
        // Skip over the header to the data
        lpMemLft = VarArrayBaseToData (lpMemLft, aplRankLft);

        // Calculate the axis NELM
        for (uCnt = 0,
               aplNELMAxis = 0,
               bLastValid = FALSE;
             uCnt < aplNELMLft;
             uCnt++)
        {
            // Get the next value from memory
            GetNextValueMem (lpMemLft,          // Ptr to item global memory data
                             aplTypeLft,        // Item storage type
                             uCnt,              // Index into item
                             NULL,              // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                            &aplLongestLft,     // Ptr to result immediate value (may be NULL)
                             NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
            if (IsSimpleFlt (aplTypeLft))
            {
                aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);
                if (!bRet)
                    goto LEFT_DOMAIN_EXIT;
            } // End IF

            // If the value is non-zero, ...
            if (aplLongestLft)
            {
                if (bLastValid)
                {
                    if ((APLINT) aplLongestLast < (APLINT) aplLongestLft)
                    {
                        // Count in another item in the result
                        aplNELMAxis++;

                        // Save the last left arg value
                        aplLongestLast = aplLongestLft;
                        bLastValid = TRUE;

                        // Save as the starting index
                        uStartCnt = uCnt;
                    } // End IF
                } else
                {
                    // Save the last left arg value
                    aplLongestLast = aplLongestLft;
                    bLastValid = TRUE;

                    // Save as the starting index
                    uStartCnt = uCnt;
                } // End IF/ELSE
            } else
            if (bLastValid
             && uStartCnt < uCnt)
            {
                // Count in another item in the result
                aplNELMAxis++;

                // Clear the last left arg value
                bLastValid = FALSE;

                // Save as starting index
                uStartCnt = uCnt + 1;
            } else
            {
                // Clear the last left arg value
                bLastValid = FALSE;

                // Save as starting index
                uStartCnt = uCnt + 1;
            } // End IF/ELSE/...
        } // End FOR

        // If there's a leftover item, ...
        if (bLastValid
         && uStartCnt < uCnt)
            // Count in another item in the result
            aplNELMAxis++;
    } else
    {
        lpMemLft = &aplLongestLft;
        aplNELMAxis = (aplLongestLft NE 0);
    } // End IF/ELSE

    // Calculate the result NELM
    if (IsEmpty (aplNELMAxis))
        aplNELMRes = 0;
    else
    for (uCnt = 0, aplNELMRes = aplNELMAxis; uCnt < aplRankRht; uCnt++)
    if (uCnt NE aplAxis             // Not the function axis
     || hGlbLft EQ NULL)            //   or left arg is scalar extended (hence unused)
        aplNELMRes *= ((LPAPLDIM) lpMemRht)[uCnt];

    // Fill in the result storage type & rank
    aplTypeRes = ARRAY_NESTED;
    aplRankRes = aplRankRht;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, aplRankRes);

    // Allocate space for the result.
    // N.B. Conversion from APLUINT to UINT.
    Assert (ByteRes EQ (APLU3264) ByteRes);
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = aplRankRes;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the dimensions
    for (uCnt = 0; uCnt < aplRankRes; uCnt++)
    if (uCnt NE aplAxis             // Not the function axis
     || hGlbLft EQ NULL)            //   or left arg is scalar extended (hence unused)
        *((LPAPLDIM) lpMemRes)++ = ((LPAPLDIM) lpMemRht)[uCnt];
    else
        *((LPAPLDIM) lpMemRes)++ = aplNELMAxis;

    // Save the right arg axis length
    uDimAxRht = ((LPAPLDIM) lpMemRht)[aplAxis];

    // Skip over the dimensions to the data
    lpMemRht = VarArrayDimToData (lpMemRht, aplRankRht);

     // lpMemRes and lpMemRht now point to their data

    //***************************************************************
    // Handle empty results
    //***************************************************************
    if (IsEmpty (aplNELMRes))
    {
        HGLOBAL hSymGlbProto;

        // With  L{is}{rho}R
        //       L[K]{is}0
        // The result is L{rho}{enclose}0{rho}R

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
                hSymGlbProto = MakePtrTypeGlb (hGlbZilde);

                break;

            case ARRAY_CHAR:
                hSymGlbProto = MakePtrTypeGlb (hGlbV0Char);

                break;

            case ARRAY_HETERO:
                if (IsImmChr ((*(LPAPLHETERO) lpMemRht)->stFlags.ImmType))
                    hSymGlbProto = MakePtrTypeGlb (hGlbV0Char);
                else
                    hSymGlbProto = MakePtrTypeGlb (hGlbZilde);
                break;

            case ARRAY_NESTED:
                // Make the prototype
                hSymGlbProto =
                  MakeMonPrototype_EM (*(LPAPLNESTED) lpMemRht, // Proto arg handle
                                       lptkFunc,                // Ptr to function token
                                       MP_CHARS);               // CHARs allowed
                if (!hSymGlbProto)
                    goto WSFULL_EXIT;
                break;

            defstop
                break;
        } // End SWITCH

        // Save as the nested array prototype
        *((LPAPLNESTED) lpMemRes) = hSymGlbProto;

        goto YYALLOC_EXIT;
    } // End IF

    //***************************************************************
    // The result is non-empty
    //***************************************************************

    // Run through the left arg again
    for (uCnt = 0,
           uAxis = 0,
           bLastValid = FALSE;
         uCnt < aplNELMLft;
         uCnt++)
    {
        // Get the next value from memory
        GetNextValueMem (lpMemLft,          // Ptr to item global memory data
                         aplTypeLft,        // Item storage type
                         uCnt,              // Index into item
                         NULL,              // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                        &aplLongestLft,     // Ptr to result immediate value (may be NULL)
                         NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
        if (IsSimpleFlt (aplTypeLft))
        {
            aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);
            if (!bRet)
                goto LEFT_DOMAIN_EXIT;
        } // End IF

        // If the value is non-zero, ...
        if (aplLongestLft)
        {
            if (bLastValid)
            {
                if ((APLINT) aplLongestLast < (APLINT) aplLongestLft)
                {
                    // This element ends the previous item
                    //   and starts a new one
                    if (!PrimFnDydLeftShoeAppend_EM (lpMemRht,      // Ptr to right arg global memory
                                                     aplTypeRht,    // Right arg storage type
                                                     uDimAxRht,     // Right arg axis length
                                                     uStartCnt,     // Starting index into right arg along axis dimension
                                                     uCnt,          // Ending ...
                                    (LPAPLNESTED *) &lpMemRes,      // Ptr to ptr to result global memory
                                                     uDimHi,        // Product of length of dimensions above axis
                                                     aplNELMAxis,   // Length of result axis dimension
                                                     uDimLo,        // Product of length of dimensions below axis
                                                     uAxis++,       // Current index into result axis dimension
                                                     lptkFunc))     // Ptr to function token
                        goto ERROR_EXIT;

                    // Save the last left arg value
                    aplLongestLast = aplLongestLft;
                    bLastValid = TRUE;

                    // Save as the starting index
                    uStartCnt = uCnt;
                } // End IF
            } else
            {
                // Save the last left arg value
                aplLongestLast = aplLongestLft;
                bLastValid = TRUE;

                // Save as the starting index
                uStartCnt = uCnt;
            } // End IF/ELSE
        } else
        if (bLastValid
         && uStartCnt < uCnt)
        {
            // This element ends the previous item
            if (!PrimFnDydLeftShoeAppend_EM (lpMemRht,      // Ptr to right arg global memory
                                             aplTypeRht,    // Right arg storage type
                                             uDimAxRht,     // Right arg axis length
                                             uStartCnt,     // Starting index into right arg along axis dimension
                                             uCnt,          // Ending ...
                            (LPAPLNESTED *) &lpMemRes,      // Ptr to ptr to result global memory
                                             uDimHi,        // Product of length of dimensions above axis
                                             aplNELMAxis,   // Length of result axis dimension
                                             uDimLo,        // Product of length of dimensions below axis
                                             uAxis++,       // Current index into result axis dimension
                                             lptkFunc))     // Ptr to function token
                goto ERROR_EXIT;

            // Clear the last left arg value
            bLastValid = FALSE;

            // Save as starting index
            uStartCnt = uCnt + 1;
        } else
        {
            // Clear the last left arg value
            bLastValid = FALSE;

            // Save as starting index
            uStartCnt = uCnt + 1;
        } // End IF/ELSE/...
    } // End FOR

    // Because the empty case has already been handled
    Assert (bLastValid);

    // If there's a leftover item, ...
    if (uStartCnt < uCnt)
        // One last case
        if (!PrimFnDydLeftShoeAppend_EM (lpMemRht,      // Ptr to right arg global memory
                                         aplTypeRht,    // Right arg storage type
                                         uDimAxRht,     // Right arg axis length
                                         uStartCnt,     // Starting index into right arg along axis dimension
                                         uCnt,          // Ending ...
                        (LPAPLNESTED *) &lpMemRes,      // Ptr to ptr to result global memory
                                         uDimHi,        // Product of length of dimensions above axis
                                         aplNELMAxis,   // Length of result axis dimension
                                         uDimLo,        // Product of length of dimensions below axis
                                         uAxis++,       // Current index into result axis dimension
                                         lptkFunc))     // Ptr to function token
            goto ERROR_EXIT;
YYALLOC_EXIT:
    // Unlock the result global memory in case TypeDemote actually demotes
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkLftArg);
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

    return lpYYRes;
} // End PrimFnDydLeftShoeGlb_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydLeftShoeAppend_EM
//
//  Append an item to the result
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydLeftShoeAppend_EM"
#else
#define APPEND_NAME
#endif

UBOOL PrimFnDydLeftShoeAppend_EM
    (LPVOID       lpMemRht,             // Ptr to right arg global memory
     APLSTYPE     aplTypeRht,           // Right arg storage type
     APLDIM       uDimAxRht,            // Right arg axis length
     UINT         uStartCnt,            // Starting index into right arg along axis dimension
     UINT         uEndCnt,              // Ending ...
     LPAPLNESTED *lplpMemRes,           // Ptr to ptr to result global memory
     APLUINT      uDimHi,               // Product of length of dimensions above axis
     APLNELM      aplNELMAxis,          // Length of result axis dimension
     APLUINT      uDimLo,               // Product of length of dimensions below axis
     UINT         uAxis,                // Current index into result axis dimension
     LPTOKEN      lptkFunc)             // Ptr to function token

{
    UBOOL      bRet = FALSE;            // TRUE iff the result is valid
    APLUINT    uLo,                     // uDimLo loop counter
               uHi,                     // uDimHi ...
               uSub,                    // Loop counter
               uRht,                    // ...
               uRes;                    // ...
    APLUINT    ByteItm;                 // # bytes in the item
    APLNELM    aplNELMItm;              // Item NELM
    APLRANK    aplRankItm;              // Item rank
    APLSTYPE   aplTypeItm;              // Item storage type
    APLLONGEST aplLongestRht;           // Right arg immediate value
    HGLOBAL    hGlbItm = NULL,          // Item global memory handle
               hGlbSub;                 // Subitem ...
    LPVOID     lpMemItm;                // Ptr to item global memory
    UINT       uBitIndex;               // Bit index for looping through Booleans
    TOKEN      tkItmArg = {0};          // Temporary token for TypeDemote

    // Calculate the item NELM, rank, & storage type
    aplNELMItm = uEndCnt - uStartCnt;
    aplRankItm = 1;
    if (IsSimpleAPA (aplTypeRht))
        aplTypeItm = ARRAY_INT;
    else
        aplTypeItm = aplTypeRht;

    // Loop through the high and low items of the right arg
    for (uLo = 0; uLo < uDimLo; uLo++)
    for (uHi = 0; uHi < uDimHi; uHi++)
    {
        // Calculate space needed for the item
        ByteItm = CalcArraySize (aplTypeItm, aplNELMItm, aplRankItm);

        //***************************************************************
        // Now we can allocate the storage for the item.
        // N.B.:  Conversion from APLUINT to UINT.
        //***************************************************************
        Assert (ByteItm EQ (APLU3264) ByteItm);
        hGlbItm = DbgGlobalAlloc (GHND, (APLU3264) ByteItm);
        if (!hGlbItm)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemItm = MyGlobalLock (hGlbItm);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemItm)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = aplTypeItm;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMItm;
        lpHeader->Rank       = aplRankItm;
#undef  lpHeader

        // Point to the item's dimension
        lpMemItm = VarArrayBaseToDim (lpMemItm);

        // Save the item dimension
        *((LPAPLDIM) lpMemItm)++ = aplNELMItm;

        // lpMemItm now points to the item data

        // Initiliaze in case Boolean
        uBitIndex = 0;

        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_FLOAT:
            case ARRAY_APA:
            case ARRAY_CHAR:
                // Loop through the subitems
                for (uSub = uStartCnt; uSub < uEndCnt; uSub++)
                {
                    // Calculate the starting index in the right arg of this item
                    uRht = 1 * uHi + uDimHi * (uSub + uDimAxRht * uLo);

                    // Get the next value from right arg global memory
                    GetNextValueMem (lpMemRht,          // Ptr to item global memory data
                                     aplTypeRht,        // Item storage type
                                     uRht,              // Index into item
                                     NULL,              // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                    &aplLongestRht,     // Ptr to result immediate value (may be NULL)
                                     NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                    // Split cases based upon the right arg storage type
                    switch (aplTypeItm)
                    {
                        case ARRAY_BOOL:
                            // Save the next element in the item
                            *((LPAPLBOOL)  lpMemItm) |= (BIT0 & aplLongestRht) << uBitIndex;

                            // Check for end-of-byte
                            if (++uBitIndex EQ NBIB)
                            {
                                uBitIndex = 0;              // Start over
                                ((LPAPLBOOL) lpMemItm)++;   // Skip to next byte
                            } // End IF

                            break;

                        case ARRAY_INT:
                            // Save the next element in the item
                            *((LPAPLINT)   lpMemItm)++ = (APLINT)  aplLongestRht;

                            break;

                        case ARRAY_FLOAT:
                            // Save the next element in the item
                            *((LPAPLFLOAT) lpMemItm)++ = *(LPAPLFLOAT) &aplLongestRht;

                            break;

                        case ARRAY_CHAR:
                            // Save the next element in the item
                            *((LPAPLCHAR)  lpMemItm)++ = (APLCHAR) aplLongestRht;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End FOR

                break;

            case ARRAY_HETERO:
            case ARRAY_NESTED:
                // Loop through the subitems
                for (uSub = uStartCnt; uSub < uEndCnt; uSub++)
                {
                    // Calculate the index in the right arg of this item
                    uRht = 1 * uHi + uDimHi * (uSub + uDimAxRht * uLo);

                    // Extract the next subitem from the right arg
                    hGlbSub = ((LPAPLNESTED) lpMemRht)[uRht];

                    // Save the subitem in the item
                    *((LPAPLNESTED) lpMemItm)++ =
                      CopySymGlbDir (hGlbSub);
                } // End FOR

                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this ptr
        MyGlobalUnlock (hGlbItm); lpMemItm = NULL;

        // Save in token so we can call TypeDemote
        tkItmArg.tkFlags.TknType   = TKT_VARARRAY;
////////tkItmArg.tkFlags.ImmType   = ImmType;   // Already zero from = {0}
////////tkItmArg.tkFlags.NoDisplay = FALSE;     // Already zero from = {0}
        tkItmArg.tkData.tkGlbData  = MakePtrTypeGlb (hGlbItm);
        tkItmArg.tkCharIndex       = lptkFunc->tkCharIndex;

        // See if it fits into a lower (but not necessarily smaller) datatype
        TypeDemote (&tkItmArg);

        // Calculate the index in the result of this item
        uRes = 1 * uHi + uDimHi * (uAxis + aplNELMAxis * uLo);

        // Save the item in the result
        (*lplpMemRes)[uRes] = tkItmArg.tkData.tkGlbData;

        // Mark as available
        hGlbItm = NULL;
    } // End FOR/FOR

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbItm)
    {
        if (lpMemItm)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbItm); lpMemItm = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbItm); hGlbItm = NULL;
    } // End IF

    return bRet;
} // End PrimFnDydLeftShoeAppend_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: pf_lshoe.c
//***************************************************************************
